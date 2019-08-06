//
//  RootInput.hpp
//  devices
//
//  Created by Cees Carels on 24/10/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef RootInput_hpp
#define RootInput_hpp

#include <stdio.h>

#include "TBranch.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeIndex.h"

#include "Input.hpp"

/**
 * Implementation of base class for Root Files.
 *
 * Each version of root file input should be sub-classed from the
 * RootInput class.
 */

class RootInput : public Input
{
public:
    RootInput();
    virtual ~RootInput() = 0;
    virtual bool Open();
    virtual void Close();
    TFile* getFilePointer();

    virtual int makePMTDataReady(const unsigned long long& Evt,
        unsigned long long& NPhotons,
        unsigned long long& TMin,
        unsigned long long& TMax,
        unsigned long long& NVertices,
        std::vector<int>& PmtsInEvt);

    virtual  bool getPMTData(const unsigned long long evt,
        int pmt,
        std::shared_ptr<PMT> thePMT,
	unsigned long eventLength,
        unsigned long long timeShift,
        unsigned long long TimeShiftInce,
	unsigned long long k);

protected:
    TFile* inFile;
    //Index where next event starts
    std::vector<unsigned long long> EvtStartIdx;
};

#endif /* RootInput_hpp */
