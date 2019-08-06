//
//  PMTLookup.cpp
//

#include "PMTLookup.hpp"

int getPmtNumberReal(const int& SimPmtNumber)
{
    /**
     * Provide a simulation PMT number and retrieve a real LZ PMT number.
     *
     * This will require a lookup table once geometry is fixed.
     */

    //For the moment just return the same number
    return SimPmtNumber;
}

int getChannelNumberLG(const int& RealPmtNumber)
{
    /**
     * Provide the real LZ low-gain channel number using the real PMT number.
     *
     * This will require a lookup table once geometry is fixed.
     */

    return RealPmtNumber + 1000;
}

int getChannelNumberHG(const int& RealPmtNumber)
{
    /**
     * Provide the real LZ high-gain channel number using the real PMT number.
     *
     * This will require a lookup table once geometry is fixed.
     */

    //For the moment just offset by +1000 and return number
    return RealPmtNumber;
}

unsigned short getDDC32Number(const int& chan)
{
    /**
     * Retrieve the DDC32 number on which the channel is located.
     *
     * The DDC32 boards are labelled with an index 0-45 = 46 DDC32 boards.
     *
     * The channel ranges are specified according to LZ-ICD-08-0008.
     *
     * The channel mapping to specific DDC32 board number is not yet specified.
     *
     * The mapping is implemented as a lookup with if/else statements.
     *
     * In future, this mapping may be produced through a database interface.
     *
     * The convention is to offset the DDC32 number based on the location of the
     * PMT. By doing so it acquires the DDC32 board number to which it will be
     * assigned. For example, the TPC LG PMTs are assigned the first 16 DDC32
     * boards, but the TPC HG PMTs should be assigned the second set of 16 DDC32
     * boards, therefore the DDC32 board number should be offset by 16.
     * The Skin PMTs DDC32 board number should be offset by 16 + 16 = 32.
     *
     * The above gives the correct set of DDC32 board numbers. The correct
     * individual DDC32 board number within that set is returned using a
     * multiple of 32.
     *
     * Counting of DDC32 boards starts at 0. So the first 16 boards have
     * board numbers 0-15.
     *
     * It was inferred that Dummy PMTs do not go map onto a DDC32 or DC board.
     */

    //OFFSETS
    int TPCLGOffset = 0;
    int TPCHGOffset = 16;
    int SKIOffset = 32;
    int OUTHGOffset = 38;
    int OUTLGOffset = 42;

    unsigned short DDC32Num = 0;

    //TPC TOP
    if (chan >= 0 && chan <= 252)
    {
        //TPC Top HG
        DDC32Num = (int)(chan / 32) + TPCHGOffset;
    }
    else if (chan >= 1000 && chan <= 1252)
    {
        //TPC Top LG
        DDC32Num = (int)((chan - 1000) / 32) + TPCLGOffset;
    }

    //TPC TOP DUMMY
    else if (chan == 250)
    {
        //TPC Top Dummy HG
    }
    else if (chan == 1250)
    {
        //TPC Top Dummy LG
    }

    //TPC BOTTOM
    else if (chan >= 300 && chan <= 540)
    {
        //TPC Bottom HG
        DDC32Num = (int)(chan / 32) + TPCHGOffset;
    }
    else if (chan >= 1300 && chan <= 1540)
    {
        //TPC Bottom LG
        DDC32Num = (int)((chan - 1000) / 32) + TPCLGOffset;
    }

    //TPC BOTTOM DUMMY
    else if (chan >= 550 && chan <= 556)
    {
        //TPC Bottom Dummy HG
        DDC32Num = (int)(chan / 32) + TPCHGOffset;
    }
    else if (chan >= 1550 && chan <= 1556)
    {
        //TPC Bottom Dummy LG
        DDC32Num = (int)((chan - 1000) / 32) + TPCLGOffset;
    }

    //SKIN
    else if (chan >= 600 && chan <= 689)
    {
        //Skin Top
        DDC32Num = (int)(chan / 32) + SKIOffset;
    }
    else if (chan >= 700 && chan < 719)
    {
        //Skin Bottom
        DDC32Num = (int)(chan / 32) + SKIOffset;
    }
    else if (chan >= 730 && chan <= 747)
    {
        //Skin Bottom Bottom Dome
        DDC32Num = (int)(chan / 32) + SKIOffset;
    }
    else if (chan >= 750 && chan <= 755)
    {
        //Skin Top Dummy
        DDC32Num = (int)(chan / 32) + SKIOffset;
    }
    else if (chan >= 770 && chan <= 779)
    {
        //Skin Bottom Dummy
        DDC32Num = (int)(chan / 32) + SKIOffset;
    }
    else if (chan >= 790 && chan <= 799)
    {
        //Skin Bottom Dome Dummy
        DDC32Num = (int)(chan / 32) + SKIOffset;
    }

    //OUTER
    else if (chan >= 800 && chan <= 927)
    {
        //Outer Detector HG
        DDC32Num = (int)(chan / 32) + OUTHGOffset;
    }
    else if (chan >= 1800 && chan <= 1927)
    {
        //Outer Detector LG
        DDC32Num = (int)(chan / 32) + OUTLGOffset;
    }
    else if (chan >= 950 && chan <= 957)
    {
        //Outer Detector Dummy HG
        DDC32Num = (int)(chan / 32) + OUTHGOffset;
    }
    else if (chan >= 1950 && chan <= 1957)
    {
        //Outer Detector Dummy LG
        DDC32Num = (int)(chan / 32) + OUTLGOffset;
    }
    else
    {
        //std::cout << "Warning: A DDC32 board number "
        //<< "could not be found/assigned for "
        //<< "channel " << chan << "." << std::endl;
    }

    return DDC32Num;
}

