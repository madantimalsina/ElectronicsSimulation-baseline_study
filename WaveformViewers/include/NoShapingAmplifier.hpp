//
//  NoShapingAmplifier.hpp
//  devices
//
//  Created by Cees Carels on 14/08/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef NoShapingAmplifier_hpp
#define NoShapingAmplifier_hpp

#include "Amplifier.hpp"
#include <stdio.h>

/**
 * Trivial implementation of the ShapingAmplifier class.
 *
 * This class does nothing and is used to bypass the ShapingAmplifier.
 */

class NoShapingAmplifier : public Amplifier
{
public:
    NoShapingAmplifier();
    ~NoShapingAmplifier();
    void doAmpResponse(Pulse& thePulse);
};

#endif /* NoShapingAmplifier_hpp */
