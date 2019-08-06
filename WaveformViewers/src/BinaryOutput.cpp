//
//  BinaryOutput.cpp
//  devices
//
//  Created by Cees Carels on 22/06/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "BinaryOutput.hpp"

BinaryOutput::BinaryOutput()
{
    /**
     * Constructor for BinaryOutput.
     *
     */
}

BinaryOutput::~BinaryOutput()
{
    /**
     * Destructor for BinaryOutput.
     */
}

void BinaryOutput::doWriteSummary(EBSummary& theEBSummary)
{
    /**
     * Method that gets the EBSummary for the current DER simulation, and writes
     * its information to disk.
     */

    endFlag = theEBSummary.getEndFlag();
    nEvtsFile = theEBSummary.getNEvtsFile();
}

void BinaryOutput::doWriteDERSummary(DERSummary& theDERSummary)
{
    /**
     * Method that gets the DERSummary for the current DER simulation,
     * and writes its information to disk.
     */
    Uname = theDERSummary.getUserName();
    DERUserName = theDERSummary.getDERUserName().c_str();
    DERHostName = theDERSummary.getDERHostName().c_str();
    SimFileName = theDERSummary.getFileName().c_str();
    DERFileName = theDERSummary.getDERFileName().c_str();
    SimFileRandomSeed = theDERSummary.getFileRandomSeed();
    DERFileRandomSeed = theDERSummary.getDERRandomSeed();
    PosixTime = theDERSummary.getPosixTime();
    LocalTime = theDERSummary.getLocalTime();
    GitCommitHash = theDERSummary.getGitCommitHash();

    /*fwrite(&Uname,              1, sizeof(std::string), outFileDER);
    fwrite(&DERUserName,        1, sizeof(std::string), outFileDER);
    fwrite(&DERHostName,        1, sizeof(std::string), outFileDER);
    fwrite(&SimFileName,        1, sizeof(std::string), outFileDER);
    fwrite(&DERFileName,        1, sizeof(std::string), outFileDER);
    fwrite(&SimFileRandomSeed,  1, sizeof(std::string), outFileDER);
    fwrite(&PosixTime,          1, sizeof(std::string), outFileDER);
    fwrite(&LocalTime,          1, sizeof(std::string), outFileDER);*/
}

void BinaryOutput::doWriteDERMCTruth(std::shared_ptr<MCTruth> theMCTruth)
{
    /**
     * Method intentionally left blank.
     */
}

void BinaryOutput::createOutFile(bool NewSeries, const std::string& outdir)
{
    /**
     * Create the output file at the specified directory. The output format
     * is in accordance with the DAQ format.
     *
     * This method sets up all of the file descriptors and structure
     * for each Data Collector (DC).
     */

    if (NewSeries)
    {
        CloseFile(); //Close the existing files
        int seq = std::stoi(fileSequence);
        if (seq > 999999)
        {
            std::cout << "ERROR: Cannot have more events in file." << std::endl;
            std::cout << "LZ-ICD-08-0007 does not permit >999999 files in ";
            std::cout << "acquisition sequence." << std::endl;

            std::cout << "DER will still increment file sequence number. ";
            std::cout << "Continue at your own risk" << std::endl;
        }
        ++seq;
        fileSequence = std::to_string(seq);
        unsigned long strlen = fileSequence.length();
        fileSequence = "";
        //Adds zero padding to 6 digits in accordance with LZ-ICD-08-0007.
        for (int i = 0; i < 6 - strlen; i++)
        {
            fileSequence += "0";
        }
        fileSequence += std::to_string(seq);
    }

    //Produce the .root file name
    std::string outputName = outNameBase;

    if (outdir != "")
    {
        outDir = outdir.c_str();
    }

    //Expand ~/ notation
    wordexp_t ExpResult;
    wordexp(outDir.c_str(), &ExpResult, 0);
    outDir = ExpResult.we_wordv[0];
    wordfree(&ExpResult);

    outNameRun = setRunName(runNumber);

    outputName = outputName.substr(0, outputName.find_last_of('.'));
    outputFull = outDir + outputName
        + outNameExp + outSeparator
        + yyyy + mm + dd + HH + MM + outSeparator + outNameRun
        + outSeparator + fileSequence; //Ext added below

    std::string outputFullDER = outDir + outputName + outSeparator + outNameExp + outSeparator
        + yyyy + mm + dd + HH + MM + ss + outSeparator + outNameRun + outSeparator
        + outSequence + "_DERData" + ext;

    std::string outputFullDR = outDir + outputName + outSeparator + outNameExp + outSeparator
        + yyyy + mm + dd + HH + MM + ss + outSeparator + outNameRun + outSeparator
        + outSequence + "_DR" + ext;

    //Check if file title lengths are not too long
    if (outputFull.size() > FILENAME_MAX - 20 || outputFullDR.size() > FILENAME_MAX - 20 || outputFullDER.size() > FILENAME_MAX - 20)
    {
        std::cout << "WARNING: This system supports filenames of up to "
                  << FILENAME_MAX << " bytes. "
                  << "The current filename is " << outputFull.size() << " bytes. "
                  << "Undefined behaviour will ensue."
                  << std::endl;
    }

    DC.resize(numDCs); //There are numDC Data Collectors.
    DCEvtPtr.resize(DC.size()); //A set of pointers for each DC board.
    numOfPODsPtr.resize(numDCs); //As many pointers as there are DCs.

    //Create outFiles for DAQ data for each DC
    for (int i = 0; i < DC.size(); i++)
    {
        std::string FullOutPath = outputFull + outSeparator
            + setDCName(i) + ext.c_str();
        DC[i].path = FullOutPath + ".tmp";
        DC[i].fd = fopen(DC[i].path.c_str(), "wb");
        DC[i].id = (uint8_t)i;
    }

    std::string fullOutPath_DS = outputFull + outSeparator
        + "ds" + ext.c_str();
    DS.path = fullOutPath_DS + ".tmp";
    outFileDS = fopen(DS.path.c_str(), "wb");
    DS.id = 15;
    DS.fd = outFileDS;

    //outFileDER  = fopen(outputFullDER.c_str(),  "wb");
    //outFileDR   = fopen(outputFullDR.c_str(),   "wb");

    if (NewSeries)
        doWriteGlobal();
}

