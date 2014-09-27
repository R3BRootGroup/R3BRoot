// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BCaloCal                              -----
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
#include "R3BCaloCal.h"

//R3BCaloCal: Constructor
R3BCaloCal::R3BCaloCal() : FairTask("R3B CALIFA Calibrator"),
			       fRawHitCA(0),
			       fCrystalHitCA(new TClonesArray("R3BCaloCrystalHit")), 
			       fCaloCalPar(0)
{
}



//Virtual R3BCaloCal: Public method
R3BCaloCal::~R3BCaloCal()
{
  LOG(INFO) << "R3BCaloCal: Delete instance" << FairLogger::endl;
  delete fRawHitCA;
  delete fCrystalHitCA;
}



//Init: Public method
InitStatus R3BCaloCal::Init()
{
  Register();
  return kSUCCESS;
}


void R3BCaloCal::SetParContainers()
{
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if (!run) Fatal("R3BCaloCal::SetParContainers", "No analysis run");
  
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if (!rtdb) Fatal("R3BCaloCal::SetParContainers", "No runtime database");
  
  fCaloCalPar = (R3BCaloCalPar*)(rtdb->getContainer("CaloCalPar"));
  
  if ( fVerbose && fCaloCalPar ) {
    LOG(INFO) << "R3BCaloCal::SetParContainers() "<< FairLogger::endl;
    LOG(INFO) << "Container R3BCaloCalPar loaded " << FairLogger::endl;
  }  
}


//Register: Protected method
void R3BCaloCal::Register()
{
  LOG(DEBUG) << "Registering" << FairLogger::endl;
  FairRootManager *fMan = FairRootManager::Instance();
  if(! fMan) { 
    Fatal("Init", "No FairRootManager");
    return;
  }
  fRawHitCA = (TClonesArray*) fMan->GetObject("CaloRawHit");
  if(NULL == fRawHitCA) {
    FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch CaloRawHit not found");
    }
  fMan->Register("CaloCrystalHit","CaloCalibrated",fCrystalHitCA,kTRUE);
}


// -----   Public method ReInit   --------------------------------------------
InitStatus R3BCaloCal::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

//DoCalib: Public method
void R3BCaloCal::Exec(Option_t* option)
{
  
  LOG(DEBUG) << "Calibrating CALIFA Raw Data Calo" << FairLogger::endl;
  
  TRandom *r0 = new TRandom();
  Float_t rando = r0->Rndm();

  R3BCaloRawHit*  rawHit;
  Int_t crystal_id=0;  
  Double32_t energy=0;  
  Double32_t n_f=0;  
  Double32_t n_s=0;  
  Double32_t time=0;    
  Double32_t tot_energy=0;    
  
  Int_t rawHits;        // Nb of RawHits in current event
  rawHits = fRawHitCA->GetEntries();
  if (rawHits>0) {
    for (Int_t i=0; i<rawHits; i++) {
      rawHit= (R3BCaloRawHit*) fRawHitCA->At(i);
      
      crystal_id = rawHit->GetCrystalId();
      
      energy = fCaloCalPar->GetDUCalParAt(rawHit->GetCrystalId())->GetGammaCal_offset() + 
	( (rawHit->GetEnergy()+(rando-0.5) ) * fCaloCalPar->GetDUCalParAt(rawHit->GetCrystalId())->GetGammaCal_gain());
      
      n_f = rawHit->GetNf(); 
      
      n_s = rawHit->GetNs(); 
      
      time = rawHit->GetTime();
      
      tot_energy = fCaloCalPar->GetDUCalParAt(rawHit->GetCrystalId())->GetToTCal_offset() + 
      	( (rawHit->GetTot()+(rando-0.5) ) * fCaloCalPar->GetDUCalParAt(rawHit->GetCrystalId())->GetToTCal_gain()); 
      
      //TODO: Add tot_energy in CaloCrystalHit
      new ((*fCrystalHitCA)[i]) R3BCaloCrystalHit(crystal_id, energy, n_f, n_s, time);
      
    }  
  }
  
  return;
}


void R3BCaloCal::Finish()
{
}


//Reset: Public method
void R3BCaloCal::Reset()
{
  LOG(DEBUG) << "Clearing CrystalHit Data Structure" << FairLogger::endl;
  if(fCrystalHitCA)fCrystalHitCA->Clear();
}


ClassImp(R3BCaloCal)
