//
//  MCTruth.cpp
//  devices
//
//  Created by Cees Carels on 22/08/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "MCTruth.hpp"
#include "Output.hpp"

MCTruth::MCTruth()
    : nPhotonsInPODs(0)
    , nDarkCounts(0)
{
    /**
     * Constructor for MCTruth.
     */
}

MCTruth::~MCTruth()
{
    /**
     * Destructor for MCTruth.
     */
}

void MCTruth::setMCTruthSize(const unsigned long size)
{
    /**
     * Set size of vector containing MCTruthData objects.
     */
    theMCTruthData.resize(size);
    nPhotonsInPODs = 0;
    nDarkCounts = 0;
}

unsigned long MCTruth::getTruthSize()
{
    /**
     * Get size of vector containing MCTruthData objects.
     */
    return theMCTruthData.size();
}

int MCTruth::getNPhotonsInPODs()
{
    /**
    * Get number of MCTruth entries corresponding to photons which
    * made it into a POD.
    */
    return nPhotonsInPODs;
}

int MCTruth::getNDarkCounts()
{
    /**
    * Get number of MCTruth entries corresponding to dark counts which
    * made it into a POD.
    */
    return nDarkCounts;
}

void MCTruth::addDERTruth(const int idx,
    const unsigned short int PMTNumber,
    const unsigned short int pheType,
    const bool isDER,
    const unsigned long long SimEvtNumber,
    const unsigned long long ArrivalTime,
    const double wavelength,
    const unsigned short int InteractionIdentifier,
    const short int VertexNumber,
    const unsigned short PulseID)
{
    /**
     * Method adds new MCTruthData object to theMCTruthData vector. 
     */
    MCTruthData newTruth;
    newTruth.PMTNumber = PMTNumber;
    newTruth.pheType = pheType;
    newTruth.isDER = isDER;
    newTruth.SimEvt = SimEvtNumber; //Implicit type conversion. Check type!
    newTruth.ArrivalTime = ArrivalTime;
    newTruth.wavelength = wavelength;
    newTruth.InteractionIdentifier = InteractionIdentifier;
    newTruth.VertexNumber = VertexNumber;
    newTruth.PulseID = PulseID;
    newTruth.HGPODIdx = 0; //This is set at POD stage, once a photon has actually made it into the evt.
    newTruth.LGPODIdx = 0; //This is set at POD stage.
    newTruth.DEREvt = 0; //This is set at POD stage.
    newTruth.SimRunNumber = -1; //This is set at POD stage.
    newTruth.counted = false;
    theMCTruthData[idx] = (newTruth);
}

void MCTruth::addHGPODIdx(const int idx,
    const unsigned long DERevt,
    const int SimRunNum)
{
    /**
    * Method to add HG POD idx to the truth object.
    */
    theMCTruthData[idx].HGPODIdx = Output::getNPODsEvt();
    theMCTruthData[idx].DEREvt = DERevt;
    theMCTruthData[idx].SimRunNumber = SimRunNum;

    if (theMCTruthData[idx].counted == false)
    {
        ++nPhotonsInPODs;
        theMCTruthData[idx].counted = true;
        if (theMCTruthData[idx].pheType == 5)
            ++nDarkCounts;
    }
}

void MCTruth::addLGPODIdx(const int idx,
    const unsigned long DERevt,
    const int SimRunNum)
{
    /**
    * Method to add LG POD idx to the truth object.
    */
    theMCTruthData[idx].LGPODIdx = Output::getNPODsEvt();
    theMCTruthData[idx].DEREvt = DERevt;
    theMCTruthData[idx].SimRunNumber = SimRunNum;

    if (theMCTruthData[idx].counted == false)
    {
        ++nPhotonsInPODs;
        theMCTruthData[idx].counted = true;
        if (theMCTruthData[idx].pheType == 5)
            ++nDarkCounts;
    }
}

MCTruth::MCTruthData* MCTruth::getMCTruthDataObject(const int idx)
{
    /**
     * Get pointer to MCTruthData object at idx of theMCTruthData vector.
     */
    return &theMCTruthData[idx];
}

void MCTruth::setPODTruthSize(const unsigned long size)
{
    /**
     * Set size of vector containing PODTruth objects.
     */
    thePODTruth.resize(size);
}

unsigned long MCTruth::getPODTruthSize()
{
    /**
     * Get size of vector containing PODTruth objects.
     */
    return thePODTruth.size();
}

void MCTruth::addPODTruth(const unsigned long idx,
    const unsigned long channel,
    const unsigned long Event,
    MCTruthData* Ptr)
{
    /**
     * Method to add new PODTruth object without MCTruthData pointer at
     * the end of thePODTruth vector.
     *
     * Channel and event number are set, but pointer to the TruthData
     * object is ignored.
     * This method is intended to be called for every POD. The
     * setPODTruthAt method should be subsequently used to fill
     * the MCTruthData pointer vector.
     *
     * The method is intended to be used to initialize a PODTruth object
     * for every POD.
     */
    PODTruth newPODTruth;
    newPODTruth.channel = channel;
    newPODTruth.Event = Event;
    thePODTruth.push_back(newPODTruth);
}

void MCTruth::getPODTruthAt(const int idx)
{
    /**
     * Method to get PODTruth information at idx of the PODTruth vector
     * and print the information to screen.
     */
    std::cout << "The POD Truth at idx " << idx << std::endl;
    std::cout << "Channel " << thePODTruth.at(idx).channel << std::endl;
    std::cout << "Event " << thePODTruth.at(idx).Event << std::endl;
    std::cout << "Pointers to truth objects:" << std::endl;
    std::cout << "Size " << thePODTruth.at(idx).PtrToMCTruth.size() << std::endl;
    for (int i = 0; i < thePODTruth.at(idx).PtrToMCTruth.size(); i++)
    {
        std::cout << "i " << i << " pointer to " << thePODTruth.at(i).PtrToMCTruth.at(i) << std::endl;
    }
    std::cout << "Done." << std::endl;
}

void MCTruth::setPODTruthAt(const unsigned long idx,
    const unsigned long channel,
    const unsigned long Event,
    MCTruthData* Ptr)
{
    /**
     * Method to set channel and event numbers of PODTruth object
     * at index (idx) of the PODTruth vector and add a pointer
     * to an MCTruthData object contained within the POD to a vector.
     *
     * This method is intended to be called to fill a PODTruth object with
     * pointers to MCTruthData Objects.
     */
    thePODTruth[idx].channel = channel;
    thePODTruth[idx].Event = Event;
    thePODTruth[idx].PtrToMCTruth.push_back(Ptr);
}

MCTruth::PODTruth* MCTruth::getPODTruthObject(const int idx)
{
    return &thePODTruth[idx];
}
