#include "R3BStrawtubesCal2Hit.h"
#include "R3BStrawtubesCalData.h"
#include "R3BStrawtubesHitData.h"
#include "FairLogger.h"
#include "TClonesArray.h"

R3BStrawtubesCal2Hit::R3BStrawtubesCal2Hit(const char* name, Int_t iVerbose):
  FairTask(name, iVerbose),
  fCalItems(),
  fHitItems(new TClonesArray("R3BStrawtubesHitData")),
  fNofHitItems(0)
{
}

R3BStrawtubesCal2Hit::~R3BStrawtubesCal2Hit()
{
  if (fHitItems) {
    delete fHitItems;
  }
}

void R3BStrawtubesCal2Hit::Exec(Option_t* option)
{
  Int_t nDets = fCalItems->GetEntriesFast();
  for (Int_t i = 0; i < nDets; i++) {
    auto calItem = (R3BStrawtubesCalData *)fCalItems->At(i);
    if (!calItem) {
      continue;
    }
    new ((*fHitItems)[fNofHitItems]) R3BStrawtubesHitData(*calItem);
    ++fNofHitItems;
  }
}

void R3BStrawtubesCal2Hit::FinishEvent()
{
  if (fHitItems) {
    fHitItems->Clear();
  }
  fNofHitItems = 0;
}

void R3BStrawtubesCal2Hit::FinishTask()
{
}

InitStatus R3BStrawtubesCal2Hit::Init()
{
  FairRootManager* mgr = FairRootManager::Instance();
  if (!mgr) {
    FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN,
        "FairRootManager not found");
  }
  fCalItems = (TClonesArray *)mgr->GetObject("StrawtubesCal");
  if (!fCalItems) {
    FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN,
        "Branch StrawtubesCal not found");
  }

  mgr->Register("StrawtubesHit", "Land", fHitItems, kTRUE);

  return kSUCCESS;
}

InitStatus R3BStrawtubesCal2Hit::ReInit()
{
  return kSUCCESS;
}

ClassImp(R3BStrawtubesCal2Hit)
