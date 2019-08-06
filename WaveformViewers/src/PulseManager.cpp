//
//  PulseManager.cpp
//  devices
//
//  Created by Cees Carels on 02/02/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "PulseManager.hpp"
#include "Config.hpp"

/**
 * Struct to hold information used for sorting purposes.
 */
struct ProtoPulseData
{
    double time;
    double wavelength;
};

inline unsigned long long
pow2roundup(unsigned long long x)
{
    /**
     * Round up to the nearest power of 2 for the parameter given.
     */
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

bool CompareProtoPulseData(ProtoPulseData const& lhs,
    ProtoPulseData const& rhs)
{
    /**
     * Compare the time and wavelength for the sturct ProtoPulseData.
     *
     * Used for sorting purposes. Returns the lesser of lhs and rhs for
     * each quantity in ProtoPulseData.
     */
    if (lhs.time != rhs.time)
        return lhs.time < rhs.time;
    return lhs.wavelength < rhs.wavelength;
}

PulseManager::PulseManager()
    : TimesCalled(0)
    , MoreSubsets(true)
{
    /**
     * Constructor for PulseManager
     */
}

PulseManager::PulseManager(const int& SampleRate)
    : TimesCalled(0)
    , MoreSubsets(true)
    , SubsetNumber(0)
    , SubsetIncrement(1024 * SampleRate)
    , AcquisitionDidEnd(false)
    , simPrecision(1)
{
    /**
     * Constructor for PulseManager.
     * The PulseSubset window size is simPrecision*SubsetIncrement [ns]
     */
}

PulseManager::PulseManager(Pulse& thePulse,
    const int& SampleRate)
    : TimesCalled(0)
    , MoreSubsets(true)
    , SubsetNumber(0)
    , SubsetIncrement(1024 * SampleRate)
    , AcquisitionDidEnd(false)
    , simPrecision(1)
    , PMTPulse(thePulse)
    , SamplingRate(SampleRate)
{
    /**
     * Constructor for PulseManager which creates and manages
     * the pulse passed as a parameter.
     */
    preparePulse(); //Also sets beginSubset
    setElementSampleRate();
}

PulseManager::~PulseManager()
{
    /**
     * Destructor for PulseManager.
     */
}

Pulse PulseManager::getPMTPulse()
{
    /**
     * Returns the PMTPulse which is a managed pulse object.
     */
    return PMTPulse;
}

void PulseManager::PrepareEventBounds(Pulse& thePulse,
    unsigned long long TMin,
    unsigned long long TMax)
{
    /**
     * Collapses duplicates into the Resp data of a new pulse which is then
     * returned.
     *
     */

    beginSubset = TMin * simPrecision;

    //Endsubset either takes the nearest power of 2, or a padded version of the
    //maximum photon time in the event.
    auto config = global::config;
    try
    {
        if (config->getConfig("PulsePadding") == "AUTO")
        {
            endSubset = pow2roundup(TMax * simPrecision + SubsetIncrement);
        }
        else if (config->getConfig("PulsePadding") == "SMOOTH")
        {
            endSubset = (TMax * simPrecision + SubsetIncrement + 2048);
        }
        else if (config->getConfig("PulsePadding") == "WINDOW")
        {
            unsigned long long window = TMax * simPrecision + SubsetIncrement + 2048;
            try
            {
                window = std::stoull(config->getConfig("PostWindow")) * 1.0e6 + 2048;
                if (window > 120 * 1.0e6)
                {
                    std::cout << "ERROR: The chosen value for PostWindow is too high." << std::endl;
                    std::cout << "Value will be limited to " << (unsigned long long)2.5e6 << " [ns]." << std::endl;
                    std::cout << "You continue at your own risk." << std::endl;
                }
                window = 2.5e6 + 2048;
            }
            catch (...)
            {
                std::cout << "ERROR: Could not set PulsePadding from PostWindow." << std::endl;
                std::cout << "You continue at your own risk." << std::endl;
            }
            endSubset = (window + 2048);
        }
        else
        {
            std::cout << "WARNING: PulsePadding could not be set." << std::endl;
            std::cout << "A user input error is assumed." << std::endl;
            std::cout << "Will continue with PulsePadding set to AUTO." << std::endl;
            std::cout << "PulsePadding setting will not be recorded in output!" << std::endl;
            endSubset = pow2roundup(TMax * simPrecision + SubsetIncrement);
        }
    }
    catch (...)
    {
        std::cout << "WARNING: PulsePadding could not be set." << std::endl;
        std::cout << "A user input error is assumed." << std::endl;
        std::cout << "Will continue with PulsePadding set to AUTO." << std::endl;
        std::cout << "PulsePadding setting will not be recorded in output!" << std::endl;
    }

    thePulse.resize((endSubset - beginSubset) * simPrecision);
    thePulse.setLUXSimEvtNumSize((endSubset - beginSubset) * simPrecision);

    for (unsigned long i = 0; i < thePulse.size(); i++)
    {
        thePulse[i] = 0;
        thePulse.setLUXSimEvtNum(2, 0, i);
    }
}

void PulseManager::preparePulse()
{
    /**
     * Make pulse subsets by pushing it into a vector.
     *
     * Currently this method is implemented for single pulse subsets, but can
     * be extended to support more.
     */
    PulseSubsets.push_back(PMTPulse);
    PulseSubsets.back().resize(1);

    numbOfSubsets = PulseSubsets.size();
    TimesCalled = 0;
}

Pulse PulseManager::getPMTSubset()
{
    /**
     * Returns the next PMTSubset.
     *
     * This method will become deprecated in future in light of performance
     * improvements.
     */
    Pulse Subset = PulseSubsets[TimesCalled];
    TimesCalled += 1;
    if (TimesCalled >= PulseSubsets.size())
    {
        MoreSubsets = false;
        TimesCalled = 0;
    }
    SubsetNumber += 1;
    return Subset;
}

void PulseManager::setPMTNumber(const int& PMTNumber)
{
    /**
     * Set the PMT number so that the PulseManager can manage it for other
     * devices.
     *
     * This method will become deprecated when there is a well-defined mapping
     * from LUXSim simulation PMTs and the real LZ PMT channels.
     */
    CurrentPMT = PMTNumber;
}

int PulseManager::getPMTNumber()
{
    /**
     * Retrieve the PMT number so that the PulseManager can report it to other
     * devices when they require it.
     *
     * This method will become deprecated when there is a well-defined mapping
     * from LUXSim simulation PMTs and the real LZ PMT channels.
     */
    return CurrentPMT;
}

void PulseManager::setPMTChannel(const int& PMTChannel)
{
    /**
     * Set the PMT channel so that the PulseManager can manage it for other
     * devices.
     */
    CurrentPMTChannel = PMTChannel;
}

int PulseManager::getPMTChannel()
{
    /**
     * Retrieve the PMT channel so that the PulseManager can report it to other
     * devices when they require it.
     */
    return CurrentPMTChannel;
}

void PulseManager::markEndAcquisition()
{
    /**
     * Mark the end of the acquisition that there are no more pulse subsets to
     * retrieve and analyse for the current channel.
     *
     * This method will become deprecated.
     */
    AcquisitionDidEnd = true;
}

bool PulseManager::isEndedAcquisition()
{
    /**
     * Returns whether there are more pulse subsets available.
     *
     * This method will become deprecated.
     */
    if (MoreSubsets == false)
        AcquisitionDidEnd = true;
    return AcquisitionDidEnd;
}

unsigned long PulseManager::SubPlsNumb()
{
    /**
     * Return the number in the total sequence of pulse subsets of the current
     * pulse being analysed.
     */
    return SubsetNumber - 1;
}

unsigned long PulseManager::getBeginSubset()
{
    /**
     * Get the start time of the current pulse subset.
     */
    return beginSubset;
}

unsigned long PulseManager::getEndSubset()
{
    /**
     * Get the end time of the current pulse subset.
     */
    return endSubset;
}

void PulseManager::calcNumbOfSubsets()
{
    /**
     * Calculate the number of pulse subsets that are being managed.
     */
    numbOfSubsets = PulseSubsets.size();
}

unsigned long PulseManager::NumbOfSubsets()
{
    /**
     * Return the total number of pulse subsets on that are being managed.
     */
    return numbOfSubsets;
}

void PulseManager::setSamplingRate(const int& SmplRate)
{
    /**
     * Set the sampling rate so that PulseManager can manage it.
     *
     * This is in units of [ns].
     */
    SamplingRate = SmplRate;
}

int PulseManager::getSamplingRate()
{
    /**
     * Return the sampling rate so that other devices can request it.
     *
     * This is in units of [ns].
     */
    return SamplingRate;
}

int PulseManager::setElementSampleRate()
{
    /**
     * Sampling rate devices should use when sampling at ADC sampling rate.
     *
     * This is in units of number of elements to jump in succession.
     */

    ElementSamplingRate = SamplingRate * (int)simPrecision;
    return ElementSamplingRate;
}

int PulseManager::getElementSampleRate()
{
    /**
     * Sampling rate devices should use when sampling at ADC sampling rate.
     *
     * This is in units of number of elements to jump in succession.
     */
    return ElementSamplingRate;
}

void PulseManager::setPulseSubsetBeg(const std::vector<double>& BegVec)
{
    /**
     * Method will become deprecated.
     */
    PulseSubsetBeg = BegVec;
}

std::vector<double> PulseManager::getPulseSubsetBeg()
{
    /**
     * Method will become deprecated.
     */
    return PulseSubsetBeg;
}
