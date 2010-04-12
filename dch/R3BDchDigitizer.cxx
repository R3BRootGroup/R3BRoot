#include "R3BDchDigitizer.h"
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


#include "R3BDchPoint.h"
#include "R3BMCTrack.h"		
using std::cout;
using std::endl;

		

R3BDchDigitizer::R3BDchDigitizer() :
  FairTask("R3B Dch Digitization scheme ") { 
}


R3BDchDigitizer::~R3BDchDigitizer() {
}


void R3BDchDigitizer::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if ( ! rtdb ) Fatal("SetParContainers", "No runtime database");

  fDchDigiPar = (R3BDchDigiPar*)(rtdb->getContainer("R3BDchDigiPar"));

  if ( fDchDigiPar ) {
      cout << "-I- R3BDchDigitizer::SetParContainers() "<< endl;
      cout << "-I- Container R3BDchDigiPar  loaded " << endl;
  }

}




InitStatus R3BDchDigitizer::Init() {

  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fDchPoints = (TClonesArray*) ioman->GetObject("DchPoint");
  fDchMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
   
  // Register output array DchDigi
  fDchDigi = new TClonesArray("R3BDchDigi",1000);
  ioman->Register("DchDigi", "Digital response in Dch", fDchDigi, kTRUE);

  return kSUCCESS;

}


void R3BDchDigitizer::Exec(Option_t* opt) {

   Reset();


}
// -------------------------------------------------------------------------

void R3BDchDigitizer::Reset(){
// Clear the structure
//   cout << " -I- Digit Reset() called " << endl;

   
 if (fDchDigi ) fDchDigi->Clear();
}   

void R3BDchDigitizer::Finish()
{
// Write control histograms

}

//R3BDchDigi* R3BDchDigitizer::AddHit(
 // return new(clref[size]) R3BDchDigi();
//}

ClassImp(R3BDchDigitizer)
