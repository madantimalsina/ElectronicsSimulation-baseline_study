//
//  Input.cpp
//  devices
//
//  Created by Cees Carels on 24/10/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "Input.hpp"

Input::Input()
{
    /**
     * Constructor for Input.
     */
    PhotonTimeLimit = 120000000; //Manual limit on photon times
    if (global::config->getConfig("PostWindow") == "AUTO")
    {
        std::cout << "NOTICE: PostWindow is AUTO" << std::endl;
        PostTriggerWindow = PhotonTimeLimit;
        std::cout << "Limit is " << PhotonTimeLimit << " [ns]." << std::endl;
    }
    else
    {
        try
        {
            PostTriggerWindow = std::stod(global::config->getConfig("PostWindow")) * 1.0e6;
        }
        catch (...)
        {
            std::cout << "ERROR: Could not set PostTriggerWindow in Input::Input()" << std::endl;
            std::cout << "A user input error is assumed." << std::endl;
            std::cout << "You continue at your own risk." << std::endl;
        }
    }
    std::cout << "NOTICE: PostWindow is " << (unsigned long long)PostTriggerWindow << " [ns]" << std::endl;
}

Input::~Input()
{
    /**
     * Destructor for Input.
     */
}

void Input::setFile(const std::vector<std::string> path)
{
    /**
     * Set the file path (file directory + file name + extension).
     *
     * For example: /myfolder/inputfile.root
     */
    filePath = path;
}

bool Input::ComparePMTData(PMTData const& lhs,
    PMTData const& rhs)
{
    /**
     * Function used to logically compare the PMTData struct
     * such that it can be sorted in increasing order: first by event, then by
     * PMT ID, and then by the arrival time.
     *
     * Returns the lesser of the compared quantities (lhs or rhs).
     */
    if (lhs.EventID != rhs.EventID)
        return lhs.EventID < rhs.EventID;
    if (lhs.id != rhs.id)
        return lhs.id < rhs.id;
    return lhs.time < rhs.time;
}

int Input::checkEvtList()
{
    /**
     * Return -1 if error, 0 if ok.
     */
    for (int i = 0; i < SelectedEvents.size(); i++)
    {
        if (!EventIsContained((int)SelectedEvents[i], AvailableEvents))
        {
            std::cout << "ERROR: Invalid event selection." << std::endl;
            std::cout << "Exiting..." << std::endl;
            return -1;
        }
    }

    return 0;
}

bool Input::EventIsContained(const int ID,
    const std::vector<unsigned long long>& FindIn)
{
    /**
     * Supply an event ID and a vector of event IDs
     * Return true if the supplied ID is contained in the vector of IDs
     *
     * Return false otherwise
     */

    if (FindIn.capacity() > 0)
    {
        for (int i = 0; i < FindIn.size(); i++)
        {
            if (FindIn[i] == ID)
            {
                return true;
            }
        }
    }

    return false;
}

std::string Input::getRandomSeedFromFileName()
{
    /**
     * The input file name format contains the random seed used for the LUXSim
     * simulation.
     *
     * This method retrieves it from the supplied filename, and then returns it.
     */

    std::string totalseed = "";
    for (int i = 0; i < filePath.size(); i++)
    {
        std::string filename = filePath[i];
        std::string seed;
        size_t first = filename.find("_");
        if (first == std::string::npos)
        {
            std::cout << "Warning: Random seed delimiter could not be found."
                      << std::endl;
        }
        else
        {
            size_t last = filename.find(".");
            seed = filename.substr(first, last - first);
            std::string separator = "_";
            for (int i = 0; i < separator.size(); i++)
            {
                seed.erase(std::remove(seed.begin(),
                               seed.end(),
                               separator[i]),
                    seed.end());
            }
        }

        if (seed.length() == 0)
        {
            std::cout << "Warning: Seed not found. Set to EMPTY." << std::endl;
            seed = "EMPTY";
        }
        totalseed += seed;
        if ((i + 1) < filePath.size())
            totalseed += "_"; //Append if more seeds.
    }

    return totalseed;
}