void BinaryOutput::doResolveEvtPtrs()
{
    /**
     *
     * The DAQ binary format specifies variables which store the location in
     * the file where each event in each DC starts. They are locations in
     * memory for the file descriptor to point to.
     *
     * The start position of each event is not known until after it is
     * completely written. Therefore, file descriptor positions were kept
     * to store the position in the file where this information
     * should be written.
     *
     * To clarify, the position in the file where the start position of each
     * event is stored, is the *same* for each Data Collector (DC) file.
     *
     * However, the location in the file to which it points is *not* the same
     * for each DC file (e.g. there are a different number of PODs for each
     * channel, therefore each DC has more or less POD samples, and so when the
     * event finishes, the file descriptor for each DC file has a different
     * byte offset from the start of the file).
     *
     * When this method is called, the start position of each event in the
     * simulation is resolved for each DC, by pointing the file descriptor to
     * the position where this variable should go, and then writing the byte
     * offset within the file where the event to which it points has started.
     *
     * This method should only be called after this information is determined.
     */

    int accEvt = ((globalEvt - 1) % (int)std::stof(global::config->getConfig("NewDCSeriesEvt")));
    for (int i = 0; i < DC.size(); i++)
    {
        WrAtOffsetAndRtrn(DC[i].fd,
            DCEvtPtr[i].FileEvtPtr[accEvt].Loc,
            DCEvtPtr[i].EvtStrtPtr[accEvt].Loc);

        if (DCEvtPtr[i].FileEvtPtr[accEvt].Loc == 0)
            std::cout << "FileEvtPtr wrong!" << std::endl;
        if (DCEvtPtr[i].EvtStrtPtr[accEvt].Loc == 0)
            std::cout << "EvtStrtPtr wrong!" << std::endl;
    }

    WrAtOffsetAndRtrn(DS.fd,
        DSEvtPtr.FileEvtPtr[accEvt].Loc,
        DSEvtPtr.EvtStrtPtr[accEvt].Loc);
}

