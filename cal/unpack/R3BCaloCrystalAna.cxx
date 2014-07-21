// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                        R3BCaloCrystalAna                          -----
// -----                  Created 18/07/2014 by H. Alvarez Pol             -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"

#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

#include "R3BCaloCrystalHit.h"
#include "R3BCaloCrystalAna.h"


R3BCaloCrystalAna::R3BCaloCrystalAna()
{
  fnEvents = 0;
}


R3BCaloCrystalAna::~R3BCaloCrystalAna()
{
}


InitStatus R3BCaloCrystalAna::Init()
{
  FairRootManager *fMan = FairRootManager::Instance();
  fCrystalData = (TClonesArray*) fMan->GetObject("CaloCrystalHit");
  CreateHistos();

  return kSUCCESS;
}


void R3BCaloCrystalAna::Exec(Option_t *option)
{
  Int_t nHits = fCrystalData->GetEntries();
  R3BCaloCrystalHit *hit;
  for(Int_t i = 0; i < nHits; i++) {
    hit = (R3BCaloCrystalHit*) fCrystalData->At(i);
    thCrystalID->Fill(hit->GetCrystalId());
    thEnergy->Fill(hit->GetEnergy());
    thNf->Fill(hit->GetNf());
    thNs->Fill(hit->GetNs());
    thTime->Fill(hit->GetTime());
  }
  if(0 == (fnEvents%100)) {
    LOG(INFO) << fnEvents << " events, multiplicity:  " << nHits << FairLogger::endl;
  }
  fnEvents += 1;
}


void R3BCaloCrystalAna::FinishTask()
{
}


void R3BCaloCrystalAna::CreateHistos()
{
  thCrystalID = new TH1F("Crystal ID", "", 200, -0.5, 1000.5);
  thEnergy = new TH1F("Energy", "", 100, 0., 0.1);
  thNf = new TH1F("Nf", "", 100, 0., 0.1);
  thNs = new TH1F("Ns", "", 100, 0., 0.1);
  thTime = new TH1F("Time", "", 100, 0., 4500000000.);
  FairRunOnline *run = FairRunOnline::Instance();
  run->AddObject(thCrystalID);
  run->AddObject(thEnergy);
  run->AddObject(thNf);
  run->AddObject(thNs);
  run->AddObject(thTime);
}


void R3BCaloCrystalAna::WriteHistos()
{
  thCrystalID->Write();
  thEnergy->Write();
  thNf->Write();
  thNs->Write();
  thTime->Write();
  LOG(INFO) << "R3BCaloCrystalAna --- Histograms written to Root File ..." << FairLogger::endl; 
}


ClassImp(R3BCaloCrystalAna)

