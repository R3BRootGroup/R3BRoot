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
//paolo 30062010
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
//paolo 30062010

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
     TGeoMedium* pSi = gGeoManager->GetMedium("CsI");
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
  FairRootManager::Instance()->Register("ChimeraPoint", GetName(), fChimeraCollection, kTRUE);
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
//      ConstructRootGeometry();
   } else {
      std::cout<< "Geometry format not supported " <<std::endl;
   }
      ConstructMechGeometry();


}

void R3BChimera::ConstructRootGeometry() {

    TGeoVolume *Cave=NULL;
    if (gGeoManager) {
	Cave = gGeoManager->GetTopVolume();
    }else{
	cout << "-I- no TGEO defined ! " << endl;
    }

   TGeoVolume *cell = new TGeoVolumeAssembly("CHIM");

  // Rotation (Unity)
   TGeoRotation *pRot = new TGeoRotation();
   pRot->RotateY(180.0);

   Cave->AddNode(cell,0,
                   GetGlobalPosition(
		   new TGeoCombiTrans("",0.,0.,0.,pRot)
                   )
		  ); 



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

   ExpandNode(chimerTop,cell);
 
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



void R3BChimera::ConstructMechGeometry() {
//01/06/2010 paolo
  TGeoVolume *Cave=NULL;
  if (gGeoManager) {
   Cave = gGeoManager->GetTopVolume();
  }else
  {
   cout << "-I- no TGEO defined ! " << endl;
  }
  // out-of-file geometry definition
  Double_t dx,dy,dz;
  Double_t dx1, dx2, dy1, dy2;
  Double_t a;
  Double_t z, density, w;
  Double_t tx,ty,tz;
  Int_t nel, numed;
  Double_t radl,absl;

/****************************************************************************/
// Material definition

// Mixture: Air
  TGeoMedium * pMed2=NULL;
  if (gGeoManager->GetMedium("Air") ){
     pMed2=gGeoManager->GetMedium("Air");
  }
  else{
   nel     = 2;
   density = 0.001290;
   TGeoMixture *pMat2 = new TGeoMixture("Air", nel,density);
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
//------------------Creat media----------------------------------
// --  Material: Iron
  TGeoMedium * pMedFe=NULL;
  if (gGeoManager->GetMedium("Iron") ){
   pMedFe=gGeoManager->GetMedium("Iron");
  }
  else{
   w       =        0.;
   a       = 55.850000;
   z       = 26.000000;
   density = 7.870000;
   radl    = 1.757717;
   absl    = 169.994418;
   TGeoMaterial* pMatFe = new TGeoMaterial("Iron", a,z,density,radl,absl);
   pMatFe->SetIndex(701);
   numed   = 23;  // medium number
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
// --  Material: Aluminium
  TGeoMedium * pMedAl=NULL;
  if (gGeoManager->GetMedium("aluminium") ){
   pMedAl=gGeoManager->GetMedium("aluminium");
  }
  else{
   w       =        0.;
   a       = 26.980000;
   z       = 13.000000;
   density = 2.7000000;
   radl    = 8.875105;
   absl    = 388.793113;
   TGeoMaterial*pMatAL = new TGeoMaterial("Aluminium", a,z,density,radl,absl);
   Double_t par[8];
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000100; // epsil
   par[7]  = 0.000000; // stmin
   pMedAl = new TGeoMedium("Aluminium", numed,pMatAL, par);
  }
  TGeoMedium *Aluminium = pMedAl;

// BC408 plastic medium
// Mixture: BC408
  TGeoMedium * pMed37=NULL;
  if (gGeoManager->GetMedium("BC408") ){
   pMed37=gGeoManager->GetMedium("BC408");
  }
  else{
   nel     = 2;
   density = 1.032000;
   TGeoMixture *pMat37 = new TGeoMixture("BC408", nel,density);
   a = 1.007940;   z = 1.000000;   w = 0.520000;  // H
   pMat37->DefineElement(0,a,z,w);
   a = 12.010700;   z = 6.000000;   w = 0.480000;  // C
   pMat37->DefineElement(1,a,z,w);
   pMat37->SetIndex(36);
// Medium: BC408
   numed   = 36;  // medium number
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
// Mixture: Concrete
  nel=6;
  density = 2.5;
  TGeoMixture *pMat444 = new TGeoMixture("CONCRETE",nel,density);
  a = 15.999400;   z =  8.000000;   w = 0.530000;  // O
  pMat444->DefineElement(0,a,z,w);
  a = 22.990000;   z = 11.000000;   w = 0.150E-01;  // Na
  pMat444->DefineElement(1,a,z,w);
  a = 26.980000;   z = 13.000000;   w = 0.420E-01;  // Al
  pMat444->DefineElement(2,a,z,w);
  a = 28.090000;   z = 14.000000;   w = 0.330000;  // Si
  pMat444->DefineElement(3,a,z,w);
  a = 40.078000;   z = 20.000000;   w = 0.630E-01;  // O
  pMat444->DefineElement(4,a,z,w);
  a = 55.850000;   z = 26.000000;   w = 0.200E-01;  // O
  pMat444->DefineElement(5,a,z,w);
  pMat444->SetIndex(443);
  numed=443;
  Double_t par[8];
  par[0]  = 0.000000; // isvol
  par[1]  = 0.000000; // ifield
  par[2]  = 0.000000; // fieldm
  par[3]  = 0.000000; // tmaxfd
  par[4]  = 0.000000; // stemax
  par[5]  = 0.000000; // deemax
  par[6]  = 0.000100; // epsil
  par[7]  = 0.000000; // stmin
  TGeoMedium *pMedConcrete= new TGeoMedium("CONCRETE",numed,pMat444,par);

// Mixture: CsI
  TGeoMedium * pMed9=NULL;
   if (gGeoManager->GetMedium("CsIn") ){
       pMed9=gGeoManager->GetMedium("CsIn");
  }else{
    nel     = 2;
    density = 4.510000;
    TGeoMixture*
        pMat9 = new TGeoMixture("CsIn", nel,density);
    a = 132.905450;   z = 55.000000;   w = 0.511549;  // CS
    pMat9->DefineElement(0,a,z,w);
    a = 126.904470;   z = 53.000000;   w = 0.488451;  // I
    pMat9->DefineElement(1,a,z,w);
    pMat9->SetIndex(601);
//    Double_t par[8];
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 0.000000; // tmaxfd
    par[4]  = 0.000000; // stemax
    par[5]  = 0.000000; // deemax
    par[6]  = 0.000100; // epsil
    par[7]  = 0.000000; // stmin
    pMed9 = new TGeoMedium("CsIn", 2,pMat9, par);
  }
// Material: Silicon
   a       = 28.090000;
   z       = 14.000000;
   density = 2.330000;
   radl    = 9.351106;
   absl    = 456.628489;
   TGeoMaterial*   pMat22 = new TGeoMaterial("Silicon", a,z,density,radl,absl);
   pMat22->SetIndex(21);
// Medium: Silicon
   numed   = 21;  // medium number
   TGeoMedium*
   pMed22 = new TGeoMedium("Silicon", numed,pMat22, par);
/*
  TGeoMixture *mat4 = new TGeoMixture("CONCRETE",6,   2.50000    );
  mat4->SetUniqueID(  4);
  mat4->DefineElement(0,16,8,0.53);
  mat4->DefineElement(1,28.09,14,0.33);
  mat4->DefineElement(2,40.078,20,0.6300000E-01);
  mat4->DefineElement(3,22.99,11,0.1500000E-01);
  mat4->DefineElement(4,55.85,26,0.2000000E-01);
  mat4->DefineElement(5,26.98,13,0.4200000E-01);
  TGeoMedium *pMedConcrete= new TGeoMedium("CONCRETE",5,4,0,0,0,20,0.1000000E+11,0.1880915,0.1000000E-03,0.2108371E-01);
*/
//07/06/2010 giusto questo
  Double_t myDeltaZ=9.5;

  TGeoVolume *BaseLat     = gGeoManager->MakeBox("ChimSupp",pMedAl, 7.5 ,7.5,85.); //1 m3
  TGeoTranslation *Trans6dx=new TGeoTranslation("Trans6dx", 82.5,   -192.5,225.-myDeltaZ);
  TGeoTranslation *Trans6sx=new TGeoTranslation("Trans6sx",-82.5,   -192.5,225.-myDeltaZ);
  Cave->AddNode(BaseLat ,  0,Trans6dx);
  Cave->AddNode(BaseLat ,  0,Trans6sx);

  TGeoVolume *Base= gGeoManager->MakeBox("ChimSupp",pMedAl, 90. ,7.5,7.5); //1 m3
  TGeoTranslation *Trans7fr=new TGeoTranslation("Trans7fr", 0.,   -192.5,147.5-myDeltaZ);
  Cave->AddNode(Base ,  0,Trans7fr);
  TGeoTranslation *Trans7ps=new TGeoTranslation("Trans7ps", 0.,   -192.5,302.5-myDeltaZ);
  Cave->AddNode(Base ,  0,Trans7ps);
  TGeoTranslation *Trans8fr=new TGeoTranslation("Trans8fr", 0.,   -111.,147.5-myDeltaZ);
  Cave->AddNode(Base ,  0,Trans8fr);
  TGeoTranslation *Trans8ps=new TGeoTranslation("Trans8ps", 0.,   -111.,302.5-myDeltaZ);
  Cave->AddNode(Base ,  0,Trans8ps);

  TGeoVolume *Lato     = gGeoManager->MakeBox("ChimSupp",pMedAl, 7.5 ,89.,7.5); //1 m3
  TGeoTranslation *Trans3dx=new TGeoTranslation("Trans3dx",  82.5,   -111.,147.5-myDeltaZ);
  Cave->AddNode(Lato ,  0,Trans3dx);
  TGeoTranslation *Trans3sx=new TGeoTranslation("Trans3sx", -82.5,   -111.,147.5-myDeltaZ);
  Cave->AddNode(Lato ,  0,Trans3sx);
   TGeoTranslation *Trans4dx=new TGeoTranslation("Trans4dx",  82.5,   -111.,225.-myDeltaZ);
  Cave->AddNode(Lato ,  0,Trans4dx);
  TGeoTranslation *Trans4sx=new TGeoTranslation("Trans4sx", -82.5,   -111.,225.-myDeltaZ);
  Cave->AddNode(Lato ,  0,Trans4sx);
  TGeoTranslation *Trans5dx=new TGeoTranslation("Trans5dx",  82.5,   -111.,302.5-myDeltaZ);
  Cave->AddNode(Lato ,  0,Trans5dx);
  TGeoTranslation *Trans5sx=new TGeoTranslation("Trans5sx", -82.5,   -111.,302.5-myDeltaZ);
  Cave->AddNode(Lato ,  0,Trans5sx);

  TGeoVolume *Rotaia    = gGeoManager->MakeBox("ChimSupp",pMedFe,  7.5 ,7.5,85.); //1 m3
  TGeoTranslation *Trans7dx=new TGeoTranslation("Trans7dx",  82.5,   -14.5,225.-myDeltaZ);
  Cave->AddNode(Rotaia ,  0,Trans7dx);
  TGeoTranslation *Trans7sx=new TGeoTranslation("Trans7sx", -82.5,   -14.5,225.-myDeltaZ);
  Cave->AddNode(Rotaia ,  0,Trans7sx);

  TGeoVolume *ArmRing7  = gGeoManager->MakeBox("ChimSupp",pMedFe,100.,7.,7.); //1 m3
  TGeoTranslation *TransAR7=new TGeoTranslation("TransAR7", 0.,   0.,147.);
//  Cave->AddNode(ArmRing7 ,  0,TransAR7);

  TGeoVolume *ArmRing6  = gGeoManager->MakeBox("ChimSupp",pMedFe,100.,7.,7.); //1 m3
  TGeoTranslation *TransAR6=new TGeoTranslation("TransAR6", 0.,   0.,167.);
//  Cave->AddNode(ArmRing6 ,  0,TransAR6);

  TGeoVolume *ArmRing5  = gGeoManager->MakeBox("ChimSupp",pMedFe,100.,7.,7.); //1 m3
  TGeoTranslation *TransAR5=new TGeoTranslation("TransAR5", 0.,   0.,197.);
//  Cave->AddNode(ArmRing5 ,  0,TransAR5);

  TGeoVolume *ArmRing4  = gGeoManager->MakeBox("ChimSupp",pMedFe,100.,7.,7.); //1 m3
  TGeoTranslation *TransAR4=new TGeoTranslation("TransAR4", 0.,   0.,217.);
//  Cave->AddNode(ArmRing4 ,  0,TransAR4);
//07/06/2010 giusto questo

  TGeoVolume *Wallpav    = gGeoManager->MakeBox("Wall",pMedConcrete, 850.,100.,1000.); //1 m3
  TGeoVolume *Walltet    = gGeoManager->MakeBox("Wall",pMedConcrete, 850.,100.,1000.); //1 m3
  TGeoVolume *Walldx     = gGeoManager->MakeBox("Wall" ,pMedConcrete, 100.,550.,1000.); //1 m3
  TGeoVolume *Wallsx     = gGeoManager->MakeBox("Wall" ,pMedConcrete, 100.,550.,1000.); //1 m3
  TGeoVolume *Wallfr     = gGeoManager->MakeBox("Wall", pMedConcrete,1050.,550.,100.); //1 m3

  TGeoTranslation *Transdown = new TGeoTranslation("Transdown",   0., -300.,  0.);
  TGeoTranslation *Transup   = new TGeoTranslation("Transup",   0.,  600.,    0.);
  TGeoTranslation *Transdx   = new TGeoTranslation("Transdx", 950.,  150.  ,  0.);
  TGeoTranslation *Transsx   = new TGeoTranslation("Transsx",-950.,  150.  ,  0.);
  TGeoTranslation *Transfr   = new TGeoTranslation("Transfr",   0.,  150.  ,1100.);

//   Cave->AddNode(Wallpav, 0,Transdown);
//   Cave->AddNode(Walltet, 0,Transup);
//   Cave->AddNode(Walldx,  0,Transdx);
//   Cave->AddNode(Wallsx,  0,Transsx);
//   Cave->AddNode(Wallfr,  0,Transfr);

//29/06/2010 aggiungo chimera piece by piece
//metto tutto in cm
  Double_t degtorad= 0.01745329;

  Int_t nmod[9] = {16,24,32,40,40,48,48,48,48};
  Double_t thetamin[9] = {1.0,2.6,4.6,7.,10.,13.,16.,20.,24.};
  Double_t thetamax[9] = {2.6,4.6,7.,10.,13.,16.,20.,24.,30.};
  Double_t dist[9] = {350,300,250,210,180,160,140,120,100};

  Double_t newdist[9] = {350,300,250,210,180,160,140,120,100};

  Int_t ii,i;

  Double_t halfthick = 6.;
  Double_t detheta;
  Double_t dephi;
  Double_t wheel_dz; 
  Double_t wheel_theta; 
  Double_t wheel_phi; 
  Double_t wheel_dy1; 
  Double_t wheel_dy2; 
  Double_t wheel_alp1 = 0; 
  Double_t wheel_alp2 = 0; 
  Double_t wheel_dx1; 
  Double_t wheel_dx2; 
  Double_t wheel_dx3; 
  Double_t wheel_dx4; 
  Double_t thetamean; 
  Double_t thetamean_deg; 
  Double_t modwheelPos_x = 0;
  Double_t modwheelPos_y;
  Double_t modwheelPos_z;
//*******************************************************************
  Double_t sihalfthick = 0.015;
  Double_t siwheel_dz; 
  Double_t siwheel_theta; 
  Double_t siwheel_phi; 
  Double_t siwheel_dy1; 
  Double_t siwheel_dy2; 
  Double_t siwheel_alp1 = 0; 
  Double_t siwheel_alp2 = 0; 
  Double_t siwheel_dx1; 
  Double_t siwheel_dx2; 
  Double_t siwheel_dx3; 
  Double_t siwheel_dx4; 
  Double_t siwheelPos_x = 0;
  Double_t siwheelPos_y;
  Double_t siwheelPos_z;
//*******************************************************************
  Int_t pp=0;
//  for(i=wmin-1;i<=wmax-1;i++)

  for(i=0;i<9;i++)
  {
   TGeoRotation* rotwheel = new TGeoRotation("rotwheel",0.,0.,0.);
//   cout <<" out 1  "<< thetamin[i]<<"   "<<thetamax[i]<<"   "<<i<<endl;
   thetamean_deg=(thetamax[i]+thetamin[i])/2;
   thetamean = degtorad*thetamean_deg;
   detheta = (thetamax[i]-thetamin[i])*degtorad/2;
   dephi = degtorad*360./nmod[i];
   wheel_dz = halfthick ; 
   wheel_theta = 0.; 
   wheel_phi = 0.; 
//   cout <<" out 2  "<< thetamean_deg<<"   "<<dephi/degtorad<<"   "<<detheta/degtorad<<"   "<<wheel_dz<<endl;
   wheel_dy1 = dist[i]*tan(detheta); 
   wheel_dy2 = (dist[i]+2*halfthick)*tan(detheta); 
//   cout <<" out 3  "<<wheel_dy1<<"   "<<wheel_dy2<<"   "<<endl;
   wheel_alp1 = 0; 
   wheel_alp2 = 0; 
   wheel_dx1 = 0.5*(dist[i]/cos(detheta))*sin(thetamin[i]*degtorad)*tan(dephi); 
   wheel_dx2 = 0.5*(dist[i]/cos(detheta))*sin(thetamax[i]*degtorad)*tan(dephi); 
   wheel_dx3 = 0.5*((dist[i]+2*halfthick)/cos(detheta))*sin(thetamin[i]*degtorad)*tan(dephi); 
   wheel_dx4 = 0.5*((dist[i]+2*halfthick)/cos(detheta))*sin(thetamax[i]*degtorad)*tan(dephi); 
//   wheel_dx1 = 0.5*(dist[i]*tan(thetamean)-wheel_dy1)*tan(dephi); 
//   wheel_dx2 = 0.5*(dist[i]*tan(thetamean)+wheel_dy1)*tan(dephi); 
//   wheel_dx3 = 0.5*((dist[i]+2*halfthick)*tan(thetamean)-wheel_dy2)*tan(dephi); 
//   wheel_dx4 = 0.5*((dist[i]+2*halfthick)*tan(thetamean)+wheel_dy2)*tan(dephi); 
//   cout <<" out 4  "<<wheel_dx1<<"   "<<wheel_dx2<<"   "<<wheel_dx3<<"   "<<wheel_dx4<<endl;
   modwheelPos_x = 0.;
//   cout <<" out 5  "<<(dist[i]+halfthick)<<"   "<<sin(thetamean)<<"   "<<cos(thetamean)<<endl;
   modwheelPos_y = (dist[i]+halfthick)*sin(thetamean);
   modwheelPos_z = (newdist[pp]+halfthick)*cos(thetamean);
//   cout <<" out 6  "<<modwheelPos_x<<"   "<<modwheelPos_y<<"   "<<modwheelPos_z<<endl;
   rotwheel->RotateX(thetamean_deg);
   rotwheel->RotateY(0.);
   rotwheel->RotateZ(0.);
//*******************************************************************
   TGeoRotation* sirotwheel = new TGeoRotation("sirotwheel",0.,0.,0.);
//   G4cout <<"   "<< thetamin[i]<<"   "<<thetamax[i]<<"   "<<i<<'\n';
//   thetamean_deg=(thetamax[i]+thetamin[i])/2;
//   thetamean = degrad*thetamean_deg;
//   detheta = (thetamax[i]-thetamin[i])*degrad/2;
//   dephi = degrad*360./nmod[i];
   siwheel_dz = sihalfthick ; 
   siwheel_theta = 0.; 
   siwheel_phi = 0.; 
//   G4cout <<"   "<< thetamean_deg<<"   "<<dephi/degrad<<"   "<<detheta/degrad<<"   "<<wheel_dz<<'\n';
   siwheel_dy1 = (dist[i]-0.5)*tan(detheta); 
   siwheel_dy2 = ((dist[i]-0.5)+2*sihalfthick)*tan(detheta); 
//   cout <<" out 7  "<<siwheel_dy1<<"   "<<siwheel_dy2<<"   "<<endl;
   siwheel_alp1 = 0; 
   siwheel_alp2 = 0; 
   siwheel_dx1 = 0.5*((dist[i]-0.5)/cos(detheta))*sin(thetamin[i]*degtorad)*tan(dephi); 
   siwheel_dx2 = 0.5*((dist[i]-0.5)/cos(detheta))*sin(thetamax[i]*degtorad)*tan(dephi); 
   siwheel_dx3 = 0.5*(((dist[i]-0.5)+2*sihalfthick)/cos(detheta))*sin(thetamin[i]*degtorad)*tan(dephi); 
   siwheel_dx4 = 0.5*(((dist[i]-0.5)+2*sihalfthick)/cos(detheta))*sin(thetamax[i]*degtorad)*tan(dephi); 
//   wheel_dx1 = 0.5*(dist[i]*tan(thetamean)-wheel_dy1)*tan(dephi); 
//   wheel_dx2 = 0.5*(dist[i]*tan(thetamean)+wheel_dy1)*tan(dephi); 
//   wheel_dx3 = 0.5*((dist[i]+2*halfthick)*tan(thetamean)-wheel_dy2)*tan(dephi); 
//   wheel_dx4 = 0.5*((dist[i]+2*halfthick)*tan(thetamean)+wheel_dy2)*tan(dephi); 
//   cout <<" out 8  "<<siwheel_dx1<<"   "<<siwheel_dx2<<"   "<<siwheel_dx3<<"   "<<siwheel_dx4<<endl;
   siwheelPos_x = 0.;
//   cout <<" out 9  "<<((dist[i]-0.5)+sihalfthick)<<"   "<<sin(thetamean)<<"   "<<cos(thetamean)<<endl;
   siwheelPos_y = ((dist[i]-0.5)+sihalfthick)*sin(thetamean);
   siwheelPos_z = ((newdist[pp]-0.5)+sihalfthick)*cos(thetamean);
//    cout <<" out 10  "<<siwheelPos_x<<"   "<<siwheelPos_y<<"   "<<siwheelPos_z<<endl;
//    cout <<"attenzione qui al -0.01"<<'\n';
//    cout <<"attenzione qui"<<'\n';
//    cout <<"attenzione qui"<<'\n';
//    cout <<"attenzione qui"<<'\n';
//    cout <<"attenzione qui"<<'\n';
//    cout <<"attenzione qui"<<'\n';
//    cout <<"attenzione qui"<<'\n';
//    cout <<"attenzione qui"<<'\n';
//    cout <<"attenzione qui"<<'\n';

   sirotwheel->RotateX(thetamean_deg);
   sirotwheel->RotateY(0.);
   sirotwheel->RotateZ(0.);
   pp=pp+1;

//*******************************************************************
// BEGIN RING 1------------------------------------------------------------------
   Int_t useCsI=1;
   Int_t useSi=1;
   if(i==0)
   {
    if(useCsI)
    {
     TGeoVolume *modwheel1_box = gGeoManager->MakeTrap("ModWheel1",pMed9, 
                          wheel_dz ,wheel_theta,wheel_phi,
                          wheel_dy1,wheel_dx1  ,wheel_dx2,wheel_alp1,
   			  wheel_dy2,wheel_dx3  ,wheel_dx4,wheel_alp2); //1 m3    
     modwheel1_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(modwheel1_box);
     for(ii=0;ii<nmod[i];ii++)
     {
      TGeoRotation* rotwheel1 = new TGeoRotation("rotwheel1",0.,0.,0.);
      rotwheel1->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      modwheelPos_x=(dist[i]+halfthick)*sin(thetamean)*sin(ii*dephi);
      modwheelPos_y=(dist[i]+halfthick)*sin(thetamean)*cos(ii*dephi);
//      cout<<"gino   "<<ii<<"  "<<modwheelPos_x<<"   "<<modwheelPos_y<<"   "<<modwheelPos_z<<endl;
//      cout<<"tano1"<<endl;
      TGeoCombiTrans* rotwheelcomb1 = new TGeoCombiTrans("rotwheelcomb1", -modwheelPos_x,modwheelPos_y,modwheelPos_z,rotwheel1);
      if(ii==0)Cave->AddNode(modwheel1_box ,  1000+ii,rotwheelcomb1);
//      if(ii==0)Cave->AddNode(modwheel1_box ,  1000+ii,rotwheelcomb1);
//      if(ii==2)Cave->AddNode(modwheel1_box ,  1000+ii,rotwheelcomb1);
//      if(ii==4)Cave->AddNode(modwheel1_box ,  1000+ii,rotwheelcomb1);
     }
    }
    if(useSi)
    {
     TGeoVolume *siwheel1_box = gGeoManager->MakeTrap("SiWheel1",pMed22, 
                          siwheel_dz,siwheel_theta,siwheel_phi,
                          wheel_dy1 ,siwheel_dx1  ,siwheel_dx2,siwheel_alp1,
   			  wheel_dy2 ,siwheel_dx3  ,siwheel_dx4,siwheel_alp2); //1 m3    
     siwheel1_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(siwheel1_box);
     for(ii=0;ii<nmod[i];ii++)
     {
      TGeoRotation* sirotwheel1 = new TGeoRotation("sirotwheel1",0.,0.,0.);
      sirotwheel1->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      siwheelPos_x=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*sin(ii*dephi);
      siwheelPos_y=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*cos(ii*dephi);
//      cout<<"gino Si  1 "<<siwheelPos_x<<"   "<<siwheelPos_y<<"   "<<siwheelPos_z<<endl;
      TGeoCombiTrans* sirotwheelcomb1 = new TGeoCombiTrans("sirotwheelcomb1", -siwheelPos_x,siwheelPos_y,siwheelPos_z,sirotwheel1);
      if(ii==0)Cave->AddNode(siwheel1_box ,  0+ii,sirotwheelcomb1);
     }
    }
   }
// END RING 1------------------------------------------------------------------
// BEGIN RING 2------------------------------------------------------------------
   if(i==10)
   {
    if(useCsI)
    {
     TGeoVolume *modwheel2_box = gGeoManager->MakeTrap("ModWheel2",pMed9, 
                          wheel_dz ,wheel_theta,wheel_phi,
                          wheel_dy1,wheel_dx1  ,wheel_dx2,wheel_alp1,
   			  wheel_dy2,wheel_dx3  ,wheel_dx4,wheel_alp2); //1 m3    
     modwheel2_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(modwheel2_box);
     for(ii=0;ii<nmod[i];ii++)
     {
      TGeoRotation* rotwheel2 = new TGeoRotation("rotwheel2",0.,0.,0.);
      rotwheel2->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      modwheelPos_x=(dist[i]+halfthick)*sin(thetamean)*sin(ii*dephi);
      modwheelPos_y=(dist[i]+halfthick)*sin(thetamean)*cos(ii*dephi);
//      cout<<"gino   "<<ii<<"  "<<modwheelPos_x<<"   "<<modwheelPos_y<<"   "<<modwheelPos_z<<endl;
//      cout<<"tano1"<<endl;
      TGeoCombiTrans* rotwheelcomb2 = new TGeoCombiTrans("rotwheelcomb2", -modwheelPos_x,modwheelPos_y,modwheelPos_z,rotwheel2);
      Cave->AddNode(modwheel2_box ,  1032+ii,rotwheelcomb2);
     }
    }
    if(useSi)
    {
     TGeoVolume *siwheel2_box = gGeoManager->MakeTrap("SiWheel2",pMed22, 
                          siwheel_dz,siwheel_theta,siwheel_phi,
                          wheel_dy1 ,siwheel_dx1  ,siwheel_dx2,siwheel_alp1,
   			  wheel_dy2 ,siwheel_dx3  ,siwheel_dx4,siwheel_alp2); //1 m3    
     siwheel2_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(siwheel2_box);
     for(ii=0;ii<nmod[i];ii++)
     {
      TGeoRotation* sirotwheel2 = new TGeoRotation("sirotwheel2",0.,0.,0.);
      sirotwheel2->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      siwheelPos_x=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*sin(ii*dephi);
      siwheelPos_y=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*cos(ii*dephi);
//      cout<<"gino Si  2 "<<siwheelPos_x<<"   "<<siwheelPos_y<<"   "<<siwheelPos_z<<endl;
      TGeoCombiTrans* sirotwheelcomb2 = new TGeoCombiTrans("sirotwheelcomb2", -siwheelPos_x,siwheelPos_y,siwheelPos_z,sirotwheel2);
      Cave->AddNode(siwheel2_box ,  32+ii,sirotwheelcomb2);
     }
    }
   }
// END RING 2------------------------------------------------------------------
// BEGIN RING 3------------------------------------------------------------------
   if(i==20)
   {
    if(useCsI)
    {
     TGeoVolume *modwheel3_box = gGeoManager->MakeTrap("ModWheel3",pMed9, 
                          wheel_dz ,wheel_theta,wheel_phi,
                          wheel_dy1,wheel_dx1  ,wheel_dx2,wheel_alp1,
   			  wheel_dy2,wheel_dx3  ,wheel_dx4,wheel_alp2); //1 m3    
     modwheel3_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(modwheel3_box);
     for(ii=0;ii<nmod[i];ii++)
     {
      TGeoRotation* rotwheel3 = new TGeoRotation("rotwheel3",0.,0.,0.);
      rotwheel3->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      modwheelPos_x=(dist[i]+halfthick)*sin(thetamean)*sin(ii*dephi);
      modwheelPos_y=(dist[i]+halfthick)*sin(thetamean)*cos(ii*dephi);
//      cout<<"gino   "<<ii<<"  "<<modwheelPos_x<<"   "<<modwheelPos_y<<"   "<<modwheelPos_z<<endl;
//      cout<<"tano1"<<endl;
      TGeoCombiTrans* rotwheelcomb3 = new TGeoCombiTrans("rotwheelcomb3", -modwheelPos_x,modwheelPos_y,modwheelPos_z,rotwheel3);
      Cave->AddNode(modwheel3_box ,  1080+ii,rotwheelcomb3);
     }
    }
    if(useSi)
    {
     TGeoVolume *siwheel3_box = gGeoManager->MakeTrap("SiWheel3",pMed22, 
                          siwheel_dz,siwheel_theta,siwheel_phi,
                          wheel_dy1 ,siwheel_dx1  ,siwheel_dx2,siwheel_alp1,
   			  wheel_dy2 ,siwheel_dx3  ,siwheel_dx4,siwheel_alp2); //1 m3    
     siwheel3_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(siwheel3_box);
     for(ii=0;ii<nmod[i];ii++)
     {
      TGeoRotation* sirotwheel3 = new TGeoRotation("sirotwheel3",0.,0.,0.);
      sirotwheel3->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      siwheelPos_x=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*sin(ii*dephi);
      siwheelPos_y=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*cos(ii*dephi);
//      cout<<"gino Si  3"<<siwheelPos_x<<"   "<<siwheelPos_y<<"   "<<siwheelPos_z<<endl;
      TGeoCombiTrans* sirotwheelcomb3 = new TGeoCombiTrans("sirotwheelcomb3", -siwheelPos_x,siwheelPos_y,siwheelPos_z,sirotwheel3);
      Cave->AddNode(siwheel3_box ,  80+ii,sirotwheelcomb3);
     }
    }
   }
// END RING 3------------------------------------------------------------------
// BEGIN RING 4------------------------------------------------------------------
   if(i==3)
   {
    if(useCsI)
    {
     TGeoVolume *modwheel4_box = gGeoManager->MakeTrap("ModWheel4",pMed9, 
                          wheel_dz ,wheel_theta,wheel_phi,
                          wheel_dy1,wheel_dx1  ,wheel_dx2,wheel_alp1,
   			  wheel_dy2,wheel_dx3  ,wheel_dx4,wheel_alp2); //1 m4    
     modwheel4_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(modwheel4_box);
     for(ii=0;ii<nmod[i];ii++)
     {
      TGeoRotation* rotwheel4 = new TGeoRotation("rotwheel4",0.,0.,0.);
      rotwheel4->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      modwheelPos_x=(dist[i]+halfthick)*sin(thetamean)*sin(ii*dephi);
      modwheelPos_y=(dist[i]+halfthick)*sin(thetamean)*cos(ii*dephi);
//      cout<<"gino   "<<ii<<"  "<<modwheelPos_x<<"   "<<modwheelPos_y<<"   "<<modwheelPos_z<<endl;
//      cout<<"tano1"<<endl;
      TGeoCombiTrans* rotwheelcomb4 = new TGeoCombiTrans("rotwheelcomb4", -modwheelPos_x,modwheelPos_y,modwheelPos_z,rotwheel4);
      Cave->AddNode(modwheel4_box ,  1144+ii,rotwheelcomb4);
     }
    }
    if(useSi)
    {
     TGeoVolume *siwheel4_box = gGeoManager->MakeTrap("SiWheel4",pMed22, 
                          siwheel_dz,siwheel_theta,siwheel_phi,
                          wheel_dy1 ,siwheel_dx1  ,siwheel_dx2,siwheel_alp1,
   			  wheel_dy2 ,siwheel_dx3  ,siwheel_dx4,siwheel_alp2); //1 m4    
     siwheel4_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(siwheel4_box);
     for(ii=0;ii<nmod[i];ii++)
     {
      TGeoRotation* sirotwheel4 = new TGeoRotation("sirotwheel4",0.,0.,0.);
      sirotwheel4->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      siwheelPos_x=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*sin(ii*dephi);
      siwheelPos_y=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*cos(ii*dephi);
//      cout<<"gino Si  4 "<<siwheelPos_x<<"   "<<siwheelPos_y<<"   "<<siwheelPos_z<<endl;
      TGeoCombiTrans* sirotwheelcomb4 = new TGeoCombiTrans("sirotwheelcomb4", -siwheelPos_x,siwheelPos_y,siwheelPos_z,sirotwheel4);
      if(ii==  0)Cave->AddNode(siwheel4_box ,  144+ii,sirotwheelcomb4);
      if(ii== 10)Cave->AddNode(siwheel4_box ,  144+ii,sirotwheelcomb4);
      if(ii== 20)Cave->AddNode(siwheel4_box ,  144+ii,sirotwheelcomb4);
      if(ii== 30)Cave->AddNode(siwheel4_box ,  144+ii,sirotwheelcomb4);
     }
    }
   }
// END RING 4------------------------------------------------------------------
// BEGIN RING 5------------------------------------------------------------------
   if(i==4)
   {
    if(useCsI)
    {
     TGeoVolume *modwheel5_box = gGeoManager->MakeTrap("ModWheel5",pMed9, 
                          wheel_dz ,wheel_theta,wheel_phi,
                          wheel_dy1,wheel_dx1  ,wheel_dx2,wheel_alp1,
   			  wheel_dy2,wheel_dx3  ,wheel_dx4,wheel_alp2); //1 m3    
     modwheel5_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(modwheel5_box);
     for(ii=0;ii<nmod[i];ii++)
     {
      TGeoRotation* rotwheel5 = new TGeoRotation("rotwheel5",0.,0.,0.);
      rotwheel5->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      modwheelPos_x=(dist[i]+halfthick)*sin(thetamean)*sin(ii*dephi);
      modwheelPos_y=(dist[i]+halfthick)*sin(thetamean)*cos(ii*dephi);
//      cout<<"gino   "<<ii<<"  "<<modwheelPos_x<<"   "<<modwheelPos_y<<"   "<<modwheelPos_z<<endl;
//      cout<<"tano1"<<endl;
      TGeoCombiTrans* rotwheelcomb5 = new TGeoCombiTrans("rotwheelcomb5", -modwheelPos_x,modwheelPos_y,modwheelPos_z,rotwheel5);
      Cave->AddNode(modwheel5_box ,  1224+ii,rotwheelcomb5);
     }
    }
    if(useSi)
    {
     TGeoVolume *siwheel5_box = gGeoManager->MakeTrap("SiWheel5",pMed22, 
                          siwheel_dz,siwheel_theta,siwheel_phi,
                          wheel_dy1 ,siwheel_dx1  ,siwheel_dx2,siwheel_alp1,
   			  wheel_dy2 ,siwheel_dx3  ,siwheel_dx4,siwheel_alp2); //1 m3    
     siwheel5_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(siwheel5_box);
     for(ii=0;ii<nmod[i];ii++)
     {
      TGeoRotation* sirotwheel5 = new TGeoRotation("sirotwheel5",0.,0.,0.);
      sirotwheel5->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      siwheelPos_x=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*sin(ii*dephi);
      siwheelPos_y=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*cos(ii*dephi);
 //     cout<<"gino Si  5 "<<siwheelPos_x<<"   "<<siwheelPos_y<<"   "<<siwheelPos_z<<endl;
      TGeoCombiTrans* sirotwheelcomb5 = new TGeoCombiTrans("sirotwheelcomb5", -siwheelPos_x,siwheelPos_y,siwheelPos_z,sirotwheel5);
      if(ii== 0)Cave->AddNode(siwheel5_box ,  224+ii,sirotwheelcomb5);
      if(ii==10)Cave->AddNode(siwheel5_box ,  224+ii,sirotwheelcomb5);
      if(ii==20)Cave->AddNode(siwheel5_box ,  224+ii,sirotwheelcomb5);
      if(ii==30)Cave->AddNode(siwheel5_box ,  224+ii,sirotwheelcomb5);
     }
    }
   }
// END RING 5------------------------------------------------------------------
// BEGIN RING 6------------------------------------------------------------------
   if(i==5)
   {
    if(useCsI)
    {
     TGeoVolume *modwheel6_box = gGeoManager->MakeTrap("ModWheel6",pMed9, 
                          wheel_dz ,wheel_theta,wheel_phi,
                          wheel_dy1,wheel_dx1  ,wheel_dx2,wheel_alp1,
   			  wheel_dy2,wheel_dx3  ,wheel_dx4,wheel_alp2); //1 m3    
     modwheel6_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(modwheel6_box);
     for(ii=0;ii<nmod[i];ii++)
     {
      TGeoRotation* rotwheel6 = new TGeoRotation("rotwheel6",0.,0.,0.);
      rotwheel6->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      modwheelPos_x=(dist[i]+halfthick)*sin(thetamean)*sin(ii*dephi);
      modwheelPos_y=(dist[i]+halfthick)*sin(thetamean)*cos(ii*dephi);
 //     cout<<"gino   "<<ii<<"  "<<modwheelPos_x<<"   "<<modwheelPos_y<<"   "<<modwheelPos_z<<endl;
 //     cout<<"tano1"<<endl;
      TGeoCombiTrans* rotwheelcomb6 = new TGeoCombiTrans("rotwheelcomb6", -modwheelPos_x,modwheelPos_y,modwheelPos_z,rotwheel6);
      Cave->AddNode(modwheel6_box ,  1304+ii,rotwheelcomb6);
     }
    }
    if(useSi)
    {
     TGeoVolume *siwheel6_box = gGeoManager->MakeTrap("SiWheel6",pMed22, 
                          siwheel_dz,siwheel_theta,siwheel_phi,
                          wheel_dy1 ,siwheel_dx1  ,siwheel_dx2,siwheel_alp1,
   			  wheel_dy2 ,siwheel_dx3  ,siwheel_dx4,siwheel_alp2); //1 m3    
     siwheel6_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(siwheel6_box);
     for(ii=0;ii<nmod[i];ii++)
     {
      TGeoRotation* sirotwheel6 = new TGeoRotation("sirotwheel6",0.,0.,0.);
      sirotwheel6->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      siwheelPos_x=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*sin(ii*dephi);
      siwheelPos_y=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*cos(ii*dephi);
 //     cout<<"gino Si  6 "<<siwheelPos_x<<"   "<<siwheelPos_y<<"   "<<siwheelPos_z<<endl;
      TGeoCombiTrans* sirotwheelcomb6 = new TGeoCombiTrans("sirotwheelcomb6", -siwheelPos_x,siwheelPos_y,siwheelPos_z,sirotwheel6);
      if(ii== 0)Cave->AddNode(siwheel6_box ,  304+ii,sirotwheelcomb6);
      if(ii==12)Cave->AddNode(siwheel6_box ,  304+ii,sirotwheelcomb6);
      if(ii==24)Cave->AddNode(siwheel6_box ,  304+ii,sirotwheelcomb6);
      if(ii==36)Cave->AddNode(siwheel6_box ,  304+ii,sirotwheelcomb6);
     }
    }
   }
// END RING 6------------------------------------------------------------------
// BEGIN RING 7------------------------------------------------------------------
   if(i==6)
   {
    if(useCsI)
    {
     TGeoVolume *modwheel7_box = gGeoManager->MakeTrap("ModWheel7",pMed9, 
                          wheel_dz ,wheel_theta,wheel_phi,
                          wheel_dy1,wheel_dx1  ,wheel_dx2,wheel_alp1,
   			  wheel_dy2,wheel_dx3  ,wheel_dx4,wheel_alp2); //1 m3    
     modwheel7_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(modwheel7_box);
     for(ii=0;ii<nmod[i];ii++)
     {
      TGeoRotation* rotwheel7 = new TGeoRotation("rotwheel7",0.,0.,0.);
      rotwheel7->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      modwheelPos_x=(dist[i]+halfthick)*sin(thetamean)*sin(ii*dephi);
      modwheelPos_y=(dist[i]+halfthick)*sin(thetamean)*cos(ii*dephi);
 //     cout<<"gino   "<<ii<<"  "<<modwheelPos_x<<"   "<<modwheelPos_y<<"   "<<modwheelPos_z<<endl;
 //     cout<<"tano1"<<endl;
      TGeoCombiTrans* rotwheelcomb7 = new TGeoCombiTrans("rotwheelcomb7", -modwheelPos_x,modwheelPos_y,modwheelPos_z,rotwheel7);
      Cave->AddNode(modwheel7_box ,  1400+ii,rotwheelcomb7);
     }
    }
    if(useSi)
    {
     TGeoVolume *siwheel7_box = gGeoManager->MakeTrap("SiWheel7",pMed22, 
                          siwheel_dz,siwheel_theta,siwheel_phi,
                          wheel_dy1 ,siwheel_dx1  ,siwheel_dx2,siwheel_alp1,
   			  wheel_dy2 ,siwheel_dx3  ,siwheel_dx4,siwheel_alp2); //1 m3    
     siwheel7_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(siwheel7_box);
     for(ii=0;ii<nmod[i];ii++)
     {
      TGeoRotation* sirotwheel7 = new TGeoRotation("sirotwheel7",0.,0.,0.);
      sirotwheel7->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      siwheelPos_x=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*sin(ii*dephi);
      siwheelPos_y=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*cos(ii*dephi);
  //    cout<<"gino Si  7 "<<siwheelPos_x<<"   "<<siwheelPos_y<<"   "<<siwheelPos_z<<endl;
      TGeoCombiTrans* sirotwheelcomb7 = new TGeoCombiTrans("sirotwheelcomb7", -siwheelPos_x,siwheelPos_y,siwheelPos_z,sirotwheel7);
      if(ii== 0)Cave->AddNode(siwheel7_box ,  400+ii,sirotwheelcomb7);
      if(ii== 12)Cave->AddNode(siwheel7_box ,  400+ii,sirotwheelcomb7);
      if(ii== 24)Cave->AddNode(siwheel7_box ,  400+ii,sirotwheelcomb7);
      if(ii== 36)Cave->AddNode(siwheel7_box ,  400+ii,sirotwheelcomb7);
     }
    }
   }
// END RING 7------------------------------------------------------------------
// BEGIN RING 8------------------------------------------------------------------
   if(i==70)
   {
    if(useCsI)
    {
     TGeoVolume *modwheel8_box = gGeoManager->MakeTrap("ModWheel8",pMed9, 
                          wheel_dz ,wheel_theta,wheel_phi,
                          wheel_dy1,wheel_dx1  ,wheel_dx2,wheel_alp1,
   			  wheel_dy2,wheel_dx3  ,wheel_dx4,wheel_alp2); //1 m3    
     modwheel8_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(modwheel8_box);
     for(ii=0;ii<nmod[i];ii++)
     {
      TGeoRotation* rotwheel8 = new TGeoRotation("rotwheel8",0.,0.,0.);
      rotwheel8->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      modwheelPos_x=(dist[i]+halfthick)*sin(thetamean)*sin(ii*dephi);
      modwheelPos_y=(dist[i]+halfthick)*sin(thetamean)*cos(ii*dephi);
  //    cout<<"gino   "<<ii<<"  "<<modwheelPos_x<<"   "<<modwheelPos_y<<"   "<<modwheelPos_z<<endl;
 //     cout<<"tano1"<<endl;
      TGeoCombiTrans* rotwheelcomb8 = new TGeoCombiTrans("rotwheelcomb8", -modwheelPos_x,modwheelPos_y,modwheelPos_z,rotwheel8);
      Cave->AddNode(modwheel8_box ,  1496+ii,rotwheelcomb8);
     }
    }
    if(useSi)
    {
     TGeoVolume *siwheel8_box = gGeoManager->MakeTrap("SiWheel8",pMed22, 
                          siwheel_dz,siwheel_theta,siwheel_phi,
                          wheel_dy1 ,siwheel_dx1  ,siwheel_dx2,siwheel_alp1,
   			  wheel_dy2 ,siwheel_dx3  ,siwheel_dx4,siwheel_alp2); //1 m3    
     siwheel8_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(siwheel8_box);
     for(ii=0;ii<nmod[i];ii++)
     {
      TGeoRotation* sirotwheel8 = new TGeoRotation("sirotwheel8",0.,0.,0.);
      sirotwheel8->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      siwheelPos_x=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*sin(ii*dephi);
      siwheelPos_y=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*cos(ii*dephi);
//      cout<<"gino Si  8 "<<siwheelPos_x<<"   "<<siwheelPos_y<<"   "<<siwheelPos_z<<endl;
      TGeoCombiTrans* sirotwheelcomb8 = new TGeoCombiTrans("sirotwheelcomb8", -siwheelPos_x,siwheelPos_y,siwheelPos_z,sirotwheel8);
      Cave->AddNode(siwheel8_box ,  496+ii,sirotwheelcomb8);
     }
    }
   }
// END RING 8------------------------------------------------------------------
// BEGIN RING 9------------------------------------------------------------------
   if(i==80)
   {
    if(useCsI)
    {
     TGeoVolume *modwheel9_box = gGeoManager->MakeTrap("ModWheel9",pMed9, 
                          wheel_dz ,wheel_theta,wheel_phi,
                          wheel_dy1,wheel_dx1  ,wheel_dx2,wheel_alp1,
   			  wheel_dy2,wheel_dx3  ,wheel_dx4,wheel_alp2); //1 m3    
     modwheel9_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(modwheel9_box);
     TString s;
     std:: stringstream out;

     for(ii=0;ii<nmod[i];ii++)
     {
      out << i;
      s=out.str();
      TGeoRotation* rotwheel9 = new TGeoRotation("rotwheel9",0.,0.,0.);
      rotwheel9->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      modwheelPos_x=(dist[i]+halfthick)*sin(thetamean)*sin(ii*dephi);
      modwheelPos_y=(dist[i]+halfthick)*sin(thetamean)*cos(ii*dephi);
 //     cout<<"gino CsI 9  "<<ii<<"  "<<modwheelPos_x<<"   "<<modwheelPos_y<<"   "<<modwheelPos_z<<endl;
 //     cout<<"tano1"<<endl;
      TGeoCombiTrans* rotwheelcomb9 = new TGeoCombiTrans("rotwheelcomb9", -modwheelPos_x,modwheelPos_y,modwheelPos_z,rotwheel9);
      Cave->AddNode(modwheel9_box ,  1592+ii,rotwheelcomb9);
     }
    }
    if(useSi)
    {
     TGeoVolume *siwheel9_box = gGeoManager->MakeTrap("SiWheel9",pMed22, 
                          siwheel_dz,siwheel_theta,siwheel_phi,
                          wheel_dy1 ,siwheel_dx1  ,siwheel_dx2,siwheel_alp1,
   			  wheel_dy2 ,siwheel_dx3  ,siwheel_dx4,siwheel_alp2); //1 m3    
     siwheel9_box->SetVisLeaves(kTRUE);
     AddSensitiveVolume(siwheel9_box);
     for(ii=0;ii<nmod[i];ii++)
     {
      TGeoRotation* sirotwheel9 = new TGeoRotation("sirotwheel9",0.,0.,0.);
      sirotwheel9->SetAngles(ii*dephi/degtorad, -thetamean_deg, 0.);
      siwheelPos_x=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*sin(ii*dephi);
      siwheelPos_y=((dist[i]-0.5)+sihalfthick)*sin(thetamean)*cos(ii*dephi);
  //    cout<<"gino Si  9 "<<siwheelPos_x<<"   "<<siwheelPos_y<<"   "<<siwheelPos_z<<endl;
      TGeoCombiTrans* sirotwheelcomb9 = new TGeoCombiTrans("sirotwheelcomb9", -siwheelPos_x,siwheelPos_y,siwheelPos_z,sirotwheel9);
      Cave->AddNode(siwheel9_box ,  592+ii,sirotwheelcomb9);
     }
    }
   }
// END RING 9------------------------------------------------------------------

  }
  cout<<"CHIIIIIIIIIIIIIIIIIIIIIIIIIIIMMMMMMMMMMMMMMEEEEEEEEEEEEEEEERRRRRRRRRRRRRRRRRAAAAAAAAAA"<<endl;

//29/06/2010 aggiungo chimera piece by piece
}

ClassImp(R3BChimera)
