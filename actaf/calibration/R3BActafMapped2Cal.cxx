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

// -----------------------------------------------------------------
// -----            R3BActafMapped2Cal source file             -----
// -----      Created 14/09/25 by J.L. Rodriguez-Sanchez       -----
// -----------------------------------------------------------------

// ROOT headers
#include <TClonesArray.h>
#include <TMath.h>

// FAIR headers
#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRuntimeDb.h>

// ACTAF headers
#include "R3BActafCalData.h"
#include "R3BActafCalPar.h"
#include "R3BActafMapped2Cal.h"
#include "R3BActafMappedData.h"
#include "R3BLogger.h"

// R3BActafMapped2Cal::Default Constructor --------------------------
R3BActafMapped2Cal::R3BActafMapped2Cal()
    : R3BActafMapped2Cal("R3BActafMapped2Cal", 1)
{
}

// R3BActafMapped2Cal::Standard Constructor --------------------------
R3BActafMapped2Cal::R3BActafMapped2Cal(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
    fEGain.resize(fPad);
    fEThr.resize(fPad);
}

// Virtual R3BActafMapped2Cal::Destructor
R3BActafMapped2Cal::~R3BActafMapped2Cal()
{
    R3BLOG(debug1, "");
    if (fActafCalData)
        delete fActafCalData;

    if (fCal_Par)
        delete fCal_Par;
}

void R3BActafMapped2Cal::SetParContainers()
{
    // Parameter Container
    // Reading actafCalPar from FairRuntimeDb

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(error, !rtdb, "FairRuntimeDb not found");

    fCal_Par = dynamic_cast<R3BActafCalPar*>(rtdb->getContainer("actafCalPar"));
    if (!fCal_Par)
    {
        R3BLOG(error, "actafCalPar container not found");
    }
    else
    {
        R3BLOG(info, "actafCalPar found");
    }
}

void R3BActafMapped2Cal::SetParameter()
{
    if (fCal_Par == nullptr)
    {
        R3BLOG(error, "actaf CalPar not found");
        return;
    }

    for (int i = 0; i < fPad; i++)
    {
        fEGain[i] = fCal_Par->GetGainVal(i);
        fEThr[i] = fCal_Par->GetThresholdVal(i);
    }
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BActafMapped2Cal::Init()
{
    R3BLOG(info, "");
    auto* mgr = FairRootManager::Instance();
    if (!mgr)
    {
        R3BLOG(fatal, "FairRootManager not found");
        return kFATAL;
    }

    // INPUT DATA
    fActafMappedData = dynamic_cast<TClonesArray*>(mgr->GetObject("ActafMappedData"));
    if (!fActafMappedData)
    {
        R3BLOG(fatal, "ActafMappedData not found");
        return kFATAL;
    }

    // OUTPUT DATA
    fActafCalData = new TClonesArray("R3BActafCalData");
    mgr->Register("ActafCalData", "ACTAF_Cal", fActafCalData, !fOnline);
    Reset();

    SetParameter();

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BActafMapped2Cal::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BActafMapped2Cal::Exec(Option_t*)
{
    // Reset entries in the output arrays
    Reset();

    // Reading the Input -- Mapped Data --
    auto nHits = fActafMappedData->GetEntriesFast();
    if (nHits == 0)
    {
        return;
    }

    double synTagTime = 0.;
    for (size_t index = 0; index < nHits; ++index)
    {
        auto mappedData = dynamic_cast<R3BActafMappedData const*>(fActafMappedData->At(index));
        auto pad = mappedData->GetPad();
        if (pad == 129)
            synTagTime = mappedData->GetLeadingEdgeTime() * fConversionCh2ns; // in ns
    }

    for (size_t index = 0; index < nHits; ++index)
    {
        auto mappedData = dynamic_cast<R3BActafMappedData const*>(fActafMappedData->At(index));
        auto pad = mappedData->GetPad();
        if (pad >= 129)
            continue; // syn-time
        auto energy = mappedData->GetE() * fEGain[pad - 1];
        auto energyMaxAmpl = mappedData->GetMaxampl() * fEGain[pad - 1];
        auto drift = mappedData->GetLeadingEdgeTime() * fConversionCh2ns; // in ns
        auto zpos = drift * fVelocity;                                    // in cm
        auto syntime = drift - synTagTime;                                // in ns

        if (energy >= fEThr[pad - 1])
            AddCalData(pad, energy, energyMaxAmpl, drift, zpos, syntime);
    }
    return;
}

// -----   Public method Reset   ------------------------------------------------
void R3BActafMapped2Cal::Reset()
{
    R3BLOG(debug1, "Clearing CalData Structure");
    if (fActafCalData)
    {
        fActafCalData->Clear();
    }
}

// -----   Private method AddCalData  --------------------------------------------
R3BActafCalData* R3BActafMapped2Cal::AddCalData(UInt_t padId,
                                                double energy,
                                                double maxampl,
                                                double drift,
                                                double zpos,
                                                double syntime)
{
    // It fills the R3BActafCalData
    TClonesArray& clref = *fActafCalData;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BActafCalData(padId, energy, maxampl, drift, zpos, syntime);
}

ClassImp(R3BActafMapped2Cal)
