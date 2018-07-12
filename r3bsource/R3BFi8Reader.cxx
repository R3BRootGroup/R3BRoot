#include "R3BFi8Reader.h"
#include "FairLogger.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_fibeight.h"
}

R3BFi8Reader::R3BFi8Reader(EXT_STR_h101_FIBEIGHT *a_data, UInt_t a_offset)
  : R3BBunchedFiberReader("Fi8", a_offset, 2, 256, 1)
  , fData((EXT_STR_h101_FIBSIX_onion *)a_data)
{
}

Bool_t R3BFi8Reader::Init(ext_data_struct_info *a_struct_info)
{
  R3B_BUNCHED_FIBER_INIT(FIBEIGHT, fData);
}

ClassImp(R3BFi8Reader)
