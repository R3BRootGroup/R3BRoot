//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Mar  2 10:45:09 2010 by ROOT version 5.26/00
// from TTree cbmsim//cbmroot
// found on file: r3bsim.root
//////////////////////////////////////////////////////////

#ifndef SimData_h
#define SimData_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
/*   const Int_t kMaxcbmroot_Stack_MCTrack = 1269;
   const Int_t kMaxcbmroot_Land_LandPoint = 1216;
   const Int_t kMaxcbmroot_EvtHeader_EventHeader = 1;
   const Int_t kMaxcbmroot_Event_MCEventHeader = 1;
   const Int_t kMaxcbmroot_MCGeoTrack_GeoTracks = 1268;
*/
   const Int_t kMaxcbmroot_Stack_MCTrack = 10000;
   const Int_t kMaxcbmroot_Land_LandPoint = 10000;
   const Int_t kMaxcbmroot_EvtHeader_EventHeader = 1;
   const Int_t kMaxcbmroot_Event_MCEventHeader = 1;
   const Int_t kMaxcbmroot_MCGeoTrack_GeoTracks = 10000;

class SimData {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           MCTrack_;
   UInt_t          MCTrack_fUniqueID[kMaxcbmroot_Stack_MCTrack];   //[cbmroot.Stack.MCTrack_]
   UInt_t          MCTrack_fBits[kMaxcbmroot_Stack_MCTrack];   //[cbmroot.Stack.MCTrack_]
   Int_t           MCTrack_fPdgCode[kMaxcbmroot_Stack_MCTrack];   //[cbmroot.Stack.MCTrack_]
   Int_t           MCTrack_fMotherId[kMaxcbmroot_Stack_MCTrack];   //[cbmroot.Stack.MCTrack_]
   Double32_t      MCTrack_fPx[kMaxcbmroot_Stack_MCTrack];   //[cbmroot.Stack.MCTrack_]
   Double32_t      MCTrack_fPy[kMaxcbmroot_Stack_MCTrack];   //[cbmroot.Stack.MCTrack_]
   Double32_t      MCTrack_fPz[kMaxcbmroot_Stack_MCTrack];   //[cbmroot.Stack.MCTrack_]
   Double32_t      MCTrack_fStartX[kMaxcbmroot_Stack_MCTrack];   //[cbmroot.Stack.MCTrack_]
   Double32_t      MCTrack_fStartY[kMaxcbmroot_Stack_MCTrack];   //[cbmroot.Stack.MCTrack_]
   Double32_t      MCTrack_fStartZ[kMaxcbmroot_Stack_MCTrack];   //[cbmroot.Stack.MCTrack_]
   Double32_t      MCTrack_fStartT[kMaxcbmroot_Stack_MCTrack];   //[cbmroot.Stack.MCTrack_]
   Int_t           MCTrack_fNPoints[kMaxcbmroot_Stack_MCTrack];   //[cbmroot.Stack.MCTrack_]
   Double32_t      MCTrack_fMass[kMaxcbmroot_Stack_MCTrack];   //[cbmroot.Stack.MCTrack_]

