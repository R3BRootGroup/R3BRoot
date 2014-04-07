// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                           R3BLandRawHit                                     -----
// -----                           Version 0.1                                       -----
// -----         Adapted by M.I. Cherciu @ 01.2014 after Y.Gonzalez code             -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

#include "R3BLandRawHit.h"


R3BLandRawHit::R3BLandRawHit()
{
  ftaddr = 0;
  ftch = 0;
  fcal = 0;
  fClock = 0;
  ftdata = 0;
  fqdata = 0;
  fsam = 0;
  fgtb = 0;
}



R3BLandRawHit::R3BLandRawHit(UShort_t Sam, UShort_t Gtb, UShort_t Tac_addr, UShort_t Tac_ch,
                             UShort_t Cal, UShort_t clock, UShort_t Tac_data, UShort_t Qdc_data)
{
  ftaddr = Tac_addr;
  ftch = Tac_ch;
  fcal = Cal;
  fClock = clock;
  ftdata = Tac_data;
  fqdata = Qdc_data;
  fsam = Sam;
  fgtb = Gtb;
}



ClassImp(R3BLandRawHit)
