// Standard Template Library includes
#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <ctime>

// ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"

// Analysis includes
#include "Service.h"
#include "SelectorBase.h"
#include "Enums.h"
#include "Log.h"
#include "Store.h"


int Usage(Log & log) {
  
  log << Log::INFO << "Usage :"                         << Log::endl();
  log << Log::INFO << "   bin/AnalysisManager cardFile" << Log::endl();
  log << Log::INFO << "This message :"                  << Log::endl();
  log << Log::INFO << "   bin/AnalysisManager"          << Log::endl();
  log << Log::INFO << "   bin/AnalysisManager --help"   << Log::endl();
  log << Log::INFO << "   bin/AnalysisManager -h"       << Log::endl();
  
  return 0;

}


int main(int argc, char** argv) 
{


  // declare logger
  Log log("AnalysisManager");
  log << Log::INFO << "Starting program" << Log::endl();


  // read argument/steerfile
  if ( argc != 2 || strcmp(argv[1],"--help") == 0 || strcmp(argv[1],"-h") == 0 ) return Usage( log );
  const Store * config = Store::createStore( argv[1] ); 
  if ( ! config ) return 0;

 
  // open output files
  bool fillOutputTree = true;
  config->getif<bool>( "fillOutputTree" , fillOutputTree );
  std::string histFilePath = "histograms.root";
  std::string ntupFilePath = "ntuple.root";
  config->getif<std::string>( "outputHistogramFileName" , histFilePath );
  config->getif<std::string>( "outputNtupleFileName"    , ntupFilePath );
  TFile * outFileNtup = new TFile( ntupFilePath.c_str() ,"recreate" );
  TFile * outFileHist = new TFile( histFilePath.c_str() ,"recreate" );


  // initialise Service
  Service service( log.GetLevel() );
  std::string treeName = "tree";
  config->getif<std::string>( "inputTreeName" , treeName );
  service.SetTreeName( treeName );
  std::vector<std::string> inFileNames;
  config->getif("inputFileNames",inFileNames);
  if (inFileNames.size() == 0) {
    log << Log::endl() << Log::ERROR << "No input files specified!" << Log::endl();
    return 0;
  }
  if ( service.PrepareInput( inFileNames ) != GLOBAL::SUCCESS ) return 0;;
  if ( service.PrepareOutTree() != GLOBAL::SUCCESS ) return 0;
  service.GetOutTree()->SetDirectory( outFileNtup );
  

  // declare and initialise selectors, and setup histogram directories
  std::vector<SelectorBase *> selectors;
  std::vector<std::string> selectorList;
  config->getif<std::vector<std::string> >( "selectors" , selectorList );
  for (unsigned int sel = 0; sel < selectorList.size(); ++sel ) {
    
    // get selector name 
    const std::string & name = selectorList.at(sel);

    // declare selector
    SelectorBase* theSelector = SelectorBase::CreateSelector(name,*config,service);
    if ( theSelector ) {
      log << Log::INFO << "Adding selector \"" << name << "\" to sequence" << Log::endl();
      selectors.push_back(theSelector);
    } 
    else {
      log << Log::ERROR << "Couldn't recognise selector \"" << name << "\"" << Log::endl();
      return 0;
    }

    // make directory for histograms
    TDirectory* dir = outFileHist->mkdir((selectors.back()->GetName()).c_str());
    dir->cd();

    // initialise selector
    if ( selectors.back()->Initialise() != GLOBAL::SUCCESS ) return 0;

  }

  
  //
  // start analysis
  //
  
  log << Log::INFO << "Starting analysis" << Log::endl();

  int nEventsMax = service.GetNEvents();
  config->getif<int>( "nEventsMax" , nEventsMax );
  if (nEventsMax < 0) nEventsMax = service.GetNEvents();

  int reportFrac = nEventsMax/(nEventsMax > 100000 ? 10 : 1) + 1;

  std::clock_t start = std::clock();
  for (unsigned int ifile = 0; ifile < inFileNames.size(); ++ifile) {

    // open next file and load tree
    if ( service.NextInTree() != GLOBAL::SUCCESS ) return 0;

    // update pointers in selectors
    for ( unsigned int algo=0; algo<selectors.size(); ++algo ) {      
      if ( selectors.at(algo)->BeginInputFile() != GLOBAL::SUCCESS ) {
	log << Log::ERROR << "Couldn't update pointers!" << Log::endl();
	return 0;
      }
    }
        
    // loop over events
    int nEventsProcessed = 0;
    const long nEventsTree = service.GetInTree()->GetEntries();
    log << Log::INFO << "Looping over events... (" << nEventsTree << ")" << Log::endl();
    for ( long event = 0; event < nEventsTree; ++event ) {
                  
      // increment event count
      if ((++nEventsProcessed) > nEventsMax) break;

      // print progress and remaining time estimate
      if( nEventsProcessed > 0 && nEventsProcessed % reportFrac == 0 ) {
	double duration     = (clock() - start)/static_cast<double>(CLOCKS_PER_SEC);
	double frequency    = static_cast<double>(nEventsProcessed) / duration;
	double timeEstimate = static_cast<double>(service.GetNEvents() - nEventsProcessed) / frequency;
	log << Log::INFO
	    << "Processed : " << std::setw(4) << 100*nEventsProcessed/nEventsMax << "\%"
	    << "  ---  frequency : "      << std::setw(6) << static_cast<int>(frequency) << " events/sec"
	    << "  ---  time : "           << std::setw(4) << static_cast<int>(duration)  << " sec"
	    << "  ---  remaining time : " << std::setw(4) << static_cast<int>(timeEstimate) << " sec"<< Log::endl(); 
      }

      // get event
      service.GetInTree()->GetEntry(event);

      // clear object store
      service.ClearStore();
      
      // execute analysis sequence
      bool skipEvent = false;
      for ( unsigned int algo = 0; algo < selectors.size(); ++algo ) {      
	if ( selectors.at(algo)->ExecuteEvent() != GLOBAL::SUCCESS ) {
	  skipEvent = true;
	  break;
	}
      }
      if (skipEvent) continue;
      
      // fill output tree
      if ( ! fillOutputTree ) continue;
      service.GetOutTree()->Fill();
      
    }

    double duration  = (std::clock() - start)/static_cast<double>(CLOCKS_PER_SEC);    
    double frequency = static_cast<double>(nEventsProcessed) / duration;
    log << Log::INFO
	<< "Processed :  100\%"
	<< "  ---  frequency : " << std::setw(6) << static_cast<int>(frequency) << " events/sec"
	<< "  ---  time : "      << std::setw(4) << static_cast<int>(duration) << " sec"
	<< "  ---  remaining time :    0 sec"<< Log::endl(); 
    
    // release pointers in selectors
    for ( unsigned int algo = 0; algo < selectors.size(); ++algo ) {      
      if ( selectors.at(algo)->EndInputFile() != GLOBAL::SUCCESS ) {
	log << Log::ERROR << "Couldn't release pointers!" << Log::endl();
	return 0;
      }
    }

  }


  // finalise selectors
  for ( unsigned int algo = 0; algo < selectors.size(); ++algo ) {          

    TDirectory* dir = static_cast<TDirectory *>(outFileHist->Get((selectors.back()->GetName()).c_str()));
    dir->cd();

    if ( selectors.at(algo)->Finalise() != GLOBAL::SUCCESS ) {
      log << Log::ERROR << "Couldn't finalise selectors!" << Log::endl();
      return 0;
    }
  }
  
  
  // save output
  if ( fillOutputTree ) {
    outFileNtup->Write();
    outFileNtup->Close();
  }
  outFileHist->Write();
  outFileHist->Close();
  
  log << Log::INFO << "Leaving program" << Log::endl();

  return 0;

}
