// Standard Template Library includes
#include <map>
#include <string>

// Analysis includes
#include "SelectorBase.h"
#include "AllSelectors.h"


SelectorBase::SelectorBase(const std::string & name, const Store & config, Service & service) :
  m_config(config), 
  m_name(name),
  m_service(service),
  m_log(name)
{

  // set log level
  std::string level;
  m_config.getif<std::string>( "loglevel" , level );
  m_log.SetLevel( Log::StringToLEVEL(level) );

}


GLOBAL::STATUS SelectorBase::EndInputFile()
{
  
  // Delete pointers called with 'new' operator in SelectorBase and set them to NULL

  if (m_ptrs.size()==0) {
    m_log << Log::DEBUG << "No pointers to release" << Log::endl();
    return GLOBAL::SUCCESS;
  }

  m_log << Log::INFO << "Releasing pointers..." << Log::endl();

  std::map<unsigned long,std::pair<void * , const char * > >::iterator iter = m_ptrs.begin(); 
  for ( ; iter!=m_ptrs.end(); ++iter) {
    
    m_log << Log::DEBUG << "Trying to delete pointer with type \"" << (iter->second).second << "\" and address = " << (iter->second).first << Log::endl();
    
    if      ( !strcmp((iter->second).second,TypeidType("UChar_t"  )          ) ) delete static_cast<UChar_t *>                                  ((iter->second).first);
    else if ( !strcmp((iter->second).second,TypeidType("Short_t"  )          ) ) delete static_cast<Short_t *>                                  ((iter->second).first);
    else if ( !strcmp((iter->second).second,TypeidType("UShort_t" )          ) ) delete static_cast<UShort_t *>                                 ((iter->second).first);
    else if ( !strcmp((iter->second).second,TypeidType("Int_t"    )          ) ) delete static_cast<Int_t *>                                    ((iter->second).first);
    else if ( !strcmp((iter->second).second,TypeidType("UInt_t"   )          ) ) delete static_cast<UInt_t *>                                   ((iter->second).first);
    else if ( !strcmp((iter->second).second,TypeidType("Float_t"  )          ) ) delete static_cast<Float_t *>                                  ((iter->second).first);
    else if ( !strcmp((iter->second).second,TypeidType("Double_t" )          ) ) delete static_cast<Double_t *>                                 ((iter->second).first);
    else if ( !strcmp((iter->second).second,TypeidType("Long64_t" )          ) ) delete static_cast<Long64_t *>                                 ((iter->second).first);
    else if ( !strcmp((iter->second).second,TypeidType("ULong64_t")          ) ) delete static_cast<ULong64_t *>                                ((iter->second).first);
    else if ( !strcmp((iter->second).second,TypeidType("Bool_t"   )          ) ) delete static_cast<Bool_t *>                                   ((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<short>"                  ) ) delete static_cast<std::vector<short> *>                       ((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<unsigned short>"         ) ) delete static_cast<std::vector<unsigned short> *>              ((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<int>"                    ) ) delete static_cast<std::vector<int> *>                         ((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<unsigned int>"           ) ) delete static_cast<std::vector<unsigned int> *>                ((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<float>"                  ) ) delete static_cast<std::vector<float> *>                       ((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<double>"                 ) ) delete static_cast<std::vector<double> *>                      ((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<long>"                   ) ) delete static_cast<std::vector<long> *>                        ((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<unsigned long>"          ) ) delete static_cast<std::vector<unsigned long> *>               ((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<bool>"                   ) ) delete static_cast<std::vector<bool> *>                        ((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<vector<short> >"         ) ) delete static_cast<std::vector<std::vector<short> > *>         ((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<vector<unsigned short> >") ) delete static_cast<std::vector<std::vector<unsigned short> > *>((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<vector<int> >"           ) ) delete static_cast<std::vector<std::vector<int> > *>           ((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<vector<unsigned int> >"  ) ) delete static_cast<std::vector<std::vector<unsigned int> > *>  ((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<vector<float> >"         ) ) delete static_cast<std::vector<std::vector<float> > *>         ((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<vector<double> >"        ) ) delete static_cast<std::vector<std::vector<double> > *>        ((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<vector<long> >"          ) ) delete static_cast<std::vector<std::vector<long> > *>          ((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<vector<unsogned long> >" ) ) delete static_cast<std::vector<std::vector<unsigned long> > *> ((iter->second).first);
    else if ( !strcmp((iter->second).second,"vector<vector<bool> >"          ) ) delete static_cast<std::vector<std::vector<bool> > *>          ((iter->second).first);
    else {
      m_log << Log::ERROR << "Could not release pointer for type = \"" << (iter->second).second << "\". This will cause a memory leak!" << Log::endl();
      return GLOBAL::ERROR;
    }
    
    // set pointer to zero
    (iter->second).first = 0;

  }

  // clear map of pointers
  m_ptrs.clear();


  return GLOBAL::SUCCESS;

}


SelectorBase * SelectorBase::CreateSelector(const std::string & name, const Store & config, Service & service) 
{

  // declare selector
  SelectorBase * pointer = 0;
  CREATE_SELECTOR(name,config,service,pointer);

  return pointer;

}


const char * SelectorBase::TypeidType( const char * root_type ) const
{
  if( ! strcmp( root_type, "Char_t" ) ) {
    return "c";
  } else if( ! strcmp( root_type, "UChar_t" ) ) {
    return "h";
  } else if( ! strcmp( root_type, "Short_t" ) ) {
    return "s";
  } else if( ! strcmp( root_type, "UShort_t" ) ) {
    return "t";
  } else if( ! strcmp( root_type, "Int_t" ) ) {
    return "i";
  } else if( ! strcmp( root_type, "UInt_t" ) ) {
    return "j";
  } else if( ! strcmp( root_type, "Float_t" ) ) {
    return "f";
  } else if( ! strcmp( root_type, "Double_t" ) ) {
    return "d";
  } else if( ! strcmp( root_type, "Long64_t" ) ) {
    return "x";
  } else if( ! strcmp( root_type, "ULong64_t" ) ) {
    return "y";
  } else if( ! strcmp( root_type, "Bool_t" ) ) {
    return "b";
  }
  return "";
}
