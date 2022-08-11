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

#include "R3BNeulandDigitizer.h"
#include "DigitizingTacQuila.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TString.h"
#include <TFile.h>
#include <iostream>
#include <stdexcept>

R3BNeulandDigitizer::R3BNeulandDigitizer(TString input, TString output)
    : R3BNeulandDigitizer(new Neuland::DigitizingTacQuila(), std::move(input), std::move(output))
{
}

R3BNeulandDigitizer::R3BNeulandDigitizer(Neuland::DigitizingEngine* engine, TString input, TString output)
    : FairTask("R3BNeulandDigitizer")
    , fPoints(input)
    , fHits(output)
    , fDigitizingEngine(engine)
{
}

void R3BNeulandDigitizer::SetParContainers()
{
    FairRunAna* run = FairRunAna::Instance();
    if (!run)
    {
        LOG(FATAL) << "R3BNeulandDigitizer::SetParContainers: No analysis run";
        return;
    }

    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    if (!rtdb)
    {
        LOG(FATAL) << "R3BNeulandDigitizer::SetParContainers: No runtime database";
        return;
    }

    fNeulandGeoPar = (R3BNeulandGeoPar*)rtdb->getContainer("R3BNeulandGeoPar");
    if (!fNeulandGeoPar)
    {
        LOG(FATAL) << "R3BNeulandDigitizer::SetParContainers: No R3BNeulandGeoPar";
        return;
    }
}

InitStatus R3BNeulandDigitizer::Init()
{
    fPoints.Init();
    fHits.Init();

    // Initialize control histograms
    hMultOne = new TH1F("MultiplicityOne", "Paddle multiplicity: only one PMT per paddle", 3000, 0, 3000);
    hMultTwo = new TH1F("MultiplicityTwo", "Paddle multiplicity: both PMTs of a paddle", 3000, 0, 3000);
    hRLTimeToTrig = new TH1F("hRLTimeToTrig", "R/Ltime-triggerTime", 200, -100, 100);

    hElossVSQDC = new TH2F("hElossVSQDC", "Energy loss in a paddle vs paddle qdc value", 1000, 0, 1000, 1000, 0, 100);
    hElossVSQDC->GetXaxis()->SetTitle("Deposited Energy [MeV]");
    hElossVSQDC->GetYaxis()->SetTitle("Paddle QDC [a.u.]");

    return kSUCCESS;
}

void R3BNeulandDigitizer::Exec(Option_t*)
{
    fHits.Reset();

    std::map<UInt_t, Double_t> paddleEnergyDeposit;
    // Look at each Land Point, if it deposited energy in the scintillator, store it with reference to the bar
    for (const auto& point : fPoints.Retrieve())
    {
        if (point->GetEnergyLoss() > 0.)
        {
            const Int_t paddleID = point->GetPaddle();

            // Convert position of point to paddle-coordinates, including any rotation or translation
            const TVector3 position = point->GetPosition();
            const TVector3 converted_position = fNeulandGeoPar->ConvertToLocalCoordinates(position, paddleID);
            LOG(DEBUG) << "NeulandDigitizer: Point in paddle " << paddleID
                       << " with global position XYZ: " << position.X() << " " << position.Y() << " " << position.Z();
            LOG(DEBUG) << "NeulandDigitizer: Converted to local position XYZ: " << converted_position.X() << " "
                       << converted_position.Y() << " " << converted_position.Z();

            // Within the paddle frame, the relevant distance of the light from the pmt is always given by the
            // X-Coordinate
            const Double_t dist = converted_position.X();
            fDigitizingEngine->DepositLight(paddleID, point->GetTime(), point->GetLightYield() * 1000., dist);
            paddleEnergyDeposit[paddleID] += point->GetEnergyLoss() * 1000;
        } // eloss
    }     // points

    const Double_t triggerTime = fDigitizingEngine->GetTriggerTime();
    const auto paddles = fDigitizingEngine->ExtractPaddles();

    // Fill control histograms
    hMultOne->Fill(std::count_if(paddles.begin(),
                                 paddles.end(),
                                 [](const std::pair<const Int_t, std::unique_ptr<Neuland::Digitizing::Paddle>>& kv) {
                                     return kv.second->HasHalfFired();
                                 }));

    hMultTwo->Fill(std::count_if(paddles.begin(),
                                 paddles.end(),
                                 [](const std::pair<const Int_t, std::unique_ptr<Neuland::Digitizing::Paddle>>& kv) {
                                     return kv.second->HasFired();
                                 }));

    // Create Hits
    for (const auto& kv : paddles)
    {
        const Int_t paddleID = kv.first;
        const auto& paddle = kv.second;

        if (!paddle->HasFired())
        {
            continue;
        }

        for (UInt_t i = 0; i < paddle->GetNHits(); i++)
        {
            const TVector3 hitPositionLocal = TVector3(paddle->GetPosition(i), 0., 0.);
            const TVector3 hitPositionGlobal = fNeulandGeoPar->ConvertToGlobalCoordinates(hitPositionLocal, paddleID);
            const TVector3 hitPixel = fNeulandGeoPar->ConvertGlobalToPixel(hitPositionGlobal);

            R3BNeulandHit hit(paddleID,
                              paddle->GetLeftChannel()->GetTDC(i),
                              paddle->GetRightChannel()->GetTDC(i),
                              paddle->GetTime(i),
                              paddle->GetLeftChannel()->GetEnergy(i),
                              paddle->GetRightChannel()->GetEnergy(i),
                              paddle->GetEnergy(i),
                              hitPositionGlobal,
                              hitPixel);

            if (fHitFilters.IsValid(hit))
            {
                fHits.Insert(std::move(hit));
            }
        } // loop over all hits for each paddle

    } // loop over paddles

    LOG(DEBUG) << "R3BNeulandDigitizer: produced " << fHits.Size() << " hits";
}

void R3BNeulandDigitizer::Finish()
{
    TDirectory* tmp = gDirectory;
    FairRootManager::Instance()->GetOutFile()->cd();

    gDirectory->mkdir("R3BNeulandDigitizer");
    gDirectory->cd("R3BNeulandDigitizer");

    hMultOne->Write();
    hMultTwo->Write();
    hRLTimeToTrig->Write();
    hElossVSQDC->Write();

    gDirectory = tmp;
}

ClassImp(R3BNeulandDigitizer);
