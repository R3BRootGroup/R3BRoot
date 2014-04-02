// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                       R3BLandRawHitMapped                                   -----
// -----                           Version 0.1                                       -----
// -----                 Created @ 03.2014 by M.I. Cherciu                           -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

#include "R3BLandHitTmp.h"



R3BLandRawHitMapped::R3BLandRawHitMapped()
{
  fIs17 = kFALSE;
  fBarId = 0;
  fSide = 0;
  fTime = 0;
  fCharge = 0;
  fCntl = 0;
}



R3BLandRawHitMapped::R3BLandRawHitMapped(Bool_t is17, Int_t barId, Int_t side,
                                         Int_t time, Int_t charge, Int_t cntl)
{
  fIs17 = is17;
  fBarId = barId;
  fSide = side;
  fTime = time;
  fCharge = charge;
  fCntl = cntl;
}



ClassImp(R3BLandRawHitMapped)
