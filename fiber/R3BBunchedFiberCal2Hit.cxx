#include "R3BBunchedFiberCal2Hit.h"
#include <cassert>
#include <TClonesArray.h>
#include <FairLogger.h>
#include "R3BTCalEngine.h"
#include "R3BBunchedFiberCalData.h"
#include "R3BBunchedFiberHitData.h"

R3BBunchedFiberCal2Hit::R3BBunchedFiberCal2Hit(const char *a_name, Int_t
    a_verbose, UInt_t a_mapmt_num, UInt_t a_spmt_num)
  : FairTask(TString("R3B") + a_name + "Cal2Hit", a_verbose)
  , fName(a_name)
  , fCalItems(nullptr)
  , fHitItems(new TClonesArray("R3BBunchedFiberHitData"))
  , fNofHitItems(0)
{
  fSideNum[0] = a_mapmt_num;
  fSideNum[1] = a_spmt_num;
}

R3BBunchedFiberCal2Hit::~R3BBunchedFiberCal2Hit()
{
  delete fHitItems;
}

InitStatus R3BBunchedFiberCal2Hit::Init()
{
  auto mgr = FairRootManager::Instance();
  if (!mgr) {
    LOG(ERROR) << "FairRootManager not found." << FairLogger::endl;
    return kERROR;
  }
  auto name = fName + "Cal";
  fCalItems = (TClonesArray *)mgr->GetObject(name);
  if (!fCalItems) {
    LOG(ERROR) << "Branch " << name << " not found." << FairLogger::endl;
    return kERROR;
  }
  mgr->Register(fName + "Hit", "Land", fHitItems, kTRUE);
  return kSUCCESS;
}

InitStatus R3BBunchedFiberCal2Hit::ReInit()
{
  return kSUCCESS;
}

R3BBunchedFiberCal2Hit::ChannelArray R3BBunchedFiberCal2Hit::FindMaxSignal(Int_t a_side_id) const
{
  Double_t const c_period = 4096e3 / CLOCK_TDC_MHZ;
  auto cal_num = fCalItems->GetEntriesFast();
  auto channel_num = fSideNum[a_side_id];
  ChannelArray channel_array(channel_num);
  for (size_t j = 0; j < cal_num; ++j) {
    auto cur_cal = (R3BBunchedFiberCalData const *)fCalItems->At(j);
    if ((0 == a_side_id && !cur_cal->IsMAPMT()) ||
        (1 == a_side_id && !cur_cal->IsSPMT())) {
      continue;
    }
    auto channel_i = cur_cal->GetChannel() - 1;
    auto channel = &channel_array.at(channel_i);
    auto prev_cal = channel->prev;
    if (prev_cal &&
        prev_cal->IsLeading() && cur_cal->IsTrailing()) {
      auto ToT = fmod(cur_cal->GetTime_ns() - prev_cal->GetTime_ns() + c_period, c_period);
      if (ToT > channel->max_ToT) {
        // We're only interested in the largest signals.
        channel->max_ToT = ToT;
        channel->max_leading = prev_cal;
        channel->max_trailing = cur_cal;
      }
    }
    channel->prev = cur_cal;
  }
  return channel_array;
}

void R3BBunchedFiberCal2Hit::Exec(Option_t *option)
{
  // Find largest peak (max ToT) in every electronics channel, then combine
  // the channels to create fibers.

  auto mapmt_array = FindMaxSignal(0);
  auto spmt_array = FindMaxSignal(1);

  for (auto it_spmt = spmt_array.begin(); spmt_array.end() != it_spmt; ++it_spmt) {
    auto const &spmt = *it_spmt;
    if (0 > spmt.max_ToT) {
      continue;
    }
    for (auto it_mapmt = mapmt_array.begin(); mapmt_array.end() != it_mapmt; ++it_mapmt) {
      auto const &mapmt = *it_mapmt;
      if (0 > mapmt.max_ToT) {
        continue;
      }

      // We now have a good MAPMT hit, save a fiber!
      auto fiber_id = (mapmt.max_leading->GetChannel() - 1) * fSideNum[1] +
          spmt.max_leading->GetChannel();

      // Fix fiber installation mistakes.
      fiber_id = FixMistake(fiber_id);

      new ((*fHitItems)[fNofHitItems++])
          R3BBunchedFiberHitData(fiber_id,
              mapmt.max_leading->GetTime_ns(),
              spmt.max_leading->GetTime_ns(),
              mapmt.max_ToT,
              spmt.max_ToT);
    }
  }
}

void R3BBunchedFiberCal2Hit::FinishEvent()
{
  fHitItems->Clear();
  fNofHitItems = 0;
}

void R3BBunchedFiberCal2Hit::FinishTask()
{
}

ClassImp(R3BBunchedFiberCal2Hit)
