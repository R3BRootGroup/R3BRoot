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

#include "R3BRoluHitData.h"

// for the data analysis of ROLU following LOS.

R3BRoluHitData::R3BRoluHitData()
    : fDetector(0)
{
}

R3BRoluHitData::R3BRoluHitData(UChar_t detector, Double_t tot_r, Double_t tot_o, Double_t tot_l, Double_t tot_u)

    : fDetector(detector)
    , ftot_r(tot_r)
    , ftot_o(tot_o)
    , ftot_l(tot_l)
    , ftot_u(tot_u)

{
}

ClassImp(R3BRoluHitData)
