#include "R3BTofdMappedData.h"

R3BTofdMappedData::R3BTofdMappedData()
    : fDetector(-1)
    , fSide(-1)
    , fBar(-1)
    , fTimeFine(-1)
    , fTimeCoarse(-1)
{
}

R3BTofdMappedData::R3BTofdMappedData(UInt_t detector,
    UInt_t side,
    UInt_t channel,
    UInt_t edge,
    UInt_t timeCoarse,
    UInt_t timeFine)
    : fDetector(detector)
    , fSide(side)
    , fBar(channel)
    , fEdge(edge)
    , fTimeCoarse(timeCoarse)
    , fTimeFine(timeFine)
{
}

UInt_t R3BTofdMappedData::GetDetectorId() const
{
  return fDetector;
}

UInt_t R3BTofdMappedData::GetSideId() const
{
  return fSide;
}

UInt_t R3BTofdMappedData::GetBarId() const
{
  return fBar;
}

UInt_t R3BTofdMappedData::GetEdgeId() const
{
  return fEdge;
}

UInt_t R3BTofdMappedData::GetTimeCoarse() const
{
  return fTimeCoarse;
}

UInt_t R3BTofdMappedData::GetTimeFine() const
{
  return fTimeFine;
}

ClassImp(R3BTofdMappedData)
