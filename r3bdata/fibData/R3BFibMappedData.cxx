// ----------------------------------------------------------------
// -----              R3BFibMappedData                -----
// -----             Created Jan 2018 by M.Heil        -----
// ----------------------------------------------------------------

#include "R3BFibMappedData.h"

R3BFibMappedData::R3BFibMappedData()
    : fPlane(0)
    , fFiber(0)
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

R3BFibMappedData::R3BFibMappedData(Int_t planeId, Int_t fiberId)
    : fPlane(planeId)
    , fFiber(fiberId)
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

ClassImp(R3BFibMappedData)
