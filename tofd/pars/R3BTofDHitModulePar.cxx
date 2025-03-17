/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BTofDHitModulePar.h"

#include <FairLogger.h>
#include <FairParamList.h>

R3BTofDHitModulePar::R3BTofDHitModulePar(const char* name, const char* title, const char* context, Bool_t own)
    : FairParGenericSet(name, title, context, own)
{
}

void R3BTofDHitModulePar::putParams(FairParamList* list)
{
    LOG(info) << "R3BTofDHitModulePar::putParams() called";
    if (!list)
    {
        return;
    }
}

Bool_t R3BTofDHitModulePar::getParams(FairParamList* list)
{
    if (!list)
    {
        return kFALSE;
    }
    return kTRUE;
}

void R3BTofDHitModulePar::printParams()
{
    LOG(info) << "TofD Hit Calibration Parameters for plane " << fPlane << " and paddle " << fPaddle << ":";
    LOG(info) << "   fOffset1: " << fOffset1;
    LOG(info) << "   fOffset2: " << fOffset2;
    LOG(info) << "   fToTOffset1: " << fToTOffset1;
    LOG(info) << "   fToTOffset2: " << fToTOffset2;
    LOG(info) << "   fSync: " << fSync;
    LOG(info) << "   fTofSyncOffset: " << fTofSyncOffset;
    LOG(info) << "   fVeff: " << fVeff;
    LOG(info) << "   fLambda: " << fLambda;
    LOG(info) << "   fDoubleExp1: " << fPar1a << "  " << fPar1b << "  " << fPar1c << "  " << fPar1d;
    LOG(info) << "   fDoubleExp2: " << fPar2a << "  " << fPar2b << "  " << fPar2c << "  " << fPar2d;
    LOG(info) << "   fPol3: " << fPola << "  " << fPolb << "  " << fPolc << "  " << fPold;
    LOG(info) << "   fParaZ: " << fPar1za << "  " << fPar1zb << "  " << fPar1zc;
    LOG(info) << "   fParaWalk: " << fPar1walk << "  " << fPar2walk << "  " << fPar3walk << "  " << fPar4walk << "  "
              << fPar5walk;
}

ClassImp(R3BTofDHitModulePar)
