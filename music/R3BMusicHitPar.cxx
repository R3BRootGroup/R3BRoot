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

// ------------------------------------------------------------------
// -----            R3BMusicHitPar source file                  -----
// -----       Created 24/11/19  by J.L. Rodriguez-Sanchez      -----
// ------------------------------------------------------------------

#include "R3BMusicHitPar.h"

#include "FairLogger.h"
#include "FairParamList.h"

#include "TArrayF.h"
#include "TMath.h"
#include "TString.h"

#include <iostream>

// ---- Standard Constructor ---------------------------------------------------
R3BMusicHitPar::R3BMusicHitPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
{
    fDetHitParams = new TArrayF(4); // 2 Parameters for Z and angle (Linear fits)
    fNumParamsFit = 4;              // 1st order for both, Z and angle
}

// ----  Destructor ------------------------------------------------------------
R3BMusicHitPar::~R3BMusicHitPar()
{
    clear();
    delete fDetHitParams;
}

// ----  Method clear ----------------------------------------------------------
void R3BMusicHitPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BMusicHitPar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BMusicHitPar::putParams() called";
    if (!list)
    {
        return;
    }

    Int_t array_size = fNumParamsFit;
    LOG(INFO) << "Array Size: " << array_size;

    fDetHitParams->Set(array_size);

    list->add("musicHitPar", *fDetHitParams);
    list->add("musicHitParamsFitPar", fNumParamsFit);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BMusicHitPar::getParams(FairParamList* list)
{
    LOG(INFO) << "R3BMusicHitPar::getParams() called";
    if (!list)
    {
        return kFALSE;
    }

    if (!list->fill("musicHitParamsFitPar", &fNumParamsFit))
    {
        return kFALSE;
    }

    Int_t array_size = fNumParamsFit;
    LOG(INFO) << "Array Size: " << array_size;
    fDetHitParams->Set(array_size);

    if (!(list->fill("musicHitPar", fDetHitParams)))
    {
        LOG(INFO) << "---Could not initialize musicHitPar";
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method printParams ----------------------------------------------------
void R3BMusicHitPar::printParams()
{
    LOG(INFO) << "R3BMusicHitPar: music detector Parameters: ";
    Int_t array_size = fNumParamsFit;

    for (Int_t j = 0; j < fNumParamsFit; j++)
    {
        LOG(INFO) << "FitParam(" << j << ") = " << fDetHitParams->GetAt(j);
    }
}
