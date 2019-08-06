//
//  RootOutputMDC2.cpp
//  ReadingPMTStream
//
//  Created by Theresa Fruth on 01/03/2018.
//  Copyright © 2018 LZOxford. All rights reserved.
//

#include "RootOutputMDC2.hpp"

RootOutputMDC2::RootOutputMDC2()
{
    /**
     * Constructor for RootOutputMDC2.
     */
    trgTimeStamp = 0;
    outputFormat = format::revision::ROOTvMDC2;
    LZ_Event_Format_Revision = format::LZ_ICD_08_0007::Rev_B;
    nPODInFile = 0;
    firstEvtData = 0;
#if (BACC_LIB_VERSION == 6)
    TruthObj = 0;
    BaccObj = 0;
#endif
}

RootOutputMDC2::~RootOutputMDC2()
{
    /**
     * Destructor for RootOutputMDC2.
     */
#if (BACC_LIB_VERSION == 6)
    delete TruthObj;
#endif
}

void RootOutputMDC2::doWriteData(POD& theEBDataPOD, const int& DCID)
{
    /**
     * Method that gets the POD object for the current channel, and writes it
     * to disk.
     */
#if (BACC_LIB_VERSION == 6)
    evt = theEBDataPOD.getEvent() - 1; //Subtract 1 to start at 0
    LUXSimRunNumber = theEBDataPOD.getLUXSimRunNumber();
    channel = theEBDataPOD.getChannel();
    hit = theEBDataPOD.getHitID();
    startTime = theEBDataPOD.getPODStartTimeStamp() + trgTimeStamp;
    nSamples = theEBDataPOD.getPODLength();

    //Add samples to sumPODLength to get running total for EBEvent Tree.
    sumPODlength += nSamples;
    ++nPODInFile;
    if (startTime < sumPODstartTS)
        sumPODstartTS = startTime;

    zData = theEBDataPOD.getSamples();
    fData->Fill();
#endif
}

void RootOutputMDC2::doWriteGlobal(EBGlobal* theGlobalSummary)
{
    /**
     * Method that gets the EBGlobal object for the current DER simulation,
     * and writes its information to disk.
     */

    //The DER allows for unsigned long, but is type-cast to accommodate
    //the smaller unsigned int.
    if (theGlobalSummary != NULL)
    {
        runNumber = (unsigned int)(theGlobalSummary->getRunNumber());
        formatVersionID = (unsigned short)(theGlobalSummary->getFormatVersion());
        runStartTime = theGlobalSummary->getRunStartTime();
        runType = theGlobalSummary->getRunType();
        fGlobal->Fill();
        fGlobal->Write();
    }
}

void RootOutputMDC2::doWriteEvent(EBEvent& theEBEvent)
{
    /**
     * Method that gets the EBEvent object for the event that is currently
     * being simulated, and writes its information to disk.
     */
    for (int i = 0; i < theEBEvent.getnPods().size(); i++)
    {
        nPods[i] = theEBEvent.getnPods().at(i); //nPods per DC
    }

    for (int i = 0; i < theEBEvent.getBufferLiveStartTS().size(); i++)
    {
        bufferStart[i] = theEBEvent.getBufferLiveStartTS()[i];
        bufferStop[i] = theEBEvent.getBufferLiveStopTS()[i];
    }

    trgType = theEBEvent.getTriggerType();
    trgTimeStamp = theEBEvent.getTriggerTimeStamp();
    trgMultiplicity = theEBEvent.getTriggerMultiplicity();
    sumPODstartTS = (unsigned long)sumPODstartTS + (unsigned long)startTime;
    sumPODdata = theEBEvent.getSumPODData();
    globalEvt = Output::globalEvt - 1; //Use base class (starts at 1)

    fEvent->Fill();
    firstEvtData = nPODInFile;

    //Trigger truth
    fTriggerTruth->Fill();
}

