#include "R3BBunchedFiberMapped2CalPar.h"
#include <cassert>
#include "TClonesArray.h"
#include "FairLogger.h"
#include "FairRuntimeDb.h"
#include "R3BBunchedFiberMappedData.h"
#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"

R3BBunchedFiberMapped2CalPar::R3BBunchedFiberMapped2CalPar(const char *a_name,
    Int_t a_verbose, enum Electronics a_spmt_electronics, Int_t a_update_rate,
    Int_t a_min_stats)
  : FairTask(TString("R3B") + a_name + "Mapped2CalPar", a_verbose)
  , fName(a_name)
  , fSPMTElectronics(a_spmt_electronics)
  , fUpdateRate(a_update_rate)
  , fMinStats(a_min_stats)
{
}

R3BBunchedFiberMapped2CalPar::~R3BBunchedFiberMapped2CalPar()
{
  delete fMAPMTTCalPar;
  delete fMAPMTEngine;
  delete fSPMTTCalPar;
  delete fSPMTEngine;
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
#define GET_TCALPAR(NAME) do {\
  auto name = fName + #NAME"TCalPar";\
  f##NAME##TCalPar = (R3BTCalPar *)FairRuntimeDb::instance()->getContainer(name);\
  if (!f##NAME##TCalPar) {\
    LOG(ERROR) << "Could not get " << name << '.' << FairLogger::endl;\
    abort();\
    return kFATAL;\
  }\
  f##NAME##TCalPar->setChanged();\
  f##NAME##Engine = new R3BTCalEngine(f##NAME##TCalPar, fMinStats);\
} while (0)
  GET_TCALPAR(MAPMT);
  GET_TCALPAR(SPMT);

  return kSUCCESS;
}

void R3BBunchedFiberMapped2CalPar::Exec(Option_t *option)
{
  auto mapped_num = fMapped->GetEntriesFast();
  for (auto i = 0; i < mapped_num; i++) {
    auto mapped = (R3BBunchedFiberMappedData *)fMapped->At(i);
    assert(mapped);
    if (mapped->IsMAPMT()) {
      fMAPMTEngine->Fill(1,
          mapped->GetChannel() * 2 - (mapped->IsLeading() ? 1 : 0),
          1,
          mapped->GetFine());
    } else {
      fSPMTEngine->Fill(1,
          mapped->GetChannel() * 2 - (mapped->IsLeading() ? 1 : 0),
          1,
          mapped->GetFine());
    }
  }
}

void R3BBunchedFiberMapped2CalPar::FinishEvent()
{
}

void R3BBunchedFiberMapped2CalPar::FinishTask()
{
  fMAPMTEngine->CalculateParamClockTDC();
  switch (fSPMTElectronics) {
    case CTDC:
      fSPMTEngine->CalculateParamClockTDC();
      break;
    case TAMEX:
      fSPMTEngine->CalculateParamVFTX();
      break;
    default:
      assert(0 && "This should not happen!");
  }
  fMAPMTTCalPar->printParams();
  fSPMTTCalPar->printParams();
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
