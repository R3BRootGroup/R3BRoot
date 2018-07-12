#include "R3BFi2aReader.h"
#include "FairLogger.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_fibtwoa.h"
}

R3BFi2aReader::R3BFi2aReader(EXT_STR_h101_FIBTWOA *a_data, UInt_t a_offset)
  : R3BBunchedFiberReader("Fi2a", a_offset, 1, 256, 1)
  , fData((EXT_STR_h101_FIBTWOA_onion *)a_data)
{
}

Bool_t R3BFi2aReader::Init(ext_data_struct_info *a_struct_info)
{
  R3B_BUNCHED_FIBER_INIT(FIBTWOA, fData);
}

ClassImp(R3BFi2aReader)