void RootOutputMDC2::createOutFile(const bool NewSeries,
    const std::string& outdir)
{
    /**
     * Create the output file at the specified directory. The output format
     * is in accordance with the Event Builder output file format.
     *
     * This method sets up all of the .root tree-branch-leaf structure,
     * and creates and checks the existance of the DER output .root file.
     */
    //Produce the .root file name
#if (BACC_LIB_VERSION == 6)
    gROOT->ProcessLine("#include <vector>");
    std::string outputName = outNameBase;

    if (outdir != "")
    {
        outDir = outdir.c_str();
    }

    outNameRun = setRunName(runNumber);

    outputName = outputName.substr(0, outputName.find_last_of('.'));
    outputFull = outDir + outputName
        + outNameExp + outSeparator
        + yyyy + mm + dd + HH + MM + outSeparator + outNameRun + outSeparator
        + fileSequence + outSeparator + outRaw + ext;

    outputFull += ".tmp";
    fFile = TFile::Open(outputFull.c_str(), "RECREATE");
    fpath = outputFull;

    DERRunIDIdx = 0;

    try
    {
        fDERSummary = new TTree("DERSummary", "DERSummary");
    }
    catch (std::bad_alloc& caught)
    {
        std::cerr << "bad_alloc caught: " << caught.what()
                  << std::endl;

        std::cout << "Caught while creating fDERSummary"
                  << std::endl;
    }
    fDERSummary->Branch("Uname", &Uname);
    fDERSummary->Branch("DERUserName", &DERUserName);
    fDERSummary->Branch("DERUserGroupID", &DERUserGroupID);
    fDERSummary->Branch("DERHostName", &DERHostName);
    fDERSummary->Branch("SimFileName", &SimFileName);
    fDERSummary->Branch("DERFileName", &DERFileName);
    fDERSummary->Branch("SimFileRandomSeed", &SimFileRandomSeed);
    fDERSummary->Branch("DERFileRandomSeed", &DERFileRandomSeed);
    fDERSummary->Branch("FormatRevision", &outputFormat, "FormatRevision/s");
    fDERSummary->Branch("SignalChainRevision", &SigChainVersion, "SignalChainRevision/s");
    fDERSummary->Branch("ProductionTimePosix", &PosixTime);
    fDERSummary->Branch("ProductionTimeLocal", &LocalTime);
    fDERSummary->Branch("SamplingRate_ns", &SamplingRate_ns, "SamplingRate_ns/i");
    fDERSummary->Branch("DERCONFIG", &DERCONFIG);
    fDERSummary->Branch("DERRunID", &DERRunID);
    fDERSummary->Branch("DERGitCommitHash", &GitCommitHash);

    try
    {
        fDERTime = new TTree("DERTime", "DERTime");
    }
    catch (std::bad_alloc& caught)
    {
        std::cerr << "bad_alloc caught: " << caught.what()
                  << std::endl;

        std::cout << "Caught while creating fDERTime"
                  << std::endl;
    }
    fDERTime->Branch("Real_s", &tReal, "Real_s/F");
    fDERTime->Branch("CPU_s", &tCPU, "CPU_s/F");
    fDERTime->Branch("DERRunIDIdx", &DERRunIDIdx, "DERRunIDIdx/s");

    try
    {
        fDetectorMCTruth = new TTree("DetectorMCTruthTree", "DetectorMCTruthTree");
    }
    catch (std::bad_alloc& caught)
    {
        std::cerr << "bad_alloc caught: " << caught.what()
                  << std::endl;

        std::cout << "Caught while creating fDetectorMCTruth"
                  << std::endl;
    }
    fDetectorMCTruth->Branch("DetectorMCTruthEvent", &TruthObj);
    fDetectorMCTruth->Branch("DERRunIDIdx", &DERRunIDIdx, "DERRunIDIdx/s");

    try
    {
        fDERSubsets = new TTree("DERSubsets", "DERSubsets");
    }
    catch (std::bad_alloc& caught)
    {
        std::cerr << "bad_alloc caught: " << caught.what()
                  << std::endl;

        std::cout << "Caught while creating fDERSubsets"
                  << std::endl;
    }

    fDERSubsets->Branch("phTimesLG", &OutputVectorTLG);
    fDERSubsets->Branch("phRespLG", &OutputVectorRLG);
    fDERSubsets->Branch("phTimesHG", &OutputVectorTHG);
    fDERSubsets->Branch("phRespHG", &OutputVectorRHG);
    fDERSubsets->Branch("DERRunIDIdx", &DERRunIDIdx, "DERRunIDIdx/s");

    try
    {
        fDataRate = new TTree("DERDataRate", "DERDataRate");
    }
    catch (std::bad_alloc& caught)
    {
        std::cerr << "bad_alloc caught: " << caught.what()
                  << std::endl;

        std::cout << "Caught while creating fDataRate"
                  << std::endl;
    }
    fDataRate->Branch("Time", &prevTime, "Time/D");
    fDataRate->Branch("Bps", &dataRate, "Bps/D");
    fDataRate->Branch("DERRunIDIdx", &DERRunIDIdx, "DERRunIDIdx/s");

    try
    {
        fGlobal = new TTree("Global", "LZ Global Info");
    }
    catch (std::bad_alloc& caught)
    {
        std::cerr << "bad_alloc caught: " << caught.what()
                  << std::endl;

        std::cout << "Caught while creating fGlobal"
                  << std::endl;
    }
    fGlobal->Branch("runNumber", &runNumber, "runNumber/i");
    fGlobal->Branch("formatVersion", &formatVersionID, "formatVersion/s");
    fGlobal->Branch("runStartTime", &runStartTime, "runStartTime/l");
    fGlobal->Branch("runType", &runType, "runType/s");
    fGlobal->Branch("DERRunIDIdx", &DERRunIDIdx, "DERRunIDIdx/s");

    try
    {
        fEvent = new TTree("Event", "LZ Trigger");
    }
    catch (std::bad_alloc& caught)
    {
        std::cerr << "bad_alloc caught: " << caught.what()
                  << std::endl;

        std::cout << "Caught while creating fEvent"
                  << std::endl;
    }
    fEvent->Branch("globalEvt", &globalEvt, "globalEvt/i");
    fEvent->Branch("firstData", &firstEvtData, "firstData/i");
    fEvent->Branch("nPods", &nPods, "nPods[14]/s");
    fEvent->Branch("bufferStart", &bufferStart, "bufferStart[46]/l");
    fEvent->Branch("bufferStop", &bufferStop, "bufferStop[46]/l");
    fEvent->Branch("trgType", &trgType, "trgType/s");
    fEvent->Branch("trgTimeStamp", &trgTimeStamp, "trgTimeStamp/l");
    fEvent->Branch("trgMultiplicity", &trgMultiplicity, "trgMultiplicity/s");
    fEvent->Branch("sumPODstartTS", &sumPODstartTS, "sumPODstartTS/l");
    fEvent->Branch("sumPODlength", &sumPODlength, "sumPODlength/s");
    fEvent->Branch("sumPODdata", &sumPODdata);
    fEvent->Branch("DERRunIDIdx", &DERRunIDIdx, "DERRunIDIdx/s");

    try
    {
        fData = new TTree("Data", "LZ Events");
    }
    catch (std::bad_alloc& caught)
    {
        std::cerr << "bad_alloc caught: " << caught.what()
                  << std::endl;

        std::cout << "Caught while creating fData"
                  << std::endl;
    }
    fData->Branch("evt", &evt, "evt/s");
    fData->Branch("channel", &channel, "channel/s");
    fData->Branch("hit", &hit, "hit/s");
    fData->Branch("startTime", &startTime, "startTime/l");
    fData->Branch("nSamples", &nSamples, "nSamples/s");
    fData->Branch("zData", &zData);

    try
    {
        fStageData = new TTree("StageData", "POD Waveform Data");
    }
    catch (std::bad_alloc& caught)
    {
        std::cerr << "bad_alloc caught: " << caught.what()
                  << std::endl;

        std::cout << "Caught while creating fStageData"
                  << std::endl;
    }
    fStageData->Branch("evt", &evt, "evt/s");
    fStageData->Branch("channel", &channel, "channel/s");
    fStageData->Branch("hit", &hit, "hit/s");
    fStageData->Branch("startTime", &startTime, "startTime/l");
    fStageData->Branch("nSamples", &nSamples, "nSamples/s");
    fStageData->Branch("pmtData", &stage1ZData);
    fStageData->Branch("pmtCableData", &stage2ZData);
    fStageData->Branch("ampData", &stage3ZData);
    fStageData->Branch("feedthroughCableData", &stage4ZData);
    fStageData->Branch("trigData", &stage5ZData);

    try
    {
        fRawData = new TTree("RawData", "Data for the full pulse before PODding");
    }
    catch (std::bad_alloc& caught)
    {
        std::cerr << "bad_alloc caught: " << caught.what()
                  << std::endl;

        std::cout << "Caught while creating fRawData"
                  << std::endl;
    }
    fRawData->Branch("evt", &evt, "evt/s");
    fRawData->Branch("pmt", &pmt, "pmt/s");
    fRawData->Branch("rawHGData", &rawHGData);
    fRawData->Branch("photonHGStarts", &photonHGStarts);
    fRawData->Branch("photonHGEnds", &photonHGEnds);
    fRawData->Branch("podHGStarts", &podHGStarts);
    fRawData->Branch("podHGEnds", &podHGEnds);
    fRawData->Branch("rawLGData", &rawLGData);
    fRawData->Branch("photonLGStarts", &photonLGStarts);
    fRawData->Branch("photonLGEnds", &photonLGEnds);
    fRawData->Branch("podLGStarts", &podLGStarts);
    fRawData->Branch("podLGEnds", &podLGEnds);

    try
    {
        fSummary = new TTree("Summary", "LZ Summary Info");
    }
    catch (std::bad_alloc& caught)
    {
        std::cerr << "bad_alloc caught: " << caught.what()
                  << std::endl;

        std::cout << "Caught while creating fSummary"
                  << std::endl;
    }
    fSummary->Branch("endFlag", &endFlag, "endFlag/s");
    fSummary->Branch("nEvtsFile", &nEvtsFile, "nEvtsFile/s");
    fSummary->Branch("DERRunIDIdx", &DERRunIDIdx, "DERRunIDIdx/s");

    try
    {
        fTriggerTruth = new TTree("DERTrigger", "DERTrigger");
    }
    catch (std::bad_alloc& caught)
    {
        std::cerr << "bad_alloc caught: " << caught.what()
                  << std::endl;
        std::cout << "Caught while creating TriggerTruth"
                  << std::endl;
    }
    fTriggerTruth->Branch("S1", &ChTriggerTSActiveS1);
    fTriggerTruth->Branch("S2", &ChTriggerTSActiveS2);

    if (fFile->IsZombie())
    {
        std::cout << "Error opening .root file" << std::endl;
        exit(1);
    }

    if (fFile == 0)
    {
        std::cout << "Problem with output file." << std::endl;
        exit(1);
    }

#endif
}

