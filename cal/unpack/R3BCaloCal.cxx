// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BCaloCal                            -----
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
			       fCrystalHitCA(new TClonesArray("CaloCrystalHit")), 
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

  fMan->Register("CrystalHit","Calibrated data from Califa",fCrystalHitCA,kTRUE);
}


// -----   Public method ReInit   --------------------------------------------
InitStatus R3BCaloCal::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

//DoCalib: Public method
void R3BCaloCal::Exec(/*char c*/)
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
      energy = CalibrateEnergy(rawHit[i]/*,c*/);
      n_f = CalibrateFastComponent(rawHit[i]);
      n_s = CalibrateSlowComponent(rawHit[i]);
      time = CalibrateTime(rawHit[i]);
      
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


Int_t R3BCaloCal::MapCrystalID(R3BCaloRawHit* chit)
{
  LOG(DEBUG) << "Mapping Crystal ID in R3BCaloCal" << FairLogger::endl;
  //Implement here the mapping from the unpacker to physical crystal numbers
  return fCaloCalPar->GetDUCalParAt(chit->GetCrystalId())->GetDetectionUnit();
}

Double32_t R3BCaloCal::CalibrateEnergy(R3BCaloRawHit* chit/*, char c*/)
{
  LOG(DEBUG) << "Calibrating Energies in R3BCaloCal" << FairLogger::endl;
  //Implement here the energy calibration based on the parameters
  TRandom *r0 = new TRandom();
  rando = r0->Rndm(chit->GetCrystalId());
  /*if (c=='g') {
  	return fCaloCalPar->GetDUCalParAt(chit->GetCrystalId())->GetGammaParZero()+(chit->GetEnergy()+(rando-0.5))*fCaloCalPar->GetDUCalParAt(chit->GetCrystalId())->GetGammaParOne();
  } else if (c=='p') {
	return fCaloCalPar->GetDUCalParAt(chit->GetCrystalId())->GetProtonParZero()+(chit->GetEnergy()+(rando-0.5))*fCaloCalPar->GetDUCalParAt(chit->GetCrystalId())->GetProtonParOne();
  } else {
  	return 0.;
  }*/
  return fCaloCalPar->GetDUCalParAt(chit->GetCrystalId())->GetParZero()+((chit->GetEnergy()+(rando-0.5))*fCaloCalPar->GetDUCalParAt(chit->GetCrystalId())->GetParOne())/fCaloCalPar->GetDUCalParAt(chit->GetCrystalId())->GetConversionFactor();
}

Double32_t R3BCaloCal::CalibrateFastComponent(R3BCaloRawHit* chit)
{
  LOG(DEBUG) << "Calibrating Fast Component in R3BCaloCal" << FairLogger::endl;
  return chit->GetNf();
}

Double32_t R3BCaloCal::CalibrateSlowComponent(R3BCaloRawHit* chit)
{
  LOG(DEBUG) << "Calibrating Slow Component in R3BCaloCal" << FairLogger::endl;

  return chit->GetNs();
}

Double32_t R3BCaloCal::CalibrateTime(R3BCaloRawHit* chit)
{
  LOG(DEBUG) << "Calibrating Time in R3BCaloCal" << FairLogger::endl;
  return chit->GetTime();
}



ClassImp(R3BCaloCal)
