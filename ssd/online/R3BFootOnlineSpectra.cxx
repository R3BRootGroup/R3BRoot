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

// ------------------------------------------------------------
// -----             R3BFootOnlineSpectra                 -----
// -----    Created 16/07/21 by J.L. Rodriguez-Sanchez    -----
// -----          Fill FOOT online histograms             -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with FOOT online data
 */

#include "R3BFootOnlineSpectra.h"
#include "R3BFootCalData.h"
#include "R3BFootHitData.h"
#include "R3BFootMappedData.h"
#include "R3BWRData.h"

#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "THttpServer.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TFolder.h"
#include "TH1F.h"
#include "TH2F.h"

R3BFootOnlineSpectra::R3BFootOnlineSpectra()
    : R3BFootOnlineSpectra("FootOnlineSpectra", 1)
{
}

R3BFootOnlineSpectra::R3BFootOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fEventHeader(nullptr)
    , fMappedItems(NULL)
    , fCalItems(NULL)
    , fHitItems(NULL)
    , fWRItems(NULL)
    , fWRItemsMaster(NULL)
    , fTrigger(-1)
    , fNEvents(0)
    , fNbDet(10)
{
}

R3BFootOnlineSpectra::~R3BFootOnlineSpectra()
{
    R3BLOG(DEBUG1, "Destructor");
    if (fEventHeader)
        delete fEventHeader;
    if (fMappedItems)
        delete fMappedItems;
    if (fCalItems)
        delete fCalItems;
    if (fHitItems)
        delete fHitItems;
    if (fWRItems)
        delete fWRItems;
    if (fWRItemsMaster)
        delete fWRItemsMaster;
}

InitStatus R3BFootOnlineSpectra::Init()
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
        R3BLOG(INFO, "EventHeader. found");

    // Get access to Mapped data
    fMappedItems = (TClonesArray*)mgr->GetObject("FootMappedData");
    if (!fMappedItems)
    {
        R3BLOG(FATAL, "FootMappedData not found");
        return kFATAL;
    }

    // Get access to Cal data
    fCalItems = (TClonesArray*)mgr->GetObject("FootCalData");
    R3BLOG_IF(WARNING, !fCalItems, "FootCalData not found");

    // Get access to Hit data
    fHitItems = (TClonesArray*)mgr->GetObject("FootHitData");
    R3BLOG_IF(WARNING, !fHitItems, "FootHitData not found");

    // Get access to WR data
    fWRItems = (TClonesArray*)mgr->GetObject("WRFootData");
    R3BLOG_IF(WARNING, !fWRItems, "WRFootData not found");

    // get access to WR-Master data
    fWRItemsMaster = (TClonesArray*)mgr->GetObject("WRMasterData");
    R3BLOG_IF(WARNING, !fWRItemsMaster, "WRMasterData not found");

    // Create histograms for all the detectors

    // Energy range for strips
    Double_t binsE = 5000;
    Double_t minE = 0;
    Double_t maxE = 20000;

    char Name1[255];
    char Name2[255];

    // MAIN FOLDER-FOOT
    TFolder* mainfol = new TFolder("Foot", "Foot-Si info");
    // Folder for mapped data
    TFolder* mapfol = new TFolder("Map", "Map FOOT info");
    mainfol->Add(mapfol);
    // Folder for cal data
    TFolder* calfol = new TFolder("Cal", "Cal FOOT info");
    // Folder for hit data
    TFolder* hitfol = new TFolder("Hit", "Hit FOOT info");
    // Folder for WR data
    TFolder* wrfol = new TFolder("WRs", "WR FOOT info");

    // Mapped data
    fh2_EnergyVsStrip.resize(fNbDet);
    for (Int_t i = 0; i < fNbDet; i++)
    { // one histo per detector
        sprintf(Name1, "fh2_energy_vs_strip_det_%d", i + 1);
        sprintf(Name2, "Mapped energy vs strip number for FOOT Det: %d", i + 1);
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
            sprintf(Name1, "fh2_energy_vs_strip_cal_det_%d", i + 1);
            sprintf(Name2, "Cal-energy vs strip number for FOOT Det: %d", i + 1);
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
            sprintf(Name1, "fh1_pos_det_%d", i + 1);
            sprintf(Name2, "Cluster position for FOOT Det: %d", i + 1);
            fh1_pos[i] = new TH1F(Name1, Name2, 640, -50., 50.);
            fh1_pos[i]->GetXaxis()->SetTitle("Position [mm]");
            fh1_pos[i]->GetYaxis()->SetTitle("Counts");
            fh1_pos[i]->GetYaxis()->SetTitleOffset(1.4);
            fh1_pos[i]->GetXaxis()->CenterTitle(true);
            fh1_pos[i]->GetYaxis()->CenterTitle(true);
            fh1_pos[i]->Draw("");
            hitfol->Add(fh1_pos[i]);
        }

        auto cecor = new TCanvas("Energy_cor", "", 10, 10, 500, 500);
        sprintf(Name1, "fh1_ecor_det_%d_vs_%d", 5, 6);
        sprintf(Name2, "Cluster energy for FOOT Dets (in-beam): %d vs %d", 5, 6);
        fh2_ecor = new TH2F(Name1, Name2, binsE, minE, maxE, binsE, minE, maxE);
        fh2_ecor->GetXaxis()->SetTitle("Energy [channels]");
        fh2_ecor->GetYaxis()->SetTitle("Energy [channels]");
        fh2_ecor->GetYaxis()->SetTitleOffset(1.4);
        fh2_ecor->GetXaxis()->CenterTitle(true);
        fh2_ecor->GetYaxis()->CenterTitle(true);
        cecor->cd();
        fh2_ecor->Draw("colz");
        hitfol->Add(cecor);
    }

    // WR data
    if (fWRItems && fWRItemsMaster)
    {
        auto cwr = new TCanvas("Foot_wr", "", 10, 10, 500, 500);
        fh2_wr = new TH2F("fh2_wr", "WRFoot - WRMaster", 4000, -4000, 4000, 10, 0.5, 10.5);
        fh2_wr->GetXaxis()->SetTitle("WR difference [ns]");
        fh2_wr->GetYaxis()->SetTitle("FOOT Det");
        fh2_wr->GetYaxis()->SetTitleOffset(1.4);
        fh2_wr->GetXaxis()->CenterTitle(true);
        fh2_wr->GetYaxis()->CenterTitle(true);
        cwr->cd();
        fh2_wr->Draw("colz");
        wrfol->Add(cwr);
    }

    if (fCalItems)
    {
        mainfol->Add(calfol);
    }
    if (fHitItems)
    {
        mainfol->Add(hitfol);
    }
    if (fWRItems && fWRItemsMaster)
    {
        mainfol->Add(wrfol);
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
    R3BLOG(INFO, "Reset_Histo");

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
    {
        for (Int_t i = 0; i < fNbDet; i++)
        {
            fh1_pos[i]->Reset();
        }
        fh2_ecor->Reset();
    }

    // WR data
    if (fWRItems && fWRItemsMaster)
        fh2_wr->Reset();

    return;
}

