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

#include "R3BDMOnlineSpectra.h"
#include "R3BDMCalData.h"
#include "R3BDMHitData.h"
#include "R3BDMMappedData.h"
#include "R3BLogger.h"
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

R3BDMOnlineSpectra::R3BDMOnlineSpectra()
    : R3BDMOnlineSpectra("DMOnlineSpectra", 1)
{
}

R3BDMOnlineSpectra::R3BDMOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
}

InitStatus R3BDMOnlineSpectra::Init()
{
    R3BLOG(info, "");

    // Looking for FairRootManager
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, NULL == mgr, "FairRootManager not found");

    // Get access to Mapped data
    fMappedDataItems = dynamic_cast<TClonesArray*>(mgr->GetObject("R3BDMMappedData"));
    if (!fMappedDataItems)
    {
        R3BLOG(fatal, "R3BDMMappedData not found");
        return kFATAL;
    }

    fCalDataItems = dynamic_cast<TClonesArray*>(mgr->GetObject("R3BDMCalData"));
    if (!fCalDataItems)
    {
        R3BLOG(fatal, "R3BDMCalData not found");
        return kFATAL;
    }

    fHitDataItems = dynamic_cast<TClonesArray*>(mgr->GetObject("R3BDMHitData"));
    if (!fHitDataItems)
    {
        R3BLOG(fatal, "R3BDMHitData not found");
        return kFATAL;
    }

    /* ------------- FOLDERS --------------*/
    TFolder* mainfol = new TFolder("DM", "DM_info");

    // Folder for mapped data
    TFolder* mapfol = new TFolder("Map", "Map_DM_info");
    mainfol->Add(mapfol);

    // Folder for Cal Data
    TFolder* calFolder = new TFolder("Cal", "Cal_DM_info");
    mainfol->Add(calFolder);

    // Folder for Hit Data
    TFolder* hitFolder = new TFolder("Hit", "Hit_DM_info");
    mainfol->Add(hitFolder);

    /* ----------- CANVASES ------------- */

    // Map canvases
    DMChannelsCanvas = new TCanvas("TDCChannels", "tdc_channels");
    RefChannelsCanvas = new TCanvas("RefChannels", "ref_channels");

    // ----- Cal Canvases -----
    DM_ChTotCorrCanvas = new TCanvas("Bar_Time_vs_Time1", "bar_Time_vs_Time1");
    DM_ChTimeCorrCanvas = new TCanvas("Bar_Time_vs_Time2", "bar_Time_vs_Time2");
    DM_ChTotCorrCanvas->Divide(2, 2);
    DM_ChTimeCorrCanvas->Divide(2, 2);

    // ------ Hit Canvases ------
    hitCorrCanvas = new TCanvas("Hit_ToT_Time", "hit_ToT_Time_Canvas");
    hitCorrCanvas->Divide(2, 2);

    hitPadCanvas1 = new TCanvas("Hit_Pad_Canvas1", "hit_Pad_Canvas1");
    hitPadCanvas1->Divide(2, 2);

    hitPadCanvas2 = new TCanvas("Hit_Pad_Canvas2", "hit_Pad_Canvas2");
    hitPadCanvas2->Divide(2, 2);

    // ----- Map Histograms -----
    TDCChannelsHisto = R3B::root_owned<TH1F>("tdc_channels", "tdc_channels", 72, 0.5, 72.5);
    refChannelsHisto = R3B::root_owned<TH1F>("ref_channels", "ref_channels", 4, 0, 4);

    // Cal Histograms
    for (Int_t i = 0; i < 4; i++)
    {
        TString name = Form("Ch_Cal_ToT_Corr_%i", i);
        ChCalTotCorr[i] = R3B::root_owned<TH2F>(name, name, 32, 0.5, 32.5, fDMToTBins, fLeftDMToTLim, fRightDMToTLim);
        TString name1 = Form("Ch_Cal_Time_Corr_%i", i);
        ChCalTimeCorr[i] =
            R3B::root_owned<TH2F>(name1, name1, 32, 0.5, 32.5, fDMTimeBins, fLeftDMTimeLim, fRightDMTimeLim);
    }

    // ------------- HIT Histograms ------------
    ToTPadHitCorr1 =
        R3B::root_owned<TH2F>("ToT_Pad_Hit_Corr1", "Pad_Vs_ToT1", 38, 0, 38, fDMToTBins, fLeftDMToTLim, fRightDMToTLim);
    TimePadHitCorr1 = R3B::root_owned<TH2F>(
        "Time_Pad_Hit_Corr1", "Pad_Vs_Time1", 38, 0, 38, fDMTimeBins, fLeftDMTimeLim, fRightDMTimeLim);
    ToTPadHitCorr2 =
        R3B::root_owned<TH2F>("ToT_Pad_Hit_Corr2", "Pad_Vs_ToT2", 38, 0, 38, fDMToTBins, fLeftDMToTLim, fRightDMToTLim);
    TimePadHitCorr2 = R3B::root_owned<TH2F>(
        "Time_Pad_Hit_Corr2", "Pad_Vs_Time2", 38, 0, 38, fDMTimeBins, fLeftDMTimeLim, fRightDMTimeLim);

    PadHitHisto1 = R3B::root_owned<TH1F>("Pad_Hist1", "Pad_number1", 38, 0, 38);
    PadHitsXY1 = R3B::root_owned<TH2F>("Pad_XY1", "Pad_XY1", 100, 0, 1, 100, 0, 1);
    NpadsHisto1 = R3B::root_owned<TH1F>("Npads_Hist1", "Number_of_pads1", 38, 0, 38);
    PadVsHits1 = R3B::root_owned<TH2F>("Nhits_per_pad1", "Number_of_hits_per_pad1", 38, 0, 38, 50, 0, 50);

    PadHitHisto2 = R3B::root_owned<TH1F>("Pad_Hist2", "Pad_number2", 38, 0, 38);
    PadHitsXY2 = R3B::root_owned<TH2F>("Pad_XY2", "Pad_XY2", 100, 0, 1, 100, 0, 1);
    NpadsHisto2 = R3B::root_owned<TH1F>("Npads_Hist2", "Number_of_pads2", 38, 0, 38);
    PadVsHits2 = R3B::root_owned<TH2F>("Nhits_per_pad2", "Number_of_hits_per_pad2", 38, 0, 38, 50, 0, 50);

    TDCChannelsHisto->GetXaxis()->SetTitle("TDC Channel");
    TDCChannelsHisto->GetYaxis()->SetTitle("Counts");
    DMChannelsCanvas->cd();
    TDCChannelsHisto->Draw("HIST");
    mapfol->Add(DMChannelsCanvas);

    refChannelsHisto->GetXaxis()->SetTitle("Ref. Channel");
    refChannelsHisto->GetYaxis()->SetTitle("Counts");
    RefChannelsCanvas->cd();
    refChannelsHisto->Draw("HIST");
    mapfol->Add(RefChannelsCanvas);

    for (int i = 0; i < 4; i++)
    {
        ChCalTotCorr[i]->GetXaxis()->SetTitle("TDC Channel");
        ChCalTotCorr[i]->GetYaxis()->SetTitle("ToT [ns]");
        DM_ChTotCorrCanvas->cd(i + 1);
        ChCalTotCorr[i]->Draw("COLZ");

        ChCalTimeCorr[i]->GetXaxis()->SetTitle("TDC Channel");
        ChCalTimeCorr[i]->GetYaxis()->SetTitle("Time [ns]");
        DM_ChTimeCorrCanvas->cd(i + 1);
        ChCalTimeCorr[i]->Draw("COLZ");
    }

    calFolder->Add(DM_ChTotCorrCanvas);
    calFolder->Add(DM_ChTimeCorrCanvas);

    ToTPadHitCorr1->GetXaxis()->SetTitle("Pad No.");
    ToTPadHitCorr1->GetYaxis()->SetTitle("ToT [ns]");
    hitCorrCanvas->cd(1);
    ToTPadHitCorr1->Draw("COLZ");

    TimePadHitCorr1->GetYaxis()->SetTitle("Time [ns]");
    TimePadHitCorr1->GetXaxis()->SetTitle("Pad No.");
    hitCorrCanvas->cd(2);
    TimePadHitCorr1->Draw("COLZ");

    ToTPadHitCorr2->GetXaxis()->SetTitle("Pad No.");
    ToTPadHitCorr2->GetYaxis()->SetTitle("ToT [ns]");
    hitCorrCanvas->cd(3);
    ToTPadHitCorr2->Draw("COLZ");

    TimePadHitCorr2->GetYaxis()->SetTitle("Time [ns]");
    TimePadHitCorr2->GetXaxis()->SetTitle("Pad No.");
    hitCorrCanvas->cd(4);
    TimePadHitCorr2->Draw("COLZ");

    hitFolder->Add(hitCorrCanvas);

    PadHitHisto1->GetXaxis()->SetTitle("Pad No.");
    PadHitHisto1->GetYaxis()->SetTitle("Counts");
    hitPadCanvas1->cd(1);
    PadHitHisto1->Draw("HIST");

    PadHitsXY1->GetXaxis()->SetTitle("Pos X [cm]");
    PadHitsXY1->GetYaxis()->SetTitle("Pos Y [cm]");
    hitPadCanvas1->cd(2);
    PadHitsXY1->Draw("COLZ");

    NpadsHisto1->GetXaxis()->SetTitle("N_{pads}");
    NpadsHisto1->GetYaxis()->SetTitle("Counts");
    hitPadCanvas1->cd(3);
    NpadsHisto1->Draw("HIST");

    PadVsHits1->GetXaxis()->SetTitle("Pad No.");
    PadVsHits1->GetYaxis()->SetTitle("Multiplicity");
    hitPadCanvas1->cd(4);
    PadVsHits1->Draw("COLZ");

    hitFolder->Add(hitPadCanvas1);

    PadHitHisto2->GetXaxis()->SetTitle("Pad No.");
    PadHitHisto2->GetYaxis()->SetTitle("Counts");
    hitPadCanvas2->cd(1);
    PadHitHisto2->Draw("HIST");

    PadHitsXY2->GetXaxis()->SetTitle("Pos X [cm]");
    PadHitsXY2->GetYaxis()->SetTitle("Pos Y [cm]");
    hitPadCanvas2->cd(2);
    PadHitsXY2->Draw("COLZ");

    NpadsHisto2->GetXaxis()->SetTitle("N_{pads}");
    NpadsHisto2->GetYaxis()->SetTitle("Counts");
    hitPadCanvas2->cd(3);
    NpadsHisto2->Draw("HIST");

    PadVsHits2->GetXaxis()->SetTitle("Pad No.");
    PadVsHits2->GetYaxis()->SetTitle("Multiplicity");
    hitPadCanvas2->cd(4);
    PadVsHits2->Draw("COLZ");

    hitFolder->Add(hitPadCanvas2);

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);
    run->AddObject(mainfol);

    run->GetHttpServer()->RegisterCommand("Reset_DM", Form("/Objects/%s/->Reset_DM_Histo()", GetName()));

    return kSUCCESS;
}

