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

#include "R3BFi0Cal2Hit.h"
#include "mapping_fib0_trig.hh"

R3BFi0Cal2Hit::R3BFi0Cal2Hit(Bool_t a_is_calibrator, Bool_t a_is_gain, Bool_t a_is_tsync, Int_t a_verbose)
    : R3BBunchedFiberCal2Hit("Fi0",
                             a_verbose,
                             R3BTCalEngine::CTDC_16_BWD_150,
                             VERTICAL,
                             1,
                             128,
                             0,
                             a_is_calibrator,
                             a_is_gain,
                             a_is_tsync)
{
    fib0_trig_map_setup();
    BUNCHED_FIBER_TRIGGER_MAP_SET(g_fib0m_trig_map, g_fib0s_trig_map);	
}

R3BFi0Cal2Hit::~R3BFi0Cal2Hit() {}

UInt_t R3BFi0Cal2Hit::FixMistake(UInt_t a_fiber_id)
{
    //  if (201 == a_fiber_id) {
    //    a_fiber_id = 200;
    //  }
    //  if (200 == a_fiber_id) {
    //    a_fiber_id = 201;
    //  }
    return a_fiber_id;
}

ClassImp(R3BFi0Cal2Hit)
