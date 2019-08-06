#ifndef Config_hpp
#define Config_hpp

#include <memory>

#include "DBInterface.hpp"

namespace global
{
typedef std::shared_ptr<DBInterface<std::string>> ConfigPtr;

extern ConfigPtr config;

ConfigPtr create_default_config();
}
#endif
