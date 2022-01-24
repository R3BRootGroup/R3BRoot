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

#include "R3BLosHitPar.h"
#include "FairLogger.h"
#include "FairParamList.h"
#include "TMath.h"
#include "TString.h"

#include <iostream>

// ---- Standard Constructor ---------------------------------------------------
R3BLosHitPar::R3BLosHitPar(const TString& name, const TString& title, const TString& context)
    : FairParGenericSet(name, title, context)
{
}

// ----  Destructor ------------------------------------------------------------
R3BLosHitPar::~R3BLosHitPar() { clear(); }

// ----  Method clear ----------------------------------------------------------
void R3BLosHitPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BLosHitPar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BLosHitPar::putParams() called";
    if (!list)
    {
        return;
    }
    list->add("p0", fp0);
    list->add("p1", fp1);
    list->add("xoffset_MCFD", fxoffset_MCFD);
    list->add("yoffset_MCFD", fyoffset_MCFD);
    list->add("xveff_MCFD", fxveff_MCFD);
    list->add("yveff_MCFD", fyveff_MCFD);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BLosHitPar::getParams(FairParamList* list)
{
    LOG(INFO) << "R3BLosHitPar::getParams() called";
    if (!list)
    {
        return kFALSE;
    }

    if (!(list->fill("p0", &fp0)))
    {
        LOG(INFO) << "---Could not initialize LosHit p0 Par";
        return kFALSE;
    }

    if (!(list->fill("p1", &fp1)))
    {
        LOG(INFO) << "---Could not initialize LosHit p1 Par";
        return kFALSE;
    }

    if (!(list->fill("xoffset_MCFD", &fxoffset_MCFD)))
    {
        LOG(INFO) << "---Could not initialize LosHit xoffset_MCFD Par";
        return kFALSE;
    }

    if (!(list->fill("yoffset_MCFD", &fyoffset_MCFD)))
    {
        LOG(INFO) << "---Could not initialize LosHit yoffset_MCFD Par";
        return kFALSE;
    }

    if (!(list->fill("xveff_MCFD", &fxveff_MCFD)))
    {
        LOG(INFO) << "---Could not initialize LosHit xveff_MCFD Par";
        return kFALSE;
    }

    if (!(list->fill("yveff_MCFD", &fyveff_MCFD)))
    {
        LOG(INFO) << "---Could not initialize LosHit yveff_MCFD Par";
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BLosHitPar::print() { printParams(); }

// ----  Method printParams ----------------------------------------------------
void R3BLosHitPar::printParams()
{
    LOG(INFO) << "R3BLosHitPar: Los calibration parameters (Z-charge):";

    LOG(INFO) << "p0"
              << " "
              << "p1"
              << " ";

    LOG(INFO) << fp0 << "\t" << fp1;
}

ClassImp(R3BLosHitPar)
