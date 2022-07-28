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
// -----                      R3BTwimHitData source file               -----
// -------------------------------------------------------------------------

#include "R3BTwimHitData.h"

// -----   Default constructor   -------------------------------------------
R3BTwimHitData::R3BTwimHitData()
    : fSecID(0)
    , fTheta(0.)
    , fZ(0.)
    , fX(0.)
    , fOffset(0.)
    , fE(0.)
    , fVertex(0.)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BTwimHitData::R3BTwimHitData(UInt_t secID, Double_t theta, Double_t z, Double_t x, Double_t offset, Double_t ene, Double_t vertex)
    : fSecID(secID)
    , fTheta(theta)
    , fZ(z)
    , fX(x)
    , fOffset(offset)
    , fE(ene)
    , fVertex(vertex)
{
}
// -------------------------------------------------------------------------

ClassImp(R3BTwimHitData);
