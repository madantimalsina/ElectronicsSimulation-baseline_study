//
//  Input.hpp
//  devices
//
//  Created by Cees Carels on 24/10/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef Input_hpp
#define Input_hpp

#include <algorithm>
#include <cctype>
#include <set>
#include <stdio.h>
#include <string>
#include <vector>

#include "TChain.h"

#include "DBInterface.hpp"
#include "Config.hpp"
#include "InputOutputFormats.hpp"
#include "Pulse.hpp"
#include "PMT.hpp"

/**
 * Base class providing interface to read input data from
 * pre-DER stages of simulations, such as LUXSim/PMTStream data
 */

class Input
{
protected:
    struct PMTData
    {
        /**
         * A struct used to contain the pertinent PMT data including:
         * the arrival time of the photon, its wavelength, the PMT ID it arrived on,
         * and what LUXSim event it belongs to.
         */

        double time;
        double wavelength;
        int id;
        int EventID;
        int RunNumber;
        unsigned short int InteractionIdentifier;
        short int VertexNumber;
    };

    std::vector<unsigned long long> EmptyEvents;
    std::vector<PMTData> PmtData;
    unsigned long ExpectedTotalPMTs;
    double PostTriggerWindow; //Post trigger window cut on photon times.
    double PhotonTimeLimit; //Manual limit for photon arrival time.

    void makeEvtListFromSelection();
    std::vector<unsigned long long> makeEvtList();
    std::vector<int> AvailablePMTs;

    std::vector<unsigned long long> AvailableEvents;
    std::vector<unsigned long long> SelectedEvents; //Select events

    std::vector<double> AllEventFirstPhotonTimes_ns;

    std::vector<std::string> filePath;

    bool ComparePMTData(PMTData const& lhs,
        PMTData const& rhs);
    bool EventIsContained(const int ID,
        const std::vector<unsigned long long>& FindIn);
    std::string getSeedFromFile(const std::string& inFilename);
    bool PMTwasFound(const std::vector<int>& vec,
        const int& val);
    bool toBool(const std::string& conf);

    bool checkForMissingPMTs();

    void SortData();

    //Detector selection
    bool doTPC = false;
    bool doOut = false;
    bool doSkin = false;

    //Number of expected PMTs
    int TPCTopPMTs;
    int TPCTopDummyPMTs;
    int TPCBotPMTs;
    int TPCBotDummyPMTs;
    int TPCPMTs;
    int SkinTPMTs;
    int SkinTDummy;
    int SkinBPMTs;
    int SkinBDummyPMTs;
    int SkinBDPMTs;
    int SkinBDDummyPMTs;
    int SkinPMTs;
    int OuterPMTs;
    int OuterDummyPMTs;

    //Check the number of PMTs
    std::vector<int> TPCBFound; //TPC bottom
    std::vector<int> TPCBDmyFound; //TPC bottom dummy
    std::vector<int> TPCTFound; //TPC top
    std::vector<int> TPCTDmyFound; //TPC top dummy
    std::vector<int> SkiTFound; //Skin top
    std::vector<int> SkiTDmyFound; //Skin top dummy
    std::vector<int> SkiBFound; //Skin bottom
    std::vector<int> SkiBDmyFound; //Skin bottom dummy
    std::vector<int> SkiBDFound; //Skin bottom dome
    std::vector<int> SkiBDDmyFound; //Skin bottom dome dummy
    std::vector<int> OutFound; //Outer
    std::vector<int> OutDmyFound; //Outer dummy

    format::LZ_ICD_08_0008 LZ_PMT_Numbering_Revision;

    TChain* data; //Pointer to input chain of data files.
    //Only Jul2017 and later currently support using this variable.

public:
    Input();
    virtual ~Input() = 0;

    void setFile(const std::vector<std::string> path);
    virtual TFile* getFilePointer() = 0;
    virtual bool Open() = 0;
    virtual void Close() = 0;

    void SelectDetectors();

    virtual int makePMTDataReady() = 0;
    virtual int makePMTDataReady(const unsigned long long& Evt,
        unsigned long long& NPhotons,
        unsigned long long& TMin,
        unsigned long long& TMax,
        unsigned long long& NVertices,
        std::vector<int>& PmtsInEvt);

    virtual bool getPMTData(const unsigned long long evt) = 0;
    virtual  bool getPMTData(const unsigned long long evt,
        int pmt,
        std::shared_ptr<PMT> thePMT,
	unsigned long eventLength,
        unsigned long long timeShift,
        unsigned long long TimeShiftInce,
        unsigned long long k)
        = 0;
    unsigned long long PMTDataSize();
    unsigned long long getAvailPMTat(const unsigned long long it);
    unsigned long long getAvailPMTback();

    virtual std::string getUserName() = 0;

    std::vector<unsigned long long> getPMTs();
    unsigned long getNPMTs();
    unsigned long getNExpectedPMTs();

    unsigned long getNEvts();
    unsigned long getNNonEmptyEvts(); //Used for Binary output

    int checkEvtList();

    unsigned long long getSelecEvtsAt(const unsigned long long it);
    unsigned long long getSelecEvtsSize();
    unsigned long long getAvailEvtsAt(const unsigned long long it);
    unsigned long long getAvailEvtsSize();

    std::string getRandomSeedFromFileName();
    TChain* getDataTChain();

    PMTData* PMTDataAt(const unsigned long long it);
    virtual void getBaccObj();
    virtual double getEventFirstPhotonTime(unsigned long long evt);
};

#endif /* Input_hpp */
