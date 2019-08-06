//
//  OutputFactory.hpp
//  devices
//
//  Created by Cees Carels on 27/10/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef OutputFactory_hpp
#define OutputFactory_hpp

#include <stdio.h>

#include "InputOutputFormats.hpp"
#include "Output.hpp"

class OutputFactory
{
public:
    OutputFactory();
    ~OutputFactory();

    static Output* getOutput(const format::revision type);

protected:
};

#endif /* OutputFactory_hpp */
