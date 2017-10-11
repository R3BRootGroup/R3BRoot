// -----------------------------------------------------------------------------
// -----                       R3BStraTraAnaStrip                          -----
// -----                      From R3BCaloCrystalAna                       -----
// -----                  Created 18/07/2014 by H. Alvarez Pol             -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"

#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

#include "R3BStartrackerDigitHit.h"
#include "R3BStartrackStripAna.h"


R3BStartrackStripAna::R3BStartrackStripAna()
{
  fnEvents = 0;
}


R3BStartrackStripAna::~R3BStartrackStripAna()
{
}


InitStatus R3BStartrackStripAna::Init()
{
  FairRootManager *fMan = FairRootManager::Instance();
  fSiDetData = (TClonesArray*) fMan->GetObject("StartrackStripHit");
  CreateHistos();

  return kSUCCESS;
}


void R3BStartrackStripAna::Exec(Option_t *option)
{
  Int_t nHits = fSiDetData->GetEntries();
  R3BStartrackerDigitHit *hit;
  for(Int_t i = 0; i < nHits; i++) {
    hit = (R3BStartrackerDigitHit*) fSiDetData->At(i);

    //thWordType->Fill(hit->GetWordType());  // 10->type A  ; 11-> 
    //thHitBit->Fill(hit->GetHitBit());  // 0->timestamp from energy branch (ie over energy threshold) ; 1->timestamp from fast signal (ie over time threshold)
    thModuleID->Fill(hit->GetDetector());
    thSide->Fill(hit->GetSide());
    thAsicID->Fill(hit->GetChip());
    thStripID->Fill(hit->GetStrip());
    thEnergy->Fill(hit->GetEnergy());
    thTime->Fill(hit->GetTime());
  }
  if(0 == (fnEvents%100)) {
    LOG(INFO) << fnEvents << " events, multiplicity:  " << nHits << FairLogger::endl;
  }
  fnEvents += 1;
}


void R3BStartrackStripAna::FinishTask()
{
}


void R3BStartrackStripAna::CreateHistos()
{
  //thWordType = new TH1F("Word type", "", 30, -0.5, 30.5);
  //thHitBit = new TH1F("Hit Bit value", "", 10, -0.5, 10.5);
  thModuleID = new TH1F("Module ID", "", 30, -0.5, 30.5);
  thSide = new TH1F("Side ID", "", 10, -0.5, 9.5);
  thAsicID = new TH1F("Asic ID", "", 32, -0.5, 32.5);
  thStripID = new TH1F("Strip ID", "", 128, -0.5, 128.5);
  thEnergy = new TH1F("Energy", "", 100, 0., 0.1);
  thTime = new TH1F("Time", "", 100, 0., 4500000000.);
  FairRunOnline *run = FairRunOnline::Instance();
  //run->AddObject(thWordType);
  //run->AddObject(thHitbit);
  run->AddObject(thModuleID);
  run->AddObject(thSide);
  run->AddObject(thAsicID);
  run->AddObject(thStripID);
  run->AddObject(thEnergy);
  run->AddObject(thTime);
}


void R3BStartrackStripAna::WriteHistos()
{
  //thWordType->Write();
  //thHitBit->Write();
  thModuleID->Write();
  thSide->Write();
  thAsicID->Write();
  thStripID->Write();
  thEnergy->Write();
  thTime->Write();
  LOG(INFO) << "R3BStartrackStripAna --- Histograms written to Root File ..." << FairLogger::endl; 
}


ClassImp(R3BStartrackStripAna)

