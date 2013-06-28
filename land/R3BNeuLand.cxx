// -------------------------------------------------------------------------
// -----                        R3BNeuLand source file                 -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include "R3BNeuLand.h"

#include "R3BGeoLand.h"
#include "R3BNeuLandPoint.h"
#include "R3BGeoLandPar.h"

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
#include "TGeoShapeAssembly.h"
#include <iostream>
#include <math.h>

#include "parameters.h"

using std::cout;
using std::cerr;
using std::endl;



// -----   Default constructor   -------------------------------------------
R3BNeuLand::R3BNeuLand() : R3BDetector("R3BNeuLand", kTRUE, kLAND) {
  ResetParameters();
  fLandCollection = new TClonesArray("R3BNeuLandPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 0;
  fVersion = 1;
  
  fIDMedGas = -1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BNeuLand::R3BNeuLand(const char* name, Bool_t active) 
  : R3BDetector(name, active, kLAND) {
  ResetParameters();
  fLandCollection = new TClonesArray("R3BNeuLandPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 0;
  fVersion = 1;
  
  fIDMedGas = -1;
}

R3BNeuLand::~R3BNeuLand() {
  
  if ( flGeoPar ) delete flGeoPar;
  if (fLandCollection) {
    fLandCollection->Delete();
    delete fLandCollection;
  }
}

void R3BNeuLand::Initialize()
{
  FairDetector::Initialize();
  
}


Bool_t R3BNeuLand::ProcessHits(FairVolume* vol) {
  
  Int_t iMed = gMC->CurrentMedium();
  volId1 =  gMC->CurrentVolID(cellNum);
  volId2 =  gMC->CurrentVolOffID(1,unitNum);

  Int_t UnitLayerNum = (Int_t) floor(unitNum/NofSegs);
  for(int j=0;j<2*NofCells;j++) {
    if(j == cellNum) {
      if(j<NofCells) {
	fCellID = j;
	fDetlayerid = 2*UnitLayerNum;
      }
      if(j>=NofCells) {
	fCellID = j - NofCells;
	fDetlayerid = 2*UnitLayerNum+1;
      }
    }
  }

  fSegID = (Int_t) unitNum%NofSegs;


  if (iMed == fIDMedGas) {
    if ( gMC->IsTrackEntering() ) {
      fELoss  = 0.;
      fTime   = gMC->TrackTime() * 1.0e09;
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPosIn);
      gMC->TrackMomentum(fMomIn);
    }
    
    fELoss += gMC->Edep() * 1E+6; // keV
    
    if(fELoss > 0.0){
      fEventID = gMC->CurrentEvent();
      fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
      fMot0TrackID  = gMC->GetStack()->GetCurrentTrack()->GetMother(0);
      fMass = gMC->ParticleMass(gMC->TrackPid()); // GeV/c2
      
      gMC->TrackPosition(fPosOut);
      gMC->TrackMomentum(fMomOut);

            
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
	//  TGeoNode *bla = gGeoManager->FindNextBoundary(2);
	safety = gGeoManager->GetSafeDistance();
	
	
	gGeoManager->SetCurrentDirection(-newdirection[0],-newdirection[1],-newdirection[2]);
	
	for (Int_t i=0; i<3; i++){
	  newpos[i] = oldpos[i] - (3*safety*olddirection[i]);
	}
	
	if ( fPosIn.Z() < 30. && newpos[2] > 30.02 ) {
	  cerr << "-I- R3BNeuLand : 2nd direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2] 
	       << " with safety = " << safety << endl;
	  cerr << "-I- R3BNeuLand oldpos = " << oldpos[0] << "," << oldpos[1] << "," << oldpos[2] << endl;
	  cerr << "-I- R3BNeuLand newpos = " << newpos[0] << "," << newpos[1] << "," << newpos[2] << endl;
	}
	
	fPosOut.SetX(newpos[0]);
	fPosOut.SetY(newpos[1]);
	fPosOut.SetZ(newpos[2]);
	
      } // IsTrackExiting
      
      /*      
      cout << " fEventID: " << fEventID
	   << " fTrackID: " << fTrackID
	   << " fMot0TrackID: " << fMot0TrackID
	   << " fDetlayerid: " << fDetlayerid
	   << " fSegID: " << fSegID
	   << " fCellID: " << fCellID
	   << " fMass: " << fMass
	   << " fTime: " << fTime
	   << " fELoss: " << fELoss
	   << endl;
      */
      if ( gMC->IsTrackExiting()    ||
	   gMC->IsTrackStop()       ||
	   gMC->IsTrackDisappeared()   ) {
	/*
	if(gMC->IsTrackExiting()) cout << " Track is exiting " << endl;
	if(gMC->IsTrackStop()) cout << " Track is stopped " << endl;
	if(gMC->IsTrackDisappeared()) cout << " Track is disappeared " << endl;
       	
	cout << " fEventID: " << fEventID
	     << " fTrackID: " << fTrackID
	     << " fMot0TrackID: " << fMot0TrackID
	     << " fDetlayerid: " << fDetlayerid
	     << " fSegID: " << fSegID
	     << " fCellID: " << fCellID
	     << " fMass: " << fMass
	     << " fTime: " << fTime
	     << " fELoss: " << fELoss
	     << endl;
	*/	

	AddHit(fEventID, fTrackID, fMot0TrackID,
	       fDetlayerid, fCellID, fSegID,
	       fMass,
	       TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	       TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	       TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	       TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	       fTime, fLength, fELoss);
	
      // Increment number of NeuLandPoints for this track
	FairStack* stack = (FairStack*) gMC->GetStack();
	stack->AddPoint(kLAND);
	
      } // IsTrackExiting, IsTrackStop, IsTrackDisappeared
      //ResetParameters();
    } // fELoss > 0
  } // ! iMed == Gas 
  return kTRUE;
}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BNeuLand::BeginEvent() {
  
  if (gGeoManager) {
    TGeoVolume * vol = gGeoManager->FindVolumeFast("Gas2Log");
  }
  
  fIDMedGas = pMedGas->GetId();

}


void R3BNeuLand::EndOfEvent() {
  
  if (fVerboseLevel) Print();
  fLandCollection->Clear();

  ResetParameters();
}


// -----   Public method Register   -------------------------------------------
void R3BNeuLand::Register() {
  FairRootManager::Instance()->Register("NeuLandPoint", GetName(), fLandCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BNeuLand::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fLandCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BNeuLand::Print() const {
  Int_t nHits = fLandCollection->GetEntriesFast();
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BNeuLand::Reset() {
  fLandCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BNeuLand::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- R3BNeuLand: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  R3BNeuLandPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (R3BNeuLandPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BNeuLandPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- R3BNeuLand: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BNeuLandPoint* R3BNeuLand::AddHit(Int_t eventID, Int_t trackID,
				    Int_t mot0trackID,
				    Int_t detlayerid, Int_t cellID, Int_t segID,
				    Double_t mass,
				    TVector3 posIn,
				    TVector3 posOut, TVector3 momIn,
				    TVector3 momOut, Double_t time,
				    Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fLandCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- R3BNeuLand: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  detector " << detlayerid << ", track "
	 << trackID << ", energy loss " << eLoss  << " GeV" << endl;
  return new(clref[size]) R3BNeuLandPoint(eventID, trackID, mot0trackID,
					  detlayerid, cellID, segID, mass,
					  posIn, posOut, momIn, momOut, time, length, eLoss);
}
// -----   Public method ConstructGeometry   ----------------------------------
void R3BNeuLand::ConstructGeometry() {

  if (1 == fVersion) return ConstructGeometry1();
  
  cout << "-I- R3BNeuLand  ConstructGeometry() :unknown Geometry version  !!! " << endl;
}

void R3BNeuLand::ConstructGeometry1() {
  Double_t a;
  Double_t z, density, w;
  Int_t nel, numed, aw;
  
  // -- Elements
  
  TGeoElement* elH = new TGeoElement("Hydrogen", "H", z= 1, a= 1.00794);
  TGeoElement* elC = new TGeoElement("Carbon", "C", z= 6, a= 12.0107);
  TGeoElement* elN = new TGeoElement("Nitrogen", "N", z= 7, a= 14.007);
  TGeoElement* elO = new TGeoElement("Oxygen", "O", z= 8, a= 15.9994);
  TGeoElement* elF = new TGeoElement("Fluorine", "F", z= 9, a= 18.9984032);
  TGeoElement* elNa = new TGeoElement("Sodium", "Na", z= 11, a= 22.98976928);
  TGeoElement* elMg = new TGeoElement("Magnesium", "Mg", z= 12, a= 24.3050);
  TGeoElement* elAl = new TGeoElement("Aluminum", "Al", z= 13, a= 26.9815);
  TGeoElement* elSi = new TGeoElement("Silicon", "Si", z= 14, a= 28.0855);
  TGeoElement* elS = new TGeoElement("Sulfur", "S", z= 16, a= 32.065);
  TGeoElement* elCa = new TGeoElement("Calcium", "Ca", z= 20, a= 40.078);
  TGeoElement* elCr = new TGeoElement("Chromium", "Cr", z= 24, a= 51.9961);
  TGeoElement* elFe = new TGeoElement("Iron", "Fe", z= 26, a= 55.845);
  TGeoElement* elNi = new TGeoElement("Nickel", "Ni", z= 28, a= 58.6934);
  TGeoElement* elPb = new TGeoElement("Lead", "Pb", z= 82, a= 207.2);

  
  // -- Materials
  
  // Air
  nel     = 2;
  density = 0.001290;
  TGeoMixture* pMatAir = new TGeoMixture("Air", nel,density);
  w = 0.700000;  // 70 % N
  pMatAir->AddElement(elN,w);
  w = 0.300000;  // 30 % O
  pMatAir->AddElement(elO,w);
  pMatAir->SetIndex(1);
  
  // Stainless steel
  nel     = 3;
  density = 7.9;
  TGeoMixture* pMatSteel = new TGeoMixture("Steel", nel,density);
  w = 0.710000;  // 71 % Fe
  pMatSteel->AddElement(elFe,w);
  w = 0.180000;  // 18 % Cr
  pMatSteel->AddElement(elCr,w);
  w = 0.110000;  // 11 % Ni
  pMatSteel->AddElement(elNi,w);
  pMatSteel->SetIndex(2);
  
  // SiO2
  nel     = 2;
  density = 2.634;
  TGeoMixture* pMatSiO2 = new TGeoMixture("SiO2", nel,density);
  aw = 1;  // 1 Si in chemical formula
  pMatSiO2->AddElement(elSi,aw);
  aw = 2;  // 2 O in chemical formula
  pMatSiO2->AddElement(elO,aw);
  pMatSiO2->SetIndex(3);
  
  // Na2O
  nel     = 2;
  density = 2.27;
  TGeoMixture* pMatNa2O = new TGeoMixture("Na2O", nel,density);
  aw = 2;  // 2 Na in chemical formula
  pMatNa2O->AddElement(elNa,aw);
  aw = 1;  // 1 O in chemical formula
  pMatNa2O->AddElement(elO,aw);
  pMatNa2O->SetIndex(4);
  
  // CaO
  nel     = 2;
  density = 3.35;
  TGeoMixture* pMatCaO = new TGeoMixture("CaO", nel,density);
  aw = 1;  // 1 Ca in chemical formula
  pMatCaO->AddElement(elCa,aw);
  aw = 1;  // 1 O in chemical formula
  pMatCaO->AddElement(elO,aw);
  pMatCaO->SetIndex(5);
  
  // MgO
  nel     = 2;
  density = 3.58;
  TGeoMixture* pMatMgO = new TGeoMixture("MgO", nel,density);
  aw = 1;  // 1 Mg in chemical formula
  pMatMgO->AddElement(elMg,aw);
  aw = 1;  // 1 O in chemical formula
  pMatMgO->AddElement(elO,aw);
  pMatMgO->SetIndex(6);
  
  // Soda-lime glass: 73% SiO2, 14% Na2O, 9% CaO, 4% MgO
  nel     = 4;
  density = 2.53;
  TGeoMixture* pMatGlass = new TGeoMixture("Glass", nel,density);
  w = 0.73;  // 73% SiO2
  pMatGlass->AddElement(pMatSiO2,w);
  w = 0.14;  // 14% Na2O
  pMatGlass->AddElement(pMatNa2O,w);
  w = 0.09;  // 9% CaO
  pMatGlass->AddElement(pMatCaO,w);
  w = 0.04;  // 4% MgO
  pMatGlass->AddElement(pMatMgO,w);
  pMatGlass->SetIndex(7);
  
  // Freon: C2H2F4
  nel     = 3;
  density = 0.00425;
  TGeoMixture* pMatFreon = new TGeoMixture("Freon", nel,density);
  aw = 2;  // 2 C in chemical formula
  pMatFreon->AddElement(elC,aw);
  aw = 2;  // 2 H in chemical formula
  pMatFreon->AddElement(elH,aw);
  aw = 4;  // 4 F in chemical formula
  pMatFreon->AddElement(elF,aw);
  pMatFreon->SetIndex(8);
  
  // SF6
  nel     = 2;
  density = 0.006164;
  TGeoMixture* pMatSF6 = new TGeoMixture("SF6", nel,density);
  aw = 1;  // 1 S in chemical formula
  pMatSF6->AddElement(elS,aw);
  aw = 6;  // 6 F in chemical formula
  pMatSF6->AddElement(elF,aw);
  pMatSF6->SetIndex(9);
  
  // Isobutane: C4H10
  nel     = 2;
  density = 0.00251;
  TGeoMixture* pMatIbut = new TGeoMixture("Ibut", nel,density);
  aw = 4;  // 4 C in chemical formula
  pMatIbut->AddElement(elC,aw);
  aw = 10;  // 10 H in chemical formula
  pMatIbut->AddElement(elH,aw);
  pMatIbut->SetIndex(10);
  
  // RPCgas: 84% Freon, 10% SF6, 6% Isobutane
  nel     = 3;
  density = 0.0053;
  TGeoMixture* pMatRPCgas = new TGeoMixture("RPCgas", nel,density);
  w = 0.84;  // 84% Isobutane
  pMatRPCgas->AddElement(pMatFreon,w);
  w = 0.10;  // 10% SF6
  pMatRPCgas->AddElement(pMatSF6,w);
  w = 0.06;  // 6% Isobutane
  pMatRPCgas->AddElement(pMatIbut,w);
  pMatRPCgas->SetIndex(11);
  
  // Bakelite: C43H38O7
  nel     = 3;
  density = 1.25;
  TGeoMixture* pMatBak = new TGeoMixture("Bak", nel,density);
  aw = 43;  // 43 C in chemical formula
  pMatBak->AddElement(elC,aw);
  aw = 38;  // 38 H in chemical formula
  pMatBak->AddElement(elH,aw);
  aw = 7;  // 38 O in chemical formula
  pMatBak->AddElement(elO,aw);
  pMatBak->SetIndex(12);
  
  // Al
  nel     = 1;
  density = 2.70;
  TGeoMixture* pMatAl = new TGeoMixture("Aluminum", nel,density);
  w = 1.000000;
  pMatAl->AddElement(elAl,w);
  pMatAl->SetIndex(13);
  
  // Methane: CH4
  nel     = 2;
  density = 0.000717;
  TGeoMixture* pMatMet = new TGeoMixture("Met", nel,density);
  aw = 1;  // 1 C in chemical formula
  pMatMet->AddElement(elC,aw);
  aw = 4;  // 4 H in chemical formula
  pMatMet->AddElement(elH,aw);
  pMatMet->SetIndex(14);
  
  // Paraffin: C20H42 (- C40H82)
  nel     = 2;
  density = 0.93;
  TGeoMixture* pMatPar = new TGeoMixture("Par", nel,density);
  aw = 20;  // 20 C in chemical formula
  pMatPar->AddElement(elC,aw);
  aw = 42;  // 42 H in chemical formula
  pMatPar->AddElement(elH,aw);
  pMatPar->SetIndex(15);

  // Pb
  nel     = 1;
  density = 11.43;
  TGeoMixture* pMatPb = new TGeoMixture("Lead", nel,density);
  w = 1.000000;
  pMatPb->AddElement(elPb,w);
  pMatPb->SetIndex(16);

  // Mylar: C10H8O4
  nel     = 3;
  density = 1.4;
  TGeoMixture* pMatMyl = new TGeoMixture("Myl", nel,density);
  aw = 10;  // 10 C in chemical formula
  pMatMyl->AddElement(elC,aw);
  aw = 8;  // 8 H in chemical formula
  pMatMyl->AddElement(elH,aw);
  aw = 4;  // 4 O in chemical formula
  pMatMyl->AddElement(elO,aw);
  pMatMyl->SetIndex(17);

  // -- Media
  
  Double_t par[8];
  //par[0]  = 0.000000; // isvol
  //par[1]  = 0.000000; // ifield
  //par[2]  = 0.000000; // fieldm
  //par[3]  = 10.00000; // tmaxfd
  //par[4]  = 0.000100; // stemax
  //par[5]  = 0.050000; // deemax
  //par[6]  = 0.000100; // epsil
  //par[7]  = 0.000100; // stmin
  par[0]  =   -1.0; // isvol
  par[1]  =   -1.0; // ifield
  par[2]  =   -1.0; // fieldm
  par[3]  =   -1.0; // tmaxfd
  par[4]  =  -1.0; // stemax
  par[5]  =  -1.0; // deemax
  par[6]  =  -1.0; // epsil
  par[7]  =  -1.0; // stmin
  
  numed   = 22;
  //pMedAir = new TGeoMedium("Air", numed,pMatAir, par);
  pMedAir = new TGeoMedium("Air", numed, pMatAir);
  numed   = 23;
  //pMedSteel = new TGeoMedium("Steel", numed,pMatSteel, par);
  pMedSteel = new TGeoMedium("Steel", numed, pMatSteel);
  numed   = 24;
  //pMedGlass = new TGeoMedium("Glass", numed,pMatGlass, par);
  pMedGlass = new TGeoMedium("Glass", numed, pMatGlass);
  numed   = 26;
  //pMedBak = new TGeoMedium("Bakelite", numed,pMatBak, par);
  pMedBak = new TGeoMedium("Bakelite", numed, pMatBak);
  numed   = 27;
  //pMedAl = new TGeoMedium("Aluminum", numed,pMatAl, par);
  pMedAl = new TGeoMedium("Aluminum", numed, pMatAl);
  numed   = 28;
  //pMedGas = new TGeoMedium("Gas", numed,pMatRPCgas, par);
  pMedGasInactive = new TGeoMedium("GasInactive", numed, pMatRPCgas);
  numed   = 29;
  //pMedPar = new TGeoMedium("Paraffin", numed,pMatPar, par);
  pMedPar = new TGeoMedium("Paraffin", numed, pMatPar);
  numed   = 30;
  //pMedPb = new TGeoMedium("Lead", numed,pMatPb, par);
  pMedPb = new TGeoMedium("Lead", numed, pMatPb);
  numed   = 31;
  //pMedMyl = new TGeoMedium("Mylar", numed,pMatMyl, par);
  pMedMyl = new TGeoMedium("Mylar", numed, pMatMyl);
 
  // More fine stepping def. in Gas
  par[0]  =   -1.0; // isvol
  par[1]  =   -1.0; // ifield
  par[2]  =   -1.0; // fieldm
  par[3]  =   -1.0; // tmaxfd
  par[4]  =   -1.0001; // stemax
  par[5]  =   -0.005; // deemax
  par[6]  =   -1.0001; // epsil
  par[7]  =   -1.0001; // stmin
  numed   = 25;
  pMedGas = new TGeoMedium("Gas", numed,pMatRPCgas, par);
  //pMedGas = new TGeoMedium("Gas", numed,pMatIbut, par);
  //pMedGas = new TGeoMedium("Gas", numed,pMatMet, par);
    //pMedGas = new TGeoMedium("Gas", numed,pMatRPCgas);
  
  //-- Main Geometry definition
  //-- Get the top volume (CAVE) from the TGeoManager class
  
  TGeoVolume* WorldLog = gGeoManager->GetTopVolume();
  WorldLog->SetVisLeaves(kTRUE);
  WorldLog->SetVisContainers(kTRUE);
  
  TGeoShape* St1Sol = new TGeoBBox("St1Box",DetW/2,DetH/2+SegGapWidth/2,St1T/2);
  TGeoVolume* St1Log = new TGeoVolume("St1Log",St1Sol, pMedSteel);
  AddSensitiveVolume(St1Log);

  TGeoShape* Gas1Sol = new TGeoBBox("Gas1Box",DetW/2,DetH/2,Gas1T/2);
  TGeoVolume* Gas1Log = new TGeoVolume("Gas1Log",Gas1Sol, pMedGasInactive);
  AddSensitiveVolume(Gas1Log);

  TGeoShape* Myl1Sol = new TGeoBBox("Myl1Box",DetW/2,DetH/2,Myl1T/2);
  TGeoVolume* Myl1Log = new TGeoVolume("Myl1Log",Myl1Sol, pMedMyl);
  AddSensitiveVolume(Myl1Log);

  TGeoShape* GlassSol = new TGeoBBox("GlassBox",DetW/2,DetH/2,GlassT/2);
  TGeoVolume* GlassLog = new TGeoVolume("GlassLog",GlassSol, pMedGlass);
  AddSensitiveVolume(GlassLog);

  TGeoShape* Gas2Sol = new TGeoBBox("Gas2Box",DetW/2,DetH/2,Gas2T/2);
  TGeoVolume* Gas2Log = new TGeoVolume("Gas2Log",Gas2Sol, pMedGas);
  AddSensitiveVolume(Gas2Log);

  TGeoShape* Myl2Sol = new TGeoBBox("Myl2Box",DetW/2,DetH/2,Myl2T/2);
  TGeoVolume* Myl2Log = new TGeoVolume("Myl2Log",Myl2Sol, pMedMyl);
  AddSensitiveVolume(Myl2Log);

  TGeoShape* Gas3Sol = new TGeoBBox("Gas3Box",DetW/2,AnGapWidth/2,St2T/2);
  TGeoVolume* Gas3Log = new TGeoVolume("Gas3Log",Gas3Sol, pMedGasInactive);
  AddSensitiveVolume(Gas3Log);

  TGeoShape* St2Sol = new TGeoBBox("St2Box",DetW/2,StripWidth/2,St2T/2);
  TGeoVolume* St2Log = new TGeoVolume("St2Log",St2Sol, pMedSteel);
  AddSensitiveVolume(St2Log);

  Double_t xPos,yPos,zPos;
  Double_t yPosRel,zPosRel;
  TGeoRotation *rot2 = new TGeoRotation("rot",0,0,0);
  
  xPos = 0;
  yPos = 0;
  zPos = 0;
  yPosRel = 0;
  zPosRel = 0;

  TGeoVolumeAssembly* assemblyDetector = new TGeoVolumeAssembly("ASSEMBLYDET"); 
  yPosRel = 0;
  zPosRel = -(St1T/2+Gas1T+Myl1T+(NofCells/2)*(Gas2T+GlassT)+GlassT+Myl2T+St2T/2);
  rot2->RotateY(0);
  assemblyDetector->AddNode(St1Log,1001,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
  zPosRel = zPosRel + St1T/2 + Gas1T/2;
  assemblyDetector->AddNode(Gas1Log,1101,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
  zPosRel = zPosRel + Gas1T/2 + Myl1T/2;
  assemblyDetector->AddNode(Myl1Log,1201,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
  zPosRel = zPosRel + Myl1T/2 + GlassT/2;
  assemblyDetector->AddNode(GlassLog,20,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
  zPosRel = zPosRel + GlassT/2 + Gas2T/2;
  for(Int_t i=0;i<NofCells/2;i++) {
    assemblyDetector->AddNode(Gas2Log,i,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
    zPosRel = zPosRel + Gas2T/2 + GlassT/2;
    assemblyDetector->AddNode(GlassLog,i+21,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
    if(i<NofCells/2-1) {
      zPosRel = zPosRel + GlassT/2 + Gas2T/2;
    }
  }
  zPosRel = zPosRel + GlassT/2 + Myl2T/2;
  assemblyDetector->AddNode(Myl2Log,1202,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
  zPosRel = zPosRel + Myl2T/2 + St2T/2;
  yPosRel = -DetH/2 + StripWidth/2;
  for(Int_t i=0;i<NofStrips;i++) {
    assemblyDetector->AddNode(St2Log,130+i,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
    if(i<NofStrips-1) {
      yPosRel = yPosRel + StripWidth/2 + AnGapWidth/2;
      assemblyDetector->AddNode(Gas3Log,1300+i,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
      yPosRel = yPosRel + AnGapWidth/2 + StripWidth/2;
    }
  }
  yPosRel = 0;
  zPosRel = zPosRel + St2T/2 + Myl2T/2;
  assemblyDetector->AddNode(Myl2Log,1203,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
  zPosRel = zPosRel + Myl2T/2 + GlassT/2;
  assemblyDetector->AddNode(GlassLog,24,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
  zPosRel = zPosRel + GlassT/2 + Gas2T/2;
  for(Int_t i=0;i<NofCells/2;i++) {
    assemblyDetector->AddNode(Gas2Log,i+NofCells/2,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
    zPosRel = zPosRel + Gas2T/2 + GlassT/2;
    assemblyDetector->AddNode(GlassLog,i+25,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
    if(i<NofCells/2-1) {
      zPosRel = zPosRel + GlassT/2 + Gas2T/2;
    }
  }
  zPosRel = zPosRel + GlassT/2 + Myl1T/2;
  assemblyDetector->AddNode(Myl1Log,1204,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
  zPosRel = zPosRel + Myl1T/2 + St1T/2;
  assemblyDetector->AddNode(St1Log,1002,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
  ///// first half of unit ends /////
  zPosRel = zPosRel + St1T/2 + St1T/2;
  assemblyDetector->AddNode(St1Log,1003,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
  zPosRel = zPosRel + St1T/2 + Myl1T/2;
  assemblyDetector->AddNode(Myl1Log,1205,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
  zPosRel = zPosRel + Myl1T/2 + GlassT/2;
  assemblyDetector->AddNode(GlassLog,28,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
  zPosRel = zPosRel + GlassT/2 + Gas2T/2;
  for(Int_t i=0;i<NofCells/2;i++) {
    assemblyDetector->AddNode(Gas2Log,i+2*NofCells/2,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
    zPosRel = zPosRel + Gas2T/2 + GlassT/2;
    assemblyDetector->AddNode(GlassLog,i+29,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
    if(i<NofCells/2-1) {
      zPosRel = zPosRel + GlassT/2 + Gas2T/2;
    }
  }
  zPosRel = zPosRel + GlassT/2 + Myl2T/2;
  assemblyDetector->AddNode(Myl2Log,1206,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
  zPosRel = zPosRel + Myl2T/2 + St2T/2;
  yPosRel = -DetH/2 + StripWidth/2;
  for(Int_t i=0;i<NofStrips;i++) {
    assemblyDetector->AddNode(St2Log,130+NofStrips+i,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
    if(i<NofStrips-1) {
      yPosRel = yPosRel + StripWidth/2 + AnGapWidth/2;
      assemblyDetector->AddNode(Gas3Log,1400+i,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
      yPosRel = yPosRel + AnGapWidth/2 + StripWidth/2;
    }
  }
  yPosRel = 0;
  zPosRel = zPosRel + St2T/2 + Myl2T/2;
  assemblyDetector->AddNode(Myl2Log,1207,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
  zPosRel = zPosRel + Myl2T/2 + GlassT/2;
  assemblyDetector->AddNode(GlassLog,32,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
  zPosRel = zPosRel + GlassT/2 + Gas2T/2;
  for(Int_t i=0;i<NofCells/2;i++) {
    assemblyDetector->AddNode(Gas2Log,i+3*NofCells/2,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
    zPosRel = zPosRel + Gas2T/2 + GlassT/2;
    assemblyDetector->AddNode(GlassLog,i+33,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
    if(i<NofCells/2-1) {
      zPosRel = zPosRel + GlassT/2 + Gas2T/2;
    }
  }
  zPosRel = zPosRel + GlassT/2 + Myl1T/2;
  assemblyDetector->AddNode(Myl1Log,1208,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
  zPosRel = zPosRel + Myl1T/2 + Gas1T/2;
  assemblyDetector->AddNode(Gas1Log,1102,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));
  zPosRel = zPosRel + Gas1T/2 + St1T/2;
  assemblyDetector->AddNode(St1Log,1004,new TGeoCombiTrans(0,yPosRel,zPosRel,rot2));

  for(Int_t i=0;i<NofUnitLayers;i++) {
    if(i>0) zPos = zPos + UnitT;
    yPos = -LayerH/2 + SegGapWidth/2 + DetH/2;
    for(Int_t j=0;j<NofSegs;j++) {
      WorldLog->AddNode(assemblyDetector,i*NofSegs+j,new TGeoCombiTrans(xPos,yPos,zPos,rot2));
      yPos = yPos + DetH + SegGapWidth;
    }
  }
}

ClassImp(R3BNeuLand)
