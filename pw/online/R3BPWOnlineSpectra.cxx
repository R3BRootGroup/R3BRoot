/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BPWOnlineSpectra.h"
#include "R3BLogger.h"
#include "R3BPWCalData.h"
#include "R3BPWHitData.h"
#include "R3BPWMappedData.h"
#include "R3BPWPreCalData.h"
#include "R3BShared.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TFolder.h"
#include "TH1F.h"
#include "TH2F.h"

#include <TClonesArray.h>
#include <THttpServer.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

R3BPWOnlineSpectra::R3BPWOnlineSpectra()
    : R3BPWOnlineSpectra("PWOnlineSpectra", 1)
{
}

R3BPWOnlineSpectra::R3BPWOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
}

InitStatus R3BPWOnlineSpectra::Init()
{
    R3BLOG(info, "");

    // Looking for FairRootManager
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, NULL == mgr, "FairRootManager not found");

    // Get access to Mapped data
    fMappedDataItems = dynamic_cast<TClonesArray*>(mgr->GetObject("R3BPWMappedData"));
    if (!fMappedDataItems)
    {
        R3BLOG(fatal, "R3BPWMappedData not found");
        return kFATAL;
    }

    fPreCalDataItems = dynamic_cast<TClonesArray*>(mgr->GetObject("R3BPWPreCalData"));
    if (!fPreCalDataItems)
    {
        R3BLOG(fatal, "R3BPWPreCalData not found");
        return kFATAL;
    }

    fCalDataItems = dynamic_cast<TClonesArray*>(mgr->GetObject("R3BPWCalData"));
    if (!fCalDataItems)
    {
        R3BLOG(fatal, "R3BPWCalData not found");
        return kFATAL;
    }

    fHitDataItems = dynamic_cast<TClonesArray*>(mgr->GetObject("R3BPWHitData"));
    if (!fHitDataItems)
    {
        R3BLOG(fatal, "PWHitData not found");
        return kFATAL;
    }

    /* ------------- FOLDERS --------------*/
    TFolder* mainfol = new TFolder("PW", "PW_info");

    // Folder for mapped data
    TFolder* mapfol = new TFolder("Map", "Map_PW_info");
    mainfol->Add(mapfol);

    // Folder for PreCal Data
    TFolder* preCalFolder = new TFolder("Pre Cal", "Pre_Cal_PW_info");
    mainfol->Add(preCalFolder);

    // Folder for Cal Data
    TFolder* calFolder = new TFolder("Cal", "Cal_PW_info");
    mainfol->Add(calFolder);

    // Folder for Hit Data
    TFolder* hitFolder = new TFolder("Hit", "Hit_PW_info");
    mainfol->Add(hitFolder);

    /* ----------- CANVASES ------------- */

    // Map canvases
    PWChannelsCanvas = new TCanvas("PadiwaChannels", "padiwa_channels");
    RefChannelsCanvas = new TCanvas("RefChannels", "ref_channels");

    // ----- Pre Cal Canvases -----
    PWTotCorrCanvas = new TCanvas("Channel_vs_Tot", "channel_vs_Tot");
    PWTimeCorrCanvas = new TCanvas("Channel_vs_Time", "channel_vs_Time");

    // ----- Cal Canvases -----
    PWCalTimeCorrCanvas1 = new TCanvas("Bar_Time_vs_Time1", "bar_Time_vs_Time1");
    PWCalTotCorrCanvas1 = new TCanvas("Bar_ToT_vs_ToT1", "bar_ToT_vs_ToT1");
    PWCalTimeCorrCanvas2 = new TCanvas("Bar_Time_vs_Time2", "bar_Time_vs_Time2");
    PWCalTotCorrCanvas2 = new TCanvas("Bar_ToT_vs_ToT2", "bar_ToT_vs_ToT2");
    PWCalTimeCorrCanvas1->Divide(4, 2);
    PWCalTimeCorrCanvas2->Divide(4, 2);
    PWCalTotCorrCanvas1->Divide(4, 2);
    PWCalTotCorrCanvas2->Divide(4, 2);

    // ------ Hit Canvases ------
    hitCorrCanvas = new TCanvas("Hit_ToT_Time", "hit_ToT_Time_Canvas");
    hitCorrCanvas->Divide(2, 2);

    hitMultCanvas = new TCanvas("Hit_Mult_Canvas", "hit_Mult_Canvas");
    hitMultCanvas->Divide(2, 2);

    // ----- Map Histograms -----

    SiPMChannelsHisto = R3B::root_owned<TH1F>("sipm_channels", "sipm_channels", 32, 0.5, 32.5);
    refChannelsHisto = R3B::root_owned<TH1F>("ref_channels", "ref_channels", 8, 0, 8);

    // ----- Pre Cal Histograms -----
    SiPMTotCorr =
        R3B::root_owned<TH2F>("SiPM_Tot_Corr", "SiPM_Vs_Tot", 32, 0.5, 32.5, fPWToTBins, fLeftPWToTLim, fRightPWToTLim);
    SiPMTimeCorr = R3B::root_owned<TH2F>(
        "SiPM_Time_Corr", "SiPM_Vs_Time", 32, 0.5, 32.5, fPWTimeBins, fLeftPWTimeLim, fRightPWTimeLim);

    // Cal Histograms
    for (Int_t i = 0; i < 16; i++)
    {
        TString name = Form("bar_Cal_Time_Corr_%i", i + 1);
        SiPMCalTimeCorr[i] = R3B::root_owned<TH2F>(
            name, name, fPWTimeBins, fLeftPWTimeLim, fRightPWTimeLim, fPWTimeBins, fLeftPWTimeLim, fRightPWTimeLim);
        TString name1 = Form("bar_Cal_ToT_Corr_%i", i + 1);
        SiPMCalTotCorr[i] = R3B::root_owned<TH2F>(
            name1, name1, fPWToTBins, fLeftPWToTLim, fRightPWToTLim, fPWToTBins, fLeftPWToTLim, fRightPWToTLim);
    }

    // ------------- HIT Histograms ------------
    ToTBarHitCorr =
        R3B::root_owned<TH2F>("ToT_Bar_Hit_Corr", "Bar_Vs_ToT", 18, 0, 18, fPWToTBins, fLeftPWToTLim, fRightPWToTLim);
    TimeBarHitCorr = R3B::root_owned<TH2F>(
        "Time_Bar_Hit_Corr", "Bar_Vs_Time", 18, 0, 18, fPWTimeBins, fLeftPWTimeLim, fRightPWTimeLim);
    ToTPosHitCorr =
        R3B::root_owned<TH2F>("ToT_Pos_Hit_Corr", "Pos_Vs_ToT", 80, -10, 36, fPWToTBins, fLeftPWToTLim, fRightPWToTLim);
    TimePosHitCorr = R3B::root_owned<TH2F>(
        "Time_Pos_Hit_Corr", "Pos_Vs_Time", 80, -10, 36, fPWTimeBins, fLeftPWTimeLim, fRightPWTimeLim);

    BarHitHisto = R3B::root_owned<TH1F>("Bar_Hist", "Bar_number", 18, 0, 18);
    NhitsHisto = R3B::root_owned<TH1F>("Nhits_Hist", "Number_of_hits", 50, 0, 50);
    NbarsHisto = R3B::root_owned<TH1F>("Nbars_Hist", "Number_of_bars", 18, 0, 18);
    BarVsHits = R3B::root_owned<TH2F>("Nhits_per_bar", "Number_of_hits_per_bar", 18, 0, 18, 10, 0, 10);

    SiPMChannelsHisto->GetXaxis()->SetTitle("Padiwa Channel No.");
    SiPMChannelsHisto->GetYaxis()->SetTitle("Counts");
    PWChannelsCanvas->cd();
    SiPMChannelsHisto->Draw("HIST");
    mapfol->Add(PWChannelsCanvas);

    refChannelsHisto->GetXaxis()->SetTitle("Ref. Channel No.");
    refChannelsHisto->GetYaxis()->SetTitle("Counts");
    RefChannelsCanvas->cd();
    refChannelsHisto->Draw("HIST");
    mapfol->Add(RefChannelsCanvas);

    SiPMTotCorr->GetXaxis()->SetTitle("Channel No.");
    SiPMTotCorr->GetYaxis()->SetTitle("ToT [ns]");
    PWTotCorrCanvas->cd();
    SiPMTotCorr->Draw("COLZ");
    preCalFolder->Add(PWTotCorrCanvas);

    SiPMTimeCorr->GetXaxis()->SetTitle("Channel No.");
    SiPMTimeCorr->GetYaxis()->SetTitle("Time [ns]");
    PWTimeCorrCanvas->cd();
    SiPMTimeCorr->Draw("COLZ");
    preCalFolder->Add(PWTimeCorrCanvas);

    for (int i = 0; i < 16; i++)
    {

        Int_t ii = i - 8;
        SiPMCalTimeCorr[i]->GetXaxis()->SetTitle("Time0");
        SiPMCalTimeCorr[i]->GetYaxis()->SetTitle("Time1");
        if (i < 8)
        {
            PWCalTimeCorrCanvas1->cd(i + 1);
        }
        else
            PWCalTimeCorrCanvas2->cd(ii + 1);
        SiPMCalTimeCorr[i]->Draw("COLZ");

        SiPMCalTotCorr[i]->GetXaxis()->SetTitle("ToT0");
        SiPMCalTotCorr[i]->GetYaxis()->SetTitle("ToT1");
        if (i < 8)
        {
            PWCalTotCorrCanvas1->cd(i + 1);
        }
        else
            PWCalTotCorrCanvas2->cd(ii + 1);
        SiPMCalTotCorr[i]->Draw("COLZ");
    }

    calFolder->Add(PWCalTimeCorrCanvas1);
    calFolder->Add(PWCalTimeCorrCanvas2);
    calFolder->Add(PWCalTotCorrCanvas1);
    calFolder->Add(PWCalTotCorrCanvas2);

    ToTBarHitCorr->GetXaxis()->SetTitle("Bar No.");
    ToTBarHitCorr->GetYaxis()->SetTitle("ToT [ns]");
    hitCorrCanvas->cd(1);
    ToTBarHitCorr->Draw("COLZ");

    TimeBarHitCorr->GetYaxis()->SetTitle("Time [ns]");
    TimeBarHitCorr->GetXaxis()->SetTitle("Bar No.");
    hitCorrCanvas->cd(2);
    TimeBarHitCorr->Draw("COLZ");

    ToTPosHitCorr->GetXaxis()->SetTitle("Z pos. [cm]");
    ToTPosHitCorr->GetYaxis()->SetTitle("ToT [ns]");
    hitCorrCanvas->cd(3);
    ToTPosHitCorr->Draw("COLZ");

    TimePosHitCorr->GetXaxis()->SetTitle("Z pos. [cm]");
    TimePosHitCorr->GetYaxis()->SetTitle("Time [ns]");
    hitCorrCanvas->cd(4);
    TimePosHitCorr->Draw("COLZ");

    hitFolder->Add(hitCorrCanvas);

    BarHitHisto->GetXaxis()->SetTitle("Bar No.");
    BarHitHisto->GetYaxis()->SetTitle("Counts");
    hitMultCanvas->cd(1);
    BarHitHisto->Draw("HIST");

    NhitsHisto->GetXaxis()->SetTitle("N_{hits}");
    NhitsHisto->GetYaxis()->SetTitle("Counts");
    hitMultCanvas->cd(2);
    NhitsHisto->Draw("HIST");

    NbarsHisto->GetXaxis()->SetTitle("N_{bars}");
    NbarsHisto->GetYaxis()->SetTitle("Counts");
    hitMultCanvas->cd(3);
    NbarsHisto->Draw("HIST");

    BarVsHits->GetXaxis()->SetTitle("Bar No.");
    BarVsHits->GetYaxis()->SetTitle("Multiplicity");
    hitMultCanvas->cd(4);
    BarVsHits->Draw("COLZ");

    hitFolder->Add(hitMultCanvas);

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);
    run->AddObject(mainfol);

    run->GetHttpServer()->RegisterCommand("Reset_PW", Form("/Objects/%s/->Reset_PW_Histo()", GetName()));

    return kSUCCESS;
}

