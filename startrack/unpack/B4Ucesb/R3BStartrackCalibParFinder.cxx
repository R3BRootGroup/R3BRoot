// -------------------------------------------------------------------------
// -----                 R3BStartrackCalibParFinder source file        -----
// -----                 from R3BCaloCalibParFinder source file        -----
// -----                  Created 22/07/14  by H.Alvarez               -----
// -------------------------------------------------------------------------
#include "TMath.h"
#include "TVector3.h"
#include "TGeoMatrix.h"

#include "TClonesArray.h"
#include "TObjArray.h"
#include "TRandom.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TGeoManager.h"

#include "R3BStartrackRawHit.h"
#include "R3BStartrackCalibParFinder.h"

using std::cout;
using std::cerr;
using std::endl;


R3BStartrackCalibParFinder::R3BStartrackCalibParFinder() : 
FairTask("R3B Startracker Calibration Parameters Finder ")
{
}


R3BStartrackCalibParFinder::~R3BStartrackCalibParFinder()
{
}


// -----   Public method Init   --------------------------------------------
InitStatus R3BStartrackCalibParFinder::Init()
{
  Register();
  return kSUCCESS;
}


void R3BStartrackCalibParFinder::SetParContainers()
{
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if (!run) Fatal("R3BStartrackCalibParFinder::SetParContainers", "No analysis run");

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if (!rtdb) Fatal("R3BStartrackCalibParFinder::SetParContainers", "No runtime database");

  fStartrackCalibPar = (R3BStartrackCalibPar*)(rtdb->getContainer("R3BStartrackCalibPar"));
  
  if ( fVerbose && fStartrackCalibPar ) {
    LOG(INFO) << "R3BStartrackCalibParFinder::SetParContainers() "<< FairLogger::endl;
    LOG(INFO) << "Container R3BStartrackCalibPar loaded " << FairLogger::endl;
  }
}


//Register: Protected method
void R3BStartrackCalibParFinder::Register()
{
  LOG(DEBUG) << "Registering" << FairLogger::endl;
  FairRootManager* fMan = FairRootManager::Instance();
  if(!fMan) {
    Fatal("Init", "No FairRootManager");
    return;
  }
  fSiDetRawHitCA = (TClonesArray*) fMan->GetObject("StartrackRawHit");
  
  // Parameter retrieval from par container
  // ...
  // Table for crystal parameters from Geometry
  // Initialization of variables, histograms, ...

  return;
}



// -----   Public method ReInit   --------------------------------------------
InitStatus R3BStartrackCalibParFinder::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}


// -----   Public method Exec   --------------------------------------------
void R3BStartrackCalibParFinder::Exec(Option_t* opt)
{

  // Reset entries in output arrays, local arrays
  Reset();

}


// ---- Public method Reset   --------------------------------------------------
void R3BStartrackCalibParFinder::Reset()
{
  // Clear the CA structure
  
  if (fSiDetRawHitCA) fSiDetRawHitCA->Clear();
}




// ---- Public method Finish   --------------------------------------------------
void R3BStartrackCalibParFinder::Finish()
{
}


ClassImp(R3BStartrackCalibParFinder)
