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
// -----    Created 16/07/21  by J.L. Rodriguez-Sanchez   -----
// -----          Fill FOOT online histograms             -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with FOOT online data
 */

#include "R3BFootOnlineSpectra.h"

#include "R3BFootMappedData.h"

#include "R3BEventHeader.h"
#include "THttpServer.h"

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
    , fEventHeader(nullptr)
    , fMappedItems(NULL)
    , fCalItems(NULL)
    , fHitItems(NULL)
    , fTrigger(-1)
    , fNEvents(0)
    , fNbDet(10)
{
}

R3BFootOnlineSpectra::~R3BFootOnlineSpectra()
{
    LOG(DEBUG) << "R3BFootOnlineSpectra::Delete instance";
    if (fMappedItems)
        delete fMappedItems;
    if (fCalItems)
        delete fCalItems;
    if (fHitItems)
        delete fHitItems;
}

InitStatus R3BFootOnlineSpectra::Init()
{
    LOG(INFO) << "R3BFootOnlineSpectra::Init()";

    // Looking for FairRootManager
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(FATAL) << "R3BFootOnlineSpectra::FairRootManager not found";

    // Look for the R3BEventHeader
    fEventHeader = (R3BEventHeader*)mgr->GetObject("EventHeader.");
    if (!fEventHeader)
    {
        LOG(WARNING) << "R3BFootOnlineSpectra::Init() EventHeader. not found";
        fEventHeader = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    }
    else
        LOG(INFO) << "R3BFootOnlineSpectra::Init() EventHeader. found";

    // Get access to Mapped data
    fMappedItems = (TClonesArray*)mgr->GetObject("FootMappedData");
    if (!fMappedItems)
    {
        LOG(FATAL) << "R3BFootOnlineSpectra::FootMappedData not found";
        return kFATAL;
    }

    // Get access to Cal data
    fCalItems = (TClonesArray*)mgr->GetObject("FootCalData");
    if (!fCalItems)
        LOG(WARNING) << "R3BFootOnlineSpectra::FootCalData not found";

    // Get access to Hit data
    fHitItems = (TClonesArray*)mgr->GetObject("FootHitData");
    if (!fHitItems)
        LOG(WARNING) << "R3BFootOnlineSpectra::FootHitData not found";

    // Create histograms for all the detectors

    // Energy range for strips
    Double_t binsE = 200;
    Double_t minE = 0;
    Double_t maxE = 5500;

    char Name1[255];
    char Name2[255];

    // MAIN FOLDER-FOOT
    TFolder* mainfol = new TFolder("FootSi", "Foot-Si info");
    // Folder for mapped data
    TFolder* mapfol = new TFolder("Map", "Map FOOT info");

    // Mapped data
    fh2_EnergyVsStrip.resize(fNbDet);
    for (Int_t i = 0; i < fNbDet; i++)
    { // one histo per detector
        sprintf(Name1, "fh2_energy_vs_strip_det_%d", i + 1);
        sprintf(Name2, "Energy vs strip number for FOOT Det: %d", i + 1);
        fh2_EnergyVsStrip[i] = new TH2F(Name1, Name2, 640, 1, 641, binsE, minE, maxE);
        fh2_EnergyVsStrip[i]->GetXaxis()->SetTitle("Strip number");
        fh2_EnergyVsStrip[i]->GetYaxis()->SetTitle("Energy [channels]");
        fh2_EnergyVsStrip[i]->GetYaxis()->SetTitleOffset(1.4);
        fh2_EnergyVsStrip[i]->GetXaxis()->CenterTitle(true);
        fh2_EnergyVsStrip[i]->GetYaxis()->CenterTitle(true);
        fh2_EnergyVsStrip[i]->Draw("col");
        mapfol->Add(fh2_EnergyVsStrip[i]);
    }
    mainfol->Add(mapfol);

    // Folder for cal data
    if (fCalItems)
    {
        TFolder* calfol = new TFolder("Cal", "Cal FOOT info");
        mainfol->Add(calfol);
    }

    // Folder for hit data
    if (fHitItems)
    {
        TFolder* hitfol = new TFolder("Hit", "Hit FOOT info");
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
    LOG(INFO) << "R3BFootOnlineSpectra::Reset_FOOT_Histo";

    // Mapped data
    for (Int_t i = 0; i < fNbDet; i++)
    {
        fh2_EnergyVsStrip[i]->Reset();
    }
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
    return;
}

void R3BFootOnlineSpectra::FinishTask()
{
    if (fMappedItems)
    {
        for (Int_t i = 0; i < fNbDet; i++)
            fh2_EnergyVsStrip[i]->Write();
    }
    return;
}

ClassImp(R3BFootOnlineSpectra);
