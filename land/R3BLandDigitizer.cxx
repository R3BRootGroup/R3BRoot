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
  h_ne = new TH1F("Ne","primary_el",30,0.,30.);
  h_ch = new TH1F("Charge","",1000,0.,1.);

  
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

// Create Stockastic Avalanche

//-Now do the job event/event

   Int_t nentries = fLandPoints->GetEntries();



for (Int_t l=0;l<nentries;l++){
      // Get the Land Object in array
      R3BLandPoint *land_obj = (R3BLandPoint*) fLandPoints->At(l);
      Int_t paddle = land_obj->GetSector();
      Int_t gap = land_obj->GetPaddleNb();


      // -- <DB>
      // -- Example how to use the ROOT Geometry navigation
      // -- System to recover Geometrical information for
      // -- the analysis part


      // 1)
      // Calculate point in the middle of the detector. This is
      // for safety reasons, because the point at exit is slightly
      // outside of the active volume. If one does not use a point
      // in the detector one will get a wrong volume from the
      // geomanager. Navigate to the correct volume to get all
      // necessary information about this volume

      Double_t x_mean = (land_obj->GetXIn()+land_obj->GetXOut())/2.;
      Double_t y_mean = (land_obj->GetYIn()+land_obj->GetYOut())/2.;
      Double_t z_mean = (land_obj->GetZIn()+land_obj->GetZOut())/2.;

      // 2)
      // Find the corresponding Node containing these coordinates

      gGeoManager->FindNode(x_mean, y_mean, z_mean);

      //3)
      // Get the corresponding Node containing this point
      TGeoNode* curNode = gGeoManager->GetCurrentNode();
      TGeoMedium *med = curNode->GetMedium();

      //4)
      // Navigator should properly calculate the transformation
      // ( Matrix Product) taking  full volume hiearchy into account.

      const Double_t *global_point = gGeoManager->GetCurrentPoint();
      Double_t local_point[3];
      gGeoManager->MasterToLocal(global_point, local_point);
      TGeoHMatrix *matrix = gGeoManager->GetCurrentMatrix();

      //5)
      // TGeoHMatrix is a special transformation representation
      //
      //         r11  r12  r13   tx
      //         r21  r22  r23   ty
      //         r31  r32  r33   tz
      //          0    0    0    1
      //
      //  where: rij are the 3x3 rotation matrix components,
      //         tx, ty, tz are the translation components
      // Get the corresponding Rotation/Translation

      cout << "-I- Global Matrices representation for Node: "
	   << curNode->GetName() <<  endl;
      const Double_t *rot = matrix->GetRotationMatrix();
      const Double_t *tr  = matrix->GetTranslation();

      printf("-I- Global Matrix Info: %s - tr=%d  rot=%d  refl=%d  scl=%d\n",
	     matrix->GetName(),
	     (Int_t) matrix->IsTranslation(),
	     (Int_t) matrix->IsRotation(),
	     (Int_t) matrix->IsReflection(),
	     (Int_t) matrix->IsScale()
	    );

      // Global Rotation
      printf("%10.6f%12.6f%12.6f \n", rot[0], rot[1], rot[2]);
      printf("%10.6f%12.6f%12.6f \n", rot[3], rot[4], rot[5]);
      printf("%10.6f%12.6f%12.6f \n", rot[6], rot[7], rot[8]);

      // Global Translation
      printf(" Tx = %10.6f\n", tr[0]);
      printf(" Ty = %10.6f\n", tr[1]);
      printf(" Tz = %10.6f\n", tr[2]);

      // 6)
      // Get Shape Info
      TGeoVolume *curvol = gGeoManager->GetCurrentVolume();
      TGeoBBox *shape = (TGeoBBox*)curvol->GetShape();

      Float_t sizex = shape->GetDX();
      Float_t sizey = shape->GetDY();
      Float_t sizez = shape->GetDZ();

      cout << "-I- Corresponding Current Shape BBox: (X,Y,Z) " << " X (cm): "
	  << sizex << " Y(cm): " << sizey << " Z (cm): " << sizez << endl;

      // <DB>
      // End of example



      Double_t eloss = land_obj->GetEnergyLoss();
      if (eloss > 0 ) {

	   Double_t ne = 0.0;
	   if ( tof[paddle] < 1.e-15 ){
	      tof[paddle] = land_obj->GetTime(); // time since part. start [ns] 
	      //cout << "-I- LANDOBJ tof " << tof[paddle] << endl;  
	   }//! tof > 1e-15
	   
           // Check the Units of Tof in [ns] here    
	   if ( ( land_obj->GetTime()-tof[paddle] )*1e+9 < 100. ){
	     ne = eloss * 1e+9 / 25.; 
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
	  }//!tof 
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
   h_ch->Write();
}

ClassImp(R3BLandDigitizer)