void RootOutputMDC2::doInputTreeClone(TFile* pointerToFile)
{
    /**
     * Method to make input tree availble in output.
     * In previous implementation this was done using the clone function.
     *
     */
}

void RootOutputMDC2::doInitInputVariables(Input* input)
{
    /**
     * Method designed to take pointer to BaccMCTruthEvent object and to
     * the TChain pointer. Both come from the input class.
     */
#if (BACC_LIB_VERSION == 6)
    try
    {
        data = input->getDataTChain();
    }
    catch (...)
    {
        std::cout << "Caught exception " << std::endl;
        return;
    }
    if (data == NULL)
        std::cout << "ERROR: Pointer to TChain is NULL" << std::endl;

    RootInputMDC2* temp = dynamic_cast<RootInputMDC2*>(input);
    temp->getBaccObj(BaccObj);
    if (input == NULL || input == 0 || BaccObj == NULL || BaccObj == 0)
    {
        std::cout << "ERROR: Could not perform dynamic_cast of type Input* to "
                  << "RootInputMDC2*" << std::endl;
    }
#endif
}

bool RootOutputMDC2::comp(short int lhs, short int rhs)
{
    /**
     * Function used to logically compare the PMTData struct
     * such that it can be sorted in increasing order: first by event, then by
     * PMT ID, and then by the arrival time.
     *
     * Returns the lesser of the compared quantities (lhs or rhs).
     */

    return lhs < rhs;
}

