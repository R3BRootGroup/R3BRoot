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

// ----------------------------------------------------------------
// -----                R3BEventFilter                        -----
// -----     Created 05/10/25 by J.L. Rodriguez-Sanchez       -----
// ----------------------------------------------------------------

/*
 * This class selects which events should be stored in the ROOT file.
 * It acts as a filter, deciding whether an event is kept or discarded.
 */

#include "FairRunOnline.h"
#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>

#include "R3BEventFilter.h"
#include "R3BEventHeader.h"
#include "R3BFrsData.h"
#include "R3BLogger.h"
#include "R3BTofdHitData.h"

#include <TClonesArray.h>
#include <TMath.h>

R3BEventFilter::R3BEventFilter()
    : R3BEventFilter("R3BEventFilter", 1)
{
}

R3BEventFilter::R3BEventFilter(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
}

void R3BEventFilter::SetParContainers()
{
    R3BLOG(info, "");
    // Reading IncomingIDPar from FairRuntimeDb
    auto rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");

    return;
}

void R3BEventFilter::SetParameter()
{
    //--- Parameter Containers ---
    return;
}

InitStatus R3BEventFilter::Init()
{
    R3BLOG(info, "");

    auto mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, nullptr == mgr, "FairRootManager not found");

    fHeader = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));

    fTofdHit = dynamic_cast<TClonesArray*>(mgr->GetObject("TofdHit"));

    fFrsData = dynamic_cast<TClonesArray*>(mgr->GetObject("FrsData"));

    if (fCutFrsId == nullptr)
    {
        fCutFrsId = new TCutG("fCutFrsId", 5);
        fCutFrsId->SetPoint(0, 0.5, 0.5);
        fCutFrsId->SetPoint(1, 4., 0.5);
        fCutFrsId->SetPoint(2, 4., 4.);
        fCutFrsId->SetPoint(3, 0.5, 4.);
        fCutFrsId->SetPoint(4, 0.5, 0.5);
    }

    if (fChargeLimits.empty())
    {
        R3BLOG(warn, "Charge limits not set.");
        fUseTofd = false;
    }

    SetParameter();
    return kSUCCESS;
}

InitStatus R3BEventFilter::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

inline bool CheckFireConditionsTwoPlanes(const std::vector<int>& v)
{

    bool planes13ok = false;
    bool planes24ok = false;

    if ((v[0] == 1) && (v[2] == 1))
        planes13ok = true;

    if ((v[1] == 1) && (v[3] == 1))
        planes24ok = true;

    if (planes13ok || planes24ok)
        return true;
    else
        return false;
}

inline bool CheckFireConditionsAllPlanes(const std::vector<int>& v)
{

    bool goodHit = true;

    for (const auto& val : v)
        if (val == 0)
            goodHit = false;

    return goodHit;
}

void R3BEventFilter::Exec(Option_t*)
{
    bool fValidEvent = false;

    if (fHeader->GetExpId() == 249)
    {
        if (fFrsData != nullptr && fFrsData->GetEntriesFast() > 0 && fCutFrsId != nullptr)
        {
            auto nHits = fFrsData->GetEntriesFast();
            for (size_t ihit = 0; ihit < nHits; ihit++)
            {
                auto frsdata = dynamic_cast<R3BFrsData*>(fFrsData->At(ihit));
                auto charge = frsdata->GetZ();
                auto aoq = frsdata->GetAq();
                if (fCutFrsId->IsInside(charge, aoq))
                {
                    fValidEvent = true;
                }
            }
        }

        // Function to determinate the charge in terms of the limits
        auto isSelectedCharge = [&](int plane, double charge)
        {
            if ((charge > fChargeLimits[plane - 1][0]) && (charge < fChargeLimits[plane - 1][1]))
                return 1;
            else
                return 0;
        };

        if (fTofdHit != nullptr && fTofdHit->GetEntriesFast() > 0 && fValidEvent && !fChargeLimits.empty() && fUseTofd)
        {
            std::vector<int> chargeSelected(4, -1);
            auto nHits = fTofdHit->GetEntriesFast();

            for (size_t ihit = 0; ihit < nHits; ihit++)
            {
                auto tofdhit = dynamic_cast<R3BTofdHitData*>(fTofdHit->At(ihit));
                auto plane = tofdhit->GetPlaneId();

                // Add a method to calculate the charge in each plane
                chargeSelected[plane - 1] = isSelectedCharge(plane, tofdhit->GetChargeZ());
            }

            // To identify something as a good hit it is necessary that in 1-3 or 2-4 we have the same
            // charge
            if (fUseAllPlanes)
                fValidEvent = CheckFireConditionsAllPlanes(chargeSelected);
            else
                fValidEvent = CheckFireConditionsTwoPlanes(chargeSelected);
        }
    }
    else
    {
        R3BLOG(fatal, "Unknown experiment ID: event filter not yet implemented");
    }
    StoreEvent(fValidEvent);
}

void R3BEventFilter::FinishEvent()
{
    if (fFrsData)
    {
        fFrsData->Clear();
    }
}

void R3BEventFilter::StoreEvent(bool valid)
{
    if (valid)
    {
        FairRunOnline::Instance()->MarkFill(kTRUE);
    }
    else
    {
        FairRunOnline::Instance()->MarkFill(kFALSE);
    }
}

ClassImp(R3BEventFilter)
