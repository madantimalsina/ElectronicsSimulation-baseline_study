//
//  InputOutputFormats.hpp
//  devices
//
//  Created by Cees Carels on 08/02/2017.
//  Copyright © 2017 LZOxford. All rights reserved.
//

#ifndef InputOutputFormats_hpp
#define InputOutputFormats_hpp

#include <stdio.h>

namespace format
{
//Formats pertaining to BACCARAT/DER/LZap input/output data formats
enum class revision : unsigned short
{
    Off = 0,
    BINv1 = 1,
    ROOTvMDC1 = 2,
    ROOTvMDC2 = 3
};

//Formats pertaining to configuration and database parameters
enum class config : unsigned short
{
    CONFIG = 0,
    CSV = 1,
    MONGODB = 2,
    MYSQL = 3
};

//Formats pertaining to LZ Event Format
enum class LZ_ICD_08_0007 : unsigned short
{
    Rev_Draft_1 = 0,
    Rev_Draft_2 = 1,
    Rev_A = 2,
    Rev_B = 3
};

//Formats pertaining to PMT Array Numbering
enum class LZ_ICD_08_0008 : unsigned short
{
    Rev_Draft_1 = 0,
    Rev_Draft_2 = 1,
    Rev_Draft_3 = 2,
    Rev_Draft_4 = 3,
    Rev_V1p0 = 4,
    Rev_A = 5,
    Rev_Ap1 = 6,
    Rev_Ap2 = 7,
    Rev_B = 8
};

enum class AcquisitionMode : unsigned short
{
    Monomodal = 0,
    MonomodalJan2018 = 1
};

enum class SignalChain : unsigned short
{
    ANALYTIC = 0,
    SAMPLED = 1,
    PMTTOPOD = 2,
    CUSTOM = 3,
    OUTER2015 = 4,
    SKIN2015 = 5,
    TPC2015 = 6
};
}

#endif /* InputOutputFormats_hpp */
