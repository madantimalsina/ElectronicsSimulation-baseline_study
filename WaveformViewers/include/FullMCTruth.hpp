//
//  FullMCTruth.hpp
//  devices
//
//  Created by Cees Carels on 22/08/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef FullMCTruth_hpp
#define FullMCTruth_hpp

#include <stdio.h>

#include "MCTruth.hpp"

/**
 * Class implementation of MCTruth base class providing full MC Truth data.
 */

class FullMCTruth : public MCTruth
{
public:
    FullMCTruth();
    FullMCTruth(const int& nChannels);
    ~FullMCTruth();
    void addMCTruth(const MCTruthData& channelTruth, const int it);

private:
    //Take care, AllTruth vector elements may not be in the order one expects.
    //The index number inside the vector need not correspond to the channel
    //to which the data in that entry refers.
    std::vector<MCTruthData> AllTruth;
};

#endif /* FullMCTruth_hpp */
