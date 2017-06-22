// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                    R3BCalifaCrystalCalDataAnalysis                -----
// -----                    Created 18/07/14  by H.Alvarez                 -----
// -----                   Modified 20/03/17  by P.Cabanelas               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"

#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaCrystalCalDataAnalysis.h"


R3BCalifaCrystalCalDataAnalysis::R3BCalifaCrystalCalDataAnalysis()
{
  fnEvents = 0;
}


R3BCalifaCrystalCalDataAnalysis::~R3BCalifaCrystalCalDataAnalysis()
{
}


InitStatus R3BCalifaCrystalCalDataAnalysis::Init()
{
  FairRootManager *fMan = FairRootManager::Instance();
  fCrystalData = (TClonesArray*) fMan->GetObject("CalifaCrystalCalData");
  CreateHistos();

  return kSUCCESS;
}


void R3BCalifaCrystalCalDataAnalysis::Exec(Option_t *option)
{
  Int_t nHits = fCrystalData->GetEntries();
  R3BCalifaCrystalCalData *hit;
  for(Int_t i = 0; i < nHits; i++) {
    hit = (R3BCalifaCrystalCalData*) fCrystalData->At(i);
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


void R3BCalifaCrystalCalDataAnalysis::FinishTask()
{
}


void R3BCalifaCrystalCalDataAnalysis::CreateHistos()
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


void R3BCalifaCrystalCalDataAnalysis::WriteHistos()
{
  thCrystalID->Write();
  thEnergy->Write();
  thNf->Write();
  thNs->Write();
  thTime->Write();
  LOG(INFO) << "R3BCalifaCrystalCalDataAnalysis --- Histograms written to Root File ..." << FairLogger::endl; 
}


ClassImp(R3BCalifaCrystalCalDataAnalysis)

