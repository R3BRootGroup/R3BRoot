
#include "R3BNeulandUnpackData.h"

R3BNeulandUnpackData::R3BNeulandUnpackData()
    : fSam(0)
    , fGtb(0)
    , fTacAddr(0)
    , fTacCh(0)
    , fCal(0)
    , fClock(0)
    , fTacData(0)
    , fQdcData(0)
{
}

R3BNeulandUnpackData::R3BNeulandUnpackData(UShort_t sam,
                                     UShort_t gtb,
                                     UShort_t tacAddr,
                                     UShort_t tacCh,
                                     UShort_t cal,
                                     UShort_t clock,
                                     UShort_t tacData,
                                     UShort_t qdcData)
    : fSam(sam)
    , fGtb(gtb)
    , fTacAddr(tacAddr)
    , fTacCh(tacCh)
    , fCal(cal)
    , fClock(clock)
    , fTacData(tacData)
    , fQdcData(qdcData)
{
}

R3BNeulandUnpackData::R3BNeulandUnpackData(const R3BNeulandUnpackData& right)
    : fSam(right.fSam)
    , fGtb(right.fGtb)
    , fTacAddr(right.fTacAddr)
    , fTacCh(right.fTacCh)
    , fCal(right.fCal)
    , fClock(right.fClock)
    , fTacData(right.fTacData)
    , fQdcData(right.fQdcData)
{
}

ClassImp(R3BNeulandUnpackData)
