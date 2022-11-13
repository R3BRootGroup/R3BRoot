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

// ----------------------------------------------------------------
// -----           R3BTwimDigitizer source file               -----
// -----         Created 06/11/19 by JL Rodriguez             -----
// ----------------------------------------------------------------

#include "R3BTwimDigitizer.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"

#include "TMath.h"
#include "TRandom.h"
#include "TVector3.h"
#include <iostream>
#include <string>

#include "R3BLogger.h"
#include "R3BMCTrack.h"
#include "R3BTwimHitData.h"
#include "R3BTwimPoint.h"

// R3BTwimDigitizer: Default Constructor --------------------------
R3BTwimDigitizer::R3BTwimDigitizer()
    : R3BTwimDigitizer("R3BTwimDigitizer", 1)
{
}

// R3BTwimDigitizer: Standard Constructor --------------------------
R3BTwimDigitizer::R3BTwimDigitizer(const TString& name, Int_t iVerbose)
    : FairTask(name + "Digi", iVerbose)
    , fName(name)
    , fDetId(0)
    , fMCTrack(NULL)
    , fTwimPoints(NULL)
    , fTwimHits(NULL)
    , fsigma_x(0.020) // 20um
    , fZsig(0.)
    , fPosX(0.)
    , fPosZ(0.)
    , fangle(0.)
{
}

// Virtual R3BTwimDigitizer: Destructor ----------------------------
R3BTwimDigitizer::~R3BTwimDigitizer()
{
    R3BLOG(debug, "");
    if (fTwimHits)
    {
        delete fTwimHits;
    }
}

// ----   Public method Init  -----------------------------------------
InitStatus R3BTwimDigitizer::Init()
{
    R3BLOG(info, "");

    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    R3BLOG_IF(fatal, !ioman, "FairRootManager not found");

    fMCTrack = (TClonesArray*)ioman->GetObject("MCTrack");
    fTwimPoints = (TClonesArray*)ioman->GetObject(fName + "Point");
    R3BLOG_IF(fatal, !fTwimPoints, fName + "Point not found");

    // Register output array fTwimHits
    fTwimHits = new TClonesArray("R3BTwimHitData");
    ioman->Register(fName + "HitData", "Digital response of Twim" + fName, fTwimHits, kTRUE);

    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BTwimDigitizer::Exec(Option_t* opt)
{
    // The idea of this digitizer is to provide the charge of the fragments
    // from the sum of energy loss in the anodes.
    // At the moment, we take the Z from the point level and store it in the
    // R3BTwimHitData class. This should be changed in the future ...

    Reset();
    // Reading the Input -- Point Data --
    Int_t nHits = fTwimPoints->GetEntriesFast();
    if (nHits == 0)
    {
        return;
    }
    // Data from Point level
    R3BTwimPoint** pointData = new R3BTwimPoint*[nHits];
    Int_t TrackId = 0, PID = 0, anodeId = 0;

    for (Int_t i = 0; i < nHits; i++)
    {
        pointData[i] = (R3BTwimPoint*)(fTwimPoints->At(i));
        TrackId = pointData[i]->GetTrackID();

        auto Track = (R3BMCTrack*)fMCTrack->At(TrackId);
        PID = Track->GetPdgCode();

        if (PID > 1000080160) // Z=8 and A=16
        {
            anodeId = pointData[i]->GetDetCopyID();
            if (anodeId % 16 == 7)
            {
                Double_t fX_in = pointData[i]->GetXIn();
                Double_t fX_out = pointData[i]->GetXOut();
                auto zf = pointData[i]->GetZFF();
                if (zf > 1)
                {
                    int secID = 1 + (anodeId - 7) / 16;
                    AddR3BHitData(secID,
                                  TMath::ATan((fX_out - fX_in) / 2.5),
                                  zf + gRandom->Gaus(0., fZsig),
                                  (fX_out + fX_in) / 2. * 10. + gRandom->Gaus(0., fsigma_x));
                }
            }
        }
    }
    if (pointData)
    {
        delete[] pointData;
    }
    LOG(info) << "R3B" << fName << "Digitizer: " << fTwimHits->GetEntriesFast() << " points registered in this event";
    return;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BTwimDigitizer::ReInit() { return kSUCCESS; }

// -----   Public method Reset   -----------------------------------------------
void R3BTwimDigitizer::Reset()
{
    R3BLOG(debug, "");
    if (fTwimHits)
    {
        fTwimHits->Clear();
    }
}

// -----   Private method AddR3BHitData  -------------------------------------------
R3BTwimHitData* R3BTwimDigitizer::AddR3BHitData(Int_t secId, Double_t theta, Double_t z, Double_t x)
{
    // It fills the R3BHit
    TClonesArray& clref = *fTwimHits;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BTwimHitData(secId, theta, z, x);
}

ClassImp(R3BTwimDigitizer);
