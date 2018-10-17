#include "R3BBunchedFiberCalData.h"

R3BBunchedFiberCalData::R3BBunchedFiberCalData()
  : fIsMAPMT()
  , fChannel(-1)
  , fIsLeading()
  , fTime_ns(-1)
{
}

R3BBunchedFiberCalData::R3BBunchedFiberCalData(Bool_t a_is_mapmt, Int_t
    a_channel, Bool_t a_is_leading, Double_t a_time_ns)
  : fIsMAPMT(a_is_mapmt)
  , fChannel(a_channel)
  , fIsLeading(a_is_leading)
  , fTime_ns(a_time_ns)
{
}

R3BBunchedFiberCalData::~R3BBunchedFiberCalData()
{
}

Int_t R3BBunchedFiberCalData::GetChannel() const
{
  return fChannel;
}


Double_t R3BBunchedFiberCalData::GetTime_ns() const
{
  return fTime_ns;
}

Bool_t R3BBunchedFiberCalData::IsMAPMT() const
{
  return fIsMAPMT;
}

Bool_t R3BBunchedFiberCalData::IsSPMT() const
{
  return !fIsMAPMT;
}

Bool_t R3BBunchedFiberCalData::IsLeading() const
{
  return fIsLeading;
}

Bool_t R3BBunchedFiberCalData::IsSortable() const
{
  return kTRUE;
}

Bool_t R3BBunchedFiberCalData::IsTrailing() const
{
  return !fIsLeading;
}

ClassImp(R3BBunchedFiberCalData)
