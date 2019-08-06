//
//  InputFactory.hpp
//  devices
//
//  Created by Cees Carels on 24/10/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef InputFactory_hpp
#define InputFactory_hpp

#include <stdio.h>

#include "Input.hpp"
#include "InputOutputFormats.hpp"

/**
 * Abstract factory that provides interface to concrete factory
 * implementations of Input classes.
 */

class InputFactory
{
public:
    InputFactory();
    virtual ~InputFactory() = 0;

    static Input* getInput(const format::revision inputType);
};

#endif /* InputFactory_hpp */
