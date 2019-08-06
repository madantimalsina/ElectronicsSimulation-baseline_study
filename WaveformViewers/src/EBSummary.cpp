//
//  EBSummary.cpp
//  devices
//
//  Created by Cees Carels on 13/03/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "EBSummary.hpp"

EBSummary::EBSummary()
{
    /**
     * Constructor for EBSummary.
     */
}

EBSummary::~EBSummary()
{
    /**
     * Destructor for EBSummary.
     */
}

void EBSummary::setEndFlag(const unsigned short& end)
{
    /**
     * Set the end flag for the EB summary.
     *
     * To understand this variable better refer to the Event Builder.
     */
    endFlag = end;
}

unsigned short EBSummary::getEndFlag()
{
    /**
     * Get the end flag for the EB summary.
     *
     * To understand this variable better refer to the Event Builder.
     */
    return endFlag;
}

void EBSummary::setNEvtsFile(const unsigned short& nEventsFile)
{
    /**
     * Set the total number of events in the output file that DER processed.
     */
    nEvtsFile = nEventsFile;
}

unsigned short EBSummary::getNEvtsFile()
{
    /**
     * Get the total number of events in the output file that DER processed.
     */
    return nEvtsFile;
}
