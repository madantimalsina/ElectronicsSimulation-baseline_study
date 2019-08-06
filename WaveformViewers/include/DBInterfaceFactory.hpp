//
//  DBInterfaceFactory.hpp
//  DER
//
//  Created by Cees Carels on 31/03/2017.
//  Copyright © 2017 Carels. All rights reserved.
//

#ifndef DBInterfaceFactory_hpp
#define DBInterfaceFactory_hpp

#include <stdio.h>

#include "DBInterface.hpp"
#include "InputOutputFormats.hpp"

#include "DBInterfaceCSV.hpp"
#include "DBInterfaceConfig.hpp"
#include "DBInterfaceMongoDB.hpp"
#include "DBInterfacemySQLDB.hpp"

/**
 * Abstract factory that provides interface to concrete factory
 * implementations of DBInterface classes.
 *
 * DBInterface classes are used to load, for example, the DERCONFIG file and
 * PMT gain settings. These could come from text files or from a database
 */

template <class TYPE>
class DBInterfaceFactory
{
public:
    DBInterfaceFactory();
    ~DBInterfaceFactory();

    std::shared_ptr<DBInterface<TYPE>> createDB(const format::config factory);
};

template <class TYPE>
DBInterfaceFactory<TYPE>::DBInterfaceFactory()
{
}

template <class TYPE>
DBInterfaceFactory<TYPE>::~DBInterfaceFactory()
{
}

#endif /* DBInterfaceFactory_hpp */

template <class TYPE>
std::shared_ptr<DBInterface<TYPE>> DBInterfaceFactory<TYPE>::createDB(const format::config factory)
{
    /**
     * Method to provide the correct derived class concrete factory
     * object.
     */
    if (factory == format::config::CONFIG)
    {
        return std::make_shared<DBInterfaceConfig<TYPE>>();
    }
    else if (factory == format::config::CSV)
    {
        return std::make_shared<DBInterfaceCSV<TYPE>>();
    }
    else if (factory == format::config::MONGODB)
    {
        return std::make_shared<DBInterfaceMongoDB<TYPE>>();
    }
    else if (factory == format::config::MYSQL)
    {
        return std::make_shared<DBInterfacemySQLDB<TYPE>>();
    }
    else
        return std::make_shared<DBInterfaceCSV<TYPE>>(); //Default option
}