void RootOutputMDC2::doWriteDetectorMCTruthEvent()
{
    /**
     * Method pushes DetectorMCTruthEvent information for an event to disk.
     *
     * This method is called at the end of every event.
     * 
     * It packages the MCTruth belonging to the event into a
     * DetectorMCTruthEvent object and fills the DetectorMCTruthTree.
     *
     * The method relies on a one-to-one correspondence between DER and BACCARAT
     * events.
     */
#if (BACC_LIB_VERSION == 6)
    if (DERevt != 0)
    {
        int PulseCounter = 0; //Counting the number of "non-empty" pulses
        int VertexCounter = 0; //Counting the number of "non-empty" vectors
        //newVertexNum is a lookup to get the new Vertex Numbers from the original one
        //coming from BACCARAT (vertices with no photons in the output are not written to file).
        //It has NVertex+1 entries to allow for -1.
        //VertexIdx is a lookup to know the indices of the vertices which should be written to file.
        std::vector<short int> newVertexNum(VertexCount.size(), -1);
        std::vector<unsigned int> VertexIdx;
        VertexIdx.reserve(VertexCount.size());
        //starting at 1, as not interested in -1.
        for (int i = 1; i < VertexCount.size(); i++)
        {
            if (VertexCount[i] != 0)
            {
                newVertexNum[i] = VertexCounter;
                VertexIdx.push_back(i - 1);
                ++VertexCounter;
            }
        }

        //vertexMap is used to find out the pulse indices of pulses
        //associated with a vertex. It has as many "rows" as there were original
        //vertices. It is differentiated between S1, S2 and Scintillation pulses.
        //All other pulses are other pulses are not distinguished by type and
        //stored in a OtherIdx vector.
        std::vector<VertexInfo> vertexMap(NVertices);
        for (int i = 0; i < vertexMap.size(); i++)
        {
            vertexMap[i].S1Idx = -1;
            vertexMap[i].S2Idx = -1;
            vertexMap[i].ScintillationIdx = -1;
            vertexMap[i].DetectedS1Photons = 0;
            vertexMap[i].DetectedS2Photons = 0;
            vertexMap[i].DetectedScintPhotons = 0;
        }
        unsigned int detectedPhotons;
        for (int i = 0; i < AllPulses.size(); i++)
        {
            //Check whether pulse is non-empty.
            if (AllPulses[i].PheCount != 0)
            {
                //Get coincidence for this pulse (needed to resize vectors)
                detectedPhotons = 0;
                for (int k = 0; k < AllPulses[i].PmtsHit.size(); k++)
                {
                    if (AllPulses[i].PmtsHit[k] != 0)
                    {
                        ++AllPulses[i].Coincidence;
                        detectedPhotons += AllPulses[i].PmtsHit[k];
                    }
                }
                if (AllPulses[i].BaccVertexNumber != -1)
                {
                    if (AllPulses[i].PulseType == 1)
                    {
                        vertexMap[AllPulses[i].BaccVertexNumber].S1Idx = PulseCounter;
                        vertexMap[AllPulses[i].BaccVertexNumber].DetectedS1Photons = detectedPhotons;
                    }
                    else if (AllPulses[i].PulseType == 2)
                    {
                        vertexMap[AllPulses[i].BaccVertexNumber].S2Idx = PulseCounter;
                        vertexMap[AllPulses[i].BaccVertexNumber].DetectedS2Photons = detectedPhotons;
                    }
                    else if (AllPulses[i].PulseType == 4)
                    {
                        vertexMap[AllPulses[i].BaccVertexNumber].ScintillationIdx = PulseCounter;
                        vertexMap[AllPulses[i].BaccVertexNumber].DetectedScintPhotons = detectedPhotons;
                    }
                    else
                        vertexMap[AllPulses[i].BaccVertexNumber].OtherIdx.push_back(PulseCounter);
                }
                ++PulseCounter;
            }
            if (AftPulses[i].PheCount != 0)
            {
                if (AftPulses[i].BaccVertexNumber != -1)
                {
                    if (AftPulses[i].PulseType == 1)
                        vertexMap[AftPulses[i].BaccVertexNumber].S1Idx = PulseCounter;
                    else if (AftPulses[i].PulseType == 2)
                        vertexMap[AftPulses[i].BaccVertexNumber].S2Idx = PulseCounter;
                    else if (AftPulses[i].PulseType == 4)
                        vertexMap[AftPulses[i].BaccVertexNumber].ScintillationIdx = PulseCounter;
                    else
                        vertexMap[AftPulses[i].BaccVertexNumber].OtherIdx.push_back(PulseCounter);
                }
                //Get coincidence for this pulse (needed to resize vectors)
                for (int k = 0; k < AftPulses[i].PmtsHit.size(); k++)
                {
                    if (AftPulses[i].PmtsHit[k] != 0)
                    {
                        ++AftPulses[i].Coincidence;
                    }
                }
                ++PulseCounter;
            }
        }

        //Filling of per event information.
        //Each DER event corresponds to exactly one BACCARAT event.
        TruthObj->iDEREventNumber = (unsigned short)DERevt - 1; //subtract 1 to match data tree.
        TruthObj->iRunNumber = (*BaccObj)->iRunNumber;
        TruthObj->iBaccEventNumber = (*BaccObj)->iEventNumber;
        TruthObj->sParentParticleName = (*BaccObj)->sParentParticleName;
        TruthObj->sParentVolumeName = (*BaccObj)->sParentVolumeName;
        TruthObj->fParentPosition_mm = (*BaccObj)->fParentPosition_mm;
        TruthObj->fParentEnergy_keV = (*BaccObj)->fParentEnergy_keV;
        TruthObj->fParentDirection = (*BaccObj)->fParentDirection;
        TruthObj->fParentTime_ns = (*BaccObj)->fParentTime_ns;

        //Iterate over BaccVertices vector to fill DetectorVertexMCTruth
        TruthObj->vertices.resize(VertexCounter);
        for (int i = 0; i < VertexIdx.size(); i++)
        {
            BaccVertex = (*BaccObj)->vertices[VertexIdx[i]];
            DetectorVertexMCTruth newVertexTruth;
            newVertexTruth.sParticleName = BaccVertex.sParticleName;
            newVertexTruth.sVolumeName = BaccVertex.sVolumeName;
            newVertexTruth.fPosition_mm = BaccVertex.fPosition_mm;
            newVertexTruth.fEnergyDep_keV = BaccVertex.fEnergyDep_keV;
            newVertexTruth.fTime_ns = BaccVertex.fTime_ns;
            newVertexTruth.fElectricFieldDirection = BaccVertex.fElectricFieldDirection;
            newVertexTruth.fElectricFieldStrength_Vcm = BaccVertex.fElectricFieldStrength_Vcm;
            newVertexTruth.iRawS1Photons = BaccVertex.iRawS1Photons;
            newVertexTruth.iRawS2Photons = BaccVertex.iRawS2Photons;
            newVertexTruth.iRawScintPhotons = BaccVertex.iRawScintPhotons;
            newVertexTruth.iS1PhotonHits = BaccVertex.iS1PhotonHits;
            newVertexTruth.iS2PhotonHits = BaccVertex.iS2PhotonHits;
            newVertexTruth.iScintPhotonHits = BaccVertex.iScintPhotonHits;
            newVertexTruth.iDetectedS1Photons = vertexMap[VertexIdx[i]].DetectedS1Photons;
            newVertexTruth.iDetectedS2Photons = vertexMap[VertexIdx[i]].DetectedS2Photons;
            newVertexTruth.iDetectedScintPhotons = vertexMap[VertexIdx[i]].DetectedScintPhotons;
            newVertexTruth.iS1PulseIndex = vertexMap[VertexIdx[i]].S1Idx;
            newVertexTruth.iS2PulseIndex = vertexMap[VertexIdx[i]].S2Idx;
            newVertexTruth.iScintillationPulseIndex = vertexMap[VertexIdx[i]].ScintillationIdx;
            newVertexTruth.iArtifactPulseIndices.resize(vertexMap[VertexIdx[i]].OtherIdx.size());
            for (int j = 0; j < vertexMap[VertexIdx[i]].OtherIdx.size(); j++)
            {
                newVertexTruth.iArtifactPulseIndices[j] = vertexMap[VertexIdx[i]].OtherIdx[j];
            }
            TruthObj->vertices[i] = newVertexTruth;
        }

        //Loop over AllPulses and AftPulses to fill PulseTruth
        int pmtCntr;
        int idx = 0;
        TruthObj->pulses.resize(PulseCounter);
        for (int i = 0; i < AllPulses.size(); i++)
        {
            DetectorPulseMCTruth newPulseTruth;
            if (AllPulses[i].PheCount != 0)
            {
                pmtCntr = 0;
                newPulseTruth.iPulseIdentifier = AllPulses[i].PulseType;
                newPulseTruth.iPheCount = AllPulses[i].PheCount;
                newPulseTruth.iVertexNumber = newVertexNum[AllPulses[i].BaccVertexNumber + 1];
                newPulseTruth.fFirstPheTime_ns = AllPulses[i].FirstPheTime;
                newPulseTruth.fLastPheTime_ns = AllPulses[i].LastPheTime;
                newPulseTruth.iPMTHits.resize(AllPulses[i].Coincidence);
                newPulseTruth.iPMTIndices.resize(AllPulses[i].Coincidence);
                for (int j = 0; j < AllPulses[i].PmtsHit.size(); j++)
                {
                    if (AllPulses[i].PmtsHit[j] != 0)
                    {
                        newPulseTruth.iPMTIndices[pmtCntr] = j;
                        newPulseTruth.iPMTHits[pmtCntr] = AllPulses[i].PmtsHit[j];
                        ++pmtCntr;
                    }
                }
                TruthObj->pulses[idx] = newPulseTruth;
                ++idx;
            }
            if (AftPulses[i].PheCount != 0)
            {
                pmtCntr = 0;
                newPulseTruth.iPulseIdentifier = 0;
                newPulseTruth.iPheCount = AftPulses[i].PheCount;
                newPulseTruth.iVertexNumber = newVertexNum[AftPulses[i].BaccVertexNumber + 1];
                newPulseTruth.fFirstPheTime_ns = AftPulses[i].FirstPheTime;
                newPulseTruth.fLastPheTime_ns = AftPulses[i].LastPheTime;
                newPulseTruth.iPMTHits.resize(AftPulses[i].Coincidence);
                newPulseTruth.iPMTIndices.resize(AftPulses[i].Coincidence);
                pmtCntr = 0;
                for (int j = 0; j < AftPulses[i].PmtsHit.size(); j++)
                {
                    if (AftPulses[i].PmtsHit[j] != 0)
                    {
                        newPulseTruth.iPMTIndices[pmtCntr] = j;
                        newPulseTruth.iPMTHits[pmtCntr] = AftPulses[i].PmtsHit[j];
                        ++pmtCntr;
                    }
                }
                TruthObj->pulses[idx] = newPulseTruth;
                ++idx;
            }
        }
        TruthObj->darkCounts.resize(darkpulses.size());
        for (int i = 0; i < darkpulses.size(); i++)
        {
            DarkCountMCTruth newDarkCount;
            newDarkCount.iPMTIndex = darkpulses[i].pmt;
            newDarkCount.fTime_ns = darkpulses[i].time;
            TruthObj->darkCounts[i] = newDarkCount;
        }
        fDetectorMCTruth->Fill();
        TruthObj->ClearEverything();
        std::vector<PulseInfo>().swap(AllPulses);
        std::vector<PulseInfo>().swap(AftPulses);
        std::vector<DarkCountInfo>().swap(darkpulses);
        std::vector<unsigned int>().swap(VertexCount);
        DERevt = 0;
    }
#endif
}

