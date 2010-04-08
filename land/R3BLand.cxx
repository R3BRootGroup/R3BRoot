// -------------------------------------------------------------------------
// -----								R3BLand source file							-----
// -----						Created 26/03/09  by D.Bertini					-----
// -------------------------------------------------------------------------
#include "R3BLand.h"

#include "R3BGeoLand.h"
#include "R3BLandPoint.h"
#include "R3BGeoLandPar.h"
#include "R3BLandDigiPar.h"

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



// -----	Default constructor	-------------------------------------------
R3BLand::R3BLand() : R3BDetector("R3BLand", kTRUE, kLAND) {
	ResetParameters();
	fLandCollection = new TClonesArray("R3BLandPoint");
	fPosIndex = 0;
	kGeoSaved = kFALSE;
	flGeoPar = new TList();
	flGeoPar->SetName( GetName());
	fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----	Standard constructor	------------------------------------------
R3BLand::R3BLand(const char* name, Bool_t active) 
	: R3BDetector(name, active, kLAND) {
	ResetParameters();
	fLandCollection = new TClonesArray("R3BLandPoint");
	fPosIndex = 0;
	kGeoSaved = kFALSE;
	flGeoPar = new TList();
	flGeoPar->SetName( GetName());
	fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----	Destructor	----------------------------------------------------
R3BLand::~R3BLand() {

	if ( flGeoPar ) delete flGeoPar;
	if (fLandCollection) {
	 fLandCollection->Delete();
	 delete fLandCollection;
	}
}
// -------------------------------------------------------------------------

void R3BLand::Initialize()
{

	FairDetector::Initialize();

	cout << "-I- R3BLand: initialisation " << endl;
	cout << "-I- R3BLand: Paddle B3 (McId): " << gMC->VolId("padle_h_box3") << endl;
	cout << "-I- R3BLand: Paddle B4 (McId): " << gMC->VolId("padle_h_box4") << endl;
	cout << "-I- R3BLand: Paddle B5 (McId): " << gMC->VolId("padle_h_box5") << endl;

 Int_t id1 = gMC->VolId("padle_h_box3");
 Int_t id2 = gMC->VolId("padle_h_box4");
 Int_t id3 = gMC->VolId("padle_h_box5");

 // Sensitive Volumes :: Unique  Id
 //  paddle_h_box3			 1
 //  paddle_h_box4			 2
 //  paddle_h_box5			 3

 fMapMcId[id1]=1;
 fMapMcId[id2]=2;
 fMapMcId[id3]=3;

 //<D.B> Check me
 // Initialise variables from Birk law
 Double_t dP = 1.032 ;
 // Set constants for Birk's Law implentation
 fBirkC0 =  1;
 fBirkC1 =  0.013/dP;
 fBirkC2 =  9.6e-6/(dP * dP);

 useNeuLAND=false;

}


// -----	Public method ProcessHits  --------------------------------------
Bool_t R3BLand::ProcessHits(FairVolume* vol) {

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
	 fTime	= gMC->TrackTime() * 1.0e09;
	 fLength = gMC->TrackLength();
	 gMC->TrackPosition(fPosIn);
	 gMC->TrackMomentum(fMomIn);
	}

	// Sum energy loss for all steps in the active volume
	fELoss += gMC->Edep();


	// Set additional parameters at exit of active volume. Create R3BLandPoint.
	if ( gMC->IsTrackExiting()	 ||
		 gMC->IsTrackStop()		 ||
		 gMC->IsTrackDisappeared()	)
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
		
	//		 cout << "1st direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2] << endl;

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

		// Apply Birk's law ( Adapted from G3BIRK/Geant3)
		Double_t lightYield =  fELoss ;
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
				lightYield=lightYield/(1.+birkC1Mod*dedxcm+fBirkC2*dedxcm*dedxcm);
			}
		}


		AddHit(fTrackID, fVolumeID,	fPaddleTyp,	cp2, cp1,
			TVector3(fPosIn.X(),	fPosIn.Y(),	fPosIn.Z()),
			TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
			TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
			TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
			fTime, fLength, fELoss, lightYield);
		 
		 // Increment number of LandPoints for this track
 		FairStack* stack = (FairStack*) gMC->GetStack();
		stack->AddPoint(kLAND);
		 
		ResetParameters();
	}

	return kTRUE;
}




