//
//  RootInputMDC1.cpp
//  DER
//
//  Created by Cees Carels on 05/06/2017.
//  Copyright © 2017 Carels. All rights reserved.
//

#include "RootInputMDC1.hpp"

#ifndef BACC_LIB_VERSION
#define BACC_LIB_VERSION __BACC_LIB_REV
#endif

#if (BACC_LIB_VERSION == 3)
RootInputMDC1::RootInputMDC1()
    : BaccObj(0)
    , pmthits(new std::vector<PhotonMCTruth>)
{
    /**
     * Constructor for RootInputMDC1.
     */
    LZ_PMT_Numbering_Revision = format::LZ_ICD_08_0008::Rev_Ap1;
}
#else
RootInputMDC1::RootInputMDC1()
{
    /**
     * Constructor for RootInputMDC1.
     */
    LZ_PMT_Numbering_Revision = format::LZ_ICD_08_0008::Rev_Ap1;
}
#endif

RootInputMDC1::~RootInputMDC1()
{
    /**
     * Destructor for RootInputMDC1.
     */
#if (BACC_LIB_VERSION == 3)
    delete pmthits;
    delete BaccObj;
#endif
}

int RootInputMDC1::makePMTDataReady()
{
/**
     * Returns -1 if error, 0 if ok.
     */
#if (BACC_LIB_VERSION == 3)
    fMCTruthEvent = (TTree*)inFile->Get("MCTruthTree");

    if (fMCTruthEvent == 0)
    {
        std::cout << "Could not find MCTruthTree, "
                  << "used for RootvJun2017 format."
                  << std::endl;
        return -1;
    }

    fMCTruthEvent->SetBranchAddress("MCTruthEvent", &BaccObj);
    std::cout << "Preparing input file..." << std::endl;
    unsigned long long prevEvtIdx = 0;
    EvtStartIdx.push_back(0);

    fMCTruthEvent->GetEntry(0);
    unsigned long long prevRunNumber = BaccObj->iRunNumber;
    for (int i = 0; i < fMCTruthEvent->GetEntries(); i++)
    {
        //One entry per event therefore we do not need to check the BaccEvent and RunNumber
        fMCTruthEvent->GetEntry(i);
        EvtStartIdx.push_back(i);

        //Checks for differing run numbers for the same event number
        //are not necessary in this version, so the condition that is
        //checked in preceding versions (e.g. RootInputJan2017) is not
        //present here.

        //Try to reduce these if statements, or use Root methods
        //Add new available PMTs
        //Can turn this into a set.

        //Create list of empty events
        if (!BaccObj->photons.size()
            || ((BaccObj->fEventLastPhotonTime_ns) - (BaccObj->fEventFirstPhotonTime_ns)) >= PhotonTimeLimit)
        {
            EmptyEvents.push_back(i);
        }

        for (int k = 0; k < BaccObj->photons.size(); k++)
        {
            if (AvailablePMTs.size() == 0)
            {
                AvailablePMTs.push_back(BaccObj->photons[k].iPMTIndex);
            }
            if (std::find(AvailablePMTs.begin(),
                    AvailablePMTs.end(),
                    BaccObj->photons[k].iPMTIndex)
                != AvailablePMTs.end())
            {
            }
            else
            {
                AvailablePMTs.push_back(BaccObj->photons[k].iPMTIndex);
            }
        }

        //Get all availble Events.
        //Removed check for whether event is a new one from previous implementations
        //see for example RootInputOct2016, as the new input structure guarantees
        //that every entry  is a new event. This also allows us to use the index
        //as event identifier.
        AvailableEvents.push_back(i);
    }
    EvtStartIdx.push_back(fMCTruthEvent->GetEntries());

    SortData();
    makeEvtListFromSelection();
#endif
    return 0;
}

bool RootInputMDC1::getPMTData(const unsigned long long evt)
{
    /**
     * Retrieve the PMT data for the given event.
     * Can replace this with a buffer with iterator method
     * In this implementation of the method, the EventID contains the index of the 
     * BACCARAT event in the input file instead of the BaccMCTruthEvent iEventNumber.
     * These two numbers do not necessarily coincide. 
     * In all previous implementations EventID corresponds to iEventNumber.
     */
#if (BACC_LIB_VERSION == 3)
    fMCTruthEvent->SetBranchAddress("MCTruthEvent", &BaccObj);
    fMCTruthEvent->GetEntry(evt);
    std::vector<PMTData>().swap(PmtData);
    PmtData.reserve(BaccObj->photons.size());
    for (int j = 0; j < BaccObj->photons.size(); j++)
    {
        if (BaccObj->photons[j].fTime_ns - BaccObj->fEventFirstPhotonTime_ns > PostTriggerWindow)
            continue;
        PMTData newPmtData;
        newPmtData.id = BaccObj->photons[j].iPMTIndex;
        newPmtData.time = BaccObj->photons[j].fTime_ns - BaccObj->fEventFirstPhotonTime_ns;
        newPmtData.wavelength = BaccObj->photons[j].fWavelength_nm;
        newPmtData.EventID = evt; //to get actual BACCARAT event numer: BaccObj->iEventNumber;
        newPmtData.RunNumber = BaccObj->iRunNumber;
        newPmtData.InteractionIdentifier = BaccObj->photons[j].iInteractionIdentifier;
        newPmtData.VertexNumber = BaccObj->photons[j].iVertexNumber;
        PmtData.push_back(newPmtData);
    }
#endif
    return true;
}

std::string RootInputMDC1::getUserName()
{
/**
     * Get the username from the HeaderTree as provided by the
     * input file.
     */
#if (BACC_LIB_VERSION == 3)
    TTree* fHeaderTree = (TTree*)inFile->Get("HeaderTree");

    if (fHeaderTree == 0)
    {
        std::cout << "Error: Could not find HeaderTree in input file. "
                  << std::endl;
        std::cout << "Continue at your own risk." << std::endl;
        return "Error: Undefined";
    }

    std::string* suName = 0;
    fHeaderTree->SetBranchAddress("sUname", &suName);
    fHeaderTree->GetEntry(0);
    return *suName;
#endif
    return "";
}
