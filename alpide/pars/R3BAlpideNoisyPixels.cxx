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

// -----------------------------------------------------------------
// -----           R3BAlpideNoisyPixels source file            -----
// -----      Created 22/03/22 by J.L. Rodriguez-Sanchez       -----
// -----------------------------------------------------------------

// ROOT headers
#include <TClonesArray.h>
#include <TMath.h>

// FAIR headers
#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>

// ALPIDE headers
#include "R3BAlpideMappedData.h"
#include "R3BAlpideNoisyPixels.h"
#include "R3BLogger.h"

// R3BAlpideNoisyPixels::Default Constructor --------------------------
R3BAlpideNoisyPixels::R3BAlpideNoisyPixels()
    : R3BAlpideNoisyPixels("R3BAlpideNoisyPixels", 1)
{
}

// R3BAlpideNoisyPixels::Standard Constructor --------------------------
R3BAlpideNoisyPixels::R3BAlpideNoisyPixels(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
    for (auto c = 0; c < DAlpideCols; c++)
        for (auto r = 0; r < DAlpideRows; r++)
        {
            fMap[c][r].resize(fNbSensors);
            for (auto s = 0; s < fNbSensors; s++)
            {
                fMap[c][r][s] = 0;
            }
        }
}

// Virtual R3BAlpideNoisyPixels::Destructor
R3BAlpideNoisyPixels::~R3BAlpideNoisyPixels() { R3BLOG(debug1, ""); }

// ----  Method SetNbSensors ---------------------------------------------------
void R3BAlpideNoisyPixels::SetNbSensors(int n)
{
    fNbSensors = n;
    for (auto c = 0; c < DAlpideCols; c++)
        for (auto r = 0; r < DAlpideRows; r++)
        {
            fMap[c][r].resize(fNbSensors);
            for (auto s = 0; s < fNbSensors; s++)
                fMap[c][r][s] = 0;
        }
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BAlpideNoisyPixels::Init()
{
    R3BLOG(info, "");
    auto* mgr = FairRootManager::Instance();
    if (!mgr)
    {
        R3BLOG(fatal, "FairRootManager not found");
        return kFATAL;
    }

    // INPUT DATA
    fAlpideMappedData = dynamic_cast<TClonesArray*>(mgr->GetObject("AlpideMappedData"));
    R3BLOG_IF(fatal, !fAlpideMappedData, "AlpideMappedData not found");

    auto* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");
    fMap_Par = dynamic_cast<R3BAlpideMappingPar*>(rtdb->getContainer("alpideMappingPar"));
    R3BLOG_IF(fatal, !fMap_Par, "Couldn't get handle on alpideMappingPar container");

    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BAlpideNoisyPixels::Exec(Option_t*)
{
    // Reading the Input -- Mapped Data --
    if (fAlpideMappedData->GetEntries() == 0)
    {
        return;
    }

    for (auto* obj : *fAlpideMappedData)
    {
        if (auto const* mappedData = dynamic_cast<R3BAlpideMappedData const*>(obj))
        {
            auto det = mappedData->GetSensorId() - 1;
            auto col = mappedData->GetCol() - 1;
            auto row = mappedData->GetRow() - 1;
            fMap[col][row][det]++;
        }
    }
    return;
}

void R3BAlpideNoisyPixels::FinishTask()
{
    fMap_Par->SetNbSensors(fNbSensors);
    for (auto c = 0; c < DAlpideCols; c++)
        for (auto r = 0; r < DAlpideRows; r++)
            for (auto s = 0; s < fNbSensors; s++)
                if (fMap[c][r][s] > fThr)
                {
                    fMap_Par->SetMaskPixel((s + 1) * 10000000 + (r + 1) * 10000 + c + 1);
                }
    fMap_Par->setChanged();
}

ClassImp(R3BAlpideNoisyPixels)
