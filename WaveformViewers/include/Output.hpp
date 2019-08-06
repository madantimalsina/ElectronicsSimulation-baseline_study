//
//  Output.hpp
//  devices
//
//  Created by Cees Carels on 07/02/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef Output_hpp
#define Output_hpp

#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>

#include "TFile.h"
#include "TLeaf.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "TTree.h"

#include "DERSummary.hpp"
#include "Device.hpp"
#include "EBEvent.hpp"
#include "EBGlobal.hpp"
#include "EBSummary.hpp"
#include "MCTruth.hpp"
#include "PODContainer.hpp"
#include "PulseManager.hpp"
#include "RootInput.hpp"

#include "InputOutputFormats.hpp"

/**
 * Base class providing the interface for and several implementations of methods
 * used to produce the output format.
 *
 * The output produced by this class and its derived classes is identical to the
 * Event Builder format and the DAQ format.
 */

class Output
{
public:
    Output();
    virtual ~Output() = 0;

    virtual void doStoreSubsetLG(const Pulse& thePulse = Pulse());
    virtual void doStoreSubsetHG(const Pulse& thePulse = Pulse());
    virtual void doInputTreeClone(TFile* pointerToFile) = 0;
    virtual void doInitInputVariables(Input* input);
    virtual void doWriteDERSummary(DERSummary& theDERSummary) = 0;
    virtual void doWriteDERMCTruth(std::shared_ptr<MCTruth> theTruth) = 0;
    virtual void doWriteGlobal(EBGlobal* theGlobalSummary = NULL) = 0;
    virtual void doWriteEvent(EBEvent& theEBEvent) = 0;
    virtual void doWriteData(POD& theEBDataPOD, const int& DCID) = 0;
    virtual void doWriteSummary(EBSummary& theEBSummary) = 0;
    virtual void doPrepareEvent() = 0;
    virtual void doWriteTruthTree() = 0;
    virtual void doWriteDetectorMCTruthEvent() = 0;
    virtual void doPreparePulseMCTruth(unsigned long long NVertices,
        unsigned long long TMax)
        = 0;
    void IncEvtNum();
    UInt_t EvtNum();

    void setRunNumber(const unsigned int& RunNumber);
    virtual void createOutFile(const bool NewSeries,
        const std::string& outdir = "")
        = 0;
    void setSimSource(const std::vector<std::string>& SimSourceFileName);
    void setNumDCs(const unsigned int& DCs,
        const unsigned int& ChanPerDDC32);
    virtual void doResolveEvtPtrs() = 0;
    virtual void CloseFile() = 0;
    virtual std::vector<Pulse> getFullChannelPulseLG();
    virtual std::vector<Pulse> getFullChannelPulseHG();
    virtual std::string getOutFileName();
    void setTime(const time_t& CurrentTime);
    virtual unsigned long long getPosixTime();
    virtual double DataRate(TStopwatch& Timer) = 0;
    void setFileSeq(const std::string& seq);
    virtual void doWriteTime(const double Real, const double CPU) = 0;
    void setTriggerRunNumber(const unsigned int);
    void setTimeStamp(EBEvent CurrentEvent);
    void setSignalChainIdentifier(format::SignalChain SigChain);
    void setChannelTriggerActiveS1(unsigned long long ts, int ch);
    void setChannelTriggerActiveS2(unsigned long long ts, int ch);

    format::revision getOutputFormat();
    virtual void doWriteStageData(PODContainer& stagePODs);
    virtual void doWriteRawData(Pulse& theLGPulse, Pulse& theHGPulse, 
				std::shared_ptr<PODContainer> theLGPODs,
				std::shared_ptr<PODContainer> theHGPODs);

    static unsigned short getNPODsEvt()
    {
        return nPODInEvt;
    }

protected:
    format::revision outputFormat;
    format::LZ_ICD_08_0007 LZ_Event_Format_Revision;
    format::SignalChain SigChainVersion;
    unsigned long nextHG;
    unsigned long nextLG;

    std::string setRunName(const int& RunNumber);

    std::string yyyy = "yyyy";
    std::string mm = "mm";
    std::string dd = "dd";
    std::string HH = "HH";
    std::string MM = "MM";
    std::string ss = "ss";

