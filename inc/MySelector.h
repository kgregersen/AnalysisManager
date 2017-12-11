#ifndef MYSELECTOR
#define MYSELECTOR

// Standard Template Library includes
#include <vector>
#include <string>

// ROOT includes
#include "TH1F.h"

// Analysis includes
#include "SelectorBase.h"
#include "Enums.h"
#include "Log.h"
#include "Service.h"

// forward declarations
class Store;

class MySelector : public SelectorBase {

public:

  // constructor
  MySelector (const std::string & name, const Store & config, Service & service);

  // analysis functions
  GLOBAL::STATUS Initialise();
  GLOBAL::STATUS BeginInputFile();
  GLOBAL::STATUS ExecuteEvent();
  GLOBAL::STATUS Finalise();


private:

  // variables in input and output tree (need to be pointers!)
  int                * my_int;
  float              * my_float;
  std::vector<float> * my_vector_float; 

  // new variables declared in output tree (need to be pointers!)
  float              * new_float;
  std::vector<float> * new_vector_float; 
  
  // parameters
  float my_float_min;
  int   my_int_min;

  // histograms
  TH1F* my_hist1D;

};

#endif

