//
//  FPGATrigger.hpp
//  FPGATrigger
//
//  Created by Cees Carels on 13/02/2018.
//  Copyright © 2018 Carels. All rights reserved.
//

#ifndef FPGATrigger_hpp
#define FPGATrigger_hpp

#include <stdio.h>

#include "POD.hpp"

//Trigger from:
//https://arxiv.org/pdf/1511.03541.pdf

class FPGATrigger
{
public:
    FPGATrigger(const std::string& model);
    ~FPGATrigger();
    double calculateFilterResponse(std::shared_ptr<POD> thePOD, const size_t index);
    std::shared_ptr<POD> processPOD(std::shared_ptr<POD> thePOD);
    void resetPODParameters();
    void addTriggeredChannel(unsigned int channel);
    size_t getTriggeredChannelsSize();
    std::vector<unsigned long long>& getTriggerPoints();
    bool isPossibleForTrigger();

private:
  double A;
  double B;
  double m;
  double n;
  double threshold;
  unsigned int coincidenceWindow;
  unsigned short coincidenceRequirement;
  unsigned long long quietTime;
  unsigned long long holdOffTime;

  size_t previousFilteredIndex;
  double filterResponse;

  std::vector<unsigned long long> triggeredTimeStamps;
  std::vector<unsigned int> triggeredChannels;
  std::vector<unsigned long long> eventTriggerPoints;
  
};

#endif /* Trigger_hpp */
