// ROOT includes
#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"

// Analysis includes
#include "Service.h"
#include "Enums.h"


Service::Service(const Log::LEVEL & logLevel) : 
  m_treeName("tree"),
  m_inTree(0), 
  m_outTree(0),
  m_counter(0),
  m_nEvents(0),
  m_log("Service")
{

  // set log level
  m_log.SetLevel(logLevel);

}


Service::~Service() 
{

  // close input files
  for (unsigned int ifile = 0; ifile < m_inFiles.size(); ++ifile) {
    m_inFiles.at(ifile)->Close();
    delete m_inFiles.at(ifile);
  }  

}


GLOBAL::STATUS Service::PrepareOutTree()
{

  // initialise output tree
  if ( m_outTree ) {
    m_log << Log::ERROR << "Output tree already initialised!" << Log::endl();
    return GLOBAL::ERROR;
  }
  m_outTree = new TTree(m_treeName.c_str(),m_treeName.c_str());

  return GLOBAL::SUCCESS;
  
}


GLOBAL::STATUS Service::PrepareInput(const std::vector<std::string> & inFileNames) 
{

  // open files in fileNames 
  for (unsigned int ifile = 0; ifile < inFileNames.size(); ++ifile) {
     
    // get file
    const std::string & name = inFileNames.at( ifile );
    TFile * file = new TFile( name.c_str() , "read" );
    
    // check if file is found
    if ( ! file->IsOpen() ) {
      m_log << Log::ERROR << "Couldn't open file \"" << name << "\"" << Log::endl();
      return GLOBAL::ERROR;
    }
    
    // check if tree is there
    TTree * tree = static_cast<TTree*>( file->Get( m_treeName.c_str() ) );
    if ( ! tree ) {
      m_log << Log::ERROR << "Couldn't open tree \"" << m_treeName << "\"in file \"" << file->GetName() << "\"" << Log::endl();
      return GLOBAL::ERROR;
    }
    
    // register file
    m_inFiles.push_back( file );

    // add to event counter
    m_nEvents += tree->GetEntries();
   
  }

  return GLOBAL::SUCCESS;
  
}


GLOBAL::STATUS Service::NextInTree()
{
  
  // reset input tree
  m_inTree = 0;
  
  // check index
  if ( m_counter < 0 || m_counter > m_inFiles.size() ) {
    m_log << Log::ERROR << "Couldn't get next input tree - index is out of range (" << m_counter << ")" << Log::endl();
    return GLOBAL::ERROR;
  }

  // get next file
  TFile * file = m_inFiles.at( m_counter++ );
  
  // get tree
  m_inTree = static_cast<TTree *>( file->Get( m_treeName.c_str() ) );
  if ( ! m_inTree ) {
    m_log << Log::ERROR << "Couldn't get tree \"" << m_treeName << "\"in file \"" << file->GetName() << "\"" << Log::endl();
    return GLOBAL::ERROR;    
  }

  // disable all branches (later, used branches will be activated by selectors)
  m_inTree->SetBranchStatus("*",0);
 
  m_log << Log::INFO << "Loaded tree \"" << m_treeName << "\"in file \"" << file->GetName() << "\"" << Log::endl();
  
  return GLOBAL::SUCCESS;

}
