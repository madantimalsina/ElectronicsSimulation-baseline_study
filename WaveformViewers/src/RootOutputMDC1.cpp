//
//  RootOutputMDC1.cpp
//  DER
//
//  Created by Cees Carels on 05/06/2017.
//  Copyright © 2017 Carels. All rights reserved.
//

#include "RootOutputMDC1.hpp"

RootOutputMDC1::RootOutputMDC1()
{
    /**
     * Constructor for RootOutputMDC1.
     */
    outputFormat = format::revision::ROOTvMDC1;
    LZ_Event_Format_Revision = format::LZ_ICD_08_0007::Rev_A;
    nPODInEvt = 1;
    nPODInFile = 0;
#if (BACC_LIB_VERSION == 3)
    TruthObj = 0;
    BaccObj = 0;
#endif
}

RootOutputMDC1::~RootOutputMDC1()
{
/**
     * Destructor for RootOutputJan2017.
     */
#if (BACC_LIB_VERSION == 3)
    delete TruthObj;
    delete BaccObj;
#endif
}

void RootOutputMDC1::doWriteData(POD& theEBDataPOD, const int& DCID)
{
/**
     * Method that gets the POD object for the current channel, and writes it
     * to disk.
     */
#if (BACC_LIB_VERSION == 3)
    evt = theEBDataPOD.getEvent();
    LUXSimRunNumber = theEBDataPOD.getLUXSimRunNumber();
    channel = theEBDataPOD.getChannel();
    hit = theEBDataPOD.getHitID();
    startTime = theEBDataPOD.getPODStartTimeStamp();
    nSamples = theEBDataPOD.getPODLength();

    //Add samples to sumPODLength to get running total for EBEvent Tree.
    sumPODlength += nSamples;
    ++nPODInEvt;
    if (startTime < sumPODstartTS)
        sumPODstartTS = startTime;

    zData.resize(theEBDataPOD.size());
    for (int i = 0; i < theEBDataPOD.size(); i++)
    {
        zData[i] = theEBDataPOD[i];
    }
    fData->Fill();
#endif
}

