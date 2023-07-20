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

// -------------------------------------------------------------------------
// -----                      R3BGadastMappedData source file           -----
// -------------------------------------------------------------------------

#include "R3BGadastMappedData.h"

// -----   Default constructor   -------------------------------------------
R3BGadastMappedData::R3BGadastMappedData()
    : fAnodeID(0)
    , fTime(0)
    , fEnergy(0)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BGadastMappedData::R3BGadastMappedData(UShort_t anodeID, UShort_t time, UShort_t energy)
    : fAnodeID(anodeID)
    , fTime(time)
    , fEnergy(energy)
{
}
// -------------------------------------------------------------------------

ClassImp(R3BGadastMappedData);
