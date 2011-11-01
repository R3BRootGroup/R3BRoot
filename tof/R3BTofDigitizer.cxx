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
#include "R3BMCTrack.h"

		
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
     Double_t tfwpath_p1;
     Double_t tfwpx_p1;
     Double_t tfwpy_p1;
     Double_t tfwpz_p1;
   

   for (Int_t l=0;l<nentriesTof;l++){
//cout<<"Point 1"<<endl;    
     R3BTofPoint *tof_obj = (R3BTofPoint*) fTofPoints->At(l);     
//     if (tof_obj==NULL) continue;
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
  tfwpath_p1=0;
  tfwpx_p1=0;
  tfwpy_p1=0;
  tfwpz_p1=0;
  
   for (Int_t l=0;l<nentriesTof;l++){
//cout<<"Point 2"<<endl;    
     R3BTofPoint *tof_obj = (R3BTofPoint*) fTofPoints->At(l);     
//     if (tof_obj==NULL) continue;     
     TrackIdTof = tof_obj->GetTrackID();
     R3BMCTrack *aTrack = (R3BMCTrack*) fTofMCTrack->At(TrackIdTof);   
     Int_t PID = aTrack->GetPdgCode();
     Int_t mother = aTrack->GetMotherId();
 
     Double_t fX_in = tof_obj->GetXIn();
     Double_t fY_in = tof_obj->GetYIn();
     Double_t fZ_in = tof_obj->GetZIn();
     Double_t fX_out = tof_obj->GetXOut();
     Double_t fY_out = tof_obj->GetYOut();
     Double_t fZ_out = tof_obj->GetZOut();
     Double_t PX_in = tof_obj->GetPx();
     Double_t PY_in = tof_obj->GetPy();
     Double_t PZ_in = tof_obj->GetPz();
     Double_t PX_out = tof_obj->GetPxOut();
     Double_t PY_out = tof_obj->GetPyOut();
     Double_t PZ_out = tof_obj->GetPzOut();
     Double_t ftime = tof_obj->GetTime();
     Double_t flength = tof_obj->GetLength();
     
     Double_t fX = ((fX_in + fX_out)/2);
     Double_t fY = ((fY_in + fY_out)/2);
     Double_t fZ = ((fZ_in + fZ_out)/2); 
     
     Double_t PX = ((PX_in + PX_out)/2);
     Double_t PY = ((PY_in + PY_out)/2);
     Double_t PZ = ((PZ_in + PZ_out)/2);
 
    
    
    // offset - position of detector x: -417.359574; y: +2.4; z: +960.777114
    // angle = -31 degrees
    // cos(angle) = 0.857167301
    //sin(angle) = -0.515038075
    
//     if (total_energy_dch1>0. && total_energy_dch2>0. && total_energy_tof>0. && PID==2212){
    if (PID==2212 && mother<0){
     tfwx_p1=(((fX+417.359574) * 0.857167301) - ((fZ - 960.777114) * (-0.515038075)));
//     tfwx_p1=(((fX+421.33683) * 0.857167301) - ((fZ - 958.387337) * (-0.515038075)));  //Christoph
     tfwy_p1=(fY-2.4);
//     tfwy_p1=(fY-2.12);  //Christoph
     tfwt_p1=ftime;
     tfwpath_p1=flength;
     tfwpx_p1=PX;
     tfwpy_p1=PY;
     tfwpz_p1=PZ;
     
     TfwXhis->Fill(tfwx_p1);
     TfwYhis->Fill(tfwy_p1);
     TfwThis->Fill(tfwt_p1);
     

//     cout<<"TFW - proton tof wall "<<PID<<endl;
     tfmul++;
     
     }

   }



AddHit(tfmul,tfwx_p1,tfwy_p1,tfwt_p1,tfwpath_p1,tfwpx_p1,tfwpy_p1,tfwpz_p1);

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

R3BTofDigi* R3BTofDigitizer::AddHit(Int_t tfmul,Double_t tfwx_p1,Double_t tfwy_p1,Double_t tfwt_p1,Double_t tfwpath_p1,
Double_t tfwpx_p1,Double_t tfwpy_p1,Double_t tfwpz_p1){   
  TClonesArray& clref = *fTofDigi;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BTofDigi(tfmul,tfwx_p1,tfwy_p1,tfwt_p1,tfwpath_p1,tfwpx_p1,tfwpy_p1,tfwpz_p1);
 
}



//R3BTofDigi* R3BTofDigitizer::AddHit(
 // return new(clref[size]) R3BTofDigi();
//}

ClassImp(R3BTofDigitizer)
