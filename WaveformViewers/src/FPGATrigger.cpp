//
//  FPGATrigger.cpp
//  FPGATrigger
//
//  Created by Cees Carels on 13/02/2018.
//  Copyright © 2018 Carels. All rights reserved.
//

#include <algorithm>

#include "FPGATrigger.hpp"
#include "Config.hpp"

FPGATrigger::FPGATrigger(const std::string& model) : filterResponse(0), coincidenceRequirement(3), quietTime(1500000), holdOffTime(2500000)
{
  if (model.find("S1") != std::string::npos)
    { //S1
      A = 1.0;
      B = (model.find("LG") != std::string::npos ? 0.5 : 5); //LG v HG - says 0.5 in the paper
      n = std::stoi(global::config->getConfig("S1TriggerN")); //programmable from 1 to 16 samples
      m = n;
      threshold = 10; //not sure where this comes from...
      coincidenceWindow = 20;
    }
  else if (model.find("S2") != std::string::npos)
    { //S2
      A = 1.0;
      B = 2.0;
      n = std::stoi(global::config->getConfig("S2TriggerN")); //n can be 1 to 64 samples
      m = 4*n;
      threshold = (model.find("LG") != std::string::npos ? 10000 : 30000); 
      coincidenceWindow = (model.find("LG") != std::string::npos ? 60 : 80); 
    }
}

FPGATrigger::~FPGATrigger()
{
}

double FPGATrigger::calculateFilterResponse(std::shared_ptr<POD> thePOD, const size_t index)
{
  double DCOffset = 7372; //need to rebase all the POD data - if there were noise, we should really take a baseline
  if(index==0)
    filterResponse = 0; //assume we're restricted to the POD, so no samples to iterate on
  else if(index==previousFilteredIndex+1){ //If cycling through the POD 
    //indices in order, then tweak the previous trigger response 

    filterResponse -= B*((int)index-2*n-m >= 0 ? 
  			 (double)(thePOD->at(index-2*n-m))-DCOffset : 0);

    filterResponse += (B+A)*((int)index-n-m >= 0 ? 
  			     (double)(thePOD->at(index-n-m))-DCOffset : 0);

    filterResponse += (-A-B)*((int)index-n >= 0 ? 
			      (double)(thePOD->at(index-n))-DCOffset : 0);

    filterResponse += B*((double)(thePOD->at(index))-DCOffset);

  }
  else{ //else calculate from scratch 
    filterResponse = 0;
    unsigned int endPoint = index;
    unsigned int startPoint = ((int)index-n+1>0 ? index-n+1 : 0);
    for(unsigned int i = startPoint; i <= endPoint; ++i)
      filterResponse += ((double)thePOD->at(i)-DCOffset)*B;
    
    endPoint = (startPoint<=1 ? 0 : startPoint-1);
    startPoint = ((int)endPoint-m+1>0 ? endPoint-m+1 : 0);
    for(unsigned int i = startPoint; i <= endPoint; ++i)
      filterResponse -= ((double)thePOD->at(i)-DCOffset)*A;

    endPoint = (startPoint<=1 ? 0 : startPoint-1);
    startPoint = ((int)endPoint-n+1>0 ? endPoint-n+1 : 0);
    for(unsigned int i = startPoint; i <= endPoint; ++i)
      filterResponse += ((double)thePOD->at(i)-DCOffset)*B;
  }
  previousFilteredIndex = index;
  return filterResponse;
}

std::shared_ptr<POD> FPGATrigger::processPOD(std::shared_ptr<POD> thePOD)
{
  short DCOffset = 7372;
  std::shared_ptr<POD> theTriggerPOD(new POD());
  theTriggerPOD->resize(thePOD->size());
  theTriggerPOD->setPODLength(thePOD->size());
  theTriggerPOD->setHitID(thePOD->getHitID());
  for (size_t i = 0; i < thePOD->size(); ++i)
    {
      short response = (short)calculateFilterResponse(thePOD,i)+DCOffset; //reapply the DC offset so that we can view this on the PODViewer
      theTriggerPOD->at(i) = response;
      if(response>threshold){
        thePOD->setIsTriggered(true);
        thePOD->setTriggeredSample(i);
	triggeredTimeStamps.push_back(thePOD->getTriggeredTimeStamp());
      }
    }
  resetPODParameters();
  return theTriggerPOD;
}

void FPGATrigger::resetPODParameters()
{
  filterResponse = 0;
  previousFilteredIndex = 0;
}

void FPGATrigger::addTriggeredChannel(unsigned int channel)
{
  triggeredChannels.push_back(channel);
}

size_t FPGATrigger::getTriggeredChannelsSize()
{
  return triggeredChannels.size();
}

std::vector<unsigned long long>& FPGATrigger::getTriggerPoints()
{
  if(triggeredTimeStamps.size()>coincidenceRequirement){
    std::sort(triggeredTimeStamps.begin(), triggeredTimeStamps.end());
    unsigned long long previousPoint = triggeredTimeStamps.at(0);
    unsigned long long triggerResumeTime = 0;
    unsigned int coincidence = 0;
    for(size_t i = 1; i<triggeredTimeStamps.size(); ++i){
      if(triggeredTimeStamps[i]>triggerResumeTime){
	if(triggeredTimeStamps[i]-previousPoint < coincidenceWindow)
	  ++coincidence;
	else{
	  if(coincidence >= coincidenceRequirement){ //triggered
	    eventTriggerPoints.push_back(previousPoint);
	    //triggerResumeTime = previousPoint + quietTime + holdOffTime; //do not trigger again for a certain amount of time
	  }
	  coincidence = 0;
	  previousPoint = triggeredTimeStamps[i];
	}
      }
    }
  }
  return eventTriggerPoints;
} 

bool FPGATrigger::isPossibleForTrigger(){
  return (triggeredChannels.size() >= coincidenceRequirement ? true : false);
}
