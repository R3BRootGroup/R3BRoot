
#include "R3BLosHitData.h"

// for the data analysis of the Los detector. 
// Introduced by Ralf, Feb 2016


R3BLosHitData::R3BLosHitData()
    : fDetector(0)
{
}

R3BLosHitData::R3BLosHitData(	UChar_t detector,
								Double_t t,
								Double_t x,
								Double_t y,
								Double_t diff
								)
    : fDetector(detector)
    , fTime_ns( t ) 
    , fX_cm(x)
    , fY_cm(y)
    , fTimeDiff2Ref_ns( diff )
{
}

ClassImp(R3BLosHitData)


