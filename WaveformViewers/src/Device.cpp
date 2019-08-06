//
//  Device.cpp
//
//  Created by Cees Carels on 30/12/2015.
//  Copyright © 2015 LZOxford. All rights reserved.
//

#include "Device.hpp"

Device::Device() : fTimeBase(1.0), fHalfADCC(0.5/8192)
{
  /**
   * Constructor for Device.
   */
}

Device::~Device()
{
  /**
   * Destructor for Device.
   */
}

std::string Device::getName()
{
  /**
   * Return the Device name.
   */
  return sName;
}

void Device::setName(std::string name)
{
  /**
   * Set Device name.
   */
  sName = name;
}

void Device::doResponse(Pulse& thePulse)
{
  unsigned int j = (thePulse.getPhotonSize()>=1 ? 
		    thePulse.getPhotonIntervalAt(0).first : 0);

  for(unsigned int i = 0; i<thePulse.getPhotonSize(); ++i){

    bool changeIntervals = false;

    for(; j<thePulse.getPhotonIntervalAt(i).second; ++j){
      runFilters(thePulse[j]);
    }

    findBaseline(thePulse,j); //continue filtering until pulse settles
    thePulse.setPhotonIntervalEnd(i,j); //set the end sample to that as a result of baseline finding
    thePulse.findNextFilterSample(i,j); //find the next j to start from, skipping overlapping intervals
  }
  reset();
}

void Device::doResponse(Pulse& theLGPulse, Pulse& theHGPulse)
{
  doResponse(theLGPulse);
  doResponse(theHGPulse);
}

void Device::doStageResponse(Pulse& thePulse)
{
}

void Device::doStageResponse(Pulse& theLGPulse, Pulse& theHGPulse){
  doStageResponse(theLGPulse);
  doStageResponse(theHGPulse);
}

void Device::prepareMCTruth(std::shared_ptr<MCTruth> theMCTruth)
{
}

void Device::printRunningTime()
{
}

void Device::runFilters(double& sampleValue){
}

void Device::reset(){
}

void Device::findBaseline(Pulse& thePulse, unsigned int& j){
  unsigned int avgSamples = 10;
  if(j>avgSamples){
    double average = 0;
    for(unsigned int k = j-avgSamples; k<j; ++k){
      average += thePulse[k];
    }
    average /= avgSamples;

    bool baselineFound = false;
    while(!baselineFound && j<thePulse.size()-1){
      baselineFound = true;
      for(unsigned int k = j-avgSamples; k<j; ++k){
	if(std::abs(thePulse[k]-average)>fHalfADCC/avgSamples) {
	  baselineFound = false;
	  break;
	}
      }
      if(!baselineFound){
	runFilters(thePulse[j]);
	average -= thePulse[j-avgSamples]/avgSamples;
	average += thePulse[j]/avgSamples;
	++j;
      }
    }
  }
}
