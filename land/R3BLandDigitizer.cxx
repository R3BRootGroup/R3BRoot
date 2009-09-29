#include "R3BLandDigitizer.h"
#include "TClonesArray.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"


#include "TVector3.h"
#include "TH1F.h"
#include <string>
#include <iostream>

		
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
// here do the job event/event  
   cout << " Entries in LandPoint " << fLandPoints->GetEntries() << endl;

}
// -------------------------------------------------------------------------

void R3BLandDigitizer::Reset(){
// Clear the structure
   
// if (fDigis ) fDigis->Clear();
}   

void R3BLandDigitizer::Finish()
{
// here event. write histos

}

ClassImp(R3BLandDigitizer)
