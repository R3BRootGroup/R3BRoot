#include "R3BLandDigitizer.h"
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


#include "R3BLandPoint.h"
#include "R3BMCTrack.h"		
using std::cout;
using std::endl;

		

R3BLandDigitizer::R3BLandDigitizer() :
  FairTask("R3B Land Digitization scheme ")
{
  fVerbose = 1;
}


R3BLandDigitizer::R3BLandDigitizer(Int_t verbose) :
  FairTask("R3B Land Digitization scheme ", verbose)
{
  fVerbose = verbose;
}


R3BLandDigitizer::~R3BLandDigitizer() {
}


void R3BLandDigitizer::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if ( ! rtdb ) Fatal("SetParContainers", "No runtime database");

  fLandDigiPar = (R3BLandDigiPar*)(rtdb->getContainer("R3BLandDigiPar"));

  if ( fVerbose && fLandDigiPar ) {
      cout << "-I- R3BLandDigitizer::SetParContainers() "<< endl;
      cout << "-I- Container R3BLandDigiPar  loaded " << endl;
  }

}




InitStatus R3BLandDigitizer::Init() {

  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fLandPoints = (TClonesArray*) ioman->GetObject("LandPoint");
  fLandMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
   
  // Register output array LandDigi
  fLandDigi = new TClonesArray("R3BLandDigi",1000);
  ioman->Register("LandDigi", "Digital response in Land", fLandDigi, kTRUE);

  // Parameter retrieval
  // Only after Init one retrieve the Digitization Parameters!
  npaddles = fLandDigiPar->GetMaxPaddle()+1;
  nplanes = fLandDigiPar->GetMaxPlane();
  paddle_per_plane=(npaddles-1)/nplanes;
  cout<<"# paddles: "<<npaddles-1<<"  # planes: "<<nplanes<<endl;
  cout<<"paddles per plane: "<<paddle_per_plane<<endl;
  mn = 939.565; // mass of neutron in MeV/c**2
  mnu = 1.0086649; // mass of neutron in atomic mass units
  amu = 931.494028; //atomic mass unit in MeV/c**2
  c = 2.99792458E8;
  cMedia = 14.;// speed of light in material in [cm/ns]
  eventNo=0;

  PM_res=new PM_RES*[npaddles];
  for (int i=0;i<npaddles;i++){
    PM_res[i]=new PM_RES[1500];
  }  
  // Initialise control histograms
     hPMl = new TH1F("PM_left","Arrival times of left PM",1000,0.,1000.);
     hPMl->GetXaxis()->SetTitle("Time (ns)");
     hPMl->GetYaxis()->SetTitle("Counts");

     hPMr = new TH1F("PM_right","Arrival times of right PM",1000,0.,1000.);
     hPMr->GetXaxis()->SetTitle("Time (ns)");
     hPMr->GetYaxis()->SetTitle("Counts");

     hTotalLight = new TH1F("Total_Light","Total light detected (energy equivalent)",5000,0.,1000.);
     hTotalLight->GetXaxis()->SetTitle("Energy (MeV)");
     hTotalLight->GetYaxis()->SetTitle("Counts");

     hTotalLight1 = new TH1F("Total_Light1","Total light detected (energy equivalent)",55,-0.02,1.08);
     hTotalLight1->GetXaxis()->SetTitle("Energy/Ebeam");
     hTotalLight1->GetYaxis()->SetTitle("Counts");
  
     hTotalEnergy = new TH1F("Total_Energy","Total energy detected",5000,0.,5000.);
     hTotalEnergy->GetXaxis()->SetTitle("Energy (MeV)");
     hTotalEnergy->GetYaxis()->SetTitle("Counts");

     hTotalEnergy1 = new TH1F("Total_Energy1","Total energy detected / Ebeam",55,-0.02,1.08);
     hTotalEnergy1->GetXaxis()->SetTitle("Energy/Ebeam");
     hTotalEnergy1->GetYaxis()->SetTitle("Counts");
     
     hTotalLightRel = new TH1F("Total_Light_rel","Relative Total light detected (energy equivalent)",1000,0.,1000.);
     hTotalLightRel->GetXaxis()->SetTitle("Energy/Ebeam");
     hTotalLightRel->GetYaxis()->SetTitle("Counts");
  
     hTotalLightRel2 = new TH1F("Total_Light_rel2","Relative Total light detected (energy equivalent)",55,0.,1.1);
     hTotalLightRel2->GetXaxis()->SetTitle("Energy/Ebeam");
     hTotalLightRel2->GetYaxis()->SetTitle("Counts");
  
     hTotalEnergyRel = new TH1F("Total_Energy_rel","Relative Total energy detected",5000,0.,5000);
     hTotalEnergyRel->GetXaxis()->SetTitle("Energy/Ebeam");
     hTotalEnergyRel->GetYaxis()->SetTitle("Counts");

     hParticle = new TH1F("Particle","PID of particle which produced the energy loss",3000,0.,3000.);
     hParticle->GetXaxis()->SetTitle("Particle ID");
     hParticle->GetYaxis()->SetTitle("Counts");
  
     hPaddleEnergy = new TH1F("PaddleEnergy","Energy deposit in one paddle",500,0.,100.);
     hPaddleEnergy->GetXaxis()->SetTitle("Energy (MeV)");
     hPaddleEnergy->GetYaxis()->SetTitle("Counts");

     hFirstEnergy = new TH1F("FirstEnergy","First energy deposit in a paddle",500,0.,100.);
     hFirstEnergy->GetXaxis()->SetTitle("Energy (MeV)");
     hFirstEnergy->GetYaxis()->SetTitle("Counts");

     hElossLight = new TH2F("ElossLight","Light quenching for protons",200,0.,1000.,200,0.,1000.);
     hElossLight->GetXaxis()->SetTitle("Energy (MeV)");
     hElossLight->GetYaxis()->SetTitle("Light (MeV)");
        
  if(npaddles<202){
     //LAND detector

     hMult1 = new TH1F("Multiplicity1","Paddle multiplicity if two Pms have fired",20,-0.5,19.5);
     hMult1->GetXaxis()->SetTitle("Multiplicity");
     hMult1->GetYaxis()->SetTitle("Counts");

     hMult2 = new TH1F("Multiplicity2","Paddle multiplicity if one PM has fired",20,-0.5,19.5);
     hMult2->GetXaxis()->SetTitle("Multiplicity");
     hMult2->GetYaxis()->SetTitle("Counts");
  }
  else if(npaddles>202){
     //Neuland detector

     hMult1 = new TH1F("Multiplicity1","Paddle multiplicity",100,0.,100.);
     hMult1->GetXaxis()->SetTitle("Multiplicity");
     hMult1->GetYaxis()->SetTitle("Counts");

     hMult2 = new TH1F("Multiplicity2","Paddle multiplicity",100,0.,100.);
     hMult2->GetXaxis()->SetTitle("Multiplicity");
     hMult2->GetYaxis()->SetTitle("Counts");
  }  
  return kSUCCESS;

}


// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void R3BLandDigitizer::Exec(Option_t* opt)
{
//-Reset entries in output arrays
//-Reset local arrays 
   Reset();

//-Now do the job event/event
//   cout<<"**** In land digitizer ****"<< endl;

// The user can adjust here some basic variables:
// **********************************************   
// time resolution of the scintillator in sigma
   eventNo=eventNo+1;
   Double_t timeRes;
   if(npaddles<202){
      //LAND detector
      timeRes = 0.25;//ns
   }
   else if(npaddles>201){
      //NeuLAND detector
     timeRes = 0.15;//ns  
   }

// light attenuation of plastic scintillator
//   att = 0.005; // in [1/cm]
   att = 0.008; // in [1/cm]
// decay constant of BC408 scintillator
//   Double_t lambda=0.693147/2.5; // in [1/ns] This lambda is actually a t1/2
//   Double_t lambda=1./3.1; // in [1/ns]
   Double_t lambda=1./2.1; // in [1/ns]

// half of the length of a scintillator
   plength = fLandDigiPar->GetPaddleLength(); // in [cm]

// length of time gate for QDC
   Double_t tofRange = 400.;//in [ns]
   
// energy calibration factor   
//   calFactor=0.6; 
   calFactor=0.4;   
// Individual thresholds for PM's 
   Double_t thresh[npaddles];
   for (Int_t i=0;i<npaddles;i++){
      Double_t ran=(gRandom->Rndm(0)-0.5)*0.15;   
      // here one has the possibility to assign individual thresholds
      thresh[i] = 0.16+ran; // [MeV] for geant4
   }
   
//****************************************************   
   Int_t nentries = fLandPoints->GetEntries();
   Int_t PMmult[npaddles];   
   Double_t QDC[npaddles],TDC[npaddles];
   Double_t QDC_temp[npaddles],TDC_temp[npaddles];
   Double_t TotalEnergy;
   Double_t TotalEnergyLee;
   Double_t TotalLight = 0;
   Int_t mult1=0;
   Int_t mult2=0;   
   Int_t TrackId=0;
   Double_t xpos[npaddles],ypos[npaddles],zpos[npaddles];
   Double_t xpos_temp[npaddles],ypos_temp[npaddles],zpos_temp[npaddles];
   Double_t beta,gamma,pnx,pny,pnz,pnzcm,p,betaNeut,betaFrag,gammaNeut,gammaFrag;
   Double_t E_lab;
   Double_t xpaddle[npaddles],ypaddle[npaddles],zpaddle[npaddles];
   Int_t PDG;
   Double_t en,s,rr;

   // reset    
   TotalEnergy=0.;
   TotalEnergyLee=0.;
   
   for (Int_t j=0;j<npaddles;j++){      
      PMmult[j]=0;
      memset(PM_res[j],0,1500*sizeof(PM_RES));
   }

   
   for (Int_t l=0;l<nentries;l++){
      //cout<<"loop over entries "<<l<<endl;
      // Get the Land Object in array
      R3BLandPoint *land_obj = (R3BLandPoint*) fLandPoints->At(l);

      Int_t paddle = int(land_obj->GetSector())-1; //note that paddle starts at 1 
      Int_t scint = int(land_obj->GetPaddleNb());
      Double_t eloss = land_obj->GetEnergyLoss()*1000.*calFactor;  
      Double_t light = land_obj->GetLightYield()*1000.*calFactor;  
      Double_t xIn = land_obj->GetXIn();
      Double_t yIn = land_obj->GetYIn();
      Double_t zIn = land_obj->GetZIn();
      Double_t xOut = land_obj->GetXOut();
      Double_t yOut = land_obj->GetYOut();
      Double_t zOut = land_obj->GetZOut();
      Double_t time = land_obj->GetTime();
      Int_t media = int(land_obj->GetPaddleType());
      TrackId = land_obj->GetTrackID();
      R3BMCTrack *aTrack = (R3BMCTrack*) fLandMCTrack->At(TrackId);      
      Int_t PID = aTrack->GetPdgCode();
                
//      Double_t x = (xIn+xOut)/2.;
//      Double_t y = (yIn+yOut)/2.;
//      Double_t z = (zIn+zOut)/2.;

      Double_t x = xIn;
      Double_t y = yIn;
      Double_t z = zIn;
     
      
      if (eloss > 0. && media == 3) {
        TotalEnergy+=eloss;     
        TotalEnergyLee+=light;
      }
      Int_t mother = aTrack->GetMotherId();

      if(1>2){
         cout<< "entry "<<l<<endl;
         cout<< "TrackId "<<TrackId<<endl;
         cout<< "Paddle "<<paddle<<endl;
         cout<< "Sector "<<scint<<endl;
         cout<< "PID "<<PID<<endl;
         cout<< "time "<<time<<endl;     
         cout<< "energy loss "<<eloss<<endl;
         cout<< "light yield "<<light<<endl;
         cout<< "X In "<<xIn<<endl;
         cout<< "Y In "<<yIn<<endl;
         cout<< "Z In "<<zIn<<endl;
         cout<< "X Out "<<xOut<<endl;
         cout<< "Y Out "<<yOut<<endl;
         cout<< "Z Out "<<zOut<<endl;
         cout<< "media "<<media<<endl;
	 
      }
      
              
      if (eloss > 0. && media == 3) {
	 
         // store Particle ID which caused the energy loss
         if(eloss > 1.) hParticle->Fill(PID,1.);


         PMmult[paddle] = PMmult[paddle] + 1;
	 
         Int_t m = PMmult[paddle]; 
//	 cout << "multi per paddle: "<<m<<endl;
//           cout<< "light yield "<<light<<endl;
	 
	 if (m>1500) {
	   cout<<"Too many hits in one paddle: "<< m<< " hits"<<endl;
           cout<< "entry "<<l<<" after sorting"<<endl;
  	   cout<<"paddle "<<paddle<<endl;
	   cout<<"padlle mult "<<PMmult[paddle]<<endl;
           cout<< "Sector "<<scint<<endl;
	   cout<<"eloss "<<eloss<<endl;
           cout<< "light yield "<<light<<endl;
	   cout<<"PID "<<PID<<endl;
           cout<< "time "<<time<<endl;     
           cout<< "X In "<<xIn<<endl;
           cout<< "Y In "<<yIn<<endl;
           cout<< "Z In "<<zIn<<endl;
           cout<< "X Out "<<xOut<<endl;
           cout<< "Y Out "<<yOut<<endl;
           cout<< "Z Out "<<zOut<<endl;	   	   
	   return;
         }
	 
         gGeoManager->FindNode(x,y,z);
         TGeoNode* curNode = gGeoManager->GetCurrentNode();
         gGeoManager->CdUp();
         TGeoNode* curNode1 = gGeoManager->GetCurrentNode();
   
         //cout << "Node: "  << curNode1->GetName() << "  " << paddle <<  endl;
	 
         Double_t local_point[3];
         Double_t global_point[3];
         local_point[0]=0.;
         local_point[1]=0.;
         local_point[2]=0.;
         gGeoManager->LocalToMaster(local_point, global_point);

//         cout<<"test "<< global_point[0] << "  " << global_point[1] << 
//               "  " << global_point[2] << endl;
      
         xpaddle[paddle] = global_point[0];
         ypaddle[paddle] = global_point[1];
         zpaddle[paddle] = global_point[2];

              
         // identify x and y paddles and calculate light transmission to the PM's
	 // first plane are horizontal paddles
    
//         cout<<"paddle "<<paddle<<"  "<<(int)(((paddle-1)/40.))/2.<<"  "<<(int)((int)(((paddle-1)/40.))/2.)<<endl;

//	 if(paddle > (npaddles-1)/2) {
         if((int)(((paddle-1)/paddle_per_plane))/2.==(int)((int)(((paddle-1)/paddle_per_plane))/2.)){
	    // horizontal paddles
	    PM_res[paddle][m].Ltime = time+(plength-x)/cMedia;
            PM_res[paddle][m].LlightCFD = light*exp(-att*(plength-x));
            PM_res[paddle][m].LlightQDC = light*exp(-att*(plength-x));

	    PM_res[paddle][m].Rtime = time+(plength+x)/cMedia;
            PM_res[paddle][m].RlightCFD = light*exp(-att*(plength+x));
            PM_res[paddle][m].RlightQDC = light*exp(-att*(plength+x));
//             cout<<"horizontal paddle "<< PM_res[paddle][m].LlightCFD<<
//	     "  "<<PM_res[paddle][m].RlightCFD <<endl;
         }
	 else {
	    // vertical paddles
	    PM_res[paddle][m].Ltime = time+(plength-y)/cMedia;
            PM_res[paddle][m].LlightCFD = light*exp(-att*(plength-y));
            PM_res[paddle][m].LlightQDC = light*exp(-att*(plength-y));

	    PM_res[paddle][m].Rtime = time+(plength+y)/cMedia;
            PM_res[paddle][m].RlightCFD = light*exp(-att*(plength+y));
            PM_res[paddle][m].RlightQDC = light*exp(-att*(plength+y));
//             cout<<"vertical paddle"<< PM_res[paddle][m].LlightCFD<<
//	     "  "<<PM_res[paddle][m].RlightCFD<<endl;
         }

      }//! eloss	 
           
   }//! MC hits   

   hElossLight->Fill(TotalEnergy,TotalEnergyLee,1.);
   hTotalEnergy->Fill(TotalEnergy);  
   hTotalEnergy1->Fill(TotalEnergy/calFactor/fBeamEnergy);  
   hTotalLightRel->Fill(TotalEnergyLee);  
   hTotalLight1->Fill(TotalEnergyLee/calFactor/fBeamEnergy);  
   hTotalEnergyRel->Fill(TotalEnergy);  

//    cout << "-I- R3BLandDigitizer : energy=" << TotalEnergy << ",  light=" << TotalEnergyLee << "." << endl;
   
   // Check for leading edge
   Double_t triggerTime=1e10;
   for(Int_t i = 0; i < npaddles; i++) {
      for (Int_t j = 0; j < PMmult[i]; j++) {

	PM_res[i][j+1].Lenergy=lambda*PM_res[i][j+1].LlightCFD;
	PM_res[i][j+1].Lenergy=PM_res[i][j+1].Lenergy+PM_res[i][j].Lenergy* 
	  exp(-lambda*(PM_res[i][j+1].Ltime-PM_res[i][j].Ltime));       

	if(PM_res[i][j+1].Lenergy > thresh[i]) {
	  if(PM_res[i][j+1].Ltime < triggerTime) {
	    triggerTime = PM_res[i][j+1].Ltime;
	  }// find minimum
	}// if above threshold

	PM_res[i][j+1].Renergy=lambda*PM_res[i][j+1].RlightCFD;
	PM_res[i][j+1].Renergy=PM_res[i][j+1].Renergy+PM_res[i][j].Renergy* 
	  exp(-lambda*(PM_res[i][j+1].Rtime-PM_res[i][j].Rtime));       

	if(PM_res[i][j+1].Renergy > thresh[i]) {
	  if(PM_res[i][j+1].Rtime < triggerTime) {
	    triggerTime = PM_res[i][j+1].Rtime;
	  }// find minimum
	}// if above threshold

      }// digis
   }// paddles

//    cout << "-I- R3BLandDigitizer : triggerTime=" << triggerTime << "." << endl;

   Double_t temp[1500][3];
      
   for (Int_t i=0;i<npaddles;i++){
      //cout<<"loop over paddles "<<i<<endl; 

      // sort hits according to time; left PM
      for (Int_t j=0;j<PMmult[i];j++){
         temp[j][0]=PM_res[i][j+1].Ltime;
         temp[j][1]=PM_res[i][j+1].LlightCFD;	 
         temp[j][2]=PM_res[i][j+1].LlightQDC;	 
      }
      
      for (Int_t j=0;j<PMmult[i];j++){
         Double_t min = 100000;
         Int_t index = 0;
         for (Int_t k=0;k<PMmult[i];k++){
            if(temp[k][0] < min){
	       min = temp[k][0];
	       index = k;
	    }
         }
         PM_res[i][j+1].Ltime=temp[index][0];
         PM_res[i][j+1].LlightCFD=temp[index][1];
         PM_res[i][j+1].LlightQDC=temp[index][2];
         temp[index][0] = 100000.;
      }	
   
      // sort hits according to time; right PM
      for (Int_t j=0;j<PMmult[i];j++){
         temp[j][0]=PM_res[i][j+1].Rtime;
         temp[j][1]=PM_res[i][j+1].RlightCFD;	 
         temp[j][2]=PM_res[i][j+1].RlightQDC;	 
      }
      
      for (Int_t j=0;j<PMmult[i];j++){
         Double_t min = 100000;
         Int_t index = 0;
         for (Int_t k=0;k<PMmult[i];k++){
            if(temp[k][0] < min){
	       min = temp[k][0];
	       index = k;
	    }
         }
         PM_res[i][j+1].Rtime=temp[index][0];
         PM_res[i][j+1].RlightCFD=temp[index][1];
         PM_res[i][j+1].RlightQDC=temp[index][2];
         temp[index][0] = 100000.;
      }	
       
      Double_t tofl = 0.;
      Double_t tofr = 0.;
      Double_t lightr = 0.;
      Double_t lightl = 0.;
      Int_t multl = 0;
      Int_t multr = 0;
      PM_res[i][0].Lenergy=0.;
      PM_res[i][0].Renergy=0.;
      PM_res[i][0].Ltime=PM_res[i][1].Ltime;
      PM_res[i][0].Rtime=PM_res[i][1].Rtime;

      
      for (Int_t j=0;j<PMmult[i];j++){
/*
         cout<<"j: "<<j<<"  "<< PMmult[i] <<endl;
         cout<<"energy: "<<PM_res[i][j+1].Lenergy<<endl;
         cout<<"light: "<<PM_res[i][j+1].LlightCFD<<endl;
         cout<<"new energy: "<< lambda*PM_res[i][j+1].LlightCFD<<endl;
         cout<<"times: "<< PM_res[i][j+1].Ltime<<"  "<<PM_res[i][j].Ltime <<endl;
*/	 
//         cout<<"adding energy: "<< PM_res[i][j].Lenergy* 
//	 exp(-lambda*(PM_res[i][j+1].Ltime-PM_res[i][j].Ltime))<<endl;
//         cout<<"result: "<<PM_res[i][j+1].Lenergy<<endl;

//         PM_res[i][j+1].Lenergy=PM_res[i][j+1].LlightCFD;


         if(PM_res[i][j+1].Lenergy>thresh[i] && multl==0) {
//             if (triggerTime==0.) triggerTime=PM_res[i][j+1].Ltime;
	    // This is supposed to mimic a QDC and a TDC
	    // check if light is larger than threshold and register time
            // Take also time resolution of sigma=timeRes ps into account
            Double_t num=gRandom->Gaus(0.,timeRes);
	    tofl=PM_res[i][j+1].Ltime+num;
	    multl=multl+1;
	    lightl=0.;	   
            for(Int_t k=1;k<=PMmult[i];k++){
	    // add all times inside +- tofRange (ns) for QDC 
	       if(TMath::Abs(PM_res[i][k].Ltime-triggerTime+tofRange/2.) < tofRange) {
  	          lightl=lightl+PM_res[i][k].LlightQDC;
	          if(i<68){
                     // control histograms
	             hPMl ->Fill(PM_res[i][k].Ltime,PM_res[i][k].LlightCFD);
                  }
	       }
	    }
	 
	 }
	 	 
//         PM_res[i][j+1].Renergy=lambda*PM_res[i][j+1].RlightCFD;

         if(PM_res[i][j+1].Renergy>thresh[i] && multr==0) {
//             if (triggerTime==0.) triggerTime=PM_res[i][j+1].Rtime;
	    // This is supposed to mimic a QDC and a TDC
	    // check if light is larger than threshold and register time
            // Take also time resolution of sigma=timeRes ps into account
            Double_t num=gRandom->Gaus(0.,timeRes);
	    tofr=PM_res[i][j+1].Rtime+num;
	    multr=multr+1;
	    lightr=0.;	   
            for(Int_t k=1;k<=PMmult[i];k++){
	    // add all times inside +- tofRange (ns) for QDC 
	       if(TMath::Abs(PM_res[i][k].Rtime-triggerTime+tofRange/2.) < tofRange) {
  	          lightr=lightr+PM_res[i][k].RlightQDC;
	          if(i<68){
                     // control histograms
	             hPMr ->Fill(PM_res[i][k].Rtime,PM_res[i][k].RlightCFD);
                  }
	       }
	    }
	 
	 }

      }


      // Multiplicity if only one PM has fired
      if(multl>0 || multr>0) mult2=mult2+1;

      if(multl>0 && multr>0){
           
	 //multiplicity if 2 PM's have fired
	 mult1=mult1+1;
//	 cout << "light l and r " <<  lightl<<"  "<<lightr << endl;	 

         QDC_temp[mult1] = sqrt(lightl*lightr)*exp((2.*plength)*att/2.);
//	 cout << "QDC " <<  QDC_temp[mult1] << endl;	 
         TDC_temp[mult1] = (tofl + tofr) / 2. - plength/cMedia;
	 
//	 if(i > (npaddles-1)/2) {
         if((int)(((i-1)/paddle_per_plane))/2.==(int)((int)(((i-1)/paddle_per_plane))/2.)){
	    //horizontal paddles
	    xpos_temp[mult1] = (tofr - tofl)/2.*cMedia;
	    ypos_temp[mult1] = ypaddle[i];	    
	    zpos_temp[mult1] = zpaddle[i];
//	    cout << "delta tof x " << (tofl - tofr) << endl; 
         }
	 else {
	    // vertical paddles
	    xpos_temp[mult1] = xpaddle[i];
	    ypos_temp[mult1] = (tofr - tofl)/2.*cMedia;	    
	    zpos_temp[mult1] = zpaddle[i];
//	    cout << "delta tof y " << (tofl - tofr) << endl; 
         }
	 
         // Here is an example how to fill the R3BLandDigi structure
         Double_t tdcL = tofl;
         Double_t tdcR = tofr;
         Double_t qdcL = lightl;
         Double_t qdcR = lightr;
         Int_t paddleNr = i+1;
	 Double_t qdc=QDC_temp[mult1];
	 Double_t tdc=TDC_temp[mult1];	 
	 Double_t xx=xpos_temp[mult1];
	 Double_t yy=ypos_temp[mult1];
	 Double_t zz=zpos_temp[mult1];

         // calling AddHit function will create a object
         // R3BLandDigi in memory and will automatically
         // add it to the array
         // sothat :  1 call  to AddHit() 1 object in array
         //           n calls to AddHit() n object in array

         AddHit(paddleNr, tdcL, tdcR, tdc, qdcL, qdcR, qdc, xx, yy, zz);


      }


      else {
         // recover hits with partial information

      }
   } // loop over paddles

//   cout << "multiplicity " << mult1 << endl;
   
   // sort final hits for time again
   TotalLight=0.;
   for (Int_t i=1;i<mult1+1;i++){
      Double_t min = 100000.;
      Int_t index = 0;
      for (Int_t j=1;j<mult1+1;j++){
         if(TDC_temp[j] < min){
	    min = TDC_temp[j];
	    index = j;
	 }
      }
      TDC[i] = TDC_temp[index];
      TDC_temp[index] = 100000.;
      QDC[i] = QDC_temp[index];
      xpos[i] = xpos_temp[index];
      ypos[i] = ypos_temp[index];
      zpos[i] = zpos_temp[index];
           
      //cout<<"hit "<< i << " of " << mult1 << endl;
      //cout<<"QDC "<< QDC[i] << " TDC " << TDC[i] << endl;
      //cout<<"POS "<< xpos[i] << "  " << ypos[i]<< "  " << zpos[i] << endl;
      hPaddleEnergy->Fill(QDC[i],1.);
      
      TotalLight=TotalLight+QDC[i];
   }	  

//    cout << "-I- R3BLandDigitizer : totalLight=" << TotalLight << "." << endl;
   
      // control histograms
   if(mult2>0){
      hMult2 ->Fill(mult2,1.);
   }      
   hMult1 ->Fill(mult1,1.);
   hTotalLight->Fill(TotalLight,1.);      
   hTotalLightRel2->Fill(TotalLight/200.,1.);
   hFirstEnergy->Fill(QDC[1],1.);

   if(fVerbose) {
     cout << "-I- R3BLandDigitizer : produced " << fLandDigi->GetEntries() << " digis." << endl;
   }
}
// -------------------------------------------------------------------------

