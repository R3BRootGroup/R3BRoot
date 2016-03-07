// ----------------------------------------------------------------
// -----              R3BNeulandTamexMappedItem               -----
// -----             Created 27-01-2015 by M.Heil             -----
// ----------------------------------------------------------------

#include "R3BNeulandTamexMappedItem.h"

R3BNeulandTamexMappedItem::R3BNeulandTamexMappedItem()
    : fPlaneId(0)
    , fBarId(0)
    , fSide(0)
    , fCoarseTimeLE(0)
    , fFineTimeLE(0)
    , fCoarseTimeTE(0)
    , fFineTimeTE(0)
    , fIs17(kFALSE)
{
}

R3BNeulandTamexMappedItem::R3BNeulandTamexMappedItem(Int_t planeId,
                              Int_t barId,
                              Int_t side,
                              Int_t coarseTimeLE,
                              Int_t fineTimeLE,
                              Int_t coarseTimeTE,
                              Int_t fineTimeTE,
                              Bool_t is17)
    : fPlaneId(planeId)
    , fBarId(barId)
    , fSide(side)
    , fCoarseTimeLE(coarseTimeLE)
    , fFineTimeLE(fineTimeLE)
    , fCoarseTimeTE(coarseTimeTE)
    , fFineTimeTE(fineTimeTE)    
    , fIs17(is17)
{
}

R3BNeulandTamexMappedItem::R3BNeulandTamexMappedItem(const R3BNeulandTamexMappedItem& right)
    : fPlaneId(right.fPlaneId)
    , fBarId(right.fBarId)
    , fSide(right.fSide)
    , fCoarseTimeLE(right.fCoarseTimeLE)
    , fFineTimeLE(right.fFineTimeLE)
    , fCoarseTimeTE(right.fCoarseTimeTE)
    , fFineTimeTE(right.fFineTimeTE)        
    , fIs17(right.fIs17)
{
}

ClassImp(R3BNeulandTamexMappedItem)
