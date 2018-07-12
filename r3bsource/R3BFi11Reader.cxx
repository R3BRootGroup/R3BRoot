#include "R3BFi11Reader.h"
#include "FairLogger.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_fibeleven.h"
}

R3BFi6Reader::R3BFi11Reader(EXT_STR_h101_FIBELEVEN *a_data, UInt_t a_offset)
  : R3BBunchedFiberReader("Fi11", a_offset, 2, 256, 2)
  , fData((EXT_STR_h101_FIBELEVEN_onion *)a_data)
{
}

Bool_t R3BFi11Reader::Init(ext_data_struct_info *a_struct_info)
{
  R3B_BUNCHED_FIBER_INIT(FIBELEVEN, fData);
}

ClassImp(R3BFi11Reader)
