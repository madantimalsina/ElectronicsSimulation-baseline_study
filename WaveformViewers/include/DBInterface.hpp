//
//  DBInterface.hpp
//  devices
//
//  Created by Cees Carels on 17/02/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef DBInterface_hpp
#define DBInterface_hpp

#include <iostream>
#include <memory>
#include <stdio.h>
#include <string>

#include "Riostream.h"
#include "TFile.h"

#include "Setting.hpp"

/**
 * Base class providing interface to database for retrieving 
 * parameter values.
 */

template <class TYPE>
class DBInterface
{
public:
    DBInterface();
    virtual ~DBInterface() = 0;

    virtual void doConnect() = 0;
    virtual bool doDisconnect() = 0;
    virtual bool doCheckConnection() = 0;

    virtual void RetrieveParameter(const std::string& name) = 0;

    virtual void setConfig(const std::string& path) = 0;
    virtual void setConfigSize(std::vector<std::string> titlevec, unsigned long size2) = 0;

    virtual int CLISet(const std::string& SID, const std::string val) = 0;
    virtual TYPE getConfig(const std::string& SID) = 0;
    virtual TYPE getConfig(const std::string& SID, const unsigned int Entry) = 0;
    virtual void setConfig(const std::string& SID, const unsigned int Entry, TYPE val) = 0;

    bool isOK();

    virtual void Notification(std::string& par1, std::string& par2, std::string& par3) = 0;

    virtual TYPE getALL() = 0;

protected:
    bool confIsOK; //!< Check if source is ok to be used.
    bool connectSuccess; //!< Check if successfully connected to source.
};

template <class TYPE>
DBInterface<TYPE>::DBInterface()
{
    /**
     * Constructor for DBInterface.
     */
}

template <class TYPE>
DBInterface<TYPE>::~DBInterface()
{
    /**
     * Destructor for DBInterface.
     */
}

template <class TYPE>
bool DBInterface<TYPE>::isOK()
{
    /**
     * Check if the configuration source is OK to be used.
     */
    return confIsOK;
}

template <class TYPE>
void DBInterface<TYPE>::Notification(std::string& par1, std::string& par2, std::string& par3)
{
    /**
     * Method does nothing for now. Can be used to provide a
     * notification when required.
     */
    if (getConfig("doNotify") == "true")
    {
    }
}

#endif /* DBInterface_hpp */