unsigned short getDCNumber(const unsigned short& DDC32Num,
    const unsigned short& chan)
{
    /**
     * Retrieve the data collector number using the DDC32 Number,
     * and the PMT channel number (low-gain or high-gain, as the case may be.)
     *
     * This can be implemented as a lookuptable.
     *
     * The mapping from DDC32 board number to DC number is not yet specified.
     * Therefore a suitable mapping is used for the time being.
     *
     * The correct DC number is produced by applying an offset based on the
     * DDC32 number. Then one only needs to determine the correct individual
     * DC number. This is explained in the next paragraph.
     *
     * According to the TDR there are 6 DC boards for 6 DE boards which have
     * 16 DDC32 boards plugged in. Therefore the mapping from DDC32 to DC is
     * 16 into 6. The only way to do this is by having 3 DDC32 boards per
     * DE (i.e. DC if one makes a direct mapping). Therefore the mapping will
     * use multiples of 3.
     *
     * Counting of boards starts at 0. There are 14 DC boards so the range of
     * board numbers is 0-13. To be clear, the algorithms will calculate the
     * DC number in the range 1-14. This number is then decremented so that it
     * is an index number.
     *
     * Contrast to getDDC32Number which uses indices 0-15 and outputs 1-16.
     */

    int TPCLGOffset = 0;
    int TPCHGOffset = 6;
    int SKIOffset = 12;
    int OutOffset = 13;

    int DDC32perDC = 3;
    int DC = 0;

    //std::cout << "DDC32Num " << DDC32Num << std::endl;
    //std::cout << "chan " << chan << std::endl;

    if (DDC32Num >= 0 && DDC32Num <= 15)
    {
        //First set of 16 DDC32 boards, i.e. TPC PMT LG
        DC = (int)(floor(DDC32Num / DDC32perDC) + TPCLGOffset);
    }
    else if (DDC32Num >= 16 && DDC32Num <= 31)
    {
        //Second set of 16 DDC32 boards, i.e. TPC PMT HG
        DC = (int)(floor((DDC32Num - 16) / DDC32perDC) + TPCHGOffset);
    }
    else if (DDC32Num >= 32 && DDC32Num <= 36)
    {
        //Set of 5 DDC32 boards, i.e. Skin PMT
        DC = (int)(floor((DDC32Num - 32) / DDC32perDC) + SKIOffset);
    }
    else if (DDC32Num >= 37 && DDC32Num <= 44)
    {
        //Last set of 8 DDC32 boards, i.e. Outer PMTs
        DC = (int)(floor((DDC32Num - 48) / DDC32perDC) + OutOffset);
    }
    else
    {
        //std::cout << "Warning: A DC board number "
        //<< "could not be found/assigned for "
        //<< "channel " << chan << " which is on DDC32 board " << DDC32Num
        //<< "." << std::endl;
    }

    //Numbering goes from 0-44, i.e. there are 45 digitisers.

    //std::cout << "DC " << DC << std::endl;

    return (DC < 1) ? 0 : DC;
}

unsigned short getNDigisInDC(const int& DC)
{
    /**
     * Lookup how many digitisers there should be on a given DC board.
     */
    switch (DC)
    {
    case 0:
        return 3;
        break;
    case 1:
        return 3;
        break;
    case 2:
        return 3;
        break;
    case 3:
        return 3;
        break;
    case 4:
        return 3;
        break;
    case 5:
        return 1;
        break;
    case 6:
        return 3;
        break;
    case 7:
        return 3;
        break;
    case 8:
        return 3;
        break;
    case 9:
        return 3;
        break;
    case 10:
        return 3;
        break;
    case 11:
        return 1;
        break;
    case 12:
        return 6;
        break;
    case 13:
        return 8;
        break;
    default:
        return 0;
        break;
    }

    //std::cout << "The number of digitisers could not be found "
    //<< "for the requested DC number " << DC << std::endl;
    //std::cout << "Default to 0." << std::endl;
    return 0;
}
