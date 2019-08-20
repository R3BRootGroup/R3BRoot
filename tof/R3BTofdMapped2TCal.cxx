// ------------------------------------------------------------
// -----                  R3BTofdMapped2TCal                -----
// -----          Created Feb 4th 2016 by R.Plag          -----
// ------------------------------------------------------------

/* March 2016
 * Reconstructing data into PaddleItems. Each paddle holds two times,
 * (for PM1 and PM2). This allows to plot
 * the time differences e.g. via evt->Draw(...) interactively (aka without
 * looping over all channels) which is crucial for a quick check of the
 * detector status during the experiment.
 *
 */


#include "R3BTofdMapped2TCal.h"

#include "R3BTCalEngine.h"
#include "R3BTofdMappedData.h"
#include "R3BTofdCalData.h"
#include "R3BTCalPar.h"
#include "R3BEventHeader.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TMath.h"

#define IS_NAN(x) TMath::IsNaN(x)

R3BTofdMapped2TCal::R3BTofdMapped2TCal()
  : FairTask("TofdTcal", 1)
  , fMappedItems(nullptr)
  , fCalItems(new TClonesArray("R3BTofdCalData"))
  , fNofCalItems(0)
  , fTcalPar(0)
  , fNofTcalPars(0)
  , header(nullptr)
  , fTrigger(-1)
  , fNofPlanes(0)
  , fPaddlesPerPlane(0)
  , fNofEdges(0)
  , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

  R3BTofdMapped2TCal::R3BTofdMapped2TCal(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose)
  , fMappedItems(nullptr)
  , fCalItems(new TClonesArray("R3BTofdCalData"))
  , fNofCalItems(0)
  , fTcalPar(0)
  , fNofTcalPars(0)
  , header(nullptr)
  , fTrigger(-1)
  , fNofPlanes(0)
  , fPaddlesPerPlane(0)
  , fNofEdges(0)
  , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BTofdMapped2TCal::~R3BTofdMapped2TCal()
{
  delete fCalItems;
}

InitStatus R3BTofdMapped2TCal::Init()
{
  fNofTcalPars = fTcalPar->GetNumModulePar();
  if (fNofTcalPars == 0) {
    LOG(ERROR) << "There are no TCal parameters in container TofdTCalPar";
    return kFATAL;
  }

  LOG(INFO) << "R3BTofdMapped2TCal::Init : read " << fNofTcalPars << " modules";

  // try to get a handle on the EventHeader. EventHeader may not be
  // present though and hence may be null. Take care when using.
  FairRootManager* mgr = FairRootManager::Instance();
  if (NULL == mgr)
    LOG(fatal) << "FairRootManager not found";
  header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

  // get access to Mapped data
  fMappedItems = (TClonesArray*)mgr->GetObject("TofdMapped");
  if (NULL == fMappedItems)
    LOG(fatal) << "Branch TofdMapped not found";

  // request storage of Cal data in output tree
  mgr->Register("TofdCal", "Land", fCalItems, kTRUE);

  return kSUCCESS;
}

// Note that the container may still be empty at this point.
void R3BTofdMapped2TCal::SetParContainers()
{
  fTcalPar = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("TofdTCalPar");
  if (!fTcalPar) {
    LOG(ERROR) << "Could not get access to TofdTCalPar-Container.";
    fNofTcalPars = 0;
    return;
  }
}

InitStatus R3BTofdMapped2TCal::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

void R3BTofdMapped2TCal::Exec(Option_t* option)
{
  // check for requested trigger (Todo: should be done globablly / somewhere else)
  if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
    return;

  Int_t nHits = fMappedItems->GetEntriesFast();
  for (Int_t ihit = 0; ihit < nHits; ihit++) {
    R3BTofdMappedData* hit = (R3BTofdMappedData*)fMappedItems->At(ihit);

    Int_t iDetector = hit->GetDetector(); // 1..n
    Int_t iSide     = hit->GetSide();     // 1/2
    Int_t iBar      = hit->GetBar();      // 1..n
    Int_t iEdge     = hit->GetEdge();     // 1/2

    if ((iDetector<1) || (iDetector>fNofPlanes)) {
      LOG(DEBUG) << "R3BTofdMapped2TCal::Exec : Plane number out of range: " <<
        iDetector;
      continue;
    }
    if ((iBar<1) || (iBar>fPaddlesPerPlane)) {
      LOG(DEBUG) << "R3BTofdMapped2TCal::Exec : Bar number out of range: " <<
        iBar << ", "<<fPaddlesPerPlane;
      continue;
    }

    R3BTofdCalData *cal;

    // Look for a coincident CalDatum on this channel.
    Int_t nCals = fCalItems->GetEntriesFast();
    for (Int_t ical = 0; ical < nCals; ical++) {
      cal = (R3BTofdCalData *)fCalItems->At(ical);
      if (cal->GetBar() != iBar) {
        continue;
      }
      if (1 == iSide) {
      }
      if (IS_NAN(cal->GetBottom_ns())) {
      } else {
      }
    }

    R3BTofdCalData* calData = new ((*fCalItems)[fNofCalItems++]) R3BTofdCalData(iDetector, iBar);

    R3BTCalModulePar const *par = fTcalPar->GetModuleParAt(iDetector, iBar, 2 * iSide + iEdge - 2);

    if (!par) {
      LOG(INFO) << "R3BTofdMapped2TCal::Exec : Tcal par not found, Plane: " <<
        iDetector << ", Bar: " << iBar << ", Edge: " << iEdge;
      continue;
    }

    // Convert TDC to [ns] ...
    Double_t time_ns = par->GetTimeVFTX(hit->GetTimeFine());
    // ... and subtract it from the next clock cycle.
    time_ns = (hit->GetTimeCoarse() + 1) * fClockFreq - time_ns;

    calData->Set(hit->GetSide() - 1, hit->GetEdge() - 1, time_ns);
  }
}

void R3BTofdMapped2TCal::FinishEvent()
{
  if (fCalItems) {
    fCalItems->Clear();
    fNofCalItems = 0;
  }
}

void R3BTofdMapped2TCal::FinishTask()
{
}

ClassImp(R3BTofdMapped2TCal)
