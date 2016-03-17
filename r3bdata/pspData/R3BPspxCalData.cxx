
#include "R3BPspxCalData.h"

// for the data analysis of the s438b PSPX detectors. 
// Introduced by Ralf, Jan 2016


R3BPspxCalData::R3BPspxCalData()
    : fDetector(0)
    , fChannel(0)
    , fEnergy(0)
{
}

R3BPspxCalData::R3BPspxCalData(	UChar_t detector, 
								UChar_t channel,
								Float_t energy)
    : fDetector(detector)
    , fChannel(channel)
    , fEnergy(energy)
{
}

ClassImp(R3BPspxCalData)
