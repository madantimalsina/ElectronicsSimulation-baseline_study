//
//  POD.hpp
//  devices
//
//  Created by Cees Carels on 07/03/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef POD_hpp
#define POD_hpp

#include <iostream>
#include <stdio.h>
#include <vector>

#include "DBInterface.hpp"
#include "DataSparsifier.hpp"
#include "MCTruth.hpp"
#include "Pulse.hpp"

class POD : public std::vector<short>
{
public:
    POD();

    ~POD();

    void setEvent(const unsigned long long& evt);
    unsigned long long getEvent() const;

    void setLUXSimRunNumber(int runN);
    int getLUXSimRunNumber() const;

    void setChannel(const int& chan);
    int getChannel() const;

    void setHitID(const short& id);
    short getHitID() const;

    void setPODStartTimeStamp(const unsigned long long& TS);
    unsigned long long getPODStartTimeStamp() const;

    void setPODLength(const unsigned long long& length);
    unsigned long long getPODLength() const;

    const std::vector<short>& getSamples() const;

    void setDataCollector(const int& dc); //[0-14]
    int getDataCollector() const;

    void setElementSamplingRate(const int& rate);

    void setMCTruth(std::shared_ptr<MCTruth> theTruth);

    void setIsTriggered(const bool value);
    bool getIsTriggered() const;

    void setTriggeredSample(const size_t sample);
    size_t getTriggeredSample() const;
    unsigned long long getTriggeredTimeStamp() const;

private:
    void set_mctruth(std::shared_ptr<MCTruth> theTruth, const unsigned long long& idx, bool highGain) const;

private:
    unsigned long long fEvent; //!< Event the POD is on.
    int fLuxSimRunNumber; //!< LUXSim Run number
    int fChannel; //!< Channel the POD is on.
    short fHitID; //!< Number of POD in sequence.
    unsigned long long fStartTime; //!< Start time of POD.
    unsigned long long fLength; //!< Length of POD in samples.
    int fDataCollectorID; // ID for DC on which POD belongs.
    int fSamplingRate; //[ns]
    bool fIsTriggered;
    size_t fTriggeredSample;
};
#endif /* POD_hpp */
