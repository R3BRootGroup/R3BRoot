/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
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
// -----    for the info from the lookup table	              -----
// -----    Modified 08/2025 by Pablo González Rusell         -----
// ----------------------------------------------------------------

#include "R3BFootMappingPar.h"
#include "R3BLogger.h"

#include "FairLogger.h"
#include "FairParamList.h"

#include "TMath.h"
#include "TString.h"

#include <iostream>

// ---- Standard Constructor ---------------------------------------------------
R3BFootMappingPar::R3BFootMappingPar(const TString& name, const TString& title, const TString& context)
    : FairParGenericSet(name, title, context)
    , fNumDet(20)
    , fPolPar(5)
    , fGeo(202111)
{
    fDistance2target = new TArrayF(fNumDet);
    fAngleTheta = new TArrayF(fNumDet);
    fAnglePhi = new TArrayF(fNumDet);
    fAnglePsi = new TArrayF(fNumDet);
    fAnglePsi->Reset(0.0);
    fOffsetX = new TArrayF(fNumDet);
    fOffsetY = new TArrayF(fNumDet);
    fEnevsPosCorr = new TArrayF(fPolPar * fNumDet);
}

// ----  Destructor ------------------------------------------------------------
R3BFootMappingPar::~R3BFootMappingPar()
{
    clear();
    if (fDistance2target)
        delete fDistance2target;
    if (fAngleTheta)
        delete fAngleTheta;
    if (fAnglePhi)
        delete fAnglePhi;
    if (fAnglePsi)
        delete fAnglePsi;
    if (fOffsetX)
        delete fOffsetX;
    if (fOffsetY)
        delete fOffsetY;
    if (fEnevsPosCorr)
        delete fEnevsPosCorr;
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
    LOG(info) << "R3BFootMappingPar::putParams() called";
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
    fAnglePhi->Set(fNumDet);
    list->add("footAnglePhiPar", *fAnglePhi);
    fAnglePsi->Set(fNumDet);
    list->add("footAnglePsiPar", *fAnglePsi);
    fOffsetX->Set(fNumDet);
    list->add("footOffsetXPar", *fOffsetX);
    fOffsetY->Set(fNumDet);
    list->add("footOffsetYPar", *fOffsetY);
    list->add("footPolPar", fPolPar);
    fEnevsPosCorr->Set(fNumDet * fPolPar);
    list->add("footEnevsPosCorrPar", *fEnevsPosCorr);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BFootMappingPar::getParams(FairParamList* list)
{
    R3BLOG(info, "getParams() called");
    if (!list)
    {
        return kFALSE;
    }
    if (!list->fill("footGeoPar", &fGeo))
    {
        R3BLOG(info, "Could not initialize footGeoPar");
        return kFALSE;
    }
    if (!list->fill("footDetPar", &fNumDet))
    {
        R3BLOG(info, "Could not initialize footDetPar");
        return kFALSE;
    }
    fDistance2target->Set(fNumDet);
    if (!(list->fill("footDistance2targetPar", fDistance2target)))
    {
        R3BLOG(info, "Could not initialize footDistance2targetPar");
        return kFALSE;
    }
    fAngleTheta->Set(fNumDet);
    if (!(list->fill("footAngleThetaPar", fAngleTheta)))
    {
        R3BLOG(info, "Could not initialize footAngleThetaPar");
        return kFALSE;
    }
    fAnglePhi->Set(fNumDet);
    if (!(list->fill("footAnglePhiPar", fAnglePhi)))
    {
        R3BLOG(info, "Could not initialize footAnglePhiPar");
        return kFALSE;
    }

    fAnglePsi->Set(fNumDet);
    if (!(list->fill("footAnglePsiPar", fAnglePsi)))
    {
        R3BLOG(info, "Could not initialize footAnglePsiPar. (Setting to zero by default)");
        fAnglePsi->Reset(0.0);
    }

    fOffsetX->Set(fNumDet);
    if (!(list->fill("footOffsetXPar", fOffsetX)))
    {
        R3BLOG(info, "Could not initialize footOffsetXPar");
        return kFALSE;
    }
    fOffsetY->Set(fNumDet);
    if (!(list->fill("footOffsetYPar", fOffsetY)))
    {
        R3BLOG(info, "Could not initialize footOffsetYPar");
        return kFALSE;
    }
    if (!list->fill("footPolPar", &fPolPar))
    {
        R3BLOG(info, "Could not initialize footPolPar");
        return kFALSE;
    }
    fEnevsPosCorr->Set(fNumDet * fPolPar);
    if (!(list->fill("footEnevsPosCorrPar", fEnevsPosCorr)))
    {
        R3BLOG(info, "Could not initialize footEnevsPosCorrPar");
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BFootMappingPar::print() { printParams(); }

// ----  Method printParams ----------------------------------------------------
void R3BFootMappingPar::printParams()
{
    R3BLOG(info, Form("FOOT mapping parameters for geometry %i:", fGeo));

    R3BLOG(info,
           "Detector"
           " Distance_to_target"
           " Angle_theta"
           " Angle_Phi"
           " Angle_Psi"
           " Offset_X"
           " Offset_Y");

    for (Int_t i = 0; i < fNumDet; i++)
    {
        R3BLOG(info,
               TString::Format("%d\t\t%.2f\t\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f",
                               i + 1,
                               fDistance2target->GetAt(i),
                               fAngleTheta->GetAt(i),
                               fAnglePhi->GetAt(i),
                               fAnglePsi->GetAt(i),
                               fOffsetX->GetAt(i),
                               fOffsetY->GetAt(i)));
    }

    R3BLOG(info, "Energy vs Position Correction Parameters :");

    for (Int_t i = 0; i < fNumDet; i++)
    {
        R3BLOG(info, TString::Format("FOOT %d :", i + 1));

        for (Int_t j = 0; j < fPolPar; j++)
        {
            R3BLOG(info, TString::Format("Pol%d\t\t%.6f", j, fEnevsPosCorr->GetAt(i * fPolPar + j)));
        }
    }
}

ClassImp(R3BFootMappingPar);
