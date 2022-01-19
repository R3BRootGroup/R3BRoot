/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BFi31Reader.h"
#include "FairLogger.h"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_fib31.h"
}

R3BFi31Reader::R3BFi31Reader(EXT_STR_h101_FIBEI* a_data, UInt_t a_offset)
    : R3BFiberMAPMTReader("Fi31", a_offset, 512)
    , fData((EXT_STR_h101_FIBEI_onion*)a_data)
{
}

Bool_t R3BFi31Reader::Init(ext_data_struct_info* a_struct_info) { R3B_FIBER_MAPMT_INIT_TRIG(FIBEI, fData); }

ClassImp(R3BFi31Reader)
