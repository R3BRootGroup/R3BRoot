// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BCaloCalib                            -----
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
#include "R3BCaloCrystalHit.h"
#include "R3BCaloCalib.h"

//R3BCaloCalib: Constructor
R3BCaloCalib::R3BCaloCalib() : FairTask("R3B CALIFA Calibrator"),
			       fRawHitCA(0),
			       fCrystalHitCA(new TClonesArray("R3BCaloCrystalHit")), 
			       fCaloCalibPar(0)
{
}



//Virtual R3BCaloCalib: Public method
R3BCaloCalib::~R3BCaloCalib()
{
  LOG(INFO) << "R3BCaloCalib: Delete instance" << FairLogger::endl;
  delete fRawHitCA;
  delete fCrystalHitCA;
}



//Init: Public method
InitStatus R3BCaloCalib::Init()
{
  Register();
  return kSUCCESS;
}


void R3BCaloCalib::SetParContainers()
{
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if (!run) Fatal("R3BCaloCalib::SetParContainers", "No analysis run");
  
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if (!rtdb) Fatal("R3BCaloCalib::SetParContainers", "No runtime database");
  
  fCaloCalibPar = (R3BCaloCalibPar*)(rtdb->getContainer("R3BCaloCalibPar"));
  
  if ( fVerbose && fCaloCalibPar ) {
    LOG(INFO) << "R3BCaloCalib::SetParContainers() "<< FairLogger::endl;
    LOG(INFO) << "Container R3BCaloCalibPar loaded " << FairLogger::endl;
  }  
}


//Register: Protected method
void R3BCaloCalib::Register()
{
  LOG(DEBUG) << "Registering" << FairLogger::endl;
  FairRootManager *fMan = FairRootManager::Instance();
  if(! fMan) { 
    Fatal("Init", "No FairRootManager");
    return;
  }
  fRawHitCA = (TClonesArray*) fMan->GetObject("CaloRawHit");

  fMan->Register("CrystalHit","Calibrated data from Califa",fCrystalHitCA,kTRUE);
}


// -----   Public method ReInit   --------------------------------------------
InitStatus R3BCaloCalib::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

//DoCalib: Public method
void R3BCaloCalib::Exec(Option_t* opt)
{
  
  LOG(DEBUG) << "Calibring CALIFA Raw Data Calo" << FairLogger::endl;
  
  R3BCaloRawHit**    rawHit;
  Int_t crystal_id=0;  
  Double32_t energy=0;  
  Double32_t n_f=0;  
  Double32_t n_s=0;  
  Double32_t time=0;  


  Int_t rawHits;        // Nb of RawHits in current event
  rawHits = fRawHitCA->GetEntries();
  if (rawHits>0) {
    rawHit = new R3BCaloRawHit*[rawHits];
    for (Int_t i=0; i<rawHits; i++) {
      rawHit[i] = new R3BCaloRawHit;
      rawHit[i] = (R3BCaloRawHit*) fCrystalHitCA->At(i);
      
      crystal_id = MapCrystalID(rawHit[i]);
      energy = CalibrateEnergy(rawHit[i]);
      n_f = CalibrateFastComponent(rawHit[i]);
      n_s = CalibrateSlowComponent(rawHit[i]);
      time = CalibrateTime(rawHit[i]);
      
      new ((*fCrystalHitCA)[i]) R3BCaloCrystalHit(crystal_id, energy, n_f, n_s, time);
      
    }  
  }
  
  return;
}


void R3BCaloCalib::Finish()
{
}


//Reset: Public method
void R3BCaloCalib::Reset()
{
  LOG(DEBUG) << "Clearing CrystalHit Data Structure" << FairLogger::endl;
  if(fCrystalHitCA)fCrystalHitCA->Clear();
}


Int_t R3BCaloCalib::MapCrystalID(R3BCaloRawHit* chit)
{
  LOG(DEBUG) << "Mapping Crystal ID in R3BCaloCalib" << FairLogger::endl;

  //Implement here the mapping from the unpacker to physical crystal numbers
  return chit->GetCrystalId();
}

Double32_t R3BCaloCalib::CalibrateEnergy(R3BCaloRawHit* chit)
{
  LOG(DEBUG) << "Calibrating Energies in R3BCaloCalib" << FairLogger::endl;

  //Implement here the energy calibration based on the parameters
  return chit->GetEnergy();
}

Double32_t R3BCaloCalib::CalibrateFastComponent(R3BCaloRawHit* chit)
{
  LOG(DEBUG) << "Calibrating Fast Component in R3BCaloCalib" << FairLogger::endl;

  return chit->GetNf();
}

Double32_t R3BCaloCalib::CalibrateSlowComponent(R3BCaloRawHit* chit)
{
  LOG(DEBUG) << "Calibrating Slow Component in R3BCaloCalib" << FairLogger::endl;

  return chit->GetNs();
}

Double32_t R3BCaloCalib::CalibrateTime(R3BCaloRawHit* chit)
{
  LOG(DEBUG) << "Calibrating Time in R3BCaloCalib" << FairLogger::endl;

  return chit->GetTime();
}



ClassImp(R3BCaloCalib)
