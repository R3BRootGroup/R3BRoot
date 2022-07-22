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

// -----------------------------------------------------------------
// -----                R3BTwimCalPar source file              -----
// -----         Created 24/11/19 by J.L. Rodriguez-Sanchez    -----
// -----------------------------------------------------------------

#include "R3BTwimCalPar.h"
#include "R3BLogger.h"

#include "FairDetParIo.h"
#include "FairLogger.h"
#include "FairParamList.h"

#include "TArrayF.h"
#include "TArrayI.h"
#include "TMath.h"
#include "TString.h"

// ---- Standard Constructor ---------------------------------------------------
R3BTwimCalPar::R3BTwimCalPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fNumSections(4)
    , fNumAnodes(16)
    , fNumParamsEFit(2)
    , fNumParamsPosFit(3)
    , fNumAnodesTRef(2)
    , fNumAnodesTrig(2)
    , fMaxMult(20)
    , fMinDT(-1000)
    , fMaxDT(1000)
{
    fIn_use.resize(fNumSections);
    fAnodeECalParams.resize(fNumSections);
    fAnodePosCalParams.resize(fNumSections);
    for (Int_t s = 0; s < fNumSections; s++)
    {
        fIn_use[s] = new TArrayI(fNumAnodes);
        fAnodeECalParams[s] = new TArrayF(fNumAnodes * fNumParamsEFit);
        fAnodePosCalParams[s] = new TArrayF(fNumAnodes * fNumParamsPosFit);
    }
}

// ----  Destructor ------------------------------------------------------------
R3BTwimCalPar::~R3BTwimCalPar()
{
    clear();
    for (Int_t s = 0; s < fNumSections; s++)
    {
        if (fIn_use[s])
            delete fIn_use[s];
        if (fAnodeECalParams[s])
            delete fAnodeECalParams[s];
        if (fAnodePosCalParams[s])
            delete fAnodePosCalParams[s];
    }
}

