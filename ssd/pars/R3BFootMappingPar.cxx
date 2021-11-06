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

// ----------------------------------------------------------------
// -----        R3BFootMappingPar source file                 -----
// -----    Created 05/11/21  by J.L. Rodriguez-Sanchez       -----
// -----    for the info from the lookup table	        -----
// ----------------------------------------------------------------

#include "R3BFootMappingPar.h"

#include "FairLogger.h"
#include "FairParamList.h"

#include "TMath.h"
#include "TString.h"

#include <iostream>

// ---- Standard Constructor ---------------------------------------------------
R3BFootMappingPar::R3BFootMappingPar(const TString& name, const TString& title, const TString& context)
    : FairParGenericSet(name, title, context)
    , fNumDet(10)
    , fGeo(202111)
{
    fDistance2target = new TArrayF(fNumDet);
    fAngleTheta = new TArrayF(fNumDet);
    fOffsetY = new TArrayF(fNumDet);
}

// ----  Destructor ------------------------------------------------------------
R3BFootMappingPar::~R3BFootMappingPar()
{
    clear();
    if (fDistance2target)
        delete fDistance2target;
    if (fAngleTheta)
        delete fAngleTheta;
    if (fOffsetY)
        delete fOffsetY;
}

// ----  Method clear ----------------------------------------------------------
void R3BFootMappingPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BFootMappingPar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BFootMappingPar::putParams() called";
    if (!list)
    {
        return;
    }
    list->add("footGeoPar", fGeo);
    list->add("footDetPar", fNumDet);
    fDistance2target->Set(fNumDet);
    list->add("footDistance2targetPar", *fDistance2target);
    fAngleTheta->Set(fNumDet);
    list->add("footAngleThetaPar", *fAngleTheta);
    fOffsetY->Set(fNumDet);
    list->add("footOffsetYPar", *fOffsetY);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BFootMappingPar::getParams(FairParamList* list)
{
    LOG(INFO) << "R3BFootMappingPar::getParams() called";
    if (!list)
    {
        return kFALSE;
    }
    if (!list->fill("footGeoPar", &fGeo))
    {
        LOG(INFO) << "Could not initialize footGeoPar";
        return kFALSE;
    }
    if (!list->fill("footDetPar", &fNumDet))
    {
        LOG(INFO) << "Could not initialize footDetPar";
        return kFALSE;
    }
    fDistance2target->Set(fNumDet);
    if (!(list->fill("footDistance2targetPar", fDistance2target)))
    {
        LOG(INFO) << "Could not initialize footDistance2targetPar";
        return kFALSE;
    }
    fAngleTheta->Set(fNumDet);
    if (!(list->fill("footAngleThetaPar", fAngleTheta)))
    {
        LOG(INFO) << "Could not initialize footAngleThetaPar";
        return kFALSE;
    }
    fOffsetY->Set(fNumDet);
    if (!(list->fill("footOffsetYPar", fOffsetY)))
    {
        LOG(INFO) << "Could not initialize footOffsetYPar";
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BFootMappingPar::print() { printParams(); }

// ----  Method printParams ----------------------------------------------------
void R3BFootMappingPar::printParams()
{
    LOG(INFO) << "R3BFootMappingPar::FOOT mapping parameters for geometry " << fGeo << ": ";

    LOG(INFO) << "Detector"
              << " "
              << "Distance_to_target"
              << " "
              << "Angle_theta"
              << " "
              << "Offset_Y";

    for (Int_t i = 0; i < fNumDet; i++)
    {
        LOG(INFO) << i + 1 << "\t"
                  << "\t" << fDistance2target->GetAt(i) << "\t\t" << fAngleTheta->GetAt(i) << "\t"
                  << fOffsetY->GetAt(i);
    }
}

ClassImp(R3BFootMappingPar);
