#include "R3BSci2TcalData.h"

R3BSci2TcalData::R3BSci2TcalData()
    : fDetector(0)
    , fChannel(0)
    , fRawTimeNs(0)
{
}

R3BSci2TcalData::R3BSci2TcalData(UInt_t detector, UInt_t channel, Double_t tns)
    : fDetector(detector)
    , fChannel(channel)
    , fRawTimeNs(tns)
{
}
ClassImp(R3BSci2TcalData)
