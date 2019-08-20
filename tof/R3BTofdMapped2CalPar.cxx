// ----------------------------------------------------------------
// -----            R3BTofdMapped2CalPar                     -----
// -----           Created Apr 2016 by R.Plag                 -----
// ----------------------------------------------------------------

#include "R3BTofdMapped2CalPar.h"

#include "TClonesArray.h"

#include "FairLogger.h"
#include "FairRuntimeDb.h"

#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"
#include "R3BTofdMappedData.h"

R3BTofdMapped2CalPar::R3BTofdMapped2CalPar()
  : FairTask("R3BTofdMapped2CalPar", 1)
  , fUpdateRate(1000000)
  , fMinStats(100000)
  , fNofPlanes(0)
  , fPaddlesPerPlane(0)
  , fNofModules(0)
  , fCalPar(nullptr)
  , fMapped(nullptr)
  , fEngine(nullptr)
{
}

R3BTofdMapped2CalPar::R3BTofdMapped2CalPar(const char *name, Int_t iVerbose)
  : FairTask(name, iVerbose)
  , fUpdateRate(1000000)
  , fMinStats(100000)
  , fNofPlanes(0)
  , fPaddlesPerPlane(0)
  , fNofModules(0)
  , fCalPar(nullptr)
  , fMapped(nullptr)
  , fEngine(nullptr)
{
}

R3BTofdMapped2CalPar::~R3BTofdMapped2CalPar()
{
  delete fCalPar;
  delete fEngine;
}

InitStatus R3BTofdMapped2CalPar::Init()
{
  FairRootManager* rm = FairRootManager::Instance();
  if (!rm) {
    return kFATAL;
  }

  fMapped = (TClonesArray *)rm->GetObject("TofdMapped");
  if (!fMapped) {
    return kFATAL;
  }

  // container needs to be created in tcal/R3BTCalContFact.cxx AND R3BTCal needs
  // to be set as dependency in CMakelists.txt (in this case in the tof directory)
  fCalPar = (R3BTCalPar *)FairRuntimeDb::instance()->getContainer("TofdTCalPar");
  if (!fCalPar) {
    LOG(ERROR) << "R3BTofdMapped2CalPar::Init() Couldn't get handle on TofdTCalPar. ";
    return kFATAL;
  }

  fCalPar->setChanged();

  if (!fNofModules) {
    LOG(ERROR) << "R3BTofdMapped2CalPar::Init() Number of modules not set. ";
    return kFATAL;
  }

  fEngine = new R3BTCalEngine(fCalPar, fMinStats);

  return kSUCCESS;
}

void R3BTofdMapped2CalPar::Exec(Option_t *option)
{
  Int_t nHits = fMapped->GetEntries();

  // Loop over mapped hits
  for (Int_t i = 0; i < nHits; i++) {
    auto mapped = (R3BTofdMappedData const *)fMapped->At(i);

    if (mapped->GetDetectorId()>fNofPlanes) {
      LOG(ERROR) << "R3BTofdMapped2CalPar::Exec() : more planes than expected! Plane: " << mapped->GetDetectorId() << " allowed are 1.." << fNofPlanes;
      continue;
    }
    if (mapped->GetBarId()>fPaddlesPerPlane) {
      LOG(ERROR) << "R3BTofdMapped2CalPar::Exec() : more bars then expected! Det: " << mapped->GetBarId() << " allowed are 1.." << fPaddlesPerPlane;
      continue;
    }

    Int_t edge = mapped->GetSideId() * 2 + mapped->GetEdgeId() - 2; // 1..4
    fEngine->Fill(mapped->GetDetectorId(), mapped->GetBarId(), edge, mapped->GetTimeFine());
  }
}

void R3BTofdMapped2CalPar::FinishEvent()
{
}

void R3BTofdMapped2CalPar::FinishTask()
{
  fEngine->CalculateParamVFTX();
  fCalPar->printParams();
}

void R3BTofdMapped2CalPar::SetUpdateRate(Int_t rate)
{
    fUpdateRate = rate;
}

void R3BTofdMapped2CalPar::SetMinStats(Int_t minStats)
{
  fMinStats = minStats;
}

void R3BTofdMapped2CalPar::SetNofModules(Int_t nDets, Int_t nCh)
{
  fNofPlanes = nDets;		
  fPaddlesPerPlane  = nCh;
  fNofModules   = nDets * nCh * 4;
}

ClassImp(R3BTofdMapped2CalPar)
