//
//  RootOutputMDC1.hpp
//  DER
//
//  Created by Cees Carels on 05/06/2017.
//  Copyright © 2017 Carels. All rights reserved.
//

#ifndef RootOutputMDC1_hpp
#define RootOutputMDC1_hpp

#include <stdio.h>

#include "RootOutput.hpp"

#define BACC_LIB_VERSION __BACC_LIB_REV

#if (BACC_LIB_VERSION == 3)
#include "BaccMCTruthEvent.hh"
#include "DetectorMCTruthEvent.hpp"
#include "DetectorMCTruthEvent_dict.h"
#include "TObject.h"
#endif

/**
 * RootOutput implementation that is compatible with
 * BACCARAT/BaccMCTruth files as of 05/06/2017.
 */

class RootOutputMDC1 : public RootOutput
{
public:
    RootOutputMDC1();
    ~RootOutputMDC1();

    void doInputTreeClone(TFile* pointerToFile);
    void doWriteTruthTree();
    void createOutFile(const bool NewSeries,
        const std::string& outdir = ""); //!< Create output file.
    void doWriteData(POD& theEBDataPOD,
        const int& DCID); //!< Write data.
    void CloseFile(); //!< Close output file
    void doWriteDERMCTruth(std::shared_ptr<MCTruth> theTruth); //!< Truth data.
    void doWriteDetectorMCTruthEvent(); //!< Write DetectorMCTruthEvent.

protected:
#if (BACC_LIB_VERSION == 3)
    TTree* fMCTruthTree; //MCTruthTree from BACCARAT MC Truth file
    BaccMCTruthEvent* BaccObj;
    VertexMCTruth BaccVertex;

    DetectorMCTruthEvent* TruthObj;
#endif

    struct PhotonInfo
    {
        double DERArrivalTime;
        unsigned short HGPODIdx;
        unsigned short LGPODIdx;
        short int BaccVertexIdx;
        unsigned char Origin;
    };

    unsigned short int DERRunIDIdx;

    std::vector<PhotonInfo> AllPhotons;
    unsigned long DERevt;
    unsigned char Origin;
    unsigned long long SimEvt;
    unsigned int RunNum;
};

#endif /* RootOutputMDC1_hpp */
