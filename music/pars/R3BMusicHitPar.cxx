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
    , fNumParamsZFit(2)
    , fNumAnodes(8)
{
    fDetZHitParams = new TArrayF(fNumParamsZFit); // 2 Parameters for Z (Linear fits)
    fIn_use = new TArrayI(fNumAnodes);
    fAnode_pos = new TArrayF(fNumAnodes);
}

// ----  Destructor ------------------------------------------------------------
R3BMusicHitPar::~R3BMusicHitPar()
{
    clear();
    if (fIn_use)
        delete fIn_use;
    if (fAnode_pos)
        delete fAnode_pos;
    if (fDetZHitParams)
        delete fDetZHitParams;
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

    list->add("musicAnodeNumberPar", fNumAnodes);
    Int_t array_anodes = fNumAnodes;
    LOG(INFO) << "Array Size Anodes: " << array_anodes;
    fIn_use->Set(array_anodes);
    list->add("musicInUsePar", *fIn_use);

    fAnode_pos->Set(array_anodes);
    list->add("musicAnodePosPar", *fAnode_pos);

    list->add("musicZHitParamsFitPar", fNumParamsZFit);
    Int_t array_size = fNumParamsZFit;
    LOG(INFO) << "Number of parameters for charge-Z: " << array_size;
    fDetZHitParams->Set(array_size);
    list->add("musicZHitPar", *fDetZHitParams);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BMusicHitPar::getParams(FairParamList* list)
{
    LOG(INFO) << "R3BMusicHitPar::getParams() called";
    if (!list)
    {
        return kFALSE;
    }

    if (!list->fill("musicAnodeNumberPar", &fNumAnodes))
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

    fAnode_pos->Set(array_anode);
    if (!(list->fill("musicAnodePosPar", fAnode_pos)))
    {
        LOG(INFO) << "---Could not initialize musicAnodePosPar";
        return kFALSE;
    }

    if (!list->fill("musicZHitParamsFitPar", &fNumParamsZFit))
    {
        return kFALSE;
    }
    Int_t array_size = fNumParamsZFit;
    LOG(INFO) << "Array Size: " << array_size;
    fDetZHitParams->Set(array_size);
    if (!(list->fill("musicZHitPar", fDetZHitParams)))
    {
        LOG(INFO) << "---Could not initialize musicZHitPar";
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method printParams ----------------------------------------------------
void R3BMusicHitPar::printParams()
{
    LOG(INFO) << "R3BMusicHitPar: music detector Parameters";
    LOG(INFO) << "R3BMusicHitPar: music anodes in use: ";
    for (Int_t j = 0; j < fNumAnodes; j++)
    {
        LOG(INFO) << "Anode " << j + 1 << " in use " << fIn_use->GetAt(j) << ", Position-Z: " << fAnode_pos->GetAt(j);
    }

    LOG(INFO) << "FitParam for charge-Z: " << fNumParamsZFit;
    for (Int_t j = 0; j < fNumParamsZFit; j++)
    {
        LOG(INFO) << "FitParam (" << j + 1 << ") = " << fDetZHitParams->GetAt(j);
    }
}
