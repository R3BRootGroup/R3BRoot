/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ------------------------------------------------------------
// -----             R3BFootOnlineSpectra                 -----
// -----    Created 16/07/21 by J.L. Rodriguez-Sanchez    -----
// -----          Fill FOOT online histograms             -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with FOOT online data
 */

#include "R3BFootOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BFootCalData.h"
#include "R3BFootHitData.h"
#include "R3BFootMappedData.h"
#include "R3BLogger.h"
#include "R3BShared.h"

#include <THttpServer.h>

#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRunOnline.h>
#include <FairRuntimeDb.h>
#include <TCanvas.h>
#include <TFolder.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLine.h>

#include <TClonesArray.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

R3BFootOnlineSpectra::R3BFootOnlineSpectra()
    : R3BFootOnlineSpectra("FootOnlineSpectra", 1)
{
}

R3BFootOnlineSpectra::R3BFootOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
}

InitStatus R3BFootOnlineSpectra::Init()
{
    R3BLOG(info, "");
    // Looking for FairRootManager
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, mgr == nullptr, "FairRootManager not found");

    // Look for the R3BEventHeader
    fEventHeader = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    if (fEventHeader == nullptr)
    {
        R3BLOG(error, "EventHeader. not found");
    }
    else
    {
        R3BLOG(info, "EventHeader. found");
    }

    // Get access to Mapped data
    fMappedItems = dynamic_cast<TClonesArray*>(mgr->GetObject("FootMappedData"));
    R3BLOG_IF(fatal, fMappedItems == nullptr, "FootMappedData not found");

    // Get access to Cal data
    fCalItems = dynamic_cast<TClonesArray*>(mgr->GetObject("FootCalData"));
    R3BLOG_IF(warn, fCalItems == nullptr, "FootCalData not found");

    // Get access to Hit data
    fHitItems = dynamic_cast<TClonesArray*>(mgr->GetObject("FootHitData"));
    R3BLOG_IF(warn, fHitItems == nullptr, "FootHitData not found");

    // Create histograms for all the detectors
    // Energy range for strips
    Double_t binsE = 1000;
    Double_t minE = -100;
    Double_t maxE = 3000;

    char Name1[255];
    char Name2[255];

    // MAIN FOLDER-FOOT
    TFolder* mainfol = new TFolder("Foot", "Foot-Si info");
    TFolder* mapfol = new TFolder("Map", "Map FOOT info");
    TFolder* calfol = new TFolder("Cal", "Cal FOOT info");
    TFolder* hitfol = new TFolder("Hit", "Hit FOOT info");
    mainfol->Add(mapfol);

    auto cMap = new TCanvas("FOOT_mapped", "mapped info", 10, 10, 500, 500);
    cMap->Divide(4, 2);
    mapfol->Add(cMap);

    //================  Mapped data =====================
    fh2_EnergyVsStrip.resize(fNbDet);
    int i_pad = 1; // pad iterator
    int i_pad_double = 1;
    int i_pad_corr = 1;
    for (Int_t i = 0; i < fNbDet; i++)
    { // one histo per detector
        sprintf(Name1, "fh2_energy_vs_strip_det_%d", i + 1);
        sprintf(Name2, "Mapped energy vs strip number for FOOT Det: %d", i + 1);
        fh2_EnergyVsStrip[i] = R3B::root_owned<TH2F>(Name1, Name2, 640, 1, 641, binsE, -100, 3000);
        fh2_EnergyVsStrip[i]->GetXaxis()->SetTitle("Strip number");
        fh2_EnergyVsStrip[i]->GetYaxis()->SetTitle("Energy [channels]");
        fh2_EnergyVsStrip[i]->GetYaxis()->SetTitleOffset(1.4);
        fh2_EnergyVsStrip[i]->GetXaxis()->CenterTitle(true);
        int foot_num = i + 1;
        if (foot_num < 9)
        {
            cMap->cd(i_pad);
            fh2_EnergyVsStrip[i]->Draw("col");
            for (int i_asic = 1; i_asic < 10; i_asic++)
            {
                TLine* l = new TLine(64.5 * i_asic, minE, 64.5 * i_asic, maxE);
                l->Draw("same");
                l->SetLineStyle(7);
                l->SetLineWidth(1);
                l->SetLineColor(13);
            }
            i_pad++;
        }
    }

    //============ CAL data ==================

    auto cCal = new TCanvas("FOOT_cal", "cal info", 10, 10, 500, 500);
    cCal->Divide(4, 2);
    calfol->Add(cCal);

    if (fCalItems)
    {
        i_pad = 1; // pad iterator
        fh2_EnergyVsStrip_cal.resize(fNbDet);
        for (Int_t i = 0; i < fNbDet; i++)
        {
            sprintf(Name1, "fh2_energy_vs_strip_cal_det_%d", i + 1);
            sprintf(Name2, "Cal-energy vs strip number for FOOT Det: %d", i + 1);
            fh2_EnergyVsStrip_cal[i] = R3B::root_owned<TH2F>(Name1, Name2, 640, 1, 641, binsE, -100, 1000);
            fh2_EnergyVsStrip_cal[i]->GetXaxis()->SetTitle("Strip number");
            fh2_EnergyVsStrip_cal[i]->GetYaxis()->SetTitle("Energy [channels]");
            fh2_EnergyVsStrip_cal[i]->GetYaxis()->SetTitleOffset(1.4);
            fh2_EnergyVsStrip_cal[i]->GetXaxis()->CenterTitle(true);
            fh2_EnergyVsStrip_cal[i]->GetYaxis()->CenterTitle(true);
            int foot_num = i + 1;
            if (foot_num < 9)
            {
                cCal->cd(i_pad);
                fh2_EnergyVsStrip_cal[i]->Draw("col");
                for (int i_asic = 1; i_asic < 10; i_asic++)
                {
                    TLine* l = new TLine(64.5 * i_asic, minE, 64.5 * i_asic, maxE);
                    l->Draw("same");
                    l->SetLineStyle(7);
                    l->SetLineWidth(1);
                    l->SetLineColor(13);
                }
                i_pad++;
            }
        }
    }

    //------------- SIGMA -------------------

    auto cSigma = new TCanvas("FOOT_sigma", "Sigma info", 10, 10, 500, 500);
    cSigma->Divide(4, 2);
    calfol->Add(cSigma);

    fh2_SigmaVsStrip.resize(fNbDet);
    i_pad = 1; // pad iterator

    for (Int_t i = 0; i < fNbDet; i++)
    {
        sprintf(Name1, "fh2_sigma_vs_strip_det_%d", i + 1);
        sprintf(Name2, "Sigma vs strip number for FOOT Det: %d", i + 1);
        fh2_SigmaVsStrip[i] = R3B::root_owned<TH2F>(Name1, Name2, 640, 1, 641, 100, 0, 15);
        fh2_SigmaVsStrip[i]->GetXaxis()->SetTitle("Strip number");
        fh2_SigmaVsStrip[i]->GetYaxis()->SetTitle("Sigma");
        fh2_SigmaVsStrip[i]->GetYaxis()->SetTitleOffset(1.4);
        fh2_SigmaVsStrip[i]->GetXaxis()->CenterTitle(true);
        fh2_SigmaVsStrip[i]->GetYaxis()->CenterTitle(true);

        int foot_num = i + 1;
        if (foot_num < 9)
        {
            cSigma->cd(i_pad);
            fh2_SigmaVsStrip[i]->Draw("col");
            i_pad++;
        }
    }

    //================ HIT data ==========================

    // General canvas info (cluster position and energy)
    auto cHit = new TCanvas("FOOT_hit", "hit general info", 10, 10, 500, 500);
    cHit->Divide(4, 4);

    // Correlation position between detectors
    auto cHit_PosCorr = new TCanvas("FOOT_posCorr", "hit position correlation info", 10, 10, 500, 500);
    cHit_PosCorr->Divide(2, 2);

    // Eta
    auto cHit_Eta = new TCanvas("FOOT_eta", "hit eta plot info", 10, 10, 500, 500);
    cHit_Eta->Divide(4, 2);

    // Multiplicity and cluster size
    auto cHit_MultSize = new TCanvas("FOOT_mulSize", "hit multiplicity and size info", 10, 10, 500, 500);
    cHit_MultSize->Divide(4, 4);

    hitfol->Add(cHit);
    hitfol->Add(cHit_PosCorr);
    hitfol->Add(cHit_Eta);
    hitfol->Add(cHit_MultSize);

    if (fHitItems)
    {
        i_pad = 1; // pad iterator
        i_pad_double = 1;
        i_pad_corr = 1;
        fh1_pos.resize(fNbDet);
        fh1_ene.resize(fNbDet);
        fh1_mult.resize(fNbDet);
        fh1_size.resize(fNbDet);
        fh2_eta.resize(fNbDet);
        fh2_foot_corr.resize(fNbDet / 2);

        for (Int_t i = 0; i < fNbDet; i++)
        { // one histo per detector
            sprintf(Name1, "fh1_pos_det_%d", i + 1);
            sprintf(Name2, "Cluster position for FOOT Det: %d", i + 1);
            fh1_pos[i] = R3B::root_owned<TH1F>(Name1, Name2, 600, -50., 50.);
            fh1_pos[i]->GetXaxis()->SetTitle("Position [mm]");
            fh1_pos[i]->GetYaxis()->SetTitle("Counts");
            fh1_pos[i]->GetYaxis()->SetTitleOffset(1.4);
            fh1_pos[i]->GetXaxis()->CenterTitle(true);
            fh1_pos[i]->GetYaxis()->CenterTitle(true);

            sprintf(Name1, "fh1_ene_det_%d", i + 1);
            sprintf(Name2, "Cluster energy for FOOT Det: %d", i + 1);
            fh1_ene[i] = R3B::root_owned<TH1F>(Name1, Name2, binsE, minE, maxE);
            fh1_ene[i]->GetXaxis()->SetTitle("Energy");
            fh1_ene[i]->GetYaxis()->SetTitle("Counts");
            fh1_ene[i]->GetYaxis()->SetTitleOffset(1.4);
            fh1_ene[i]->GetXaxis()->CenterTitle(true);
            fh1_ene[i]->GetYaxis()->CenterTitle(true);

            sprintf(Name1, "fh2_eta_vs_strip_det_%d", i + 1);
            sprintf(Name2, "Eta parameter for FOOT Det: %d", i + 1);
            fh2_eta[i] = R3B::root_owned<TH2F>(Name1, Name2, 100, 0, 1, binsE, 0, maxE);
            fh2_eta[i]->GetXaxis()->SetTitle("Strip");
            fh2_eta[i]->GetYaxis()->SetTitle("Eta");
            fh2_eta[i]->GetYaxis()->SetTitleOffset(1.4);
            fh2_eta[i]->GetXaxis()->CenterTitle(true);
            fh2_eta[i]->GetYaxis()->CenterTitle(true);

            sprintf(Name1, "fh1_mult_det_%d", i + 1);
            sprintf(Name2, "Cluster multiplicity for FOOT Det: %d", i + 1);
            fh1_mult[i] = R3B::root_owned<TH1F>(Name1, Name2, 20, 0, 20);
            fh1_mult[i]->GetXaxis()->SetTitle("Multiplicity");
            fh1_mult[i]->GetYaxis()->SetTitle("Counts");
            fh1_mult[i]->GetYaxis()->SetTitleOffset(1.4);
            fh1_mult[i]->GetXaxis()->CenterTitle(true);
            fh1_mult[i]->GetYaxis()->CenterTitle(true);

            sprintf(Name1, "fh1_size_det_%d", i + 1);
            sprintf(Name2, "Cluster size for FOOT Det: %d", i + 1);
            fh1_size[i] = R3B::root_owned<TH1F>(Name1, Name2, 50, 0, 50);
            fh1_size[i]->GetXaxis()->SetTitle("Size");
            fh1_size[i]->GetYaxis()->SetTitle("Counts");
            fh1_size[i]->GetYaxis()->SetTitleOffset(1.4);
            fh1_size[i]->GetXaxis()->CenterTitle(true);
            fh1_size[i]->GetYaxis()->CenterTitle(true);

            if ((i % 2) == 0)
            {
                sprintf(Name1, "fh2_pos_corr_dets_%d_%d", i + 1, i + 2);
                sprintf(Name2, "Cluster position correlation for FOOTs: %d and %d", i + 1, i + 2);
                fh2_foot_corr[i / 2] = R3B::root_owned<TH2F>(Name1, Name2, 100, -50., 50., 100, -50, 50);
                fh2_foot_corr[i / 2]->GetXaxis()->SetTitle("x - Position [mm]");
                fh2_foot_corr[i / 2]->GetYaxis()->SetTitle("y - Position [mm]");
                fh2_foot_corr[i / 2]->GetYaxis()->SetTitleOffset(1.4);
                fh2_foot_corr[i / 2]->GetXaxis()->CenterTitle(true);
                fh2_foot_corr[i / 2]->GetYaxis()->CenterTitle(true);
            }

            int foot_num = i + 1;
            if (foot_num < 9)
            {

                cHit->cd(i_pad_double);
                fh1_ene[i]->Draw();
                cHit->cd(i_pad_double + 1);
                fh1_pos[i]->Draw();

                if ((foot_num % 2) == 1)
                {
                    if (foot_num < 12)
                    {
                        cHit_PosCorr->cd(i_pad_corr);
                        fh2_foot_corr[i / 2]->Draw();
                        i_pad_corr++;
                    }
                }

                cHit_Eta->cd(i_pad);
                fh2_eta[i]->Draw();

                cHit_MultSize->cd(i_pad_double);
                fh1_size[i]->Draw();
                cHit_MultSize->cd(i_pad_double + 1);
                fh1_mult[i]->Draw();

                i_pad_double++;
                i_pad_double++;

                i_pad++;
            }
        }
    }

    if (fCalItems)
    {
        mainfol->Add(calfol);
    }
    if (fHitItems)
    {
        mainfol->Add(hitfol);
    }

    // Looking for FairRunOnline
    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);
    run->AddObject(mainfol);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_FOOT", Form("/Objects/%s/->Reset_FOOT_Histo()", GetName()));

    return kSUCCESS;
}