// ----  Method clear ----------------------------------------------------------
void R3BTwimCalPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BTwimCalPar::putParams(FairParamList* list)
{
    R3BLOG(INFO, "called");
    if (!list)
    {
        R3BLOG(FATAL, "Could not find FairParamList");
        return;
    }

    list->add("twimSectionNumberPar", fNumSections);
    list->add("twimAnodeNumberPar", fNumAnodes);
    list->add("twimAnodeTRefPar", fNumAnodesTRef);
    list->add("twimAnodeTrigPar", fNumAnodesTrig);
    list->add("twimMaxMultPar", fMaxMult);
    list->add("twimMinDTPar", fMinDT);
    list->add("twimMaxDTPar", fMaxDT);
    list->add("twimAnodeEFitPar", fNumParamsEFit);
    list->add("twimAnodePosFitPar", fNumParamsPosFit);

    R3BLOG(INFO, "Nb of twim sections: " << fNumSections);
    fIn_use.resize(fNumSections);
    fAnodeECalParams.resize(fNumSections);
    fAnodePosCalParams.resize(fNumSections);

    char name[300];
    for (Int_t s = 0; s < fNumSections; s++)
    {
        fIn_use[s]->Set(fNumAnodes);
        sprintf(name, "twimSec%dInUsePar", s + 1);
        list->add(name, *fIn_use[s]);
    }

    Int_t array_e = fNumAnodes * fNumParamsEFit;
    LOG(INFO) << "Array twim energy calibration per section: " << array_e;
    for (Int_t s = 0; s < fNumSections; s++)
    {
        fAnodeECalParams[s]->Set(array_e);
        sprintf(name, "twimSec%dCalEPar", s + 1);
        list->add(name, *fAnodeECalParams[s]);
    }

    Int_t array_pos = fNumAnodes * fNumParamsPosFit;
    LOG(INFO) << "Array twim position calibration per section: " << array_pos;
    for (Int_t s = 0; s < fNumSections; s++)
    {
        fAnodePosCalParams[s]->Set(array_pos);
        sprintf(name, "twimSec%dPosPar", s + 1);
        list->add(name, *fAnodePosCalParams[s]);
    }
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BTwimCalPar::getParams(FairParamList* list)
{
    R3BLOG(INFO, "called");
    if (!list)
    {
        R3BLOG(FATAL, "Could not find FairParamList");
        return kFALSE;
    }

    if (!list->fill("twimSectionNumberPar", &fNumSections))
    {
        LOG(ERROR) << "Could not initialize twimSectionNumberPar";
        return kFALSE;
    }

    if (!list->fill("twimAnodeNumberPar", &fNumAnodes))
    {
        LOG(ERROR) << "Could not initialize twimAnodeNumberPar";
        return kFALSE;
    }

    if (!list->fill("twimAnodeTRefPar", &fNumAnodesTRef))
    {
        LOG(ERROR) << "Could not initialize twimAnodeTRefPar";
        return kFALSE;
    }

    if (!list->fill("twimAnodeTrigPar", &fNumAnodesTrig))
    {
        LOG(ERROR) << "Could not initialize twimAnodeTrigPar";
        return kFALSE;
    }

    if (!list->fill("twimMaxMultPar", &fMaxMult))
    {
        LOG(ERROR) << "Could not initialize twimMaxMultPar";
        return kFALSE;
    }

    if (!list->fill("twimMinDTPar", &fMinDT))
    {
        LOG(WARNING) << "Could not initialize twimMinDTPar";
    }

    if (!list->fill("twimMaxDTPar", &fMaxDT))
    {
        LOG(WARNING) << "Could not initialize twimMaxDTPar";
    }

    if (!list->fill("twimAnodeEFitPar", &fNumParamsEFit))
    {
        LOG(ERROR) << "Could not initialize twimAnodeEFitPar";
        return kFALSE;
    }

    if (!list->fill("twimAnodePosFitPar", &fNumParamsPosFit))
    {
        LOG(ERROR) << "Could not initialize twimAnodePosFitPar";
        return kFALSE;
    }

    fIn_use.resize(fNumSections);
    fAnodeECalParams.resize(fNumSections);
    fAnodePosCalParams.resize(fNumSections);

    char name[300];
    for (Int_t s = 0; s < fNumSections; s++)
    {
        fIn_use[s]->Set(fNumAnodes);
        sprintf(name, "twimSec%dInUsePar", s + 1);
        if (!(list->fill(name, fIn_use[s])))
        {
            LOG(ERROR) << "Could not initialize " << name;
            return kFALSE;
        }
    }

    Int_t array_e = fNumAnodes * fNumParamsEFit;
    for (Int_t s = 0; s < fNumSections; s++)
    {
        fAnodeECalParams[s]->Set(array_e);
        sprintf(name, "twimSec%dCalEPar", s + 1);
        if (!(list->fill(name, fAnodeECalParams[s])))
        {
            LOG(ERROR) << "Could not initialize " << name;
            return kFALSE;
        }
    }

    Int_t array_pos = fNumAnodes * fNumParamsPosFit;
    for (Int_t s = 0; s < fNumSections; s++)
    {
        fAnodePosCalParams[s]->Set(array_pos);
        sprintf(name, "twimSec%dPosPar", s + 1);
        if (!(list->fill(name, fAnodePosCalParams[s])))
        {
            LOG(ERROR) << "Could not initialize " << name;
            return kFALSE;
        }
    }

    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BTwimCalPar::print() { printParams(); }

// ----  Method printParams ----------------------------------------------------
void R3BTwimCalPar::printParams()
{
    R3BLOG(INFO, "Nb of twim sections: " << fNumSections);
    R3BLOG(INFO, "Nb of twim Ref anodes: " << fNumAnodesTRef);
    R3BLOG(INFO, "Nb of twim Trigger anodes: " << fNumAnodesTrig);
    R3BLOG(INFO, "Nb of twim Max. multiplicity per anode: " << fMaxMult);
    R3BLOG(INFO, "Min Drift Time accepted: " << fMinDT);
    R3BLOG(INFO, "Max Drift Time accepted: " << fMaxDT);
    R3BLOG(INFO, "Twim anode parameters for energy calibration");

    for (Int_t s = 0; s < fNumSections; s++)
    {
        LOG(INFO) << "Twim section: " << s + 1;
        for (Int_t i = 0; i < fNumAnodes; i++)
        {
            LOG(INFO) << "Anode number: " << i + 1;
            for (Int_t j = 0; j < fNumParamsEFit; j++)
            {
                LOG(INFO) << "FitParam(" << j << ") = " << fAnodeECalParams[s]->GetAt(i * fNumParamsEFit + j);
            }
        }
    }

    R3BLOG(INFO, "Twim anode parameters for position calibration");
    for (Int_t s = 0; s < fNumSections; s++)
    {
        LOG(INFO) << "Twim section: " << s + 1;
        for (Int_t i = 0; i < fNumAnodes; i++)
        {
            LOG(INFO) << "Anode number: " << i + 1;
            for (Int_t j = 0; j < fNumParamsPosFit; j++)
            {
                LOG(INFO) << "FitParam(" << j << ") = " << fAnodePosCalParams[s]->GetAt(i * fNumParamsPosFit + j);
            }
        }
    }
}

ClassImp(R3BTwimCalPar);
