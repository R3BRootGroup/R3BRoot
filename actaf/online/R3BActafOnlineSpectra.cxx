/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
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
// -----             R3BActafOnlineSpectra                -----
// -----    Created 27/02/25 by J.L. Rodriguez-Sanchez    -----
// -----    Modified 15/09/25 by Pablo González Rusell    -----
// -----          Fill Actaf online histograms            -----
// ------------------------------------------------------------

// ROOT headers
#include <TCanvas.h>
#include <TClonesArray.h>
#include <TFolder.h>
#include <TGaxis.h>
#include <TH1F.h>
#include <TH2F.h>
#include <THttpServer.h>
#include <TMath.h>

// FAIR headers
#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunOnline.h>
#include <FairRuntimeDb.h>

// R3B headers
#include "R3BActafCalData.h"
#include "R3BActafGeometry.h"
#include "R3BActafHitData.h"
#include "R3BActafMappedData.h"
#include "R3BActafMappingPar.h"
#include "R3BActafOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BShared.h"

// R3BActafOnlineSpectra::Default Constructor --------------------------
R3BActafOnlineSpectra::R3BActafOnlineSpectra()
    : R3BActafOnlineSpectra("ActafOnlineSpectra", 1)
{
}

// R3BActafOnlineSpectra::Standard Constructor --------------------------
R3BActafOnlineSpectra::R3BActafOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{

    fh1_RingCounts.resize(2);
    fh2_XYPos.resize(2);
    fh2_XYPosRand.resize(2);
    fh1_PhiCounts.resize(2);
}

void R3BActafOnlineSpectra::SetParContainers()
{
    // Parameter Container
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, rtdb == nullptr, "FairRuntimeDb not found");

    fMap_Par = dynamic_cast<R3BActafMappingPar*>(rtdb->getContainer("actafMappingPar"));
    R3BLOG_IF(fatal, fMap_Par == nullptr, "Container ActafMappingPar not found");
}

void R3BActafOnlineSpectra::SetParameter()
{
    //--- Parameter Container ---
}

