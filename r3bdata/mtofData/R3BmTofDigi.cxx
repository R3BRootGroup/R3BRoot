/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
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
// -----                      R3BmTofPoint source file                  -----
// -------------------------------------------------------------------------

#include "R3BmTofDigi.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;

// -----   Default constructor   -------------------------------------------
R3BmTofDigi::R3BmTofDigi()
{

    Ntmul = 0;
    Ntx = 0.;
    Nty = 0.;
    Ntt = 0.;
    Ntpath = 0.;
    Ntfpx = 0.;
    Ntfpy = 0.;
    Ntfpz = 0.;
    Nte = 0.;
}

R3BmTofDigi::R3BmTofDigi(Int_t ntmul,
                         Double_t ntfx,
                         Double_t ntfy,
                         Double_t ntft,
                         Double_t ntfpath,
                         Double_t ntfpx,
                         // Double_t ntfpy,Double_t ntfpz) {
                         Double_t ntfpy,
                         Double_t ntfpz,
                         Double_t ntfe)
{

    Ntmul = ntmul;
    Ntx = ntfx;
    Nty = ntfy;
    Ntt = ntft;
    Ntpath = ntfpath;
    Ntfpx = ntfpx;
    Ntfpy = ntfpy;
    Ntfpz = ntfpz;
    Nte = ntfe;
}

// -----   Destructor   ----------------------------------------------------
R3BmTofDigi::~R3BmTofDigi() {}

// -----   Public method Print   -------------------------------------------
void R3BmTofDigi::Print(const Option_t*) const {}
// -------------------------------------------------------------------------

ClassImp(R3BmTofDigi)
