//
//  RootInput.cpp
//  devices
//
//  Created by Cees Carels on 24/10/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "RootInput.hpp"

RootInput::RootInput()
{
    /**
     * Constructor for RootInput.
     */
}

RootInput::~RootInput()
{
    /**
     * Destructor for RootInput.
     */
}

bool RootInput::Open()
{
    /**
     * Open the root file given by filePath.
     */
    size_t root = filePath.back().rfind(".root");

    if (root == std::string::npos)
    {
        std::cerr << "Did not recognise a .root file. Exiting..." << std::endl;
        return false;
    }

    inFile = TFile::Open(filePath.back().c_str());
    if (inFile == 0)
    {
        std::cout << "ERROR:\tcannot open file" << std::endl;
        return false;
    }
    else
    {
        std::cout << "File Opened:\t" << filePath.back() << std::endl;
    }

    return true;
}

void RootInput::Close()
{
    /**
     * Close the root file.
     */
    inFile->Close();
    delete inFile;
}

TFile* RootInput::getFilePointer()
{
    /**
     * Get the pointer to the root file.
     */
    return inFile;
}

int RootInput::makePMTDataReady(const unsigned long long& Evt,
    unsigned long long& NPhotons,
    unsigned long long& TMin,
    unsigned long long& TMax,
    unsigned long long& NVertices,
    std::vector<int>& PmtsInEvent)
{
    /**
     * Method required by interface. Intentionally left empty.
     */
    return 1;
}

bool RootInput::getPMTData(const unsigned long long evt,
        int pmt,
        std::shared_ptr<PMT> thePMT,
	unsigned long eventLength,
        unsigned long long timeShift,
        unsigned long long TimeShiftInce,
	unsigned long long k)
{
  return false;
}
