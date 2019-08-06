//
//  PODContainer.hpp
//  devices
//
//  Created by Cees Carels on 07/03/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef PODContainer_hpp
#define PODContainer_hpp

#include <iostream>
#include <stdio.h>
#include <vector>

#include "DBInterface.hpp"
#include "DataSparsifier.hpp"
#include "MCTruth.hpp"
#include "PMTLookup.hpp"
#include "POD.hpp"
#include "Pulse.hpp"

/**
 * Class providing the necessary interface and tools to form PODs from
 * digitised pulse objects.
 *
 * The typical usage of this class is: provide a pulse from a given channel and
 * event. Then pass it to the makePODfromPulse method. This method will store
 * all the PODs on this pulse inside a vector of POD objects. Each POD object
 * inside this vector then contains the ADCC values of each POD as well as the
 * associated time and event information.
 *
 * After this is done, the PODs are extracted from the getNextPOD() method,
 * where the user of the class provides a loop which checks the conditional
 * hasMorePODS() to extract them all.
 *
 * In future this class will change internally for improved performance.
 * It is not anticipated that the usage of the class will change external to
 * this class.
 *
 */

class PODContainer : public std::vector<std::shared_ptr<POD>>
{
public:
    PODContainer();
    ~PODContainer();

    void fillPODContainerFromPulse(Pulse& thePulse, 
				   std::shared_ptr<MCTruth> theMCTruth,
				   const std::string& LGHG);

    std::shared_ptr<POD> getNextPOD();

    unsigned long returnFinalPODSize();

    std::shared_ptr<PODContainer> makePODsFromBoundaries(std::shared_ptr<Pulse> thePulse);
    std::shared_ptr<PODContainer> makePODsFromBoundaries(Pulse& thePulse);

    void setElementSamplingRate(const int& rate);

    void resetPODCounter();

    const std::vector<unsigned long long>& getPodStarts() const;
    const std::vector<unsigned long long>& getPodEnds() const;

private:
    std::vector<unsigned long long> podStarts;
    std::vector<unsigned long long> podEnds;

    unsigned long long timesNextCalled; //!< Counter for getNextPOD().

    int elementSamplingRate; //!< Sampling rate of trigger
    //Units are in number of elements of the pulse.
    //The size of this rate is a function of sampling rate (ADC) or free
    //choice.
    int interPodSampleThreshold;
    //In the case that ElementSamplingRate is the ADC sampling rate, then this
    //variable is in units of samples. If the sampling rate is 10 ns, then 32
    //is the current LZ inter-pod distance threshold to make a single POD.
    unsigned long preTriggerSamples;
    unsigned long postTriggerSamples;

    double podTriggerThreshold; //!< [ADCC] Trigger threshold
};
#endif /* PODContainer_hpp */
