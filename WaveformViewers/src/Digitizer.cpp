//
//  Digitizer.cpp
//  devices
//
//  Created by Cees Carels on 11/02/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include <stdio.h>

#include "Digitizer.hpp"

Digitizer::Digitizer()
{
    /**
     * Constructor for Digitizer.
     */
}

Digitizer::Digitizer(const der::DeviceModel& model)
{
    /**
     * Constructor for Digitizer.
     */
    sModel = model;
    fDigMax = 1000;
    fDigMin = -1000;
    fNumBits = 16384;
    fMinBit = int(-0.5 * fNumBits);
    fMaxBit = fNumBits * 0.5 - 1;
    fADCpermV = (fNumBits - 1.0) / (fDigMax - fDigMin);
    fDCOffset = 900;
    iSamplingInterval = (sModel == der::DeviceModel::kSampled ? 1 : 10);

    double alpha = 1/11.1745;
    double gamma = 1/6.58429;
    double tauR = gamma/(gamma*gamma+alpha*alpha);
    double tauI = alpha/(gamma*gamma+alpha*alpha);
    double expR = exp(-fTimeBase*tauR);
    skExp = expR*cos(-fTimeBase*tauI);
    skOneMinusExp = 1-skExp;
    skExpRsinI = expR*sin(-fTimeBase*tauI);
    skAccus.fill(0.0);

    doNoiseAddition = true;
    baselineSigma = std::stod(global::config->getConfig("baselineSigma"));
    if(std::abs(baselineSigma)<0.01) doNoiseAddition = false;

    if(sModel == der::DeviceModel::kSampled) 
      doDownConvertPhotonIntervals = false;
    else
      doDownConvertPhotonIntervals = global::config->getConfig("WriteRawData")
	                             == "true" ?  true : false;
}

Digitizer::~Digitizer()
{
    /**
     * Destructor for Digitizer.
     */
}

void Digitizer::setSamplingInterval(int samplingInterval)
{
    iSamplingInterval = samplingInterval;
}

void Digitizer::doResponse(Pulse& thePulse)
{
  if (sModel == der::DeviceModel::kAnalytic)
    {
      Device::doResponse(thePulse);
    }

  if(doDownConvertPhotonIntervals){
    for(size_t i = 0; i<thePulse.getPhotonSize(); ++i){
      thePulse.setPhotonIntervalStart(i, thePulse.getPhotonIntervalAt(i).first
				      * (1.0 / (double)iSamplingInterval));
      thePulse.setPhotonIntervalEnd(i, thePulse.getPhotonIntervalAt(i).second
				    * (1.0 / (double)iSamplingInterval));
    }
  }

  size_t digitizedSize = thePulse.size() * (1.0 / (double)iSamplingInterval);
  for (size_t i = 0; i < digitizedSize; ++i)
    {
      thePulse[i] = digitizePoint(thePulse[i * iSamplingInterval], true);
    }
  thePulse.resize(digitizedSize);
}

void Digitizer::doStageResponse(Pulse& thePulse){
  size_t digitizedSize = thePulse.size() * (1.0 / (double)iSamplingInterval);
  for (size_t i = 0; i < digitizedSize; ++i)
    {
      thePulse[i] = digitizePoint(thePulse[i * iSamplingInterval], false);
    }
  thePulse.resize(digitizedSize);
}

int Digitizer::digitizePoint(double mV, bool addNoise)
{
    mV += fDCOffset;
    if(doNoiseAddition && addNoise)
      return mVtoADC(addBaselineNoise(mV));
    else
      return mVtoADC(mV);
}

int Digitizer::mVtoADC(const double& mV)
{
    /**
     * Accepts a voltage [mV] and passes it through the ADC.
     *
     * This method was originally ported from LUXSim2evt but has been improved.
     *
     *\returns This function returns an unsigned short integer [ADCC]
     */

    if (mV > fDigMax)
        return fNumBits / 2 - 1;
    if (mV <= fDigMin)
        return -fNumBits + fNumBits / 2;

    return round(fADCpermV * mV);
}

double Digitizer::addBaselineNoise(const double& mV){
    return mV + gRandom->Gaus(0, baselineSigma/fADCpermV);
}


void Digitizer::runFilters(double& sampleValue){
  sallenKey(sampleValue, skAccus, skExp, skExpRsinI, skOneMinusExp);
}


void Digitizer::reset(){
  skAccus.fill(0.0);
}
