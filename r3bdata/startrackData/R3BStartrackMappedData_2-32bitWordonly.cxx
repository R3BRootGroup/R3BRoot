// ----------------------------------------------------------------
// -----              R3BStartrackMappedData                    -----
// -----        (replacing  R3BStartrackerDigitHit)           ----- 
// -----          Created 26-05-2017 by M.Labiche             -----
// ----------------------------------------------------------------

#include "R3BStartrackMappedData.h"

/* If no problem with ucesb:

R3BStartrackMappedData::R3BStartrackMappedData()
    : fLadder(0)
    , fSide(0)
    , fAsic(0)
    , fStrip(0)
    , fEdep(0)
    , fTimeStp(0)
{
}

R3BStartrackMappedData::R3BStartrackMappedData(Int_t ladderId,
                              Int_t side,
                              Int_t asicId,
                              Int_t stripId,
                              Double_t edep,
                              Double_t timestp)
    : fLadder(ladderId)
    , fSide(side)
    , fAsic(asicId)
    , fStrip(stripId)
    , fEdep(edep)
    , fTimeStp(timestp)
{
}

R3BStartrackMappedData::R3BStartrackMappedData(const R3BStartrackMappedData& right)
    : fLadder(right.fLadder)
    , fSide(right.fSide)
    , fAsic(right.fAsic)
    , fStrip(right.fStrip)
    , fEdep(right.fEdep)
    , fTimeStp(right.fTimeStp)        
{
}

*/
R3BStartrackMappedData::R3BStartrackMappedData()
    : fData(0)
{
}

R3BStartrackMappedData::R3BStartrackMappedData(Int_t data)
    : fData(data)

{
}

R3BStartrackMappedData::R3BStartrackMappedData(const R3BStartrackMappedData& right)
    : fData(right.fData)        
{
}

R3BStartrackMappedData::R3BStartrackMappedData(const R3BStartrackMappedData& right1, const R3BStartrackMappedData& right2)
  : fData(right1.fData), fData2(right2.fData)        
{
}


ClassImp(R3BStartrackMappedData)
