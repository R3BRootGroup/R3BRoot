/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2024 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BCalifavsSpiderOnlineSpectra.h"
#include "R3BAlpideHitData.h"
#include "R3BCalifaClusterData.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BShared.h"
#include "R3BWRData.h"

#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunOnline.h>
#include <FairRuntimeDb.h>

#include <TCanvas.h>
#include <TFolder.h>
#include <TH1F.h>
#include <TH2F.h>
#include <THttpServer.h>
#include <TVector3.h>

#include <TClonesArray.h>
#include <TMath.h>
#include <TRandom.h>
#include <sstream>

R3BCalifavsSpiderOnlineSpectra::R3BCalifavsSpiderOnlineSpectra()
    : R3BCalifavsSpiderOnlineSpectra("R3BCalifavsSpiderOnlineSpectra", 1)
{
}

R3BCalifavsSpiderOnlineSpectra::R3BCalifavsSpiderOnlineSpectra(const TString& name, int iVerbose)
    : FairTask(name, iVerbose)
{
}

void R3BCalifavsSpiderOnlineSpectra::SetParContainers()
{
    // Parameter Container
    // Reading amsStripCalPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");
}

InitStatus R3BCalifavsSpiderOnlineSpectra::Init()
{
    R3BLOG(info, "");
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, mgr == nullptr, "FairRootManager not found");

    fHeader = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    R3BLOG_IF(fatal, fHeader == nullptr, "EventHeader. not found");

    // get access to Hit data
    fHitItemsCalifa = dynamic_cast<TClonesArray*>(mgr->GetObject("CalifaClusterData"));
    R3BLOG_IF(fatal, !fHitItemsCalifa, "CalifaClusterData not found");

    fHitItemsSpider = dynamic_cast<TClonesArray*>(mgr->GetObject("AlpideHitData"));
    R3BLOG_IF(fatal, !fHitItemsSpider, "AlpideHitData not found");

    // Create histograms for detectors

    // CANVAS Theta vs Phi
    cCalifa_angles = new TCanvas("Califa_Theta_vs_Phi_and_Spider", "Theta vs Phi", 10, 10, 500, 500);
    cCalifa_angles->Divide(2, 2);
    fh2_Califa_theta_phi.resize(2);

    for (int i = 0; i < 2; i++)
    {
        cCalifa_angles->cd(i + 1);
        std::stringstream ss1;
        ss1 << "fh2_Califa_Spider_theta_vs_phi" << ((i == 0) ? "" : "_with_Spider");
        fh2_Califa_theta_phi[i] =
            R3B::root_owned<TH2F>(ss1.str().c_str(), ss1.str().c_str(), 50, 0, 90, 180, -180, 180);
        fh2_Califa_theta_phi[i]->GetXaxis()->SetTitle("Theta [deg]");
        fh2_Califa_theta_phi[i]->GetYaxis()->SetTitle("Phi [deg]");
        fh2_Califa_theta_phi[i]->GetYaxis()->SetTitleOffset(1.2);
        fh2_Califa_theta_phi[i]->GetXaxis()->CenterTitle(true);
        fh2_Califa_theta_phi[i]->GetYaxis()->CenterTitle(true);
        fh2_Califa_theta_phi[i]->Draw("COLZ");
    }

    cCalifa_angles->cd(3);
    fh2_Califa_coinTheta =
        R3B::root_owned<TH2F>("fh2_Califa_Spider_theta_cors", "Califa vs Spider Theta cors", 500, 0, 100, 500, 0, 100);
    fh2_Califa_coinTheta->GetXaxis()->SetTitle("Theta_califa [deg]");
    fh2_Califa_coinTheta->GetYaxis()->SetTitle("Theta_spider [deg]");
    fh2_Califa_coinTheta->GetYaxis()->SetTitleOffset(1.2);
    fh2_Califa_coinTheta->GetXaxis()->CenterTitle(true);
    fh2_Califa_coinTheta->GetYaxis()->CenterTitle(true);
    fh2_Califa_coinTheta->Draw("COLZ");

    cCalifa_angles->cd(4);
    fh2_Califa_coinPhi = R3B::root_owned<TH2F>(
        "fh2_Califa_Spider_phi_cors", "Califa vs Spider Phi cors", 600, -190, 190, 600, -190, 190);
    fh2_Califa_coinPhi->GetXaxis()->SetTitle("Phi_califa [deg]");
    fh2_Califa_coinPhi->GetYaxis()->SetTitle("Phi_spider [deg]");
    fh2_Califa_coinPhi->GetYaxis()->SetTitleOffset(1.2);
    fh2_Califa_coinPhi->GetXaxis()->CenterTitle(true);
    fh2_Califa_coinPhi->GetYaxis()->CenterTitle(true);
    fh2_Califa_coinPhi->Draw("COLZ");

    // MAIN FOLDER-Califa-Spider
    auto mainfolCalifa = new TFolder("Califa_vs_Spider", "CALIFA vs Spider info");

    if (fHitItemsCalifa && fHitItemsSpider)
    {
        mainfolCalifa->Add(cCalifa_angles);
    }

    auto run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    run->AddObject(mainfolCalifa);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_Califa_vs_Spider", Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BCalifavsSpiderOnlineSpectra::ReInit()
{
    SetParContainers();
    // SetParameter();
    return kSUCCESS;
}