void RootOutputMDC1::createOutFile(const bool NewSeries,
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
#if (BACC_LIB_VERSION == 3)
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

void RootOutputMDC1::doInputTreeClone(TFile* pointerToFile)
{
    /**
     * Method to make input tree availble in output.
     * In previous implementation this was done using the clone function.
     * 
     * In this version we read the vertex tree and package it for DetectorMCTruth output.
     */

#if (BACC_LIB_VERSION == 3)
    fMCTruthTree = (TTree*)pointerToFile->Get("MCTruthTree");
    if (fMCTruthTree == 0)
    {
        std::cout << "Could not find MCTruthTree, "
                  << "used for RootvJun2017 format."
                  << std::endl;
    }
#endif
}

void RootOutputMDC1::doWriteDetectorMCTruthEvent()
{
/**
     * This method is called at the end of every event. It packages the MCTruth belonging
     * to the event into a DetectorMCTruthEvent object and fills the DetectorMCTruthTree.
     *
     * The method relies on a one-to-one correspondence between DER and BACCARAT events.
     */
#if (BACC_LIB_VERSION == 3)
    fMCTruthTree->SetBranchAddress("MCTruthEvent", &BaccObj);
    if (fDetectorMCTruth == 0)
        std::cout << "fDetectorMCTruth == 0, problem!" << std::endl;

    unsigned long long vecSize = AllPhotons.size();
    //vecSize is only non-zero if MCTruth is active
    if (vecSize)
    {
        //Find all unique vertex numbers and their frequency.
        //Vertex Number -1 is used for photons which did not have an associated
        //vertex, e.g. dark counts. An extra -1 is added to the AllUniqueVertexNum map,
        //to ensure the first entry is always -1 and avoid having to check the lowest number.
        std::map<short int, int> AllUniqueVertexNum;
        AllUniqueVertexNum[-1] = 1;
        for (int i = 0; i < vecSize; i++)
        {
            ++AllUniqueVertexNum[AllPhotons[i].BaccVertexIdx];
        }

        //Resize TruthObj->photons and TruthObj->vertices to correct length.
        TruthObj->photons.resize(vecSize);
        TruthObj->vertices.resize(AllUniqueVertexNum.size() - 1);

        //vertexMap is a vector containing one vector per vertexNumber starting at
        //-1 and going up to the largest vertexNumber present in the current event.
        //These vectors will be filled with indices corresponding to PhotonMCTruth
        //entries originating at the respective vertex.
        //Vertex number -1 always corresponds to index 0. Therefore the vertexNumber
        //value and its index in vertexMap are always offset by 1 and the largest
        //vertexNumber value and the required vector size are offset by 2.

        //The largest vertexNumber is found using the reverse iterator. The vertexMap
        //is resized accordingly. Subsequently the vertexMap vectors are resized
        //according to the freqency of the vertexNumbers.
        int maxVertex = AllUniqueVertexNum.rbegin()->first;
        std::vector<std::vector<int>> vertexMap(maxVertex + 2);
        for (std::map<short int, int>::iterator it = AllUniqueVertexNum.begin();
             it != AllUniqueVertexNum.end();
             it++)
        {
            vertexMap[(it->first) + 1].resize(it->second);
        }

        //vertexMapCount is used to determine indices when filling the vertexMap.
        std::vector<int> vertexMapCount(maxVertex + 2, 0);

        //Filling of per event information. This relies on one-to-one DER to BACCARAT
        //event correspondence.
        fMCTruthTree->GetEntry(SimEvt);

        TruthObj->iDEREventNumber = (unsigned short)DERevt;
        TruthObj->iRunNumber = BaccObj->iRunNumber;
        TruthObj->iFirstPodIndex = nPODInFile;
        TruthObj->iBaccEventNumber = BaccObj->iEventNumber;
        TruthObj->sParentParticleName = BaccObj->sParentParticleName;
        TruthObj->sParentVolumeName = BaccObj->sParentVolumeName;
        TruthObj->fParentPosition_mm = BaccObj->fParentPosition_mm;
        TruthObj->fParentEnergy_keV = BaccObj->fParentEnergy_keV;
        TruthObj->fParentDirection = BaccObj->fParentDirection;
        TruthObj->fParentTime_ns = BaccObj->fParentTime_ns;

        for (int j = 0; j < vecSize; j++)
        {
            //Filling of per photon information
            DetectorPhotonMCTruth newPhotonTruth;
            newPhotonTruth.fTime_ns = AllPhotons[j].DERArrivalTime;
            newPhotonTruth.iOrigin = AllPhotons[j].Origin;
            newPhotonTruth.iVertexNumber = AllPhotons[j].BaccVertexIdx;
            newPhotonTruth.iHighGainPodIndex = AllPhotons[j].HGPODIdx;
            newPhotonTruth.iLowGainPodIndex = AllPhotons[j].LGPODIdx;

            TruthObj->photons[j] = newPhotonTruth;

            //Record in vertex map, which vertex the current photon corresponds to.
            //+1 is needed as vertex number -1 corresponds to index 0.
            vertexMap[AllPhotons[j].BaccVertexIdx + 1][vertexMapCount[AllPhotons[j].BaccVertexIdx + 1]] = j;
            ++vertexMapCount[AllPhotons[j].BaccVertexIdx + 1]; //update next index to be filled.
        }

        int oldVertexNum = 0;
        int newVertexNum = 0;
        //Iterate over AllUniqueVertexNum, fill vertices entry and look up
        //corresponding photons in vertex map to update their iVertexNumber.
        //The loop starts at second AllUniqueVertexNum entry, as the first entry is
        //-1, which corresponds to no vertex.
        for (std::map<short int, int>::iterator it = std::next(AllUniqueVertexNum.begin());
             it != AllUniqueVertexNum.end();
             it++)
        {
            oldVertexNum = it->first;
            BaccVertex = BaccObj->vertices[oldVertexNum];

            DetectorVertexMCTruth newVertexTruth;
            newVertexTruth.sParticleName = BaccVertex.sParticleName;
            newVertexTruth.sVolumeName = BaccVertex.sVolumeName;
            newVertexTruth.fPosition_mm = BaccVertex.fPosition_mm;
            newVertexTruth.fEnergyDep_keV = BaccVertex.fEnergyDep_keV;
            newVertexTruth.fTime_ns = BaccVertex.fTime_ns;
            newVertexTruth.fElectricFieldDirection = BaccVertex.fElectricFieldDirection;
            newVertexTruth.fElectricFieldStrength_Vcm = BaccVertex.fElectricFieldStrength_Vcm;
            newVertexTruth.iS1Quanta = BaccVertex.iS1Quanta;
            newVertexTruth.iS2Quanta = BaccVertex.iS2Quanta;

            TruthObj->vertices[newVertexNum] = newVertexTruth;

            //For all photons originated at the current vertex, the iVertexNumber
            //in the DetectorPhotonMCTruth is updated. Updating the iVertexNumber
            //is necessary, as only vertices, which have at least one corresponding photon
            //surviving are recorded in the DetectorMCTruth output. Therefore the original
            //iVertexNumber might not correspond to the actual index in the vertices vector.
            for (int l = 0; l < vertexMap[oldVertexNum + 1].size(); l++)
            {
                TruthObj->photons[vertexMap[oldVertexNum + 1][l]].iVertexNumber = newVertexNum;
            }
            ++newVertexNum;
        }
        //Filling the TTree
        fDetectorMCTruth->Fill();

        //Clear object and vectors used for buffering.
        TruthObj->ClearEverything();
        std::vector<PhotonInfo>().swap(AllPhotons);
    }

    //Updating nPODInFile counter and resetting nPODInEvt counter
    nPODInFile += (nPODInEvt - 1);
    nPODInEvt = 1;
#endif
}

void RootOutputMDC1::doWriteTruthTree()
{
    /**
     * Method intentionally left empty.
     */
}

void RootOutputMDC1::doWriteDERMCTruth(std::shared_ptr<MCTruth> theTruth)
{
    /*
     * Method to write PODTruth data to disk and add the MCTruth Data objects to
     * a vector.
     */
    if (theTruth != nullptr)
    {
        int prevSize = AllPhotons.size();
        int newSize = theTruth->getNPhotonsInPODs();

        AllPhotons.resize(prevSize + newSize);

        int counter = 0;
        for (int j = 0; j < theTruth->getTruthSize(); j++)
        {
            //Only keep information for photons which made it into a POD.
            if (theTruth->getMCTruthDataObject(j)->DEREvt)
            {
                //Determine iOrigin type: Combination of InteractionIdentifier and pheType
                //First digit gives information about pheType:
                //1 SPHE, 2 DPHE, 3 firstDynHits, 4 SecDynColl, 5 DarkCount, 6 AftPulse
                //Second digit gives information abour InteractionIdentifier:
                //1 S1, 2 S2, 3 Cherenkov, 4 Scintillation, 5 Other.
                Origin = (unsigned char)(10 * (theTruth->getMCTruthDataObject(j)->pheType) + (theTruth->getMCTruthDataObject(j)->InteractionIdentifier));

                //per event variables
                DERevt = theTruth->getMCTruthDataObject(j)->DEREvt;
                SimEvt = theTruth->getMCTruthDataObject(j)->SimEvt;
                RunNum = theTruth->getMCTruthDataObject(j)->SimRunNumber;

                //Store MCTruth information in buffer
                PhotonInfo ph;
                ph.Origin = Origin;
                ph.DERArrivalTime = theTruth->getMCTruthDataObject(j)->ArrivalTime;
                ph.HGPODIdx = theTruth->getMCTruthDataObject(j)->HGPODIdx;
                ph.LGPODIdx = theTruth->getMCTruthDataObject(j)->LGPODIdx;
                ph.BaccVertexIdx = theTruth->getMCTruthDataObject(j)->VertexNumber;

                AllPhotons[prevSize + counter] = ph;

                ++counter;
            }
        }
    }
}

void RootOutputMDC1::CloseFile()
{
/**
     * Method that closes the DER .root file.
     *
     * Before closing, the relavant information for each remaining branch should
     * be finally written to disk.
     */
#if (BACC_LIB_VERSION == 3)
    std::string CnfTruth = Config->getConfig("MCTruthInfo").c_str();
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

    delete fFile;
#endif
}
