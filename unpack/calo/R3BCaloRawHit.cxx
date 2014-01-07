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
fTime(0)
{
}


R3BCaloRawHit::R3BCaloRawHit(UShort_t crystalId, UShort_t energy, ULong_t time)
: fCrystalId(crystalId),
fEnergy(energy),
fTime(time)
{
}


ClassImp(R3BCaloRawHit)
