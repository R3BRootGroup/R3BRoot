// -------------------------------------------------------------------------
// -----                    R3BLand source file                        -----
// -----               Created 26/03/09  by D.Bertini                  -----
// -------------------------------------------------------------------------
#include "R3BLand.h"

#include "R3BGeoLand.h"
#include "R3BLandPoint.h"
#include "R3BGeoLandPar.h"
#include "R3BLandDigiPar.h"
#include "R3BMCStack.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
//#include "FairStack.h"
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

using std::cout;
using std::cerr;
using std::endl;



// -----  Default constructor  -------------------------------------------
R3BLand::R3BLand() : R3BDetector("R3BLand", kTRUE, kLAND) {
  ResetParameters();
  fLandCollection = new TClonesArray("R3BLandPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fLandFirstHits = new TClonesArray("R3BLandFirstHits");
}
// -------------------------------------------------------------------------



// -----  Standard constructor  ------------------------------------------
R3BLand::R3BLand(const char* name, Bool_t active) 
    : R3BDetector(name, active, kLAND) {
  ResetParameters();
  fLandCollection = new TClonesArray("R3BLandPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fLandFirstHits = new TClonesArray("R3BLandFirstHits");
}
// -------------------------------------------------------------------------



// -----  Destructor  ----------------------------------------------------
R3BLand::~R3BLand() {
  if ( flGeoPar ) delete flGeoPar;
  if (fLandCollection) {
   fLandCollection->Delete();
   delete fLandCollection;
  }
  if (fLandFirstHits) {
   fLandFirstHits->Delete();
   delete fLandFirstHits;
  }
}
// -------------------------------------------------------------------------

void R3BLand::Initialize()
{

  FairDetector::Initialize();

  cout << "-I- R3BLand: initialisation " << endl;
  
  Int_t id1 = 0, id2 = 0, id3 = 0;
  
  TString fileName = GetGeometryFileName();
  if(fileName.Contains("neuland")) {
    cout << "-I- R3BLand: Paddle B5 (McId): " << gMC->VolId("padle_h_box5") << endl;
    id3 = gMC->VolId("padle_h_box5");
    fMapMcId[id3]=3;
  } else {
    cout << "-I- R3BLand: Paddle B3 (McId): " << gMC->VolId("padle_h_box3") << endl;
    cout << "-I- R3BLand: Paddle B4 (McId): " << gMC->VolId("padle_h_box4") << endl;
    cout << "-I- R3BLand: Paddle B5 (McId): " << gMC->VolId("padle_h_box5") << endl;
    id1 = gMC->VolId("padle_h_box3");
    id2 = gMC->VolId("padle_h_box4");
    id3 = gMC->VolId("padle_h_box5");
    fMapMcId[id1]=1;
    fMapMcId[id2]=2;
    fMapMcId[id3]=3;
  }

  // Sensitive Volumes :: Unique  Id
  //  paddle_h_box3       1
  //  paddle_h_box4       2
  //  paddle_h_box5       3

  //<D.B> Check me
  // Initialise variables from Birk law
  Double_t dP = 1.032 ;
  // Set constants for Birk's Law implentation
  fBirkC0 =  1.;
  fBirkC1 =  0.013/dP;
  fBirkC2 =  9.6e-6/(dP * dP);

  useNeuLAND=false;
}


// -----  Public method ProcessHits  --------------------------------------
Bool_t R3BLand::ProcessHits(FairVolume* vol) {

    // <DB>  Before filling MC info for current track
	//       print step info if verbose > 1
//    cout<<"Test verbose: "<<fVerboseLevel<<endl;
    if (fVerboseLevel>1) StepHistory();

  // --- get Geometry hiearchical Information
  Int_t cp1=-1;
  Int_t cp2=-1;
  Int_t volId1=-1;
  Int_t volId2=-1;


  // Crystals Ids
  volId1 =  gMC->CurrentVolID(cp1);
  // Lead - Crystal numbering scheme
  fPaddleTyp = fMapMcId[volId1];
  // Mother Assembly def
  volId2 =  gMC->CurrentVolOffID(1, cp2);


  if ( gMC->IsTrackEntering() ) {
   fELoss  = 0.;
   fLightYield = 0.;
   fTime  = gMC->TrackTime() * 1.0e09;
   fLength = gMC->TrackLength();
   gMC->TrackPosition(fPosIn);
   gMC->TrackMomentum(fMomIn);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();
//  cout<<"fELoss: "<<fELoss<<endl;

  // Apply Birk's law ( Adapted from G3BIRK/Geant3)
  Double_t lightYield =  gMC->Edep() ;
/*  
  const Double_t* cpos;
  cpos = gGeoManager->GetCurrentPoint();
  cout<<"current pos: "<<cpos[0]<<"  "<<cpos[1]<<"  "<<cpos[2]<<endl;
  cout<<"particle: "<<gMC->TrackPid()<<endl;
  cout<<"step: "<<gMC->TrackStep()<<endl;
  cout<<"fELoss: "<<fELoss<<endl;
*/  
  // Correction for all charge states
  if (gMC->TrackCharge()!=0) {
    Double_t birkC1Mod = 0;
     // Apply correction for higher charge states
    if (fBirkC0==1){
      if (TMath::Abs(gMC->TrackCharge())>=2)
        birkC1Mod=fBirkC1*7.2/12.6;
      else
        birkC1Mod=fBirkC1;
    }

    Double_t dedxcm=0.;
    if (gMC->TrackStep()>0)
    {
      dedxcm=1000.*gMC->Edep()/gMC->TrackStep();
//      cout<<"Edep: "<<gMC->Edep()<<" Trackstep: "<<gMC->TrackStep()<<endl;
      lightYield=lightYield/(1.+birkC1Mod*dedxcm+fBirkC2*dedxcm*dedxcm);
//      cout<<"dedxcm "<<dedxcm<<" factor "<<1./(1.+birkC1Mod*dedxcm+fBirkC2*dedxcm*dedxcm)
//      <<" light yield "<<lightYield<<endl;
      fLightYield=fLightYield+lightYield;
//      cout<<"fLightYield: "<<fLightYield<<endl;
    }
  }


  // Set additional parameters at exit of active volume. Create R3BLandPoint.
  if ( gMC->IsTrackExiting()   ||
     gMC->IsTrackStop()        ||
     gMC->IsTrackDisappeared()  )
  {
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
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
    
  //     cout << "1st direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2] << endl;

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

      fPosOut.SetX(newpos[0]);
      fPosOut.SetY(newpos[1]);
      fPosOut.SetZ(newpos[2]);
    }



    AddHit(fTrackID, fVolumeID,  fPaddleTyp,  cp2, cp1,
      TVector3(fPosIn.X(),  fPosIn.Y(),  fPosIn.Z()),
      TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
      TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
      TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
      fTime, fLength, fELoss, fLightYield);
     

     // Increment number of LandPoints for this track
    R3BStack* stack = (R3BStack*) gMC->GetStack();
    stack->AddPoint(kLAND);
     
    ResetParameters();
  }

  return kTRUE;
}




// -----  Public method BeginOfEvent  -----------------------------------------
void R3BLand::BeginEvent() {
   //cout << "-I- begin event called ############################# " << endl;

//   if (gGeoManager)
//   TGeoVolume * vol = gGeoManager->FindVolumeFast("padle_h_box3");
   for(Int_t l=0;l<6;l++){
      firstHitX[l]=0.;
      firstHitY[l]=0.;
      firstHitZ[l]=0.;
      firstT[l]=0.;      
   }
}

void R3BLand::FinishEvent() {
   AddHit1(firstHitX[0], firstHitY[0], firstHitZ[0], firstT[0],
           firstHitX[1], firstHitY[1], firstHitZ[1], firstT[1],
           firstHitX[2], firstHitY[2], firstHitZ[2], firstT[2],
           firstHitX[3], firstHitY[3], firstHitZ[3], firstT[3],
           firstHitX[4], firstHitY[4], firstHitZ[4], firstT[4],	   	   
           firstHitX[5], firstHitY[5], firstHitZ[5], firstT[5]);


}




// -----  Public method EndOfEvent  -----------------------------------------
void R3BLand::EndOfEvent() {

  if (fVerboseLevel) Print();
  Reset();
 

}
// ----------------------------------------------------------------------------

// -----  Public method Register  -------------------------------------------
void R3BLand::Register() {
  FairRootManager::Instance()->Register("LandPoint", GetName(), fLandCollection, kTRUE);
  FairRootManager::Instance()->Register("LandFirstHits", GetName(), fLandFirstHits, kTRUE);
}
// ----------------------------------------------------------------------------



// -----  Public method GetCollection  --------------------------------------
TClonesArray* R3BLand::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fLandCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----  Public method Print  ----------------------------------------------
void R3BLand::Print() const {
  Int_t nHits = fLandCollection->GetEntriesFast();
  cout << "-I- R3BLand: " << nHits << " points registered in this event." 
     << endl;
  nHits = fLandFirstHits->GetEntriesFast();
  cout << "-I- R3BLandFirstHits: " << nHits << " points registered in this event." 
     << endl;
}
// ----------------------------------------------------------------------------



// -----  Public method Reset  ----------------------------------------------
void R3BLand::Reset() {
  cout << "------------------------- Reset() called " << endl;
  fLandCollection->Clear();
  ResetParameters();
  fLandFirstHits->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----  Public method CopyClones  -----------------------------------------
void R3BLand::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- R3BLand: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  R3BLandPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
  oldpoint = (R3BLandPoint*) cl1->At(i);
   Int_t index = oldpoint->GetTrackID() + offset;
   oldpoint->SetTrackID(index);
   new (clref[fPosIndex]) R3BLandPoint(*oldpoint);
   fPosIndex++;
  }
  cout << " -I- R3BLand: " << cl2->GetEntriesFast() << " merged entries." << endl;
}

// -----  Private method AddHit  --------------------------------------------
R3BLandPoint* R3BLand::AddHit(Int_t trackID, Int_t detID, Int_t box, Int_t id1, Int_t id2,
			      TVector3 posIn,
			      TVector3 posOut, TVector3 momIn,
			      TVector3 momOut, Double_t time,
			      Double_t length, Double_t eLoss, Double_t lightYield)
{
  TClonesArray& clref = *fLandCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) {
    cout << "-I- R3BLand: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss  << " GeV" << endl;
  }
  return new(clref[size]) R3BLandPoint(trackID, detID, box, id1, id2,  posIn, posOut,
				       momIn, momOut, time, length, eLoss, lightYield);
}

