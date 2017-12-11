// Standard Template Library includes
#include <string>

// Analysis includes
#include "MySelector.h"
#include "Store.h"


MySelector::MySelector(const std::string & name, const Store & config, Service & service) :
  SelectorBase(name,config,service),
  my_int(0),
  my_float(0),
  my_vector_float(0),
  new_float(0),
  new_vector_float(0),
  my_hist1D(0)
{
  
}

GLOBAL::STATUS MySelector::Initialise() 
{

  log() << Log::INFO << "Initialising..." << Log::endl();

  // get parameters from steer file
  my_float_min = m_config.get<float>("MySelector::my_float_min");
  my_int_min   = m_config.get<int>("MySelector::my_int_min");

  // declare histograms
  my_hist1D = new TH1F("my_hist","my_hist",100,0.,100.); 

  return GLOBAL::SUCCESS;
  
}

GLOBAL::STATUS MySelector::BeginInputFile() 
{

  // connect variables to input and output trees
  log() << Log::INFO << "Connecting variables" << Log::endl();
  GetVariable( "my_int"           , my_int           );
  GetVariable( "my_float"         , my_float         );
  GetVariable( "my_vector_float"  , my_vector_float  );
  GetVariable( "new_float"        , new_float        );
  GetVariable( "new_vector_float" , new_vector_float );
  
  return GLOBAL::SUCCESS;
  
}


GLOBAL::STATUS MySelector::ExecuteEvent()
{

  // clear vector not present in input tree
  new_vector_float->clear();
  
  // set output variables - here just dummy values
  (*new_float) = (*my_float) + static_cast<float>(*my_int);
  new_vector_float->push_back( (*new_float)/2. );
  new_vector_float->push_back( (*new_float)*2. - (*my_float) );

  // note, that it is also possible to overwrite variables from the input tree
  // BUT be careful - any following selector will use the modified values!

  // do selection
  int nAbove = 0;
  for (unsigned int i=0; i<my_vector_float->size(); ++i) {
    if ( my_vector_float->at(i) < my_float_min ) {
      ++nAbove;
    }
  }
  if ( nAbove < my_int_min ) return GLOBAL::SKIP;
  
  // fill histogram
  my_hist1D->Fill( (*new_float) );


  // event passes selector
  return GLOBAL::SUCCESS;

}


GLOBAL::STATUS MySelector::Finalise()
{

  log() << Log::INFO << "Finalising..." << Log::endl();

  return GLOBAL::SUCCESS;

}


