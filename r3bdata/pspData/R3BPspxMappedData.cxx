#include "R3BPspxMappedData.h"

R3BPspxMappedData::R3BPspxMappedData()
    : fDetector(0)
    , fChannel(0)
    , fEnergy(0)
{
}

R3BPspxMappedData::R3BPspxMappedData(UShort_t detector, UShort_t channel, Int_t energy)
    : fDetector(detector)
    , fChannel(channel)
    , fEnergy(energy)
{
}

ClassImp(R3BPspxMappedData)
