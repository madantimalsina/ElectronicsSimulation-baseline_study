//
//  Cable.cpp
//  devices
//
//  Created by Cees Carels on 29/12/2015.
//  Copyright © 2015 LZOxford. All rights reserved.
//

#include "Cable.hpp"
#include "Pulse.hpp"
#include "Filters.hpp"

Cable::Cable()
{
 /**
   * Default constructor for Cable.
   */
}

Cable::Cable(const der::DeviceModel& model)
{
  sModel = model;

  if (sModel == der::DeviceModel::kPMT)
    { //Fill out parameters for PMTCable  
      taus1 = {34, 8.77, 2.70, 0.61, 0.61, 254.4};
      taus2 = {210, 27.2, 1.07, 5.84, 4.97E-6, 254.4};
    }
  else
    { //LMR100
      taus1 = {624, 8.19, 4.15, 2.35, 0.802, .286};
      taus2 = {593, 7.05, 3.79, 1.85, 0.529, .286};
    }

  for(int i = 0; i<6; ++i){
    exps[i] = exp(-fTimeBase/taus2[i]);
    oneMinusExps[i] = 1-exps[i];
    tauRatios[i] = taus1[i]/taus2[i];
    oneMinusTauRatios[i] = 1-tauRatios[i];
  }

  accus.fill(0.0);
}

Cable::~Cable()
{
  /**
   * Destructor for Cable.
   */
}

void Cable::runFilters(double& sampleValue){
  for(int k = 0; k<5; ++k)
    poleZero(sampleValue,accus[k],exps[k],tauRatios[k],oneMinusExps[k],oneMinusTauRatios[k]);
}

void Cable::reset(){
  accus.fill(0.0);
}

