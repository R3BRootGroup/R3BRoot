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

#include "R3BHFIBTotCalPar.h"

#include "FairLogger.h"
#include "FairParamList.h"

#include "TMath.h"
#include "TString.h"

#include <iostream>

// ---- Standard Constructor ---------------------------------------------------
R3BHFIBTotCalPar::R3BHFIBTotCalPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fNumChannels(N_NUM)
    , fCalParams(new TArrayF(N_NUM))
{
}

// ----  Destructor ------------------------------------------------------------
R3BHFIBTotCalPar::~R3BHFIBTotCalPar() { delete fCalParams; }

// ----  Method clear ----------------------------------------------------------
void R3BHFIBTotCalPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BHFIBTotCalPar::putParams(FairParamList* list)
{
    LOG(info) << "HFIBTotCalPar::putParams() called";
    if (!list)
    {
        return;
    }

    Int_t array_size = fNumChannels;
    LOG(info) << "Array Size: " << array_size;

    fCalParams->Set(array_size);

    list->add("HFIBTotCalPar", *fCalParams);
    list->add("HFIBChannelsNumberPar", fNumChannels);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BHFIBTotCalPar::getParams(FairParamList* list)
{
    LOG(info) << "HFIBTotCalPar::getParams() called";
    if (!list)
    {
        return kFALSE;
    }

    if (!list->fill("HFIBChannelsNumberPar", &fNumChannels))
    {
        return kFALSE;
    }

    Int_t array_size = fNumChannels;
    LOG(info) << "N_channels: " << array_size;
    fCalParams->Set(array_size);

    if (!(list->fill("HFIBTotCalPar", fCalParams)))
    {
        LOG(info) << "---Could not initialize HFIBTotCalPar";
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BHFIBTotCalPar::print() { printParams(); }

// ----  Method printParams ----------------------------------------------------
void R3BHFIBTotCalPar::printParams()
{
    LOG(info) << "HFIBTotCalPar::HFIB Calibration Parameters: ";

    for (Int_t i = 0; i < fNumChannels; i++)
    {
        LOG(info) << "Channel number: " << i + 1;
        LOG(info) << "Param= " << fCalParams->GetAt(i);
    }
}

ClassImp(R3BHFIBTotCalPar)
