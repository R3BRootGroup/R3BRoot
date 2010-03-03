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
  h_pdg = new TH1F("PDG","Code of particle deposited energy in gas",3000,-220.,3000.);
  h_eloss = new TH1F("Eloss","Energy loss in gas [keV]",100,0.,50.);
  h_ne = new TH1F("Ne","primary_el",100,0.,100.);
  h_ch = new TH1F("Charge","",1000,0.,1.);
  h_time = new TH1F("Time [ns]","",100,0.,1000.);
  h_energy = new TH1F("Total energy [MeV]","",1000,0.,4000.);
  h_zpos = new TH1F("Zpos [mm]","",1000,-1.,30.);
  h_xpos = new TH1F("Xpos [mm]","",100,-50.,50.);
  h_ypos = new TH1F("Ypos [mm]","",100,-50.,50.);
  h_xpos1_vs_ypos1 = new TH2F("Xpos1 [mm] vs Ypos1 [mm]","",100,-50.,50.,100,-50.,50.);
  h_xpos2_vs_ypos2 = new TH2F("Xpos2 [mm] vs Ypos2 [mm]","",100,-50.,50.,100,-50.,50.);
  h_xpos3_vs_ypos3 = new TH2F("Xpos3 [mm] vs Ypos3 [mm]","",100,-50.,50.,100,-50.,50.);
  h_xpos4_vs_ypos4 = new TH2F("Xpos4 [mm] vs Ypos4 [mm]","",100,-50.,50.,100,-50.,50.);
  h_xpos5_vs_ypos5 = new TH2F("Xpos5 [mm] vs Ypos5 [mm]","",100,-50.,50.,100,-50.,50.);
  h_xpos6_vs_ypos6 = new TH2F("Xpos6 [mm] vs Ypos6 [mm]","",100,-50.,50.,100,-50.,50.);
  h_pdg_vs_eloss = new TH2F("PDG vs Eloss [keV]","",100,-220.,3000.,1000,0.,50.);
  h_zpos_vs_pdg = new TH2F("Zpos [mm] vs PDG","",100,-1.,30.,100,-220.,3000.);
  h_zpos_vs_eloss = new TH2F("Zpos [mm] vs Eloss [keV]","",100,-1.,30.,100,0.,50.);
  h_zpos_vs_time = new TH2F("Zpos [mm] vs Time [ns]","",100,-1.,30.,100,0.,1000.);
  h_pdg_vs_m0pdg = new TH2F("PDG vs m0PDG","",3000,-10000.,10000.,3000,-10000.,10000.);
  h_pdg_vs_m1pdg = new TH2F("PDG vs m1PDG","",3000,-10000.,10000.,3000,-10000.,10000.);
  h_pdg_vs_m2pdg = new TH2F("PDG vs m2PDG","",3000,-10000.,10000.,3000,-10000.,10000.);
  h_pdg_vs_m3pdg = new TH2F("PDG vs m3PDG","",3000,-10000.,10000.,3000,-10000.,10000.);
  h_energy_vs_eloss = new TH2F("Total energy [MeV] vs Eloss [keV]","",100,0.,4000.,100,0.,50.);
  h_energy_vs_pdg = new TH2F("Total energy [MeV] vs PDG [keV]","",10000,0.,4000.,100,-220.,3000.);
  h_trackid_vs_m0trackid = new TH2F("TrackId vs m0TrackId","",4002,-1.5,4000.5,4002,-1.5,4000.5);
  h_trackid_vs_m1trackid = new TH2F("TrackId vs m1TrackId","",4002,-1.5,4000.5,4002,-1.5,4000.5);
  h_trackid_vs_m2trackid = new TH2F("TrackId vs m2TrackId","",4002,-1.5,4000.5,4002,-1.5,4000.5);
  h_trackid_vs_m3trackid = new TH2F("TrackId vs m3trackId","",4002,-1.5,4000.5,4002,-1.5,4000.5);

  
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
  
  // Create Stockastic Avalanche
  
  //-Now do the job event/event
  //Int_t no_interaction = 0;
  Int_t nentries = fLandPoints->GetEntries();
  if(nentries==0){
    no_interaction = no_interaction + 1;
    cout << " no_interaction: " << no_interaction << endl;
  }

  
  for (Int_t l=0;l<nentries;l++){
    // Get the Land Object in array
    R3BNeuLandPoint *land_obj = (R3BNeuLandPoint*) fLandPoints->At(l);
    Int_t detId = land_obj->GetDetID();
    Int_t segId = land_obj->GetSegID();
    Int_t cellId = land_obj->GetCellID();
    //cout << " detID: " << detId << " segID: " << segId << " cellID: " << cellId << endl;

    Int_t TrackId = land_obj->GetTrackID();
    Int_t Mot0TrackId = land_obj->GetMot0TrackID();
    Int_t Mot1TrackId = land_obj->GetMot1TrackID();
    Int_t Mot2TrackId = land_obj->GetMot2TrackID();
    Int_t Mot3TrackId = land_obj->GetMot3TrackID();
    //cout << " TrackID: " << TrackId << endl;
    //cout << " Mot0TrackID: " << Mot0TrackId << endl;
    //cout << " Mot1TrackID: " << Mot1TrackId << endl;
    //cout << " Mot2TrackID: " << Mot2TrackId << endl;
    //cout << " Mot3TrackID: " << Mot3TrackId << endl;

    R3BMCTrack *aTrack = (R3BMCTrack*) fLandMCTrack->At(TrackId);
    Int_t PID = aTrack->GetPdgCode();
    Double_t eloss = land_obj->GetEnergyLoss();
    Double_t energy = aTrack->GetEnergy() * 1E3; //MeV
    Double_t zIn = land_obj->GetZIn() * 10;
    Double_t xPos = land_obj->GetX(zIn * 0.1) * 10;
    Double_t yPos = land_obj->GetY(zIn * 0.1) * 10;
    Double_t time = land_obj->GetTime();
    
    //cout << " energy: " << eloss << endl;

    
    if(Mot0TrackId>-1){
      R3BMCTrack *aMot0Track = (R3BMCTrack*) fLandMCTrack->At(Mot0TrackId);
      mot0PID = aMot0Track->GetPdgCode();
      //cout << " mot0PID: " << mot0PID << endl;
    }
    else{
      Mot0TrackId=Mot1TrackId=Mot2TrackId=Mot3TrackId=mot0PID=mot1PID=mot2PID=mot3PID=-1;
      //cout << " mot1PID: " << mot1PID << endl;
      //cout << " mot2PID: " << mot2PID << endl;
      //cout << " mot3PID: " << mot3PID << endl;
    }
    if(Mot1TrackId>-1){
      R3BMCTrack *aMot1Track = (R3BMCTrack*) fLandMCTrack->At(Mot1TrackId);
      mot1PID = aMot1Track->GetPdgCode();
      //cout << " mot1PID: " << mot1PID << endl;
    }
    else{
      Mot1TrackId=Mot2TrackId=Mot3TrackId=mot1PID=mot2PID=mot3PID=-1;
    }
    if(Mot2TrackId>-1){
      R3BMCTrack *aMot2Track = (R3BMCTrack*) fLandMCTrack->At(Mot2TrackId);
      mot2PID = aMot2Track->GetPdgCode();
      //cout << " mot2PID: " << mot2PID << endl;
    }
    else{
      Mot2TrackId=Mot3TrackId=mot2PID=mot3PID=-1;
    }
    if(Mot3TrackId>-1){
      R3BMCTrack *aMot3Track = (R3BMCTrack*) fLandMCTrack->At(Mot3TrackId);
      mot3PID = aMot3Track->GetPdgCode();
      //cout << " mot3PID: " << mot3PID << endl;
      cout << "error: 3rd mother is not a primary. " << endl;
    }
    else{
      Mot3TrackId=mot3PID=-1;
    }

    if (eloss > 0.0 ) {
      if(detId == 101){
	
	h_pdg->Fill(PID);
	h_time->Fill(time);
	h_eloss->Fill( eloss );
	h_xpos->Fill( xPos );
	h_ypos->Fill( yPos );
	h_zpos->Fill( zIn );
	
	//if(PID == 2212){
	if(segId == 11){
	  if(cellId == 1){
	    h_xpos1_vs_ypos1->Fill(xPos , yPos );
	    //h_pdg_vs_m0pdg->Fill(PID, mot0PID);
	    //h_pdg_vs_m1pdg->Fill(PID, mot1PID);
	    //h_pdg_vs_m2pdg->Fill(PID, mot2PID);
	    //h_pdg_vs_m3pdg->Fill(PID, mot3PID);
	  }
	  if(cellId == 2){
	    h_xpos2_vs_ypos2->Fill(xPos , yPos );
	  }
	  if(cellId == 3){
	    h_xpos3_vs_ypos3->Fill(xPos , yPos );
	  }
	} //segID
	if(segId == 12){
	  if(cellId == 3){
	    h_xpos4_vs_ypos4->Fill(xPos , yPos );
	  }
	  if(cellId == 2){
	    h_xpos5_vs_ypos5->Fill(xPos , yPos );
	  }
	  if(cellId == 1){
	    h_xpos6_vs_ypos6->Fill(xPos , yPos );
	  }
	} //segID
	//} // PID
	
	h_pdg_vs_eloss->Fill(PID , eloss ); // keV
	h_zpos_vs_pdg->Fill(zIn, PID);
	h_zpos_vs_eloss->Fill(zIn, eloss);
	h_zpos_vs_time->Fill(zIn, time);
	h_pdg_vs_m0pdg->Fill(PID, mot0PID);
	h_pdg_vs_m1pdg->Fill(PID, mot1PID);
	h_pdg_vs_m2pdg->Fill(PID, mot2PID);
	h_pdg_vs_m3pdg->Fill(PID, mot3PID);
	if(PID == 11){
	  h_trackid_vs_m0trackid->Fill(TrackId, Mot0TrackId);
	  h_trackid_vs_m1trackid->Fill(TrackId, Mot1TrackId);
	  h_trackid_vs_m2trackid->Fill(TrackId, Mot2TrackId);
	  h_trackid_vs_m3trackid->Fill(TrackId, Mot3TrackId);
	}
	h_energy->Fill( energy ); // MeV
	h_energy_vs_eloss->Fill(energy , eloss );
	h_energy_vs_pdg->Fill(energy , PID);
      } // detID
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
    }//! eloss	 
    
  }//! MC hits   
  
}
// -------------------------------------------------------------------------

