// ----------------------------------------------------------------
// -----              R3BPaddleTamexMappedData                -----
// -----             Created August 2nd 2016 by R.Plag        -----
// ----------------------------------------------------------------

#include "R3BPaddleTamexMappedData.h"

R3BPaddleTamexMappedData::R3BPaddleTamexMappedData()
    : fPlane(0)
    , fBar(0)
    , fCoarseTime1LE(0)
    , fFineTime1LE(0)
    , fCoarseTime1TE(0)
    , fFineTime1TE(0)
    , fCoarseTime2LE(0)
    , fFineTime2LE(0)
    , fCoarseTime2TE(0)
    , fFineTime2TE(0)
{
}

R3BPaddleTamexMappedData::R3BPaddleTamexMappedData(Int_t planeId, Int_t barId)
    : fPlane(planeId)
    , fBar(barId)
    , fCoarseTime1LE(0)
    , fFineTime1LE(0)
    , fCoarseTime1TE(0)
    , fFineTime1TE(0)
    , fCoarseTime2LE(0)
    , fFineTime2LE(0)
    , fCoarseTime2TE(0)
    , fFineTime2TE(0)
{
}

ClassImp(R3BPaddleTamexMappedData)