void BinaryOutput::CloseFile()
{
    /**
     * Method that closes the DER binary output files.
     *
     * As this is binary output, all information has already been written
     * to disk. So the files can be immediately closed.
     */

    std::cout << "Closing file..." << std::endl;
    for (int i = 0; i < DC.size(); i++)
    {
        fclose(DC[i].fd); //!< Close DC files.
        DC[i].fd = NULL;
        std::string patho = DC[i].path;
        size_t last = DC[i].path.find_last_of(".");
        std::string pathn = DC[i].path.substr(0, last);
        if (std::rename(patho.c_str(), pathn.c_str()))
        {
            std::cout << "Error renaming file: " << patho << " to " << pathn << std::endl;
        }
        DC[i].path = pathn;
    }
    fclose(outFileDS); //!< Close DS file.
    std::string patho = DS.path;
    size_t last = DS.path.find_last_of(".");
    std::string pathn = DS.path.substr(0, last);
    if (std::rename(patho.c_str(), pathn.c_str()))
    {
        std::cout << "Error renaming file: " << patho << " to " << pathn << std::endl;
    }

    //fclose(outFileDER); //!< Close DER information file.
    //fclose(outFileDR);  //!< Close Data Rate information file.

    outFileDS = NULL;
    //outFileDER  = NULL;
    //outFileDR   = NULL;
}

double BinaryOutput::StatFileSize()
{
    /**
     * Return the current size (in bytes) of the DER output file.
     *
     * https://www.securecoding.cert.org/confluence/display/c/FIO19-C.+Do+not+use+fseek()+and+ftell()+to+compute+the+size+of+a+regular+file
     */
    off_t FileSize = 0;
    char* Buffer;
    struct stat StatBuffer;
    for (int i = 0; i < DC.size(); i++)
    {
        int fd = open(DC[i].path.c_str(), O_RDONLY);
        if (fd == -1)
            std::cout << "Error opening file" << std::endl;
        if ((fstat(fd, &StatBuffer) != 0) || (!S_ISREG(StatBuffer.st_mode)))
            std::cout << "Error statting" << std::endl;

        FileSize += StatBuffer.st_size;
        Buffer = (char*)malloc(FileSize);
        if (Buffer == NULL)
            std::cout << "Buffer error" << std::endl;
        close(fd);
    }
    return FileSize;
}

double BinaryOutput::DataRate(TStopwatch& Timer)
{
    /**
     * Get data rate in [kB/s] based on time of last measurement.
     */
    Long64_t newSize = StatFileSize();
    Long64_t SizeDiff = newSize - prevSize;
    Timer.Stop();
    double CurrentTime = Timer.CpuTime();
    Timer.Continue();
    if (CurrentTime == 0)
        dataRate = 0.0;
    else if (CurrentTime - prevTime == 0)
        dataRate = 0.0;
    else
        dataRate = SizeDiff / (CurrentTime - prevTime);
    prevTime = CurrentTime;
    prevSize = newSize;

    //Write data rate to disk
    //fwrite(&dataRate, 1,    sizeof(Double_t), outFileDR);
    //fwrite(&CurrentTime, 1, sizeof(Double_t), outFileDR);

    return dataRate / 1000.0;
}

std::string BinaryOutput::setDCName(const int& dcNum)
{
    /**
     * Set and return DC name.
     */
    DCNumber = std::to_string(dcNum);
    std::string out;
    for (int i = 0; i < 2 - DCNumber.length(); i++)
    {
        out += "0";
    }
    out += DCNumber;

    return out;
}

void BinaryOutput::doWriteTruthTree()
{
    /**
     * Method required by interface but may be filled eventually.
     */
}

void BinaryOutput::doInputTreeClone(TFile* pointerToFile)
{
    /**
     * Method required by interface and intentionally left blank.
     */
}

void BinaryOutput::doWriteDetectorMCTruthEvent()
{
    /**
     * Method required by interface and intentionally left blank
     */
}

void BinaryOutput::doPreparePulseMCTruth(unsigned long long NVertices,
    unsigned long long TMax)
{
    /**
     * Method required by interface and intentionally left blank
     */
}

void BinaryOutput::doWriteTime(const double Real, const double CPU)
{
    /**
     * Set the real and cpu time taken to run the entire DER
     * simulation.
     */
    tReal = Real;
    tCPU = CPU;
}

