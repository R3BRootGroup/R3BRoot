#include "R3BFi2bReader.h"
#include "FairLogger.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_fibtwob.h"
}

R3BFi2bReader::R3BFi2bReader(EXT_STR_h101_FIBTWOB *a_data, UInt_t a_offset)
  : R3BBunchedFiberReader("Fi2b", a_offset, 1, 256, 1)
  , fData((EXT_STR_h101_FIBTWOB_onion *)a_data)
{
}

Bool_t R3BFi2bReader::Init(ext_data_struct_info *a_struct_info)
{
  R3B_BUNCHED_FIBER_INIT(FIBTWOB, fData);
}

ClassImp(R3BFi2bReader)