std::vector<unsigned long long> Input::makeEvtList()
{
    /**
     * Make a vector of event numbers from the configuration setting in
     * DERCONFIG.txt
     *
     * Format: #-#' where #, #', and all events between them are included also.
     *         #, #' where only #, #' are included
     *         #-#', #"-#'" where both ranges are included.
     *         and combinations of the above.
     *
     * Output: vector of events.
     *
     * Returns a list of numbers representing the events chosen in DERCONFIG.txt
     */

    std::string s = global::config->getConfig("SelectedEvents");
    std::string delimiter = "-";
    std::string commaDelimiter = ",";
    std::string part;
    std::string token;
    size_t subbegin = 0;
    int delimiterCount = 0;
    size_t len = 0;
    size_t pos = 0;

    s.erase(std::remove(s.begin(), s.end(), ' '), s.end()); //removing any white spaces
    size_t length = s.length();

    std::set<unsigned long long> evtList;
    std::vector<std::string> subsets;

    for (std::size_t i = 0; i < length; i++)
    {
        if (s[i] == commaDelimiter)
        {
            len = i - subbegin;
            part = s.substr(subbegin, len);
            subbegin = (i + 1);
            subsets.push_back(part);
        }
        part.clear();
        part.shrink_to_fit();
    }

    len = length - subbegin;
    part = s.substr(subbegin, len);
    subsets.push_back(part);

    for (int i = 0; i < subsets.size(); i++)
    {
        delimiterCount = 0;
        pos = 0;
        token.clear();
        token.shrink_to_fit();

        subsets[i].erase(std::remove(subsets[i].begin(), subsets[i].end(), ','), subsets[i].end());

        while ((pos = subsets[i].find(delimiter)) != std::string::npos)
        {
            token = subsets[i].substr(0, pos);
            subsets[i].erase(0, pos + delimiter.length());
            ++delimiterCount;
        }

        if (delimiterCount == 0)
        {
            evtList.insert(stoi(subsets[i]));
        }
        else
        {
            for (int k = std::stoi(token); k <= std::stoi(subsets[i]); k++)
            {
                evtList.insert(k);
            }
        }
    }

    std::vector<unsigned long long> finalEvtList(evtList.begin(), evtList.end());
    std::sort(finalEvtList.begin(), finalEvtList.end());
    return finalEvtList;
}

bool Input::PMTwasFound(const std::vector<int>& vec, const int& val)
{
    if (std::find(vec.begin(), vec.end(), val) != vec.end())
    {
        return true;
    }
    else
        return false;
}

bool Input::toBool(const std::string& conf)
{
    /**
     * Configuration settings loaded from the DERCONFIG.txt configuration file
     * are read as std::string variables.
     *
     * This method converts the string to a bool.
     *
     * Returns true if the string reads "true" and false otherwise.
     */

    if (conf == "true")
        return true;
    else
        return false;
}

unsigned long long Input::getAvailEvtsAt(const unsigned long long it)
{
    return AvailableEvents[it];
}

unsigned long long Input::getSelecEvtsAt(const unsigned long long it)
{
    return SelectedEvents[it];
}

unsigned long long Input::getAvailEvtsSize()
{
    return AvailableEvents.size();
}

unsigned long long Input::getSelecEvtsSize()
{
    return SelectedEvents.size();
}

unsigned long long Input::PMTDataSize()
{
    return PmtData.size();
}

unsigned long long Input::getAvailPMTat(const unsigned long long it)
{
    return AvailablePMTs.at(it);
}

unsigned long long Input::getAvailPMTback()
{
    return AvailablePMTs.back();
}

Input::PMTData* Input::PMTDataAt(const unsigned long long it)
{
    return &PmtData[it];
}

unsigned long Input::getNPMTs()
{
    return AvailablePMTs.size();
}

unsigned long Input::getNExpectedPMTs()
{
    return ExpectedTotalPMTs;
}

unsigned long Input::getNEvts()
{
    return SelectedEvents.size();
}

unsigned long Input::getNNonEmptyEvts()
{
    /**
     * Method returns the number of non-empty events from selected event list.
     *
     * Method can only be called after event list is generated.
     */

    unsigned long cnt = 0;
    for (int i = 0; i < SelectedEvents.size(); i++)
    {
        if (std::find(EmptyEvents.begin(), EmptyEvents.end(), SelectedEvents[i]) != EmptyEvents.end())
            ++cnt;
    }
    return (SelectedEvents.size() - cnt);
}

