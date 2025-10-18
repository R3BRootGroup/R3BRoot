/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
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
// -----                R3BFootHitPar source file               -----
// -----         Created 17/05/25 by Pablo González Rusell      -----
// ------------------------------------------------------------------

#include "R3BFootHitPar.h"
#include "R3BLogger.h"

#include <FairParamList.h>

#include <TArrayF.h>
#include <TMath.h>
#include <TString.h>

// ---- Standard Constructor ---------------------------------------------------
R3BFootHitPar::R3BFootHitPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
{
    detName = "FootHit";
    fMultCharPar = new TArrayF(fNumDets * 10);
    fCharCalPar = new TArrayF();
    fCharCalParSM = new TArrayF();
    fEtaCorrPar = new TArrayF();
}

// ----  Destructor ------------------------------------------------------------
R3BFootHitPar::~R3BFootHitPar()
{
    this->clear(); // NOLINT
    delete fCharCalPar;
    fCharCalPar = nullptr;
    delete fCharCalParSM;
    fCharCalParSM = nullptr;
    delete fMultCharPar;
    fMultCharPar = nullptr;
    delete fEtaCorrPar;
    fEtaCorrPar = nullptr;
}

// ----  Method clear ----------------------------------------------------------
void R3BFootHitPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BFootHitPar::putParams(FairParamList* list)
{
    R3BLOG(info, "called");
    if (!list)
    {
        R3BLOG(fatal, "Could not initialize FairParamList");
        return;
    }

    // Get the size of the parameters
    Int_t array_size = 0;

    for (int i = 0; i < fMultCharPar->GetSize(); ++i)
        array_size += static_cast<int>(fMultCharPar->At(i));

    R3BLOG(info, Form("Number of active asics: %i", array_size));

    // Create the charge and eta arrays
    fCharCalPar->Set(array_size * fNumParsCal);
    fCharCalParSM->Set(array_size * fNumParsCal);
    fEtaCorrPar->Set(array_size * fNumParsEtaCorr);

    // Save the values
    list->add("footDetNbPar", fNumDets);
    list->add("footNumberCalPars", fNumParsCal);
    list->add("footNumberEtaCorrPars", fNumParsEtaCorr);

    list->add("footCharCalPar", *fCharCalPar);
    list->add("footCharCalParSM", *fCharCalParSM);
    list->add("footEtaCorrPar", *fEtaCorrPar);
    list->add("footMultCharPar", *fMultCharPar);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BFootHitPar::getParams(FairParamList* list)
{
    R3BLOG(info, "called");
    if (!list)
    {
        R3BLOG(fatal, "Could not initialize FairParamList");
        return kFALSE;
    }

    if (!list->fill("footDetNbPar", &fNumDets))
    {
        R3BLOG(error, "Could not initialize footDetNbPar");
        return kFALSE;
    }

    if (!list->fill("footNumberCalPars", &fNumParsCal))
    {
        R3BLOG(fatal, "R3BFootHitPar::Could not initialize footNumberCalPars");
        return kFALSE;
    }

    if (!list->fill("footNumberEtaCorrPars", &fNumParsEtaCorr))
    {
        R3BLOG(fatal, "R3BFootHitPar::Could not initialize footNumberEtaCorrPars");
        return kFALSE;
    }

    if (!list->fill("footMultCharPar", fMultCharPar))
    {
        R3BLOG(fatal, "R3BFootHitPar::Could not initialize footMultCharPar");
        return kFALSE;
    }

    Int_t array_size = 0;

    for (int i = 0; i < fMultCharPar->GetSize(); ++i)
        array_size += static_cast<int>(fMultCharPar->At(i));

    fCharCalPar->Set(array_size * fNumParsCal);
    fCharCalParSM->Set(array_size * fNumParsCal);
    fEtaCorrPar->Set(array_size * fNumParsEtaCorr);

    if (!(list->fill("footCharCalPar", fCharCalPar)))
    {
        R3BLOG(fatal, "R3BFootHitPar::Could not initialize footCharCalPar");
        return kFALSE;
    }

    // We assume that the calibration parameters are the same for all multiplicities
    if (!(list->fill("footCharCalParSM", fCharCalParSM)))
    {
        R3BLOG(
            warn,
            "R3BFootHitPar::Could not initialize footCharCalParSM. Single multiplicity calibration will not be used");
        list->fill("footCharCalPar", fCharCalParSM);
    }

    if (!(list->fill("footEtaCorrPar", fEtaCorrPar)))
    {
        R3BLOG(fatal, "R3BFootHitPar::Could not initialize footEtaCorrPar");
        return kFALSE;
    }

    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BFootHitPar::print()
{
    R3BLOG(info, "Foot Hit Parameters");
    R3BLOG(info, Form("Number of calibration parameters: %i", fNumParsCal));
    R3BLOG(info, Form("Number of eta correction parameters: %i", fNumParsEtaCorr));
}

ClassImp(R3BFootHitPar)
