//
//  PMT.cpp
//  devices
//
//  Created by Cees Carels on 05/01/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include <stdio.h>

#include "PMT.hpp"
#include "TStopwatch.h"
#include "Config.hpp"

std::shared_ptr<DBInterface<double>> PMT::myCSV{nullptr};

PMT::PMT(const der::DeviceModel& model)
    : iPMTNumber(0)
    , fInitialised(false)
{
    this->setName("PMT");
    sModel = model;

    //////////////////////////////////////////////////////////////////////////
    //Items that are common across all PMTs and all models

    //Parameters to do with the PMT pulse response 
    iNumberOfPhotoElectrons = 1;
    fElectricCharge = 1.6e-19; //electron charge
    fRload = 50; //Load resistance
    fRiseTime = 3.81; //time constant rising - from fit (?) THIS COULD VARY
    fDecayTime = 4.76; //time constant falling - from fit (?) THIS COULD VARY

    //Parameters for afterpulsing
    fRates = { 0.0, 0.0, 0.165, 0.165, 0, 0, 0.67, 0, 0 };
    fTimingParameters = { { 280, 15 }, { 390, 15 }, { 520, 15 },
			  { 1010, 20 }, { 1130, 30 }, { 1330, 30 },
			  { 1580, 30 }, { 2020, 30 }, { 2850, 50 } };
    fPEParameters = { { 17.5, 5.3 }, { 16.5, 5.0 }, { 13.0, 4.3 },
		      { 10.6, 3.9 }, { 8.0, 3.2 }, { 7.0, 3.0 },
		      { 4.3, 1.2 }, { 3.0, 1.9 }, { 2.0, 1.3 } };


    //Probability of different PMT responses
    if (global::config->getConfig("SecPMTEffects") == "true")
      {
        fFirstDynHitProb = 0.06 / std::stod(global::config->getConfig("BaccQEFactor"));
        fSecondDynodeCollProb = 0.01;
      }
    else
      {
        fFirstDynHitProb = 0;
        fSecondDynodeCollProb = 0;
      }

    fAftPlsProb = std::stof(global::config->getConfig("AftPlsProb"));

    //////////////////////////////////////////////////////////////////////////
    if (sModel == der::DeviceModel::kSampled)
    {
        thePulses = std::make_shared<PulseReader>();
        thePulses->setupPulseReader();
      }
    else{
      cumulativeRealTimes.fill(0.0);
      cumulativeCPUTimes.fill(0.0);
      constructBasePMTPulse(2048);
    }

}

PMT::PMT(unsigned int RealLZPMTNumber)
    : iPMTNumber(RealLZPMTNumber)
{
    /**
     * Constructor for PMT setting PMT number.
     */
}

PMT::~PMT()
{
    /**
     * Destructor for PMT.
     */
}

void PMT::setPMTNumber(unsigned int realLZPMTNumber)
{
    iPMTNumber = realLZPMTNumber;
    setPMTParameters();
    fInitialised = true;
}

