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
    , fNumAnodes(8)
    , fNumParamsEFit(3) // Gaussian fit
    , fNumParamsPosFit(2)
{
    fAnodeCalParams = new TArrayF(fNumAnodes * fNumParamsEFit); // 8 anodes * 3 Calibration-Parameters
    fPosParams = new TArrayF(fNumAnodes * fNumParamsPosFit);
    fIn_use = new TArrayI(fNumAnodes);
}

// ----  Destructor ------------------------------------------------------------
R3BMusicCalPar::~R3BMusicCalPar()
{
    clear();
    if (fIn_use)
        delete fIn_use;
    if (fAnodeCalParams)
        delete fAnodeCalParams;
    if (fPosParams)
        delete fPosParams;
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

    list->add("musicAnodeNumberPar", fNumAnodes);
    list->add("musicAnodeEFitPar", fNumParamsEFit);

    fIn_use->Set(fNumAnodes);
    list->add("musicInUsePar", *fIn_use);

    Int_t array_size = fNumAnodes * fNumParamsEFit;
    LOG(INFO) << "Array Size: " << array_size;
    fAnodeCalParams->Set(array_size);
    list->add("musicCalEPar", *fAnodeCalParams);

    list->add("musicPosFitPar", fNumParamsPosFit);
    Int_t array_pos = fNumAnodes * fNumParamsPosFit;
    LOG(INFO) << "Array Size: " << array_pos;
    fPosParams->Set(array_pos);
    list->add("musicPosPar", *fPosParams);
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

    if (!list->fill("musicAnodeEFitPar", &fNumParamsEFit))
    {
        return kFALSE;
    }

    Int_t array_anode = fNumAnodes;
    LOG(INFO) << "Array Size: " << array_anode;
    fIn_use->Set(array_anode);
    if (!(list->fill("musicInUsePar", fIn_use)))
    {
        LOG(INFO) << "---Could not initialize musicInUsePar";
        return kFALSE;
    }

    Int_t array_size = fNumAnodes * fNumParamsEFit;
    LOG(INFO) << "Parameters for E calibration: " << array_size;
    fAnodeCalParams->Set(array_size);

    if (!(list->fill("musicCalEPar", fAnodeCalParams)))
    {
        LOG(INFO) << "---Could not initialize musicCalEPar";
        return kFALSE;
    }

    if (!list->fill("musicPosFitPar", &fNumParamsPosFit))
    {
        return kFALSE;
    }

    Int_t array_pos = fNumAnodes * fNumParamsPosFit;
    LOG(INFO) << "Parameters for Position calibration: " << array_pos;
    fPosParams->Set(array_pos);

    if (!(list->fill("musicPosPar", fPosParams)))
    {
        LOG(INFO) << "---Could not initialize musicPosPar";
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method printParams ----------------------------------------------------
void R3BMusicCalPar::printParams()
{
    LOG(INFO) << "R3BMusicCalPar::Music anode parameters for energy: " << fNumAnodes * fNumParamsEFit;

    for (Int_t i = 0; i < fNumAnodes; i++)
    {
        LOG(INFO) << "Anode number: " << i << " in use " << fIn_use->GetAt(i);
        for (Int_t j = 0; j < fNumParamsEFit; j++)
        {
            LOG(INFO) << "FitParam(" << j << ") = " << fAnodeCalParams->GetAt(i * fNumParamsEFit + j);
        }
    }

    LOG(INFO) << "R3BMusicCalPar::Music anode parameters for position: " << fNumAnodes * fNumParamsPosFit;

    for (Int_t i = 0; i < fNumAnodes; i++)
    {
        LOG(INFO) << "Anode number: " << i;
        for (Int_t j = 0; j < fNumParamsPosFit; j++)
        {
            LOG(INFO) << "FitParam(" << j << ") = " << fPosParams->GetAt(i * fNumParamsPosFit + j);
        }
    }
}
