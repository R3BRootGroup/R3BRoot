#include "R3BNeulandDigitizer.h"

#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "TGeoManager.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TH1F.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"


inline bool IsHorizontalPaddle(const Int_t paddle, const Int_t paddle_per_plane)
{
    // TODO: Wait, what?
    return (int)(((paddle - 1) / paddle_per_plane)) / 2. == (int)((int)(((paddle - 1) / paddle_per_plane)) / 2.);
}


R3BNeulandDigitizer::R3BNeulandDigitizer()
    : FairTask("R3B NeuLAND Digitizer", 0),
      fLandDigi(new TClonesArray("R3BLandDigi")),
      fDigitizingEngine(new Neuland::DigitizingEngine())
{
}


R3BNeulandDigitizer::~R3BNeulandDigitizer()
{
}


void R3BNeulandDigitizer::SetParContainers()
{
    FairRunAna *run = FairRunAna::Instance();
    if (!run) {
        Fatal("SetParContainers", "No analysis run");
        return;
    }

    FairRuntimeDb *rtdb = run->GetRuntimeDb();
    if (!rtdb) {
        Fatal("SetParContainers", "No runtime database");
        return;
    }

    fLandDigiPar = (R3BLandDigiPar *)rtdb->getContainer("R3BLandDigiPar");
    if (!fLandDigiPar) {
        Fatal("SetParContainers", "No R3BLandDigiPar");
        return;
    }

    if (fVerbose && fLandDigiPar) {
        LOG(INFO) << "R3BNeulandDigitizer::SetParContainers() " << FairLogger::endl;
        LOG(INFO) << "Container R3BLandDigiPar loaded " << FairLogger::endl;
    }
}


InitStatus R3BNeulandDigitizer::Init()
{
    FairRootManager *ioman = FairRootManager::Instance();
    if (!ioman) {
        Fatal("Init", "No FairRootManager");
        return kFATAL;
    }

    fLandPoints = (TClonesArray *) ioman->GetObject("NeulandPoints");
    if (fLandPoints == nullptr) {
        fLandPoints = (TClonesArray *) ioman->GetObject("LandPoint");
    }
    if (fLandPoints == nullptr) {
        LOG(FATAL) << "R3BNeulandDigitizer: No NeulandPoints!" << FairLogger::endl;
        return kFATAL;
    }

    ioman->Register("LandDigi", "Digital response in Land", fLandDigi, kTRUE);


    // Parameter retrieval
    // Only after Init one retrieve the Digitization Parameters!
    npaddles = fLandDigiPar->GetMaxPaddle() + 1;
    nplanes = fLandDigiPar->GetMaxPlane();
    LOG(INFO) << "R3BNeulandDigitizer: # paddles: " << npaddles - 1 << ", # planes: " << nplanes << FairLogger::endl;
    paddle_per_plane = (npaddles - 1) / nplanes;
    LOG(INFO) << "R3BNeulandDigitizer: paddles per plane: " << paddle_per_plane << FairLogger::endl;

    // half of the length of a scintillator
    fPlength = fLandDigiPar->GetPaddleLength(); // [cm]
    LOG(INFO) << "R3BNeulandDigitizer: paddle length: " << fPlength << FairLogger::endl;
    fDigitizingEngine->SetPaddleHalfLength(fPlength);

    // Initialise control histograms
    hPMl = new TH1F("PM_left", "Arrival times of left PM", 1000, 0., 1000.);
    hPMr = new TH1F("PM_right", "Arrival times of right PM", 1000, 0., 1000.);
    hMultOne = new TH1F("MultiplicityOne", "Paddle multiplicity: only one PMT per paddle", 3000, 0, 3000);
    hMultTwo = new TH1F("MultiplicityTwo", "Paddle multiplicity: both PMTs of a paddle",   3000, 0, 3000);
    hRLTimeToTrig = new TH1F("hRLTimeToTrig", "R/Ltime-triggerTime", 200, -100, 100);

    return kSUCCESS;
}