R3BLandFirstHits* R3BLand::AddHit1(Double_t x0, Double_t y0, Double_t z0, Double_t t0,
                                   Double_t x1, Double_t y1, Double_t z1, Double_t t1,
                                   Double_t x2, Double_t y2, Double_t z2, Double_t t2,
                                   Double_t x3, Double_t y3, Double_t z3, Double_t t3,
                                   Double_t x4, Double_t y4, Double_t z4, Double_t t4,
                                   Double_t x5, Double_t y5, Double_t z5, Double_t t5){
  TClonesArray& clref = *fLandFirstHits;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BLandFirstHits(x0,y0,z0,t0,x1,y1,z1,t1,x2,y2,z2,t2,x3,y3,z3,t3,x4,y4,z4,t4,x5,y5,z5,t5);
}

// -----  Public method UseNeuLand  ----------------------------------
void R3BLand::UseNeuLand(Double_t paddle_length, Double_t paddle_width, Double_t paddle_depth, 
Double_t neuLAND_depth, Double_t paddle_gap, Double_t paddle_wrapping1, Double_t paddle_wrapping2)
{
  useNeuLAND=true;
  neuLAND_paddle_dimx = paddle_length;    // half of the length [cm]
  neuLAND_paddle_dimy = paddle_width;      // half of the width [cm]
  neuLAND_paddle_dimz = paddle_depth;      // half of the depth [cm]
  neuLAND_depth_dim   = neuLAND_depth;    // half detector depth [cm]
  neuLAND_gap_dim   =paddle_gap;    // total detector depth [cm]
  neuLAND_wrapping1_dim   = paddle_wrapping1;    // thickness of wrapping material [cm]
  neuLAND_wrapping2_dim   = paddle_wrapping2;    // thickness of wrapping material [cm]
  
}


