//
//  FullMCTruth.cpp
//  devices
//
//  Created by Cees Carels on 22/08/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "FullMCTruth.hpp"

FullMCTruth::FullMCTruth()
{
}

FullMCTruth::FullMCTruth(const int& nChannels)
{
    AllTruth.resize(nChannels);
}

FullMCTruth::~FullMCTruth()
{
}

void FullMCTruth::addMCTruth(const MCTruthData& channelTruth, const int it)
{
    AllTruth[it] = channelTruth;
}