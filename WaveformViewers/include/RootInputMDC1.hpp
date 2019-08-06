//
//  RootInputMDC1.hpp
//  DER
//
//  Created by Cees Carels on 05/06/2017.
//  Copyright © 2017 Carels. All rights reserved.
//

#ifndef RootInputMDC1_hpp
#define RootInputMDC1_hpp

#include "RootInput.hpp"
#include <stdio.h>
#include <vector>

#define BACC_LIB_VERSION __BACC_LIB_REV

#if (BACC_LIB_VERSION == 3)
#include "BaccMCTruthEvent.hh"
static_assert(BACCMCTRUTHEVENT_VERSION == 5,
    "Incorrect BaccMCTruthEvent version, "
    "you have not supplied the correct version of "
    "BaccMCTruthEvent.hh that is expected for "
    "the configuration you are trying to compile. "
    "Please review the interface revisions to "
    "BaccMCTruthEvent.hh for the current BACCARAT release.");
#endif

/**
 * RootInput implementation that is compatible with BaccRootConverter input
 * files as of 05/06/2017.
 */

class RootInputMDC1 : public RootInput
{
public:
    RootInputMDC1();
    ~RootInputMDC1();

    int makePMTDataReady();
    bool getPMTData(const unsigned long long evt);
    std::string getUserName();

protected:
#if (BACC_LIB_VERSION == 3)
    TTree* fMCTruthEvent; //Follow convention from this format
    //Stacked structure for Jun2017 Root input file.
    std::vector<PhotonMCTruth>* pmthits;
    BaccMCTruthEvent* BaccObj;
#endif
};

#endif /* RootInputMDC1_hpp */
