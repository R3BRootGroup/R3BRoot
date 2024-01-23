/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
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
// -----               R3BTttxOnlineSpectra               -----
// -----         Created 23/01/24 by M. Whitehead         -----
// -----           Fill TTT10 online histograms           -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with TTTX online data
 */

#include "R3BTttxOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BShared.h"
#include "R3BTttxMappedData.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TFolder.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THttpServer.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVector3.h"
#include <fstream>
#include <iostream>
#include <sstream>

R3BTttxOnlineSpectra::R3BTttxOnlineSpectra()
    : R3BTttxOnlineSpectra("TttxOnlineSpectra", 1)
{
}

R3BTttxOnlineSpectra::R3BTttxOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItemsTttx(NULL)
    , fNEvents(0)
    , fExpId(0)
    , fNbDets(2)
    , fNbStrips(32)
    , fNbTref(1)
    , fNbTrig(1)
    , fNbEntries(50000)
{
}

R3BTttxOnlineSpectra::~R3BTttxOnlineSpectra() { R3BLOG(debug1, ""); }

InitStatus R3BTttxOnlineSpectra::Init()
{
    R3BLOG(info, "");

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* rootManager = FairRootManager::Instance();
    R3BLOG_IF(fatal, NULL == rootManager, "FairRootManager not found");

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    header = dynamic_cast<R3BEventHeader*>(rootManager->GetObject("EventHeader."));
    if (fExpId == 0) // Obtain global ExpId if it's not set locally.
    {
        fExpId = header->GetExpId();
        R3BLOG(info, "fExpId: " << fExpId);
    }

    // get access to mapped data of TTT10
    fMappedItemsTttx = dynamic_cast<TClonesArray*>(rootManager->GetObject("tttxMappedData"));
    if (!fMappedItemsTttx)
    {
        R3BLOG(fatal, "tttxMappedData not found");
        return kFATAL;
    }

    // get access to cal data of the TTTX

    // get access to hit data of the TTTX

    // Create histograms for detectors
    char Name1[255];
    char Name2[255];

    // Energy range for strips
    Double_t binsE = 10000;
    Double_t minE = 0;
    Double_t maxE = 500000;

    // Time range for strips
    Double_t binsT = 2000;
    Double_t minT = -64000;
    Double_t maxT = 64000;

    // Multiplicity range for strips
    Double_t binsM = 10;
    Double_t minM = -0.5;
    Double_t maxM = 9.5;

    // MAIN FOLDER
    TFolder* mainfolTTTX = new TFolder("TTT10", "TTT10 info");

    // Folder for mapped data
    TFolder* mapfolTTTX = new TFolder("Map", "Map TTT10 info");

    cMap = new TCanvas("Tttx_Mapped", "Tttx_Mapped", 10, 10, 800, 700);
    cMap->Divide(4, 3);
    mapfolTTTX->Add(cMap);
    mainfolTTTX->Add(mapfolTTTX);

    //==============================  Mapped Data  ========================================

    fh2_EnergyVsStrip_all.resize(fNbDets);
    fh2_EnergyVsStrip_recent.resize(fNbDets);
    fh2_TimeVsStrip.resize(fNbDets);
    fh1_Strip.resize(fNbDets);
    fh2_MultVsStrip.resize(fNbDets);

    for (Int_t i = 0; i < fNbDets; i++)
    {
        sprintf(Name1, "fh2_energy_vs_strip_recent_det_%d", i + 1);
        sprintf(Name2, "Mapped energy vs strip number for TTT10 Det: %d, last %d events", i + 1, fNbEntries);
        fh2_EnergyVsStrip_recent[i] =
            R3B::root_owned<TH2F>(Name1, Name2, fNbStrips, 1, fNbStrips + 1, binsE, minE, maxE);
        fh2_EnergyVsStrip_recent[i]->GetXaxis()->SetTitle("Strip number");
        fh2_EnergyVsStrip_recent[i]->GetYaxis()->SetTitle("Energy [channels]");
        fh2_EnergyVsStrip_recent[i]->GetYaxis()->SetTitleOffset(1.4);
        fh2_EnergyVsStrip_recent[i]->GetXaxis()->CenterTitle(true);
        cMap->cd(i + 1);
        fh2_EnergyVsStrip_recent[i]->Draw("colz");
    }
    for (Int_t i = 0; i < fNbDets; i++)
    {
        sprintf(Name1, "fh2_energy_vs_strip_all_det_%d", i + 1);
        sprintf(Name2, "Mapped energy vs strip number for TTT10 Det: %d, all events", i + 1);
        fh2_EnergyVsStrip_all[i] = R3B::root_owned<TH2F>(Name1, Name2, fNbStrips, 1, fNbStrips + 1, binsE, minE, maxE);
        fh2_EnergyVsStrip_all[i]->GetXaxis()->SetTitle("Strip number");
        fh2_EnergyVsStrip_all[i]->GetYaxis()->SetTitle("Energy [channels]");
        fh2_EnergyVsStrip_all[i]->GetYaxis()->SetTitleOffset(1.4);
        fh2_EnergyVsStrip_all[i]->GetXaxis()->CenterTitle(true);
        cMap->cd(i + 3);
        fh2_EnergyVsStrip_all[i]->Draw("colz");
    }
    for (Int_t i = 0; i < fNbDets; i++)
    {
        sprintf(Name1, "strip_count_det_%d", i + 1);
        sprintf(Name2, "Number of counts per strip for TTT10 Det: %d, all events", i + 1);
        fh1_Strip[i] = R3B::root_owned<TH1F>(Name1, Name2, fNbStrips, 1, fNbStrips + 1);
        fh1_Strip[i]->GetXaxis()->SetTitle("Strip number");
        fh1_Strip[i]->GetYaxis()->SetTitle("Counts");
        fh1_Strip[i]->GetYaxis()->SetTitleOffset(1.4);
        fh1_Strip[i]->GetXaxis()->CenterTitle(true);
        cMap->cd(i + 5);
        fh1_Strip[i]->Draw();
    }
    for (Int_t i = 0; i < fNbDets; i++)
    {
        sprintf(Name1, "time_vs_strip_det_%d", i + 1);
        sprintf(Name2, "Time - trigger time vs strip number for TTT10 Det: %d", i + 1);
        fh2_TimeVsStrip[i] = R3B::root_owned<TH2F>(Name1, Name2, fNbStrips, 1, fNbStrips + 1, binsT, minT, maxT);
        fh2_TimeVsStrip[i]->GetXaxis()->SetTitle("Strip number");
        fh2_TimeVsStrip[i]->GetYaxis()->SetTitle("Time - trigger time [channels]");
        fh2_TimeVsStrip[i]->GetYaxis()->SetTitleOffset(1.4);
        fh2_TimeVsStrip[i]->GetXaxis()->CenterTitle(true);
        cMap->cd(i + 7);
        fh2_TimeVsStrip[i]->Draw("colz");
    }

    sprintf(Name1, "highest_e_correlation");
    sprintf(Name2, "Highest energy of Det 2 Vs highest energy of Det 1");
    fh2_E2VsE1 = R3B::root_owned<TH2F>(Name1, Name2, binsE, minE, maxE, binsE, minE, maxE);
    fh2_E2VsE1->GetXaxis()->SetTitle("Highest energy hit of Det 1 [channels]");
    fh2_E2VsE1->GetYaxis()->SetTitle("Highest energy hit of Det 2 [channels]");
    fh2_E2VsE1->GetYaxis()->SetTitleOffset(1.4);
    fh2_E2VsE1->GetXaxis()->CenterTitle(true);
    cMap->cd(9);
    fh2_E2VsE1->Draw("colz");

    sprintf(Name1, "highest_e_strip_correlation");
    sprintf(Name2, "Strip with highest energy of Det 2 Vs strip with highest energy of Det 1");
    fh2_E2StripVsE1Strip =
        R3B::root_owned<TH2F>(Name1, Name2, fNbStrips, 1, fNbStrips + 1, fNbStrips, 1, fNbStrips + 1);
    fh2_E2StripVsE1Strip->GetXaxis()->SetTitle("strip number of Det 1");
    fh2_E2StripVsE1Strip->GetYaxis()->SetTitle("strip number of Det 2");
    fh2_E2StripVsE1Strip->GetYaxis()->SetTitleOffset(1.4);
    fh2_E2StripVsE1Strip->GetXaxis()->CenterTitle(true);
    cMap->cd(10);
    fh2_E2StripVsE1Strip->Draw("colz");

    for (Int_t i = 0; i < fNbDets; i++)
    {
        sprintf(Name1, "multiplicity_det_%d", i + 1);
        sprintf(Name2, "Multiplicity vs strip number for TTT10 Det: %d", i + 1);
        fh2_MultVsStrip[i] = R3B::root_owned<TH2F>(Name1, Name2, fNbStrips, 1, fNbStrips + 1, binsM, minM, maxM);
        fh2_MultVsStrip[i]->GetXaxis()->SetTitle("Strip number");
        fh2_MultVsStrip[i]->GetYaxis()->SetTitle("Multiplicity");
        fh2_MultVsStrip[i]->GetYaxis()->SetTitleOffset(1.4);
        fh2_MultVsStrip[i]->GetXaxis()->CenterTitle(true);
        cMap->cd(i + 11);
        fh2_MultVsStrip[i]->Draw("colz");
    }

    run->AddObject(mainfolTTTX);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_Tttx_HIST", Form("/Objects/%s/->Reset_Histo()", GetName()));
    R3BLOG(info, "Init Complete");

    return kSUCCESS;
}

