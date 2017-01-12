#include "R3BNeulandDigitizer.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TString.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "DigitizingTacQuila.h"

R3BNeulandDigitizer::R3BNeulandDigitizer(const TString input, const TString output, const TString outputpx)
    : FairTask("R3BNeulandDigitizer")
    , fDigitizingEngine(new Neuland::DigitizingTacQuila())
    , fInput(input)
    , fOutput(output)
    , fOutputPx(outputpx)
    , fNeulandDigis(new TClonesArray("R3BNeulandDigi"))
    , fNeulandPixels(new TClonesArray("R3BNeulandPixel"))
{
}

R3BNeulandDigitizer::R3BNeulandDigitizer(Neuland::DigitizingEngine* engine,
                                         const TString input,
                                         const TString output,
                                         const TString outputpx)
    : FairTask("R3BNeulandDigitizer")
    , fDigitizingEngine(engine)
    , fInput(input)
    , fOutput(output)
    , fOutputPx(outputpx)
    , fNeulandDigis(new TClonesArray("R3BNeulandDigi"))
    , fNeulandPixels(new TClonesArray("R3BNeulandPixel"))
{
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
    if ((TClonesArray*)ioman->GetObject(fInput) == nullptr)
    {
        LOG(FATAL) << "R3BNeulandDigitizer::Init No NeulandPoints!" << FairLogger::endl;
        return kFATAL;
    }
    if (!TString(((TClonesArray*)ioman->GetObject(fInput))->GetClass()->GetName()).EqualTo("R3BNeulandPoint"))
    {
        LOG(FATAL) << "R3BNeulandDigitizer::Init Branch " << fInput << " does not contain R3BNeulandPoints!"
                   << FairLogger::endl;
        return kFATAL;
    }
    fNeulandPoints = (TClonesArray*)ioman->GetObject(fInput);

    // Set Output: TClonesArray of R3BNeulandDigis, TClonesArray of R3BNeulandPixles
    ioman->Register(fOutput, "Digital response in Neuland", fNeulandDigis.get(), kTRUE);
    ioman->Register(fOutputPx, "Neuland Pixels", fNeulandPixels.get(), kTRUE);

    // Get Paddle Size
    LOG(DEBUG) << "R3BNeulandDigitizer: Paddle Half Length is: " << fNeulandGeoPar->GetPaddleHalfLength()
               << FairLogger::endl;
    fDigitizingEngine->SetPaddleHalfLength(fNeulandGeoPar->GetPaddleHalfLength());

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
    fNeulandDigis->Clear();
    fNeulandPixels->Clear();

    std::map<UInt_t, Double_t> paddleEnergyDeposit;
    // Look at each Land Point, if it deposited energy in the scintillator, store it with reference to the bar
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
    auto paddles = fDigitizingEngine->ExtractPaddles();

    for (const auto& kv : paddles)
    {
        const auto& paddle = kv.second;

        // Multiplicity if only PMT has fired
        if (paddle->GetLeftPMT()->HasFired() && !paddle->GetRightPMT()->HasFired())
        {
            multOne++;
        }
        if (!paddle->GetLeftPMT()->HasFired() && paddle->GetRightPMT()->HasFired())
        {
            multOne++;
        }

        // Multiplicity if two PMT have fired
        if (paddle->GetLeftPMT()->HasFired() && paddle->GetRightPMT()->HasFired())
        {
            multTwo++;
            hRLTimeToTrig->Fill(paddle->GetLeftPMT()->GetTDC() - triggerTime);
            hRLTimeToTrig->Fill(paddle->GetRightPMT()->GetTDC() - triggerTime);
        }
    } // loop over paddles
    hMultOne->Fill(multOne);
    hMultTwo->Fill(multTwo);

    // Create Digis
    for (const auto& kv : paddles)
    {
        const Int_t paddleID = kv.first;
        const auto& paddle = kv.second;

        if (paddle->HasFired())
        {
            const TVector3 digiPositionLocal = TVector3(paddle->GetPosition(), 0., 0.);
            const TVector3 digiPositionGlobal = fNeulandGeoPar->ConvertToGlobalCoordinates(digiPositionLocal, paddleID);
            const TVector3 digiPixel = fNeulandGeoPar->ConvertGlobalToPixel(digiPositionGlobal);

            std::unique_ptr<R3BNeulandDigi> digi(new R3BNeulandDigi(paddleID,
                                                                    paddle->GetLeftPMT()->GetTDC(),
                                                                    paddle->GetRightPMT()->GetTDC(),
                                                                    paddle->GetTime(),
                                                                    paddle->GetLeftPMT()->GetEnergy(),
                                                                    paddle->GetRightPMT()->GetEnergy(),
                                                                    paddle->GetEnergy(),
                                                                    digiPositionGlobal));

            if (IsValid(digi.get()))
            {
                new ((*fNeulandDigis)[fNeulandDigis->GetEntries()]) R3BNeulandDigi(*std::move(digi.release()));

                new ((*fNeulandPixels)[fNeulandPixels->GetEntries()]) R3BNeulandPixel((Int_t)digiPixel.X(),
                                                                                      (Int_t)digiPixel.Y(),
                                                                                      (Int_t)digiPixel.Z(),
                                                                                      (Float_t)paddle->GetEnergy(),
                                                                                      (Float_t)paddle->GetTime());
            }

            hElossVSQDC->Fill(paddleEnergyDeposit[paddleID], paddle->GetEnergy());
        }
    } // loop over paddles

    LOG(DEBUG) << "R3BNeulandDigitizer: produced " << fNeulandDigis->GetEntries() << " digis" << FairLogger::endl;
}

inline Bool_t R3BNeulandDigitizer::IsValid(const R3BNeulandDigi* digi) const
{
    for (const auto& filter : fDigiFilters)
    {
        if (filter(digi) == false)
        {
            return false;
        }
    }
    return true;
}

void R3BNeulandDigitizer::Finish()
{
    hMultOne->Write();
    hMultTwo->Write();
    hRLTimeToTrig->Write();
    hElossVSQDC->Write();
}

ClassImp(R3BNeulandDigitizer);
