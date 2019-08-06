//
//  EBEvent.cpp
//  devices
//
//  Created by Cees Carels on 13/03/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "EBEvent.hpp"

EBEvent::EBEvent()
    : TSMaxArrSize(46)
{
    /**
     * Constructor for EBEvent.
     *
     * The default initialiser list specifies 46 digitizers,
     * which is in accordance with LZ-ICD-08-0007.
     */
}

EBEvent::~EBEvent()
{
    /**
     * Destructor for EBEvent.
     */
}

void EBEvent::setEBfromDCs(DDC32& theDCs)
{
    /**
     * Given the Data Collector information (provided as the input parameter),
     * set the relevant Event-Builder information such as the start, length,
     * and the sum POD data for the current event only.
     */

    //Get nPods on each DC
    nPods = theDCs.getnPods();

    sumPODstartTS = theDCs.getPODStartTS();
    sumPODlength = theDCs.getPODLength();
    // sumPODdata = theDCs.getSumPODData(); //BUG WITH REJIGGED PULSE
}

void EBEvent::setEvtSeqNumb(const unsigned long& evt)
{
    /**
     * Set the current event sequence number.
     *
     * To understand this variable better, refer to the Event Builder.
     */
    EventSeqNumber = evt;
}

unsigned long EBEvent::getEvtSeqNumb()
{
    /**
     * Get the current event sequence number.
     *
     * To understand this variable better, refer to the Event Builder.
     */
    return EventSeqNumber;
}

std::vector<unsigned short> EBEvent::getnPods()
{
    /**
     * Return the number of PODs in each DC.
     */
    return nPods;
}

void EBEvent::setBufferLiveStartTS(const unsigned long& StartTS)
{
    /**
     * Set Time Stamp (TS) when the buffer for each DC went live.
     */
    bufferLiveStartTS.resize(TSMaxArrSize);
    for (int i = 0; i < TSMaxArrSize; i++)
    {
        bufferLiveStartTS[i] = StartTS;
    }
}

std::vector<unsigned long> EBEvent::getBufferLiveStartTS()
{
    /**
     * Get the Time Stamp (TS) when the buffer for each DC went live.
     */
    return bufferLiveStartTS;
}

void EBEvent::setBufferLiveStopTS(const unsigned long& EndTS)
{
    /**
     * Set Time Stamp (TS) when the buffer for each DC stopped.
     */
    bufferLiveStopTS.resize(TSMaxArrSize);
    for (int i = 0; i < TSMaxArrSize; i++)
    {
        bufferLiveStopTS[i] = EndTS;
    }
}

std::vector<unsigned long> EBEvent::getBufferLiveStopTS()
{
    /**
     * Get Time Stamp (TS) when the buffer for each DC stopped.
     */
    return bufferLiveStopTS;
}

void EBEvent::setTriggerType(const short& type)
{
    /**
     * Set the trigger type that prompted storing this event.
     */
    triggerType = type;
}

short EBEvent::getTriggerType()
{
    /**
     * Get the trigger type that prompted storing this event.
     */
    return triggerType;
}

void EBEvent::setTriggerTimeStamp(const unsigned long& TS)
{
    /**
     * Set the Time Stamp (TS) when the trigger for this event was triggered.
     *
     * This should be in units of sampling rate intervals. For example,
     * if the ADC samples at 10 ns, the timestamp should be given as the number
     * 
     */
    triggerTimeStamp = TS;
}

unsigned long EBEvent::getTriggerTimeStamp()
{
    /**
     * Get the Time Stamp (TS) when the trigger for this event was triggered.
     */
    return triggerTimeStamp;
}

void EBEvent::setTriggerMultiplicity(const short& multiplicity)
{
    /**
     * Set the multiplicity of the trigger.
     *
     * To understand this variable better refer to the Event Builder and DAQ
     * format.
     */
    triggerMultiplicity = multiplicity;
}

short EBEvent::getTriggerMultiplicity()
{
    /**
     * Get the multiplicity of the trigger.
     *
     * To understand this variable better refer to the Event Builder and DAQ
     * format.
     */
    return triggerMultiplicity;
}

void EBEvent::setSumPODstartTS(const unsigned long& StartTS)
{
    /**
     * Set the Time Stamp (TS) for the start of the summed POD for the event.
     */
    sumPODstartTS = StartTS;
}

unsigned long EBEvent::getSumPODStartTS()
{
    /**
     * Get the Time Stamp (TS) for the start of the summed POD for the event.
     */
    return sumPODstartTS;
}

void EBEvent::setSumPODLength(const short& length)
{
    /**
     * Set the length of the summed POD for the event.
     */
    sumPODlength = length;
}

short EBEvent::getSumPODLength()
{
    /**
     * Get the length of the summed POD for the event.
     */
    return sumPODlength;
}

void EBEvent::setSumPODData(const int& sumdata)
{
    /**
     * Set the data for the summed POD.
     */
    sumPODdata.push_back(sumdata);
}
std::vector<int> EBEvent::getSumPODData()
{
    /**
     * Retrieve the summed POD data.
     */
    return sumPODdata;
}
