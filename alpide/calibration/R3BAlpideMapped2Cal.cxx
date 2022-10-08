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

// -----------------------------------------------------------------
// -----           R3BAlpideMapped2Cal source file             -----
// -----      Created 09/02/22 by J.L. Rodriguez-Sanchez       -----
// -----------------------------------------------------------------

// ROOT headers
#include "TClonesArray.h"
#include "TMath.h"

// FAIR headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

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
    , fAlpideMappedData(NULL)
    , fAlpideCalData(NULL)
    , fMap_Par(NULL)
    , fOnline(kFALSE)
{
}

// Virtual R3BAlpideMapped2Cal::Destructor
R3BAlpideMapped2Cal::~R3BAlpideMapped2Cal()
{
    R3BLOG(DEBUG1, "");
    if (fAlpideCalData)
    {
        delete fAlpideCalData;
    }
}

void R3BAlpideMapped2Cal::SetParContainers()
{
    // Parameter Container
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(FATAL, !rtdb, "FairRuntimeDb not found");

    fMap_Par = (R3BAlpideMappingPar*)rtdb->getContainer("alpideMappingPar");
    R3BLOG_IF(FATAL, !fMap_Par, "Container alpideMappingPar not found");
}

void R3BAlpideMapped2Cal::SetParameter()
{
    //--- Parameter Container ---
    // R3BLOG(INFO, "Nb of sensors: " << fMap_Par->GetNbSensors());
    fMap_Par->printParams();
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BAlpideMapped2Cal::Init()
{
    R3BLOG(INFO, "");
    FairRootManager* mgr = FairRootManager::Instance();
    if (!mgr)
    {
        R3BLOG(FATAL, "FairRootManager not found");
        return kFATAL;
    }

    // INPUT DATA
    fAlpideMappedData = (TClonesArray*)mgr->GetObject("AlpideMappedData");
    if (!fAlpideMappedData)
    {
        R3BLOG(FATAL, "AlpideMappedData not found");
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
void R3BAlpideMapped2Cal::Exec(Option_t* option)
{
    // Reset entries in the output arrays
    Reset();

    // Reading the Input -- Mapped Data --
    Int_t nHits = fAlpideMappedData->GetEntriesFast();
    if (nHits == 0)
    {
        return;
    }

    auto mappedData = new R3BAlpideMappedData*[nHits];
    for (Int_t i = 0; i < nHits; i++)
    {
        mappedData[i] = (R3BAlpideMappedData*)(fAlpideMappedData->At(i));
        auto det = mappedData[i]->GetSensorId();
        auto col = mappedData[i]->GetCol();
        auto row = mappedData[i]->GetRow();
        if (fMap_Par->GetInUse(det, col, row) == 1)
        {
            AddCalData(det, col, row);
        }
    }
    if (mappedData)
        delete[] mappedData;
    return;
}

// -----   Private method GetCol  -----------------------------------------------
int R3BAlpideMapped2Cal::GetCol(int reg, int dcol, int ads)
{
    int col = reg * 32 + dcol * 2;
    int lr = ((ads % 4) < 2 ? 1 : 0);
    col += lr;
    return col;
}

// -----   Private method GetRow  -----------------------------------------------
int R3BAlpideMapped2Cal::GetRow(int ads)
{
    // This is OK for pixels within a group of 4
    return ads / 2;
}

// -----   Public method Reset   ------------------------------------------------
void R3BAlpideMapped2Cal::Reset()
{
    R3BLOG(DEBUG1, "Clearing CalData Structure");
    if (fAlpideCalData)
    {
        fAlpideCalData->Clear();
    }
}

// -----   Private method AddCalData  --------------------------------------------
R3BAlpideCalData* R3BAlpideMapped2Cal::AddCalData(UShort_t senId, Int_t col, Int_t row)
{
    // It fills the R3BAlpideCalData
    TClonesArray& clref = *fAlpideCalData;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BAlpideCalData(senId, col, row);
}

ClassImp(R3BAlpideMapped2Cal);
