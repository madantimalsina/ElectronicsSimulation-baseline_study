//
//  PMT.hpp
//  devices
//
//  Created by Cees Carels on 06/07/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef PMT_hpp
#define PMT_hpp

#include <stdio.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <random>
#include <stdio.h>

#include "TRandom.h"

#include "Device.hpp"
#include "Pulse.hpp"
#include "PulseReader.hpp"

#include "MCTruth.hpp"

/**
 * Base class providing interface for
 * photomultiplier tube implementations.
 */

class PMT : public Device
{
public:

   enum class photonResp : unsigned short int
    {
        kKill = 0,
        kSphe,
        kDphe,
        kFirstDynHit,
        kSecDynColl,
        kDarkCount,
        kAftPulse
    };

    struct TimesAndPheResp
    {
        unsigned long long idx;
        unsigned long long cathodeTime;
        unsigned long long BaccEvtNum;
        double wavelength;
        photonResp pheType;
        bool isDER;
        bool is2Phe;
        bool passQE;
        unsigned short int interactionID;
        short int vertexNum;
        unsigned short pulseID;

        //Afterpulse effects
        unsigned short AftPlsNPE; //0 if no afterpulse, else the number of phe
        unsigned short AftPlsIon; //Time delay of afterpulsed phe w.r.t ph
        unsigned long AftPlsDly; //Time delay of afterpulsed phe w.r.t ph
    };

    typedef std::vector<std::shared_ptr<TimesAndPheResp>> PhotonInfos;

    PMT(const der::DeviceModel& model);
    PMT(unsigned int RealLZPMTNumber);
    ~PMT();

    void setPMTNumber(unsigned int realLZPMTNumber);
    void doResponse(Pulse& thePulse);
    void doResponse(Pulse& theLGPulse, Pulse& theHGPulse);
    void prepareMCTruth(std::shared_ptr<MCTruth> theTruth);
    void resetPMTVectors();
    void printRunningTime();

  void assignPhotonToList(std::shared_ptr<TimesAndPheResp> photon, unsigned long eventLength);
  void generateDarkCounts(int event, int noOfDarkCounts, unsigned long eventLength);

    static void setParamPointer(std::shared_ptr<DBInterface<double>> ptr)
    {
        myCSV = ptr;
    }

    static std::shared_ptr<DBInterface<double>> getParamPointer()
    {
        return myCSV;
    }

private:
    std::shared_ptr<PulseReader> thePulses;
    unsigned long long iBaccEvtNum;
    unsigned int iPMTNumber;
    bool fInitialised;

    std::array<double,8> cumulativeRealTimes;
    std::array<double,8> cumulativeCPUTimes;

    //Probabilities and rates of different response types
    double fQE; //!< Prob. photon survives QE
    double fNQE; //!< Prob. photon does not induce response
    double fSpheProb; //!< Prob. sphe following ideal path
    double fTwoPheProb; //!< Prob. 2Phe response
    double fFirstDynHitProb; //!< Prob. first dynode photon conv
    double fSecondDynodeCollProb; //!< Prob. phe collected on sec dynode
    double fAftPlsProb; //!< Prob. for afterpulsing
    double fDarkCountRate; //!< [Hz] of dark counts in PMT
    unsigned long long iNumDarkCount; //!< Number of dark counts in PMT.

    std::vector<double> fWavelengthNQE; //!< Energy dep. prob. photon does not induce response
    std::vector<double> fWavelengthSPHEProb; //!< Engrgy dep. prob. sphe following ideal path
    std::vector<double> fWavelengthTwoPheProb; //!< Energy dep. prob. 2Phe response
    std::vector<double> fWavelengthFirstDynHitProb; //!< Energy dep. prob. first dynode photon conv
    std::vector<double> fWavelengthSecondDynodeCollProb; //!< Energy dep. prob. phe collected on sec dynode
    std::vector<double> fWavelengthAftPlsProb; //!< Energy dep. prob. for afterpulsing.

    //Transit time variables
    double fAvgTranTime; //!< [ns] mean transit times
    double fTTS; //!< [ns] stdev of transit time distribution
    double fFirstDynTranTime; //!< [ns] first dynode transit time
    double fFirstDynTTS; //!< [ns] first dynode stdev of transit time distirbution
    double fSecondDynTranTime; //!< [ns] second dynode collection transit time
    double fSecondDynTTS; //!< [ns] second dynode collection stdev of transit time distribution

    //Gain, resistances and time constants
    unsigned int iNumberOfPhotoElectrons; //!< [N] Number of photoelectrons
    double fElectricCharge; //!< [e] Electric charge
    double fMultiplierGain; //!< [#] Multiplication gain
    double fSpheRes; //!< [%] SPHE Resolution
    double fFirstDynHitGain; //!< [#] 1st dynode phe gain
    double fFirstDynHitRes; //!< [%] 1st dynode hit response resolution
    double fSecondDynCollGain; //!< [#] 2nd dynode phe collection gain
    double fSecondDynCollRes; //!< [%] 2nd dynode hit
    double fRout; //!< [Ohm] Output resistance
    double fRload; //!< [Ohm] Load resistance
    double fRiseTime; //!< [ns] Rise time
    double fDecayTime; //!< [ns] Decay time
    double fR; //!< [Ohm] Resistance

    //PMT pulse parameters
    std::vector<double> fPmtPulseSamples;

    //Scale gains for analytic PMT response
    double fAnalyticGainFactor;
    double fNominalScaleGain;
    double fFirstDynodeScaleGain;
    double fSecondDynodeScaleGain;

    //Scale gains and factors for sampled PMT response
    double fDigConvFactor; //!< Factor used for rescaling photon times when digitized pulses are used
    double fGainSampled; //Rescale gain after already being rescaled un RunControl
    double fFirstDynHitGainSampled;
    double fSecondDynCollGainSampled;

    //Used for cases 1, 2
    PhotonInfos IdxList; //Pointer to elements passing QE
    //Used for case 3
    PhotonInfos FirstDyn; //Pointer for first dynode
    //Used for case 4
    PhotonInfos SecondDyn; //Pointer for second dynode QE
    //Used for case 5
    PhotonInfos AftPlsList; //Pointer for afterpulsing
    //Used for dark counts
    PhotonInfos DarkList; //Pointer to dark counts

    std::vector<double> fRates; //!< Afterpulsing rates for different isotopes
    std::vector<std::vector<double>> fTimingParameters; //!< Time delays for different options. Gaussian assumed.
    std::vector<std::vector<double>> fPEParameters; //!<Generated photoelectrons for different options. Gaussian assumed.

    void setPMTParameters();
    void getWavelengthDependentQE(unsigned int PMTNumber);
    void determineDarkCounts(const unsigned long Length);
    unsigned long getTransitTime(const double tt, const double spread);
    unsigned short getCaseNumber();
    unsigned long long getTiming(const unsigned short);
    unsigned short getNumPE(const unsigned short);
    photonResp biasedDieWithAP(const double wavelength);
    double getGaussSpread(const double, const double);
    void doAnalyticPMTResponse(Pulse& thePulse);
    void doSampledPMTResponse(Pulse& thePulse,
        Pulse& thePulseHG);
    void constructBasePMTPulse(const unsigned long N);
    void makeTruthFromPhotonTimes(std::shared_ptr<MCTruth> theTruth, 
				  PhotonInfos& photonList, 
				  size_t& photonCounter);

    static std::shared_ptr<DBInterface<double>> myCSV;
};

#endif /* PMT_hpp */
