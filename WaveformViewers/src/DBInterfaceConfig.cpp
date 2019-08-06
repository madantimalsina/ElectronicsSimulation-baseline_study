#include "DBInterfaceConfig.hpp"

template <> void DBInterfaceConfig<std::string>::setConfig(const std::string& path)
{
    /**
     * Set configuration values by sourcing from a text file.
     */
    TString dir = path;

    std::ifstream config;
    config.open(dir);

    if (!config.good())
    {
        DBInterface<std::string>::confIsOK = false;
        std::cout << "Error:\tcannot open config file, use default settings" << std::endl;
    }
    else
    {
        DBInterface<std::string>::confIsOK = true;
        std::vector<std::string> lines;
        while (config.good())
        {
            std::string line;
            getline(config, line);
            lines.push_back(line);
        }

        for (int i = StartLine; i < lines.size(); i++)
        {
            std::string s = lines[i];
            std::string delimiter = "\t";
            size_t pos = 0;
            std::string token;

            while ((pos = s.find(delimiter)) != std::string::npos)
            {
                Setting<std::string> CurrSetting;
                token = s.substr(0, pos);
                CurrSetting.SID = token;
                s.erase(0, pos + delimiter.length());
                while ((pos = s.find(delimiter)) != std::string::npos)
                {
                    token = s.substr(0, pos);
                    if (token.size() != 0)
                    {
                        CurrSetting.Val = token;
                        Settings.push_back(CurrSetting);
                    }
                    s.erase(0, pos + delimiter.length());
                }
            }
        }

        if (std::stoi(getConfig("Config")) != configVersion)
            std::cout << "Configuration file version mismatch, use defaults" << std::endl;
        else
            std::cout << "Settings loaded from " << dir << std::endl;
    }

    config.close();
}

template <> void DBInterfaceConfig<double>::setConfig(const std::string& path)
{
    /**
     * Set configuration values by sourcing from a text file.
     */
    std::cout << "Case not implemented" << std::endl;
}

template <>
void DBInterfaceConfig<std::string>::setConfig(const std::string& SID, const unsigned int Entry, std::string val)
{
    // Determine which column by comparing title
    // Then get row "Entry"
    std::cout << "Not yet implemented" << std::endl;
}

template <> void DBInterfaceConfig<double>::setConfig(const std::string& SID, const unsigned int Entry, double val)
{
    // Determine which column by comparing title
    // Then get row "Entry"
    std::cout << "Not yet implemented" << std::endl;
}

template <> int DBInterfaceConfig<std::string>::CLISet(const std::string& SID, const std::string val)
{
    /**
     * Override a DERCONFIG setting that was provided as a CLI
     * flag.
     */
    for (int i = 0; i < Settings.size(); i++)
    {
        if (Settings.at(i).SID == SID)
        {
            std::cout << "CLI Setting Override: " << Settings.at(i).SID;
            std::cout << ": " << Settings.at(i).Val;
            Settings.at(i).Val = val;
            std::cout << "\t->\t" << Settings.at(i).Val << std::endl;
            return 0;
        }
    }

    std::cout << "Setting " << SID << " was not found." << std::endl;
    std::cout << "A user input error is assumed." << std::endl;
    return -1;
}

template <> int DBInterfaceConfig<double>::CLISet(const std::string& SID, const std::string val)
{
    /**
     * Override a DERCONFIG setting that was provided as a CLI
     * flag.
     */
    std::cout << "Type not implemented" << std::endl;
    return 0;
}

template <> std::string DBInterfaceConfig<std::string>::getALL()
{
    /**
     * Method to return the DERCONFIG including any CLI overrides, if
     * the method is called after the overrides are processed.
     */
    std::string CurrentConfig;
    CurrentConfig += "DERCONFIG SETTINGS\n";

    for (int i = 0; i < Settings.size(); i++)
    {
        CurrentConfig += Settings[i].SID + " " + Settings[i].Val + "\n";
    }

    return CurrentConfig;
}

template <> double DBInterfaceConfig<double>::getALL()
{
    /**
     * Method to return the DERCONFIG including any CLI overrides, if
     * the method is called after the overrides are processed.
     */
    return 0;
}

template <> void DBInterfaceConfig<std::string>::setConfigSize(std::vector<std::string> titlevec, unsigned long size2)
{
    std::cout << "Not yet implemented for this type" << std::endl;
}

template <> void DBInterfaceConfig<double>::setConfigSize(std::vector<std::string> titlevec, unsigned long size2)
{
    std::cout << "Not yet implemented for this type" << std::endl;
}
