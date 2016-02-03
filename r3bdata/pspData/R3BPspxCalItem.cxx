
#include "R3BPspxCalItem.h"

// for the data analysis of the s438b PSPX detectors. 
// Introduced by Ralf, Jan 2016


R3BPspxCalItem::R3BPspxCalItem()
    : fDetector(0)
    , fChannel(0)
    , fEnergy(0)
{
}

R3BPspxCalItem::R3BPspxCalItem(	UChar_t detector, 
								UChar_t channel,
								Float_t energy)
    : fDetector(detector)
    , fChannel(channel)
    , fEnergy(energy)
{
}

ClassImp(R3BPspxCalItem)
