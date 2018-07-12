#include "R3BFi1Reader.h"
#include "FairLogger.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_fiboneb.h"
}

R3BFi1bReader::R3BFi1bReader(EXT_STR_h101_FIBONEB *a_data, UInt_t a_offset)
  : R3BBunchedFiberReader("Fi1b", a_offset, 1, 256, 1)
  , fData((EXT_STR_h101_FIBONEB_onion *)a_data)
{
}

Bool_t R3BFi1bReader::Init(ext_data_struct_info *a_struct_info)
{
  R3B_BUNCHED_FIBER_INIT(FIBONEB, fData);
}

ClassImp(R3BFi1bReader)
