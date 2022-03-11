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
// -----                  R3BMusliOnlineSpectra           -----
// -----    Created 29/09/19  by J.L. Rodriguez-Sanchez   -----
// -----           Fill MUSIC online histograms           -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with MUSIC online data
 */

#include "R3BMusliOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BMusliMappedData.h"
#include "THttpServer.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TFolder.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TVector3.h"

#include "TClonesArray.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TMath.h"
#include "TRandom.h"
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

R3BMusliOnlineSpectra::R3BMusliOnlineSpectra()
    : R3BMusliOnlineSpectra("MusliOnlineSpectra", 1)
{
}

R3BMusliOnlineSpectra::R3BMusliOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMapItemsMusli(NULL)
    , fNEvents(0)
{
}

R3BMusliOnlineSpectra::~R3BMusliOnlineSpectra()
{
    LOG(INFO) << "R3BMusliOnlineSpectra::Delete instance";
    if (fMapItemsMusli)
        delete fMapItemsMusli;
}

InitStatus R3BMusliOnlineSpectra::Init()
{
    LOG(INFO) << "R3BMusliOnlineSpectra::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        LOG(FATAL) << "R3BMusliOnlineSpectra::Init FairRootManager not found";
        return kFATAL;
    }
    // header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // get access to mapped data of the MUSIC
    fMapItemsMusli = (TClonesArray*)mgr->GetObject("MusliMappedData");
    if (!fMapItemsMusli)
    {
        return kFATAL;
    }

    // Create histograms for detectors
    char Name1[255];
    char Name2[255];

    // === =========== === //
    // === MAPPED DATA === //
    // === =========== === //

    // --- Multiplicities --- //
    cMusliMap_Mult = new TCanvas("Musli_Mult", "", 10, 10, 800, 700);
    cMusliMap_Mult->Divide(2, 1);

    fh1_Muslimap_mult = new TH1I("MultTot", "Total Multiplicity", 20, -0.5, 19.5);
    fh1_Muslimap_mult->GetXaxis()->SetTitle("fSignal");
    fh1_Muslimap_mult->GetYaxis()->SetTitle("Total Counts");
    fh1_Muslimap_mult->GetYaxis()->SetTitleOffset(1.1);
    fh1_Muslimap_mult->GetXaxis()->CenterTitle(true);
    fh1_Muslimap_mult->GetYaxis()->CenterTitle(true);
    fh1_Muslimap_mult->GetXaxis()->SetLabelSize(0.045);
    fh1_Muslimap_mult->GetXaxis()->SetTitleSize(0.045);
    fh1_Muslimap_mult->GetYaxis()->SetLabelSize(0.045);
    fh1_Muslimap_mult->GetYaxis()->SetTitleSize(0.045);
    fh1_Muslimap_mult->SetFillColor(31);
    cMusliMap_Mult->cd(1);
    fh1_Muslimap_mult->Draw("");

    fh2_Muslimap_mult = new TH2I("MultDist", "Multiplicity Distribution per fSignal", 20, -0.5, 19.5, 50, -0.5, 49.5);
    fh2_Muslimap_mult->GetXaxis()->SetTitle("fSignal");
    fh2_Muslimap_mult->GetYaxis()->SetTitle("Counts per Event");
    fh2_Muslimap_mult->GetYaxis()->SetTitleOffset(1.1);
    fh2_Muslimap_mult->GetXaxis()->CenterTitle(true);
    fh2_Muslimap_mult->GetYaxis()->CenterTitle(true);
    fh2_Muslimap_mult->GetXaxis()->SetLabelSize(0.045);
    fh2_Muslimap_mult->GetXaxis()->SetTitleSize(0.045);
    fh2_Muslimap_mult->GetYaxis()->SetLabelSize(0.045);
    fh2_Muslimap_mult->GetYaxis()->SetTitleSize(0.045);
    fh2_Muslimap_mult->SetFillColor(31);
    cMusliMap_Mult->cd(2);
    fh2_Muslimap_mult->Draw("COL");

    // --- Energies --- //
    cMusliMap_E2 = new TCanvas("Musli_Emap2", "", 10, 10, 800, 700);
    cMusliMap_E2->Divide(4, 2);
    for (Int_t j = 0; j < 8; j++)
    {
        sprintf(Name1, "fh1_Musli_Emap_a%d_a%d", 2 * j + 1, 2 * j + 2);
        sprintf(Name2, " E A%02d and A%02d if mult==1", 2 * j + 1, 2 * j + 2);
        fh1_Muslimap_E[j] = new TH1I(Name1, Name2, 64000, 0, 64000);
        fh1_Muslimap_E[j]->GetXaxis()->SetTitle("Energy [channels]");
        fh1_Muslimap_E[j]->GetYaxis()->SetTitle("Counts");
        fh1_Muslimap_E[j]->GetYaxis()->SetTitleOffset(1.1);
        fh1_Muslimap_E[j]->GetXaxis()->CenterTitle(true);
        fh1_Muslimap_E[j]->GetYaxis()->CenterTitle(true);
        fh1_Muslimap_E[j]->GetXaxis()->SetLabelSize(0.045);
        fh1_Muslimap_E[j]->GetXaxis()->SetTitleSize(0.045);
        fh1_Muslimap_E[j]->GetYaxis()->SetLabelSize(0.045);
        fh1_Muslimap_E[j]->GetYaxis()->SetTitleSize(0.045);
        fh1_Muslimap_E[j]->SetFillColor(31);
        cMusliMap_E2->cd(j + 1);
        fh1_Muslimap_E[j]->Draw("");
    }

    cMusliMap_E4 = new TCanvas("Musli_Emap4", "", 10, 10, 800, 700);
    cMusliMap_E4->Divide(2, 2);
    for (Int_t j = 0; j < 4; j++)
    {
        sprintf(Name1, "fh1_Musli_Emap_a%d_to_a%d", j * 4 + 1, j * 4 + 4);
        sprintf(Name2, "E A%02d to A%02d if mult==1", j * 4 + 1, j * 4 + 5);
        fh1_Muslimap_E[j + 8] = new TH1I(Name1, Name2, 64000, 0, 64000);
        fh1_Muslimap_E[j + 8]->GetXaxis()->SetTitle("Energy [channels]");
        fh1_Muslimap_E[j + 8]->GetYaxis()->SetTitle("Counts");
        fh1_Muslimap_E[j + 8]->GetYaxis()->SetTitleOffset(1.1);
        fh1_Muslimap_E[j + 8]->GetXaxis()->CenterTitle(true);
        fh1_Muslimap_E[j + 8]->GetYaxis()->CenterTitle(true);
        fh1_Muslimap_E[j + 8]->GetXaxis()->SetLabelSize(0.045);
        fh1_Muslimap_E[j + 8]->GetXaxis()->SetTitleSize(0.045);
        fh1_Muslimap_E[j + 8]->GetYaxis()->SetLabelSize(0.045);
        fh1_Muslimap_E[j + 8]->GetYaxis()->SetTitleSize(0.045);
        fh1_Muslimap_E[j + 8]->SetFillColor(31);
        cMusliMap_E4->cd(j + 1);
        fh1_Muslimap_E[j + 8]->Draw("");
    }

    cMusliMap_E8 = new TCanvas("Musli_Emap8", "", 10, 10, 800, 700);
    cMusliMap_E8->Divide(2, 1);
    for (Int_t j = 0; j < 2; j++)
    {
        sprintf(Name1, "fh1_Musli_Emap_mean_a%d_to_a%d", j * 8 + 1, j * 8 + 8);
        sprintf(Name2, "E A%02d to A%02d if mult==1", j * 8 + 1, j * 8 + 8);
        fh1_Muslimap_E[j + 12] = new TH1I(Name1, Name2, 64000, 0, 64000);
        fh1_Muslimap_E[j + 12]->GetXaxis()->SetTitle("Energy [channels]");
        fh1_Muslimap_E[j + 12]->GetYaxis()->SetTitle("Counts");
        fh1_Muslimap_E[j + 12]->GetYaxis()->SetTitleOffset(1.1);
        fh1_Muslimap_E[j + 12]->GetXaxis()->CenterTitle(true);
        fh1_Muslimap_E[j + 12]->GetYaxis()->CenterTitle(true);
        fh1_Muslimap_E[j + 12]->GetXaxis()->SetLabelSize(0.045);
        fh1_Muslimap_E[j + 12]->GetXaxis()->SetTitleSize(0.045);
        fh1_Muslimap_E[j + 12]->GetYaxis()->SetLabelSize(0.045);
        fh1_Muslimap_E[j + 12]->GetYaxis()->SetTitleSize(0.045);
        fh1_Muslimap_E[j + 12]->SetFillColor(31);
        cMusliMap_E8->cd(j + 1);
        fh1_Muslimap_E[j + 12]->Draw("");
    }

    cMusliMap_E16 = new TCanvas("Musli_Emap16", "", 10, 10, 800, 700);
    sprintf(Name1, "fh1_Musli_Emap_mean_a1_to_a16");
    sprintf(Name2, "E A01 to A16 if mult==1");
    fh1_Muslimap_E[14] = new TH1I(Name1, Name2, 64000, 0, 64000);
    fh1_Muslimap_E[14]->GetXaxis()->SetTitle("Energy [channels]");
    fh1_Muslimap_E[14]->GetYaxis()->SetTitle("Counts");
    fh1_Muslimap_E[14]->GetYaxis()->SetTitleOffset(1.1);
    fh1_Muslimap_E[14]->GetXaxis()->CenterTitle(true);
    fh1_Muslimap_E[14]->GetYaxis()->CenterTitle(true);
    fh1_Muslimap_E[14]->GetXaxis()->SetLabelSize(0.045);
    fh1_Muslimap_E[14]->GetXaxis()->SetTitleSize(0.045);
    fh1_Muslimap_E[14]->GetYaxis()->SetLabelSize(0.045);
    fh1_Muslimap_E[14]->GetYaxis()->SetTitleSize(0.045);
    fh1_Muslimap_E[14]->SetFillColor(31);
    cMusliMap_E16->cd();
    fh1_Muslimap_E[14]->Draw("");

    // --- Energy vs Energy --- //
    cMusliMap_EvsE2 = new TCanvas("Musli_EvsEmap2", "", 10, 10, 800, 700);
    cMusliMap_EvsE2->Divide(4, 2);
    for (Int_t j = 0; j < 7; j++)
    {
        sprintf(Name1, "fh2_Musli_Ea%da%d_vs_Ea%da%d", 2 * j + 1, 2 * j + 2, 2 * j + 3, 2 * j + 4);
        sprintf(Name2, "E A%02d and A%02d vs E A%02d and A%02d if mult==1", 2 * j + 1, 2 * j + 2, 2 * j + 3, 2 * j + 4);
        fh2_Muslimap_EvsE[j] = new TH2I(Name1, Name2, 1280, 0, 64000, 1280, 0, 64000);
        fh2_Muslimap_EvsE[j]->GetXaxis()->SetTitle("Energy [channel]");
        fh2_Muslimap_EvsE[j]->GetYaxis()->SetTitle("Energy [channel]");
        fh2_Muslimap_EvsE[j]->GetYaxis()->SetTitleOffset(1.1);
        fh2_Muslimap_EvsE[j]->GetXaxis()->CenterTitle(true);
        fh2_Muslimap_EvsE[j]->GetYaxis()->CenterTitle(true);
        fh2_Muslimap_EvsE[j]->GetXaxis()->SetLabelSize(0.045);
        fh2_Muslimap_EvsE[j]->GetXaxis()->SetTitleSize(0.045);
        fh2_Muslimap_EvsE[j]->GetYaxis()->SetLabelSize(0.045);
        fh2_Muslimap_EvsE[j]->GetYaxis()->SetTitleSize(0.045);
        fh2_Muslimap_EvsE[j]->SetFillColor(31);
        cMusliMap_EvsE2->cd(j + 1);
        fh2_Muslimap_EvsE[j]->Draw("COL");
    }

    cMusliMap_EvsE4 = new TCanvas("Musli_EvsEmap4", "", 10, 10, 800, 700);
    cMusliMap_EvsE4->Divide(3, 1);
    for (Int_t j = 0; j < 3; j++)
    {
        sprintf(Name1, "fh2_Musli_Ea%da%d_vs_Ea%da%d", 4 * j + 1, 4 * j + 4, 4 * j + 5, 4 * j + 8);
        sprintf(Name2, "E A%02d to A%02d vs E A%02d to A%02d if mult==1", 4 * j + 1, 4 * j + 4, 4 * j + 5, 4 * j + 8);
        fh2_Muslimap_EvsE[j + 8] = new TH2I(Name1, Name2, 1280, 0, 64000, 1280, 0, 64000);
        fh2_Muslimap_EvsE[j + 8]->GetXaxis()->SetTitle("Energy [channel]");
        fh2_Muslimap_EvsE[j + 8]->GetYaxis()->SetTitle("Energy [channel]");
        fh2_Muslimap_EvsE[j + 8]->GetYaxis()->SetTitleOffset(1.1);
        fh2_Muslimap_EvsE[j + 8]->GetXaxis()->CenterTitle(true);
        fh2_Muslimap_EvsE[j + 8]->GetYaxis()->CenterTitle(true);
        fh2_Muslimap_EvsE[j + 8]->GetXaxis()->SetLabelSize(0.045);
        fh2_Muslimap_EvsE[j + 8]->GetXaxis()->SetTitleSize(0.045);
        fh2_Muslimap_EvsE[j + 8]->GetYaxis()->SetLabelSize(0.045);
        fh2_Muslimap_EvsE[j + 8]->GetYaxis()->SetTitleSize(0.045);
        fh2_Muslimap_EvsE[j + 8]->SetFillColor(31);
        cMusliMap_EvsE4->cd(j + 1);
        fh2_Muslimap_EvsE[j + 8]->Draw("COL");
    }

    cMusliMap_EvsE8 = new TCanvas("Musli_EvsEmap8", "", 10, 10, 800, 700);
    sprintf(Name1, "fh2_Musli_Ea1a8_vs_Ea9a16");
    sprintf(Name2, "E A01 to A08 vs E A09 to A16 if mult==1");
    fh2_Muslimap_EvsE[12] = new TH2I(Name1, Name2, 1280, 0, 64000, 1280, 0, 64000);
    fh2_Muslimap_EvsE[12]->GetXaxis()->SetTitle("Energy [channel]");
    fh2_Muslimap_EvsE[12]->GetYaxis()->SetTitle("Energy [channel]");
    fh2_Muslimap_EvsE[12]->GetYaxis()->SetTitleOffset(1.1);
    fh2_Muslimap_EvsE[12]->GetXaxis()->CenterTitle(true);
    fh2_Muslimap_EvsE[12]->GetYaxis()->CenterTitle(true);
    fh2_Muslimap_EvsE[12]->GetXaxis()->SetLabelSize(0.045);
    fh2_Muslimap_EvsE[12]->GetXaxis()->SetTitleSize(0.045);
    fh2_Muslimap_EvsE[12]->GetYaxis()->SetLabelSize(0.045);
    fh2_Muslimap_EvsE[12]->GetYaxis()->SetTitleSize(0.045);
    fh2_Muslimap_EvsE[12]->SetFillColor(31);
    cMusliMap_EvsE8->cd();
    fh2_Muslimap_EvsE[12]->Draw("COL");

    // --- Drift times --- //
    cMusliMap_DT2 = new TCanvas("Musli_DTmap2", "", 10, 10, 800, 700);
    cMusliMap_DT2->Divide(4, 2);
    for (Int_t j = 0; j < 8; j++)
    {
        sprintf(Name1, "fh1_Musli_DTmap_mean_a%d_a%d", 2 * j + 1, 2 * j + 2);
        sprintf(Name2, "DT A%02d and A%02d if mult==1", 2 * j + 1, 2 * j + 2);
        fh1_Muslimap_DT[j] = new TH1I(Name1, Name2, 5000, 0, 25000);
        fh1_Muslimap_DT[j]->GetXaxis()->SetTitle("Drift Time [100ps]");
        fh1_Muslimap_DT[j]->GetYaxis()->SetTitle("Counts");
        fh1_Muslimap_DT[j]->GetYaxis()->SetTitleOffset(1.1);
        fh1_Muslimap_DT[j]->GetXaxis()->CenterTitle(true);
        fh1_Muslimap_DT[j]->GetYaxis()->CenterTitle(true);
        fh1_Muslimap_DT[j]->GetXaxis()->SetLabelSize(0.045);
        fh1_Muslimap_DT[j]->GetXaxis()->SetTitleSize(0.045);
        fh1_Muslimap_DT[j]->GetYaxis()->SetLabelSize(0.045);
        fh1_Muslimap_DT[j]->GetYaxis()->SetTitleSize(0.045);
        fh1_Muslimap_DT[j]->SetFillColor(31);
        cMusliMap_DT2->cd(j + 1);
        fh1_Muslimap_DT[j]->Draw("");
    }

    cMusliMap_DT4 = new TCanvas("Musli_DTmap4", "", 10, 10, 800, 700);
    cMusliMap_DT4->Divide(2, 2);
    for (Int_t j = 0; j < 4; j++)
    {
        sprintf(Name1, "fh1_Musli_DTmap_mean_a%d_to_a%d", j * 4 + 1, j * 4 + 4);
        sprintf(Name2, "DT A%02d to A%02d if mult==1", j * 4 + 1, j * 4 + 5);
        fh1_Muslimap_DT[j + 8] = new TH1I(Name1, Name2, 5000, 0, 25000);
        fh1_Muslimap_DT[j + 8]->GetXaxis()->SetTitle("Drift Time [100ps]");
        fh1_Muslimap_DT[j + 8]->GetYaxis()->SetTitle("Counts");
        fh1_Muslimap_DT[j + 8]->GetYaxis()->SetTitleOffset(1.1);
        fh1_Muslimap_DT[j + 8]->GetXaxis()->CenterTitle(true);
        fh1_Muslimap_DT[j + 8]->GetYaxis()->CenterTitle(true);
        fh1_Muslimap_DT[j + 8]->GetXaxis()->SetLabelSize(0.045);
        fh1_Muslimap_DT[j + 8]->GetXaxis()->SetTitleSize(0.045);
        fh1_Muslimap_DT[j + 8]->GetYaxis()->SetLabelSize(0.045);
        fh1_Muslimap_DT[j + 8]->GetYaxis()->SetTitleSize(0.045);
        fh1_Muslimap_DT[j + 8]->SetFillColor(31);
        cMusliMap_DT4->cd(j + 1);
        fh1_Muslimap_DT[j + 8]->Draw("");
    }

    cMusliMap_DT8 = new TCanvas("Musli_DTmap8", "", 10, 10, 800, 700);
    cMusliMap_DT8->Divide(2, 1);
    for (Int_t j = 0; j < 2; j++)
    {
        sprintf(Name1, "fh1_Musli_DTmap_mean_a%d_to_a%d", j * 8 + 1, j * 8 + 8);
        sprintf(Name2, "DT A%02d to A%02d if mult==1", j * 8 + 1, j * 8 + 8);
        fh1_Muslimap_DT[j + 12] = new TH1I(Name1, Name2, 5000, 0, 25000);
        fh1_Muslimap_DT[j + 12]->GetXaxis()->SetTitle("Drift Time [100ps]");
        fh1_Muslimap_DT[j + 12]->GetYaxis()->SetTitle("Counts");
        fh1_Muslimap_DT[j + 12]->GetYaxis()->SetTitleOffset(1.1);
        fh1_Muslimap_DT[j + 12]->GetXaxis()->CenterTitle(true);
        fh1_Muslimap_DT[j + 12]->GetYaxis()->CenterTitle(true);
        fh1_Muslimap_DT[j + 12]->GetXaxis()->SetLabelSize(0.045);
        fh1_Muslimap_DT[j + 12]->GetXaxis()->SetTitleSize(0.045);
        fh1_Muslimap_DT[j + 12]->GetYaxis()->SetLabelSize(0.045);
        fh1_Muslimap_DT[j + 12]->GetYaxis()->SetTitleSize(0.045);
        fh1_Muslimap_DT[j + 12]->SetFillColor(31);
        cMusliMap_DT8->cd(j + 1);
        fh1_Muslimap_DT[j + 12]->Draw("");
    }

    cMusliMap_DT16 = new TCanvas("Musli_DTmap16", "", 10, 10, 800, 700);
    sprintf(Name1, "fh1_Musli_DTmap_mean_a1_to_a16");
    sprintf(Name2, "DT A01 to A16 if mult==1");
    fh1_Muslimap_DT[14] = new TH1I(Name1, Name2, 5000, 0, 25000);
    fh1_Muslimap_DT[14]->GetXaxis()->SetTitle("Drift Time [100ps]");
    fh1_Muslimap_DT[14]->GetYaxis()->SetTitle("Counts");
    fh1_Muslimap_DT[14]->GetYaxis()->SetTitleOffset(1.1);
    fh1_Muslimap_DT[14]->GetXaxis()->CenterTitle(true);
    fh1_Muslimap_DT[14]->GetYaxis()->CenterTitle(true);
    fh1_Muslimap_DT[14]->GetXaxis()->SetLabelSize(0.045);
    fh1_Muslimap_DT[14]->GetXaxis()->SetTitleSize(0.045);
    fh1_Muslimap_DT[14]->GetYaxis()->SetLabelSize(0.045);
    fh1_Muslimap_DT[14]->GetYaxis()->SetTitleSize(0.045);
    fh1_Muslimap_DT[14]->SetFillColor(31);
    cMusliMap_DT16->cd();
    fh1_Muslimap_DT[14]->Draw("");

    // --- Drift Time vs Drift Time --- //
    cMusliMap_DTvsDT2 = new TCanvas("Musli_DTvsDTmap2", "", 10, 10, 800, 700);
    cMusliMap_DTvsDT2->Divide(4, 2);
    for (Int_t j = 0; j < 7; j++)
    {
        sprintf(Name1, "fh2_Musli_DTa%da%d_vs_DTa%da%d", 2 * j + 1, 2 * j + 2, 2 * j + 3, 2 * j + 4);
        sprintf(
            Name2, "DT A%02d and A%02d vs DT A%02d and A%02d if mult==1", 2 * j + 1, 2 * j + 2, 2 * j + 3, 2 * j + 4);
        fh2_Muslimap_DTvsDT[j] = new TH2I(Name1, Name2, 1000, 0, 25000, 1000, 0, 25000);
        fh2_Muslimap_DTvsDT[j]->GetXaxis()->SetTitle("Drift Time [100ps]");
        fh2_Muslimap_DTvsDT[j]->GetYaxis()->SetTitle("Drift Time [100ps]");
        fh2_Muslimap_DTvsDT[j]->GetYaxis()->SetTitleOffset(1.1);
        fh2_Muslimap_DTvsDT[j]->GetXaxis()->CenterTitle(true);
        fh2_Muslimap_DTvsDT[j]->GetYaxis()->CenterTitle(true);
        fh2_Muslimap_DTvsDT[j]->GetXaxis()->SetLabelSize(0.045);
        fh2_Muslimap_DTvsDT[j]->GetXaxis()->SetTitleSize(0.045);
        fh2_Muslimap_DTvsDT[j]->GetYaxis()->SetLabelSize(0.045);
        fh2_Muslimap_DTvsDT[j]->GetYaxis()->SetTitleSize(0.045);
        fh2_Muslimap_DTvsDT[j]->SetFillColor(31);
        cMusliMap_DTvsDT2->cd(j + 1);
        fh2_Muslimap_DTvsDT[j]->Draw("COL");
    }

    cMusliMap_DTvsDT4 = new TCanvas("Musli_DTvsDTmap4", "", 10, 10, 800, 700);
    cMusliMap_DTvsDT4->Divide(3, 1);
    for (Int_t j = 0; j < 3; j++)
    {
        sprintf(Name1, "fh2_Musli_DTa%da%d_vs_DTa%da%d", 4 * j + 1, 4 * j + 4, 4 * j + 5, 4 * j + 8);
        sprintf(Name2, "DT A%02d to A%02d vs DT A%02d to A%02d if mult==1", 4 * j + 1, 4 * j + 4, 4 * j + 5, 4 * j + 8);
        fh2_Muslimap_DTvsDT[j + 8] = new TH2I(Name1, Name2, 1000, 0, 25000, 1000, 0, 25000);
        fh2_Muslimap_DTvsDT[j + 8]->GetXaxis()->SetTitle("Drift Time [100ps]");
        fh2_Muslimap_DTvsDT[j + 8]->GetYaxis()->SetTitle("Drift Time [100ps]");
        fh2_Muslimap_DTvsDT[j + 8]->GetYaxis()->SetTitleOffset(1.1);
        fh2_Muslimap_DTvsDT[j + 8]->GetXaxis()->CenterTitle(true);
        fh2_Muslimap_DTvsDT[j + 8]->GetYaxis()->CenterTitle(true);
        fh2_Muslimap_DTvsDT[j + 8]->GetXaxis()->SetLabelSize(0.045);
        fh2_Muslimap_DTvsDT[j + 8]->GetXaxis()->SetTitleSize(0.045);
        fh2_Muslimap_DTvsDT[j + 8]->GetYaxis()->SetLabelSize(0.045);
        fh2_Muslimap_DTvsDT[j + 8]->GetYaxis()->SetTitleSize(0.045);
        fh2_Muslimap_DTvsDT[j + 8]->SetFillColor(31);
        cMusliMap_DTvsDT4->cd(j + 1);
        fh2_Muslimap_DTvsDT[j + 8]->Draw("COL");
    }

    cMusliMap_DTvsDT8 = new TCanvas("Musli_DTvsDTmap8", "", 10, 10, 800, 700);
    sprintf(Name1, "fh2_Musli_DTa1a8_vs_DTa9a16");
    sprintf(Name2, "DT A01 to A08 vs DT A09 to A16 if mult==1");
    fh2_Muslimap_DTvsDT[12] = new TH2I(Name1, Name2, 1000, 0, 25000, 1000, 0, 25000);
    fh2_Muslimap_DTvsDT[12]->GetXaxis()->SetTitle("Drift Time [100ps]");
    fh2_Muslimap_DTvsDT[12]->GetYaxis()->SetTitle("Drift Time [100ps]");
    fh2_Muslimap_DTvsDT[12]->GetYaxis()->SetTitleOffset(1.1);
    fh2_Muslimap_DTvsDT[12]->GetXaxis()->CenterTitle(true);
    fh2_Muslimap_DTvsDT[12]->GetYaxis()->CenterTitle(true);
    fh2_Muslimap_DTvsDT[12]->GetXaxis()->SetLabelSize(0.045);
    fh2_Muslimap_DTvsDT[12]->GetXaxis()->SetTitleSize(0.045);
    fh2_Muslimap_DTvsDT[12]->GetYaxis()->SetLabelSize(0.045);
    fh2_Muslimap_DTvsDT[12]->GetYaxis()->SetTitleSize(0.045);
    fh2_Muslimap_DTvsDT[12]->SetFillColor(31);
    cMusliMap_DTvsDT8->cd();
    fh2_Muslimap_DTvsDT[12]->Draw("COL");

    // --- Energy vs Drift time --- //
    cMusliMap_EvsDT2 = new TCanvas("Musli_EvsDTmap2", "", 10, 10, 800, 700);
    cMusliMap_EvsDT2->Divide(4, 2);
    for (Int_t j = 0; j < 8; j++)
    {
        sprintf(Name1, "fh1_Musli_EvsDTmap_mean_a%d_a%d", 2 * j + 1, 2 * j + 2);
        sprintf(Name2, "EvsDT A%02d and A%02d if mult==1", 2 * j + 1, 2 * j + 2);
        fh2_Muslimap_EvsDT[j] = new TH2I(Name1, Name2, 1000, 0, 25000, 1280, 0, 64000);
        fh2_Muslimap_EvsDT[j]->GetXaxis()->SetTitle("Drift Time [100ps]");
        fh2_Muslimap_EvsDT[j]->GetYaxis()->SetTitle("Energy [channel]");
        fh2_Muslimap_EvsDT[j]->GetYaxis()->SetTitleOffset(1.1);
        fh2_Muslimap_EvsDT[j]->GetXaxis()->CenterTitle(true);
        fh2_Muslimap_EvsDT[j]->GetYaxis()->CenterTitle(true);
        fh2_Muslimap_EvsDT[j]->GetXaxis()->SetLabelSize(0.045);
        fh2_Muslimap_EvsDT[j]->GetXaxis()->SetTitleSize(0.045);
        fh2_Muslimap_EvsDT[j]->GetYaxis()->SetLabelSize(0.045);
        fh2_Muslimap_EvsDT[j]->GetYaxis()->SetTitleSize(0.045);
        fh2_Muslimap_EvsDT[j]->SetFillColor(31);
        cMusliMap_EvsDT2->cd(j + 1);
        fh2_Muslimap_EvsDT[j]->Draw("COL");
    }

    cMusliMap_EvsDT4 = new TCanvas("Musli_EvsDTmap4", "", 10, 10, 800, 700);
    cMusliMap_EvsDT4->Divide(2, 2);
    for (Int_t j = 0; j < 4; j++)
    {
        sprintf(Name1, "fh1_Musli_EvsDTmap_mean_a%d_to_a%d", j * 4 + 1, j * 4 + 4);
        sprintf(Name2, "EvsDT A%02d to A%02d if mult==1", j * 4 + 1, j * 4 + 5);
        fh2_Muslimap_EvsDT[j + 8] = new TH2I(Name1, Name2, 1000, 0, 25000, 1280, 0, 64000);
        fh2_Muslimap_EvsDT[j + 8]->GetXaxis()->SetTitle("Drift Time [100ps]");
        fh2_Muslimap_EvsDT[j + 8]->GetYaxis()->SetTitle("Energy [channel]");
        fh2_Muslimap_EvsDT[j + 8]->GetYaxis()->SetTitleOffset(1.1);
        fh2_Muslimap_EvsDT[j + 8]->GetXaxis()->CenterTitle(true);
        fh2_Muslimap_EvsDT[j + 8]->GetYaxis()->CenterTitle(true);
        fh2_Muslimap_EvsDT[j + 8]->GetXaxis()->SetLabelSize(0.045);
        fh2_Muslimap_EvsDT[j + 8]->GetXaxis()->SetTitleSize(0.045);
        fh2_Muslimap_EvsDT[j + 8]->GetYaxis()->SetLabelSize(0.045);
        fh2_Muslimap_EvsDT[j + 8]->GetYaxis()->SetTitleSize(0.045);
        fh2_Muslimap_EvsDT[j + 8]->SetFillColor(31);
        cMusliMap_EvsDT4->cd(j + 1);
        fh2_Muslimap_EvsDT[j + 8]->Draw("COL");
    }

    cMusliMap_EvsDT8 = new TCanvas("Musli_EvsDTmap8", "", 10, 10, 800, 700);
    cMusliMap_EvsDT8->Divide(2, 1);
    for (Int_t j = 0; j < 2; j++)
    {
        sprintf(Name1, "fh1_Musli_EvsDTmap_mean_a%d_to_a%d", j * 8 + 1, j * 8 + 8);
        sprintf(Name2, "EvsDT A%02d to A%02d if mult==1", j * 8 + 1, j * 8 + 8);
        fh2_Muslimap_EvsDT[j + 12] = new TH2I(Name1, Name2, 1000, 0, 25000, 1280, 0, 64000);
        fh2_Muslimap_EvsDT[j + 12]->GetXaxis()->SetTitle("Drift Time [100ps]");
        fh2_Muslimap_EvsDT[j + 12]->GetYaxis()->SetTitle("Energy [channel]");
        fh2_Muslimap_EvsDT[j + 12]->GetYaxis()->SetTitleOffset(1.1);
        fh2_Muslimap_EvsDT[j + 12]->GetXaxis()->CenterTitle(true);
        fh2_Muslimap_EvsDT[j + 12]->GetYaxis()->CenterTitle(true);
        fh2_Muslimap_EvsDT[j + 12]->GetXaxis()->SetLabelSize(0.045);
        fh2_Muslimap_EvsDT[j + 12]->GetXaxis()->SetTitleSize(0.045);
        fh2_Muslimap_EvsDT[j + 12]->GetYaxis()->SetLabelSize(0.045);
        fh2_Muslimap_EvsDT[j + 12]->GetYaxis()->SetTitleSize(0.045);
        fh2_Muslimap_EvsDT[j + 12]->SetFillColor(31);
        cMusliMap_EvsDT8->cd(j + 1);
        fh2_Muslimap_EvsDT[j + 12]->Draw("COL");
    }

    cMusliMap_EvsDT16 = new TCanvas("Musli_EvsDTmap16", "", 10, 10, 800, 700);
    sprintf(Name1, "fh1_Musli_EvsDTmap_mean_a1_to_a16");
    sprintf(Name2, "EvsDT A01 to A16 if mult==1");
    fh2_Muslimap_EvsDT[14] = new TH2I(Name1, Name2, 1000, 0, 25000, 1280, 0, 64000);
    fh2_Muslimap_EvsDT[14]->GetXaxis()->SetTitle("Drift Time [100ps]");
    fh2_Muslimap_EvsDT[14]->GetYaxis()->SetTitle("Energy [channel]");
    fh2_Muslimap_EvsDT[14]->GetYaxis()->SetTitleOffset(1.1);
    fh2_Muslimap_EvsDT[14]->GetXaxis()->CenterTitle(true);
    fh2_Muslimap_EvsDT[14]->GetYaxis()->CenterTitle(true);
    fh2_Muslimap_EvsDT[14]->GetXaxis()->SetLabelSize(0.045);
    fh2_Muslimap_EvsDT[14]->GetXaxis()->SetTitleSize(0.045);
    fh2_Muslimap_EvsDT[14]->GetYaxis()->SetLabelSize(0.045);
    fh2_Muslimap_EvsDT[14]->GetYaxis()->SetTitleSize(0.045);
    fh2_Muslimap_EvsDT[14]->SetFillColor(31);
    cMusliMap_EvsDT16->cd();
    fh2_Muslimap_EvsDT[14]->Draw("COL");

    // --- Mean Energy --- //

    cMusliMap_Emean = new TCanvas("Musli_Emean", "", 10, 10, 800, 700);
    cMusliMap_Emean->Divide(3, 3);

    sprintf(Name1, "fh1_Musli_Emean_from_Emap2");
    sprintf(Name2, "Mean Energy from Emap2");
    fh1_Muslimap_Emean2 = new TH1F(Name1, Name2, 64000, 0, 64000);
    fh1_Muslimap_Emean2->GetXaxis()->SetTitle("Energy [channels]");
    fh1_Muslimap_Emean2->GetYaxis()->SetTitle("Counts");
    fh1_Muslimap_Emean2->GetYaxis()->SetTitleOffset(1.1);
    fh1_Muslimap_Emean2->GetXaxis()->CenterTitle(true);
    fh1_Muslimap_Emean2->GetYaxis()->CenterTitle(true);
    fh1_Muslimap_Emean2->GetXaxis()->SetLabelSize(0.045);
    fh1_Muslimap_Emean2->GetXaxis()->SetTitleSize(0.045);
    fh1_Muslimap_Emean2->GetYaxis()->SetLabelSize(0.045);
    fh1_Muslimap_Emean2->GetYaxis()->SetTitleSize(0.045);
    fh1_Muslimap_Emean2->SetFillColor(31);
    cMusliMap_Emean->cd(1);
    fh1_Muslimap_Emean2->Draw("");

    sprintf(Name1, "fh1_Musli_Emean_from_Emap4");
    sprintf(Name2, "Mean Energy from Emap4");
    fh1_Muslimap_Emean4 = new TH1F(Name1, Name2, 64000, 0, 64000);
    fh1_Muslimap_Emean4->GetXaxis()->SetTitle("Energy [channels]");
    fh1_Muslimap_Emean4->GetYaxis()->SetTitle("Counts");
    fh1_Muslimap_Emean4->GetYaxis()->SetTitleOffset(1.1);
    fh1_Muslimap_Emean4->GetXaxis()->CenterTitle(true);
    fh1_Muslimap_Emean4->GetYaxis()->CenterTitle(true);
    fh1_Muslimap_Emean4->GetXaxis()->SetLabelSize(0.045);
    fh1_Muslimap_Emean4->GetXaxis()->SetTitleSize(0.045);
    fh1_Muslimap_Emean4->GetYaxis()->SetLabelSize(0.045);
    fh1_Muslimap_Emean4->GetYaxis()->SetTitleSize(0.045);
    fh1_Muslimap_Emean4->SetFillColor(31);
    cMusliMap_Emean->cd(2);
    fh1_Muslimap_Emean4->Draw("");

    sprintf(Name1, "fh1_Musli_Emean_from_Emap8");
    sprintf(Name2, "Mean Energy from Emap8");
    fh1_Muslimap_Emean8 = new TH1F(Name1, Name2, 64000, 0, 64000);
    fh1_Muslimap_Emean8->GetXaxis()->SetTitle("Energy [channels]");
    fh1_Muslimap_Emean8->GetYaxis()->SetTitle("Counts");
    fh1_Muslimap_Emean8->GetYaxis()->SetTitleOffset(1.1);
    fh1_Muslimap_Emean8->GetXaxis()->CenterTitle(true);
    fh1_Muslimap_Emean8->GetYaxis()->CenterTitle(true);
    fh1_Muslimap_Emean8->GetXaxis()->SetLabelSize(0.045);
    fh1_Muslimap_Emean8->GetXaxis()->SetTitleSize(0.045);
    fh1_Muslimap_Emean8->GetYaxis()->SetLabelSize(0.045);
    fh1_Muslimap_Emean8->GetYaxis()->SetTitleSize(0.045);
    fh1_Muslimap_Emean8->SetFillColor(31);
    cMusliMap_Emean->cd(3);
    fh1_Muslimap_Emean8->Draw("");

    sprintf(Name1, "fh2_Musli_Emean2_vs_Emean4");
    sprintf(Name2, "Emean2 vs Emean4");
    fh2_Muslimap_Emean2vsEmean4 = new TH2F(Name1, Name2, 1280, 0, 64000, 1280, 0, 64000);
    fh2_Muslimap_Emean2vsEmean4->GetXaxis()->SetTitle("Energy [channels]");
    fh2_Muslimap_Emean2vsEmean4->GetYaxis()->SetTitle("Energy [channels]");
    fh2_Muslimap_Emean2vsEmean4->GetYaxis()->SetTitleOffset(1.1);
    fh2_Muslimap_Emean2vsEmean4->GetXaxis()->CenterTitle(true);
    fh2_Muslimap_Emean2vsEmean4->GetYaxis()->CenterTitle(true);
    fh2_Muslimap_Emean2vsEmean4->GetXaxis()->SetLabelSize(0.045);
    fh2_Muslimap_Emean2vsEmean4->GetXaxis()->SetTitleSize(0.045);
    fh2_Muslimap_Emean2vsEmean4->GetYaxis()->SetLabelSize(0.045);
    fh2_Muslimap_Emean2vsEmean4->GetYaxis()->SetTitleSize(0.045);
    fh2_Muslimap_Emean2vsEmean4->SetFillColor(31);
    cMusliMap_Emean->cd(4);
    fh2_Muslimap_Emean2vsEmean4->Draw("COL");

    sprintf(Name1, "fh2_Musli_Emean2_vs_Emean8");
    sprintf(Name2, "Emean2 vs Emean8");
    fh2_Muslimap_Emean2vsEmean8 = new TH2F(Name1, Name2, 1280, 0, 64000, 1280, 0, 64000);
    fh2_Muslimap_Emean2vsEmean8->GetXaxis()->SetTitle("Energy [channels]");
    fh2_Muslimap_Emean2vsEmean8->GetYaxis()->SetTitle("Energy [channels]");
    fh2_Muslimap_Emean2vsEmean8->GetYaxis()->SetTitleOffset(1.1);
    fh2_Muslimap_Emean2vsEmean8->GetXaxis()->CenterTitle(true);
    fh2_Muslimap_Emean2vsEmean8->GetYaxis()->CenterTitle(true);
    fh2_Muslimap_Emean2vsEmean8->GetXaxis()->SetLabelSize(0.045);
    fh2_Muslimap_Emean2vsEmean8->GetXaxis()->SetTitleSize(0.045);
    fh2_Muslimap_Emean2vsEmean8->GetYaxis()->SetLabelSize(0.045);
    fh2_Muslimap_Emean2vsEmean8->GetYaxis()->SetTitleSize(0.045);
    fh2_Muslimap_Emean2vsEmean8->SetFillColor(31);
    cMusliMap_Emean->cd(5);
    fh2_Muslimap_Emean2vsEmean8->Draw("COL");

    sprintf(Name1, "fh2_Musli_Emean2_vs_E16");
    sprintf(Name2, "Emean2 vs E16");
    fh2_Muslimap_Emean2vsE16 = new TH2F(Name1, Name2, 1280, 0, 64000, 1280, 0, 64000);
    fh2_Muslimap_Emean2vsE16->GetXaxis()->SetTitle("Energy [channels]");
    fh2_Muslimap_Emean2vsE16->GetYaxis()->SetTitle("Energy [channels]");
    fh2_Muslimap_Emean2vsE16->GetYaxis()->SetTitleOffset(1.1);
    fh2_Muslimap_Emean2vsE16->GetXaxis()->CenterTitle(true);
    fh2_Muslimap_Emean2vsE16->GetYaxis()->CenterTitle(true);
    fh2_Muslimap_Emean2vsE16->GetXaxis()->SetLabelSize(0.045);
    fh2_Muslimap_Emean2vsE16->GetXaxis()->SetTitleSize(0.045);
    fh2_Muslimap_Emean2vsE16->GetYaxis()->SetLabelSize(0.045);
    fh2_Muslimap_Emean2vsE16->GetYaxis()->SetTitleSize(0.045);
    fh2_Muslimap_Emean2vsE16->SetFillColor(31);
    cMusliMap_Emean->cd(6);
    fh2_Muslimap_Emean2vsE16->Draw("COL");

    sprintf(Name1, "fh2_Musli_Emean4_vs_Emean8");
    sprintf(Name2, "Emean4 vs Emean8");
    fh2_Muslimap_Emean4vsEmean8 = new TH2F(Name1, Name2, 1280, 0, 64000, 1280, 0, 64000);
    fh2_Muslimap_Emean4vsEmean8->GetXaxis()->SetTitle("Energy [channels]");
    fh2_Muslimap_Emean4vsEmean8->GetYaxis()->SetTitle("Energy [channels]");
    fh2_Muslimap_Emean4vsEmean8->GetYaxis()->SetTitleOffset(1.1);
    fh2_Muslimap_Emean4vsEmean8->GetXaxis()->CenterTitle(true);
    fh2_Muslimap_Emean4vsEmean8->GetYaxis()->CenterTitle(true);
    fh2_Muslimap_Emean4vsEmean8->GetXaxis()->SetLabelSize(0.045);
    fh2_Muslimap_Emean4vsEmean8->GetXaxis()->SetTitleSize(0.045);
    fh2_Muslimap_Emean4vsEmean8->GetYaxis()->SetLabelSize(0.045);
    fh2_Muslimap_Emean4vsEmean8->GetYaxis()->SetTitleSize(0.045);
    fh2_Muslimap_Emean4vsEmean8->SetFillColor(31);
    cMusliMap_Emean->cd(7);
    fh2_Muslimap_Emean4vsEmean8->Draw("COL");

    sprintf(Name1, "fh2_Musli_Emean4_vs_E16");
    sprintf(Name2, "Emean4 vs E16");
    fh2_Muslimap_Emean4vsE16 = new TH2F(Name1, Name2, 1280, 0, 64000, 1280, 0, 64000);
    fh2_Muslimap_Emean4vsE16->GetXaxis()->SetTitle("Energy [channels]");
    fh2_Muslimap_Emean4vsE16->GetYaxis()->SetTitle("Energy [channels]");
    fh2_Muslimap_Emean4vsE16->GetYaxis()->SetTitleOffset(1.1);
    fh2_Muslimap_Emean4vsE16->GetXaxis()->CenterTitle(true);
    fh2_Muslimap_Emean4vsE16->GetYaxis()->CenterTitle(true);
    fh2_Muslimap_Emean4vsE16->GetXaxis()->SetLabelSize(0.045);
    fh2_Muslimap_Emean4vsE16->GetXaxis()->SetTitleSize(0.045);
    fh2_Muslimap_Emean4vsE16->GetYaxis()->SetLabelSize(0.045);
    fh2_Muslimap_Emean4vsE16->GetYaxis()->SetTitleSize(0.045);
    fh2_Muslimap_Emean4vsE16->SetFillColor(31);
    cMusliMap_Emean->cd(8);
    fh2_Muslimap_Emean4vsE16->Draw("COL");

    sprintf(Name1, "fh2_Musli_Emean8_vs_E16");
    sprintf(Name2, "Emean8 vs E16");
    fh2_Muslimap_Emean8vsE16 = new TH2F(Name1, Name2, 1280, 0, 64000, 1280, 0, 64000);
    fh2_Muslimap_Emean8vsE16->GetXaxis()->SetTitle("Energy [channels]");
    fh2_Muslimap_Emean8vsE16->GetYaxis()->SetTitle("Energy [channels]");
    fh2_Muslimap_Emean8vsE16->GetYaxis()->SetTitleOffset(1.1);
    fh2_Muslimap_Emean8vsE16->GetXaxis()->CenterTitle(true);
    fh2_Muslimap_Emean8vsE16->GetYaxis()->CenterTitle(true);
    fh2_Muslimap_Emean8vsE16->GetXaxis()->SetLabelSize(0.045);
    fh2_Muslimap_Emean8vsE16->GetXaxis()->SetTitleSize(0.045);
    fh2_Muslimap_Emean8vsE16->GetYaxis()->SetLabelSize(0.045);
    fh2_Muslimap_Emean8vsE16->GetYaxis()->SetTitleSize(0.045);
    fh2_Muslimap_Emean8vsE16->SetFillColor(31);
    cMusliMap_Emean->cd(9);
    fh2_Muslimap_Emean8vsE16->Draw("COL");

    // --- DeltaDT --- //

    cMusliMap_DeltaDT = new TCanvas("Musli_DeltaDT", "", 10, 10, 800, 700);

    sprintf(Name1, "fh1_Musli_DeltaDT");
    sprintf(Name2, "Delta DTmap2 between first and last pair ");
    fh1_Muslimap_DeltaDT = new TH1I(Name1, Name2, 3000, -1500, 1500);
    fh1_Muslimap_DeltaDT->GetXaxis()->SetTitle("Delta Drift Time [100ps]");
    fh1_Muslimap_DeltaDT->GetYaxis()->SetTitle("Counts");
    fh1_Muslimap_DeltaDT->GetYaxis()->SetTitleOffset(1.1);
    fh1_Muslimap_DeltaDT->GetXaxis()->CenterTitle(true);
    fh1_Muslimap_DeltaDT->GetYaxis()->CenterTitle(true);
    fh1_Muslimap_DeltaDT->GetXaxis()->SetLabelSize(0.045);
    fh1_Muslimap_DeltaDT->GetXaxis()->SetTitleSize(0.045);
    fh1_Muslimap_DeltaDT->GetYaxis()->SetLabelSize(0.045);
    fh1_Muslimap_DeltaDT->GetYaxis()->SetTitleSize(0.045);
    fh1_Muslimap_DeltaDT->SetFillColor(31);
    cMusliMap_DeltaDT->cd();
    fh1_Muslimap_DeltaDT->Draw("");

    // === ================= === //
    // === MAIN FOLDER-Musli === //
    // === ================= === //

    TFolder* mainfolMusli = new TFolder("MusicLightIon", "Music Light Ion info");

    mainfolMusli->Add(cMusliMap_Mult);

    mainfolMusli->Add(cMusliMap_E2);
    mainfolMusli->Add(cMusliMap_E4);
    mainfolMusli->Add(cMusliMap_E8);
    mainfolMusli->Add(cMusliMap_E16);

    mainfolMusli->Add(cMusliMap_EvsE2);
    mainfolMusli->Add(cMusliMap_EvsE4);
    mainfolMusli->Add(cMusliMap_EvsE8);

    mainfolMusli->Add(cMusliMap_DT2);
    mainfolMusli->Add(cMusliMap_DT4);
    mainfolMusli->Add(cMusliMap_DT8);
    mainfolMusli->Add(cMusliMap_DT16);

    mainfolMusli->Add(cMusliMap_DTvsDT2);
    mainfolMusli->Add(cMusliMap_DTvsDT4);
    mainfolMusli->Add(cMusliMap_DTvsDT8);

    mainfolMusli->Add(cMusliMap_EvsDT2);
    mainfolMusli->Add(cMusliMap_EvsDT4);
    mainfolMusli->Add(cMusliMap_EvsDT8);
    mainfolMusli->Add(cMusliMap_EvsDT16);

    mainfolMusli->Add(cMusliMap_Emean);

    mainfolMusli->Add(cMusliMap_DeltaDT);

    run->AddObject(mainfolMusli);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_MusLI_HIST", Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

