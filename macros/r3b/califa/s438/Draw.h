//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Nov 25 11:50:45 2014 by ROOT version 5.34/22
// from TTree cbmsim//cbmout
// found on file: root/cal/run291-293.root
//////////////////////////////////////////////////////////

#ifndef Draw_h
#define Draw_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>

// Header file for the classes stored in the TTree if any.
#include <TObject.h>
#include <TNamed.h>
#include <TClonesArray.h>

// Fixed size dimensions of array or collections stored in the TTree if any.
   const Int_t kMaxcbmout_EvtHeader_EventHeader = 1;
   const Int_t kMaxcbmout_CaloCalibrated_CaloCrystalHit = 80;

class Draw {
protected:
	TH1	*getHist(TH1 *h = NULL);
	TH2	*getHist(TH2 *h = NULL);

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
   Int_t           CaloCrystalHit_;
   UInt_t          CaloCrystalHit_fUniqueID[kMaxcbmout_CaloCalibrated_CaloCrystalHit];   //[cbmout.CaloCalibrated.CaloCrystalHit_]
   UInt_t          CaloCrystalHit_fBits[kMaxcbmout_CaloCalibrated_CaloCrystalHit];   //[cbmout.CaloCalibrated.CaloCrystalHit_]
 //set<FairLink>   CaloCrystalHit_fLinks[kMaxcbmout_CaloCalibrated_CaloCrystalHit];
   Bool_t          CaloCrystalHit_fPersistanceCheck[kMaxcbmout_CaloCalibrated_CaloCrystalHit];   //[cbmout.CaloCalibrated.CaloCrystalHit_]
   Int_t           CaloCrystalHit_fVerbose[kMaxcbmout_CaloCalibrated_CaloCrystalHit];   //[cbmout.CaloCalibrated.CaloCrystalHit_]
   Int_t           CaloCrystalHit_fDefaultType[kMaxcbmout_CaloCalibrated_CaloCrystalHit];   //[cbmout.CaloCalibrated.CaloCrystalHit_]
   Double32_t      CaloCrystalHit_fEnergy[kMaxcbmout_CaloCalibrated_CaloCrystalHit];   //[cbmout.CaloCalibrated.CaloCrystalHit_]
   Double32_t      CaloCrystalHit_fNf[kMaxcbmout_CaloCalibrated_CaloCrystalHit];   //[cbmout.CaloCalibrated.CaloCrystalHit_]
   Double32_t      CaloCrystalHit_fNs[kMaxcbmout_CaloCalibrated_CaloCrystalHit];   //[cbmout.CaloCalibrated.CaloCrystalHit_]
   Double32_t      CaloCrystalHit_fToT_Energy[kMaxcbmout_CaloCalibrated_CaloCrystalHit];   //[cbmout.CaloCalibrated.CaloCrystalHit_]
   ULong64_t      CaloCrystalHit_fTime[kMaxcbmout_CaloCalibrated_CaloCrystalHit];   //[cbmout.CaloCalibrated.CaloCrystalHit_]
   Int_t           CaloCrystalHit_fCrystalId[kMaxcbmout_CaloCalibrated_CaloCrystalHit];   //[cbmout.CaloCalibrated.CaloCrystalHit_]

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
   TBranch        *b_cbmout_CaloCalibrated_CaloCrystalHit_;   //!
   TBranch        *b_CaloCrystalHit_fUniqueID;   //!
   TBranch        *b_CaloCrystalHit_fBits;   //!
   TBranch        *b_CaloCrystalHit_fPersistanceCheck;   //!
   TBranch        *b_CaloCrystalHit_fVerbose;   //!
   TBranch        *b_CaloCrystalHit_fDefaultType;   //!
   TBranch        *b_CaloCrystalHit_fEnergy;   //!
   TBranch        *b_CaloCrystalHit_fNf;   //!
   TBranch        *b_CaloCrystalHit_fNs;   //!
   TBranch        *b_CaloCrystalHit_fToT_Energy;   //!
   TBranch        *b_CaloCrystalHit_fTime;   //!
   TBranch        *b_CaloCrystalHit_fCrystalId;   //!

   Draw(TTree *tree=0);
   virtual ~Draw();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Energy(UInt_t CrystalId, TH1 *hist = NULL);
   virtual void     Pid(UInt_t CrystalId, TH2 *hist = NULL);
   virtual void     GenAll(Int_t n_channels = 128, bool drawEnergy = false, bool drawPid = true);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef Draw_cxx
Draw::Draw(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("root/cal/run291-293.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("root/cal/run291-293.root");
      }
      f->GetObject("cbmsim",tree);

   }
   Init(tree);
}

Draw::~Draw()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Draw::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Draw::LoadTree(Long64_t entry)
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

void Draw::Init(TTree *tree)
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
   fChain->SetBranchAddress("CaloCrystalHit", &CaloCrystalHit_, &b_cbmout_CaloCalibrated_CaloCrystalHit_);
   fChain->SetBranchAddress("CaloCrystalHit.fUniqueID", CaloCrystalHit_fUniqueID, &b_CaloCrystalHit_fUniqueID);
   fChain->SetBranchAddress("CaloCrystalHit.fBits", CaloCrystalHit_fBits, &b_CaloCrystalHit_fBits);
   fChain->SetBranchAddress("CaloCrystalHit.fPersistanceCheck", CaloCrystalHit_fPersistanceCheck, &b_CaloCrystalHit_fPersistanceCheck);
   fChain->SetBranchAddress("CaloCrystalHit.fVerbose", CaloCrystalHit_fVerbose, &b_CaloCrystalHit_fVerbose);
   fChain->SetBranchAddress("CaloCrystalHit.fDefaultType", CaloCrystalHit_fDefaultType, &b_CaloCrystalHit_fDefaultType);
   fChain->SetBranchAddress("CaloCrystalHit.fEnergy", CaloCrystalHit_fEnergy, &b_CaloCrystalHit_fEnergy);
   fChain->SetBranchAddress("CaloCrystalHit.fNf", CaloCrystalHit_fNf, &b_CaloCrystalHit_fNf);
   fChain->SetBranchAddress("CaloCrystalHit.fNs", CaloCrystalHit_fNs, &b_CaloCrystalHit_fNs);
   fChain->SetBranchAddress("CaloCrystalHit.fToT_Energy", CaloCrystalHit_fToT_Energy, &b_CaloCrystalHit_fToT_Energy);
   fChain->SetBranchAddress("CaloCrystalHit.fTime", CaloCrystalHit_fTime, &b_CaloCrystalHit_fTime);
   fChain->SetBranchAddress("CaloCrystalHit.fCrystalId", CaloCrystalHit_fCrystalId, &b_CaloCrystalHit_fCrystalId);
   Notify();
}

Bool_t Draw::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Draw::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Draw::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Draw_cxx
