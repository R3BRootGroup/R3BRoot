#include "R3BTofDigitizer.h"
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


#include "R3BTofPoint.h"
//#include "/u/jmargan/fairroot_160311/r3broot/r3bdata/R3BMCTrack.h"
#include "../R3BMCTrack.h"
//#include "R3BMCTrack.h"
		
using std::cout;
using std::endl;
		

R3BTofDigitizer::R3BTofDigitizer() :
  FairTask("R3B Tof Digitization scheme ") { 
}


R3BTofDigitizer::~R3BTofDigitizer() {
}


void R3BTofDigitizer::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if ( ! rtdb ) Fatal("SetParContainers", "No runtime database");

  fTofDigiPar = (R3BTofDigiPar*)(rtdb->getContainer("R3BTofDigiPar"));

  if ( fTofDigiPar ) {
      cout << "-I- R3BTofDigitizer::SetParContainers() "<< endl;
      cout << "-I- Container R3BTofDigiPar  loaded " << endl;
  }

}




InitStatus R3BTofDigitizer::Init() {

//  cout<<"Init "<<endl;
  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fTofPoints = (TClonesArray*) ioman->GetObject("TOFPoint");
  fTofMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
   
  // Register output array TofDigi
  fTofDigi = new TClonesArray("R3BTofDigi",1000);
  ioman->Register("TofDigi", "Digital response in Tof", fTofDigi, kTRUE);
  
  eventNoTof=0;
  
  // Initialise control histograms
     
     TOFelosshis = new TH1F("TOFelosshis","TOFelosshis",500,0.,80.);
     
     TfwXhis = new TH1F("TfwXhis","TfwXhis",500,-110.,110.);
     TfwXhis->GetXaxis()->SetTitle("Position");
     TfwXhis->GetYaxis()->SetTitle("Counts");
     
     TfwYhis = new TH1F("TfwYhis","TfwYhis",500,-80.,80.);
     TfwYhis->GetXaxis()->SetTitle("Position");
     TfwYhis->GetYaxis()->SetTitle("Counts");
     
     TfwThis = new TH1F("TfwThis","TfwThis",500,-10.,4000.);
     TfwThis->GetXaxis()->SetTitle("Time");
     TfwThis->GetYaxis()->SetTitle("Counts");

  return kSUCCESS;

}


void R3BTofDigitizer::Exec(Option_t* opt) {

   Reset();
   eventNoTof+=1;
//     if(eventNoTof/1000. == (int)eventNoTof/1000.) cout<<"Event #: "<<eventNoTof-1<<endl;
     
     Int_t nentriesTof = fTofPoints->GetEntries();
     Int_t TrackIdTof=0;

     Double_t total_energy_tof=0.;
     Double_t TOFeloss;
     
     Int_t tfmul;
     Double_t tfwx_p1;
     Double_t tfwy_p1;
     Double_t tfwt_p1;
   
   

   for (Int_t l=0;l<nentriesTof;l++){
    
     R3BTofPoint *tof_obj = (R3BTofPoint*) fTofPoints->At(l);     
     
     TOFeloss = tof_obj->GetEnergyLoss()*1000;
     
     TrackIdTof = tof_obj->GetTrackID();
     R3BMCTrack *aTrack = (R3BMCTrack*) fTofMCTrack->At(TrackIdTof);   
     Int_t PID = aTrack->GetPdgCode();
     
     
     if (PID==2212){
     total_energy_tof+=TOFeloss;
     TOFelosshis->Fill(total_energy_tof);
     }
     
   
   }


//******************** TFW ********************//
  tfmul=0;
  tfwx_p1=0;
  tfwy_p1=0;
  tfwt_p1=0;
  
   for (Int_t l=0;l<nentriesTof;l++){
    
     R3BTofPoint *tof_obj = (R3BTofPoint*) fTofPoints->At(l);     
          
     TrackIdTof = tof_obj->GetTrackID();
     R3BMCTrack *aTrack = (R3BMCTrack*) fTofMCTrack->At(TrackIdTof);   
     Int_t PID = aTrack->GetPdgCode();
     Int_t mother = aTrack->GetMotherId();
     
     Double_t fX = tof_obj->GetXIn();
     Double_t fY = tof_obj->GetYIn();
     Double_t fZ = tof_obj->GetZIn();
     Double_t ftime = tof_obj->GetTime();
    
    
    // offset - position of detector x: -417.359574; y: +2.4; z: +960.777114
    // angle = 31 degrees
    // cos(angle) = 0.857167301
    //sin(angle) = 0.515038075
    
//     if (total_energy_dch1>0. && total_energy_dch2>0. && total_energy_tof>0. && PID==2212){
    if (PID==2212 && mother<0){
     tfwx_p1=(((fX+417.359574) * 0.857167301) - ((fZ - 960.777114) * 0.515038075));
     tfwy_p1=(fY-2.4);
     tfwt_p1=ftime;
     TfwXhis->Fill(tfwx_p1);
     TfwYhis->Fill(tfwy_p1);
     TfwThis->Fill(tfwt_p1);

     cout<<"TFW - proton tof wall "<<PID<<endl;
     tfmul++;
     
     }

   }



AddHit(tfmul,tfwx_p1,tfwy_p1,tfwt_p1);

//     cout<<"addhit"<<nentriesTof<<" tfmul "<<tfmul<<" tfwx_p1 "<<tfwx_p1<<" tfwy_p1 "<<tfwy_p1<<endl;

}
// -------------------------------------------------------------------------

void R3BTofDigitizer::Reset(){
// Clear the structure
//   cout << " -I- Digit Reset() called " << endl;

   
 if (fTofDigi ) fTofDigi->Clear();

}   

void R3BTofDigitizer::Finish()
{
// Write control histograms
   
   TOFelosshis->Write();
   
   TfwXhis->Write();
   TfwYhis->Write();
   TfwThis->Write();


}

R3BTofDigi* R3BTofDigitizer::AddHit(Int_t tfmul,Double_t tfwx_p1,Double_t tfwy_p1,Double_t tfwt_p1){   
  TClonesArray& clref = *fTofDigi;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BTofDigi(tfmul,tfwx_p1,tfwy_p1,tfwt_p1);
 
}



//R3BTofDigi* R3BTofDigitizer::AddHit(
 // return new(clref[size]) R3BTofDigi();
//}

ClassImp(R3BTofDigitizer)
