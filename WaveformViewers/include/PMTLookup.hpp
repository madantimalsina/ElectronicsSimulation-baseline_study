//
//  PMTLookup.hpp
//  devices
//
//  Created by Cees Carels on 06/02/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef PMTLookup_hpp
#define PMTLookup_hpp

#include <iostream>
#include <math.h>
#include <stdio.h>

#include "InputOutputFormats.hpp"

int getPmtNumberReal(const int& SimPmtNumber);
int getChannelNumberLG(const int& RealPmtNumber);
int getChannelNumberHG(const int& RealPmtNumber);

unsigned short getDDC32Number(const int& chan);
unsigned short getDCNumber(const unsigned short& DDC32Num,
    const unsigned short& chan);
unsigned short getNDigisInDC(const int& DC);

#endif /* PMTLookup_hpp */
