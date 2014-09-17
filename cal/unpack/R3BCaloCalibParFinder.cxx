// -------------------------------------------------------------------------
// -----                 R3BCaloCalibParFinder source file             -----
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

#include "R3BCaloRawHit.h"
#include "R3BCaloCalibParFinder.h"

using std::cout;
using std::cerr;
using std::endl;


R3BCaloCalibParFinder::R3BCaloCalibParFinder() : 
FairTask("R3B CALIFA Calibration Parameters Finder ")
{
}


R3BCaloCalibParFinder::~R3BCaloCalibParFinder()
{
}


// -----   Public method Init   --------------------------------------------
InitStatus R3BCaloCalibParFinder::Init()
{
  Register();
  return kSUCCESS;
}


void R3BCaloCalibParFinder::SetParContainers()
{
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if (!run) Fatal("R3BCaloCalibParFinder::SetParContainers", "No analysis run");

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if (!rtdb) Fatal("R3BCaloCalibParFinder::SetParContainers", "No runtime database");

  fCaloCalibPar = (R3BCaloCalibPar*)(rtdb->getContainer("R3BCaloCalibPar"));
  
  if ( fVerbose && fCaloCalibPar ) {
    LOG(INFO) << "R3BCaloCalibParFinder::SetParContainers() "<< FairLogger::endl;
    LOG(INFO) << "Container R3BCaloCalibPar loaded " << FairLogger::endl;
  }
}


//Register: Protected method
void R3BCaloCalibParFinder::Register()
{
  LOG(DEBUG) << "Registering" << FairLogger::endl;
  FairRootManager* fMan = FairRootManager::Instance();
  if(!fMan) {
    Fatal("Init", "No FairRootManager");
    return;
  }
  fCaloRawHitCA = (TClonesArray*) fMan->GetObject("CaloRawHit");
  
  // Parameter retrieval from par container
  // ...
  // Table for crystal parameters from Geometry
  // Initialization of variables, histograms, ...

  return;
}



// -----   Public method ReInit   --------------------------------------------
InitStatus R3BCaloCalibParFinder::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}


// -----   Public method Exec   --------------------------------------------
void R3BCaloCalibParFinder::Exec(Option_t* opt)
{

  // Reset entries in output arrays, local arrays
  Reset();

}


// ---- Public method Reset   --------------------------------------------------
void R3BCaloCalibParFinder::Reset()
{
  // Clear the CA structure
  
  if (fCaloRawHitCA) fCaloRawHitCA->Clear();
}




// ---- Public method Finish   --------------------------------------------------
void R3BCaloCalibParFinder::Finish()
{
}


ClassImp(R3BCaloCalibParFinder)
