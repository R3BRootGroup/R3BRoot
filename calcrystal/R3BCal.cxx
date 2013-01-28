// -------------------------------------------------------------------------
// -----                  R3BCal source file                           -----
// -----            Created 26/03/09  by D.Bertini                     -----
// -----        new Version: 20/07/09 <D.Bertini@gsi.de>               -----
// -----        new Version: 08/04/10 <wranne@student.chalmers.se>     -----
// -----        new Version: 25/11/10 <marc.labiche@stfc.ac.uk>        -----
// -----        new Version: 08/06/12 <pablo.cabanelas@usc.es>         -----
// -------------------------------------------------------------------------
#include "R3BCal.h"

#include "R3BGeoCal.h"
#include "R3BCalPoint.h"
#include "R3BCalCrystalHit.h"
#include "R3BCalCrystalHitSim.h"
#include "R3BGeoCalPar.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairStack.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairVolume.h"

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TObjArray.h"
#include "TRotation.h"

// includes for modeling
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoArb8.h"
#include "TGeoBBox.h"
#include "TGeoPgon.h"
#include "TGeoCone.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include "TGeoTube.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include <iostream>
// try the mathematical ROOT package
#include "Math/Rotation3D.h"
#include "Math/Point3D.h"

using std::cout;
using std::cerr;
using std::endl;

#define XB_CRYSTAL(no,type,theta,phi,psi,n1,n2,n3,n4,n5,n6) { no, type, theta, phi, psi },
#define XB_CRYSTAL_MOD(nr,mod,delta) { nr, mod, delta },

#define XB_WRAPPING(no,type,theta,phi,psi,n1,n2,n3,n4,n5,n6) { no, type, theta, phi, psi },
#define XB_WRAPPING_MOD(nr,mod,delta) { nr, mod, delta },


