// ---------------------------------------------------------------------------
// -----                                                                 -----
// -----                      R3BWRCalifaData                            -----
// -----                  Created 28/02/2019 by J.L. Rodriguez           -----
// -----                                                                 -----
// ---------------------------------------------------------------------------

#include "R3BWRCalifaData.h"

R3BWRCalifaData::R3BWRCalifaData()
  : fTimeStamp(0)
{
}

//------------------------------

R3BWRCalifaData::R3BWRCalifaData(Int_t timestamp)
  : fTimeStamp(timestamp)
{
}

ClassImp(R3BWRCalifaData)