void PMT::setPMTParameters()
{
    //Get PMT parameters from csv file
    //If no csv file provided, default parameters are set in RunControl.hp
	auto userGain = std::stod(global::config->getConfig("UserGain"));
    fQE = (PMT::getParamPointer()->getConfig("QE", iPMTNumber)) / std::stod(global::config->getConfig("BaccQEFactor"));
    fQE *= std::stod(global::config->getConfig("CE"));
    fTwoPheProb = PMT::getParamPointer()->getConfig("DPHE", iPMTNumber);
    fDarkCountRate = PMT::getParamPointer()->getConfig("DarkCount", iPMTNumber);
    fMultiplierGain = PMT::getParamPointer()->getConfig("Gain", iPMTNumber);
    fFirstDynHitGain = PMT::getParamPointer()->getConfig("fGain", iPMTNumber);
    fSecondDynCollGain = PMT::getParamPointer()->getConfig("sGain", iPMTNumber);
    fGainSampled = PMT::getParamPointer()->getConfig("PMTGain", iPMTNumber) / userGain;
    fFirstDynHitGainSampled = PMT::getParamPointer()->getConfig("fPMTGain", iPMTNumber) / userGain;
    fSecondDynCollGainSampled = PMT::getParamPointer()->getConfig("sPMTGain", iPMTNumber) / userGain;
    fSpheRes = PMT::getParamPointer()->getConfig("SpheRes", iPMTNumber);
    fFirstDynHitRes = PMT::getParamPointer()->getConfig("fRes", iPMTNumber);
    fSecondDynCollRes = PMT::getParamPointer()->getConfig("sRes", iPMTNumber);
    fAvgTranTime = PMT::getParamPointer()->getConfig("TransitTime", iPMTNumber);
    fTTS = PMT::getParamPointer()->getConfig("TTS", iPMTNumber);
    fFirstDynTranTime = PMT::getParamPointer()->getConfig("FTransitTime", iPMTNumber); //12.0; //[ns]
    fFirstDynTTS = PMT::getParamPointer()->getConfig("FTTS", iPMTNumber); //1.27; //[ns]
    fSecondDynTranTime = PMT::getParamPointer()->getConfig("STransitTime", iPMTNumber); //36.0; //[ns]
    fSecondDynTTS = PMT::getParamPointer()->getConfig("STTS", iPMTNumber); //3.6;  //[ns]

    //PMT base parameters for pulse shaping
    fRout = (iPMTNumber>=600 && iPMTNumber<800 ? 100000 : 50 ); //Base resistance across which signal is read out
    fR = ((fRout * fRload) / (fRout + fRload));

    //Setting the gains for each scenario
    fAnalyticGainFactor = -1.0 * 1E12 * iNumberOfPhotoElectrons * fElectricCharge * fR / (fRiseTime - fDecayTime);
    fNominalScaleGain = fAnalyticGainFactor * fMultiplierGain; // standard PMT gain
    fFirstDynodeScaleGain = fAnalyticGainFactor * fFirstDynHitGain;
    fSecondDynodeScaleGain = fAnalyticGainFactor * fSecondDynCollGain;

    //Further probabilities of PMT responses
    if (global::config->getConfig("WavelengthDepQE") == "true")
        getWavelengthDependentQE(iPMTNumber);
    else
    {
        fTwoPheProb *= fQE;
        fSecondDynodeCollProb *= fQE;
        fAftPlsProb *= fQE;
        fSpheProb = (fQE - (fSecondDynodeCollProb + fAftPlsProb + fTwoPheProb));
        fNQE = 1 - (fFirstDynHitProb + fSecondDynodeCollProb + fAftPlsProb + fSpheProb + fTwoPheProb);
        //The following is a safeguard that if QE is very close to or equal to BaccQE, nQE won't become negative
        //due to the FirstDynHitProb not being a function nor being subtracted from QE.
        //In such cases the first dynode hit probability is set to 0.
        if (fNQE < 0)
        {
            fFirstDynHitProb = 0;
            fNQE = 1 - (fFirstDynHitProb + fSecondDynodeCollProb + fAftPlsProb + fSpheProb + fTwoPheProb);
        }
        fWavelengthNQE.resize(651, fNQE);
        fWavelengthAftPlsProb.resize(651, fAftPlsProb);
        fWavelengthFirstDynHitProb.resize(651, fFirstDynHitProb);
        fWavelengthSecondDynodeCollProb.resize(651, fSecondDynodeCollProb);
        fWavelengthSPHEProb.resize(651, fSpheProb);
        fWavelengthTwoPheProb.resize(651, fTwoPheProb);
    }
}

void PMT::doResponse(Pulse& thePulse)
{
    if (thePulse.getChannel() != iPMTNumber || !fInitialised)
    {
        this->setPMTNumber(iPMTNumber);
        fInitialised = true;
    }
    if (sModel == der::DeviceModel::kSampled)
        std::cout << "NOTICE : no possible sampled reponse" << std::endl;
    else
        doAnalyticPMTResponse(thePulse);
}

void PMT::doResponse(Pulse& theLGPulse, Pulse& theHGPulse)
{
    if (theLGPulse.getChannel() != iPMTNumber || !fInitialised)
    {
        this->setPMTNumber(iPMTNumber);
        setPMTParameters();
        fInitialised = true;
    }

    if (sModel == der::DeviceModel::kSampled){
        theLGPulse.resize(theHGPulse.size());
        theLGPulse.setChannel(theHGPulse.getChannel());
        theLGPulse.setEvent(theHGPulse.getEvent());
        theLGPulse.setLUXSimEvtNum(0, theHGPulse.getLUXSimEvtNum(2,0));
        doSampledPMTResponse(theLGPulse, theHGPulse);
    }
    else
    {
        doAnalyticPMTResponse(theLGPulse);
        doAnalyticPMTResponse(theHGPulse);
    }
}

