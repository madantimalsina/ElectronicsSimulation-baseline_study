//
//  EBGlobal.cpp
//  devices
//
//  Created by Cees Carels on 12/03/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "EBGlobal.hpp"

EBGlobal::EBGlobal()
{
    /**
     * Constructor for EBGlobal
     */
}

EBGlobal::~EBGlobal()
{
    /**
     * Destructor for EBGlobal
     */
}

void EBGlobal::setRunNumber(const unsigned int& run)
{
    /**
     * Set the run number for the experiment.
     *
     * Refer to the DAQ and EB formats to understand this variable better.
     *
     */
    runNumber = run;
}

unsigned long EBGlobal::getRunNumber()
{
    /**
     * Return the current run number for the experiment.
     *
     * Refer to the DAQ and EB formats to understand this variable better
     *
     */
    return runNumber;
}

void EBGlobal::setFormatVersion(const unsigned short& id)
{
    /**
     * Set the format version number for the current output format.
     */
    formatVersionID = id;
}

unsigned long EBGlobal::getFormatVersion()
{
    /**
     * Get the format version number for the current output format.
     */
    return formatVersionID;
}

void EBGlobal::setRunStartTime(const unsigned long long& time)
{
    /**
     * Set the start time of the run.
     *
     * It is assumed the input time is the GPS time.
     *
     * Then, the GPS time is converted into the number of 10 ns samples,
     * in order to agree with LZ-ICD-08-0007.
     */
    runStartTime = time * 1e8;
}

unsigned long long EBGlobal::getRunStartTime()
{
    /**
     * Get the start time of the run.
     */
    return runStartTime;
}

void EBGlobal::setRunType(unsigned short type)
{
    /**
     * Set run type.
     */
    runType = type;
}

unsigned short EBGlobal::getRunType()
{
    /**
     * Get run type.
     */
    return runType;
}
