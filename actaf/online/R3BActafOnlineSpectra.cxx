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
#include <TH2Poly.h>
#include <THttpServer.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TMath.h>
#include <TROOT.h>
#include <TStyle.h>

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
#include "R3BWRData.h"

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
    fh2_XYPos_Evts.resize(2);
    fh2_XYPos_Evts_Automatic.resize(2);
    fh1_PhiCounts.resize(2);
    fh2_RawTraces.resize(fPads);
    g_CorrectedTraces_4pads_highestAmp_auto.resize(4 * 2); // 4 pads per side
    g_CorrectedTraces_4pads_highestAmp.resize(4 * 2);      // ensure main vector has room
    fh2_FilteredTraces.resize(fPads);
    fh2_mawVsECal.resize(2);
    fh2_mawVsEMap.resize(2);
    fh1_RawE.resize(fPads);
    fh1_Baseline.resize(fPads);
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
    if (!fMap_Par)
    {
        R3BLOG(warn, "Container actafMappingPar not found.");
        return;
    }

    nBinsSample = fMap_Par->GetNBinsSample();
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

    fWrItems = dynamic_cast<TClonesArray*>(mgr->GetObject("WRActafData"));
    R3BLOG_IF(warn, fWrItems == nullptr, "WRActafData not found");

    fActafGeo = R3BActafGeometry::Instance();
    R3BLOG_IF(warn, !fActafGeo->Init(fGeoversion), "Actaf geometry " << fGeoversion << " not found");

    // MAIN FOLDER-Actaf
    auto* mainfol = new TFolder("Actaf", "Actaf info");
    // Folder for mapped data
    auto* mapfol = new TFolder("Map", "Map Actaf info");
    // Folder for cal data
    auto* calfol = new TFolder("Cal", "Cal Actaf info");
    // Folder for hit data
    auto* hitfol = new TFolder("Hit", "Hit Actaf info");
    // Folder for sync data
    auto* syncfol = new TFolder("Sync", "Sync Actaf info");

    //
    // Create histograms
    //

    SetParameter();

    // ********* MAP HISTOGRAMS ********* //

    auto* cSum = new TCanvas("Summary_map", "mapped info", 10, 10, 500, 500);
    cSum->Divide(3, 2);

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
    fh2_MaxPos_map = R3B::root_owned<TH2F>(
        "fh2_Maxpos_vs_pad_map", "Max.-position vs Pad", fPads, 0.5, 0.5 + fPads, 300, 0, nBinsSample);
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

    cSum->cd(5);
    fh2_RmsMapVsPad =
        R3B::root_owned<TH2F>("fh2_RmsMapVsPad", "Baseline RMS per pad", fPads, 0.5, 0.5 + fPads, 100, 0, 50);
    fh2_RmsMapVsPad->GetXaxis()->SetTitle("Pad");
    fh2_RmsMapVsPad->GetYaxis()->SetTitle("RMS [ADC Chn]");
    fh2_RmsMapVsPad->GetYaxis()->SetTitleOffset(1.1);
    fh2_RmsMapVsPad->GetXaxis()->CenterTitle(true);
    fh2_RmsMapVsPad->GetYaxis()->CenterTitle(true);
    fh2_RmsMapVsPad->Draw("colz");

    mapfol->Add(cSum);

    // Canvas for ring and side
    std::vector<std::vector<std::vector<TCanvas*>>> cMap_perRing(2);
    std::vector<std::vector<int>> countsPerRing(2);

    for (int iside = 0; iside < 2; iside++)
    {
        cMap_perRing[iside].resize(fRings);
        countsPerRing[iside].resize(fRings);

        for (int iring = 0; iring < fRings; iring++)
        {
            countsPerRing[iside][iring] = 0;

            std::string sideName = iside == 0 ? "UP" : "DOWN";

            std::string nameCanvas = sideName + "_RING_" + std::to_string(iring + 1) + "_traces_map";
            auto* cMap = new TCanvas(nameCanvas.c_str(), "mapped info", 10, 10, 500, 500);
            cMap->Divide(4, 4);
            cMap_perRing[iside][iring].push_back(cMap);

            std::string nameCanvasE = sideName + "_RING_" + std::to_string(iring + 1) + "_ERaw";
            auto* cMapE = new TCanvas(nameCanvasE.c_str(), "ERaw info", 10, 10, 500, 500);
            cMapE->Divide(4, 4);
            cMap_perRing[iside][iring].push_back(cMapE);

            std::string nameCanvasB = sideName + "_RING_" + std::to_string(iring + 1) + "_Baseline";
            auto* cMapB = new TCanvas(nameCanvasB.c_str(), "Baseline info", 10, 10, 500, 500);
            cMapB->Divide(4, 4);
            cMap_perRing[iside][iring].push_back(cMapB);
        }
    }

    for (int adc = 0; adc < fFadcs; adc++)
    {
        // Canvas per FADC Module
        std::string nameCanvas = "FADC_" + std::to_string(adc + 1) + "_traces_map";
        auto* cMap = new TCanvas(nameCanvas.c_str(), "mapped info", 10, 10, 500, 500);
        cMap->Divide(4, 4);

        std::string nameCanvasE = "FADC_" + std::to_string(adc + 1) + "_ERaw";
        auto* cMapE = new TCanvas(nameCanvasE.c_str(), "ERaw info", 10, 10, 500, 500);
        cMapE->Divide(4, 4);

        std::string nameCanvasB = "FADC_" + std::to_string(adc + 1) + "_Baseline";
        auto* cMapB = new TCanvas(nameCanvasB.c_str(), "Baseline info", 10, 10, 500, 500);
        cMapB->Divide(4, 4);

        std::string canvasFilteredTraces = "FADC_" + std::to_string(adc + 1) + "_Filtered";
        auto* cCalFilt = new TCanvas(canvasFilteredTraces.c_str(), "Filtered Signals info", 10, 10, 500, 500);
        cCalFilt->Divide(4, 4);

        int chn = 0;

        for (int index = 0; index < fPads; ++index)
        {
            int FADCnum = fMap_Par->GetFADCModuleByPad(index + 1);
            int FADCchn = fMap_Par->GetFADCChannelByPad(index + 1);

            int sideNb = index + 1 > 64 ? 1 : 0;
            int ringNb = fActafGeo->GetRingId(index + 1);

            int padOff = index + 1 > 64 ? 64 : 0;

            std::string titleHist = "Raw trace: Pad " + std::to_string(index + 1 - padOff) + " (Mod " +
                                    std::to_string(FADCnum) + " Chn " + std::to_string(FADCchn) + ")";
            std::string nameHist = "fh2_Pad_" + std::to_string(index) + "_trace";

            // Only plot the pads that belong to the FADC
            if (FADCnum != adc + 1)
                continue;
            else
                chn++;

            fh2_RawTraces[index] =
                R3B::root_owned<TH2F>(nameHist.c_str(), titleHist.c_str(), nBinsSample, 1, nBinsSample, 1000, 0, 20000);

            fh2_RawTraces[index]->GetXaxis()->SetTitle("Time [Chn]");
            fh2_RawTraces[index]->GetYaxis()->SetTitle("A");
            fh2_RawTraces[index]->GetYaxis()->SetTitleOffset(1.1);
            fh2_RawTraces[index]->GetXaxis()->CenterTitle(true);
            fh2_RawTraces[index]->GetYaxis()->CenterTitle(true);
            cMap->cd(chn);
            fh2_RawTraces[index]->Draw("colz");

            // Draw the per-ring canvas
            cMap_perRing[sideNb][ringNb - 1][0]->cd(++countsPerRing[sideNb][ringNb - 1]);
            fh2_RawTraces[index]->Draw("colz");

            // Filtered traces (CAL LEVEL!)
            std::string nameFiltHist = "fh2_Pad_" + std::to_string(index + 1) + "filtered_trace";
            fh2_FilteredTraces[index] = R3B::root_owned<TH2F>(
                nameFiltHist.c_str(), titleHist.c_str(), nBinsSample, 1, nBinsSample, nBinsTrace, nTraceMin, nTraceMax);
            fh2_FilteredTraces[index]->GetXaxis()->SetTitle("Time [Chn]");
            fh2_FilteredTraces[index]->GetYaxis()->SetTitle("A");
            fh2_FilteredTraces[index]->GetYaxis()->SetTitleOffset(1.1);
            fh2_FilteredTraces[index]->GetXaxis()->CenterTitle(true);
            fh2_FilteredTraces[index]->GetYaxis()->CenterTitle(true);
            cCalFilt->cd(chn);
            fh2_FilteredTraces[index]->Draw("colz");

            std::string nameHistE = "fh1_Pad_" + std::to_string(index + 1) + "_Eraw";
            std::string titleHistE = "ERaw: Pad " + std::to_string(index + 1 - padOff) + " (Mod " +
                                     std::to_string(FADCnum) + " Chn " + std::to_string(FADCchn) + ")";
            fh1_RawE[index] = R3B::root_owned<TH1F>(nameHistE.c_str(), titleHistE.c_str(), 1000, 0, 30000);
            fh1_RawE[index]->GetXaxis()->SetTitle("E [ADC Chn]");
            fh1_RawE[index]->GetYaxis()->SetTitle("Counts");
            fh1_RawE[index]->GetYaxis()->SetTitleOffset(1.1);
            fh1_RawE[index]->GetXaxis()->CenterTitle(true);
            fh1_RawE[index]->GetYaxis()->CenterTitle(true);
            fh1_RawE[index]->SetFillColor(31);
            cMapE->cd(chn);
            fh1_RawE[index]->Draw();

            // Draw the per-ring canvas
            cMap_perRing[sideNb][ringNb - 1][1]->cd(countsPerRing[sideNb][ringNb - 1]);
            fh1_RawE[index]->Draw("colz");

            std::string nameHistB = "fh1_Pad_" + std::to_string(index + 1) + "_Baseline";
            std::string titleHistB = "Baseline: Pad " + std::to_string(index + 1 - padOff) + " (Mod " +
                                     std::to_string(FADCnum) + " Chn " + std::to_string(FADCchn) + ")";
            fh1_Baseline[index] = R3B::root_owned<TH1F>(nameHistB.c_str(), titleHistB.c_str(), 300, 7000, 9000);
            fh1_Baseline[index]->GetXaxis()->SetTitle("Baseline [ADC Chn]");
            fh1_Baseline[index]->GetYaxis()->SetTitle("Counts");
            fh1_Baseline[index]->GetYaxis()->SetTitleOffset(1.1);
            fh1_Baseline[index]->GetXaxis()->CenterTitle(true);
            fh1_Baseline[index]->GetYaxis()->CenterTitle(true);
            fh1_Baseline[index]->SetFillColor(31);
            cMapB->cd(chn);
            fh1_Baseline[index]->Draw();

            // Draw the per-ring canvas
            cMap_perRing[sideNb][ringNb - 1][2]->cd(countsPerRing[sideNb][ringNb - 1]);
            fh1_Baseline[index]->Draw("colz");
        }

        if (fDisplaytraces)
        {
            mapfol->Add(cMap);
        }

        mapfol->Add(cMapE);
        mapfol->Add(cMapB);
        calfol->Add(cCalFilt);
    }

    for (auto const& vecSide : cMap_perRing)
        for (auto const& vecSideRing : vecSide)
            for (int iCanvasSideRing = 2 * (1 - fDisplaytraces); iCanvasSideRing < 3; iCanvasSideRing++)
                mapfol->Add(vecSideRing[iCanvasSideRing]);

    // FADC Channel [1 - 16] vs Module [1 - 9] (8 modules + 1 for AMBER signal)
    auto* cModVsCh = new TCanvas("ModVsCh_map", "FADC Module vs Channel", 10, 10, 500, 500);
    fh2_ModVsCh_map = R3B::root_owned<TH2F>("fh2_ModVsCh_map", "Channel vs Mod", 9, 0.5, 9.5, 16, 0.5, 16.5);
    fh2_ModVsCh_map->GetXaxis()->SetTitle("Module");
    fh2_ModVsCh_map->GetYaxis()->SetTitle("Channel");
    fh2_ModVsCh_map->GetYaxis()->SetTitleOffset(1.1);
    fh2_ModVsCh_map->GetXaxis()->CenterTitle(true);
    fh2_ModVsCh_map->GetYaxis()->CenterTitle(true);
    fh2_ModVsCh_map->Draw("colz");
    mapfol->Add(cModVsCh);

    auto* cdetmask = new TCanvas("Det_mask", "Detector mask", 10, 10, 500, 500);
    fh1_DetMask = R3B::root_owned<TH1F>("fh1_detmask", "Detector mask", 513, -0.5, 512.5);
    fh1_DetMask->GetXaxis()->SetTitle("Detector mask");
    fh1_DetMask->GetYaxis()->SetTitle("Counts");
    fh1_DetMask->GetYaxis()->SetTitleOffset(1.1);
    fh1_DetMask->GetXaxis()->CenterTitle(true);
    fh1_DetMask->GetYaxis()->CenterTitle(true);
    fh1_DetMask->SetFillColor(2);
    gPad->SetLogy();
    fh1_DetMask->Draw();

    mapfol->Add(cdetmask);

    auto* ctimetag = new TCanvas("TimeTag_signal", "TimeTag Signal", 10, 10, 500, 500);
    fh2_timetag_signal = R3B::root_owned<TH2F>(
        "fh2_timetag_signal", "TimeTag Signal", nBinsSample / 2, 1, nBinsSample, nBinsTrace, nTraceMin, 11000);
    fh2_timetag_signal->GetXaxis()->SetTitle("Time [Chn]");
    fh2_timetag_signal->GetYaxis()->SetTitle("A");
    fh2_timetag_signal->GetYaxis()->SetTitleOffset(1.1);
    fh2_timetag_signal->GetXaxis()->CenterTitle(true);
    fh2_timetag_signal->GetYaxis()->CenterTitle(true);
    fh2_timetag_signal->Draw("colz");
    mapfol->Add(ctimetag);

    // Mean value of the baseline after and before filtering
    auto* cmawMap = new TCanvas("Maw_map", "Maw (Map) Vs Integrated Energy", 10, 10, 500, 500);
    cmawMap->Divide(2, 1);

    for (int iside = 0; iside < 2; iside++)
    {
        TString name = iside == 1 ? "fh2_mawVsEMap_Down" : "fh2_mawVsEMap_Up";
        TString tit = iside == 1 ? "Maw (Map) Vs Integrated Energy (Downstream side)"
                                 : "Maw (Map) Vs Integrated Energy (Upstream side)";

        cmawMap->cd(iside + 1);
        fh2_mawVsEMap[iside] = R3B::root_owned<TH2F>(name, tit, nBinsEcal, 0.5, 0.5 + nEcalMax, 300, 1, 500000);
        fh2_mawVsEMap[iside]->GetXaxis()->SetTitle("Energy [Chn]");
        fh2_mawVsEMap[iside]->GetYaxis()->SetTitle("MAW parameter");
        fh2_mawVsEMap[iside]->GetYaxis()->SetTitleOffset(1.1);
        fh2_mawVsEMap[iside]->GetXaxis()->CenterTitle(true);
        fh2_mawVsEMap[iside]->GetYaxis()->CenterTitle(true);
        fh2_mawVsEMap[iside]->Draw("colz");
    }

    mapfol->Add(cmawMap);

    mainfol->Add(mapfol);

    // ********* CAL HISTOGRAMS ********* //

    auto* cCal = new TCanvas("Cal_data", "cal info", 10, 10, 500, 500);
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

    // RMS of the baseline after and before filtering
    auto* cRms = new TCanvas("Rms_cal", "Baseline RMS", 10, 10, 500, 500);
    cRms->Divide(2, 2);

    cRms->cd(1);
    fh2_sigmaInitVsPad =
        R3B::root_owned<TH2F>("fh2_sigmaInitVsPad", "Baseline RMS per pad", fPads, 0.5, 0.5 + fPads, 100, 0, 50);
    fh2_sigmaInitVsPad->GetXaxis()->SetTitle("Pad");
    fh2_sigmaInitVsPad->GetYaxis()->SetTitle("RMS [ADC Chn]");
    fh2_sigmaInitVsPad->GetYaxis()->SetTitleOffset(1.1);
    fh2_sigmaInitVsPad->GetXaxis()->CenterTitle(true);
    fh2_sigmaInitVsPad->GetYaxis()->CenterTitle(true);
    fh2_sigmaInitVsPad->Draw("colz");

    cRms->cd(2);
    fh2_sigmaFiltVsPad = R3B::root_owned<TH2F>(
        "fh2_sigmaFiltVsPad", "Baseline RMS (filtered) per pad", fPads, 0.5, 0.5 + fPads, 100, 0, 50);
    fh2_sigmaFiltVsPad->GetXaxis()->SetTitle("Pad");
    fh2_sigmaFiltVsPad->GetYaxis()->SetTitle("RMS [ADC Chn]");
    fh2_sigmaFiltVsPad->GetYaxis()->SetTitleOffset(1.1);
    fh2_sigmaFiltVsPad->GetXaxis()->CenterTitle(true);
    fh2_sigmaFiltVsPad->GetYaxis()->CenterTitle(true);
    fh2_sigmaFiltVsPad->Draw("colz");

    cRms->cd(3);
    fh1_sigmaInit = R3B::root_owned<TH1F>("fh1_sigmaInit", "Baseline RMS", 100, 0, 50);
    fh1_sigmaInit->GetXaxis()->SetTitle("RMS [ADC Chn]");
    fh1_sigmaInit->GetYaxis()->SetTitle("Counts");
    fh1_sigmaInit->GetYaxis()->SetTitleOffset(1.1);
    fh1_sigmaInit->GetXaxis()->CenterTitle(true);
    fh1_sigmaInit->GetYaxis()->CenterTitle(true);
    fh1_sigmaInit->SetFillColor(31);
    fh1_sigmaInit->Draw();

    cRms->cd(4);
    fh1_sigmaFilt = R3B::root_owned<TH1F>("fh1_sigmaFilt", "Baseline RMS (filtered)", 100, 0, 50);
    fh1_sigmaFilt->GetXaxis()->SetTitle("RMS [ADC Chn]");
    fh1_sigmaFilt->GetYaxis()->SetTitle("Counts");
    fh1_sigmaFilt->GetYaxis()->SetTitleOffset(1.1);
    fh1_sigmaFilt->GetXaxis()->CenterTitle(true);
    fh1_sigmaFilt->GetYaxis()->CenterTitle(true);
    fh1_sigmaFilt->SetFillColor(31);
    fh1_sigmaFilt->Draw();

    calfol->Add(cRms);

    // Mean value of the baseline after and before filtering
    auto* cmean = new TCanvas("Mean_cal", "Baseline Mean", 10, 10, 500, 500);
    cmean->Divide(2, 1);

    cmean->cd(1);
    fh2_meanInitVsPad =
        R3B::root_owned<TH2F>("fh2_meanInitVsPad", "Baseline mean per pad", fPads, 0.5, 0.5 + fPads, 200, 5000, 10000);
    fh2_meanInitVsPad->GetXaxis()->SetTitle("Pad");
    fh2_meanInitVsPad->GetYaxis()->SetTitle("Baseline [ADC Chn]");
    fh2_meanInitVsPad->GetYaxis()->SetTitleOffset(1.1);
    fh2_meanInitVsPad->GetXaxis()->CenterTitle(true);
    fh2_meanInitVsPad->GetYaxis()->CenterTitle(true);
    fh2_meanInitVsPad->Draw("colz");

    cmean->cd(2);
    fh2_meanFiltVsPad = R3B::root_owned<TH2F>(
        "fh2_meanFiltVsPad", "Baseline mean (filtered) per pad", fPads, 0.5, 0.5 + fPads, 200, 5000, 10000);
    fh2_meanFiltVsPad->GetXaxis()->SetTitle("Pad");
    fh2_meanFiltVsPad->GetYaxis()->SetTitle("Baseline [ADC Chn]");
    fh2_meanFiltVsPad->GetYaxis()->SetTitleOffset(1.1);
    fh2_meanFiltVsPad->GetXaxis()->CenterTitle(true);
    fh2_meanFiltVsPad->GetYaxis()->CenterTitle(true);
    fh2_meanFiltVsPad->Draw("colz");

    calfol->Add(cmean);

    // Mean value of the baseline after and before filtering
    auto* cmaw = new TCanvas("Maw_cal", "Maw Vs Integrated Energy", 10, 10, 500, 500);
    cmaw->Divide(2, 1);

    for (int iside = 0; iside < 2; iside++)
    {

        TString name = iside == 1 ? "fh2_mawVsECal_Down" : "fh2_mawVsECal_Up";
        TString tit =
            iside == 1 ? "Maw Vs Integrated Energy (Downstream side)" : "Maw Vs Integrated Energy (Upstream side)";

        cmaw->cd(iside + 1);
        fh2_mawVsECal[iside] = R3B::root_owned<TH2F>(name, tit, nBinsEcal, 0.5, 0.5 + nEcalMax, 300, 1, 500000);
        fh2_mawVsECal[iside]->GetXaxis()->SetTitle("Energy [Chn]");
        fh2_mawVsECal[iside]->GetYaxis()->SetTitle("MAW parameter");
        fh2_mawVsECal[iside]->GetYaxis()->SetTitleOffset(1.1);
        fh2_mawVsECal[iside]->GetXaxis()->CenterTitle(true);
        fh2_mawVsECal[iside]->GetYaxis()->CenterTitle(true);
        fh2_mawVsECal[iside]->Draw("colz");
    }

    calfol->Add(cmaw);

    if (fCalItems != nullptr)
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
        i == 0 ? tit = "Counts per ring (upstream)" : tit = "Counts per ring (downstream)";
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
        i == 0 ? tit = "Pad plane (upstream)" : tit = "Pad plane (downstream)";

        cXY->cd(i + 1);

        fh2_XYPos[i] = R3B::root_owned<TH2Poly>();

        for (int iPad = 1 + 64 * i; iPad <= 65 + 64 * i; iPad++)
        {

            const std::vector<double> padParams = fActafGeo->GetPadParams(iPad);
            TVector3 padPos = fActafGeo->GetPosition(iPad);
            padPos.SetZ(0);

            // Parameters of pad
            const double rMin = padParams[0];
            const double rMax = padParams[1];

            const double phiCentral = padPos.Phi();
            const double phiMin = phiCentral - padParams[3] / 2.;
            const double phiMax = phiCentral + padParams[3] / 2.;

            const int nPoints = 30;

            // For the central pad
            if (padParams[0] == 0)
            {
                Double_t x[nPoints + 1];
                Double_t y[nPoints + 1];

                for (int iPoint = 0; iPoint < nPoints; iPoint++)
                {
                    x[iPoint] = padParams[1] *
                                TMath::Cos(static_cast<double>(iPoint) / static_cast<double>(nPoints) * padParams[3]);
                    y[iPoint] = padParams[1] *
                                TMath::Sin(static_cast<double>(iPoint) / static_cast<double>(nPoints) * padParams[3]);
                }

                fh2_XYPos[i]->AddBin(nPoints, x, y);
            }

            // other pads
            else
            {
                std::vector<double> x_vec;
                std::vector<double> y_vec;

                for (int iPoint = 0; iPoint < nPoints; iPoint++)
                {
                    x_vec.push_back(rMax * TMath::Cos(static_cast<double>(iPoint) / static_cast<double>(nPoints - 1) *
                                                          (phiMax - phiMin) +
                                                      phiMin));

                    y_vec.push_back(rMax * TMath::Sin(static_cast<double>(iPoint) / static_cast<double>(nPoints - 1) *
                                                          (phiMax - phiMin) +
                                                      phiMin));
                }

                for (int iPoint = 0; iPoint < nPoints; iPoint++)
                {
                    x_vec.push_back(rMin * TMath::Cos(static_cast<double>(nPoints - 1 - iPoint) /
                                                          static_cast<double>(nPoints - 1) * (phiMax - phiMin) +
                                                      phiMin));

                    y_vec.push_back(rMin * TMath::Sin(static_cast<double>(nPoints - 1 - iPoint) /
                                                          static_cast<double>(nPoints - 1) * (phiMax - phiMin) +
                                                      phiMin));
                }

                fh2_XYPos[i]->AddBin(2 * nPoints, x_vec.data(), y_vec.data());
            }
        }

        fh2_XYPos[i]->SetTitle(tit);
        fh2_XYPos[i]->GetXaxis()->SetTitle("X [cm]");
        fh2_XYPos[i]->GetYaxis()->SetTitle("Y [cm]");
        fh2_XYPos[i]->GetYaxis()->SetTitleOffset(1.1);
        fh2_XYPos[i]->GetXaxis()->CenterTitle(true);
        fh2_XYPos[i]->GetYaxis()->CenterTitle(true);
        fh2_XYPos[i]->SetLineColor(kBlack);
        fh2_XYPos[i]->SetLineWidth(1);
        fh2_XYPos[i]->Draw("colz ]");
        fh2_XYPos[i]->Draw("same L");
    }

    hitfol->Add(cXY);

    // Canvas with XY positions (one per ring) storing the tracks of three events (updated manually)
    auto* cXY_nevents = new TCanvas("X_Y_events", "XY positions per event", 10, 10, 500, 500);
    cXY_nevents->Divide(2, 1);

    for (int i = 0; i < fh2_XYPos_Evts.size(); i++)
    {
        cXY_nevents->cd(i + 1);
        fh2_XYPos_Evts[i] =
            static_cast<TH2Poly*>(fh2_XYPos[i]->Clone((fh2_XYPos[i]->GetTitle() + TString("_events_auto")).Data()));
        fh2_XYPos_Evts[i]->SetTitle(fh2_XYPos[i]->GetTitle() + TString(" per event (automatic)"));
        fh2_XYPos_Evts[i]->SetLineColor(kBlack);
        fh2_XYPos_Evts[i]->SetLineWidth(1);
        fh2_XYPos_Evts[i]->Draw("colz ]");
        fh2_XYPos_Evts[i]->Draw("same L");
    }

    hitfol->Add(cXY_nevents);

    // Canvas with XY positions (one per ring) storing the tracks of three events (updated each 3000 events)
    auto* cXY_nevents_auto = new TCanvas("X_Y_events_auto", "XY positions per event (automatic)", 10, 10, 500, 500);
    cXY_nevents_auto->Divide(2, 1);

    for (int i = 0; i < fh2_XYPos_Evts_Automatic.size(); i++)
    {
        cXY_nevents_auto->cd(i + 1);
        fh2_XYPos_Evts_Automatic[i] =
            static_cast<TH2Poly*>(fh2_XYPos[i]->Clone((fh2_XYPos[i]->GetTitle() + TString("_events")).Data()));
        fh2_XYPos_Evts_Automatic[i]->SetTitle(fh2_XYPos[i]->GetTitle() + TString(" per event"));
        fh2_XYPos_Evts_Automatic[i]->SetLineColor(kBlack);
        fh2_XYPos_Evts_Automatic[i]->SetLineWidth(1);
        fh2_XYPos_Evts_Automatic[i]->Draw("colz ]");
        fh2_XYPos_Evts_Automatic[i]->Draw("same L");
    }

    hitfol->Add(cXY_nevents_auto);

    // Canvas with XY positions (one per ring) and waveforms of the 4 pads with highest amplitude
    auto* cXY_nevents_waveform = new TCanvas("X_Y_events_waveform", "XY positions per event", 10, 10, 500, 500);
    cXY_nevents_waveform->Divide(2, 2);
    cXY_nevents_waveform->cd(1);
    fh2_XYPos_Evts[0]->SetMaximum(2e3);
    fh2_XYPos_Evts[1]->SetMaximum(2e3);
    fh2_XYPos_Evts[0]->Draw("colz ]");
    fh2_XYPos_Evts[0]->Draw("same L");
    cXY_nevents_waveform->cd(2);
    fh2_XYPos_Evts[1]->Draw("colz ]");
    fh2_XYPos_Evts[1]->Draw("same L");
    cXY_nevents_waveform->cd(3);
    TPad* p3 = (TPad*)gPad;
    p3->Divide(2, 2);
    cXY_nevents_waveform->cd(4);
    TPad* p4 = (TPad*)gPad;
    p4->Divide(2, 2);
    for (int iPad = 0; iPad < 8; iPad++)
    {
        // Use TGraph for the corrected traces of the 4 pads with highest amplitude
        g_CorrectedTraces_4pads_highestAmp[iPad] = new TGraph();
        g_CorrectedTraces_4pads_highestAmp[iPad]->SetName(Form("g_CorrectedTraces_4pads_highestAmp_%d", iPad));
        g_CorrectedTraces_4pads_highestAmp[iPad]->SetTitle(
            Form("Corrected Trace of pad with highest amplitude %d;Time [Chn];A", iPad + 1));
        g_CorrectedTraces_4pads_highestAmp[iPad]->SetLineColor(kBlue);
        if (iPad < 4)
            p3->cd(iPad + 1);
        else
            p4->cd(iPad - 4 + 1);
        g_CorrectedTraces_4pads_highestAmp[iPad]->GetYaxis()->SetRangeUser(-200, 1000);
        g_CorrectedTraces_4pads_highestAmp[iPad]->Draw("AL");
        g_CorrectedTraces_4pads_highestAmp[iPad]->GetYaxis()->SetRangeUser(-200, 1000);
    }

    hitfol->Add(cXY_nevents_waveform);

    // Canvas with XY positions (one per ring) and waveforms of the 4 pads with highest amplitude (auto)
    auto* cXY_nevents_waveform_auto =
        new TCanvas("X_Y_events_waveform auto", "XY positions per event auto", 10, 10, 500, 500);
    cXY_nevents_waveform_auto->Divide(2, 2);
    cXY_nevents_waveform_auto->cd(1);
    fh2_XYPos_Evts_Automatic[0]->SetMaximum(2e3);
    fh2_XYPos_Evts_Automatic[1]->SetMaximum(2e3);
    fh2_XYPos_Evts_Automatic[0]->Draw("colz ]");
    fh2_XYPos_Evts_Automatic[0]->Draw("same L");
    cXY_nevents_waveform_auto->cd(2);
    fh2_XYPos_Evts_Automatic[1]->Draw("colz ]");
    fh2_XYPos_Evts_Automatic[1]->Draw("same L");
    cXY_nevents_waveform_auto->cd(3);
    TPad* p3_auto = (TPad*)gPad;
    p3_auto->Divide(2, 2);
    cXY_nevents_waveform_auto->cd(4);
    TPad* p4_auto = (TPad*)gPad;
    p4_auto->Divide(2, 2);
    for (int iPad = 0; iPad < 8; iPad++)
    {
        // Use TGraph for the corrected traces of the 4 pads with highest amplitude
        g_CorrectedTraces_4pads_highestAmp_auto[iPad] = new TGraph();
        g_CorrectedTraces_4pads_highestAmp_auto[iPad]->SetName(
            Form("g_CorrectedTraces_4pads_highestAmp_auto_%d", iPad));
        g_CorrectedTraces_4pads_highestAmp_auto[iPad]->SetTitle(
            Form("Corrected Trace of pad with highest amplitude %d;Time [Chn];A", iPad + 1));
        g_CorrectedTraces_4pads_highestAmp_auto[iPad]->SetLineColor(kBlue);
        if (iPad < 4)
            p3_auto->cd(iPad + 1);
        else
            p4_auto->cd(iPad - 4 + 1);
        g_CorrectedTraces_4pads_highestAmp_auto[iPad]->GetYaxis()->SetRangeUser(-200, 1000);
        g_CorrectedTraces_4pads_highestAmp_auto[iPad]->Draw("AL");
        g_CorrectedTraces_4pads_highestAmp_auto[iPad]->GetYaxis()->SetRangeUser(-200, 1000);
    }

    hitfol->Add(cXY_nevents_waveform_auto);

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

    if (fHitItems != nullptr)
        mainfol->Add(hitfol);

    auto* cSync = new TCanvas("Sync", "", 10, 10, 500, 500);
    cSync->Divide(3, 3);

    for (auto i = 0; i < nbWrs; i++)
    {
        fh1_Sync.push_back(
            R3B::root_owned<TH1F>(Form("fh1_wr%d_timetag", i + 1), Form("WR%d - TimeTag", i + 1), 2000, -50, 50));
        fh1_Sync[i]->GetXaxis()->SetTitle(Form("WR%d - TimeTag [ns]", i + 1));
        fh1_Sync[i]->GetYaxis()->SetTitle("Counts");
        fh1_Sync[i]->GetYaxis()->SetTitleOffset(1.1);
        fh1_Sync[i]->GetXaxis()->CenterTitle(true);
        fh1_Sync[i]->GetYaxis()->CenterTitle(true);
        fh1_Sync[i]->SetFillColor(31);
        cSync->cd(i + 1);
        gPad->SetLogy();
        fh1_Sync[i]->Draw();
        gStyle->SetOptStat(111111);
        cSync->Update();
    }
    syncfol->Add(cSync);

    auto* cWrSync = new TCanvas("WrSync", "", 10, 10, 500, 500);
    cWrSync->Divide(3, 3);

    for (auto i = 0; i < nbWrs - 1; i++)
    {
        fh1_WrSync.push_back(R3B::root_owned<TH1F>(Form("fh1_wr%d", i + 1), Form("WR%d - WR1", i + 2), 2000, -50, 50));
        fh1_WrSync[i]->GetXaxis()->SetTitle(Form("WR%d - WR1 [ns]", i + 2));
        fh1_WrSync[i]->GetYaxis()->SetTitle("Counts");
        fh1_WrSync[i]->GetYaxis()->SetTitleOffset(1.1);
        fh1_WrSync[i]->GetXaxis()->CenterTitle(true);
        fh1_WrSync[i]->GetYaxis()->CenterTitle(true);
        fh1_WrSync[i]->SetFillColor(31);
        cWrSync->cd(i + 1);
        gPad->SetLogy();
        fh1_WrSync[i]->Draw();
        gStyle->SetOptStat(111111);
        cWrSync->Update();
    }
    syncfol->Add(cWrSync);

    cRates = new TCanvas("Rates", "", 10, 10, 500, 500);

    std::vector<std::string> titles = { "Overall rate [Hz]", "Upstream rate [Hz]", "Downstream rate [Hz]" };

    auto fh1_rates = R3B::root_owned<TH1F>("fh1_rates", "Rates [Hz]", max_second_for_rate, 0, max_second_for_rate);
    fh1_rates->GetXaxis()->SetTitle("Time [s]");
    fh1_rates->GetYaxis()->SetTitle("Rate of good signals [Hz]");
    fh1_rates->GetYaxis()->SetTitleOffset(1.1);
    fh1_rates->GetXaxis()->CenterTitle(true);
    fh1_rates->GetYaxis()->CenterTitle(true);
    fh1_rates->SetMinimum(0);
    fh1_rates->SetMaximum(max_rate);
    fh1_rates->SetDirectory(0);
    fh1_rates->SetStats(0);

    for (int i = 0; i < 3; i++)
    {
        auto gr = new TGraph();
        gr->SetName(Form("gr%d", i));
        gr->SetTitle(titles[i].c_str());
        gr->SetMarkerStyle(24);
        gr->SetMarkerColor(i + 1);
        gr->SetMarkerSize(2);
        fgraph_rates.push_back(gr);
        if (i == 0)
        {
            fgraph_rates[i]->SetHistogram(fh1_rates);
            fgraph_rates[i]->Draw("ap");
        }
        else
            fgraph_rates[i]->Draw("samep");
    }

    auto* leg = new TLegend(0.58, 0.7, 0.84, 0.88, NULL, "brNDC");
    leg->SetBorderSize(0);
    leg->SetTextFont(62);
    leg->SetTextSize(0.04);
    leg->SetLineColor(1);
    leg->SetLineStyle(1);
    leg->SetLineWidth(1);
    leg->SetFillStyle(0);
    auto* entry = leg->AddEntry("gr0", "", "p");
    entry->SetTextFont(62);
    entry = leg->AddEntry("gr1", "", "p");
    entry->SetTextFont(62);
    entry = leg->AddEntry("gr2", "", "p");
    entry->SetTextFont(62);
    leg->Draw();

    syncfol->Add(cRates);

    if (fWrItems != nullptr)
    {
        mainfol->Add(syncfol);
    }

    run->AddObject(mainfol);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_Actaf_HIST", Form("/Objects/%s/->Reset_Histo()", GetName()));
    run->GetHttpServer()->RegisterCommand("Next_event", Form("/Objects/%s/->Next_event()", GetName()));
    run->GetHttpServer()->RegisterCommand("Prev_event", Form("/Objects/%s/->Prev_event()", GetName()));
    run->GetHttpServer()->RegisterCommand("Reset_event", Form("/Objects/%s/->Reset_event()", GetName()));

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BActafOnlineSpectra::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

