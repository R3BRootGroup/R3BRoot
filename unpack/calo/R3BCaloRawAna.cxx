// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                          R3BCaloRawAna                            -----
// -----                           Version 0.1                             -----
// -----                    Created 03.02.2014 by Y.Gonzalez               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include <iostream>
using namespace std;

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"

#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

#include "R3BCaloRawHit.h"
#include "R3BCaloRawAna.h"


R3BCaloRawAna::R3BCaloRawAna()
{
  fnEvents = 0;
}


R3BCaloRawAna::~R3BCaloRawAna()
{
}


InitStatus R3BCaloRawAna::Init()
{
  FairRootManager *fMan = FairRootManager::Instance();
  fRawData = (TClonesArray*) fMan->GetObject("CaloRawHit");
  CreateHistos();

  return kSUCCESS;
}


void R3BCaloRawAna::Exec(Option_t *option)
{
  Int_t nHits = fRawData->GetEntries();
  //cout << "-I- R3BCaloRawAna::Exec : event " << fnEvents << ", multiplicity " << nHits << endl;
  R3BCaloRawHit *hit;
  for(Int_t i = 0; i < nHits; i++) {
    hit = (R3BCaloRawHit*) fRawData->At(i);
    thc->Fill(hit->GetCrystalId());
    the->Fill(hit->GetEnergy());
    tht->Fill(hit->GetTime());
  }
  if(0 == (fnEvents%100)) {
    cout << fnEvents << "  " << nHits << endl;
  }
  fnEvents += 1;
}


void R3BCaloRawAna::FinishTask()
{
}


void R3BCaloRawAna::CreateHistos()
{
  thc = new TH1F("Crystal ID", "", 9, -0.5, 8.5);
  the = new TH1F("Energy", "", 30, 320., 350.);
  tht = new TH1F("Time", "", 100, 0., 4500000000.);
  FairRunOnline *run = FairRunOnline::Instance();
  run->AddObject(thc);
  run->AddObject(the);
  run->AddObject(tht);
}


void R3BCaloRawAna::WriteHistos()
{
  thc->Write();
  the->Write();
  tht->Write();
  LOG(INFO) << "R3BCaloRawAna --- Histograms writed to the Root File ..." << FairLogger::endl; 
}


ClassImp(R3BCaloRawAna)

