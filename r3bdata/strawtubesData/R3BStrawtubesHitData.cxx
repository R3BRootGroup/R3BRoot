#include "R3BStrawtubesHitData.h"
#include "R3BStrawtubesCalData.h"

R3BStrawtubesHitData::R3BStrawtubesHitData(R3BStrawtubesCalData const &cal):
  fPlane(cal.GetPlane()),
  fStraw(cal.GetStraw()),
  fAverage_ns(0.5 * (cal.GetTime(1) + cal.GetTime(0))),
  fDiff_ns(cal.GetTime(1) - cal.GetTime(0))
{
}

R3BStrawtubesHitData::~R3BStrawtubesHitData()
{
}

ClassImp(R3BStrawtubesHitData)
