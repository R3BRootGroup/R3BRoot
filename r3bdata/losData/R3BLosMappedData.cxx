
#include "R3BLosMappedData.h"

// for the data analysis of the Los detector. 
// Introduced by Ralf, Jan 2016


R3BLosMappedData::R3BLosMappedData()
    : fDetector(0)
    , fChannel(0)
    , fTimeCoarse(0)
    , fTimeFine(0)
{
}

R3BLosMappedData::R3BLosMappedData(	UChar_t detector,
									UChar_t channel,
									UInt_t timeCoarse,
									UInt_t timeFine)
    : fDetector(detector)
    , fChannel(channel)
    , fTimeCoarse(timeCoarse)
    , fTimeFine(timeFine)
{
}

ClassImp(R3BLosMappedData)
