//
//  RootOutput.hpp
//  devices
//
//  Created by Cees Carels on 22/06/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef RootOutput_hpp
#define RootOutput_hpp

#include <stdio.h>

#include "Output.hpp"

#include "MCTruth.hpp"

/**
 * Class providing the implementation of the .root output file format, in
 * agreement with the Event Builder output file format.
 *
 * If this class is used for output, then the output of the DER simulation will
 * contain the output data in the same format that the Event Builder would
 * produce. In addition, a number of other trees are written that
 */

class RootOutput : public Output
{
public:
    RootOutput();

    ~RootOutput(); //!< Destructor.

    void doWriteDERSummary(DERSummary& theDERSummary); //!< Summary.
    virtual void doWriteGlobal(EBGlobal* theGlobalSummary = NULL);
    virtual void doWriteEvent(EBEvent& theEBEvent); //!< Write event data.
    void doWriteSummary(EBSummary& theEBSummary); //!< Write summary.
    void doPrepareEvent(); //!< Not used for .root output.

    void doResolveEvtPtrs(); //!< Not used for .root output.
    double StatFileSize(); //!< Get file size.
    double DataRate(TStopwatch& Timer); //!< Write the data rate.
    void doWriteTime(const double Real, const double CPU);
    virtual void doPreparePulseMCTruth(unsigned long long NVertices,
        unsigned long long TMax);

protected:
    std::string ext = ".root"; //!< File extension.
    std::string fpath; //!< File path of ROOT output file.

    //Root output file
    TFile* fFile; //!< Pointer to output file object.
    TTree* fDERSummary; //!< Pointer to DERSummary output.
    TTree* fDERTime; //!< Pointer to DERTime output.
    TTree* fLUXSimSummary; //!< Pointer to LUX Sim header tree output.
    TTree* fPhotonMCTruth; //!< Pointer to Photon MC Truth output.
    TTree* fDERMCTruth; //!< Pointer to DER MC Truth output.
    TTree* fDERMCPODTruth; //!< Pointer to DER MC POD Truth output.
    TTree* fVertexMCTruth; //!< Pointer to MC Truth Vertex output.
    TTree* fDetectorMCTruth; //!< Pointer to Detector MC Truth output.
    TTree* fTriggerTruth; //!< Pointer to TriggerTruth output.
    TTree* fDataRate; //!< Pointer to DataRate output.
    TTree* fDERSubsets; //!< Pointer to DERSubset output (deprecated).
    TTree* fGlobal; //!< Pointer to Global output.
    TTree* fEvent; //!< Pointer to Event output.
    TTree* fData; //!< Pointer to Data output.
    TTree* fSummary; //!< Pointer to Summary output.
    TTree* fStageData;
    TTree* fRawData;
};

#endif /* RootOutput_hpp */
