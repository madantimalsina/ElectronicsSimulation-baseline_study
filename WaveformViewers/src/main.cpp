#include "TStopwatch.h"
#include <iostream>

#include "RunControl.hpp"

void print_timer(TStopwatch& timer);

/**
 * Configure all settings for run.
 *
 * This includes the input file, the output file and output file format,
 * the events to analyse, which PMT banks to use, what user and host are
 * running the simulation.
 *
 * Attemps to catch all user input errors.
 *
 * Also loads the input data and handles passing it to the signal chain for
 * analysis.
 *
 * Reports the time taken to process.
 *
 * Returns 0.
 */
int main(int argc, char** argv)
{
    TStopwatch timer;
    timer.Start();

    RunControl::print_welcome_text();

    // Read configuration file
    global::config = global::create_default_config();

    DERSummary dersummary;
    EBSummary ebsummary;
    EBGlobal ebGlobal;

    DBInterfaceFactory<double> pmtParamsFactory;
    std::shared_ptr<DBInterface<double>> pmtCSVParams;
    DERSummary* theDERSummary = &dersummary;

    EBSummary* theEBSummary = &ebsummary;
    EBGlobal* theGlobalEB = &ebGlobal;

    std::vector<std::string> input_files;
    std::string inFilename;

    try
    {
        RunControl::parse_user_inputs(argc, argv, input_files, inFilename, global::config);
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    format::revision formatVersion = format::revision::ROOTvMDC2;
    Input* input = InputFactory::getInput(formatVersion);

    Output* output = NULL;

    std::string fileRandomSeed = "";
    unsigned long posixTime = 0;
    unsigned long realPosixTime = 0;
    std::string localTime = "";
    std::string derRandomSeed = "UNKNOWN";
    int dataCollectors = 0;
    int channelsPerDDC32 = 0;
    int samplingRate_ns = 0;
    int ddc32Boards = 0;
    bool outputIsBinary = false;
    unsigned int runNumber = 0;

    try
    {
        RunControl::set_number_of_threads(global::config);
        RunControl::set_random_number_seeds(derRandomSeed, global::config);
        RunControl::set_pmt_parameters(pmtParamsFactory, pmtCSVParams, global::config);
        //    	RunControl::set_trigger_parameters();
        RunControl::set_data_collector_events(global::config);
        RunControl::check_file_permissions(global::config);
        RunControl::setup_input(input, input_files);
        RunControl::check_pmts_and_events(input, global::config);
        RunControl::setup_output(output, formatVersion, outputIsBinary, global::config);
        RunControl::setup_time_stamp(output, realPosixTime, posixTime, localTime, global::config);
        RunControl::setup_summary(input, output, *theDERSummary, input_files, posixTime, realPosixTime, fileRandomSeed,
            derRandomSeed, localTime, *theGlobalEB, dataCollectors, channelsPerDDC32, ddc32Boards, samplingRate_ns,
            runNumber, global::config);
        RunControl::print_user_info(*theDERSummary);
        RunControl::user_check(*theDERSummary, global::config);

        std::cout << std::endl;
        std::cout << std::endl;

        RunControl::setup_pmt_start_int(input);

        output->doInitInputVariables(input);

        RunControl::write_summary_objects(input, output, theDERSummary, theEBSummary, theGlobalEB);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    //----------------------------------------------------
    // Recreate the Data Collectors on each event
    // Currently uses PosixTime. Will become (PosixTime) + (duration [samples = 10 ns] of previous event)
    DDC32 theDCs(dataCollectors, channelsPerDDC32, posixTime);
    theDCs.setSamplingRate(samplingRate_ns);

    RunControl::acquire_and_process_data(input, output, theDCs, theEBSummary, global::config);

    output->doWriteTruthTree();

    output->doWriteTime(timer.RealTime(), timer.CpuTime());
    output->CloseFile();

    delete output;

    input->Close();
    delete input;

    timer.Stop();

    print_timer(timer);

    return 0;
}

void print_timer(TStopwatch& timer)
{
    std::cout << std::endl;
    std::cout << "Processing Time: " << std::endl;
    std::cout << "Type\tTime [s]" << std::endl;
    std::cout << "Real\t" << timer.RealTime() << std::endl;
    std::cout << "CPU\t" << timer.CpuTime() << std::endl;
    std::cout << "Closing..." << std::endl;
    std::cout << "Finished." << std::endl;
}
