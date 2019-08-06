//
//  RootOutputMDC2.hpp
//  ReadPMTStream
//
//  Created by Theresa Fruth on 01/03/2018.
//  Copyright © 2018 LZOxford. All rights reserved.
//

#ifndef RootOutputMDC2_hpp
#define RootOutputMDC2_hpp

#include <stdio.h>

#include "RootOutput.hpp"
#define BACC_LIB_VERSION __BACC_LIB_REV

#if (BACC_LIB_VERSION == 6)
#include "BaccMCTruthEvent.hh"
#include "DetectorMCTruthEvent.hpp"
#include "RootInputMDC2.hpp"
#include "TChain.h"
#include "TObject.h"
#endif

/**
 * RootOutput implementation that is compatible with
 * BACCARAT/BaccMCTruth files as of 20/01/2018.
 *
 * Changes are internal to DER only.
 */

class RootOutputMDC2 : public RootOutput
{
public:
    friend class Input;
    RootOutputMDC2();
    ~RootOutputMDC2();

    void doInputTreeClone(TFile* pointerToFile);
    void doWriteTruthTree();
    void createOutFile(const bool NewSeries,
        const std::string& outdir = ""); //!< Create output file.
    void doInitInputVariables(Input* input);
    void doWriteGlobal(EBGlobal* theGlobalSummary); //!< Write global.
    void doWriteEvent(EBEvent& theEBEvent); //!< Write event.
    void doWriteData(POD& theEBDataPOD,
        const int& DCID); //!< Write data.
    void CloseFile(); //!< Close output file.
    void doPreparePulseMCTruth(
        unsigned long long NVertices,
        unsigned long long TMax); //!< Prepare structure to hold Pulse truth.
    void doWriteDERMCTruth(std::shared_ptr<MCTruth> theTruth); //!< Truth data.
    void doWriteDetectorMCTruthEvent(); //!< Write DetectorMCTruthEvent.

    void doWriteStageData(PODContainer& stagePODs);
    void doWriteRawData(Pulse& theLGPulse, Pulse& theHGPulse, 
		        std::shared_ptr<PODContainer> theLGPODs, 
			std::shared_ptr<PODContainer> theHGPODs);

protected:
#if (BACC_LIB_VERSION == 6)
    TChain* data;
    BaccMCTruthEvent** BaccObj;
    VertexMCTruth BaccVertex;
    DetectorMCTruthEvent* TruthObj;
#endif

    struct PulseInfo
    {
        unsigned int SimEvt;
        unsigned short PulseType;
        unsigned int PheCount;
        int BaccVertexNumber;
        unsigned short EvtNumber;
        double FirstPheTime;
        double LastPheTime;
        int Coincidence;
        std::vector<unsigned int> PmtsHit;
    };

    struct DarkCountInfo
    {
        unsigned short pmt;
        double time;
    };

    struct VertexInfo
    {
        unsigned int S1Idx;
        unsigned int S2Idx;
        unsigned int ScintillationIdx;
        std::vector<unsigned int> OtherIdx;
        unsigned int DetectedS1Photons;
        unsigned int DetectedS2Photons;
        unsigned int DetectedScintPhotons;
    };

    static bool comp(short int lhs, short int rhs);

    std::vector<PulseInfo> AllPulses;
    std::vector<PulseInfo> AftPulses;
    std::vector<DarkCountInfo> darkpulses;
    std::vector<unsigned int> VertexCount;

    unsigned long DERevt;
    unsigned short int DERRunIDIdx;
    unsigned char Origin;
    int NInteractionID; //!< Number of different interaction IDs possible.
    unsigned long long NVertices;
    unsigned long long TMaxEvt;

    uint16_t runType; //!< Run type
    uint32_t firstEvtData; //!< First event data
    uint32_t globalEvt;
    //!< Event number which uses the new convention to start at 0.
};

#endif /* RootOutputMDC2_hpp */
