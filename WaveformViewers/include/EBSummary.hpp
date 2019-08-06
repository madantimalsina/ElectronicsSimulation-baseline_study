//
//  EBSummary.hpp
//  devices
//
//  Created by Cees Carels on 13/03/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef EBSummary_hpp
#define EBSummary_hpp

#include <stdio.h>

/**
 * Class containg summary information about the DER simulation.
 */

class EBSummary
{
public:
    EBSummary();
    ~EBSummary();

    void setEndFlag(const unsigned short& endFlag);
    unsigned short getEndFlag();

    void setNEvtsFile(const unsigned short& nEventsFile);
    unsigned short getNEvtsFile();

private:
    unsigned short endFlag; //!< End flag for Event Builder.
    unsigned short nEvtsFile; //!< Number of events in the output file.
};

#endif /* EBSummary_hpp */