void R3BLand::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")) {
    fLogger->Info(MESSAGE_ORIGIN,
		  "Constructing NEULAND geometry from ROOT file %s", 
		  fileName.Data());
    ConstructRootGeometry();
    TGeoBBox *box = (TGeoBBox*) gGeoManager->GetVolume(gMC->VolId("padle_h_box5"))->GetShape();
    TGeoBBox *bbox = (TGeoBBox*) gGeoManager->GetVolume(gMC->VolId("ALAND"))->GetShape();
    TGeoBBox *bbbox = (TGeoBBox*) gGeoManager->GetVolume("CELL")->GetShape();
    Int_t maxPlane;
    Int_t maxPaddle;
    maxPlane = (Int_t) (bbbox->GetDZ() / bbox->GetDZ());
    if(fileName.Contains("neuland_proto")) {
      maxPaddle = (Int_t) ( maxPlane * ((Int_t)(bbbox->GetDX()/bbox->GetDY())) );
    } else if(fileName.Contains("neuland")) {
      maxPaddle = (Int_t) ( maxPlane * ((Int_t)(box->GetDX()/bbox->GetDY())) );
    } else {
      maxPaddle = (Int_t) ( maxPlane * ((Int_t)(box->GetDX()/box->GetDY())) );
    }
//     cout << box->GetDX() << "   " << box->GetDY() << "   " << box->GetDZ() << endl;
//     cout << bbox->GetDX() << "   " << bbox->GetDY() << "   " << bbox->GetDZ() << endl;
//     cout << bbbox->GetDX() << "   " << bbbox->GetDY() << "   " << bbbox->GetDZ() << endl;
//     cout << maxPlane << endl;
//     cout << maxPaddle << endl;
    FairRun *fRun = FairRun::Instance();
    FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
    R3BLandDigiPar* par=(R3BLandDigiPar*)(rtdb->getContainer("R3BLandDigiPar"));
    par->SetGeometryFileName(GetGeometryFileName());
    par->SetMaxPaddle(maxPaddle);
    par->SetMaxPlane(maxPlane);
    par->SetPaddleLength(box->GetDX());
    par->setChanged();
  } else {
    fLogger->Info(MESSAGE_ORIGIN,
		  "Constructing hardcoded NEULAND geometry");
    ConstructGeometryOld();
  }
}



// -----   CheckIfSensitive   -------------------------------------------------
Bool_t R3BLand::CheckIfSensitive(std::string name)
{
  TString volName = name;
  if(volName.Contains("padle_h_box5")) {
    return kTRUE;
  }
  return kFALSE;
}
// ----------------------------------------------------------------------------




