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

// ----------------------------------------------------------------
// -----             R3BActafCalPar source file               -----
// -----    Created 17/09/25  by Pablo González Rusell        -----
// ----------------------------------------------------------------

#include "R3BActafCalPar.h"
#include "R3BLogger.h"

R3BActafCalPar::R3BActafCalPar(const TString& name, const TString& title, const TString& context)
    : FairParGenericSet(name, title, context)
{

    fGainPars = new TArrayF(fPads);
    fThresholdsPars = new TArrayF(fPads);
    fSGCoeffs.resize(fNbSGcoefs);

    for (int i = 0; i < fPads; i++)
    {
        fGainPars->AddAt(1.0, i);
        fThresholdsPars->AddAt(0.0, i);
    }
}

void R3BActafCalPar::SetNbSGCoeffs(Int_t num) { fSGCoeffs.resize(num); }

R3BActafCalPar::~R3BActafCalPar()
{
    if (fGainPars)
        delete fGainPars;
    if (fThresholdsPars)
        delete fThresholdsPars;
}

void R3BActafCalPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

void R3BActafCalPar::putParams(FairParamList* list)
{
    R3BLOG(info, "called");
    if (!list)
    {
        R3BLOG(fatal, "Could not initialize FairParamList");
        return;
    }

    R3BLOG(info, Form("Array size = %d", fPads));

    list->add("actafGainPars", *fGainPars);
    list->add("actafThresholdPars", *fThresholdsPars);

    list->add("NbSGPar", fNbSGcoefs);
    R3BLOG(info, "Nb of pads: " << fNbSGcoefs);

    TArrayD SGCoeffs(fNbSGcoefs);
    for (int idx = 0; idx < fNbSGcoefs; idx++)
    {
        SGCoeffs[idx] = fSGCoeffs[idx];
    }
    list->add("SGCoeffs", SGCoeffs);
}
bool R3BActafCalPar::getParams(FairParamList* list)
{
    if (!list)
    {
        R3BLOG(error, "Could not initialize FairParamList");
        return false;
    }

    TArrayF Gain, Thr;
    if (!list->fill("actafGainPars", &Gain))
    {
        R3BLOG(fatal, "R3BActafCalPar could not initialize actafGainPars");
        return false;
    }
    if (!list->fill("actafThresholdPars", &Thr))
    {
        R3BLOG(fatal, "R3BActafCalPar could not initialize actafThresholdPars");
        return false;
    }

    if (!list->fill("NbSGcoefs", &fNbSGcoefs))
    {
        R3BLOG(error, "Could not initialize SGCoeffs");
        return kFALSE;
    }
    else
    {
        R3BLOG(info, "Nb of sg coeffs: " << fNbSGcoefs);
    }

    if (fGainPars->GetSize() != Gain.GetSize())
        fGainPars->Set(Gain.GetSize());
    if (fThresholdsPars->GetSize() != Thr.GetSize())
        fThresholdsPars->Set(Thr.GetSize());

    for (int i = 0; i < Gain.GetSize(); ++i)
        fGainPars->SetAt(Gain.At(i), i);
    for (int i = 0; i < Thr.GetSize(); ++i)
        fThresholdsPars->SetAt(Thr.At(i), i);

    TArrayD SGCoeffs(fNbSGcoefs);
    if (!list->fill("SGCoeffs", &SGCoeffs))
    {
        R3BLOG(warn, "---Could not initialize SGCoeffs");
        return kFALSE;
    }

    const Int_t nSGValues = SGCoeffs.GetSize();
    fSGCoeffs.resize(nSGValues);
    std::copy_n(SGCoeffs.GetArray(), nSGValues, fSGCoeffs.begin());

    return kTRUE;
}

void R3BActafCalPar::print()
{
    R3BLOG(info, "Actaf cal params");

    for (int i = 0; i < fPads; i++)
    {
        R3BLOG(info, Form("Pad %d: gain = %f    thr = %f", i + 1, fGainPars->GetAt(i), fThresholdsPars->GetAt(i)));
    }

    R3BLOG(info, "Nb of SG Coeffs: " << fNbSGcoefs);
}

ClassImp(R3BActafCalPar)
