#include "R3BSamplerMappedData.h"
#include "FairLogger.h"
#include <iostream>

using namespace std;

R3BSamplerMappedData::R3BSamplerMappedData()
    : fTime(-1) // VULOM timestamp.
{
}

R3BSamplerMappedData::R3BSamplerMappedData(UInt_t time)
    : fTime(time)
{
}

UInt_t R3BSamplerMappedData::GetTime() const { return fTime; }

ClassImp(R3BSamplerMappedData)
