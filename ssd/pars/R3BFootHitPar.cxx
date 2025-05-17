/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ------------------------------------------------------------------
// -----                R3BFootHitPar source file               -----
// -----         Created 17/05/25 by J.L. Rodriguez-Sanchez     -----
// ------------------------------------------------------------------

#include "R3BFootHitPar.h"
#include "R3BLogger.h"

#include <FairParamList.h>

#include <TArrayF.h>
#include <TMath.h>
#include <TString.h>

// ---- Standard Constructor ---------------------------------------------------
R3BFootHitPar::R3BFootHitPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
{
    detName = "FootHit";
}

// ----  Destructor ------------------------------------------------------------
R3BFootHitPar::~R3BFootHitPar()
{
    this->clear(); // NOLINT
}

// ----  Method clear ----------------------------------------------------------
void R3BFootHitPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BFootHitPar::putParams(FairParamList* list)
{
    R3BLOG(info, "called");
    if (!list)
    {
        R3BLOG(fatal, "Could not initialize FairParamList");
        return;
    }

    list->add("footDetNbPar", fNumDets);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BFootHitPar::getParams(FairParamList* list)
{
    R3BLOG(info, "called");
    if (!list)
    {
        R3BLOG(fatal, "Could not initialize FairParamList");
        return kFALSE;
    }

    if (!list->fill("footDetNbPar", &fNumDets))
    {
        R3BLOG(error, "Could not initialize footDetNbPar");
        return kFALSE;
    }
    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BFootHitPar::print()
{
    R3BLOG(info, "Foot Hit Parameters");

    for (Int_t d = 0; d < fNumDets; d++)
    {
        R3BLOG(info, "Foot detector number: " << d + 1);
    }
}

ClassImp(R3BFootHitPar)