void R3BNeuLandDigitizer::Reset(){
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
  h_eloss->Write();
  h_ne->Write();
  h_ch->Write();
  h_pdg->Write();
  h_time->Write();
  h_energy->Write();
  h_xpos->Write();
  h_ypos->Write();
  h_xpos1_vs_ypos1->Write();
  h_xpos2_vs_ypos2->Write();
  h_xpos3_vs_ypos3->Write();
  h_xpos4_vs_ypos4->Write();
  h_xpos5_vs_ypos5->Write();
  h_xpos6_vs_ypos6->Write();
  h_zpos->Write();
  h_pdg_vs_eloss->Write();
  h_zpos_vs_pdg->Write();
  h_zpos_vs_eloss->Write();
  h_zpos_vs_time->Write();
  h_pdg_vs_m0pdg->Write();
  h_pdg_vs_m1pdg->Write();
  h_pdg_vs_m2pdg->Write();
  h_pdg_vs_m3pdg->Write();
  h_energy_vs_eloss->Write();
  h_energy_vs_pdg->Write();
  h_trackid_vs_m0trackid->Write();
  h_trackid_vs_m1trackid->Write();
  h_trackid_vs_m2trackid->Write();
  h_trackid_vs_m3trackid->Write();
}

ClassImp(R3BNeuLandDigitizer)
