// ------------------------------------------------------------
// -----                  R3BBunchedFiberMapped2TCal                -----
// -----          Created Feb 13th 2018 by M.Heil          -----
// ------------------------------------------------------------

#include "R3BBunchedFiberMapped2Cal.h"
#include <cassert>
#include "TClonesArray.h"
#include "FairLogger.h"
#include "FairRuntimeDb.h"
#include "R3BBunchedFiberCalData.h"
#include "R3BBunchedFiberMappedData.h"
#include "R3BTCalEngine.h"

R3BBunchedFiberMapped2Cal::R3BBunchedFiberMapped2Cal(const char *a_name, Int_t
    a_verbose, UInt_t a_mapmt_num, UInt_t a_spmt_num)
  : FairTask(TString("R3B") + a_name + "Mapped2Cal", a_verbose)
  , fName(a_name)
  , fTCalPar(nullptr)
  , fMappedItems(nullptr)
  , fCalItems(new TClonesArray("R3BBunchedFiberCalData"))
  , fNofCalItems(0)
  , fClockFreq(1000. / CLOCK_TDC_MHZ)
{
  fSideNum[0] = a_mapmt_num;
  fSideNum[1] = a_spmt_num;
}

R3BBunchedFiberMapped2Cal::~R3BBunchedFiberMapped2Cal()
{
  delete fCalItems;
}

InitStatus R3BBunchedFiberMapped2Cal::Init()
{
  if (!fTCalPar) {
    LOG(ERROR) << "No TCal parameter container, "
        "did you forget SetParContainers?" << FairLogger::endl;
    return kERROR; 
  }
  if (0 == fTCalPar->GetNumModulePar()) {
    LOG(ERROR) << "No TCal parameters in container " << fTCalPar->GetName() <<
        "." << FairLogger::endl;
    return kERROR;
  }
  auto mgr = FairRootManager::Instance();
  if (!mgr) {
    LOG(ERROR) << "FairRootManager not found." << FairLogger::endl;
    return kERROR;
  }
  auto name = fName + "Mapped";
  fMappedItems = (TClonesArray *)mgr->GetObject(name);
  if (!fMappedItems) {
    LOG(ERROR) << "Branch " << name << " not found." << FairLogger::endl;
    return kERROR;
  }
  mgr->Register(fName + "Cal", "Land", fCalItems, kTRUE);
  return kSUCCESS;
}

void R3BBunchedFiberMapped2Cal::SetParContainers()
{
  fTCalPar = (R3BTCalPar *)FairRuntimeDb::instance()->getContainer(fName +
      "TCalPar");
  if (!fTCalPar) {
    LOG(ERROR) << "Could not get access to " << fName << "TCalPar-Container." << FairLogger::endl;
  }
}

InitStatus R3BBunchedFiberMapped2Cal::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

void R3BBunchedFiberMapped2Cal::Exec(Option_t *option)
{
  auto mapped_num = fMappedItems->GetEntriesFast();
  LOG(DEBUG) << "R3BBunchedFiberMapped2Cal::Exec:fMappedItems=" <<
      fMappedItems->GetName() << '.' << FairLogger::endl;
  for (auto i = 0; i < mapped_num; i++) {
    auto mapped = (R3BBunchedFiberMappedData *)fMappedItems->At(i);
    assert(mapped);

    auto channel = mapped->GetChannel();
    LOG(DEBUG) << " R3BBunchedFiberMapped2Cal::Exec:Channel=" << channel << ":Edge=" <<
        (mapped->IsLeading() ? "Leading" : "Trailing") << '.' <<
        FairLogger::endl;

    // Fetch tcal parameters.
    auto par = fTCalPar->GetModuleParAt(1,
        channel * 2 - (mapped->IsLeading() ? 1 : 0),
        mapped->IsMAPMT() ? 1 : 2);
    if (!par) {
      LOG(WARNING) << "R3BBunchedFiberMapped2Cal::Exec:Channel=" << channel <<
          ": TCal par not found." << FairLogger::endl;
      continue;
    }

    // Calibrate fine time.
    auto fine_raw = mapped->GetFine();
    if (-1 == fine_raw) {
      // TODO: Is this really ok?
      continue;
    }
    auto fine_ns = par->GetTimeVFTX(fine_raw);
    LOG(DEBUG) << " R3BBunchedFiberMapped2Cal::Exec: Fine raw=" << fine_raw <<
        " -> ns=" << fine_ns << '.' << FairLogger::endl;
    if (fine_ns < 0. || fine_ns >= fClockFreq) {
      LOG(WARNING) << 
          "R3BBunchedFiberMapped2Cal::Exec:Channel=" << channel <<
          ": Bad fine time (raw=" << fine_raw << ",ns=" << fine_ns << ")." << FairLogger::endl;
      continue;
    }

    // Calculate final time with clock cycles.
    auto time_ns = mapped->GetCoarse() * fClockFreq +
        (mapped->IsLeading() ? -fine_ns : fine_ns);
    LOG(DEBUG) << " R3BBunchedFiberMapped2Cal::Exec:Channel=" << channel << ": Time=" <<
        time_ns  << "ns." << FairLogger::endl;

    new ((*fCalItems)[fNofCalItems++])
        R3BBunchedFiberCalData(
            mapped->IsMAPMT(),
            channel,
            mapped->IsLeading(),
            time_ns);
  }
}

void R3BBunchedFiberMapped2Cal::FinishEvent()
{
  fCalItems->Clear();
  fNofCalItems = 0;
}

void R3BBunchedFiberMapped2Cal::FinishTask()
{
}

ClassImp(R3BBunchedFiberMapped2Cal)
