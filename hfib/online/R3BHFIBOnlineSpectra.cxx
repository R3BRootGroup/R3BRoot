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

#include "R3BHFIBOnlineSpectra.h"
#include "R3BHFIBCalData.h"
#include "R3BHFIBHitData.h"
#include "R3BHFIBMappedData.h"
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

#include "TClonesArray.h"
#include "THttpServer.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

R3BHFIBOnlineSpectra::R3BHFIBOnlineSpectra()
    : R3BHFIBOnlineSpectra("HFIBOnlineSpectra", 1)
{
}

R3BHFIBOnlineSpectra::R3BHFIBOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
}

InitStatus R3BHFIBOnlineSpectra::Init()
{
    R3BLOG(info, "");

    // Looking for FairRootManager
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, NULL == mgr, "FairRootManager not found");

    // Get access to Mapped data
    fMappedDataItems = dynamic_cast<TClonesArray*>(mgr->GetObject("R3BHFIBMappedData"));
    if (!fMappedDataItems)
    {
        R3BLOG(fatal, "HFIBMappedData not found");
        return kFATAL;
    }

    fCalDataItems = dynamic_cast<TClonesArray*>(mgr->GetObject("R3BHFIBCalData"));
    if (!fCalDataItems)
    {
        R3BLOG(fatal, "R3BHFIBCalData not found");
        return kFATAL;
    }

    fHitDataItems = dynamic_cast<TClonesArray*>(mgr->GetObject("R3BHFIBHitData"));
    if (!fHitDataItems)
    {
        R3BLOG(fatal, "HFIBHitData not found");
        return kFATAL;
    }

    char name[100];

    /* ------------- FOLDERS --------------*/
    TFolder* mainfol = new TFolder("HFIB", "HFIB_info");

    // Folder for mapped data
    TFolder* mapfol = new TFolder("Map", "Map_HFIB_info");
    mainfol->Add(mapfol);

    // Folder for Cal Data
    TFolder* calFolder = new TFolder("Cal", "Cal_HFIB_info");
    mainfol->Add(calFolder);

    // Folder for Hit Data
    TFolder* hitFolder = new TFolder("Hit", "Hit_HFIB_info");
    mainfol->Add(hitFolder);

    /* ----------- CANVASES ------------- */
    // Map canvases
    Fib1_FaRICHChannelsCanvas = new TCanvas("Fib1_FaRICH_Channels", "Fib1_FaRICHChannelsCanvas");
    Fib1_FaRICHChannelsCanvas->Divide(4, 2);
    refChannelsCanvas = new TCanvas("RefChannels", "refChannelsCanvas");
    refChannelsCanvas->Divide(3, 2);

    // ----- Cal Canvases -----
    Fib1_ChTotCorrCanvas = new TCanvas("Ch_vs_Tot", "ch_vs_tot");
    Fib1_ChTimeCorrCanvas = new TCanvas("Ch_vs_Time", "ch_vs_time");
    Fib1_ChTotCorrCanvas->Divide(4, 2);
    Fib1_ChTimeCorrCanvas->Divide(4, 2);

    // ------ Hit Canvases ------
    hitTimeCanvas = new TCanvas("Hit_Time_Plane", "hit_time_plane_canvas");
    hitTimeCanvas->Divide(3, 2);

    hitToTCanvas = new TCanvas("Hit_ToT_Plane", "hit_tot_plane_canvas");
    hitToTCanvas->Divide(3, 2);

    Fib1_hitMultCanvas = new TCanvas("Fib1_Hit_Mult_Canvas", "fib1_hit_mult_Canvas");
    Fib1_hitMultCanvas->Divide(2, 2);

    // ----- Map Histograms -----
    for (Int_t i = 0; i < 8; i++)
    {
        sprintf(name, "farich1_cahnnels_tdc_%i", i + 1);
        Fib1_FaRICHChannelsHisto[i] = R3B::root_owned<TH1F>(name, name, 32, 0.5, 32.5);
    }
    for (Int_t i = 0; i < 6; i++)
    {
        sprintf(name, "ref_channels_fib_%i", i + 1);
        refChannelsHisto[i] = R3B::root_owned<TH1F>(name, name, 9, 0, 9);
    }

    // ----- Cal Histograms -----
    for (Int_t i = 0; i < 8; i++)
    {
        sprintf(name, "ch_vs_tot_tdc_%i", i + 1);
        Fib1_ChTotCorrHisto[i] =
            R3B::root_owned<TH2F>(name, name, 32, 0.5, 32.5, fHFIBToTBins, fLeftHFIBToTLim, fRightHFIBToTLim);
        sprintf(name, "ch_vs_time_tdc_%i", i + 1);
        Fib1_ChTimeCorrHisto[i] =
            R3B::root_owned<TH2F>(name, name, 32, 0.5, 32.5, fHFIBTimeBins, fLeftHFIBTimeLim, fRightHFIBTimeLim);
    }

    // ------------- HIT Histograms ------------
    for (Int_t i = 0; i < 6; i++)
    {
        sprintf(name, "fiber_vs_tot_det%i", i + 1);
        ToTFibHitCorr[i] =
            R3B::root_owned<TH2F>(name, name, 257, 0, 257, fHFIBToTBins, fLeftHFIBToTLim, fRightHFIBToTLim);
        sprintf(name, "fiber_vs_time_det%i", i + 1);
        TimeFibHitCorr[i] =
            R3B::root_owned<TH2F>(name, name, 257, 0, 257, fHFIBTimeBins, fLeftHFIBTimeLim, fRightHFIBTimeLim);
    }

    Fib1_FibHitHisto = R3B::root_owned<TH1F>("Fib1_Fiber_Hist", "Fiber_number", 257, 0, 257);
    Fib1_NhitsHisto = R3B::root_owned<TH1F>("Fib1_Nhits_Hist", "Number_of_hits", 100, 0, 100);
    Fib1_NfibHisto = R3B::root_owned<TH1F>("Fib1_Nfibers_Hist", "Number_of_fibers", 257, 0, 257);
    Fib1_FibVsHits = R3B::root_owned<TH2F>("Fib1_Nhits_per_fiber", "Number_of_hits_per_fiber", 257, 0, 257, 20, 0, 20);

    for (int i = 0; i < 8; i++)
    {
        Fib1_FaRICHChannelsCanvas->cd(i + 1);
        Fib1_FaRICHChannelsHisto[i]->GetXaxis()->SetTitle("Channel number");
        Fib1_FaRICHChannelsHisto[i]->GetYaxis()->SetTitle("Counts");
        Fib1_FaRICHChannelsHisto[i]->Draw("HIST");
    }
    mapfol->Add(Fib1_FaRICHChannelsCanvas);

    for (int i = 0; i < 6; i++)
    {
        refChannelsCanvas->cd(i + 1);
        refChannelsHisto[i]->GetXaxis()->SetTitle("ref channel number");
        refChannelsHisto[i]->GetYaxis()->SetTitle("Counts");
        refChannelsHisto[i]->Draw("HIST");
    }
    mapfol->Add(refChannelsCanvas);

    for (int i = 0; i < 8; i++)
    {
        Fib1_ChTotCorrCanvas->cd(i + 1);
        Fib1_ChTotCorrHisto[i]->GetXaxis()->SetTitle("Channel number");
        Fib1_ChTotCorrHisto[i]->GetYaxis()->SetTitle("ToT [ns]");
        Fib1_ChTotCorrHisto[i]->Draw("COLZ");
        Fib1_ChTimeCorrCanvas->cd(i + 1);
        Fib1_ChTimeCorrHisto[i]->GetXaxis()->SetTitle("Channel number");
        Fib1_ChTimeCorrHisto[i]->GetYaxis()->SetTitle("Time [ns]");
        Fib1_ChTimeCorrHisto[i]->Draw("COLZ");
    }

    calFolder->Add(Fib1_ChTotCorrCanvas);
    calFolder->Add(Fib1_ChTimeCorrCanvas);

    for (Int_t i = 0; i < 6; i++)
    {
        ToTFibHitCorr[i]->GetXaxis()->SetTitle("Fiber No.");
        ToTFibHitCorr[i]->GetYaxis()->SetTitle("ToT [ns]");
        TimeFibHitCorr[i]->GetYaxis()->SetTitle("Time [ns]");
        TimeFibHitCorr[i]->GetXaxis()->SetTitle("Fiber No.");
        hitToTCanvas->cd(i + 1);
        ToTFibHitCorr[i]->Draw("COLZ");
        hitTimeCanvas->cd(i + 1);
        TimeFibHitCorr[i]->Draw("COLZ");
    }

    hitFolder->Add(hitToTCanvas);
    hitFolder->Add(hitTimeCanvas);

    Fib1_FibHitHisto->GetXaxis()->SetTitle("Fiber No.");
    Fib1_FibHitHisto->GetYaxis()->SetTitle("Counts");
    Fib1_hitMultCanvas->cd(1);
    Fib1_FibHitHisto->Draw("HIST");

    Fib1_NhitsHisto->GetXaxis()->SetTitle("N_{hits}");
    Fib1_NhitsHisto->GetYaxis()->SetTitle("Counts");
    Fib1_hitMultCanvas->cd(2);
    Fib1_NhitsHisto->Draw("HIST");

    Fib1_NfibHisto->GetXaxis()->SetTitle("N_{fibers}");
    Fib1_NfibHisto->GetYaxis()->SetTitle("Counts");
    Fib1_hitMultCanvas->cd(3);
    Fib1_NfibHisto->Draw("HIST");

    Fib1_FibVsHits->GetXaxis()->SetTitle("Fiber No.");
    Fib1_FibVsHits->GetYaxis()->SetTitle("Multiplicity");
    Fib1_hitMultCanvas->cd(4);
    Fib1_FibVsHits->Draw("COLZ");

    hitFolder->Add(Fib1_hitMultCanvas);

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);
    run->AddObject(mainfol);

    run->GetHttpServer()->RegisterCommand("Reset_HFIB", Form("/Objects/%s/->Reset_PW_Histo()", GetName()));

    return kSUCCESS;
}

