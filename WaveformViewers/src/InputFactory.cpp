//
//  InputFactory.cpp
//  devices
//
//  Created by Cees Carels on 24/10/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//
/////////////////////////////////////////////////////////////////////
// Log:                                                            //
// 11/14/18 - Factory.hpp -> .hpp; removed all input factories     //
//            as factory method can fetch the input directly (AC)  //
/////////////////////////////////////////////////////////////////////

#include "InputFactory.hpp"

#include "RootInputMDC1.hpp"
#include "RootInputMDC2.hpp"

InputFactory::InputFactory()
{
    /**
     * Constructor for Input.
     */
}

InputFactory::~InputFactory()
{
    /**
     * Destructor for Input.
     */
}

Input* InputFactory::getInput(const format::revision inputType)
{
    /**
     * Method to provide the correct derived class concrete factory
     * object.
     */
    if (inputType == format::revision::ROOTvMDC2)
    {
        return new RootInputMDC2();
    }
    else if (inputType == format::revision::ROOTvMDC1)
    {
        return new RootInputMDC1();
    }
    else
        return new RootInputMDC2(); //Default to latest version
}
