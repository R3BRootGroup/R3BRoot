// -----------------------------------------------------------------------
// -----                      R3BNeulandCalData                      -----
// -----              Created 22-04-2014 by D.Kresan                 -----
// -----------------------------------------------------------------------

#include "R3BNeulandCalData.h"

R3BNeulandCalData::R3BNeulandCalData()
    : fBarId(0)
    , fSide(0)
    , fTime(0.)
    , fQdc(0)
{
}

R3BNeulandCalData::R3BNeulandCalData(Int_t barId, Int_t side, Double_t time, Int_t qdc)
: fBarId(barId)
, fSide(side)
, fTime(time)
, fQdc(qdc)
{
}

R3BNeulandCalData::~R3BNeulandCalData()
{
}

ClassImp(R3BNeulandCalData)
