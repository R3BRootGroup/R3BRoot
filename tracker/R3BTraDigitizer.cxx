#include "R3BTraDigitizer.h"
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
		

R3BTraDigitizer::R3BTraDigitizer() :
  FairTask("R3B Tra Digitization scheme ") { 
}


R3BTraDigitizer::~R3BTraDigitizer() {
}


void R3BTraDigitizer::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if ( ! rtdb ) Fatal("SetParContainers", "No runtime database");

  fTraDigiPar = (R3BTraDigiPar*)(rtdb->getContainer("R3BTraDigiPar"));

  if ( fTraDigiPar ) {
      cout << "-I- R3BTraDigitizer::SetParContainers() "<< endl;
      cout << "-I- Container R3BTraDigiPar  loaded " << endl;
  }
}




InitStatus R3BTraDigitizer::Init() {
//  cout<<"Init "<<endl;
  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fTraPoints = (TClonesArray*) ioman->GetObject("TraPoint");
  fTraMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
  fMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
  
   
  // Register output array TraDigi
  fTraDigi = new TClonesArray("R3BTraDigi",1000);
  ioman->Register("TraDigi", "Digital response in Tra", fTraDigi, kTRUE);
  
  eventNoTra=0;
  
  // Initialise control histograms


  return kSUCCESS;
}


