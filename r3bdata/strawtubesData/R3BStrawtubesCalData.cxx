#include "R3BStrawtubesCalData.h"
#include <cmath>

R3BStrawtubesCalData::R3BStrawtubesCalData(UChar_t plane, UChar_t straw):
  fPlane(plane),
  fStraw(straw),
  fTime_ns()
{
  fTime_ns[0] = NAN;
  fTime_ns[1] = NAN;
}

R3BStrawtubesCalData::~R3BStrawtubesCalData()
{
}

UChar_t R3BStrawtubesCalData::GetPlane() const
{
  return fPlane;
}

UChar_t R3BStrawtubesCalData::GetStraw() const
{
  return fStraw;
}

Double_t R3BStrawtubesCalData::GetTime(UChar_t side) const
{
  return fTime_ns[side];
}

void R3BStrawtubesCalData::SetTime(UChar_t side, Double_t time)
{
  fTime_ns[side] = time;
}

ClassImp(R3BStrawtubesCalData)
