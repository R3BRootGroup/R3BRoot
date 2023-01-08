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
// -----                   R3BSTaRTraHitFinder source file                 -----
// -----                  Created 01/06/11  by N.Ashwood               -----
// -------------------------------------------------------------------------
#include "R3BSTaRTraHitFinder.h"
#include "TGeoMatrix.h"
#include "TMath.h"
#include "TVector3.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"
#include "TRandom.h"

#include "TGeoManager.h"

#include "R3BSTaRTraPoint.h"

using std::cout;
using std::endl;

R3BSTaRTraHitFinder::R3BSTaRTraHitFinder()
    : FairTask("R3B Tracker Hit Finder ")
{
    fThreshold = 0.;         // no threshold
    fTrackerResolution = 0.; // perfect resolution
}

R3BSTaRTraHitFinder::~R3BSTaRTraHitFinder() {}

// -----   Public method Init   --------------------------------------------
InitStatus R3BSTaRTraHitFinder::Init()
{
    FairRootManager* ioManager = FairRootManager::Instance();
    if (!ioManager)
        LOG(fatal) << "Init: No FairRootManager";
    fTrackerHitCA = (TClonesArray*)ioManager->GetObject("TraPoint");

    // Register output array TraHit
    fTraHitCA = new TClonesArray("R3BSTaRTrackerHit", 1000);
    ioManager->Register("TrackerHit", "Tracker Hit", fTraHitCA, kTRUE);

    return kSUCCESS;
}

// -----   Public method ReInit   --------------------------------------------
InitStatus R3BSTaRTraHitFinder::ReInit() { return kSUCCESS; }

// -----   Public method Exec   --------------------------------------------
void R3BSTaRTraHitFinder::Exec(Option_t* opt)
{

    Reset();

    Double_t energy = 0.;

    R3BSTaRTraPoint** traHit;

    Int_t traHitsPerEvent = 0;
    traHitsPerEvent = fTrackerHitCA->GetEntries();

    if (traHitsPerEvent > 0)
    {
        traHit = new R3BSTaRTraPoint*[traHitsPerEvent];
        for (Int_t i = 0; i < traHitsPerEvent; i++)
        {
            traHit[i] = new R3BSTaRTraPoint;
            traHit[i] = (R3BSTaRTraPoint*)fTrackerHitCA->At(i);
            energy = ExpResSmearing(traHit[i]->GetEnergyLoss());
        }
    }

    AddHit(traHitsPerEvent, energy);
}

// ---- Public method Reset   --------------------------------------------------
void R3BSTaRTraHitFinder::Reset()
{
    // Clear the CA structure
    // cout << " -I- R3BSTaRTraHitFinder:Reset() called " << endl;

    if (fTraHitCA)
        fTraHitCA->Clear();
}

// ---- Public method Finish   --------------------------------------------------
void R3BSTaRTraHitFinder::Finish()
{
    // here event. write histos
    cout << " -I- Digit Finish() called " << endl;
    // Write control histograms
}

// -----  Public method SetExperimentalResolution  ----------------------------------
void R3BSTaRTraHitFinder::SetExperimentalResolution(Double_t trackerRes)
{
    fTrackerResolution = trackerRes;
    cout << "-I- R3BSTaRTraHitFinder::SetExperimentalResolution to " << fTrackerResolution << endl;
}

// -----  Public method SetDetectionThreshold  ----------------------------------
void R3BSTaRTraHitFinder::SetDetectionThreshold(Double_t thresholdEne)
{
    fThreshold = thresholdEne;
    cout << "-I- R3BSTaRTraHitFinder::SetDetectionThreshold to " << fThreshold << " GeV." << endl;
}

// -----   Private method ExpResSmearing  --------------------------------------------
Double_t R3BSTaRTraHitFinder::ExpResSmearing(Double_t inputEnergy)
{

    if (fTrackerResolution == 0)
        return inputEnergy;
    else
    {
        Double_t energy = gRandom->Gaus(inputEnergy, fTrackerResolution);
        cout << "energy " << energy << " for and Energy of " << inputEnergy << endl;
        return energy;
    }
}

// -----   Private method AddHit  --------------------------------------------
// R3BSTaRTrackerHit* R3BSTaRTraHitFinder::AddHit(UInt_t Nb,Double_t ene){
R3BSTaRTrackerHit* R3BSTaRTraHitFinder::AddHit(UInt_t Nb, Double_t ene)
{
    TClonesArray& clref = *fTraHitCA;
    Int_t size = clref.GetEntriesFast();
    // return new(clref[size]) R3BSTaRTrackerHit(Nb, ene);
    return new (clref[size]) R3BSTaRTrackerHit(Nb, ene);
}

ClassImp(R3BSTaRTraHitFinder)
