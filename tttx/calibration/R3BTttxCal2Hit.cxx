/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -------------------------------------------------------------------------
// -----              R3BTttxCal2Hit source file                       -----
// -----               Created 18/01/24  by XXX                        -----
// -------------------------------------------------------------------------

// ROOT headers
#include <TClonesArray.h>
#include <TMath.h>
#include <TRandom.h>
#include <iomanip>

// Fair headers
#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>

// R3B headers
#include "R3BLogger.h"
#include "R3BTttxCal2Hit.h"
#include "R3BTttxCalData.h"
#include "R3BTttxHitData.h"
#include "R3BTttxHitPar.h"

// R3BTttxCal2Hit: Default Constructor --------------------------
R3BTttxCal2Hit::R3BTttxCal2Hit()
    : R3BTttxCal2Hit("R3BTttxCal2Hit", 1)
{
}

// R3BTttxCal2HitPar: Standard Constructor --------------------------
R3BTttxCal2Hit::R3BTttxCal2Hit(const TString& name, int iVerbose)
    : FairTask(name, iVerbose)
{
}

// Virtual R3BTttxCal2Hit: Destructor
R3BTttxCal2Hit::~R3BTttxCal2Hit()
{
    if (fTttxHitData)
        delete fTttxHitData;
}

void R3BTttxCal2Hit::SetParContainers()
{
    // Parameter Containers
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(error, rtdb == nullptr, "FairRuntimeDb not found");

    fHit_Par = dynamic_cast<R3BTttxHitPar*>(rtdb->getContainer("tttxHitPar"));
    if (!fHit_Par)
    {
        R3BLOG(error, "Couldn't get handle on tttxHitPar container");
    }
    else
    {
        R3BLOG(info, "tttxHitPar container open");
    }
}

void R3BTttxCal2Hit::SetParameter()
{
    //--- Parameter Container ---
    NumDets = fHit_Par->GetNumDets(); // Number of Detectors

    R3BLOG(info, "Nb detectors: " << NumDets);
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BTttxCal2Hit::Init()
{
    R3BLOG(info, "");

    FairRootManager* rootManager = FairRootManager::Instance();
    R3BLOG_IF(fatal, rootManager == nullptr, "FairRootManager not found");

    // INPUT DATA
    fTttxCalData = dynamic_cast<TClonesArray*>(rootManager->GetObject("tttxCalData"));
    R3BLOG_IF(fatal, fTttxCalData == nullptr, "tttxCalData not found");

    // OUTPUT DATA
    fTttxHitData = new TClonesArray("R3BTttxHitData");
    rootManager->Register("tttxHitData", "TTTX strip Hit", fTttxHitData, fOnline ? false : true);

    SetParameter();
    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BTttxCal2Hit::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BTttxCal2Hit::Exec(Option_t* /*option*/)
{
    // Reset entries in output arrays, local arrays
    Reset();

    if (!fHit_Par)
    {
        LOG(error) << "NO Container Parameter";
    }

    // Reading the Input -- Cal Data --
    auto nHits = fTttxCalData->GetEntriesFast();
    if (!nHits)
        return;

    // Your code
    // AddHitData(double xpos, double energy, double ang, double charge, double time)

    return;
}

// -----   Public method Reset   ------------------------------------------------
void R3BTttxCal2Hit::Reset()
{
    R3BLOG(debug, "Clearing tttxHitData structure");
    if (fTttxHitData)
        fTttxHitData->Clear();
}

// -----   Private method AddHitData  --------------------------------------------
R3BTttxHitData* R3BTttxCal2Hit::AddHitData(double xpos, double energy, double ang, double charge, double time)
{
    // It fills the R3BTttxHitData
    TClonesArray& clref = *fTttxHitData;
    int size = clref.GetEntriesFast();
    return new (clref[size]) R3BTttxHitData(xpos, energy, ang, charge, time);
}

ClassImp(R3BTttxCal2Hit)
