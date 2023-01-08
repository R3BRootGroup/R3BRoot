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

// -------------------------------------------------------------------
// -----            R3BAlpideCalData source file                 -----
// -------------------------------------------------------------------

#include "R3BAlpideCalData.h"

R3BAlpideCalData::R3BAlpideCalData()
    : fSenId(0)
    , fCol(0)
    , fRow(0)
{
}

R3BAlpideCalData::R3BAlpideCalData(UShort_t senId, Int_t col, Int_t row)
    : fSenId(senId)
    , fCol(col)
    , fRow(row)
{
}

ClassImp(R3BAlpideCalData);
