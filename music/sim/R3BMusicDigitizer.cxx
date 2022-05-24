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
// -----            R3BMusicDigitizer source file             -----
// -----         Created 18/10/21  by JL Rodriguez            -----
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

#include "R3BMCTrack.h"
#include "R3BMusicCalData.h"
#include "R3BMusicDigitizer.h"
#include "R3BMusicPoint.h"

// R3BMusicDigitizer: Default Constructor --------------------------
R3BMusicDigitizer::R3BMusicDigitizer()
    : FairTask("R3BMusic Digitizer", 1)
    , fName("Music")
    , fMCTrack(NULL)
    , fMusicPoints(NULL)
    , fMusicCal(NULL)
    , fsigma_x(0.003) // sigma=0.03mm
{
}

// R3BMusicDigitizer: Standard Constructor --------------------------
R3BMusicDigitizer::R3BMusicDigitizer(const TString& name, Int_t iVerbose)
    : FairTask(name + "Digi", iVerbose)
    , fName(name)
    , fMCTrack(NULL)
    , fMusicPoints(NULL)
    , fMusicCal(NULL)
    , fsigma_x(0.003)
{
}

// Virtual R3BMusicDigitizer: Destructor ----------------------------
R3BMusicDigitizer::~R3BMusicDigitizer()
{
    LOG(INFO) << "R3B" + fName + "Digitizer: Delete instance";
    if (fMusicPoints)
        delete fMusicPoints;
    if (fMusicCal)
        delete fMusicCal;
}

// ----   Public method Init  -----------------------------------------
InitStatus R3BMusicDigitizer::Init()
{
    LOG(INFO) << "R3B" + fName + "Digitizer::Init()";

    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        LOG(fatal) << "Init::No FairRootManager";

    fMCTrack = (TClonesArray*)ioman->GetObject("MCTrack");
    fMusicPoints = (TClonesArray*)ioman->GetObject(fName + "Point");

    // Register output array fMusicCal
    fMusicCal = new TClonesArray("R3BMusicCalData", 10);
    ioman->Register(fName + "CalData", "Digital response in " + fName, fMusicCal, kTRUE);

    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BMusicDigitizer::Exec(Option_t* opt)
{
    // The idea of this digitizer is to provide the charge of the fragments
    // from the sum of energy loss in the anodes.

    Reset();
    // Reading the Input -- Point Data --
    Int_t nHits = fMusicPoints->GetEntriesFast();
    if (!nHits)
        return;
    // Data from Point level
    R3BMusicPoint** pointData;
    pointData = new R3BMusicPoint*[nHits];
    Int_t TrackId = 0, PID = 0, anodeId = 0;
    Double_t x[8], zf[4], y = 0., z = 0.;
    Double_t eloss[8];
    for (Int_t i = 0; i < 8; i++)
    {
        x[i] = 0.;
        eloss[i] = 0.;
    }

    for (Int_t i = 0; i < nHits; i++)
    {
        pointData[i] = (R3BMusicPoint*)(fMusicPoints->At(i));
        TrackId = pointData[i]->GetTrackID();

        R3BMCTrack* Track = (R3BMCTrack*)fMCTrack->At(TrackId);
        PID = Track->GetPdgCode();
        anodeId = pointData[i]->GetDetCopyID();
        // std::cout<<anodeId<<std::endl;
        // eloss[anodeId] = eloss[anodeId] + pointData[i]->GetEnergyLoss();

        if (PID > 1000020040) // Z=2 and A=4
        {
            Double_t fX_in = pointData[i]->GetXIn();
            Double_t fX_out = pointData[i]->GetXOut();
            x[anodeId] = (fX_out + fX_in) / 2. + gRandom->Gaus(0., fsigma_x);
            eloss[anodeId] = eloss[anodeId] + pointData[i]->GetEnergyLoss();
        }
    }

    for (Int_t i = 0; i < 8; i++)
    {
        if (eloss[i] > 0.)
            AddHitData(i + 1, x[i], eloss[i]);
    }

    if (pointData)
        delete pointData;
    return;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BMusicDigitizer::ReInit() { return kSUCCESS; }

// -----   Public method Reset   -----------------------------------------------
void R3BMusicDigitizer::Reset()
{
    LOG(DEBUG) << "Clearing R3B" + fName + "Digitizer Structure";
    if (fMusicCal)
        fMusicCal->Clear();
}

// -----   Private method AddR3BHitData  -------------------------------------------
R3BMusicCalData* R3BMusicDigitizer::AddHitData(Int_t anodeId, Double_t pos, Double_t e)
{
    // It fills the R3BMusicCalData
    TClonesArray& clref = *fMusicCal;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BMusicCalData(anodeId, pos, e);
}

ClassImp(R3BMusicDigitizer);