void R3BFootOnlineSpectra::Reset_FOOT_Histo()
{
    LOG(info) << "R3BFootOnlineSpectra::Reset_FOOT_Histo";

    // Mapped data
    for (Int_t i = 0; i < fNbDet; i++)
    {
        fh2_EnergyVsStrip[i]->Reset();
    }

    // Cal data
    if (fCalItems)
    {
        for (Int_t i = 0; i < fNbDet; i++)
        {
            fh2_EnergyVsStrip_cal[i]->Reset();
            fh2_SigmaVsStrip[i]->Reset();
        }
    }

    // Hit data
    if (fHitItems)
    {
        for (Int_t i = 0; i < fNbDet; i++)
        {
            fh1_pos[i]->Reset();
            fh1_ene[i]->Reset();
            fh1_mult[i]->Reset();
            fh1_size[i]->Reset();
            fh2_eta[i]->Reset();

            if ((i % 2) == 0)
            {
                fh2_foot_corr[i / 2]->Reset();
            }
        }
    }
    return;
}

void R3BFootOnlineSpectra::Exec(Option_t* option)
{

    if (fEventHeader->GetTrigger() != fTrigger && fTrigger > -1)
        return;

    if (fTpat > 0 && (fEventHeader->GetTpat() & fTpat) != fTpat)
        return;

    // Fill mapped data
    if (fMappedItems && fMappedItems->GetEntriesFast() > 0)
    {
        auto nHits = fMappedItems->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BFootMappedData* hit = dynamic_cast<R3BFootMappedData*>(fMappedItems->At(ihit));
            if (!hit)
                continue;
            fh2_EnergyVsStrip[hit->GetDetId() - 1]->Fill(hit->GetStripId(), hit->GetEnergy());
        }
    }

    // Fill cal data
    if (fCalItems && fCalItems->GetEntriesFast() > 0)
    {
        auto nHits = fCalItems->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BFootCalData* hit = dynamic_cast<R3BFootCalData*>(fCalItems->At(ihit));
            if (!hit)
                continue;
            fh2_EnergyVsStrip_cal[hit->GetDetId() - 1]->Fill(hit->GetStripId(), hit->GetEnergy());

            if ((eventNumber % fSigmaRefreshRate) == 0)
            {
                fh2_SigmaVsStrip[hit->GetDetId() - 1]->Fill(hit->GetStripId(), hit->GetSigma());
            }
        }
    }

    // Vector to store the multiplicity of each event
    std::vector<int> mult(fNbDet, 0);

    // Fill hit data
    if (fHitItems && fHitItems->GetEntriesFast() > 0)
    {
        auto nHits = fHitItems->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BFootHitData* hit = dynamic_cast<R3BFootHitData*>(fHitItems->At(ihit));
            if (!hit)
                continue;
            fh1_pos[hit->GetDetId() - 1]->Fill(hit->GetPos());
            fh1_ene[hit->GetDetId() - 1]->Fill(hit->GetEnergy());
            fh1_size[hit->GetDetId() - 1]->Fill(hit->GetMulStrip());
            mult[hit->GetDetId() - 1]++;
            fh2_eta[hit->GetDetId() - 1]->Fill(hit->GetEta(), hit->GetEnergy());

            int pairNdx = (hit->GetDetId() - 1) / 2;
            int pairPos = (hit->GetDetId() - 1) % 2;

            double pos1 = hit->GetPos();
            double det1 = hit->GetDetId();

            if (det1 > 9)
            {
                continue;
            }

            // Correlation position between each pair of FOOTs
            if (ihit < nHits - 1)
            {
                for (Int_t ihit2 = ihit + 1; ihit2 < nHits; ihit2++)
                {
                    R3BFootHitData* hit2 = dynamic_cast<R3BFootHitData*>(fHitItems->At(ihit2));

                    // The hit is in another pair
                    if ((hit2->GetDetId() - 1) / 2 != pairNdx)
                    {
                        continue;
                    }

                    double pos2 = hit2->GetPos();
                    double det2 = hit2->GetDetId();

                    if (det1 == det2)
                    {
                        continue;
                    }

                    if (pairPos == 0) // Is the first FOOT on the pair
                    {
                        fh2_foot_corr[pairNdx]->Fill(pos1, pos2);
                    }
                    else
                    {
                        fh2_foot_corr[pairNdx]->Fill(pos2, pos1);
                    }
                }
            }
        }
    }

    // Fill the mutiplicity of the cluster per event
    for (int i = 0; i < fNbDet; i++)
    {
        if (mult[i] == 0)
        {
            continue;
        }
        fh1_mult[i]->Fill(mult[i]);
    }

    fNEvents += 1;
    return;
}

void R3BFootOnlineSpectra::FinishEvent()
{
    if (fMappedItems)
    {
        fMappedItems->Clear();
    }
    if (fCalItems)
    {
        fCalItems->Clear();
    }
    if (fHitItems)
    {
        fHitItems->Clear();
    }

    // Increase the event number variable
    eventNumber++;
    return;
}

void R3BFootOnlineSpectra::FinishTask()
{
    if (fMappedItems)
    {
        for (Int_t i = 0; i < fNbDet; i++)
            fh2_EnergyVsStrip[i]->Write();
    }
    if (fCalItems)
    {
        for (Int_t i = 0; i < fNbDet; i++)
        {
            fh2_EnergyVsStrip_cal[i]->Write();
            fh2_SigmaVsStrip[i]->Write();
        }
    }

    if (fHitItems)
    {
        for (Int_t i = 0; i < fNbDet; i++)
        {
            fh1_pos[i]->Write();
            fh1_ene[i]->Write();
            fh1_mult[i]->Write();
            fh1_size[i]->Write();
            fh2_eta[i]->Write();

            if ((i % 2) == 0)
            {
                fh2_foot_corr[i / 2]->Write();
            }
        }
    }
    return;
}

ClassImp(R3BFootOnlineSpectra)
