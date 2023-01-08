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

// -------------------------------------------------------------------------
// -----                 R3BStartrackCalibParFinder source file        -----
// -----                 from R3BCaloCalibParFinder source file        -----
// -----                  Created 22/07/14  by H.Alvarez               -----
// -------------------------------------------------------------------------
#include "TGeoMatrix.h"
#include "TMath.h"
#include "TVector3.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TRandom.h"

#include "TGeoManager.h"

#include "R3BStartrackCalibParFinder.h"
#include "R3BStartrackRawHit.h"

using std::cerr;
using std::cout;
using std::endl;

R3BStartrackCalibParFinder::R3BStartrackCalibParFinder()
    : FairTask("R3B Startracker Calibration Parameters Finder ")
{
}

R3BStartrackCalibParFinder::~R3BStartrackCalibParFinder() {}

// -----   Public method Init   --------------------------------------------
InitStatus R3BStartrackCalibParFinder::Init()
{
    Register();
    return kSUCCESS;
}

void R3BStartrackCalibParFinder::SetParContainers()
{
    // Get run and runtime database
    FairRunAna* run = FairRunAna::Instance();
    if (!run)
        LOG(fatal) << "R3BStartrackCalibParFinder::SetParContainers: No analysis run";

    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    if (!rtdb)
        LOG(fatal) << "R3BStartrackCalibParFinder::SetParContainers: No runtime database";

    fStartrackCalibPar = (R3BStartrackCalibPar*)(rtdb->getContainer("R3BStartrackCalibPar"));

    if (fVerbose && fStartrackCalibPar)
    {
        LOG(info) << "R3BStartrackCalibParFinder::SetParContainers() ";
        LOG(info) << "Container R3BStartrackCalibPar loaded ";
    }
}

// Register: Protected method
void R3BStartrackCalibParFinder::Register()
{
    LOG(debug) << "Registering";
    FairRootManager* fMan = FairRootManager::Instance();
    if (!fMan)
    {
        LOG(fatal) << "Init: No FairRootManager";
        return;
    }
    fSiDetRawHitCA = (TClonesArray*)fMan->GetObject("StartrackRawHit");

    // Parameter retrieval from par container
    // ...
    // Table for crystal parameters from Geometry
    // Initialization of variables, histograms, ...

    return;
}

// -----   Public method ReInit   --------------------------------------------
InitStatus R3BStartrackCalibParFinder::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void R3BStartrackCalibParFinder::Exec(Option_t* opt)
{

    // Reset entries in output arrays, local arrays
    Reset();
}

// ---- Public method Reset   --------------------------------------------------
void R3BStartrackCalibParFinder::Reset()
{
    // Clear the CA structure

    if (fSiDetRawHitCA)
        fSiDetRawHitCA->Clear();
}

// ---- Public method Finish   --------------------------------------------------
void R3BStartrackCalibParFinder::Finish() {}

ClassImp(R3BStartrackCalibParFinder)