// -----  Public method ConstructGeometry  ----------------------------------
void R3BLand::ConstructGeometryOld() {

  // out-of-file geometry definition
  Double_t a;
  Double_t z, density, w;
  Int_t nel, numed;
  Double_t radl,absl;

/****************************************************************************/
// Material definition

//------------------Creat media----------------------------------
 // Material: Iron
  TGeoMedium * pMedFe=NULL;
  if (gGeoManager->GetMedium("Iron") ){
    pMedFe=gGeoManager->GetMedium("Iron");
  }
  else{
    w     =      0.;
    a     = 55.850000;
    z     = 26.000000;
    density = 7.870000;
    radl   = 1.757717;
    absl   = 169.994418;
    TGeoMaterial*
  pMatFe = new TGeoMaterial("Iron", a,z,density,radl,absl);
    pMatFe->SetIndex(701);
    numed  = 23;  // medium number
    Double_t par[8];
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 0.000000; // tmaxfd
    par[4]  = 0.000000; // stemax
    par[5]  = 0.000000; // deemax
    par[6]  = 0.000000; // epsil
    par[7]  = 0.000000; // stmin
    pMedFe = new TGeoMedium("Iron", numed,pMatFe, par);
  }

  TGeoMedium *Iron = pMedFe;

  // BC408 plastic medium
  // Mixture: BC408
  TGeoMedium * pMed37=NULL;
  if (gGeoManager->GetMedium("BC408") ){
    pMed37=gGeoManager->GetMedium("BC408");
  }
  else{
    nel    = 2;
    density = 1.032000;
    TGeoMixture*
    pMat37 = new TGeoMixture("BC408", nel,density);
    a = 1.007940;  z = 1.000000;  w = 0.0764;  // H
    pMat37->DefineElement(0,a,z,w);
    a = 12.010700;  z = 6.000000;  w = 0.916;  // C
    pMat37->DefineElement(1,a,z,w);
    pMat37->SetIndex(36);
    // Medium: BC408
    numed  = 36;  // medium number
    Double_t par[8];
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 0.000000; // tmaxfd
    par[4]  = 0.000000; // stemax
    par[5]  = 0.000000; // deemax
    par[6]  = 0.000000; // epsil
    par[7]  = 0.000000; // stmin
    pMed37 = new TGeoMedium("BC408", numed,pMat37,par);
  }

  // plastic wrapping
  // Mixture: CH2
  TGeoMedium * pMed38=NULL;
  if (gGeoManager->GetMedium("CH2") ){
    pMed38=gGeoManager->GetMedium("CH2");
  }
  else{
    nel    = 2;
    density = 1.0;
    TGeoMixture*
    pMat38 = new TGeoMixture("CH2", nel,density);
    a = 1.007940;  z = 1.000000;  w = 0.857;  // H
    pMat38->DefineElement(0,a,z,w);
    a = 12.010700;  z = 6.000000;  w = 0.143;  // C
    pMat38->DefineElement(1,a,z,w);
    pMat38->SetIndex(38);
    // Medium: CH2
    numed  = 38;  // medium number
    Double_t par[8];
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 0.000000; // tmaxfd
    par[4]  = 0.000000; // stemax
    par[5]  = 0.000000; // deemax
    par[6]  = 0.000100; // epsil
    par[7]  = 0.000000; // stmin
    pMed38 = new TGeoMedium("CH2", numed,pMat38,par);
  }
  
  

// Material: Alu
  TGeoMedium * pMedAl=NULL;
  if (gGeoManager->GetMedium("aluminium") ){
    pMedAl=gGeoManager->GetMedium("aluminium");
  }
  else{
    w     =      0.;
    a     = 26.981538;
    z     = 13.000000;
    density = 2.70000;
    radl   = 8.875105;
    absl   = 388.793113;
    TGeoMaterial*
    pMatAl = new TGeoMaterial("Aluminium", a,z,density,radl,absl);
    pMatAl->SetIndex(13);
    numed  = 27;  // medium number
    Double_t par[8];
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 0.000000; // tmaxfd
    par[4]  = 0.000000; // stemax
    par[5]  = 0.000000; // deemax
    par[6]  = 0.000100; // epsil
    par[7]  = 0.000000; // stmin
    pMedAl = new TGeoMedium("Aluminium", numed, pMatAl, par);
  }

  TGeoMedium *Aluminium = pMedAl;

  TGeoVolume* vWorld = gGeoManager->GetTopVolume();
  vWorld->SetVisLeaves(kTRUE);

  //Switch between Land and NeuLand
  if (useNeuLAND)
    ConstructNeuLandGeometry(vWorld, Aluminium, pMed37, pMed38);
  else
    ConstructLandGeometry(vWorld, Iron, pMed37);

}

