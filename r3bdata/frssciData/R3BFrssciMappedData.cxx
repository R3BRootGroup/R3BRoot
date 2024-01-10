#include "R3BFrssciMappedData.h"
#include "FairLogger.h"
#include "TMath.h"
#define IS_NAN(x) TMath::IsNaN(x)
#include <iostream>

using namespace std;

R3BFrssciMappedData::R3BFrssciMappedData()
    : fDetector(0)    // Detector number: 1...n
    , fPmt(0)         // Pmt number:  1..3 [4 reserved for sync check]
    , fTimeFine(-1)   // Fine time
    , fTimeCoarse(-1) // Coarse time
{
}

R3BFrssciMappedData::R3BFrssciMappedData(UShort_t detector, UShort_t pmt, UInt_t timeFine, UInt_t timeCoarse)
    : fDetector(detector)
    , fPmt(pmt)
    , fTimeFine(timeFine)
    , fTimeCoarse(timeCoarse)
{
}

ClassImp(R3BFrssciMappedData)