void R3BTraDigitizer::Exec(Option_t* opt) {

   Reset();
   eventNoTra+=1;
//     if(eventNoTra/1000. == (int)eventNoTra/1000.) cout<<"Event #: "<<eventNoTra-1<<endl;
     
     Int_t nentriesTra = fTraPoints->GetEntries();
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
     
     Double_t ss03_se_p1;
     Double_t ss03_spos_p1;
     Double_t ss03_sbw_p1;
     Double_t ss03_sarea_p1;
     Double_t ss03_seta_p1;

     Double_t ss03_ke_p1;
     Double_t ss03_kpos_p1;
     Double_t ss03_kbw_p1;
     Double_t ss03_karea_p1;
     Double_t ss03_keta_p1;

     Double_t ss06_se_p1;
     Double_t ss06_spos_p1;
     Double_t ss06_sbw_p1;
     Double_t ss06_sarea_p1;
     Double_t ss06_seta_p1;

     Double_t ss06_ke_p1;
     Double_t ss06_kpos_p1;
     Double_t ss06_kbw_p1;
     Double_t ss06_karea_p1;
     Double_t ss06_keta_p1;
     


//******************** SSTs **************************//   


   for (Int_t l=0;l<nentriesTra;l++){
//   cout<<"entries "<<l<<endl;
    
     R3BTraPoint *Tra_obj = (R3BTraPoint*) fTraPoints->At(l);

     Int_t DetID = Tra_obj->GetDetectorID();
     Double_t fX_In = Tra_obj->GetXIn();
     Double_t fY_In = Tra_obj->GetYIn();
     Double_t fX_Out = Tra_obj->GetXOut();
     Double_t fY_Out = Tra_obj->GetYOut();
     TrackIdTra = Tra_obj->GetTrackID();
     R3BMCTrack *aTrack = (R3BMCTrack*) fTraMCTrack->At(TrackIdTra);   
     Int_t PID = aTrack->GetPdgCode();
     Int_t mother = aTrack->GetMotherId();

     Double_t fX = ((fX_In + fX_Out)/2);
     Double_t fY = ((fY_In + fY_Out)/2);     


//    if(PID==1000080150 && mother<0){
    if(PID==1000170310 && mother<0){  //Christoph 1p
    
      if (DetID==15)
      {
        ss03_se_f = 2000;
//	ss03_spos_f = (-0.038230 + (3.5 + fX)); //detectors couldn't be shifted in simulation, they are shifted here 0.038230 
//	ss03_spos_f = (3.5 + fX);
        ss03_spos_f = (-0.039 + (3.5 + fX));  //Christoph
	ss03_sbw_f = 2;
	ss03_sarea_f = 0;
	ss03_seta_f = 0;
	  
	ss03_ke_f = 2000;
//	ss03_kpos_f = (3.9936-(-0.006402 + (2 + fY)));//detectors couldn't be shifted in simulation, they are shifted here -0.006402 
//        ss03_kpos_f = (2 + fY);
	ss03_kpos_f = ((-0.01035 + (2 + fY)));  //Christoph (no swop)!!!
	ss03_kbw_f = 2;  //kpos for 1 SST swop (with 3.9936-) - requirement for tracker
	ss03_karea_f = 0;
	ss03_keta_f = 0;
//	cout<<"SST1 - fragment "<<PID<<endl;   
      }     
      if (DetID==16)
      {
        ss06_se_f = 2000;
//	ss06_spos_f = (0.038495 + (3.5 + fX));//detectors couldn't be shifted in simulation, they are shifted here -0.038495 
//	ss06_spos_f = (3.5 + fX);
	ss06_spos_f = (0.039 + (3.5 + fX));  //Christoph
	ss06_sbw_f = 2;
	ss06_sarea_f = 0;
	ss06_seta_f = 0;
	  
	ss06_ke_f = 2000;
//	ss06_kpos_f = (0.00798 + (2 + fY));//detectors couldn't be shifted in simulation, they are shifted here 0.00798
//	ss06_kpos_f = (2 + fY);
	ss06_kpos_f = (0.01035 + (2 + fY));  //Christoph
	ss06_kbw_f = 2;
	ss06_karea_f = 0;
	ss06_keta_f = 0;	
//	cout<<"SST2 - fragment "<<PID<<endl;
      }
            
   }
     
     if(PID==2212 && mother<0){
     
      if (DetID==15)
      {
        ss03_se_p1 = 50;
//	ss03_spos_p1 = (-0.038230 + (3.5 + fX)); //detectors couldn't be shifted in simulation, they are shifted here 0.038230
//	ss03_spos_p1 = (3.5 + fX);
	ss03_spos_f = (-0.039 + (3.5 + fX));  //Christoph
	ss03_sbw_p1 = 2;
	ss03_sarea_p1 = 0;
	ss03_seta_p1 = 0;
	  
	ss03_ke_p1 = 50;
//	ss03_kpos_p1 = (3.9936-(-0.006402 + (2 + fY))); //detectors couldn't be shifted in simulation, they are shifted here -0.006402
//	ss03_kpos_p1 = (2 + fY);
	ss03_kpos_f = ((-0.01035 + (2 + fY)));  //Christoph (no swop)!!!
	ss03_kbw_p1 = 2;   //kpos for 1 SST swop (with 3.9936-) - requirement for tracker 
	ss03_karea_p1 = 0;
	ss03_keta_p1 = 0;
//	cout<<"SST1 - proton "<<PID<<endl;
//	cout<<"l "<<l<<" DetID "<<DetID<<" PID "<<PID<<endl;
      }     
      if (DetID==16)
      {
        ss06_se_p1 = 50;
//	ss06_spos_p1 = (0.038495 + (3.5 + fX));//detectors couldn't be shifted in simulation, they are shifted here -0.038495
//	ss06_spos_p1 = (3.5 + fX);
	ss06_spos_f = (0.039 + (3.5 + fX));  //Christoph
	ss06_sbw_p1 = 2;
	ss06_sarea_p1 = 0;
	ss06_seta_p1 = 0;
	  
	ss06_ke_p1 = 50;
//	ss06_kpos_p1 = (0.00798 + (2 + fY));//detectors couldn't be shifted in simulation, they are shifted here 0.00798
//	ss06_kpos_p1 = (2 + fY);
	ss06_kpos_f = (0.01035 + (2 + fY));  //Christoph
	ss06_kbw_p1 = 2;
	ss06_karea_p1 = 0;
	ss06_keta_p1 = 0;
//	cout<<"SST2 - proton "<<PID<<endl;
//	cout<<"l "<<l<<" DetID "<<DetID<<" PID "<<PID<<endl;   
      }
     
    }     

 } 
  


AddHit(ss03_se_f,ss03_spos_f,ss03_sbw_f,ss03_sarea_f,ss03_seta_f,ss03_ke_f,
ss03_kpos_f,ss03_kbw_f,ss03_karea_f,ss03_keta_f,ss06_se_f,ss06_spos_f,ss06_sbw_f,ss06_sarea_f,ss06_seta_f,
ss06_ke_f,ss06_kpos_f,ss06_kbw_f,ss06_karea_f,ss06_keta_f,ss03_se_p1,ss03_spos_p1,ss03_sbw_p1,ss03_sarea_p1,
ss03_seta_p1,ss03_ke_p1,ss03_kpos_p1,ss03_kbw_p1,ss03_karea_p1,ss03_keta_p1,ss06_se_p1,ss06_spos_p1,ss06_sbw_p1,
ss06_sarea_p1,ss06_seta_p1,ss06_ke_p1,ss06_kpos_p1,ss06_kbw_p1,ss06_karea_p1,ss06_keta_p1);


}
// -------------------------------------------------------------------------