void R3BNeulandDigitizer::Exec(Option_t *)
{
    Reset();

    Double_t xpaddle[npaddles], ypaddle[npaddles], zpaddle[npaddles];

    /* Look at each Land Point, if it deposited energy in the szintillator,
     * store it with reference to the bar */
    const UInt_t nLandPoints = fLandPoints->GetEntries();
    R3BLandPoint *landPoint;
    for (UInt_t l = 0; l < nLandPoints; l++) {
        landPoint = (R3BLandPoint *)fLandPoints->At(l);
        const Int_t paddle_id = int(landPoint->GetSector()) - 1; //note that paddle starts at 1

        Double_t light = landPoint->GetLightYield() * 1000.;
        Int_t media = int(landPoint->GetPaddleType());

        if (landPoint->GetEnergyLoss() > 0. && media == 3) {

            // TODO: What is this?
            gGeoManager->FindNode(landPoint->GetXIn(), landPoint->GetYIn(), landPoint->GetZIn());
            gGeoManager->CdUp();
            Double_t local_point[] = {0., 0., 0.};
            Double_t global_point[3];
            gGeoManager->LocalToMaster(local_point, global_point);
            xpaddle[paddle_id] = global_point[0];
            ypaddle[paddle_id] = global_point[1];
            zpaddle[paddle_id] = global_point[2];

            Double_t dist;
            if (fLandDigiPar->GetGeometryFileName().Contains("proto")) {
                dist = landPoint->GetYIn(); // only vertical paddles
            } else {
                if (IsHorizontalPaddle(paddle_id, paddle_per_plane)) {
                    dist = landPoint->GetXIn();
                } else {
                    dist = landPoint->GetYIn();
                }
            }

            try {
                fDigitizingEngine->DepositLight(paddle_id, landPoint->GetTime(), light, dist);
            } catch (std::exception &e) {
                Fatal("Exec", "%s", e.what());
            }

        } //! eloss
    } //! MC hits


    const Double_t triggerTime = fDigitizingEngine->GetTriggerTime();


    /* Fill histograms */
    Int_t multOne = 0;
    Int_t multTwo = 0;
    for (const auto &kv : fDigitizingEngine->paddles) {
        const auto &paddle = kv.second;

        for (const auto &hit : paddle.leftPMT.GetHits()) {
            hPMl->Fill(hit.time);
        }
        for (const auto &hit : paddle.rightPMT.GetHits()) {
            hPMr->Fill(hit.time);
        }

        // Multiplicity if only PMT has fired
        if (paddle.leftPMT.HasFired() && !paddle.rightPMT.HasFired()) {
            multOne++;
        }
        if (!paddle.leftPMT.HasFired() && paddle.rightPMT.HasFired()) {
            multOne++;
        }

        // Multiplicity if two PMT have fired
        if (paddle.leftPMT.HasFired() && paddle.rightPMT.HasFired()) {
            multTwo++;
            hRLTimeToTrig->Fill(paddle.leftPMT.GetTDC() - triggerTime);
            hRLTimeToTrig->Fill(paddle.rightPMT.GetTDC() - triggerTime);
        }
    }
    hMultOne->Fill(multOne);
    hMultTwo->Fill(multTwo);



    for (const auto &kv : fDigitizingEngine->paddles) {
        const Int_t paddleNr = kv.first;
        const auto &paddle = kv.second;

        if (paddle.HasFired()) {

            // Get position and other information and fill digis.
            Double_t xx, yy, zz;

            if (fLandDigiPar->GetGeometryFileName().Contains("proto")) {
                // vertical paddles
                xx = xpaddle[paddleNr];
                yy = paddle.GetPosition();
                zz = zpaddle[paddleNr];
            } else {
                if (IsHorizontalPaddle(paddleNr, paddle_per_plane)) {
                    //horizontal paddles
                    xx = paddle.GetPosition();
                    yy = ypaddle[paddleNr];
                    zz = zpaddle[paddleNr];
                } else {
                    // vertical paddles
                    xx = xpaddle[paddleNr];
                    yy = paddle.GetPosition();
                    zz = zpaddle[paddleNr];
                }
            }

            Double_t tdcL = paddle.leftPMT.GetTDC();
            Double_t tdcR = paddle.rightPMT.GetTDC();
            Double_t qdcL = paddle.leftPMT.GetEnergy();
            Double_t qdcR = paddle.rightPMT.GetEnergy();

            Double_t qdc = paddle.GetPaddleEnergy();
            Double_t tdc = paddle.GetPaddleTime();

            new((*fLandDigi)[fLandDigi->GetEntriesFast()]) R3BLandDigi(paddleNr,
                    tdcL, tdcR, tdc,
                    qdcL, qdcR, qdc,
                    xx, yy, zz);
        }
    } // loop over paddles



    if (fVerbose) {
        LOG(INFO) << "R3BNeulandDigitizer: produced "
                  << fLandDigi->GetEntries() << " digis" << FairLogger::endl;
    }
}


void R3BNeulandDigitizer::Reset()
{
    fLandDigi->Clear();
    fDigitizingEngine->Clear();
}


void R3BNeulandDigitizer::Finish()
{
    hPMl->Write();
    hPMr->Write();
    hMultOne->Write();
    hMultTwo->Write();
    hRLTimeToTrig->Write();
}


ClassImp(R3BNeulandDigitizer);
