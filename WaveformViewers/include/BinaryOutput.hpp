//
//  BinaryOutput.hpp
//  devices
//
//  Created by Cees Carels on 22/06/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef BinaryOutput_hpp
#define BinaryOutput_hpp

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <wordexp.h>

#include "Output.hpp"
#include "PMTLookup.hpp"

/**
 * Class providing the implementation of the .bin output file format, in
 * agreement with the DAQ binary format.
 *
 * If this class is used for output, then the output of the DER simulation will
 * contain the output data in the same format that the DAQ would
 * produce. In addition, a number of other files are written that contain
 * information about how the simulation was run, as well as the data rate.
 *
 * N.B. Take care! Just like in real LZ, the binary format output supports
 * up to only events in the range 1-65535, i.e. a total of 65534 events in one
 * simulation file.
 *
 */

class BinaryOutput : public Output
{
public:
    BinaryOutput();
    virtual ~BinaryOutput();

    void doWriteDERSummary(DERSummary& theDERSummary);
    void doWriteDERMCTruth(std::shared_ptr<MCTruth> theMCTruth);
    void doWriteSummary(EBSummary& theEBSummary);
    void doInputTreeClone(TFile* pointerToFile);
    void doWriteTruthTree();
    void doWriteDetectorMCTruthEvent();
    void doPreparePulseMCTruth(unsigned long long NVertices,
        unsigned long long TMax);

    template <typename T>
    void inline WrAtOffsetAndRtrn(FILE* fd,
        const unsigned long& offset,
        const T& value)
    {
        //Method writes data at given offset, then returns to previous position
        const long int CurrPos = ftell(fd); //Start position
        fseek(fd, offset, SEEK_SET); //New position
        fwrite(&value, 1, sizeof(T), fd); //Write at new position
        fseek(fd, CurrPos, SEEK_SET); //Return to position before writing
    }

    void createOutFile(const bool NewSeries, const std::string& outdir = "");
    void CloseFile();
    void doResolveEvtPtrs();
    double StatFileSize();
    double DataRate(TStopwatch& Timer);
    void doWriteTime(const double Real, const double CPU);
    void doWriteGlobal(EBGlobal* theGlobalSummary = NULL);
    void doPrepareEvent();
    void doWriteData(POD& theEBDataPOD, const int& DCID);
    void doWriteEvent(EBEvent& theEBEvent);

protected:
    std::string setDCName(const int& dcNum);
    std::string DCNumber;

    /**
     * Structure for DC file properties
     */
    struct DataCollector
    {
        uint8_t id; //DC ID number
        FILE* fd; //DC file descriptor
        std::string path; //DC file path
    };

    std::vector<DataCollector> DC;

    /**
     * Explanation of this structure is useful here.
     *
     * The struct DCPointers contains two sets of vectors:
     * FileEvtPtr refers to the position in the binary file.
     * There are as many entries in the vector as there are events
     * in that binary file.
     *
     * What does FileEvtPtr represent?
     * It is the position of the variable inside the binary file that
     * contains a number.
     *
     * What does that number refer to?
     * The number is the offset from the start of the
     * binary file where the event starts.
     *
     * How do we determine that number?
     * The number is determined while the data is being written to the
     * file. Once the number is known, it is stored inside EvtStrtPtr.
     *
     * The number is effectively a pointer. When someone opens the
     * binary file they can look up the location in the binary file
     * where each event starts.
     *
     * The structure should be clear now:
     * FileEvtPtr represents where the event pointer is stored.
     * EvtStrtPtr represents the location in the file where that event
     * starts. There is such a pair for each event in the file.
     *
     * Do the positions differ per DC file?
     * Yes. The EvtStrtPtr differs per DC file because it depends on
     * how much data was written for each event. If one DC file
     * contains more PODs than another DC file, then the next event
     * would start at a later position in the file. This is why
     * a std::vector<DCPointers> DCEvtPtr is needed. It tracks where
     * each event starts for each DC file.
     *
     * The FileEvtPtr locations do not differ per DC file, because
     * the pointers themselves are positioned at the top of the
     * binary file, before any event information is written.
     *
     */

    //Structure for pointers for event start positions
    struct EvtPtr
    {
        uint32_t Loc; //Location in file where pointer goes
        int EvtNum; //Event number
    };

    struct DCPointers
    {
        std::vector<EvtPtr> FileEvtPtr; //Where pointer is
        std::vector<EvtPtr> EvtStrtPtr; //Where pointer points to
    };

    std::vector<DCPointers> DCEvtPtr;
    unsigned long long numEvtsPtr;

    //Position of numOfPODs
    std::vector<unsigned long> numOfPODsPtr;

    //Structure for pointers in DS file
    DataCollector DS; //DS properties
    DCPointers DSEvtPtr; //DS event pointers

    EvtPtr AllEvts; //Position of total events in file in header

    FILE* outFileDS = NULL; //DS File
    FILE* outFileDER = NULL; //DER Specific data
    FILE* outFileDR = NULL; //Data rate data

    uint8_t endOfAcq = 1;
    std::string ext = ".bin";
    std::string DCID = "rr";

    //Variables for tracking data rate
    Long64_t prevSize = 0.0;
    Double_t prevTime = 0.0;
    Double_t dataRate = 0.0;
};
#endif /* BinaryOutput_hpp */
