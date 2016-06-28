#include "R3BNeulandDigitizer.h"

#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "TGeoManager.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"
#include "TGeoNode.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

R3BNeulandDigitizer::R3BNeulandDigitizer()
    : FairTask("R3B NeuLAND Digitizer", 0)
    , fNeulandDigis(new TClonesArray("R3BNeulandDigi"))
    , fDigitizingEngine(new Neuland::DigitizingEngine())
{
}

R3BNeulandDigitizer::~R3BNeulandDigitizer()
{
    if (fNeulandDigis)
    {
        delete fNeulandDigis;
    }
    if (fDigitizingEngine)
    {
        delete fDigitizingEngine;
    }
}

void R3BNeulandDigitizer::SetParContainers()
{
    FairRunAna* run = FairRunAna::Instance();
    if (!run)
    {
        LOG(FATAL) << "R3BNeulandDigitizer::SetParContainers: No analysis run" << FairLogger::endl;
        return;
    }

    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    if (!rtdb)
    {
        LOG(FATAL) << "R3BNeulandDigitizer::SetParContainers: No runtime database" << FairLogger::endl;
        return;
    }

    fNeulandGeoPar = (R3BNeulandGeoPar*)rtdb->getContainer("R3BNeulandGeoPar");
    if (!fNeulandGeoPar)
    {
        LOG(FATAL) << "R3BNeulandDigitizer::SetParContainers: No R3BNeulandGeoPar" << FairLogger::endl;
        return;
    }
}

