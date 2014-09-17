// -------------------------------------------------------------------------
// -----                 R3BCaloCalParFinder source file             -----
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

#include "R3BCaloCalParFinder.h"

using std::cout;
using std::cerr;
using std::endl;


R3BCaloCalParFinder::R3BCaloCalParFinder() : 
FairTask("R3B CALIFA Calibration Parameters Finder ")
{
}


R3BCaloCalParFinder::~R3BCaloCalParFinder()
{
}


// -----   Public method Init   --------------------------------------------
InitStatus R3BCaloCalParFinder::Init()
{
  Register();
  return kSUCCESS;
}


void R3BCaloCalParFinder::SetParContainers()
{
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if (!run) Fatal("R3BCaloCalParFinder::SetParContainers", "No analysis run");

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if (!rtdb) Fatal("R3BCaloCalParFinder::SetParContainers", "No runtime database");

  fCaloCalibPar = (R3BCaloCalPar*)(rtdb->getContainer("R3BCaloCalPar"));
  
  if ( fVerbose && fCaloCalibPar ) {
    LOG(INFO) << "R3BCaloCalParFinder::SetParContainers() "<< FairLogger::endl;
    LOG(INFO) << "Container R3BCaloCalPar loaded " << FairLogger::endl;
  }
}


//Register: Protected method
void R3BCaloCalParFinder::Register()
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
InitStatus R3BCaloCalParFinder::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}


// -----   Public method Exec   --------------------------------------------
void R3BCaloCalParFinder::Exec(Option_t* opt)
{

  // Reset entries in output arrays, local arrays
  Reset();

}


// ---- Public method Reset   --------------------------------------------------
void R3BCaloCalParFinder::Reset()
{
  // Clear the CA structure
  
  if (fCaloRawHitCA) fCaloRawHitCA->Clear();
}




// ---- Public method Finish   --------------------------------------------------
void R3BCaloCalParFinder::Finish()
{
}


ClassImp(R3BCaloCalParFinder)
