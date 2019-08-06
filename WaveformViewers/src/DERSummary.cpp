//
//  DERSummary.cpp
//  devices
//
//  Created by Cees Carels on 12/03/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "DERSummary.hpp"

DERSummary::DERSummary()
{
    /**
     * Constructor for DERSummary
     */
}

DERSummary::~DERSummary()
{
    /**
     * Destructor for DERSummary
     */
}

void DERSummary::setFileName(const std::vector<std::string>& name)
{
    /**
     * Set the simulation input file name.
     */
    for (int i = 0; i < name.size(); i++)
    {
        FileName += name[i];
        FileName += ". ";
    }
}

std::string DERSummary::getFileName()
{
    /**
     * Return the simulation input file name.
     */
    return FileName;
}

void DERSummary::setDERFileName(const std::string& name)
{
    /**
     * Set the DER file name.
     */
    DERFileName = name;
}

std::string DERSummary::getDERFileName()
{
    /**
     * Get the DER file name.
     */
    return DERFileName;
}

void DERSummary::setUserName(const std::string& name)
{
    /**
     * Set the username of the user who ran the input file simulation (LUXSim).
     */
    Uname = name;
}

std::string DERSummary::getUserName()
{
    /**
     * Get the username of the user who ran the input file simulation (LUXSim).
     */
    return Uname;
}

int DERSummary::setDERUserName()
{
    /**
     * Set the username of the user currently running the DER simulation.
     */
    int size = 1024;
    char username[size];
    int result;

    result = getlogin_r(username, size);
    if (result)
    {
        DERUname = "Unknown";
        return EXIT_FAILURE;
    }
    DERUname = username;
    return EXIT_SUCCESS;
}

std::string DERSummary::getDERUserName()
{
    /**
     * Get the username of the user currently running the DER simulation.
     */
    return DERUname;
}

int DERSummary::setDERHostName()
{
    /**
     * Set the host name of the host of the user 
     * currently running the DER simulation.
     */
    int size = 1024;
    char hostname[size];
    int result;
    result = gethostname(hostname, size);
    if (result)
    {
        perror("gethostname");
        return EXIT_FAILURE;
    }
    DERHostName = hostname;
    return EXIT_SUCCESS;
}

void DERSummary::setDERUserGroupID()
{
    gid_t mygroup = getegid();
    group mygroupidinfo = *getgrgid(mygroup);
    DERUserGroupID = mygroupidinfo.gr_name;
}

std::string DERSummary::getDERUserGroupID()
{
    return DERUserGroupID;
}

std::string DERSummary::getDERHostName()
{
    /**
     * Get the host name of the host of the user
     * currently running the DER simulation.
     */
    return DERHostName;
}

void DERSummary::setFileRandomSeed(const std::string& seed)
{
    /**
     * Set the file random seed of the simulation input file (LUXSim).
     */
    SimFileRandomSeed = seed;
}

std::string DERSummary::getFileRandomSeed()
{
    /**
     * Get the file random seed of the simulation input file (LUXSim).
     */
    return SimFileRandomSeed;
}

void DERSummary::setDERRandomSeed(const std::string& seed)
{
    /**
     * Set the DER random number generator seed.
     */
    DERFileRandomSeed = seed;
}

std::string DERSummary::getDERRandomSeed()
{
    /**
     * Get the DER random number generator seed.
     */
    return DERFileRandomSeed;
}

void DERSummary::setPosixTime(const std::string& time)
{
    /**
     * Set the current Posix time.
     */
    PosixTime = time;
}

std::string DERSummary::getPosixTime()
{
    /**
     * Get the current Posix time.
     */
    return PosixTime;
}

void DERSummary::setLocalTime(const std::string& time)
{
    /**
     * Set the local time of the simulation.
     *
     * This time is usually produced at the start of the DER simulation.
     */
    LocalTime = time;
}

std::string DERSummary::getLocalTime()
{
    /**
     * Get the local time of the simulation.
     *
     * This time is usually produced at the start of the DER simulation.
     */
    return LocalTime;
}

void DERSummary::setSamplingRate_ns(const unsigned int& smplrate_ns)
{
    /**
     * Set the sampling rate that the DERSummary will report.
     */
    samplingRate_ns = smplrate_ns;
}

unsigned int DERSummary::getSamplingRate_ns()
{
    /**
     * Get the sampling rate that the DERSummary will report.
     */
    return samplingRate_ns;
}

void DERSummary::setDERCONFIG(const std::string& CONFIG)
{
    /**
     * Set the entire DERCONFIG file as a string that the DERSummary
     * can report.
     */
    DERCONFIG = CONFIG;
}

std::string DERSummary::getDERCONFIG()
{
    /**
     * Get the entire DERCONFIG file as it was run.
     */
    return DERCONFIG;
}

void DERSummary::setDERRunID(const std::string& RunID)
{
    /**
     * Get the DERRunID from the DERCONFIG file as a string
     */
    DERRunID = RunID;
}

std::string DERSummary::getDERRunID()
{
    /**
     * Get the DER Run ID.
     */

    return DERRunID;
}

void DERSummary::setGitCommitHash(const std::string& Hash)
{
    GitCommitHash = Hash;
}

std::string DERSummary::getGitCommitHash()
{
    return GitCommitHash;
}