InitStatus R3BActafOnlineSpectra::Init()
{
    R3BLOG(info, "");
    auto* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, nullptr == mgr, "FairRootManager not found");

    header = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    R3BLOG_IF(error, header == nullptr, "EventHeader. not found");
    R3BLOG_IF(info, header, "EventHeader. found");

    auto* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    fMappedItems = dynamic_cast<TClonesArray*>(mgr->GetObject("ActafMappedData"));
    R3BLOG_IF(fatal, fMappedItems == nullptr, "ActafMappedData not found");

    fCalItems = dynamic_cast<TClonesArray*>(mgr->GetObject("ActafCalData"));
    R3BLOG_IF(warn, fCalItems == nullptr, "ActafCalData not found");

    fHitItems = dynamic_cast<TClonesArray*>(mgr->GetObject("ActafHitData"));
    R3BLOG_IF(warn, fHitItems == nullptr, "ActafHitData not found");

    fActafGeo = R3BActafGeometry::Instance();
    R3BLOG_IF(warn, !fActafGeo->Init(fGeoversion), "Actaf geometry " << fGeoversion << " not found");

    // MAIN FOLDER-Actaf
    auto* mainfol = new TFolder("Actaf", "Actaf info");
    // Folder for mapped data
    auto* mapfol = new TFolder("Map", "Map Actaf info");
    // Folder for cal data
    auto* calfol = new TFolder("Cal", "Cal Actaf info");
    // Folder for cal data
    auto* hitfol = new TFolder("Hit", "Hit Actaf info");

    //
    // Create histograms
    //

    SetParameter();

    // ********* MAP HISTOGRAMS ********* //

    auto* cSum = new TCanvas("Summary_map", "mapped info", 10, 10, 500, 500);
    cSum->Divide(1, 4);

    cSum->cd(1);
    fh2_ERaw_map = R3B::root_owned<TH2F>("fh2_ERaw_vs_pad_map", "ERaw vs Pad", fPads, 0.5, 0.5 + fPads, 500, 0, 30000);
    fh2_ERaw_map->GetXaxis()->SetTitle("Pad");
    fh2_ERaw_map->GetYaxis()->SetTitle("Energy [chn]");
    fh2_ERaw_map->GetYaxis()->SetTitleOffset(1.1);
    fh2_ERaw_map->GetXaxis()->CenterTitle(true);
    fh2_ERaw_map->GetYaxis()->CenterTitle(true);
    fh2_ERaw_map->Draw("colz");

    cSum->cd(2);
    fh2_Baseline_map =
        R3B::root_owned<TH2F>("fh2_Baseline_vs_pad_map", "Baseline vs Pad", fPads, 0.5, 0.5 + fPads, 300, 7000, 10000);
    fh2_Baseline_map->GetXaxis()->SetTitle("Pad");
    fh2_Baseline_map->GetYaxis()->SetTitle("Baseline [chn]");
    fh2_Baseline_map->GetYaxis()->SetTitleOffset(1.1);
    fh2_Baseline_map->GetXaxis()->CenterTitle(true);
    fh2_Baseline_map->GetYaxis()->CenterTitle(true);
    fh2_Baseline_map->Draw("colz");

    cSum->cd(3);
    fh2_MaxPos_map =
        R3B::root_owned<TH2F>("fh2_Maxpos_vs_pad_map", "Max.-position vs Pad", fPads, 0.5, 0.5 + fPads, 300, 0, 2692);
    fh2_MaxPos_map->GetXaxis()->SetTitle("Pad");
    fh2_MaxPos_map->GetYaxis()->SetTitle("Max.-position [chn]");
    fh2_MaxPos_map->GetYaxis()->SetTitleOffset(1.1);
    fh2_MaxPos_map->GetXaxis()->CenterTitle(true);
    fh2_MaxPos_map->GetYaxis()->CenterTitle(true);
    fh2_MaxPos_map->Draw("colz");

    cSum->cd(4);
    fh2_Risetime_map =
        R3B::root_owned<TH2F>("fh2_Risetime_vs_pad_map", "Risetime vs Pad", fPads, 0.5, 0.5 + fPads, 100, 0, 100);
    fh2_Risetime_map->GetXaxis()->SetTitle("Pad");
    fh2_Risetime_map->GetYaxis()->SetTitle("Risetime [chn]");
    fh2_Risetime_map->GetYaxis()->SetTitleOffset(1.1);
    fh2_Risetime_map->GetXaxis()->CenterTitle(true);
    fh2_Risetime_map->GetYaxis()->CenterTitle(true);
    fh2_Risetime_map->Draw("colz");

    mapfol->Add(cSum);

    for (int adc = 0; adc < fFadcs; adc++)
    {
        std::string nameCanvas = "FADC_" + std::to_string(adc + 1) + "_traces_map";
        auto* cMap = new TCanvas(nameCanvas.c_str(), "mapped info", 10, 10, 500, 500);
        cMap->Divide(4, 4);

        std::string nameCanvasE = "FADC_" + std::to_string(adc + 1) + "_ERaw";
        auto* cMapE = new TCanvas(nameCanvasE.c_str(), "ERaw info", 10, 10, 500, 500);
        cMapE->Divide(4, 4);

        std::string nameCanvasB = "FADC_" + std::to_string(adc + 1) + "_Baseline";
        auto* cMapB = new TCanvas(nameCanvasB.c_str(), "Baseline info", 10, 10, 500, 500);
        cMapB->Divide(4, 4);

        for (int chn = 0; chn < fChn; ++chn)
        {
            auto index = adc * fChn + chn;
            std::string nameHist = "fh2_Pad_" + std::to_string(index) + "_trace";
            std::string titleHist = "Raw trace: Pad " + std::to_string(index + 1);

            fh2_RawTraces.push_back(
                R3B::root_owned<TH2F>(nameHist.c_str(), titleHist.c_str(), 1346, 1, 2692, 500, 6000, 13000));

            fh2_RawTraces[index]->GetXaxis()->SetTitle("Time [Chn]");
            fh2_RawTraces[index]->GetYaxis()->SetTitle("A");
            fh2_RawTraces[index]->GetYaxis()->SetTitleOffset(1.1);
            fh2_RawTraces[index]->GetXaxis()->CenterTitle(true);
            fh2_RawTraces[index]->GetYaxis()->CenterTitle(true);
            cMap->cd(chn + 1);
            fh2_RawTraces[index]->Draw("colz");

            std::string nameHistE = "fh1_Pad_" + std::to_string(index) + "_Eraw";
            std::string titleHistE = "ERaw: Pad " + std::to_string(index + 1);
            fh1_RawE.push_back(R3B::root_owned<TH1F>(nameHistE.c_str(), titleHistE.c_str(), 100, 0, 300000));
            fh1_RawE[index]->GetXaxis()->SetTitle("E [Chn]");
            fh1_RawE[index]->GetYaxis()->SetTitle("Counts");
            fh1_RawE[index]->GetYaxis()->SetTitleOffset(1.1);
            fh1_RawE[index]->GetXaxis()->CenterTitle(true);
            fh1_RawE[index]->GetYaxis()->CenterTitle(true);
            fh1_RawE[index]->SetFillColor(31);
            cMapE->cd(chn + 1);
            fh1_RawE[index]->Draw();

            std::string nameHistB = "fh1_Pad_" + std::to_string(index) + "_Baseline";
            std::string titleHistB = "Baseline: Pad " + std::to_string(index + 1);
            fh1_Baseline.push_back(R3B::root_owned<TH1F>(nameHistB.c_str(), titleHistB.c_str(), 300, 7000, 10000));
            fh1_Baseline[index]->GetXaxis()->SetTitle("Baseline [Chn]");
            fh1_Baseline[index]->GetYaxis()->SetTitle("Counts");
            fh1_Baseline[index]->GetYaxis()->SetTitleOffset(1.1);
            fh1_Baseline[index]->GetXaxis()->CenterTitle(true);
            fh1_Baseline[index]->GetYaxis()->CenterTitle(true);
            fh1_Baseline[index]->SetFillColor(31);
            cMapB->cd(chn + 1);
            fh1_Baseline[index]->Draw();
        }
        if (fDisplaytraces)
        {
            mapfol->Add(cMap);
        }
        mapfol->Add(cMapE);
        mapfol->Add(cMapB);
    }

    // FADC Channel [1 - 16] vs Module [1 - 9] (8 modules + 1 for AMBER signal)
    auto* cModVsCh = new TCanvas("ModVsCh_map", "FADC Module vs Channel", 10, 10, 500, 500);
    fh2_ModVsCh_map = R3B::root_owned<TH2F>("fh2_ModVsCh_map", "Channel vs Mod", 9, 0.5, 9.5, 16, 0.5, 16.5);
    fh2_ModVsCh_map->GetXaxis()->SetTitle("FADC Module");
    fh2_ModVsCh_map->GetYaxis()->SetTitle("FADC Channel");
    fh2_ModVsCh_map->GetYaxis()->SetTitleOffset(1.1);
    fh2_ModVsCh_map->GetXaxis()->CenterTitle(true);
    fh2_ModVsCh_map->GetYaxis()->CenterTitle(true);
    fh2_ModVsCh_map->Draw("colz");
    mapfol->Add(cModVsCh);

    mainfol->Add(mapfol);

    // ********* CAL HISTOGRAMS ********* //

    auto* cCal = new TCanvas("cCal", "cal info", 10, 10, 500, 500);
    cCal->Divide(2, 3);

    cCal->cd(1);
    fh2_Ecal_cal =
        R3B::root_owned<TH2F>("fh2_Ecal_cal", "Ecal vs Pad", fPads, 0.5, 0.5 + fPads, nBinsEcal, nEcalMin, nEcalMax);
    fh2_Ecal_cal->GetXaxis()->SetTitle("Pad");
    fh2_Ecal_cal->GetYaxis()->SetTitle("Energy [MeV]");
    fh2_Ecal_cal->GetYaxis()->SetTitleOffset(1.1);
    fh2_Ecal_cal->GetXaxis()->CenterTitle(true);
    fh2_Ecal_cal->GetYaxis()->CenterTitle(true);
    fh2_Ecal_cal->Draw("colz");

    cCal->cd(2);
    fh2_zPos_cal =
        R3B::root_owned<TH2F>("fh2_zPos_cal", "zPos vs Pad", fPads, 0.5, 0.5 + fPads, nBinsZcal, nZcalMin, nZcalMax);
    fh2_zPos_cal->GetXaxis()->SetTitle("Pad");
    fh2_zPos_cal->GetYaxis()->SetTitle("z_{pos} [cm]");
    fh2_zPos_cal->GetYaxis()->SetTitleOffset(1.1);
    fh2_zPos_cal->GetXaxis()->CenterTitle(true);
    fh2_zPos_cal->GetYaxis()->CenterTitle(true);
    fh2_zPos_cal->Draw("colz");

    cCal->cd(3);
    fh2_tLeading_cal = R3B::root_owned<TH2F>(
        "fh2_tLeading_cal", "tLeading vs Pad", fPads, 0.5, 0.5 + fPads, nBinstLeading, ntLeadingMin, ntLeadingMax);
    fh2_tLeading_cal->GetXaxis()->SetTitle("Pad");
    fh2_tLeading_cal->GetYaxis()->SetTitle("t_{Leading} [ns]");
    fh2_tLeading_cal->GetYaxis()->SetTitleOffset(1.1);
    fh2_tLeading_cal->GetXaxis()->CenterTitle(true);
    fh2_tLeading_cal->GetYaxis()->CenterTitle(true);
    fh2_tLeading_cal->Draw("colz");

    cCal->cd(4);
    fh2_maxAmp_cal = R3B::root_owned<TH2F>(
        "fh2_maxAmp_cal", "maxAmp vs Pad", fPads, 0.5, 0.5 + fPads, nBinsMaxAmp, nMaxAmpMin, nMaxAmpMax);
    fh2_maxAmp_cal->GetXaxis()->SetTitle("Pad");
    fh2_maxAmp_cal->GetYaxis()->SetTitle("Max Ampl [MeV]");
    fh2_maxAmp_cal->GetYaxis()->SetTitleOffset(1.1);
    fh2_maxAmp_cal->GetXaxis()->CenterTitle(true);
    fh2_maxAmp_cal->GetYaxis()->CenterTitle(true);
    fh2_maxAmp_cal->Draw("colz");

    cCal->cd(5);
    fh2_tSync_cal = R3B::root_owned<TH2F>(
        "fh2_tSync_cal", "tSync vs Pad", fPads, 0.5, 0.5 + fPads, nBinsTSync, nTSyncMin, nTSyncMax);
    fh2_tSync_cal->GetXaxis()->SetTitle("Pad");
    fh2_tSync_cal->GetYaxis()->SetTitle("t_{sync} [ns]");
    fh2_tSync_cal->GetYaxis()->SetTitleOffset(1.1);
    fh2_tSync_cal->GetXaxis()->CenterTitle(true);
    fh2_tSync_cal->GetYaxis()->CenterTitle(true);
    fh2_tSync_cal->Draw("colz");

    calfol->Add(cCal);
    mainfol->Add(calfol);

    // ********* HIT HISTOGRAMS ********* //

    // Canvas with counts: 3 hist
    auto* cCounts = new TCanvas("Counts_per_ring", "Hit level info", 10, 10, 500, 500);
    cCounts->Divide(3, 1);

    // Counts vs side (up - down)
    cCounts->cd(1);
    fh1_CountsPerSide = R3B::root_owned<TH1F>("fh1_CountsPerSide", "Counts per side", 2, 0.5, 2.5);
    fh1_CountsPerSide->GetXaxis()->SetTitle("Side");
    fh1_CountsPerSide->GetYaxis()->SetTitle("Counts");
    fh1_CountsPerSide->GetYaxis()->SetTitleOffset(1.1);
    fh1_CountsPerSide->GetXaxis()->CenterTitle(true);
    fh1_CountsPerSide->GetYaxis()->CenterTitle(true);
    fh1_CountsPerSide->SetFillColor(31);
    fh1_CountsPerSide->Draw();

    // Counts vs ring per side
    for (auto i = 0; i < fh1_RingCounts.size(); i++)
    {

        TString tit;
        i == 0 ? tit = "Counts per ring (upstream side)" : tit = "Counts per ring (downstream side)";

        cCounts->cd(i + 2);
        fh1_RingCounts[i] = R3B::root_owned<TH1F>(Form("fh1_RingCounts_side%d", i + 1), tit, 8, 0.5, 8.5);
        fh1_RingCounts[i]->GetXaxis()->SetTitle("Ring");
        fh1_RingCounts[i]->GetYaxis()->SetTitle("Counts");
        fh1_RingCounts[i]->GetYaxis()->SetTitleOffset(1.1);
        fh1_RingCounts[i]->GetXaxis()->CenterTitle(true);
        fh1_RingCounts[i]->GetYaxis()->CenterTitle(true);
        fh1_RingCounts[i]->SetFillColor(31);
        fh1_RingCounts[i]->Draw();
    }

    hitfol->Add(cCounts);

    // Canvas with XY positions (one per ring)
    auto* cXY = new TCanvas("X_Y", "XY positions", 10, 10, 500, 500);
    cXY->Divide(2, 1);

    for (auto i = 0; i < fh2_XYPos.size(); i++)
    {
        TString tit;
        i == 0 ? tit = "XY (upstream)" : tit = "XY (downstream)";

        cXY->cd(i + 1);
        fh2_XYPos[i] = R3B::root_owned<TH2F>(Form("fh2_XY_side%d", i + 1), tit, 100, -40, 40, 100, -40, 40);
        fh2_XYPos[i]->GetXaxis()->SetTitle("X [cm]");
        fh2_XYPos[i]->GetYaxis()->SetTitle("Y [cm]");
        fh2_XYPos[i]->GetYaxis()->SetTitleOffset(1.1);
        fh2_XYPos[i]->GetXaxis()->CenterTitle(true);
        fh2_XYPos[i]->GetYaxis()->CenterTitle(true);
        fh2_XYPos[i]->Draw("colz");
    }

    hitfol->Add(cXY);

    // Canvas with XY positions randomly sampling the pad (one per ring)
    auto* cXYRand = new TCanvas("X_Y_Rand", "XY positions (sampling the whole pad)", 10, 10, 500, 500);
    cXYRand->Divide(2, 1);

    for (auto i = 0; i < fh2_XYPosRand.size(); i++)
    {
        TString tit;
        i == 0 ? tit = "XY (upstream)" : tit = "XY (downstream)";

        cXYRand->cd(i + 1);
        fh2_XYPosRand[i] = R3B::root_owned<TH2F>(Form("fh2_XYRand_side%d", i + 1), tit, 400, -40, 40, 400, -40, 40);
        fh2_XYPosRand[i]->GetXaxis()->SetTitle("X [cm]");
        fh2_XYPosRand[i]->GetYaxis()->SetTitle("Y [cm]");
        fh2_XYPosRand[i]->GetYaxis()->SetTitleOffset(1.1);
        fh2_XYPosRand[i]->GetXaxis()->CenterTitle(true);
        fh2_XYPosRand[i]->GetYaxis()->CenterTitle(true);
        fh2_XYPosRand[i]->Draw("colz");
    }

    hitfol->Add(cXYRand);

    // Canvas with phi angle -> 3 histograms
    auto* cPhi = new TCanvas("Phi_correlations", "Phi angles", 10, 10, 500, 500);
    cPhi->Divide(1, 3);

    // phi counts for side up and down
    for (auto i = 0; i < fh1_PhiCounts.size(); i++)
    {

        TString tit;
        i == 0 ? tit = "Phi (upstream)" : tit = "Phi (downstream)";
        cPhi->cd(i + 1);
        fh1_PhiCounts[i] = R3B::root_owned<TH1F>(Form("fh1_phi_side%d", i + 1), tit, 100, -180, 180);
        fh1_PhiCounts[i]->GetXaxis()->SetTitle("Phi [deg]");
        fh1_PhiCounts[i]->GetYaxis()->SetTitle("counts");
        fh1_PhiCounts[i]->GetYaxis()->SetTitleOffset(1.1);
        fh1_PhiCounts[i]->GetXaxis()->CenterTitle(true);
        fh1_PhiCounts[i]->GetYaxis()->CenterTitle(true);
        fh1_PhiCounts[i]->SetFillColor(31);
        fh1_PhiCounts[i]->Draw();
    }

    cPhi->cd(3);
    fh2_Phi1VsPhi2 = R3B::root_owned<TH2F>("fh2_Phi1VsPhi2", "Phi upstream vs Phi downstream", 100, 0, 360, 50, 0, 360);
    fh2_Phi1VsPhi2->GetXaxis()->SetTitle("Phi_{upstream} [deg]");
    fh2_Phi1VsPhi2->GetYaxis()->SetTitle("Phi_{downstream} [deg]");
    fh2_Phi1VsPhi2->GetYaxis()->SetTitleOffset(1.1);
    fh2_Phi1VsPhi2->GetXaxis()->CenterTitle(true);
    fh2_Phi1VsPhi2->GetYaxis()->CenterTitle(true);
    fh2_Phi1VsPhi2->Draw("colz");
    hitfol->Add(cPhi);

    mainfol->Add(hitfol);

    run->AddObject(mainfol);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_Actaf_HIST", Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BActafOnlineSpectra::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BActafOnlineSpectra::Reset_Histo()
{
    R3BLOG(info, "");

    if (fMappedItems)
    {
        fh2_ERaw_map->Reset();
        fh2_Baseline_map->Reset();
        fh2_MaxPos_map->Reset();
        fh2_Risetime_map->Reset();
        fh2_ModVsCh_map->Reset();
        for (const auto& hist : fh2_RawTraces)
        {
            hist->Reset();
        }
        for (const auto& hist : fh1_RawE)
        {
            hist->Reset();
        }
        for (const auto& hist : fh1_Baseline)
        {
            hist->Reset();
        }
    }

    if (fCalItems)
    {
        fh2_Ecal_cal->Reset();
        fh2_zPos_cal->Reset();
        fh2_tLeading_cal->Reset();
        fh2_maxAmp_cal->Reset();
        fh2_tSync_cal->Reset();
    }

    if (fHitItems)
    {
        for (auto& h : fh1_RingCounts)
            h->Reset();

        for (auto& h : fh2_XYPos)
            h->Reset();

        for (auto& h : fh2_XYPosRand)
            h->Reset();

        for (auto& h : fh1_PhiCounts)
            h->Reset();

        fh1_CountsPerSide->Reset();

        fh2_Phi1VsPhi2->Reset();
    }

    return;
}

