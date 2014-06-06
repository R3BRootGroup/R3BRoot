// ---------------------------------------------------------
// -----                    R3BLosHit                  -----
// -----          Created 29-04-2014 by D.Kresan       -----
// ---------------------------------------------------------

#include "R3BLosHit.h"

R3BLosHit::R3BLosHit()
    : fChannel(0)
    , fTime(0.)
{
}

R3BLosHit::R3BLosHit(Int_t channel, Double_t time)
    : fChannel(channel)
    , fTime(time)
{
}

R3BLosHit::~R3BLosHit()
{
}

ClassImp(R3BLosHit)
