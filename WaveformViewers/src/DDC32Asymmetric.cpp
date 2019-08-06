//
//  DDC32Asymmetric.cpp
//  devices
//
//  Created by Cees Carels on 30/09/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "DDC32Asymmetric.hpp"

DDC32Asymmetric::DDC32Asymmetric()
{
    /**
     * Constructor for DDC32Asymmetric
     */
}

DDC32Asymmetric::~DDC32Asymmetric()
{
    /**
     * Destructor for DDC32Asymmetric.
     */
}

int DDC32Asymmetric::mVtoADC(double& mV)
{
    /**
     * Provides an overloaded method of the base class and implemented an
     * asymmetric ADC range.
     */
    if (mV > digMax)
        return numBits / 2 - 1;
    if (mV <= digMin)
        return -numBits + numBits / 2;

    double ADCCpermV = (numBits - 1.0) / (digMax - digMin);
    int ADCC = round(ADCCpermV * (mV - digMin)) - numBits / 2; //Subtract offset and convert.
    return ADCC;
}
