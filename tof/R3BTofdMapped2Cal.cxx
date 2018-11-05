// ------------------------------------------------------------
// -----                  R3BTofdMapped2Cal                -----
// -----          Created Feb 4th 2016 by R.Plag          -----
// ------------------------------------------------------------

/*
 * March 2016
 * Reconstructing data into PaddleItems. Each paddle holds two times,
 * (for PM1 and PM2). This allows to plot
 * the time differences e.g. via evt->Draw(...) interactively (aka without
 * looping over all channels) which is crucial for a quick check of the
 * detector status during the experiment.
 */

#include <assert.h>

#include "R3BTofdMapped2Cal.h"

#include "TClonesArray.h"
#include "TMath.h"

#include "FairLogger.h"
#include "FairRuntimeDb.h"

#include "R3BTCalEngine.h"
#include "R3BTofdCalData.h"
#include "R3BTofdMappedData.h"

#define IS_NAN(x) TMath::IsNaN(x)

namespace {
  double const c_acceptance_ns = 200;
}

R3BTofdMapped2Cal::R3BTofdMapped2Cal()
  : FairTask("TofdTcal", 1)
  , fMappedItems(nullptr)
  , fCalItems(new TClonesArray("R3BTofdCalData"))
  , fNofCalItems(0)
  , fTcalPar(0)
  , fNofTcalPars(0)
  , fNofPlanes(0)
  , fPaddlesPerPlane(0)
  , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
  , fCalLookup()
{
}

  R3BTofdMapped2Cal::R3BTofdMapped2Cal(const char *name, Int_t iVerbose)
  : FairTask(name, iVerbose)
  , fMappedItems(nullptr)
  , fCalItems(new TClonesArray("R3BTofdCalData"))
  , fNofCalItems(0)
  , fTcalPar(0)
  , fNofTcalPars(0)
  , fNofPlanes(0)
  , fPaddlesPerPlane(0)
  , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
  , fCalLookup()
{
}

R3BTofdMapped2Cal::~R3BTofdMapped2Cal()
{
  delete fCalItems;
}

size_t R3BTofdMapped2Cal::GetCalLookupIndex(R3BTofdMappedData const &a_mapped)
  const
{
  size_t i = (a_mapped.GetDetectorId() - 1) * fPaddlesPerPlane +
      (a_mapped.GetBarId() - 1);
  assert(i < fCalLookup.size());
  return i;
}

InitStatus R3BTofdMapped2Cal::Init()
{
  fNofTcalPars = fTcalPar->GetNumModulePar();
  if (fNofTcalPars == 0) {
    LOG(ERROR) << "There are no TCal parameters in container TofdTCalPar" << FairLogger::endl;
    return kFATAL;
  }

  LOG(INFO) << "R3BTofdMapped2Cal::Init : read " << fNofTcalPars << " modules" << FairLogger::endl;

  // try to get a handle on the EventHeader. EventHeader may not be
  // present though and hence may be null. Take care when using.
  FairRootManager *mgr = FairRootManager::Instance();
  if (NULL == mgr)
    LOG(fatal) << "FairRootManager not found";

  // get access to Mapped data
  fMappedItems = (TClonesArray *)mgr->GetObject("TofdMapped");
  if (NULL == fMappedItems)
    LOG(fatal) << "Branch TofdMapped not found";

  // request storage of Cal data in output tree
  mgr->Register("TofdCal", "Land", fCalItems, kTRUE);

  return kSUCCESS;
}

// Note that the container may still be empty at this point.
void R3BTofdMapped2Cal::SetParContainers()
{
  fTcalPar = (R3BTCalPar *)FairRuntimeDb::instance()->getContainer("TofdTCalPar");
  if (!fTcalPar) {
    LOG(ERROR) << "Could not get access to TofdTCalPar-Container." << FairLogger::endl;
    fNofTcalPars = 0;
  }
}

InitStatus R3BTofdMapped2Cal::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

void R3BTofdMapped2Cal::Exec(Option_t *option)
{
  for (auto it = fCalLookup.begin(); fCalLookup.end() != it; ++it) {
    auto &vec = *it;
    vec.clear();
  }

  Int_t mapped_num = fMappedItems->GetEntriesFast();
  for (Int_t mapped_i = 0; mapped_i < mapped_num; mapped_i++) {
    auto mapped = (R3BTofdMappedData const *)fMappedItems->At(mapped_i);

    if ((mapped->GetDetectorId()<1) || (mapped->GetDetectorId()>fNofPlanes)) {
      LOG(DEBUG) << "R3BTofdMapped2Cal::Exec : Plane number out of range: " <<
        mapped->GetDetectorId() << FairLogger::endl;
      continue;
    }
    if ((mapped->GetBarId()<1) || (mapped->GetBarId()>fPaddlesPerPlane)) {
      LOG(DEBUG) << "R3BTofdMapped2Cal::Exec : Bar number out of range: " <<
        mapped->GetBarId() << ", "<<fPaddlesPerPlane<<FairLogger::endl;
      continue;
    }

    // Tcal parameters.
    auto *par = fTcalPar->GetModuleParAt(mapped->GetDetectorId(),
        mapped->GetBarId(), 2 * mapped->GetSideId() + mapped->GetEdgeId() - 2);
    if (!par) {
      LOG(INFO) << "R3BTofdMapped2Cal::Exec : Tcal par not found, Plane: " <<
        mapped->GetDetectorId() << ", Bar: " << mapped->GetBarId() << ", Side: " <<
        mapped->GetSideId() << ", Edge: " << mapped->GetEdgeId() <<
        FairLogger::endl;
      continue;
    }

    // Convert TDC to [ns] ...
    Double_t time_ns = par->GetTimeVFTX(mapped->GetTimeFine());
    // ... and subtract it from the next clock cycle.
    time_ns = (mapped->GetTimeCoarse() + 1) * fClockFreq - time_ns;

    R3BTofdCalData *cal = nullptr;
    size_t i = mapped->GetSideId() * 2 + mapped->GetEdgeId() - 3;

    // Look for coincident CalDatum.
    auto idx = GetCalLookupIndex(*mapped);
    auto &vec = fCalLookup.at(idx);
    for (auto it = vec.begin(); vec.end() != it; ++it) {
      cal = *it;
      assert(cal->GetDetectorId() == mapped->GetDetectorId());
      assert(cal->GetBarId() == mapped->GetBarId());

      if (cal->SetTime_ns(i, time_ns, c_acceptance_ns)) {
        break;
      }
    }
    if (!cal) {
      // Nothing coincident found, make a new datum.
      cal = new ((*fCalItems)[fNofCalItems++])
          R3BTofdCalData(mapped->GetDetectorId(), mapped->GetBarId());
      cal->SetTime_ns(i, time_ns, c_acceptance_ns);
      fCalLookup.at(idx).push_back(cal);
    }
  }
}

void R3BTofdMapped2Cal::FinishEvent()
{
  if (fCalItems) {
    fCalItems->Clear();
    fNofCalItems = 0;
  }
}

void R3BTofdMapped2Cal::FinishTask()
{
}

void R3BTofdMapped2Cal::SetNofModules(Int_t planes, Int_t ppp)
{
  fNofPlanes       = planes;
  fPaddlesPerPlane = ppp;
  // #planes * #bars, each entry is a full bar, i.e. merged sides + edges.
  fCalLookup.resize(planes * ppp);
}

ClassImp(R3BTofdMapped2Cal)
