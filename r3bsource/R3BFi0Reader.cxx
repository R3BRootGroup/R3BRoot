#include "R3BFi0Reader.h"
#include "FairLogger.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_fibzero.h"
}

R3BFi0Reader::R3BFi0Reader(EXT_STR_h101_FIBZERO *a_data, UInt_t a_offset)
  : R3BBunchedFiberReader("Fi0", a_offset, 1, 256, 0)
  , fData((EXT_STR_h101_FIBZERO_onion *)a_data)
{
}

Bool_t R3BFi0Reader::Init(ext_data_struct_info *a_struct_info)
{
  R3B_BUNCHED_FIBER_INIT_MAPMT_ONLY(FIBZERO, fData);
}

ClassImp(R3BFi0Reader)
