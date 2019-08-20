
#include "R3BSci2HitData.h"

// for the data analysis of the Sci2 detector.

R3BSci2HitData::R3BSci2HitData()
    : fDetector(0)
{
}

R3BSci2HitData::R3BSci2HitData(UChar_t detector, Double_t t, Double_t x, Double_t Z)

    : fDetector(detector)
    , fTime_ns(t)
    , fX_cm(x)
    , fZ(Z)

{
}

ClassImp(R3BSci2HitData)