void R3BCalifavsSpiderOnlineSpectra::Reset_Histo()
{
    R3BLOG(info, "");
    for (const auto& hist : fh2_Califa_theta_phi)
    {
        hist->Reset();
    }
    fh2_Califa_coinPhi->Reset();
    fh2_Califa_coinTheta->Reset();
}

void R3BCalifavsSpiderOnlineSpectra::Exec(Option_t* /*option*/)
{
    if ((fTpat > 0) && (fHeader) && ((fHeader->GetTpat() & fTpat) != fTpat))
        return;

    int nHitsCalifa = fHitItemsCalifa->GetEntriesFast();
    int nHitsSpider = fHitItemsSpider->GetEntriesFast();

    if (nHitsCalifa == 0 || nHitsSpider == 0)
        return;

    std::vector<TVector3> Vcalifa;
    for (int ihit = 0; ihit < nHitsCalifa; ihit++)
    {
        auto hit = dynamic_cast<R3BCalifaClusterData*>(fHitItemsCalifa->At(ihit));
        if (hit->GetEnergy() < fMinProtonE)
            continue;
        TVector3 temp;
        temp.SetMagThetaPhi(hit->GetEnergy(), hit->GetTheta(), hit->GetPhi());
        Vcalifa.push_back(temp);
        fh2_Califa_theta_phi[0]->Fill(hit->GetTheta() * TMath::RadToDeg(), hit->GetPhi() * TMath::RadToDeg());
    }

    for (int ihit = 0; ihit < nHitsSpider; ihit++)
    {
        auto hit = dynamic_cast<R3BAlpideHitData*>(fHitItemsSpider->At(ihit));
        auto track = hit->GetTrack();
        for (const auto& vcalifa : Vcalifa)
            if (abs(vcalifa.Phi() - track.Phi()) * TMath::RadToDeg() < fMinPhi_Dif)
            {
                if ((vcalifa.Theta() - track.Theta()) * TMath::RadToDeg() < 20.)
                    fh2_Califa_coinTheta->Fill(vcalifa.Theta() * TMath::RadToDeg(), track.Theta() * TMath::RadToDeg());

                fh2_Califa_coinPhi->Fill(vcalifa.Phi() * TMath::RadToDeg(), track.Phi() * TMath::RadToDeg());
                fh2_Califa_theta_phi[1]->Fill(vcalifa.Theta() * TMath::RadToDeg(), vcalifa.Phi() * TMath::RadToDeg());
            }
    }

    fNEvents += 1;
}

void R3BCalifavsSpiderOnlineSpectra::FinishEvent()
{
    if (fHitItemsCalifa)
    {
        fHitItemsCalifa->Clear();
    }
    if (fHitItemsSpider)
    {
        fHitItemsSpider->Clear();
    }
}

void R3BCalifavsSpiderOnlineSpectra::FinishTask()
{
    // Write canvas for Hit data
    if (fHitItemsCalifa)
    {
        fh2_Califa_theta_phi[0]->Write();
        if (fHitItemsSpider)
            fh2_Califa_theta_phi[1]->Write();

        fh2_Califa_coinPhi->Write();
        fh2_Califa_coinTheta->Write();
    }
}

ClassImp(R3BCalifavsSpiderOnlineSpectra)
