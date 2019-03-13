// ---------------------------------------------------------------------------
// -----                                                                 -----
// -----                      R3BWRAmsData                               -----
// -----                  Created 28/02/2019 by J.L. Rodriguez           -----
// -----                                                                 -----
// ---------------------------------------------------------------------------

#include "R3BWRAmsData.h"

R3BWRAmsData::R3BWRAmsData()
  : fTimeStamp(0)
{
}

//------------------------------

R3BWRAmsData::R3BWRAmsData(uint64_t timestamp)
  : fTimeStamp(timestamp)
{
}

ClassImp(R3BWRAmsData)
