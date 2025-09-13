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
// -----             R3BActafCal2Hit source file               -----
// -----      Created 14/09/25 by J.L. Rodriguez-Sanchez       -----
// -----------------------------------------------------------------

// ROOT headers
#include <TClonesArray.h>
#include <TMath.h>

// FAIR headers
#include <FairLogger.h>
#include <FairRootManager.h>

// ACTAF headers
#include "R3BActafCal2Hit.h"
#include "R3BActafCalData.h"
#include "R3BActafGeometry.h"
#include "R3BActafHitData.h"
#include "R3BLogger.h"

// R3BActafCal2Hit::Default Constructor --------------------------
R3BActafCal2Hit::R3BActafCal2Hit()
    : R3BActafCal2Hit("R3BActafCal2Hit", 1)
{
}

// R3BActafCal2Hit::Standard Constructor --------------------------
R3BActafCal2Hit::R3BActafCal2Hit(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
}

// Virtual R3BActafCal2Hit::Destructor
R3BActafCal2Hit::~R3BActafCal2Hit()
{
    R3BLOG(debug1, "");
    if (fActafHitData)
    {
        delete fActafHitData;
    }
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BActafCal2Hit::Init()
{
    R3BLOG(info, "");
    auto* mgr = FairRootManager::Instance();
    if (!mgr)
    {
        R3BLOG(fatal, "FairRootManager not found");
        return kFATAL;
    }

    // INPUT DATA
    fActafCalData = dynamic_cast<TClonesArray*>(mgr->GetObject("ActafCalData"));
    if (!fActafCalData)
    {
        R3BLOG(fatal, "ActafMappedData not found");
        return kFATAL;
    }

    // OUTPUT DATA
    fActafHitData = new TClonesArray("R3BActafHitData");
    mgr->Register("ActafHitData", "ACTAF_Hit", fActafHitData, !fOnline);
    Reset();

    fActafGeo = R3BActafGeometry::Instance();
    R3BLOG_IF(warn, !fActafGeo->Init(fGeoversion), "Actaf geometry " << fGeoversion << " not found");

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BActafCal2Hit::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BActafCal2Hit::Exec(Option_t*)
{
    // Reset entries in the output arrays
    Reset();

    // Reading the Input -- Cal Data --
    auto nHits = fActafCalData->GetEntriesFast();
    if (nHits == 0)
    {
        return;
    }

    for (size_t index = 0; index < nHits; ++index)
    {
        auto calData = dynamic_cast<R3BActafCalData const*>(fActafCalData->At(index));
        auto pad = calData->GetPad();
        auto ring = fActafGeo->GetRingId(pad);
        auto zpos = calData->GetZpos();
        auto energy = calData->GetEnergy();
        TVector3 track = fActafGeo->GetPosition(pad);
        AddHitData(pad, (pad < 65 ? 1 : 2), ring, track.X(), track.Y(), zpos, energy, track);
    }
    return;
}

// -----   Public method Reset   ------------------------------------------------
void R3BActafCal2Hit::Reset()
{
    R3BLOG(debug1, "Clearing HitData Structure");
    if (fActafHitData)
    {
        fActafHitData->Clear();
    }
}

// -----   Private method AddHitData  --------------------------------------------
R3BActafHitData* R3BActafCal2Hit::AddHitData(UInt_t padId,
                                             UInt_t side,
                                             UInt_t ring,
                                             double xpos,
                                             double ypos,
                                             double zpos,
                                             double energy,
                                             TVector3 track)
{
    // It fills the R3BActafCalData
    TClonesArray& clref = *fActafHitData;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BActafHitData(padId, side, ring, xpos, ypos, zpos, energy, track);
}

ClassImp(R3BActafCal2Hit)
