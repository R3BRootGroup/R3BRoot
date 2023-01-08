/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
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
#include "TClonesArray.h"
#include "TMath.h"

// FAIR headers
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

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
    , fAlpideMappedData(NULL)
    , fMap_Par(NULL)
    , fNbSensors(1)
    , fThr(1000)
{
    for (Int_t c = 0; c < DAlpideCols; c++)
        for (Int_t r = 0; r < DAlpideRows; r++)
        {
            fMap[c][r].resize(fNbSensors);
            for (Int_t s = 0; s < fNbSensors; s++)
            {
                fMap[c][r][s] = 0;
            }
        }
}

// Virtual R3BAlpideNoisyPixels::Destructor
R3BAlpideNoisyPixels::~R3BAlpideNoisyPixels() { R3BLOG(debug1, ""); }

// ----  Method SetNbSensors ---------------------------------------------------
void R3BAlpideNoisyPixels::SetNbSensors(UInt_t n)
{
    fNbSensors = n;
    for (Int_t c = 0; c < DAlpideCols; c++)
        for (Int_t r = 0; r < DAlpideRows; r++)
        {
            fMap[c][r].resize(fNbSensors);
            for (Int_t s = 0; s < fNbSensors; s++)
                fMap[c][r][s] = 0;
        }
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BAlpideNoisyPixels::Init()
{
    R3BLOG(info, "");
    FairRootManager* mgr = FairRootManager::Instance();
    if (!mgr)
    {
        R3BLOG(fatal, "FairRootManager not found");
        return kFATAL;
    }

    // INPUT DATA
    fAlpideMappedData = (TClonesArray*)mgr->GetObject("AlpideMappedData");
    if (!fAlpideMappedData)
    {
        R3BLOG(fatal, "AlpideMappedData not found");
        return kFATAL;
    }

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");
    fMap_Par = (R3BAlpideMappingPar*)rtdb->getContainer("alpideMappingPar");
    if (!fMap_Par)
    {
        R3BLOG(fatal, "Couldn't get handle on alpideMappingPar container");
        return kFATAL;
    }

    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BAlpideNoisyPixels::Exec(Option_t* option)
{
    // Reading the Input -- Mapped Data --
    Int_t nHits = fAlpideMappedData->GetEntries();
    if (nHits == 0)
    {
        return;
    }

    auto mappedData = new R3BAlpideMappedData*[nHits];

    for (Int_t i = 0; i < nHits; i++)
    {
        mappedData[i] = (R3BAlpideMappedData*)(fAlpideMappedData->At(i));
        auto det = mappedData[i]->GetSensorId() - 1;
        auto col = mappedData[i]->GetCol() - 1;
        auto row = mappedData[i]->GetAds() - 1;
        fMap[col][row][det]++;
    }
    if (mappedData)
        delete[] mappedData;
    return;
}

void R3BAlpideNoisyPixels::FinishTask()
{
    fMap_Par->SetNbSensors(fNbSensors);
    for (Int_t c = 0; c < DAlpideCols; c++)
        for (Int_t r = 0; r < DAlpideRows; r++)
            for (Int_t s = 0; s < fNbSensors; s++)
                if (fMap[c][r][s] > fThr)
                {
                    fMap_Par->SetInUse(s + 1, c + 1, r + 1, 0);
                }
    fMap_Par->setChanged();
}

ClassImp(R3BAlpideNoisyPixels);
