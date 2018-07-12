#include "R3BFi9Reader.h"
#include "FairLogger.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_fibnine.h"
}

R3BFi9Reader::R3BFi9Reader(EXT_STR_h101_FIBNINE *a_data, UInt_t a_offset)
  : R3BBunchedFiberReader("Fi9", a_offset, 2, 256, 1)
  , fData((EXT_STR_h101_FIBNINE_onion *)a_data)
{
}

Bool_t R3BFi9Reader::Init(ext_data_struct_info *a_struct_info)
{
  R3B_BUNCHED_FIBER_INIT(FIBNINE, fData);
}

ClassImp(R3BFi9Reader)
