// -------------------------------------------------------------------------
// -----                        R3BDch source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include "R3BDch.h"

#include "R3BGeoDch.h"
#include "R3BDchPoint.h"
#include "R3BDchFullPoint.h"
#include "R3BGeoDchPar.h"

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

// includes for modeling
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoMatrix.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoBBox.h"
#include "TGeoPara.h"
#include "TGeoPgon.h"
#include "TGeoSphere.h"
#include "TGeoArb8.h"
#include "TGeoCone.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include "TRandom.h"

#include <iostream>
#include <stdlib.h> 
#include <iomanip>

using namespace std;
using std::cout;
using std::cerr;
using std::endl;



// -----   Default constructor   -------------------------------------------
R3BDch::R3BDch() : R3BDetector("R3BDch", kTRUE, kDCH) {
  ResetParameters();
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  kHelium = kFALSE;
  fDynamicStepSize=kFALSE;
  fVerbose=kFALSE;
  refMatrix = NULL;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BDch::R3BDch(const char* name, Bool_t active) 
  : R3BDetector(name, active, kDCH) {
  ResetParameters();
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  kHelium = kFALSE;
  fDynamicStepSize = kFALSE;
  fVerbose=kFALSE;
  refMatrix=NULL;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BDch::~R3BDch() {

  if ( flGeoPar ) delete flGeoPar;
  if (fDchCollection) {
    fDchCollection->Delete();
    delete fDchCollection;
  }
}
// -------------------------------------------------------------------------

void R3BDch::Initialize()
{
   if (fDynamicStepSize)
   fDchCollection = new TClonesArray("R3BDchFullPoint");
   else
   fDchCollection = new TClonesArray("R3BDchPoint");


   FairDetector::Initialize();

   cout << endl;
   cout << "-I- R3BDch: initialisation" << endl;
   cout << "-I- R3BDch: Gas. Vol: (McId) " << gMC->VolId("GASBoxLog")<< endl;

   // get the transformation matrixes for
   // the sensitive nodes.
  TGeoVolume *vol = gGeoManager->GetVolume("ALBoxLog");
  vol->SetVisibility(kTRUE);
  vol->SetVisContainers();
  gGeoManager->GetTopVolume()->SetVisContainers();


}

void R3BDch::FindNodePath(TObjArray * arr) {
 TString sVol = "GASBoxLog";
 TString topName = gGeoManager->GetTopNode()->GetName();
 TString path = "/" + topName;

  for (Int_t i=0;i<arr->GetEntries();i++ ){
      TGeoNode *aNode = (TGeoNode*) arr->At(i);
      TString nodName = aNode->GetName();
      if ( nodName.Contains("DCH1") || nodName.Contains(sVol.Data())) {
	  path+="/"+nodName;
	  cout << " -I intermediate path: " << path << endl;
      }
      FindNodePath(aNode->GetNodes());
  }

  cout << "-I- found path " << path << endl;

}

void R3BDch::SetSpecialPhysicsCuts(){

   cout << endl;
   cout << "-I- R3Dch: Adding customized Physics cut: " << endl;
   cout << "-I- R3Dch: not yet implemented ... " << endl;
   cout << endl;


   cout << endl;

   cout << "-I- R3Dch: Adding customized Physics cut ... " << endl;

   if (gGeoManager) {
     TGeoMedium* pSi = gGeoManager->GetMedium("mixtureForDCH");
     if ( pSi ) {
	 // Setting processes for Gas mixture only
         // ELoss + deltas + reduced fluctuation
	 gMC->Gstpar(pSi->GetId()  ,"LOSS",3);
         // collision sampling on PAI Model ( thin layer )
	 gMC->Gstpar(pSi->GetId()  ,"STRA",1.0);
         // Deltas on
	 gMC->Gstpar(pSi->GetId()  ,"DRAY",1.0);
         // Default processes
         gMC->Gstpar(pSi->GetId()  ,"PAIR",1.0);
	 gMC->Gstpar(pSi->GetId()  ,"COMP",1.0);
	 gMC->Gstpar(pSi->GetId()  ,"PHOT",1.0);
         gMC->Gstpar(pSi->GetId()  ,"ANNI",1.0);
	 gMC->Gstpar(pSi->GetId()  ,"BREM",1.0);
	 gMC->Gstpar(pSi->GetId()  ,"HADR",1.0);
         gMC->Gstpar(pSi->GetId()  ,"DCAY",1.0);
         gMC->Gstpar(pSi->GetId()  ,"MULS",1.0);
	 gMC->Gstpar(pSi->GetId()  ,"RAYL",1.0);

	 // Setting Energy-CutOff for Drift chamber Gas Only
	Double_t cutE = fCutE; // GeV-> 1 keV

	cout << "-I- R3Dch Gas Mixture Medium Id " << pSi->GetId()
	    << " Energy Cut-Off : " << cutE
	    << endl;
        cout << endl;
        //Si
	gMC->Gstpar(pSi->GetId(),"CUTGAM",cutE);   /** gammas (GeV)*/
        gMC->Gstpar(pSi->GetId(),"CUTELE",cutE);   /** electrons (GeV)*/
	gMC->Gstpar(pSi->GetId(),"CUTNEU",cutE);   /** neutral hadrons (GeV)*/
	gMC->Gstpar(pSi->GetId(),"CUTHAD",cutE);   /** charged hadrons (GeV)*/
	gMC->Gstpar(pSi->GetId(),"CUTMUO",cutE);   /** muons (GeV)*/
	gMC->Gstpar(pSi->GetId(),"BCUTE",cutE);    /** electron bremsstrahlung (GeV)*/
	gMC->Gstpar(pSi->GetId(),"BCUTM",cutE);    /** muon and hadron bremsstrahlung(GeV)*/
	gMC->Gstpar(pSi->GetId(),"DCUTE",cutE);    /** delta-rays by electrons (GeV)*/
	gMC->Gstpar(pSi->GetId(),"DCUTM",cutE);    /** delta-rays by muons (GeV)*/
        gMC->Gstpar(pSi->GetId(),"PPCUTM",-1.);   /** direct pair production by muons (GeV)*/

     }

 } //!gGeoManager

}


// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BDch::ProcessHits(FairVolume* vol) {


   if (fVerbose ) PrintInfo();

   if (fDynamicStepSize) RecordFullMcHit();
   else RecordPartialMcHit();

   return kTRUE;

}

void R3BDch::RecordFullMcHit(){

 // Record the full McHit history using
 // a dynamical update of the step size
 // at each Geant steps
 // Better estimation for Gaseous thin
 // layer

    // DCH Geometry Hiearchy
     Int_t medId = gMC->CurrentMedium();
     Int_t copyNo    = -1;
     Int_t ModuleNr0 = -1;
     Int_t ModuleNr1 = -1;
     Int_t ModuleNr2 = -1;
     Int_t ModuleNr3 = -1;
    // Get the Geometry info from MC Point
     gMC->CurrentVolID(copyNo);
     gMC->CurrentVolOffID(0,ModuleNr0);
     gMC->CurrentVolOffID(1,ModuleNr1);
     gMC->CurrentVolOffID(2,ModuleNr2); //! Module No
     gMC->CurrentVolOffID(3,ModuleNr3);

    // Detector plane
     Int_t mod = ModuleNr2;
    // MC Track Info
     Int_t trackId = gMC->GetStack()->GetCurrentTrackNumber();
     Double_t time   = gMC->TrackTime() * 1.0e09;
     Double_t length = gMC->TrackStep();
     TLorentzVector pos1;
     gMC->TrackPosition(pos1);
     TLorentzVector mom1;
     gMC->TrackMomentum(mom1);
     TVector3 pos(pos1.X(),pos1.Y(),pos1.Z());
     TVector3 mom(mom1.X(),mom1.Y(),mom1.Z());
    // Mc Eloss
     Double_t  eLoss   = gMC->Edep();
    // Local Coordinates
     Double_t MasterPoint[]={pos.X(),pos.Y(),pos.Z()};
     Double_t MasterMom[]={mom.X(),mom.Y(),mom.Z()};
     Double_t localPoint[3]  =  {0.,0.,0.};
     Double_t localMom[3]    =  {0.,0.,0.};

     gGeoManager->FindNode(MasterPoint[0],MasterPoint[1],MasterPoint[2]);
     gGeoManager->GetCurrentNavigator()->MasterToLocal(MasterPoint,localPoint);
     gGeoManager->GetCurrentNavigator()->MasterToLocal(MasterMom,localMom);
     TVector3 lpos(localPoint[0],localPoint[1],localPoint[2]);
     TVector3 lmom(localMom[0],localMom[1],localMom[2]);
     // Layer & cell
     Int_t layer =  -1;
     Int_t cell  =  -1; // not filled for now !
     layer = (localPoint[2] > 0. ) ? 1:0;

   // Add hit if Energy deposited > 0
     if (eLoss>0.) {

	 AddFullHit(trackId, mod, layer, cell, pos, lpos,  mom, lmom, time, length, eLoss) ;

      // Increment number of DCH Points for this track
      FairStack* stack = (FairStack*) gMC->GetStack();
      stack->AddPoint(kDCH);

      ResetParameters();

     } //! eLoss

   // Set maximum MC step size allowed
    SetStepToNextCollision();
}


void R3BDch::RecordPartialMcHit(){

    // DCH Geometry Hiearchy
     Int_t medId = gMC->CurrentMedium();
     Int_t copyNo    = -1;
     Int_t ModuleNr0 = -1;
     Int_t ModuleNr1 = -1;
     Int_t ModuleNr2 = -1;
     Int_t ModuleNr3 = -1;
    // Get the Geometry info from MC Point
     gMC->CurrentVolID(copyNo);
     gMC->CurrentVolOffID(0,ModuleNr0);
     gMC->CurrentVolOffID(1,ModuleNr1);
     gMC->CurrentVolOffID(2,ModuleNr2); //! Module No
     gMC->CurrentVolOffID(3,ModuleNr3);

    // Detector plane
     Int_t mod = ModuleNr2;

    // Standard registration
     if ( gMC->IsTrackEntering() ) {
	 fELoss  = 0.;
	 fTime   = gMC->TrackTime() * 1.0e09;
	 fLength = gMC->TrackLength();
	 gMC->TrackPosition(fPosIn);
	 gMC->TrackMomentum(fMomIn);
         refMatrix=gGeoManager->GetCurrentMatrix();
     }

  // Sum energy loss for all steps in the active volume
     fELoss += gMC->Edep();

 
  // Set additional parameters at exit of active volume. Create R3BmTofPoint.
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
      fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
      gMC->TrackPosition(fPosOut);
      gMC->TrackMomentum(fMomOut);
      if (fELoss == 0. ) return ;

      if (gMC->IsTrackExiting()) {
	  const Double_t* oldpos;
	  const Double_t* olddirection;
	  Double_t newpos[3];
	  Double_t newdirection[3];
	  Double_t safety;

	  gGeoManager->FindNode(fPosOut.X(),fPosOut.Y(),fPosOut.Z());
	  oldpos = gGeoManager->GetCurrentPoint();
	  olddirection = gGeoManager->GetCurrentDirection();

	  //       cout << "1st direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2] << endl;

	  for (Int_t i=0; i<3; i++){
	      newdirection[i] = -1*olddirection[i];
	  }

	  gGeoManager->SetCurrentDirection(newdirection);
	  //   TGeoNode *bla = gGeoManager->FindNextBoundary(2);
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

      }//! track exiting

     // Local Coordinates In
     Double_t MasterPointIn[]={fPosIn.X(),fPosIn.Y(),fPosIn.Z()};
     Double_t MasterMomIn[]={fMomIn.X(),fMomIn.Y(),fMomIn.Z()};
     Double_t LocalPointIn[3]  =  {0.,0.,0.};
     Double_t LocalMomIn[3]    =  {0.,0.,0.};
     refMatrix->MasterToLocal(MasterPointIn,LocalPointIn);
     refMatrix->MasterToLocal(MasterMomIn,LocalMomIn);
     // Local Coordinates Out
     Double_t MasterPointOut[]={fPosOut.X(),fPosOut.Y(),fPosOut.Z()};
     Double_t MasterMomOut[]={fMomOut.X(),fMomOut.Y(),fMomOut.Z()};
     Double_t LocalPointOut[3]  =  {0.,0.,0.};
     Double_t LocalMomOut[3]    =  {0.,0.,0.};
     refMatrix->MasterToLocal(MasterPointOut,LocalPointOut);
     refMatrix->MasterToLocal(MasterMomOut,LocalMomOut);


     Int_t layer =  -1;
     Int_t cell  =  -1; // not filled for now !
     // layer has no meaning here and will
     // be always set to 1
     layer = (LocalPointOut[2] > 0. ) ? 1:0;

      AddHit(fTrackID, mod, layer ,cell,
	     TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	     TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	     TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	     TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
             TVector3(LocalPointIn[0],LocalPointIn[1],LocalPointIn[2]),
	     TVector3(LocalMomIn[0] ,LocalMomIn[1], LocalMomIn[2]),
             TVector3(LocalPointOut[0],LocalPointOut[1],LocalPointOut[2]),
	     TVector3(LocalMomOut[0] ,LocalMomOut[1], LocalMomOut[2]),
           fTime, fLength, fELoss);

      // Increment number of DCH Points for this track
      FairStack* stack = (FairStack*) gMC->GetStack();
      stack->AddPoint(kDCH);

      ResetParameters();

  }

}


