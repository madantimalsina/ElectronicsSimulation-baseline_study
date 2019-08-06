//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Dec  7 07:11:00 2018 by ROOT version 6.04/02
// from TTree StageData/POD Waveform Data
// found on file: lz_197001010000_000010_000000_raw.root
//////////////////////////////////////////////////////////

#ifndef StageData_h
#define StageData_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"

class StageData {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   UShort_t        evt;
   UShort_t        channel;
   UShort_t        hit;
   ULong64_t       startTime;
   UShort_t        nSamples;
   std::vector<short>   *pmtData;
   std::vector<short>   *pmtCableData;
   std::vector<short>   *ampData;
   std::vector<short>   *feedthroughCableData;
   std::vector<short>   *trigData;

   // List of branches
   TBranch        *b_evt;   //!
   TBranch        *b_channel;   //!
   TBranch        *b_hit;   //!
   TBranch        *b_startTime;   //!
   TBranch        *b_nSamples;   //!
   TBranch        *b_pmtData;   //!
   TBranch        *b_pmtCableData;   //!
   TBranch        *b_ampData;   //!
   TBranch        *b_feedthroughCableData;   //!
   TBranch        *b_trigData;   //!

   StageData(TTree *tree=0);
   virtual ~StageData();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef StageData_cxx
StageData::StageData(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("lz_197001010000_000010_000000_raw.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("lz_197001010000_000010_000000_raw.root");
      }
      f->GetObject("StageData",tree);

   }
   Init(tree);
}

StageData::~StageData()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t StageData::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t StageData::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void StageData::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   pmtData = 0;
   pmtCableData = 0;
   ampData = 0;
   feedthroughCableData = 0;
   trigData = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("evt", &evt, &b_evt);
   fChain->SetBranchAddress("channel", &channel, &b_channel);
   fChain->SetBranchAddress("hit", &hit, &b_hit);
   fChain->SetBranchAddress("startTime", &startTime, &b_startTime);
   fChain->SetBranchAddress("nSamples", &nSamples, &b_nSamples);
   fChain->SetBranchAddress("pmtData", &pmtData, &b_pmtData);
   fChain->SetBranchAddress("pmtCableData", &pmtCableData, &b_pmtCableData);
   fChain->SetBranchAddress("ampData", &ampData, &b_ampData);
   fChain->SetBranchAddress("feedthroughCableData", &feedthroughCableData, &b_feedthroughCableData);
   fChain->SetBranchAddress("trigData", &trigData, &b_trigData);
   Notify();
}

Bool_t StageData::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void StageData::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t StageData::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef StageData_cxx