// Event viewer controls
void R3BActafOnlineSpectra::plotSingleEventCanvas()
{
    if (eventViewerNb >= maxEventViewerBatch)
    {

        fh2_XYPos_Evts[0]->Reset("");
        fh2_XYPos_Evts[1]->Reset("");
        for (auto& graph : g_CorrectedTraces_4pads_highestAmp)
        {
            if (graph)
                graph->Set(0); // Clear
        }

        for (int ipad = 0; ipad < 128; ipad++)
        {
            int iside = ipad > 63 ? 1 : 0;

            double x = eventCountsX[selectEvent][ipad];
            double y = eventCountsY[selectEvent][ipad];
            double energy = eventCountsE[selectEvent][ipad];

            if (energy > 0)
            {
                auto bin = fh2_XYPos_Evts[iside]->FindBin(x, y);
                fh2_XYPos_Evts[iside]->SetBinContent(bin, energy);
            }
        }

        for (int iside = 0; iside < 2; iside++)
        {
            TString tit = "Pad plane";
            tit += iside == 0 ? " (upstream)" : " (downstream)";
            tit += Form(" event Nb: %d", selectEvent + firstBufferEvent);
            fh2_XYPos_Evts[iside]->SetTitle(tit);
            fh2_XYPos_Evts[iside]->Draw("colz ]");
            fh2_XYPos_Evts[iside]->Draw("same L");
        }

        // draw traces for the selected event: pick top-4 pads per side by energy
        // compute top-4 pads per side
        std::array<int, 4> topUp;
        topUp.fill(-1);
        std::array<double, 4> topUpE;
        topUpE.fill(-1.0);
        std::array<int, 4> topDown;
        topDown.fill(-1);
        std::array<double, 4> topDownE;
        topDownE.fill(-1.0);

        for (int ipad = 0; ipad < fPads; ++ipad)
        {
            double e = eventCountsE[selectEvent][ipad];
            if (e <= 0.)
                continue;
            int pad = ipad + 1;
            if (pad < 65)
            {
                for (int i = 0; i < 4; ++i)
                {
                    if (e > topUpE[i])
                    {
                        for (int j = 3; j > i; --j)
                        {
                            topUpE[j] = topUpE[j - 1];
                            topUp[j] = topUp[j - 1];
                        }
                        topUpE[i] = e;
                        topUp[i] = pad;
                        break;
                    }
                }
            }
            else
            {
                for (int i = 0; i < 4; ++i)
                {
                    if (e > topDownE[i])
                    {
                        for (int j = 3; j > i; --j)
                        {
                            topDownE[j] = topDownE[j - 1];
                            topDown[j] = topDown[j - 1];
                        }
                        topDownE[i] = e;
                        topDown[i] = pad;
                        break;
                    }
                }
            }
        }

        // find the waveform canvas and its pads (created in Init)
        // use the global gROOT pointer to search the object table
        TCanvas* cWave = static_cast<TCanvas*>(gROOT->FindObject("X_Y_events_waveform"));
        TPad* p3 = nullptr;
        TPad* p4 = nullptr;
        if (cWave)
        {
            cWave->cd(3);
            p3 = static_cast<TPad*>(gPad);
            cWave->cd(4);
            p4 = static_cast<TPad*>(gPad);
        }

        // fill and draw the graphs: upstream -> graphs 0..3 in p3, downstream -> 4..7 in p4
        for (int i = 0; i < 4; ++i)
        {
            int pad = topUp[i];
            auto* g = g_CorrectedTraces_4pads_highestAmp[i];
            g->GetYaxis()->SetRangeUser(-200, 1000);
            g->GetHistogram()->SetMinimum(-200);
            g->GetHistogram()->SetMaximum(1000);
            if (g)
            {
                g->Set(0);
                if (pad > 0)
                {
                    const auto& vec = eventCountsTrace[selectEvent][pad - 1];
                    std::size_t idx = 0;
                    for (const auto& v : vec)
                    {
                        if (v == 0)
                            continue;
                        g->SetPoint(g->GetN(), idx++, v);
                    }
                    g->SetTitle(Form("Corrected Trace of pad U%d;Time [Chn];A", pad));
                }
                if (cWave && p3)
                {
                    cWave->cd(3);
                    p3->cd(i + 1);
                    g->Draw("AL");
                    g->GetYaxis()->SetRangeUser(-200, 1000);
                    g->GetHistogram()->SetMinimum(-200);
                    g->GetHistogram()->SetMaximum(1000);
                }
            }
        }

        for (int i = 0; i < 4; ++i)
        {
            int pad = topDown[i];
            auto* g = g_CorrectedTraces_4pads_highestAmp[i + 4];
            g->GetYaxis()->SetRangeUser(-200, 1000);
            g->GetHistogram()->SetMinimum(-200);
            g->GetHistogram()->SetMaximum(1000);
            if (g)
            {
                g->Set(0);
                if (pad > 0)
                {
                    const auto& vec = eventCountsTrace[selectEvent][pad - 1];
                    std::size_t idx = 0;
                    for (const auto& v : vec)
                    {
                        if (v == 0)
                            continue;
                        g->SetPoint(g->GetN(), idx++, v);
                    }
                    g->SetTitle(Form("Corrected Trace of pad D%d;Time [Chn];A", pad - 64));
                }
                if (cWave && p4)
                {
                    cWave->cd(4);
                    p4->cd(i + 1);
                    g->Draw("AL");
                    g->GetYaxis()->SetRangeUser(-200, 1000);
                    g->GetHistogram()->SetMinimum(-200);
                    g->GetHistogram()->SetMaximum(1000);
                }
            }
        }
    }
    else
    {
        std::cout << "The buffer is being filled. Please wait some events more.\n";
    }
}

