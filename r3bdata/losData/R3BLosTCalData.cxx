#include "R3BLosTCalData.h"

R3BLosTCalData::R3BLosTCalData()
    : fDetector(0)
    , fChannel(0)
    , fType(0)
    , fRawTimeNs(0)
{
}

R3BLosTCalData::R3BLosTCalData(UInt_t detector, UInt_t channel, UInt_t type, Double_t tns)
    : fDetector(detector)
    , fChannel(channel)
    , fType(type)
    , fRawTimeNs(tns)
{
}
ClassImp(R3BLosTCalData)
