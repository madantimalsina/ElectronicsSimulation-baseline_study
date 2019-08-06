//
//  DDC32Asymmetric.hpp
//  devices
//
//  Created by Cees Carels on 30/09/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef DDC32Asymmetric_hpp
#define DDC32Asymmetric_hpp

#include <stdio.h>

#include "DDC32.hpp"

/**
 * Class providing the DDC32 with asymmetric voltage bounds.
 */

class DDC32Asymmetric : public DDC32
{
public:
    DDC32Asymmetric();
    ~DDC32Asymmetric();
    int mVtoADC(double& mV);

protected:
};

#endif /* DDC32Asymmetric_hpp */
