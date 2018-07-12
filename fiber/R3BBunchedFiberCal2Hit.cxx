#include "R3BBunchedFiberCal2Hit.h"
#include <cassert>
#include <TClonesArray.h>
#include <FairLogger.h>
#include "R3BTCalEngine.h"
#include "R3BBunchedFiberCalData.h"
#include "R3BBunchedFiberHitData.h"

R3BBunchedFiberCal2Hit::R3BBunchedFiberCal2Hit(const char *a_name, Int_t
    a_verbose, Direction a_direction, UInt_t a_sub_num, UInt_t a_mapmt_per_sub,
    UInt_t a_spmt_per_sub)
  : FairTask(TString("R3B") + a_name + "Cal2Hit", a_verbose)
  , fName(a_name)
  , fDirection(a_direction)
  , fSubNum(a_sub_num)
  , fCalItems(nullptr)
  , fHitItems(new TClonesArray("R3BBunchedFiberHitData"))
  , fNofHitItems(0)
{
  fChPerSub[0] = a_mapmt_per_sub;
  fChPerSub[1] = a_spmt_per_sub;
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
  // Resize per-channel info arrays.
  for (auto side_i = 0; side_i < 2; ++side_i) {
    fChannelArray[side_i].resize(fSubNum * fChPerSub[side_i]);
  }
  return kSUCCESS;
}

InitStatus R3BBunchedFiberCal2Hit::ReInit()
{
  return kSUCCESS;
}

void R3BBunchedFiberCal2Hit::Exec(Option_t *option)
{
  // Find largest peak (max ToT) in every electronics channel, then combine
  // the channels to create fibers.
  for (auto side_i = 0; side_i < 2; ++side_i) {
    auto &array = fChannelArray[side_i];
    for (auto it = array.begin(); array.end() != it; ++it) {
      it->prev = nullptr;
      it->max_ToT = -1;
    }
  }
  auto const c_period = 4096e3 / CLOCK_TDC_MHZ;
  auto cal_num = fCalItems->GetEntriesFast();
  for (size_t j = 0; j < cal_num; ++j) {
    auto cur_cal = (R3BBunchedFiberCalData const *)fCalItems->At(j);

    auto side_i = cur_cal->IsMAPMT() ? 0 : 1;
    auto channel = &fChannelArray[side_i].at(cur_cal->GetChannel() - 1);
    auto prev_cal = channel->prev;
    if (prev_cal &&
        prev_cal->IsLeading() && cur_cal->IsTrailing()) {
      auto ToT = fmod(cur_cal->GetTime_ns() - prev_cal->GetTime_ns() + c_period, c_period);
      if (ToT > channel->max_ToT) {
        // We're only interested in the largest signals (so far).
        channel->max_ToT = ToT;
        channel->max_leading = prev_cal;
        channel->max_trailing = cur_cal;
      }
    }
    channel->prev = cur_cal;
  }

  for (UInt_t sub_i = 0; sub_i < fSubNum; ++sub_i) {
    auto const &mapmt_array = fChannelArray[0];
    auto const &spmt_array = fChannelArray[1];

    for (auto it_mapmt = mapmt_array.begin(); mapmt_array.end() != it_mapmt; ++it_mapmt) {
      auto const &mapmt = *it_mapmt;
      if (0 > mapmt.max_ToT) {
        continue;
      }
      for (auto it_spmt = spmt_array.begin(); spmt_array.end() != it_spmt; ++it_spmt) {
        auto const &spmt = *it_spmt;
        if (0 > spmt.max_ToT) {
          continue;
        }

        // We now have a good MAPMT hit, save a fiber!

	/*
	 * How to get a fiber ID for a fiber detector defined as:
	 *  SubNum = 2
	 *  MAPMT = 256
	 *  SPMT = 2
	 * This means we'll receive 512 MAPMT channels as 1..512, and 4 SPMT
	 * channels, but the first half (sub-detector) are completely decoupled
	 * from the second half. The sequence of all fibers in order is then,
	 * as (MAPMT,SPMT)-pairs:
	 *  (1,1), (1,2), (2,1), ... (256,1), (256,2),
	 *  (257,3), (257,4), (258,3), ... (512,3), (512,4)
	 */
        auto fiber_id = (mapmt.max_leading->GetChannel() - 1) * fChPerSub[1] +
            (spmt.max_leading->GetChannel() % fChPerSub[1]);
        // TODO: Use it_sub->direction to find real life coordinates.

        // Fix fiber installation mistakes.
        fiber_id = FixMistake(fiber_id);

        new ((*fHitItems)[fNofHitItems++])
            R3BBunchedFiberHitData(fiber_id,
                mapmt.max_leading->GetTime_ns(),
                spmt.max_leading->GetTime_ns(),
                mapmt.max_ToT,
                spmt.max_ToT,
                sqrt(mapmt.max_ToT*spmt.max_ToT));
      }
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
