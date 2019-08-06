//
//  RootInputMDC2.cpp
//  ReadPMTStream
//
//  Created by Theresa Fruth on 01/03/2018.
//  Copyright © 2018 LZOxford. All rights reserved.
//

#include "RootInputMDC2.hpp"
//#include "PMT.hpp"

#ifndef BACC_LIB_VERSION
#define BACC_LIB_VERSION __BACC_LIB_REV
#endif

#if (BACC_LIB_VERSION == 6)
RootInputMDC2::RootInputMDC2()
    : BaccObj(0)
    , pmthits(0)
{
    /**
     * Constructor for RootInputMDC2.
     */
    LZ_PMT_Numbering_Revision = format::LZ_ICD_08_0008::Rev_Ap1;
    inFile = NULL;
}
#else
RootInputMDC2::RootInputMDC2()
{
    /**
     * Constructor for RootInputMDC2.
     */
    LZ_PMT_Numbering_Revision = format::LZ_ICD_08_0008::Rev_Ap1;
    inFile = NULL;
}
#endif

RootInputMDC2::~RootInputMDC2()
{
    /**
     * Destructor for RootInputMDC2.
     */
#if (BACC_LIB_VERSION == 6)
    delete pmthits;
    delete data;
#endif
}

bool RootInputMDC2::Open()
{
    /**
     * Open the root file given by filePath.
     */

    data = new TChain("MCTruthTree");

    for (int i = 0; i < filePath.size(); i++)
    {
        TFile* file = TFile::Open(filePath[i].c_str());
        if (!file)
        {
            std::cout << "ERROR: Cannot find file " << filePath[i].c_str() << std::endl;
            delete file;
            return false;
        }
        if (file->IsZombie())
        {
            std::cout << "ERROT: Cannot open file " << filePath[i].c_str() << std::endl;
            delete file;
            return false;
        }
        else
        {
            delete file;
            data->Add(filePath[i].c_str());
        }
    }
    return true;
}

void RootInputMDC2::Close()
{
    /**
     * Close the root file. No need to close when using TChain.
     */
}

int RootInputMDC2::makePMTDataReady()
{
    /**
     * Returns -1 if error, 0 if ok.
     */
#if (BACC_LIB_VERSION == 6)
    std::cout << "Preparing input file..." << std::endl;
    long long N = data->GetEntries();
    data->SetBranchAddress("MCTruthEvent", &BaccObj);
    std::vector<int> startIdx(BaccObj->iPMTHits.size(), 0);
    nDataEntries.resize(N);
    EventAndPMTInfos.resize(N);
    AllEventFirstPhotonTimes_ns.resize(N);
    fPreEventWindow = std::stoull(global::config->getConfig("PreEventWindow"));
    fPostEventWindow = std::stoull(global::config->getConfig("PostEventWindow"));

    for (int i = 0; i < N; i++)
    {
        data->GetEvent(i);
        int nPMTs = 0;
        AllEventFirstPhotonTimes_ns[i] = (BaccObj->fEventFirstPhotonTime_ns + BaccObj->fParentTime_ns - fPreEventWindow);
        unsigned long long Length = (unsigned long long) BaccObj->fEventLastPhotonTime_ns - (unsigned long long) BaccObj->fEventFirstPhotonTime_ns 
                                    + fPreEventWindow + fPostEventWindow;
        if(Length > (unsigned long long)PostTriggerWindow) Length = (unsigned long long)PostTriggerWindow;
        for (int j = 0; j < BaccObj->iPMTHits.size(); j++)
        {
            int darkCountRate = PMT::getParamPointer()->getConfig("DarkCount",    j);
            int DarkCounts    = determineDarkCounts(Length, darkCountRate);
            if(BaccObj->iPMTHits[j]!=0 || DarkCounts != 0) 
            {
                ++nPMTs;
                PMTStreamInfo info;
                info.PMTnumber = j;
                info.StartIdx = (int)startIdx[j];
                info.NumberOfPhotons = (int)BaccObj->iPMTHits[j];
                info.NumberOfDarkCounts = DarkCounts; 
                EventAndPMTInfos[i].push_back(info);
                startIdx[j] += (BaccObj->iPMTHits[j]);
                nDataEntries[i] += (BaccObj->iPMTHits[j]);
                if (AvailablePMTs.size() == 0)
                {
                    AvailablePMTs.push_back(j);
                }
                if (std::find(AvailablePMTs.begin(),
                        AvailablePMTs.end(),
                        j)
                    != AvailablePMTs.end())
                {
                }
                else
                {
                    AvailablePMTs.push_back(j);
                }
            }
        }

        if (nPMTs == 0
            || ((BaccObj->fEventLastPhotonTime_ns) - (BaccObj->fEventFirstPhotonTime_ns)) >= PhotonTimeLimit)
        {
            EmptyEvents.push_back(i);
        }
        AvailableEvents.push_back(i);
    }
    SortData();
    makeEvtListFromSelection();

#endif
    return 0;
}

