/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
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
// -----             R3BAlpideOnlineSpectra               -----
// -----    Created 18/03/22 by J.L. Rodriguez-Sanchez    -----
// -----          Fill Alpide online histograms           -----
// ------------------------------------------------------------

// ROOT headers
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TFolder.h"
#include "TGaxis.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THttpServer.h"
#include "TMath.h"

// FAIR headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"

// R3B headers
#include "R3BAlpideCalData.h"
#include "R3BAlpideHitData.h"
#include "R3BAlpideMappedData.h"
#include "R3BAlpideMappingPar.h"
#include "R3BAlpideOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"

// R3BAlpideOnlineSpectra::Default Constructor --------------------------
R3BAlpideOnlineSpectra::R3BAlpideOnlineSpectra()
    : R3BAlpideOnlineSpectra("AlpideOnlineSpectra", 1)
{
}

// R3BAlpideOnlineSpectra::Standard Constructor --------------------------
R3BAlpideOnlineSpectra::R3BAlpideOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat1(-1)
    , fTpat2(-1)
    , fNEvents(0)
    , fNbSensors(1)
    , fMap_Par(NULL)
    , fMappedItems(NULL)
    , fCalItems(NULL)
    , fHitItems(NULL)
{
}

// Virtual R3BAlpideOnlineSpectra::Destructor
R3BAlpideOnlineSpectra::~R3BAlpideOnlineSpectra() { R3BLOG(debug, ""); }

void R3BAlpideOnlineSpectra::SetParContainers()
{
    // Parameter Container
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");

    fMap_Par = (R3BAlpideMappingPar*)rtdb->getContainer("alpideMappingPar");
    R3BLOG_IF(fatal, !fMap_Par, "Container alpideMappingPar not found");
}

void R3BAlpideOnlineSpectra::SetParameter()
{
    //--- Parameter Container ---
    if (fMap_Par)
    {
        fNbSensors = fMap_Par->GetNbSensors();
        R3BLOG(info, "Nb of sensors: " << fNbSensors);
    }
}