void R3BActafOnlineSpectra::Exec(Option_t* /*option*/)
{
    // Check for requested trigger
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    if (fTpat1 > 0 && fTpat2 > 0 && (header))
    {
        // fTpat = 1-16; fTpat_bit = 0-15
        Int_t fTpat_bit1 = fTpat1 - 1;
        Int_t fTpat_bit2 = fTpat2 - 1;
        Int_t tpatbin = 0;
        for (int i = 0; i < 16; i++)
        {
            tpatbin = (header->GetTpat() & (1 << i));
            if (tpatbin != 0 && (i < fTpat_bit1 || i > fTpat_bit2))
            {
                return;
            }
        }
    }

    // Fill mapped data
    if (fMappedItems && fMappedItems->GetEntriesFast() > 0)
    {
        auto nHits = fMappedItems->GetEntriesFast();
        for (int ihit = 0; ihit < nHits; ihit++)
        {
            auto* hit = dynamic_cast<R3BActafMappedData*>(fMappedItems->At(ihit));
            if (!hit)
                continue;
            auto pad = hit->GetPad() - 1;

            // Allow 128 pad for Amber
            if (pad > fMap_Par->GetNbPads())
                continue;

            const int moduleNb = pad >= fMap_Par->GetNbPads() ? 9 : fMap_Par->GetFADCChannelByPad(pad + 1);
            const int channelNb = pad >= fMap_Par->GetNbPads() ? 1 : fMap_Par->GetFADCModuleByPad(pad + 1);

            fh2_ModVsCh_map->Fill(moduleNb, channelNb);

            if (pad >= fMap_Par->GetNbPads())
                continue;

            if (hit->GetE() > 0)
            {
                fh1_RawE[pad]->Fill(hit->GetE());
                fh2_ERaw_map->Fill(pad + 1, hit->GetE());
                fh2_Risetime_map->Fill(pad + 1, hit->GetRisetime());
                fh2_MaxPos_map->Fill(pad + 1, hit->GetMaxpos());
            }
            if (hit->GetBaseline() > 0)
            {
                fh1_Baseline[pad]->Fill(hit->GetBaseline());
                fh2_Baseline_map->Fill(pad + 1, hit->GetBaseline());
            }

            if (fDisplaytraces)
            {
                auto vec = hit->GetTrace(); // std::vector
                std::size_t index = 0;
                for (const auto& value : vec)
                    fh2_RawTraces[pad]->Fill(index++, value);
            }
        }
    }

    // Fill cal data
    if (fCalItems && fCalItems->GetEntriesFast() > 0)
    {
        auto nHits = fCalItems->GetEntriesFast();
        for (int ihit = 0; ihit < nHits; ihit++)
        {
            auto* hit = dynamic_cast<R3BActafCalData*>(fCalItems->At(ihit));
            if (!hit)
                continue;

            int pad = hit->GetPad();

            if (pad > fPads)
                continue;

            double Ecal = hit->GetEnergy();
            double maxAmp = hit->GetEMaxAmpl();
            double tLeading = hit->GetLeadingEdgeTime();
            double zPos = hit->GetZpos();
            double tSync = hit->GetSynTime();

            fh2_Ecal_cal->Fill(pad, Ecal);
            fh2_zPos_cal->Fill(pad, zPos);
            fh2_tLeading_cal->Fill(pad, tLeading);
            fh2_maxAmp_cal->Fill(pad, maxAmp);
            fh2_tSync_cal->Fill(pad, tSync);
        }
    }

    // Fill hit data
    // In principle each events has 128 hits
    if (fHitItems && fHitItems->GetEntriesFast() > 0)
    {
        auto nHits = fHitItems->GetEntriesFast();
        for (int ihit = 0; ihit < nHits; ihit++)
        {
            auto* hit = dynamic_cast<R3BActafHitData*>(fHitItems->At(ihit));
            if (!hit)
                continue;

            int pad = hit->GetPad();

            if (pad > fPads)
                continue;

            int side = hit->GetSide() - 1;
            int ring = hit->GetRing();
            double x = hit->GetXpos();
            double y = hit->GetYpos();
            // double z = hit->GetZpos();

            TVector3 track = hit->GetTrack();
            double phi = track.Phi() * TMath::RadToDeg();

            fh1_RingCounts[side]->Fill(ring);
            fh2_XYPos[side]->Fill(x, y);

            TVector3 trackRand = fActafGeo->GetPosition(pad, true);
            double xrand = trackRand.X();
            double yrand = trackRand.Y();

            fh2_XYPosRand[side]->Fill(xrand, yrand);

            fh1_PhiCounts[side]->Fill(phi);
            fh1_CountsPerSide->Fill(side + 1);
        }
    }

    fNEvents++;
    return;
}

