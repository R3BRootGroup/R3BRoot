#include "R3BFrsSciPosCalData.h"

R3BFrsSciPosCalData::R3BFrsSciPosCalData()
    : fDetector(0)
    , fRawTimeNs(0)
    , fRawPosNs(0)
    , fCalPosMm(0)
{
}

R3BFrsSciPosCalData::R3BFrsSciPosCalData(UShort_t det, Double_t rawT, Float_t rawPos, Float_t calPos)
    : fDetector(det)
    , fRawTimeNs(rawT)
    , fRawPosNs(rawPos)
    , fCalPosMm(calPos)
{
}
ClassImp(R3BFrsSciPosCalData)