void R3BHFIBOnlineSpectra::Reset_HFIB_Histo()
{

    for (Int_t i = 0; i < 8; i++)
    {
        Fib1_FaRICHChannelsHisto[i]->Reset();
    }

    for (Int_t i = 0; i < 6; i++)
    {
        refChannelsHisto[i]->Reset();
    }

    for (Int_t i = 0; i < 8; i++)
    {
        Fib1_ChTimeCorrHisto[i]->Reset();
        Fib1_ChTotCorrHisto[i]->Reset();
    }

    for (Int_t i = 0; i < 6; i++)
    {
        ToTFibHitCorr[i]->Reset();
        TimeFibHitCorr[i]->Reset();
    }

    Fib1_FibHitHisto->Reset();
    Fib1_NhitsHisto->Reset();
    Fib1_NfibHisto->Reset();
    Fib1_FibVsHits->Reset();

    return;
}

void R3BHFIBOnlineSpectra::Exec(Option_t* option)
{

    /* ------------------- Map EventLoop ------------------*/
    auto nMappedHits = fMappedDataItems->GetEntriesFast();
    for (Int_t ihit = 0; ihit < nMappedHits; ihit++)
    {

        R3BHFIBMappedData* hit = dynamic_cast<R3BHFIBMappedData*>(fMappedDataItems->At(ihit));

        if (!hit)
            continue;

        UInt_t idetector = hit->GetDetId();

        if (idetector == 0)
        {
            refChannelsHisto[0]->Fill(hit->GetChannelId());
        }

        if (idetector == 1)
        {
            UInt_t mtdc = hit->GetTDC();
            Fib1_FaRICHChannelsHisto[mtdc - 1]->Fill(hit->GetChannelId());
        }
    }

    /* ----------------- Cal EventLoop ------------------*/
    auto nCalHits = fCalDataItems->GetEntriesFast();

    for (Int_t ihit = 0; ihit < nCalHits; ihit++)
    {

        R3BHFIBCalData* hit = dynamic_cast<R3BHFIBCalData*>(fCalDataItems->At(ihit));

        if (hit->GetDetId() == 1)
        {
            UInt_t ctdc = hit->GetTDC();
            Fib1_ChTotCorrHisto[ctdc - 1]->Fill(hit->GetChannelId(), hit->GetTot());
            Fib1_ChTimeCorrHisto[ctdc - 1]->Fill(hit->GetChannelId(), hit->GetTime());
        }
    }

    //---------------- HIT EventLoop ---------------
    auto nHits = fHitDataItems->GetEntriesFast();

    Int_t channelId, detectorId;
    Float_t pos, charge;
    Float_t time = 0.;
    Int_t fibMulArray[256] = { 0 };

    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {

        R3BHFIBHitData* hit = dynamic_cast<R3BHFIBHitData*>(fHitDataItems->At(ihit));

        channelId = hit->GetFiberId();
        detectorId = hit->GetDetId();
        charge = hit->GetCharge();
        time = hit->GetTime();

        ToTFibHitCorr[detectorId - 1]->Fill(channelId, charge);
        TimeFibHitCorr[detectorId - 1]->Fill(channelId, time);

        if (detectorId == 1 && channelId > 0)
        {
            Fib1_FibHitHisto->Fill(channelId);
            fibMulArray[channelId - 1] += 1;
        }
    }

    Fib1_NhitsHisto->Fill(nHits);

    Int_t fibers = 0;
    for (Int_t b = 0; b < 256; b++)
    {
        if (fibMulArray[b] > 0)
            fibers++;
        Fib1_FibVsHits->Fill(b + 1, fibMulArray[b]);
    }

    Fib1_NfibHisto->Fill(fibers);

    return;
}

void R3BHFIBOnlineSpectra::FinishEvent()
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

ClassImp(R3BHFIBOnlineSpectra);
