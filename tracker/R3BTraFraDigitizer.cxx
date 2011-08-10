#include "R3BTraFraDigitizer.h"
#include "TClonesArray.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"


// includes for modeling
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoMatrix.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoBBox.h"
#include "TGeoCompositeShape.h"
#include "TGeoShapeAssembly.h"


#include "TVector3.h"
#include "TMath.h"
#include "TRandom.h"
#include "TH1F.h"
#include "TH2F.h"
#include <string>
#include <iostream>


#include "R3BTraPoint.h"
#include "R3BMCTrack.h"
		
using std::cout;
using std::endl;
		

R3BTraFraDigitizer::R3BTraFraDigitizer() :
  FairTask("R3B TraFra Digitization scheme ") { 
}


R3BTraFraDigitizer::~R3BTraFraDigitizer() {
}


void R3BTraFraDigitizer::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if ( ! rtdb ) Fatal("SetParContainers", "No runtime database");

  fTraFraDigiPar = (R3BTraFraDigiPar*)(rtdb->getContainer("R3BTraFraDigiPar"));

  if ( fTraFraDigiPar ) {
      cout << "-I- R3BTraFraDigitizer::SetParContainers() "<< endl;
      cout << "-I- Container R3BTraFraDigiPar  loaded " << endl;
  }

}




InitStatus R3BTraFraDigitizer::Init() {

//  cout<<"Init "<<endl;
  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fTraFraPoints = (TClonesArray*) ioman->GetObject("TraPoint");
  fTraFraMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
  fMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
  
   
  // Register output array TraFraDigi
  fTraFraDigi = new TClonesArray("R3BTraFraDigi",1000);
  ioman->Register("TraFraDigi", "Digital response in TraFra", fTraFraDigi, kTRUE);
  
  eventNoTra=0;
  
  // Initialise control histograms


  return kSUCCESS;

}


