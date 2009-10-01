#include "R3BLandDigitizer.h"
#include "TClonesArray.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"


#include "TVector3.h"
#include "TMath.h"
#include "TRandom.h"
#include "TH1F.h"
#include <string>
#include <iostream>


#include "R3BLandPoint.h"
		
using std::cout;
using std::endl;

		

R3BLandDigitizer::R3BLandDigitizer() :
  FairTask("R3B Land Digitization scheme ") { 
}


R3BLandDigitizer::~R3BLandDigitizer() {
}


InitStatus R3BLandDigitizer::Init() {

  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fLandPoints = (TClonesArray*) ioman->GetObject("LandPoint");
   
  // Register output array LandDigi
  //fDigis = new TClonesArray("R3BLandDigi",1000);
  //ioman->Register("LandDigi", "Digital response in Land", fDigis, kTRUE);

  // Initialise control histograms

  h_ne = new TH1F("ne","primary_el",100,1.,50.);
  

  
  return kSUCCESS;

}

void R3BLandDigitizer::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  //FairRuntimeDb* db = run->GetRuntimeDb();
  //if ( ! db ) Fatal("SetParContainers", "No runtime database");

 
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void R3BLandDigitizer::Exec(Option_t* opt) {
 
//-Reset entries in output arrays
//-Reset local arrays 
   Reset();

//-Now do the job event/event    
   // cout << " Entries in LandPoint " << fLandPoints->GetEntries() << endl;
   Int_t nentries = fLandPoints->GetEntries();
   
   for (Int_t l=0;l<nentries;l++){
      // Get the Land Object in array
      R3BLandPoint *land_obj = (R3BLandPoint*) fLandPoints->At(l);
      Int_t paddle = land_obj->GetSector();
      Int_t gap = land_obj->GetPaddleNb();
      //cout << "-I- LANDOBJ Paddle " << paddle << "gap " << gap << endl;
      Double_t eloss = land_obj->GetEnergyLoss();  

      if (eloss > 0 ) {
	   Double_t ne = 0.0;  
	   if ( tof[paddle] < 1.e-15 ){
	      tof[paddle] = land_obj->GetTime(); // time since part. start [ns] 
	      //cout << "-I- LANDOBJ tof " << tof[paddle] << endl;  
	   }//! tof > 1e-15
	   
           // Check the Units of Tof in [ns] here    
	   if ( ( land_obj->GetTime()-tof[paddle] )*1e+9 < 1. ){
	     ne = eloss * 1e+9 / 25.; 
	     //   cout << "-I- LANDOBJ Nb of el: " << ne << endl;  
	   }
	   
           h_ne ->Fill( ne );  

	   // Check the Ne ??  
	   Int_t ine = (Int_t) (ne+0.5);
	   for (Int_t i=0;i<ine;i++ ){
	      Double_t zz = gRandom->Rndm();     
	      Double_t xx = zz * 0.3;
              Double_t dd =0.3;       // [mm]
	      Double_t lambda = 0.1;  // [mm]
	      Double_t vv = 0.21e-3;  // [mm/ps]
	      Double_t tt = ( xx - lambda ) / vv ;
	      //  cout << "-I LandOBJ tt: " <<  tt << endl;
	      Double_t alpha = 123.0;
	      Double_t eta = 10.5;
	      Double_t l1=0.008;
              Double_t tsat = TMath::Log(1.6e+7)/(alpha-eta)/vv;   
              Double_t qq=0.0;
	      Double_t qq2=0.0;

	      Int_t itt = (Int_t) (tt);
	      for (Int_t j=0;j<itt;j++){
		  Double_t strom = 0.0;
		  if ( j < Int_t(tsat) ) strom=0.5*1.6022e-19*vv*TMath::Exp((alpha-eta)*vv*j*1.);
		  else
		     strom=0.5*1.6022e-19*vv*1.6e+7;    
		  qq=qq+strom;
		  Double_t nbar = TMath::Exp((alpha-eta)*vv*j*1.);
		  Double_t kk = eta/alpha;
		  zz = gRandom->Rndm();
		  Double_t avan =0.0;
		  if ( zz < (kk*(nbar-1)/(nbar-kk))) avan =0.0; 
                  else {
		     Double_t f1 = TMath::Log(1-(1-kk)/(nbar-kk));
		     Double_t f2=  TMath::Log((nbar-kk)*(1-zz)/nbar/(1-kk) );
		     avan =1.+ (1./f1)*(f2);
		  }

                  // QDC Saturation   
		  if (avan > 1.6e+7 ) avan = 1.6e+7;
		  if (avan < 0. ) avan = 0.0;
		  //  cout << "-I- LandOBJ Avalanche:" << xx << " avan: " << avan << endl;
		  
		   Double_t current = 0.5*vv*1.6022e-19*avan;
		   qq2=qq2+current;
	      }// !j

	      qq  =  qq *1e+12;
	      qq2 =  qq2*1e+12;

              // ??? is qcharge recalculated here or it is "qq2"??
	      
	      Double_t qcharge = 0.5 / (alpha-eta)*1.6022e-19*
		 TMath::Exp((alpha-eta)*(0.3-xx)-1)*1.e+12;
	      if ( qcharge>2.56 ) qcharge = 2.56;
              qcharge = qq2 ;
	           cout << "-I LandObj  Qcharge: " << qcharge << endl;



	      //??? The processHit has to be changed and Hit Info also !!!
	      x_pos[paddle]  = x_pos[paddle] + land_obj->GetXOut() *qcharge;
	      y_pos[paddle]  = y_pos[paddle] + land_obj->GetYOut() *qcharge;
	      z_pos[paddle]  = z_pos[paddle] + land_obj->GetZOut() *qcharge;
	      nuhits[paddle] = nuhits[paddle] +1;

	      paddle_E[paddle][gap] = paddle_E[paddle][gap] + qcharge;
	      paddle_E[paddle][9] = paddle_E[paddle][9] + qcharge;

	      Int_t first =  (Int_t) (paddle/20.)/2.;
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
	   
	}//! eloss	 
           
   }//! MC hits   

   
}
// -------------------------------------------------------------------------

void R3BLandDigitizer::Reset(){
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

void R3BLandDigitizer::Finish()
{
// here event. write histos
//   cout << " -I- Digit Finish() called " << endl;
// Write control histograms
   h_ne->Write();
}

ClassImp(R3BLandDigitizer)
