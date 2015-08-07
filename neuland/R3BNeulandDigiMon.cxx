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
   FairRootManager *rm = FairRootManager::Instance();

   fDigis = (TClonesArray *) rm->GetObject("LandDigi");

   // XYZ -> ZXY (side view)
   fh3 = new TH3D("hDigis", "hDigis", 60, 1400, 1700, 50, -125, 125, 50, -125, 125);
   fh3->SetTitle("NeuLAND Digis");
   fh3->GetXaxis()->SetTitle("Z");
   fh3->GetYaxis()->SetTitle("X");
   fh3->GetZaxis()->SetTitle("Y");

   rm->Register("NeulandDigiMon", "Digis in NeuLAND", fh3, kTRUE);

   return kSUCCESS;
}


void R3BNeulandDigiMon::Exec(Option_t *)
{
   fh3->Reset("ICES");
   R3BLandDigi *digi;
   const unsigned int nDigis = fDigis->GetEntries();
   for (unsigned int i = 0; i < nDigis; i++) {
      digi = (R3BLandDigi *) fDigis->At(i);
      // XYZ -> ZXY (side view)
      fh3->Fill(digi->GetZZ(), digi->GetXX(), digi->GetYY(), digi->GetQdc());
   }
}


void R3BNeulandDigiMon::Finish()
{}


ClassImp(R3BNeulandDigiMon)