void R3BFootOnlineSpectra::Exec(Option_t* option)
{
    if (fEventHeader->GetTrigger() != fTrigger && fTrigger > -1)
        return;

    // Fill mapped data
    if (fMappedItems && fMappedItems->GetEntriesFast() > 0)
    {
        auto nHits = fMappedItems->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BFootMappedData* hit = (R3BFootMappedData*)fMappedItems->At(ihit);
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
            R3BFootCalData* hit = (R3BFootCalData*)fCalItems->At(ihit);
            if (!hit)
                continue;
            fh2_EnergyVsStrip_cal[hit->GetDetId() - 1]->Fill(hit->GetStripId(), hit->GetEnergy());
        }
    }

    // Fill hit data
    if (fHitItems && fHitItems->GetEntriesFast() > 0)
    {
        auto nHits = fHitItems->GetEntriesFast();
        double e1 = 0., e2 = 0.;
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BFootHitData* hit = (R3BFootHitData*)fHitItems->At(ihit);
            if (!hit)
                continue;
            fh1_pos[hit->GetDetId() - 1]->Fill(hit->GetPos());
            if (hit->GetDetId() == 5)
                e1 = hit->GetEnergy();
            if (hit->GetDetId() == 6)
                e2 = hit->GetEnergy();
        }
        if (e1 > 0 && e2 > 0)
            fh2_ecor->Fill(e1, e2);
    }

    // Fill wr data
    if (fWRItems && fWRItems->GetEntriesFast() > 0 && fWRItemsMaster && fWRItemsMaster->GetEntriesFast() > 0)
    {
        int64_t wrm = 0.0;
        auto nHits = fWRItemsMaster->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hit = (R3BWRData*)fWRItemsMaster->At(ihit);
            if (!hit)
                continue;
            wrm = hit->GetTimeStamp();
        }

        nHits = fWRItems->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hit = (R3BWRData*)fWRItems->At(ihit);
            if (!hit)
                continue;
            fh2_wr->Fill(hit->GetTimeStamp() - wrm, hit->GetId());
        }
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
    if (fWRItems)
    {
        fWRItems->Clear();
    }
    if (fWRItemsMaster)
    {
        fWRItemsMaster->Clear();
    }
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
        for (Int_t i = 0; i < fNbDet; i++)
        {
            fh2_EnergyVsStrip_cal[i]->Write();
        }

    if (fHitItems)
    {
        for (Int_t i = 0; i < fNbDet; i++)
        {
            fh1_pos[i]->Write();
        }
        fh2_ecor->Write();
    }

    if (fWRItems && fWRItemsMaster)
        fh2_wr->Write();

    return;
}

ClassImp(R3BFootOnlineSpectra);
