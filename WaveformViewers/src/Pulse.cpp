//
//  Pulse.cpp
//  devices
//
//  Created by Cees Carels on 08/01/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "Pulse.hpp"
#include "FFT.hpp"

Pulse::Pulse()
    : fLuxSimEvtNumbers()
    , fEventID(0)
    , fPulseID(0)
    , fChannel(0)
    , fNextit(1)
    , fLastItPos(0)
    , fLuxSimRunNumber(-1)
{
    /**
     * Constructor for pulse.
     */
}

Pulse::~Pulse()
{
    /**
     * Destructor for pulse.
     */
}

void Pulse::setPulseID(unsigned long PlsID)
{
    /**
     * Set pulse ID.
     */
    fPulseID = PlsID;
}

unsigned long Pulse::getPulseID() const
{
    /**
     * Get pulse ID.
     */
    return fPulseID;
}

void Pulse::setEvent(const unsigned long long& evt)
{
    /**
     * Set the event to which the pulse belongs.
     */
    fEventID = evt;
}

unsigned long long Pulse::getEvent() const
{
    /**
     * Get the event to which the pulse belongs.
     */
    return fEventID;
}

void Pulse::setLUXSimEvtNum(const int& form, const unsigned long long sim, const unsigned long idx)
{
    /**
     * Set the LUXSim event to which the pulse belongs.
     */
    switch (form)
    {
    case 0:
        fLuxSimEvtNumbers.insert(fLuxSimEvtNumbers.begin(), sim);
        break;
    case 1:
        fLuxSimEvtNumbers.push_back(sim);
        break;
    case 2:
        fLuxSimEvtNumbers[idx] = sim;
        break;
    default:
        std::cout << "Invalid option" << std::endl;
        break;
    }
}

unsigned long long Pulse::getLUXSimEvtNum(const int& form, const unsigned long idx) const
{
    /**
     * Get the LUXSim event to which the pulse belongs.
     */
    switch (form)
    {
    case 1:
        return fLuxSimEvtNumbers[idx];
        break;
    case 2:
        return fLuxSimEvtNumbers.front();
        break;
    default:
        std::cout << "Invalid option" << std::endl;
        return 0;
        break;
    }
}

void Pulse::setLUXSimEvtNumSize(const unsigned long& size)
{
    /**
     * Set the size of the LUXSimEvtNumber vector.
     */
    fLuxSimEvtNumbers.resize(size);
}

void Pulse::setLUXSimRunNumber(int rnum)
{
    /**
     * Set the event to which the pulse belongs.
     */
    fLuxSimRunNumber = rnum;
}

int Pulse::getLUXSimRunNumber() const
{
    /**
     * Get the event to which the pulse belongs.
     */
    return fLuxSimRunNumber;
}

void Pulse::setChannel(unsigned int cn)
{
    /**
     * Set the channel on which the pulse belongs.
     */
    fChannel = cn;
}

unsigned int Pulse::getChannel() const
{
    /**
     * Get the channel on which the pulse belongs.
     */
    return fChannel;
}

void Pulse::doFFT()
{
    /**
     * Do FFT on current pulse.
     */
    FFT myFFT(0.000000001); //Pulse sampling rate, hardcoded for now
    myFFT.doRealFFT(*this);
}

void Pulse::addPhotonInterval(const unsigned int photonStart, 
			      const unsigned int photonEnd)
{
  fPhotonIntervals.push_back(std::make_pair(photonStart,photonEnd+20)); 
  //discretionary 20 samples added to let the filter response return to ~0
  fOverlappingIntervals.push_back(false);
}

void Pulse::sortPhotonIntervals()
{
  if(getPhotonSize()>1){
    std::sort(fPhotonIntervals.begin(),fPhotonIntervals.end());
  }
}

unsigned int Pulse::getPhotonSize() const
{
  return fPhotonIntervals.size();
}

const std::pair<unsigned int, unsigned int>& Pulse::getPhotonIntervalAt(const unsigned int i) const 
{
  return fPhotonIntervals[i];
}

void Pulse::setPhotonIntervalStart(const unsigned int intervalNo, const unsigned int photonStart)
{
  fPhotonIntervals[intervalNo].first = photonStart;
}

void Pulse::setPhotonIntervalEnd(const unsigned int intervalNo, const unsigned int photonEnd)
{
  fPhotonIntervals[intervalNo].second = photonEnd;
}

const bool Pulse::isIntervalOverlapping(const unsigned int i)
{
  return fOverlappingIntervals[i];
}
   
void Pulse::setIsIntervalOverlapping(const unsigned int i, const bool isOverlapping)
 {
   fOverlappingIntervals[i] = isOverlapping;
 }

void Pulse::findNextFilterSample(unsigned int& intervalNo, unsigned int& sampleNo)
{
   while(intervalNo+1<fPhotonIntervals.size() &&
	fOverlappingIntervals[intervalNo+1])
    ++intervalNo; //skip any intervals that are fully covered

  while(intervalNo+1<fPhotonIntervals.size() &&
	sampleNo>fPhotonIntervals[intervalNo+1].second)
    {
      fOverlappingIntervals[intervalNo+1] = true; 
      //save this to avoid the comparison again
      ++intervalNo; //skip any intervals that are fully covered
    }

  if(intervalNo+1<fPhotonIntervals.size())
    {
      if(sampleNo<fPhotonIntervals[intervalNo+1].first)
  	sampleNo = fPhotonIntervals[intervalNo+1].first;
      //else retain the same sampleNo to avoid filtering over the same
      //samples, since we're partially into the interval
    }
}

const std::vector<double>& Pulse::getSamples() const
{
    /**
     * Get the vector of ADCC values of the POD.
     */
  return dynamic_cast<const std::vector<double> &>(*this);
}
