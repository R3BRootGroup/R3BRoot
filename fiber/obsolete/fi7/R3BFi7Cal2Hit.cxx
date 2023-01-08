/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BFi7Cal2Hit.h"

R3BFi7Cal2Hit::R3BFi7Cal2Hit(Bool_t a_is_calibrator, Bool_t a_is_gain, Bool_t a_is_tsync, Int_t a_verbose)
    : R3BBunchedFiberCal2Hit("Fi7",
                             a_verbose,
                             R3BTCalEngine::CTDC_8_12_FWD_250,
                             VERTICAL,
                             2,
                             256,
                             1,
                             a_is_calibrator,
                             a_is_gain,
                             a_is_tsync)
{
}

R3BFi7Cal2Hit::~R3BFi7Cal2Hit() {}

UInt_t R3BFi7Cal2Hit::FixMistake(UInt_t a_fiber_id)
{
    UInt_t n_fiber_id = a_fiber_id;
    //  if (320 == a_fiber_id) {n_fiber_id = 336;std::cout << a_fiber_id << "->" << n_fiber_id << '\n';}
    //  if (336 == a_fiber_id) {n_fiber_id = 320;std::cout << a_fiber_id << "->" << n_fiber_id << '\n';}
    //  if (431 == a_fiber_id) {n_fiber_id = 447;std::cout << a_fiber_id << "->" << n_fiber_id << '\n';}
    //  if (447 == a_fiber_id) {n_fiber_id = 431;std::cout << a_fiber_id << "->" << n_fiber_id << '\n';}
    return n_fiber_id;
}

ClassImp(R3BFi7Cal2Hit)
