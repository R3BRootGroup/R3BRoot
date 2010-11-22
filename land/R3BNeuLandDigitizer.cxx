#include "R3BNeuLandDigitizer.h"
#include "TClonesArray.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"


#include "TVector3.h"
#include "TMath.h"
#include "TRandom.h"
#include "TH1F.h"
#include "TH2F.h"
#include <string>
#include <iostream>

#include "R3BNeuLandPoint.h"
#include "R3BMCTrack.h"

using std::cout;
using std::endl;

		

R3BNeuLandDigitizer::R3BNeuLandDigitizer() :
  FairTask("R3BNeuLand Digitization scheme ") { 
}


R3BNeuLandDigitizer::~R3BNeuLandDigitizer() {
}


InitStatus R3BNeuLandDigitizer::Init() {

  NofNeuLandPoints = 0;
  NofEvts = 0;
  NofProtEvts = 0;

  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fLandPoints = (TClonesArray*) ioman->GetObject("NeuLandPoint");
  fLandMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
  
  // Register output array LandDigi
  //fDigis = new TClonesArray("R3BNeuLandDigi",1000);
  //ioman->Register("LandDigi", "Digital response in Land", fDigis, kTRUE);

  h_time = new TH1F("Time","Time of detection", 1500, 150.05, 300.05);
  h_xAv = new TH1F("xAv","xAv pattern for the entries", 300, -150.5, 149.5);
  h_yAv = new TH1F("yAv","yAv pattern for the entries", 300, -0.5, 299.5);
  h_eDepTot = new TH1F("eDepTot", "Deposited energy for the entries", 3000, -0.05, 299.95);



  return kSUCCESS;

}

void R3BNeuLandDigitizer::SetParContainers() {
  
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");
  
  //FairRuntimeDb* db = run->GetRuntimeDb();
  //if ( ! db ) Fatal("SetParContainers", "No runtime database");
  
  
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void R3BNeuLandDigitizer::Exec(Option_t* opt) {
  
  //-Reset entries in output arrays
  //-Reset local arrays 
  Reset();
  
  Int_t NofEntr = fLandPoints->GetEntries();
  
  Int_t eventID[NofEntr];
  Int_t trackID[NofEntr];
  Int_t parentTrackID[NofEntr];
  Int_t detLayerID[NofEntr];
  Int_t cellID[NofEntr];
  Int_t segID[NofEntr];
  Double_t mass[NofEntr];
  Double_t time[NofEntr];
  Double_t eDepTot[NofEntr];
  Double_t xIn[NofEntr];
  Double_t yIn[NofEntr];
  Double_t xOut[NofEntr];
  Double_t yOut[NofEntr];
  Double_t zIn[NofEntr];
  Double_t zOut[NofEntr];
  Double_t PxIn[NofEntr];
  Double_t PyIn[NofEntr];
  Double_t PzIn[NofEntr];
  Double_t PxOut[NofEntr];
  Double_t PyOut[NofEntr];
  Double_t PzOut[NofEntr];

  Int_t partID[NofEntr];
  Int_t parentPartID[NofEntr];
  Double_t KinE[NofEntr];
  Double_t xAv[NofEntr];
  Double_t yAv[NofEntr];
  
  Bool_t ProtFlag[NofEntr];

  // fill arrays
  if(NofEntr > 0) {
    for (Int_t l=0; l<NofEntr; l++) {
      R3BNeuLandPoint* land_obj = (R3BNeuLandPoint*) fLandPoints->At(l);
      eventID[l] = land_obj->GetEventID();
      trackID[l] = land_obj->GetTrackID();
      parentTrackID[l] = land_obj->GetMot0TrackID();
      detLayerID[l] = land_obj->GetDetID();
      cellID[l] = land_obj->GetCellID();
      segID[l] = land_obj->GetSegID();
      mass[l] = land_obj->GetMass();
      time[l] = land_obj->GetTime();
      eDepTot[l] = land_obj->GetEnergyLoss();
      xIn[l] = land_obj->GetXIn(); // mm
      yIn[l] = land_obj->GetYIn(); // mm
      zIn[l] = land_obj->GetZIn(); // mm
      xOut[l] = land_obj->GetXOut(); // mm
      yOut[l] = land_obj->GetYOut(); // mm
      zOut[l] = land_obj->GetZOut(); // mm
      PxOut[l] = land_obj->GetPxOut() * 1E3; // MeV/c
      PyOut[l] = land_obj->GetPyOut() * 1E3; // MeV/c
      PzOut[l] = land_obj->GetPzOut() * 1E3; // MeV/c

      xAv[l] = ( xIn[l] + xOut[l] ) / 2; // mm
      yAv[l] = ( yIn[l] + yOut[l] ) / 2; // mm

      R3BMCTrack* aTrack = (R3BMCTrack*) fLandMCTrack->At(trackID[l]);
      partID[l] = aTrack->GetPdgCode();

      if(partID[l] == 2212) ProtFlag[l] = 1;
      else ProtFlag[l] = 0;

      if(trackID[l] > 0) {
	R3BMCTrack* aTrack1 = (R3BMCTrack*) fLandMCTrack->At(parentTrackID[l]);
	parentPartID[l] = aTrack->GetPdgCode();
      }

      Double_t energy = aTrack->GetEnergy();
      KinE[l] = (energy - mass[l]) * 1E3; // MeV


      h_time->Fill(time[l]);
      h_xAv->Fill(xAv[l]);
      h_yAv->Fill(yAv[l]);
      h_eDepTot->Fill(eDepTot[l]);

    } // for(NofEntr)
  } // if(NofEntr)



  NofEvts++;
  NofNeuLandPoints += NofEntr;
  
  Bool_t UsedProtEvt = 0;
  for (Int_t l=0; l<NofEntr; l++) {
    if(UsedProtEvt == 0 && ProtFlag[l] == 1) {
      NofProtEvts++;
      UsedProtEvt = 1;
    }
  }

}
// -------------------------------------------------------------------------

void R3BNeuLandDigitizer::Reset()
{
}   

void R3BNeuLandDigitizer::Finish()
{
  cout << " Number of NeuLandPoints: " << NofNeuLandPoints << endl;
  cout << " Percentage of events with proton involvement: " << 100*NofProtEvts/NofEvts << "%"<< endl;

  h_time->Write();
  h_xAv->Write();
  h_yAv->Write();
  h_eDepTot->Write();
}

ClassImp(R3BNeuLandDigitizer)