void R3BTttxOnlineSpectra::Reset_Histo()
{
    R3BLOG(info, "Reset_Histo");
    // Map data
    for (Int_t i = 0; i < fNbDets; i++)
    {
        fh2_EnergyVsStrip_recent[i]->Reset();
        fh2_EnergyVsStrip_all[i]->Reset();
        fh1_Strip[i]->Reset();
        fh2_TimeVsStrip[i]->Reset();
        fh2_MultVsStrip[i]->Reset();
    }
    fh2_E2VsE1->Reset();
    fh2_E2StripVsE1Strip->Reset();
}

void R3BTttxOnlineSpectra::Exec(Option_t* option)
{
    // Fill Mapped Data
    if (!fMappedItemsTttx || fMappedItemsTttx->GetEntriesFast() == 0)
    {
        return;
    }
    UInt_t det = 0;
    UInt_t strip = 0;

    energy.clear();
    time.clear();
    mult.clear();
    highest_e.clear();
    highest_e_strip.clear();

    energy.resize(fNbDets);
    time.resize(fNbDets);
    mult.resize(fNbDets);
    for (int idet = 0; idet < fNbDets; idet++)
    {
        energy.at(idet).resize(fNbStrips);
        time.at(idet).resize(fNbStrips + fNbTref + fNbTrig);
        mult.at(idet).resize(fNbStrips);
    }
    highest_e.resize(fNbDets);
    highest_e_strip.resize(fNbDets);

    for (Int_t j = 0; j < fNbDets; j++)
    {
        highest_e[j] = 0;
        highest_e_strip[j] = 0;
        for (Int_t i = 0; i < fNbStrips; i++)
        {
            energy[j][i] = NAN; // mult=1 !!!
            mult[j][i] = 0;
        }
        for (Int_t i = 0; i < fNbStrips + fNbTref + fNbTrig; i++)
        {
            time[j][i] = NAN; // mult=1 !!!
        }
    }

    Int_t nHits = fMappedItemsTttx->GetEntriesFast();
    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
        R3BTttxMappedData* hit = dynamic_cast<R3BTttxMappedData*>(fMappedItemsTttx->At(ihit));
        if (!hit)
        {
            continue;
        }
        det = hit->GetDetID() - 1;
        strip = hit->GetStripID() - 1; // stripID - 1 --> 32 => Tref, 33 => Ttrig

        if (strip < fNbStrips)
        {
            energy[det][strip] = hit->GetEnergy() & 0xffff; // Take only lower 16 bits
            mult[det][strip]++;
            if (!isnan(energy[det][strip]) && energy[det][strip] > highest_e[det])
            {
                highest_e[det] = hit->GetEnergy();
                highest_e_strip[det] = hit->GetStripID();
            }
        }
        time[det][strip] = hit->GetTime();

        fh2_EnergyVsStrip_recent[det]->Fill(strip + 1, hit->GetEnergy());
        fh2_EnergyVsStrip_all[det]->Fill(strip + 1, hit->GetEnergy());
        fh1_Strip[det]->Fill(strip + 1);
    }
    for (Int_t i_det = 0; i_det < fNbDets; i_det++)
    {
        for (Int_t i_strip = 0; i_strip < fNbStrips; i_strip++)
        {
            if (mult[i_det][i_strip] > 0)
                fh2_MultVsStrip[i_det]->Fill(i_strip + 1, mult[i_det][i_strip]);
            if (!isnan(time[i_det][i_strip]) && !isnan(time[i_det][fNbStrips + fNbTref + fNbTrig - 1]))
                fh2_TimeVsStrip[i_det]->Fill(
                    i_strip + 1,
                    time[i_det][i_strip] - time[i_det][fNbStrips + fNbTref + fNbTrig - 1]); // mult=1 !
        }
    }
    if (!isnan(highest_e[0]) && !isnan(highest_e[1]))
        fh2_E2VsE1->Fill(highest_e[0], highest_e[1]);
    fh2_E2StripVsE1Strip->Fill(highest_e_strip[0], highest_e_strip[1]);

    for (Int_t i_det = 0; i_det < fNbDets; i_det++)
    {
        if (fh2_EnergyVsStrip_recent[i_det]->GetEntries() > fNbEntries)
        {
            fh2_EnergyVsStrip_recent[i_det]->Reset();
        }
    }
}

void R3BTttxOnlineSpectra::FinishEvent()
{
    if (fMappedItemsTttx)
    {
        fMappedItemsTttx->Clear();
    }
}

void R3BTttxOnlineSpectra::FinishTask()
{
    if (fMappedItemsTttx)
    {
        cMap->Write();
    }
}

ClassImp(R3BTttxOnlineSpectra);
