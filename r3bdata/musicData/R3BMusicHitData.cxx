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

// -------------------------------------------------------------------------
// -----                      R3BMusicHitData source file              -----
// -------------------------------------------------------------------------

#include "R3BMusicHitData.h"

// -----   Default constructor   -------------------------------------------
R3BMusicHitData::R3BMusicHitData()
    : fTheta(0.)
    , fZ(0.)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BMusicHitData::R3BMusicHitData(Double_t theta, Double_t z)
    : fTheta(theta)
    , fZ(z)
{
}
// -------------------------------------------------------------------------

ClassImp(R3BMusicHitData)
