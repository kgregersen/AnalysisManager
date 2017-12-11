// Dear emacs, this is -*- c++ -*-
#ifndef __SERVICE__
#define __SERVICE__

// Standard Template Library includes
#include <string>
#include <vector>

// Analysis includes
#include "Enums.h"
#include "Log.h"
#include "Store.h"

// forward declarations
class TTree;
class TFile;


class Service {

public:

  // constructor
  Service(const Log::LEVEL& logLevel);

  // destructor
  ~Service();

  // utility functions
  GLOBAL::STATUS PrepareOutTree();
  GLOBAL::STATUS PrepareInput(const std::vector<std::string>& inFileNames);
  GLOBAL::STATUS NextInTree();


  // get object store
  Store & GetStore() { return m_objects; }

  // clear object store
  void ClearStore() { m_objects.flush(); }
  
  // inline functions
  void       SetTreeName(std::string& treeName) { m_treeName = treeName;    }
  TTree *    GetInTree  ()                      { return m_inTree;          }
  TTree *    GetOutTree ()                      { return m_outTree;         }
  long       GetNEvents () const                { return m_nEvents;         }
  Log::LEVEL GetLogLevel() const                { return m_log.GetLevel() ; }
  
  // connect/declare variables in input and output trees
  template< typename T>
  void ConnectVariable(const char* _keyword, T*& _addr);
  template< typename T>
  void ConnectVariable(const char* _keyword, T& _addr);
  template< typename T>
  void DeclareVariable(const char* _keyword, T*& _addr);
  template< typename T>
  void DeclareVariable(const char* _keyword, T& _addr);

  
private:

  // Object store (for passing objects between selectors)
  Store m_objects;

  // TTrees
  std::string m_treeName;
  TTree * m_inTree;
  TTree * m_outTree;

  // input files
  unsigned int m_counter;
  std::vector<TFile *> m_inFiles;
  
  // total number of events
  long m_nEvents;

  // logger
  Log m_log;

};

#include "Service.icc"

#endif