// --------------------------------------------------------------------------------
// Construct Land Geometry
// --------------------------------------------------------------------------------
void R3BLand::ConstructLandGeometry(  TGeoVolume* vWorld,  TGeoMedium *Iron, TGeoMedium *BC408)
{
  Double_t dx1, dx2, dy1, dy2, dz;
  Double_t tx,ty,tz;

  // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
  //------------------ Iron shell ------------------------------------------
  Double_t padle_h_dim1x = 100.0;
  Double_t padle_h_dim1y = 5.1;
  Double_t padle_h_dim1z = 5.1;
  TGeoShape* padle_h_box1 = new TGeoBBox("padle_h_box1",
      padle_h_dim1x, padle_h_dim1y,  padle_h_dim1z);
  Double_t padle_h_dim2x = 100.0;
  Double_t padle_h_dim2y = 5.0;
  Double_t padle_h_dim2z = 5.0;
  TGeoShape* padle_h_box2 = new TGeoBBox("padle_h_box2",
       padle_h_dim2x, padle_h_dim2y, padle_h_dim2z);

  // Create a composite shape
  TGeoCompositeShape *sheetbox = new TGeoCompositeShape("diffbox", "padle_h_box1 - padle_h_box2");

  //------------------ Iron endpoints --------------------------------------
  TGeoVolume *bvol = new TGeoVolume("sheetbox",sheetbox,Iron);
   //end-cap list
  dx1 = 5.10;
  dx2 = 2.51;
  dy1 = 5.10;
  dy2 = 2.51;
  dz  = 5.00;
  TGeoVolume *trap = gGeoManager->MakeTrd2("atrap",BC408,dx1,dx2,dy1,dy2,dz);
 
  //------------------ Iron sheets -----------------------------------------
  Double_t padle_h_dim3x = 100.0;
  Double_t padle_h_dim3y = 5.0;
  Double_t padle_h_dim3z = 0.125;
  TGeoVolume *padle_h_box3 = gGeoManager->MakeBox("padle_h_box3",Iron,
          padle_h_dim3x, padle_h_dim3y, padle_h_dim3z);

  Double_t padle_h_dim4x = 100.0;
  Double_t padle_h_dim4y = 5.0;
  Double_t padle_h_dim4z = 0.25;
  TGeoVolume *padle_h_box4 = gGeoManager->MakeBox("padle_h_box4",Iron,
          padle_h_dim4x, padle_h_dim4y, padle_h_dim4z);

  //------------------ BC408 sheets -----------------------------------------
  Double_t padle_h_dim5x = 100.0;
  Double_t padle_h_dim5y = 5.0;
  Double_t padle_h_dim5z = 0.25;
  TGeoVolume *padle_h_box5 = gGeoManager->MakeBox("padle_h_box5",BC408,
        padle_h_dim5x, padle_h_dim5y, padle_h_dim5z);



  // Make the elementary assembly of the whole structure
  //------------------ Paddle shell assembly --------------------------------
  TGeoVolume *aLand = new TGeoVolumeAssembly("ALAND");
  // Add Iron shell
  TGeoRotation *zeroRotation= new TGeoRotation();
  zeroRotation->RotateX(0.);
  zeroRotation->RotateY(0.);
  zeroRotation->RotateZ(0.);
  Double_t xx = 0.;
  Double_t yy = 0.;
  Double_t zz = 0.;
  aLand->AddNode(bvol,1,new TGeoCombiTrans(xx,yy,zz,zeroRotation));

  // Add Iron endpoints
  TGeoRotation *rot1 = new TGeoRotation();
  rot1->RotateX(0.);
  rot1->RotateY(90.);
  rot1->RotateZ(0.);
  xx = 105.;
  yy = 0.;
  zz = 0.;
  aLand->AddNode(trap,2,new TGeoCombiTrans(xx,yy,zz,rot1));

  TGeoRotation *rot2 = new TGeoRotation();
  rot2->RotateX(0.);
  rot2->RotateY(270.);
  rot2->RotateZ(0.);
  xx = -105.;
  yy = 0.;
  zz = 0.;
  aLand->AddNode(trap,3,new TGeoCombiTrans(xx,yy,zz,rot2));

//  AddSensitiveVolume(padle_h_box3); //Fe
//  AddSensitiveVolume(padle_h_box4); //Fe
  AddSensitiveVolume(padle_h_box5); //Scint.

  fNbOfSensitiveVol+=1; //? FIXME

  //------------------ Paddle assembly -------------------------------------
  // Iron end paddles
  xx = 0.;
  yy = 0.;
  zz = -4.875;
  aLand->AddNode(padle_h_box3,1,new TGeoCombiTrans(xx,yy,zz,zeroRotation));
  zz =  4.875;
  aLand->AddNode(padle_h_box3,2,new TGeoCombiTrans(xx,yy,zz,zeroRotation));

  // Iron padddles
  xx = 0.;
  yy = 0.;
  zz = -4.;
  for (int i = 1; i < 10; i++)
  {
    aLand->AddNode(padle_h_box4,i,new TGeoCombiTrans(xx,yy,zz,zeroRotation));
    zz+=1;
  }

  // BC408 paddles
  xx = 0.;
  yy = 0.;
  zz = -4.5;
  for (int i = 1; i < 11; i++)
  {
    aLand->AddNode(padle_h_box5,i,new TGeoCombiTrans(xx,yy,zz,zeroRotation));
    zz+=1;
  }

  //----------------------- Assembly ------------------------
  TGeoVolume *cell = new TGeoVolumeAssembly("CELL");

  TGeoRotation *rot4 = new TGeoRotation();
  rot4->RotateX(0.);
  rot4->RotateY(0.);
  rot4->RotateZ(90.);

  tx=0.;
  int nrPaddle=0, nrPlane=0;

  // Assembly includes spacing of 0.1cm on eather side of LAND paddles
  double spacing =0.2;

  // With LAND centerd around (0,0,0) the start of assembly is calculated with spacings
  double startZ=-(10.2 * 10 + 9 * spacing - 10.2)/2;
  double startY=-(10.2 * 20 + 19 * spacing - 10.2)/2;

  for (tz=startZ; nrPlane < 10; tz+=10.2+spacing)
  {
    nrPlane++;
    int j=0;
    for (ty=startY; j < 20; ty+=10.2+spacing)
    {
      j++;
      nrPaddle++;
      if (nrPlane % 2 == 1)
        cell->AddNode(aLand,nrPaddle,new TGeoCombiTrans(tx,ty,tz,zeroRotation));
      else
        cell->AddNode(aLand,nrPaddle,new TGeoCombiTrans(ty,tx,tz,rot4));
    }
  }

  // Add the whole assembly to the world
  tx = 0.0;
  ty = 0.0;
  tz = 0.0; // cm
  TGeoCombiTrans *t0 = new TGeoCombiTrans(tx,ty,tz,zeroRotation);

  vWorld->AddNode(cell,1, GetGlobalPosition(t0) );

  // Now save some basic geometry parameters
  // Get the parameter factory
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  R3BLandDigiPar* par=(R3BLandDigiPar*)(rtdb->getContainer("R3BLandDigiPar"));
  par->SetMaxPaddle(200) ;
  par->SetMaxPlane(10);
  par->SetPaddleLength(padle_h_dim1x);
  par->SetPaddleHeight(padle_h_dim1y);
  par->SetPaddleDepth(padle_h_dim1z);
  par->SetPaddleSpacing(spacing);
  par->setChanged();
  //par->setInputVersion(fRun->GetRunId(),1);
}

