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

   UInt_t        Pspx03n, Pspx01n, Pspx02n, Pspx04n, Pspx05n;
   UInt_t        Pspx03ni[128];
   UInt_t        Pspx03e01[128];

   Float_t      Pspx04e, Pspx05e;
   Float_t      Pspx04u, Pspx05u;
   Float_t      Pspx01u, Pspx02u;

   // List of branches
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

   TBranch        *b_Pspx04e, *b_Pspx05e;
   TBranch        *b_Pspx04u, *b_Pspx05u;
   TBranch        *b_Pspx01u, *b_Pspx02u;

   TBranch        *b_Pspx03n, *b_Pspx03ni, *b_Pspx03e01;
   TBranch        *b_Pspx01n, *b_Pspx02n, *b_Pspx04n, *b_Pspx05n;

   CaloHitAna(TTree *tree=0);
   virtual ~CaloHitAna();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(const char *fout = NULL, const char *mode = "update");
   virtual void	    DrawPID(bool cut = false);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

//   void TestPSPX();
   void SaveHists(const char *fout, const char *mode, TH1 **hists);
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

   fChain->SetBranchAddress("califa_CaloHit", &CaloHit_, &b_cbmout_CALIFA_Hit_CaloHit_);
   fChain->SetBranchAddress("califa_CaloHit.fUniqueID", CaloHit_fUniqueID, &b_CaloHit_fUniqueID);
   fChain->SetBranchAddress("califa_CaloHit.fBits", CaloHit_fBits, &b_CaloHit_fBits);
   fChain->SetBranchAddress("califa_CaloHit.fDefaultType", CaloHit_fDefaultType, &b_CaloHit_fDefaultType);
   fChain->SetBranchAddress("califa_CaloHit.fNbOfCrystalHits", CaloHit_fNbOfCrystalHits, &b_CaloHit_fNbOfCrystalHits);
   fChain->SetBranchAddress("califa_CaloHit.fEnergy", CaloHit_fEnergy, &b_CaloHit_fEnergy);
   fChain->SetBranchAddress("califa_CaloHit.fNf", CaloHit_fNf, &b_CaloHit_fNf);
   fChain->SetBranchAddress("califa_CaloHit.fNs", CaloHit_fNs, &b_CaloHit_fNs);
   fChain->SetBranchAddress("califa_CaloHit.fTheta", CaloHit_fTheta, &b_CaloHit_fTheta);
   fChain->SetBranchAddress("califa_CaloHit.fPhi", CaloHit_fPhi, &b_CaloHit_fPhi);

   fChain->SetBranchAddress("main_Pspx04e", &Pspx04e, &b_Pspx04e);
   fChain->SetBranchAddress("main_Pspx05e", &Pspx05e, &b_Pspx05e);
   fChain->SetBranchAddress("main_Pspx04u", &Pspx04u, &b_Pspx04u);
   fChain->SetBranchAddress("main_Pspx05u", &Pspx05u, &b_Pspx05u);
   fChain->SetBranchAddress("main_Pspx01u", &Pspx01u, &b_Pspx01u);
   fChain->SetBranchAddress("main_Pspx02u", &Pspx02u, &b_Pspx02u);

   fChain->SetBranchAddress("main_Pspx01n", &Pspx01n, &b_Pspx01n);
   fChain->SetBranchAddress("main_Pspx02n", &Pspx02n, &b_Pspx02n);
   fChain->SetBranchAddress("main_Pspx03n", &Pspx03n, &b_Pspx03n);
   fChain->SetBranchAddress("main_Pspx04n", &Pspx04n, &b_Pspx04n);
   fChain->SetBranchAddress("main_Pspx05n", &Pspx05n, &b_Pspx05n);
   fChain->SetBranchAddress("main_Pspx03ni", &Pspx03ni, &b_Pspx03ni);
   fChain->SetBranchAddress("main_Pspx03e01", &Pspx03e01, &b_Pspx03e01);

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
