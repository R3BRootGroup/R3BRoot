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
#include "FairMCTrack.h"		
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

  hTotalLight = new TH1F("Total_Light","Total light detected (energy equivalent)",20000,0.,2000.);
  hTotalLight->GetXaxis()->SetTitle("Energy (MeV)");
  hTotalLight->GetYaxis()->SetTitle("Counts");

  hMult = new TH1F("Multiplicity","Paddle multiplicity",100,0.,100.);
  hMult->GetXaxis()->SetTitle("Multiplicity");
  hMult->GetYaxis()->SetTitle("Counts");

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

  // Parameter retrieval
  // Only after Init one retrieve the Digitization Parameters!
  cout << "-I- Max Paddle: "<<  fLandDigiPar->GetMaxPaddle() << endl;
  cout << "-I- Max Plane: "<<   fLandDigiPar->GetMaxPlane() << endl;


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


   Int_t npaddles = 200+1;  //number of paddles
   Int_t nplanes = 10;  //number of planes
   Double_t plength = 100.; // half length of paddle
   Double_t att = 0.008; // light attenuation factor [1/cm]
   Double_t mn = 939.565; // mass of neutron in MeV/c**2
   Double_t c = 2.99792458E8;
   
   Int_t nentries = fLandPoints->GetEntries();
   
   Double_t hit[nentries][12];
   Double_t temp[nentries][12];
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
   Double_t firstHitX,firstHitY,firstHitZ,firstT;
   Double_t E_lab;
   Double_t xpaddle[npaddles],ypaddle[npaddles],zpaddle[npaddles];
   Int_t PDG;
   Double_t px0,py0,pz0,pt0,p0,x0,y0,z0,t0;

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
   FairMCTrack *aTrack = (FairMCTrack*) fLandMCTrack->At(0);

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
      // Access to Monte Carlo Info
      //  get object from the TclonesArray at index=TrackID
      if(TrackId>0){
         aTrack = (FairMCTrack*) fLandMCTrack->At(TrackId);
         temp[l][11] = aTrack->GetPdgCode();
      }
      else{
         temp[l][11] = aTrack->GetPdgCode();
      }
      //cout << "TrackID " << TrackId << "  PDG code " << temp[l][11] << endl;
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
           
      Double_t x = (xIn+xOut)/2.;
      Double_t y = (yIn+yOut)/2.;
      Double_t z = (zIn+zOut)/2.;
     
      Int_t PID = int(hit[l][11]);
      
      if(l==0){
         firstHitX = x;
         firstHitY = y;
         firstHitZ = z;
         firstT = time;
      }
      
/*      
//      if(hit[l][0]==229 || hit[l][0]==229 || hit[l][0]==229){
      cout<<"entry # "<< l << " of " << nentries << endl;
      cout<<"paddle # "<< paddle << "  " << hit[l][0] << endl;
      cout<<"scint # "<< scint << "  " << hit[l][1] << endl;
      cout<<"Eloss "<< eloss << endl;
      cout<<"Light "<< light << endl;
      cout<<"X "<< x << endl;
      cout<<"Y "<< y << endl;      
      cout<<"Z "<< z << endl;
      cout<<"time "<< time << endl;
//      }
*/  
              
      if (eloss > 0. ) {
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

         //<DB> Check me !
	 /*
	 if(paddle > 100) {
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
	 */

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
            Double_t num=gRandom->Gaus(0.,0.2);
	    tofl=PMLtime[i][j]+num;
	    multl=multl+1;
	    lightl=0.;	   
	    enerl=0.;	   
            for(Int_t k=1;k<=PMmult[i];k++){
	    // add all times inside +- 100 ns for QDC 
	       if(TMath::Abs(PMLtime[i][k]-tofl) < 100.) {
  	          lightl=lightl+PMLlight[i][k];
  	          enerl=enerl+PMLenergy[i][k];
	          if(i==110){
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
            Double_t num=gRandom->Gaus(0.,0.2);
	    tofr=PMRtime[i][j]+num;
	    multr=multr+1;
	    lightr=0.;	   
	    enerr=0.;	   
            for(Int_t k=1;k<=PMmult[i];k++){
	    // add all times inside +- 100 ns for QDC 
	       if(TMath::Abs(PMRtime[i][k]-tofr) < 100.) {
  	          lightr=lightr+PMRlight[i][k];
  	          enerr=enerr+PMRenergy[i][k];
	          if(i==110){
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
	 
	 if(i > 100) {
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
      //cout<<"POS ist  "<< xpos[1] << "  " << ypos[1]<< "  " << zpos[1] << endl;
      //cout<<"POS soll "<< firstHitX << "  " << firstHitY << "  " << firstHitZ << endl;

      // Reconstruct neutron momentum with first hit.
      // First with the ideal position from simulations.
      // This gives us a hint about maximum possible resolution.

//      px=mn*(firstHitX-x0)/(firstT-t0)*1.E7/c; 
//      py=mn*(firstHitY-y0)/(firstT-t0)*1.E7/c; 
      
      px=pz0*(firstHitX-x0)/(firstHitZ-z0);
      py=pz0*(firstHitY-y0)/(firstHitZ-z0);

      beta=(firstHitZ-z0)/(firstT-t0)*1.E7/c;
      gamma=1./sqrt(1.-beta*beta);
      pz=beta*gamma*mn;

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
  
      beta=sqrt(firstHitX*firstHitX+firstHitY*firstHitY+firstHitZ*firstHitZ)/
      firstT*1.E7/c;
      gamma=1./sqrt(1-beta*beta);
      p=beta*gamma*mn;

      //cout<<"Beta "<< beta << " Gamma " << gamma << " P " << p << endl;

      // Now reconstruction of momenta with real measured quantities
   
      px=pz0*(xpos[1]-x0)/(zpos[1]-z0); 
      py=pz0*(ypos[1]-y0)/(zpos[1]-z0); 
 
      beta=(zpos[1]-z0)/(TDC[1]-t0)*1.E7/c;
      gamma=1./sqrt(1.-beta*beta);
      pz=beta*gamma*mn;

      // Lorentz Transformation
      E_lab=sqrt(pz0*pz0+mn*mn);
      beta=pz0/sqrt(pz0*pz0+mn*mn);
      gamma=1./sqrt(1.-beta*beta);      
      pz=-1.*beta*gamma*E_lab+gamma*pz;

      hDeltaPx2 ->Fill(px-px0,1.);
      hDeltaPy2 ->Fill(py-py0,1.);
      hDeltaPz2 ->Fill(pz,1.);

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
      
   }
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
}

ClassImp(R3BLandDigitizer)