   Int_t           LandPoint_;
   UInt_t          LandPoint_fUniqueID[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   UInt_t          LandPoint_fBits[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Int_t           LandPoint_fTrackID[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Int_t           LandPoint_fDetectorID[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Double32_t      LandPoint_fX[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Double32_t      LandPoint_fY[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Double32_t      LandPoint_fZ[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Double32_t      LandPoint_fPx[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Double32_t      LandPoint_fPy[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Double32_t      LandPoint_fPz[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Double32_t      LandPoint_fTime[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Double32_t      LandPoint_fLength[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Double32_t      LandPoint_fELoss[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Double32_t      LandPoint_fX_out[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Double32_t      LandPoint_fY_out[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Double32_t      LandPoint_fZ_out[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Double32_t      LandPoint_fPx_out[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Double32_t      LandPoint_fPy_out[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Double32_t      LandPoint_fPz_out[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Int_t           LandPoint_fSector[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Int_t           LandPoint_fPaddleNb[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Int_t           LandPoint_fPaddleTyp[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]
   Double_t        LandPoint_fLightYield[kMaxcbmroot_Land_LandPoint];   //[cbmroot.Land.LandPoint_]

   FairEventHeader *EventHeader_;
   UInt_t          EventHeader_TNamed_fUniqueID;
   UInt_t          EventHeader_TNamed_fBits;
   TString         EventHeader_TNamed_fName;
   TString         EventHeader_TNamed_fTitle;
   UInt_t          EventHeader_fRunId;

   FairMCEventHeader *MCEventHeader_;
   UInt_t          MCEventHeader_TNamed_fUniqueID;
   UInt_t          MCEventHeader_TNamed_fBits;
   TString         MCEventHeader_TNamed_fName;
   TString         MCEventHeader_TNamed_fTitle;
   UInt_t          MCEventHeader_fRunId;
   UInt_t          MCEventHeader_fEventId;
   Double32_t      MCEventHeader_fX;
   Double32_t      MCEventHeader_fY;
   Double32_t      MCEventHeader_fZ;
   Double32_t      MCEventHeader_fT;
   Double32_t      MCEventHeader_fB;
   Int_t           MCEventHeader_fNPrim;
   Bool_t          MCEventHeader_fIsSet;

   Int_t           GeoTracks_;
   UInt_t          GeoTracks_fUniqueID[kMaxcbmroot_MCGeoTrack_GeoTracks];   //[cbmroot.MCGeoTrack.GeoTracks_]
   UInt_t          GeoTracks_fBits[kMaxcbmroot_MCGeoTrack_GeoTracks];   //[cbmroot.MCGeoTrack.GeoTracks_]
   UInt_t          GeoTracks_fGeoAtt[kMaxcbmroot_MCGeoTrack_GeoTracks];   //[cbmroot.MCGeoTrack.GeoTracks_]
   Short_t         GeoTracks_fLineColor[kMaxcbmroot_MCGeoTrack_GeoTracks];   //[cbmroot.MCGeoTrack.GeoTracks_]
   Short_t         GeoTracks_fLineStyle[kMaxcbmroot_MCGeoTrack_GeoTracks];   //[cbmroot.MCGeoTrack.GeoTracks_]
   Short_t         GeoTracks_fLineWidth[kMaxcbmroot_MCGeoTrack_GeoTracks];   //[cbmroot.MCGeoTrack.GeoTracks_]
   Short_t         GeoTracks_fMarkerColor[kMaxcbmroot_MCGeoTrack_GeoTracks];   //[cbmroot.MCGeoTrack.GeoTracks_]
   Short_t         GeoTracks_fMarkerStyle[kMaxcbmroot_MCGeoTrack_GeoTracks];   //[cbmroot.MCGeoTrack.GeoTracks_]
   Float_t         GeoTracks_fMarkerSize[kMaxcbmroot_MCGeoTrack_GeoTracks];   //[cbmroot.MCGeoTrack.GeoTracks_]
   Int_t           GeoTracks_fPDG[kMaxcbmroot_MCGeoTrack_GeoTracks];   //[cbmroot.MCGeoTrack.GeoTracks_]
   Int_t           GeoTracks_fId[kMaxcbmroot_MCGeoTrack_GeoTracks];   //[cbmroot.MCGeoTrack.GeoTracks_]
   Int_t           GeoTracks_fPointsSize[kMaxcbmroot_MCGeoTrack_GeoTracks];   //[cbmroot.MCGeoTrack.GeoTracks_]
   Int_t           GeoTracks_fNpoints[kMaxcbmroot_MCGeoTrack_GeoTracks];   //[cbmroot.MCGeoTrack.GeoTracks_]
   Double_t       *GeoTracks_fPoints[kMaxcbmroot_MCGeoTrack_GeoTracks];   //[cbmroot_MCGeoTrack_GeoTracks_fNpoints]

   // List of branches
   TBranch        *b_cbmroot_Stack_MCTrack_;   //!
   TBranch        *b_MCTrack_fUniqueID;   //!
   TBranch        *b_MCTrack_fBits;   //!
   TBranch        *b_MCTrack_fPdgCode;   //!
   TBranch        *b_MCTrack_fMotherId;   //!
   TBranch        *b_MCTrack_fPx;   //!
   TBranch        *b_MCTrack_fPy;   //!
   TBranch        *b_MCTrack_fPz;   //!
   TBranch        *b_MCTrack_fStartX;   //!
   TBranch        *b_MCTrack_fStartY;   //!
   TBranch        *b_MCTrack_fStartZ;   //!
   TBranch        *b_MCTrack_fStartT;   //!
   TBranch        *b_MCTrack_fNPoints;   //!
   TBranch        *b_MCTrack_fMass;   //!
   TBranch        *b_cbmroot_Land_LandPoint_;   //!
   TBranch        *b_LandPoint_fUniqueID;   //!
   TBranch        *b_LandPoint_fBits;   //!
   TBranch        *b_LandPoint_fTrackID;   //!
   TBranch        *b_LandPoint_fDetectorID;   //!
   TBranch        *b_LandPoint_fX;   //!
   TBranch        *b_LandPoint_fY;   //!
   TBranch        *b_LandPoint_fZ;   //!
   TBranch        *b_LandPoint_fPx;   //!
   TBranch        *b_LandPoint_fPy;   //!
   TBranch        *b_LandPoint_fPz;   //!
   TBranch        *b_LandPoint_fTime;   //!
   TBranch        *b_LandPoint_fLength;   //!
   TBranch        *b_LandPoint_fELoss;   //!
   TBranch        *b_LandPoint_fX_out;   //!
   TBranch        *b_LandPoint_fY_out;   //!
   TBranch        *b_LandPoint_fZ_out;   //!
   TBranch        *b_LandPoint_fPx_out;   //!
   TBranch        *b_LandPoint_fPy_out;   //!
   TBranch        *b_LandPoint_fPz_out;   //!
   TBranch        *b_LandPoint_fSector;   //!
   TBranch        *b_LandPoint_fPaddleNb;   //!
   TBranch        *b_LandPoint_fPaddleTyp;   //!
   TBranch        *b_LandPoint_fLightYield;   //!
   TBranch        *b_cbmroot_EvtHeader_EventHeader_;   //!
   TBranch        *b_EventHeader_cbmroot_EvtHeader_EventHeader_TNamed_fUniqueID;   //!
   TBranch        *b_EventHeader_cbmroot_EvtHeader_EventHeader_TNamed_fBits;   //!
   TBranch        *b_EventHeader_cbmroot_EvtHeader_EventHeader_TNamed_fName;   //!
   TBranch        *b_EventHeader_cbmroot_EvtHeader_EventHeader_TNamed_fTitle;   //!
   TBranch        *b_EventHeader_cbmroot_EvtHeader_EventHeader_fRunId;   //!
   TBranch        *b_cbmroot_Event_MCEventHeader_;   //!
   TBranch        *b_MCEventHeader_cbmroot_Event_MCEventHeader_TNamed_fUniqueID;   //!
   TBranch        *b_MCEventHeader_cbmroot_Event_MCEventHeader_TNamed_fBits;   //!
   TBranch        *b_MCEventHeader_cbmroot_Event_MCEventHeader_TNamed_fName;   //!
   TBranch        *b_MCEventHeader_cbmroot_Event_MCEventHeader_TNamed_fTitle;   //!
   TBranch        *b_MCEventHeader_cbmroot_Event_MCEventHeader_fRunId;   //!
   TBranch        *b_MCEventHeader_cbmroot_Event_MCEventHeader_fEventId;   //!
   TBranch        *b_MCEventHeader_cbmroot_Event_MCEventHeader_fX;   //!
   TBranch        *b_MCEventHeader_cbmroot_Event_MCEventHeader_fY;   //!
   TBranch        *b_MCEventHeader_cbmroot_Event_MCEventHeader_fZ;   //!
   TBranch        *b_MCEventHeader_cbmroot_Event_MCEventHeader_fT;   //!
   TBranch        *b_MCEventHeader_cbmroot_Event_MCEventHeader_fB;   //!
   TBranch        *b_MCEventHeader_cbmroot_Event_MCEventHeader_fNPrim;   //!
   TBranch        *b_MCEventHeader_cbmroot_Event_MCEventHeader_fIsSet;   //!
   TBranch        *b_cbmroot_MCGeoTrack_GeoTracks_;   //!
   TBranch        *b_GeoTracks_fUniqueID;   //!
   TBranch        *b_GeoTracks_fBits;   //!
   TBranch        *b_GeoTracks_fGeoAtt;   //!
   TBranch        *b_GeoTracks_fLineColor;   //!
   TBranch        *b_GeoTracks_fLineStyle;   //!
   TBranch        *b_GeoTracks_fLineWidth;   //!
   TBranch        *b_GeoTracks_fMarkerColor;   //!
   TBranch        *b_GeoTracks_fMarkerStyle;   //!
   TBranch        *b_GeoTracks_fMarkerSize;   //!
   TBranch        *b_GeoTracks_fPDG;   //!
   TBranch        *b_GeoTracks_fId;   //!
   TBranch        *b_GeoTracks_fPointsSize;   //!
   TBranch        *b_GeoTracks_fNpoints;   //!
   TBranch        *b_GeoTracks_fPoints;   //!

   SimData(TTree *tree=0);
   virtual ~SimData();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
	 virtual double 	MCTrack_GetEnergy(Int_t instance);
};

#endif

#ifdef SimData_cxx
SimData::SimData(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("r3bsim.root");
      if (!f) {
         f = new TFile("r3bsim.root");
      }
      tree = (TTree*)gDirectory->Get("cbmsim");

   }
   Init(tree);
}

SimData::~SimData()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t SimData::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t SimData::LoadTree(Long64_t entry)
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

void SimData::Init(TTree *tree)
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
   MCEventHeader_ = 0;
   // Set array pointer
   for(int i=0; i<kMaxcbmroot_MCGeoTrack_GeoTracks; ++i) GeoTracks_fPoints[i] = 0;

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("MCTrack", &MCTrack_, &b_cbmroot_Stack_MCTrack_);
   fChain->SetBranchAddress("MCTrack.fUniqueID", MCTrack_fUniqueID, &b_MCTrack_fUniqueID);
   fChain->SetBranchAddress("MCTrack.fBits", MCTrack_fBits, &b_MCTrack_fBits);
   fChain->SetBranchAddress("MCTrack.fPdgCode", MCTrack_fPdgCode, &b_MCTrack_fPdgCode);
   fChain->SetBranchAddress("MCTrack.fMotherId", MCTrack_fMotherId, &b_MCTrack_fMotherId);
   fChain->SetBranchAddress("MCTrack.fPx", MCTrack_fPx, &b_MCTrack_fPx);
   fChain->SetBranchAddress("MCTrack.fPy", MCTrack_fPy, &b_MCTrack_fPy);
   fChain->SetBranchAddress("MCTrack.fPz", MCTrack_fPz, &b_MCTrack_fPz);
   fChain->SetBranchAddress("MCTrack.fStartX", MCTrack_fStartX, &b_MCTrack_fStartX);
   fChain->SetBranchAddress("MCTrack.fStartY", MCTrack_fStartY, &b_MCTrack_fStartY);
   fChain->SetBranchAddress("MCTrack.fStartZ", MCTrack_fStartZ, &b_MCTrack_fStartZ);
   fChain->SetBranchAddress("MCTrack.fStartT", MCTrack_fStartT, &b_MCTrack_fStartT);
   fChain->SetBranchAddress("MCTrack.fNPoints", MCTrack_fNPoints, &b_MCTrack_fNPoints);
   fChain->SetBranchAddress("MCTrack.fMass", MCTrack_fMass, &b_MCTrack_fMass);
   fChain->SetBranchAddress("LandPoint", &LandPoint_, &b_cbmroot_Land_LandPoint_);
   fChain->SetBranchAddress("LandPoint.fUniqueID", LandPoint_fUniqueID, &b_LandPoint_fUniqueID);
   fChain->SetBranchAddress("LandPoint.fBits", LandPoint_fBits, &b_LandPoint_fBits);
   fChain->SetBranchAddress("LandPoint.fTrackID", LandPoint_fTrackID, &b_LandPoint_fTrackID);
   fChain->SetBranchAddress("LandPoint.fDetectorID", LandPoint_fDetectorID, &b_LandPoint_fDetectorID);
   fChain->SetBranchAddress("LandPoint.fX", LandPoint_fX, &b_LandPoint_fX);
   fChain->SetBranchAddress("LandPoint.fY", LandPoint_fY, &b_LandPoint_fY);
   fChain->SetBranchAddress("LandPoint.fZ", LandPoint_fZ, &b_LandPoint_fZ);
   fChain->SetBranchAddress("LandPoint.fPx", LandPoint_fPx, &b_LandPoint_fPx);
   fChain->SetBranchAddress("LandPoint.fPy", LandPoint_fPy, &b_LandPoint_fPy);
   fChain->SetBranchAddress("LandPoint.fPz", LandPoint_fPz, &b_LandPoint_fPz);
   fChain->SetBranchAddress("LandPoint.fTime", LandPoint_fTime, &b_LandPoint_fTime);
   fChain->SetBranchAddress("LandPoint.fLength", LandPoint_fLength, &b_LandPoint_fLength);
   fChain->SetBranchAddress("LandPoint.fELoss", LandPoint_fELoss, &b_LandPoint_fELoss);
   fChain->SetBranchAddress("LandPoint.fX_out", LandPoint_fX_out, &b_LandPoint_fX_out);
   fChain->SetBranchAddress("LandPoint.fY_out", LandPoint_fY_out, &b_LandPoint_fY_out);
   fChain->SetBranchAddress("LandPoint.fZ_out", LandPoint_fZ_out, &b_LandPoint_fZ_out);
   fChain->SetBranchAddress("LandPoint.fPx_out", LandPoint_fPx_out, &b_LandPoint_fPx_out);
   fChain->SetBranchAddress("LandPoint.fPy_out", LandPoint_fPy_out, &b_LandPoint_fPy_out);
   fChain->SetBranchAddress("LandPoint.fPz_out", LandPoint_fPz_out, &b_LandPoint_fPz_out);
   fChain->SetBranchAddress("LandPoint.fSector", LandPoint_fSector, &b_LandPoint_fSector);
   fChain->SetBranchAddress("LandPoint.fPaddleNb", LandPoint_fPaddleNb, &b_LandPoint_fPaddleNb);
   fChain->SetBranchAddress("LandPoint.fPaddleTyp", LandPoint_fPaddleTyp, &b_LandPoint_fPaddleTyp);
   fChain->SetBranchAddress("LandPoint.fLightYield", LandPoint_fLightYield, &b_LandPoint_fLightYield);
   fChain->SetBranchAddress("EventHeader.", &EventHeader_, &b_cbmroot_EvtHeader_EventHeader_);
   fChain->SetBranchAddress("EventHeader.TNamed.fUniqueID", &EventHeader_TNamed_fUniqueID, &b_EventHeader_cbmroot_EvtHeader_EventHeader_TNamed_fUniqueID);
   fChain->SetBranchAddress("EventHeader.TNamed.fBits", &EventHeader_TNamed_fBits, &b_EventHeader_cbmroot_EvtHeader_EventHeader_TNamed_fBits);
   fChain->SetBranchAddress("EventHeader.TNamed.fName", &EventHeader_TNamed_fName, &b_EventHeader_cbmroot_EvtHeader_EventHeader_TNamed_fName);
   fChain->SetBranchAddress("EventHeader.TNamed.fTitle", &EventHeader_TNamed_fTitle, &b_EventHeader_cbmroot_EvtHeader_EventHeader_TNamed_fTitle);
   fChain->SetBranchAddress("EventHeader.fRunId", &EventHeader_fRunId, &b_EventHeader_cbmroot_EvtHeader_EventHeader_fRunId);
   fChain->SetBranchAddress("MCEventHeader.", &MCEventHeader_, &b_cbmroot_Event_MCEventHeader_);
   fChain->SetBranchAddress("MCEventHeader.TNamed.fUniqueID", &MCEventHeader_TNamed_fUniqueID, &b_MCEventHeader_cbmroot_Event_MCEventHeader_TNamed_fUniqueID);
   fChain->SetBranchAddress("MCEventHeader.TNamed.fBits", &MCEventHeader_TNamed_fBits, &b_MCEventHeader_cbmroot_Event_MCEventHeader_TNamed_fBits);
   fChain->SetBranchAddress("MCEventHeader.TNamed.fName", &MCEventHeader_TNamed_fName, &b_MCEventHeader_cbmroot_Event_MCEventHeader_TNamed_fName);
   fChain->SetBranchAddress("MCEventHeader.TNamed.fTitle", &MCEventHeader_TNamed_fTitle, &b_MCEventHeader_cbmroot_Event_MCEventHeader_TNamed_fTitle);
   fChain->SetBranchAddress("MCEventHeader.fRunId", &MCEventHeader_fRunId, &b_MCEventHeader_cbmroot_Event_MCEventHeader_fRunId);
   fChain->SetBranchAddress("MCEventHeader.fEventId", &MCEventHeader_fEventId, &b_MCEventHeader_cbmroot_Event_MCEventHeader_fEventId);
   fChain->SetBranchAddress("MCEventHeader.fX", &MCEventHeader_fX, &b_MCEventHeader_cbmroot_Event_MCEventHeader_fX);
   fChain->SetBranchAddress("MCEventHeader.fY", &MCEventHeader_fY, &b_MCEventHeader_cbmroot_Event_MCEventHeader_fY);
   fChain->SetBranchAddress("MCEventHeader.fZ", &MCEventHeader_fZ, &b_MCEventHeader_cbmroot_Event_MCEventHeader_fZ);
   fChain->SetBranchAddress("MCEventHeader.fT", &MCEventHeader_fT, &b_MCEventHeader_cbmroot_Event_MCEventHeader_fT);
   fChain->SetBranchAddress("MCEventHeader.fB", &MCEventHeader_fB, &b_MCEventHeader_cbmroot_Event_MCEventHeader_fB);
   fChain->SetBranchAddress("MCEventHeader.fNPrim", &MCEventHeader_fNPrim, &b_MCEventHeader_cbmroot_Event_MCEventHeader_fNPrim);
   fChain->SetBranchAddress("MCEventHeader.fIsSet", &MCEventHeader_fIsSet, &b_MCEventHeader_cbmroot_Event_MCEventHeader_fIsSet);
   fChain->SetBranchAddress("GeoTracks", &GeoTracks_, &b_cbmroot_MCGeoTrack_GeoTracks_);
   fChain->SetBranchAddress("GeoTracks.fUniqueID", GeoTracks_fUniqueID, &b_GeoTracks_fUniqueID);
   fChain->SetBranchAddress("GeoTracks.fBits", GeoTracks_fBits, &b_GeoTracks_fBits);
   fChain->SetBranchAddress("GeoTracks.fGeoAtt", GeoTracks_fGeoAtt, &b_GeoTracks_fGeoAtt);
   fChain->SetBranchAddress("GeoTracks.fLineColor", GeoTracks_fLineColor, &b_GeoTracks_fLineColor);
   fChain->SetBranchAddress("GeoTracks.fLineStyle", GeoTracks_fLineStyle, &b_GeoTracks_fLineStyle);
   fChain->SetBranchAddress("GeoTracks.fLineWidth", GeoTracks_fLineWidth, &b_GeoTracks_fLineWidth);
   fChain->SetBranchAddress("GeoTracks.fMarkerColor", GeoTracks_fMarkerColor, &b_GeoTracks_fMarkerColor);
   fChain->SetBranchAddress("GeoTracks.fMarkerStyle", GeoTracks_fMarkerStyle, &b_GeoTracks_fMarkerStyle);
   fChain->SetBranchAddress("GeoTracks.fMarkerSize", GeoTracks_fMarkerSize, &b_GeoTracks_fMarkerSize);
   fChain->SetBranchAddress("GeoTracks.fPDG", GeoTracks_fPDG, &b_GeoTracks_fPDG);
   fChain->SetBranchAddress("GeoTracks.fId", GeoTracks_fId, &b_GeoTracks_fId);
   fChain->SetBranchAddress("GeoTracks.fPointsSize", GeoTracks_fPointsSize, &b_GeoTracks_fPointsSize);
   fChain->SetBranchAddress("GeoTracks.fNpoints", GeoTracks_fNpoints, &b_GeoTracks_fNpoints);
   fChain->SetBranchAddress("GeoTracks.fPoints", GeoTracks_fPoints, &b_GeoTracks_fPoints);
   Notify();
}

Bool_t SimData::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void SimData::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t SimData::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef SimData_cxx