void R3BActafOnlineSpectra::FinishEvent()
{
    R3BLOG(debug, "Cleaning data structures");
    r3b::util::ClearIfNotNull(fMappedItems);
    r3b::util::ClearIfNotNull(fCalItems);
    r3b::util::ClearIfNotNull(fHitItems);
}

void R3BActafOnlineSpectra::FinishTask()
{
    if (fMappedItems)
    {
        fh2_ERaw_map->Write();
        fh2_Baseline_map->Write();
        fh2_MaxPos_map->Write();
        fh2_Risetime_map->Write();
        fh2_ModVsCh_map->Write();
        for (const auto& hist : fh2_RawTraces)
        {
            hist->Write();
        }
        for (const auto& hist : fh1_RawE)
        {
            hist->Write();
        }
        for (const auto& hist : fh1_Baseline)
        {
            hist->Write();
        }
    }

    if (fCalItems)
    {
        fh2_Ecal_cal->Write();
        fh2_zPos_cal->Write();
        fh2_tLeading_cal->Write();
        fh2_maxAmp_cal->Write();
        fh2_tSync_cal->Write();
    }

    if (fHitItems)
    {
        for (auto& h : fh1_RingCounts)
            h->Write();

        for (auto& h : fh2_XYPos)
            h->Write();

        for (auto& h : fh2_XYPosRand)
            h->Write();

        for (auto& h : fh1_PhiCounts)
            h->Write();

        fh1_CountsPerSide->Write();

        fh2_Phi1VsPhi2->Write();
    }
}
ClassImp(R3BActafOnlineSpectra)