// -----   Default constructor   -------------------------------------------
R3BCal::R3BCal() : R3BDetector("R3BCal", kTRUE, kCAL) {
  ResetParameters();
  fCalCollection = new TClonesArray("R3BCalPoint");
  fCalCrystalHitCollection = new TClonesArray("R3BCalCrystalHitSim");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  kDrawCrystals = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fGeoVersion = 1;
  fCollectionOption = 0;
  fNonUniformity = 0.;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BCal::R3BCal(const char* name, Bool_t active) 
  : R3BDetector(name, active, kCAL) {
  ResetParameters();
  fCalCollection = new TClonesArray("R3BCalPoint");
  fCalCrystalHitCollection = new TClonesArray("R3BCalCrystalHitSim");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  kDrawCrystals = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fGeoVersion = 1;
  fCollectionOption = 0;
  fNonUniformity = 0.;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BCal::~R3BCal() {

  if ( flGeoPar ) delete flGeoPar;
  if (fCalCollection) {
   fCalCollection->Delete();
   delete fCalCollection;
  }
  if (fCalCrystalHitCollection) {
   fCalCrystalHitCollection->Delete();
   delete fCalCrystalHitCollection;
  }
}
// -------------------------------------------------------------------------

void R3BCal::Initialize()
{
  FairDetector::Initialize();

  cout << endl;
  cout << "-I- R3BCal initialisation" << endl;
  cout << "-I- R3BCal: Vol. (McId)" << endl;
  cout << "-I- R3BCal: Crystal A : " << gMC->VolId("crystalLogNAJA")<< endl;
  cout << "-I- R3BCal: Crystal B : " << gMC->VolId("crystalLogNAJB")<< endl;
  cout << "-I- R3BCal: Crystal C : " << gMC->VolId("crystalLogNAJC")<< endl;
  cout << "-I- R3BCal: Crystal D : " << gMC->VolId("crystalLogNAJD")<< endl;

  // Crystals type ID
  // type  ID
  // A    1
  // B    2
  // C    3
  // D    4

  fTypeA = gMC->VolId("crystalLogNAJA");
  fTypeB = gMC->VolId("crystalLogNAJB");
  fTypeC = gMC->VolId("crystalLogNAJC");
  fTypeD = gMC->VolId("crystalLogNAJD");

  TGeoVolume *vol = gGeoManager->GetVolume("CBLogWorld");
  vol->SetVisibility(kFALSE); 

}


void R3BCal::SetSpecialPhysicsCuts(){

  cout << endl;

  cout << "-I- R3BCal: Adding customized Physics cut ... " << endl;
  cout << "-I- R3BCal: Yet not implemented !... " << endl;

  cout << endl;

}



// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BCal::ProcessHits(FairVolume* vol) {

  Int_t copyNo=0;

  // Gets the right crystal type
  Int_t fVolID = gMC->CurrentVolID(copyNo);
  Int_t fCrystalType = GetCrystalType(fVolID);

  // Gets the right crystal number from its mother wrapping
  fVolID = gMC->CurrentVolOffID(1,copyNo);

  if ( gMC->IsTrackEntering() ) {
    fELoss = 0.;
    fNSteps  = 0;
    fTime = gMC->TrackTime() * 1.0e09;    //[ns]
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
    fEinc   = gMC->Etot();
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();
  fNSteps++;

  // Set additional parameters at exit of active volume. Create R3BCalPoint.
  if ( gMC->IsTrackExiting() ||
     gMC->IsTrackStop() ||
     gMC->IsTrackDisappeared() ) {

    fTrackID        = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID       = vol->getMCid();
    fParentTrackID  = gMC->GetStack()->GetCurrentParentTrackNumber();
    fTrackPID       = gMC->TrackPid();
    fUniqueID       = gMC->GetStack()->GetCurrentTrack()->GetUniqueID();

    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);

    if (fELoss == 0. ) return kFALSE;

    if (gMC->IsTrackExiting()) {
      const Double_t* oldpos;
      const Double_t* olddirection;
      Double_t newpos[3];
      Double_t newdirection[3];
      Double_t safety;

      gGeoManager->FindNode(fPosOut.X(),fPosOut.Y(),fPosOut.Z());
      oldpos = gGeoManager->GetCurrentPoint();
      olddirection = gGeoManager->GetCurrentDirection();

      for (Int_t i=0; i<3; i++){
        newdirection[i] = -1*olddirection[i];
      }

      gGeoManager->SetCurrentDirection(newdirection);
      safety = gGeoManager->GetSafeDistance();


      gGeoManager->SetCurrentDirection(-newdirection[0],-newdirection[1],-newdirection[2]);

      for (Int_t i=0; i<3; i++){
        newpos[i] = oldpos[i] - (3*safety*olddirection[i]);
      }

      if ( fPosIn.Z() < 30. && newpos[2] > 30.02 ) {
        cerr << "2nd direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2] 
            << " with safety = " << safety << endl;
        cerr << "oldpos = " << oldpos[0] << "," << oldpos[1] << "," << oldpos[2] << endl;
        cerr << "newpos = " << newpos[0] << "," << newpos[1] << "," << newpos[2] << endl;
      }

      fPosOut.SetX(newpos[0]);
      fPosOut.SetY(newpos[1]);
      fPosOut.SetZ(newpos[2]);
    }

    if(fCollectionOption == 0 || fCollectionOption == 2) {
      AddHit(fTrackID, fVolumeID, fCrystalType, copyNo ,
        TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()),
        TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()),
        TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
        TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
        fTime, fLength, fELoss);

      // Increment number of CalPoints for this track
      FairStack* stack = (FairStack*) gMC->GetStack();
      stack->AddPoint(kCAL);
    }

    //Adding a crystalHit support
    if(fCollectionOption == 1 || fCollectionOption == 2) {
      Int_t nCrystalHits = fCalCrystalHitCollection->GetEntriesFast();
      Bool_t existHit = 0;

      if (nCrystalHits==0) AddCrystalHit(fCrystalType, copyNo, NUSmearing(fELoss), fTime, fNSteps, fEinc,
					fTrackID, fVolumeID, fParentTrackID, fTrackPID, fUniqueID);
      else {
        for (Int_t i=0; i<nCrystalHits; i++) {
          if ( ((R3BCalCrystalHitSim *)(fCalCrystalHitCollection->At(i)))->GetCrystalNumber() == copyNo ) {
            ((R3BCalCrystalHitSim *)(fCalCrystalHitCollection->At(i)))->AddMoreEnergy(NUSmearing(fELoss));
            if ( ((R3BCalCrystalHitSim *)(fCalCrystalHitCollection->At(i)))->GetTime() > fTime ) {
              ((R3BCalCrystalHitSim *)(fCalCrystalHitCollection->At(i)))->SetTime(fTime);
            }
            existHit=1; //to avoid the creation of a new CrystalHit
            break;
          }
        }
        if (!existHit) AddCrystalHit(fCrystalType, copyNo, NUSmearing(fELoss), fTime, fNSteps, fEinc,
					fTrackID, fVolumeID, fParentTrackID, fTrackPID, fUniqueID);
      }

      existHit=0;
    }


    ResetParameters();
  }
  return kTRUE;
}
// ----------------------------------------------------------------------------
/*
void R3BCal::SaveGeoParams(){
  cout << " -I Save STS geo params " << endl;

  TFolder *mf = (TFolder*) gDirectory->FindObjectAny("cbmroot");
  cout << " mf: " << mf << endl;
  TFolder *stsf = NULL;
  if (mf ) stsf = (TFolder*) mf->FindObjectAny(GetName());
  cout << " stsf: " << stsf << endl;
  if (stsf) stsf->Add( flGeoPar0 ) ;
  FairRootManager::Instance()->WriteFolder();
  mf->Write("cbmroot",TObject::kWriteDelete);
}
*/