void PMT::doAnalyticPMTResponse(Pulse& thePulse)
{
    /**
     * Calculate the PMT response analytically.
     *
     * This includes QE, the single- and double-photoelectron response,
     * dark counts, and after pulsing.
     */

    TStopwatch timer;
    timer.Start();

    //resetPMTVectors();

    //Set parameters
    //setPMTParameters();
    fDigConvFactor = 1;

    //Decide on response: QE, DarkCounts, Afterpulsing
    //determineDarkCounts(thePulse.size()); Now done at input stage
    // doPhConvAndDynCollWithAP(thePulse);
    timer.Stop();
    cumulativeRealTimes[0] += timer.RealTime();
    cumulativeCPUTimes[0] += timer.CpuTime();
    timer.Start();
    // makeListsWithTransitTimes();
    timer.Stop();
    cumulativeRealTimes[1] += timer.RealTime();
    cumulativeCPUTimes[1] += timer.CpuTime();
    timer.Start();

    unsigned long Nhalf = fPmtPulseSamples.size() * 0.5;

    //Don't bother looping over the part of the lowPassFilter
    //waveform where there is no response
    unsigned long startPoint = Nhalf+1;
    unsigned long endPoint = Nhalf+1;
    for(unsigned long i = 0; i<fPmtPulseSamples.size(); ++i){
      if(fPmtPulseSamples[i] != 0){
	startPoint = i;
	break;
      }
    }
    for(unsigned long i = startPoint; i<fPmtPulseSamples.size(); ++i){
      if(std::abs(fPmtPulseSamples[i])<1e-8){
	endPoint = i;
	break;
      }
    }
    timer.Stop();
    cumulativeRealTimes[2] += timer.RealTime();
    cumulativeCPUTimes[2] += timer.CpuTime();

    timer.Start();
    //Go to each photon and add response
    for (int i = 0; i < IdxList.size(); i++)
    {
        double initAmp = getGaussSpread(1, fSpheRes);
        double DoublePheAmp = 0.0;
        bool is2Phe = IdxList[i]->is2Phe;
        if (is2Phe)
        {
            DoublePheAmp = getGaussSpread(1, fSpheRes); //30% variation.
        }

        double GausRespAmp = (initAmp + DoublePheAmp) * fNominalScaleGain;

	unsigned long startSample = startPoint + (IdxList[i]->idx) - Nhalf;
	unsigned long endSample = endPoint - 1 + (IdxList[i]->idx) - Nhalf;

	thePulse.addPhotonInterval(startSample, endSample);

        //Scale PMT pulse, then assign to main pulse

        for (unsigned long j = startPoint; j < endPoint; j++)
        {
            thePulse[j + IdxList[i]->idx - Nhalf] +=
	      GausRespAmp * fPmtPulseSamples[j];
        }
    }
    timer.Stop();
    cumulativeRealTimes[3] += timer.RealTime();
    cumulativeCPUTimes[3] += timer.CpuTime();
    timer.Start();
    for (int i = 0; i < FirstDyn.size(); i++)
    {
        double initAmp = getGaussSpread(1, fFirstDynHitRes);

        double GausRespAmp = initAmp * fFirstDynodeScaleGain;

	unsigned long startSample = startPoint + (FirstDyn[i]->idx) - Nhalf;
	unsigned long endSample = endPoint - 1 + (FirstDyn[i]->idx) - Nhalf;

	thePulse.addPhotonInterval(startSample, endSample);

        //Scale PMT pulse, then assign to main pulse
        for (unsigned long j = startPoint; j < endPoint; j++)
        {
	  thePulse[j + FirstDyn[i]->idx - Nhalf] += 
	    GausRespAmp * fPmtPulseSamples[j];
        }
    }
    cumulativeRealTimes[4] += timer.RealTime();
    cumulativeCPUTimes[4] += timer.CpuTime();
    timer.Stop();
    timer.Start();
    for (int i = 0; i < SecondDyn.size(); i++)
    {
        double initAmp = getGaussSpread(1, fSecondDynCollRes);

        double GausRespAmp = initAmp * fSecondDynodeScaleGain;

	unsigned long startSample = startPoint + (SecondDyn[i]->idx) - Nhalf;
	unsigned long endSample = endPoint - 1 + (SecondDyn[i]->idx) - Nhalf;

	thePulse.addPhotonInterval(startSample, endSample);

        //Scale PMT pulse, then assign to main pulse
        for (unsigned long j = startPoint; j < endPoint; j++)
        {
            thePulse[j + SecondDyn[i]->idx - Nhalf]  +=
	      GausRespAmp * fPmtPulseSamples[j];
        }
    }
    timer.Stop();
    cumulativeRealTimes[5] += timer.RealTime();
    cumulativeCPUTimes[5] += timer.CpuTime();
    timer.Start();
    for (int i = 0; i < DarkList.size(); i++)
    {
        double initAmp = getGaussSpread(1, fSpheRes);
        double GausRespAmp = initAmp * fNominalScaleGain;

	unsigned long startSample = startPoint + (DarkList[i]->idx) - Nhalf;
	unsigned long endSample = endPoint - 1 + (DarkList[i]->idx) - Nhalf;

	thePulse.addPhotonInterval(startSample, endSample);

        //Scale PMT pulse, then assign to main pulse
        for (unsigned long j = startPoint; j < endPoint; j++)
        {
            thePulse[j + DarkList[i]->idx - Nhalf] += 
	      GausRespAmp * fPmtPulseSamples[j];
        }
    }
    timer.Stop();
    cumulativeRealTimes[6] += timer.RealTime();
    cumulativeCPUTimes[6] += timer.CpuTime();
    timer.Start();
    for (int i = 0; i < AftPlsList.size(); i++)
    {
        double initAmp = 0.0;
        for (int j = 0; j < AftPlsList[i]->AftPlsNPE; j++)
        {
            initAmp += getGaussSpread(1, fSpheRes);
        }

        double GausRespAmp = initAmp * fNominalScaleGain;

	unsigned long startSample = startPoint + (AftPlsList[i]->idx) - Nhalf;
	unsigned long endSample = endPoint - 1 + (AftPlsList[i]->idx) - Nhalf;

	thePulse.addPhotonInterval(startSample, endSample);

        //Scale PMT pulse, then assign to main pulse
        for (unsigned long j = startPoint; j < endPoint; j++)
        {
            thePulse[j + AftPlsList[i]->idx - Nhalf] +=
	      GausRespAmp * fPmtPulseSamples[j];
        }
    }
    thePulse.sortPhotonIntervals();
    timer.Stop();
    cumulativeRealTimes[7] += timer.RealTime();
    cumulativeCPUTimes[7] += timer.CpuTime();
    //resetPMTVectors();
}

