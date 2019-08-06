//
//  RunControl.hpp
//
//  Created by Cees Carels on 04/01/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef RunControl_hpp
#define RunControl_hpp

#include <algorithm>
#include <cctype>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <stdio.h>
#include <string>

#include "TStopwatch.h"

#include "DERSummary.hpp"
#include "PulseReader.hpp"

#include "EBGlobal.hpp"
#include "EBSummary.hpp"

#include "Config.hpp"
#include "DBInterfaceFactory.hpp"
#include "InputFactory.hpp"
#include "InputOutputFormats.hpp"
#include "OutputFactory.hpp"
#include "PMT.hpp"
#include "FPGATrigger.hpp"
#include "DeviceFactory.hpp"

#define HASH __GIT
const std::string GitHash = HASH;

typedef std::vector<std::vector<std::shared_ptr<Device>>> DeviceVectors;
typedef std::vector<std::vector<std::shared_ptr<Pulse>>> PulseVectors;
typedef std::vector<std::shared_ptr<PODContainer>> PODContainerVector;
typedef std::vector<std::vector<std::shared_ptr<PODContainer>>> PODContainerVectors;

namespace RunControl
{
/**
 * Function that displays help and usage.
 */
void displayHelp();

/**
 * Configuration settings loaded from the DERCONFIG.txt configuration file
 * are read as std::string variables.
 *
 * This method converts the string to a bool.
 *
 * Returns true if the string reads "true" and false otherwise.
 */
bool toBool(const std::string& conf);
void print_welcome_text();

void parse_user_inputs(
    int argc, char** argv, std::vector<std::string>& input_files, std::string& inFilename, global::ConfigPtr config);

// int SetNumThreads(std::shared_ptr<DBInterface<std::string>> config);
void set_number_of_threads(global::ConfigPtr config);
void set_random_number_seeds(std::string& random_seed, global::ConfigPtr config);
void set_pmt_parameters(DBInterfaceFactory<double>& pmtParamsFactory, std::shared_ptr<DBInterface<double>> pmtCsvParams,
    global::ConfigPtr config);
void set_trigger_parameters(global::ConfigPtr config);
void acquire_and_process_data(
    Input* input, Output* output, DDC32& testDCs, EBSummary* theEBSummary, global::ConfigPtr config);

void do_analogue_electronics_response(DeviceVectors& electronics, Pulse& theLGPulse, Pulse& theHGPulse,
				      PulseVectors& allStagePulses, unsigned int firstDoubleGainStage, bool fillStagePulses);

DeviceVectors setup_analogue_electronics(unsigned int& firstDoubleGainStage, global::ConfigPtr config);
std::shared_ptr<PODContainer> create_pods(Pulse& thePulse, std::shared_ptr<MCTruth> theMCTruth, std::string gainOption, global::ConfigPtr config);
void run_trigger_on_pods(FPGATrigger& theTrigger, std::shared_ptr<PODContainer> thePODContainer, PODContainerVectors& allStagePODs);
void create_stage_pods(PODContainerVectors& allStagePODs, PulseVectors& allStagePulses,
    std::shared_ptr<PODContainer>& thePODContainer, std::string gainOption, bool useS2Trigger);

unsigned int build_event(
     PODContainerVector& thePODs, Output* output, PODContainerVectors& allStagePODs, global::ConfigPtr config);

std::string progress_status(const unsigned long long& it, const unsigned long& total);
void print_info(unsigned long long& event, unsigned long long& NPhot, unsigned long& totalPhotons,
    unsigned int& numberOfLGPODs, unsigned int& numberOfHGPODs, std::vector<TStopwatch>& timers,
    double* intermediateRealTimes, double* intermediateCPUTimes);

void set_data_collector_events(global::ConfigPtr config);
void check_file_permissions(global::ConfigPtr config);
void check_pmts_and_events(Input*& input, global::ConfigPtr config);
void setup_input(Input*& input, std::vector<std::string>& input_files);
void setup_output(Output*& output, format::revision formatVersion, bool& outputIsBinary, global::ConfigPtr config);

void setup_time_stamp(Output*& output, unsigned long& realPosixTime, unsigned long& posixTime, std::string& localTime,
    global::ConfigPtr config);
void user_check(DERSummary& theDERSummary, global::ConfigPtr config);
void setup_pulse_reader(PulseReader& theReaderPulses, global::ConfigPtr config);
void setup_summary(Input*& input, Output*& output, DERSummary& theDERSummary,
    const std::vector<std::string>& input_files, unsigned long& posixTime, unsigned long& realPosixTime,
    std::string& fileRandomSeed, std::string& derRandomSeed, std::string& localTime, EBGlobal& ebGlobal,
    int& dataCollectors, int& channelsPerDDC32, int& ddc32Boards, int& samplingRate_ns, unsigned int& runNumber,
    global::ConfigPtr config);

void setup_pmt_start_int(Input*& input);
void print_user_info(DERSummary& theDERSummary);

void write_summary_objects(
    Input*& input, Output*& output, DERSummary* theDERSummary, EBSummary* theEBSummary, EBGlobal* ebGlobal);
}

#endif /* RunControl_hpp */