// --------------------------------------------------------------------------------
// Construct NeuLand Geometry
// --------------------------------------------------------------------------------
void R3BLand::ConstructNeuLandGeometry(  TGeoVolume* vWorld,  TGeoMedium *Aluminium, 
                                         TGeoMedium *BC408, TGeoMedium *CH2)
{
  Double_t tx,ty,tz;

  //------------------ BC408 paddles -----------------------------------------
  TGeoVolume *padle_h_box5 = gGeoManager->MakeBox("padle_h_box5",BC408,
        neuLAND_paddle_dimx, neuLAND_paddle_dimy, neuLAND_paddle_dimz);

  //------------------ wrapping Alu------------------------------------------
  TGeoShape* padle_h_box1 = new TGeoBBox("padle_h_box1",
      neuLAND_paddle_dimx, 
      neuLAND_paddle_dimy+neuLAND_wrapping1_dim, 
      neuLAND_paddle_dimz+neuLAND_wrapping1_dim);
  TGeoShape* padle_h_box2 = new TGeoBBox("padle_h_box2",
       neuLAND_paddle_dimx, 
       neuLAND_paddle_dimy, 
       neuLAND_paddle_dimz);

  // Create a composite shape
  TGeoCompositeShape *wrapping1 = new TGeoCompositeShape("diffbox", "padle_h_box1 - padle_h_box2");
  TGeoVolume *bvol1 = new TGeoVolume("wrapping1",wrapping1,Aluminium);

  //------------------ wrapping Tape------------------------------------------
  TGeoShape* padle_h_box3 = new TGeoBBox("padle_h_box3",
      neuLAND_paddle_dimx, 
      neuLAND_paddle_dimy+neuLAND_wrapping1_dim+neuLAND_wrapping2_dim, 
      neuLAND_paddle_dimz+neuLAND_wrapping1_dim+neuLAND_wrapping2_dim);
  TGeoShape* padle_h_box4 = new TGeoBBox("padle_h_box4",
       neuLAND_paddle_dimx, 
       neuLAND_paddle_dimy+neuLAND_wrapping1_dim, 
       neuLAND_paddle_dimz+neuLAND_wrapping1_dim);

  // Create a composite shape
  TGeoCompositeShape *wrapping2 = new TGeoCompositeShape("diffbox", "padle_h_box3 - padle_h_box4");
  TGeoVolume *bvol2 = new TGeoVolume("wrapping2",wrapping2,CH2);



  // Make the elementary assembly of the whole structure
  TGeoVolume *aLand = new TGeoVolumeAssembly("ALAND");

  Double_t total_dimx=neuLAND_paddle_dimx;
  Double_t total_dimy=neuLAND_paddle_dimy+neuLAND_wrapping1_dim+neuLAND_wrapping2_dim+neuLAND_gap_dim;
  Double_t total_dimz=neuLAND_paddle_dimz+neuLAND_wrapping1_dim+neuLAND_wrapping2_dim+neuLAND_gap_dim;
  
  //paddles
  TGeoRotation *zeroRotation= new TGeoRotation();
  zeroRotation->RotateX(0.);
  zeroRotation->RotateY(0.);
  zeroRotation->RotateZ(0.);

  TGeoRotation *rot1 = new TGeoRotation();
  rot1->RotateX(0.);
  rot1->RotateY(0.);
  rot1->RotateZ(90.);

  Double_t xx = 0.;
  Double_t yy = 0.;
  Double_t zz = 0.;
  
  aLand->AddNode(padle_h_box5,1,new TGeoCombiTrans(xx,yy,zz,zeroRotation));
  aLand->AddNode(bvol1,1,new TGeoCombiTrans(xx,yy,zz,zeroRotation));
  aLand->AddNode(bvol2,1,new TGeoCombiTrans(xx,yy,zz,zeroRotation));

  AddSensitiveVolume(padle_h_box5); //Scint.

  fNbOfSensitiveVol+=1; //? FIXME

// positioning of the paddles for packed geometry

  TGeoVolume *cell = new TGeoVolumeAssembly("CELL");

  int nindex=0, i=0;
  tx=0.;
  tz=-neuLAND_depth_dim + total_dimz;
  for (tz=-neuLAND_depth_dim+total_dimz; tz < neuLAND_depth_dim; tz+=total_dimz*2)
  {
    i++;
    for (ty=-total_dimx + total_dimy; ty < total_dimx; ty+=total_dimy*2)
    {
      nindex++;
      if (i % 2 == 1)
        cell->AddNode(aLand,nindex,new TGeoCombiTrans(tx,ty,tz,zeroRotation));
      else
        cell->AddNode(aLand,nindex,new TGeoCombiTrans(ty,tx,tz,rot1));
    }
  }




// positioning of the paddles for loose geometry
/*
  TGeoVolume *cell = new TGeoVolumeAssembly("CELL");

  int nindex=0, i=0;
  tx=0.;
  tz=-neuLAND_depth_dim + total_dimz;
  for (tz=-neuLAND_depth_dim+total_dimz; tz < neuLAND_depth_dim; tz+=total_dimz*2)
  {
    i++;
    // first 10 paddles in a plane have double distance 
    for (ty=-total_dimx + total_dimy; ty < -50.; ty+=total_dimy*4)
    {
      nindex++;
      if (i % 2 == 1)
        cell->AddNode(aLand,nindex,new TGeoCombiTrans(tx,ty,tz,zeroRotation));
      else
        cell->AddNode(aLand,nindex,new TGeoCombiTrans(ty,tx,tz,rot1));
    }
    // middle 20 paddles in a plane are closed packed 
    for (ty=-19.*total_dimy; ty < 50.; ty+=total_dimy*2)
    {
      nindex++;
      if (i % 2 == 1)
        cell->AddNode(aLand,nindex,new TGeoCombiTrans(tx,ty,tz,zeroRotation));
      else
        cell->AddNode(aLand,nindex,new TGeoCombiTrans(ty,tx,tz,rot1));
    }
    // last 10 paddles in a plane have double distance 
    for (ty=23.*total_dimy; ty < total_dimx; ty+=total_dimy*4)
    {
      nindex++;
      if (i % 2 == 1)
        cell->AddNode(aLand,nindex,new TGeoCombiTrans(tx,ty,tz,zeroRotation));
      else
        cell->AddNode(aLand,nindex,new TGeoCombiTrans(ty,tx,tz,rot1));
    }

    
  }

*/


  tx = 0.0;
  ty = 0.0;
  tz = 0.0; // cm
  TGeoCombiTrans *t0 = new TGeoCombiTrans(tx,ty,tz,zeroRotation);

  vWorld->AddNode(cell,1, GetGlobalPosition(t0) );

  // Now save some basic geometry parameters
  // Get the parameter factory
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  R3BLandDigiPar* par=(R3BLandDigiPar*)(rtdb->getContainer("R3BLandDigiPar"));
  par->SetMaxPaddle(nindex) ;
  par->SetMaxPlane((Int_t)(neuLAND_depth_dim/total_dimy));
  par->SetPaddleLength(neuLAND_paddle_dimx);
  par->setChanged();
  //par->setInputVersion(fRun->GetRunId(),1);
}


