//
//  Output.cpp
//  devices
//
//  Created by Cees Carels on 07/02/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include <fstream>
#include <iostream>
#include <string>

#include "Device.hpp"
#include "Output.hpp"

#include "TFile.h"
#include "TLeaf.h"
#include "TTree.h"

unsigned short Output::nPODInEvt{0};

Output::Output()
    : globalEvt(0)
{
    /**
     * Constructor for Output.
     */
    //Max size determined by Event Builder
    ChTriggerTSActiveS1.resize(2000, 0);
    ChTriggerTSActiveS2.resize(2000, 0);
}

Output::~Output()
{
    /**
     * Destructor for Output.
     */
}

void Output::doInitInputVariables(Input* input)
{
    /**
     * Method intentionally left empty to satisfy the interface
     */
}

void Output::doStoreSubsetLG(const Pulse& thePulse)
{
    /**
     * Acts as an intermediary between the signal chain pulse and preparation
     * of the pulse for the output.
     *
     * This method will become deprecated in view of performance enhancements.
     *
     * This method is intended to be used only with low-gain channel pulses.
     */

    OutputVectorLG.push_back(thePulse);
}

void Output::doStoreSubsetHG(const Pulse& thePulse)
{
    /**
     * Acts as an intermediary between the signal chain pulse and preparation
     * of the pulse for the output.
     *
     * This method will become deprecated in view of performance enhancements.
     *
     * This method is intended to be used only with high-gain channel pulses.
     */
    OutputVectorHG.push_back(thePulse);
}

void Output::setSimSource(const std::vector<std::string>& SimSourceFileName)
{
    /**
     * Set the name of the simulation input file (LUXSim).
     */

    for (int i = 0; i < SimSourceFileName.size(); i++)
    {
        SimInputFileName += SimSourceFileName[i];
        SimInputFileName += ". ";
    }
}

void Output::setNumDCs(const unsigned int& DCs,
    const unsigned int& ChanPerDDC32)
{
    /**
     * Set number of Data Collectors (DC), and the number of channels per DC.
     *
     * This is used to set up the output.
     */
    numDCs = DCs;
    ChPerDDC32 = ChanPerDDC32;
}

std::string Output::getOutFileName()
{
    /**
     * Get the DER output file name.
     */
    return outputFull;
}

void Output::setTime(const time_t& CurrentTime)
{
    /**
     * Provide a time_t time and parse this to produce a series of strings
     * that can be used to form the full time stamp used in the DER output
     * file name.
     *
     * The format is identical to the Event Builder format: LZ-ICD-08-0007.
     */
    //UTC time
    time_t rawtime = CurrentTime;
    struct tm* ptm;
    ptm = gmtime(&rawtime);

    std::string leadingzero = "0";

    yyyy = std::to_string(ptm->tm_year + 1900);

    //Month
    if (ptm->tm_mon + 1 < 10)
        mm = leadingzero + std::to_string(ptm->tm_mon + 1);
    else
        mm = std::to_string(ptm->tm_mon + 1);

    //Day
    if (ptm->tm_mday < 10)
        dd = leadingzero + std::to_string(ptm->tm_mday);
    else
        dd = std::to_string(ptm->tm_mday);

    //Hour
    if (ptm->tm_hour < 10)
        HH = leadingzero + std::to_string(ptm->tm_hour);
    else
        HH = std::to_string(ptm->tm_hour);

    //Minute
    if (ptm->tm_min < 10)
        MM = leadingzero + std::to_string(ptm->tm_min);
    else
        MM = std::to_string(ptm->tm_min);

    //Second
    if (ptm->tm_sec < 10)
        ss = leadingzero + std::to_string(ptm->tm_sec);
    else
        ss = std::to_string(ptm->tm_sec);
}

unsigned long long Output::getPosixTime()
{
    /**
     * Get the current Posix time.
     */
    return std::stoull(PosixTime);
}

std::vector<Pulse> Output::getFullChannelPulseLG()
{
    /**
     * Retrieve the intermediary pulse that was set with Output::doStoreSubsetLG(pulse thePulse)
     *
     * This method will become deprecated in view of performance enhancements.
     */
    return OutputVectorLG;
}

