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

// -------------------------------------------------------------------
// -----            R3BAlpideCalData source file                 -----
// -------------------------------------------------------------------

#include "R3BAlpideCalData.h"

R3BAlpideCalData::R3BAlpideCalData()
    : fSenId(0)
    , fPosl(-4.0)
    , fPost(-4.0)
{
}

R3BAlpideCalData::R3BAlpideCalData(UShort_t senId, Float_t posl, Float_t post)
    : fSenId(senId)
    , fPosl(posl)
    , fPost(post)
{
}

ClassImp(R3BAlpideCalData);
