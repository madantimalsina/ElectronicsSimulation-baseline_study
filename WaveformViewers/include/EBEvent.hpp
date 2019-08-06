//
//  EBEvent.hpp
//  devices
//
//  Created by Cees Carels on 13/03/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef EBEvent_hpp
#define EBEvent_hpp

#include <stdio.h>
#include <vector>

#include "DDC32.hpp"
#include "Device.hpp"

/**
 * Class containing event specific information for the current event
 * being simulated.
 */

class EBEvent
{
public:
    EBEvent();
    ~EBEvent();

    void setEBfromDCs(DDC32& theDCs);

    void setEvtSeqNumb(const unsigned long& evt);
    unsigned long getEvtSeqNumb();

    std::vector<unsigned short> getnPods();

    void setBufferLiveStartTS(const unsigned long& StartTS);
    std::vector<unsigned long> getBufferLiveStartTS();

    void setBufferLiveStopTS(const unsigned long& EndTS);
    std::vector<unsigned long> getBufferLiveStopTS();

    void setTriggerType(const short& type);
    short getTriggerType();

    void setTriggerTimeStamp(const unsigned long& TS);
    unsigned long getTriggerTimeStamp();

    void setTriggerMultiplicity(const short& multiplicity);
    short getTriggerMultiplicity();

    void setSumPODstartTS(const unsigned long& StartTS);
    unsigned long getSumPODStartTS();

    void setSumPODLength(const short& length);
    short getSumPODLength();

    void setSumPODData(const int& sumdata);
    std::vector<int> getSumPODData();

private:
    unsigned long EventSeqNumber;
    std::vector<unsigned short> nPods; //[0-NumDC]
    std::vector<unsigned long> bufferLiveStartTS;
    std::vector<unsigned long> bufferLiveStopTS;
    short triggerType;
    unsigned long triggerTimeStamp;
    short triggerMultiplicity;
    unsigned long sumPODstartTS;
    short sumPODlength;
    std::vector<int> sumPODdata;
    int TSMaxArrSize;
};

#endif /* EBEvent_hpp */
