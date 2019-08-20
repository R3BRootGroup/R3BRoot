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

#include "R3BFi3aCal2Hit.h"

R3BFi3aCal2Hit::R3BFi3aCal2Hit(enum R3BTCalEngine::CTDCVariant a_variant,
                               Direction a_direction,
                               Bool_t a_is_calibrator,
                               Int_t a_verbose)
    : R3BBunchedFiberCal2Hit("Fi3a", a_verbose, a_variant, a_direction, 1, 256, 2, a_is_calibrator)
{
}

R3BFi3aCal2Hit::~R3BFi3aCal2Hit() {}

UInt_t R3BFi3aCal2Hit::FixMistake(UInt_t a_fiber_id)
{
    //  if (201 == a_fiber_id) {
    //    a_fiber_id = 200;
    //  }
    //  if (200 == a_fiber_id) {
    //    a_fiber_id = 201;
    //  }
    return a_fiber_id;
}

ClassImp(R3BFi3aCal2Hit)
