//
//  PODContainer.cpp
//

#include "PODContainer.hpp"
#include "Config.hpp"

PODContainer::PODContainer()
    : timesNextCalled(0)
    , elementSamplingRate(1)
    , podTriggerThreshold(8000)
    , preTriggerSamples(std::stoi(global::config->getConfig("PreTrigger")))
    , postTriggerSamples(std::stoi(global::config->getConfig("PostTrigger")))
    , interPodSampleThreshold(std::stoi(global::config->getConfig("IntrPodTime")))
{
    /**
   * Constructor for PODContainer.
     */
}

PODContainer::~PODContainer()
{
    /**
     * Destructor for PODContainer.
     */
    // for(auto pod : *this)
    //   delete pod;
}

void PODContainer::fillPODContainerFromPulse(Pulse& thePulse, std::shared_ptr<MCTruth> theMCTruth, const std::string& LGHG)
{
    /**
     * Supply a pulse as a parameter and form PODs according to the conditions
     * specified by WBS 1.8.
     *
     * The POD starts when a pulse is above threshold for at least 3 samples.
     * The previous 32 samples are also kept in the POD.
     * The POD continues until below threshold for 32 samples.
     * The 32 samples after the trigger ends are also kept in the POD.
     * If there is another pulse which starts within 32 samples (or fewer)
     * from the end of the pulse which was part of the prevous POD, then it too
     * will be included in the same POD as a single POD.
     *
     * This method ensures all PODs on a given pulse are stored inside a vector
     * containing POD objects. Each element of this vector contains a POD found
     * by this method.
     *
     */

    if (thePulse.size() > preTriggerSamples + postTriggerSamples + 1)
    {
        unsigned int pmtNumber = getPmtNumberReal(thePulse.getChannel());
        unsigned long event = thePulse.getEvent();
        unsigned int theChannel = 0;

        //Input commands specify the detector subsystem and threshold
        if (LGHG == "LG")
        {
            podTriggerThreshold = std::stof(global::config->getConfig("TPCLGThresh"));
            theChannel = getChannelNumberLG(pmtNumber);
        }
        else
        {
            podTriggerThreshold = std::stof(global::config->getConfig("TPCHGThresh"));
            theChannel = getChannelNumberHG(pmtNumber);
        }

        int luxSimRunNumber = thePulse.getLUXSimRunNumber();
        unsigned short theDDC32Number = getDDC32Number(theChannel);
        unsigned short theDCNumber = getDCNumber(theDDC32Number, theChannel);

        //Scan through the Pulse, when POD is found, keep last position.
        PODContainer protoPods; //PODs before timing criteria applied
        unsigned int timeThreshold = std::stoi(global::config->getConfig("ThreshTimer"));
        bool isAboveThresh = false;
        unsigned long long threshtimer = 0;
        unsigned long long potentialStart = 0;
        bool isProtoPod = false;

        //Baseline trigger calculation mode
        double sum = 0;
        double mean = 0;
        double bsl = thePulse[0];
        double lrms = std::fabs(thePulse[0]);
        double ex = exp(-0.5 / (double)timeThreshold);
        double RMSs = 0.0;
        double theVal = thePulse[0];
        double dev = 0;
        double sig = thePulse[0];
        double bslHOLD = 0;
        bool holdbsl = false;

        unsigned long long ctr = 0;
        unsigned long minWindow = preTriggerSamples;
        unsigned long maxWindow = thePulse.size() - (postTriggerSamples + 1); //Always positive by convention of pulses always greater at least 1024 in size

        for (unsigned long i = minWindow; i < maxWindow; i++)
        {
            if (i % elementSamplingRate == 0)
            {
                theVal = thePulse[i];
                //Calculate rolling baseline deviation
                if (!holdbsl)
                {
                    ++ctr;
                    sum += theVal;
                    mean = sum / ((double)(ctr));
                    bsl = mean * (1.0 - ex) + bsl * ex;
                    RMSs = RMSs + (theVal - bsl) * (theVal - bsl);
                    double RMS = RMSs / ((double)i + 1.0);
                    RMS = sqrt(RMS);
                    lrms = RMS * (1.0 - ex) + lrms * ex;
                }

                dev = std::abs(theVal - bsl);
                sig = podTriggerThreshold * lrms;

                if (dev > sig)
                {
                    //Something above threshold...
                    if (threshtimer == 0)
                    {
                        //...and first in sequence
                        potentialStart = i;
                    }
                    //Not first in sequence
                    isAboveThresh = true;
                    threshtimer += 1;
                }
                else
                {
                    //std::cout << "Not above threshold" << std::endl;
                    //Did we already have a POD? If yes, save it!
                    //Takes care of finding POD and continuing until nothing left
                    //std::cout << "ThreshTimer " << threshtimer << std::endl;
                    if (isProtoPod)
                    {
                        std::shared_ptr<POD> newlyFoundPOD(new POD());
                        newlyFoundPOD->setPODStartTimeStamp(potentialStart);
                        newlyFoundPOD->setPODLength(i - potentialStart);
                        //Established it's a POD, save as a proto-POD
                        protoPods.push_back(newlyFoundPOD);

                        isProtoPod = false; //Reset
                        threshtimer = 0;
                        potentialStart = 0; //Reset
                    }
                    else
                    {
                        if (threshtimer > 0)
                            threshtimer -= 1;
                    }
                    isAboveThresh = false;
                    bslHOLD = false;
                    holdbsl = false;
                }

                if (threshtimer >= timeThreshold)
                {
                    isProtoPod = true;
                    bslHOLD = sig;
                    holdbsl = true;
                }
            }
        }

        //It is possible that the code arrives here and there is still
        //a ProtoPod that was not captured.
        //This can happen if the signal was a ProtoPod but did not go below
        //threshold before the end of the loop (given by maxWindow).
        //Yet, there should be no signal or dark counts beyond maxWindow.
        //Therefore, if in this case the code arrives here and isProtoPod
        //is true, the samples corresponding to this waveform should be
        //saved.

        if (isProtoPod)
        {
            std::shared_ptr<POD> newlyFoundPOD(new POD());
            newlyFoundPOD->setPODStartTimeStamp(potentialStart);
            newlyFoundPOD->setPODLength(maxWindow - potentialStart);
            protoPods.push_back(newlyFoundPOD);
            isProtoPod = false;
            threshtimer = 0;
            potentialStart = 0;
        }

        //Merge waveforms into PODs as necessary.
        //The case of > 1 waveform must be clarified here:
        //Waveforms (W): _/0\_ _/1\_ _/2\_
        //diffs (D):          0     1
        //If W0,W1 sufficiently close and if W1,W2 sufficiently close then W0,W1,W2
        //go together
        //
        //If W0,W1 sufficiently close and W1,W2 not sufficiently close then W0,W1
        //and W2 are the PODs.
        //
        //If W0,W1 not sufficiently close and W1,W2 sufficiently close then W0, and
        //W1,W2 are the PODs.
        //
        //If W0,W1 not sufficiently close and W1,W2 not sufficiently close then W0,
        //and W1 are the PODs, but the loop will miss W2. Therefore add an extra
        //if(nextPulse <= N) afterwards, to add W2.
        //
        //This last case happens because there are protoPods->size()-1 differences
        //and the loop will not increment to grab the last element if it is too far
        //away. And the loop will stop at protoPods->size()-2 elements. Therefore the
        //last element is not in the loop in this case.

        if (protoPods.size())
        {
            if (protoPods.size() == 1)
            {
                //Exactly 1 POD on channel.
                unsigned long start1 = protoPods[0]->getPODStartTimeStamp();
                unsigned long end1 = start1 + protoPods[0]->getPODLength();

                (protoPods[0]->getPODStartTimeStamp() < preTriggerSamples) ? (start1 = 0) : (start1 = protoPods[0]->getPODStartTimeStamp() - preTriggerSamples);

                podStarts.push_back(start1);
                podEnds.push_back(end1 + postTriggerSamples);
            }
            else
            {
                //More than 1 POD on channel.
                unsigned long N = protoPods.size() - 1;
                std::vector<unsigned long> diffs(N, 0);

                for (int i = 0; i < N; i++)
                {

                    //Calculate time differences between starts and ends.
                    unsigned long long start2 = protoPods[i + 1]->getPODStartTimeStamp();
                    unsigned long long end1 = protoPods[i]->getPODStartTimeStamp() + protoPods[i]->getPODLength();
                    diffs[i] = start2 - end1;
                }

                int nextPulse = 0;
                for (int i = 0; i < N; i = ++nextPulse)
                {
                    unsigned long start1;
                    (protoPods[nextPulse]->getPODStartTimeStamp() < preTriggerSamples) ? (start1 = 0) : (start1 = protoPods[nextPulse]->getPODStartTimeStamp() - preTriggerSamples);
                    podStarts.push_back(start1);
                    while ((diffs[nextPulse] <= interPodSampleThreshold * elementSamplingRate)
                        && (nextPulse < N))
                        nextPulse++;
                    podEnds.push_back(protoPods[nextPulse]->getPODStartTimeStamp() + protoPods[nextPulse]->getPODLength() + postTriggerSamples);
                }

                //Last POD, if applicable.
                if (nextPulse <= N)
                {
                    podStarts.push_back(protoPods[nextPulse]->getPODStartTimeStamp() - preTriggerSamples);
                    podEnds.push_back(protoPods[nextPulse]->getPODStartTimeStamp() + protoPods[nextPulse]->getPODLength() + postTriggerSamples);
                }
            }
        }

        //Put all final PODs in theFinalPODs using podStarts and podEnds vectors
        //and using Resp from thePulse that was given. Output in units of samples
        //unsigned long prevEND = 0;

        this->resize(podStarts.size());
        for (int i = 0; i < podStarts.size(); i++)
        {
            std::shared_ptr<POD> theNewPOD(new POD());
            unsigned int podLength = 0;
            unsigned long podStart = podStarts[i];
            theNewPOD->resize((podEnds[i] - podStart) / elementSamplingRate);
            for (unsigned long long j = podStarts[i];
                 j < podEnds[i];
                 j += elementSamplingRate)
            {
	      theNewPOD->at(j-podStarts[i]) = thePulse[j];
                podLength++;
            }
            theNewPOD->setEvent(event);
            theNewPOD->setLUXSimRunNumber(luxSimRunNumber);
            theNewPOD->setChannel(theChannel);
            theNewPOD->setDataCollector(theDCNumber);
            theNewPOD->setPODStartTimeStamp(podStart);
            theNewPOD->setPODLength(podLength / elementSamplingRate);
            theNewPOD->setHitID(i);
	    theNewPOD->setMCTruth(theMCTruth);
            this->at(i) = std::move(theNewPOD);
        }
    }
}