void R3BDch::PrintInfo(){

    // dump of Hit Information
    cout << "X(cm)    " << "Y(cm)    " << "Z(cm)  " << "KinE(MeV)   " << "dE(MeV) " << "Step(cm) " << "TrackL(cm) "
       << "Volume  " << "Process " << endl;

    // Position
    Double_t x, y, z;
    gMC->TrackPosition(x, y, z);
    cout << setw(8) << setprecision(3) << x << " "
         << setw(8) << setprecision(3) << y << " "
         << setw(8) << setprecision(3) << z << "  ";

    // Kinetic energy
    Double_t px, py, pz, etot;
    gMC->TrackMomentum(px, py, pz, etot);
    Double_t ekin = etot - gMC->TrackMass();
    cout << setw(9) << setprecision(4) << ekin*1e03 << " ";

    // Energy deposit
    cout << setw(9) << setprecision(4) << gMC->Edep()*1e03 << " ";
    // Step length
    cout << setw(8) << setprecision(3) << gMC->TrackStep() << " ";

    // Track length
    cout << setw(8) << setprecision(3) << gMC->TrackLength() << "     ";

    // Volume
    if (gMC->CurrentVolName() != 0)
      cout << setw(4) << gMC->CurrentVolName() << "  ";
    else
      cout << setw(4) << "None"  << "  ";

    // Process involved
    TArrayI processes;
    Int_t nofProcesses = gMC->StepProcesses(processes);

    for(int ip=0;ip<nofProcesses; ip++)
      cout << TMCProcessName[processes[ip]]<<" / ";

    cout << endl;

}




