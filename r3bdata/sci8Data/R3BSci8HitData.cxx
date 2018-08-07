
#include "R3BSci8HitData.h"

// for the data analysis of the Sci8 detector. 


R3BSci8HitData::R3BSci8HitData()
    : fDetector(0)
{
}

R3BSci8HitData::R3BSci8HitData(	UChar_t detector,
								Double_t t,
								Double_t x,
								Double_t Z
								)

    : fDetector(detector)
    , fTime_ns( t ) 
    , fX_cm(x)
    , fZ(Z)

{
}

ClassImp(R3BSci8HitData)