void PMT::doSampledPMTResponse(Pulse& thePulse,
    Pulse& thePulseHG)
{

    /**
     * PMT Response function. The function uses the PulseReader object supplied
     * by sampling it randomly for waveform data that it contains.
     *
     * These waveforms are then used in the PMT response function, instead
     * of calculating waveforms analytically.
     */

    //resetPMTVectors();

    //Set parameters
    //setPMTParameters();
    fDigConvFactor = 1.0 / (std::stof(global::config->getConfig("SmplRate")));

    //Decide on response: QE, DarkCounts, Afterpulsing
    //determineDarkCounts(thePulse.size()); now done at input stage
    // doPhConvAndDynCollWithAP(thePulse);
    //makeLists();

    //Go to each photon and add response
    for (int i = 0; i < IdxList.size(); i++)
    {
        //Choose a random index for the waveform
        //Need to add seed.
        int ran = rand() % (thePulses->LESize());
        int ran2 = 0; //Random number for the second phe if warranted
        bool is2Phe = IdxList[i]->is2Phe; //Boolean for whether it is a 2 phe hit

        if (is2Phe)
        {
            ran2 = rand() % (thePulses->LESize()); //get a second pulse to add
        }

        //Get +/- idx bounds for response
        //These sampled pulses are assumed to be digitized at 10 ns
        unsigned long long low = (IdxList[i]->idx) * fDigConvFactor
            - (thePulses->getPulseLE(ran)->size()) * 0.5;
        unsigned long long high = (IdxList[i]->idx) * fDigConvFactor
            + (thePulses->getPulseLE(ran)->size()) * 0.5;
        (low > 0) ? low : low = 0;
        (high < thePulse.size()) ? high : (high = thePulse.size());

	thePulse.addPhotonInterval(low, high);
	thePulseHG.addPhotonInterval(low, high);

        int cntr = 0;
        int PlsSmpl = 0;
        for (unsigned long long j = low; j < high; j++)
        {
            //...from start to finish in the subset
            double RespNew = thePulses->getPulseHE(ran)->at(PlsSmpl) * fGainSampled;
            double RespNewHG = thePulses->getPulseLE(ran)->at(PlsSmpl) * fGainSampled;
            if (is2Phe)
            {
                RespNew = RespNew + thePulses->getPulseHE(ran2)->at(PlsSmpl) * fGainSampled;
                RespNewHG = RespNewHG + thePulses->getPulseLE(ran2)->at(PlsSmpl) * fGainSampled;
            }
            ++PlsSmpl;
            thePulse[j] += RespNew;
            thePulseHG[j] += RespNewHG;
            ++cntr;
        }
    }

    for (int i = 0; i < FirstDyn.size(); i++)
    {
        int ran = rand() % (thePulses->LESize());

        //Get +/- idx bounds for response
        //These pulses are assumed to be digitised at 10 ns
        unsigned long long low = (FirstDyn[i]->idx) * fDigConvFactor - (thePulses->getPulseLE(ran)->size()) / 2;
        unsigned long long high = (FirstDyn[i]->idx) * fDigConvFactor + (thePulses->getPulseLE(ran)->size()) / 2;
        (low > 0) ? low : low = 0;
        (high < thePulse.size()) ? high : (high = thePulse.size());

	thePulse.addPhotonInterval(low, high);
	thePulseHG.addPhotonInterval(low, high);

        int cntr = 0;
        int PlsSmpl = 0;
        for (unsigned long long j = low; j < high; j++)
        {
            double RespNew = thePulses->getPulseHE(ran)->at(PlsSmpl) * fFirstDynHitGainSampled;
            double RespNewHG = thePulses->getPulseLE(ran)->at(PlsSmpl) * fSecondDynCollGainSampled;
            ++PlsSmpl;
            thePulse[j] += RespNew;
            thePulseHG[j] += RespNewHG;
            ++cntr;
        }
    }

    for (int i = 0; i < SecondDyn.size(); i++)
    {
        //Choose a random index for the waveform
        //Need to add seed.
        int ran = rand() % (thePulses->LESize());

        //Get +/- idx bounds for response
        //These pulses are assumed to be digitised at 10 ns
        unsigned long long low = (SecondDyn[i]->idx) * fDigConvFactor - (thePulses->getPulseLE(ran)->size()) / 2;
        unsigned long long high = (SecondDyn[i]->idx) * fDigConvFactor + (thePulses->getPulseLE(ran)->size()) / 2;
        (low > 0) ? low : low = 0;
        (high < thePulse.size()) ? high : (high = thePulse.size());

	thePulse.addPhotonInterval(low, high);
	thePulseHG.addPhotonInterval(low, high);

        int cntr = 0;
        int PlsSmpl = 0;
        for (unsigned long long j = low; j < high; j++)
        {
            double RespNew = thePulses->getPulseHE(ran)->at(PlsSmpl) * fSecondDynCollGainSampled;
            double RespNewHG = thePulses->getPulseLE(ran)->at(PlsSmpl) * fSecondDynCollGainSampled;
            ++PlsSmpl;
            thePulse[j] += RespNew;
            thePulseHG[j] += RespNewHG;
            ++cntr;
        }
    }

    //Dark counts
    for (int i = 0; i < DarkList.size(); i++)
    {
        //Choose a random index for the waveform
        //Need to add seed.
        int ran = rand() % (thePulses->LESize());

        //Get +/- idx bounds for response
        //These pulses are assumed to be digitised at 10 ns
        unsigned long long low = (DarkList[i]->idx) * fDigConvFactor - (thePulses->getPulseLE(ran)->size()) / 2;
        unsigned long long high = (DarkList[i]->idx) * fDigConvFactor + (thePulses->getPulseLE(ran)->size()) / 2;
        (low > 0) ? low : low = 0;
        (high < thePulse.size()) ? high : (high = thePulse.size());

	thePulse.addPhotonInterval(low, high);
	thePulseHG.addPhotonInterval(low, high);

        int cntr = 0;
        int PlsSmpl = 0;
        for (unsigned long long j = low; j < high; j++)
        {
            double RespNew = thePulses->getPulseHE(ran)->at(PlsSmpl) * fGainSampled;
            double RespNewHG = thePulses->getPulseLE(ran)->at(PlsSmpl) * fGainSampled;
            ++PlsSmpl;
            thePulse[j] += RespNew;
            thePulseHG[j] += RespNewHG;
            ++cntr;
        }
    }

    //Afterpulses
    for (int i = 0; i < AftPlsList.size(); i++)
    {
        //Choose a random index for the waveform
        //Need to add seed.
        int ran = rand() % (thePulses->LESize());

        //Get +/- idx bounds for response
        //These pulses are assumed to be digitised at 10 ns
        unsigned long long low = (AftPlsList[i]->idx) * fDigConvFactor - (thePulses->getPulseLE(ran)->size()) / 2;
        unsigned long long high = (AftPlsList[i]->idx) * fDigConvFactor + (thePulses->getPulseLE(ran)->size()) / 2;
        (low > 0) ? low : low = 0;
        (high < thePulse.size()) ? high : (high = thePulse.size());

	thePulse.addPhotonInterval(low, high);
	thePulseHG.addPhotonInterval(low, high);

        int cntr = 0;
        int PlsSmpl = 0;
        for (unsigned long long j = low; j < high; j++)
        {
            double RespNew = thePulses->getPulseHE(ran)->at(PlsSmpl) * fGainSampled;
            double RespNewHG = thePulses->getPulseLE(ran)->at(PlsSmpl) * fGainSampled;
            ++PlsSmpl;
            thePulse[j] += RespNew;
            thePulseHG[j] += RespNewHG;
            ++cntr;
        }
    }
    // resetPMTVectors();
}

