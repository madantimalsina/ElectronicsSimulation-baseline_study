//
//  DDC32.hpp
//  devices
//
//  Created by Cees Carels on 06/07/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef DDC32_hpp
#define DDC32_hpp

#include "Digitizer.hpp"
#include "Pulse.hpp"
#include <stdio.h>

/**
 * Class providing the implementation of the DDC32 digitiser boards.
 */

class DDC32 : public Digitizer
{
public:
    DDC32(const unsigned short& numDC = 0,
        const unsigned short& ChansPerDC = 0,
        const unsigned long long& TS = 0);
    ~DDC32();

    Pulse doDDC32Response(Pulse& thePulse);
    Pulse doDDC32Response(Pulse& pls1, Pulse& pls2);
    void mVtoADCC(Pulse& pls1, Pulse& pls2, double DCOffset);
    int mVtoADC(const double& mV);

    void setDCNumber(const unsigned short& dc);
    unsigned short getDCNumber();

    void sumPulse(Pulse& thePulse,
        const unsigned short& DC,
        const unsigned short& Chan,
        const int& numPODsonChan);
    unsigned short getChansOnDDC32();

    void setSamplingRate(const int& SmplRate);
    int getSamplingRate();

    void setElementSamplingRate(const int& ElSmplRate);
    int getElementSamplingRate();

    std::vector<unsigned short> getnPods();
    unsigned long long getPODStartTS();
    unsigned long long getPODLength();
    std::vector<int> getSumPODData();

protected:
    void LSBRounding(Pulse& thePulse, const unsigned short& DC);

    int SamplingRate; //[ns] Sampling rate

    int ElementSamplingRate; //Sampling rate in elements of pulse array
    //Units are the number of elements of the pulse to be sampled
    //The size of this rate is a function of sampling rate (ADC) and determines simPrecision in pulsemanager
    unsigned short numBits; //2^14 bits
    double digMin; //!< Digitizer minimum value [ref needed]
    double digMax; //!< Digitizer maximum value [ref needed]

    unsigned long long EventTimeStamp;
    std::vector<Pulse> DCSummedPulses;
    std::vector<unsigned short> nPods; //Number of PODs on each DC
    bool DCNumberAssigned;
    double missRate(); //Temperature dependent miss rate
    unsigned short DCNumber;
    unsigned short NumberofDC;
    unsigned short ChansOnDDC32; //Number of channels per DDC32
    unsigned long long PODStartTS;
    unsigned long long PODLength;
    std::vector<int> SumPODData;

    //By having a DCNumber, the number of the DC can be separated from the
    //element number in the array. E.g. theDCs[0] can have an arbitrary
    //DC number. This separates the DCNumber in PMTLookup from the DDC32 class.
};

#endif /* DDC32_hpp */