// -----	Public method EndOfEvent	-----------------------------------------
void R3BLand::BeginEvent() {
	 //cout << "-I- begin event called ############################# " << endl;

//	 if (gGeoManager)
//	 TGeoVolume * vol = gGeoManager->FindVolumeFast("padle_h_box3");
}
// -----	Public method EndOfEvent	-----------------------------------------
void R3BLand::EndOfEvent() {

	if (fVerboseLevel) Print();
	fLandCollection->Clear();

	ResetParameters();
}
// ----------------------------------------------------------------------------

// -----	Public method Register	-------------------------------------------
void R3BLand::Register() {
	FairRootManager::Instance()->Register("LandPoint", GetName(), fLandCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----	Public method GetCollection	--------------------------------------
TClonesArray* R3BLand::GetCollection(Int_t iColl) const {
	if (iColl == 0) return fLandCollection;
	else return NULL;
}
// ----------------------------------------------------------------------------



// -----	Public method Print	----------------------------------------------
void R3BLand::Print() const {
	Int_t nHits = fLandCollection->GetEntriesFast();
	cout << "-I- R3BLand: " << nHits << " points registered in this event." 
		 << endl;
}
// ----------------------------------------------------------------------------



// -----	Public method Reset	----------------------------------------------
void R3BLand::Reset() {
	fLandCollection->Clear();
	ResetParameters();
}
// ----------------------------------------------------------------------------



// -----	Public method CopyClones	-----------------------------------------
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
	cout << " -I- R3BLand: " << cl2->GetEntriesFast() << " merged entries."
		 << endl;
}

// -----	Private method AddHit	--------------------------------------------
R3BLandPoint* R3BLand::AddHit(Int_t trackID, Int_t detID, Int_t box, Int_t id1, Int_t id2,
					TVector3 posIn,
					TVector3 posOut, TVector3 momIn,
					TVector3 momOut, Double_t time,
					Double_t length, Double_t eLoss, Double_t lightYield) {
	TClonesArray& clref = *fLandCollection;
	Int_t size = clref.GetEntriesFast();
	if (fVerboseLevel>1) 
	 cout << "-I- R3BLand: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss  << " GeV" << endl;
	return new(clref[size]) R3BLandPoint(trackID, detID, box, id1, id2,  posIn, posOut,
						momIn, momOut, time, length, eLoss, lightYield);
}

// -----	Public method UseNeuLand	----------------------------------
void R3BLand::UseNeuLand(Double_t paddle_length, Double_t paddle_width, Double_t paddle_depth, Double_t neuLAND_depth)
{
		useNeuLAND=true;
		neuLAND_paddle_dimx = paddle_length;		// half of the length [cm]
		neuLAND_paddle_dimy = paddle_width;			// half of the width [cm]
		neuLAND_paddle_dimz = paddle_depth;			// half of the depth [cm]
		neuLAND_depth_dim   = neuLAND_depth;		// total detector depth [cm]
}

// -----	Public method ConstructGeometry	----------------------------------
void R3BLand::ConstructGeometry() {

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
		w		 =		  0.;
		a		 = 55.850000;
		z		 = 26.000000;
		density = 7.870000;
		radl	 = 1.757717;
		absl	 = 169.994418;
		TGeoMaterial*
	pMatFe = new TGeoMaterial("Iron", a,z,density,radl,absl);
		pMatFe->SetIndex(701);
		numed	= 23;  // medium number
		Double_t par[8];
		par[0]  = 0.000000; // isvol
		par[1]  = 0.000000; // ifield
		par[2]  = 0.000000; // fieldm
		par[3]  = 0.000000; // tmaxfd
		par[4]  = 0.000000; // stemax
		par[5]  = 0.000000; // deemax
		par[6]  = 0.000100; // epsil
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
		nel	  = 2;
		density = 1.032000;
		TGeoMixture*
	 pMat37 = new TGeoMixture("BC408", nel,density);
		a = 1.007940;	z = 1.000000;	w = 0.520000;  // H
		pMat37->DefineElement(0,a,z,w);
		a = 12.010700;	z = 6.000000;	w = 0.480000;  // C
		pMat37->DefineElement(1,a,z,w);
		pMat37->SetIndex(36);
		// Medium: BC408
		numed	= 36;  // medium number
		Double_t par[8];
		par[0]  = 0.000000; // isvol
		par[1]  = 0.000000; // ifield
		par[2]  = 0.000000; // fieldm
		par[3]  = 0.000000; // tmaxfd
		par[4]  = 0.000000; // stemax
		par[5]  = 0.000000; // deemax
		par[6]  = 0.000100; // epsil
		par[7]  = 0.000000; // stmin
		pMed37 = new TGeoMedium("BC408", numed,pMat37,par);
	}

	TGeoVolume* vWorld = gGeoManager->GetTopVolume();
	vWorld->SetVisLeaves(kTRUE);

	//Switch between Land and NeuLand
	if (useNeuLAND)
		ConstructNeuLandGeometry(vWorld, Iron, pMed37);
	else
		ConstructLandGeometry(vWorld, Iron, pMed37);

}

