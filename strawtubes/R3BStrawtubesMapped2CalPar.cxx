#include "R3BStrawtubesMapped2CalPar.h"
#include "TClonesArray.h"
#include "FairLogger.h"
#include "FairRuntimeDb.h"
#include "R3BTCalPar.h"
#include "R3BTCalEngine.h"
#include "R3BStrawtubesMappedData.h"

R3BStrawtubesMapped2CalPar::R3BStrawtubesMapped2CalPar(const char* name, Int_t
    iVerbose):
  FairTask(name, iVerbose),
  fMinStats(1e5),
  fCalPar(),
  fEngine(),
  fMapped()
{
}

R3BStrawtubesMapped2CalPar::~R3BStrawtubesMapped2CalPar()
{
  if (fEngine) {
    delete fEngine;
  }
  if (fCalPar) {
    delete fCalPar;
  }
}

InitStatus R3BStrawtubesMapped2CalPar::Init()
{
  FairRootManager* rm = FairRootManager::Instance();
  if (!rm) {
    return kFATAL;
  }

  fMapped = (TClonesArray *)rm->GetObject("StrawtubesMapped");
  if (!fMapped) {
    return kFATAL;
  }

  fCalPar = (R3BTCalPar *)
      FairRuntimeDb::instance()->getContainer("StrawtubesTCalPar");
  fCalPar->setChanged();

  fEngine = new R3BTCalEngine(fCalPar, fMinStats);

  return kSUCCESS;
}

void R3BStrawtubesMapped2CalPar::Exec(Option_t* option)
{
  Int_t mapped_num = fMapped->GetEntries();
  for (Int_t i = 0; i < mapped_num; i++) {
    auto mapped = (R3BStrawtubesMappedData *)fMapped->At(i);
    if (!mapped) {
      LOG(ERROR) << "R3BStrawtubesMapped2CalPar::Exec(): What is this crap?"
          << FairLogger::endl;
      continue;
    }

    fEngine->Fill(mapped->GetPlane(), mapped->GetStraw(), mapped->GetSide(),
        mapped->GetTimeFine());
  }
}

void R3BStrawtubesMapped2CalPar::FinishEvent()
{
}

void R3BStrawtubesMapped2CalPar::FinishTask()
{
  fEngine->CalculateParamVFTX();
  fCalPar->printParams();
}

void R3BStrawtubesMapped2CalPar::SetMinStats(Int_t min_stats)
{
  fMinStats = min_stats;
}

ClassImp(R3BStrawtubesMapped2CalPar)