InitStatus R3BNeulandDigitizer::Init()
{
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        LOG(FATAL) << "R3BNeulandDigitizer::Init: No FairRootManager" << FairLogger::endl;
        return kFATAL;
    }

    // Set Input: TClonesArray of R3BNeulandPoints
    if ((TClonesArray*)ioman->GetObject("NeulandPoints") == nullptr)
    {
        LOG(FATAL) << "R3BNeulandDigitizer::Init No NeulandPoints!" << FairLogger::endl;
        return kFATAL;
    }
    if (!TString(((TClonesArray*)ioman->GetObject("NeulandPoints"))->GetClass()->GetName()).EqualTo("R3BNeulandPoint"))
    {
        LOG(FATAL) << "R3BNeulandDigitizer::Init Branch NeulandPoints does not contain R3BNeulandPoints!"
                   << FairLogger::endl;
        return kFATAL;
    }
    fNeulandPoints = (TClonesArray*)ioman->GetObject("NeulandPoints");

    // Set Output: TClonesArray of R3BNeulandDigis
    ioman->Register("NeulandDigis", "Digital response in Neuland", fNeulandDigis, kTRUE);

    // Get Paddle Size
    LOG(DEBUG) << "R3BNeulandDigitizer: Paddle Half Length is: " << fNeulandGeoPar->GetPaddleHalfLength()
               << FairLogger::endl;
    fDigitizingEngine->SetPaddleHalfLength(fNeulandGeoPar->GetPaddleHalfLength());

    // Initialise control histograms
    hPMl = new TH1F("PM_left", "Arrival times of left PM", 1000, 0., 1000.);
    hPMr = new TH1F("PM_right", "Arrival times of right PM", 1000, 0., 1000.);
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
    Reset();

    std::map<UInt_t, Double_t> paddleEnergyDeposit;

    // Look at each Land Point, if it deposited energy in the szintillator, store it with reference to the bar
    const UInt_t nPoints = fNeulandPoints->GetEntries();
    R3BNeulandPoint* point;
    for (UInt_t l = 0; l < nPoints; l++)
    {
        point = (R3BNeulandPoint*)fNeulandPoints->At(l);

        if (point->GetEnergyLoss() > 0.)
        {
            const Int_t paddleID = point->GetPaddle();

            // Convert position of point to paddle-coordinates, including any rotation or translation
            const TVector3 position = point->GetPosition();
            const TVector3 converted_position = fNeulandGeoPar->ConvertToLocalCoordinates(position, paddleID);
            LOG(DEBUG) << "NeulandDigitizer: Point in paddle " << paddleID
                       << " with global position XYZ: " << position.X() << " " << position.Y() << " " << position.Z()
                       << FairLogger::endl;
            LOG(DEBUG) << "NeulandDigitizer: Converted to local position XYZ: " << converted_position.X() << " "
                       << converted_position.Y() << " " << converted_position.Z() << FairLogger::endl;

            // Within the paddle frame, the relevant distance of the light from the pmt is always given by the
            // X-Coordinate
            const Double_t dist = converted_position.X();
            try
            {
                fDigitizingEngine->DepositLight(paddleID, point->GetTime(), point->GetLightYield() * 1000., dist);
                paddleEnergyDeposit[paddleID] += point->GetEnergyLoss() * 1000;
            }
            catch (std::exception& e)
            {
                LOG(FATAL) << "NeulandDigitizer: " << e.what() << FairLogger::endl;
            }

        } // eloss
    }     // MC hits

    // Fill histograms
    Int_t multOne = 0;
    Int_t multTwo = 0;
    const Double_t triggerTime = fDigitizingEngine->GetTriggerTime();
    for (const auto& kv : fDigitizingEngine->paddles)
    {
        const auto& paddle = kv.second;

        for (const auto& hit : paddle.leftPMT.GetHits())
        {
            hPMl->Fill(hit.time);
        }
        for (const auto& hit : paddle.rightPMT.GetHits())
        {
            hPMr->Fill(hit.time);
        }

        // Multiplicity if only PMT has fired
        if (paddle.leftPMT.HasFired() && !paddle.rightPMT.HasFired())
        {
            multOne++;
        }
        if (!paddle.leftPMT.HasFired() && paddle.rightPMT.HasFired())
        {
            multOne++;
        }

        // Multiplicity if two PMT have fired
        if (paddle.leftPMT.HasFired() && paddle.rightPMT.HasFired())
        {
            multTwo++;
            hRLTimeToTrig->Fill(paddle.leftPMT.GetTDC() - triggerTime);
            hRLTimeToTrig->Fill(paddle.rightPMT.GetTDC() - triggerTime);
        }
    } // loop over paddles
    hMultOne->Fill(multOne);
    hMultTwo->Fill(multTwo);

    // Create Digis
    for (const auto& kv : fDigitizingEngine->paddles)
    {
        const Int_t paddleID = kv.first;
        const auto& paddle = kv.second;

        if (paddle.HasFired())
        {
            const TVector3 digiPositionLocal = TVector3(paddle.GetPosition(), 0., 0.);
            const TVector3 digiPositionGlobal = fNeulandGeoPar->ConvertToGlobalCoordinates(digiPositionLocal, paddleID);

            new ((*fNeulandDigis)[fNeulandDigis->GetEntries()]) R3BNeulandDigi(paddleID,
                                                                               paddle.leftPMT.GetTDC(),
                                                                               paddle.rightPMT.GetTDC(),
                                                                               paddle.GetPaddleTime(),
                                                                               paddle.leftPMT.GetEnergy(),
                                                                               paddle.rightPMT.GetEnergy(),
                                                                               paddle.GetPaddleEnergy(),
                                                                               digiPositionGlobal);

            hElossVSQDC->Fill(paddleEnergyDeposit[paddleID], paddle.GetPaddleEnergy());
        }
    } // loop over paddles

    if (fVerbose)
    {
        LOG(INFO) << "R3BNeulandDigitizer: produced " << fNeulandDigis->GetEntries() << " digis" << FairLogger::endl;
    }
}

void R3BNeulandDigitizer::Reset()
{
    fNeulandDigis->Clear();
    fDigitizingEngine->Clear();
}

void R3BNeulandDigitizer::Finish()
{
    hPMl->Write();
    hPMr->Write();
    hMultOne->Write();
    hMultTwo->Write();
    hRLTimeToTrig->Write();
    hElossVSQDC->Write();
}

ClassImp(R3BNeulandDigitizer);
