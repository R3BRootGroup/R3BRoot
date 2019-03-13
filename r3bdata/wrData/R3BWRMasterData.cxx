// ---------------------------------------------------------------------------
// -----                                                                 -----
// -----                      R3BWRMasterData                            -----
// -----                  Created 28/02/2019 by J.L. Rodriguez           -----
// -----                                                                 -----
// ---------------------------------------------------------------------------

#include "R3BWRMasterData.h"

R3BWRMasterData::R3BWRMasterData()
  : fTimeStamp(0)
{
}

//------------------------------

R3BWRMasterData::R3BWRMasterData(uint64_t timestamp)
  : fTimeStamp(timestamp)
{
}

ClassImp(R3BWRMasterData)
