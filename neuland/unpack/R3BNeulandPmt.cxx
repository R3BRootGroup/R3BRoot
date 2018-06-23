// -----------------------------------------------------------------------
// -----                          R3BNeulandPmt                         -----
// -----                Created 27-01-2016 by M.Heil               -----
// -----------------------------------------------------------------------

#include "R3BNeulandPmt.h"

R3BNeulandPmt::R3BNeulandPmt()
    : fPlaneId(0)
    , fBarId(0)
    , fSide(0)
    , fTime(0.)
    , fCharge(0)
{
}

R3BNeulandPmt::R3BNeulandPmt(Int_t planeId, Int_t barId, Int_t side, Double_t time, Double_t charge)
    : fPlaneId(planeId)
    , fBarId(barId)
    , fSide(side)
    , fTime(time)
    , fCharge(charge)
{
}

R3BNeulandPmt::~R3BNeulandPmt() {}

ClassImp(R3BNeulandPmt)
