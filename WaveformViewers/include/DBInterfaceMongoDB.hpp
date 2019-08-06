//
//  DBInterfaceMongoDB.hpp
//  DER
//
//  Created by Cees Carels on 31/03/2017.
//  Copyright © 2017 Carels. All rights reserved.
//

#ifndef DBInterfaceMongoDB_hpp
#define DBInterfaceMongoDB_hpp

#include <stdio.h>

#include "DBInterface.hpp"

template <class TYPE>
class DBInterfaceMongoDB : public DBInterface<TYPE>
{
public:
    DBInterfaceMongoDB();
    ~DBInterfaceMongoDB();

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
DBInterfaceMongoDB<TYPE>::DBInterfaceMongoDB()
{
}

template <class TYPE>
DBInterfaceMongoDB<TYPE>::~DBInterfaceMongoDB()
{
}

template <class TYPE>
void DBInterfaceMongoDB<TYPE>::doConnect()
{
}

template <class TYPE>
bool DBInterfaceMongoDB<TYPE>::doDisconnect()
{
    return true;
}

template <class TYPE>
bool DBInterfaceMongoDB<TYPE>::doCheckConnection()
{
    return false;
}

template <class TYPE>
void DBInterfaceMongoDB<TYPE>::RetrieveParameter(const std::string& name)
{
}

template <class TYPE>
void DBInterfaceMongoDB<TYPE>::setConfig(const std::string& path)
{
}

template <class TYPE>
int DBInterfaceMongoDB<TYPE>::CLISet(const std::string& SID, const std::string val)
{
    return 1;
}

template <>
void DBInterfaceMongoDB<double>::setConfigSize(std::vector<std::string> titlevec, unsigned long size2)
{
    //Determine which column by comparing title
    //Then get row "Entry"
    std::cout << "Not yet implemented" << std::endl;
}

template <>
void DBInterfaceMongoDB<std::string>::setConfigSize(std::vector<std::string> titlevec, unsigned long size2)
{
    //Determine which column by comparing title
    //Then get row "Entry"
    std::cout << "Not yet implemented" << std::endl;
}

template <class TYPE>
TYPE DBInterfaceMongoDB<TYPE>::getConfig(const std::string& SID)
{
    return TYPE();
}

template <class TYPE>
TYPE DBInterfaceMongoDB<TYPE>::getConfig(const std::string& SID, const unsigned int Entry)
{
    return TYPE();
}

template <>
void DBInterfaceMongoDB<std::string>::setConfig(const std::string& SID, const unsigned int Entry, std::string val)
{
    std::cout << "Not yet implemented for this type" << std::endl;
}

template <>
void DBInterfaceMongoDB<double>::setConfig(const std::string& SID, const unsigned int Entry, double val)
{
    std::cout << "Not yet implemented for this type" << std::endl;
}

template <class TYPE>
bool DBInterfaceMongoDB<TYPE>::isOK()
{
    return false;
}

template <class TYPE>
void DBInterfaceMongoDB<TYPE>::Notification(std::string& par1,
    std::string& par2,
    std::string& par3)
{
}

template <class TYPE>
TYPE DBInterfaceMongoDB<TYPE>::getALL()
{
    return TYPE();
}

#endif /* DBInterfaceMongoDB_hpp */
