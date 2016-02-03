
#include "R3BPspxMappedItem.h"

// for the data analysis of the s438b PSPX detectors. 
// Introduced by Ralf, Jan 2016


R3BPspxMappedItem::R3BPspxMappedItem()
    : fDetector(0)
    , fChannel(0)
    , fEnergy(0)
{
}

R3BPspxMappedItem::R3BPspxMappedItem(	UChar_t detector, 
										UChar_t channel,
										UInt_t energy)
    : fDetector(detector)
    , fChannel(channel)
    , fEnergy(energy)
{
}

ClassImp(R3BPspxMappedItem)
