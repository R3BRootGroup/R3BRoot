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

#include "R3BRpcTotCalPar.h"

#include "FairLogger.h"
#include "FairParamList.h"

#include "TMath.h"
#include "TString.h"

#include <iostream>

// ---- Standard Constructor ---------------------------------------------------
R3BRpcTotCalPar::R3BRpcTotCalPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fNumChannels(N_NUM)
    , fCalParams(new TArrayF(N_NUM))
{
}

// ----  Destructor ------------------------------------------------------------
R3BRpcTotCalPar::~R3BRpcTotCalPar()
{
    clear();
    delete fCalParams;
}

// ----  Method clear ----------------------------------------------------------
void R3BRpcTotCalPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BRpcTotCalPar::putParams(FairParamList* list)
{
    LOG(INFO) << "RpcTotCalPar::putParams() called";
    if (!list)
    {
        return;
    }

    Int_t array_size = fNumChannels;
    LOG(INFO) << "Array Size: " << array_size;

    fCalParams->Set(array_size);

    list->add("RpcTotCalPar", *fCalParams);
    list->add("RPCChannelsNumberPar", fNumChannels);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BRpcTotCalPar::getParams(FairParamList* list)
{
    LOG(INFO) << "RpcTotCalPar::getParams() called";
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

    if (!(list->fill("RpcTotCalPar", fCalParams)))
    {
        LOG(INFO) << "---Could not initialize RpcTotCalPar";
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BRpcTotCalPar::print() { printParams(); }

// ----  Method printParams ----------------------------------------------------
void R3BRpcTotCalPar::printParams()
{
    LOG(INFO) << "RpcTotCalPar::RPC Calibration Parameters: ";
    Int_t array_size = fNumChannels;

    for (Int_t i = 0; i < fNumChannels; i++)
    {
        LOG(INFO) << "Channel number: " << i + 1;
        LOG(INFO) << "Param= " << fCalParams->GetAt(i);
    }
}

ClassImp(R3BRpcTotCalPar)
