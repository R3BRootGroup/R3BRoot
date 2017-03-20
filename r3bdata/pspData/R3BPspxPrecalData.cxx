#include "R3BPspxPrecalData.h"

R3BPspxPrecalData::R3BPspxPrecalData()
    : fDetector(0)
    , fStrip(0)
    , fEnergy1(0)
    , fEnergy2(0)
{
}

R3BPspxPrecalData::R3BPspxPrecalData(UShort_t detector, UShort_t strip, Float_t energy1, Float_t energy2)
    : fDetector(detector)
    , fStrip(strip)
    , fEnergy1(energy1)
    , fEnergy2(energy2)
{
}

ClassImp(R3BPspxPrecalData)
