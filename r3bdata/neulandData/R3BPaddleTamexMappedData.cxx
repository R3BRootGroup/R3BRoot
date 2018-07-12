// ----------------------------------------------------------------
// -----              R3BPaddleTamexMappedData                -----
// -----             Created August 2nd 2016 by R.Plag        -----
// ----------------------------------------------------------------

#include "R3BPaddleTamexMappedData.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

R3BPaddleTamexMappedData::R3BPaddleTamexMappedData()
    : fPlane(0)
    , fBar(0)
    , fCoarseTime1LE(-1)
    , fFineTime1LE(-1)
    , fCoarseTime1TE(-1)
    , fFineTime1TE(-1)
    , fCoarseTime2LE(-1)
    , fFineTime2LE(-1)
    , fCoarseTime2TE(-1)
    , fFineTime2TE(-1)
{
}

R3BPaddleTamexMappedData::R3BPaddleTamexMappedData(Int_t planeId, Int_t barId)
    : fPlane(planeId)
    , fBar(barId)
    , fCoarseTime1LE(-1)
    , fFineTime1LE(-1)
    , fCoarseTime1TE(-1)
    , fFineTime1TE(-1)
    , fCoarseTime2LE(-1)
    , fFineTime2LE(-1)
    , fCoarseTime2TE(-1)
    , fFineTime2TE(-1)
{
}

ClassImp(R3BPaddleTamexMappedData)