void R3BActafOnlineSpectra::Next_event()
{
    if (selectEvent < maxEventViewerBatch - 1)
        selectEvent++;
    else
    {
        std::cout << "You have reached the last event of the buffer. Time to reset!\n";
    }

    plotSingleEventCanvas();
}

void R3BActafOnlineSpectra::Prev_event()
{
    if (selectEvent > 0)
        selectEvent--;
    else
    {
        std::cout << "You are already in the first event of the buffer.\n";
    }

    plotSingleEventCanvas();
}

void R3BActafOnlineSpectra::Reset_event()
{
    eventViewerNb = 0;
    selectEvent = 0;
    firstBufferEvent = fNEvents;
    for (int ie = 0; ie < maxEventViewerBatch; ++ie)
    {
        for (int ip = 0; ip < fPads; ++ip)
            eventCountsTrace[ie][ip].clear();
    }
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
        fh1_sigmaInit->Reset();
        fh1_sigmaFilt->Reset();
        fh2_sigmaInitVsPad->Reset();
        fh2_RmsMapVsPad->Reset();
        fh2_sigmaFiltVsPad->Reset();
        fh2_meanInitVsPad->Reset();
        fh2_meanFiltVsPad->Reset();
        fh1_DetMask->Reset();
        fh2_timetag_signal->Reset();
        for (const auto& hist : fh2_RawTraces)
        {
            hist->Reset();
        }

        for (const auto& hist : fh2_mawVsECal)
        {
            hist->Reset();
        }

        for (const auto& hist : fh2_mawVsEMap)
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
        for (const auto& hist : fh1_Sync)
        {
            hist->Reset();
        }
        for (const auto& hist : fh1_WrSync)
        {
            hist->Reset();
        }
        for (auto* gr : fgraph_rates)
        {
            if (gr)
                gr->Set(0); // Clear
        }
    }

    if (fCalItems)
    {
        fh2_Ecal_cal->Reset();
        fh2_zPos_cal->Reset();
        fh2_tLeading_cal->Reset();
        fh2_maxAmp_cal->Reset();
        fh2_tSync_cal->Reset();
        for (const auto& hist : fh2_FilteredTraces)
        {
            hist->Reset();
        }
        for (const auto& hist : g_CorrectedTraces_4pads_highestAmp_auto)
        {
            if (hist)
                hist->Set(0); // clear TGraph points
        }
    }

    if (fHitItems)
    {
        for (auto& h : fh1_RingCounts)
            h->Reset();

        for (auto& h : fh2_XYPos)
            h->Reset("");

        for (auto& h : fh2_XYPos_Evts)
            h->Reset("");

        for (auto& h : fh2_XYPos_Evts_Automatic)
            h->Reset("");

        for (auto& h : fh1_PhiCounts)
            h->Reset();

        fh1_CountsPerSide->Reset();
        fh2_Phi1VsPhi2->Reset();
    }

    return;
}

