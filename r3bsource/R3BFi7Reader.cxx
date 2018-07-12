#include "R3BFi7Reader.h"
#include "FairLogger.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_fibseven.h"
}

R3BFi7Reader::R3BFi7Reader(EXT_STR_h101_FIBSEVEN *a_data, UInt_t a_offset)
  : R3BBunchedFiberReader("Fi7", a_offset, 2, 256, 1)
  , fData((EXT_STR_h101_FIBSEVEN_onion *)a_data)
{
}

Bool_t R3BFi7Reader::Init(ext_data_struct_info *a_struct_info)
{
  R3B_BUNCHED_FIBER_INIT(FIBSEVEN, fData);
}

ClassImp(R3BFi7Reader)