void Input::SelectDetectors()
{
	auto config = global::config;
    TPCTopPMTs = std::stoi(config->getConfig("TPCTopPMTs"));
    TPCTopDummyPMTs = std::stoi(config->getConfig("TPCTopDummy"));
    TPCBotPMTs = std::stoi(config->getConfig("TPCBotPMTs"));
    TPCBotDummyPMTs = std::stoi(config->getConfig("TPCBotDummy"));
    TPCPMTs = TPCBotPMTs + TPCTopPMTs; //+ Dummy PMTs
    SkinTPMTs = std::stoi(config->getConfig("SkinTPMTs"));
    SkinTDummy = std::stoi(config->getConfig("SkinTDummy"));
    SkinBPMTs = std::stoi(config->getConfig("SkinBPMTs"));
    SkinBDummyPMTs = std::stoi(config->getConfig("SkinBDummy"));
    SkinBDPMTs = std::stoi(config->getConfig("SkinBD"));
    SkinBDDummyPMTs = std::stoi(config->getConfig("SkinBDummy"));
    SkinPMTs = SkinTPMTs + SkinBPMTs + SkinBDPMTs;
    //+ Dummy PMTs
    OuterPMTs = std::stoi(config->getConfig("OuterPMTs"));
    OuterDummyPMTs = std::stoi(config->getConfig("OuterDummy"));

    std::size_t found = config->getConfig("IncDetectors").find("T");
    if (found != std::string::npos)
        doTPC = true;

    found = config->getConfig("IncDetectors").find("O");
    if (found != std::string::npos)
        doOut = true;

    found = config->getConfig("IncDetectors").find("S");
    if (found != std::string::npos)
        doSkin = true;

    found = config->getConfig("IncDetectors").find("A");
    if (found != std::string::npos)
    {
        doTPC = true;
        doOut = true;
        doSkin = true;
    }

    ExpectedTotalPMTs = 0;
    if (doTPC)
        ExpectedTotalPMTs += TPCPMTs;
    if (doSkin)
        ExpectedTotalPMTs += SkinPMTs;
    if (doOut)
        ExpectedTotalPMTs += OuterPMTs;
}