void RootOutputMDC2::doWriteTruthTree()
{
    /**
     * Method intentionally left empty.
     */
}

void RootOutputMDC2::doPreparePulseMCTruth(unsigned long long nVertices, unsigned long long TMax)
{
    /*
    * Method to prepare structures to collect Pulse information. 
    */
    DERevt = 0;
    NVertices = nVertices;
    TMaxEvt = TMax;
    VertexCount.resize(NVertices + 1, 0);
}

void RootOutputMDC2::doWriteDERMCTruth(std::shared_ptr<MCTruth> theTruth)
{
    /*
     * In this implementation of the method, photon truth information is sorted into
     * pulse information using the AllPulses map and AftPulses map.
     */
    if (theTruth != nullptr)
    {
        int prevSize = AllPulses.size();
        int maxPulseID = prevSize;
        //Check what the maximum PulseID is.
        for (int i = 0; i < theTruth->getTruthSize(); i++)
        {
            if (theTruth->getMCTruthDataObject(i)->DEREvt)
            {
                if (theTruth->getMCTruthDataObject(i)->PulseID >= maxPulseID)
                    maxPulseID = (theTruth->getMCTruthDataObject(i)->PulseID + 1);
            }
        }
        //Resize AllPulses and AftPulses vector to be able to accomodate all pulses
        //AftPulses has the same size as AllPulses, as every pulse can have associated afterpulses.
        AllPulses.reserve(maxPulseID);
        AftPulses.reserve(maxPulseID);
        for (int i = prevSize; i < maxPulseID; i++)
        {
            PulseInfo zeroPulse;
            zeroPulse.SimEvt = 0;
            zeroPulse.PulseType = 0;
            zeroPulse.PheCount = 0;
            zeroPulse.BaccVertexNumber = -1;
            zeroPulse.EvtNumber = 0;
            zeroPulse.FirstPheTime = TMaxEvt;
            zeroPulse.LastPheTime = 0;
            zeroPulse.Coincidence = 0;
            zeroPulse.PmtsHit.resize(921, 0);
            AllPulses.push_back(zeroPulse);
            AftPulses.push_back(zeroPulse);
        }

        //As dark counts are written to their own MCTruth class, they are sorted
        //into their own vector.
        int prevDarkCountSize = darkpulses.size();
        int newDarkCountSize = theTruth->getNDarkCounts();
        darkpulses.resize(prevDarkCountSize + newDarkCountSize);

        //Loop over the truth and sort the photon information into pulses
        int pulseID = 0;
        int cntr = 0;
        for (int i = 0; i < theTruth->getTruthSize(); i++)
        {
            //Only want to keep the information of photons which made it into a POD.
            if (theTruth->getMCTruthDataObject(i)->DEREvt)
            {
                DERevt = theTruth->getMCTruthDataObject(i)->DEREvt;
                ++VertexCount[theTruth->getMCTruthDataObject(i)->VertexNumber + 1];

                //Add dark counts to the darkpulses vector.
                if (theTruth->getMCTruthDataObject(i)->pheType == 5)
                {
                    DarkCountInfo darkCount;
                    darkCount.pmt = theTruth->getMCTruthDataObject(i)->PMTNumber;
                    darkCount.time = theTruth->getMCTruthDataObject(i)->ArrivalTime;
                    darkpulses[prevDarkCountSize + cntr] = darkCount;
                    ++cntr;
                }
                //For all other photon truth information the PulseID (coming from BACCARAT MCTruth)
                //is used to sort them into pulses. For Afterpulses the PulseID is set to that of
                //the photon which initiated the afterpulse.
                else
                {
                    pulseID = theTruth->getMCTruthDataObject(i)->PulseID;
                    //PheType == 6 means it was an afterpulse, therefore the information is stored
                    //in AftpUlses
                    if (theTruth->getMCTruthDataObject(i)->pheType == 6)
                    {
                        AftPulses[pulseID].SimEvt = (unsigned int)theTruth->getMCTruthDataObject(i)->SimEvt;
                        AftPulses[pulseID].PulseType = theTruth->getMCTruthDataObject(i)->InteractionIdentifier;
                        AftPulses[pulseID].BaccVertexNumber = theTruth->getMCTruthDataObject(i)->VertexNumber;

                        //Find the first and last time of the pulse.
                        if (AftPulses[pulseID].FirstPheTime > theTruth->getMCTruthDataObject(i)->ArrivalTime)
                        {
                            AftPulses[pulseID].FirstPheTime = theTruth->getMCTruthDataObject(i)->ArrivalTime;
                        }
                        if (AftPulses[pulseID].LastPheTime < theTruth->getMCTruthDataObject(i)->ArrivalTime)
                        {
                            AftPulses[pulseID].LastPheTime = theTruth->getMCTruthDataObject(i)->ArrivalTime;
                        }

                        //Increment count of phe for the PMT in question.
                        AftPulses[pulseID].PmtsHit[(int)theTruth->getMCTruthDataObject(i)->PMTNumber] += 1;
                        AftPulses[pulseID].PheCount += 1;
                    }
                    //All other photon information goes into the pulses in AllPulses.
                    else
                    {
                        AllPulses[pulseID].SimEvt = (unsigned int)theTruth->getMCTruthDataObject(i)->SimEvt;
                        AllPulses[pulseID].PulseType = theTruth->getMCTruthDataObject(i)->InteractionIdentifier;
                        AllPulses[pulseID].BaccVertexNumber = theTruth->getMCTruthDataObject(i)->VertexNumber;

                        //Find the first and last time of the pulse.
                        if (AllPulses[pulseID].FirstPheTime > theTruth->getMCTruthDataObject(i)->ArrivalTime)
                        {
                            AllPulses[pulseID].FirstPheTime = theTruth->getMCTruthDataObject(i)->ArrivalTime;
                        }
                        if (AllPulses[pulseID].LastPheTime < theTruth->getMCTruthDataObject(i)->ArrivalTime)
                        {
                            AllPulses[pulseID].LastPheTime = theTruth->getMCTruthDataObject(i)->ArrivalTime;
                        }

                        //Increment count of phe for the PMT in question.
                        AllPulses[pulseID].PmtsHit[(int)theTruth->getMCTruthDataObject(i)->PMTNumber] += 1;
                        AllPulses[pulseID].PheCount += 1;
                        //If double phe add additional count
                        if (theTruth->getMCTruthDataObject(i)->pheType == 2)
                            AllPulses[pulseID].PheCount += 1;
                    }
                }
            }
        }
    }
}

