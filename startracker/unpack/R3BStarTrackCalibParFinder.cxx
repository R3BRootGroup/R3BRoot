// -------------------------------------------------------------------------
// -----                 R3BStarTrackCalibParFinder source file        -----
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

#include "R3BStarTrackRawHit.h"
#include "R3BStarTrackCalibParFinder.h"

using std::cout;
using std::cerr;
using std::endl;


R3BStarTrackCalibParFinder::R3BStarTrackCalibParFinder() : 
FairTask("R3B StarTracker Calibration Parameters Finder ")
{
}


R3BStarTrackCalibParFinder::~R3BStarTrackCalibParFinder()
{
}


// -----   Public method Init   --------------------------------------------
InitStatus R3BStarTrackCalibParFinder::Init()
{
  Register();
  return kSUCCESS;
}


void R3BStarTrackCalibParFinder::SetParContainers()
{
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if (!run) Fatal("R3BStarTrackCalibParFinder::SetParContainers", "No analysis run");

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if (!rtdb) Fatal("R3BStarTrackCalibParFinder::SetParContainers", "No runtime database");

  fStarTrackCalibPar = (R3BStarTrackCalibPar*)(rtdb->getContainer("R3BStarTrackCalibPar"));
  
  if ( fVerbose && fStarTrackCalibPar ) {
    LOG(INFO) << "R3BStarTrackCalibParFinder::SetParContainers() "<< FairLogger::endl;
    LOG(INFO) << "Container R3BStarTrackCalibPar loaded " << FairLogger::endl;
  }
}


//Register: Protected method
void R3BStarTrackCalibParFinder::Register()
{
  LOG(DEBUG) << "Registering" << FairLogger::endl;
  FairRootManager* fMan = FairRootManager::Instance();
  if(!fMan) {
    Fatal("Init", "No FairRootManager");
    return;
  }
  fSiDetRawHitCA = (TClonesArray*) fMan->GetObject("StarTrackRawHit");
  
  // Parameter retrieval from par container
  // ...
  // Table for crystal parameters from Geometry
  // Initialization of variables, histograms, ...

  return;
}



// -----   Public method ReInit   --------------------------------------------
InitStatus R3BStarTrackCalibParFinder::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}


// -----   Public method Exec   --------------------------------------------
void R3BStarTrackCalibParFinder::Exec(Option_t* opt)
{

  // Reset entries in output arrays, local arrays
  Reset();

}


// ---- Public method Reset   --------------------------------------------------
void R3BStarTrackCalibParFinder::Reset()
{
  // Clear the CA structure
  
  if (fSiDetRawHitCA) fSiDetRawHitCA->Clear();
}




// ---- Public method Finish   --------------------------------------------------
void R3BStarTrackCalibParFinder::Finish()
{
}


ClassImp(R3BStarTrackCalibParFinder)
