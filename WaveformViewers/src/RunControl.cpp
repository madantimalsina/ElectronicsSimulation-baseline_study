/*!\mainpage LZ Detector Electronics Response Simulation
 *
 * \section intro_sec Introduction
 *
 * This documentation summarises the content of the simulation,
 * which is the detector electronics
 * response software for the LZ Experiment.
 *
 * \section install_sec Installation and Usage
 *
 * \subsection step1 Step 1: Download the files
 * The files can be obtained from:
 * https://lz-git.ua.edu/sim/ElectronicsSimulation
 *
 * \subsection step2 Step 2: Compile
 * The software can be compiled using the supplied makefile.
 *
 * \subsection step3 Step 3: Run
 * Run using the format: ./DER [afile.root]
 *
 * \section further_docs Further Documentation
 * More detailed documentation can be found on the LZ TWiki pages.
 *
 * Please also refer to the requirements table at [...].
 *
 * \section support_sec Contact and Support
 * If you notice any errors or have suggestions,
 * please contact cees.carels@physics.ox.ac.uk.
 *
 * \section Author
 * Created by Cees Carels on 01/01/2016.
 * Copyright © 2016-2017 Cees Carels. All rights reserved.
 */

#include "RunControl.hpp"
#include "Config.hpp"
#include "Digitizer.hpp"
#include "PMT.hpp"