std::vector<Pulse> Output::getFullChannelPulseHG()
{
    /**
     * Retrieve the intermediary pulse that was set with Output::doStoreSubsetHG(pulse thePulse)
     *
     * This method will become deprecated in view of performance enhancements.
     */
    return OutputVectorHG;
}

void Output::setRunNumber(const unsigned int& RunNumber)
{
    /**
     * Set run number.
     */
    runNumber = RunNumber;
}

void Output::IncEvtNum()
{
    ++globalEvt;
}

UInt_t Output::EvtNum()
{
    return globalEvt;
}

std::string Output::setRunName(const int& RunNumber)
{
    /**
     * Set run number.
     */
    outNameRun = std::to_string(runNumber);
    std::string out;
    for (int i = 0; i < 6 - std::to_string(runNumber).length(); i++)
    {
        out += "0";
    }
    out += std::to_string(runNumber);

    return out;
}

void Output::setFileSeq(const std::string& seq)
{
    /**
     * Set file sequence number.
     *
     * Adds zero padding to 6 digits in accordance with LZ-ICD-08-0007.
     *
     * Method gives an ERROR if seq does not consist of only digits.
     *
     * \return void.
     */
    if (!std::all_of(seq.begin(), seq.end(), ::isdigit))
    {
        std::cout << "ERROR: You provided a non-numerical fileSeqNum parameter." << std::endl;
        std::cout << "DERCONFIG states fileSeqNum must be an interger" << std::endl;
        std::cout << "DER will continue with the fileSeqNum parameter you provided, "
                  << "but it is likely to lead to undefined behaviour or a crash." << std::endl;
        std::cout << "Continue at your own risk." << std::endl;
    }

    int iseq = std::stoi(seq);
    if (iseq > 999999)
    {
        std::cout << "ERROR: Cannot have more events in file." << std::endl;
        std::cout << "LZ-ICD-08-0007 does not permit >999999 files in ";
        std::cout << "acquisition sequence." << std::endl;

        std::cout << "DER will still increment file sequence number. ";
        std::cout << "Continue at your own risk" << std::endl;
    }
    fileSequence = std::to_string(iseq);
    unsigned long strlen = fileSequence.length();
    fileSequence = "";
    for (int i = 0; i < 6 - strlen; i++)
    {
        fileSequence += "0";
    }
    fileSequence += std::to_string(iseq);
}

void Output::setTriggerRunNumber(const unsigned int triggerRunID)
{
    triggerRunNumber = triggerRunID;
}

void Output::setTimeStamp(EBEvent CurrentEvent)
{
    /**
     * Set timestamp from current EBEvent object CurrentEvent
     */
    trgTimeStamp = CurrentEvent.getTriggerTimeStamp();
}

void Output::setSignalChainIdentifier(format::SignalChain SigChain)
{
    /**
     * Method to set signal chain identifier.
     */
    SigChainVersion = SigChain;
}

void Output::setChannelTriggerActiveS1(unsigned long long ts, int ch)
{
    /**
     * Set trigger timestamp active point.
     */
    if ((ChTriggerTSActiveS1[ch] != 0) && (ts > ChTriggerTSActiveS1[ch]))
        return;
    else
        ChTriggerTSActiveS1[ch] = ts;
}

void Output::setChannelTriggerActiveS2(unsigned long long ts, int ch)
{
    /**                                                                                                            
   * Set trigger timestamp active point.                                                                         
   */
    if ((ChTriggerTSActiveS2[ch] != 0) && (ts > ChTriggerTSActiveS2[ch]))
        return;
    else
        ChTriggerTSActiveS2[ch] = ts;
}

format::revision Output::getOutputFormat()
{
    return outputFormat;
}

void Output::doWriteStageData(PODContainer& stagePODs)
{
}

void Output::doWriteRawData(Pulse& theLGPulse, Pulse& theHGPulse, 
			    std::shared_ptr<PODContainer> theLGPODs, 
			    std::shared_ptr<PODContainer> theHGPODs)
{
}