void PMT::getWavelengthDependentQE(unsigned int PMTNumber)
{
    /**
     * Calculate wavelength dependent probabilities for
     * PMT responses. This is executed once per channel per
     * event.
     */

    double fWavelengthQE = 0;
    fWavelengthNQE.resize(651);
    fWavelengthAftPlsProb.resize(651);
    fWavelengthFirstDynHitProb.resize(651);
    fWavelengthSecondDynodeCollProb.resize(651);
    fWavelengthSPHEProb.resize(651);
    fWavelengthTwoPheProb.resize(651);

    //TPC PMTs (R11410)
    if (PMTNumber < 600)
    {
        fWavelengthQE = fQE;
        for (int i = 0; i < 651; i++)
        {
            fWavelengthAftPlsProb[i] = fAftPlsProb * fWavelengthQE;
            fWavelengthSecondDynodeCollProb[i] = 0.01 * fWavelengthQE;
            fWavelengthTwoPheProb[i] = fTwoPheProb * fWavelengthQE;
            fWavelengthSPHEProb[i] = fWavelengthQE - (fWavelengthSecondDynodeCollProb[i] + fWavelengthAftPlsProb[i] + fWavelengthTwoPheProb[i]);
            fWavelengthFirstDynHitProb[i] = fFirstDynHitProb;
            fWavelengthNQE[i] = 1 - (fWavelengthFirstDynHitProb[i] + fWavelengthSecondDynodeCollProb[i] + fWavelengthAftPlsProb[i] + fWavelengthTwoPheProb[i] + fWavelengthSPHEProb[i]);
            if (fWavelengthNQE[i] < 0)
            {
                fWavelengthFirstDynHitProb[i] = 0;
                fWavelengthNQE[i] = 1 - (fWavelengthFirstDynHitProb[i] + fWavelengthSecondDynodeCollProb[i] + fWavelengthAftPlsProb[i] + fWavelengthTwoPheProb[i] + fWavelengthSPHEProb[i]);
            }
        }
    }

    //Skin PMTs ()
    else if (PMTNumber < 800)
    {
        fWavelengthQE = fQE;
        for (int i = 0; i < 651; i++)
        {
            fWavelengthAftPlsProb[i] = fAftPlsProb * fWavelengthQE;
            fWavelengthSecondDynodeCollProb[i] = 0.01 * fWavelengthQE;
            fWavelengthTwoPheProb[i] = fTwoPheProb * fWavelengthQE;
            fWavelengthSPHEProb[i] = fWavelengthQE - (fWavelengthSecondDynodeCollProb[i] + fWavelengthAftPlsProb[i] + fWavelengthTwoPheProb[i]);
            fWavelengthFirstDynHitProb[i] = fFirstDynHitProb;
            fWavelengthNQE[i] = 1 - (fWavelengthFirstDynHitProb[i] + fWavelengthSecondDynodeCollProb[i] + fWavelengthAftPlsProb[i] + fWavelengthTwoPheProb[i] + fWavelengthSPHEProb[i]);
            if (fWavelengthNQE[i] < 0)
            {
                fWavelengthFirstDynHitProb[i] = 0;
                fWavelengthNQE[i] = 1 - (fWavelengthFirstDynHitProb[i] + fWavelengthSecondDynodeCollProb[i] + fWavelengthAftPlsProb[i] + fWavelengthTwoPheProb[i] + fWavelengthSPHEProb[i]);
            }
        }
    }

    //OD PMTs (R5950)
    else if (PMTNumber >= 800)
    {
        //rescale QE according to wavelength
        //OD code from Sally.
        for (int i = 0; i < 650; ++i)
        {
            if (i < 290 || i > 620)
                fWavelengthQE = 0;
            else
            { //Cubic poly. fit to digitized plot from Hamamatsu datasheet.
                //Not perfect... but conservatively so.
                fWavelengthQE = (-366.1 + 2.402 * i
                                    - .004742 * pow(i, 2)
                                    + .000002940 * pow(i, 3))
                    * 0.01 / 0.2381966 * fQE;
                //replaces 0.01 factor with 0.01/0.2381966
            }
            fWavelengthAftPlsProb[i] = fAftPlsProb * fWavelengthQE;
            fWavelengthSecondDynodeCollProb[i] = 0.01 * fWavelengthQE;
            fWavelengthTwoPheProb[i] = fTwoPheProb * fWavelengthQE;
            fWavelengthSPHEProb[i] = fWavelengthQE - (fWavelengthSecondDynodeCollProb[i] + fWavelengthAftPlsProb[i] + fWavelengthTwoPheProb[i]);
            fWavelengthFirstDynHitProb[i] = fFirstDynHitProb;
            fWavelengthNQE[i] = 1 - (fWavelengthFirstDynHitProb[i] + fWavelengthSecondDynodeCollProb[i] + fWavelengthAftPlsProb[i] + fWavelengthTwoPheProb[i] + fWavelengthSPHEProb[i]);
            if (fWavelengthNQE[i] < 0)
            {
                fWavelengthFirstDynHitProb[i] = 0;
                fWavelengthNQE[i] = 1 - (fWavelengthFirstDynHitProb[i] + fWavelengthSecondDynodeCollProb[i] + fWavelengthAftPlsProb[i] + fWavelengthTwoPheProb[i] + fWavelengthSPHEProb[i]);
            }
        }
    }
}

