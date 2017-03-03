#include "R3BFi6Reader.h"
#include "FairLogger.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_fibsix.h"
}

R3BFi6Reader::R3BFi6Reader(EXT_STR_h101_FIBSIX *a_data, UInt_t a_offset)
  : R3BBunchedFiberReader("Fi6", a_offset, 256, 2)
  , fData((EXT_STR_h101_FIBSIX_onion *)a_data)
{
}

Bool_t R3BFi6Reader::Init(ext_data_struct_info *a_struct_info)
{
  R3B_BUNCHED_FIBER_INIT(FIBSIX, fData);
}

ClassImp(R3BFi6Reader)
