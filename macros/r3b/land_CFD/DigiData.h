//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Feb 25 16:09:39 2010 by ROOT version 5.26/00
// from TTree cbmsim//cbmout
// found on file: land_digi.root
//////////////////////////////////////////////////////////

#ifndef DigiData_h
#define DigiData_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
   const Int_t kMaxcbmout_EvtHeader_EventHeader = 1;
   const Int_t kMaxcbmout_Digital_response_in_Land_LandDigi = 200;

class DigiData {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   FairEventHeader *EventHeader_;
   UInt_t          EventHeader_TNamed_fUniqueID;
   UInt_t          EventHeader_TNamed_fBits;
   TString         EventHeader_TNamed_fName;
   TString         EventHeader_TNamed_fTitle;
   UInt_t          EventHeader_fRunId;
   Int_t           LandDigi_;
   UInt_t          LandDigi_fUniqueID[kMaxcbmout_Digital_response_in_Land_LandDigi];   //[cbmout.Digital_response_in_Land.LandDigi_]
   UInt_t          LandDigi_fBits[kMaxcbmout_Digital_response_in_Land_LandDigi];   //[cbmout.Digital_response_in_Land.LandDigi_]
   Int_t           LandDigi_fPaddleNb[kMaxcbmout_Digital_response_in_Land_LandDigi];   //[cbmout.Digital_response_in_Land.LandDigi_]
   Int_t           LandDigi_fEventNb[kMaxcbmout_Digital_response_in_Land_LandDigi];   //[cbmout.Digital_response_in_Land.LandDigi_]
   Double32_t      LandDigi_fTdcL[kMaxcbmout_Digital_response_in_Land_LandDigi];   //[cbmout.Digital_response_in_Land.LandDigi_]
   Double32_t      LandDigi_fTdcR[kMaxcbmout_Digital_response_in_Land_LandDigi];   //[cbmout.Digital_response_in_Land.LandDigi_]
   Double32_t      LandDigi_fQdcL[kMaxcbmout_Digital_response_in_Land_LandDigi];   //[cbmout.Digital_response_in_Land.LandDigi_]
   Double32_t      LandDigi_fQdcR[kMaxcbmout_Digital_response_in_Land_LandDigi];   //[cbmout.Digital_response_in_Land.LandDigi_]

   // List of branches
   TBranch        *b_cbmout_EvtHeader_EventHeader_;   //!
   TBranch        *b_EventHeader_cbmout_EvtHeader_EventHeader_TNamed_fUniqueID;   //!
   TBranch        *b_EventHeader_cbmout_EvtHeader_EventHeader_TNamed_fBits;   //!
   TBranch        *b_EventHeader_cbmout_EvtHeader_EventHeader_TNamed_fName;   //!
   TBranch        *b_EventHeader_cbmout_EvtHeader_EventHeader_TNamed_fTitle;   //!
   TBranch        *b_EventHeader_cbmout_EvtHeader_EventHeader_fRunId;   //!
   TBranch        *b_cbmout_Digital_response_in_Land_LandDigi_;   //!
   TBranch        *b_LandDigi_fUniqueID;   //!
   TBranch        *b_LandDigi_fBits;   //!
   TBranch        *b_LandDigi_fPaddleNb;   //!
   TBranch        *b_LandDigi_fEventNb;   //!
   TBranch        *b_LandDigi_fTdcL;   //!
   TBranch        *b_LandDigi_fTdcR;   //!
   TBranch        *b_LandDigi_fQdcL;   //!
   TBranch        *b_LandDigi_fQdcR;   //!

   DigiData(TTree *tree=0);
   virtual ~DigiData();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef DigiData_cxx
DigiData::DigiData(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("land_digi.root");
      if (!f) {
         f = new TFile("land_digi.root");
      }
      tree = (TTree*)gDirectory->Get("cbmsim");

   }
   Init(tree);
}

DigiData::~DigiData()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t DigiData::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t DigiData::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void DigiData::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   EventHeader_ = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("EventHeader.", &EventHeader_, &b_cbmout_EvtHeader_EventHeader_);
   fChain->SetBranchAddress("EventHeader.TNamed.fUniqueID", &EventHeader_TNamed_fUniqueID, &b_EventHeader_cbmout_EvtHeader_EventHeader_TNamed_fUniqueID);
   fChain->SetBranchAddress("EventHeader.TNamed.fBits", &EventHeader_TNamed_fBits, &b_EventHeader_cbmout_EvtHeader_EventHeader_TNamed_fBits);
   fChain->SetBranchAddress("EventHeader.TNamed.fName", &EventHeader_TNamed_fName, &b_EventHeader_cbmout_EvtHeader_EventHeader_TNamed_fName);
   fChain->SetBranchAddress("EventHeader.TNamed.fTitle", &EventHeader_TNamed_fTitle, &b_EventHeader_cbmout_EvtHeader_EventHeader_TNamed_fTitle);
   fChain->SetBranchAddress("EventHeader.fRunId", &EventHeader_fRunId, &b_EventHeader_cbmout_EvtHeader_EventHeader_fRunId);
   fChain->SetBranchAddress("LandDigi", &LandDigi_, &b_cbmout_Digital_response_in_Land_LandDigi_);
   fChain->SetBranchAddress("LandDigi.fUniqueID", LandDigi_fUniqueID, &b_LandDigi_fUniqueID);
   fChain->SetBranchAddress("LandDigi.fBits", LandDigi_fBits, &b_LandDigi_fBits);
   fChain->SetBranchAddress("LandDigi.fPaddleNb", LandDigi_fPaddleNb, &b_LandDigi_fPaddleNb);
   fChain->SetBranchAddress("LandDigi.fEventNb", LandDigi_fEventNb, &b_LandDigi_fEventNb);
   fChain->SetBranchAddress("LandDigi.fTdcL", LandDigi_fTdcL, &b_LandDigi_fTdcL);
   fChain->SetBranchAddress("LandDigi.fTdcR", LandDigi_fTdcR, &b_LandDigi_fTdcR);
   fChain->SetBranchAddress("LandDigi.fQdcL", LandDigi_fQdcL, &b_LandDigi_fQdcL);
   fChain->SetBranchAddress("LandDigi.fQdcR", LandDigi_fQdcR, &b_LandDigi_fQdcR);
   Notify();
}

Bool_t DigiData::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void DigiData::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t DigiData::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef DigiData_cxx
