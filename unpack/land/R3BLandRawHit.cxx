// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                            R3BLandRawHit                          -----
// -----                    Created 23.04.2013 by D.Kresan                 -----
// -----------------------------------------------------------------------------

#include "R3BLandRawHit.h"


R3BLandRawHit::R3BLandRawHit()
{
  fPadId = 0;
  fQdc1 = 0.;
  fQdc2 = 0.;
}


R3BLandRawHit::R3BLandRawHit(Int_t padId, Double_t qdc1, Double_t qdc2)
{
  fPadId = padId;
  fQdc1 = qdc1;
  fQdc2 = qdc2;
}


R3BLandRawHit::~R3BLandRawHit()
{
}


ClassImp(R3BLandRawHit)


