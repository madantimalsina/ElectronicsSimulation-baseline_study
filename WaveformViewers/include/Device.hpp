//
//  Device.hpp
//
//  Created by Cees Carels on 29/12/2015.
//  Copyright © 2015 LZOxford. All rights reserved.
//

#ifndef Device_hpp
#define Device_hpp

#include <algorithm>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "DBInterface.hpp"
#include "Pulse.hpp"
#include "MCTruth.hpp"

/**
 * Base class providing the basic device interface.
 *
 * Derived classes will add to the interface as needed.
 */

namespace der{
  enum class DeviceModel{
    kLowGain = 1,
    kHighGain = 2,
    kSampled = 3,
    kAnalytic = 4,
    kPMT = 5,
    kFeedthrough = 6
  };
}

class Device
{
public:
    Device();
    virtual ~Device() = 0;
    std::string getName();
    void setName(std::string name);
    virtual void doResponse(Pulse& thePulse);
    virtual void doResponse(Pulse& theLGPulse, Pulse& theHGPulse);
    virtual void doStageResponse(Pulse& thePulse);
    virtual void doStageResponse(Pulse& theLGPulse, Pulse& theHGPulse);
    virtual void prepareMCTruth(std::shared_ptr<MCTruth> theMCTruth);
    virtual void printRunningTime();
    virtual void runFilters(double& sampleValue);
    virtual void reset();
    virtual void findBaseline(Pulse& thePulse, unsigned int& j);

protected:
    std::string sName; //!< Name of device
    der::DeviceModel sModel; //!< Model of device
    double fTimeBase;
    double fHalfADCC;
};

#endif /* Device_hpp */