InitStatus R3BAlpideOnlineSpectra::Init()
{
    R3BLOG(info, "");
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, NULL == mgr, "FairRootManager not found");

    header = (R3BEventHeader*)mgr->GetObject("EventHeader.");
    if (!header)
    {
        R3BLOG(warn, "EventHeader. not found");
        header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    }
    else
    {
        R3BLOG(info, "EventHeader. found");
    }

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    fMappedItems = (TClonesArray*)mgr->GetObject("AlpideMappedData");
    R3BLOG_IF(fatal, NULL == fMappedItems, "AlpideMappedData not found");

    fCalItems = (TClonesArray*)mgr->GetObject("AlpideCalData");
    R3BLOG_IF(warn, NULL == fCalItems, "AlpideCalData not found");

    fHitItems = (TClonesArray*)mgr->GetObject("AlpideHitData");
    R3BLOG_IF(warn, NULL == fHitItems, "AlpideHitData not found");

    // MAIN FOLDER-ALPIDE
    TFolder* mainfol = new TFolder("ALPIDE", "Alpide info");
    // Folder for mapped data
    TFolder* mapfol = new TFolder("Map", "Map Alpide info");
    // Folder for cal data
    TFolder* calfol = new TFolder("Cal", "Cal Alpide info");
    // Folder for hit data
    TFolder* hitfol = new TFolder("Hit", "Hit Alpide info");

    //
    // Create histograms
    //
    SetParameter();

    char Name1[255];
    char Name2[255];
    fh2_ColVsRow.resize(fNbSensors);
    for (Int_t s = 0; s < fNbSensors; s++)
    {
        sprintf(Name1, "Sensor_%d_col_vs_row_map", s + 1);
        auto cMap = new TCanvas(Name1, "mapped info", 10, 10, 500, 500);
        sprintf(Name1, "fh2_col_vs_row_sensor_%d", s + 1);
        sprintf(Name2, "Mapped col vs row for sensor: %d", s + 1);
        fh2_ColVsRow[s] = new TH2F(Name1, Name2, 1024, 1, 1025, 512, 1, 513);
        fh2_ColVsRow[s]->GetXaxis()->SetTitle("Col");
        fh2_ColVsRow[s]->GetYaxis()->SetTitle("Row");
        fh2_ColVsRow[s]->GetYaxis()->SetTitleOffset(1.1);
        fh2_ColVsRow[s]->GetXaxis()->CenterTitle(true);
        fh2_ColVsRow[s]->GetYaxis()->CenterTitle(true);
        cMap->cd();
        fh2_ColVsRow[s]->Draw("colz");
        mapfol->Add(cMap);
    }
    mainfol->Add(mapfol);

    if (fCalItems)
    {
        fh2_ColVsRowCal.resize(fNbSensors);
        fh1_Calmult.resize(fNbSensors);
        for (Int_t s = 0; s < fNbSensors; s++)
        {
            sprintf(Name1, "Sensor_%d_col_vs_row_cal", s + 1);
            auto cCal = new TCanvas(Name1, "cal info", 10, 10, 500, 500);
            cCal->Divide(2, 1);
            sprintf(Name1, "fh2_col_vs_row_cal_sensor_%d", s + 1);
            sprintf(Name2, "Cal col vs row for sensor: %d", s + 1);
            fh2_ColVsRowCal[s] = new TH2F(Name1, Name2, 1024, 1, 1025, 512, 1, 513);
            fh2_ColVsRowCal[s]->GetXaxis()->SetTitle("Col");
            fh2_ColVsRowCal[s]->GetYaxis()->SetTitle("Row");
            fh2_ColVsRowCal[s]->GetYaxis()->SetTitleOffset(1.1);
            fh2_ColVsRowCal[s]->GetXaxis()->CenterTitle(true);
            fh2_ColVsRowCal[s]->GetYaxis()->CenterTitle(true);
            cCal->cd(1);
            fh2_ColVsRowCal[s]->Draw("colz");

            sprintf(Name1, "fh1_mulcal_sensor_%d", s + 1);
            sprintf(Name2, "Cal_mult for sensor: %d", s + 1);
            fh1_Calmult[s] = new TH1F(Name1, Name2, 1000, 0, 1000);
            fh1_Calmult[s]->GetXaxis()->SetTitle("Pixel multiplicity");
            fh1_Calmult[s]->GetYaxis()->SetTitle("Counts");
            fh1_Calmult[s]->GetYaxis()->SetTitleOffset(1.1);
            fh1_Calmult[s]->GetXaxis()->CenterTitle(true);
            fh1_Calmult[s]->GetYaxis()->CenterTitle(true);
            fh1_Calmult[s]->SetLineColor(1);
            fh1_Calmult[s]->SetFillColor(31);
            cCal->cd(2);
            fh1_Calmult[s]->Draw();
            calfol->Add(cCal);
        }
        mainfol->Add(calfol);
    }

    if (fHitItems)
    {
        fh1_Clustermult.resize(fNbSensors);
        fh1_Clustersize.resize(fNbSensors);
        fh2_PosHit.resize(fNbSensors);
        for (Int_t s = 0; s < fNbSensors; s++)
        {
            sprintf(Name1, "Cluster_size_sensor_%d", s + 1);
            auto cHit = new TCanvas(Name1, "cal info", 10, 10, 500, 500);
            cHit->Divide(2, 1);
            sprintf(Name1, "fh2_pos_hit_sensor_%d", s + 1);
            sprintf(Name2, "Hit-position for sensor: %d", s + 1);
            fh2_PosHit[s] = new TH2F(Name1, Name2, 600, -15.0, 15.0, 300, -7.5, 7.5);
            fh2_PosHit[s]->GetXaxis()->SetTitle("Posl [mm]");
            fh2_PosHit[s]->GetYaxis()->SetTitle("Post [mm]");
            fh2_PosHit[s]->GetYaxis()->SetTitleOffset(1.1);
            fh2_PosHit[s]->GetXaxis()->CenterTitle(true);
            fh2_PosHit[s]->GetYaxis()->CenterTitle(true);
            cHit->cd(1);
            fh2_PosHit[s]->Draw("colz");

            sprintf(Name1, "fh1_cluster_size_sensor_%d", s + 1);
            sprintf(Name2, "Cluster_size for sensor: %d", s + 1);
            fh1_Clustersize[s] = new TH1F(Name1, Name2, 160, 0, 160);
            fh1_Clustersize[s]->GetXaxis()->SetTitle("Cluster size [pixels]");
            fh1_Clustersize[s]->GetYaxis()->SetTitle("Counts");
            fh1_Clustersize[s]->GetYaxis()->SetTitleOffset(1.1);
            fh1_Clustersize[s]->GetXaxis()->CenterTitle(true);
            fh1_Clustersize[s]->GetYaxis()->CenterTitle(true);
            fh1_Clustersize[s]->SetLineColor(1);
            fh1_Clustersize[s]->SetFillColor(31);
            cHit->cd(2);
            fh1_Clustersize[s]->Draw();
            hitfol->Add(cHit);

            sprintf(Name1, "Cluster_multiplicity_sensor_%d", s + 1);
            auto cHitm = new TCanvas(Name1, "cal info", 10, 10, 500, 500);
            sprintf(Name1, "fh1_cluster_multiplicity_sensor_%d", s + 1);
            sprintf(Name2, "Cluster_multiplicity for sensor: %d", s + 1);
            fh1_Clustermult[s] = new TH1F(Name1, Name2, 60, 0, 60);
            fh1_Clustermult[s]->GetXaxis()->SetTitle("Cluster multiplicity");
            fh1_Clustermult[s]->GetYaxis()->SetTitle("Counts");
            fh1_Clustermult[s]->GetYaxis()->SetTitleOffset(1.1);
            fh1_Clustermult[s]->GetXaxis()->CenterTitle(true);
            fh1_Clustermult[s]->GetYaxis()->CenterTitle(true);
            fh1_Clustermult[s]->SetLineColor(1);
            fh1_Clustermult[s]->SetFillColor(31);
            cHitm->cd();
            fh1_Clustermult[s]->Draw();
            hitfol->Add(cHitm);
        }
        mainfol->Add(hitfol);
    }
    run->AddObject(mainfol);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_ALPIDE_HIST", Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BAlpideOnlineSpectra::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BAlpideOnlineSpectra::Reset_Histo()
{
    R3BLOG(info, "");

    if (fMappedItems)
    {
        for (Int_t s = 0; s < fNbSensors; s++)
            fh2_ColVsRow[s]->Reset();
    }

    if (fCalItems)
    {
        for (Int_t s = 0; s < fNbSensors; s++)
        {
            fh2_ColVsRowCal[s]->Reset();
            fh1_Calmult[s]->Reset();
        }
    }

    if (fHitItems)
    {
        for (Int_t s = 0; s < fNbSensors; s++)
        {
            fh1_Clustermult[s]->Reset();
            fh1_Clustersize[s]->Reset();
            fh2_PosHit[s]->Reset();
        }
    }

    return;
}

