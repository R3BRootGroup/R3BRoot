// ----------------------------------------------------------------
// -----              R3BPaddleTamexMappedData               -----
// -----             Created 27-01-2015 by M.Heil             -----
// ----------------------------------------------------------------

#include "R3BPaddleTamexMappedData.h"

R3BPaddleTamexMappedData::R3BPaddleTamexMappedData()
    : fPlane(0)
    , fBar(0)
    , fSide(0)
    , fCoarseTimeLE(0)
    , fFineTimeLE(0)
    , fCoarseTimeTE(0)
    , fFineTimeTE(0)
    , fIs17(kFALSE)
{
}

R3BPaddleTamexMappedData::R3BPaddleTamexMappedData(Int_t planeId,
                              Int_t barId,
                              Int_t side,
                              Int_t coarseTimeLE,
                              Int_t fineTimeLE,
                              Int_t coarseTimeTE,
                              Int_t fineTimeTE,
                              Bool_t is17)
    : fPlane(planeId)
    , fBar(barId)
    , fSide(side)
    , fCoarseTimeLE(coarseTimeLE)
    , fFineTimeLE(fineTimeLE)
    , fCoarseTimeTE(coarseTimeTE)
    , fFineTimeTE(fineTimeTE)    
    , fIs17(is17)
{
}

R3BPaddleTamexMappedData::R3BPaddleTamexMappedData(const R3BPaddleTamexMappedData& right)
    : fPlane(right.fPlane)
    , fBar(right.fBar)
    , fSide(right.fSide)
    , fCoarseTimeLE(right.fCoarseTimeLE)
    , fFineTimeLE(right.fFineTimeLE)
    , fCoarseTimeTE(right.fCoarseTimeTE)
    , fFineTimeTE(right.fFineTimeTE)        
    , fIs17(right.fIs17)
{
}

ClassImp(R3BPaddleTamexMappedData)
