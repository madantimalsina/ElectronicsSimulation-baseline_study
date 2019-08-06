//
//  DBInterfaceConfig.hpp
//  DER
//
//  Created by Cees Carels on 31/03/2017.
//  Copyright © 2017 Carels. All rights reserved.
//

#ifndef DBInterfaceConfig_hpp
#define DBInterfaceConfig_hpp

#include <stdio.h>

#include "DBInterface.hpp"

template <class TYPE> class DBInterfaceConfig : public DBInterface<TYPE>
{
public:
    DBInterfaceConfig();
    ~DBInterfaceConfig();

    void doConnect();
    bool doDisconnect();
    bool doCheckConnection();

    void RetrieveParameter(const std::string& name);

    void setConfig(const std::string& path);
    void setConfigSize(std::vector<std::string> titlevec, unsigned long size2);

    int CLISet(const std::string& SID, const std::string val);
    TYPE getConfig(const std::string& SID);
    TYPE getConfig(const std::string& SID, const unsigned int Entry);
    void setConfig(const std::string& SID, const unsigned int Entry, TYPE val);

    void Notification(std::string& par1, std::string& par2, std::string& par3);

    virtual TYPE getALL() override;

protected:
    std::string configFileName; //!< Configuration file name.
    int StartLine; //!< Line in text file where settings start.
    int configVersion; //!< Configuration source version number.
    std::vector<Setting<TYPE>> Settings; // Vector to hold setting information.
};

template <class TYPE>
DBInterfaceConfig<TYPE>::DBInterfaceConfig()
    : DBInterface<TYPE>()
    , configFileName("DERCONFIG.txt")
    , StartLine(7)
    , configVersion(2)
{
}

template <class TYPE> DBInterfaceConfig<TYPE>::~DBInterfaceConfig()
{
}

template <class TYPE> void DBInterfaceConfig<TYPE>::doConnect()
{
}

template <class TYPE> bool DBInterfaceConfig<TYPE>::doDisconnect()
{
    return true;
}

template <class TYPE> bool DBInterfaceConfig<TYPE>::doCheckConnection()
{
    bool didSucceed = false;

    return didSucceed;
}

template <class TYPE> void DBInterfaceConfig<TYPE>::RetrieveParameter(const std::string& name)
{
}

template <class TYPE> TYPE DBInterfaceConfig<TYPE>::getConfig(const std::string& SID)
{
    /**
     * Get configuration setting by identifying it using its SID.
     */

    for (int i = 0; i < Settings.size(); i++)
    {
        if (Settings.at(i).SID == SID)
            return Settings.at(i).Val;
    }

    std::cout << "Error: Setting " << SID << " was not found" << std::endl;
    return 0;
}

template <class TYPE> TYPE DBInterfaceConfig<TYPE>::getConfig(const std::string& SID, const unsigned int Entry)
{
    return TYPE();
}

template <class TYPE>
void DBInterfaceConfig<TYPE>::Notification(std::string& par1, std::string& par2, std::string& par3)
{
}

#endif /* DBInterfaceConfig_hpp */