void R3BAlpideOnlineSpectra::Exec(Option_t* option)
{
    // Check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    if (fTpat1 >= 0 && fTpat2 >= 0)
    {
        // fTpat = 1-16; fTpat_bit = 0-15
        Int_t fTpat_bit1 = fTpat1 - 1;
        Int_t fTpat_bit2 = fTpat2 - 1;
        Int_t tpatbin;
        for (Int_t i = 0; i < 16; i++)
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
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hit = (R3BAlpideMappedData*)fMappedItems->At(ihit);
            if (!hit)
                continue;
            fh2_ColVsRow[hit->GetSensorId() - 1]->Fill(hit->GetCol(), hit->GetRow());
        }
    }

    // Fill cal data
    if (fCalItems)
    {
        if (fCalItems->GetEntriesFast() > 0)
        {
            Int_t mult[fNbSensors];
            for (Int_t s = 0; s < fNbSensors; s++)
                mult[s] = 0;
            auto nHits = fCalItems->GetEntriesFast();
            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                auto hit = (R3BAlpideCalData*)fCalItems->At(ihit);
                if (!hit)
                    continue;
                Int_t senid = hit->GetSensorId() - 1;
                fh2_ColVsRowCal[senid]->Fill(hit->GetCol(), hit->GetRow());
                mult[senid]++;
            }

            for (Int_t s = 0; s < fNbSensors; s++)
                fh1_Calmult[s]->Fill(mult[s]);
        }
        else
        {
            for (Int_t s = 0; s < fNbSensors; s++)
                fh1_Calmult[s]->Fill(0);
        }
    }

    // Fill hit data
    if (fHitItems && fHitItems->GetEntriesFast() > 0)
    {
        Int_t mult[fNbSensors];
        for (Int_t s = 0; s < fNbSensors; s++)
            mult[s] = 0;
        auto nHits = fHitItems->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hit = (R3BAlpideHitData*)fHitItems->At(ihit);
            if (!hit)
                continue;
            Int_t senid = hit->GetSensorId() - 1;
            fh1_Clustersize[senid]->Fill(hit->GetClusterSize());
            fh2_PosHit[senid]->Fill(hit->GetPosl(), hit->GetPost());
            mult[senid]++;
        }
        for (Int_t s = 0; s < fNbSensors; s++)
            fh1_Clustermult[s]->Fill(mult[s]);
    }

    fNEvents++;
    return;
}

void R3BAlpideOnlineSpectra::FinishEvent()
{
    R3BLOG(debug, "Cleaning data structures");
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
}

ClassImp(R3BAlpideOnlineSpectra);