void RootOutputMDC2::CloseFile()
{
    /**
     * Method that closes the DER .root file.
     *
     * Before closing, the relavant information for each remaining branch should
     * be finally written to disk.
     */
#if (BACC_LIB_VERSION == 6)
    std::string CnfTruth = global::config->getConfig("MCTruthInfo").c_str();
    if (CnfTruth == "true")
    {
        if (fDetectorMCTruth != 0)
            fDetectorMCTruth->Write();
        else
            std::cout << "fDetectorMCTruth is 0, problem!" << std::endl;
    }

    if (fDataRate != 0)
        fDataRate->Write();
    else
        std::cout << "fDataRate is 0, problem!" << std::endl;

    if (fData != 0)
        fData->Write();
    else
        std::cout << "fData is 0, problem!" << std::endl;

    if (fStageData != 0)
        fStageData->Write();
    else
        std::cout << "fStageData is 0, problem!" << std::endl;

    if (fRawData != 0)
        fRawData->Write();
    else
        std::cout << "fRawData is 0, problem!" << std::endl;

    if (fEvent != 0)
        fEvent->Write();
    else
        std::cout << "fEvent is 0, problem!" << std::endl;

    if (fTriggerTruth != 0)
        fTriggerTruth->Write();
    else
        std::cout << "fTriggerTruth is 0, problem!" << std::endl;

    if (fFile != 0)
        fFile->Close();
    else
        std::cout << "fFile is 0, problem!" << std::endl;

    std::string patho = fpath;
    size_t last = fpath.find_last_of(".");
    std::string pathn = fpath.substr(0, last);
    if (std::rename(patho.c_str(), pathn.c_str()))
    {
        std::cout << "Error renaming file: " << patho << " to " << pathn << std::endl;
    }
#endif
}

