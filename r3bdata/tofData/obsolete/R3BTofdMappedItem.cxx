
#include "R3BTofdMappedItem.h"

// for the data analysis of the Tofd detector. 
// Introduced by Ralf, March 2016


R3BTofdMappedItem::R3BTofdMappedItem()
    : fPaddle(0)
    , fPm(0)
    , fTimeCoarse(0)
    , fTimeFine(0)
{
}

R3BTofdMappedItem::R3BTofdMappedItem(UChar_t paddle,
									 UChar_t pm,
									 UInt_t timeCoarse,
									 UInt_t timeFine)
    : fPaddle(paddle)
    , fPm(pm)
    , fTimeCoarse(timeCoarse)
    , fTimeFine(timeFine)
{
}

ClassImp(R3BTofdMappedItem)
