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

#include "R3BMusliCalPar.h"
#include "R3BLogger.h"

#include "FairDetParIo.h"
#include "FairLogger.h"
#include "FairParamList.h"

#include "TArrayF.h"
#include "TArrayI.h"
#include "TMath.h"
#include "TString.h"

// ---- Standard Constructor ---------------------------------------------------
R3BMusliCalPar::R3BMusliCalPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fNumSignals(18)
    , fNumGroupsAnodes(15)
    , fNumParamsEneFit(2)
    , fNumParamsPosFit(2)
    , fMaxMult(20)
{
    fIn_use = new TArrayI(fNumSignals);
    fEneCalParams = new TArrayF(fNumGroupsAnodes * fNumParamsEneFit);
    fPosCalParams = new TArrayF(fNumGroupsAnodes * fNumParamsPosFit);
}

// ----  Destructor ------------------------------------------------------------
R3BMusliCalPar::~R3BMusliCalPar()
{
    clear();
    if (fIn_use)
        delete fIn_use;
    if (fEneCalParams)
        delete fEneCalParams;
    if (fPosCalParams)
        delete fPosCalParams;
}

// ----  Method clear ----------------------------------------------------------
void R3BMusliCalPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BMusliCalPar::putParams(FairParamList* list)
{
    R3BLOG(INFO, "called");
    if (!list)
    {
        R3BLOG(FATAL, "Could not find FairParamList");
        return;
    }

    list->add("musliNumSignalsPar", fNumSignals);
    list->add("musliNumGroupsAnodesPar", fNumGroupsAnodes);
    list->add("musliMaxMultPar", fMaxMult);
    list->add("musliNumParamsEneFitPar", fNumParamsEneFit);
    list->add("musliNumParamsPosFitPar", fNumParamsPosFit);

    fIn_use->Set(fNumGroupsAnodes);
    list->add("musliInUsePar", *fIn_use);

    Int_t array_e = fNumGroupsAnodes * fNumParamsEneFit;
    LOG(INFO) << "Array size for musli energy calibration: " << array_e;
    fEneCalParams->Set(array_e);
    list->add("musliEneCalPar", *fEneCalParams);

    Int_t array_pos = fNumGroupsAnodes * fNumParamsPosFit;
    LOG(INFO) << "Array size  musli position calibration: " << array_pos;
    fPosCalParams->Set(array_pos);
    list->add("musliPosCalPar", *fPosCalParams);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BMusliCalPar::getParams(FairParamList* list)
{
    R3BLOG(INFO, "called");
    if (!list)
    {
        R3BLOG(FATAL, "Could not find FairParamList");
        return kFALSE;
    }

    if (!list->fill("musliNumSignalsPar", &fNumSignals))
    {
        LOG(ERROR) << "Could not initialize musliNumSignalsPar";
        return kFALSE;
    }

    if (!list->fill("musliNumGroupsAnodesPar", &fNumGroupsAnodes))
    {
        LOG(ERROR) << "Could not initialize musliNumGroupsAnodesPar";
        return kFALSE;
    }

    if (!list->fill("musliMaxMultPar", &fMaxMult))
    {
        LOG(ERROR) << "Could not initialize musliMaxMultPar";
        return kFALSE;
    }

    if (!list->fill("musliNumParamsEneFitPar", &fNumParamsEneFit))
    {
        LOG(ERROR) << "Could not initialize musliNumParamsEneFit";
        return kFALSE;
    }

    if (!list->fill("musliNumParamsPosFitPar", &fNumParamsPosFit))
    {
        LOG(ERROR) << "Could not initialize musliNumParamsPosFitPar";
        return kFALSE;
    }

    fIn_use->Set(fNumGroupsAnodes);
    if (!(list->fill("musliInUsePar", fIn_use)))
    {
        LOG(ERROR) << "Could not initialize musliInUsePar ";
        return kFALSE;
    }

    Int_t array_e = fNumGroupsAnodes * fNumParamsEneFit;
    fEneCalParams->Set(array_e);
    if (!(list->fill("musliEneCalPar", fEneCalParams)))
    {
        LOG(ERROR) << "Could not initialize musliEneCalPar";
        return kFALSE;
    }

    Int_t array_pos = fNumGroupsAnodes * fNumParamsPosFit;
    fPosCalParams->Set(array_pos);
    if (!(list->fill("musliPosCalPar", fPosCalParams)))
    {
        LOG(ERROR) << "Could not initialize musliPosCalPar";
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BMusliCalPar::print() { printParams(); }

// ----  Method printParams ----------------------------------------------------
void R3BMusliCalPar::printParams()
{
    R3BLOG(INFO, "Nb of musli signals: " << fNumSignals);
    R3BLOG(INFO, "Nb of musli groups anodes signals: " << fNumGroupsAnodes);
    R3BLOG(INFO, "Nb of musli Max. multiplicity per anode: " << fMaxMult);
    R3BLOG(INFO, "Musli anode parameters for energy calibration");

    for (Int_t i = 0; i < fNumGroupsAnodes; i++)
    {
        LOG(INFO) << "Group of Anodes number: " << i + 1;
        for (Int_t j = 0; j < fNumParamsEneFit; j++)
        {
            LOG(INFO) << "FitParam(" << j << ") = " << fEneCalParams->GetAt(i * fNumParamsEneFit + j);
        }
    }

    R3BLOG(INFO, "Musli anode parameters for position calibration");
    for (Int_t i = 0; i < fNumGroupsAnodes; i++)
    {
        LOG(INFO) << "Group of Anodea number: " << i + 1;
        for (Int_t j = 0; j < fNumParamsPosFit; j++)
        {
            LOG(INFO) << "FitParam(" << j << ") = " << fPosCalParams->GetAt(i * fNumParamsPosFit + j);
        }
    }
}

ClassImp(R3BMusliCalPar);
