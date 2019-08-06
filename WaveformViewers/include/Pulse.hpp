//
//  Pulse.hpp
//  devices
//
//  Created by Cees Carels on 06/07/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef Pulse_hpp
#define Pulse_hpp

#include <cmath>
#include <iostream>
#include <set>
#include <stdio.h>
#include <string>
#include <vector>

#include "TAxis.h"
#include "TCanvas.h"
#include "TGaxis.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TImage.h"
#include "TLegend.h"
#include "TStyle.h"

/**
 * Class providing the interface and implementation of pulse.
 *
 * A pulse object is the unit of calculation in DER. A pulse can contains timing
 * information, as well as the photon wavelength and response to it at a time.
 * The binning and associations between these quantities is not controlled,
 * meaning a pulse object can travel through various stages in the signal chain
 * while retaining the key information (the response). Management for these
 * quantities happens by a user of the class, in order to provide near direct
 * access to the pulse information.
 */

class Pulse : public std::vector<double>
{
public:
    Pulse();
    ~Pulse();
    Pulse(const Pulse& other)
    {
        *this = other;
    }

    unsigned long getPulseID() const;
    void setPulseID(unsigned long pulseID);

    void setEvent(const unsigned long long& evt);
    unsigned long long getEvent() const;

    void setLUXSimRunNumber(int run_number);
    int getLUXSimRunNumber() const;

    unsigned int getChannel() const;
    void setChannel(unsigned int cn);

    void setLUXSimEvtNumSize(const unsigned long& size);

    unsigned long long getLUXSimEvtNum(const int& form, const unsigned long idx = 0) const;
    void setLUXSimEvtNum(const int& form, const unsigned long long sim, const unsigned long idx = 0);

    void doFFT();

    void addPhotonInterval(const unsigned int photonStart, const unsigned int photonEnd);
    void sortPhotonIntervals();
    void setPhotonIntervalStart(const unsigned int intervalNo, const unsigned int photonStart);
    void setPhotonIntervalEnd(const unsigned int intervalNo, const unsigned int photonEnd);
    unsigned int getPhotonSize() const;
    const std::pair<unsigned int, unsigned int>& getPhotonIntervalAt(const unsigned int i) const;
    const bool isIntervalOverlapping(const unsigned int i);
    void setIsIntervalOverlapping(const unsigned int i, const bool isOverlapping);
    void findNextFilterSample(unsigned int& intervalNo, unsigned int& sampleNo);

    const std::vector<double>& getSamples() const;

    Pulse operator+(const Pulse& thePulse)
    {
      if(this->size() == thePulse.size())
	{
	  for(auto it = this->begin(); it != this->end(); ++it)
	    *it += thePulse[it-this->begin()];
	}
      else
        {
            std::cout << "Warning: pulses not of equal length" << std::endl;
	    return *this;
	}
    }

    Pulse operator-(const Pulse& thePulse)
    {
      if(this->size() == thePulse.size())
	{
	  for(auto it = this->begin(); it != this->end(); ++it)
	    *it -= thePulse[it-this->begin()];
	}
      else
        {
	  std::cout << "Warning: pulses not of equal length" << std::endl;
	  return *this;
	}
    }

  
    Pulse operator+=(const Pulse& thePulse)
     {
       if(this->size() == thePulse.size())
	 {
	   for(auto it = this->begin(); it != this->end(); ++it)
	     *it += thePulse[it-this->begin()];
	 }
       else
	 {
	   std::cout << "Warning: pulses not of equal length" << std::endl;
	 }
     }

    Pulse operator-=(const Pulse& thePulse)
    {
      if(this->size() == thePulse.size())
	{
	  for(auto it = this->begin(); it != this->end(); ++it)
	    *it -= thePulse[it-this->begin()];
	}
      else
        {
	  std::cout << "Warning: pulses not of equal length" << std::endl;
	}
    }

protected:

    unsigned long long fEventID; //Pulse belongs to Event
    unsigned long fPulseID;
    unsigned int fChannel; //!< PMT/Channel number
    unsigned long fNextit; //Arrival time iterator
    unsigned long fLastItPos;

    int fLuxSimRunNumber;
    std::vector<unsigned long long> fLuxSimEvtNumbers;

    std::vector<std::pair<unsigned int, unsigned int> > fPhotonIntervals;
    std::vector<bool> fOverlappingIntervals;

};

#endif /* Pulse_hpp */
