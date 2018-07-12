#include "R3BFi3bReader.h"
#include "FairLogger.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_fibthreeb.h"
}

R3BFi3bReader::R3BFi3bReader(EXT_STR_h101_FIBTHREEB *a_data, UInt_t a_offset)
  : R3BBunchedFiberReader("Fi3b", a_offset, 1, 256, 2)
  , fData((EXT_STR_h101_FIBTHREEB_onion *)a_data)
{
}

Bool_t R3BFi3bReader::Init(ext_data_struct_info *a_struct_info)
{
  R3B_BUNCHED_FIBER_INIT(FIBTHREEB, fData);
}

ClassImp(R3BFi3bReader)
