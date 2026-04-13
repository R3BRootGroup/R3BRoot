/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BPWHitPar.h"

#include "FairLogger.h"
#include "FairParamList.h"

#include "TMath.h"
#include "TString.h"

#include <iostream>

// ---- Standard Constructor ---------------------------------------------------
R3BPWHitPar::R3BPWHitPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fNumChannels(N_BAR)
    , fHitCalParams(new TArrayF(N_BAR))
{
}

// ----  Destructor ------------------------------------------------------------
R3BPWHitPar::~R3BPWHitPar() { delete fHitCalParams; }

// ----  Method clear ----------------------------------------------------------
void R3BPWHitPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BPWHitPar::putParams(FairParamList* list)
{
    LOG(info) << "PWHitPar::putParams() called";
    if (!list)
    {
        return;
    }

    Int_t array_size = fNumChannels;
    LOG(info) << "Array Size: " << array_size;

    fHitCalParams->Set(array_size);

    list->add("PWHitPar", *fHitCalParams);
    list->add("PWChannelsNumberPar", fNumChannels);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BPWHitPar::getParams(FairParamList* list)
{
    LOG(info) << "PWHitPar::getParams() called";
    if (!list)
    {
        return kFALSE;
    }

    if (!list->fill("PWChannelsNumberPar", &fNumChannels))
    {
        return kFALSE;
    }

    Int_t array_size = fNumChannels;
    LOG(info) << "N_channels: " << array_size;
    fHitCalParams->Set(array_size);

    if (!(list->fill("PWHitPar", fHitCalParams)))
    {
        LOG(info) << "---Could not initialize PWHitPar";
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BPWHitPar::print() { printParams(); }

// ----  Method printParams ----------------------------------------------------
void R3BPWHitPar::printParams()
{
    LOG(info) << "PWHitPar::PW Calibration Parameters: ";

    for (Int_t i = 0; i < fNumChannels; i++)
    {
        std::cout << "Parameter  " << i + 1 << "  " << fHitCalParams->GetAt(i) << std::endl;
        LOG(info) << "Channel number: " << i + 1;
        LOG(info) << "Param= " << fHitCalParams->GetAt(i);
    }
}

ClassImp(R3BPWHitPar)