void BinaryOutput::doWriteGlobal(EBGlobal* theGlobalSummary)
{
    /**
     * Method that gets the EBGlobal object for the current DER simulation,
     * and writes its information to disk.
     */

    //The following checks on the pointer are to check whether this method
    //was called in the update mode where the simulation is already in progress,
    //or as the first time where the pointer points to useful information.
    //If the pointer is NULL, then the simulation is already in progress and
    //additional simulation files are being created, so the values set by this
    //method should reflect the current status.
    uint16_t nEvts = 0;
    if (theGlobalSummary != NULL)
    {
        //The DER allows for unsigned long, but is type-cast to accommodate
        //the smaller unsigned int.
        runNumber = (unsigned int)(theGlobalSummary->getRunNumber());
        formatVersionID = (unsigned int)(theGlobalSummary->getFormatVersion());
        runStartTime = theGlobalSummary->getRunStartTime();
    }

    std::cout << "Setting up new DC+DS files..." << std::endl;

    //Check the number of events total,
    //the number of events in the next set of file,
    //and the current event.
    int NewDCSeriesEvt = std::stoi(global::config->getConfig("NewDCSeriesEvt"));

    bool lastsequence = false; //Determines endOfAcq
    //Case: more events left to process than should be in one DC file
    nEvts = (nEvtsFile > NewDCSeriesEvt) ? NewDCSeriesEvt : nEvtsFile % NewDCSeriesEvt;

    //Case: fewer events left to process than should be in one DC file
    if (nEvtsFile - (unsigned short)globalEvt < NewDCSeriesEvt)
    {
        nEvts = nEvtsFile % NewDCSeriesEvt;
        lastsequence = true;
    }
    //Case: from the beginning there were already exactly as many events
    //to process as should be in one DC file
    if (nEvtsFile == NewDCSeriesEvt)
    {
        nEvts = NewDCSeriesEvt;
        lastsequence = true;
    }

    if (nEvtsFile % NewDCSeriesEvt == 0)
    {
        //If nEvtsFile is an exact multiple of NewDCSeriesEvt,
        //then the above check will be wrong for the last file sequence,
        //due to the modulus operator evaluating the expression to 0.
        //In this case, division can be used
        nEvts = (nEvtsFile > NewDCSeriesEvt) ? NewDCSeriesEvt : nEvtsFile / NewDCSeriesEvt;
        if (nEvtsFile == NewDCSeriesEvt)
        {
            nEvts = nEvtsFile;
            lastsequence = true;
        }
    }

    //Edge case of NewDCSeriesEvt == 1
    if (NewDCSeriesEvt == 1)
    {
        nEvts = 1;
        if (globalEvt == nEvtsFile)
            lastsequence = true;
    }

    endOfAcq = (lastsequence) ? 1 : 0;

    for (int i = 0; i < DC.size(); i++)
    {
        if (DC[i].fd == NULL)
        {
            std::cout << "DC " << i << " is NULL!" << std::endl;
        }
    }

    if (DS.fd == NULL)
        std::cout << "DS is NULL!" << std::endl;

    //fileSeq value is set by
    //BinaryOutput::createOutFile(bool NewSeries, const std::string &outdir)
    uint32_t fileSeq = std::stoi(fileSequence);
    for (int i = 0; i < DC.size(); i++)
    {
        uint8_t nDigis = getNDigisInDC(i);
        fwrite(&formatVersionID, sizeof(uint16_t), 1, DC[i].fd);
        fwrite(&fileSeq, sizeof(uint32_t), 1, DC[i].fd);
        fwrite(&endOfAcq, sizeof(uint8_t), 1, DC[i].fd);
        fwrite(&DC[i].id, sizeof(uint8_t), 1, DC[i].fd);
        fwrite(&nDigis, sizeof(uint8_t), 1, DC[i].fd);
        fwrite(&runStartTime, sizeof(uint64_t), 1, DC[i].fd);
        AllEvts.Loc = (uint32_t)ftell(DC[i].fd);
        fwrite(&nEvts, sizeof(uint16_t), 1, DC[i].fd);
    }

    //Write pointers to location in file of start of each event
    //Location pointer points to number of bytes from start of file
    //This is determined later and resolved later

    //First apply to DC[0] and get pointer positions
    //Happens once per run.
    uint32_t dummy = 0;

    //0 position for EvtStrtPtr at first.
    EvtPtr dummyEvtStrt;
    dummyEvtStrt.Loc = 0; //Will be properly set later

    for (int i = 0; i < DC.size(); i++)
    {
        DCEvtPtr[i].FileEvtPtr.resize(0);
        DCEvtPtr[i].EvtStrtPtr.resize(0);
    }

    int accEvt = (((int)globalEvt - 1) % (int)std::stof(global::config->getConfig("NewDCSeriesEvt")));
    for (int i = 0; i < nEvts; i++)
    {
        //Get current file position in DC output file.
        //Up until this point the position is the same in each file,
        //by definition of the binary file format.
        //The byte offset for the first event pointer is always at 19 bytes.
        EvtPtr CurrPtr;
        CurrPtr.EvtNum = i + accEvt; //Placeholder indexed relative to file
        dummyEvtStrt.EvtNum = i + accEvt; //Placeholder indexed relative to file
        CurrPtr.Loc = (uint32_t)ftell(DC[0].fd);
        for (int j = 0; j < DC.size(); j++)
        {
            DCEvtPtr[j].FileEvtPtr.push_back(CurrPtr);
            DCEvtPtr[j].EvtStrtPtr.push_back(dummyEvtStrt);
        }
        fwrite(&dummy, sizeof(uint32_t), 1, DC[0].fd);
    }

    //Now write pointers to remaining DC files
    for (int i = 1; i < DC.size(); i++)
    {
        for (int j = 0; j < nEvts; j++)
        {
            fwrite(&dummy, sizeof(uint32_t), 1, DC[i].fd);
        }
    }

    //Set up the DS file
    triggerRunNumber = (uint32_t)runNumber; //EB calls this the "Run Number"
    fwrite(&formatVersionID, sizeof(uint16_t), 1, DS.fd);
    fwrite(&triggerRunNumber, sizeof(uint32_t), 1, DS.fd);
    fwrite(&fileSeq, sizeof(uint32_t), 1, DS.fd);
    fwrite(&endOfAcq, sizeof(uint8_t), 1, DS.fd);
    fwrite(&runStartTime, sizeof(uint64_t), 1, DS.fd);
    fwrite(&nEvts, sizeof(uint16_t), 1, DS.fd);

    DSEvtPtr.FileEvtPtr.resize(nEvts);
    DSEvtPtr.EvtStrtPtr.resize(nEvts);
    for (int i = 0; i < nEvts; i++)
    {
        EvtPtr CurrPtr;
        CurrPtr.EvtNum = i + accEvt; //Placeholder indexed relative to file
        dummyEvtStrt.EvtNum = i + accEvt; //Placeholder indexed relative to file
        CurrPtr.Loc = (uint32_t)ftell(DS.fd);
        DSEvtPtr.FileEvtPtr[i] = CurrPtr;
        DSEvtPtr.EvtStrtPtr[i] = dummyEvtStrt;
        fwrite(&dummy, sizeof(uint32_t), 1, DS.fd);
    }
}

