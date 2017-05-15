// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                R3BCalifaCrystalCal2CrystalCalPID                  -----
// -----                      Created by M. Winkel                         -----
// -----                Modified by P.Cabanelas 14/03/2017                 -----
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
#include <fstream>

//Califa headers
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaCrystalCal2CrystalCalPID.h"

using namespace std;

//R3BCalifaCrystalCal2CrystalCalPID: Constructor
R3BCalifaCrystalCal2CrystalCalPID::R3BCalifaCrystalCal2CrystalCalPID() : FairTask("R3B CALIFA QPID Calibrator"),
                              nEvents(0),
			      fCrystalCalDataCA(0) 
{
  //counter1=0;
  //counter2=0;
  
  ifstream fcal("qpidscale.txt");
  int ch = 0;
  int ch_file;
  double s, d;

  cout << "QPID calibration:" << endl << endl;

  while(fcal.good() && ch < 128)
  {
    fcal >> ch_file >> s >> d;
    if(ch != ch_file)
    {
      cerr << "Invalid channel: " << ch_file << " != " << ch << endl;
      continue;
    }
    scale[ch] = s;
    delta[ch++] = d;

    cout << dec << ch << " " << scientific << s << " " << d << endl;
  }
}

//Virtual R3BCalifaCrystalCal2CrystalCalPID: Public method
R3BCalifaCrystalCal2CrystalCalPID::~R3BCalifaCrystalCal2CrystalCalPID()
{
  LOG(INFO) << "R3BCalifaCrystalCal2CrystalCalPID: Delete instance" << FairLogger::endl;
  delete fCrystalCalDataCA;
}



//Init: Public method
InitStatus R3BCalifaCrystalCal2CrystalCalPID::Init()
{
  Register();
  return kSUCCESS;
}

void R3BCalifaCrystalCal2CrystalCalPID::SetParContainers()
{
//  // Get run and runtime database
//  FairRunAna* run = FairRunAna::Instance();
//  if (!run) Fatal("R3BCalifaCrystalCal2CrystalCalPID::SetParContainers", "No analysis run");
//  
//  FairRuntimeDb* rtdb = run->GetRuntimeDb();
//  if (!rtdb) Fatal("R3BCalifaCrystalCal2CrystalCalPID::SetParContainers", "No runtime database");
//  
//  if ( fVerbose && fCaloCalPar ) {
//    LOG(INFO) << "R3BCalifaCrystalCal2CrystalCalPID::SetParContainers() "<< FairLogger::endl;
//    LOG(INFO) << "Container R3BCalifaCrystalCal2CrystalCalPIDPar loaded " << FairLogger::endl;
//  }  
}


//Register: Protected method
void R3BCalifaCrystalCal2CrystalCalPID::Register()
{
  LOG(DEBUG) << "Registering" << FairLogger::endl;
  FairRootManager *fMan = FairRootManager::Instance();
  if(! fMan) { 
    Fatal("Init", "No FairRootManager");
    return;
  }
  fCrystalCalDataCA = (TClonesArray*) fMan->GetObject("CalifaCrystalCalData");
  if(NULL == fCrystalCalDataCA) {
    FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch CalifaCrystalCalData not found");
    }
}


// -----   Public method ReInit   --------------------------------------------
InitStatus R3BCalifaCrystalCal2CrystalCalPID::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

//DoCalib: Public method
void R3BCalifaCrystalCal2CrystalCalPID::Exec(Option_t* option)
{
  LOG(DEBUG) << "Calibrating CALIFA PID Data" << FairLogger::endl;
 
  if(++nEvents % 10000 == 0)
     LOG(INFO) << nEvents << FairLogger::endl;

  R3BCalifaCrystalCalData*  hit;
  int ch;
  double nf, ns, s, d;
  
  for(Int_t i = 0; i < fCrystalCalDataCA->GetEntries(); i++)
  {
    hit = dynamic_cast<R3BCalifaCrystalCalData*>(fCrystalCalDataCA->At(i));
    ch = hit->GetCrystalId();
    if(ch < 0 || ch >= 128)
    {
      cerr << "Invalid channel: " << ch << endl;
      continue;
    }
    nf = hit->GetNf(); ns = hit->GetNs();

    s = 1. + (scale[ch] - 1.)*(nf + ns)/.1;
    d = delta[ch]*(nf + ns)/.1;

    hit->SetNf(nf*(1.+s)/2. + ns*(1.-s)/2. - d);
    hit->SetNs(nf*(1.-s)/2. + ns*(1.+s)/2. + d);
  }
   
  return;
}


void R3BCalifaCrystalCal2CrystalCalPID::Finish()
{
}


//Reset: Public method
void R3BCalifaCrystalCal2CrystalCalPID::Reset()
{
  LOG(DEBUG) << "Clearing CalifaCrystalCal Data Structure" << FairLogger::endl;
  if(fCrystalCalDataCA)fCrystalCalDataCA->Clear();
}


ClassImp(R3BCalifaCrystalCal2CrystalCalPID)
