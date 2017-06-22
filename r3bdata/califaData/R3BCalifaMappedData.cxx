// -----------------------------------------------------------------------------
// -----                       R3BCalifaMappedData                         -----
// -----                           Version 0.1                             -----
// -----                   Created 15/10/2013 by Y.Gonzalez                -----
// -----                  Modified 19/12/2016 by P.Cabanelas               -----
// -----------------------------------------------------------------------------

#include "R3BCalifaMappedData.h"


R3BCalifaMappedData::R3BCalifaMappedData()
  : fCrystalId(0),
    fEnergy(0),
    fNf(0),
    fNs(0),
    fTime(0),
    fError(0)
{
}


//------------------------------
/**
 * 
 * 
 */
R3BCalifaMappedData::R3BCalifaMappedData(UShort_t crystalId, Int_t energy, Int_t nf, Int_t ns, ULong_t time, UChar_t error, UShort_t tot)
  : fCrystalId(crystalId),
    fEnergy(energy),
    fNf(nf),
    fNs(ns),
    fTime(time),
    fError(error),
    fTot(tot) 
{
}


ClassImp(R3BCalifaMappedData)