void RootOutputMDC2::doWriteStageData(PODContainer& stagePODs)
{

    if (stagePODs.size() > 0)
    {
        stage1ZData.clear();
        stage2ZData.clear();
        stage3ZData.clear();
        stage4ZData.clear();
        stage5ZData.clear();
      
        evt = stagePODs[0]->getEvent();
        channel = stagePODs[0]->getChannel();
        hit = stagePODs[0]->getHitID();
        startTime = stagePODs[0]->getPODStartTimeStamp();
        nSamples = stagePODs[0]->getPODLength();

        std::vector<std::vector<Short_t>*> data;
        if (global::config->getConfig("UseS2Trigger") == "true")
        {
            data.push_back(&stage5ZData);
            data.push_back(&stage1ZData);
            data.push_back(&stage2ZData);
            data.push_back(&stage3ZData);
            data.push_back(&stage4ZData);
        }
        else
        {
            data.push_back(&stage1ZData);
            data.push_back(&stage2ZData);
            data.push_back(&stage3ZData);
            data.push_back(&stage4ZData);
            data.push_back(&stage5ZData);
        }

        for (size_t i = 0; i < stagePODs.size(); ++i)
        {
	  *data[i] = stagePODs[i]->getSamples();
        }
    }

    fStageData->Fill();
}

