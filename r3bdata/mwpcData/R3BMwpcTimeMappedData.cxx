/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ----------------------------------------------------------------------
// -----              R3BMwpcTimeMappedData source file                 -----
// ----------------------------------------------------------------------

#include "R3BMwpcTimeMappedData.h"

// -----   Default constructor   -------------------------------------------
R3BMwpcTimeMappedData::R3BMwpcTimeMappedData()
    : fPlane(0)
    , fTime(0)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BMwpcTimeMappedData::R3BMwpcTimeMappedData(UInt_t plane, UInt_t time)
    : fPlane(plane)
    , fTime(time)
{
}
// -------------------------------------------------------------------------

ClassImp(R3BMwpcTimeMappedData);
