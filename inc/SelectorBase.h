// Dear emacs, this is -*- c++ -*-
#ifndef __SELECTORBASE__
#define __SELECTORBASE__

// Standard Template Library
#include <string>
#include <map>

// Analysis includes
#include "Enums.h"
#include "Service.h"
#include "Log.h"
#include "Store.h"

// forward declarations
class TFile;
class TBranch;


class SelectorBase {

public:

  // constructor
  SelectorBase(const std::string & name, const Store & config, Service & service);

  // analysis functions
  virtual GLOBAL::STATUS Initialise()     = 0;
  virtual GLOBAL::STATUS BeginInputFile() = 0;
  virtual GLOBAL::STATUS ExecuteEvent()   = 0;
  GLOBAL::STATUS         EndInputFile();
  virtual GLOBAL::STATUS Finalise()       = 0;

  // get name
  const std::string & GetName() const { return m_name ; }

  // get object store
  Store & store() { return m_service.GetStore() ; }
  
  // connect/declare variable in input/output trees
  template< typename T>
  void GetVariable(const char * _keyword, T *& _addr, const int & isNewVar=-1);
  template< typename T>
  void GetVariable(const char * _keyword, const T *& _addr, const int & isNewVar=-1);

  // get service
  const Service & GetService() const { return m_service; }

  // get logger 
  Log & log() const { return m_log ; }

  // configuration (card file)
  const Store & m_config;

  // declare selector
  static SelectorBase * CreateSelector(const std::string & name, const Store & config, Service & service);


private:

  // selector name
  const std::string & m_name;

  // service
  Service & m_service;

  // logger
  mutable Log m_log;

  // book-keeping of memory allocations
  std::map<unsigned long,std::pair<void *,const char *> > m_ptrs; // address of pointer , pair(pointer, type)

  // re-bind pointer for already connected variable 
  template< typename T>
  void SetAddress(TBranch* branch, T *& _addr);

  // convert root-type name
  const char * TypeidType(const char * root_type) const;

};

#include "SelectorBase.icc"

#endif

