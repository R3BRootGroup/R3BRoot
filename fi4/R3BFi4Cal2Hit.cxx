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

#include "R3BFi4Cal2Hit.h"

R3BFi4Cal2Hit::R3BFi4Cal2Hit(Bool_t a_is_calibrator, Bool_t a_is_gain, Bool_t a_is_tsync, Int_t a_verbose)
    : R3BBunchedFiberCal2Hit("Fi4",
                             a_verbose,
                             R3BTCalEngine::CTDC_8_12_FWD_250,
                             VERTICAL,
                             2,
                             256,
                             4,
                             a_is_calibrator,
                             a_is_gain,
                             a_is_tsync)
{
}

R3BFi4Cal2Hit::~R3BFi4Cal2Hit() {}

UInt_t R3BFi4Cal2Hit::FixMistake(UInt_t a_fiber_id)
{
    //  if (192 == a_fiber_id) a_fiber_id = 15;
    //  if (256 == a_fiber_id) a_fiber_id = 16;

    return a_fiber_id;
}

ClassImp(R3BFi4Cal2Hit)
