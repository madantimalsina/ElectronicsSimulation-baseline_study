//
//  NoiseCable.hpp
//  devices
//
//  Created by Cees Carels on 06/07/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef NoiseCable_hpp
#define NoiseCable_hpp

#include "OldCable.hpp"
#include <stdio.h>

/**
 * Class providing further specification of the Cable interface classes.
 */

class NoisyCable : public OldCable
{
public:
    NoisyCable();
    ~NoisyCable();
    void doCableResponse(Pulse& thePulse);
    void Init();

protected:
};

#endif /* NoiseCable_hpp */
