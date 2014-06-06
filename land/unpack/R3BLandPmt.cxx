// -----------------------------------------------------------------------
// -----                          R3BLandPmt                         -----
// -----                Created 22-04-2014 by D.Kresan               -----
// -----------------------------------------------------------------------

#include "R3BLandPmt.h"

R3BLandPmt::R3BLandPmt()
    : fBarId(0)
    , fSide(0)
    , fTime(0.)
    , fQdc(0)
{
}

R3BLandPmt::R3BLandPmt(Int_t barId, Int_t side, Double_t time, Int_t qdc)
: fBarId(barId)
, fSide(side)
, fTime(time)
, fQdc(qdc)
{
}

R3BLandPmt::~R3BLandPmt()
{
}

ClassImp(R3BLandPmt)
