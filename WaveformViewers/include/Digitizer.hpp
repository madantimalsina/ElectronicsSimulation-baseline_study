//
//  Digitizer.hpp
//  devices
//
//  Created by Cees Carels on 06/07/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef Digitizer_hpp
#define Digitizer_hpp

#include <stdio.h>
#include <math.h>
#include <complex>

#include "Device.hpp"
#include "Filters.hpp"
#include "Pulse.hpp"
#include "Config.hpp"

#include "TRandom.h"

/**
 * Base class for Digitizer devices.
 */

class Digitizer : public Device
{
public:
    Digitizer();
    Digitizer(const der::DeviceModel& model);
    virtual ~Digitizer();
    void doResponse(Pulse& thePulse);
    void doStageResponse(Pulse& thePulse);
    void setSamplingInterval(int samplingInterval);
    int digitizePoint(double mV, bool addNoise);
    int mVtoADC(const double& mV);
    double addBaselineNoise(const double& mV);
    void runFilters(double& sampleValue);
    void reset();

private:
    double fDigMax;
    double fDigMin;
    double fNumBits;
    double fMinBit;
    double fMaxBit;
    double fADCpermV;
    double fDCOffset;
    int iSamplingInterval;
    bool doNoiseAddition;
    double baselineSigma;
    bool doDownConvertPhotonIntervals;
    std::array<double, 4> skAccus;
    double skExp;
    double skOneMinusExp;
    double skExpRsinI;
};

#endif /* Digitizer_hpp */
