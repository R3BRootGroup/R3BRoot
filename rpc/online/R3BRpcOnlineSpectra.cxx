/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -----------------------------------------------------------
// -----             R3BRpcOnlineSpectra                 -----
// -----   Created 23/01/22 by J.L. Rodriguez-Sanchez    -----
// -----         Fill RPC online histograms              -----
// -----------------------------------------------------------

/*
 * This task should fill histograms with RPC online data
 */

#include "R3BRpcOnlineSpectra.h"
#include "R3BLogger.h"
#include "R3BEventHeader.h"
#include "R3BRpcCalData.h"
#include "R3BRpcHitData.h"
#include "R3BRpcMappedData.h"

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

R3BRpcOnlineSpectra::R3BRpcOnlineSpectra()
    : R3BRpcOnlineSpectra("RpcOnlineSpectra", 1)
{
}

R3BRpcOnlineSpectra::R3BRpcOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fEventHeader(nullptr)
    , fMappedItems(NULL)
    , fCalItems(NULL)
    , fHitItems(NULL)
    , fTrigger(-1)
    , fNEvents(0)
    , fNbDet(1)
{
}

R3BRpcOnlineSpectra::~R3BRpcOnlineSpectra()
{
    R3BLOG(DEBUG1, "");
    if (fEventHeader)
        delete fEventHeader;
    if (fMappedItems)
        delete fMappedItems;
    if (fCalItems)
        delete fCalItems;
    if (fHitItems)
        delete fHitItems;
}

