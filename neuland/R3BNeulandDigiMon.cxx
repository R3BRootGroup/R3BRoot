#include "R3BNeulandDigiMon.h"

#include <iostream>

#include "TClonesArray.h"
#include "TH3D.h"

#include "FairRootManager.h"

#include "R3BLandDigi.h"


R3BNeulandDigiMon::R3BNeulandDigiMon() : FairTask("R3B NeuLAND NeulandDigi Monitor")
{}


R3BNeulandDigiMon::~R3BNeulandDigiMon()
{}


InitStatus R3BNeulandDigiMon::Init()
{
  FairRootManager* frm = FairRootManager::Instance();

  fArrayDigi = (TClonesArray*) frm->GetObject("LandDigi");

  // XYZ -> ZXY (side view)
  h3 = new TH3D("hDigis", "hDigis", 60, 1400, 1700, 50, -125, 125, 50, -125, 125);
  h3->SetTitle("NeuLAND Digis");
  h3->GetXaxis()->SetTitle("Z");
  h3->GetYaxis()->SetTitle("X");
  h3->GetZaxis()->SetTitle("Y");

  frm->Register("NeulandDigiMon", "Digis in NeuLAND", h3, kTRUE);

  return kSUCCESS;
}


void R3BNeulandDigiMon::Exec(Option_t* option)
{
  h3->Reset("ICES");
  R3BLandDigi* digi;
  const unsigned int nDigis = fArrayDigi->GetEntries();
  for (unsigned int i = 0; i < nDigis; i++) {
    digi = (R3BLandDigi*) fArrayDigi->At(i);
    // XYZ -> ZXY (side view)
    h3->Fill( digi->GetZZ(), digi->GetXX(), digi->GetYY(), digi->GetQdc() );
  }
}


void R3BNeulandDigiMon::Finish()
{}


ClassImp(R3BNeulandDigiMon);
