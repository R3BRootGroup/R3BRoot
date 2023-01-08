/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BStartrackHitPar.h"
#include "FairLogger.h"
#include "TMath.h"

R3BStartrackHitPar::R3BStartrackHitPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
{
    clear();
}

void R3BStartrackHitPar::putParams(FairParamList* list)
{
    LOG(info) << "-I- R3BStartrackHitPar::putParams() called";
    if (!list)
        return;
    list->add("fThreshold", (Double_t)fThreshold);
    list->add("fEStripResolution", (Double_t)fEStripResolution);

    //  list->add("max_paddle", (Int_t)nMaxPaddle);
    //  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BStartrackHitPar::getParams(FairParamList* list)
{
    LOG(info) << "-I- R3BStartrackHitPar::getParams() called";
    if (!list)
        return kFALSE;
    LOG(info) << "-I- R3BStartrackHitPar::getParams() 1 ";

    if (!list->fill("fThreshold", &fThreshold, 1))
        return kFALSE;
    if (!list->fill("fEStripResolution", &fEStripResolution, 1))
        return kFALSE;

    //  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
    //  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
    return kTRUE;
}

void R3BStartrackHitPar::Print(Option_t* option) const
{
    LOG(info) << "-I- STaRTracker Hit Parameters:";
    LOG(info) << " Energy Threshold   = " << fThreshold;
    LOG(info) << " Energy resolution  = " << fEStripResolution;

    //    LOG(info)<<"   Max Paddle   = "<<nMaxPaddle;
    //    LOG(info)<<"   Max Plane   = "<<nMaxPlane;
}

ClassImp(R3BStartrackHitPar);
