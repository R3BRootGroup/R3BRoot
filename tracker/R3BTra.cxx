// -------------------------------------------------------------------------
// -----                        R3BTra source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include "R3BTra.h"

#include "R3BGeoTra.h"
#include "R3BTraPoint.h"
#include "R3BGeoTraPar.h"
#include "R3BMCStack.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
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
#include "TGeoTube.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include "TGeoShapeAssembly.h"
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

// -----   Default constructor   -------------------------------------------
R3BTra::R3BTra() : R3BDetector("R3BTra", kTRUE, kTRA) {
	ResetParameters();
	fTraCollection = new TClonesArray("R3BTraPoint");
	fPosIndex = 0;
	kGeoSaved = kFALSE;
	flGeoPar = new TList();
	flGeoPar->SetName( GetName());
	fVerboseLevel = 1;
	fCutE=1.0e-04; // 100keV default
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BTra::R3BTra(const char* name, Bool_t active) 
	: R3BDetector(name, active, kTRA) {
		ResetParameters();
		fTraCollection = new TClonesArray("R3BTraPoint");
		fPosIndex = 0;
		kGeoSaved = kFALSE;
		flGeoPar = new TList();
		flGeoPar->SetName( GetName());
		fVerboseLevel = 1;
		fCutE=1.0e-04; // 100keV default
	}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
R3BTra::~R3BTra() {

	if ( flGeoPar ) delete flGeoPar;
	if (fTraCollection) {
		fTraCollection->Delete();
		delete fTraCollection;
	}
}
// -------------------------------------------------------------------------

// ----   Initialize   -----------------------------------------------------

void R3BTra::Initialize()
{
	FairDetector::Initialize();

	LOG(INFO) << "R3BTra: initialisation" << FairLogger::endl;
	LOG(DEBUG) << "R3BTra: Sens. Vol. (McId) " << gMC->VolId("TraLog") << FairLogger::endl;
}


void R3BTra::SetSpecialPhysicsCuts()
{
	LOG(INFO) << "R3BTra: Adding customized Physics cut ... " << FairLogger::endl;

	if (gGeoManager) {

		TGeoMedium* pSi = gGeoManager->GetMedium("Silicon");
		if ( pSi ) {
			// Setting processes for Si only
			gMC->Gstpar(pSi->GetId()  ,"LOSS",1);
			gMC->Gstpar(pSi->GetId()  ,"STRA",1.0);
			gMC->Gstpar(pSi->GetId()  ,"PAIR",1.0);
			gMC->Gstpar(pSi->GetId()  ,"COMP",1.0);
			gMC->Gstpar(pSi->GetId()  ,"PHOT",1.0);
			gMC->Gstpar(pSi->GetId()  ,"ANNI",1.0);
			gMC->Gstpar(pSi->GetId()  ,"BREM",1.0);
			gMC->Gstpar(pSi->GetId()  ,"HADR",1.0);
			gMC->Gstpar(pSi->GetId()  ,"DRAY",1.0);
			gMC->Gstpar(pSi->GetId()  ,"DCAY",1.0);
			gMC->Gstpar(pSi->GetId()  ,"MULS",1.0);
			gMC->Gstpar(pSi->GetId()  ,"RAYL",1.0);

			// Setting Energy-CutOff for Si Only
			Double_t cutE = fCutE; // GeV-> 100 keV

			LOG(INFO) << "R3BTra Silicon Medium Id " << pSi->GetId()
			<< " Energy Cut-Off : " << cutE
			<< FairLogger::endl;

			//Si
			gMC->Gstpar(pSi->GetId(),"CUTGAM",cutE);   /** gammas (GeV)*/
			gMC->Gstpar(pSi->GetId(),"CUTELE",cutE);   /** electrons (GeV)*/
			gMC->Gstpar(pSi->GetId(),"CUTNEU",cutE);   /** neutral hadrons (GeV)*/
			gMC->Gstpar(pSi->GetId(),"CUTHAD",cutE);   /** charged hadrons (GeV)*/
			gMC->Gstpar(pSi->GetId(),"CUTMUO",cutE);   /** muons (GeV)*/
			gMC->Gstpar(pSi->GetId(),"BCUTE" ,cutE);    /** electron bremsstrahlung (GeV)*/
			gMC->Gstpar(pSi->GetId(),"BCUTM" ,cutE);    /** muon and hadron bremsstrahlung(GeV)*/
			gMC->Gstpar(pSi->GetId(),"DCUTE" ,cutE);    /** delta-rays by electrons (GeV)*/
			gMC->Gstpar(pSi->GetId(),"DCUTM" ,cutE);    /** delta-rays by muons (GeV)*/
			gMC->Gstpar(pSi->GetId(),"PPCUTM",-1.);   /** direct pair production by muons (GeV)*/

		}
		// <DB> trick to remove too much internal
		// tracking in the Aladin magnet yoke
		TGeoMedium* pFe = gGeoManager->GetMedium("Iron");

		if ( pFe ) {
			Double_t cutM= 1.e-01; // 100 MeV
			gMC->Gstpar(pFe->GetId(),"CUTELE",cutM);
			gMC->Gstpar(pFe->GetId()  ,"DRAY",0.0);
		}

	} //!gGeoManager


}

// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BTra::ProcessHits(FairVolume* vol) {

	//      cout << " -I process hit called for:" <<  vol->GetName() << endl;
	// Set parameters at entrance of volume. Reset ELoss.

	//    if ( vol ) {
	//        cout << " Name Id:copy "
	//            << vol->getName() << " : " << vol->getMCid() << " : " << vol->getCopyNo() << endl;
	//        Int_t copyNo=0;
	//        cout << " Geant: " << gMC->CurrentVolID(copyNo) << ":" << copyNo << endl;
	//    }

	if ( gMC->IsTrackEntering() ) {
		fELoss  = 0.;
		fTime   = gMC->TrackTime() * 1.0e09;
		fLength = gMC->TrackLength();
		gMC->TrackPosition(fPosIn);
		gMC->TrackMomentum(fMomIn);
		//cout << "X,Y,X tracker=" << fPosIn(0) << " " << fPosIn(1) << " " << fPosIn(2) << endl;
		//cout << "track length=" << fLength << endl;
	}

	// Sum energy loss for all steps in the active volume
	fELoss += gMC->Edep();
	//cout << "Tracker Eloss=" << fELoss << "  " << gMC->Edep() << endl;

	// Set additional parameters at exit of active volume. Create R3BTraPoint.
	if ( gMC->IsTrackExiting()    ||
			gMC->IsTrackStop()       ||
			gMC->IsTrackDisappeared()   ) {
		fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
		fVolumeID = vol->getMCid();
		fDetCopyID = vol->getCopyNo();  // added by Marc
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
				LOG(ERROR) << "2nd direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2]
				<< " with safety = " << safety << FairLogger::endl;
				LOG(ERROR) << "oldpos = " << oldpos[0] << "," << oldpos[1] << "," << oldpos[2] << FairLogger::endl;
				LOG(ERROR) << "newpos = " << newpos[0] << "," << newpos[1] << "," << newpos[2] << FairLogger::endl;
			}

			fPosOut.SetX(newpos[0]);
			fPosOut.SetY(newpos[1]);
			fPosOut.SetZ(newpos[2]);
		}

		AddHit(fTrackID, fVolumeID, fDetCopyID, // added by Marc
				TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
				TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
				TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
				TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
				fTime, fLength, fELoss);

		// Increment number of TraPoints for this track
		R3BStack* stack = (R3BStack*) gMC->GetStack();
		stack->AddPoint(kTRA);

		ResetParameters();
	}

	return kTRUE;
}
// ----------------------------------------------------------------------------
//void R3BTra::SaveGeoParams(){
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
void R3BTra::BeginEvent() {

	//  cout << "-I- begin tracker event called ##########" << endl;

	if(gGeoManager){
		TGeoVolume * vol=gGeoManager->FindVolumeFast("TraLog");

		if(vol){
			//    cout << "id tracker serial number : " << vol->GetNumber() << endl;
		}
	}

	//  if (! kGeoSaved ) {
	//      SaveGeoParams();
	//  cout << "-I STS geometry parameters saved " << endl;
	//  kGeoSaved = kTRUE;
	//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BTra::EndOfEvent() {

	if (fVerboseLevel) Print();
	fTraCollection->Clear();

	ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void R3BTra::Register() {
	FairRootManager::Instance()->Register("TraPoint", GetName(), fTraCollection, kTRUE);
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BTra::GetCollection(Int_t iColl) const {
	if (iColl == 0) return fTraCollection;
	else return NULL;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void R3BTra::Print(Option_t *option) const {
	Int_t nHits = fTraCollection->GetEntriesFast();
	LOG(INFO) << "R3BTra: " << nHits << " points registered in this event" << FairLogger::endl;
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void R3BTra::Reset() {
	fTraCollection->Clear();
	ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method CopyClones   -----------------------------------------
void R3BTra::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
	Int_t nEntries = cl1->GetEntriesFast();
	LOG(INFO) << "R3BTra: " << nEntries << " entries to add" << FairLogger::endl;
	TClonesArray& clref = *cl2;
	R3BTraPoint* oldpoint = NULL;
	for (Int_t i=0; i<nEntries; i++) {
		oldpoint = (R3BTraPoint*) cl1->At(i);
		Int_t index = oldpoint->GetTrackID() + offset;
		oldpoint->SetTrackID(index);
		new (clref[fPosIndex]) R3BTraPoint(*oldpoint);
		fPosIndex++;
	}
	LOG(INFO) << "R3BTra: " << cl2->GetEntriesFast() << " merged entries"	<< FairLogger::endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BTraPoint* R3BTra::AddHit(Int_t trackID, Int_t detID, Int_t detCopyID,   // added by Marc 
		TVector3 posIn,
		TVector3 posOut, TVector3 momIn, 
		TVector3 momOut, Double_t time, 
		Double_t length, Double_t eLoss) {
	TClonesArray& clref = *fTraCollection;
	Int_t size = clref.GetEntriesFast();
	if (fVerboseLevel>1) 
		LOG(INFO) << "R3BTra: Adding Point at (" << posIn.X() << ", " << posIn.Y()
		<< ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
		<< trackID << ", energy loss " << eLoss*1e06 << " keV" << FairLogger::endl;
	return new(clref[size]) R3BTraPoint(trackID, detID, detCopyID, posIn, posOut,  // detCopyID added by Marc
			momIn, momOut, time, length, eLoss);
}


// -----   Public method ConstructGeometry   ----------------------------------
void R3BTra::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")) {
    LOG(INFO) << "Constructing TRACKER geometry from ROOT file " << fileName.Data() << FairLogger::endl;
    ConstructRootGeometry();
  } else {
    LOG(FATAL) << "TRACKER Geometry file is not specified" << FairLogger::endl;
  }
}


Bool_t R3BTra::CheckIfSensitive(std::string name)
{
  if(TString(name).Contains("TraLog")) {
    return kTRUE;
  }
  return kFALSE;
}


ClassImp(R3BTra)