int RootInputMDC2::makePMTDataReady(const unsigned long long& Evt,
    unsigned long long& NPhotons,
    unsigned long long& TMin,
    unsigned long long& TMax,
    unsigned long long& NVertices,
    std::vector<int>& PmtsInEvt)
{
    /**
     * Method sets number of photons, first and last photon time for the event.
     * Returns -1 if error, 0 if ok.
     */
#if (BACC_LIB_VERSION == 6)
    data->GetEntry(Evt);
    NPhotons = nDataEntries[Evt];
    TMin = (unsigned long long)BaccObj->fEventFirstPhotonTime_ns - fPreEventWindow; //variable coming in from BACC is double
    TMax = (unsigned long long)BaccObj->fEventLastPhotonTime_ns + fPostEventWindow; //variable coming in from BACC is double
    TMax -= TMin;
    // if (TMax > (unsigned long long)PostTriggerWindow)
    //     TMax = (unsigned long long)PostTriggerWindow;
    NVertices = BaccObj->vertices.size();
    PmtsInEvt.resize(EventAndPMTInfos[Evt].size());
    for (int i = 0; i < EventAndPMTInfos[Evt].size(); i++)
    {
        PmtsInEvt[i] = (int)EventAndPMTInfos[Evt][i].PMTnumber;
    }

#endif
    return 0;
}

bool RootInputMDC2::getPMTData(const unsigned long long evt)
{
    /**
     * Retrieve the PMT data for the given event.
     * Can replace this with a buffer with iterator method
     * In this implementation of the method, the EventID contains the index of the
     * BACCARAT event in the input file instead of the BaccMCTruthEvent iEventNumber.
     * These two numbers do not necessarily coincide.
     * In all previous implementations EventID corresponds to iEventNumber.
     */

#if (BACC_LIB_VERSION == 6)
    //First get event level information: RunNumber and EventFirstPhotonTime_ns
    data->GetEntry(evt);
    int RunNumber = BaccObj->iRunNumber;
    double EventFirstPhotonTimeRelativeToParent_ns = (BaccObj->fEventFirstPhotonTime_ns - BaccObj->fParentTime_ns - fPreEventWindow);

    //Get the number of PMTs and data entries in the evet
    int NumberOfPMTs = EventAndPMTInfos[evt].size();
    std::vector<PMTData>().swap(PmtData);
    PmtData.reserve(nDataEntries[evt]);

    TString treename = "PMTStream";
    TString currenttreename;
    int pmtID = 0;

    for (int k = 0; k < NumberOfPMTs; k++)
    {
        //Find the TTree for the current pmt
        pmtID = EventAndPMTInfos[evt][k].PMTnumber;
        currenttreename = treename + std::to_string(pmtID);
        TChain* pmttree = new TChain(currenttreename);
        for (int i = 0; i < filePath.size(); i++)
        {
            pmttree->Add(filePath[i].c_str());
        }
        pmttree->SetBranchAddress("PhotonMCTruth", &pmthits);

        for (int i = EventAndPMTInfos[evt][k].StartIdx; i < (EventAndPMTInfos[evt][k].StartIdx + EventAndPMTInfos[evt][k].NumberOfPhotons); i++)
        {
            pmttree->GetEntry(i);
            if ((pmthits->fTime_ns - EventFirstPhotonTimeRelativeToParent_ns) > PostTriggerWindow)
                continue;
            PMTData newPmtData;
            newPmtData.id = pmtID;
            newPmtData.time = (double)pmthits->fTime_ns - EventFirstPhotonTimeRelativeToParent_ns;
            newPmtData.wavelength = (double)pmthits->fWavelength_nm;
            newPmtData.EventID = (int)evt; //to get actual BACCARAT event numer: BaccObj->iEventNumber;
            newPmtData.RunNumber = RunNumber;
            newPmtData.InteractionIdentifier = pmthits->iInteractionIdentifier;
            newPmtData.VertexNumber = pmthits->iVertexNumber;
            PmtData.push_back(newPmtData);
        }
        delete pmttree;
    }
#endif
    return true;
}