inline PMT::photonResp PMT::biasedDieWithAP(const double wavelength)
{
    /**
     * Throw a biased die that is biased at the appropriate PMT
     * quantities: the spheProb, TwoPheProb, FirstDynHitProb,
     * SecondDynodeCollProb, and AftPlsProb.
     * This method uses QE wavelength dependence.
     *
     * Returns an unsigned short integer corresponding to the
     * outcome of the biasedDie (values 0-5).
     */

    double p1(0), p2(0), p3(0), p4(0), p5(0);
    int idx = round(wavelength);
    if (idx > 650)
    {
        p1 = 1, p2 = 0, p3 = 0, p4 = 0, p5 = 0;
    }
    else
    {
        p1 = fWavelengthNQE[idx], p2 = fWavelengthSPHEProb[idx],
        p3 = fWavelengthTwoPheProb[idx], p4 = fWavelengthFirstDynHitProb[idx],
        p5 = fWavelengthAftPlsProb[idx];
    }

    double pr1 = p1, pr2 = pr1 + p2, pr3 = pr2 + p3, pr4 = pr3 + p4, pr5 = pr4 + p5;
    double ran = (double)((double)rand() / (double)RAND_MAX);

    if (ran <= pr1)
        return photonResp::kKill;
    else if (ran >= pr1 && ran <= pr2)
        return photonResp::kSphe;
    else if (ran >= pr2 && ran <= pr3)
        return photonResp::kDphe;
    else if (ran >= pr3 && ran <= pr4)
        return photonResp::kFirstDynHit;
    else if (ran >= pr4 && ran <= pr5)
        return photonResp::kAftPulse;
    else
        return photonResp::kSecDynColl;
}

void PMT::determineDarkCounts(const unsigned long Length)
{
    /**
     * Method to generate Dark counts
     * Method has now been moved to RootInputMar2018
     * Leaving this version of the method here for now.
     */
    //Add dark counts from photocathode
    unsigned long long seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator((unsigned int)seed);
    std::poisson_distribution<int> distribution(fDarkCountRate * Length / (1E9 * fDigConvFactor));
    iNumDarkCount = distribution(generator);
}

unsigned short PMT::getCaseNumber()
{
    /**
 * Throw a biased die that is biased at the appropriate Ion
 * probabilities.
 *
 * Returns an integer corresponding to the outcome of the biasedDie (0-7).
 */
    double p0 = fRates[0];
    double p1 = p0 + fRates[1];
    double p2 = p1 + fRates[2];
    double p3 = p2 + fRates[3];
    double p4 = p3 + fRates[4];
    double p5 = p4 + fRates[5];
    double p6 = p5 + fRates[6];
    double p7 = p6 + fRates[7];
    double ran = (double)((double)rand() / (double)RAND_MAX);

    if (ran <= p0)
        return 0;
    if (ran > p0 && ran <= p1)
        return 1;
    if (ran > p1 && ran <= p2)
        return 2;
    if (ran > p2 && ran <= p3)
        return 3;
    if (ran > p3 && ran <= p4)
        return 4;
    if (ran > p4 && ran <= p5)
        return 5;
    if (ran > p5 && ran <= p6)
        return 6;
    if (ran > p6 && ran <= p7)
        return 7;
    else
        return 8;
}