void R3BDch::SetStepToNextCollision()
{
  //  <DB>
  //  Weighted average calculation of the
  //  number of primary collisions per 1 cm for MIPs
  // in case of LAND DCH gas mixture
  //
  //           80% Ar + 20% CO2

  const Float_t prim = 25.5;

  Double_t charge= gMC->TrackCharge();
  Float_t pp;

  TLorentzVector mom;
  gMC->TrackMomentum(mom);
  Float_t ptot=mom.Rho();
  if (ptot<=1e-12) return;
  Float_t beta_gamma = ptot/gMC->TrackMass();
  

  // Select Particle types from Pids
  // electron(3), positron(2), photon(1)
  // to optimize the BBloch calculation

  if(gMC->IdFromPDG(gMC->TrackPid()) <= 3 && ptot > 0.02)
  {
      // electrons above 20 MeV/c reach the plateau!
      pp = prim*1.58;
    }
  else
    {
      //assuming form of Bethe Bloch depends only on mean free path
      pp=prim*BetheBloch(beta_gamma);
      if(TMath::Abs(charge) > 1.) pp *= (charge*charge);
    }

  // Mean free path sampling
  TRandom * rGenerator=gMC->GetRandom();
  Double_t rnd=rGenerator->Rndm();
  
  //get random free mean path from poisson statistics and mean pp
  gMC->SetMaxStep(-TMath::Log(rnd)/pp); 
                                        
}

 Double_t R3BDch::BetheBloch(Double_t bg)
{
  // <DB>
  // Parametrisation of the
  // Bethe-Bloch energy loss formula
  // taken from  Lehrhaus NIM 217 (1983) 43.


  // This parameters have been adjusted
  // to averaged values from GEANT
  const Double_t kP1 = 7.17960e-02;
  const Double_t kP2 = 8.54196;
  const Double_t kP3 = 1.38065e-06;
  const Double_t kP4 = 5.30972;
  const Double_t kP5 = 2.83798;


  // Lower cutoff of the Bethe-Bloch-curve to limit step sizes
  const Double_t kBgMin = 0.8;
  const Double_t kBBMax = 6.83298;

  if (bg > kBgMin) {
    Double_t yy = bg / TMath::Sqrt(1. + bg*bg);
    Double_t aa = TMath::Power(yy,kP4);
    Double_t bb = TMath::Power((1./bg),kP5);
             bb = TMath::Log(kP3 + bb);
    return ((kP2 - aa - bb)*kP1 / aa);
  }else {
    return kBBMax;
  }

}



// ----------------------------------------------------------------------------
//void R3BDch::SaveGeoParams(){
//
//  cout << " -I Save STS geo params " << endl;
//
//  TFolder *mf = (TFolder*) gDirectory->FindObjectAny("cbmroot");
//  cout << " mf: " << mf << endl;
//  TFolder *stsf = NULL;
//  if (mf ) stsf = (TFolder*) mf->FindObjectAny(GetName());
//  cout << " stsf: " << stsf << endl;
//  if (stsf) stsf->Add( flGeoPar0 ) ;
 //  FairRootManager::Instance()->WriteFolder();
//  mf->Write("cbmroot",TObject::kWriteDelete);
//}


