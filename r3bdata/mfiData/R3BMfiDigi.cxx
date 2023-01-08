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
// -----                      R3BMfiPoint source file                  -----
// -------------------------------------------------------------------------

#include "R3BMfiDigi.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;

// -----   Default constructor   -------------------------------------------
R3BMfiDigi::R3BMfiDigi()
    : Mf1mul(0)
    , Mf1x(0.)
    , Mf01ni(0)
{
}

R3BMfiDigi::R3BMfiDigi(Int_t mf1mul, Double_t mf1x, Int_t fiber)
    : Mf1mul(mf1mul)
    , Mf1x(mf1x)
    , Mf01ni(fiber)
{
}

R3BMfiDigi::R3BMfiDigi(const R3BMfiDigi& right)
    : Mf1mul(right.Mf1mul)
    , Mf1x(right.Mf1x)
    , Mf01ni(right.Mf01ni)
{
}

// -----   Destructor   ----------------------------------------------------
R3BMfiDigi::~R3BMfiDigi() {}

// -----   Public method Print   -------------------------------------------
void R3BMfiDigi::Print(const Option_t* opt) const {}
// -------------------------------------------------------------------------

ClassImp(R3BMfiDigi)
