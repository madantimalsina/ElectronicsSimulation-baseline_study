//
//  DERSummary.hpp
//  devices
//
//  Created by Cees Carels on 12/03/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef DERSummary_hpp
#define DERSummary_hpp

#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <vector>

/**
 * The DERSummary class contains information about how the simulation was
 * configured and run.
 */

class DERSummary
{
public:
    DERSummary();
    ~DERSummary();

    void setFileName(const std::vector<std::string>& name);
    std::string getFileName();

    void setDERFileName(const std::string& name);
    std::string getDERFileName();

    void setUserName(const std::string& name);
    std::string getUserName();

    int setDERUserName();
    std::string getDERUserName();

    void setDERUserGroupID();
    std::string getDERUserGroupID();

    int setDERHostName();
    std::string getDERHostName();

    void setFileRandomSeed(const std::string& seed);
    std::string getFileRandomSeed();

    void setDERRandomSeed(const std::string& seed);
    std::string getDERRandomSeed();

    void setPosixTime(const std::string& time); //DER start time
    std::string getPosixTime();

    void setLocalTime(const std::string& time); //DER start time
    std::string getLocalTime();

    void setSamplingRate_ns(const unsigned int& smplrate_ns);
    unsigned int getSamplingRate_ns();

    void setDERCONFIG(const std::string& CONFIG);
    std::string getDERCONFIG();

    void setDERRunID(const std::string& RunID);
    std::string getDERRunID();

    void setGitCommitHash(const std::string& Hash);
    std::string getGitCommitHash();

private:
    std::string FileName;
    std::string DERFileName;
    std::string Uname;
    std::string DERUname;
    std::string DERUserGroupID;
    std::string DERHostName;
    std::string SimFileRandomSeed;
    std::string DERFileRandomSeed;
    std::string PosixTime;
    std::string LocalTime;
    unsigned int samplingRate_ns;
    std::string DERCONFIG;
    std::string DERRunID;
    std::string GitCommitHash;
};

#endif /* DERSummary_hpp */
