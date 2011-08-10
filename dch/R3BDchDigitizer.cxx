#include "R3BDchDigitizer.h"
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
		

R3BDchDigitizer::R3BDchDigitizer() :
  FairTask("R3B Dch Digitization scheme ") { 
}


R3BDchDigitizer::~R3BDchDigitizer() {
}


void R3BDchDigitizer::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if ( ! rtdb ) Fatal("SetParContainers", "No runtime database");

  fDchDigiPar = (R3BDchDigiPar*)(rtdb->getContainer("R3BDchDigiPar"));

  if ( fDchDigiPar ) {
      cout << "-I- R3BDchDigitizer::SetParContainers() "<< endl;
      cout << "-I- Container R3BDchDigiPar  loaded " << endl;
  }

}




InitStatus R3BDchDigitizer::Init() {

//  cout<<"Init "<<endl;
  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fDchPoints = (TClonesArray*) ioman->GetObject("DCHPoint");
  fDchMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
  
   
  // Register output array DchDigi
  fDchDigi = new TClonesArray("R3BDchDigi",1000);
  ioman->Register("DchDigi", "Digital response in Dch", fDchDigi, kTRUE);
  
  eventNoDch=0;
  
  // Initialise control histograms
     DCH1Px = new TH1F("DCH1Px","DCH1Px",500,-1.,0.2);
     DCH1Px->GetXaxis()->SetTitle("Momentum");
     DCH1Px->GetYaxis()->SetTitle("Counts");
     
     DCH2Px = new TH1F("DCH2Px","DCH2Px",500,-1.,0.2);
     DCH2Px->GetXaxis()->SetTitle("Momentum");
     DCH2Px->GetYaxis()->SetTitle("Counts");
     
     DCH1Py = new TH1F("DCH1Py","DCH1Py",500,-0.15,0.15);
     DCH1Py->GetXaxis()->SetTitle("Momentum");
     DCH1Py->GetYaxis()->SetTitle("Counts");
     
     DCH2Py = new TH1F("DCH2Py","DCH2Py",500,-0.15,0.15);
     DCH2Py->GetXaxis()->SetTitle("Momentum");
     DCH2Py->GetYaxis()->SetTitle("Counts");
     
     DCH1Pz = new TH1F("DCH1Pz","DCH1Pz",500,-0.2,1.2);
     DCH1Pz->GetXaxis()->SetTitle("Momentum");
     DCH1Pz->GetYaxis()->SetTitle("Counts");
     
     DCH2Pz = new TH1F("DCH2Pz","DCH2Pz",500,-0.2,1.2);
     DCH2Pz->GetXaxis()->SetTitle("Momentum");
     DCH2Pz->GetYaxis()->SetTitle("Counts");
     
     
     DCH1X = new TH1F("DCH1X","DCH1X",500,-50.,50.);
     DCH1X->GetXaxis()->SetTitle("Position");
     DCH1X->GetYaxis()->SetTitle("Counts");
     
     DCH2X = new TH1F("DCH2X","DCH2X",500,-50.,50);
     DCH2X->GetXaxis()->SetTitle("Position");
     DCH2X->GetYaxis()->SetTitle("Counts");
     
     DCH1Y = new TH1F("DCH1Y","DCH1Y",500,-50.,50);
     DCH1Y->GetXaxis()->SetTitle("Position");
     DCH1Y->GetYaxis()->SetTitle("Counts");
     
     DCH2Y = new TH1F("DCH2Y","DCH2Y",500,-50.,50.);
     DCH2Y->GetXaxis()->SetTitle("Position");
     DCH2Y->GetYaxis()->SetTitle("Counts");
     

     DCH1elosshis = new TH1F("DCH1elosshis","DCH1elosshis",500,0.,0.1);
         
     DCH2elosshis = new TH1F("DCH2elosshis","DCH2elosshis",500,0.,0.1);
          

     TrackPx = new TH1F("TrackPx","TrackPx",500,-0.15,0.15);
     TrackPx->GetXaxis()->SetTitle("Momentum");
     TrackPx->GetYaxis()->SetTitle("Counts");
     
     TrackPy = new TH1F("TrackPy","TrackPy",500,-0.15,0.15);
     TrackPy->GetXaxis()->SetTitle("Momentum");
     TrackPy->GetYaxis()->SetTitle("Counts");
     
     TrackPz = new TH1F("TrackPz","TrackPz",500,0.5,1.5);
     TrackPz->GetXaxis()->SetTitle("Momentum");
     TrackPz->GetYaxis()->SetTitle("Counts");
     
     TrackPxVSDCH1Px = new TH2F("TrackPxVSDCH1Px","TrackPxVSDCH1Px",500,-0.8,-0.2,500,-0.15,0.15);
     TrackPxVSDCH1Px->GetXaxis()->SetTitle("DCH1Px");
     TrackPxVSDCH1Px->GetYaxis()->SetTitle("TrackPx");
     

  return kSUCCESS;

}


