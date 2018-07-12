#include "R3BFi1aReader.h"
#include "FairLogger.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_fibonea.h"
}

R3BFi1aReader::R3BFi1aReader(EXT_STR_h101_FIBONEA *a_data, UInt_t a_offset)
  : R3BBunchedFiberReader("Fi1a", a_offset, 1, 256, 1)
  , fData((EXT_STR_h101_FIBONEA_onion *)a_data)
{
}

Bool_t R3BFi1aReader::Init(ext_data_struct_info *a_struct_info)
{
  R3B_BUNCHED_FIBER_INIT(FIBONEA, fData);
}

ClassImp(R3BFi1aReader)