void R3BLand::StepHistory(){
  // <DB> dump of step info.
  static Int_t iStepN;

  // Particle being tracked
  const char *sParticle;
  switch(gMC->TrackPid()){
    case 2212:          sParticle="proton"    ;break;
    case 2112:          sParticle="neutron"   ;break;
    case 22:            sParticle="gamma"     ;break;
    case 50000050:      sParticle="ckov"      ;break;
    case 111:           sParticle="pi0"       ;break;  
    case 211:           sParticle="pi+"       ;break;  
    case -211:          sParticle="Pi-"       ;break;  
    case 1000010020:            sParticle="deuteron"        ;break;
    case 1000010030:            sParticle="triton"        ;break;
    case 1000020030:            sParticle="he3"        ;break;
    case 1000020040:            sParticle="alpha"        ;break;
    default:            sParticle="not known" ;break;
  }


  TString flag="-I- STEPINFO: tracking status: ";
  if(gMC->IsTrackAlive()) {
    if(gMC->IsTrackEntering())      flag="enters to";
    else if(gMC->IsTrackExiting())  flag="exits from";
    else if(gMC->IsTrackInside())   flag="inside";
  } else {
    if(gMC->IsTrackStop())          flag="stopped in";
  }
  
  //Where am i ?
  Int_t vid=0,copy=0;
  //current volume and his mother are always there
  TString path=gMC->CurrentVolName(); path.Prepend("-");path.Prepend(gMC->CurrentVolOffName(1));
  vid=gMC->CurrentVolOffID(2,copy);  if(vid) {path.Prepend("-");path.Prepend(gMC->VolName(vid));}
  vid=gMC->CurrentVolOffID(3,copy);  if(vid) {path.Prepend("-");path.Prepend(gMC->VolName(vid));}
  
//  printf("step  (nr=%i): %s (%i) %s %s m=%.6f GeV q=%.1f dEdX=%.4f Etot=%.4f \n",
//  iStepN,sParticle,gMC->TrackPid(),flag.Data(),path.Data(),gMC->TrackMass(),gMC->TrackCharge(),gMC->Edep()*1e9,gMC->Etot());
  
  Double_t gMcTrackPos[3]; gMC->TrackPosition(gMcTrackPos[0],gMcTrackPos[1],gMcTrackPos[2]);
  Double_t  gMcTrackPosLoc[3]; gMC->Gmtod(gMcTrackPos,gMcTrackPosLoc,1);
//  printf(" : track Position (MARS) x: %5.3lf, y: %5.3lf, z: %5.3lf (r: %5.3lf) ---> (LOC) x: %5.3f, y: %5.3f, z: %5.3f",
//          gMcTrackPos[0],gMcTrackPos[1],gMcTrackPos[2],
//          TMath::Sqrt(gMcTrackPos[0]*gMcTrackPos[0]+gMcTrackPos[1]*gMcTrackPos[1]+gMcTrackPos[2]*gMcTrackPos[2]),
//          gMcTrackPosLoc[0],gMcTrackPosLoc[1],gMcTrackPosLoc[2]);
  
//  printf("step (nr=%i): tid=%i flags alive=%i disap=%i enter=%i exit=%i inside=%i out=%i stop=%i new=%i",
//		 iStepN,  gMC->GetStack()->GetCurrentTrackNumber(),
//		 gMC->IsTrackAlive(), gMC->IsTrackDisappeared(),gMC->IsTrackEntering(), gMC->IsTrackExiting(),
//		 gMC->IsTrackInside(),gMC->IsTrackOut(),        gMC->IsTrackStop(),     gMC->IsNewTrack());
  
  Float_t a,z,den,rad,abs; a=z=den=rad=abs=-1;
  Int_t mid=gMC->CurrentMaterial(a,z,den,rad,abs);
//  printf("step (nr=%i): mid=%i a=%7.2f z=%7.2f den=%9.4f rad=%9.2f abs=%9.2f\n\n",iStepN,mid,a,z,den,rad,abs);

/*  
  TArrayI proc;  gMC->StepProcesses(proc); 
  printf("-I- STEPINFO: Processes in this step:\n");
  for ( int i = 0 ; i < proc.GetSize(); i++)
  {
    printf("-I- Process involved --->   code: %i : name: %s\n", proc.At(i) , TMCProcessName[proc.At(i)]);
  }
  printf("-I- STEPINFO: end of process list\n");  
*/

  Int_t Nprim=gMC->GetStack()->GetNprimary();

  Int_t trackNo=gMC->GetStack()->GetCurrentTrackNumber();
  Int_t motherNo=gMC->GetStack()->GetCurrentParentTrackNumber();
  Double_t time=gMC->TrackTime();

/*
  cout << "Prim: " << Nprim<<endl;
  cout << "TrackID: " << trackNo<<endl;
  cout << "MotherID: " << motherNo <<endl;
  cout << "time: " << time <<endl;
  cout << "PiD: "<<gMC->TrackPid()<<endl;
*/  

  if(gMC->TrackPid()==2112 && motherNo<0){
  // we have a primary neutron
     TArrayI proc;  gMC->StepProcesses(proc); 
     for ( int i = 0 ; i < proc.GetSize(); i++){
//        printf("-I- Process involved --->   code: %i : name: %s\n", proc.At(i) , TMCProcessName[proc.At(i)]);
     
        if(proc.At(i)!=22 && proc.At(i)!=23 && proc.At(i)!=31 && proc.At(i)!=43 &&  proc.At(i)!=13){
           cout<<"new primary neutron interaction: " << proc.At(i) <<"  "<< TMCProcessName[proc.At(i)] <<endl;
	
	}
  
// make histogram with first interaction
  
        if(proc.At(i)==22 && trackNo<7){
	   // elastic scattering
//           cout<<"primary neutron interaction elastic"<<endl;
           firstHitX[trackNo-1] = gMcTrackPos[0];
           firstHitY[trackNo-1] = gMcTrackPos[1];
           firstHitZ[trackNo-1] = gMcTrackPos[2];
           firstT[trackNo-1] = time;
  
        }
	
        if(proc.At(i)==13 && trackNo<7){
	   // elastic scattering
//           cout<<"primary neutron interaction hadronic"<<endl;
           firstHitX[trackNo-1] = gMcTrackPos[0];
           firstHitY[trackNo-1] = gMcTrackPos[1];
           firstHitZ[trackNo-1] = gMcTrackPos[2];
           firstT[trackNo-1] = time;
  
        }

        if(proc.At(i)==23 && trackNo<7){
	   // elastic scattering
//           cout<<"primary neutron interaction inelastic"<<endl;
           firstHitX[trackNo-1] = gMcTrackPos[0];
           firstHitY[trackNo-1] = gMcTrackPos[1];
           firstHitZ[trackNo-1] = gMcTrackPos[2];
           firstT[trackNo-1] = time;
  
        }
     }  
  }
  
  iStepN++;

}

void R3BLand::FinishRun() {cout << "-I- finishing  run"<< endl;}



ClassImp(R3BLand)