unsigned long long PMT::getTiming(const unsigned short CaseNumber)
{
    unsigned long long result = (unsigned long long)getGaussSpread(fTimingParameters[CaseNumber][0],
        fTimingParameters[CaseNumber][1]);
    return result;
}

unsigned short PMT::getNumPE(const unsigned short CaseNumber)
{
    unsigned short result = (unsigned short)getGaussSpread(fPEParameters[CaseNumber][0],
        fPEParameters[CaseNumber][1]);
    return result;
}

inline double PMT::getGaussSpread(const double mean, const double sigma)
{
    /**
     * Method that returns a Gaussian spread, where the return value is strictly non-zero.
     *
     * Returns a non-zero Gaussian number spread according to the mean and sigma provided.
     */

    //TRandom *r2 = new TRandom2(0);

    double value;
    int cntr = 0;
    do
    {
        value = gRandom->Gaus(mean, sigma);
        ++ cntr;
    } while (value < 0.006 && cntr < 3);

    if(value < 0.006) value = 0.006;
    return value;
}

void PMT::prepareMCTruth(std::shared_ptr<MCTruth> theTruth)
{
  if (theTruth != nullptr)
    {
      theTruth->setMCTruthSize(IdxList.size() + FirstDyn.size() + 
			       SecondDyn.size() + AftPlsList.size()
			       + DarkList.size());
      size_t photonCounter = 0;
      makeTruthFromPhotonTimes(theTruth, IdxList, photonCounter);
      makeTruthFromPhotonTimes(theTruth, FirstDyn, photonCounter);
      makeTruthFromPhotonTimes(theTruth, SecondDyn, photonCounter);
      makeTruthFromPhotonTimes(theTruth, AftPlsList, photonCounter);
      makeTruthFromPhotonTimes(theTruth, DarkList, photonCounter);
    }
}

void PMT::makeTruthFromPhotonTimes(std::shared_ptr<MCTruth> theTruth,
				   PhotonInfos& photonList, 
				   size_t& photonCounter)
{
    /**
     * Add truth information from the PMT stage.
     *
     * If a binning other than the default of 1 ns is used,
     * then the index will not simply be the time of the photon.
     *
     * In this case, the method will need to be updated.
     */
  
     for (int i = 0; i < photonList.size(); i++)
       {
            theTruth->addDERTruth(i+photonCounter,
                iPMTNumber,
                (unsigned short int)photonList[i]->pheType,
                photonList[i]->isDER,
                photonList[i]->BaccEvtNum,
                photonList[i]->idx,
                photonList[i]->wavelength,
                photonList[i]->interactionID,
                photonList[i]->vertexNum,
                photonList[i]->pulseID);
       }
     photonCounter += photonList.size();
}

void PMT::resetPMTVectors()
{
    IdxList.clear();
    FirstDyn.clear();
    SecondDyn.clear();
    AftPlsList.clear();
    DarkList.clear();
}

void PMT::printRunningTime(){
  double cumulativeRealTime = 0;
  double cumulativeCPUTime = 0;
  for(int i = 0; i<8; ++i){
    cumulativeRealTime += cumulativeRealTimes[i];
    cumulativeCPUTime += cumulativeCPUTimes[i];
  }
  std::cout << "\nPMT Total Times: " << std::endl;
  std::cout << std::left << std::setw(17) << "Conversion: " << std::setw(12)
	    << cumulativeRealTimes[0] << std::setw(8)
	    << cumulativeCPUTimes[0] << std::endl;
  std::cout << std::left << std::setw(17) << "Transit List: "
	    << std::setw(12) << cumulativeRealTimes[1]
	    << std::setw(8) << cumulativeCPUTimes[1] << std::endl;
  std::cout << std::left << std::setw(17) << "Base Pulse: " << std::setw(12)
	    << cumulativeRealTimes[2] << std::setw(8)
	    << cumulativeCPUTimes[2] << std::endl;
  std::cout << std::left << std::setw(17) << "Index List: " << std::setw(12)
	    << cumulativeRealTimes[3] << std::setw(8)
	    << cumulativeCPUTimes[3] << std::endl;
  std::cout << std::left << std::setw(17) << "First Dynode: " << std::setw(12)
	    << cumulativeRealTimes[4] << std::setw(8)
	    << cumulativeCPUTimes[4] << std::endl;
  std::cout << std::left << std::setw(17) << "Second Dynode: " << std::setw(12)
	    << cumulativeRealTimes[5] << std::setw(8)
	    << cumulativeCPUTimes[5] << std::endl;
  std::cout << std::left << std::setw(17) << "Dark List: " << std::setw(12)
	    << cumulativeRealTimes[6] << std::setw(8)
	    << cumulativeCPUTimes[6] << std::endl;
  std::cout << std::left << std::setw(17) << "Afterpulse List: " << std::setw(12)
	    << cumulativeRealTimes[7] << std::setw(8)
	    << cumulativeCPUTimes[7] << std::endl;
  std::cout << std::left << std::setw(17) << "Full: " << std::setw(12)
	    << cumulativeRealTime << std::setw(8)
	    << cumulativeCPUTime << "\n" << std::endl;
}


