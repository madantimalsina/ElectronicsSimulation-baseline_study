//
//  OutputFactory.cpp
//  devices
//
//  Created by Cees Carels on 27/10/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "OutputFactory.hpp"
#include "BinaryOutput.hpp"
#include "RootOutputMDC1.hpp"
#include "RootOutputMDC2.hpp"

OutputFactory::OutputFactory()
{
    /**
     * Constructor for Output.
     */
}

OutputFactory::~OutputFactory()
{
    /**
     * Destructor for Output.
     */
}

Output* OutputFactory::getOutput(const format::revision type)
{
    /**
     * Method to provide the correct derived class concrete type
     * object.
     */
    if (type == format::revision::ROOTvMDC2)
    {
        return new RootOutputMDC2();
    }
    else if (type == format::revision::ROOTvMDC1)
    {
        return new RootOutputMDC1();
    }
    else if (type == format::revision::BINv1)
    {
        return new BinaryOutput();
    }
    else
        return new RootOutputMDC2(); //Default to latest version
}