void BinaryOutput::doPrepareEvent()
{
    /**This method is only used for the binary case.
     * It can only be called repeatedly in exactly one place in RunControl.cpp,
     * just before the channel data starts to be written.
     *
     * This method just writes dummy data for nPODs but keeps the position
     * to its location in the file.
     *
     * The correct values for the event sequence number are already available
     * and so are written already.
     *
     * The arguments apply equally to every DC file by construction
     * of the binary file format. Therefore the same data can be written to
     * each DC and the pointer to the event sequence number applies to every DC.
     *
     * The nPODs variable itself is different for each DC
     *
     * The position of eventSeqNum, which is here called globalEvt, is also the
     * start of the event. Therefore the event start position pointers are
     * assigned here.
     *
     * Their values must be re-assigned since objects do not go out of scope.
     *
     * The start positions are not necessarily equal in the various DC files,
     * since the number of PODs can vary per channel and per event.
     *
     * Current position in each DC file is the start pos of evt.
     */

    //If checked here, the fd offset should be: (19 + 4*nEvts) bytes
    int accEvt = (((int)globalEvt - 1) % (int)std::stof(global::config->getConfig("NewDCSeriesEvt")));
    for (int i = 0; i < DC.size(); i++)
    {
        nPods[i] = 0; //Reset
        DCEvtPtr[i].EvtStrtPtr[accEvt].EvtNum = (int)globalEvt;
        DCEvtPtr[i].EvtStrtPtr[accEvt].Loc = (uint32_t)ftell(DC[i].fd);

        //Write evtSeqNum and numPODs to each DC
        //Store pointer for pos of numPODs, write dummy variable placeholder
        uint16_t dummy = 0;
        uint32_t globalEvt_0 = globalEvt - 1; //Starts at 0
        fwrite(&globalEvt_0, sizeof(uint32_t), 1, DC[i].fd);
        numOfPODsPtr[i] = (uint32_t)ftell(DC[i].fd);
        if (numOfPODsPtr[i] == 0)
            std::cout << "numOfPODsPtr == 0, problem!" << std::endl;
        fwrite(&dummy, sizeof(uint16_t), 1, DC[i].fd);
    }
}