bool Input::checkForMissingPMTs()
{
    //Check whether number of PMTs in each bank matches the configured value
    bool MissingPMTs = false;

    //TPC Top
    if ((TPCTFound.size() != TPCTopPMTs) && doTPC)
    {
        std::cout << "Warning: There should be " << TPCTopPMTs;
        std::cout << " PMTs in the TPC top bank, ";
        std::cout << "found " << TPCTFound.size() << std::endl;
        MissingPMTs = true;
    }

    //TPC Top Dummy
    if ((TPCTDmyFound.size() != TPCTopDummyPMTs) && doTPC)
    {
        std::cout << "Warning: There should be " << TPCTopDummyPMTs;
        std::cout << " PMTs in the TPC Top Dummy bank, ";
        std::cout << "found " << TPCTDmyFound.size() << std::endl;
        MissingPMTs = true;
    }

    //TPC Bottom
    if ((TPCBFound.size() != TPCBotPMTs) && doTPC)
    {
        std::cout << "Warning: There should be " << TPCBotPMTs;
        std::cout << " PMTs in the TPC Bottom bank, ";
        std::cout << "found " << TPCBFound.size() << std::endl;
        MissingPMTs = true;
    }

    //TPC Bottom Dummy
    if ((TPCBDmyFound.size() != TPCBotDummyPMTs) && doTPC)
    {
        std::cout << "Warning: There should be " << TPCBotDummyPMTs;
        std::cout << " PMTs in the TPC Bottom Dummy bank, ";
        std::cout << "found " << TPCBDmyFound.size() << std::endl;
        MissingPMTs = true;
    }

    //Skin Top
    if ((SkiTFound.size() != SkinTPMTs) && doSkin)
    {
        std::cout << "Warning: There should be " << SkinPMTs;
        std::cout << " PMTs in the Skin Top bank, ";
        std::cout << "found " << SkiTFound.size() << std::endl;
        MissingPMTs = true;
    }

    //Skin Top Dummy
    if ((SkiTDmyFound.size() != SkinTDummy) && doSkin)
    {
        std::cout << "Warning: There should be " << SkinTDummy;
        std::cout << " PMTs in the Skin Top Dummy bank, ";
        std::cout << "found " << SkiTFound.size() << std::endl;
        MissingPMTs = true;
    }

    //Skin Bottom
    if ((SkiBFound.size() != SkinBPMTs) && doSkin)
    {
        std::cout << "Warning: There should be " << SkinBPMTs;
        std::cout << " PMTs in the Skin Bottom bank, ";
        std::cout << "found " << SkiBFound.size() << std::endl;
        MissingPMTs = true;
    }

    //Skin Bottom Dummy
    if ((SkiBDmyFound.size() != SkinBDummyPMTs) && doSkin)
    {
        std::cout << "Warning: There should be " << SkinBDummyPMTs;
        std::cout << " PMTs in the Skin Bottom Dummy bank, ";
        std::cout << "found " << SkiBDFound.size() << std::endl;
        MissingPMTs = true;
    }

    //Skin Bottom Dome
    if ((SkiBDFound.size() != SkinBDPMTs) && doSkin)
    {
        std::cout << "Warning: There should be " << SkinBDPMTs;
        std::cout << " PMTs in the Skin Bottom Dome bank, ";
        std::cout << "found " << SkiBDFound.size() << std::endl;
        MissingPMTs = true;
    }

    //Skin Bottom Dome Dummy
    if ((SkiBDDmyFound.size() != SkinBDDummyPMTs) && doSkin)
    {
        std::cout << "Warning: There should be " << SkinBDDummyPMTs;
        std::cout << " PMTs in the Skin Bottom Dome Dummy bank, ";
        std::cout << "found " << SkiBDDmyFound.size() << std::endl;
        MissingPMTs = true;
    }

    //Outer
    if ((OutFound.size() != OuterPMTs) && doOut)
    {
        std::cout << "Warning: There should be " << OuterPMTs;
        std::cout << " PMTs in the Outer bank, ";
        std::cout << "found " << OutFound.size() << std::endl;
        MissingPMTs = true;
    }

    //Outer Dummy
    if ((OutDmyFound.size() != OuterDummyPMTs) && doOut)
    {
        std::cout << "Warning: There should be " << OuterDummyPMTs;
        std::cout << " PMTs in the Outer Dummy bank, ";
        std::cout << "found " << OutDmyFound.size() << std::endl;
        MissingPMTs = true;
    }

    if (MissingPMTs)
    {
        std::cout << "PMTs are missing. ";

        if (toBool(global::config->getConfig("StopIfPMTMissing")))
        {
            std::cout << "Exiting..." << std::endl;
            return false;
        }
        else
        {
            std::cout << "You have chosen to continue." << std::endl;
        }
    }
    return true;
}

void Input::SortData()
{
    //Sort data by increasing Event, PMT, then by increasing time
    //std::sort(PmtData.begin(), PmtData.end(), &ComparePMTData);

    //Sort AvailablePMTs by increasing PMT
    std::sort(AvailablePMTs.begin(), AvailablePMTs.end());

    //Sort AvailableEvents by increasing Event
    std::sort(AvailableEvents.begin(), AvailableEvents.end());
}

TChain* Input::getDataTChain()
{
    /**
     * Returns TChain to input stream when the input class uses multiple files.
     *
     * Currently, only RootOutputJul2017 and later support using this feature.
     */

    return data;
}

void Input::makeEvtListFromSelection()
{
    //Analyse all events or just selection
    bool doAnalyseAll = toBool(global::config->getConfig("ProcessAllEvts"));

    if (doAnalyseAll)
    {
        //Input method makes event list
        for (int i = 0; i < getAvailEvtsSize(); i++)
        {
            SelectedEvents.push_back(getAvailEvtsAt(i));
        }
    }
    else
        SelectedEvents = makeEvtList();

    checkEvtList();
}

void Input::getBaccObj()
{
    /**
     * Method intentionally left blank.
     */
}

int Input::makePMTDataReady(const unsigned long long& Evt,
    unsigned long long& NPhotons,
    unsigned long long& TMin,
    unsigned long long& TMax,
    unsigned long long& NVertices,
    std::vector<int>& PmtsInEvt)
{
    /**
     * Default implementation required by interface.
     */
    return 1;
}

double Input::getEventFirstPhotonTime(unsigned long long evt)
{
    /**
     * Default implementation required by interface.
     */
    return 1;
}
