//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Feb 14 04:59:03 2019 by ROOT version 6.04/02
// from TTree RawData/Data for the full pulse before PODding
// found on file: lz_197001061853_000010_000000_raw.root
//////////////////////////////////////////////////////////

#ifndef RawData_h
#define RawData_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"

class RawData {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   UShort_t        evt;
   UShort_t        pmt;
   std::vector<double>   *rawHGData;
   std::vector<int>     *photonHGStarts;
   std::vector<int>     *photonHGEnds;
   std::vector<int>     *podHGStarts;
   std::vector<int>     *podHGEnds;
   std::vector<double>   *rawLGData;
   std::vector<int>     *photonLGStarts;
   std::vector<int>     *photonLGEnds;
   std::vector<int>     *podLGStarts;
   std::vector<int>     *podLGEnds;

   // List of branches
   TBranch        *b_evt;   //!
   TBranch        *b_pmt;   //!
   TBranch        *b_rawHGData;   //!
   TBranch        *b_photonHGStarts;   //!
   TBranch        *b_photonHGEnds;   //!
   TBranch        *b_podHGStarts;   //!
   TBranch        *b_podHGEnds;   //!
   TBranch        *b_rawLGData;   //!
   TBranch        *b_photonLGStarts;   //!
   TBranch        *b_photonLGEnds;   //!
   TBranch        *b_podLGStarts;   //!
   TBranch        *b_podLGEnds;   //!

   RawData(TTree *tree=0);
   virtual ~RawData();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef RawData_cxx
RawData::RawData(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("lz_197001061853_000010_000000_raw.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("lz_197001061853_000010_000000_raw.root");
      }
      f->GetObject("RawData",tree);

   }
   Init(tree);
}

RawData::~RawData()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t RawData::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t RawData::LoadTree(Long64_t entry)
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

void RawData::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   rawHGData = 0;
   photonHGStarts = 0;
   photonHGEnds = 0;
   podHGStarts = 0;
   podHGEnds = 0;
   rawLGData = 0;
   photonLGStarts = 0;
   photonLGEnds = 0;
   podLGStarts = 0;
   podLGEnds = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("evt", &evt, &b_evt);
   fChain->SetBranchAddress("pmt", &pmt, &b_pmt);
   fChain->SetBranchAddress("rawHGData", &rawHGData, &b_rawHGData);
   fChain->SetBranchAddress("photonHGStarts", &photonHGStarts, &b_photonHGStarts);
   fChain->SetBranchAddress("photonHGEnds", &photonHGEnds, &b_photonHGEnds);
   fChain->SetBranchAddress("podHGStarts", &podHGStarts, &b_podHGStarts);
   fChain->SetBranchAddress("podHGEnds", &podHGEnds, &b_podHGEnds);
   fChain->SetBranchAddress("rawLGData", &rawLGData, &b_rawLGData);
   fChain->SetBranchAddress("photonLGStarts", &photonLGStarts, &b_photonLGStarts);
   fChain->SetBranchAddress("photonLGEnds", &photonLGEnds, &b_photonLGEnds);
   fChain->SetBranchAddress("podLGStarts", &podLGStarts, &b_podLGStarts);
   fChain->SetBranchAddress("podLGEnds", &podLGEnds, &b_podLGEnds);
   Notify();
}

Bool_t RawData::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void RawData::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t RawData::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef RawData_cxx
