// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                           R3BLandUnpack                                     -----
// -----                           Version 0.1                                       -----
// -----         Adapted by M.I. Cherciu @ 01.2014 after Y.Gonzalez code             -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

#include "R3BLandRawHit.h"


R3BLandRawHit::R3BLandRawHit(){
ftaddr = 0.;
ftch = 0.;
fcal = 0.;
fcntdata = 0.;
ftdata = 0.;
fqdata = 0.;
}


R3BLandRawHit::R3BLandRawHit(UShort_t Tac_addr, UShort_t Tac_ch, UShort_t Cal, UShort_t Cntrl_data, UShort_t Tac_data, UShort_t Qdc_data)
{
ftaddr = Tac_addr;
ftch = Tac_ch;
fcal = Cal;
fcntdata = Cntrl_data;
ftdata = Tac_data;
fqdata = Qdc_data;
}


ClassImp(R3BLandRawHit)
