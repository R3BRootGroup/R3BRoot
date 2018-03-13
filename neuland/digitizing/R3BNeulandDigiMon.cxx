#include "R3BNeulandDigiMon.h"

#include <algorithm>
#include <iostream>
#include <numeric>

#include "TClonesArray.h"
#include "TDirectory.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

#include "FairLogger.h"
#include "FairRootManager.h"

#include "R3BNeulandDigi.h"

R3BNeulandDigiMon::R3BNeulandDigiMon(const TString input, const TString output, const Option_t* option)
    : FairTask("R3B NeuLAND NeulandDigi Monitor")
    , fInput(input)
    , fOutput(output)
{
    LOG(INFO) << "Using R3B NeuLAND NeulandDigi Monitor" << FairLogger::endl;

    TString opt = option;
    opt.ToUpper();

    if (opt.Contains("3DTRACK"))
    {
        fIs3DTrackEnabled = true;
        LOG(INFO) << "... with 3D track visualization" << FairLogger::endl;
    }
    else
    {
        fIs3DTrackEnabled = false;
    }
}

R3BNeulandDigiMon::~R3BNeulandDigiMon() {}

InitStatus R3BNeulandDigiMon::Init()
{
    FairRootManager* rm = FairRootManager::Instance();

    fDigis = (TClonesArray*)rm->GetObject(fInput);
    if (fDigis == nullptr)
    {
        LOG(FATAL) << "R3BNeulandDigiMon: No NeulandDigis!" << FairLogger::endl;
        return kFATAL;
    }

    if (fIs3DTrackEnabled)
    {
        // XYZ -> ZXY (side view)
        fh3 = new TH3D("hDigis", "hDigis", 60, 1400, 1700, 50, -125, 125, 50, -125, 125);
        fh3->SetTitle("NeuLAND Digis");
        fh3->GetXaxis()->SetTitle("Z");
        fh3->GetYaxis()->SetTitle("X");
        fh3->GetZaxis()->SetTitle("Y");

        rm->Register("NeulandDigiMon", "Digis in NeuLAND", fh3, kTRUE);
    }

    hTime = new TH1D("hTime", "Digi time", 400, 0, 200);
    hDepth = new TH1D("hDepth", "Maxial penetration depth", 60, 1400, 1700);
    hForemostEnergy = new TH1D("hForemostEnergy", "Foremost energy deposition", 100, 0, 100);
    hSternmostEnergy = new TH1D("hSternmostEnergy", "Sternmost energy deposition", 100, 0, 100);
    hDepthVSForemostEnergy = new TH2D("hDepthVSFrontEnergy", "Depth vs Foremost Energy", 60, 1400, 1700, 100, 0, 100);
    hDepthVSSternmostEnergy =
        new TH2D("hDepthVSSternmostEnergy", "Depth vs Sternmost Energy", 60, 1400, 1700, 100, 0, 100);
    hEtot = new TH1D("hEtot", "Total Energy", 10000, 0, 10000);
    hDepthVSEtot = new TH2D("hDepthVSEtot", "Depth vs Total Energy", 60, 1400, 1700, 1000, 0, 1000);
    hdeltaEE = new TH2D("hdeltaEE", "Energy in Foremost Plane vs Etot", 100, 0, 2000, 100, 0, 250);
    hPosVSEnergy = new TH2D("hPosVSEnergy", "Position vs Energy deposition", 60, 1400, 1700, 1000, 0, 1000);
    hBeta = new TH1D("hBeta", "Velocity", 200., 0., 1.);
    hE = new TH1D("hE", "Digi Energy", 300, 0., 300.);

    return kSUCCESS;
}

void R3BNeulandDigiMon::Exec(Option_t*)
{
    const unsigned int nDigis = fDigis->GetEntries();

    if (fIs3DTrackEnabled)
    {
        fh3->Reset("ICES");
        R3BNeulandDigi* digi;
        for (unsigned int i = 0; i < nDigis; i++)
        {
            digi = (R3BNeulandDigi*)fDigis->At(i);
            // XYZ -> ZXY (side view)
            fh3->Fill(digi->GetPosition().Z(), digi->GetPosition().X(), digi->GetPosition().Y(), digi->GetE());
        }
    }

    std::vector<R3BNeulandDigi*> digis;
    for (unsigned int i = 0; i < nDigis; i++)
    {
        digis.push_back((R3BNeulandDigi*)fDigis->At(i));
    }

    for (auto digi : digis)
    {
        hPosVSEnergy->Fill(digi->GetPosition().Z(), digi->GetE());
        hTime->Fill(digi->GetT());
        hBeta->Fill(digi->GetBeta());
        hE->Fill(digi->GetE());
    }

    auto maxDepthDigi = std::max_element(digis.begin(), digis.end(), [](R3BNeulandDigi* a, R3BNeulandDigi* b) {
        return a->GetPosition().Z() < b->GetPosition().Z();
    });
    if (maxDepthDigi != digis.end())
    {
        hDepth->Fill((*maxDepthDigi)->GetPosition().Z());
        hSternmostEnergy->Fill((*maxDepthDigi)->GetE());
        hDepthVSSternmostEnergy->Fill((*maxDepthDigi)->GetPosition().Z(), (*maxDepthDigi)->GetE());
    }

    auto minDepthDigi = std::min_element(digis.begin(), digis.end(), [](R3BNeulandDigi* a, R3BNeulandDigi* b) {
        return a->GetPosition().Z() < b->GetPosition().Z();
    });
    auto Etot = std::accumulate(
        digis.begin(), digis.end(), Double_t(0.), [](const Double_t a, R3BNeulandDigi* b) { return a + b->GetE(); });

    if (minDepthDigi != digis.end())
    {
        hForemostEnergy->Fill((*minDepthDigi)->GetE());
        hDepthVSForemostEnergy->Fill((*maxDepthDigi)->GetPosition().Z(), (*minDepthDigi)->GetE());
        hdeltaEE->Fill(Etot, (*minDepthDigi)->GetE());
    }

    hEtot->Fill(Etot);
    if (maxDepthDigi != digis.end())
    {
        hDepthVSEtot->Fill((*maxDepthDigi)->GetPosition().Z(), Etot);
    }
}

void R3BNeulandDigiMon::Finish()
{
    TDirectory* tmp = gDirectory;
    FairRootManager::Instance()->GetOutFile()->cd();

    gDirectory->mkdir(fOutput);
    gDirectory->cd(fOutput);

    hDepth->Write();
    hTime->Write();
    hForemostEnergy->Write();
    hSternmostEnergy->Write();
    hDepthVSForemostEnergy->Write();
    hDepthVSSternmostEnergy->Write();
    hEtot->Write();
    hDepthVSEtot->Write();
    hPosVSEnergy->Write();
    hdeltaEE->Write();
    hBeta->Write();
    hE->Write();

    gDirectory = tmp;
}

ClassImp(R3BNeulandDigiMon)