void R3BMusliOnlineSpectra::Reset_Histo()
{
    LOG(INFO) << "R3BMusliOnlineSpectra::Reset_Histo";

    // --- Reset Histo from mapped level --- //
    fh1_Muslimap_mult->Reset();
    fh2_Muslimap_mult->Reset();
    for (Int_t j = 0; j < 15; j++)
    {
        fh1_Muslimap_E[j]->Reset();
        fh1_Muslimap_DT[j]->Reset();
        fh2_Muslimap_EvsDT[j]->Reset();
    }
    for (Int_t j = 0; j < 13; j++)
    {
        if (j == 7 || j == 11)
            continue;
        fh2_Muslimap_EvsE[j]->Reset();
        fh2_Muslimap_DTvsDT[j]->Reset();
    }
    fh1_Muslimap_Emean2->Reset();
    fh1_Muslimap_Emean4->Reset();
    fh1_Muslimap_Emean8->Reset();
    fh2_Muslimap_Emean2vsEmean4->Reset();
    fh2_Muslimap_Emean2vsEmean8->Reset();
    fh2_Muslimap_Emean2vsE16->Reset();
    fh2_Muslimap_Emean4vsEmean8->Reset();
    fh2_Muslimap_Emean4vsE16->Reset();
    fh2_Muslimap_Emean8vsE16->Reset();
    fh1_Muslimap_DeltaDT->Reset();
}

