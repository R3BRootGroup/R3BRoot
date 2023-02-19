/******************************************************************************
 *   Copyright (C) 2021 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2021-2023 Members of R3B Collaboration                     *
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
// -----            R3BMusicDigitizer source file             -----
// -----          Created 18/10/21 by JL Rodriguez            -----
// ----------------------------------------------------------------

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"

#include "TMath.h"
#include "TRandom.h"
#include <iostream>
#include <string>

#include "R3BLogger.h"
#include "R3BMCTrack.h"
#include "R3BMusicCalData.h"
#include "R3BMusicDigitizer.h"
#include "R3BMusicPoint.h"

// R3BMusicDigitizer: Default Constructor --------------------------
R3BMusicDigitizer::R3BMusicDigitizer()
    : R3BMusicDigitizer("Music", 1)
{
}

// R3BMusicDigitizer: Standard Constructor --------------------------
R3BMusicDigitizer::R3BMusicDigitizer(const TString& name, Int_t iVerbose)
    : FairTask("R3B" + name + "Digitizer", iVerbose)
    , fName(name)
    , fMCTrack(nullptr)
    , fMusicPoints(nullptr)
    , fMusicCal(nullptr)
    , fSigmaX(0.03) // mm
{
}

// Virtual R3BMusicDigitizer: Destructor ----------------------------
R3BMusicDigitizer::~R3BMusicDigitizer()
{
    R3BLOG(debug1, "for " << fName);
    if (fMusicCal)
    {
        delete fMusicCal;
    }
}

// ----   Public method Init  -----------------------------------------
InitStatus R3BMusicDigitizer::Init()
{
    R3BLOG(info, "for " << fName);

    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    R3BLOG_IF(fatal, !ioman, "FairRootManager not found.");

    fMCTrack = dynamic_cast<TClonesArray*>(ioman->GetObject("MCTrack"));
    R3BLOG_IF(fatal, !fMCTrack, "MCTrack not found.");

    fMusicPoints = dynamic_cast<TClonesArray*>(ioman->GetObject(fName + "Point"));
    R3BLOG_IF(fatal, !fMusicPoints, fName << "Point not found.");

    // Register output array fMusicCal
    fMusicCal = new TClonesArray("R3BMusicCalData");
    ioman->Register(fName + "CalData", "Digital response in " + fName, fMusicCal, kTRUE);

    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BMusicDigitizer::Exec(Option_t*)
{
    Reset();
    // Reading the Input -- Point Data --
    int nHits = fMusicPoints->GetEntriesFast();
    if (nHits == 0)
    {
        return;
    }

    int TrackId = 0, PID = 0, anodeId = 0;
    int NbAnodes = 8;
    double x[8], y = 0., z = 0.;
    double eloss[8];
    for (int i = 0; i < NbAnodes; i++)
    {
        x[i] = 0.;
        eloss[i] = 0.;
    }

    for (int i = 0; i < nHits; i++)
    {
        // Data from Point
        auto pointData = dynamic_cast<R3BMusicPoint*>(fMusicPoints->At(i));
        TrackId = pointData->GetTrackID();

        auto Track = dynamic_cast<R3BMCTrack*>(fMCTrack->At(TrackId));
        PID = Track->GetPdgCode();
        anodeId = pointData->GetDetCopyID() - 1;
        eloss[anodeId] = eloss[anodeId] + pointData->GetEnergyLoss() * 1e3; // MeV

        if (PID > 1000020040) // Z=2 and A=4
        {
            double fX_in = pointData->GetXIn();
            double fX_out = pointData->GetXOut();
            x[anodeId] = (fX_out + fX_in) * 5. + gRandom->Gaus(0., fSigmaX); // mm
        }
    }

    for (int i = 0; i < NbAnodes; i++)
        if (eloss[i] > 0.)
        {
            AddHitData(i + 1, x[i], eloss[i]);
        }

    R3BLOG(info, fMusicCal->GetEntriesFast() << " points registered in this event");
    return;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BMusicDigitizer::ReInit() { return kSUCCESS; }

// -----   Public method Reset   -----------------------------------------------
void R3BMusicDigitizer::Reset()
{
    LOG(debug) << "Clearing R3B" + fName + "Digitizer Structure";
    if (fMusicCal)
    {
        fMusicCal->Clear();
    }
}

// -----   Private method AddR3BHitData  -------------------------------------------
R3BMusicCalData* R3BMusicDigitizer::AddHitData(int anodeId, double pos, double e)
{
    // It fills the R3BMusicCalData
    TClonesArray& clref = *fMusicCal;
    int size = clref.GetEntriesFast();
    return new (clref[size]) R3BMusicCalData(anodeId, pos, e);
}

ClassImp(R3BMusicDigitizer);
