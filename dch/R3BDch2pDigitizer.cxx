#include "R3BDch2pDigitizer.h"
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


#include "R3BDchPoint.h"
#include "R3BMCTrack.h"




		
using std::cout;
using std::endl;
		

R3BDch2pDigitizer::R3BDch2pDigitizer() :
  FairTask("R3B Dch2p Digitization scheme ") { 
}


R3BDch2pDigitizer::~R3BDch2pDigitizer() {
}


void R3BDch2pDigitizer::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if ( ! rtdb ) Fatal("SetParContainers", "No runtime database");

  fDch2pDigiPar = (R3BDch2pDigiPar*)(rtdb->getContainer("R3BDch2pDigiPar"));

  if ( fDch2pDigiPar ) {
      cout << "-I- R3BDch2pDigitizer::SetParContainers() "<< endl;
      cout << "-I- Container R3BDch2pDigiPar  loaded " << endl;
  }

}





InitStatus R3BDch2pDigitizer::Init() {

//  cout<<"Init "<<endl;
  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fDch2pPoints = (TClonesArray*) ioman->GetObject("DCHPoint");
  fDch2pMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
  
   
  // Register output array DchDigi
  fDch2pDigi = new TClonesArray("R3BDch2pDigi",1000);
  ioman->Register("Dch2pDigi", "Digital response in Dch2p", fDch2pDigi, kTRUE);
  
  eventNoDch=0;
  
  // Initialise control histograms

  return kSUCCESS;

}


void R3BDch2pDigitizer::Exec(Option_t* opt) {

   Reset();
   eventNoDch+=1;
//     if(eventNoDch/1000. == (int)eventNoDch/1000.) cout<<" "<<"Event #: "<<eventNoDch-1<<endl;
     
     Int_t nentriesDch = fDch2pPoints->GetEntries();     
     Int_t TrackId=0;

     Double_t total_energy_dch1=0.;
     Double_t total_energy_dch2=0.;
  
     Double_t DCH1eloss;
     Double_t DCH2eloss;

     Int_t pd1mul;
     Int_t pd2mul;
     Double_t Pdx1_p1;
     Double_t Pdx2_p1;
     Double_t Pdy1_p1;
     Double_t Pdy2_p1;
     Double_t Pdx1_p2;
     Double_t Pdx2_p2;
     Double_t Pdy1_p2;
     Double_t Pdy2_p2;



//********************** Energy loss ***************************//   
   
   for (Int_t l=0;l<nentriesDch;l++){
     
     R3BDchPoint *dch2p_obj = (R3BDchPoint*) fDch2pPoints->At(l);
    

     Int_t DetID = dch2p_obj->GetDetectorID();     
    
     if (DetID==0){
       DCH1eloss = dch2p_obj->GetEnergyLoss()*1000.;
       total_energy_dch1+=DCH1eloss;
     }
     
     if (DetID==1){
       DCH2eloss = dch2p_obj->GetEnergyLoss()*1000.;
       total_energy_dch2+=DCH2eloss;
     }

   }
   
//******************** DCH **************************//   
  pd1mul=0;
  pd2mul=0;
  
  
   for (Int_t l=0;l<nentriesDch;l++){
//   cout<<"entries "<<l<<endl;
     
     R3BDchPoint *dch2p_obj = (R3BDchPoint*) fDch2pPoints->At(l);

     Int_t DetID = dch2p_obj->GetDetectorID();
     Double_t fPx = dch2p_obj->GetPx();
     Double_t fPy = dch2p_obj->GetPy();
     Double_t fPz = dch2p_obj->GetPz();
     Double_t fX_Local_In = dch2p_obj->GetXLocalIn();
     Double_t fY_Local_In = dch2p_obj->GetYLocalIn();
     Double_t fX_Local_Out = dch2p_obj->GetXLocalOut();
     Double_t fY_Local_Out = dch2p_obj->GetYLocalOut();
     TrackId = dch2p_obj->GetTrackID();
     R3BMCTrack *aTrack = (R3BMCTrack*) fDch2pMCTrack->At(TrackId);   
     Int_t PID = aTrack->GetPdgCode();
     Int_t mother = aTrack->GetMotherId();
     Double_t fPx_track = aTrack->GetPx();
     Double_t fPy_track = aTrack->GetPy();
     Double_t fPz_track = aTrack->GetPz();
     
     Double_t fX_Local = ((fX_Local_In + fX_Local_Out)/2);
     Double_t fY_Local = ((fY_Local_In + fY_Local_Out)/2);




    if(PID==2212 && mother<0){
    
    if (DetID==0 && l==0)
    {
     Pdx1_p1 = fX_Local;
     Pdy1_p1 = fY_Local;
     
//     cout<<"DCH1 - first proton"<<endl;
     pd1mul++;
    }
    if (DetID==0 && l==2)
    {
     Pdx1_p2 = fX_Local;
     Pdy1_p2 = fY_Local;
     
//     cout<<"DCH1 - second proton"<<endl;
     pd1mul++;
    }
         
    if (DetID==1 && l==1)
    {
     Pdx2_p1 = fX_Local;
     Pdy2_p1 = fY_Local;
     
//     cout<<"DCH2 - first proton"<<endl;
     pd2mul++;
    }
    if (DetID==1 && l==3)
    {
     Pdx2_p2 = fX_Local;
     Pdy2_p2 = fY_Local;
     
//     cout<<"DCH2 - second proton"<<endl;
     pd2mul++;
    }
    
    }
  
      

   }
  


AddHit(pd1mul,Pdx1_p1,Pdy1_p1,Pdx1_p2,Pdy1_p2,pd2mul,Pdx2_p1,Pdy2_p1,Pdx2_p2,Pdy2_p2);


}
// -------------------------------------------------------------------------

void R3BDch2pDigitizer::Reset(){
// Clear the structure
//   cout << " -I- Digit Reset() called " << endl;

   
 if (fDch2pDigi ) fDch2pDigi->Clear();

}   

void R3BDch2pDigitizer::Finish()
{
// Write control histograms

   
}

R3BDch2pDigi* R3BDch2pDigitizer::AddHit(Int_t pd1mul,Double_t Pdx1_p1,Double_t Pdy1_p1,Double_t Pdx1_p2,Double_t Pdy1_p2,
Int_t pd2mul,Double_t Pdx2_p1,Double_t Pdy2_p1,Double_t Pdx2_p2,Double_t Pdy2_p2){   
  TClonesArray& clref = *fDch2pDigi;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BDch2pDigi(pd1mul,Pdx1_p1,Pdy1_p1,Pdx1_p2,Pdy1_p2,pd2mul,Pdx2_p1,Pdy2_p1,Pdx2_p2,Pdy2_p2);
 
}


//R3BDch2pDigi* R3BDch2pDigitizer::AddHit(
 // return new(clref[size]) R3BDch2pDigi();
//}

ClassImp(R3BDch2pDigitizer)