namespace RunControl
{

void parse_user_inputs(int argc, char** argv, std::vector<std::string>& input_files, std::string& inFilename,
    std::shared_ptr<DBInterface<std::string> > config)
{

    // Override Config settings with CLI settings

    if (argc == 1)
    {
        std::stringstream err_msg{ "" };
        err_msg << "No input parameters were supplied." << std::endl;
        displayHelp();
        err_msg << "Exiting..." << std::endl;
        throw std::runtime_error(err_msg.str());
    }

    if (argc == 2)
    {
        if (!strcmp("-h", argv[1]) || !strcmp("--help", argv[1]))
        {
            displayHelp();
            throw std::runtime_error("Exiting...");
        }
    }

    std::string inputfile = argv[1];

    int args;
    bool DERCONFIGPATHOverride = false;
    const std::string kDER_ROOT(std::getenv("DER_ROOT"));
    std::string path = kDER_ROOT + "/DERCONFIG.txt";
    (argc % 2 == 0) ? args = 1 : args = 0; // Set args to loop over arguments.
    for (int i = 0; i < argc - args; i++)
    {
        std::string CurrArg = argv[i];
        if (CurrArg == "--DERCONFIGPath")
        {
            if (i + 1 < argc)
                path = argv[i + 1];
            else
            {
                throw std::runtime_error("Invalid input format, exiting...");
            }
            std::cout << "Setting DERCONFIG path to " << path << std::endl;
            DERCONFIGPATHOverride = true;
        }
    }

    config->setConfig(path.c_str()); // Directory of config file
    if (!config->isOK())
    {
        throw std::runtime_error("Unable to load config: " + path);
    }

    if ((argc > 2 && !DERCONFIGPATHOverride) || (argc > 3 && DERCONFIGPATHOverride))
    {
        std::cout << "Overriding DERCONFIG settings with CLI inputs." << std::endl;
    }

    bool sourceFound = false;

    for (int i = 1; i < argc - args; i += 2)
    {
        std::string currSID = argv[i];
        currSID.erase(0, 2);

        if (currSID == "DERCONFIGPath")
        {
            continue;
        }

        if (currSID.compare("source") == 0)
        {
            inFilename = argv[i + 1];
            input_files.push_back(inFilename);
            sourceFound = true;
            continue;
        }

        std::string CurrVal = argv[i + 1];

        if (config->CLISet(currSID, CurrVal) == -1)
        {
            throw std::runtime_error("RunControl: Unable to set Config::SID");
        }
    }

    if (!sourceFound)
    {
        inFilename = argv[argc - 1];
        input_files.push_back(inFilename);
    }

    if (input_files.size() == 0)
    {
        std::stringstream err_msg{ "" };
        err_msg << "No input files were specified." << std::endl;
        err_msg << "Exiting..." << std::endl;
        throw std::runtime_error(err_msg.str());
    }
}

void set_random_number_seeds(std::string& random_seed, global::ConfigPtr config)
{
    // Set random number generator seed
    if (config->getConfig("RandomNumberSeed") == "AUTO")
    {
        std::cout << "NOTICE: Random number generator seed set using POSIX time." << std::endl;
        unsigned int seed = (unsigned int)time(NULL);
        srand(seed);
        random_seed = std::to_string(seed);
        gRandom->SetSeed(seed);
        std::cout << "NOTICE: Random number seed set to " << seed << std::endl;
        if (config->CLISet("RandomNumberSeed", std::to_string(seed)) == -1)
        {
            std::cout << "Error setting CLISet RandomNumberSeed" << std::endl;
        }
    }
    else
    {
        std::string sseed = config->getConfig("RandomNumberSeed");
        unsigned int seed = 0;
        try
        {
            seed = (unsigned int)std::stoul(sseed);
        }
        catch (...)
        {
            std::cout << "ERROR: Caught exception while setting random number generator seed." << std::endl;
            std::cout << "It is likely that you set a seed that caused a buffer underflow or overflow." << std::endl;
        }
        // Convert seed back to string, to check for overflow or underflow
        if (std::to_string(seed) != sseed)
        {
            std::stringstream err_msg{ "" };
            err_msg << "RunControl: User input seed outside permissible range." << std::endl;
            err_msg << "Please check seed for overflow/underflow." << std::endl;
            err_msg << "Seed (input): " << sseed << " != " << seed << std::endl;
            err_msg << "The DER will now exit." << std::endl;
            throw std::runtime_error(err_msg.str());
        }
        srand(seed);
        random_seed = std::to_string(seed);
        gRandom->SetSeed(seed);
        if (seed == 0)
        {
            std::cout << "WARNING: Setting seed to 0 does not set ROOT-based "
                      << "random number generator to a fixed seed." << std::endl;
            std::cout << "ROOT-based random number generators are "
                      << "still random, but rand() generators will be seeded with the seed "
                      << "that you have provided." << std::endl;
            std::cout << "You have chosen to continue at your own risk." << std::endl;
        }
        std::cout << "Random number generator seed set to " << seed << std::endl;
        if (config->CLISet("RandomNumberSeed", std::to_string(seed)) == -1)
        {
            std::cout << "Error setting CLISet RandomNumberSeed" << std::endl;
        }
    }
}

void set_pmt_parameters(DBInterfaceFactory<double>& pmtParamsFactory,
    std::shared_ptr<DBInterface<double> > pmtCsvParams, global::ConfigPtr config)
{
    // Read PMT parameters file
    pmtCsvParams = pmtParamsFactory.createDB(format::config::CSV);
    pmtCsvParams->setConfig(config->getConfig("PMTParamsPath"));
    int maxPmtNumber = 921;
    if (!pmtCsvParams->isOK())
    {
        std::cout << "Could not open file for PMT Parameters" << std::endl;
        int allPMTs = std::stoi(config->getConfig("TPCTopPMTs")) + std::stoi(config->getConfig("TPCBotPMTs"))
            + std::stoi(config->getConfig("SkinTPMTs")) + std::stoi(config->getConfig("SkinBPMTs"))
            + std::stoi(config->getConfig("OuterPMTs"));
        std::vector<std::string> PMTParameterList = { "QE", "CE", "DPHE", "DarkCount", "Gain", "PMTGain", "fGain",
            "sGain", "SpheRes", "fRes", "sRes", "TransitTime", "TTS", "FTransitTime", "FTTS", "STransitTime", "STTS" };
        std::cout << "NOTICE: Generating default PMT parameters for " << allPMTs << " PMTs" << std::endl;
        pmtCsvParams->setConfigSize(PMTParameterList, maxPmtNumber);
        // Implement the case where there is no PMT parameter file.
        for (int pmtNumber = 0; pmtNumber < maxPmtNumber; pmtNumber++)
        {
            pmtCsvParams->setConfig("QE", pmtNumber, std::stod(config->getConfig("PMTQE")));
            pmtCsvParams->setConfig("CE", pmtNumber, std::stod(config->getConfig("CE")));
            pmtCsvParams->setConfig("DPHE", pmtNumber, std::stod(config->getConfig("2PheProb")));
            pmtCsvParams->setConfig("DarkCount", pmtNumber, std::stod(config->getConfig("DarkRate")));
            pmtCsvParams->setConfig("Gain", pmtNumber, 1754531.25);
            pmtCsvParams->setConfig("PMTGain", pmtNumber, 3.5e6);
            pmtCsvParams->setConfig("fGain", pmtNumber, 125323.66);
            pmtCsvParams->setConfig("sGain", pmtNumber, 350906.25);
            pmtCsvParams->setConfig("SpheRes", pmtNumber, 0.3);
            pmtCsvParams->setConfig("fRes", pmtNumber, 0.08);
            pmtCsvParams->setConfig("sRes", pmtNumber, 0.13);
            pmtCsvParams->setConfig("TransitTime", pmtNumber, 42.0);
            pmtCsvParams->setConfig("TTS", pmtNumber, 3.68);
            pmtCsvParams->setConfig("FTransitTime", pmtNumber, 12.0); // 12.0; //[ns]
            pmtCsvParams->setConfig("FTTS", pmtNumber, 1.27); // 1.27; //[ns]
            pmtCsvParams->setConfig("STransitTime", pmtNumber, 36.0); // 36.0; //[ns]
            pmtCsvParams->setConfig("STTS", pmtNumber, 3.6); // 3.6;  //[ns]
        }
        std::cout << "NOTICE: Finished setting default PMT Parameter values." << std::endl;
    }
    PMT::setParamPointer(pmtCsvParams);

    // Check that QE/BaccQEFactor is never bigger than 1.
    double baccQEFactor = std::stof(config->getConfig("BaccQEFactor"));
    for (int pmtNumber = 0; pmtNumber < maxPmtNumber; pmtNumber++)
    {
        if (pmtCsvParams->getConfig("QE", pmtNumber) > baccQEFactor)
        {
            std::stringstream err_msg{ "" };
            err_msg << "ERROR: QE/BaccQEFactor for PMT " << pmtNumber << " is bigger than 1." << std::endl;
            err_msg << "It is likely that a change to BACCARAT is the cause of this behaviour." << std::endl;
            err_msg << "Exiting..." << std::endl;
            throw std::runtime_error(err_msg.str());
        }
    }
}

void set_trigger_parameters(global::ConfigPtr config)
{
    std::stringstream err_msg{ "" };
    // Check POD pre-, post-, and inter- sample settings from DERCONFIG
    try
    {
        int pretr = std::stoi(config->getConfig("PreTrigger"));
        int postr = std::stoi(config->getConfig("PostTrigger"));
        int inter = std::stoi(config->getConfig("IntrPodTime"));

        if ((pretr + postr) > inter)
        {
            std::cout << "WARNING: Setting sum of pre- and post- triggers "
                      << "greater than the inter- POD sample setting may result "
                      << "in PODs with overlapping samples." << std::endl;
            std::cout << "Continue at your own risk." << std::endl;
        }

        if (pretr <= 0 || postr <= 0 || inter <= 0)
        {
            err_msg << "RunControl: PreTrigger, PostTrigger, "
                    << "and IntrPodTime must all be greater than 0." << std::endl;
            err_msg << "Exiting..." << std::endl;
            throw std::runtime_error(err_msg.str());
        }
    }
    catch (...)
    {
        err_msg << "RunControl: Exception caught while trying to determine POD "
                << "pre- post- and inter- sample settings." << std::endl;
        err_msg << "Exiting..." << std::endl;
        throw std::runtime_error(err_msg.str());
    }
}

void set_data_collector_events(global::ConfigPtr config)
{
    std::stringstream err_msg{ "" };
    try
    {
        int evtindc = std::stof(config->getConfig("NewDCSeriesEvt"));
        if (evtindc <= 0)
        {
            err_msg << "ERROR: NewDCSeriesEvent must be >= 0." << std::endl;
            err_msg << "Exiting..." << std::endl;
            throw std::runtime_error(err_msg.str());
        }
    }
    catch (...)
    {
        err_msg << "ERROR: Exception caught when checking setting: NewDCSeriesEvent" << std::endl;
        err_msg << "It is likely you have entered invalid input" << std::endl;
        err_msg << "Exiting..." << std::endl;
        throw std::runtime_error(err_msg.str());
    }
}

void check_file_permissions(global::ConfigPtr config)
{
    if (access(config->getConfig("outDir").c_str(), W_OK) == -1)
    {
        std::stringstream err_msg{ "" };
        err_msg << "RunControl: File permissions not ok: " << std::strerror(errno) << std::endl;
        throw std::runtime_error(err_msg.str());
    }
}

void check_pmts_and_events(Input*& input, global::ConfigPtr config)
{
    std::stringstream err_msg{ "" };
    if (input->makePMTDataReady() == -1)
    {
        err_msg << "ERROR: PMT data could not be made ready." << std::endl;
        err_msg << "Exiting..." << std::endl;
        throw std::runtime_error(err_msg.str());
    }

    if (input->getNPMTs() != input->getNExpectedPMTs())
    {
        std::cout << "WARNING: Expect " << input->getNExpectedPMTs() << " PMTs,"
                  << " but " << input->getNPMTs() << " were found." << std::endl;

        if (toBool(config->getConfig("StopIfPMTMissing")))
        {
            throw std::runtime_error("Exiting...");
        }
        else
        {
            std::cout << "You have chosen to continue." << std::endl;
        }
    }

    if (input->getNEvts() == 0)
    {
        err_msg << "ERROR: Simulation input contains no events" << std::endl;
        err_msg << "Exiting..." << std::endl;
        throw std::runtime_error(err_msg.str());
    }

    if (input->checkEvtList() == -1)
    {
        err_msg << "ERROR: Simulation input event list error." << std::endl;
        err_msg << "Exiting..." << std::endl;
        throw std::runtime_error(err_msg.str());
    }
}

void setup_input(Input*& input, std::vector<std::string>& input_files)
{

    input->setFile(input_files);
    if (!input->Open())
    {
        throw std::runtime_error("RunControl: Unable to open input file");
    }
    input->SelectDetectors();
}

void setup_output(Output*& output, format::revision formatVersion, bool& outputIsBinary, global::ConfigPtr config)
{
    std::size_t derExtension = 0;

    // Decide on output format (.bin, .root) and create output
    outputIsBinary = false;
    derExtension = config->getConfig("DERExt").find(".root");
    if (derExtension != std::string::npos)
    {
        // The output will be a .root file
        try
        {
            output = OutputFactory::getOutput(formatVersion);
        }
        catch (std::bad_alloc& caught)
        {
            std::cerr << "bad_alloc caught: " << caught.what() << std::endl;

            std::cout << "Caught while creating RootOutput" << std::endl;
        }
    }

    derExtension = config->getConfig("DERExt").find(".bin");
    std::stringstream err_msg{ "" };
    if (derExtension != std::string::npos)
    {
        try
        {
            // The output will be a .bin file
            output = OutputFactory::getOutput(format::revision::BINv1);
            outputIsBinary = true;
        }
        catch (std::bad_alloc& caught)
        {
            err_msg << "bad_alloc caught: " << caught.what() << std::endl;
            err_msg << "Caught while creating BinaryOutput" << std::endl;
            throw std::runtime_error(err_msg.str());
        }
    }

    if (output == 0)
    {
        err_msg << "ERROR: Output could not be set up." << std::endl;
        err_msg << "Exiting..." << std::endl;
        throw std::runtime_error(err_msg.str());
    }
}

void setup_time_stamp(Output*& output, unsigned long& realPosixTime, unsigned long& posixTime, std::string& localTime,
    global::ConfigPtr config)
{
    // UserTimeStamp is used for filename and trigger timestamps
    // It is equal to the current time unless it is overriden here
    // Get Posix and LocalTime
    std::time_t time = std::time(nullptr); // Current time (PosixTime (UTC))
    posixTime = time; // PosixTime of production (UTC)
    localTime = std::asctime(std::localtime(&time)); // Local time of production (computer time)

    time_t userTimeStamp = time;
    realPosixTime = posixTime;
    if (config->getConfig("FileTimeStamp") != "AUTO")
    {
        std::cout << "NOTICE: Overriding current POSIX time with " << config->getConfig("FileTimeStamp") << std::endl;
        userTimeStamp = (time_t)std::stoull(config->getConfig("FileTimeStamp"));
        posixTime = (unsigned long)userTimeStamp;
        output->setTime(userTimeStamp);
    }
    else
    {
        output->setTime(time);
    }
}

void user_check(DERSummary& theDERSummary, global::ConfigPtr config)
{
    if (toBool(config->getConfig("UserCheck")) && theDERSummary.getDERUserGroupID() != "lz")
    {
        std::stringstream err_msg{ "" };
        err_msg << "WARNING: Non-LZ user group detected in calling process." << std::endl;
        err_msg << "Please run DER as a user of the lz user group." << std::endl;
        err_msg << "Exiting..." << std::endl;
        throw std::runtime_error(err_msg.str());
    }
}

void setup_pulse_reader(PulseReader& theReaderPulses, global::ConfigPtr config)
{
    std::stringstream err_msg{ "" };
    // PE Reader Module
    if (config->getConfig("SignalChain") == "SAMPLED")
    {
        if (!toBool(config->getConfig("UsePEReader")))
        {
            err_msg << "WARNING: 'SignalChain' is set to: " << config->getConfig("SignalChain")
                    << ", but 'UsePEReader' is set to " << config->getConfig("UsePEReader") << "." << std::endl;

            err_msg << "Please set PEReader to true,"
                    << " or use a different signal chain." << std::endl;

            err_msg << "Exiting..." << std::endl;
            throw std::runtime_error(err_msg.str());
        }
        else
        {
            if (theReaderPulses.doReadPulses() == 0)
            {
                std::cout << "NOTICE: Pulses read successfully from " << config->getConfig("PEReaderBinPath") << "."
                          << std::endl;
                theReaderPulses.setSampledGain(std::stod(config->getConfig("SampledPMTGain")));
                theReaderPulses.setUserGain(std::stod(config->getConfig("UserGain")));
                theReaderPulses.RescalePulses();
                if (config->getConfig("SmplSrc") != "DER")
                {
                    theReaderPulses.SubtractBaseline();
                }
            }
            else
            {
                err_msg << "ERROR: An error occurred while reading file " << config->getConfig("PEReaderBinPath") << "."
                        << std::endl;

                err_msg << "The file could not be opened or does not exist." << std::endl;

                err_msg << "Exiting..." << std::endl;
                throw std::runtime_error(err_msg.str());
            }
        }
    }
}

void setup_summary(Input*& input, Output*& output, DERSummary& theDERSummary,
    const std::vector<std::string>& input_files, unsigned long& posixTime, unsigned long& realPosixTime,
    std::string& fileRandomSeed, std::string& derRandomSeed, std::string& localTime, EBGlobal& ebGlobal,
    int& dataCollectors, int& channelsPerDDC32, int& ddc32Boards, int& samplingRate_ns, unsigned int& runNumber,
    global::ConfigPtr config)
{
    // Set up the data collector information
    dataCollectors = std::stoi(config->getConfig("DataCollectors"));
    channelsPerDDC32 = std::stoi(config->getConfig("ChanPerDDC32"));
    ddc32Boards = std::stoi(config->getConfig("DDC32Boards"));
    fileRandomSeed = input->getRandomSeedFromFileName();
    //[ns] Sampling Rate of DAQ
    output->setNumDCs(dataCollectors, channelsPerDDC32); // Inform to create a file for each DC
    output->setSimSource(input_files);
    unsigned short FormatVersionID = std::stoi(config->getConfig("FormatVersioID"));
    runNumber = std::stoi(config->getConfig("SimRunNumberID"));
    ebGlobal.setFormatVersion(FormatVersionID);
    ebGlobal.setRunNumber(runNumber);
    ebGlobal.setRunStartTime(
        (unsigned long long)posixTime + std::stoull(config->getConfig("UTCLeapSeconds"))); // GPS Time in seconds
    ebGlobal.setRunType(1);
    output->setRunNumber(runNumber);
    output->setFileSeq(config->getConfig("fileSeqNum"));
    std::string outDirectory = config->getConfig("outDir");
    std::string slash = "/";
    if (outDirectory.at(outDirectory.length() - 1) != slash)
    {
        outDirectory.append(slash);
        std::cout << "NOTICE: The following path was assumed to be a directory: " << outDirectory << std::endl;
    }
    output->createOutFile(false, outDirectory);
    output->doInputTreeClone(input->getFilePointer());
    std::cout << "DER Output File (base name and path): " << output->getOutFileName() << std::endl;

    theDERSummary.setUserName(input->getUserName());
    theDERSummary.setDERUserGroupID();
    theDERSummary.setDERUserName();
    theDERSummary.setDERHostName();
    std::string CurrConfig = config->getALL();
    theDERSummary.setDERCONFIG(CurrConfig);
    theDERSummary.setFileName(input_files);
    theDERSummary.setDERFileName(output->getOutFileName().c_str());
    theDERSummary.setFileRandomSeed(fileRandomSeed);
    theDERSummary.setDERRandomSeed(derRandomSeed);
    theDERSummary.setPosixTime(std::to_string(realPosixTime));
    theDERSummary.setLocalTime(localTime);
    samplingRate_ns = std::stoi(config->getConfig("SmplRate"));
    theDERSummary.setSamplingRate_ns(samplingRate_ns);
    theDERSummary.setDERRunID(config->getConfig("DERRunID"));
    theDERSummary.setGitCommitHash(GitHash);
}

void setup_pmt_start_int(Input*& input)
{
    unsigned long pmtStartInt = input->getAvailPMTat(0);

    if (pmtStartInt > input->getAvailPMTback())
    {
        std::stringstream err_msg{ "" };
        err_msg << "ERROR: Start PMTIndex out of range" << std::endl;
        err_msg << "Exiting..." << std::endl;
        throw std::runtime_error(err_msg.str());
    }
}

void print_user_info(DERSummary& theDERSummary)
{
    std::cout << "DER User:\t" << theDERSummary.getDERUserName() << " (" << theDERSummary.getDERUserGroupID() << ")"
              << " - " << theDERSummary.getDERHostName() << std::endl;
}

void write_summary_objects(
    Input*& input, Output*& output, DERSummary* theDERSummary, EBSummary* theEBSummary, EBGlobal* ebGlobal)
{
    theEBSummary->setEndFlag(0);
    theEBSummary->setNEvtsFile(input->getNNonEmptyEvts());
    output->doWriteSummary(*theEBSummary);
    output->doWriteDERSummary(*theDERSummary);
    output->doWriteGlobal(ebGlobal);
}

void displayHelp()
{

    std::cout << "Help and Usage:" << std::endl;
    std::cout << "     ./DER --setting value /path/to/inputfile.root" << std::endl;
    std::cout << "     ./DER or ./DER -h for help" << std::endl;
}

bool toBool(const std::string& conf)
{

    if (conf == "true")
        return true;
    else
        return false;
}

void print_welcome_text()
{
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "-- Detector Electronics Response --" << std::endl;
    std::cout << "Contact: https://luxzeplin.slack.com/messages/C04UMD0BM" << std::endl;
    std::cout << "Starting DER" << std::endl;
    std::cout << std::endl;
}

void set_number_of_threads(global::ConfigPtr config)
{
    int numThreads = std::stoi(config->getConfig("NCores")) * std::stoi(config->getConfig("Thread"));

    if (numThreads > 16)
    {
        std::cout << "Warning: Using " << numThreads << " threads may upset IT administrators." << std::endl;
    }
}

void acquire_and_process_data(
    Input* input, Output* output, DDC32& testDCs, EBSummary* theEBSummary, global::ConfigPtr config)
{

    std::vector<TStopwatch> timers(4, TStopwatch());
    timers[0].Start();

    double cumulativeRealTimes[4] = {};
    double cumulativeCPUTimes[4] = {};
    unsigned long totalPhotons = 0;

    unsigned int firstDoubleGainStage = 0;
    DeviceVectors electronics = setup_analogue_electronics(firstDoubleGainStage, config);

    bool useMCTruth = (config->getConfig("DERMCTruthInfo").c_str() == "true" ? true : false);
    useMCTruth = (electronics[0][0]->getName() == "PMT" ? true : false);

    bool fillStagePulses = (config->getConfig("GenerateStageData") == "true" ? true : false);
    bool useS2Trigger = (config->getConfig("UseS2Trigger") == "true" ? true : false);
    bool writeRawData = (config->getConfig("WriteRawData") == "true" ?  true : false);

    ////////////////////////////////////////////////////////////

    // 1024 is always added by default to ensure continuous pulse boundaries
    unsigned long long timeShift = 1024;
    unsigned long long timeShiftInc = 0;
    std::stringstream err_msg{ "" };
    try
    {
        timeShift += std::stoull(config->getConfig("TEvtOffset")); //[ns] time shift for all times
        timeShiftInc = std::stoull(config->getConfig("TEvtOffsetInc"));
    }
    catch (...)
    {
        std::cout << "ERROR: Caught exception while trying to set "
                  << " TEvtOffset and TEvtOffsetInc " << std::endl;
        std::cout << "Please ensure you have set valid values" << std::endl;
        std::cout << "Exiting..." << std::endl;
        throw std::runtime_error(err_msg.str());
    }

    //////////////

    std::cout << "Monomodal Event Acquisition - Jan2018/Feb2018/Mar2018" << std::endl;
    unsigned long long detectorEventCounter = 0;

    // Set timestamp offset in 10 ns samples
    unsigned long interEvtDeltaTS = 0;
    if (std::stod(config->getConfig("InterEvtDeltaTS")) < 0)
    {
        std::runtime_error("RunControl: InterEvtDeltaTS < 0. Not allowed");
    }
    try
    {
        interEvtDeltaTS = std::stoul(config->getConfig("InterEvtDeltaTS"));
    }
    catch (...)
    {
        err_msg << "ERROR: Could not set InterEvtDeltaTS." << std::endl;
        err_msg << "A user input error is assumed." << std::endl;
        throw std::runtime_error(err_msg.str());
    }
    unsigned long triggerTime = 0;
    unsigned long previousSamples = 0;
    unsigned long long nEvents = input->getSelecEvtsSize();

    timers[0].Stop();

    for (unsigned long long k = 0; k < nEvents; k++)
    {

        timers[1].Start();

        //////////////////////////////////////////////////
        //////////////////////////////////////////////////
        // THIS METHOD MUST BE PREPARE THE QUANTITIES NEEDED TO KNOW IF
        // THE EVENT IS OK TO RUN, BUT WITHOUT LOADING PHOTONS INTO A CONTAINER YET
        // Prepare output with writing eventSeq and nPODs.
        unsigned long long nPhot = 0;
        unsigned long long tMin = 0;
        unsigned long long tMax = 0;
        unsigned long long nVert = 0;
        std::vector<int> pmtsInEvt;

        input->makePMTDataReady(input->getSelecEvtsAt(k), nPhot, tMin, tMax, nVert, pmtsInEvt);

        std::cout << progress_status(k, input->getSelecEvtsSize()) << "\r" << std::flush;

        DDC32 theDCs = testDCs;
        int samplingRate_ns = theDCs.getSamplingRate();
        EBEvent currentEvent;

        triggerTime = (unsigned long long)input->getEventFirstPhotonTime(input->getSelecEvtsAt(k)) / samplingRate_ns;
        if (k)
            triggerTime += interEvtDeltaTS; // Time between consecutive events

        currentEvent.setBufferLiveStartTS(triggerTime);
        currentEvent.setTriggerType(1);
        currentEvent.setTriggerTimeStamp(triggerTime);
        currentEvent.setTriggerMultiplicity(0); // Set to 0 for now.
        output->setTimeStamp(currentEvent);
        output->setTriggerRunNumber(std::stoi(config->getConfig("SimRunNumberID")));

        // Analyse single events in sequence.
        unsigned long long liveStop = 0;
        if (nPhot == 0)
        {
            std::cout << "Event " << input->getSelecEvtsAt(k) << " is empty." << std::endl;
            std::cout << "Skip to next event..." << std::endl;

            theEBSummary->setEndFlag(0);

            liveStop = triggerTime + previousSamples;
            triggerTime = liveStop;
            currentEvent.setBufferLiveStopTS(liveStop);

            // Data acquisition finished for event, update EBEvent
            currentEvent.setEBfromDCs(theDCs);

            detectorEventCounter += 1;
            continue;
        }
	else{
	   output->IncEvtNum();
	}
	currentEvent.setEvtSeqNumb(output->EvtNum());
        output->doPrepareEvent();
        output->doPreparePulseMCTruth(nVert, (tMax + timeShift + timeShiftInc * k));

        //---------------------
        PODContainerVector allHGPODs;
        PODContainerVector allLGPODs;
        PODContainerVectors allHGStagePODs;
        PODContainerVectors allLGStagePODs;

        FPGATrigger S2HGTrigger("S2HG");
        FPGATrigger S2LGTrigger("S2LG");
        //---------------------

        Pulse theCurrentPulse;
        PulseManager prep(theDCs.getSamplingRate());

        if (config->getConfig("SignalChain") == "SAMPLED")
        {
            output->setSignalChainIdentifier(format::SignalChain::SAMPLED);
            prep.PrepareEventBounds(theCurrentPulse, 0, (tMax + timeShift + timeShiftInc * k) * 0.1);
            //theCurrentPulse.noBasePulse();
        }
        else
        {
            output->setSignalChainIdentifier(format::SignalChain::ANALYTIC);
            prep.PrepareEventBounds(theCurrentPulse, 0, tMax + timeShift + timeShiftInc * k);
            //theCurrentPulse.constructBasePulse(2048);
        }
        timers[1].Stop();

        for (unsigned int j = 0; j < pmtsInEvt.size(); j++)
        {
            // Setup the HG and LG pulses
            timers[1].Start();
	    Pulse theHGPulse;
            Pulse theLGPulse;
	    theHGPulse.resize(theCurrentPulse.size());

	    input->getPMTData(input->getSelecEvtsAt(k), j, std::dynamic_pointer_cast<PMT>(electronics[0][0]), theCurrentPulse.size(), timeShift, timeShiftInc, k);
            theHGPulse.setChannel(pmtsInEvt[j]);
            theHGPulse.setEvent(output->EvtNum());
            theHGPulse.setLUXSimEvtNum(0, input->getSelecEvtsAt(k));
            timers[1].Stop();

            // Setup the stage pulses to be used in the PODViewer
            PulseVectors allStagePulses(
                electronics.size() - 1, std::vector<std::shared_ptr<Pulse> >(1, std::shared_ptr<Pulse>()));

            // Find the electronics response
            //---------------------------------------------------------
            timers[2].Start();
            do_analogue_electronics_response(electronics, theLGPulse, theHGPulse, allStagePulses, firstDoubleGainStage, fillStagePulses);
            timers[2].Stop();

            // Setup MCTruth object
            std::shared_ptr<MCTruth> theMCTruth;
            if (useMCTruth)
            {
                theMCTruth = std::make_shared<MCTruth>();
                electronics[0][0]->prepareMCTruth(theMCTruth);
            }

            // Create the PODs
            allHGPODs.push_back(std::move(create_pods(theHGPulse, theMCTruth, "HG", config)));
            allLGPODs.push_back(std::move(create_pods(theLGPulse, theMCTruth, "LG", config)));

            if (useS2Trigger)
            {
                run_trigger_on_pods(S2HGTrigger, allHGPODs[allHGPODs.size() - 1], allHGStagePODs);
                run_trigger_on_pods(S2LGTrigger, allLGPODs[allLGPODs.size() - 1], allLGStagePODs);
            }

	    if(fillStagePulses){
	      create_stage_pods(allHGStagePODs, allStagePulses, allHGPODs[allHGPODs.size() - 1], "HG", useS2Trigger);
	      create_stage_pods(allLGStagePODs, allStagePulses, allLGPODs[allLGPODs.size() - 1], "LG", useS2Trigger);
	    }

	    if (writeRawData && output->getOutputFormat() == format::revision::ROOTvMDC2){
	      output->doWriteRawData(theLGPulse, theHGPulse, 
				     allLGPODs[allLGPODs.size() - 1], allHGPODs[allHGPODs.size() - 1]);
	    }
            output->doWriteDERMCTruth(theMCTruth);

            previousSamples = (double)theLGPulse.size() / (double)samplingRate_ns;
        }
        theEBSummary->setEndFlag(0);

        // Build the events
        timers[3].Start();
        unsigned int numberOfHGPODs = build_event(allHGPODs, output, allHGStagePODs, config);
        unsigned int numberOfLGPODs = build_event(allLGPODs, output, allLGStagePODs, config);
        timers[3].Stop();

        liveStop = triggerTime + previousSamples;
        triggerTime = liveStop;
        currentEvent.setBufferLiveStopTS(liveStop);

        detectorEventCounter += 1;
        output->doWriteEvent(currentEvent);
        output->doWriteDetectorMCTruthEvent();
        output->doResolveEvtPtrs();

        if (config->getConfig("PrintInfo") == "true")
        {
            totalPhotons += nPhot;
            print_info(k, nPhot, totalPhotons, numberOfLGPODs, numberOfHGPODs, timers, cumulativeRealTimes,
                cumulativeCPUTimes);
        }
    }
    if (config->getConfig("PrintInfo") == "true")
        electronics[0][0]->printRunningTime();
}

void do_analogue_electronics_response(DeviceVectors& electronics, Pulse& theLGPulse, Pulse& theHGPulse,
				      PulseVectors& allStagePulses, unsigned int firstDoubleGainStage, bool fillStagePulses)
{
    unsigned int stageNumber = 0;
    for (auto deviceStage : electronics)
    {
      if(deviceStage.size()>0){
        if (stageNumber < firstDoubleGainStage)
        { // single pulse
            deviceStage[0]->doResponse(theHGPulse);
            if(fillStagePulses) allStagePulses[stageNumber][0].reset(new Pulse(theHGPulse));
            if (stageNumber + 1 == firstDoubleGainStage)
            { // at this point, split into HG & LG
                theLGPulse = theHGPulse;
            }
        }
        else
        {
            if (deviceStage.size() == 2)
            { // separate processing for HG & LG
                deviceStage[0]->doResponse(theLGPulse);
                deviceStage[1]->doResponse(theHGPulse);
            }
            else
            {
                deviceStage[0]->doResponse(theLGPulse, theHGPulse); // if processing is the same for both HG & LG
            }
	    if(fillStagePulses){
	      if (stageNumber + 1 < electronics.size())
		{
		  allStagePulses[stageNumber][0].reset(new Pulse(theLGPulse));
		  allStagePulses[stageNumber].push_back(std::shared_ptr<Pulse>());
		  allStagePulses[stageNumber][1].reset(new Pulse(theHGPulse));
		}
	      else
		{
		  for (auto stagePulses : allStagePulses)
		    { // Apply the digitiser to all of them
		      if (stagePulses.size() > 1)
                        deviceStage[0]->doStageResponse(*stagePulses[0], *stagePulses[1]);
		      else
                        deviceStage[0]->doStageResponse(*stagePulses[0]);
		    }
		}
	    }
        }
      }
      ++stageNumber;
    }
}

DeviceVectors setup_analogue_electronics(unsigned int& firstDoubleGainStage, global::ConfigPtr config)
{
    DeviceVectors devices; // LG and HG chains;
    if (config->getConfig("SignalChain") == "SAMPLED")
    {
        for (unsigned int i = 0; i < 2; ++i)
            devices.push_back(std::vector<std::shared_ptr<Device> >());
        devices[0].push_back(DeviceFactory::getDevice("PMT", der::DeviceModel::kSampled));
        devices[1].push_back(DeviceFactory::getDevice("Digitizer", der::DeviceModel::kSampled));
    }
    else if (config->getConfig("SignalChain") == "ANALYTIC")
    {
        for (unsigned int i = 0; i < 5; ++i)
            devices.push_back(std::vector<std::shared_ptr<Device> >());
        devices[0].push_back(DeviceFactory::getDevice("PMT", der::DeviceModel::kAnalytic));
        devices[1].push_back(DeviceFactory::getDevice("Cable", der::DeviceModel::kPMT));
        devices[2].push_back(DeviceFactory::getDevice("Amplifier", der::DeviceModel::kLowGain));
        devices[2].push_back(DeviceFactory::getDevice("Amplifier", der::DeviceModel::kHighGain));
        devices[3].push_back(DeviceFactory::getDevice("Cable", der::DeviceModel::kFeedthrough));
        devices[4].push_back(DeviceFactory::getDevice("Digitizer", der::DeviceModel::kAnalytic));
        firstDoubleGainStage = 2;
    }
    return devices;
}

std::shared_ptr<PODContainer> create_pods(
    Pulse& thePulse, std::shared_ptr<MCTruth> theMCTruth, std::string gainOption, global::ConfigPtr config)
{
    std::shared_ptr<PODContainer> thePODContainer(new PODContainer());
    thePODContainer->fillPODContainerFromPulse(thePulse, theMCTruth, gainOption);
    return thePODContainer;
}

void run_trigger_on_pods(
    FPGATrigger& theTrigger, std::shared_ptr<PODContainer> thePODContainer, PODContainerVectors& allStagePODs)
{
    bool triggered = false;
    std::shared_ptr<PODContainer> theTriggerPODContainer(new PODContainer());
    unsigned int channel = 0;
    for (int i = 0; i < thePODContainer->size(); i++)
    {
        std::shared_ptr<POD> thePOD = thePODContainer->getNextPOD();
        std::shared_ptr<POD> theTriggerPOD;
        if (thePOD->getChannel() % 1000 <= 540)
        {
            channel = thePOD->getChannel();
            theTriggerPOD = theTrigger.processPOD(thePOD);
            if (thePOD->getIsTriggered())
                triggered = true;
        }
        else
        {
            theTriggerPOD.reset(new POD());
            theTriggerPOD->resize(thePOD->size());
            std::fill(theTriggerPOD->begin(), theTriggerPOD->end(), 7372);
        }
        theTriggerPODContainer->push_back(theTriggerPOD);
    }
    if (triggered)
        theTrigger.addTriggeredChannel(channel);
    thePODContainer->resetPODCounter();
    PODContainerVector stagePODs;
    stagePODs.push_back(std::move(theTriggerPODContainer));
    allStagePODs.push_back(stagePODs);
}

void create_stage_pods(PODContainerVectors& allStagePODs, PulseVectors& allStagePulses,
		       std::shared_ptr<PODContainer>& thePODContainer, std::string gainOption, bool useS2Trigger)
{
    bool newPODVector = true;
    if (useS2Trigger)
        newPODVector = false;

    PODContainerVector stagePODs;
    for (size_t i = 0; i < allStagePulses.size(); ++i)
    {
        unsigned int p = 0;
        if (gainOption == "HG")
        {
            if (allStagePulses[i].size() > 1)
                p = 1;
        }
        if (!newPODVector)
            allStagePODs.back().push_back(std::move(thePODContainer->makePODsFromBoundaries(allStagePulses[i][p])));
        else
            stagePODs.push_back(std::move(thePODContainer->makePODsFromBoundaries(allStagePulses[i][p])));
    }
    if (newPODVector)
    {
        allStagePODs.push_back(stagePODs);
    }
}

unsigned int build_event(
    PODContainerVector& thePODs, Output* output, PODContainerVectors& allStagePODs, global::ConfigPtr config)
{
    unsigned long numberOfPODs = 0;

    if (config->getConfig("UseS2Trigger") == "true")
    {
        // NEED TO APPLY THE TRIGGER LOGIC HERE TO DECIDE WHICH PODS FALL IN WHICH
        // EVENT, AND THEREFORE OUGHT TO BE WRITTEN OUT
    }

    bool writeStagePODs = (config->getConfig("GenerateStageData") == "true" ? true : false);

    for (size_t i = 0; i < thePODs.size(); ++i)
    {
        for (size_t j = 0; j < thePODs[i]->returnFinalPODSize(); ++j)
        {
            std::shared_ptr<POD> thePOD = thePODs[i]->getNextPOD();
            output->doWriteData(*thePOD, thePOD->getDataCollector());
            if (writeStagePODs && output->getOutputFormat() == format::revision::ROOTvMDC2)
            {
                PODContainer stagePODs;
                for (size_t k = 0; k < allStagePODs[i].size(); ++k)
                {
                    stagePODs.push_back(allStagePODs[i][k]->getNextPOD());
                }
                output->doWriteStageData(stagePODs);
            }
            ++numberOfPODs;
        }
    }
    return numberOfPODs;
}

std::string progress_status(const unsigned long long& it, const unsigned long& total)
{
    /**
     * Method to return the status based on the current supplied event number It
     * and the total number of events Tot
     *
     * Each = symbol represents 10% progress, so ==> represents 20% progress
     *
     * Returns a std::string in the format [==>] - [It/Tot] where the progress
     * bar represents the overall progress in processing all events.
     */

    std::string status;

    int percentageComplete = (int)(100 * it / total);
    int percentageIncrement = 10;
    std::string marker;
    for (int i = 0; i < percentageComplete / percentageIncrement; ++i)
    {
        marker += "=";
    }

    status = "[" + marker + ">]" + " - " + "[" + std::to_string(it) + "/" + std::to_string(total) + "]";

    return status;
}

void print_info(unsigned long long& event, unsigned long long& NPhot, unsigned long& totalPhotons,
    unsigned int& numberOfLGPODs, unsigned int& numberOfHGPODs, std::vector<TStopwatch>& timers,
    double* cumulativeRealTimes, double* cumulativeCPUTimes)
{
    std::cout << "\n\nTimers for event " << event << " with " << NPhot << " photons; " << numberOfHGPODs << " HG PODs; "
              << numberOfLGPODs << " LG PODs" << std::endl;
    std::cout << "This event only: " << std::endl;
    std::cout << std::left << std::setw(17) << "" << std::setw(12) << "Real (s)" << std::setw(8) << "CPU (s)"
              << std::endl;
    std::cout << std::left << std::setw(17) << "Setup: " << std::setw(12) << timers[0].RealTime() << std::setw(8)
              << timers[0].CpuTime() << std::endl;
    std::cout << std::left << std::setw(17) << "Setup: " << std::setw(12)
              << timers[0].RealTime() - cumulativeRealTimes[0] << std::setw(8)
              << timers[0].CpuTime() - cumulativeCPUTimes[0] << std::endl;
    std::cout << std::left << std::setw(17) << "Initialisation: " << std::setw(12) << timers[1].RealTime()
              << std::setw(8) << timers[1].CpuTime() << std::endl;
    std::cout << std::left << std::setw(17) << "Response: " << std::setw(12) << timers[2].RealTime() << std::setw(8)
              << timers[2].CpuTime() << "-> " << (timers[2].RealTime()) / NPhot << " real time (s) per photon"
              << std::endl;
    std::cout << std::left << std::setw(17) << "Event build: " << std::setw(12) << timers[3].RealTime() << std::setw(8)
              << timers[3].CpuTime() << std::endl;

    cumulativeRealTimes[0] += timers[0].RealTime();
    cumulativeRealTimes[1] += timers[1].RealTime();
    cumulativeRealTimes[2] += timers[2].RealTime();
    cumulativeRealTimes[3] += timers[3].RealTime();
    cumulativeCPUTimes[0] += timers[0].CpuTime();
    cumulativeCPUTimes[1] += timers[1].CpuTime();
    cumulativeCPUTimes[2] += timers[2].CpuTime();
    cumulativeCPUTimes[3] += timers[3].CpuTime();

    std::cout << "\nRunning totals: " << std::endl;
    std::cout << std::left << std::setw(17) << "Setup: " << std::setw(12) << cumulativeRealTimes[0] << std::setw(8)
              << cumulativeCPUTimes[0] << std::endl;
    std::cout << std::left << std::setw(17) << "Initialisation: " << std::setw(12) << cumulativeRealTimes[1]
              << std::setw(8) << cumulativeCPUTimes[1] << std::endl;
    std::cout << std::left << std::setw(17) << "Response: " << std::setw(12) << cumulativeRealTimes[2] << std::setw(8)
              << cumulativeCPUTimes[2] << "-> " << cumulativeRealTimes[2] / totalPhotons << " real time (s) per photon"
              << std::endl;
    std::cout << std::left << std::setw(17) << "Event build: " << std::setw(12) << cumulativeRealTimes[3]
              << std::setw(8) << cumulativeCPUTimes[3] << "\n"
              << std::endl;
}
}
