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
// -----             R3BMusicCalPar source file                 -----
// -----         Created 24/11/19  by J.L. Rodriguez-Sanchez    -----
// ------------------------------------------------------------------

#include "R3BMusicCalPar.h"

#include "FairLogger.h"
#include "FairParamList.h"

#include "TArrayF.h"
#include "TMath.h"
#include "TString.h"

#include <iostream>

// ---- Standard Constructor ---------------------------------------------------
R3BMusicCalPar::R3BMusicCalPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
{
    fAnodeCalParams = new TArrayF(24); // 8 anodes * 3 Calibration-Parameters
    fNumAnodes = 8;
    fNumParamsFit = 3; // Gaussian fit
}

// ----  Destructor ------------------------------------------------------------
R3BMusicCalPar::~R3BMusicCalPar()
{
    clear();
    delete fAnodeCalParams;
}

// ----  Method clear ----------------------------------------------------------
void R3BMusicCalPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BMusicCalPar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BMusicCalPar::putParams() called";
    if (!list)
    {
        return;
    }

    Int_t array_size = fNumAnodes * fNumParamsFit;
    LOG(INFO) << "Array Size: " << array_size;

    fAnodeCalParams->Set(array_size);

    list->add("musicCalPar", *fAnodeCalParams);
    list->add("musicAnodeNumberPar", fNumAnodes);
    list->add("musicAnodeParamsFitPar", fNumParamsFit);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BMusicCalPar::getParams(FairParamList* list)
{
    LOG(INFO) << "R3BMusicCalPar::getParams() called";
    if (!list)
    {
        return kFALSE;
    }

    if (!list->fill("musicAnodeNumberPar", &fNumAnodes))
    {
        return kFALSE;
    }

    if (!list->fill("musicAnodeParamsFitPar", &fNumParamsFit))
    {
        return kFALSE;
    }

    Int_t array_size = fNumAnodes * fNumParamsFit;
    LOG(INFO) << "Array Size: " << array_size;
    fAnodeCalParams->Set(array_size);

    if (!(list->fill("musicCalPar", fAnodeCalParams)))
    {
        LOG(INFO) << "---Could not initialize musicCalPar";
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method printParams ----------------------------------------------------
void R3BMusicCalPar::printParams()
{
    LOG(INFO) << "R3BMusicCalPar: music anode Parameters: ";
    Int_t array_size = fNumAnodes * fNumParamsFit;

    for (Int_t i = 0; i < fNumAnodes; i++)
    {
        LOG(INFO) << "Anode number: " << i;
        for (Int_t j = 0; j < fNumParamsFit; j++)
        {
            LOG(INFO) << "FitParam(" << j << ") = " << fAnodeCalParams->GetAt(i * fNumParamsFit + j);
        }
    }
}
