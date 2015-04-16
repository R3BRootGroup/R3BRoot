// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BCaloCal                              -----
// -----                 Created 18/07/2014 by H. Alvarez Pol              -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

//ROOT headers
#include "TClonesArray.h"
#include "TMath.h"

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
			       fCaloCalPar(0), calHighRange(false), calPID(false),
                               nEvents(0)
{
  //counter1=0;
  //counter2=0;
}

void R3BCaloCal::UseHighRange(bool highRange)
{
	this->calHighRange = highRange;
}

void R3BCaloCal::SetCalibratePID(bool cal)
{
	this->calPID = cal;
}

//Virtual R3BCaloCal: Public method
R3BCaloCal::~R3BCaloCal()
{
  LOG(INFO) << "R3BCaloCal: Delete instance" << FairLogger::endl;
  delete fRawHitCA;
  delete fCrystalHitCA;
  delete ran;
}



//Init: Public method
InitStatus R3BCaloCal::Init()
{
  Register();
  ran = new TRandom(0);
  return kSUCCESS;
}

void bicycle() {
  static int pos=0;
  char cursor[4]={'/','-','\\','|'};
  printf("(%c)  (%c)  (%c)  (%c)  (%c)  (%c)  (%c)  \r                     ",cursor[pos],cursor[pos],cursor[pos],cursor[pos],cursor[pos],cursor[pos],cursor[pos]);
  fflush(stdout);
  pos = (pos+1)%4;
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
 
  if(++nEvents % 10000 == 0)
     LOG(INFO) << nEvents << FairLogger::endl;

// Note: Using the same "random" value all the time is useless against quantization effects
//  Float_t rando = ran->Rndm()-0.5;
  
  if(fCrystalHitCA)fCrystalHitCA->Delete();

  R3BCaloRawHit*  rawHit;
  Int_t rawHit_id=0;  
  Int_t crystal_id=0;  
  Double32_t energy=0;  
  Double32_t n_f=0;  
  Double32_t n_s=0;  
  ULong64_t time=0;    
  Double32_t tot_energy=0;    
  Double32_t tempResult=0;    
  Double_t tau = 878.625358; //electronics constant, taken a fixed value for the moment! // Not any more.. BP
   
  Int_t rawHits;        // Nb of RawHits in current event
  rawHits = fRawHitCA->GetEntries();
 
  Double32_t gamma_gain, gamma_offs, range_gain, range_offs, pid_gain;
  R3BCaloDUCalPar *p;
 
  if (rawHits>0) {
    for (Int_t i=0; i<rawHits; i++) {
      rawHit= (R3BCaloRawHit*) fRawHitCA->At(i);      
      rawHit_id= rawHit->GetCrystalId();
      crystal_id = rawHit_id;
      p = fCaloCalPar->GetDUCalParAt(rawHit_id);

      gamma_offs = p->GetGammaCal_offset();
      gamma_gain = p->GetGammaCal_gain();
      energy =  gamma_offs + ((rawHit->GetEnergy() + ran->Rndm()-0.5) * gamma_gain);
      if(calHighRange)
      {
	range_gain = p->GetRangeCal_gain();
	range_offs = p->GetRangeCal_offset();
	energy = range_offs + (energy * range_gain);
      }
      n_f = (rawHit->GetNf() + ran->Rndm()-0.5); 
      n_s = (rawHit->GetNs() + ran->Rndm()-0.5); 
      if(calPID)
      {
         n_f *= gamma_gain * p->GetPidGain();
         n_s *= gamma_gain * p->GetPidGain();

         if(calHighRange)
         {
            n_f *= range_gain;
            n_s *= range_gain;
         }
      }
      time = rawHit->GetTime();
      
      //tot converted from tot-channels to 300MeV range channels
      
//      tempResult = fCaloCalPar->GetDUCalParAt(rawHit_id)->GetToTCal_offset() + 	( TMath::Exp((rawHit->GetTot()+rando)/tau) * fCaloCalPar->GetDUCalParAt(rawHit_id)->GetToTCal_gain()); 

	  tempResult = p->GetToTCal_par0()*(TMath::Exp((rawHit->GetTot()+ran->Rndm()-0.5)/p->GetToTCal_par1()))+p->GetToTCal_par2();
	  // E=par0*exp(TOT/par1)+par2, par1 is not constant..
	
      //tot converted from 300MeV range channels to 30MeV range channels
      tempResult = p->GetRangeCal_offset() + 
	( tempResult* p->GetRangeCal_gain()); 
	
      //tot converted from 30MeV range channels to energy (keV)
         if(rawHit->GetTot()>0)
      		{   
      tot_energy = p->GetGammaCal_offset() + 
      	(tempResult * p->GetGammaCal_gain());
      		}
	  else tot_energy = 0;
      new ((*fCrystalHitCA)[i]) R3BCaloCrystalHit(crystal_id, energy, n_f, n_s, time, tot_energy);
      
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