InitStatus R3BRpcOnlineSpectra::Init()
{
    R3BLOG(INFO, "");

    // Looking for FairRootManager
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(FATAL, NULL == mgr, "FairRootManager not found");

    // Look for the R3BEventHeader
    fEventHeader = (R3BEventHeader*)mgr->GetObject("EventHeader.");
    if (!fEventHeader)
    {
        R3BLOG(WARNING, "EventHeader. not found");
        fEventHeader = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    }
    else
        R3BLOG(INFO,"EventHeader. found");

    // Get access to Mapped data
    fMappedItems = (TClonesArray*)mgr->GetObject("RpcMappedData");
    if (!fMappedItems)
    {
        R3BLOG(FATAL, "RpcMappedData not found");
        return kFATAL;
    }

    // Get access to Cal data
    fCalItems = (TClonesArray*)mgr->GetObject("RpcCalData");
    R3BLOG_IF(WARNING, !fCalItems,"RpcCalData not found");

    // Get access to Hit data
    fHitItems = (TClonesArray*)mgr->GetObject("RpcHitData");
    R3BLOG_IF(WARNING, !fHitItems, "RpcHitData not found");

    // Create histograms for all the detectors

    // Energy range for strips
    Double_t binsE = 5000;
    Double_t minE = 0;
    Double_t maxE = 20000;

    char Name1[255];
    char Name2[255];

    // MAIN FOLDER-RPC
    TFolder* mainfol = new TFolder("Rpc", "RPC info");
    // Folder for mapped data
    TFolder* mapfol = new TFolder("Map", "Map RPC info");
    mainfol->Add(mapfol);
    // Folder for cal data
    TFolder* calfol = new TFolder("Cal", "Cal RPC info");
    // Folder for hit data
    TFolder* hitfol = new TFolder("Hit", "Hit RPC info");

    // Mapped data
    fh2_EnergyVsStrip.resize(fNbDet);
    for (Int_t i = 0; i < fNbDet; i++)
    { // one histo per detector
        sprintf(Name1, "fh2_energy_vs_strip_rpc_%d", i + 1);
        sprintf(Name2, "Mapped energy vs strip number for RPC Det: %d", i + 1);
        fh2_EnergyVsStrip[i] = new TH2F(Name1, Name2, 640, 1, 641, binsE, minE, maxE);
        fh2_EnergyVsStrip[i]->GetXaxis()->SetTitle("Strip number");
        fh2_EnergyVsStrip[i]->GetYaxis()->SetTitle("Energy [channels]");
        fh2_EnergyVsStrip[i]->GetYaxis()->SetTitleOffset(1.4);
        fh2_EnergyVsStrip[i]->GetXaxis()->CenterTitle(true);
        fh2_EnergyVsStrip[i]->GetYaxis()->CenterTitle(true);
        fh2_EnergyVsStrip[i]->Draw("col");
        mapfol->Add(fh2_EnergyVsStrip[i]);
    }

    // Cal data
    if (fCalItems)
    {
        fh2_EnergyVsStrip_cal.resize(fNbDet);
        for (Int_t i = 0; i < fNbDet; i++)
        { // one histo per detector
            sprintf(Name1, "fh2_energy_vs_strip_cal_rpc_%d", i + 1);
            sprintf(Name2, "Cal-energy vs strip number for RPC Det: %d", i + 1);
            fh2_EnergyVsStrip_cal[i] = new TH2F(Name1, Name2, 640, 1, 641, binsE, minE, maxE);
            fh2_EnergyVsStrip_cal[i]->GetXaxis()->SetTitle("Strip number");
            fh2_EnergyVsStrip_cal[i]->GetYaxis()->SetTitle("Energy [channels]");
            fh2_EnergyVsStrip_cal[i]->GetYaxis()->SetTitleOffset(1.4);
            fh2_EnergyVsStrip_cal[i]->GetXaxis()->CenterTitle(true);
            fh2_EnergyVsStrip_cal[i]->GetYaxis()->CenterTitle(true);
            fh2_EnergyVsStrip_cal[i]->Draw("col");
            calfol->Add(fh2_EnergyVsStrip_cal[i]);
        }
    }

    // Hit data
    if (fHitItems)
    {
        fh1_pos.resize(fNbDet);
        for (Int_t i = 0; i < fNbDet; i++)
        { // one histo per detector
            sprintf(Name1, "fh1_pos_rpc_%d", i + 1);
            sprintf(Name2, "Cluster position for RPC Det: %d", i + 1);
            fh1_pos[i] = new TH1F(Name1, Name2, 600, -50., 50.);
            fh1_pos[i]->GetXaxis()->SetTitle("Position [mm]");
            fh1_pos[i]->GetYaxis()->SetTitle("Counts");
            fh1_pos[i]->GetYaxis()->SetTitleOffset(1.4);
            fh1_pos[i]->GetXaxis()->CenterTitle(true);
            fh1_pos[i]->GetYaxis()->CenterTitle(true);
            fh1_pos[i]->Draw("");
            hitfol->Add(fh1_pos[i]);
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
    run->GetHttpServer()->RegisterCommand("Reset_RPC", Form("/Objects/%s/->Reset_RPC_Histo()", GetName()));

    return kSUCCESS;
}

void R3BRpcOnlineSpectra::Reset_RPC_Histo()
{
    R3BLOG(INFO, "");

    // Mapped data
    for (Int_t i = 0; i < fNbDet; i++)
    {
        fh2_EnergyVsStrip[i]->Reset();
    }

    // Cal data
    if (fCalItems)
        for (Int_t i = 0; i < fNbDet; i++)
        {
            fh2_EnergyVsStrip_cal[i]->Reset();
        }

    // Hit data
    if (fHitItems)
        for (Int_t i = 0; i < fNbDet; i++)
        {
            fh1_pos[i]->Reset();
        }

    return;
}

void R3BRpcOnlineSpectra::Exec(Option_t* option)
{
    if (fEventHeader->GetTrigger() != fTrigger && fTrigger > -1)
        return;

    // Fill mapped data
    if (fMappedItems && fMappedItems->GetEntriesFast() > 0)
    {
        auto nHits = fMappedItems->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BRpcMappedData* hit = (R3BRpcMappedData*)fMappedItems->At(ihit);
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
            R3BRpcCalData* hit = (R3BRpcCalData*)fCalItems->At(ihit);
            if (!hit)
                continue;
            fh2_EnergyVsStrip_cal[hit->GetDetId() - 1]->Fill(hit->GetStripId(), hit->GetEnergy());
        }
    }

    // Fill hit data
    if (fHitItems && fHitItems->GetEntriesFast() > 0)
    {
        auto nHits = fHitItems->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BRpcHitData* hit = (R3BRpcHitData*)fHitItems->At(ihit);
            if (!hit)
                continue;
            fh1_pos[hit->GetDetId() - 1]->Fill(hit->GetPos());
        }
    }

    fNEvents += 1;
    return;
}

void R3BRpcOnlineSpectra::FinishEvent()
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
    return;
}

void R3BRpcOnlineSpectra::FinishTask()
{
    if (fMappedItems)
    {
        for (Int_t i = 0; i < fNbDet; i++)
            fh2_EnergyVsStrip[i]->Write();
    }
    if (fCalItems)
        for (Int_t i = 0; i < fNbDet; i++)
        {
            fh2_EnergyVsStrip_cal[i]->Write();
        }

    if (fHitItems)
        for (Int_t i = 0; i < fNbDet; i++)
        {
            fh1_pos[i]->Write();
        }
    return;
}

ClassImp(R3BRpcOnlineSpectra);
