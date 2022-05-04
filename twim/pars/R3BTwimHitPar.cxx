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

// ------------------------------------------------------------------
// -----             R3BTwimHitPar source file                  -----
// -----       Created 24/11/19 by J.L. Rodriguez-Sanchez       -----
// ------------------------------------------------------------------

#include "R3BTwimHitPar.h"

#include "FairDetParIo.h"
#include "FairLogger.h"
#include "FairParamList.h"

#include "TArrayF.h"
#include "TMath.h"
#include "TString.h"

#include <iostream>

// ---- Standard Constructor ---------------------------------------------------
R3BTwimHitPar::R3BTwimHitPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fNumParamsZFit(4)
    , fNumAnodes(16)
    , fNumSec(4)
{
    fDetZHitParams = new TArrayF(fNumSec * fNumParamsZFit); // 2 Parameters for Z (Linear fits)
    fIn_use = new TArrayI(fNumAnodes * fNumSec);
    fAnode_pos = new TArrayF(fNumAnodes * fNumSec);
    // left and right + up and down --> 4
    fTofHitParams = new TArrayF(fNumSec * 3);
    fEmean_tof.resize(fNumSec);
    fEmean_dt.resize(fNumSec);
    fSpline.resize(fNumSec);
    char spline_name[200];
    for (Int_t s = 0; s < fNumSec; s++)
    {
        sprintf(spline_name, "spline%d", s + 1);
        fSpline[s] = new R3BTsplinePar(spline_name);
        fEmean_tof[s] = 0.;
        fEmean_dt[s] = 0.;
    }
    // fSpline = new R3BTsplinePar("TwimSpline");
}

// ----  Destructor ------------------------------------------------------------
R3BTwimHitPar::~R3BTwimHitPar()
{
    clear();
    if (fIn_use)
        delete fIn_use;
    if (fAnode_pos)
        delete fAnode_pos;
    if (fDetZHitParams)
        delete fDetZHitParams;
    if (fTofHitParams)
        delete fTofHitParams;

    for (Int_t s = 0; s < fNumSec; s++)
    {
        if (fSpline[s])
            delete fSpline[s];
    }
}