void RootOutputMDC2::doWriteRawData(Pulse& theLGPulse, Pulse& theHGPulse, 
				    std::shared_ptr<PODContainer> theLGPODs, 
				    std::shared_ptr<PODContainer> theHGPODs)
{
  evt = theHGPulse.getEvent();
  pmt = theHGPulse.getChannel();

  rawHGData.clear();
  rawLGData.clear();
  photonHGStarts.clear();
  photonHGEnds.clear();
  photonLGStarts.clear();
  photonLGEnds.clear();
  podHGStarts.clear();
  podHGEnds.clear();
  podLGStarts.clear();
  podLGEnds.clear();

  std::vector<Double_t>* doubleHGData = &rawHGData;
  (*doubleHGData)= theHGPulse.getSamples();

  std::vector<Int_t>* photonHGStartData = &photonHGStarts;
  std::vector<Int_t>* photonHGEndData = &photonHGEnds;

  for(size_t i = 0; i<theHGPulse.getPhotonSize(); ++i){
    (*photonHGStartData).push_back(theHGPulse.getPhotonIntervalAt(i).first);
    (*photonHGEndData).push_back(theHGPulse.getPhotonIntervalAt(i).second);
  }

  std::vector<unsigned long long> longPodHGStartData = theHGPODs->getPodStarts();
  std::vector<Int_t>* podHGStartData = &podHGStarts;
  (*podHGStartData).assign(longPodHGStartData.begin(),
			   longPodHGStartData.end());

  std::vector<unsigned long long> longPodHGEndData = theHGPODs->getPodEnds();
  std::vector<Int_t>* podHGEndData = &podHGEnds;
  (*podHGEndData).assign(longPodHGEndData.begin(),
			 longPodHGEndData.end());

  //-------------------------------------------------------------------------

  std::vector<Double_t>* doubleLGData = &rawLGData;
  (*doubleLGData)= theLGPulse.getSamples();

  std::vector<Int_t>* photonLGStartData = &photonLGStarts;
  std::vector<Int_t>* photonLGEndData = &photonLGEnds;

  for(size_t i = 0; i<theLGPulse.getPhotonSize(); ++i){
    (*photonLGStartData).push_back(theLGPulse.getPhotonIntervalAt(i).first);
    (*photonLGEndData).push_back(theLGPulse.getPhotonIntervalAt(i).second);
  }

  std::vector<unsigned long long> longPodLGStartData = theLGPODs->getPodStarts();
  std::vector<Int_t>* podLGStartData = &podLGStarts;
  (*podLGStartData).assign(longPodLGStartData.begin(),
			   longPodLGStartData.end());

  std::vector<unsigned long long> longPodLGEndData = theLGPODs->getPodEnds();
  std::vector<Int_t>* podLGEndData = &podLGEnds;
  (*podLGEndData).assign(longPodLGEndData.begin(),
			 longPodLGEndData.end());

  fRawData->Fill();
}