// --------------------------------------------------------------------------------
// Construct Land Geometry
// --------------------------------------------------------------------------------
void R3BLand::ConstructLandGeometry(	TGeoVolume* vWorld,	TGeoMedium *Iron, TGeoMedium *BC408)
{
	Double_t dx1, dx2, dy1, dy2, dz;
	Double_t tx,ty,tz;

	// SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
	//------------------ Iron shell ------------------------------------------
	Double_t padle_h_dim1x = 100.0;
	Double_t padle_h_dim1y = 5.1;
	Double_t padle_h_dim1z = 5.1;
	TGeoShape* padle_h_box1 = new TGeoBBox("padle_h_box1",
					 padle_h_dim1x, padle_h_dim1y,	padle_h_dim1z);
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

	AddSensitiveVolume(padle_h_box3); //Fe
	AddSensitiveVolume(padle_h_box4); //Fe
	AddSensitiveVolume(padle_h_box5); //Scint.

	fNbOfSensitiveVol+=3; //? FIXME

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
	int nindex=0, i=0;
	for (tz=-45.9; i < 10; tz+=10.2)
	{
		i++;
		int j=0;
		for (ty=-96.9; j < 20; ty+=10.2)
		{
			j++;
			nindex++;
			if (i % 2 == 1)
				cell->AddNode(aLand,nindex,new TGeoCombiTrans(tx,ty,tz,zeroRotation));
			else
				cell->AddNode(aLand,nindex,new TGeoCombiTrans(ty,tx,tz,rot4));
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
	par->setChanged();
	//par->setInputVersion(fRun->GetRunId(),1);
}

// --------------------------------------------------------------------------------
// Construct NeuLand Geometry
// --------------------------------------------------------------------------------
void R3BLand::ConstructNeuLandGeometry(	TGeoVolume* vWorld,	TGeoMedium *Iron, TGeoMedium *BC408)
{
	Double_t tx,ty,tz;

	//------------------ BC408 paddles -----------------------------------------
	TGeoVolume *padle_h_box5 = gGeoManager->MakeBox("padle_h_box5",BC408,
												neuLAND_paddle_dimx, neuLAND_paddle_dimy, neuLAND_paddle_dimz);

	// Make the elementary assembly of the whole structure
	TGeoVolume *aLand = new TGeoVolumeAssembly("ALAND");

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

	AddSensitiveVolume(padle_h_box5); //Scint.

	fNbOfSensitiveVol+=1; //? FIXME

	TGeoVolume *cell = new TGeoVolumeAssembly("CELL");

	int nindex=0, i=0;
	tx=0.;
	tz=-neuLAND_depth_dim + neuLAND_paddle_dimz;
	for (tz=-neuLAND_depth_dim+neuLAND_paddle_dimz; tz < neuLAND_depth_dim; tz+=neuLAND_paddle_dimz*2)
	{
		i++;
		for (ty=-neuLAND_paddle_dimx + neuLAND_paddle_dimy; ty < neuLAND_paddle_dimx; ty+=neuLAND_paddle_dimy*2)
		{
			nindex++;
			if (i % 2 == 1)
				cell->AddNode(aLand,nindex,new TGeoCombiTrans(tx,ty,tz,zeroRotation));
			else
				cell->AddNode(aLand,nindex,new TGeoCombiTrans(ty,tx,tz,rot1));
		}
	}

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
		par->SetMaxPlane((Int_t)neuLAND_depth_dim/neuLAND_paddle_dimy);
	par->setChanged();
	//par->setInputVersion(fRun->GetRunId(),1);
}

ClassImp(R3BLand)