    std::string outDir = "~/ElecSim/";
    std::string outNameBase = "";
    std::string outNameExp = "lz";
    std::string outNameRun = "rrrrrr";
    std::string outSequence = "ssssss";
    std::string fileSequence = "000000"; //File sequence number
    std::string outRaw = "raw";
    std::string outSeparator = "_";
    std::string outputFull; //Assembled in method after variables set

    std::string SimInputFileName;

    //Time
    double tReal;
    double tCPU;

    //Variables for tracking data rate
    Long64_t prevSize = 0.0;
    Double_t prevTime = 0.0;
    Double_t dataRate = 0.0;

    //Event Builder Output Variables
    unsigned int numDCs = 0;
    unsigned int ChPerDDC32 = 0;

    //DERSummary
    std::string Uname; //Machine where sim input was produced
    std::string DERUserName; //User of DER output
    std::string DERUserGroupID; //User group of DER output
    std::string DERHostName; //Host where DER output was produced
    std::string SimFileName; //Name of input sim file
    std::string DERFileName = ""; //Name of output DER file
    std::string SimFileRandomSeed; //Random seed of input sim file
    std::string DERFileRandomSeed; //Random seed of DER sim file
    std::string PosixTime; //Posix time of DER production
    std::string LocalTime; //Local time of DER production
    unsigned int SamplingRate_ns; //[ns] Analog sampling rate
    std::string DERCONFIG;
    std::string DERRunID;
    std::string GitCommitHash; //Git commit hash

    //DERSubsets
    std::vector<Pulse> OutputVectorLG; //LG
    std::vector<Pulse> OutputVectorHG; //HG

    //DER MC Truth
    std::vector<double> OutputVectorTLG;
    std::vector<double> OutputVectorRLG;
    std::vector<double> OutputVectorTHG;
    std::vector<double> OutputVectorRHG;

    MCTruth theDERMCTruthInfo;
    unsigned long long SimEvt; //LUXSim/BACCARAT event number
    unsigned long nPODInFile;
    static unsigned short nPODInEvt;

    //PODTruth
    UShort_t DERevt;
    UShort_t DERchannel;
    std::vector<unsigned long long> MCTruthIdx; //Idx map of POD to MCTruthData.

    //Global
    UInt_t runNumber;
    UShort_t formatVersionID;
    uint64_t runStartTime;

    //Event
    uint32_t globalEvt; //Event in sequence (eventSeqNum)
    UShort_t nPods[14]; //[14] Means nPods per DC
    uint64_t bufferStart[46]; //[46] Digitiser live start
    uint64_t bufferStop[46]; //[46] Digitiser live stop
    UShort_t trgType;
    uint64_t trgTimeStamp;
    uint16_t trgMultiplicity;
    uint64_t sumPODstartTS;
    uint16_t sumPODlength;
    std::vector<Int_t> sumPODdata; //[up to 9000] Size from EB
    uint32_t triggerRunNumber; //Trigger run number

    //Data
    Int_t LUXSimRunNumber;
    UShort_t evt;
    UShort_t channel;
    UShort_t hit;
    ULong64_t startTime;
    UShort_t nSamples;
    std::vector<Short_t> zData;

    //Stage Data
    std::vector<Short_t> stage1ZData;
    std::vector<Short_t> stage2ZData;
    std::vector<Short_t> stage3ZData;
    std::vector<Short_t> stage4ZData;
    std::vector<Short_t> stage5ZData;

    //Raw Data
    UShort_t pmt;
    std::vector<Double_t> rawHGData;
    std::vector<Double_t> rawLGData;
    std::vector<Int_t> photonHGStarts;
    std::vector<Int_t> photonHGEnds;
    std::vector<Int_t> photonLGStarts;
    std::vector<Int_t> photonLGEnds;
    std::vector<Int_t> podHGStarts;
    std::vector<Int_t> podHGEnds;
    std::vector<Int_t> podLGStarts;
    std::vector<Int_t> podLGEnds;

    //Summary
    UShort_t endFlag;
    UShort_t nEvtsFile; //DER can accommodate unsigned long long
    //nEvtsFile refers to the total number of events in the
    //simulation input file.
    //The number of events in a DC binary output file is nEvts.
    //nEvts corresponds to "numEvents" in LZ-ICD-08-0007.

    //Trigger timestamps
    std::vector<unsigned long> ChTriggerTSActiveS1;
    std::vector<unsigned long> ChTriggerTSActiveS2;
};

#endif /* Output_hpp */