void R3BPWOnlineSpectra::Reset_PW_Histo()
{

    SiPMChannelsHisto->Reset();
    refChannelsHisto->Reset();

    SiPMTotCorr->Reset();
    SiPMTimeCorr->Reset();

    for (Int_t i = 0; i < 16; i++)
    {
        SiPMCalTimeCorr[i]->Reset();
        SiPMCalTotCorr[i]->Reset();
    }

    ToTBarHitCorr->Reset();
    TimeBarHitCorr->Reset();
    ToTPosHitCorr->Reset();
    TimePosHitCorr->Reset();
    BarHitHisto->Reset();
    NhitsHisto->Reset();
    NbarsHisto->Reset();
    BarVsHits->Reset();

    return;
}

void R3BPWOnlineSpectra::Exec(Option_t* option)
{

    /* ------------------- Map EventLoop ------------------*/
    auto nMappedHits = fMappedDataItems->GetEntriesFast();
    for (Int_t ihit = 0; ihit < nMappedHits; ihit++)
    {

        R3BPWMappedData* hit = dynamic_cast<R3BPWMappedData*>(fMappedDataItems->At(ihit));

        if (!hit)
            continue;

        UInt_t idetector = hit->GetDetId();

        if (idetector == 0)
        {
            SiPMChannelsHisto->Fill(hit->GetChannelId());
        }

        if (idetector == 1)
        {
            refChannelsHisto->Fill(hit->GetChannelId());
        }
    }

    /* ----------------- PreCal EventLoop ------------------*/
    auto nPreCalHits = fPreCalDataItems->GetEntriesFast();

    for (Int_t ihit = 0; ihit < nPreCalHits; ihit++)
    {

        R3BPWPreCalData* hit = dynamic_cast<R3BPWPreCalData*>(fPreCalDataItems->At(ihit));

        if (hit->GetDetId() == 0)
        {
            SiPMTotCorr->Fill(hit->GetChannelId(), hit->GetTot());
            SiPMTimeCorr->Fill(hit->GetChannelId(), hit->GetTime());
        }
    }

    /* -------------------- Cal EventLoop ---------------------*/
    auto nCalHits = fCalDataItems->GetEntriesFast();

    for (Int_t ihit = 0; ihit < nCalHits; ihit++)
    {

        R3BPWCalData* hit = dynamic_cast<R3BPWCalData*>(fCalDataItems->At(ihit));

        Int_t ch = hit->GetChannelId();
        if (hit->GetDetId() == 0)
        {

            SiPMCalTimeCorr[ch - 1]->Fill(hit->GetTime0(), hit->GetTime1());
            SiPMCalTotCorr[ch - 1]->Fill(hit->GetTot0(), hit->GetTot1());
        }
    }

    //---------------- HIT EventLoop ---------------
    auto nHits = fHitDataItems->GetEntriesFast();

    Int_t channelId;
    Float_t pos, charge;
    Float_t time = 0.0;
    Int_t barMulArray[16] = { 0 };

    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {

        R3BPWHitData* hit = dynamic_cast<R3BPWHitData*>(fHitDataItems->At(ihit));

        channelId = hit->GetChannelId();
        pos = hit->GetPos();
        charge = hit->GetCharge();
        time = hit->GetTime();

        ToTBarHitCorr->Fill(channelId, charge);
        ToTPosHitCorr->Fill(pos, charge);
        TimeBarHitCorr->Fill(channelId, time);
        TimePosHitCorr->Fill(pos, time);

        BarHitHisto->Fill(channelId);
        barMulArray[channelId - 1] += 1;
    }

    NhitsHisto->Fill(nHits);

    Int_t bars = 0;
    for (Int_t b = 0; b < 16; b++)
    {
        if (barMulArray[b] > 0)
            bars++;
        BarVsHits->Fill(b + 1, barMulArray[b]);
    }

    NbarsHisto->Fill(bars);

    return;
}

void R3BPWOnlineSpectra::FinishEvent()
{

    if (fMappedDataItems)
    {
        fMappedDataItems->Clear();
    }

    if (fPreCalDataItems)
    {
        fPreCalDataItems->Clear();
    }

    if (fCalDataItems)
    {
        fCalDataItems->Clear();
    }

    if (fHitDataItems)
    {
        fHitDataItems->Clear();
    }
}

ClassImp(R3BPWOnlineSpectra);
