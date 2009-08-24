// -------------------------------------------------------------------------
// -----                        R3BChimera source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include "R3BChimera.h"

#include "R3BGeoChimera.h"
#include "R3BChimeraPoint.h"
#include "R3BGeoChimeraPar.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairStack.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairVolume.h"
#include "FairGeoMedia.h"


#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TObjArray.h"
#include "TGeoVoxelFinder.h"
#include "TGeoMatrix.h"

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
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;



// -----   Default constructor   -------------------------------------------
R3BChimera::R3BChimera() : R3BDetector("R3BChimera", kTRUE, kTOF) {
  ResetParameters();
  fChimeraCollection = new TClonesArray("R3BChimeraPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fCutE=1.0e-03; // 1 MeV default
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BChimera::R3BChimera(const char* name, Bool_t active) 
  : R3BDetector(name, active, kTOF) {
  ResetParameters();
  fChimeraCollection = new TClonesArray("R3BChimeraPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fCutE=1.0e-03; // 1MeV default
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BChimera::~R3BChimera() {

  if ( flGeoPar ) delete flGeoPar;
  if (fChimeraCollection) {
    fChimeraCollection->Delete();
    delete fChimeraCollection;
  }
}
// -------------------------------------------------------------------------


void R3BChimera::Initialize()
{
  FairDetector::Initialize();

   cout << endl;
   cout << "-I- R3BChimera: initialisation" << endl;
   cout << "-I- R3BChimera: Sci. Vol. (McId) " << gMC->VolId("TOFLog")<< endl;

}


void R3BChimera::SetSpecialPhysicsCuts(){

   cout << endl;

   cout << "-I- R3BChimera: Adding customized Physics cut ... " << endl;

   if (gGeoManager) {
     TGeoMedium* pSi = gGeoManager->GetMedium("plasticForTOF");
     if ( pSi ) {
      // Setting processes for Si only
	 gMC->Gstpar(pSi->GetId()  ,"LOSS",3);
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
	Double_t cutE = fCutE; // GeV-> 1 keV

	cout << "-I- R3bChimera Scintillator Medium Id " << pSi->GetId()
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
Bool_t R3BChimera::ProcessHits(FairVolume* vol) {

  // Simple Det PLane

    if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Set additional parameters at exit of active volume. Create R3BChimeraPoint.
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid();
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
     // TGeoNode *bla = gGeoManager->FindNextBoundary(2);
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

    AddHit(fTrackID, fVolumeID,
	   TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	   TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	   TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	   TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	   fTime, fLength, fELoss);
    
    // Increment number of ChimeraPoints for this track
    FairStack* stack = (FairStack*) gMC->GetStack();
    stack->AddPoint(kTOF);
    
    ResetParameters();
  }

  return kTRUE;
}
// ----------------------------------------------------------------------------
//void R3BChimera::SaveGeoParams(){
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
void R3BChimera::BeginEvent() {

//  if (! kGeoSaved ) {
//      SaveGeoParams();
//  cout << "-I STS geometry parameters saved " << endl;
//  kGeoSaved = kTRUE;
//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BChimera::EndOfEvent() {

  if (fVerboseLevel) Print();
  fChimeraCollection->Clear();

  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void R3BChimera::Register() {
  FairRootManager::Instance()->Register("TOFPoint", GetName(), fChimeraCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BChimera::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fChimeraCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BChimera::Print() const {
  Int_t nHits = fChimeraCollection->GetEntriesFast();
  cout << "-I- R3BChimera: " << nHits << " points registered in this event." 
       << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BChimera::Reset() {
  fChimeraCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BChimera::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- R3BChimera: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  R3BChimeraPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (R3BChimeraPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BChimeraPoint(*oldpoint);
    fPosIndex++;
  }
   cout << " -I- R3BChimera: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BChimeraPoint* R3BChimera::AddHit(Int_t trackID, Int_t detID, TVector3 posIn,
			    TVector3 posOut, TVector3 momIn, 
			    TVector3 momOut, Double_t time, 
			    Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fChimeraCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- R3BChimera: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss*1e06 << " keV" << endl;
  return new(clref[size]) R3BChimeraPoint(trackID, detID, posIn, posOut,
				      momIn, momOut, time, length, eLoss);
}






// -----   Public method ConstructGeometry   ----------------------------------
void R3BChimera::ConstructGeometry() {

   TString fileName=GetGeometryFileName();
   if (fileName.EndsWith(".geo")) {
      ConstructASCIIGeometry();
   } else if(fileName.EndsWith(".root")) {
      ConstructRootGeometry();
   } else {
      std::cout<< "Geometry format not supported " <<std::endl;
   }


}

void R3BChimera::ConstructRootGeometry() {

    TGeoVolume *Cave=NULL;
    if (gGeoManager) {
	Cave = gGeoManager->GetTopVolume();
    }else{
	cout << "-I- no TGEO defined ! " << endl;
    }


   TFile *f=new TFile(GetGeometryFileName().Data());
  
   TGeoVolume *chimerTop=(TGeoVolume *)f->Get("chimera_log");

   //***
    TGeoNode* n = chimerTop->GetNode(0);

   TGeoVoxelFinder *voxels = chimerTop->GetVoxels();
   if (voxels) voxels->SetNeedRebuild();
    TGeoMatrix *M = n->GetMatrix();
    M->SetDefaultName();
    gGeoManager->GetListOfMatrices()->Remove(M);
    TGeoHMatrix *global = gGeoManager->GetHMatrix();             
    gGeoManager->GetListOfMatrices()->Remove(global); //Remove the Identity matrix 

   ExpandNode(chimerTop,Cave);
 
}

void R3BChimera::ExpandNode(TGeoVolume *fVol, TGeoVolume *Cave){

   fVerboseLevel=3;
   FairGeoLoader*geoLoad = FairGeoLoader::Instance();
   FairGeoInterface *geoFace = geoLoad->getGeoInterface();
   FairGeoMedia *Media =  geoFace->getMedia();
   FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();
  
   TObjArray *nodeList=fVol->GetNodes();
   if(fVerboseLevel>2)
     std::cout<< "DEBUG NodeListEntries = " << nodeList->GetEntries() << std::endl;
   
   for (Int_t nod=0; nod < nodeList->GetEntries(); nod++) {
     
     if(fVerboseLevel>2)   std::cout<< "DEBUG nod = " << nod << std::endl;
     TGeoNode *fNode =(TGeoNode *)nodeList->At(nod);
     TGeoVolume *v= fNode->GetVolume();
     if(fNode->GetNdaughters()>0) 	
       ExpandNode(v, Cave);
     
     TGeoMedium* med1=v->GetMedium();
     if(fVerboseLevel>2) std::cout<< "DEBUG NodeName = " << fNode->GetName() << std::endl;
     if (med1) {
       if(fVerboseLevel>2) std::cout<< "DEBUG medium  = " << med1->GetName() << std::endl;
       TGeoMaterial*mat1=v->GetMaterial(); 
       TGeoMaterial *newMat = gGeoManager->GetMaterial(mat1->GetName());
       if (newMat==0) {
	 std::cout<< "Material " << mat1->GetName() << " is not defined " << std::endl;
	 FairGeoMedium *pMedium=Media->getMedium(mat1->GetName());
	 if (!pMedium) {
	   std::cout << "Material is not defined in ASCII file nor in Root file" << std::endl;
	   pMedium=new FairGeoMedium(mat1->GetName());
	   Media->addMedium(pMedium);
	 }
	 std::cout << "Create Medium " << mat1->GetName() << std::endl;
	 Int_t nmed=geobuild->createMedium(pMedium);
	 v->SetMedium(gGeoManager->GetMedium(nmed));
	 gGeoManager->SetAllIndex();
       } else {
	  if(fVerboseLevel>2)  
	    std::cout<< "DEBUG material was defined  MaterialName= " << mat1->GetName() << std::endl;
	  TGeoMedium *med2= gGeoManager->GetMedium(mat1->GetName());
	  v->SetMedium(med2);
       }
     }
     if (!gGeoManager->FindVolumeFast(v->GetName())) {
       if(fVerboseLevel>2) std::cout<< "DEBUG registration form"  << std::endl;
       v->RegisterYourself();
     }


     //only dchVolume added to the Node list of Cave
     TString name = v->GetName();
     if(fVerboseLevel>2) std::cout<< "DEBUG VolumeName "  <<name<< std::endl;

     // Add node to Cave
     Cave->AddNode(v,0, fNode->GetMatrix());

      // Declare Sentivity
     if ( (name.Contains("Si"))  || ( name.Contains("CsI") )  ) {
	 if (nod==0) {
	     if(fVerboseLevel>2) std::cout<< "DEBUG AddSensitive "  <<name<< std::endl;
	     AddSensitiveVolume(v);
	 }
     }//! Sensitive

   }
}




ClassImp(R3BChimera)