// ----  Method clear ----------------------------------------------------------
void R3BTwimHitPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BTwimHitPar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BTwimHitPar::putParams() called";
    if (!list)
    {
        return;
    }

    list->add("twimHitNumSecPar", fNumSec);
    list->add("twimAnodeNumberPar", fNumAnodes);
    Int_t array_anodes = fNumAnodes * fNumSec;
    LOG(INFO) << "Array Size Anodes: " << array_anodes;
    fIn_use->Set(array_anodes);
    list->add("twinInUsePar", *fIn_use);

    fAnode_pos->Set(array_anodes);
    list->add("twimAnodePosPar", *fAnode_pos);

    list->add("twimZHitParamsFitPar", fNumParamsZFit);
    Int_t array_size = fNumSec * fNumParamsZFit;
    LOG(INFO) << "Number of parameters for charge-Z: " << array_size;
    fDetZHitParams->Set(array_size);
    list->add("twimZHitPar", *fDetZHitParams);
    // fTofHitParams->Set(2 * 28 * 3);
    fTofHitParams->Set(fNumSec * 3);
    list->add("twimvstofHitPar", *fTofHitParams);

    fSpline.resize(fNumSec);
    for (Int_t s = 0; s < fNumSec; s++)
    {
        fSpline[s]->putParams(list);
    }
    fEmean_tof.resize(fNumSec);
    fEmean_dt.resize(fNumSec);
    char nametof[300];
    char namedt[300];
    for (Int_t s = 0; s < fNumSec; s++)
    {
        sprintf(nametof, "Emean_tof%d", s + 1);
        sprintf(namedt, "Emean_dt%d", s + 1);
        list->add(nametof, fEmean_tof[s]);
        list->add(namedt, fEmean_dt[s]);
    }
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BTwimHitPar::getParams(FairParamList* list)
{
    LOG(INFO) << "R3BTwimHitPar::getParams() called";
    if (!list)
    {
        LOG(FATAL) << "Could not find FairParamList";
        return kFALSE;
    }

    if (!list->fill("twimHitNumSecPar", &fNumSec))
    {
        LOG(ERROR) << "Could not initialize twimHitNumSecPar";
        return kFALSE;
    }

    if (!list->fill("twimAnodeNumberPar", &fNumAnodes))
    {
        LOG(ERROR) << "Could not initialize twimAnodeNumberPar";
        return kFALSE;
    }

    Int_t array_anode = fNumAnodes * fNumSec;
    LOG(INFO) << "Array Size: " << array_anode;
    fIn_use->Set(array_anode);
    if (!(list->fill("twimInUsePar", fIn_use)))
    {
        LOG(ERROR) << "Could not initialize twimInUsePar";
        return kFALSE;
    }

    fAnode_pos->Set(array_anode);
    if (!(list->fill("twimAnodePosPar", fAnode_pos)))
    {
        LOG(ERROR) << "Could not initialize twimAnodePosPar";
        return kFALSE;
    }

    if (!list->fill("twimZHitParamsFitPar", &fNumParamsZFit))
    {
        LOG(ERROR) << "Could not initialize twimZHitParamsFitPar";
        return kFALSE;
    }

    Int_t array_size = fNumParamsZFit * fNumSec;
    LOG(INFO) << "Array Size: " << array_size;
    fDetZHitParams->Set(array_size);

    if (!(list->fill("twimZHitPar", fDetZHitParams)))
    {
        LOG(ERROR) << "Could not initialize twimZHitPar";
        return kFALSE;
    }

    if (!(list->fill("twimvstofHitPar", fTofHitParams)))
    {
        LOG(WARNING) << "Could not initialize twimvstofHitPar";
        // return kFALSE;
    }
    fSpline.resize(fNumSec);
    for (Int_t s = 0; s < fNumSec; s++)
    {
        fSpline[s]->getParams(list);
        ////std::cout << "Eval tspliine = " << fSpline[s]->Eval(-35);
    }

    fEmean_tof.resize(fNumSec);
    fEmean_dt.resize(fNumSec);
    char nametof[300];
    char namedt[300];
    for (Int_t s = 0; s < fNumSec; s++)
    {
        sprintf(nametof, "Emean_tof%d", s + 1);
        sprintf(namedt, "Emean_dt%d", s + 1);
        if (!(list->fill(nametof, &fEmean_tof[s])))
        {
            LOG(ERROR) << "Could not initialize " << nametof;
            return kFALSE;
        }
        if (!(list->fill(namedt, &fEmean_dt[s])))
        {
            LOG(ERROR) << "Could not initialize " << namedt;
            return kFALSE;
        }
    }

    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BTwimHitPar::print() { printParams(); }

// ----  Method printParams ----------------------------------------------------
void R3BTwimHitPar::printParams()
{
    LOG(INFO) << "R3BTwimHitPar::printParams() twim detector Parameters";
    LOG(INFO) << "R3BTwimHitPar::printParams() twim anodes in use: ";

    for (Int_t s = 0; s < fNumSec; s++)
    {
        LOG(INFO) << "Section = " << s + 1;
        for (Int_t j = 0; j < fNumAnodes; j++)
        {
            LOG(INFO) << "Anode " << j + 1 << " in use " << fIn_use->GetAt(s * fNumAnodes + j)
                      << ", Position-Z: " << fAnode_pos->GetAt(j);
        }
    }

    for (Int_t s = 0; s < fNumSec; s++)
    {
        LOG(INFO) << "Section = " << s + 1;
        for (Int_t j = 0; j < fNumParamsZFit; j++)
        {
            LOG(INFO) << "FitParam(" << j << ") = " << fDetZHitParams->GetAt(j + s * fNumParamsZFit);
        }
    }

    if (fTofHitParams)
    {
        for (Int_t s = 0; s < fNumSec; s++)
        {
            LOG(INFO) << "Section = " << s + 1;
            // for (Int_t p = 0; p < 28; p++)
            for (Int_t j = 0; j < 3; j++)
            {
                // LOG(INFO) << "Tof sci nb " << p + 1 << ": FitParam(" << j
                //       << ") = " << fTofHitParams->GetAt(j + p * 3 + s * 3 * 28);
                LOG(INFO) << "FitParam(" << j << ") = " << fTofHitParams->GetAt(j + s * 3);
            }
        }
    }
}

ClassImp(R3BTwimHitPar);
