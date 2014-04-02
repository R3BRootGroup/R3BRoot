// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                          R3BCaloRawHit                            -----
// -----                           Version 0.1                             -----
// -----                    Created 15.10.2013 by Y.Gonzalez               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include "R3BCaloRawHit.h"


R3BCaloRawHit::R3BCaloRawHit()
  : fCrystalId(0),
    fEnergy(0),
    fNf(0),
    fNs(0),
    fTime(0),
    fError(0)
{
}


R3BCaloRawHit::R3BCaloRawHit(UShort_t crystalId, UShort_t energy,
			     UShort_t nf, UShort_t ns, ULong_t time, UChar_t error)
  : fCrystalId(crystalId),
    fEnergy(energy),
    fNf(nf),
    fNs(ns),
    fTime(time),
    fError(error)
{
}


ClassImp(R3BCaloRawHit)
