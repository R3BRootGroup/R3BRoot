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

#include "R3BRpcHitPar.h"
#include "TMath.h"
#include <iostream>

R3BRpcHitPar::R3BRpcHitPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
{
    clear();
}

void R3BRpcHitPar::putParams(FairParamList* list)
{
    std::cout << "-I- R3BRpcHitPar::putParams() called" << std::endl;

    if (!list)
        return;
    list->add("fExample", (Double_t)fExample);
}

Bool_t R3BRpcHitPar::getParams(FairParamList* list)
{
    std::cout << "-I- R3BRpcHitPar::getParams() called" << std::endl;
    if (!list)
        return kFALSE;
    std::cout << "-I- R3BRpcHitPar::getParams() 1 ";

    if (!list->fill("fExample", &fExample, 1))
        return kFALSE;

    return kTRUE;
}

void R3BRpcHitPar::Print(Option_t* option) const
{
    std::cout << "-I- CALIFA HitFinder Parameters:" << std::endl;
    std::cout << "fExample " << fExample << std::endl;
}

ClassImp(R3BRpcHitPar);
