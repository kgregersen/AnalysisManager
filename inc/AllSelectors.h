// Autogenerated include file for Selector instantiation handling...
#include <string>
#include "MySelector.h"
#define CREATE_SELECTOR(name,config,service,pointer) \
pointer = 0; \
if (!std::string(name).compare("MySelector")) pointer = new MySelector(name,config,service); \
// EOL