void R3BDchDigitizer::Exec(Option_t* opt) {

   Reset();
   eventNoDch+=1;
//     if(eventNoDch/1000. == (int)eventNoDch/1000.) cout<<" "<<endl<<"Event #: "<<eventNoDch-1<<endl;
     
     Int_t nentriesDch = fDchPoints->GetEntries();
     
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
     

   
   
   for (Int_t l=0;l<nentriesDch;l++){
     
     R3BDchPoint *dch_obj = (R3BDchPoint*) fDchPoints->At(l);
    

     Int_t DetID = dch_obj->GetDetectorID();     
    
     if (DetID==0){
       DCH1eloss = dch_obj->GetEnergyLoss()*1000.;
       total_energy_dch1+=DCH1eloss;
       DCH1elosshis->Fill(total_energy_dch1);
     }
     
     if (DetID==1){
       DCH2eloss = dch_obj->GetEnergyLoss()*1000.;
       total_energy_dch2+=DCH2eloss;
       DCH2elosshis->Fill(total_energy_dch2);
     }

   }
   
//******************** DCH **************************//   
  pd1mul=0;
  pd2mul=0;
  
  
   for (Int_t l=0;l<nentriesDch;l++){
//   cout<<"entries "<<l<<endl;
     
     R3BDchPoint *dch_obj = (R3BDchPoint*) fDchPoints->At(l);

     Int_t DetID = dch_obj->GetDetectorID();
     Double_t fPx = dch_obj->GetPx();
     Double_t fPy = dch_obj->GetPy();
     Double_t fPz = dch_obj->GetPz();
     Double_t fX_Local_In = dch_obj->GetXLocalIn();
     Double_t fY_Local_In = dch_obj->GetYLocalIn();
     Double_t fX_Local_Out = dch_obj->GetXLocalOut();
     Double_t fY_Local_Out = dch_obj->GetYLocalOut();
     TrackId = dch_obj->GetTrackID();
     R3BMCTrack *aTrack = (R3BMCTrack*) fDchMCTrack->At(TrackId);   
     Int_t PID = aTrack->GetPdgCode();
     Int_t mother = aTrack->GetMotherId();
     Double_t fPx_track = aTrack->GetPx();
     Double_t fPy_track = aTrack->GetPy();
     Double_t fPz_track = aTrack->GetPz();
     
     Double_t fX_Local = ((fX_Local_In + fX_Local_Out)/2);
     Double_t fY_Local = ((fY_Local_In + fY_Local_Out)/2);





    if(PID==2212 && mother<0){
    if (DetID==0)
    {
     Pdx1_p1 += fX_Local;
     Pdy1_p1 += fY_Local;
     DCH1Px->Fill(fPx);
     DCH1Py->Fill(fPy);
     DCH1Pz->Fill(fPz);
     DCH1X->Fill(fX_Local);
     DCH1Y->Fill(fY_Local);
     TrackPxVSDCH1Px->Fill(fPx,fPx_track);
//     cout<<"Dch1 - first drift chamber "<<PID<<endl;
     pd1mul++;
    }
    

     
    if (DetID==1)
    {
     Pdx2_p1 += fX_Local;
     Pdy2_p1 += fY_Local;
     DCH2Px->Fill(fPx);
     DCH2Py->Fill(fPy);
     DCH2Pz->Fill(fPz);
     DCH2X->Fill(fX_Local);
     DCH2Y->Fill(fY_Local);
//     cout<<"Dch2 - second drift chamber"<<PID<<endl;
     pd2mul++;
    }
    
    
    if (mother<0)
    {
    TrackPx->Fill(fPx_track);
    TrackPy->Fill(fPy_track);
    TrackPz->Fill(fPz_track);
    }
    
    }
  
      

   }
  


AddHit(pd1mul,Pdx1_p1,Pdy1_p1,pd2mul,Pdx2_p1,Pdy2_p1);


}
// -------------------------------------------------------------------------

void R3BDchDigitizer::Reset(){
// Clear the structure
//   cout << " -I- Digit Reset() called " << endl;

   
 if (fDchDigi ) fDchDigi->Clear();

}   

void R3BDchDigitizer::Finish()
{
// Write control histograms
   DCH1Px->Write();
   DCH2Px->Write();

   DCH1Py->Write();
   DCH2Py->Write();
   
   DCH1Pz->Write();
   DCH2Pz->Write();
   
   DCH1X->Write();
   DCH2X->Write();
   
   DCH1Y->Write();
   DCH2Y->Write();
   
   DCH1elosshis->Write();
   DCH2elosshis->Write();
   
   TrackPx->Write();
   TrackPy->Write();
   TrackPz->Write();
   
   TrackPxVSDCH1Px->Write();
   
}

R3BDchDigi* R3BDchDigitizer::AddHit(Int_t pd1mul,Double_t Pdx1_p1,Double_t Pdy1_p1,Int_t pd2mul,Double_t Pdx2_p1,
Double_t Pdy2_p1){   
  TClonesArray& clref = *fDchDigi;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BDchDigi(pd1mul,Pdx1_p1,Pdy1_p1,pd2mul,Pdx2_p1,Pdy2_p1);
 
}


//R3BDchDigi* R3BDchDigitizer::AddHit(
 // return new(clref[size]) R3BDchDigi();
//}

ClassImp(R3BDchDigitizer)