void R3BDMOnlineSpectra::Reset_DM_Histo()
{

    TDCChannelsHisto->Reset();
    refChannelsHisto->Reset();

    for (Int_t i = 0; i < 4; i++)
    {
        ChCalTotCorr[i]->Reset();
        ChCalTimeCorr[i]->Reset();
    }

    ToTPadHitCorr1->Reset();
    TimePadHitCorr1->Reset();
    ToTPadHitCorr2->Reset();
    TimePadHitCorr2->Reset();
    PadHitHisto1->Reset();
    PadHitsXY1->Reset();
    NpadsHisto1->Reset();
    PadVsHits1->Reset();
    PadHitHisto2->Reset();
    PadHitsXY2->Reset();
    NpadsHisto2->Reset();
    PadVsHits2->Reset();

    return;
}

void R3BDMOnlineSpectra::Exec(Option_t* option)
{

    /* ------------------- Map EventLoop ------------------*/
    auto nMappedHits = fMappedDataItems->GetEntriesFast();
    for (Int_t ihit = 0; ihit < nMappedHits; ihit++)
    {

        R3BDMMappedData* hit = dynamic_cast<R3BDMMappedData*>(fMappedDataItems->At(ihit));

        if (!hit)
            continue;

        UInt_t idetector = hit->GetDetId();

        if (idetector > 0)
        {
            TDCChannelsHisto->Fill(hit->GetChannelId());
        }

        if (idetector == 0)
        {
            refChannelsHisto->Fill(hit->GetTDC());
        }
    }

    /* -------------------- Cal EventLoop ---------------------*/
    auto nCalHits = fCalDataItems->GetEntriesFast();

    for (Int_t ihit = 0; ihit < nCalHits; ihit++)
    {

        R3BDMCalData* hit = dynamic_cast<R3BDMCalData*>(fCalDataItems->At(ihit));

        if (hit->GetDetId() == 0)
            continue;

        Int_t TDC0[18] = { 1, 2, 3, 4, 5, 6, 13, 14, 15, 17, 18, 19, 20, 21, 22, 29, 30, 31 };
        Int_t TDC1[18] = { 2, 3, 4, 11, 12, 13, 14, 15, 16, 18, 19, 20, 27, 28, 29, 30, 31, 32 };
        Int_t TDC2[18] = { 1, 2, 3, 4, 5, 6, 13, 14, 15, 17, 18, 19, 20, 21, 22, 29, 30, 31 };
        Int_t TDC3[18] = { 2, 3, 4, 11, 12, 13, 14, 15, 16, 18, 19, 20, 27, 28, 29, 30, 31, 32 };

        Int_t ch = hit->GetChannelId() - 1;
        Int_t tdc = hit->GetTDC();
        Int_t tdc_ch = 0;
        if (tdc == 0)
            tdc_ch = TDC0[ch];
        if (tdc == 1)
            tdc_ch = TDC1[ch - 18];
        if (tdc == 2)
            tdc_ch = TDC2[ch - 36];
        if (tdc == 3)
            tdc_ch = TDC3[ch - 54];

        ChCalTotCorr[tdc]->Fill(tdc_ch, hit->GetTot());
        ChCalTimeCorr[tdc]->Fill(tdc_ch, hit->GetTime());
    }

    //---------------- HIT EventLoop ---------------
    auto nHits = fHitDataItems->GetEntriesFast();

    Int_t pad, det;
    Double_t pos_x, pos_y, charge;
    Double_t time = 0.0;
    Int_t padMulArray1[36] = { 0 };
    Int_t padMulArray2[36] = { 0 };

    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
        R3BDMHitData* hit = dynamic_cast<R3BDMHitData*>(fHitDataItems->At(ihit));

        det = hit->GetDetId();
        pad = hit->GetPad();
        pos_x = hit->GetPosX();
        pos_y = hit->GetPosY();
        charge = hit->GetCharge();
        time = hit->GetTime();

        if (det == 1)
        {
            ToTPadHitCorr1->Fill(pad, charge);
            TimePadHitCorr1->Fill(pad, time);
            PadHitHisto1->Fill(pad);
            PadHitsXY1->Fill(pos_x, pos_y);
            padMulArray1[pad - 1] += 1;
        }
        if (det == 2)
        {
            ToTPadHitCorr2->Fill(pad, charge);
            TimePadHitCorr2->Fill(pad, time);
            PadHitHisto2->Fill(pad);
            PadHitsXY2->Fill(pos_x, pos_y);
            padMulArray2[pad - 1] += 1;
        }
    }

    Int_t pads1 = 0;
    Int_t pads2 = 0;
    for (Int_t b = 0; b < 36; b++)
    {
        if (padMulArray1[b] > 0)
            pads1++;
        if (padMulArray2[b] > 0)
            pads2++;
        PadVsHits1->Fill(b + 1, padMulArray1[b]);
        PadVsHits2->Fill(b + 1, padMulArray2[b]);
    }

    NpadsHisto1->Fill(pads1);
    NpadsHisto2->Fill(pads1);

    return;
}

void R3BDMOnlineSpectra::FinishEvent()
{

    if (fMappedDataItems)
    {
        fMappedDataItems->Clear();
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

ClassImp(R3BDMOnlineSpectra);