void BinaryOutput::doWriteData(POD& theEBDataPOD, const int& DCID)
{
    /**
     * Method that gets the POD object for the current channel, and writes it
     * to disk.
     */
    channel = theEBDataPOD.getChannel();
    startTime = theEBDataPOD.getPODStartTimeStamp() + trgTimeStamp;
    nSamples = theEBDataPOD.getPODLength();

    if (DCID >= DC.size())
    {
        std::cout << "ERROR: Requested DC ID unavailable." << std::endl;
    }

    //Add control to choose data collector, i.e. outFile becomes an array index
    fwrite(&channel, sizeof(uint16_t), 1, DC[DCID].fd);
    fwrite(&startTime, sizeof(uint64_t), 1, DC[DCID].fd);
    fwrite(&nSamples, sizeof(uint16_t), 1, DC[DCID].fd);

    if (startTime < sumPODstartTS)
        sumPODstartTS = startTime;

    //Add samples to sumPODLength to get running total for EBEvent Tree
    //Only 1 gain setting is added to the sumPOD so the length is that of 1
    //gain setting.
    //Not needed for binary output. Remove.
    sumPODlength += nSamples;

    nPods[DCID] += 1;

    for (int i = 0; i < nSamples; i++)
    {
        fwrite(&theEBDataPOD[i], sizeof(int16_t), 1, DC[DCID].fd);
    }
}

void BinaryOutput::doWriteEvent(EBEvent& theEBEvent)
{
    /**
     * Method that gets the EBEvent object for the event that is currently
     * being simulated, and writes its information to disk.
     */

    //In this revision, the digitisers are distributed inside the DC as follows
    //    The starting index for the digitizers is based on the following:
    //        DC = 0- 4 have 3 digitizers each for a total of 15,
    //        DC =    5 has  1 digitizer
    //        DC = 6-10 have 3 digitizers each for a total of 15,
    //        DC =   11 has  1 digitizer
    //        DC =   12 has  6 digitizers
    //        DC =   13 has  8 digitizers.
    //In the DER the digitiser times are all equivalent for now.

    //Event Live Structure
    int LastDC = 0;
    for (int i = 0; i < DC.size(); i++)
    {
        for (int j = 0; j < getNDigisInDC(i); j++)
        {
            fwrite(&theEBEvent.getBufferLiveStartTS()[0], sizeof(uint64_t), 1, DC[i].fd);
            fwrite(&theEBEvent.getBufferLiveStopTS()[0], sizeof(uint64_t), 1, DC[i].fd);
        }
        LastDC += getNDigisInDC(i);
    }

    //Resolve nPODs with value and position for each DC
    int TotalPODs = 0;
    for (int i = 0; i < DC.size(); i++)
    {
        WrAtOffsetAndRtrn(DC[i].fd, numOfPODsPtr[i], (uint16_t)nPods[i]);
        TotalPODs += nPods[i];
    }

    //Trigger data
    trgType = theEBEvent.getTriggerType();
    uint8_t trgType_B = (uint8_t)trgType; //Base class uses a larger type, so create a separate variable of the correct type here.
    trgTimeStamp = theEBEvent.getTriggerTimeStamp();
    trgMultiplicity = (uint16_t)theEBEvent.getTriggerMultiplicity();
    sumPODstartTS = theEBEvent.getSumPODStartTS() + (unsigned long)startTime;
    sumPODlength = theEBEvent.getSumPODLength();
    sumPODdata = theEBEvent.getSumPODData();

    //DS File
    int accEvt = (((int)globalEvt - 1) % (int)std::stof(global::config->getConfig("NewDCSeriesEvt")));
    DSEvtPtr.EvtStrtPtr[accEvt].Loc = (uint32_t)ftell(DS.fd);
    uint32_t globalEvt_0 = globalEvt - 1; //Starts at 0

    //Overwrite sumPODlength to be length 1, to write 1 sample as a placeholder.
    sumPODlength = 1;
    fwrite(&globalEvt_0, sizeof(uint32_t), 1, DS.fd);
    fwrite(&trgType_B, sizeof(uint8_t), 1, DS.fd);
    fwrite(&trgTimeStamp, sizeof(uint64_t), 1, DS.fd);
    fwrite(&trgMultiplicity, sizeof(uint16_t), 1, DS.fd);
    fwrite(&sumPODstartTS, sizeof(uint64_t), 1, DS.fd);
    fwrite(&sumPODlength, sizeof(uint16_t), 1, DS.fd);
    fwrite(&sumPODdata, (sumPODlength) * sizeof(int32_t), 1, DS.fd);
}
