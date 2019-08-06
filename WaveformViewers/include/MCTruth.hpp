//
//  MCTruth.hpp
//  devices
//
//  Created by Cees Carels on 22/08/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef MCTruth_hpp
#define MCTruth_hpp

#include <iostream>
#include <stdio.h>
#include <vector>

/**
 * Base class providing interface for MCTruth information.
 * The base class contains two structures: MCTruthData, which contains the DER 
 * information for a single photon, and the PODTruth, which provides the MCTruth Data 
 * indices of photons contained within a POD. The MCTruthData objects can be mapped back
 * to the PMTStream and contain the LUXSim event number.
 *
 * The typical use of this class is: at the PMT stage a MCTruthData object is created
 * for every incoming photon hitting a PMT. All MCTruthData objects for one PMT are 
 * stored in a vector. After the PODs for the corresponding high and low gain channels 
 * are created, a PODTruth instance is added containing pointers to all MCTruthData objects
 * belonging to this POD. All PODTruth objects for a channel are stored in a vector,  
 * which is then passed to the ouput stage.
 * 
 * Currently the phdArrivaltime is stored in the MCTruthData, they could be replaced by 
 * referring to indices.
 */

class MCTruth
{
protected:
    struct MCTruthData
    {
        unsigned long long ArrivalTime; //[ns] Arrival time
        double wavelength; //[nm] photon wavelength
        unsigned short int PMTNumber; //PMT number
        unsigned long long SimEvt; //LUXSim/BACCARAT evt number
        int SimRunNumber; //Simulation run number
        unsigned long DEREvt; //DEREvt number
        unsigned short int pheType; //Type/origin of phe
        bool isDER; //True (false) if (not) DER
        unsigned short int InteractionIdentifier; //1 (S1), 2 (S2), 3 (Cherenkov), 4 (other)
        short int VertexNumber; //vertex index
        unsigned short int PulseID; //pulse ID
        unsigned short int HGPODIdx; //High gain pod index in DER event
        unsigned short int LGPODIdx; //Low gain pod index in DER event
        bool counted; //True (false) if entry has (not) been
        //added to sum of photons in pods yet.
    };

    struct PODTruth
    {
        std::vector<MCTruthData*> PtrToMCTruth; //Pointers to MCTruthData
        unsigned long channel; //DER Channel
        unsigned long long Event; //DER Event
    };

    std::vector<MCTruthData> theMCTruthData;
    std::vector<PODTruth> thePODTruth;
    int nPhotonsInPODs;
    int nDarkCounts;

public:
    MCTruth();
    ~MCTruth();

    void setMCTruthSize(const unsigned long size);
    unsigned long getTruthSize();
    int getNPhotonsInPODs();
    int getNDarkCounts();

    void addDERTruth(const int idx,
        const unsigned short int PMTNumber,
        const unsigned short int pheType,
        const bool isDER,
        const unsigned long long SimEvtNumber,
        const unsigned long long ArrivalTime,
        const double wavelength,
        const unsigned short int InteractionIdentifier,
        const short int VertexNumber,
        const unsigned short PulseID);

    void addHGPODIdx(const int idx,
        const unsigned long DEREvt,
        const int SimRunNum);
    void addLGPODIdx(const int idx,
        const unsigned long DEREvt,
        const int SimRunNum);

    MCTruthData* getMCTruthDataObject(const int idx);

    void setPODTruthSize(const unsigned long size);
    unsigned long getPODTruthSize();

    void addPODTruth(const unsigned long idx,
        const unsigned long channel,
        const unsigned long Event,
        MCTruthData* Ptr);

    void getPODTruthAt(const int idx);

    void setPODTruthAt(const unsigned long idx,
        const unsigned long channel,
        const unsigned long Event,
        MCTruthData* Ptr);

    PODTruth* getPODTruthObject(const int idx);
};

#endif /* MCTruth_hpp */
