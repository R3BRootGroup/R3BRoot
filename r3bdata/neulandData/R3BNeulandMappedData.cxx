
#include "R3BNeulandMappedData.h"

R3BNeulandMappedData::R3BNeulandMappedData()
    : fBarId(0)
    , fSide(0)
    , fIs17(kFALSE)
{
}

R3BNeulandMappedData::R3BNeulandMappedData(Int_t sam,
                                           Int_t gtb,
                                           Int_t tacAddr,
                                           Int_t cal,
                                           Int_t clock,
                                           Int_t tacData,
                                           Int_t qdcData,
                                           Int_t barId,
                                           Int_t side,
                                           Bool_t is17)
    : R3BNeulandUnpackData(sam, gtb, tacAddr, 0, cal, clock, tacData, qdcData)
    , fBarId(barId)
    , fSide(side)
    , fIs17(is17)
{
}

ClassImp(R3BNeulandMappedData)