void R3BLandDigitizer::Reset(){
// Clear the structure
//   cout << " -I- Digit Reset() called " << endl;

   
 if (fLandDigi ) fLandDigi->Clear();
}   

void R3BLandDigitizer::Finish()
{
// here event. write histos
//   cout << " -I- Digit Finish() called " << endl;
// Write control histograms
   hPMl->Write();
   hPMr->Write();
   hMult1->Write();
   hMult2->Write();
   hParticle->Write();
   hFirstEnergy->Write();
   hPaddleEnergy->Write();
   
   hTotalLight->Write();
   hTotalLight1->Write();
   hTotalEnergy->Write();
   hTotalEnergy1->Write();
   hTotalLightRel->Write();
   hTotalLightRel2->Write();
   hTotalEnergyRel->Write();

   hElossLight->Write();

}

R3BLandDigi* R3BLandDigitizer::AddHit(Int_t paddleNr, Double_t tdcL, Double_t tdcR,
   Double_t tdc, Double_t qdcL, Double_t qdcR, Double_t qdc, 
   Double_t xx, Double_t yy, Double_t zz){   
  // It fills the R3BLandDigi array
  R3BLandDigi *digi =
    new((*fLandDigi)[fLandDigi->GetEntriesFast()]) R3BLandDigi(paddleNr,tdcL,tdcR,tdc,qdcL,qdcR,qdc,xx,yy,zz);
  return digi;
}
	      

ClassImp(R3BLandDigitizer)
