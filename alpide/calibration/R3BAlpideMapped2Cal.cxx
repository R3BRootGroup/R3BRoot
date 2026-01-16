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
// -----           R3BAlpideMapped2Cal source file             -----
// -----      Created 09/02/22 by J.L. Rodriguez-Sanchez       -----
// -----------------------------------------------------------------

// ROOT headers
#include <TClonesArray.h>
#include <TMath.h>

// FAIR headers
#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>

// ALPIDE headers
#include "R3BAlpideCalData.h"
#include "R3BAlpideMapped2Cal.h"
#include "R3BAlpideMappedData.h"
#include "R3BAlpideMappingPar.h"
#include "R3BLogger.h"

// R3BAlpideMapped2Cal::Default Constructor --------------------------
R3BAlpideMapped2Cal::R3BAlpideMapped2Cal()
    : R3BAlpideMapped2Cal("R3BAlpideMapped2Cal", 1)
{
}

// R3BAlpideMapped2Cal::Standard Constructor --------------------------
R3BAlpideMapped2Cal::R3BAlpideMapped2Cal(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
}

// Virtual R3BAlpideMapped2Cal::Destructor
R3BAlpideMapped2Cal::~R3BAlpideMapped2Cal()
{
    R3BLOG(debug1, "");
    if (fAlpideCalData)
    {
        delete fAlpideCalData;
    }
}

void R3BAlpideMapped2Cal::SetParContainers()
{
    // Parameter Container
    auto* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");

    fMap_Par = dynamic_cast<R3BAlpideMappingPar*>(rtdb->getContainer("alpideMappingPar"));
    R3BLOG_IF(fatal, !fMap_Par, "Container alpideMappingPar not found");
}

void R3BAlpideMapped2Cal::SetParameter()
{
    //--- Parameter Container ---
    // R3BLOG(info, "Nb of sensors: " << fMap_Par->GetNbSensors());
    fMap_Par->printParams();

    auto nSensors = fMap_Par->GetNbSensors();
    auto nNbMaskPixels = fMap_Par->GetNbMaskPixel();

    inUseCache.resize(nSensors, std::vector<std::vector<int>>(DAlpideCols, std::vector<int>(DAlpideRows, 1)));

    for (auto pixel = 0; pixel < nNbMaskPixels; ++pixel)
    {
        auto pixelid = fMap_Par->GetMaskPixel(pixel);
        auto sen = fMap_Par->GetSensorId(pixelid) - 1;
        auto col = fMap_Par->GetCol(pixelid) - 1;
        auto row = fMap_Par->GetRow(pixelid) - 1;
        inUseCache[sen][col][row] = 0;
    }
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BAlpideMapped2Cal::Init()
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
    if (!fAlpideMappedData)
    {
        R3BLOG(fatal, "AlpideMappedData not found");
        return kFATAL;
    }

    // OUTPUT DATA
    fAlpideCalData = new TClonesArray("R3BAlpideCalData");
    mgr->Register("AlpideCalData", "ALPIDE_Cal", fAlpideCalData, !fOnline);
    Reset();

    SetParameter();
    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BAlpideMapped2Cal::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BAlpideMapped2Cal::Exec(Option_t*)
{
    // Reset entries in the output arrays
    Reset();

    // Reading the Input -- Mapped Data --
    if (fAlpideMappedData->GetEntriesFast() == 0)
    {
        return;
    }

    for (auto* obj : *fAlpideMappedData)
    {
        if (auto const* mappedData = dynamic_cast<R3BAlpideMappedData const*>(obj))
        {
            auto sen = mappedData->GetSensorId();
            auto col = mappedData->GetCol();
            auto row = mappedData->GetRow();
            // std::cout << det <<" "<< col <<" "<< row <<std::endl;
            if (inUseCache[sen - 1][col][row] == 1) // this should be 1-base
            {
                AddCalData(sen, row, col);
            }
        }
    }
    return;
}

// -----   Public method Reset   ------------------------------------------------
void R3BAlpideMapped2Cal::Reset()
{
    R3BLOG(debug1, "Clearing CalData Structure");
    if (fAlpideCalData)
    {
        fAlpideCalData->Clear();
    }
}

// -----   Private method AddCalData  --------------------------------------------
R3BAlpideCalData* R3BAlpideMapped2Cal::AddCalData(uint16_t senId, uint16_t row, uint16_t col)
{
    // It fills the R3BAlpideCalData
    TClonesArray& clref = *fAlpideCalData;
    auto size = clref.GetEntriesFast();
    return new (clref[size]) R3BAlpideCalData(senId, row, col);
}

ClassImp(R3BAlpideMapped2Cal)
