//
//  PulseManager.hpp
//  devices
//
//  Created by Cees Carels on 02/02/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef PulseManager_hpp
#define PulseManager_hpp

#include <iomanip>
#include <iostream>
#include <math.h>
#include <set>
#include <stdio.h>
#include <vector>

#include "Device.hpp"
#include "PMTLookup.hpp"
#include "Pulse.hpp"

/**
 * Class to store and manage pulses that came from input data. The PulseManger
 * also retains information about the current PMT and PMT channel, providing
 * it to the signal chain where needed.
 *
 * Pulse Manager stores its own copy of single channel pulse object
 * and provides it upon request.
 */

class PulseManager
{
public:
    PulseManager();
    PulseManager(const int& SampleRate);
    PulseManager(Pulse& thePulse,
        const int& SampleRate);
    ~PulseManager();

    Pulse getPMTPulse();

    void PrepareEventBounds(Pulse& thePulse,
        unsigned long long TMin,
        unsigned long long TMax);
    void preparePulse();

    Pulse getPMTSubset();

    void setPMTNumber(const int& PMTNumber);
    int getPMTNumber();

    void setPMTChannel(const int& PMTChannel);
    int getPMTChannel();

    void markEndAcquisition();
    bool isEndedAcquisition();

    unsigned long SubPlsNumb();

    unsigned long getBeginSubset();
    unsigned long getEndSubset();

    unsigned long NumbOfSubsets();

    void setSamplingRate(const int& SmplRate);
    int getSamplingRate();

    int setElementSampleRate();
    int getElementSampleRate();

    void setPulseSubsetBeg(const std::vector<double>& BegVec);
    std::vector<double> getPulseSubsetBeg();

protected:
    Pulse PMTPulse;
    std::vector<Pulse> PulseSubsets; //!< As prepared by preparePulse.
    std::vector<double> PulseSubsetBeg; //!< Used in separate object.
    int TimesCalled; //!< Number of Subsets called.
    bool MoreSubsets; //!< Are there more Subsets?
    std::vector<double> PMTPulseE; //!< Will become deprecated.
    bool AcquisitionDidEnd; //!< Will become deprecated.
    unsigned long long beginSubset; //!< Begin time of pulse subset.
    unsigned long long endSubset; //!< End time of pulse subset.
    long SubsetIncrement; //!< Increment time between start/end.
    unsigned long SubsetNumber; //!< Number of the pulse subset.
    int CurrentPMT; //!< PMT number of current pulse.
    int CurrentPMTChannel; //!< Channel number of current pulse.
    int SamplingRate; //!< [ns] Real sampling rate.
    int ElementSamplingRate; //!< Array element sampling rate.
    unsigned long long simPrecision; //!< Precision of binning in time.
    //!< Describes level of precision in time.
    //!< A precision of 0.001 ns is a simPrecision of 1/0.001 = 1000.
    unsigned long numbOfSubsets; //!< Number of subsets
    void calcNumbOfSubsets(); //!< Sets the number of subsets.
};

#endif /* pulsemanager_hpp */
