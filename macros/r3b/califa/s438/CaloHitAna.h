//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Nov 26 16:42:51 2014 by ROOT version 5.34/22
// from TTree cbmsim//cbmout
// found on file: root/hit/run291-293.root
//////////////////////////////////////////////////////////

#ifndef CaloHitAna_h
#define CaloHitAna_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <TObject.h>
#include <TNamed.h>
#include <TClonesArray.h>

// Fixed size dimensions of array or collections stored in the TTree if any.
   const Int_t kMaxcbmout_EvtHeader_EventHeader = 1;
   const Int_t kMaxcbmout_CALIFA_Hit_CaloHit = 30;

class CaloHitAna {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
//   FairEventHeader *EventHeader_;
   UInt_t          EventHeader_TNamed_fUniqueID;
   UInt_t          EventHeader_TNamed_fBits;
   TString         EventHeader_TNamed_fName;
   TString         EventHeader_TNamed_fTitle;
   UInt_t          EventHeader_fRunId;
   Double_t        EventHeader_fEventTime;
   Int_t           EventHeader_fInputFileId;
   Int_t           EventHeader_fMCEntryNo;
   Int_t           CaloHit_;
   UInt_t          CaloHit_fUniqueID[kMaxcbmout_CALIFA_Hit_CaloHit];   //[cbmout.CALIFA Hit.CaloHit_]
   UInt_t          CaloHit_fBits[kMaxcbmout_CALIFA_Hit_CaloHit];   //[cbmout.CALIFA Hit.CaloHit_]
 //set<FairLink>   CaloHit_fLinks[kMaxcbmout_CALIFA_Hit_CaloHit];
   Bool_t          CaloHit_fPersistanceCheck[kMaxcbmout_CALIFA_Hit_CaloHit];   //[cbmout.CALIFA Hit.CaloHit_]
   Int_t           CaloHit_fVerbose[kMaxcbmout_CALIFA_Hit_CaloHit];   //[cbmout.CALIFA Hit.CaloHit_]
   Int_t           CaloHit_fDefaultType[kMaxcbmout_CALIFA_Hit_CaloHit];   //[cbmout.CALIFA Hit.CaloHit_]
   UInt_t          CaloHit_fNbOfCrystalHits[kMaxcbmout_CALIFA_Hit_CaloHit];   //[cbmout.CALIFA Hit.CaloHit_]
   Double_t        CaloHit_fEnergy[kMaxcbmout_CALIFA_Hit_CaloHit];   //[cbmout.CALIFA Hit.CaloHit_]
   Double_t        CaloHit_fNf[kMaxcbmout_CALIFA_Hit_CaloHit];   //[cbmout.CALIFA Hit.CaloHit_]
   Double_t        CaloHit_fNs[kMaxcbmout_CALIFA_Hit_CaloHit];   //[cbmout.CALIFA Hit.CaloHit_]
   Double_t        CaloHit_fTheta[kMaxcbmout_CALIFA_Hit_CaloHit];   //[cbmout.CALIFA Hit.CaloHit_]
   Double_t        CaloHit_fPhi[kMaxcbmout_CALIFA_Hit_CaloHit];   //[cbmout.CALIFA Hit.CaloHit_]

   // List of branches
//   TBranch        *b_cbmout_EvtHeader_EventHeader_;   //!
   TBranch        *b_EventHeader_cbmout_EvtHeader_EventHeader_TNamed_fUniqueID;   //!
   TBranch        *b_EventHeader_cbmout_EvtHeader_EventHeader_TNamed_fBits;   //!
   TBranch        *b_EventHeader_cbmout_EvtHeader_EventHeader_TNamed_fName;   //!
   TBranch        *b_EventHeader_cbmout_EvtHeader_EventHeader_TNamed_fTitle;   //!
   TBranch        *b_EventHeader_cbmout_EvtHeader_EventHeader_fRunId;   //!
   TBranch        *b_EventHeader_cbmout_EvtHeader_EventHeader_fEventTime;   //!
   TBranch        *b_EventHeader_cbmout_EvtHeader_EventHeader_fInputFileId;   //!
   TBranch        *b_EventHeader_cbmout_EvtHeader_EventHeader_fMCEntryNo;   //!
   TBranch        *b_cbmout_CALIFA_Hit_CaloHit_;   //!
   TBranch        *b_CaloHit_fUniqueID;   //!
   TBranch        *b_CaloHit_fBits;   //!
   TBranch        *b_CaloHit_fPersistanceCheck;   //!
   TBranch        *b_CaloHit_fVerbose;   //!
   TBranch        *b_CaloHit_fDefaultType;   //!
   TBranch        *b_CaloHit_fNbOfCrystalHits;   //!
   TBranch        *b_CaloHit_fEnergy;   //!
   TBranch        *b_CaloHit_fNf;   //!
   TBranch        *b_CaloHit_fNs;   //!
   TBranch        *b_CaloHit_fTheta;   //!
   TBranch        *b_CaloHit_fPhi;   //!