// -----   Public method EndOfEvent   -----------------------------------------
void R3BCal::BeginEvent() {

//  if (! kGeoSaved ) {
//    SaveGeoParams();
//  cout << "-I STS geometry parameters saved " << endl;
//  kGeoSaved = kTRUE;
//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BCal::EndOfEvent() {

  if (fVerboseLevel) Print();
 
  if(fCollectionOption == 0) { 
    fCalCollection->Clear();
  } else if(fCollectionOption == 1) { 
    fCalCrystalHitCollection->Clear();
  } else if(fCollectionOption == 2) { 
    fCalCollection->Clear();
    fCalCrystalHitCollection->Clear();
  }

  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void R3BCal::Register() {
  if(fCollectionOption == 0) { 
    FairRootManager::Instance()->Register("XBCrystalPoint", GetName(), fCalCollection, kTRUE);
  } else if(fCollectionOption == 1) { 
    FairRootManager::Instance()->Register("XBCrystalHitSim", GetName(), fCalCrystalHitCollection, kTRUE);
  } else if(fCollectionOption == 2) { 
    FairRootManager::Instance()->Register("XBCrystalPoint", GetName(), fCalCollection, kTRUE);
    FairRootManager::Instance()->Register("XBCrystalHitSim", GetName(), fCalCrystalHitCollection, kTRUE);
  }
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BCal::GetCollection(Int_t iColl) const {
  if(fCollectionOption == 0) { 
    if (iColl == 0) return fCalCollection;
    else return NULL;
  } else if(fCollectionOption == 1) { 
    if (iColl == 0) return fCalCrystalHitCollection;
    else return NULL;
  } else if(fCollectionOption == 2) { 
    if (iColl == 0) return fCalCollection;
    if (iColl == 1) return fCalCrystalHitCollection;
    else return NULL;
  } else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BCal::Print() const {
  if(fCollectionOption == 0) { 
    Int_t nHits = fCalCollection->GetEntriesFast();
    cout << "-I- R3BCal: " << nHits << " points registered in this event." << endl;
  } else if(fCollectionOption == 1) { 
    Int_t nHits = fCalCrystalHitCollection->GetEntriesFast();
    cout << "-I- R3BCal: " << nHits << " hits registered in this event." << endl;
  } else if(fCollectionOption == 2) { 
    Int_t nHits = fCalCollection->GetEntriesFast();
    cout << "-I- R3BCal: " << nHits << " points registered in this event." << endl;
    nHits = fCalCrystalHitCollection->GetEntriesFast();
    cout << "-I- R3BCal: " << nHits << " hits registered in this event." << endl;
  }
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BCal::Reset() {
  fCalCollection->Clear();
  fCalCrystalHitCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BCal::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- R3BCal: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  R3BCalPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++)
  {
    oldpoint = (R3BCalPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BCalPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- R3BCal: " << cl2->GetEntriesFast() << " merged entries." << endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BCalPoint* R3BCal::AddHit(Int_t trackID, Int_t detID, Int_t type, Int_t cp, TVector3 posIn,
        TVector3 posOut, TVector3 momIn, 
        TVector3 momOut, Double_t time, 
        Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fCalCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- R3BCal: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
     << ", " << posIn.Z() << ") cm, detector " << detID << ", track "
     << trackID << ", energy loss " << eLoss*1e06 << " keV" << endl;
  return new(clref[size]) R3BCalPoint(trackID, detID, type, cp , posIn, posOut,
            momIn, momOut, time, length, eLoss);
}

// -----   Private method AddCrystalHit   --------------------------------------------
R3BCalCrystalHitSim* R3BCal::AddCrystalHit(Int_t type, Int_t copy,
    Double_t energy, Double_t time, Int_t steps, Double_t einc,
    Int_t trackid, Int_t volid, Int_t partrackid, Int_t pdgtype, Int_t uniqueid)
{
  TClonesArray& clref = *fCalCrystalHitCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1)
    cout << "-I- R3BCal: Adding Hit in detector type " << type << ", and number " << copy
         << " entering with " << einc*1e06 << " keV, depositing " << energy*1e06 << " keV" << endl;
    cout << " -I- trackid: " << trackid << " volume id: " << volid << " partrackid : " << partrackid << " type: " << pdgtype << " unique id: " << uniqueid << endl; 
  return new(clref[size]) R3BCalCrystalHitSim(type, copy, energy, time, steps, einc, trackid, volid, partrackid, pdgtype, uniqueid);
}

// -----   Private method NUSmearing  --------------------------------------------
Double_t R3BCal::NUSmearing(Double_t inputEnergy)
{
  // Very simple preliminary scheme where the NU is introduced as a flat random
  // distribution with limits fNonUniformity (%) of the energy value.
  //
  return gRandom->Uniform(inputEnergy-inputEnergy*fNonUniformity/100,inputEnergy+inputEnergy*fNonUniformity/100);
}

// -----  Public method SetNonUniformity  ----------------------------------
void R3BCal::SetNonUniformity(Double_t nonU)
{
  fNonUniformity = nonU;
  cout << "-I- R3BCal::SetNonUniformity to " << fNonUniformity << " %." << endl;
}



void R3BCal::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")) {
    fLogger->Info(MESSAGE_ORIGIN,
		  "Constructing Crystal Ball geometry from ROOT file %s", 
		  fileName.Data());
    ConstructRootGeometry();
  } else {
    fLogger->Info(MESSAGE_ORIGIN,
		  "Constructing hardcoded Crystal Ball geometry");
    ConstructGeometryOld();
  }
}



Bool_t R3BCal::CheckIfSensitive(std::string name)
{
  TString volName = name;
  if(volName.Contains("crystalLogNAJ")) {
    return kTRUE;
  }
  return kFALSE;
}




void R3BCal::ConstructGeometryOld()
{
    cout
    << "##################################################################\n"
    << "*     -I- R3BCal R3BCal::ConstructGeometry()                     *\n"
    << "*        Darmstadt-Heidelberg Crystal Ball - CAVE C Setup        *\n"
    << "##################################################################\n";

  // out-of-file geometry definition
  Double_t dx,dy,dz;
  Double_t rmin, rmax, rmin1, rmax1, rmin2, rmax2;
  Double_t a;
  Double_t theta1, theta2, phi1, phi2, dphi;
  Double_t z, density, radl, absl, w;
  Int_t nel, numed;
  double crystalPosRad =35;  //Radius of the center of the crystals [cm]
  double wrappingPosRad=35;  //Radius of the center of the wrappings [cm]

// --- Aluminum Wrappings

  // Definitions for the Al wrapping types from "xb_wrapping_loc.hh"
  int XB_WTYPE_A=0, XB_WTYPE_B=1, XB_WTYPE_C=2, XB_WTYPE_D=3, WDISABLE=0, WMOVE_R=1;

  // Get wrapping position information
  xb_wrapping xb_wrappings[] =
  {
    #include "xb_wrapping_loc.hh"
  };

  int nrWrappings = sizeof( xb_wrappings ) / sizeof( xb_wrappings[0] );
  for (int i = 0; i < nrWrappings; i++){
    xb_wrappings[i].active = true;}

  // Get corner information for the wrappings
  double xb_wrapping_parts[][16] =
  {
    #include "xb_wrapping_part.hh"
  };

  // Get modification information for wrappings
  xb_wrapping_mod xb_wrappings_mod[] =
  {
    #include "xb_wrapping_mod.hh"
  };

// --- NaI Crystals

  // Get corner information for the crystals
  double xb_crystal_parts[][16] =
  {
    #include "xb_crystal_part.hh"
  };


// --- Local Material definition

  // Mixture: Air
  TGeoMedium * pMed2=NULL;
  if (gGeoManager->GetMedium("Air") )
    pMed2=gGeoManager->GetMedium("Air");
  else
  {
    nel  = 2;
    density = 0.001290;
    TGeoMixture*
    pMat2 = new TGeoMixture("Air", nel,density);
    a = 14.006740; z = 7.000000; w = 0.700000; // N
    pMat2->DefineElement(0,a,z,w);
    a = 15.999400; z = 8.000000; w = 0.300000; // O
    pMat2->DefineElement(1,a,z,w);
    pMat2->SetIndex(1);
    // Medium: Air
    numed = 1; // medium number
    Double_t par[8];
    par[0] = 0.000000; // isvol
    par[1] = 0.000000; // ifield
    par[2] = 0.000000; // fieldm
    par[3] = 0.000000; // tmaxfd
    par[4] = 0.000000; // stemax
    par[5] = 0.000000; // deemax
    par[6] = 0.000100; // epsil
    par[7] = 0.000000; // stmin
    pMed2 = new TGeoMedium("Air", numed,pMat2, par);
  }

  // Mixture: NaI
  TGeoMedium * pMed10=NULL;
   if (gGeoManager->GetMedium("NaIn") ){
       pMed10=gGeoManager->GetMedium("NaIn");
  }else{
    nel     = 2;
    density = 3.670000;
    TGeoMixture*
	pMat10 = new TGeoMixture("NaIn", nel,density);
    a = 22.989770;   z = 11.000000;   w = 0.15337;  // Na
    pMat10->DefineElement(0,a,z,w);
    a = 126.904470;   z = 53.000000;   w = 0.84663;  // I
    pMat10->DefineElement(1,a,z,w);
    pMat10->SetIndex(611);
    Double_t par[8];
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 0.000000; // tmaxfd
    par[4]  = 0.000000; // stemax
    par[5]  = 0.000000; // deemax
    par[6]  = 0.000100; // epsil
    par[7]  = 0.000000; // stmin
    pMed10 = new TGeoMedium("NaIn", 2,pMat10, par);
  }

/* ------- TGeoMedium not used any longer -------
  // Mixture: CsI
  TGeoMedium * pMed9=NULL;
  if (gGeoManager->GetMedium("CsIn") )
    pMed9=gGeoManager->GetMedium("CsIn");
  else
  {
    nel  = 2;
    density = 4.510000;
    TGeoMixture*
    pMat9 = new TGeoMixture("CsIn", nel,density);
    a = 132.905450; z = 55.000000; w = 0.511549;  // CS
    pMat9->DefineElement(0,a,z,w);
    a = 126.904470; z = 53.000000; w = 0.488451;  // I
    pMat9->DefineElement(1,a,z,w);
    pMat9->SetIndex(601);
    Double_t par[8];
    par[0] = 0.000000; // isvol
    par[1] = 0.000000; // ifield
    par[2] = 0.000000; // fieldm
    par[3] = 0.000000; // tmaxfd
    par[4] = 0.000000; // stemax
    par[5] = 0.000000; // deemax
    par[6] = 0.000100; // epsil
    par[7] = 0.000000; // stmin
    pMed9 = new TGeoMedium("CsIn", 2,pMat9, par);
  }
------------------------------------------------- */

  // Material: Aluminum
  TGeoMedium * pMed21=NULL;
  if (gGeoManager->GetMedium("Aluminum") )
       pMed21=gGeoManager->GetMedium("Aluminum");
  else
  {
    a = 26.980000;
    z = 13.000000;
    density = 2.700000;
    radl = 8.875105;
    absl = 388.793113;
    TGeoMaterial *matAl = new TGeoMaterial("Aluminum", a,z,density,radl,absl);
    Double_t par[8];
    par[0] = 0.000000; // isvol
    par[1] = 0.000000; // ifield
    par[2] = 0.000000; // fieldm
    par[3] = 0.000000; // tmaxfd
    par[4] = 0.000000; // stemax
    par[5] = 0.000000; // deemax
    par[6] = 0.000100; // epsil
    par[7] = 0.000000; // stmin
    pMed21 = new TGeoMedium("Aluminum",3, matAl, par);
  }


// --- Create Volumes: Aluminum Wrappings

  // Create volume: wrappingLogNAJA
  TGeoShape *pWTRA = createVolume(xb_wrapping_parts[0],0);
  pWTRA->SetTitle("WTRA1+WTRA2");
  TGeoVolume* pwrappingLogNAJA = new TGeoVolume("wrappingLogNAJA",pWTRA, pMed21);
  pwrappingLogNAJA->SetLineColor(34);
  pwrappingLogNAJA->VisibleDaughters(kDrawCrystals);

  // Create volume: wrappingLogNAJB
  TGeoShape *pWTRB = createVolume(xb_wrapping_parts[1],0);
  pWTRB->SetTitle("WTRB1+WTRB2");
  TGeoVolume* pwrappingLogNAJB = new TGeoVolume("wrappingLogNAJB",pWTRB, pMed21);
  pwrappingLogNAJB->SetLineColor(35);
  pwrappingLogNAJB->VisibleDaughters(kDrawCrystals);

  // Create volume: wrappingLogNAJC
  TGeoShape *pWTRC = createVolume(xb_wrapping_parts[2],0);
  pWTRC->SetTitle("WTRC1+WTRC2");
  TGeoVolume* pwrappingLogNAJC = new TGeoVolume("wrappingLogNAJC",pWTRC, pMed21);
  pwrappingLogNAJC->SetLineColor(36);
  pwrappingLogNAJC->VisibleDaughters(kDrawCrystals);

  // Create volume: wrappingLogNAJC
  TGeoShape *pWTRD = createVolume(xb_wrapping_parts[3],0);
  pWTRD->SetTitle("WTRD1+WTRD2");
  TGeoVolume* pwrappingLogNAJD = new TGeoVolume("wrappingLogNAJD",pWTRD, pMed21);
  pwrappingLogNAJD->SetLineColor(37);
  pwrappingLogNAJD->VisibleDaughters(kDrawCrystals);

  TGeoVolume* wrappingVolumes[]={pwrappingLogNAJA,
                                pwrappingLogNAJB,
                                pwrappingLogNAJC,
                                pwrappingLogNAJD};

// --- Create Volumes: NaI Volumes

  // Create volume: crystalLogNAJA
  TGeoShape *pTRA = createVolume(xb_crystal_parts[0],1);
  pTRA->SetTitle("TRA1+TRA2");
  TGeoVolume* pcrystalLogNAJA = new TGeoVolume("crystalLogNAJA",pTRA, pMed10);
  pcrystalLogNAJA->SetLineColor(41);

  // Create volume: crystalLogNAJB
  TGeoShape *pTRB = createVolume(xb_crystal_parts[1],1);
  pTRB->SetTitle("TRB1+TRB2");
  TGeoVolume* pcrystalLogNAJB = new TGeoVolume("crystalLogNAJB",pTRB, pMed10);
  pcrystalLogNAJB->SetLineColor(42);

  // Create volume: crystalLogNAJC
  TGeoShape *pTRC = createVolume(xb_crystal_parts[2],1);
  pTRC->SetTitle("TRC1+TRC2");
  TGeoVolume* pcrystalLogNAJC = new TGeoVolume("crystalLogNAJC",pTRC, pMed10);
  pcrystalLogNAJC->SetLineColor(43);

  // Create volume: crystalLogNAJC
  TGeoShape *pTRD = createVolume(xb_crystal_parts[3],1);
  pTRD->SetTitle("TRD1+TRD2");
  TGeoVolume* pcrystalLogNAJD = new TGeoVolume("crystalLogNAJD",pTRD, pMed10);
  pcrystalLogNAJD->SetLineColor(44);

  TGeoVolume* crystalVolumes[]={pcrystalLogNAJA,
                                pcrystalLogNAJB,
                                pcrystalLogNAJC,
                                pcrystalLogNAJD};

// ---- Geometry Definition

  // Get the top Volume
  TGeoVolume *top = gGeoManager->GetTopVolume();

  // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
  // Shape: CBSphereWorld type: TGeoSphere
  rmin  = 24.250000;
  rmax  = 70.000000;
  theta1 = 0.000000;
  theta2 = 180.000000;
  phi1  = 0.000000;
  phi2  = 360.000000;
  TGeoShape *pCBSphereWorld = new TGeoSphere("CBSphereWorld",rmin,rmax,theta1, theta2,phi1,phi2);
  // Volume: CBLogWorld
  TGeoVolume*
  pCBLogWorld = new TGeoVolume("CBLogWorld",pCBSphereWorld, pMed2);
  pCBLogWorld->SetVisLeaves(kTRUE);

  TGeoRotation *zeroRot = new TGeoRotation;
  TGeoTranslation *zeroTrans = new TGeoTranslation;
  TGeoCombiTrans *zeroRotTrans = new TGeoCombiTrans(*zeroTrans, *zeroRot);

  // add the sphere as Mother Volume
  top->AddNode(pCBLogWorld, 0, zeroRotTrans);

  // --- Definition of the Reaction Chamber
  // Combi transformation: 
  dx = 0.000000;
  dy = 0.000000;
  dz = 39.100000;
  // Rotation: 
  TGeoCombiTrans* pCombTrans1 = new TGeoCombiTrans("", dx,dy,dz, zeroRot);
  // Combi transformation: 
  dx = 0.000000;
  dy = 0.000000;
  dz = -39.100000;
  // Rotation:
  TGeoRotation *pRot1 = new TGeoRotation("", 0, 180, 0);
  TGeoCombiTrans* pCombTrans2 = new TGeoCombiTrans("", dx,dy,dz, pRot1);
  // Combi transformation:
  dx = 0.000000; 
  dy = -39.100000;
  dz = 0.000000;
  // Rotation: 
  TGeoRotation *pRot2 = new TGeoRotation("", 0, 90, 0);
  TGeoCombiTrans* pCombTrans3 = new TGeoCombiTrans("", dx,dy,dz, pRot2);

  // Shape: CBReactionChamber type: TGeoSphere
  rmin = 24.250000;
  rmax = 24.500000;
  theta1 = 6.423161;
  theta2 = 173.576839;
  phi1 = 0.000000;
  phi2 = 360.000000;
  TGeoShape *pCBReactionChamber = new TGeoSphere("CBReactionChamber",rmin,rmax,theta1, theta2,phi1,phi2);
  // Volume: CBChamberLog
  TGeoVolume* pCBChamberLog = new TGeoVolume("CBChamberLog",pCBReactionChamber, pMed21);
  pCBChamberLog->SetVisLeaves(kTRUE);
  pCBChamberLog->SetLineColor(18);
  pCBLogWorld->AddNode(pCBChamberLog, 0, zeroRotTrans);

  // Shape: CBConicalTube type: TGeoConeSeg
  dz = 14.600000;
  rmin1 = 2.730000;
  rmax1 = 2.855000;
  rmin2 = 5.125000;
  rmax2 = 5.250000;
  phi1 = 0.000000;
  phi2 = 360.000000;
  TGeoShape *pCBConicalTube = new TGeoConeSeg("CBConicalTube", dz,rmin1,rmax1,rmin2,rmax2,phi1,phi2);
  // Volume: CBConicalTubeLog
  TGeoVolume* pCBConicalTubeLog = new TGeoVolume("CBConicalTubeLog",pCBConicalTube, pMed21);
  pCBConicalTubeLog->SetVisLeaves(kTRUE);
  pCBConicalTubeLog->SetLineColor(33);
  pCBLogWorld->AddNode(pCBConicalTubeLog, 0, pCombTrans1);
  pCBLogWorld->AddNode(pCBConicalTubeLog, 1, pCombTrans2);
  pCBLogWorld->AddNode(pCBConicalTubeLog, 2, pCombTrans3);

// --- Add Wrappings to mother volume (CBWorld)

  // Add modified wrappings to the crystal ball, defined in "xb_wrapping_mod.hh"
  int nrModifiedWrappings = sizeof( xb_wrappings_mod ) / sizeof( xb_wrappings_mod[0] );
  for (int i = 0; i < nrModifiedWrappings; i++)
  {
    xb_wrappings[xb_wrappings_mod[i].no - 1].active = false;
    if (xb_wrappings_mod[i].mod == WMOVE_R)
      insertWrapping(&xb_wrappings[xb_wrappings_mod[i].no - 1], wrappingVolumes, pCBLogWorld, wrappingPosRad + xb_wrappings_mod[i].delta);
  }

  // Add all enabled wrappings to the wrapping ball
  for (int i=0; i < nrWrappings; i++){
    if (xb_wrappings[i].active) 
      insertWrapping(&xb_wrappings[i], wrappingVolumes, pCBLogWorld, wrappingPosRad);
  }

// --- Add crystals as wrappings' daughters
  pwrappingLogNAJA->AddNode(pcrystalLogNAJA, 0);
  pwrappingLogNAJB->AddNode(pcrystalLogNAJB, 0);
  pwrappingLogNAJC->AddNode(pcrystalLogNAJC, 0);
  pwrappingLogNAJD->AddNode(pcrystalLogNAJD, 0);

// --- End of Cal Crystal definition

// --- Define Sensitive Modules
  AddSensitiveVolume(pcrystalLogNAJA);
  AddSensitiveVolume(pcrystalLogNAJB);
  AddSensitiveVolume(pcrystalLogNAJC);
  AddSensitiveVolume(pcrystalLogNAJD);

}

// -----   Private method insertWrapping   ----------------------------------
// Add a wrapping in the right location using radious and information from "xb_wrapping_loc.hh"
void R3BCal::insertWrapping(xb_wrapping *wrapping, TGeoVolume **wrappingVolumes, TGeoVolume *worldVolume, double r)
{
  double conv=TMath::DegToRad();
  double dx = r*sin(wrapping->theta*conv)*cos(wrapping->phi*conv);
  double dy = r*sin(wrapping->theta*conv)*sin(wrapping->phi*conv);
  double dz = r*cos(wrapping->theta*conv);

  TGeoTranslation *trans = new TGeoTranslation(dx,dy,dz);
  TGeoRotation *pRot = new TGeoRotation;
  pRot->RotateZ(wrapping->psi);
  pRot->RotateY(wrapping->theta);
  pRot->RotateZ(wrapping->phi);

  TGeoCombiTrans *combi = new TGeoCombiTrans(*trans,*pRot);
 
  worldVolume->AddNode(wrappingVolumes[wrapping->type], wrapping->no, combi);
}


// -----   Private method createVolume   ----------------------------------
// Create TGeoShape from two TGeoArb8 for both crystals and wrappings
//   using the information in "xb_crysta_part.hh" and "xb_wrapping_part.hh"
TGeoShape* R3BCal::createVolume(double *arbVolumes, int voltype)
{

  double innerR = -1.;
  double dZ = -1.;

  double thick = 0.06;   // Al wrapping thickness in cm (600microns)

  if(voltype == 0) {          //wrappings
    innerR=25.;  //[cm]
    dZ=10.;      //[cm]
  } else if(voltype == 1) {   //crystals
    innerR=25.-thick;  //[cm]
    dZ=10.-thick;      //[cm]
  } else { cerr << " ******** Unknown volume type ******** " << endl; }

  // frameR is the radious of the frame which was the radious used during cornar definitions
  double frameR = 49.86932985;  // [cm]
  double scaleOuter=(innerR+(2*dZ))/frameR;
  double scaleInner=innerR/frameR;

  double volume1[][2]={
    arbVolumes[0]*scaleInner, arbVolumes[1]*scaleInner,
    arbVolumes[2]*scaleInner, arbVolumes[3]*scaleInner,
    arbVolumes[4]*scaleInner, arbVolumes[5]*scaleInner,
    arbVolumes[6]*scaleInner, arbVolumes[7]*scaleInner,
    arbVolumes[0]*scaleOuter, arbVolumes[1]*scaleOuter,
    arbVolumes[2]*scaleOuter, arbVolumes[3]*scaleOuter,
    arbVolumes[4]*scaleOuter, arbVolumes[5]*scaleOuter,
    arbVolumes[6]*scaleOuter, arbVolumes[7]*scaleOuter};

  double volume2[][2]={
    arbVolumes[8]*scaleInner, arbVolumes[9]*scaleInner,
    arbVolumes[10]*scaleInner, arbVolumes[11]*scaleInner,
    arbVolumes[12]*scaleInner, arbVolumes[13]*scaleInner,
    arbVolumes[14]*scaleInner, arbVolumes[15]*scaleInner,
    arbVolumes[8]*scaleOuter,  arbVolumes[9]*scaleOuter,
    arbVolumes[10]*scaleOuter, arbVolumes[11]*scaleOuter,
    arbVolumes[12]*scaleOuter, arbVolumes[13]*scaleOuter,
    arbVolumes[14]*scaleOuter, arbVolumes[15]*scaleOuter};

  TGeoArb8 *pTRA1 = new TGeoArb8("PARB1", dZ, *volume1);
  TGeoArb8 *pTRA2 = new TGeoArb8("PARB2", dZ, *volume2);
  TGeoBoolNode *pBoolNode = new TGeoUnion(pTRA1, pTRA2, 0, 0);
  TGeoShape *pTRA = new TGeoCompositeShape("", pBoolNode);
  return pTRA;
}



ClassImp(R3BCal)
