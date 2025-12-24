/******************************************************************************
 *   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "FairLogger.h"
#include <FairRootManager.h>

#include "R3BDataPropagator.h"
#include "R3BLogger.h"

#include <Rtypes.h>
#include <TClonesArray.h>
#include <utility>

R3BDataPropagator::R3BDataPropagator()
    : R3BDataPropagator("R3BDataPropagator", 1)
{
}

R3BDataPropagator::R3BDataPropagator(const TString& name, int iVerbose, TString namebranch)
    : FairTask(name, iVerbose)
    , fNameBranch(std::move(namebranch))
{
}

InitStatus R3BDataPropagator::Init()
{
    R3BLOG(info, "");
    auto* frm = FairRootManager::Instance();

    fInputData = dynamic_cast<TClonesArray*>(frm->GetObject(fNameBranch));
    if (!fInputData)
    {
        R3BLOG(fatal, "branch " << fNameBranch << "not found");
        return kFATAL;
    }
    else
        R3BLOG(info, "branch " << fNameBranch << " found");

    frm->Register(fNameBranch, "Data", fInputData, kTRUE);

    return kSUCCESS;
}

void R3BDataPropagator::Exec(Option_t* /*option*/) { R3BLOG(debug, "Called"); } // NOLINT

ClassImp(R3BDataPropagator)
