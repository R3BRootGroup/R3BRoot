#include "R3BSamplerMappedData.h"
#include "FairLogger.h"
#include <iostream>

R3BSamplerMappedData::R3BSamplerMappedData()
    : fTime(-1) // VULOM timestamp.
{
}

R3BSamplerMappedData::R3BSamplerMappedData(int time)
    : fTime(time)
{
}

int R3BSamplerMappedData::GetTime() const { return fTime; }

ClassImp(R3BSamplerMappedData)
