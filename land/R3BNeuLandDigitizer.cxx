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


//#include "R3BLandPoint.h"
#include "R3BNeuLandPoint.h"
#include "R3BMCTrack.h"

		
using std::cout;
using std::endl;

		

R3BNeuLandDigitizer::R3BNeuLandDigitizer() :
  FairTask("R3BNeu Land Digitization scheme ") { 
}


R3BNeuLandDigitizer::~R3BNeuLandDigitizer() {
}


InitStatus R3BNeuLandDigitizer::Init() {
  
  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fLandPoints = (TClonesArray*) ioman->GetObject("NeuLandPoint");
  fLandMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
  
  // Register output array LandDigi
  //fDigis = new TClonesArray("R3BNeuLandDigi",1000);
  //ioman->Register("LandDigi", "Digital response in Land", fDigis, kTRUE);
  
  // Initialise control histograms
  TString elossNamesP[6] = {"Eloss-p 1", "Eloss-p 2", "Eloss-p 3",
			    "Eloss-p 4", "Eloss-p 5", "Eloss-p 6"};
  TString elossNamesE[6] = {"Eloss-e 1", "Eloss-e 2", "Eloss-e 3",
			    "Eloss-e 4", "Eloss-e 5", "Eloss-e 6"};
  TString PxOutNamesP[6] = {"PxOut-p 1", "PxOut-p 2", "PxOut-p 3",
			    "PxOut-p 4", "PxOut-p 5", "PxOut-p 6"};
  TString PyOutNamesP[6] = {"PyOut-p 1", "PyOut-p 2", "PyOut-p 3",
			    "PyOut-p 4", "PyOut-p 5", "PyOut-p 6"};
  TString PzOutNamesP[6] = {"PzOut-p 1", "PzOut-p 2", "PzOut-p 3",
			    "PzOut-p 4", "PzOut-p 5", "PzOut-p 6"};
  TString xInNamesP[6] = {"xIn-p 1", "xIn-p 2", "xIn-p 3",
			  "xIn-p 4", "xIn-p 5", "xIn-p 6"};
  TString yInNamesP[6] = {"yIn-p 1", "yIn-p 2", "yIn-p 3",
			  "yIn-p 4", "yIn-p 5", "yIn-p 6"};
  TString zInNamesP[6] = {"zIn-p 1", "zIn-p 2", "zIn-p 3",
			  "zIn-p 4", "zIn-p 5", "zIn-p 6"};
  TString momNamesP[6] = {"Momentum-p 1", "Momentum-p 2", "Momentum-p 3",
			  "Momentum-p 4", "Momentum-p 5", "Momentum-p 6"};
  TString etotNamesP[6] = {"Total energy-p 1", "Total energy-p 2", "Total energy-p 3",
			   "Total energy-p 4", "Total energy-p 5", "Total energy-p 6"};
  TString timeNamesP[6] = {"Time-p [ns] 1", "Time-p [ns] 2", "Time-p [ns] 3",
			   "Time-p [ns] 4", "Time-p [ns] 5", "Time-p [ns] 6"};
  TString timeNamesE[6] = {"Time-e [ns] 1", "Time-e [ns] 2", "Time-e [ns] 3",
			   "Time-e [ns] 4", "Time-e [ns] 5", "Time-e [ns] 6"};
  TString etotNamesE[6] = {"Total energy-e 1", "Total energy-e 2", "Total energy-e 3",
			   "Total energy-e 4", "Total energy-e 5", "Total energy-e 6"};
  TString etot_elossNamesP[6] = {"Total energy-p vs Eloss-p 1", "Total energy-p vs Eloss-p 2",
				 "Total energy-p vs Eloss-p 3", "Total energy-p vs Eloss-p 4",
				 "Total energy-p vs Eloss-p 5", "Total energy-p vs Eloss-p 6"};
  TString etot_elossNamesE[6] = {"Total energy-e vs Eloss-e 1", "Total energy-e vs Eloss-e 2",
				 "Total energy-e vs Eloss-e 3", "Total energy-e vs Eloss-e 4",
				 "Total energy-e vs Eloss-e 5", "Total energy-e vs Eloss-e 6"};

  TString elossTitlesP[6] = {"Eloss-p in gas [keV] 1",
			     "Eloss-p in gas [keV] 2",
			     "Eloss-p in gas [keV] 3",
			     "Eloss-p in gas [keV] 4",
			     "Eloss-p in gas [keV] 5",
			     "Eloss-p in gas [keV] 6"};
  TString elossTitlesE[6] = {"Eloss-e in gas [keV] 1",
			     "Eloss-e in gas [keV] 2",
			     "Eloss-e in gas [keV] 3",
			     "Eloss-e in gas [keV] 4",
			     "Eloss-e in gas [keV] 5",
			     "Eloss-e in gas [keV] 6"};
  TString PxOutTitlesP[6] = {"PxOut-p in gas [MeV/c] 1",
			     "PxOut-p in gas [MeV/c] 2",
			     "PxOut-p in gas [MeV/c] 3",
			     "PxOut-p in gas [MeV/c] 4",
			     "PxOut-p in gas [MeV/c] 5",
			     "PxOut-p in gas [MeV/c] 6"};
  TString PyOutTitlesP[6] = {"PyOut-p in gas [MeV/c] 1",
			     "PyOut-p in gas [MeV/c] 2",
			     "PyOut-p in gas [MeV/c] 3",
			     "PyOut-p in gas [MeV/c] 4",
			     "PyOut-p in gas [MeV/c] 5",
			     "PyOut-p in gas [MeV/c] 6"};
  TString PzOutTitlesP[6] = {"PzOut-p in gas [MeV/c] 1",
			     "PzOut-p in gas [MeV/c] 2",
			     "PzOut-p in gas [MeV/c] 3",
			     "PzOut-p in gas [MeV/c] 4",
			     "PzOut-p in gas [MeV/c] 5",
			     "PzOut-p in gas [MeV/c] 6"};
  TString xInTitlesP[6] = {"xIn-p in gas [mm] 1",
			   "xIn-p in gas [mm] 2",
			   "xIn-p in gas [mm] 3",
			   "xIn-p in gas [mm] 4",
			   "xIn-p in gas [mm] 5",
			   "xIn-p in gas [mm] 6"};
  TString yInTitlesP[6] = {"yIn-p in gas [mm] 1",
			   "yIn-p in gas [mm] 2",
			   "yIn-p in gas [mm] 3",
			   "yIn-p in gas [mm] 4",
			   "yIn-p in gas [mm] 5",
			   "yIn-p in gas [mm] 6"};
  TString zInTitlesP[6] = {"zIn-p in gas [mm] 1",
			   "zIn-p in gas [mm] 2",
			   "zIn-p in gas [mm] 3",
			   "zIn-p in gas [mm] 4",
			   "zIn-p in gas [mm] 5",
			   "zIn-p in gas [mm] 6"};
  TString momTitlesP[6] = {"Momentum-p in gas [MeV/c] 1",
			   "Momentum-p in gas [MeV/c] 2",
			   "Momentum-p in gas [MeV/c] 3",
			   "Momentum-p in gas [MeV/c] 4",
			   "Momentum-p in gas [MeV/c] 5",
			   "Momentum-p in gas [MeV/c] 6"};
  TString etotTitlesP[6] = {"Total energy-p in gas [MeV] 1",
			    "Total energy-p in gas [MeV] 2",
			    "Total energy-p in gas [MeV] 3",
			    "Total energy-p in gas [MeV] 4",
			    "Total energy-p in gas [MeV] 5",
			    "Total energy-p in gas [MeV] 6"};
  TString etotTitlesE[6] = {"Total energy-e in gas [keV] 1",
			    "Total energy-e in gas [keV] 2",
			    "Total energy-e in gas [keV] 3",
			    "Total energy-e in gas [keV] 4",
			    "Total energy-e in gas [keV] 5",
			    "Total energy-e in gas [keV] 6"};
  TString timeTitlesP[6] = {"Time-p in gas [ns] 1",
			    "Time-p in gas [ns] 2",
			    "Time-p in gas [ns] 3",
			    "Time-p in gas [ns] 4",
			    "Time-p in gas [ns] 5",
			    "Time-p in gas [ns] 6"};
  TString timeTitlesE[6] = {"Time-e in gas [ns] 1",
			    "Time-e in gas [ns] 2",
			    "Time-e in gas [ns] 3",
			    "Time-e in gas [ns] 4",
			    "Time-e in gas [ns] 5",
			    "Time-e in gas [ns] 6"};
  TString etot_elossTitlesP[6] = {"Total energy-p [MeV] vs Eloss [keV] in gas 1",
				  "Total energy-p [MeV] vs Eloss [keV] in gas 2",
				  "Total energy-p [MeV] vs Eloss [keV] in gas 3",
				  "Total energy-p [MeV] vs Eloss [keV] in gas 4",
				  "Total energy-p [MeV] vs Eloss [keV] in gas 5",
				  "Total energy-p [MeV] vs Eloss [keV] in gas 6"};
  TString etot_elossTitlesE[6] = {"Total energy-e [keV] vs Eloss [keV] in gas 1",
				  "Total energy-e [keV] vs Eloss [keV] in gas 2",
				  "Total energy-e [keV] vs Eloss [keV] in gas 3",
				  "Total energy-e [keV] vs Eloss [keV] in gas 4",
				  "Total energy-e [keV] vs Eloss [keV] in gas 5",
				  "Total energy-e [keV] vs Eloss [keV] in gas 6"};

  for(Int_t i=0;i<6;i++){
    h_eloss_p[i] = new TH1F(elossNamesP[i], elossTitlesP[i] , 3110 , -10.05 , 300.05);
    h_eloss_e[i] = new TH1F(elossNamesE[i], elossTitlesE[i] , 3110 , -10.05 , 300.05);
    h_xin_p[i] = new TH1F(xInNamesP[i], xInTitlesP[i] , 210 , -10.05 , 10.05);
    h_yin_p[i] = new TH1F(yInNamesP[i], yInTitlesP[i] , 210 , -10.05 , 10.05);
    h_zin_p[i] = new TH1F(zInNamesP[i], zInTitlesP[i] , 165 , 4.0 , 20.5);
    h_pxout_p[i] = new TH1F(PxOutNamesP[i], PxOutTitlesP[i] , 1001 , -500.5 , 500.5);
    h_pyout_p[i] = new TH1F(PyOutNamesP[i], PyOutTitlesP[i] , 1001 , -500.5 , 500.5);
    h_pzout_p[i] = new TH1F(PzOutNamesP[i], PzOutTitlesP[i] , 1101 , -100.5 , 1000.5);
    h_mom_p[i] = new TH1F(momNamesP[i], momTitlesP[i] , 1051 , -50.5 , 1000.5);
    h_etot_p[i] = new TH1F(etotNamesP[i], etotTitlesP[i] , 1101 , 899.5 , 2000.5);
    h_etot_e[i] = new TH1F(etotNamesE[i], etotTitlesE[i] , 950 , 495 , 9995);
    h_time_p[i] = new TH1F(timeNamesP[i], timeTitlesP[i] , 1000 , -0.1 , 1.9);
    h_time_e[i] = new TH1F(timeNamesE[i], timeTitlesE[i] , 1000 , -0.1 , 2.9);
    h_etot_eloss_p[i] = new TH2F(etot_elossNamesP[i], etot_elossTitlesP[i] , 1101 , 899.5 , 2000.5, 3110 , -10.5 , 300.5);
    h_etot_eloss_e[i] = new TH2F(etot_elossNamesE[i], etot_elossTitlesE[i] , 950 , 500.5 , 10000.5, 3110 , -10.5 , 300.5);
  }


  h_pdg = new TH1F("PDG","Code of particle deposited energy in gas", 3220, -220., 3000.);
  h_cellid = new TH1F("CellID","Gas cell ID where deposition occured", 10, 0.5, 10.5);
  h_detid = new TH1F("DetID","Detector ID where deposition occured", 101, 99.5, 200.5);
  h_trackid = new TH1F("TrackID","Track ID which deposited energy", 102, -1.5, 100.5);
  h_mot0trackid = new TH1F("Mot0TrackID","Mother of Track ID which deposited energy", 102, -1.5, 100.5);
  h_cellhits = new TH1F("Cell Hits","Number of hits in the gas cells in an event", 16, -0.5, 15.5);
  h_toteloss = new TH1F("Total Eloss", "Total energy loss in an event" , 3110 , -10.5 , 300.5);
  h_ne = new TH1F("Ne","primary_el",100,0.,100.);
  h_ch = new TH1F("Charge","",1000,0.,1.);
  h_pdg_vs_m0pdg = new TH2F("PDG vs m0PDG","",3221,-220.5,3000.5,3221,-220.5,3000.5);
  h_trackid_vs_m0trackid = new TH2F("TrackId vs Mot0TrackId","TrackId vs Mother of TrackId",102,-1.5,100.5,102,-1.5,100.5);
  h_cellhits_vs_totaleloss = new TH2F("Cell Hits vs Total Eloss","Number of hits vs Total Eloss in an event", 16, -0.5, 15.5, 3110 , -10.5 , 300.5);

  
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
  
  // Create Stochastic Avalanche
  
  //-Now do the job event/event
  

  Int_t nentries = fLandPoints->GetEntries();
  if(nentries==0){
    no_interaction = no_interaction + 1;
  }
  
  for (Int_t l=0;l<nentries;l++){
    // Get the Land Object in array
    
    R3BNeuLandPoint *land_obj = (R3BNeuLandPoint*) fLandPoints->At(l);
    Int_t eventId = land_obj->GetEventID();
    Int_t detId = land_obj->GetDetID();
    Int_t cellId = land_obj->GetCellID();
    
    Int_t TrackId = land_obj->GetTrackID();
    Int_t Mot0TrackId = land_obj->GetMot0TrackID();
    
    R3BMCTrack *aTrack = (R3BMCTrack*) fLandMCTrack->At(TrackId);
    Int_t PID = aTrack->GetPdgCode();
    Double_t eloss = land_obj->GetEnergyLoss();
    Double_t totaleloss = land_obj->GetTotalEloss();
    Int_t cellhits = land_obj->GetCellHits();
    Double_t energy = aTrack->GetEnergy();
    if(PID == 2212) energy = energy * 1E3; //MeV
    if(PID == 11) energy = energy * 1E6; //keV
    Double_t startx = aTrack->GetStartX() * 10;
    Double_t starty = aTrack->GetStartY() * 10;
    Double_t startz = aTrack->GetStartZ() * 10;
    Double_t xIn = land_obj->GetXIn() * 10;
    Double_t yIn = land_obj->GetYIn() * 10;
    Double_t zIn = land_obj->GetZIn() * 10;
    Double_t zOut = land_obj->GetZOut() * 10;
    Double_t xPos = land_obj->GetX(zIn * 0.1) * 10;
    Double_t yPos = land_obj->GetY(zIn * 0.1) * 10;
    Double_t time = land_obj->GetTime();
    Double_t PxOut = land_obj->GetPxOut() * 1E3; // MeV/c
    Double_t PyOut = land_obj->GetPyOut() * 1E3; // MeV/c
    Double_t PzOut = land_obj->GetPzOut() * 1E3; // MeV/c
    
    Double_t momentum = sqrt( PxOut * PxOut + PyOut * PyOut + PzOut * PzOut);
    
    
    if(Mot0TrackId>-1){
      R3BMCTrack *aMot0Track = (R3BMCTrack*) fLandMCTrack->At(Mot0TrackId);
      mot0PID = aMot0Track->GetPdgCode();
    }
       
    switch(cellId){
    case 1:
      if (PID == 2212 ){
	h_eloss_p[0]->Fill(eloss);
	h_pxout_p[0]->Fill(PxOut);
	h_pyout_p[0]->Fill(PyOut);
	h_pzout_p[0]->Fill(PzOut);
	h_xin_p[0]->Fill(xIn);
	h_yin_p[0]->Fill(yIn);
	h_zin_p[0]->Fill(zIn);
	h_mom_p[0]->Fill(momentum);
	h_etot_p[0]->Fill(energy);
	h_time_p[0]->Fill(time);
	h_etot_eloss_p[0]->Fill(energy,eloss);
      }
      if (PID == 11 ){
	h_eloss_e[0]->Fill(eloss);
	h_etot_e[0]->Fill(energy);
	h_time_e[0]->Fill(time);
	h_etot_eloss_e[0]->Fill(energy,eloss);
      }
      break; // case 1
    case 2:
      if (PID == 2212 ){
	h_eloss_p[1]->Fill(eloss);
	h_pxout_p[1]->Fill(PxOut);
	h_pyout_p[1]->Fill(PyOut);
	h_pzout_p[1]->Fill(PzOut);
	h_xin_p[1]->Fill(xIn);
	h_yin_p[1]->Fill(yIn);
	h_zin_p[1]->Fill(zIn);
	h_mom_p[1]->Fill(momentum);
	h_etot_p[1]->Fill(energy);
	h_time_p[1]->Fill(time);
	h_etot_eloss_p[1]->Fill(energy,eloss);
      }
      if (PID == 11 ){
	h_eloss_e[1]->Fill(eloss);
	h_etot_e[1]->Fill(energy);
	h_time_e[1]->Fill(time);
	h_etot_eloss_e[1]->Fill(energy,eloss);
      }
      break; // case 2
    case 3:
      if (PID == 2212 ){
	h_eloss_p[2]->Fill(eloss);
	h_pxout_p[2]->Fill(PxOut);
	h_pyout_p[2]->Fill(PyOut);
	h_pzout_p[2]->Fill(PzOut);
	h_xin_p[2]->Fill(xIn);
	h_yin_p[2]->Fill(yIn);
	h_zin_p[2]->Fill(zIn);
	h_mom_p[2]->Fill(momentum);
	h_etot_p[2]->Fill(energy);
	h_time_p[2]->Fill(time);
	h_etot_eloss_p[2]->Fill(energy,eloss);
      }
      if (PID == 11 ){
	h_eloss_e[2]->Fill(eloss);
	h_etot_e[2]->Fill(energy);
	h_time_e[2]->Fill(time);
	h_etot_eloss_e[2]->Fill(energy,eloss);
      }
      break; // case 3
    case 4:
      if (PID == 2212 ){
	h_eloss_p[3]->Fill(eloss);
	h_pxout_p[3]->Fill(PxOut);
	h_pyout_p[3]->Fill(PyOut);
	h_pzout_p[3]->Fill(PzOut);
	h_xin_p[3]->Fill(xIn);
	h_yin_p[3]->Fill(yIn);
	h_zin_p[3]->Fill(zIn);
	h_mom_p[3]->Fill(momentum);
	h_etot_p[3]->Fill(energy);
	h_time_p[3]->Fill(time);
	h_etot_eloss_p[3]->Fill(energy,eloss);
      }
      if (PID == 11 ){
	h_eloss_e[3]->Fill(eloss);
	h_etot_e[3]->Fill(energy);
	h_time_e[3]->Fill(time);
	h_etot_eloss_e[3]->Fill(energy,eloss);
      }
      break; // case 4
    case 5:
      if (PID == 2212 ){
	h_eloss_p[4]->Fill(eloss);
	h_pxout_p[4]->Fill(PxOut);
	h_pyout_p[4]->Fill(PyOut);
	h_pzout_p[4]->Fill(PzOut);
	h_xin_p[4]->Fill(xIn);
	h_yin_p[4]->Fill(yIn);
	h_zin_p[4]->Fill(zIn);
	h_mom_p[4]->Fill(momentum);
	h_etot_p[4]->Fill(energy);
	h_time_p[4]->Fill(time);
	h_etot_eloss_p[4]->Fill(energy,eloss);
      }
      if (PID == 11 ){
	h_eloss_e[4]->Fill(eloss);
	h_etot_e[4]->Fill(energy);
	h_time_e[4]->Fill(time);
	h_etot_eloss_e[4]->Fill(energy,eloss);
      }
      break; // case 5
    case 6:
      if (PID == 2212 ){
	h_eloss_p[5]->Fill(eloss);
	h_pxout_p[5]->Fill(PxOut);
	h_pyout_p[5]->Fill(PyOut);
	h_pzout_p[5]->Fill(PzOut);
	h_xin_p[5]->Fill(xIn);
	h_yin_p[5]->Fill(yIn);
	h_zin_p[5]->Fill(zIn);
	h_mom_p[5]->Fill(momentum);
	h_etot_p[5]->Fill(energy);
	h_time_p[5]->Fill(time);
	h_etot_eloss_p[5]->Fill(energy,eloss);
      }
      if (PID == 11 ){
	h_eloss_e[5]->Fill(eloss);
	h_etot_e[5]->Fill(energy);
	h_time_e[5]->Fill(time);
	h_etot_eloss_e[5]->Fill(energy,eloss);
      }
      break; // case 6
    } // cellId

      h_pdg->Fill(PID);
      h_detid->Fill(detId);
      h_cellid->Fill(cellId);
      h_trackid->Fill(TrackId);
      h_mot0trackid->Fill(Mot0TrackId);
      if(l == nentries-1){
	h_toteloss->Fill(totaleloss);
	h_cellhits->Fill(cellhits);
	h_cellhits_vs_totaleloss->Fill(cellhits, totaleloss);
      }
      h_pdg_vs_m0pdg->Fill(PID, mot0PID);
      h_trackid_vs_m0trackid->Fill(TrackId, Mot0TrackId);
 
      /*
      Double_t ne = 0.0;
      if ( tof[paddle] < 1.e-15 ){
	tof[paddle] = land_obj->GetTime(); // time since part. start [ns] 
	//cout << "-I- LANDOBJ tof " << tof[paddle] << endl;  
      }//! tof > 1e-15
      
       // Check the Units of Tof in [ns] here    
      if ( ( land_obj->GetTime()-tof[paddle] )*1e+9 < 100. ){
	//ne = eloss * 1e+9 / 25.; 
	ne = eloss * 1e+3 / 25.; 
	// cout << "-I- LANDOBJ Nb of el: " << ne << endl;
	
	h_ne ->Fill( ne );
	// Check the Ne ??  
	Int_t ine = (Int_t) (ne+0.5);
	
	// Loop over primary electrons
	for (Int_t i=0;i<ine;i++ ){
	  Double_t zz = gRandom->Rndm();     
	  Double_t xx = zz * 0.3;
	  Double_t dd =0.3;       // [mm]
	  
	  Double_t vv = 0.21e-3;  // [mm/ps]
	  Double_t nsteps = xx/0.0025; // mm
	  Double_t dt = 0.0025 / vv ;   // ps
	  Double_t tt = dt*nsteps; 
	  //  cout << "-I LandOBJ tt: " <<  tt << endl;
	  Double_t alpha = 123.0;
	  Double_t eta = 10.5;
	  Double_t wfield = 0.5;
	  
	  
	  Double_t qq2=0.0;
	  Double_t f1=0.0;
	  Double_t f2=0.0;
	  
	  Int_t ix = (int) (nsteps);
      */
	  // Add on Avan
	  /*   for (Int_t j=1;j<ix+1;j++){
	       Double_t nbar = TMath::Exp((alpha-eta)*j*0.0025);
	       Double_t kk = eta/alpha;
	       zz = gRandom->Rndm();
	       Double_t avan =0.0;
	       if ( zz < (kk*(nbar-1.)/(nbar-kk))) avan =0.0;
	       else {
	       f1 = TMath::Log(1.-(1.-kk)/(nbar-kk));
	       f2=  TMath::Log((nbar-kk)*(1.-zz)/nbar/(1.-kk));
	       avan =1.+ f2/f1;
	       }
	       //cout << "-I- LandOBJ f1:" << f1 << " f2: " << f2 << endl;
	       // QDC Saturation   
	       if (avan > 1.6e+7 ) avan = 1.6e+7;
	       //cout << "-I- LandOBJ Avalanche:" << j << " avan: " << avan << endl;
	       Double_t current = wfield*vv*1.6022e-19*avan;
	       //cout << "-I- LandOBJ current:" << current << endl;
	       qq2=qq2+current*dt;
	       }// !j
	  */
	  /*
	  qq2= 0.5 / (alpha-eta)*1.6022e-19* TMath::Exp((alpha-eta)*(0.3-xx)-1)*1.e+12;
	  Double_t qcharge = qq2 ;
	  // Fill Charge Control hist.
	  h_ch->Fill(qcharge);
	  
	  //??? The processHit has to be changed and Hit Info also !!!
	  x_pos[paddle]  = x_pos[paddle] + land_obj->GetXOut() *qcharge;
	  y_pos[paddle]  = y_pos[paddle] + land_obj->GetYOut() *qcharge;
	  z_pos[paddle]  = z_pos[paddle] + land_obj->GetZOut() *qcharge;
	  nuhits[paddle] = nuhits[paddle] +1;
	  
	  paddle_E[paddle][gap] = paddle_E[paddle][gap] + qcharge;
	  paddle_E[paddle][9] = paddle_E[paddle][9] + qcharge;
	  
	  Int_t first =  (Int_t) ((paddle/20.)/2.);
	  Int_t second = (Int_t) (paddle/20./2.);
	  
	  // ?? here what is the diff ??
	  if(first==second){
	    pm[paddle][1] = pm[paddle][1]+qcharge;
	    pm[paddle][2] = pm[paddle][2]+qcharge;
	    
	  }else{
	    
	    pm[paddle][1] = pm[paddle][1]+qcharge;
	    pm[paddle][2] = pm[paddle][2]+qcharge;
	    
	  }
	  
	  if (paddle_E[paddle][gap]>2.5) continue;
	  
	}//! ine
	}//!tof */
      //}//! eloss	 
    
  }//! MC hits   
 
}
// -------------------------------------------------------------------------