void R3BActafOnlineSpectra::Exec(Option_t* /*option*/)
{
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

    uint64_t timetag = 0;

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

            if (pad == 129)
            {
                fh1_DetMask->Fill(hit->GetDetMask());
                timetag = hit->GetTimeTag();
            }

            if (pad == 128)
            {
                // timetag = hit->GetMaxpos();
                if (fDisplaytraces)
                {
                    auto vec = hit->GetTrace(); // std::vector
                    std::size_t index = 0;

                    for (const auto& value : vec)
                    {
                        if (value == 0)
                            continue;

                        if (hit->GetBaseline() > 0)
                            fh2_timetag_signal->Fill(index++, value + hit->GetBaseline());
                    }
                }
            }

            // Allow 128 pads for AMBER and R3B
            if (pad > fMap_Par->GetNbPads())
                continue;

            const int moduleNb = pad >= fMap_Par->GetNbPads() ? 9 : fMap_Par->GetFADCModuleByPad(pad + 1);
            const int channelNb = pad >= fMap_Par->GetNbPads() ? 1 : fMap_Par->GetFADCChannelByPad(pad + 1);

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
                fh2_RmsMapVsPad->Fill(pad + 1, hit->GetRms());

                int indexside = pad < 65 ? 0 : 1;
                fh2_mawVsEMap[indexside]->Fill(hit->GetE(), hit->GetMaw());
            }

            if (fDisplaytraces)
            {
                auto vec = hit->GetTrace();
                std::size_t index = 0;
                for (const auto& value : vec)
                {
                    if (value == 0)
                        continue;

                    if (hit->GetBaseline() > 0)
                    {
                        fh2_RawTraces[pad]->Fill(index++, value + hit->GetBaseline());
                    }
                }
            }
        }
    }

    // For overall rates
    overall_rate += fCalItems->GetEntriesFast();

    // Fill cal data
    std::array<double, 4> topAmpf;
    std::array<int, 4> topPadf;
    topAmpf.fill(0.);
    topPadf.fill(0.);
    std::array<double, 4> topAmps;
    std::array<int, 4> topPads;
    topAmps.fill(0.);
    topPads.fill(0.);
    for (auto& hist : g_CorrectedTraces_4pads_highestAmp_auto)
    {
        if (hist)
            hist->Set(0); // clear TGraph points
    }
    if (fCalItems && fCalItems->GetEntriesFast() > 0)
    {
        auto nHits = fCalItems->GetEntriesFast();
        for (int ihit = 0; ihit < nHits; ihit++)
        {
            auto* hit = dynamic_cast<R3BActafCalData*>(fCalItems->At(ihit));
            if (!hit)
                continue;

            double Ecal = hit->GetEnergy();
            double maxAmp = hit->GetEMaxAmpl();
            double tLeading = hit->GetLeadingEdgeTime();
            double zPos = hit->GetZpos();
            double tSync = hit->GetSynTime();
            int pad = hit->GetPad();
            if (pad < 65)
            {
                fsec_rate++;
                // Store top 4 pads with highest amplitude upstream
                for (int i = 0; i < 4; i++)
                {
                    if (maxAmp > topAmpf[i])
                    {
                        for (int j = 3; j > i; j--)
                        {
                            topAmpf[j] = topAmpf[j - 1];
                            topPadf[j] = topPadf[j - 1];
                        }
                        topAmpf[i] = maxAmp;
                        topPadf[i] = pad;
                        break;
                    }
                }
            }
            else if (pad > 64 && pad < 129)
            {
                ssec_rate++;
                for (int i = 0; i < 4; i++)
                {
                    if (maxAmp > topAmps[i])
                    {
                        for (int j = 3; j > i; j--)
                        {
                            topAmps[j] = topAmps[j - 1];
                            topPads[j] = topPads[j - 1];
                        }
                        topAmps[i] = maxAmp;
                        topPads[i] = pad;
                        break;
                    }
                }
            }

            if (pad > fPads)
                continue;

            fh2_Ecal_cal->Fill(pad, Ecal);
            fh2_zPos_cal->Fill(pad, zPos);
            fh2_tLeading_cal->Fill(pad, tLeading);
            fh2_maxAmp_cal->Fill(pad, maxAmp);
            fh2_tSync_cal->Fill(pad, tSync);

            if (hit->GetMean() > 0)
            {
                fh1_sigmaInit->Fill(hit->GetRmsRaw());
                fh1_sigmaFilt->Fill(hit->GetRms());

                fh2_sigmaFiltVsPad->Fill(pad + 1, hit->GetRms());
                fh2_sigmaInitVsPad->Fill(pad + 1, hit->GetRmsRaw());

                fh2_meanFiltVsPad->Fill(pad + 1, hit->GetMean());
                fh2_meanInitVsPad->Fill(pad + 1, hit->GetMeanRaw());

                int indexside = pad < 65 ? 0 : 1;

                fh2_mawVsECal[indexside]->Fill(Ecal, hit->GetMaw());
            }

            if (fDisplaytraces && pad < 129)
            {
                auto vec = hit->GetTrace(); // std::vector
                std::size_t index = 0;
                for (const auto& value : vec)
                {
                    if (value == 0)
                        continue;
                    fh2_FilteredTraces[pad - 1]->Fill(index++, value);
                }
            }
        }
    }

    // Second pass: fill the top-4 graphs now that topPadf/topPads are final
    if (fDisplaytraces)
    {
        // map pad -> graph index (-1 = none); pads are 1..fPads
        std::vector<int> padToGraph(fPads + 1, -1);
        for (int i = 0; i < 4; ++i)
        {
            if (topPadf[i] >= 1 && topPadf[i] <= fPads)
                padToGraph[topPadf[i]] = i; // upstream graphs 0..3
            if (topPads[i] >= 1 && topPads[i] <= fPads)
                padToGraph[topPads[i]] = i + 4; // downstream graphs 4..7
        }

        Int_t nHits2 = fCalItems ? fCalItems->GetEntriesFast() : 0;
        for (int ih = 0; ih < nHits2; ++ih)
        {
            auto* h = dynamic_cast<R3BActafCalData*>(fCalItems->At(ih));
            if (!h)
                continue;
            int pad = h->GetPad();
            if (pad < 1 || pad > fPads)
                continue;
            int gidx = padToGraph[pad];
            if (gidx < 0 || gidx >= static_cast<int>(g_CorrectedTraces_4pads_highestAmp_auto.size()))
                continue;

            auto vec = h->GetTrace();
            // store trace for event viewer buffer (cal-level traces)
            if (eventViewerNb < maxEventViewerBatch)
            {
                const auto& arr = h->GetTrace();
                auto& vec_trace = eventCountsTrace[eventViewerNb][pad - 1];
                vec_trace.assign(arr.begin(), arr.end());
            }
            std::size_t idx = 0;
            for (const auto& v : vec)
            {
                if (v == 0)
                    continue;
                if (g_CorrectedTraces_4pads_highestAmp_auto[gidx])
                    g_CorrectedTraces_4pads_highestAmp_auto[gidx]->SetPoint(
                        g_CorrectedTraces_4pads_highestAmp_auto[gidx]->GetN(), idx++, v);
            }
            if (g_CorrectedTraces_4pads_highestAmp_auto[gidx])
            {
                if (gidx < 4)
                    g_CorrectedTraces_4pads_highestAmp_auto[gidx]->SetTitle(
                        Form("Corrected Trace of pad U%d;Time [Chn];A", pad));
                else
                    g_CorrectedTraces_4pads_highestAmp_auto[gidx]->SetTitle(
                        Form("Corrected Trace of pad D%d;Time [Chn];A", pad - 64));
            }
        }
        for (auto& graph : g_CorrectedTraces_4pads_highestAmp_auto)
            graph->GetYaxis()->SetRangeUser(-200, 1000);
    }

    // Fill hit data

    if (fNEvents % updateRate == 0)
    {
        for (auto& hist : fh2_XYPos_Evts_Automatic)
            hist->Reset("");
    }

    bool goodEventForView = false;

    if (fHitItems && fHitItems->GetEntriesFast() > 0)
    {
        auto nHits = fHitItems->GetEntriesFast();
        for (size_t ihit = 0; ihit < nHits; ihit++)
        {
            auto* hit = dynamic_cast<R3BActafHitData*>(fHitItems->At(ihit));
            if (!hit)
                continue;

            auto pad = hit->GetPad();

            if (pad > fPads)
                continue;

            auto side = hit->GetSide() - 1;
            auto ring = hit->GetRing();
            auto x = hit->GetXpos();
            auto y = hit->GetYpos();
            auto energy = hit->GetEnergy();
            auto maxAmp = hit->GetMaxAmpl();

            auto track = hit->GetTrack();
            auto phi = track.Phi() * TMath::RadToDeg();

            fh1_RingCounts[side]->Fill(ring);
            fh2_XYPos[side]->Fill(x, y);

            if (eventViewerNb < maxEventViewerBatch)
            {
                if (maxAmp < 200)
                    continue;

                goodEventForView = true;
                eventCountsX[eventViewerNb][pad - 1] = x;
                eventCountsY[eventViewerNb][pad - 1] = y;
                eventCountsE[eventViewerNb][pad - 1] = maxAmp;
            }
            auto bin = fh2_XYPos_Evts_Automatic[side]->FindBin(x, y);
            maxAmp ? 0 : maxAmp = 0.1; // to avoid zeroing bin content for visualization
            fh2_XYPos_Evts_Automatic[side]->SetBinContent(bin, maxAmp);

            fh1_PhiCounts[side]->Fill(phi);
            fh1_CountsPerSide->Fill(side + 1);
        }
    }

    // R3BLOG(info,"wr: "<<fWrItems->GetEntriesFast());

    if (fWrItems && fWrItems->GetEntriesFast() > 0)
    {

        if (timetag < pre_timetag)
        {
            pre_timetag = timetag;
        }

        auto nHits = fWrItems->GetEntriesFast();
        std::vector<uint64_t> timestamps{ 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        for (int ihit = 0; ihit < nHits; ihit++)
        {
            auto* hit = dynamic_cast<R3BWRData*>(fWrItems->At(ihit));
            if (!hit)
                continue;

            auto id = hit->GetId() > 0 ? hit->GetId() - 1 : 0;

            timestamps[id] = hit->GetTimeStamp();

            fh1_Sync[id]->Fill(hit->GetTimeStamp() - pre_timestamp[id] - 2 * (timetag - pre_timetag));

            pre_timestamp[id] = hit->GetTimeStamp();
        }
        pre_timetag = timetag;

        if (first_timestamp == 0 || first_timestamp > timestamps[0])
            first_timestamp = timestamps[0];

        auto time_s = (timestamps[0] - first_timestamp) * 1e-9; // in seconds
        int sec = static_cast<int>(time_s);

        if (sec > last_second)
        {
            fgraph_rates[0]->SetPoint(fgraph_rates[0]->GetN(), sec, overall_rate);
            fgraph_rates[1]->SetPoint(fgraph_rates[1]->GetN(), sec, fsec_rate);
            fgraph_rates[2]->SetPoint(fgraph_rates[2]->GetN(), sec, ssec_rate);
            last_second = sec;

            if (overall_rate > max_rate)
            {
                max_rate = overall_rate + 100;
                cRates->cd();

                std::cout << "Update max rate: " << max_rate << std::endl << std::endl;

                TH1* hframe = fgraph_rates[0]->GetHistogram();
                if (hframe)
                {
                    hframe->SetMaximum(max_rate);
                    hframe->SetMinimum(0);
                }
                cRates->Modified();
                cRates->Update();
            }

            overall_rate = 0;
            fsec_rate = 0;
            ssec_rate = 0;
        }

        for (size_t index = 1; index < timestamps.size(); ++index)
        {
            fh1_WrSync[index - 1]->Fill(timestamps[index] - timestamps[0]);
        }
    }

    fNEvents++;
    if (goodEventForView)
        eventViewerNb++;
    return;
}

