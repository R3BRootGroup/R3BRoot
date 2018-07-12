#include "R3BFi5Reader.h"
#include "FairLogger.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_fibfive.h"
}

R3BFi5Reader::R3BFi5Reader(EXT_STR_h101_FIBFIVE *a_data, UInt_t a_offset)
  : R3BBunchedFiberReader("Fi5", a_offset, 1, 256, 4)
  , fData((EXT_STR_h101_FIBFIVE_onion *)a_data)
{
}

Bool_t R3BFi5Reader::Init(ext_data_struct_info *a_struct_info)
{
  R3B_BUNCHED_FIBER_INIT(FIBFIVE, fData);
}

ClassImp(R3BFi5Reader)
