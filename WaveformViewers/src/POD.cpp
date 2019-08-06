//
//  POD.cpp
//  devices
//
//  Created by Cees Carels on 07/03/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "POD.hpp"
#include "Config.hpp"

POD::POD()
    : fEvent{ 0 }
    , fLuxSimRunNumber{ 0 }
    , fChannel{ 0 }
    , fHitID{ 0 }
    , fStartTime{ 0 }
    , fLength{ 0 }
    , fDataCollectorID{ 0 }
    , fSamplingRate{ std::stoi(global::config->getConfig("SmplRate")) }
    , fIsTriggered{ false }
    , fTriggeredSample{ 0 }
{
    /**
     * Constructor for POD.
     *
     * Configures initial value conditions.
     */
}

POD::~POD()
{
    /**
     * Destructor for POD.
     */
}

void POD::setMCTruth(std::shared_ptr<MCTruth> theTruth)
{
    /**
     * This function is intended to be called for every POD
     * supplying a vector of TruthData objects as input.
     * A PODTruth object is created for every POD and pointers to TruthData
     * Objects are added according to timing.
     *
     * For every POD a PODTruth object is created containing event and
     * channel number. The timing of all TruthData objects which survive QE
     * are compared to the POD start and end times. A pointer to every
     * TruthData object corresponding to photons inside the POD is added to
     * the PODTruth object.
     *
     * All PODTruth objects are added to the MCTruth vector.
     *
     * This method ensures that pointers to all TruthData objects
     * corresponding to photons inside a POD are stored inside a PODTruth
     * object, which is in turn contained within a vector.
     */

    if (theTruth != nullptr)
    {
        theTruth->addPODTruth(0, fChannel, fEvent, 0);
        const unsigned long long idx{ theTruth->getPODTruthSize() - 1 };
        const bool isHighGain{ fChannel < 1000 };
        set_mctruth(theTruth, idx, isHighGain);
    }
}

void POD::set_mctruth(std::shared_ptr<MCTruth> theTruth, const unsigned long long& idx, bool highGain) const
{
    const unsigned long startTime{ fStartTime * fSamplingRate };
    const unsigned long endTime{ startTime + (this->size() * fSamplingRate) };
    for (auto i = 0; i < theTruth->getTruthSize(); i++)
    {
        const unsigned long long time{ theTruth->getMCTruthDataObject(i)->ArrivalTime };
        if (theTruth->getMCTruthDataObject(i)->pheType)
        {
            if (time >= startTime && time <= endTime)
            {
                theTruth->setPODTruthAt(idx, fChannel, fEvent, theTruth->getMCTruthDataObject(i));
                if (highGain)
                {
                    theTruth->addLGPODIdx(i, fEvent, fLuxSimRunNumber);
                }
                else
                {
                    theTruth->addLGPODIdx(i, fEvent, fLuxSimRunNumber);
                }
            }
        }
    }
}

void POD::setEvent(const unsigned long long& evt)
{
    /**
     * Set the event number associated with the POD.
     */
    fEvent = evt;
}

unsigned long long POD::getEvent() const
{
    /**
     * Return the event number associated with a POD.
     */
    return fEvent;
}

void POD::setLUXSimRunNumber(int rNum)
{
    /**
     * Set the event number associated with the POD.
     */
    fLuxSimRunNumber = rNum;
}

int POD::getLUXSimRunNumber() const
{
    /**
     * Return the event number associated with a POD.
     */
    return fLuxSimRunNumber;
}

void POD::setChannel(const int& chan)
{
    /**
     * Set the channel number associated with a POD.
     */
    fChannel = chan;
}

int POD::getChannel() const
{
    /**
     * Return the channel number associated with a POD.
     */
    return fChannel;
}

void POD::setHitID(const short& id)
{
    /**
     * Set the number of the POD in the sequence of PODs on this channel in the
     * given event.
     */
    fHitID = id;
}

short POD::getHitID() const
{
    /**
     * Get the number of the POD in the sequence of PODs on this channel in the
     * given event.
     */
    return fHitID;
}

void POD::setPODStartTimeStamp(const unsigned long long& TS)
{
    /**
     * Set the start time when the POD started.
     */
    fStartTime = TS;
}

unsigned long long POD::getPODStartTimeStamp() const
{
    /**
     * Get the start time when the POD started.
     */
    return fStartTime;
}

void POD::setPODLength(const unsigned long long& length)
{
    /**
     * Set the length of the POD.
     */
    fLength = length;
}

unsigned long long POD::getPODLength() const
{
    /**
     * Get the length of the POD.
     */
    return fLength;
}

const std::vector<short>& POD::getSamples() const
{
    /**
     * Get the vector of ADCC values of the POD.
     */
  return dynamic_cast<const std::vector<short> &>(*this);
}

void POD::setDataCollector(const int& dc)
{
    /**
     * Set the DataCollector number that should be associated with the POD.
     *
     * This method may become deprecated since this information can be
     * managed outside this class and the PODs will still arrive at the correct
     * DC.
     */
    fDataCollectorID = dc;
}

int POD::getDataCollector() const
{
    /**
     * Get the DataCollector number that should be associated with the POD.
     *
     * This method may become deprecated since this information can be
     * managed outside this class and the PODs will still arrive at the correct
     * DC.
     */
    return fDataCollectorID;
}

void POD::setIsTriggered(const bool value)
{
  fIsTriggered = value;
}

bool POD::getIsTriggered() const
{
  return fIsTriggered;
}

void POD::setTriggeredSample(const size_t i)
{
  fTriggeredSample = i;
}

size_t POD::getTriggeredSample() const
{
  return fTriggeredSample;
}

unsigned long long POD::getTriggeredTimeStamp() const
{
  return fStartTime + fSamplingRate * fTriggeredSample;
}
