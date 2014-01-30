// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                            R3BLandRawAna                          -----
// -----                    Created 23.04.2013 by D.Kresan                 -----
// -----------------------------------------------------------------------------

#include <iostream>
using namespace std;

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"

#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

#include "R3BLandRawHit.h"
#include "R3BLandRawAna.h"


R3BLandRawAna::R3BLandRawAna()
{
  fnEvents = 0;
}


R3BLandRawAna::~R3BLandRawAna()
{
}


InitStatus R3BLandRawAna::Init()
{
  FairRootManager *fMan = FairRootManager::Instance();
  fRawData = (TClonesArray*) fMan->GetObject("LandRawHit");
  CreateHistos();

  return kSUCCESS;
}


void R3BLandRawAna::Exec(Option_t *option)
{
  Int_t nHits = fRawData->GetEntries();
  //cout << "-I- R3BLandRawAna::Exec : event " << fnEvents << ", multiplicity " << nHits << endl;
  R3BLandRawHit *hit;
  for(Int_t i = 0; i < nHits; i++) {
    hit = (R3BLandRawHit*) fRawData->At(i);
    thch->Fill(hit->GetQdcData());
  }
  thmul->Fill(nHits);
  if(0 == (fnEvents%100)) {
    cout << fnEvents << "  " << nHits << endl;
  }
  fnEvents += 1;
}


void R3BLandRawAna::FinishTask()
{
}


void R3BLandRawAna::CreateHistos()
{
  thmul = new TH1F("Multiplicity", "", 400, -0.5, 399.5);
  thch = new TH1F("Charge", "", 1500, 0., 1500.);
  FairRunOnline *run = FairRunOnline::Instance();
  run->AddObject(thmul);
  run->AddObject(thch);
}


void R3BLandRawAna::WriteHistos()
{
  thmul->Write();
  thch->Write();
  LOG(INFO) << "R3BLandRawAna --- Histograms writed to the Root File ..." << FairLogger::endl; 
}


ClassImp(R3BLandRawAna)