std::shared_ptr<POD> PODContainer::getNextPOD()
{
    ++timesNextCalled;
    return this->at(timesNextCalled - 1);
}

void PODContainer::resetPODCounter()
{
    timesNextCalled = 0;
}

unsigned long PODContainer::returnFinalPODSize()
{
    return this->size();
}

std::shared_ptr<PODContainer> PODContainer::makePODsFromBoundaries(std::shared_ptr<Pulse> thePulse)
{
    ;
    std::shared_ptr<PODContainer> thePODs(new PODContainer());
    thePODs->resize(podStarts.size());
    for (int i = 0; i < podStarts.size(); i++)
    {
        std::shared_ptr<POD> theNewPOD(new POD());
        unsigned int podLength = 0;
        theNewPOD->resize((podEnds[i] - podStarts[i]) / elementSamplingRate);
        for (unsigned long long j = podStarts[i];
             j < podEnds[i];
             j += elementSamplingRate)
        {
	  theNewPOD->at(j-podStarts[i]) = thePulse->at(j);
            podLength++;
        }
        theNewPOD->setEvent(this->at(i)->getEvent());
        //theNewPOD->setLUXSimRunNumber(luxSimRunNumber);
        theNewPOD->setChannel(this->at(i)->getChannel());
        //theNewPOD->setDataCollector(theDCNumber);
        theNewPOD->setPODStartTimeStamp(podStarts[i]);
        theNewPOD->setPODLength(podLength / elementSamplingRate);
        theNewPOD->setHitID(i);
        thePODs->at(i) = theNewPOD;
    }
    return thePODs;
}

