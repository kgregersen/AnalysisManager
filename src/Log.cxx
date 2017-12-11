// Standard Template Library includes
#include <algorithm>

// Analysis includes
#include "Log.h"


Log::LEVEL Log::StringToLEVEL(const std::string& str_level)
{

  // convert to lower case
  std::string lowercase(str_level);
  std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);

  // determine level
  if      ( lowercase == "debug"   ) return Log::DEBUG;
  else if ( lowercase == "info"    ) return Log::INFO;
  else if ( lowercase == "warning" ) return Log::WARNING;
  else if ( lowercase == "error"   ) return Log::ERROR;
  
  // not recognised
  return Log::INDENT;
  
}

