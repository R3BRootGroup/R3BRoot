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
    thw->Fill(hit->GetWordType());
    thh->Fill(hit->GetHitBit());
    thm->Fill(hit->GetModuleId());
    thsd->Fill(hit->GetSide());
    tha->Fill(hit->GetAsicId());
    thst->Fill(hit->GetStripId());
    the->Fill(hit->GetADCdata());
    tht->Fill(hit->GetTime());
    thif->Fill(hit->GetInfoField());
    thic->Fill(hit->GetInfoCode());
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
  thw = new TH1F("Word type 10->2 11->3", "", 20, -0.5, 20.5);
  thh = new TH1F("hit value", "", 9, -0.5, 8.5);
  thm = new TH1F("Module ID", "", 30, -0.5, 30.5);
  thsd = new TH1F("Side", "", 9, -0.5, 8.5);
  tha = new TH1F("Asic ID", "", 17, -0.5, 16.5);
  thst = new TH1F("Strip ID", "", 131, -0.5, 130.5);
  the = new TH1F("Energy", "", 50, 0., 500.);
  tht = new TH1F("Time", "", 1000, 0., 450000000.);
  thif = new TH1F("InfoField", "", 21, 0., 20.);
  thic = new TH1F("InfoCode", "", 21, 0., 20.);
  FairRunOnline *run = FairRunOnline::Instance();
  run->AddObject(thw);
  run->AddObject(thh);
  run->AddObject(thm);
  run->AddObject(thsd);
  run->AddObject(tha);
  run->AddObject(thst);
  run->AddObject(the);
  run->AddObject(tht);
  run->AddObject(thif);
  run->AddObject(thic);
}


void R3BStarTrackRawAna::WriteHistos()
{
  thw->Write();
  thh->Write();
  thm->Write();
  thsd->Write();
  tha->Write();
  thst->Write();
  the->Write();
  tht->Write();
  thif->Write();
  thic->Write();
  LOG(INFO) << "R3BStarTrackRawAna --- Histograms writed to the Root File ..." << FairLogger::endl; 
}


ClassImp(R3BStarTrackRawAna)

