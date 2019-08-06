//
//  DDC32.cpp
//  devices
//
//  Created by Cees Carels on 01/02/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include <cmath>
#include <iostream>
#include <stdio.h>

#include "DBInterface.hpp"
#include "DDC32.hpp"
#include "Config.hpp"

DDC32::DDC32(const unsigned short& numDC,
    const unsigned short& ChansPerDDC32,
    const unsigned long long& TS)
    : SamplingRate(0)
    , ElementSamplingRate(10)
    , numBits(std::stoi(global::config->getConfig("numbBits")))
    , digMin(std::stof(global::config->getConfig("digMin")))
    , digMax(std::stof(global::config->getConfig("digMax")))
    , EventTimeStamp(TS)
    , NumberofDC(numDC)
    , ChansOnDDC32(32)
    , DCNumber(0)
    , PODStartTS(0)
    , PODLength(0)
{
    /**
     * Constructor for DDC32 which allows setting up the digitizer properties.
     *
     * The data collector properties are also configured.
     */

    DCSummedPulses.resize(numDC);
    nPods.assign(numDC, 0);
}

DDC32::~DDC32()
{
    /**
     * Destructor for DDC32.
     */
}

Pulse DDC32::doDDC32Response(Pulse& thePulse)
{
    /**
     * Transfer function of DDC32.
     */

    //Provide a ready-made digital pulse so that it does not need
    //to be allocated for each PMT.
    Pulse DigPulse(thePulse);

    DigPulse.resize((thePulse.size() / SamplingRate) + 1);

    int idx = 0;
    for (int i = 0; i < thePulse.size(); i += ElementSamplingRate)
    {
      DigPulse[idx] = thePulse[i];
        ++idx;
    }

    //thePulse = DigPulse;

    //Convert mV to ADCC
    for (int i = 0; i < DigPulse.size(); i++)
    {
        double CurrResp = DigPulse[i];
        CurrResp = mVtoADC(CurrResp);
        DigPulse[i] = CurrResp;
    }

    return DigPulse;
}

Pulse DDC32::doDDC32Response(Pulse& pls1, Pulse& pls2)
{
    Pulse DigPls1(pls1);
    Pulse DigPls2(pls2);

    for (int i = 0; i < pls1.size(); i += ElementSamplingRate)
    {
        DigPls1.push_back(pls1[i]);
        DigPls2.push_back(pls2[i]);
    }

    pls1 = DigPls1;
    pls2 = DigPls2;

    //Convert mV to ADCC
    for (int i = 0; i < pls1.size(); i++)
    {
        double CurrResp = pls1[i];
        CurrResp = mVtoADC(CurrResp);
        pls1[i] = CurrResp;

        CurrResp = pls2[i];
        CurrResp = mVtoADC(CurrResp);
        pls2[i] = CurrResp;
    }
    return DigPls1; //Return only one pulse for now, but it could eventually return 2 pulses.
}

void DDC32::mVtoADCC(Pulse& pls1, Pulse& pls2, double DCOffset)
{
    /**
   * Digitise to pulses of the same size.
   */
    for (int i = 0; i < pls1.size(); i++)
    {
        pls1[i] = mVtoADC(pls1[i] + DCOffset);
        pls2[i] = mVtoADC(pls2[i] + DCOffset);
    }
}

int DDC32::mVtoADC(const double& mV)
{
    /**
     * Accepts a voltage [mV] and passes it through the ADC.
     *
     * This method was originally ported from LUXSim2evt but has been improved.
     *
     *\returns This function returns an unsigned short integer [ADCC]
     */

    if (mV > digMax)
        return numBits / 2 - 1;
    if (mV <= digMin)
        return -numBits + numBits / 2;

    double ADCCpermV = (numBits - 1.0) / (digMax - digMin);
    int ADCC = round(ADCCpermV * mV);
    return ADCC;
}

void DDC32::setDCNumber(const unsigned short& dc)
{
    /**
     * Set the Data Collector number on which the DDC32 sits.
     *
     * Method may become redundant.
     */
    DCNumber = dc;
}

unsigned short DDC32::getDCNumber()
{
    /**
     * Return the Data Collector number on which the DDC32 sits.
     *
     * Method may become redundant.
     */
    return DCNumber;
}

void DDC32::sumPulse(Pulse& thePulse,
    const unsigned short& DC,
    const unsigned short& Chan,
    const int& numPODsonChan)
{
    /**
     * Sum the pulse into a summed pulse for each DC.
     */
    DCSummedPulses[DC] += thePulse;
    nPods[DC] += numPODsonChan;
}

void DDC32::LSBRounding(Pulse& thePulse,
    const unsigned short& DC)
{
    /**
     * Rounding to summation to the least significant bit.
     */
}

unsigned short DDC32::getChansOnDDC32()
{
    /**
     * Get the number of channels on each DC.
     */
    return ChansOnDDC32;
}

void DDC32::setSamplingRate(const int& SmplRate)
{
    /**
     * Set the sampling rate in units of [ns].
     */

    SamplingRate = SmplRate;
}

int DDC32::getSamplingRate()
{
    /**
     * Return sampling rate of ADC [ns].
     */
    return SamplingRate;
}

void DDC32::setElementSamplingRate(const int& ElSmplRate)
{
    /**
     * Set how fast the pulse should be sampled in units of elements
     * of the pulse.
     *
     * Example: for 1 ns binning at 10 ns sampling, ElSmplRate = 10.
     */

    ElementSamplingRate = ElSmplRate;
}

int DDC32::getElementSamplingRate()
{
    /**
     * Return the element sampling rate, which is how fast the pulse should be
     * sampled in units of elements of the pulse.
     */
    return ElementSamplingRate;
}

std::vector<unsigned short> DDC32::getnPods()
{
    /**
     * Return the number of PODs.
     *
     * Method may become deprecated.
     */
    return nPods;
}

unsigned long long DDC32::getPODStartTS()
{
    /**
     * Return the start time stamp of PODs.
     *
     * Method may become deprecated.
     */
    return PODStartTS;
}

unsigned long long DDC32::getPODLength()
{
    /**
     * Method may become deprecated.
     */
    return PODLength;
}

std::vector<int> DDC32::getSumPODData()
{
    /**
     * Method to get the summed POD data from the DC.
     */

    Pulse DCSum;
    DCSum.resize(DCSummedPulses[0].size());
    for (int i = 0; i < DCSummedPulses.size(); i++)
    {
        DCSum += DCSummedPulses.at(i);
    }

    SumPODData.reserve(DCSummedPulses[0].size());

    for (int i = 0; i < DCSum.size(); i++)
        SumPODData.push_back(DCSum[i]);

    return SumPODData;
}
