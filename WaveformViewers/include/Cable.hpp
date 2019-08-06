//
//  Cable.hpp
//  devices
//
//  Created by Cees Carels on 06/07/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef Cable_hpp
#define Cable_hpp

#include <iostream>
#include <stdio.h>
#include "Device.hpp"
#include "Pulse.hpp"

/**
 * Base class providing the interface of and implementation for many cable
 * methods. The class allows calculating properties of the signal propagation
 * in terms of the cable properties.
 */

class Cable : public Device
{
public:
  Cable();
  Cable(const der::DeviceModel& model);
  virtual ~Cable();
  void runFilters(double& sampleValue);
  void reset();

  std::array<double,6> accus;
  std::array<double,6> taus1;
  std::array<double,6> taus2;
  std::array<double,6> exps;
  std::array<double,6> oneMinusExps;
  std::array<double,6> tauRatios;
  std::array<double,6> oneMinusTauRatios;

};

#endif /* Cable_hpp */
