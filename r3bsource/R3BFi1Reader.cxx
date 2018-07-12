#include "R3BFi1Reader.h"
#include "FairLogger.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_fibone.h"
}

R3BFi1Reader::R3BFi1Reader(EXT_STR_h101_FIBONE *a_data, UInt_t a_offset)
  : R3BBunchedFiberReader("Fi1", a_offset, 2, 256, 1)
  , fData((EXT_STR_h101_FIBONE_onion *)a_data)
{
}

Bool_t R3BFi1Reader::Init(ext_data_struct_info *a_struct_info)
{
  R3B_BUNCHED_FIBER_INIT(FIBONE, fData);
}

ClassImp(R3BFi1Reader)