void R3BTraFraDigitizer::Exec(Option_t* opt) {

   Reset();
   eventNoTra+=1;
//     if(eventNoTra/1000. == (int)eventNoTra/1000.) cout<<"Event #: "<<eventNoTra-1<<endl;
     
     Int_t nentriesTra = fTraFraPoints->GetEntries();
     Int_t nentries = fMCTrack->GetEntries();
     
     Int_t TrackIdTra=0;
     Int_t TrackId=0;
     

     Double_t ss03_se_f;
     Double_t ss03_spos_f;
     Double_t ss03_sbw_f;
     Double_t ss03_sarea_f;
     Double_t ss03_seta_f;

     Double_t ss03_ke_f;
     Double_t ss03_kpos_f;
     Double_t ss03_kbw_f;
     Double_t ss03_karea_f;
     Double_t ss03_keta_f;

     Double_t ss06_se_f;
     Double_t ss06_spos_f;
     Double_t ss06_sbw_f;
     Double_t ss06_sarea_f;
     Double_t ss06_seta_f;

     Double_t ss06_ke_f;
     Double_t ss06_kpos_f;
     Double_t ss06_kbw_f;
     Double_t ss06_karea_f;
     Double_t ss06_keta_f;
     



//******************** SSTs **************************//   


   for (Int_t l=0;l<nentriesTra;l++){
//   cout<<"entries "<<l<<endl;
     
     R3BTraPoint *TraFra_obj = (R3BTraPoint*) fTraFraPoints->At(l);

     Int_t DetID = TraFra_obj->GetDetectorID();
     Double_t fX_In = TraFra_obj->GetXIn();
     Double_t fY_In = TraFra_obj->GetYIn();
     Double_t fX_Out = TraFra_obj->GetXOut();
     Double_t fY_Out = TraFra_obj->GetYOut();
     TrackIdTra = TraFra_obj->GetTrackID();
     R3BMCTrack *aTrack = (R3BMCTrack*) fTraFraMCTrack->At(TrackIdTra);   
     Int_t PID = aTrack->GetPdgCode();
     Int_t mother = aTrack->GetMotherId();

     Double_t fX = ((fX_In + fX_Out)/2);
     Double_t fY = ((fY_In + fY_Out)/2);     


    if(PID==1000080150 && mother<0){
//    if(PID==1000160300 && mother<0){   //Christoph 2p
    
      if (DetID==15)
      {
        ss03_se_f = 2000;
//	ss03_spos_f = (-0.038230 + (3.5 + fX)); //detectors couldn't be shifted in simulation, they are shifted here 0.038230
	ss03_spos_f = (3.5 + fX);
//	ss03_spos_f = (-0.039 + (3.5 + fX));  //Christoph
	ss03_sbw_f = 2;
	ss03_sarea_f = 0;
	ss03_seta_f = 0;
	  
	ss03_ke_f = 2000;
//	ss03_kpos_f = (3.9936-((2 + fY)-0.006402 )); //detectors couldn't be shifted in simulation, they are shifted here -0.006402
//	ss03_kpos_f = (-0.006402 + (2 + fY)); 
	ss03_kpos_f = (2 + fY);
//	ss03_kpos_f = ((-0.01035 + (2 + fY)));  //Christoph (no swop)!!!
	ss03_kbw_f = 2;   //kpos for 1 SST swop (with 3.9936-) - requirement for tracker
	ss03_karea_f = 0;
	ss03_keta_f = 0;
//	cout<<"ss03_se_f "<<ss03_se_f<<endl;   
      }     
      if (DetID==16)
      {
        ss06_se_f = 2000;
//	ss06_spos_f = (0.038495 + (3.5 + fX));//detectors couldn't be shifted in simulation, they are shifted here -0.038495
	ss06_spos_f = (3.5 + fX);
//	ss06_spos_f = (0.039 + (3.5 + fX));  //Christoph
	ss06_sbw_f = 2;
	ss06_sarea_f = 0;
	ss06_seta_f = 0;
	  
	ss06_ke_f = 2000;
//	ss06_kpos_f = (0.00798 + (2 + fY));//detectors couldn't be shifted in simulation, they are shifted here 0.00798
	ss06_kpos_f = (2 + fY);
//	ss06_kpos_f = (0.01035 + (2 + fY));  //Christoph
	ss06_kbw_f = 2;
	ss06_karea_f = 0;
	ss06_keta_f = 0;
	
      }
            
   }
          

 }
   
  


AddHit(ss03_se_f,ss03_spos_f,ss03_sbw_f,ss03_sarea_f,ss03_seta_f,ss03_ke_f,
ss03_kpos_f,ss03_kbw_f,ss03_karea_f,ss03_keta_f,ss06_se_f,ss06_spos_f,ss06_sbw_f,ss06_sarea_f,ss06_seta_f,
ss06_ke_f,ss06_kpos_f,ss06_kbw_f,ss06_karea_f,ss06_keta_f);


}
// -------------------------------------------------------------------------

void R3BTraFraDigitizer::Reset(){
// Clear the structure
//   cout << " -I- Digit Reset() called " << endl;

   
 if (fTraFraDigi ) fTraFraDigi->Clear();

}   

void R3BTraFraDigitizer::Finish()
{
// Write control histograms

   
}

R3BTraFraDigi* R3BTraFraDigitizer::AddHit(Double_t ss03_se_f,
Double_t ss03_spos_f,Double_t ss03_sbw_f,Double_t ss03_sarea_f,Double_t ss03_seta_f,Double_t ss03_ke_f,
Double_t ss03_kpos_f,Double_t ss03_kbw_f,Double_t ss03_karea_f,Double_t ss03_keta_f,Double_t ss06_se_f,
Double_t ss06_spos_f,Double_t ss06_sbw_f,Double_t ss06_sarea_f,Double_t ss06_seta_f,Double_t ss06_ke_f,
Double_t ss06_kpos_f,Double_t ss06_kbw_f,Double_t ss06_karea_f,Double_t ss06_keta_f){   
  TClonesArray& clref = *fTraFraDigi;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BTraFraDigi(ss03_se_f,ss03_spos_f,ss03_sbw_f,ss03_sarea_f,ss03_seta_f,ss03_ke_f,
ss03_kpos_f,ss03_kbw_f,ss03_karea_f,ss03_keta_f,ss06_se_f,ss06_spos_f,ss06_sbw_f,ss06_sarea_f,ss06_seta_f,
ss06_ke_f,ss06_kpos_f,ss06_kbw_f,ss06_karea_f,ss06_keta_f);
 
}



//R3BTraFraDigi* R3BTraFraDigitizer::AddHit(
 // return new(clref[size]) R3BTraFraDigi();
//}

ClassImp(R3BTraFraDigitizer)