void R3BTraDigitizer::Reset(){
// Clear the structure
//   cout << " -I- Digit Reset() called " << endl;

   
 if (fTraDigi ) fTraDigi->Clear();

}   

void R3BTraDigitizer::Finish()
{
// Write control histograms

   
}

R3BTraDigi* R3BTraDigitizer::AddHit(Double_t ss03_se_f,
Double_t ss03_spos_f,Double_t ss03_sbw_f,Double_t ss03_sarea_f,Double_t ss03_seta_f,Double_t ss03_ke_f,
Double_t ss03_kpos_f,Double_t ss03_kbw_f,Double_t ss03_karea_f,Double_t ss03_keta_f,Double_t ss06_se_f,
Double_t ss06_spos_f,Double_t ss06_sbw_f,Double_t ss06_sarea_f,Double_t ss06_seta_f,Double_t ss06_ke_f,
Double_t ss06_kpos_f,Double_t ss06_kbw_f,Double_t ss06_karea_f,Double_t ss06_keta_f,Double_t ss03_se_p1,
Double_t ss03_spos_p1,Double_t ss03_sbw_p1,Double_t ss03_sarea_p1,Double_t ss03_seta_p1,Double_t ss03_ke_p1,
Double_t ss03_kpos_p1,Double_t ss03_kbw_p1,Double_t ss03_karea_p1,Double_t ss03_keta_p1,Double_t ss06_se_p1,
Double_t ss06_spos_p1,Double_t ss06_sbw_p1,Double_t ss06_sarea_p1,Double_t ss06_seta_p1,Double_t ss06_ke_p1,
Double_t ss06_kpos_p1,Double_t ss06_kbw_p1,Double_t ss06_karea_p1,Double_t ss06_keta_p1){   
  TClonesArray& clref = *fTraDigi;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BTraDigi(ss03_se_f,ss03_spos_f,ss03_sbw_f,ss03_sarea_f,ss03_seta_f,ss03_ke_f,
ss03_kpos_f,ss03_kbw_f,ss03_karea_f,ss03_keta_f,ss06_se_f,ss06_spos_f,ss06_sbw_f,ss06_sarea_f,ss06_seta_f,
ss06_ke_f,ss06_kpos_f,ss06_kbw_f,ss06_karea_f,ss06_keta_f,ss03_se_p1,ss03_spos_p1,ss03_sbw_p1,ss03_sarea_p1,
ss03_seta_p1,ss03_ke_p1,ss03_kpos_p1,ss03_kbw_p1,ss03_karea_p1,ss03_keta_p1,ss06_se_p1,ss06_spos_p1,ss06_sbw_p1,
ss06_sarea_p1,ss06_seta_p1,ss06_ke_p1,ss06_kpos_p1,ss06_kbw_p1,ss06_karea_p1,ss06_keta_p1);
cout<<"Point 19"<<endl; 
}



//R3BTraDigi* R3BTraDigitizer::AddHit(
 // return new(clref[size]) R3BTraDigi();
//}

ClassImp(R3BTraDigitizer)