void R3BActafOnlineSpectra::FinishEvent()
{
    R3BLOG(debug, "Cleaning data structures");
    r3b::util::ClearIfNotNull(fMappedItems);
    r3b::util::ClearIfNotNull(fCalItems);
    r3b::util::ClearIfNotNull(fHitItems);
    r3b::util::ClearIfNotNull(fWrItems);
}

void R3BActafOnlineSpectra::FinishTask()
{
    if (saveHistos)
    {
        if (fMappedItems)
        {
            fh2_ERaw_map->Write();
            fh2_Baseline_map->Write();
            fh2_MaxPos_map->Write();
            fh2_Risetime_map->Write();
            fh2_ModVsCh_map->Write();
            fh1_sigmaInit->Write();
            fh1_sigmaFilt->Write();
            fh2_sigmaInitVsPad->Write();
            fh2_RmsMapVsPad->Write();
            fh2_sigmaFiltVsPad->Write();
            fh2_meanInitVsPad->Write();
            fh2_meanFiltVsPad->Write();
            fh1_DetMask->Write();
            fh2_timetag_signal->Write();
            for (const auto& hist : fh2_RawTraces)
            {
                hist->Write();
            }
            for (const auto& hist : fh2_mawVsECal)
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
            for (const auto& hist : fh1_Sync)
            {
                hist->Write();
            }
            for (const auto& hist : fh1_WrSync)
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
            for (const auto& hist : fh2_FilteredTraces)
            {
                hist->Write();
            }
        }

        if (fHitItems)
        {
            for (auto& h : fh1_RingCounts)
                h->Write();

            for (auto& h : fh2_XYPos)
                h->Write();

            for (auto& h : fh2_XYPos_Evts_Automatic)
                h->Write();

            for (auto& h : fh1_PhiCounts)
                h->Write();

            fh1_CountsPerSide->Write();

            fh2_Phi1VsPhi2->Write();
        }
    }
}
ClassImp(R3BActafOnlineSpectra)
