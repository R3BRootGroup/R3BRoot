#include "R3BNeutronTracker.h"
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
#include "R3BLandDigi.h"
#include "R3BMCTrack.h"		
#include "R3BNeutronTrack.h"

using std::cout;
using std::endl;

		

R3BNeutronTracker::R3BNeutronTracker() :
  FairTask("R3B Land Digitization scheme ") { 
}


R3BNeutronTracker::~R3BNeutronTracker() {
}


void R3BNeutronTracker::SetParContainers() {

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




InitStatus R3BNeutronTracker::Init() {

  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fLandPoints = (TClonesArray*) ioman->GetObject("LandPoint");
  fLandMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
  fLandDigi = (TClonesArray*) ioman->GetObject("LandDigi");

  // New structure created by the Neutron Tracker
  fNeutronTracks = new TClonesArray("R3BNeutronTrack");
  ioman->Register("LandNeTracks", "Neutron Tracks", fNeutronTracks, kTRUE);

  npaddles = fLandDigiPar->GetMaxPaddle()+1;
  nplanes = fLandDigiPar->GetMaxPlane();
  cout<<"# paddles: "<<npaddles<<"  # planes: "<<nplanes<<endl;
  amu = 931.494028; //atomic mass unit in MeV/c**2
  c = 2.99792458E8;
  cMedia = 20.;// speed of light in material in [cm/ns]

  PM_hit=new PM_HIT[npaddles];
  PRIM_part=new PRIM_PART[11];
  NEUT1_hit=new NEUT1_HIT[100];
  NEUT2_hit=new NEUT2_HIT[100];

  hNeutmult1 = new TH1F("Neutmult1","Neutron multiplicity from beta beam considerations",20,-0.5,19.5);
  hNeutmult1->GetXaxis()->SetTitle("Multiplicity");
  hNeutmult1->GetYaxis()->SetTitle("Counts");
     
  hNeutmult2 = new TH1F("Neutmult2","Neutron multiplicity from speed of light considerations",20,-0.5,19.5);
  hNeutmult2->GetXaxis()->SetTitle("Multiplicity");
  hNeutmult2->GetYaxis()->SetTitle("Counts");

  hMinv = new TH1F("Minv","Minv",10000,0.,10.);
  hMinv->GetXaxis()->SetTitle("Erel (MeV)");
  hMinv->GetYaxis()->SetTitle("Counts");

  hMinv0 = new TH1F("Minv0","Minv0",10000,0.,10.);
  hMinv0->GetXaxis()->SetTitle("Erel (MeV)");
  hMinv0->GetYaxis()->SetTitle("Counts");

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

  return kSUCCESS;

}


// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void R3BNeutronTracker::Exec(Option_t* opt) {
 
//-Reset entries in output arrays
//-Reset local arrays 
   Reset();
//   cout<<"in NeutronTracker"<<endl;
   
   Int_t nentries = fLandDigi->GetEntries();
   Double_t temp[npaddles][14];
   Int_t nPrim=0;
   Double_t momentumT[10],momentumX[10],momentumY[10],momentumZ[10],energy[10];
   Double_t beta[10], gamma[10],rr,s;  
   Double_t sum_momentumX,sum_momentumY,sum_momentumZ,sum_masses,sum_energy;
   Double_t betaNeutron,E_lab,pnzcm;
   // Get parameter from original neutrons and fragment
   // Access to Monte Carlo Info
   //  get object from the TclonesArray at index=TrackID
   R3BMCTrack *aTrack1 = (R3BMCTrack*) fLandMCTrack->At(0);
   Int_t prim = aTrack1->GetMotherId();

   while (prim < 0 ) {
   
      PRIM_part[nPrim].pdg = aTrack1->GetPdgCode();
      PRIM_part[nPrim].px = aTrack1->GetPx()*1000.;  
      PRIM_part[nPrim].py = aTrack1->GetPy()*1000.;  
      PRIM_part[nPrim].pz = aTrack1->GetPz()*1000.;  
      PRIM_part[nPrim].pt = aTrack1->GetPt()*1000.;  
      PRIM_part[nPrim].p = aTrack1->GetP()*1000.;  
      PRIM_part[nPrim].x = aTrack1->GetStartX();  
      PRIM_part[nPrim].y = aTrack1->GetStartY();  
      PRIM_part[nPrim].z = aTrack1->GetStartZ();  
      PRIM_part[nPrim].t = aTrack1->GetStartT();
      PRIM_part[nPrim].M = aTrack1->GetMass();
	 
/*
      cout << "primary particle " << PRIM_part[nPrim].pdg<<"  "<< endl;
      cout << "mass " << PRIM_part[nPrim].M<< endl;
      cout << "px, py, pz " << PRIM_part[nPrim].px << "  " << 
          PRIM_part[nPrim].py << "  " << PRIM_part[nPrim].pz << endl;
      cout << "Ptransversal, P total " << PRIM_part[nPrim].pt <<
      	  "  " << PRIM_part[nPrim].p << endl;
*/

      if(PRIM_part[nPrim].pdg==2112){
         //neutron
	 PRIM_part[nPrim].A=1.0086649;
	 PRIM_part[nPrim].M=PRIM_part[nPrim].A*amu;
      }
      else if(PRIM_part[nPrim].pdg==2212){
         //proton
	 PRIM_part[nPrim].A=1.0072765;
	 PRIM_part[nPrim].M=PRIM_part[nPrim].A*amu;
      }
      else{
	 //fragment
         PRIM_part[nPrim].A=10.;
         PRIM_part[nPrim].M=PRIM_part[nPrim].A*amu;
      }

      nPrim=nPrim+1;
      aTrack1 = (R3BMCTrack*) fLandMCTrack->At(nPrim);
      if (aTrack1!=0) prim = aTrack1->GetMotherId();
      else prim= 1;
   }
   
   for (Int_t l=0;l<nentries;l++){
      //cout<<"loop over entries "<<l<<endl;
      // Get the Land Object in array
      R3BLandDigi *land_obj = (R3BLandDigi*) fLandDigi->At(l);


      temp[l][0] = int(land_obj->GetPaddleNr())-1; //note that paddle starts at 1 
      temp[l][1] = land_obj->GetTdcL();  
      temp[l][2] = land_obj->GetTdcR();  
      temp[l][3] = land_obj->GetTdc();  
      temp[l][4] = land_obj->GetQdcL();  
      temp[l][5] = land_obj->GetQdcR();  
      temp[l][6] = land_obj->GetQdc();  
      temp[l][7] = land_obj->GetXX();  
      temp[l][8] = land_obj->GetYY();  
      temp[l][9] = land_obj->GetZZ();  
      temp[l][10] = land_obj->GetX0();  
      temp[l][11] = land_obj->GetY0();  
      temp[l][12] = land_obj->GetZ0();  
      temp[l][13] = land_obj->GetT0();  

      
   }//loop over entries

   //cout<<"sort hits"<<endl;
   
   // sort hits for time
   for (Int_t i=0;i<nentries;i++){
      Double_t min = 100000.;
      Int_t index = 0;
      for (Int_t j=0;j<nentries;j++){
         if(temp[j][3] < min){
	    min = temp[j][3];
	    index = j;
	 }
      }

      PM_hit[i].paddle = temp[index][0];
      PM_hit[i].tdcL = temp[index][1];  
      PM_hit[i].tdcR = temp[index][2];  
      PM_hit[i].tdc = temp[index][3];  
      PM_hit[i].qdcL = temp[index][4];  
      PM_hit[i].qdcR = temp[index][5];  
      PM_hit[i].qdc = temp[index][6];  
      PM_hit[i].x = temp[index][7];  
      PM_hit[i].y = temp[index][8];  
      PM_hit[i].z = temp[index][9];  
      PM_hit[i].x0 = temp[index][10];  
      PM_hit[i].y0 = temp[index][11];  
      PM_hit[i].z0 = temp[index][12];  
      PM_hit[i].t0 = temp[index][13];  
      temp[index][3] = 100000.;
/*
      cout<<"hit # "<<i<<endl;
      cout<<"paddle # "<<PM_hit[i].paddle<<endl;
      cout<<"tdc "<<PM_hit[i].tdcL<<"  "<<PM_hit[i].tdcR<<"  "<<PM_hit[i].tdc<<endl;
      cout<<"qdc "<<PM_hit[i].qdcL<<"  "<<PM_hit[i].qdcR<<"  "<<PM_hit[i].qdc<<endl;
      cout<<"pos "<<PM_hit[i].x<<"  "<<PM_hit[i].y<<"  "<<PM_hit[i].z<<endl;
*/           
   }	  

   if(nentries>0){

      // Reconstruct neutron momentum with first hit.
      // First with the ideal position from simulations.
      // This gives us a hint about maximum possible resolution.


      sum_energy=0.;
      sum_momentumX=0.;
      sum_momentumY=0.;
      sum_momentumZ=0.;
      sum_masses=0.;

      for (Int_t i=0;i<nPrim;i++){
      
         if(PRIM_part[i].pdg==2112){
	    // neutron: beta is calculated with position and time of first hit
            s = sqrt((PM_hit[0].x0-PRIM_part[i].x)*(PM_hit[0].x0-PRIM_part[i].x)+
                     (PM_hit[0].y0-PRIM_part[i].y)*(PM_hit[0].y0-PRIM_part[i].y)+
                     (PM_hit[0].z0-PRIM_part[i].z)*(PM_hit[0].z0-PRIM_part[i].z));

            rr = sqrt((PM_hit[0].x0-PRIM_part[i].x)*(PM_hit[0].x0-PRIM_part[i].x)+
                      (PM_hit[0].y0-PRIM_part[i].y)*(PM_hit[0].y0-PRIM_part[i].y));

	 
	    beta[i]=s/(PM_hit[0].t0-PRIM_part[i].t)*1.E7/c;
	    gamma[i]=1./sqrt(1.-beta[i]*beta[i]); // gamma 
            momentumT[i]=beta[i]*gamma[i]*PRIM_part[i].M;
            momentumZ[i]=cos(atan(rr/(PM_hit[0].z0-PRIM_part[i].z)))*momentumT[i];
            momentumX[i]=momentumZ[i]*(PM_hit[0].x0-PRIM_part[i].x)/(PM_hit[0].z0-PRIM_part[i].z);
            momentumY[i]=momentumZ[i]*(PM_hit[0].y0-PRIM_part[i].y)/(PM_hit[0].z0-PRIM_part[i].z);
            energy[i] = gamma[i]*PRIM_part[i].A; //total energy neutron

            //for multihit analysis
	    betaNeutron=PRIM_part[i].pz/sqrt(PRIM_part[i].p*PRIM_part[i].p+
                    PRIM_part[i].M*PRIM_part[i].M); // beta
/*
            //test if one can reproduce Erel
            beta[i]=PRIM_part[i].p/sqrt(PRIM_part[i].p*PRIM_part[i].p+
                    PRIM_part[i].M*PRIM_part[i].M); // beta
            gamma[i]=1./sqrt(1.-beta[i]*beta[i]); // gamma         
            momentumT[i]=PRIM_part[i].p;
            momentumZ[i]=PRIM_part[i].pz;
            momentumX[i]=PRIM_part[i].px;
            momentumY[i]=PRIM_part[i].py;
            energy[i] = gamma[i]*PRIM_part[i].A; //total energy
*/

/*
	    cout<<" beta gamma "<<beta[i]<<"  "<<gamma[i]<<endl;
	    cout<<" momentum T "<<momentumT[i]<<endl;
	    cout<<" momentum "<<momentumX[i]<<"  "<<momentumY[i]<<"  "<<momentumZ[i]<<endl;
*/	    

            // Lorentz Transformation in z
            E_lab=sqrt(PRIM_part[i].pz*PRIM_part[i].pz+PRIM_part[i].M*PRIM_part[i].M);
            beta[i]=PRIM_part[i].pz/sqrt(PRIM_part[i].pz*PRIM_part[i].pz+
                    PRIM_part[i].M*PRIM_part[i].M); // beta
	    gamma[i]=1./sqrt(1.-beta[i]*beta[i]); // gamma 
            pnzcm=-1.*beta[i]*gamma[i]*E_lab+gamma[i]*momentumZ[i];

            hDeltaPx1 ->Fill(momentumX[i]-PRIM_part[i].px,1.);
            hDeltaPy1 ->Fill(momentumY[i]-PRIM_part[i].py,1.);
            hDeltaPz1 ->Fill(pnzcm,1.);

      	 }
	 else{
	    // fragment: This information have to come later from the Tracker
            beta[i]=PRIM_part[i].p/sqrt(PRIM_part[i].p*PRIM_part[i].p+
                    PRIM_part[i].M*PRIM_part[i].M); // beta
            gamma[i]=1./sqrt(1.-beta[i]*beta[i]); // gamma         
            momentumT[i]=PRIM_part[i].p;
            momentumZ[i]=PRIM_part[i].pz;
            momentumX[i]=PRIM_part[i].px;
            momentumY[i]=PRIM_part[i].py;
            energy[i] = gamma[i]*PRIM_part[i].A; //total energy
         }
	 
	 sum_energy+=energy[i]*amu;
	 sum_momentumX+=momentumX[i];
	 sum_momentumY+=momentumY[i];
	 sum_momentumZ+=momentumZ[i];
	 sum_masses+=PRIM_part[i].M;
      }
      // calculate invariant mass with ideal quantities for a check
      Double_t xinv0;
      xinv0=(sqrt(sum_energy*sum_energy-
           (sum_momentumX*sum_momentumX + sum_momentumY*sum_momentumY +
	   sum_momentumZ*sum_momentumZ))
           - sum_masses );
      hMinv0->Fill(xinv0);
//      cout<<"xinv0 "<<xinv0<<endl;
              

      // Now reconstruction of momenta with real measured quantities

      sum_energy=0.;
      sum_momentumX=0.;
      sum_momentumY=0.;
      sum_momentumZ=0.;
      sum_masses=0.;
      Int_t numneut=0;
      
      for (Int_t i=0;i<nPrim;i++){
         if(PRIM_part[i].pdg==2112){
	    // neutron: beta is calculated with position and time of first hit
	    
	    // This works so far only with one neutron
            s = sqrt((PM_hit[0].x-PRIM_part[i].x)*(PM_hit[0].x-PRIM_part[i].x)+
                     (PM_hit[0].y-PRIM_part[i].y)*(PM_hit[0].y-PRIM_part[i].y)+
                     (PM_hit[0].z-PRIM_part[i].z)*(PM_hit[0].z-PRIM_part[i].z));

            rr = sqrt((PM_hit[0].x-PRIM_part[i].x)*(PM_hit[0].x-PRIM_part[i].x)+
                      (PM_hit[0].y-PRIM_part[i].y)*(PM_hit[0].y-PRIM_part[i].y));
	    
	    
	    beta[i]=s/(PM_hit[0].tdc-PRIM_part[i].t)*1.E7/c;
	    gamma[i]=1./sqrt(1.-beta[i]*beta[i]); // gamma 
            momentumT[i]=beta[i]*gamma[i]*PRIM_part[i].M;
            momentumZ[i]=cos(atan(rr/(PM_hit[0].z-PRIM_part[i].z)))*momentumT[i];
            momentumX[i]=momentumZ[i]*(PM_hit[0].x-PRIM_part[i].x)/(PM_hit[0].z-PRIM_part[i].z);
            momentumY[i]=momentumZ[i]*(PM_hit[0].y-PRIM_part[i].y)/(PM_hit[0].z-PRIM_part[i].z);
            energy[i] = gamma[i]*PRIM_part[i].A; //total energy neutron

/*
	    cout<<" beta gamma "<<beta[i]<<"  "<<gamma[i]<<endl;
	    cout<<" momentum T "<<momentumT[i]<<endl;
	    cout<<" momentum "<<momentumX[i]<<"  "<<momentumY[i]<<"  "<<momentumZ[i]<<endl;
*/
            // Lorentz Transformation in z
            E_lab=sqrt(PRIM_part[i].pz*PRIM_part[i].pz+PRIM_part[i].M*PRIM_part[i].M);
            beta[i]=PRIM_part[i].pz/sqrt(PRIM_part[i].pz*PRIM_part[i].pz+
                    PRIM_part[i].M*PRIM_part[i].M); // beta
	    gamma[i]=1./sqrt(1.-beta[i]*beta[i]); // gamma 
            pnzcm=-1.*beta[i]*gamma[i]*E_lab+gamma[i]*momentumZ[i];

            hDeltaPx2 ->Fill(momentumX[i]-PRIM_part[i].px,1.);
            hDeltaPy2 ->Fill(momentumY[i]-PRIM_part[i].py,1.);
            hDeltaPz2 ->Fill(pnzcm,1.);
     
      	 }
	 else{
	    // fragment: This information have to come later from the Tracker
            beta[i]=PRIM_part[i].p/sqrt(PRIM_part[i].p*PRIM_part[i].p+
                    PRIM_part[i].M*PRIM_part[i].M); // beta
            gamma[i]=1./sqrt(1.-beta[i]*beta[i]); // gamma         
            momentumT[i]=PRIM_part[i].p;
            momentumZ[i]=PRIM_part[i].pz;
            momentumX[i]=PRIM_part[i].px;
            momentumY[i]=PRIM_part[i].py;
            energy[i] = gamma[i]*PRIM_part[i].A; //total energy
         }
	 sum_energy+=energy[i]*amu;
	 sum_momentumX+=momentumX[i];
	 sum_momentumY+=momentumY[i];
	 sum_momentumZ+=momentumZ[i];
	 sum_masses+=PRIM_part[i].M;
/*
	 cout<<"Sum energy " <<sum_energy<<endl;
	 cout<<"sum momentumX " <<sum_momentumX<<endl;
	 cout<<"sum momentumY " <<sum_momentumY<<endl;
	 cout<<"sum momentumZ " <<sum_momentumZ<<endl;
	 cout<<"sum masses " <<sum_masses<<endl;
*/
      }
      // calculate invariant mass with ideal quantities for a check
      Double_t xinv;
      xinv=(sqrt(sum_energy*sum_energy-
           (sum_momentumX*sum_momentumX + sum_momentumY*sum_momentumY +
	   sum_momentumZ*sum_momentumZ))
           - sum_masses );
      hMinv->Fill(xinv);
//      cout<<"xinv "<<xinv<<endl;


   }  


   // Analyse multiple neutron hits     
   // This is not working yet and needs more work
   Int_t neutMult1,neutMult2,flag_new;
   Double_t deltat,deltar,deltaz,dt,dx,dy,dz,speed;
   if (nentries==0) {
      neutMult1=0;
      neutMult2=0;
   }   	
   if (nentries>0) {
      neutMult1=1;
      NEUT1_hit[0].x=PM_hit[0].x;
      NEUT1_hit[0].y=PM_hit[0].y;
      NEUT1_hit[0].z=PM_hit[0].z;
      NEUT1_hit[0].t=PM_hit[0].tdc;
      neutMult2=1;
      NEUT2_hit[0].x=PM_hit[0].x;
      NEUT2_hit[0].y=PM_hit[0].y;
      NEUT2_hit[0].z=PM_hit[0].z;
      NEUT2_hit[0].t=PM_hit[0].tdc;
   }	
   for (Int_t i=1;i<nentries;i++){
      flag_new=1;
      for (Int_t j=0;j<neutMult2;j++){
         deltat=PM_hit[i].tdc-NEUT2_hit[j].t;
         deltar=sqrt((PM_hit[i].x-NEUT2_hit[j].x)*(PM_hit[i].x-NEUT2_hit[j].x)+
    	             (PM_hit[i].y-NEUT2_hit[j].y)*(PM_hit[i].y-NEUT2_hit[j].y)+
     		     (PM_hit[i].z-NEUT2_hit[j].z)*(PM_hit[i].z-NEUT2_hit[j].z));
	 
         speed = TMath::Abs(deltar/deltat/c/100./1.E-9);
//	 cout<<"speed "<<speed<<" distance "<< deltar<< " time "<<deltat<<endl;
         if(speed<1.0 || deltar<10. || deltat<0.2) {
            flag_new=0;
	 }
      }
      if (flag_new==1) {
         neutMult2=neutMult2+1;
         NEUT2_hit[neutMult2].x=PM_hit[i].x;
         NEUT2_hit[neutMult2].y=PM_hit[i].y;
         NEUT2_hit[neutMult2].z=PM_hit[i].z;  
         NEUT2_hit[neutMult2].t=PM_hit[i].tdc;  	    
//         cout<<"**** another neutron needed ****"<<endl;
//         cout<<"pos "<<PM_hit[i].x<<"  "<<PM_hit[i].y<<"  "<<PM_hit[i].z<<endl;
      }

      // calculate ToF for the distance with nominal beam energy
      deltaz=PM_hit[i].z-PM_hit[0].z;
      deltat=PM_hit[i].tdc-PM_hit[0].tdc;
      dt=deltaz/betaNeutron/c/100.;

      Double_t beta1=deltaz/deltat/1.E-9/100./c;
      
//      cout<<"beta "<<betaNeutron<<" beta1 "<<beta1<<endl;

      if(TMath::Abs(betaNeutron-beta1)<0.01) {
         flag_new=1;
         for (Int_t j=0;j<neutMult1;j++){
	    dx=(NEUT1_hit[j].x-PM_hit[i].x);
	    dy=(NEUT1_hit[j].y-PM_hit[i].y);
	    dz=(NEUT1_hit[j].z-PM_hit[i].z);
	    if(sqrt(dx*dx+dy*dy+dz*dz)<20.) {
//	       cout<<"Too close to the hit "<< j<<endl;
	       flag_new=0;
	    }
	 }
         if (flag_new==1) {
	    neutMult1=neutMult1+1;
	    NEUT1_hit[neutMult1].x=PM_hit[i].x;
	    NEUT1_hit[neutMult1].y=PM_hit[i].y;
	    NEUT1_hit[neutMult1].z=PM_hit[i].z;  
	    NEUT1_hit[neutMult1].t=PM_hit[i].tdc;
//            cout<<"new neutron "<<endl;
//            cout<<"beta "<<deltaz/deltat/1.E-9/c/100.<<endl;
//            cout<<"pos "<<PM_hit[i].x<<"  "<<PM_hit[i].y<<"  "<<PM_hit[i].z<<endl;
	 }
      }
   }
   hNeutmult1->Fill(neutMult1);
   hNeutmult2->Fill(neutMult2);



}
// -------------------------------------------------------------------------

void R3BNeutronTracker::Reset(){
// Clear the structure
//   cout << " -I- Digit Reset() called " << endl;
// if (fDigis ) fDigis->Clear();
}   

void R3BNeutronTracker::Finish()
{
// here event. write histos
//   cout << " -I- Digit Finish() called " << endl;
// Write control histograms
   hNeutmult1->Write();
   hNeutmult2->Write();

   hMinv->Write();
   hMinv0->Write();

   hDeltaPx1->Write();
   hDeltaPy1->Write();
   hDeltaPz1->Write();
   hDeltaPx2->Write();
   hDeltaPy2->Write();
   hDeltaPz2->Write();

}

R3BNeutronTrack* R3BNeutronTracker::AddHit(Int_t paddleNr, Double_t tdcR, Double_t tdcL,
				     Double_t qdcR,Double_t qdcL){
  // It fills the R3BLandDigi array
  TClonesArray& clref = *fNeutronTracks;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BNeutronTrack(paddleNr, tdcR, tdcL, qdcR, qdcL);
}

ClassImp(R3BNeutronTracker)
