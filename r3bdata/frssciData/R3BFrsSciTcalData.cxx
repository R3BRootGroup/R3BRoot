#include "R3BFrsSciTcalData.h"

R3BFrsSciTcalData::R3BFrsSciTcalData()
    : fDetector(0)
    , fPmt(0)
    , fRawTimeNs(0)
{
}

R3BFrsSciTcalData::R3BFrsSciTcalData(UShort_t detector, UShort_t pmt, Double_t tns)
    : fDetector(detector)
    , fPmt(pmt)
    , fRawTimeNs(tns)
{
}
ClassImp(R3BFrsSciTcalData)
