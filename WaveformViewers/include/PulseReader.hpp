//
//  PulseReader.hpp
//  devices
//
//  Created by Cees Carels on 15/08/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef PulseReader_hpp
#define PulseReader_hpp

#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <vector>

#include <fcntl.h>
#include <sys/stat.h>
#include <wordexp.h>

#include "DBInterface.hpp"

/**
 * Class to read the measured pulse data from 1.8.
 */

class PulseReader
{
public:
    PulseReader();
    ~PulseReader();

    int doReadPulses();
    std::vector<float>* getPulseHE(const int& it);
    unsigned long HESize();

    std::vector<float>* getPulseLE(const int& it);
    unsigned long LESize();

    int getPlsStartIdx();

    void setSampledGain(const double Gain);
    void setUserGain(const double Gain);
    void SubtractBaseline();
    void RescalePulses();
    int setupPulseReader();

private:
    std::vector<std::vector<float>> HEpulses;
    std::vector<std::vector<float>> LEpulses;

    int PlsStrtIdx; //Known from inspecting the files.
    //All samples before the 30th sample are baseline.
    double IGain; //Input sampled gain
    double UGain; //User gain
};

#endif /* PulseReader_hpp */
