//
//  EBGlobal.hpp
//  devices
//
//  Created by Cees Carels on 12/03/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef EBGlobal_hpp
#define EBGlobal_hpp

#include <stdio.h>

/**
 * Class containing information about the current run of the experiment.
 *
 * The format follows that of the Event-Builder (EB).
 */

class EBGlobal
{
public:
    EBGlobal();
    ~EBGlobal();

    void setRunNumber(const unsigned int& run);
    unsigned long getRunNumber();

    void setFormatVersion(const unsigned short& id);
    unsigned long getFormatVersion();

    void setRunStartTime(const unsigned long long& time);
    unsigned long long getRunStartTime();

    void setRunType(unsigned short type);
    unsigned short getRunType();

private:
    unsigned long long runStartTime;
    unsigned int runNumber;
    unsigned short formatVersionID;
    unsigned short runType;
};

#endif /* EBGlobal_hpp */
