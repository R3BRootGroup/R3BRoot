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

#include "R3BFi3bCal2Hit.h"
#include "mapping_fib3b_trig.hh"

R3BFi3bCal2Hit::R3BFi3bCal2Hit(enum R3BTCalEngine::CTDCVariant a_variant,
                               Direction a_direction,
                               Bool_t a_is_calibrator,
                               Bool_t a_is_gain,
                               Bool_t a_is_tsync,
                               Int_t a_verbose)
    : R3BBunchedFiberCal2Hit("Fi3b",
                             a_verbose,
                             a_variant,
                             a_direction,
                             1,
                             256,
                             2,
                             a_is_calibrator,
                             a_is_gain,
                             a_is_tsync)
{
    fib3b_trig_map_setup();
    BUNCHED_FIBER_TRIGGER_MAP_SET(g_fib3bm_trig_map, g_fib3bs_trig_map);
}

R3BFi3bCal2Hit::~R3BFi3bCal2Hit() {}

UInt_t R3BFi3bCal2Hit::FixMistake(UInt_t a_fiber_id)
{
    //  if (201 == a_fiber_id) {
    //    a_fiber_id = 200;
    //  }
    //  if (200 == a_fiber_id) {
    //    a_fiber_id = 201;
    //  }
    return a_fiber_id;
}

ClassImp(R3BFi3bCal2Hit)
