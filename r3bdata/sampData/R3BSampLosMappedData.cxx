#include "R3BSampLosMappedData.h"
#include "FairLogger.h"
#include <iostream>

using namespace std;

R3BSampLosMappedData::R3BSampLosMappedData()
    : fTimeL(-1) // VULOM timestamp.
    , fTimeH(-1)
{
}

R3BSampLosMappedData::R3BSampLosMappedData(UInt_t timeL, UInt_t timeH)
    : fTimeL(timeL)
    , fTimeH(timeH)
{
}

UInt_t R3BSampLosMappedData::GetTimeL() const { return fTimeL; }
UInt_t R3BSampLosMappedData::GetTimeH() const { return fTimeH; }

ClassImp(R3BSampLosMappedData)