// -----   Public method EndOfEvent   -----------------------------------------
void R3BDch::BeginEvent() {

//  if (! kGeoSaved ) {
//      SaveGeoParams();
//  cout << "-I STS geometry parameters saved " << endl;
//  kGeoSaved = kTRUE;
//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BDch::EndOfEvent() {

  if (fVerboseLevel) Print();
  fDchCollection->Clear();

  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void R3BDch::Register() {
  if (fDynamicStepSize) FairRootManager::Instance()->Register("DCHFullPoint", GetName(), fDchCollection, kTRUE);
  else FairRootManager::Instance()->Register("DCHPoint", GetName(), fDchCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BDch::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fDchCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BDch::Print() const {
  Int_t nHits = fDchCollection->GetEntriesFast();
  cout << "-I- R3BDch: " << nHits << " points registered in this event." 
       << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BDch::Reset() {
  fDchCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BDch::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- R3BDch: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;

  if (fDynamicStepSize) {
      R3BDchFullPoint* oldpoint = NULL;
      for (Int_t i=0; i<nEntries; i++) {
	  oldpoint = (R3BDchFullPoint*) cl1->At(i);
	  Int_t index = oldpoint->GetTrackID() + offset;
	  oldpoint->SetTrackID(index);
	  new (clref[fPosIndex]) R3BDchFullPoint(*oldpoint);
	  fPosIndex++;
      }

  }else{

      R3BDchPoint* oldpoint = NULL;
      for (Int_t i=0; i<nEntries; i++) {
	  oldpoint = (R3BDchPoint*) cl1->At(i);
	  Int_t index = oldpoint->GetTrackID() + offset;
	  oldpoint->SetTrackID(index);
	  new (clref[fPosIndex]) R3BDchPoint(*oldpoint);
	  fPosIndex++;
      }
  }
      cout << " -I- R3BDch: " << cl2->GetEntriesFast() << " merged entries."
	  << endl;
  }

// -----   Private method AddHit   --------------------------------------------
R3BDchFullPoint* R3BDch::AddFullHit(Int_t trackId, Int_t mod, Int_t layer, Int_t cell, TVector3 pos,
			        TVector3 lpos, TVector3 mom,
			        TVector3 lmom, Double_t time,
			        Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fDchCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BDchFullPoint(trackId, mod, layer, cell, pos, lpos,
				      mom, lmom, time, length, eLoss);
}

R3BDchPoint* R3BDch::AddHit(Int_t trackId, Int_t mod, Int_t layer, Int_t cell, TVector3 posIn,
			       TVector3 posOut, TVector3 momIn,
			       TVector3 momOut, TVector3 lpos1, TVector3 lmom1,
			       TVector3 lpos2, TVector3 lmom2, Double_t time,
			       Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fDchCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BDchPoint(trackId, mod, layer, cell, posIn, posOut,
				      momIn, momOut, lpos1, lmom1, lpos2, lmom2,
				      time, length, eLoss);
}
// -----   Public method ConstructGeometry   ----------------------------------
void R3BDch::ConstructGeometry() {
  return ConstructGeometryJustyna();
}

void R3BDch::ConstructGeometryJustyna() {
 // out-of-file geometry definition
   Double_t dx,dy,dz;
   Double_t theta, phi;
   Double_t a;
   Double_t thx, phx, thy, phy, thz, phz;
   Double_t alpha;
   Double_t z, density, radl, absl, w;
   Int_t nel, numed;

   Int_t matIndex = gGeoManager->GetListOfMaterials()->GetEntries();
   cout << " Matindex : " << matIndex<< endl;

   //-------------    Material definition
   // Mixture: Air
   TGeoMedium * pMed2=NULL;
   if (gGeoManager->GetMedium("Air") ){
       pMed2=gGeoManager->GetMedium("Air");
   }else{
    nel     = 2;
    density = 0.001290;
    TGeoMixture*
	pMat2 = new TGeoMixture("Air", nel,density);
    a = 14.006740;   z = 7.000000;   w = 0.700000;  // N
    pMat2->DefineElement(0,a,z,w);
    a = 15.999400;   z = 8.000000;   w = 0.300000;  // O
    pMat2->DefineElement(1,a,z,w);
    matIndex++;
    pMat2->SetIndex(matIndex);
    // Medium: Air
    numed   = matIndex;  // medium number
  
    pMed2 = new TGeoMedium("Air", numed,pMat2);
   }

   // Material: Aluminum
   TGeoMedium * pMed21=NULL;
   if (gGeoManager->GetMedium("Aluminum") ){
       pMed21=gGeoManager->GetMedium("Aluminum");
   }else{
      a       = 26.980000;
      z       = 13.000000;
      density = 2.700000;
      matIndex++;
      numed=matIndex;
      TGeoMaterial *matAl
	   = new TGeoMaterial("Aluminum", a,z,density);
      pMed21 = new TGeoMedium("Aluminum",numed, matAl);
   }




   // Mixture: Mylar
   TGeoMedium * pMed15=NULL;
   if (gGeoManager->GetMedium("Mylar") ){
       pMed15=gGeoManager->GetMedium("Mylar");
   }else{
       nel     = 3;
       density = 1.397000;
       TGeoMixture*
	   pMat15 = new TGeoMixture("Mylar", nel,density);
       a = 12.010700;   z = 6.000000;   w = 0.625010;  // C
       pMat15->DefineElement(0,a,z,w);
       a = 1.007940;   z = 1.000000;   w = 0.041961;  // H
       pMat15->DefineElement(1,a,z,w);
       a = 15.999400;   z = 8.000000;   w = 0.333029;  // O
       pMat15->DefineElement(2,a,z,w);
       matIndex++;
       pMat15->SetIndex(matIndex);
       // Medium: Mylar
       numed   = matIndex;  // medium number
    pMed15 = new TGeoMedium("mylar", numed,pMat15);
   }



   // Material: HeliumGas
   TGeoMedium * pMed4=NULL;
   if (gGeoManager->GetMedium("HeliumGas") ){
       pMed4=gGeoManager->GetMedium("HeliumGas");
   }else{
       a       = 4.000000;
       z       = 2.000000;
       density = 0.000125;
       TGeoMaterial*
	   pMat4 = new TGeoMaterial("HeliumGas", a,z,density);
       matIndex++;
       pMat4->SetIndex(matIndex);
       // Medium: HeliumGas
       numed   = matIndex;  // medium number

     pMed4 = new TGeoMedium("HeliumGas", numed,pMat4);
   }

  // DCH Gas definition
  Float_t aP[3]={39.948,12.0107,15.9994};
  Float_t zP[3]={18.,6.,8.};
  Float_t wP[3]={0.8, 0.054582,0.145418} ;
  Float_t dP = 0.001017 ;
  Int_t   nP = 3;
  //sumWeight = 0;
  //for (i=0; i<nP; i++) sumWeight += aP[i]*wP[i];
  //for (i=0; i<nP; i++) wP[i] *= aP[i]/sumWeight;
  matIndex++;
  TGeoMaterial* pMat33 = gGeoManager->Mixture("DCHGas",aP,zP,dP,nP,wP,matIndex);
  TGeoMedium* pMed33 = new TGeoMedium("DCHGas",matIndex,pMat33);




   // Mixture: mixtureForDCH
   /*
   TGeoMedium * pMed33=NULL;
   if (gGeoManager->GetMedium("mixtureForDCH") ){
       pMed33=gGeoManager->GetMedium("mixtureForDCH");
   }else{
      nel     = 3;
      density = 0.001017;
      TGeoMixture*
	  pMat33 = new TGeoMixture("mixtureForDCH", nel,density);
      a = 39.948000;   z = 18.000000;   w = 0.800000;  // AR
      pMat33->DefineElement(0,a,z,w);
      a = 12.010700;   z = 6.000000;   w = 0.054582;  // C
      pMat33->DefineElement(1,a,z,w);
      a = 15.999400;   z = 8.000000;   w = 0.145418;  // O
      pMat33->DefineElement(2,a,z,w);
      matIndex++;
      pMat33->SetIndex(600);
      // Medium: mixtureForDCH
      numed   = 602;  // medium number
  
      pMed33 = new TGeoMedium("mixtureForDCH", numed,pMat33);
   }
   */



   //Get Top Volume
   TGeoVolume* vWorld = gGeoManager->GetTopVolume();
   vWorld->SetVisLeaves(kTRUE);
   // Define DCH Geometry

   // Gas box
   Double_t gasDx = 106.4/2.; // [cm]
   Double_t gasDy = 83.4/2.;  // [cm]
   Double_t gasDz = 4.06;     // [cm]

   TGeoShape* pGasBox = new TGeoBBox("GasBox",
				     gasDx,
				     gasDy,
				     gasDz);
   TGeoVolume*
       pGasDchLog = new TGeoVolume("GASBoxLog",pGasBox, pMed33);
   pGasDchLog->SetVisLeaves(kTRUE);
   pGasDchLog->SetVisContainers(kTRUE);


   // Al Frame
   Double_t alDx = 125.8/2.; // [cm]
   Double_t alDy = 103.4/2.; // [cm]
   Double_t alDz = 4.06;     // [cm]

   TGeoShape* pAlBox = new TGeoBBox("AlBox",
					 alDx,
					 alDy,
					 alDz);

   TGeoVolume*
   pAlDchLog = new TGeoVolume("ALBoxLog",pAlBox, pMed21);
   pAlDchLog->SetVisLeaves(kTRUE);


   // Add gas layer as sensitive
   AddSensitiveVolume(pGasDchLog);

   // Mylar Entrance exit windows
   Double_t mylDx= gasDx; //[cm]
   Double_t mylDy= gasDy; //[cm]
   Double_t mylDz= 0.0006; //[cm]
   TGeoShape* pMylarBox = new TGeoBBox("MylarBox",
					 mylDx,
					 mylDy,
					 mylDz);

   TGeoVolume*
   pMylDchLog = new TGeoVolume("MYLBoxLog",pMylarBox, pMed15);
   pMylDchLog->SetVisLeaves(kTRUE);



   // First assembly
   TGeoVolume *dch1 = new TGeoVolumeAssembly("DCH1");

   TGeoRotation *rot = new TGeoRotation();
   rot->RotateX(0.);
   rot->RotateY(0.);
   rot->RotateZ(0.);
   Double_t tx = -3.5;
   Double_t ty = -5.;
   Double_t tz = 0.;

   TGeoCombiTrans*
   pTransfo1 = new TGeoCombiTrans("", 0.,0.,0.,rot);
   TGeoCombiTrans*
   pTransfo2 = new TGeoCombiTrans("", tx,ty,tz,rot);

   dch1->AddNode(pAlDchLog,0,pTransfo1);
   pAlDchLog->AddNode(pGasDchLog,0,pTransfo2);
   // Mylar Windows front+back
   dch1->AddNode(pMylDchLog,0,new TGeoCombiTrans("", tx,ty,-alDz-mylDz,rot));
   dch1->AddNode(pMylDchLog,1,new TGeoCombiTrans("", tx,ty, alDz+mylDz,rot));



   // Global Positioning
   Double_t pDch1x = -123.22 ; 
   Double_t pDch1y = 3.6 ; 
   Double_t pDch1z = 444.13 ; 

   Double_t pDch2x = -167.0 ;
   Double_t pDch2y = 1.02 ; 
   Double_t pDch2z = 535.1 ; 
   
   Double_t aDch1 = -31.0 ; 
   Double_t aDch2 = -31.0 ; 

   TGeoRotation *gRot1 = new TGeoRotation();
   gRot1->RotateX(0.);
   gRot1->RotateY(aDch1);
   gRot1->RotateZ(8.880000);
   
   TGeoRotation *gRot2 = new TGeoRotation();
   gRot2->RotateX(0.);
   gRot2->RotateY(aDch2);
   gRot2->RotateZ(-9.350000);



   // Helium Bag definition
   Double_t heDx= alDx ; //[cm]
   Double_t heDy= alDy ; //[cm]
   Double_t heDz=(pDch2z-pDch1z)*0.953874/2.; //[cm]
   alpha=0.;     //[degre]
   Double_t beta =0.;     //[degre]
   phi  =15.20; //[degre]

   TGeoShape* pHePara = new TGeoPara("HePara", heDx, heDy, heDz,
				     alpha,beta,phi);

   TGeoVolume*
   pHeDchLog = new TGeoVolume("HeParaLog",pHePara, pMed4);
   pHeDchLog->SetVisLeaves(kTRUE);




   vWorld->AddNode(dch1,0,new TGeoCombiTrans("",pDch1x,pDch1y,pDch1z,gRot1) );
   vWorld->AddNode(dch1,1,new TGeoCombiTrans("",pDch2x,pDch2y,pDch2z,gRot2) );

   if(kHelium)
   vWorld->AddNode(pHeDchLog,0,new TGeoCombiTrans("",(pDch1x+pDch2x)/2.,
						      pDch2y,
						      (pDch1z+pDch2z)/2.,
						      gRot1) );

  dch1->SetVisContainers(kTRUE);
}

void R3BDch::ConstructGeometry2() {
 // out-of-file geometry definition
   Double_t dx,dy,dz;
   Double_t theta, phi;
   Double_t a;
   Double_t thx, phx, thy, phy, thz, phz;
   Double_t alpha;
   Double_t z, density, radl, absl, w;
   Int_t nel, numed;

   Int_t matIndex = gGeoManager->GetListOfMaterials()->GetEntries();
   cout << " Matindex : " << matIndex<< endl;

   //-------------    Material definition
   // Mixture: Air
   TGeoMedium * pMed2=NULL;
   if (gGeoManager->GetMedium("Air") ){
       pMed2=gGeoManager->GetMedium("Air");
   }else{
    nel     = 2;
    density = 0.001290;
    TGeoMixture*
	pMat2 = new TGeoMixture("Air", nel,density);
    a = 14.006740;   z = 7.000000;   w = 0.700000;  // N
    pMat2->DefineElement(0,a,z,w);
    a = 15.999400;   z = 8.000000;   w = 0.300000;  // O
    pMat2->DefineElement(1,a,z,w);
    matIndex++;
    pMat2->SetIndex(matIndex);
    // Medium: Air
    numed   = matIndex;  // medium number
  
    pMed2 = new TGeoMedium("Air", numed,pMat2);
   }

   // Material: Aluminum
   TGeoMedium * pMed21=NULL;
   if (gGeoManager->GetMedium("Aluminum") ){
       pMed21=gGeoManager->GetMedium("Aluminum");
   }else{
      a       = 26.980000;
      z       = 13.000000;
      density = 2.700000;
      matIndex++;
      numed=matIndex;
      TGeoMaterial *matAl
	   = new TGeoMaterial("Aluminum", a,z,density);
      pMed21 = new TGeoMedium("Aluminum",numed, matAl);
   }




   // Mixture: Mylar
   TGeoMedium * pMed15=NULL;
   if (gGeoManager->GetMedium("Mylar") ){
       pMed15=gGeoManager->GetMedium("Mylar");
   }else{
       nel     = 3;
       density = 1.397000;
       TGeoMixture*
	   pMat15 = new TGeoMixture("Mylar", nel,density);
       a = 12.010700;   z = 6.000000;   w = 0.625010;  // C
       pMat15->DefineElement(0,a,z,w);
       a = 1.007940;   z = 1.000000;   w = 0.041961;  // H
       pMat15->DefineElement(1,a,z,w);
       a = 15.999400;   z = 8.000000;   w = 0.333029;  // O
       pMat15->DefineElement(2,a,z,w);
       matIndex++;
       pMat15->SetIndex(matIndex);
       // Medium: Mylar
       numed   = matIndex;  // medium number
    pMed15 = new TGeoMedium("mylar", numed,pMat15);
   }



   // Material: HeliumGas
   TGeoMedium * pMed4=NULL;
   if (gGeoManager->GetMedium("HeliumGas") ){
       pMed4=gGeoManager->GetMedium("HeliumGas");
   }else{
       a       = 4.000000;
       z       = 2.000000;
       density = 0.000125;
       TGeoMaterial*
	   pMat4 = new TGeoMaterial("HeliumGas", a,z,density);
       matIndex++;
       pMat4->SetIndex(matIndex);
       // Medium: HeliumGas
       numed   = matIndex;  // medium number

     pMed4 = new TGeoMedium("HeliumGas", numed,pMat4);
   }

  // DCH Gas definition
  Float_t aP[3]={39.948,12.0107,15.9994};
  Float_t zP[3]={18.,6.,8.};
  Float_t wP[3]={0.8, 0.054582,0.145418} ;
  Float_t dP = 0.001017 ;
  Int_t   nP = 3;
  //sumWeight = 0;
  //for (i=0; i<nP; i++) sumWeight += aP[i]*wP[i];
  //for (i=0; i<nP; i++) wP[i] *= aP[i]/sumWeight;
  matIndex++;
  TGeoMaterial* pMat33 = gGeoManager->Mixture("DCHGas",aP,zP,dP,nP,wP,matIndex);
  TGeoMedium* pMed33 = new TGeoMedium("DCHGas",matIndex,pMat33);




   // Mixture: mixtureForDCH
   /*
   TGeoMedium * pMed33=NULL;
   if (gGeoManager->GetMedium("mixtureForDCH") ){
       pMed33=gGeoManager->GetMedium("mixtureForDCH");
   }else{
      nel     = 3;
      density = 0.001017;
      TGeoMixture*
	  pMat33 = new TGeoMixture("mixtureForDCH", nel,density);
      a = 39.948000;   z = 18.000000;   w = 0.800000;  // AR
      pMat33->DefineElement(0,a,z,w);
      a = 12.010700;   z = 6.000000;   w = 0.054582;  // C
      pMat33->DefineElement(1,a,z,w);
      a = 15.999400;   z = 8.000000;   w = 0.145418;  // O
      pMat33->DefineElement(2,a,z,w);
      matIndex++;
      pMat33->SetIndex(600);
      // Medium: mixtureForDCH
      numed   = 602;  // medium number
  
      pMed33 = new TGeoMedium("mixtureForDCH", numed,pMat33);
   }
   */



   //Get Top Volume
   TGeoVolume* vWorld = gGeoManager->GetTopVolume();
   vWorld->SetVisLeaves(kTRUE);
   // Define DCH Geometry

   // Gas box
   Double_t gasDx = 106.4/2.; // [cm]
   Double_t gasDy = 83.4/2.;  // [cm]
   Double_t gasDz = 4.06;     // [cm]

   TGeoShape* pGasBox = new TGeoBBox("GasBox",
				     gasDx,
				     gasDy,
				     gasDz);
   TGeoVolume*
       pGasDchLog = new TGeoVolume("GASBoxLog",pGasBox, pMed33);
   pGasDchLog->SetVisLeaves(kTRUE);
   pGasDchLog->SetVisContainers(kTRUE);


   // Al Frame
   Double_t alDx = 125.8/2.; // [cm]
   Double_t alDy = 103.4/2.; // [cm]
   Double_t alDz = 4.06;     // [cm]

   TGeoShape* pAlBox = new TGeoBBox("AlBox",
					 alDx,
					 alDy,
					 alDz);

   TGeoVolume*
   pAlDchLog = new TGeoVolume("ALBoxLog",pAlBox, pMed21);
   pAlDchLog->SetVisLeaves(kTRUE);


   // Add gas layer as sensitive
   AddSensitiveVolume(pGasDchLog);

   // Mylar Entrance exit windows
   Double_t mylDx= gasDx; //[cm]
   Double_t mylDy= gasDy; //[cm]
   Double_t mylDz= 0.0006; //[cm]
   TGeoShape* pMylarBox = new TGeoBBox("MylarBox",
					 mylDx,
					 mylDy,
					 mylDz);

   TGeoVolume*
   pMylDchLog = new TGeoVolume("MYLBoxLog",pMylarBox, pMed15);
   pMylDchLog->SetVisLeaves(kTRUE);



   // First assembly
   TGeoVolume *dch1 = new TGeoVolumeAssembly("DCH1");

   TGeoRotation *rot = new TGeoRotation();
   rot->RotateX(0.);
   rot->RotateY(0.);
   rot->RotateZ(0.);
   Double_t tx = -3.5;
   Double_t ty = -5.;
   Double_t tz = 0.;

   TGeoCombiTrans*
   pTransfo1 = new TGeoCombiTrans("", 0.,0.,0.,rot);
   TGeoCombiTrans*
   pTransfo2 = new TGeoCombiTrans("", tx,ty,tz,rot);

   dch1->AddNode(pAlDchLog,0,pTransfo1);
   pAlDchLog->AddNode(pGasDchLog,0,pTransfo2);
   // Mylar Windows front+back
   dch1->AddNode(pMylDchLog,0,new TGeoCombiTrans("", tx,ty,-alDz-mylDz,rot));
   dch1->AddNode(pMylDchLog,1,new TGeoCombiTrans("", tx,ty, alDz+mylDz,rot));



   // Global Positioning
   Double_t pDch1x = 128.7 ; 
   Double_t pDch1y = 0.0 ; 
   Double_t pDch1z = 443.9 ;
    
   Double_t pDch2x = 169.1 ;
   Double_t pDch2y = 0.0 ; 
   Double_t pDch2z = 535.8 ; 
   
   Double_t aDch1 = 31.0 ; 
   Double_t aDch2 = 31.0 ; 

   TGeoRotation *gRot = new TGeoRotation();
   gRot->RotateX(0.);
   gRot->RotateY(aDch1);
   gRot->RotateZ(0.);



   // Helium Bag definition
   Double_t heDx= alDx ; //[cm]
   Double_t heDy= alDy ; //[cm]
   Double_t heDz=(pDch2z-pDch1z)*0.953874/2.; //[cm]
   alpha=0.;     //[degre]
   Double_t beta =0.;     //[degre]
   phi  =15.20; //[degre]

   TGeoShape* pHePara = new TGeoPara("HePara", heDx, heDy, heDz,
				     alpha,beta,phi);

   TGeoVolume*
   pHeDchLog = new TGeoVolume("HeParaLog",pHePara, pMed4);
   pHeDchLog->SetVisLeaves(kTRUE);




   vWorld->AddNode(dch1,0,new TGeoCombiTrans("",pDch1x,pDch1y,pDch1z,gRot) );
   vWorld->AddNode(dch1,1,new TGeoCombiTrans("",pDch2x,pDch2y,pDch2z,gRot) );

   if(kHelium)
   vWorld->AddNode(pHeDchLog,0,new TGeoCombiTrans("",(pDch1x+pDch2x)/2.,
						      pDch2y,
						      (pDch1z+pDch2z)/2.,
						      gRot) );

  dch1->SetVisContainers(kTRUE);
}


void R3BDch::ConstructGeometry1() {

  // out-of-file geometry definition
   Double_t dx,dy,dz;
   Double_t theta, phi;
   Double_t a;
   Double_t thx, phx, thy, phy, thz, phz;
   Double_t alpha;
   Double_t z, density, radl, absl, w;
   Int_t nel, numed;


/****************************************************************************/
// Material definition

   // Mixture: Air
   TGeoMedium * pMed2=NULL;
   if (gGeoManager->GetMedium("Air") ){
       pMed2=gGeoManager->GetMedium("Air");
   }else{
    nel     = 2;
    density = 0.001290;
    TGeoMixture*
	pMat2 = new TGeoMixture("Air", nel,density);
    a = 14.006740;   z = 7.000000;   w = 0.700000;  // N
    pMat2->DefineElement(0,a,z,w);
    a = 15.999400;   z = 8.000000;   w = 0.300000;  // O
    pMat2->DefineElement(1,a,z,w);
    pMat2->SetIndex(1);
    // Medium: Air
    numed   = 1;  // medium number
    Double_t par[8];
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 0.000000; // tmaxfd
    par[4]  = 0.000000; // stemax
    par[5]  = 0.000000; // deemax
    par[6]  = 0.000100; // epsil
    par[7]  = 0.000000; // stmin
    pMed2 = new TGeoMedium("Air", numed,pMat2, par);
   }

// Mixture: mixtureForDCH
   TGeoMedium * pMed33=NULL;
   if (gGeoManager->GetMedium("mixtureForDCH") ){
       pMed33=gGeoManager->GetMedium("mixtureForDCH");
   }else{
      nel     = 3;
      density = 0.001017;
      TGeoMixture*
	  pMat33 = new TGeoMixture("mixtureForDCH", nel,density);
      a = 39.948000;   z = 18.000000;   w = 0.800000;  // AR
      pMat33->DefineElement(0,a,z,w);
      a = 12.010700;   z = 6.000000;   w = 0.054582;  // C
      pMat33->DefineElement(1,a,z,w);
      a = 15.999400;   z = 8.000000;   w = 0.145418;  // O
      pMat33->DefineElement(2,a,z,w);
      pMat33->SetIndex(32);
      // Medium: mixtureForDCH
      numed   = 32;  // medium number
      Double_t par[8];
      par[0]  = 0.000000; // isvol
      par[1]  = 0.000000; // ifield
      par[2]  = 0.000000; // fieldm
      par[3]  = 0.000000; // tmaxfd
      par[4]  = 0.000000; // stemax
      par[5]  = 0.000000; // deemax
      par[6]  = 0.000100; // epsil
      par[7]  = 0.000000; // stmin
      pMed33 = new TGeoMedium("mixtureForDCH", numed,pMat33, par);
   }

   // Material: Aluminum
   TGeoMedium * pMed21=NULL;
   if (gGeoManager->GetMedium("Aluminum") ){
       pMed21=gGeoManager->GetMedium("Aluminum");
   }else{
      a       = 26.980000;
      z       = 13.000000;
      density = 2.700000;
      radl    = 8.875105;
      absl    = 388.793113;

      TGeoMaterial *matAl
	   = new TGeoMaterial("Aluminum", a,z,density,radl,absl);

      Double_t par[8];
      par[0]  = 0.000000; // isvol
      par[1]  = 0.000000; // ifield
      par[2]  = 0.000000; // fieldm
      par[3]  = 0.000000; // tmaxfd
      par[4]  = 0.000000; // stemax
      par[5]  = 0.000000; // deemax
      par[6]  = 0.000100; // epsil
      par[7]  = 0.000000; // stmin

      pMed21 = new TGeoMedium("Aluminum",3, matAl, par);
   }


// Mixture: Mylar
   TGeoMedium * pMed15=NULL;
   if (gGeoManager->GetMedium("Mylar") ){
       pMed15=gGeoManager->GetMedium("Mylar");
   }else{
    nel     = 3;
    density = 1.397000;
    TGeoMixture*
	pMat15 = new TGeoMixture("Mylar", nel,density);
    a = 12.010700;   z = 6.000000;   w = 0.625010;  // C
    pMat15->DefineElement(0,a,z,w);
    a = 1.007940;   z = 1.000000;   w = 0.041961;  // H
    pMat15->DefineElement(1,a,z,w);
    a = 15.999400;   z = 8.000000;   w = 0.333029;  // O
    pMat15->DefineElement(2,a,z,w);
    pMat15->SetIndex(14);
    // Medium: Mylar
    numed   = 14;  // medium number
    Double_t par[8];
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 0.000000; // tmaxfd
    par[4]  = 0.000000; // stemax
    par[5]  = 0.000000; // deemax
    par[6]  = 0.000100; // epsil
    par[7]  = 0.000000; // stmin
    pMed15 = new TGeoMedium("Mylar", numed,pMat15,par);
   }


// Material: HeliumGas
   TGeoMedium * pMed4=NULL;
   if (gGeoManager->GetMedium("HeliumGas") ){
       pMed4=gGeoManager->GetMedium("HeliumGas");
   }else{
     a       = 4.000000;
     z       = 2.000000;
     density = 0.000125;
     radl    = 683475.828563;
     absl    = 4444726.310227;
     TGeoMaterial*
	 pMat4 = new TGeoMaterial("HeliumGas", a,z,density,radl,absl);
     pMat4->SetIndex(50);
     // Medium: HeliumGas
     numed   = 50;  // medium number
     Double_t par[8];
     par[0]  = 0.000000; // isvol
     par[1]  = 0.000000; // ifield
     par[2]  = 0.000000; // fieldm
     par[3]  = 0.000000; // tmaxfd
     par[4]  = 0.000000; // stemax
     par[5]  = 0.000000; // deemax
     par[6]  = 0.000100; // epsil
     par[7]  = 0.000000; // stmin
     pMed4 = new TGeoMedium("HeliumGas", numed,pMat4, par);
   }

    // TRANSFORMATION MATRICES
   // Combi transformation: 
   dx = 128.700000;
   dy = 0.000000;
   dz = 443.900000;
   // Rotation: 
   thx = 121.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 31.000000;    phz = 0.000000;
   TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix9 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
    TGeoCombiTrans*
   pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);
   // Combi transformation: 
   dx = 0.000000;
   dy = 42.200000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
    TGeoCombiTrans*
   pMatrix10 = new TGeoCombiTrans("", dx,dy,dz,pMatrix11);
   // Combi transformation: 
   dx = 0.000000;
   dy = -42.200000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix13 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
    TGeoCombiTrans*
   pMatrix12 = new TGeoCombiTrans("", dx,dy,dz,pMatrix13);
   // Combi transformation: 
   dx = 53.400000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix15 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix14 = new TGeoCombiTrans("", dx,dy,dz,pMatrix15);
   // Combi transformation: 
   dx = -53.400000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix17 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix16 = new TGeoCombiTrans("", dx,dy,dz,pMatrix17);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 4.060600;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix19 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix18 = new TGeoCombiTrans("", dx,dy,dz,pMatrix19);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = -4.060600;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix21 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
    TGeoCombiTrans*
   pMatrix20 = new TGeoCombiTrans("", dx,dy,dz,pMatrix21);
   // Combi transformation: 
   dx = 169.100000;
   dy = 0.000000;
   dz = 535.800000;
   // Rotation: 
   thx = 121.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 31.000000;    phz = 0.000000;
   TGeoRotation *pMatrix5 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix4 = new TGeoCombiTrans("", dx,dy,dz,pMatrix5);
   // Combi transformation: 
   dx = 148.900000;
   dy = 0.000000;
   dz = 489.850000;
   // Rotation: 
   thx = 121.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 31.000000;    phz = 0.000000;
   TGeoRotation *pMatrix7 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix6 = new TGeoCombiTrans("", dx,dy,dz,pMatrix7);

   // SET TOP VOLUME OF GEOMETRY
   TGeoVolume * pWorld = gGeoManager->GetTopVolume();
   pWorld->SetVisLeaves(kTRUE);

   //Create a global Mother Volume
   /*
   dx = 100.000000;
   dy = 100.000000;
   dz = 100.000000;
   TGeoShape *pBoxWorld = new TGeoBBox("DCHBoxWorld", dx,dy,dz);
   TGeoVolume*
   pWorld  = new TGeoVolume("DCHBoxLogWorld",pBoxWorld, pMed2);
   pWorld->SetVisLeaves(kTRUE);
   TGeoCombiTrans * t0 = new TGeoCombiTrans();
   TGeoCombiTrans *pGlobalc = GetGlobalPosition(t0);
   // add the sphere as Mother Volume
   pAWorld->AddNodeOverlap(pWorld, 0, pGlobalc);
   */

   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: DCHBoxWorld type: TGeoBBox
   dx = 55.400000;
   dy = 44.200000;
   dz = 4.061200;
   TGeoShape *pDCHBoxWorld = new TGeoBBox("DCHBoxWorld", dx,dy,dz);
   // Volume: DCHLogWorld
   TGeoVolume*
   pDCHLogWorld = new TGeoVolume("DCHLogWorld",pDCHBoxWorld, pMed2);
   pDCHLogWorld->SetVisLeaves(kTRUE);


   pWorld->AddNode(pDCHLogWorld, 0, GetGlobalPosition(pMatrix2));
   pWorld->AddNode(pDCHLogWorld, 1, GetGlobalPosition(pMatrix4));

   // Shape: heliumBag type: TGeoPara
   dx    = 55.400000;
   dy    = 44.200000;
   dz    = 43.830510;
   alpha = 0.000000;
   theta = 0.000000;
   phi   = 0.000000;

   TGeoShape *pheliumBagv =
       new TGeoPara("heliumBag",dx,dy,dz,alpha,theta,phi);
   // Volume: heliumBag
   TGeoVolume*
   pheliumBag = new TGeoVolume("heliumBag",pheliumBagv, pMed4);
   pheliumBag->SetVisLeaves(kTRUE);

   if (kHelium == kTRUE){
   pWorld->AddNode(pheliumBag, 0, GetGlobalPosition(pMatrix6));
   }

   // Shape: DCHBox type: TGeoBBox
   dx = 51.400000;
   dy = 40.200000;
   dz = 4.060000;
   TGeoShape *pDCHBox = new TGeoBBox("DCHBox", dx,dy,dz);
   // Volume: DCHLog
   TGeoVolume*
   pDCHLog = new TGeoVolume("DCHLog",pDCHBox, pMed33);
   pDCHLog->SetVisLeaves(kTRUE);
   pDCHLogWorld->AddNode(pDCHLog, 0, pMatrix8);



   // Shape: UpFrame type: TGeoBBox
   dx = 55.400000;
   dy = 2.000000;
   dz = 4.061200;
   TGeoShape *pUpFrame = new TGeoBBox("UpFrame", dx,dy,dz);
   // Volume: logicUpFrame
   TGeoVolume*
   plogicUpFrame = new TGeoVolume("logicUpFrame",pUpFrame, pMed21);
   plogicUpFrame->SetVisLeaves(kTRUE);
   pDCHLogWorld->AddNode(plogicUpFrame, 0, pMatrix10);
   // Shape: DownFrame type: TGeoBBox
   dx = 55.400000;
   dy = 2.000000;
   dz = 4.061200;
   TGeoShape *pDownFrame = new TGeoBBox("DownFrame", dx,dy,dz);
   // Volume: logicDownFrame
   TGeoVolume*
   plogicDownFrame = new TGeoVolume("logicDownFrame",pDownFrame, pMed21);
   plogicDownFrame->SetVisLeaves(kTRUE);
   pDCHLogWorld->AddNode(plogicDownFrame, 0, pMatrix12);
   // Shape: RightFrame type: TGeoBBox
   dx = 2.000000;
   dy = 40.200000;
   dz = 4.060000;
   TGeoShape *pRightFrame = new TGeoBBox("RightFrame", dx,dy,dz);
   // Volume: logicRightFrame
   TGeoVolume*
   plogicRightFrame = new TGeoVolume("logicRightFrame",pRightFrame, pMed21);
   plogicRightFrame->SetVisLeaves(kTRUE);
   pDCHLogWorld->AddNode(plogicRightFrame, 0, pMatrix14);
   // Shape: LeftFrame type: TGeoBBox
   dx = 2.000000;
   dy = 40.200000;
   dz = 4.060000;
   TGeoShape *pLeftFrame = new TGeoBBox("LeftFrame", dx,dy,dz);
   // Volume: logicLeftFrame
   TGeoVolume*
   plogicLeftFrame = new TGeoVolume("logicLeftFrame",pLeftFrame, pMed21);
   plogicLeftFrame->SetVisLeaves(kTRUE);
   pDCHLogWorld->AddNode(plogicLeftFrame, 0, pMatrix16);
   // Shape: FrontFrame type: TGeoBBox
   dx = 51.400000;
   dy = 40.200000;
   dz = 0.000600;
   TGeoShape *pFrontFrame = new TGeoBBox("FrontFrame", dx,dy,dz);
   // Volume: logicFrontFrame
   TGeoVolume*
   plogicFrontFrame = new TGeoVolume("logicFrontFrame",pFrontFrame, pMed15);
   plogicFrontFrame->SetVisLeaves(kTRUE);
   pDCHLogWorld->AddNode(plogicFrontFrame, 0, pMatrix18);
   // Shape: BackFrame type: TGeoBBox
   dx = 51.400000;
   dy = 40.200000;
   dz = 0.000600;
   TGeoShape *pBackFrame = new TGeoBBox("BackFrame", dx,dy,dz);
   // Volume: logicBackFrame
   TGeoVolume*
   plogicBackFrame = new TGeoVolume("logicBackFrame",pBackFrame, pMed15);
   plogicBackFrame->SetVisLeaves(kTRUE);
   pDCHLogWorld->AddNode(plogicBackFrame, 0, pMatrix20);


   // add sensitive volume to DCH
   AddSensitiveVolume(pDCHLog);
   fNbOfSensitiveVol+=1;

}


/*
void R3BDch::ConstructGeometry() {
  
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  R3BGeoDch*       stsGeo  = new R3BGeoDch();
  stsGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(stsGeo);

  Bool_t rc = geoFace->readSet(stsGeo);
  if (rc) stsGeo->create(geoLoad->getGeoBuilder());
  TList* volList = stsGeo->getListOfVolumes();
  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  R3BGeoDchPar* par=(R3BGeoDchPar*)(rtdb->getContainer("R3BGeoDchPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray *fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode* node   = NULL;
  FairGeoVolume *aVol=NULL;

  while( (node = (FairGeoNode*)iter.Next()) ) {
      aVol = dynamic_cast<FairGeoVolume*> ( node );
       if ( node->isSensitive()  ) {
           fSensNodes->AddLast( aVol );
       }else{
           fPassNodes->AddLast( aVol );
       }
  }
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);
  ProcessNodes( volList );

}
*/

ClassImp(R3BDch)
