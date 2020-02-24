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

R3BLosProvideTStart::R3BLosProvideTStart()
    : FairTask("R3BLosProvideTStart", 0)
    , fLosCalData("LosCal")
    , fEventHeader(nullptr)
{
}

InitStatus R3BLosProvideTStart::Init()
{
    fLosCalData.Init();

    auto ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        throw std::runtime_error("R3BLosProvideTStart: No FairRootManager");
    }

    fEventHeader = (R3BEventHeader*)ioman->GetObject("R3BEventHeader");
    if (fEventHeader == nullptr)
    {
        throw std::runtime_error("R3BLosProvideTStart: No R3BEventHeader");
    }
}

void R3BLosProvideTStart::Exec(Option_t*) { fEventHeader->SetTStart(GetTStart()); }

Double_t R3BLosProvideTStart::GetTStart() const
{
    const auto losCalData = fLosCalData.Retrieve();
    if (losCalData.empty())
    {
        return std::numeric_limits<Double_t>::quiet_NaN();
    }
    return losCalData.back()->GetMeanTimeVFTX();
}

bool R3BLosProvideTStart::IsBeam() const { return !std::isnan(GetTStart()); }

ClassImp(R3BLosProvideTStart)
