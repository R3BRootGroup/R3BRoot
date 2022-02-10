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

// -----------------------------------------------------------------------
// -----            R3BAlpideMappedData source file                  -----
// -----------------------------------------------------------------------

#include "R3BAlpideMappedData.h"

R3BAlpideMappedData::R3BAlpideMappedData()
    : fSenId(0)
    , fReg(0)
    , fCol(0)
    , fAds(0)
{
}

R3BAlpideMappedData::R3BAlpideMappedData(UShort_t senId, UShort_t reg, UShort_t col, UShort_t ads)
    : fSenId(senId)
    , fReg(reg)
    , fCol(col)
    , fAds(ads)
{
}

ClassImp(R3BAlpideMappedData);
