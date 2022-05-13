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

#include "R3BFi7DigiPar.h"
#include "TMath.h"
#include <iostream>

R3BFi7DigiPar::R3BFi7DigiPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
{
    clear();
}

void R3BFi7DigiPar::putParams(FairParamList* list)
{
    std::cout << "-I- R3BFi7DigiPar::putParams() called" << std::endl;
    if (!list)
        return;
    //  list->add("max_paddle", (Int_t)nMaxPaddle);
    //  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BFi7DigiPar::getParams(FairParamList* list)
{
    std::cout << "-I- R3BFi7DigiPar::getParams() called" << std::endl;
    if (!list)
        return kFALSE;
    std::cout << "-I- R3BFi7DigiPar::getParams() 1 ";

    //  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
    //  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
    return kTRUE;
}

void R3BFi7DigiPar::Print(Option_t* option) const
{
    std::cout << "-I- Fi7 Digi Parameters:" << std::endl;
    //    std::cout<<"   Max Paddle   = "<<nMaxPaddle<<std::endl;
    //    std::cout<<"   Max Plane   = "<<nMaxPlane<<std::endl;
}

ClassImp(R3BFi7DigiPar);