void R3BMusliOnlineSpectra::Exec(Option_t* option)
{
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(FATAL) << "R3BMusliOnlineSpectra::Exec FairRootManager not found";

    // Local variables for mapped data
    UInt_t mult[18], e[15], t[18];
    UInt_t rank;
    UInt_t tref = 0;
    Int_t dt[15];
    Float_t emean2 = 0;
    Float_t emean4 = 0;
    Float_t emean8 = 0;
    UShort_t cptemean2 = 0;
    UShort_t cptemean4 = 0;
    UShort_t cptemean8 = 0;
    for (UShort_t i = 0; i < 15; i++)
    {
        mult[i] = 0;
        e[i] = 0;
        t[i] = 0;
        dt[i] = 0;
    }
    for (UShort_t i = 15; i < 18; i++)
    {
        mult[i] = 0;
        t[i] = 0;
    }

    // mapped data
    if (fMapItemsMusli && fMapItemsMusli->GetEntriesFast() > 0)
    {
        // read mapped data
        Int_t nHits = fMapItemsMusli->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BMusliMappedData* hit = (R3BMusliMappedData*)fMapItemsMusli->At(ihit);
            if (!hit)
                continue;
            rank = hit->GetSignal() - 1;
            mult[rank]++;
            e[rank] = hit->GetEnergy();
            t[rank] = hit->GetTime();
        }

        if (mult[16] == 1)
            tref = t[16];

        // calculate drift time and fill mapped data histo
        for (UShort_t i = 0; i < 15; i++)
        {
            if (mult[i] == 1)
            {
                fh1_Muslimap_mult->Fill(i + 1);
                fh1_Muslimap_E[i]->Fill(e[i]);
                if (tref > 0)
                {
                    dt[i] = t[i] - tref;
                    fh1_Muslimap_DT[i]->Fill(dt[i]);
                    fh2_Muslimap_EvsDT[i]->Fill(dt[i], e[i]);
                }
            }
            fh2_Muslimap_mult->Fill(i + 1, mult[i]);
        }
        for (UShort_t i = 15; i < 18; i++)
        {
            if (mult[i] == 1)
            {
                fh1_Muslimap_mult->Fill(i + 1);
            }
            fh2_Muslimap_mult->Fill(i + 1, mult[i]);
        }
        for (UShort_t i = 0; i < 13; i++)
        {
            if (i == 7 || i == 11)
                continue;
            if (mult[i] == 1 && mult[i + 1] == 1)
            {
                fh2_Muslimap_EvsE[i]->Fill(e[i + 1], e[i]);
                if (tref > 0)
                {
                    fh2_Muslimap_DTvsDT[i]->Fill(dt[i + 1], dt[i]);
                }
            }
        }
        if (mult[0] == 1 && mult[7] == 1 && dt[0] > 0 && dt[7] > 0)
        {
            fh1_Muslimap_DeltaDT->Fill(dt[7] - dt[0]);
        }

        // calculate the mean energy from the different type of data
        // fType = 2
        for (UShort_t i = 0; i < 8; i++)
        {
            if (mult[i] == 1)
            {
                emean2 += e[i];
                cptemean2++;
            }
        }
        if (cptemean2 == 8)
        {
            emean2 = emean2 / 8.;
            fh1_Muslimap_Emean2->Fill(emean2);
            if (mult[14] == 1)
                fh2_Muslimap_Emean2vsE16->Fill(e[14], emean2);
        }
        else
            emean2 = 0.;
        // fType = 4
        for (UShort_t i = 8; i < 12; i++)
        {
            if (mult[i] == 1)
            {
                emean4 += e[i];
                cptemean4++;
            }
        }
        if (cptemean4 == 4)
        {
            emean4 = emean4 / 4.;
            fh1_Muslimap_Emean4->Fill(emean4);
            if (emean2 > 0)
                fh2_Muslimap_Emean2vsEmean4->Fill(emean4, emean2);
            if (mult[14] == 1)
                fh2_Muslimap_Emean4vsE16->Fill(e[14], emean4);
        }
        else
            emean4 = 0.;
        // fType = 8
        for (UShort_t i = 12; i < 14; i++)
        {
            if (mult[i] == 1)
            {
                emean8 += e[i];
                cptemean8++;
            }
        }
        if (cptemean8 == 2)
        {
            emean8 = emean8 / 2.;
            fh1_Muslimap_Emean8->Fill(emean8);
            if (emean2 > 0)
                fh2_Muslimap_Emean2vsEmean8->Fill(emean8, emean2);
            if (emean4 > 0)
                fh2_Muslimap_Emean4vsEmean8->Fill(emean8, emean4);
            if (mult[14] == 1)
                fh2_Muslimap_Emean8vsE16->Fill(e[14], emean8);
        }
        else
            emean8 = 0.;

    } // end of if(fMapItemsMusli && fMapItemsMusli->GetEntriesFast()>0)

    fNEvents += 1;
}

void R3BMusliOnlineSpectra::FinishEvent()
{
    if (fMapItemsMusli)
    {
        fMapItemsMusli->Clear();
    }
}

void R3BMusliOnlineSpectra::FinishTask()
{

    if (fMapItemsMusli)
    {
        cMusliMap_Mult->Write();

        cMusliMap_E2->Write();
        cMusliMap_E4->Write();
        cMusliMap_E8->Write();
        cMusliMap_E16->Write();

        cMusliMap_EvsE2->Write();
        cMusliMap_EvsE4->Write();
        cMusliMap_EvsE8->Write();

        cMusliMap_DT2->Write();
        cMusliMap_DT4->Write();
        cMusliMap_DT8->Write();
        cMusliMap_DT16->Write();

        cMusliMap_DTvsDT2->Write();
        cMusliMap_DTvsDT4->Write();
        cMusliMap_DTvsDT8->Write();

        cMusliMap_EvsDT2->Write();
        cMusliMap_EvsDT4->Write();
        cMusliMap_EvsDT8->Write();
        cMusliMap_EvsDT16->Write();

        cMusliMap_Emean->Write();

        cMusliMap_DeltaDT->Write();
    }
}

ClassImp(R3BMusliOnlineSpectra)
