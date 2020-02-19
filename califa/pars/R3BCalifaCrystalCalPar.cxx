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

#include "R3BCalifaCrystalCalPar.h"

#include "FairLogger.h"
#include "FairParamList.h"

#include "TMath.h"
#include "TString.h"

#include <iostream>

R3BCalifaCrystalCalPar::R3BCalifaCrystalCalPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fNumCrystals(4864)
    , fNumParamsFit(2) // pol1
{
    fCryCalParams = new TArrayF(fNumCrystals * fNumParamsFit);
}

R3BCalifaCrystalCalPar::~R3BCalifaCrystalCalPar()
{
    clear();
    if (fCryCalParams)
        delete fCryCalParams;
}

void R3BCalifaCrystalCalPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

void R3BCalifaCrystalCalPar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BCalifaCrystalCalPar::putParams() called";
    if (!list)
    {
        return;
    }

    Int_t array_size = fNumCrystals * fNumParamsFit;
    LOG(INFO) << "Array Size: " << array_size;

    fCryCalParams->Set(array_size);

    list->add("califaCrystalCalPar", *fCryCalParams);
    list->add("califaCrystalNumberPar", fNumCrystals);
    list->add("califaCrystalParamsFitPar", fNumParamsFit);
}

Bool_t R3BCalifaCrystalCalPar::getParams(FairParamList* list)
{
    LOG(INFO) << "R3BCalifaCrystalCalPar::getParams() called";
    if (!list)
    {
        return kFALSE;
    }

    if (!list->fill("califaCrystalNumberPar", &fNumCrystals))
    {
        return kFALSE;
    }

    if (!list->fill("califaCrystalParamsFitPar", &fNumParamsFit))
    {
        return kFALSE;
    }

    Int_t array_size = fNumCrystals * fNumParamsFit;
    LOG(INFO) << "Array Size: " << array_size;
    fCryCalParams->Set(array_size);

    if (!(list->fill("califaCrystalCalPar", fCryCalParams)))
    {
        LOG(INFO) << "---Could not initialize califaCrystalCalPar";
        return kFALSE;
    }

    return kTRUE;
}

void R3BCalifaCrystalCalPar::printParams()
{
    LOG(INFO) << "R3BCalifaCrystalCalPar: Califa Crystal Calibration Parameters: ";
    Int_t array_size = fNumCrystals * fNumParamsFit;

    for (Int_t i = 0; i < fNumCrystals; i++)
    {
        LOG(INFO) << "Califa Crystal number: " << i + 1;
        for (Int_t j = 0; j < fNumParamsFit; j++)
        {
            LOG(INFO) << "FitParam(" << j << ") = " << fCryCalParams->GetAt(i * fNumParamsFit + j);
        }
    }
}
