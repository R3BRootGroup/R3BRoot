// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           from R3BCaloCalib                       -----
// -----                 Created 18/07/2014 by H. Alvarez Pol              -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

//ROOT headers
#include "TClonesArray.h"

//Fair headers
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include <iomanip>

//Califa headers
#include "R3BSTaRTrackerDigitHit.h"
#include "R3BStarTrackCalib.h"

//R3BStarTrackCalib: Constructor
R3BStarTrackCalib::R3BStarTrackCalib() : FairTask("R3B STaRTracker Calibrator"),
			       fRawHitCA(0),
			       fSiDetHitCA(new TClonesArray("R3BSTaRTrackHit")), 
			       fStarTrackCalibPar(0)
{
}



//Virtual R3BStarTrackCalib: Public method
R3BStarTrackCalib::~R3BStarTrackCalib()
{
  LOG(INFO) << "R3BStarTrackCalib: Delete instance" << FairLogger::endl;
  delete fRawHitCA;
  delete fSiDetHitCA;
}



//Init: Public method
InitStatus R3BStarTrackCalib::Init()
{
  Register();
  return kSUCCESS;
}


void R3BStarTrackCalib::SetParContainers()
{
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if (!run) Fatal("R3BStarTrackCalib::SetParContainers", "No analysis run");
  
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if (!rtdb) Fatal("R3BStarTraCalib::SetParContainers", "No runtime database");
  
  fStarTrackCalibPar = (R3BStarTrackCalibPar*)(rtdb->getContainer("R3BStarTrackCalibPar"));
  
  if ( fVerbose && fStarTrackCalibPar ) {
    LOG(INFO) << "R3BStarTraCalib::SetParContainers() "<< FairLogger::endl;
    LOG(INFO) << "Container R3BStarTraCalibPar loaded " << FairLogger::endl;
  }  
}


//Register: Protected method
void R3BStarTrackCalib::Register()
{
  LOG(DEBUG) << "Registering" << FairLogger::endl;
  FairRootManager *fMan = FairRootManager::Instance();
  if(! fMan) { 
    Fatal("Init", "No FairRootManager");
    return;
  }
  fRawHitCA = (TClonesArray*) fMan->GetObject("StarTrackRawHit");

  fMan->Register("SiDetHit","Calibrated data from Startracker",fSiDetHitCA,kTRUE);
}


// -----   Public method ReInit   --------------------------------------------
InitStatus R3BStarTrackCalib::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

//DoCalib: Public method
void R3BStarTrackCalib::Exec(Option_t* opt)
{
  
  LOG(DEBUG) << "Calibring StarTracker Raw Data" << FairLogger::endl;
  
  R3BStarTrackRawHit**    rawHit;
  Int_t module_id=0;  
  Int_t side=0;  
  Int_t asic_id=0;  
  Int_t strip_id=0;  
  Double32_t energy=0;  
  Double32_t time=0;  


  Int_t rawHits;        // Nb of RawHits in current event
  rawHits = fRawHitCA->GetEntries();
  if (rawHits>0) {
    rawHit = new R3BStarTrackRawHit*[rawHits];
    for (Int_t i=0; i<rawHits; i++) {
      rawHit[i] = new R3BStarTrackRawHit;
      rawHit[i] = (R3BStarTrackRawHit*) fSiDetHitCA->At(i);
      
      module_id = MapModuleID(rawHit[i]);
      side = MapSide(rawHit[i]);
      asic_id = MapAsicID(rawHit[i]);
      strip_id = MapStripID(rawHit[i]);
      energy = CalibrateEnergy(rawHit[i]);
      time = CalibrateTime(rawHit[i]);
      
      new ((*fSiDetHitCA)[i]) R3BSTaRTrackerDigitHit(module_id, asic_id, side, strip_id, energy, time);
      
    }  
  }
  
  return;
}


void R3BStarTrackCalib::Finish()
{
}


//Reset: Public method
void R3BStarTrackCalib::Reset()
{
  LOG(DEBUG) << "Clearing Si tracker Hit Data Structure" << FairLogger::endl;
  if(fSiDetHitCA)fSiDetHitCA->Clear();
}


Int_t R3BStarTrackCalib::MapModuleID(R3BStarTrackRawHit* chit)
{
  LOG(DEBUG) << "Mapping Module ID in R3BStarTraCalib" << FairLogger::endl;

  //Implement here the mapping from the unpacker to physical strip numbers
  return chit->GetModuleId();
}

Int_t R3BStarTrackCalib::MapSide(R3BStarTrackRawHit* chit)
{
  LOG(DEBUG) << "Mapping Side ID in R3BStarTraCalib" << FairLogger::endl;

  //Implement here the mapping from the unpacker to physical strip numbers
  return chit->GetSide();
}

Int_t R3BStarTrackCalib::MapAsicID(R3BStarTrackRawHit* chit)
{
  LOG(DEBUG) << "Mapping Asic ID in R3BStarTraCalib" << FairLogger::endl;

  //Implement here the mapping from the unpacker to physical strip numbers
  return chit->GetAsicId();
}

Int_t R3BStarTrackCalib::MapStripID(R3BStarTrackRawHit* chit)
{
  LOG(DEBUG) << "Mapping Strip ID in R3BStarTraCalib" << FairLogger::endl;

  //Implement here the mapping from the unpacker to physical strip numbers
  return chit->GetStripId();
}


Double32_t R3BStarTrackCalib::CalibrateEnergy(R3BStarTrackRawHit* chit)
{
  LOG(DEBUG) << "Calibrating Energies in R3BStarTraCalib" << FairLogger::endl;

  //Implement here the energy calibration based on the parameters
  return chit->GetADCdata();
}

Double32_t R3BStarTrackCalib::CalibrateTime(R3BStarTrackRawHit* chit)
{
  LOG(DEBUG) << "Calibrating Time in R3BStarTraCalib" << FairLogger::endl;

  //Implement here the time calibration based on the parameters
  return chit->GetTime();
  //return chit->GetTimelb();
}



ClassImp(R3BStarTrackCalib)