   CaloHitAna(TTree *tree=0);
   virtual ~CaloHitAna();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual void	    DrawPID();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef CaloHitAna_cxx
CaloHitAna::CaloHitAna(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("root/hit/run291-293.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("root/hit/run291-293.root");
      }
      f->GetObject("cbmsim",tree);

   }
   Init(tree);
}

CaloHitAna::~CaloHitAna()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t CaloHitAna::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t CaloHitAna::LoadTree(Long64_t entry)
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

void CaloHitAna::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
//   EventHeader_ = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

//   fChain->SetBranchAddress("EventHeader.", &EventHeader_, &b_cbmout_EvtHeader_EventHeader_);
   fChain->SetBranchAddress("EventHeader.TNamed.fUniqueID", &EventHeader_TNamed_fUniqueID, &b_EventHeader_cbmout_EvtHeader_EventHeader_TNamed_fUniqueID);
   fChain->SetBranchAddress("EventHeader.TNamed.fBits", &EventHeader_TNamed_fBits, &b_EventHeader_cbmout_EvtHeader_EventHeader_TNamed_fBits);
   fChain->SetBranchAddress("EventHeader.TNamed.fName", &EventHeader_TNamed_fName, &b_EventHeader_cbmout_EvtHeader_EventHeader_TNamed_fName);
   fChain->SetBranchAddress("EventHeader.TNamed.fTitle", &EventHeader_TNamed_fTitle, &b_EventHeader_cbmout_EvtHeader_EventHeader_TNamed_fTitle);
   fChain->SetBranchAddress("EventHeader.fRunId", &EventHeader_fRunId, &b_EventHeader_cbmout_EvtHeader_EventHeader_fRunId);
   fChain->SetBranchAddress("EventHeader.fEventTime", &EventHeader_fEventTime, &b_EventHeader_cbmout_EvtHeader_EventHeader_fEventTime);
   fChain->SetBranchAddress("EventHeader.fInputFileId", &EventHeader_fInputFileId, &b_EventHeader_cbmout_EvtHeader_EventHeader_fInputFileId);
   fChain->SetBranchAddress("EventHeader.fMCEntryNo", &EventHeader_fMCEntryNo, &b_EventHeader_cbmout_EvtHeader_EventHeader_fMCEntryNo);
   fChain->SetBranchAddress("CaloHit", &CaloHit_, &b_cbmout_CALIFA_Hit_CaloHit_);
   fChain->SetBranchAddress("CaloHit.fUniqueID", CaloHit_fUniqueID, &b_CaloHit_fUniqueID);
   fChain->SetBranchAddress("CaloHit.fBits", CaloHit_fBits, &b_CaloHit_fBits);
   fChain->SetBranchAddress("CaloHit.fPersistanceCheck", CaloHit_fPersistanceCheck, &b_CaloHit_fPersistanceCheck);
   fChain->SetBranchAddress("CaloHit.fVerbose", CaloHit_fVerbose, &b_CaloHit_fVerbose);
   fChain->SetBranchAddress("CaloHit.fDefaultType", CaloHit_fDefaultType, &b_CaloHit_fDefaultType);
   fChain->SetBranchAddress("CaloHit.fNbOfCrystalHits", CaloHit_fNbOfCrystalHits, &b_CaloHit_fNbOfCrystalHits);
   fChain->SetBranchAddress("CaloHit.fEnergy", CaloHit_fEnergy, &b_CaloHit_fEnergy);
   fChain->SetBranchAddress("CaloHit.fNf", CaloHit_fNf, &b_CaloHit_fNf);
   fChain->SetBranchAddress("CaloHit.fNs", CaloHit_fNs, &b_CaloHit_fNs);
   fChain->SetBranchAddress("CaloHit.fTheta", CaloHit_fTheta, &b_CaloHit_fTheta);
   fChain->SetBranchAddress("CaloHit.fPhi", CaloHit_fPhi, &b_CaloHit_fPhi);
   Notify();
}

Bool_t CaloHitAna::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void CaloHitAna::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t CaloHitAna::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef CaloHitAna_cxx
