//
//  PODfromPMT.hpp
//  devices
//
//  Created by Cees Carels on 14/08/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef PODfromPMT_hpp
#define PODfromPMT_hpp

#include <stdio.h>

#include "POD.hpp"

/**
 * An extension to the POD class, providing an alternative form of
 * the makePODfromPulse method.
 *
 * This class assumes the pulse is pre-digitsed and contains the POD bounds.
 *
 * Therefore, the makePODfromPulse method inside this class only forms the PODs
 * from these bounds, and does not scan on the threshold conditions.
 */

class PODfromPMT : public POD
{
public:
    PODfromPMT();
    ~PODfromPMT();
    void makePODfromPulse(Pulse& thePulse, const std::string& LGHG);

private:
};

#endif /* PODfromPMT_hpp */