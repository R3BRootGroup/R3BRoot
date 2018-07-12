#include "R3BFi10Reader.h"
#include "FairLogger.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_fibten.h"
}

R3BFi6Reader::R3BFi10Reader(EXT_STR_h101_FIBTEN *a_data, UInt_t a_offset)
  : R3BBunchedFiberReader("Fi10", a_offset, 2, 256, 2)
  , fData((EXT_STR_h101_FIBTEN_onion *)a_data)
{
}

Bool_t R3BFi10Reader::Init(ext_data_struct_info *a_struct_info)
{
  R3B_BUNCHED_FIBER_INIT(FIBTEN, fData);
}

ClassImp(R3BFi10Reader)
