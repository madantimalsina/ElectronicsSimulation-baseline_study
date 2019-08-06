//
//  RootOutput.cpp
//  devices
//
//  Created by Cees Carels on 22/06/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "RootOutput.hpp"

RootOutput::RootOutput()
{
    /**
     * Constructor for RootOutput.
     */
}

RootOutput::~RootOutput()
{
    /**
     * Destructor for RootOutput.
     */
}

void RootOutput::doWriteDERSummary(DERSummary& theDERSummary)
{
    /**
     * Method that gets the DERSummary for the current DER simulation,
     * and writes its information to disk.
     */
    Uname = theDERSummary.getUserName().c_str();
    DERUserName = theDERSummary.getDERUserName().c_str();
    DERUserGroupID = theDERSummary.getDERUserGroupID().c_str();
    DERHostName = theDERSummary.getDERHostName().c_str();
    SimFileName = theDERSummary.getFileName().c_str();
    DERFileName = theDERSummary.getDERFileName().c_str();
    SimFileRandomSeed = theDERSummary.getFileRandomSeed().c_str();
    DERFileRandomSeed = theDERSummary.getDERRandomSeed().c_str();
    PosixTime = theDERSummary.getPosixTime().c_str();
    LocalTime = theDERSummary.getLocalTime().c_str();
    SamplingRate_ns = theDERSummary.getSamplingRate_ns();
    DERCONFIG = theDERSummary.getDERCONFIG();
    DERRunID = theDERSummary.getDERRunID();
    GitCommitHash = theDERSummary.getGitCommitHash();

    fDERSummary->Fill();
    fDERSummary->Write();
}

void RootOutput::doWriteGlobal(EBGlobal* theGlobalSummary)
{
    /**
     * Method that gets the EBGlobal object for the current DER simulation,
     * and writes its information to disk.
     */

    //The DER allows for unsigned long, but is type-cast to accommodate
    //the smaller unsigned int.
    if (theGlobalSummary != NULL)
    {
        runNumber = (unsigned int)(theGlobalSummary->getRunNumber());
        formatVersionID = (unsigned short)(theGlobalSummary->getFormatVersion());
        runStartTime = theGlobalSummary->getRunStartTime();
        fGlobal->Fill();
        fGlobal->Write();
    }
}

void RootOutput::doWriteEvent(EBEvent& theEBEvent)
{
    /**
     * Method that gets the EBEvent object for the event that is currently
     * being simulated, and writes its information to disk.
     */

    for (int i = 0; i < theEBEvent.getnPods().size(); i++)
    {
        nPods[i] = theEBEvent.getnPods().at(i); //nPods per DC
    }

    for (int i = 0; i < theEBEvent.getBufferLiveStartTS().size(); i++)
    {
        bufferStart[i] = theEBEvent.getBufferLiveStartTS()[i];
        bufferStop[i] = theEBEvent.getBufferLiveStopTS()[i];
    }

    trgType = theEBEvent.getTriggerType();
    trgTimeStamp = theEBEvent.getTriggerTimeStamp();
    trgMultiplicity = theEBEvent.getTriggerMultiplicity();
    sumPODdata = theEBEvent.getSumPODData();

    fEvent->Fill();

    //Trigger truth
    fTriggerTruth->Fill();
}

void RootOutput::doWriteSummary(EBSummary& theEBSummary)
{
    /**
     * Method that gets the EBSummary for the current DER simulation, and writes
     * its information to disk.
     */
    endFlag = theEBSummary.getEndFlag();
    nEvtsFile = theEBSummary.getNEvtsFile();
    fSummary->Fill();
    fSummary->Write();
}

void RootOutput::doPrepareEvent()
{
    /**
     * The class interface requires this method for all derived classes, but it
     * is not used for the .root output file.
     */
}

void RootOutput::doResolveEvtPtrs()
{
    /**
     * The class interface requires this method for all derived classes, but it
     * is not used for the .root output file.
     */
}

double RootOutput::StatFileSize()
{
    /**
     * Return the current size (in bytes) of the DER output file.
     */
    return fFile->GetSize();
}

void RootOutput::doPreparePulseMCTruth(unsigned long long NVertices,
    unsigned long long TMax)
{
    /**
     * Method required by interface and intentionally left blank
     */
}

double RootOutput::DataRate(TStopwatch& Timer)
{
    /**
     * Calculate the current data rate by taking the ratio of the change in file
     * size divided by the change in time since the last time it was checked.
     *
     * Then, convert to kB/s
     *
     * Both the data rate [kB/s] and the time at which it was calculated, are
     * then written to disk.
     */
    /*Get data rate in [kB/s] based on time*/
    Long64_t newSize = StatFileSize();
    Long64_t SizeDiff = newSize - prevSize;
    Timer.Stop();
    double CurrentTime = Timer.CpuTime();
    Timer.Continue();
    if (CurrentTime == 0)
        dataRate = 0.0;
    else if (CurrentTime - prevTime == 0)
        dataRate = 0.0;
    else
        dataRate = SizeDiff / (CurrentTime - prevTime);
    prevTime = CurrentTime;
    prevSize = newSize;
    fDataRate->Fill();
    return dataRate / 1000.0;
}

void RootOutput::doWriteTime(const double Real, const double CPU)
{
    /**
     * Set the real and cpu time taken to run the entire DER
     * simulation. Then write these numbers to disk.
     */
    tReal = Real;
    tCPU = CPU;
    fDERTime->Fill();
    fDERTime->Write();
}
