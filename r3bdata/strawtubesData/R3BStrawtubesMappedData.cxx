#include "R3BStrawtubesMappedData.h"

R3BStrawtubesMappedData::R3BStrawtubesMappedData(UChar_t plane, UChar_t straw,
    UChar_t side, UInt_t timeCoarse, UInt_t timeFine):
  fPlane(plane),
  fStraw(straw),
  fSide(side),
  fTimeCoarse(timeCoarse),
  fTimeFine(timeFine)
{
}

R3BStrawtubesMappedData::~R3BStrawtubesMappedData()
{
}

ClassImp(R3BStrawtubesMappedData)
