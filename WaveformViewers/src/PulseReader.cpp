//
//  PulseReader.cpp
//  devices
//
//  Created by Cees Carels on 15/08/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "PulseReader.hpp"
#include "Config.hpp"
#include "TRandom3.h"

PulseReader::PulseReader()
    : PlsStrtIdx(30)
{
    /**
     * Constructor for PulseReader
     */
}

PulseReader::~PulseReader()
{
    /**
     * Destructor for PulseReader
     */
}

int PulseReader::doReadPulses()
{
    /**
     * Read pulses from .bin file
     */

    std::ifstream infile;
    std::string outDir = global::config->getConfig("PEReaderBinPath");

    UInt_t seed = 0;
    try
    {
        seed = (UInt_t)(std::stoi(global::config->getConfig("RandomNumberSeed")));
    }
    catch (...)
    {
        std::cout << "ERROR: Exception caught while setting seed to RandomNumberSeed." << std::endl;
        return -1;
    }
    TRandom3 digRand(seed); //Random number generator to smear digitizer noise
    //This is required becuase we are "redigitizing" digitized pulses, so the
    //noise is not random without re-randomizing it

    if (global::config->getConfig("SmplSrc") == "ECT")
    {
        std::streampos size;
        int numSampsPerPulse = 82; //Number of samples in each single pe
        int numPulses = 9856; //Total number of pulses
        double mVperADCsamps = 1; //mV per ADC for this set of samples

        wordexp_t ExpResult;
        wordexp(outDir.c_str(), &ExpResult, 0);
        outDir = ExpResult.we_wordv[0];
        wordfree(&ExpResult);

        infile.open(outDir.c_str(), std::ios::binary | std::ios::in | std::ios::ate);

        unsigned int counter = 0;
        float x = 0;

        if (infile.is_open())
        {
            std::cout << "NOTICE: SAMPLED from ECT." << std::endl;
            size = infile.tellg();
            infile.seekg(0, std::ios::beg);
            while (!infile.eof() && counter < numPulses * numSampsPerPulse * 2)
            {
                if (counter < (numPulses * numSampsPerPulse))
                {
                    std::vector<float> row;
                    row.resize(numSampsPerPulse);
                    for (int i = 0; i < numSampsPerPulse; i++)
                    {
                        infile.read(reinterpret_cast<char*>(&x), sizeof(float));
                        double digNoise = digRand.Uniform(-0.5 * mVperADCsamps, 0.5 * mVperADCsamps);
                        row[i] = x + digNoise;
                        ++counter;
                    }
                    LEpulses.push_back(row);
                }
                else
                {
                    std::vector<float> row;
                    row.resize(numSampsPerPulse);
                    for (int i = 0; i < numSampsPerPulse; i++)
                    {
                        infile.read(reinterpret_cast<char*>(&x), sizeof(float));
                        double digNoise = digRand.Uniform(-0.5 * mVperADCsamps, 0.5 * mVperADCsamps);
                        row[i] = x + digNoise;
                        ++counter;
                    }
                    HEpulses.push_back(row);
                }
            }
            infile.close();
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else if (global::config->getConfig("SmplSrc") == "DER")
    {
        //std::cout << "NOTICE: SAMPLED from DER." << std::endl;
        infile.open(outDir.c_str(), std::ios::binary | std::ios::in | std::ios::ate);
        std::streampos size;
        if (infile.is_open())
        {
            size = infile.tellg();
            infile.seekg(0, std::ios::beg);
            //  std::cout << "File is " << outDir.c_str() << std::endl;
            while (!infile.eof())
            {
                int32_t x = 0;
                infile.read((char*)&x, sizeof(int32_t));
                int nPODs = x;
                //std::cout << "NumPODs " << nPODs << std::endl;
                for (int i = 0; i < 2 * nPODs; i++)
                {
                    infile.read((char*)&x, sizeof(int32_t));
                    int nSamples = x;
                    //if(i < nPODs) std::cout << "POD " << i << " nSamples " << nSamples << " is HG" << std::endl;
                    //else std::cout << "POD " << i << " nSamples " << nSamples << " is LG" << std::endl;
                    std::vector<float> row;
                    row.resize(nSamples);
                    for (int j = 0; j < nSamples; j++)
                    {
                        float val = 0.0;
                        infile.read((char*)&val, sizeof(float));
                        double digNoise = digRand.Gaus(-0.0, 0.5);
                        //std::cout << "Sample j " << val << std::endl;
                        row[j] = val + digNoise;
                    }
                    if (i < nPODs)
                        LEpulses.push_back(row);
                    else
                        HEpulses.push_back(row);
                }
            }
            infile.close();
            return 0;
        }
        else
        {
            return -1;
        }
        return 0;
    }
    else
    {
        std::cout << "ERROR: Sample source could not be determined.";
        return -1;
    }
}

void PulseReader::SubtractBaseline()
{
    /**
     * Method to subtract baseline from signals to centre around 0.
     */

    for (int j = 0; j < HEpulses.size(); j++)
    {
        double HESum = 0.0;
        for (int i = 0; i < PlsStrtIdx; i++)
        {
            HESum += HEpulses.at(j).at(i);
        }
        HESum /= HEpulses.at(j).size();
        for (int i = 0; i < HEpulses.at(j).size(); i++)
        {
            HEpulses.at(j).at(i) -= HESum;
        }
    }

    for (int j = 0; j < LEpulses.size(); j++)
    {
        double LESum = 0.0;
        for (int i = 0; i < PlsStrtIdx; i++)
        {
            LESum += LEpulses.at(j).at(i);
        }
        LESum /= LEpulses.at(j).size();
        for (int i = 0; i < LEpulses.at(j).size(); i++)
        {
            LEpulses.at(j).at(i) -= LESum;
        }
    }
}

std::vector<float>* PulseReader::getPulseHE(const int& it)
{
    /**
     * Get a high-energy (i.e. low-gain) pulse at index.
     */
    return &HEpulses.at(it);
}

std::vector<float>* PulseReader::getPulseLE(const int& it)
{
    /**
     * Get a low-energy (i.e. high-gain) pulse at index.
     */
    return &LEpulses.at(it);
}

unsigned long PulseReader::HESize()
{
    /**
     * Get number of high-energy pulses.
     */
    return HEpulses.size();
}

unsigned long PulseReader::LESize()
{
    /**
     * Get number of low-energy pulses.
     */
    return LEpulses.size();
}

int PulseReader::getPlsStartIdx()
{
    /**
     * Get start index of pulse.
     */
    return PlsStrtIdx;
}

void PulseReader::setSampledGain(const double Gain)
{
    /**
     * Set PMT gain at which the input sampled pulses were sampled.
     */
    IGain = Gain;
}

void PulseReader::setUserGain(const double Gain)
{
    /**
     * Set the desired PMT gain for the pulses.
     */
    UGain = Gain;
}

void PulseReader::RescalePulses()
{
    /**
     * Rescale the sampled pulses according to the user-desired PMT
     * gain relative to the PMT gain at which they were originally
     * sampled.
     */
    for (int j = 0; j < HEpulses.size(); j++)
    {
        for (int i = 0; i < HEpulses.at(j).size(); i++)
        {
            HEpulses.at(j).at(i) *= (UGain / IGain);
        }
    }

    for (int j = 0; j < LEpulses.size(); j++)
    {
        for (int i = 0; i < HEpulses.at(j).size(); i++)
        {
            LEpulses.at(j).at(i) *= (UGain / IGain);
        }
    }
}

int PulseReader::setupPulseReader()
{
    if (this->doReadPulses() == 0)
    {
        // std::cout << "NOTICE: Pulses read successfully from "
        // 	      << Config->getConfig("PEReaderBinPath")
        // 	      << "."
        // 	      << std::endl;
        this->setSampledGain(std::stod(global::config->getConfig("SampledPMTGain")));
        this->setUserGain(std::stod(global::config->getConfig("UserGain")));
        this->RescalePulses();
        if (global::config->getConfig("SmplSrc") != "DER")
        {
            this->SubtractBaseline();
        }
    }
    else
    {
        // std::cout << "ERROR: An error occurred while reading file "
        // 	      << Config->getConfig("PEReaderBinPath")
        // 	      << "."
        // 	      << std::endl;

        // std::cout << "The file could not be opened or does not exist."
        // 	      << std::endl;

        // std::cout << "Exiting..." << std::endl;
        return -1;
    }
    return 0;
}
