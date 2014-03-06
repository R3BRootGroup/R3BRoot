// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                           R3BLandHitTmp                                     -----
// -----                           Version 0.1                                       -----
// -----                      Created @ 03.2014 by M.I. Cherciu                      -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

#include "R3BLandHitTmp.h"



R3BLandHitTmp::R3BLandHitTmp()
{
  barid = 0.;
  time = 0.;
  charge = 0.;
}



R3BLandHitTmp::R3BLandHitTmp(Int_t ibar_id, Int_t dtime, Int_t icharge)
{
  barid = ibar_id;
  time = dtime;
  charge = icharge;
}



ClassImp(R3BLandHitTmp)
