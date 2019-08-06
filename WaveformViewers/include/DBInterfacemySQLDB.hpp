//
//  DBInterfacemySQLDB.hpp
//  DER
//
//  Created by Cees Carels on 31/03/2017.
//  Copyright © 2017 Carels. All rights reserved.
//

#ifndef DBInterfacemySQLDB_hpp
#define DBInterfacemySQLDB_hpp

#include <stdio.h>

#include "DBInterface.hpp"

template <class TYPE>
class DBInterfacemySQLDB : public DBInterface<TYPE>
{
public:
    DBInterfacemySQLDB();
    ~DBInterfacemySQLDB();

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

    bool isOK();

    void Notification(std::string& par1,
        std::string& par2,
        std::string& par3);

    TYPE getALL();

protected:
};

template <class TYPE>
DBInterfacemySQLDB<TYPE>::DBInterfacemySQLDB()
{
}

template <class TYPE>
DBInterfacemySQLDB<TYPE>::~DBInterfacemySQLDB()
{
}

template <class TYPE>
void DBInterfacemySQLDB<TYPE>::doConnect()
{
}

template <class TYPE>
bool DBInterfacemySQLDB<TYPE>::doDisconnect()
{
    return true;
}

template <class TYPE>
bool DBInterfacemySQLDB<TYPE>::doCheckConnection()
{
    return false;
}

template <class TYPE>
void DBInterfacemySQLDB<TYPE>::RetrieveParameter(const std::string& name)
{
}

template <class TYPE>
void DBInterfacemySQLDB<TYPE>::setConfig(const std::string& path)
{
}

template <class TYPE>
int DBInterfacemySQLDB<TYPE>::CLISet(const std::string& SID, const std::string val)
{
    return 1;
}

template <class TYPE>
TYPE DBInterfacemySQLDB<TYPE>::getConfig(const std::string& SID)
{
    return TYPE();
}

template <class TYPE>
TYPE DBInterfacemySQLDB<TYPE>::getConfig(const std::string& SID, const unsigned int Entry)
{
    return TYPE();
}

template <>
void DBInterfacemySQLDB<std::string>::setConfig(const std::string& SID, const unsigned int Entry, std::string val)
{
    std::cout << "Not yet implemented for this type" << std::endl;
}

template <>
void DBInterfacemySQLDB<double>::setConfig(const std::string& SID, const unsigned int Entry, double val)
{
    std::cout << "Not yet implemented for this type" << std::endl;
}

template <>
void DBInterfacemySQLDB<double>::setConfigSize(std::vector<std::string> titlevec, unsigned long size2)
{
    //Determine which column by comparing title
    //Then get row "Entry"
    std::cout << "Not yet implemented" << std::endl;
}

template <>
void DBInterfacemySQLDB<std::string>::setConfigSize(std::vector<std::string> titlevec, unsigned long size2)
{
    //Determine which column by comparing title
    //Then get row "Entry"
    std::cout << "Not yet implemented" << std::endl;
}

template <class TYPE>
bool DBInterfacemySQLDB<TYPE>::isOK()
{
    return false;
}

template <class TYPE>
void DBInterfacemySQLDB<TYPE>::Notification(std::string& par1,
    std::string& par2,
    std::string& par3)
{
}

template <class TYPE>
TYPE DBInterfacemySQLDB<TYPE>::getALL()
{
    return TYPE();
}

#endif /* DBInterfacemySQLDB_hpp */