bool RootInputMDC2::getPMTData(const unsigned long long evt,
			       int idx,
			       std::shared_ptr<PMT> thePMT,
			       unsigned long eventLength,
			       unsigned long long timeShift,
			       unsigned long long TimeShiftInc,
			       unsigned long long k)
{
    /**
     * Retrieve the PMT data for a given Event and PMT.
     * Can replace this with a buffer with iterator method
     * In this implementation of the method, the EventID contains the index of the
     * BACCARAT event in the input file instead of the BaccMCTruthEvent iEventNumber.
     * These two numbers do not necessarily coincide.
     * In all previous implementations EventID corresponds to iEventNumber.
     */
#if (BACC_LIB_VERSION == 6)
    //First get event level information: RunNumber and EventFirstPhotonTime_ns
    int RunNumber = BaccObj->iRunNumber;
    double EventFirstPhotonTimeRelativeToReferenceTime_ns = (BaccObj->fEventFirstPhotonTime_ns - fPreEventWindow - BaccObj->fReferencePhotonTime_ns);
    int pmtNumber = EventAndPMTInfos[(int)evt][idx].PMTnumber;
    thePMT->setPMTNumber(pmtNumber);
    thePMT->resetPMTVectors();

    TString treeName = "PMTStream" + std::to_string(pmtNumber);
    TChain* pmtTree = new TChain(treeName);
    for (int i = 0; i < filePath.size(); i++)
    {
        pmtTree->Add(filePath[i].c_str());
    }
    pmtTree->SetBranchAddress("PhotonMCTruth", &pmthits);

    for (int i = EventAndPMTInfos[(int)evt][idx].StartIdx;
         i < (EventAndPMTInfos[(int)evt][idx].StartIdx + EventAndPMTInfos[(int)evt][idx].NumberOfPhotons);
         i++)
    {
        pmtTree->GetEntry(i);
	double photonTime = (double)pmthits->fTime_ns - EventFirstPhotonTimeRelativeToReferenceTime_ns;
        if (photonTime > PostTriggerWindow)
	  continue;
        if (photonTime < 0)
	  photonTime = 0;
	std::shared_ptr<PMT::TimesAndPheResp> thePhoton = std::make_shared<PMT::TimesAndPheResp>();
        thePhoton->wavelength = pmthits->fWavelength_nm;
	thePhoton->idx = (unsigned long long)photonTime + timeShift + TimeShiftInc * k; //Not cntr?
        thePhoton->cathodeTime = (unsigned long long)photonTime + timeShift + TimeShiftInc * k;
        thePhoton->isDER = false;
	thePhoton->BaccEvtNum = (int)evt;
        thePhoton->interactionID = pmthits->iInteractionIdentifier;
        thePhoton->vertexNum = pmthits->iVertexNumber;
        thePhoton->pulseID = pmthits->iPulseID;
	thePMT->assignPhotonToList(thePhoton, eventLength);
    }
    thePMT->generateDarkCounts(evt, EventAndPMTInfos[(int)evt][idx].NumberOfDarkCounts, eventLength);
    delete pmtTree;
#endif
    return true;
}

std::string RootInputMDC2::getUserName()
{
    /**
     * Get the username from the HeaderTree as provided by the
     * input file.
     */
#if (BACC_LIB_VERSION == 6)
    std::string suname_total = "";
    for (int i = 0; i < filePath.size(); i++)
    {
        TChain temp("HeaderTree");
        temp.Add(filePath[i].c_str());
        long long N = temp.GetEntries();
        std::string* suName = 0;
        temp.SetBranchAddress("sUname", &suName);
        for (int i = 0; i < N; i++)
        {
            temp.GetEntry(i);
            suname_total += *suName;
            suname_total += " ";
        }
    }
    return suname_total;
#endif
    return "";
}

void RootInputMDC2::getBaccObj()
{
    /**
     * Protected method that returns BaccObj.
     */
}

#if (BACC_LIB_VERSION == 6)
void RootInputMDC2::getBaccObj(BaccMCTruthEvent**& bacc)
{
    /**
     * Sets parameter to BaccObj.
     */
    bacc = &BaccObj;
}
#endif

double RootInputMDC2::getEventFirstPhotonTime(unsigned long long evt)
{
    /**
     * Returns the EventFirstPhotonTime_ns of the specified event.
     */
    return AllEventFirstPhotonTimes_ns[evt];
}

int RootInputMDC2::determineDarkCounts(const unsigned long length, double darkCountRate) {
    /**
     * Method to generate Dark counts
     */
    //Add dark counts from photocathode
    unsigned long long seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator((unsigned int)seed);
    std::poisson_distribution<int> distribution(darkCountRate*length/1e9);
    return distribution(generator); 
}
