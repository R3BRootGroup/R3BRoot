#include "R3BmTofDigitizer.h"
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


#include "R3BmTofPoint.h"
#include "R3BMCTrack.h"
		
using std::cout;
using std::endl;
		

R3BmTofDigitizer::R3BmTofDigitizer() :
  FairTask("R3B mTof Digitization scheme ") { 
}


R3BmTofDigitizer::~R3BmTofDigitizer() {
}


void R3BmTofDigitizer::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if ( ! rtdb ) Fatal("SetParContainers", "No runtime database");

  fmTofDigiPar = (R3BmTofDigiPar*)(rtdb->getContainer("R3BmTofDigiPar"));

  if ( fmTofDigiPar ) {
      cout << "-I- R3BmTofDigitizer::SetParContainers() "<< endl;
      cout << "-I- Container R3BmTofDigiPar  loaded " << endl;
  }

}




InitStatus R3BmTofDigitizer::Init() {

//  cout<<"Init "<<endl;
  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fmTofPoints = (TClonesArray*) ioman->GetObject("mTOFPoint");
  fmTofMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
   
  // Register output array DchDigi
  fmTofDigi = new TClonesArray("R3BmTofDigi",1000);
  ioman->Register("mTofDigi", "Digital response in mTof", fmTofDigi, kTRUE);
  
  eventNomTof=0;
  
  // Initialise control histograms
     NtfXhis = new TH1F("NtfXhis","NtfXhis",500,-80.,80.);
     NtfXhis->GetXaxis()->SetTitle("Position");
     NtfXhis->GetYaxis()->SetTitle("Counts");

     fMult = new TH1F("fMult","fMult",50,0.,50.);
     fMult->GetXaxis()->SetTitle("Multiplicity");
     fMult->GetYaxis()->SetTitle("Counts");

     fEloss = new TH1F("fEloss","fEloss",5000,0.,500.);
     fEloss->GetXaxis()->SetTitle("Energy loss in MeV");
     fEloss->GetYaxis()->SetTitle("Counts");

  return kSUCCESS;

}


