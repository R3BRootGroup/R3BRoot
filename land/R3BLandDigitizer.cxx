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
  FairTask("R3B Land Digitization scheme ") { 
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

  if ( fLandDigiPar ) {
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
  //fDigis = new TClonesArray("R3BLandDigi",1000);
  //ioman->Register("LandDigi", "Digital response in Land", fDigis, kTRUE);

  // Parameter retrieval
  // Only after Init one retrieve the Digitization Parameters!
  npaddles = fLandDigiPar->GetMaxPaddle()+1;
  nplanes = fLandDigiPar->GetMaxPlane();
  cout<<"# paddles: "<<npaddles<<"  # planes: "<<nplanes<<endl;
  plength = 100.; // half length of paddle
  att = 0.008; // light attenuation factor [1/cm]
  mn = 939.565; // mass of neutron in MeV/c**2
  c = 2.99792458E8;

  // Initialise control histograms
     hPMl = new TH1F("PM_left","Arrival times of left PM",1000,0.,1000.);
     hPMl->GetXaxis()->SetTitle("Time (ns)");
     hPMl->GetYaxis()->SetTitle("Counts");

     hPMr = new TH1F("PM_right","Arrival times of right PM",1000,0.,1000.);
     hPMr->GetXaxis()->SetTitle("Time (ns)");
     hPMr->GetYaxis()->SetTitle("Counts");

     hTotalEnergy = new TH1F("Total_Energy","Total energy deposit",20000,0.,2000.);
     hTotalEnergy->GetXaxis()->SetTitle("Energy (MeV)");
     hTotalEnergy->GetYaxis()->SetTitle("Counts");

     hTotalLight = new TH1F("Total_Light","Total light detected (energy equivalent)",500,0.,100.);
     hTotalLight->GetXaxis()->SetTitle("Energy (MeV)");
     hTotalLight->GetYaxis()->SetTitle("Counts");
  
     hParticle = new TH1F("Particle","PID of particle which produced the energy loss",3000,0.,3000.);
     hParticle->GetXaxis()->SetTitle("Particle ID");
     hParticle->GetYaxis()->SetTitle("Counts");
  
     hPaddleEnergy = new TH1F("PaddleEnergy","Energy deposit in one paddle",1000,0.,500.);
     hPaddleEnergy->GetXaxis()->SetTitle("Energy (MeV)");
     hPaddleEnergy->GetYaxis()->SetTitle("Counts");

     hFirstEnergy = new TH1F("FirstEnergy","First energy deposit in a paddle",1000,0.,100.);
     hFirstEnergy->GetXaxis()->SetTitle("Energy (MeV)");
     hFirstEnergy->GetYaxis()->SetTitle("Counts");

     hDeltaPx1 = new TH1F("DeltaPx1","difference in reconstucted momenta px (ideal case)",1000,-50.,50.);
     hDeltaPx1->GetXaxis()->SetTitle("Delta Px (MeV/c)");
     hDeltaPx1->GetYaxis()->SetTitle("Counts");

     hDeltaPy1 = new TH1F("DeltaPy1","difference in reconstucted momenta py (ideal case)",1000,-50.,50.);
     hDeltaPy1->GetXaxis()->SetTitle("Delta Py (MeV/c)");
     hDeltaPy1->GetYaxis()->SetTitle("Counts");

     hDeltaPz1 = new TH1F("DeltaPz1","difference in reconstucted momenta pz (ideal case)",1000,-150.,50.);
     hDeltaPz1->GetXaxis()->SetTitle("Delta Pz (MeV/c)");
     hDeltaPz1->GetYaxis()->SetTitle("Counts");

     hDeltaPx2 = new TH1F("DeltaPx2","difference in reconstucted momenta px (exp case)",1000,-50.,50.);
     hDeltaPx2->GetXaxis()->SetTitle("Delta Px (MeV/c)");
     hDeltaPx2->GetYaxis()->SetTitle("Counts");

     hDeltaPy2 = new TH1F("DeltaPy2","difference in reconstucted momenta py (exp case)",1000,-50.,50.);
     hDeltaPy2->GetXaxis()->SetTitle("Delta Py (MeV/c)");
     hDeltaPy2->GetYaxis()->SetTitle("Counts");
 
     hDeltaPz2 = new TH1F("DeltaPz2","difference in reconstucted momenta pz (exp case)",1000,-150.,50.);
     hDeltaPz2->GetXaxis()->SetTitle("Delta Pz (MeV/c)");
     hDeltaPz2->GetYaxis()->SetTitle("Counts");

     hElossLight = new TH2F("ElossLight","Light quenching for protons",100,0.,100.,100,0.,100.);
     hElossLight->GetXaxis()->SetTitle("Energy (MeV)");
     hElossLight->GetYaxis()->SetTitle("Light (MeV)");

     hNeutmult1 = new TH1F("Neutmult1","Neutron multiplicity from beta beam considerations",20,-0.5,19.5);
     hNeutmult1->GetXaxis()->SetTitle("Multiplicity");
     hNeutmult1->GetYaxis()->SetTitle("Counts");
     
     hNeutmult2 = new TH1F("Neutmult2","Neutron multiplicity from speed of light considerations",20,-0.5,19.5);
     hNeutmult2->GetXaxis()->SetTitle("Multiplicity");
     hNeutmult2->GetYaxis()->SetTitle("Counts");

     hDeltaP1 = new TH1F("DeltaP1","difference in reconstucted momenta using beta beam",1000,-150.,50.);
     hDeltaP1->GetXaxis()->SetTitle("Delta P (MeV/c)");
     hDeltaP1->GetYaxis()->SetTitle("Counts");

     hDeltaP2 = new TH1F("DeltaP2","difference in reconstucted momenta using measured beta",1000,-150.,50.);
     hDeltaP2->GetXaxis()->SetTitle("Delta P (MeV/c)");
     hDeltaP2->GetYaxis()->SetTitle("Counts");

     hDeltaX = new TH1F("DeltaX","error in x determination",300,-150.,150.);
     hDeltaX->GetXaxis()->SetTitle("x position (cm)");
     hDeltaX->GetYaxis()->SetTitle("Counts");
     
     hDeltaY = new TH1F("DeltaY","error in y determination",300,-150.,150.);
     hDeltaY->GetXaxis()->SetTitle("y position (cm)");
     hDeltaY->GetYaxis()->SetTitle("Counts");
     
     hDeltaZ = new TH1F("DeltaZ","error in z determination",300,-150.,150.);
     hDeltaZ->GetXaxis()->SetTitle("z position (cm)");
     hDeltaZ->GetYaxis()->SetTitle("Counts");
     
     hDeltaT = new TH1F("DeltaT","error in time determination",2000,-10.,10.);
     hDeltaT->GetXaxis()->SetTitle("time (ns)");
     hDeltaT->GetYaxis()->SetTitle("Counts");

     hFirstMedia = new TH1F("FirstMedia","First media hit",10,0.,10.);
     hFirstMedia->GetXaxis()->SetTitle("Media");
     hFirstMedia->GetYaxis()->SetTitle("Counts");
     
  if(npaddles<202){
     //LAND detector

     hMult = new TH1F("Multiplicity","Paddle multiplicity",20,-0.5,19.5);
     hMult->GetXaxis()->SetTitle("Multiplicity");
     hMult->GetYaxis()->SetTitle("Counts");

  }
  else if(npaddles>202){
     //Neuland detector

     hMult = new TH1F("Multiplicity","Paddle multiplicity",100,0.,100.);
     hMult->GetXaxis()->SetTitle("Multiplicity");
     hMult->GetYaxis()->SetTitle("Counts");

  }  
  return kSUCCESS;

}


// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void R3BLandDigitizer::Exec(Option_t* opt) {
 
//-Reset entries in output arrays
//-Reset local arrays 
   Reset();

//-Now do the job event/event
   //cout<<"**** In land digitizer ****"<< endl;

   Double_t timeRes;
   if(npaddles<202){
      //LAND detector
      timeRes = 0.2;//ns
   }
   else if(npaddles>201){
      //NeuLAND detector
      timeRes = 0.1;//ns  
   }

   Int_t nentries = fLandPoints->GetEntries();
   
   Double_t hit[nentries][14];
   Double_t temp[nentries][14];
   Double_t PMLtime[npaddles][100],PMRtime[npaddles][100];
   Double_t PMLlight[npaddles][100],PMRlight[npaddles][100];
   Double_t PMLenergy[npaddles][100],PMRenergy[npaddles][100];
   Int_t PMmult[npaddles];   
   Double_t QDC[npaddles],TDC[npaddles],QDC2[npaddles];
   Double_t QDC_temp[npaddles],TDC_temp[npaddles],QDC2_temp[npaddles];
   Double_t TotalEnergy = 0;
   Double_t TotalLight = 0;
   Int_t mult=0;
   Int_t TrackId=0;
   Double_t xpos[npaddles],ypos[npaddles],zpos[npaddles];
   Double_t xpos_temp[npaddles],ypos_temp[npaddles],zpos_temp[npaddles];
   Double_t beta,gamma,px,py,pz,p;
   Double_t firstHitX,firstHitY,firstHitZ,firstT,firstMedia;
   Double_t E_lab;
   Double_t xpaddle[npaddles],ypaddle[npaddles],zpaddle[npaddles];
   Int_t PDG;
   Double_t px0,py0,pz0,pt0,p0,x0,y0,z0,t0,s,rr;

   // reset    
   for (Int_t j=0;j<npaddles;j++){      
      PMmult[j]=0;
      for (Int_t i=0;i<100;i++){            
	 PMLtime[j][i]=0.;
         PMLlight[j][i]=0.;
         PMLenergy[j][i]=0.;
	 PMRtime[j][i]=0.;
         PMRlight[j][i]=0.;
         PMRenergy[j][i]=0.;
      }
   }

   // Get parameter from original neutron   
   // Access to Monte Carlo Info
   //  get object from the TclonesArray at index=TrackID
   R3BMCTrack *aTrack = (R3BMCTrack*) fLandMCTrack->At(0);
   
   px0=0;
   PDG = aTrack->GetPdgCode();
   px0 = aTrack->GetPx()*1000.;  
   py0 = aTrack->GetPy()*1000.;  
   pz0 = aTrack->GetPz()*1000.;  
   pt0 = aTrack->GetPt();  
   p0 = aTrack->GetP();  
   x0 = aTrack->GetStartX();  
   y0 = aTrack->GetStartY();  
   z0 = aTrack->GetStartZ();  
   t0 = aTrack->GetStartT();
      
   //cout << "primary particle " << PDG << endl;
   //cout << "px, py, pz " << px0 << "  " << py0 << "  " << pz0 << endl;
   //cout << "Ptransversal, P total " << pt0 << "  " << p0 << endl;
   

   for (Int_t l=0;l<nentries;l++){
      // Get the Land Object in array
      R3BLandPoint *land_obj = (R3BLandPoint*) fLandPoints->At(l);
                    
      temp[l][0] = land_obj->GetSector();	  
      temp[l][1] = land_obj->GetPaddleNb();	  
      temp[l][2] = land_obj->GetEnergyLoss()*1000.; // [MeV]	  
      temp[l][3] = land_obj->GetLightYield()*1000.; // [MeV]	  
      temp[l][4] = land_obj->GetXIn();	  
      temp[l][5] = land_obj->GetYIn();	  
      temp[l][6] = land_obj->GetZIn();	  
      temp[l][7] = land_obj->GetXOut();	  
      temp[l][8] = land_obj->GetYOut();	  
      temp[l][9] = land_obj->GetZOut();	  
      temp[l][10] = land_obj->GetTime();
      TrackId = land_obj->GetTrackID();
      aTrack = (R3BMCTrack*) fLandMCTrack->At(TrackId);
      temp[l][11] = aTrack->GetPdgCode();
      temp[l][12] = land_obj->GetPaddleType();
      temp[l][13] = TrackId;
   

      if(temp[l][11]==2212){
         hElossLight->Fill(temp[l][2],temp[l][3],1.);
         //cout << "Eloss " << temp[l][2] << "  Light " << temp[l][3] << endl;
      }
      
         
   }
    
   
   // sort hits according to time
   for (Int_t i=0;i<nentries;i++){
      Double_t min = 100000;
      Int_t index = 0;
      for (Int_t j=0;j<nentries;j++){
         if(temp[j][10]<min){
	    min = temp[j][10];
	    index = j;
	 }
      }
      hit[i][0] = temp[index][0];
      hit[i][1] = temp[index][1];
      hit[i][2] = temp[index][2];
      hit[i][3] = temp[index][3];
      hit[i][4] = temp[index][4];
      hit[i][5] = temp[index][5];
      hit[i][6] = temp[index][6];
      hit[i][7] = temp[index][7];
      hit[i][8] = temp[index][8];
      hit[i][9] = temp[index][9];
      hit[i][10] = temp[index][10];
      hit[i][11] = temp[index][11];
      hit[i][12] = temp[index][12];
      hit[i][13] = temp[index][13];
      temp[index][10] = 100000.;

   }	   
   
   for (Int_t l=0;l<nentries;l++){
      //cout<<"loop over entries "<<l<<endl;
      Int_t paddle = int(hit[l][0]); //note that paddle starts at 1 
      Int_t scint = int(hit[l][1]);
      Double_t eloss = hit[l][2];  
      Double_t light = hit[l][3];  
      Double_t xIn = hit[l][4];
      Double_t yIn = hit[l][5];
      Double_t zIn = hit[l][6];
      Double_t xOut = hit[l][7];
      Double_t yOut = hit[l][8];
      Double_t zOut = hit[l][9];
      Double_t time = hit[l][10];
      Int_t media = int(hit[l][12]);
           
      Double_t x = (xIn+xOut)/2.;
      Double_t y = (yIn+yOut)/2.;
      Double_t z = (zIn+zOut)/2.;

      //cout<<"z in "<< zIn << endl;
      //cout<<"z out "<< zOut << endl;
     
      Int_t PID = int(hit[l][11]);
      
      if(l==0){
         firstHitX = x;
         firstHitY = y;
         firstHitZ = z;
         firstT = time;
         firstMedia=media;
	 
/*
         aTrack = (R3BMCTrack*) fLandMCTrack->At(int(hit[l][13]));
   
         cout <<"paddle type "<<temp[l][12]<<endl;
         cout<<"PID "<< aTrack->GetPdgCode() << endl;
         cout<<"PID "<< PID << endl;

         cout<<"PX "<< aTrack->GetPx()*1000. << endl;
         cout<<"PY "<< aTrack->GetPy()*1000. << endl;      
         cout<<"PZ "<< aTrack->GetPz()*1000. << endl;

         cout<<"1st entry # "<< l << " of " << nentries << endl;
         cout<<"1st paddle # "<< paddle << "  " << hit[l][0] << endl;
         cout<<"1st scint # "<< scint << "  " << hit[l][1] << endl;
         cout<<"1st Eloss "<< eloss << endl;
         cout<<"1st Light "<< light << endl;
         cout<<"1st X "<< x << endl;
         cout<<"1st Y "<< y << endl;      
         cout<<"1st Z "<< z << endl;
         cout<<"1st Media "<< media << endl;      
         cout<<"1st time "<< time << endl;
*/
      }
      
              
      if (eloss > 0. && media == 3) {
	 
         // store Particle ID which caused the energy loss
         if(eloss > 1.) hParticle->Fill(PID,1.);


         PMmult[paddle] = PMmult[paddle] + 1;
         Int_t m = PMmult[paddle]; 

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

	 if(paddle > (npaddles-1)/2) {
	    // vertical paddles
	    PMLtime[paddle][m] = time+(plength-y)/20.;
            PMLlight[paddle][m] = PMLlight[paddle][m]+light*exp(-att*(plength-y));
            PMLenergy[paddle][m] = PMLenergy[paddle][m]+eloss*exp(-att*(plength-y));
	    PMRtime[paddle][m] = time+(plength+y)/20.;
            PMRlight[paddle][m] = PMRlight[paddle][m]+light*exp(-att*(plength+y));
            PMRenergy[paddle][m] = PMRenergy[paddle][m]+eloss*exp(-att*(plength+y));
         }
	 else {
	    // horizontal paddles
	    PMLtime[paddle][m] = time+(plength-x)/20.;
            PMLlight[paddle][m] = PMLlight[paddle][m]+light*exp(-att*(plength-x));
            PMLenergy[paddle][m] = PMLenergy[paddle][m]+eloss*exp(-att*(plength-x));
	    PMRtime[paddle][m] = time+(plength+x)/20.;
            PMRlight[paddle][m] = PMRlight[paddle][m]+light*exp(-att*(plength+x));
            PMRenergy[paddle][m] = PMRenergy[paddle][m]+eloss*exp(-att*(plength+x));
         }

      }//! eloss	 
           
   }//! MC hits   
   // Check for leading edge 
   Double_t thresh = 0.8;//0.75; // [MeV]

   for (Int_t i=0;i<npaddles;i++){
      //cout<<"loop over paddles "<<i<<endl;
      Double_t tofl = 0.;
      Double_t tofr = 0.;
      Double_t enerr = 0.;
      Double_t enerl = 0.;
      Double_t lightr = 0.;
      Double_t lightl = 0.;
      Int_t multl = 0;
      Int_t multr = 0;
      Int_t j = 0;
      while (tofl<0.1 && j<PMmult[i]+1){         
         if(PMLlight[i][j] > thresh){	 
	    // This is supposed to mimic a QDC and a TDC
	    // check if light is larger than threshold and register time
            // Take also time resolution of sigma=200 ps into account
            Double_t num=gRandom->Gaus(0.,timeRes);
	    tofl=PMLtime[i][j]+num;
	    multl=multl+1;
	    lightl=0.;	   
	    enerl=0.;	   
            for(Int_t k=1;k<=PMmult[i];k++){
	    // add all times inside +- 100 ns for QDC 
	       if(TMath::Abs(PMLtime[i][k]-tofl) < 100.) {
  	          lightl=lightl+PMLlight[i][k];
  	          enerl=enerl+PMLenergy[i][k];
	          if(i<41){
                     // control histograms
	             hPMl ->Fill(PMLtime[i][k],PMLlight[i][k]);
                  }
	       }
	    }
	 }
	 j = j + 1;	
      }
      j = 0;
      while (tofr<0.1 && j<PMmult[i]+1){   
         if(PMRlight[i][j] > thresh){
	    // This is supposed to mimic a QDC and a TDC
	    // check if light is larger than threshold and register time
            // Take also time resolution of sigma=200 ps into account
            Double_t num=gRandom->Gaus(0.,timeRes);
	    tofr=PMRtime[i][j]+num;
	    multr=multr+1;
	    lightr=0.;	   
	    enerr=0.;	   
            for(Int_t k=1;k<=PMmult[i];k++){
	    // add all times inside +- 100 ns for QDC 
	       if(TMath::Abs(PMRtime[i][k]-tofr) < 100.) {
  	          lightr=lightr+PMRlight[i][k];
  	          enerr=enerr+PMRenergy[i][k];
	          if(i<41){
                     // control histograms
	             hPMr ->Fill(PMRtime[i][k],PMRlight[i][k]);
                  }
	       }
	    }
	 }
	 j = j + 1;	
      }

      if(multl==1 && multr==1){
	 mult=mult+1;
         QDC_temp[mult] = sqrt(lightl*lightr);
         QDC2_temp[mult] = sqrt(enerl*enerr);
         TDC_temp[mult] = (tofl + tofr) / 2. - 5.;
	 
	 if(i > (npaddles-1)/2) {
	    // vertical paddles
	    xpos_temp[mult] = xpaddle[i];
	    ypos_temp[mult] = (tofr - tofl)*10.;	    
	    zpos_temp[mult] = zpaddle[i];
//	    cout << "delta tof y " << (tofl - tofr)*20. << endl; 
         }
	 else {
	    //horizontal paddles
	    xpos_temp[mult] = (tofr - tofl)*10.;
	    ypos_temp[mult] = ypaddle[i];	    
	    zpos_temp[mult] = zpaddle[i];
//	    cout << "delta tof x " << (tofl - tofr)*20. << endl; 
         }
      }
      else {
         // recover hits with partial information

      }
   } // loop over paddles

//   cout << "multiplicity " << mult << endl;
   
   // sort final hits for time again
   for (Int_t i=1;i<mult+1;i++){
      Double_t min = 100000.;
      Int_t index = 0;
      for (Int_t j=1;j<mult+1;j++){
         if(TDC_temp[j] < min){
	    min = TDC_temp[j];
	    index = j;
	 }
      }
      TDC[i] = TDC_temp[index];
      TDC_temp[index] = 100000.;
      QDC[i] = QDC_temp[index];
      QDC2[i] = QDC2_temp[index];
      xpos[i] = xpos_temp[index];
      ypos[i] = ypos_temp[index];
      zpos[i] = zpos_temp[index];
           
      //cout<<"hit "<< i << " of " << mult << endl;
      //cout<<"QDC "<< QDC[i] << " TDC " << TDC[i] << endl;
      //cout<<"POS "<< xpos[i] << "  " << ypos[i]<< "  " << zpos[i] << endl;
      hPaddleEnergy->Fill(QDC[i],1.);
      
      TotalLight=TotalLight+QDC[i];
      TotalEnergy=TotalEnergy+QDC2[i];
      
   }	  
   if(mult>0){

      // Record the difference in detection of visible light and real position or time
      hDeltaX->Fill(firstHitX-xpos[1],1.);
      hDeltaY->Fill(firstHitY-ypos[1],1.);
      hDeltaZ->Fill(firstHitZ-zpos[1],1.);
      hDeltaT->Fill(firstT-TDC[1],1.);
      hFirstMedia->Fill(firstMedia,1.);   
      
      
//      cout<<"POS ist  "<< xpos[1] << "  " << ypos[1]<< "  " << zpos[1] <<"  "<< TDC[1]<< endl;
//      cout<<"POS soll "<< firstHitX << "  " << firstHitY << "  " << firstHitZ <<"  "<< firstT<< endl;
      
//      cout<<"DeltaT "<< TDC[1]-firstT << endl;

      // Reconstruct neutron momentum with first hit.
      // First with the ideal position from simulations.
      // This gives us a hint about maximum possible resolution.

      s = sqrt((firstHitX-x0)*(firstHitX-x0)+
                        (firstHitY-y0)*(firstHitY-y0)+
                        (firstHitZ-z0)*(firstHitZ-z0));
      
      beta=s/(firstT-t0)*1.E7/c;
      //cout<<"beta "<< beta << endl;
      gamma=1./sqrt(1.-beta*beta);
      p=beta*gamma*mn;    
      rr=sqrt((firstHitX-x0)*(firstHitX-x0)+
              (firstHitY-y0)*(firstHitY-y0));
      pz=cos(atan(rr/(firstHitZ-z0)))*p;
      px=pz*(firstHitX-x0)/(firstHitZ-z0);
      py=pz*(firstHitY-y0)/(firstHitZ-z0);

      // Lorentz Transformation
      E_lab=sqrt(pz0*pz0+mn*mn);
      beta=pz0/sqrt(pz0*pz0+mn*mn);
      gamma=1./sqrt(1.-beta*beta);      
      pz=-1.*beta*gamma*E_lab+gamma*pz;

      hDeltaPx1 ->Fill(px-px0,1.);
      hDeltaPy1 ->Fill(py-py0,1.);
      hDeltaPz1 ->Fill(pz,1.);

      //cout<<"momentum exp "<< px << "  " << py << "  " << pz << endl;
      //cout<<"momentum sim "<< px0 << "  " << py0 << "  " << pz0 << endl;
  

      // Now reconstruction of momenta with real measured quantities
   
      s = sqrt((xpos[1]-x0)*(xpos[1]-x0)+
               (ypos[1]-y0)*(ypos[1]-y0)+
               (zpos[1]-z0)*(zpos[1]-z0));
      
      beta=s/(TDC[1]-t0)*1.E7/c;
      //cout<<"beta "<< beta << endl;
      gamma=1./sqrt(1.-beta*beta);
      p=beta*gamma*mn;    
      rr=sqrt((xpos[1]-x0)*(xpos[1]-x0)+
              (ypos[1]-y0)*(ypos[1]-y0));
      pz=cos(atan(rr/(zpos[1]-z0)))*p;
      px=pz*(xpos[1]-x0)/(zpos[1]-z0);
      py=pz*(ypos[1]-y0)/(zpos[1]-z0);

      // Lorentz Transformation
      E_lab=sqrt(pz0*pz0+mn*mn);
      beta=pz0/sqrt(pz0*pz0+mn*mn);
      gamma=1./sqrt(1.-beta*beta);      
      pz=-1.*beta*gamma*E_lab+gamma*pz;

      hDeltaPx2 ->Fill(px-px0,1.);
      hDeltaPy2 ->Fill(py-py0,1.);
      hDeltaPz2 ->Fill(pz,1.);

      hDeltaP1->Fill(px0*px0+py0*py0-(px*px+py*py+pz*pz));
      
      //cout<<"momentum exp "<< px << "  " << py << "  " << pz << endl;
      //cout<<"momentum sim "<< px0 << "  " << py0 << "  " << pz0 << endl;
 
      beta=sqrt(xpos[1]*xpos[1]+ypos[1]*ypos[1]+zpos[1]*zpos[1])/
      TDC[1]*1.E7/c;
      gamma=1./sqrt(1-beta*beta);
      p=beta*gamma*mn;

      //cout<<"Beta "<< beta << " Gamma " << gamma << " P " << p << endl;
   
      // control histograms
      hMult ->Fill(mult,1.);
      hTotalLight->Fill(TotalLight,1.);      
      hTotalEnergy->Fill(TotalEnergy,1.);      
      hFirstEnergy->Fill(QDC[1],1.);

      //cout<<"Total light "<< TotalLight << endl;
      //cout<<"Total energy "<< TotalEnergy << endl;
      
   }

   // Analyse multiple neutron hits     
   Int_t neutmult1,neutmult2,flag_new;
   Double_t neut2x[npaddles],neut2y[npaddles],neut2z[npaddles],neut2t[npaddles];
   Double_t neut1x[npaddles],neut1y[npaddles],neut1z[npaddles],neut1t[npaddles];
   Double_t deltat,deltar,deltaz,dt,dx,dy,dz;
   if (mult==0) {
      neutmult1=0;
      neutmult2=0;
   }   	
   if (mult>0) {
      neutmult1=1;
      neut1x[1]=xpos[1];
      neut1y[1]=ypos[1];
      neut1z[1]=zpos[1];
      neut1t[1]=TDC[1];
      neutmult2=1;
      neut2x[1]=xpos[1];
      neut2y[1]=ypos[1];
      neut2z[1]=zpos[1];
      neut2t[1]=TDC[1];
   }	
   for (Int_t i=2;i<mult+1;i++){
      flag_new=1;
      for (Int_t j=1;j<neutmult2+1;j++){
         deltat=TDC[i]-neut2t[j];
         deltar=sqrt((xpos[i]-neut2x[j])*(xpos[i]-neut2x[j])+
    	             (ypos[i]-neut2y[j])*(ypos[i]-neut2y[j])+
     		     (zpos[i]-neut2z[j])*(zpos[i]-neut2z[j]));

//         cout<<"neutron "<<neut2x[j]<<"  "<<neut2y[j]<<"  "<<neut2z[j]<<endl;		     
//         cout<<"time "<<TDC[i]<<"  "<<neut2t[j]<<endl;		     
	 
         Double_t speed = TMath::Abs(deltar/deltat/c/100./1.E-9);
//	 cout<<"speed "<<speed<<" distance "<< deltar<< " time "<<deltat<<endl;
         if(speed<1.1 || deltar<20. || deltat<0.2) {
            flag_new=0;
	 }
      }
      if (flag_new==1) {
         neutmult2=neutmult2+1;
         neut2x[neutmult2]=xpos[i];
         neut2y[neutmult2]=ypos[i];
         neut2z[neutmult2]=zpos[i];  
         neut2t[neutmult2]=TDC[i];  	    
//         cout<<"**** another neutron needed ****"<<endl;
//         cout<<"pos "<<xpos[i]<<"  "<<ypos[i]<<"  "<<zpos[i]<<endl;
      }

      // calculate ToF for the distance with nominal beam energy
      beta=pz0/sqrt(pz0*pz0+mn*mn);
      deltaz=zpos[i]-zpos[1];
      deltat=TDC[i]-TDC[1];
      dt=deltaz/beta/c/100.;

      Double_t beta1=deltaz/deltat/1.E-9/100./c;
      
//      cout<<"beta "<<beta<<" beta1 "<<beta1<<endl;

      if(TMath::Abs(beta-beta1)<0.2) {
         flag_new=1;
         for (Int_t j=1;j<neutmult1+1;j++){
	    dx=(neut1x[j]-xpos[j]);
	    dy=(neut1y[j]-ypos[j]);
	    dz=(neut1z[j]-zpos[j]);
	    if(sqrt(dx*dx+dy*dy+dz*dz)<20.) {
//	       cout<<"Too close to the hit "<< j<<endl;
	       flag_new=0;
	    }
	 }
         if (flag_new==1) {
	    neutmult1=neutmult1+1;
	    neut1x[neutmult1]=xpos[i];
	    neut1y[neutmult1]=ypos[i];
	    neut1z[neutmult1]=zpos[i];  
	    neut1t[neutmult1]=TDC[i];
//            cout<<"new neutron "<<endl;
//            cout<<"beta "<<deltaz/deltat/c/100.<<endl;
//            cout<<"pos "<<xpos[i]<<"  "<<ypos[i]<<"  "<<zpos[i]<<endl;
	 }
      }
   }
   hNeutmult1->Fill(neutmult1);
   hNeutmult2->Fill(neutmult2);

}
// -------------------------------------------------------------------------

void R3BLandDigitizer::Reset(){
// Clear the structure
//   cout << " -I- Digit Reset() called " << endl;

   
// if (fDigis ) fDigis->Clear();
}   

void R3BLandDigitizer::Finish()
{
// here event. write histos
//   cout << " -I- Digit Finish() called " << endl;
// Write control histograms
   hPMl->Write();
   hPMr->Write();
   hMult->Write();
   hParticle->Write();
   hFirstEnergy->Write();
   hPaddleEnergy->Write();
   
   hDeltaPx1->Write();
   hDeltaPy1->Write();
   hDeltaPz1->Write();
   hDeltaPx2->Write();
   hDeltaPy2->Write();
   hDeltaPz2->Write();
   hTotalLight->Write();
   hTotalEnergy->Write();
   hElossLight->Write();

   hNeutmult1->Write();
   hNeutmult2->Write();

   hDeltaX->Write();
   hDeltaY->Write();
   hDeltaZ->Write();
   hDeltaT->Write();

   hFirstMedia->Write();
   
}

ClassImp(R3BLandDigitizer)
