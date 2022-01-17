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

#include "R3BRpcCalPar.h"

#include "FairLogger.h"
#include "FairParamList.h"

#include "TMath.h"
#include "TString.h"

#include <iostream>

// ---- Standard Constructor ---------------------------------------------------
R3BRpcCalPar::R3BRpcCalPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fNumChannels(64)
{
}

// ----  Destructor ------------------------------------------------------------
R3BRpcCalPar::~R3BRpcCalPar()
{
    clear();
    if (fCalParams)
        delete fCalParams;
}

// ----  Method clear ----------------------------------------------------------
void R3BRpcCalPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BRpcCalPar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BRpcCalPar::putParams() called";
    if (!list)
    {
        return;
    }

    Int_t array_size = fNumChannels;
    LOG(INFO) << "Array Size: " << array_size;

    fCalParams->Set(array_size);

    list->add("RPCCalPar", *fCalParams);
    list->add("RPCChannelsNumberPar", fNumChannels);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BRpcCalPar::getParams(FairParamList* list)
{
    LOG(INFO) << "R3BRpcCalPar::getParams() called";
    if (!list)
    {
        return kFALSE;
    }

    if (!list->fill("RPCChannelsNumberPar", &fNumChannels))
    {
        return kFALSE;
    }

    Int_t array_size = fNumChannels;
    LOG(INFO) << "Nb_channels: " << array_size;
    fCalParams->Set(array_size);

    if (!(list->fill("RPCCalPar", fCalParams)))
    {
        LOG(INFO) << "---Could not initialize RPCCalPar";
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BRpcCalPar::print() { printParams(); }

// ----  Method printParams ----------------------------------------------------
void R3BRpcCalPar::printParams()
{
    LOG(INFO) << "R3BRpcCalPar::RPC Calibration Parameters: ";
    Int_t array_size = fNumChannels;

    for (Int_t i = 0; i < fNumChannels; i++)
    {
        LOG(INFO) << "Channel number: " << i + 1;
        LOG(INFO) << "Param= " << fCalParams->GetAt(i);
    }
}

ClassImp(R3BRpcCalPar)