void R3BmTofDigitizer::Exec(Option_t* opt) {

   Reset();
   eventNomTof+=1;
//     if(eventNomTof/1000. == (int)eventNomTof/1000.) cout<<"Event #: "<<eventNomTof-1<<endl;
  
  Int_t nentriesmTof = fmTofPoints->GetEntries();
  Int_t TrackIdmTof=0;
     
  Int_t ntmul; 
  Double_t ntfx;
  Double_t ntfy;
  Double_t ntft;
  Double_t ntfpath;
  Double_t ntfpx;
  Double_t ntfpy;
  Double_t ntfpz;
  Double_t ntfe;

  Int_t multi;

//******************** NTF ********************//
  ntmul=0;
  ntfx=0;
  ntfy=0;
  ntft=0;
  ntfpx=0;
  ntfpy=0;
  ntfpz=0;
  ntfe=0;
  
  multi=0;
   for (Int_t l=0;l<nentriesmTof;l++){
//   cout<<"entries-mTof "<<l<<endl;
    
     R3BmTofPoint *mtof_obj = (R3BmTofPoint*) fmTofPoints->At(l);     
          
     TrackIdmTof = mtof_obj->GetTrackID();
     R3BMCTrack *aTrack = (R3BMCTrack*) fmTofMCTrack->At(TrackIdmTof);   
     Int_t PID = aTrack->GetPdgCode();
     Int_t mother = aTrack->GetMotherId();
     

     Double_t fX_in = mtof_obj->GetXIn();
     Double_t fY_in = mtof_obj->GetYIn();
     Double_t fZ_in = mtof_obj->GetZIn();
     Double_t fX_out = mtof_obj->GetXOut();
     Double_t fY_out = mtof_obj->GetYOut();
     Double_t fZ_out = mtof_obj->GetZOut();
     Double_t PX_in = mtof_obj->GetPx();
     Double_t PY_in = mtof_obj->GetPy();
     Double_t PZ_in = mtof_obj->GetPz();
     Double_t PX_out = mtof_obj->GetPxOut();
     Double_t PY_out = mtof_obj->GetPyOut();
     Double_t PZ_out = mtof_obj->GetPzOut();
     Double_t ftime = mtof_obj->GetTime();
     Double_t flength = mtof_obj->GetLength();
     Double_t TOFeloss = mtof_obj->GetEnergyLoss()*1000;
     
     Double_t fX = ((fX_in + fX_out)/2);
     Double_t fY = ((fY_in + fY_out)/2);
     Double_t fZ = ((fZ_in + fZ_out)/2);
     
     Double_t PX = ((PX_in + PX_out)/2);
     Double_t PY = ((PY_in + PY_out)/2);
     Double_t PZ = ((PZ_in + PZ_out)/2);

     
     fEloss->Fill(TOFeloss);
     if(TOFeloss>4.) multi++;
     
    // offset - position of detector x: -154.815998; z: +761.75516
    // angle = -16.7 degrees
    // cos(angle) = 0.957822495
    //sin(angle) = -0.28736052
    
//    if (PID==2212 && mother<0){

    if (PID==1000080150 && mother<0){
//    if (PID==1000170310 && mother<0){  //Christoph  1p
//      if (PID==1000160300 && mother<0){  //Christoph  2p

     ntfx=(-(((fX + 157.536214) * 0.957822495) - ((fZ - 760.139056) * (-0.28736052)))); //new x position to set tracker, justyna
     //ntfx=(-(((fX + 157.536214) * 0.957822495) - ((fZ - 760.939056) * (-0.28736052))));//x position swop (with -) - requirement for tracker //justyna new
     //ntfx=(-(((fX + 154.815998) * 0.957822495) - ((fZ - 761.75516) * (-0.28736052))));//x position swop (with -) - requirement for tracker
//     ntfx=(-(((fX + 154.10721) * 0.957822495) - ((fZ - 761.967807) * (-0.28736052))));  //Christoph
     //ntfy=fY;
     ntfy=fY+0.01;
//     ntfy=(fY-0.92);  //Christoph
     ntft=ftime;
     ntfpath=flength;
     ntfpx=PX;
     ntfpy=PY;
     ntfpz=PZ;
     ntfe += TOFeloss;
     
     
     NtfXhis->Fill(ntfx);
//     cout<<"NTF - fragment tof wall "<<PID<<endl;
//     cout<<"NTF X "<<fX<<" Y "<<fY<<" Z "<<fZ<<endl;
     ntmul++;
     }
     
   }
   if (multi>0) fMult->Fill(multi);
   if(eventNomTof<100) cout << "Event " << eventNomTof << " Mult "<< multi << endl;

AddHit(ntmul,ntfx,ntfy,ntft,ntfpath,ntfpx,ntfpy,ntfpz,ntfe);


}
// -------------------------------------------------------------------------

void R3BmTofDigitizer::Reset(){
// Clear the structure
//   cout << " -I- Digit Reset() called " << endl;

   
 if (fmTofDigi ) fmTofDigi->Clear();

}   

void R3BmTofDigitizer::Finish()
{
// Write control histograms

   NtfXhis->Write();
   fEloss->Write();
   fMult->Write();

}

R3BmTofDigi* R3BmTofDigitizer::AddHit(Int_t ntmul,Double_t ntfx,Double_t ntfy,Double_t ntft,Double_t ntfpath,Double_t ntfpx,
//Double_t ntfpy,Double_t ntfpz){   
Double_t ntfpy,Double_t ntfpz, Double_t ntfe){   
  TClonesArray& clref = *fmTofDigi;
  Int_t size = clref.GetEntriesFast();
  //return new(clref[size]) R3BmTofDigi(ntmul,ntfx,ntfy,ntft,ntfpath,ntfpx,ntfpy,ntfpz);
  return new(clref[size]) R3BmTofDigi(ntmul,ntfx,ntfy,ntft,ntfpath,ntfpx,ntfpy,ntfpz,ntfe);
 
}



//R3BDchDigi* R3BDchDigitizer::AddHit(
 // return new(clref[size]) R3BDchDigi();
//}

ClassImp(R3BmTofDigitizer)