std::shared_ptr<PODContainer> PODContainer::makePODsFromBoundaries(Pulse& thePulse)
{
    std::shared_ptr<PODContainer> thePODs(new PODContainer());
    thePODs->resize(podStarts.size());
    for (int i = 0; i < podStarts.size(); i++)
    {
        std::shared_ptr<POD> theNewPOD(new POD());
        unsigned int podLength = 0;
        unsigned long podStart = podStarts[i];

        theNewPOD->resize((podEnds[i] - podStart) / elementSamplingRate);
        for (unsigned long long j = podStarts[i];
             j < podEnds[i];
             j += elementSamplingRate)
        {
	  theNewPOD->at(j-podStarts[i]) = thePulse[j];
            podLength++;
        }

        theNewPOD->setPODStartTimeStamp(podStart);
        theNewPOD->setPODLength(podLength / elementSamplingRate);
        theNewPOD->setHitID(i);
        thePODs->at(i) = theNewPOD;
    }
    return thePODs;
}

void PODContainer::setElementSamplingRate(const int& rate)
{
    /**
     * Set the sampling rate that should be used to sample the provided pulse.
     *
     * This method will become deprecated since the provided pulse will always
     * be ADC sampled and therefore only digitised data is passed. Therefore the
     * makePODfromPulse() method will automatically sample the pulse elements
     * in the correct incrementation.
     */
    elementSamplingRate = rate;
}

const std::vector<unsigned long long>& PODContainer::getPodStarts() const
{
  return podStarts;
}

const std::vector<unsigned long long>& PODContainer::getPodEnds() const
{
  return podEnds;
}
