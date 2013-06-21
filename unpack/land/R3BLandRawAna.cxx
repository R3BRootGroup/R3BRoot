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
  Int_t nHits = fRawData->GetEntriesFast();
  cout << "-I- R3BLandRawAna::Exec : event " << fnEvents << ", multiplicity " << nHits << endl;
  R3BLandRawHit *hit;
  for(Int_t i = 0; i < nHits; i++) {
    hit = (R3BLandRawHit*) fRawData->At(i);
    fh_padId->Fill(hit->GetPadId());
    fh_qdc_padId->Fill(hit->GetPadId(), hit->GetQdc1());
  }
  
  fnEvents += 1;
}


void R3BLandRawAna::FinishTask()
{
}


void R3BLandRawAna::CreateHistos()
{
  fh_padId = new TH1F("h_padId", "", 500, -0.5, 499.5);

  fh_qdc_padId = new TH2F("h_qdc_padId", "",
			  50, 0., 500.,
			  20, 0., 4000.);

  FairRunOnline *run = FairRunOnline::Instance();

  run->AddObject(fh_padId);
  run->AddObject(fh_qdc_padId);
}


void R3BLandRawAna::WriteHistos()
{
  fh_padId->Write();
  fh_qdc_padId->Write();
}


ClassImp(R3BLandRawAna)

