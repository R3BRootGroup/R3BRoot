// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                     R3BCalifaMapped2CrystalCal                    -----
// -----                Created 18/07/2014 by H. Alvarez-Pol               -----
// -----                 Modified 20/03/2017 by P. Cabanelas               -----
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
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaMapped2CrystalCal.h"

//R3BCalifaMapped2CrystalCal: Constructor
R3BCalifaMapped2CrystalCal::R3BCalifaMapped2CrystalCal() : FairTask("R3B CALIFA Calibrator"),
			       fMappedDataCA(0),
			       fCrystalCalDataCA(new TClonesArray("R3BCalifaCrystalCalData")), 
			       fCalifaCalPar(0), calHighRange(false), calPID(false),
                               nEvents(0)
{
  //counter1=0;
  //counter2=0;
}

void R3BCalifaMapped2CrystalCal::UseHighRange(bool highRange)
{
	this->calHighRange = highRange;
}

void R3BCalifaMapped2CrystalCal::SetCalibratePID(bool cal)
{
	this->calPID = cal;
}

//Virtual R3BCalifaMapped2CrystalCal: Public method
R3BCalifaMapped2CrystalCal::~R3BCalifaMapped2CrystalCal()
{
  LOG(INFO) << "R3BCalifaMapped2CrystalCal: Delete instance" << FairLogger::endl;
  delete fMappedDataCA;
  delete fCrystalCalDataCA;
  delete ran;
}



//Init: Public method
InitStatus R3BCalifaMapped2CrystalCal::Init()
{
  Register();
  ran = new TRandom(0);
  return kSUCCESS;
}

void R3BCalifaMapped2CrystalCal::SetParContainers()
{
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if (!run) Fatal("R3BCalifaMapped2CrystalCal::SetParContainers", "No analysis run");
  
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  if (!rtdb) Fatal("R3BCalifaMapped2CrystalCal::SetParContainers", "No runtime database");
  
  fCalifaCalPar = (R3BCalifaMapped2CrystalCalPar*)(rtdb->getContainer("CalifaMapped2CrystalCalPar"));
  
  if ( fVerbose && fCalifaCalPar ) {
    LOG(INFO) << "R3BCalifaMapped2CrystalCal::SetParContainers() "<< FairLogger::endl;
    LOG(INFO) << "Container R3BCalifaMapped2CrystalCalPar loaded " << FairLogger::endl;
  }  
}


//Register: Protected method
void R3BCalifaMapped2CrystalCal::Register()
{
  LOG(DEBUG) << "Registering" << FairLogger::endl;
  FairRootManager *fMan = FairRootManager::Instance();
  if(! fMan) { 
    Fatal("Init", "No FairRootManager");
    return;
  }
  fMappedDataCA = (TClonesArray*) fMan->GetObject("CalifaMappedData");
  if(NULL == fMappedDataCA) {
    FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch CalifaMappedData not found");
    }
  fMan->Register("CalifaCrystalCalData","CalifaCalibrated",fCrystalCalDataCA,kTRUE);
}


// -----   Public method ReInit   --------------------------------------------
InitStatus R3BCalifaMapped2CrystalCal::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

//DoCalib: Public method
void R3BCalifaMapped2CrystalCal::Exec(Option_t* option)
{
  LOG(DEBUG) << "Calibrating CALIFA Mapped Data" << FairLogger::endl;
 
  if(++nEvents % 10000 == 0)
     LOG(INFO) << nEvents << FairLogger::endl;

// Note: Using the same "random" value all the time is useless against quantization effects
//  Float_t rando = ran->Rndm()-0.5;
  
  if(fCrystalCalDataCA)fCrystalCalDataCA->Delete();

  R3BCalifaMappedData*  rawHit;
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
  Int_t calHits = 0;
  rawHits = fMappedDataCA->GetEntries();
 
  /*
  //================================================================================
  //FIXME: re-write this part once the DUCalPar and CarystalCalPar containers
  //  are written (needed to be adapted from califadb-fairdb removed module)
  //================================================================================

  Double32_t gamma_gain, gamma_offs, range_gain, range_offs, pid_gain, nfbar, nsbar;
  R3BCalifaDUCalPar *p;
 
  if (rawHits>0) {
    for (Int_t i=0; i<rawHits; i++) {
      rawHit= (R3BCalifaMappedData*) fMappedDataCA->At(i);      

//      if(rawHit->GetError())
//        continue;

      rawHit_id= rawHit->GetCrystalId();
      crystal_id = rawHit_id;
      p = fCalifaCalPar->GetDUCalParAt(rawHit_id);

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
         n_f *= p->GetPidGain();
         n_s *= p->GetPidGain();

         if(n_f + n_s != 0)
         {
           nfbar = gamma_offs*n_f/(n_f + n_s) + gamma_gain*n_f;
           nsbar = gamma_offs*n_s/(n_f + n_s) + gamma_gain*n_s;

           n_f = nfbar;
           n_s = nsbar;
          
           if(calHighRange)
           {
            nfbar = range_offs*n_f/(n_f+n_s) + range_gain * n_f;
            nsbar = range_offs*n_s/(n_f+n_s) + range_gain * n_s;

            n_f = nfbar;
            n_s = nsbar;
           }
         }
      }
      time = rawHit->GetTime();
      
      //tot converted from tot-channels to 300MeV range channels
      
//      tempResult = fCalifaCalPar->GetDUCalParAt(rawHit_id)->GetToTCal_offset() + 	( TMath::Exp((rawHit->GetTot()+rando)/tau) * fCalifaCalPar->GetDUCalParAt(rawHit_id)->GetToTCal_gain()); 

	  tempResult = p->GetToTCal_par0()*(TMath::Exp((rawHit->GetTot()+ran->Rndm()-0.5)/p->GetToTCal_par1()))+p->GetToTCal_par2();
	  // E=par0*exp(TOT/par1)+par2, par1 is not constant..
	
      //tot converted from 300MeV range channels to 30MeV range channels
//      tempResult = p->GetRangeCal_offset() + 
//	( tempResult* p->GetRangeCal_gain()); 
      if(!calHighRange)
      {
        tempResult /= p->GetRangeCal_gain();
      }
	
      //tot converted from 30MeV range channels to energy (keV)
         if(rawHit->GetTot()>0)
      		{   
      tot_energy = p->GetGammaCal_offset() + 
      	(tempResult * p->GetGammaCal_gain());
      		}
	  else tot_energy = 0;
      new ((*fCrystalCalDataCA)[calHits++]) R3BCalifaCrystalCalData(crystal_id, energy, n_f, n_s, time, tot_energy);
      
    }  
  }
  //===========================================================================
  */
  
  return;
}


void R3BCalifaMapped2CrystalCal::Finish()
{
}


//Reset: Public method
void R3BCalifaMapped2CrystalCal::Reset()
{
  LOG(DEBUG) << "Clearing CrystalCalData Structure" << FairLogger::endl;
  if(fCrystalCalDataCA)fCrystalCalDataCA->Clear();
}


ClassImp(R3BCalifaMapped2CrystalCal)