void PMT::constructBasePMTPulse(const unsigned long N)
{
    //NEED TO EXPLAIN WHAT'S GOING ON IN HERE
    //Heavisde step function???
    std::vector<double> fStep(N,0);
    for (int j = 0; j <= N*0.5; j++){
	fStep[j] = 1.0;
    }

    /**
     * These pulse boundaries are guaranteed by PulseManager
     * and the TimeShift, which ensures that all pulses have
     * have sufficient pre- and post- padding.
     *
     * Do not change the value of N as it will cause undefined behaviour
     * at the pulse boundary.
     */
    fPmtPulseSamples.resize(N,0);
    std::array<double, 2> accus = {0,0};
    double samplingInterval = 1.0; //this is governed by precision of BACCARAT photon info
    std::array<double, 4> coeffs;
    coeffs[0] = exp(-samplingInterval / fDecayTime);
    coeffs[1] = 1 - coeffs[0];
    coeffs[2] = exp(-samplingInterval / fRiseTime);
    coeffs[3] = 1 - coeffs[2];

    for (int i = 0; i < N; i++)
      {
	accus[0] = (coeffs[0] * accus[0] - coeffs[1] * fStep[i]);
	accus[1] = (coeffs[2] * accus[1] - coeffs[3] * fStep[i]);
	fPmtPulseSamples[i] = accus[0] - accus[1];
	if (fPmtPulseSamples[i] > 0)
	  fPmtPulseSamples[i] = 0;
      }
}

void PMT::assignPhotonToList(std::shared_ptr<TimesAndPheResp> photon, unsigned long eventLength)
{
  photon->passQE = true;
  photon->AftPlsIon = 0;
  photon->AftPlsNPE = 0;
  photon->pheType = biasedDieWithAP(photon->wavelength);

  switch(photon->pheType){
  case photonResp::kKill :
     photon->passQE = false;
     break;
  case photonResp::kSphe :
    IdxList.push_back(photon);
    break;
  case photonResp::kDphe :
    IdxList.push_back(photon);
    photon->is2Phe = true;
    break;
  case photonResp::kFirstDynHit :
    FirstDyn.push_back(photon);
    break;
  case photonResp::kSecDynColl :
    SecondDyn.push_back(photon);
    break;
  case photonResp::kAftPulse :
    photon->pheType = photonResp::kSphe;
    IdxList.push_back(photon);
    unsigned short num = getCaseNumber();
    unsigned long long timing = photon->idx + getTiming(num);
    if (timing < (((unsigned long long)eventLength - 2048 - 1) + 1024))
      {
	unsigned short noOfAfterPulsePhotons = getNumPE(num);
	photon->AftPlsNPE = noOfAfterPulsePhotons;
	for(size_t k = 0; k<noOfAfterPulsePhotons; ++k){
	  std::shared_ptr<TimesAndPheResp> afterPulsePhoton(new TimesAndPheResp());
	  afterPulsePhoton->wavelength = 0.0;    
	  afterPulsePhoton->idx = timing; 
	  afterPulsePhoton->cathodeTime = timing;
	  afterPulsePhoton->isDER = true;
	  afterPulsePhoton->is2Phe = false;
	  afterPulsePhoton->passQE = false;
	  afterPulsePhoton->pheType = photonResp::kAftPulse;
	  afterPulsePhoton->BaccEvtNum = photon->BaccEvtNum;
	  afterPulsePhoton->interactionID = photon->interactionID;
	  afterPulsePhoton->vertexNum = photon->vertexNum;
	  afterPulsePhoton->pulseID = photon->pulseID;
	  afterPulsePhoton->AftPlsIon = num;
	  afterPulsePhoton->AftPlsNPE = 0; //is this correct?
	  AftPlsList.push_back(afterPulsePhoton);
	}
      }
    break;
  }
}

void PMT::generateDarkCounts(int event, int noOfDarkCounts, unsigned long eventLength){
  for(size_t i = 0; i<noOfDarkCounts; ++i){
    {
        unsigned long long idx    = (rand()%((unsigned long long)eventLength-2048-1))+1024;
	std::shared_ptr<TimesAndPheResp> thePhoton = std::make_shared<TimesAndPheResp>();
        thePhoton->idx = idx / fDigConvFactor;
        thePhoton->cathodeTime = idx / fDigConvFactor;
        thePhoton->wavelength = 0.0;
        thePhoton->isDER = true;
        thePhoton->pheType = photonResp::kDarkCount;
        thePhoton->is2Phe = false;
        thePhoton->passQE = false;
        thePhoton->BaccEvtNum = event;
        thePhoton->interactionID = 0;
        thePhoton->vertexNum = -1;
        thePhoton->AftPlsIon = 0;
        thePhoton->AftPlsNPE = 0;
        thePhoton->pulseID = 0;
	DarkList.push_back(thePhoton);
    }
  }
}