void R3BNeuLandDigitizer::Reset()
{
  // Clear the structure
  //   cout << " -I- Digit Reset() called " << endl;
  
  for(Int_t i=0;i<200;i++) {
    nuhits[i]=0;
    x_pos[i]=y_pos[i]=z_pos[i]=0.0;
    tof[i]=0.0;
    
    for (Int_t j=0;j<2;j++){
      pm[i][j]=0.0;
    }
    for (Int_t k=0;k<9;k++){
      paddle_E[i][k]=0.0;
    }
    
  } 
  
  for(Int_t i=0;i<50;i++) {
    part_E[i] = part_n[i] = 0.0;      
  }
  
  for(Int_t i=0;i<4;i++) {
    first_hit[i] = 0.0;      
  }   
  
  // if (fDigis ) fDigis->Clear();
}   

void R3BNeuLandDigitizer::Finish()
{
  // here event. write histos
  //   cout << " -I- Digit Finish() called " << endl;
  // Write control histograms
  for(Int_t i=0;i<6;i++){
    h_eloss_p[i]->Write();
    h_pxout_p[i]->Write();
    h_pyout_p[i]->Write();
    h_pzout_p[i]->Write();
    h_xin_p[i]->Write();
    h_yin_p[i]->Write();
    h_zin_p[i]->Write();
    h_mom_p[i]->Write();
    h_etot_p[i]->Write();
    h_time_p[i]->Write();
    h_etot_eloss_p[i]->Write();

    h_eloss_e[i]->Write();
    h_etot_e[i]->Write();
    h_time_e[i]->Write();
    h_etot_eloss_e[i]->Write();
  }
  h_ne->Write();
  h_ch->Write();
  h_pdg->Write();
  h_detid->Write();
  h_cellid->Write();
  h_trackid->Write();
  h_mot0trackid->Write();
  h_toteloss->Write();
  h_cellhits->Write();
  h_pdg_vs_m0pdg->Write();
  h_trackid_vs_m0trackid->Write();
  h_cellhits_vs_totaleloss->Write();

  cout << " Number of non-interacting primaries: " << no_interaction << endl;
}

ClassImp(R3BNeuLandDigitizer)
