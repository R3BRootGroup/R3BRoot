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

#include "R3BFi12Reader.h"
#include "FairLogger.h"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_fibtwelve.h"
}

R3BFi12Reader::R3BFi12Reader(EXT_STR_h101_FIBTWELVE* a_data, UInt_t a_offset)
    : R3BBunchedFiberReader("Fi12", a_offset, 2, 256, 2)
    , fData((EXT_STR_h101_FIBTWELVE_onion*)a_data)
{
}

Bool_t R3BFi12Reader::Init(ext_data_struct_info* a_struct_info) { R3B_BUNCHED_FIBER_INIT(FIBTWELVE, fData); }

ClassImp(R3BFi12Reader)
