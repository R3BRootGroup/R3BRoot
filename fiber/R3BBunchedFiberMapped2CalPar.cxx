#include "R3BBunchedFiberMapped2CalPar.h"
#include <cassert>
#include "TClonesArray.h"
#include "FairLogger.h"
#include "FairRuntimeDb.h"
#include "R3BBunchedFiberMappedData.h"
#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"

R3BBunchedFiberMapped2CalPar::R3BBunchedFiberMapped2CalPar(const char *a_name,
    Int_t a_verbose, Int_t a_update_rate, Int_t a_min_stats)
  : FairTask(TString("R3B") + a_name + "Mapped2CalPar", a_verbose)
  , fName(a_name)
  , fUpdateRate(a_update_rate)
  , fMinStats(a_min_stats)
{
}

R3BBunchedFiberMapped2CalPar::~R3BBunchedFiberMapped2CalPar()
{
  delete fTCalPar;
  delete fEngine;
}

InitStatus R3BBunchedFiberMapped2CalPar::Init()
{
  auto rm = FairRootManager::Instance();
  if (!rm) {
    return kFATAL;
  }

  fMapped = (TClonesArray *)rm->GetObject(fName + "Mapped");
  if (!fMapped) {
    return kFATAL;
  }

  // container needs to be created in tcal/R3BTCalContFact.cxx AND R3BTCal needs
  // to be set as dependency in CMakeLists.txt in the detector directory.
  fTCalPar = (R3BTCalPar *)FairRuntimeDb::instance()->getContainer(fName + "TCalPar");
  if (!fTCalPar) {
    LOG(ERROR) << "Could not get " << fName << "TCalPar." << FairLogger::endl;
    abort();
    return kFATAL;
  }
  fTCalPar->setChanged();
  fEngine = new R3BTCalEngine(fTCalPar, fMinStats);

  return kSUCCESS;
}

void R3BBunchedFiberMapped2CalPar::Exec(Option_t *option)
{
  auto mapped_num = fMapped->GetEntriesFast();
  for (auto i = 0; i < mapped_num; i++) {
    auto mapped = (R3BBunchedFiberMappedData *)fMapped->At(i);
    assert(mapped);
    fEngine->Fill(1,
        mapped->GetChannel() * 2 - (mapped->IsLeading() ? 1 : 0),
        mapped->IsMAPMT() ? 1 : 2,
        mapped->GetFine());
  }
}

void R3BBunchedFiberMapped2CalPar::FinishEvent()
{
}

void R3BBunchedFiberMapped2CalPar::FinishTask()
{
  fEngine->CalculateParamClockTDC();
  fTCalPar->printParams();
}

void R3BBunchedFiberMapped2CalPar::SetUpdateRate(Int_t a_rate)
{
  fUpdateRate = a_rate;
}

void R3BBunchedFiberMapped2CalPar::SetMinStats(Int_t a_min_stats)
{
  fMinStats = a_min_stats;
}

ClassImp(R3BBunchedFiberMapped2CalPar)
