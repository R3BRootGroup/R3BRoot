#include "R3BTargetDigitizer.h"
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

#define SQR(x) ((x)*(x))
#define U_MEV_C2 931.494
#define MASS_PROTON_MEV_C2 938.272046 //938.279
//#define MASS_17NE_MEV_C2   29796.65565 //Ar
//#define MASS_15O_MEV_C2    28860.78 //Cl
#define MASS_17NE_MEV_C2	15846.9
#define MASS_16F_MEV_C2		14910.1
#define MASS_15O_MEV_C2		13971.3
		
using std::cout;
using std::endl;
		

R3BTargetDigitizer::R3BTargetDigitizer() :
  FairTask("R3B Target Digitization scheme ") { 
}


R3BTargetDigitizer::~R3BTargetDigitizer() {
}


void R3BTargetDigitizer::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if ( ! rtdb ) Fatal("SetParContainers", "No runtime database");

  fTargetDigiPar = (R3BTargetDigiPar*)(rtdb->getContainer("R3BTargetDigiPar"));

  if ( fTargetDigiPar ) {
      cout << "-I- R3BTargetDigitizer::SetParContainers() "<< endl;
      cout << "-I- Container R3BTargetDigiPar  loaded " << endl;
  }

}




InitStatus R3BTargetDigitizer::Init() {

//  cout<<"Init "<<endl;
  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fTargetPoints = (TClonesArray*) ioman->GetObject("TraPoint");
  fTargetMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
  fMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
  
   
  // Register output array TraDigi
  fTargetDigi = new TClonesArray("R3BTargetDigi",1000);
  ioman->Register("TargetDigi", "Digital response in Target", fTargetDigi, kTRUE);
  
  eventNoTra=0;
  
  // Initialise control histograms
  ExEnIn_his = new TH1F("ExEnIn_his","ExEnIn_his",110,0.,22.);

  return kSUCCESS;

}


