// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BCaloPIDCal                           -----
// -----                 Created by M. Winkel                              -----
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
#include "R3BCaloCrystalHit.h"
#include "R3BCaloPIDCal.h"

using namespace std;

//R3BCaloPIDCal: Constructor
R3BCaloPIDCal::R3BCaloPIDCal() : FairTask("R3B CALIFA QPID Calibrator"),
                              nEvents(0),
			       fCrystalHitCA(0) 
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

//Virtual R3BCaloPIDCal: Public method
R3BCaloPIDCal::~R3BCaloPIDCal()
{
  LOG(INFO) << "R3BCaloPIDCal: Delete instance" << FairLogger::endl;
  delete fCrystalHitCA;
}



//Init: Public method
InitStatus R3BCaloPIDCal::Init()
{
  Register();
  return kSUCCESS;
}

void R3BCaloPIDCal::SetParContainers()
{
//  // Get run and runtime database
//  FairRunAna* run = FairRunAna::Instance();
//  if (!run) Fatal("R3BCaloPIDCal::SetParContainers", "No analysis run");
//  
//  FairRuntimeDb* rtdb = run->GetRuntimeDb();
//  if (!rtdb) Fatal("R3BCaloPIDCal::SetParContainers", "No runtime database");
//  
//  if ( fVerbose && fCaloCalPar ) {
//    LOG(INFO) << "R3BCaloPIDCal::SetParContainers() "<< FairLogger::endl;
//    LOG(INFO) << "Container R3BCaloPIDCalPar loaded " << FairLogger::endl;
//  }  
}


//Register: Protected method
void R3BCaloPIDCal::Register()
{
  LOG(DEBUG) << "Registering" << FairLogger::endl;
  FairRootManager *fMan = FairRootManager::Instance();
  if(! fMan) { 
    Fatal("Init", "No FairRootManager");
    return;
  }
  fCrystalHitCA = (TClonesArray*) fMan->GetObject("CaloCrystalHit");
  if(NULL == fCrystalHitCA) {
    FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch CaloCrystalHit not found");
    }
}


// -----   Public method ReInit   --------------------------------------------
InitStatus R3BCaloPIDCal::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

//DoCalib: Public method
void R3BCaloPIDCal::Exec(Option_t* option)
{
  LOG(DEBUG) << "Calibrating CALIFA PID Data" << FairLogger::endl;
 
  if(++nEvents % 10000 == 0)
     LOG(INFO) << nEvents << FairLogger::endl;

  R3BCaloCrystalHit*  hit;
  int ch;
  double nf, ns, s, d;
  
  for(Int_t i = 0; i < fCrystalHitCA->GetEntries(); i++)
  {
    hit = dynamic_cast<R3BCaloCrystalHit*>(fCrystalHitCA->At(i));
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


void R3BCaloPIDCal::Finish()
{
}


//Reset: Public method
void R3BCaloPIDCal::Reset()
{
  LOG(DEBUG) << "Clearing CrystalHit Data Structure" << FairLogger::endl;
  if(fCrystalHitCA)fCrystalHitCA->Clear();
}


ClassImp(R3BCaloPIDCal)
