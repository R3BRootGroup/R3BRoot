// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                        R3BStarTrackRawAna                         -----
// -----                           Version 0.1                             -----
// -----                    Created 28.04.2014 by M. Labiche               -----
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

#include "R3BStarTrackRawHit.h"
#include "R3BStarTrackRawAna.h"


R3BStarTrackRawAna::R3BStarTrackRawAna()
{
  fnEvents = 0;
}


R3BStarTrackRawAna::~R3BStarTrackRawAna()
{
}


InitStatus R3BStarTrackRawAna::Init()
{
  FairRootManager *fMan = FairRootManager::Instance();
  fRawData = (TClonesArray*) fMan->GetObject("StarTrackRawHit");
  CreateHistos();

  return kSUCCESS;
}


void R3BStarTrackRawAna::Exec(Option_t *option)
{
  Int_t nHits = fRawData->GetEntries();
  //cout << "-I- R3BStarTrackRawAna::Exec : event " << fnEvents << ", multiplicity " << nHits << endl;
  R3BStarTrackRawHit *hit;
  for(Int_t i = 0; i < nHits; i++) {
    hit = (R3BStarTrackRawHit*) fRawData->At(i);
    thm->Fill(hit->GetModuleId());
    ths->Fill(hit->GetSide());
    tha->Fill(hit->GetAsicId());
    thm->Fill(hit->GetStripId());
    the->Fill(hit->GetADCdata());
    tht->Fill(hit->GetTime());
  }
  if(0 == (fnEvents%100)) {
    cout << fnEvents << "  " << nHits << endl;
  }
  fnEvents += 1;
}


void R3BStarTrackRawAna::FinishTask()
{
}


void R3BStarTrackRawAna::CreateHistos()
{
  thm = new TH1F("Module ID", "", 9, -0.5, 8.5);
  thsd = new TH1F("Side", "", 9, -0.5, 8.5);
  tha = new TH1F("Asic ID", "", 9, -0.5, 8.5);
  ths = new TH1F("Strip ID", "", 9, -0.5, 8.5);
  the = new TH1F("Energy", "", 30, 320., 350.);
  tht = new TH1F("Time", "", 100, 0., 4500000000.);
  FairRunOnline *run = FairRunOnline::Instance();
  run->AddObject(thm);
  run->AddObject(thsd);
  run->AddObject(tha);
  run->AddObject(ths);
  run->AddObject(the);
  run->AddObject(tht);
}


void R3BStarTrackRawAna::WriteHistos()
{
  thm->Write();
  thsd->Write();
  tha->Write();
  ths->Write();
  the->Write();
  tht->Write();
  LOG(INFO) << "R3BStarTrackRawAna --- Histograms writed to the Root File ..." << FairLogger::endl; 
}


ClassImp(R3BStarTrackRawAna)