void R3BTargetDigitizer::Exec(Option_t* opt) {

   Reset();
   eventNoTra+=1;
     if(eventNoTra/1000. == (int)eventNoTra/1000.) cout<<"Event #: "<<eventNoTra-1<<endl;
     
     Int_t nentriesTra = fTargetPoints->GetEntries();
     Int_t nentries = fMCTrack->GetEntries();
     
     Int_t TrackIdTra=0;
     Int_t TrackId=0;


     
     Double_t x0;
     Double_t y0;
     Double_t t0;
     

     Int_t ss03_smul;
     Int_t ss03_kmul;
     Int_t ss06_smul;
     Int_t ss06_kmul;
     
     Double_t Pxf=0;
     Double_t Pyf=0;
     Double_t Pzf=0;
     
     Double_t Pxp1=0;
     Double_t Pyp1=0;
     Double_t Pzp1=0;
     
     Double_t Pf_tot;
     Double_t Pp1_tot;
     
     Double_t f_beta;
     Double_t p1_beta;
     
     Double_t p2;
     Double_t E2;
     Double_t estar;
     

//******************** Target **************************//

   for (Int_t l=0;l<nentries;l++){
//   cout<<"entries "<<l<<endl;
     
     R3BMCTrack *aTrack = (R3BMCTrack*) fMCTrack->At(l);
     
     Int_t PID = aTrack->GetPdgCode();
     Int_t mother = aTrack->GetMotherId();
     
     Double_t fX = aTrack->GetStartX();
     Double_t fY = aTrack->GetStartY();
     Double_t fT = aTrack->GetStartT();
     
     Double_t Px = aTrack->GetPx();
     Double_t Py = aTrack->GetPy();
     Double_t Pz = aTrack->GetPz();
     
     
   if (mother<0 && PID==1000080150){
   //if (mother<0 && PID==1000170310){  //Christoph
   Pxf=(Px*1000);
   Pyf=(Py*1000);
   Pzf=(Pz*1000);
//   cout<<"In "<<"Pxf "<<Pxf<<" Pyf "<<Pyf<<" Pzf "<<Pzf<<endl;
   Pf_tot=sqrt(SQR(Pxf)+SQR(Pyf)+SQR(Pzf));
//   f_beta=0.7579865;
   f_beta=sqrt((SQR(Pf_tot))/((SQR(MASS_15O_MEV_C2))+(SQR(Pf_tot))));
//   cout<<"In "<<"Pf_tot "<<Pf_tot<<" f_beta "<<f_beta<<endl;
   }
   
   
   if (mother<0 && PID==2212 /*&& l==1*/){
   Pxp1=(Px*1000);
   Pyp1=(Py*1000);
   Pzp1=(Pz*1000);
//   cout<<"In "<<"Pxp1 "<<Pxp1<<" Pyp1 "<<Pyp1<<" Pzp1 "<<Pzp1<<endl;
   Pp1_tot=sqrt(SQR(Pxp1)+SQR(Pyp1)+SQR(Pzp1));
//   p1_beta=0.7579865;
   p1_beta=sqrt((SQR(Pp1_tot))/((SQR(MASS_PROTON_MEV_C2))+(SQR(Pp1_tot))));
//   cout<<"In "<<"Pp1_tot "<<Pp1_tot<<" p1_beta "<<p1_beta<<endl;
   }    
   
     
   if(mother<0){  
      x0=fX;
      y0=fY;
      t0=fT;
   }            

 }
 
 
 
   Double_t px=SQR(Pxp1+Pxf);
   Double_t py=SQR(Pyp1+Pyf);
   Double_t pz=SQR(Pzp1+Pzf);     
   p2=px+py+pz; // MeV^2/c^2
//   cout<<"In "<<"px "<<px<<" py "<<py<<" pz "<<pz<<" sqrt_p2 "<<sqrt(p2)<<endl;

   Double_t E_f =1.0/sqrt(1-f_beta*f_beta)   * MASS_15O_MEV_C2; // *c   
   Double_t E_p1=1.0/sqrt(1-p1_beta*p1_beta) * MASS_PROTON_MEV_C2; // *c
   E2=(E_f+E_p1)*(E_f+E_p1); // MeV^2/c^2
//   cout<<"In "<<"E_f "<<E_f<<" E_p1 "<<E_p1<<" sqrt_E2 "<<sqrt(E2)<<endl;
  

   //  sqrt(MeV^2/c^2 - MeV^2/c^2)=MeV/c
   //estar=sqrt(E2-p2)-MASS_17NE_MEV_C2; // *c2
   //estar=sqrt(E2-p2)-MASS_16F_MEV_C2; // *c2
   //estar=sqrt(E2-p2)-(MASS_15O_MEV_C2+MASS_PROTON_MEV_C2)+1.0; // *c2 // shift by 1 extra MeV, not to start at 0.
   estar=sqrt(E2-p2)-(MASS_15O_MEV_C2+MASS_PROTON_MEV_C2); // *c2
  
   
//   cout<<"Estar In "<<Estar<<endl;
   
   ExEnIn_his->Fill(estar); 
  
//******************** SSTs **************************//   

  ss03_smul=0;
  ss03_kmul=0;
  ss06_smul=0;
  ss06_kmul=0;
  
  
   for (Int_t l=0;l<nentriesTra;l++){
//   cout<<"entries "<<l<<endl;
     
     R3BTraPoint *Tra_obj = (R3BTraPoint*) fTargetPoints->At(l);

     Int_t DetID = Tra_obj->GetDetectorID();
     
     TrackIdTra = Tra_obj->GetTrackID();
     R3BMCTrack *aTrack = (R3BMCTrack*) fTargetMCTrack->At(TrackIdTra);   
     Int_t PID = aTrack->GetPdgCode();
     Int_t mother = aTrack->GetMotherId();     

    if(PID==1000080150 && mother<0){
    //if(PID==1000170310 && mother<0){ //Christoph
    
      if (DetID==15)
      {
        ss03_smul++;
	ss03_kmul++;   
      }     
      if (DetID==16)
      {  
        ss06_smul++;
	ss06_kmul++;   
      }
            
   }
     
     if(PID==2212 && mother<0){
     
      if (DetID==15)
      { 
        ss03_smul++;
	ss03_kmul++;   
      }     
      if (DetID==16)
      { 
        ss06_smul++;
	ss06_kmul++;   
      }
      
    }     

 }




AddHit(ss03_smul,ss03_kmul,ss06_smul,ss06_kmul,x0,y0,t0,estar);


}
// -------------------------------------------------------------------------

void R3BTargetDigitizer::Reset(){
// Clear the structure
//   cout << " -I- Digit Reset() called " << endl;

   
 if (fTargetDigi ) fTargetDigi->Clear();

}   

void R3BTargetDigitizer::Finish()
{
// Write control histograms


    ExEnIn_his->Write();
   
}

R3BTargetDigi* R3BTargetDigitizer::AddHit(Double_t ss03_smul,Double_t ss03_kmul,Double_t ss06_smul,Double_t ss06_kmul,
Double_t x0,Double_t y0,Double_t t0,Double_t estar){   
  TClonesArray& clref = *fTargetDigi;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BTargetDigi(ss03_smul,ss03_kmul,ss06_smul,ss06_kmul,x0,y0,t0,estar);
 
}



//R3BTraDigi* R3BTargetDigitizer::AddHit(
 // return new(clref[size]) R3BTargetDigi();
//}

ClassImp(R3BTargetDigitizer)
