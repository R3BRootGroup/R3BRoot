
#include "R3BPspxCalData.h"

// for the data analysis of the s438b PSPX detectors. 
// Introduced by Ralf, Jan 2016
// Changed by Ina Syndikus, April 2016


R3BPspxCalData::R3BPspxCalData()
    : fDetector(0)
    , fStrip(0)
    , fEnergy1(0)
    , fEnergy2(0)
{
}

R3BPspxCalData::R3BPspxCalData(	UChar_t detector, 
				UChar_t strip,
				Float_t energy1,
				Float_t energy2)
    : fDetector(detector)
    , fStrip(strip)
    , fEnergy1(energy1)
    , fEnergy2(energy2)
{
}


ClassImp(R3BPspxCalData)
