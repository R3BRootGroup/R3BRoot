#include "R3BCalifaMappedData.h"

R3BCalifaMappedData::R3BCalifaMappedData()
    : fCrystalId(0)
    , fEnergy(0)
    , fNf(0)
    , fNs(0)
    , fTime(0)
    , fError(0)
    , fTot(0)
{
}

R3BCalifaMappedData::R3BCalifaMappedData(UShort_t crystalId,
                                         Int_t energy,
                                         Int_t nf,
                                         Int_t ns,
                                         uint64_t time,
                                         UChar_t error,
                                         UShort_t tot)
    : fCrystalId(crystalId)
    , fEnergy(energy)
    , fNf(nf)
    , fNs(ns)
    , fTime(time)
    , fError(error)
    , fTot(tot)
{
}

ClassImp(R3BCalifaMappedData)
