
#include "R3BPspxMappedData.h"

// for the data analysis of the s438b PSPX detectors. 
// Introduced by Ralf, Jan 2016


R3BPspxMappedData::R3BPspxMappedData()
    : fDetector(0)
    , fChannel(0)
    , fEnergy(0)
{
}

R3BPspxMappedData::R3BPspxMappedData(	UShort_t detector, 
					UShort_t channel,
					UInt_t energy)
    : fDetector(detector)
    , fChannel(channel)
    , fEnergy(energy)
{
}

ClassImp(R3BPspxMappedData)
