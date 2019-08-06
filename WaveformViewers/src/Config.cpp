#include "Config.hpp"
#include "DBInterfaceConfig.hpp"

namespace global
{
ConfigPtr config{nullptr};// = create_default_config();

ConfigPtr create_default_config()
{
	ConfigPtr config = std::make_shared<DBInterfaceConfig<std::string>>();
    //    DER_ROOT
    const std::string DER_ROOT(std::getenv("DER_ROOT"));
    config->setConfig(DER_ROOT + "/DERCONFIG.txt");
    return config;
}
}
