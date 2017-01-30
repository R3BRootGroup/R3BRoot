
#include "R3BFi4MappedItem.h"

// for the data analysis of the s438b PSPX detectors. 
// Introduced by Ralf, Jan 2016
// adapted by Joachim March 2016

R3BFi4MappedItem::R3BFi4MappedItem()
    : fDetector(0)
    , fMPPC(0)
    , fEnergy(0)
    , fTime(0)
{
}

R3BFi4MappedItem::R3BFi4MappedItem(	UChar_t detector, 
					UInt_t mppc,
					UInt_t energy,
					UInt_t time)
: fDetector(detector)
  , fMPPC(mppc)
  , fEnergy(energy)
  , fTime(time)
{
}

ClassImp(R3BFi4MappedItem)
