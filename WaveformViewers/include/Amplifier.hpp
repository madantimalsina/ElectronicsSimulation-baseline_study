//
//  Amplifier.hpp
//  devices
//
//  Created by Cees Carels on 06/07/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef Amplifier_hpp
#define Amplifier_hpp

#include <complex>
#include <math.h>
#include <stdio.h>

#include "Device.hpp"
#include "Filters.hpp"
#include "Pulse.hpp"

/**
 * Base class for amplifiers.
 */

class Amplifier : public Device
{
public:
    Amplifier();
    Amplifier(const der::DeviceModel& model);
    virtual ~Amplifier();
    void runFilters(double& sampleValue);
    void reset();

private:
    double G;
    std::array<double, 10> taus;
    std::array<double, 10> accus;
    std::array<double, 4> skAccus;
    std::array<double, 10> exps;
    std::array<double, 10> oneMinusExps;
    std::array<double, 5> tauRatios;
    std::array<double, 5> oneMinusTauRatios;
    double expRsinI;
    void doLGSetup();
    void doHGSetup();  
    void runLGFilters(double& sampleValue);
    void runHGFilters(double& sampleValue);
};

#endif /* Amplifier_hpp */
