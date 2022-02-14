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

#include "R3BLosProvideTStart.h"
#include "FairRootManager.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BTimeStitch.h"

R3BLosProvideTStart::R3BLosProvideTStart()
    : FairTask("R3BLosProvideTStart", 0)
    , fLosCalData("LosCal")
    , fLosTriggerCalData("LosTriggerCal")
    , fEventHeader(nullptr)
{
}

InitStatus R3BLosProvideTStart::Init()
{
    R3BLOG(INFO, "");
    fLosCalData.Init();
    fLosTriggerCalData.Init();

    auto ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        throw std::runtime_error("R3BLosProvideTStart: No FairRootManager");
    }

    fEventHeader = (R3BEventHeader*)ioman->GetObject("EventHeader.");
    if (fEventHeader == nullptr)
    {
        fEventHeader = (R3BEventHeader*)ioman->GetObject("R3BEventHeader");
        R3BLOG(WARNING, "R3BEventHeader was found instead of EventHeader.");
    }
    // Definition of a time stich object to correlate times coming from different systems
    fTimeStitch = new R3BTimeStitch();

    return kSUCCESS;
}

void R3BLosProvideTStart::Exec(Option_t*) { fEventHeader->SetTStart(GetTStart()); }

Double_t R3BLosProvideTStart::GetTStart() const
{
    const auto losCalData = fLosCalData.Retrieve();
    const auto losTriggerCalData = fLosTriggerCalData.Retrieve();
    Float_t diff;
    auto T1 = 10240; // TAMEX, range is 2048*5ns
    auto T2 = 40960; // VFTX, range is 40960*5ns

    const int c1 = std::min(T1,T2);
    const int c2 = std::max(T1,T2);

    if (losCalData.empty())
    {
        return std::numeric_limits<Double_t>::quiet_NaN();
    }
    else if (losTriggerCalData.empty())
    {
        R3BLOG(DEBUG1, "CalData info for LOS");
        return losCalData.back()->GetMeanTimeVFTX();
    }
    else
    {
        if (losTriggerCalData.back()->GetTimeV_ns(0) > 0.)
        {
            R3BLOG(DEBUG1, "CalData with VFTX trigger info for LOS");
            return fTimeStitch->GetTime(
                losCalData.back()->GetMeanTimeVFTX() - losTriggerCalData.back()->GetTimeV_ns(0), "vftx", "vftx");
        }
        else
        {
            R3BLOG(DEBUG1, "CalData with Tamex trigger info for LOS");
            return fTimeStitch->GetTime(
                losCalData.back()->GetMeanTimeVFTX() - losTriggerCalData.back()->GetTimeL_ns(0), "vftx", "tamex");
        }
    }
}

bool R3BLosProvideTStart::IsBeam() const { return !std::isnan(GetTStart()); }

ClassImp(R3BLosProvideTStart);
