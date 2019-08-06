//
//  RootInputMDC2.hpp
//  ReadPMTStream
//
//  Created by Theresa Fruth on 01/03/2018.
//  Copyright © 2018 LZOxford. All rights reserved.
//

#ifndef RootInputMDC2_hpp
#define RootInputMDC2_hpp

#include <stdio.h>
#include <ctime>
#include <random>
#include <chrono>

#include "RootInput.hpp"
#include "PMT.hpp"

#define BACC_LIB_VERSION __BACC_LIB_REV

#if (BACC_LIB_VERSION == 6)
#include "BaccMCTruthEvent.hh"
static_assert(BACCMCTRUTHEVENT_VERSION == 8,
    "Incorrect BaccMCTruthEvent version, "
    "you have not supplied the correct version of "
    "BaccMCTruthEvent.hh that is expected for "
    "the configuration you are trying to compile. "
    "Please review the interface revisions to "
    "BaccMCTruthEvent.hh for the current BACCARAT release.");
#endif

/**
 * RootInput implementation that is compatible with BaccRootConverter input
 * files as of 08/02/2018.
 *
 */

class RootInputMDC2 : public RootInput
{
public:
    RootInputMDC2();
    ~RootInputMDC2();
    bool Open();
    void Close();
    int makePMTDataReady();
    int makePMTDataReady(const unsigned long long& Evt,
        unsigned long long& NPhotons,
        unsigned long long& TMin,
        unsigned long long& TMax,
        unsigned long long& NVertices,
        std::vector<int>& PmtsInEvt);
    bool getPMTData(const unsigned long long evt);
    bool getPMTData(const unsigned long long evt,
        int pmt,
        std::shared_ptr<PMT> thePMT,
	unsigned long eventLength,
        unsigned long long timeShift,
        unsigned long long TimeShiftInce,
        unsigned long long k);
    std::string getUserName();
    void getBaccObj();
    double getEventFirstPhotonTime(unsigned long long evt);
#if (BACC_LIB_VERSION == 6)
    void getBaccObj(BaccMCTruthEvent**& BaccObj);
#endif
protected:
    struct PMTStreamInfo
    {
        int PMTnumber;
        int StartIdx;
        int NumberOfPhotons;
        int NumberOfDarkCounts;
    };

    std::vector<std::vector<PMTStreamInfo>> EventAndPMTInfos;
    std::vector<int> nDataEntries;

    int         determineDarkCounts(const unsigned long length, 
                                    double darkCountRate);

    unsigned long long fPreEventWindow;
    unsigned long long fPostEventWindow;

#if (BACC_LIB_VERSION == 6)
    PhotonMCTruth* pmthits;
    BaccMCTruthEvent* BaccObj;

#endif
};

#endif /* RootInputMDC2_hpp */
