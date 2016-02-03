
#include "R3BLosMappedItem.h"

// for the data analysis of the Los detector. 
// Introduced by Ralf, Jan 2016


R3BLosMappedItem::R3BLosMappedItem()
    : fDetector(0)
    , fChannel(0)
    , fTimeCoarse(0)
    , fTimeFine(0)
{
}

R3BLosMappedItem::R3BLosMappedItem(	UChar_t detector,
									UChar_t channel,
									UInt_t timeCoarse,
									UInt_t timeFine)
    : fDetector(detector)
    , fChannel(channel)
    , fTimeCoarse(timeCoarse)
    , fTimeFine(timeFine)
{
}

ClassImp(R3BLosMappedItem)
