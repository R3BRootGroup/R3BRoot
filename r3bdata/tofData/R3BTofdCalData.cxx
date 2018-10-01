#include "R3BTofdCalData.h"
#include <cmath>
#include <iostream>
#include "TMath.h"
#include "FairLogger.h"

#define ASSERT(a, op, b) do {\
    auto const a_ = a;\
    auto const b_ = b;\
    if (!(a_ op b_)) {\
      std::cerr << "Assertion \"" #a << '=' << a_ << ' ' << #op << " " #b\
          << '=' << b_ << "\" failed." << std::endl;\
      abort();\
    }\
  } while (0)
#define IS_NAN(x) TMath::IsNaN(x)
#define LENGTH(x) (sizeof x / sizeof x[0])

namespace {
  double const c_range = 5 * 2048;
}

R3BTofdCalData::R3BTofdCalData()
    : fDetector(-1)
    , fBar(-1)
{
  Reset();
}

R3BTofdCalData::R3BTofdCalData(UInt_t detector, UInt_t channel)
    : fDetector(detector)
    , fBar(channel)
{	 
  Reset();
}

UInt_t R3BTofdCalData::GetDetectorId() const
{
  return fDetector;
}

UInt_t R3BTofdCalData::GetBarId() const
{
  return fBar;
}

Double_t R3BTofdCalData::GetTimeBL_ns() const
{
  return GetTime_ns(0);
}

Double_t R3BTofdCalData::GetTimeBT_ns() const
{
  return GetTime_ns(1);
}

Double_t R3BTofdCalData::GetTimeTL_ns() const
{
  return GetTime_ns(2);
}

Double_t R3BTofdCalData::GetTimeTT_ns() const
{
  return GetTime_ns(3);
}

Double_t R3BTofdCalData::GetTime_ns(UInt_t i) const
{
  ASSERT(i, <, LENGTH(fTime_ns));
  return fTime_ns[i];
}

Double_t R3BTofdCalData::GetTimeDiff(Double_t a_early, Double_t a_late) const
{
  return fmod(a_late - a_early + c_range, c_range);
}

void R3BTofdCalData::Reset()
{
  for (size_t i = 0; i < LENGTH(fTime_ns); ++i) {
    fTime_ns[i] = 0.0 / 0.0;
  }
  fFirst = -1;
  fLast = -1;
}

Bool_t R3BTofdCalData::SetTimeBL_ns(Double_t time_ns, Double_t time_width)
{
  return SetTime_ns(0, time_ns, time_width);
}

Bool_t R3BTofdCalData::SetTimeBT_ns(Double_t time_ns, Double_t time_width)
{
  return SetTime_ns(1, time_ns, time_width);
}

Bool_t R3BTofdCalData::SetTimeTL_ns(Double_t time_ns, Double_t time_width)
{
  return SetTime_ns(2, time_ns, time_width);
}

Bool_t R3BTofdCalData::SetTimeTT_ns(Double_t time_ns, Double_t time_width)
{
  return SetTime_ns(3, time_ns, time_width);
}

Bool_t R3BTofdCalData::SetTime_ns(UInt_t i, Double_t time_ns, Double_t
    time_width)
{
  ASSERT(i, <, LENGTH(fTime_ns));
if (time_ns >= c_range) return kFALSE;
  ASSERT(time_ns, <=, c_range);

  if (!IS_NAN(fTime_ns[i])) {
    return kFALSE;
  }

  if (fFirst < 0) {
    // Nothing set, just grab this one.
    fLast = fFirst = time_ns;
  } else {
    auto fFirst_time_ns = GetTimeDiff(fFirst, time_ns);
    auto time_ns_fLast = GetTimeDiff(time_ns, fLast);
    if (fFirst_time_ns < time_width) {
      if (time_ns_fLast > 0) {
        fLast = time_ns;
      }
    } else if (time_ns_fLast < time_width) {
      if (fFirst_time_ns > 0) {
        fFirst = time_ns;
      }
    } else {
      // New time is way off previous times, incompatible.
      return kFALSE;
    }
  }

  fTime_ns[i] = time_ns;
  return kTRUE;
}

ClassImp(R3BTofdCalData)
