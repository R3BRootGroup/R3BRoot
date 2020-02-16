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
// -----                  R3BMusicOnlineSpectra           -----
// -----    Created 29/09/19  by J.L. Rodriguez-Sanchez   -----
// -----           Fill MUSIC online histograms           -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with MUSIC online data
 */

#include "R3BMusicOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BMusicCalData.h"
#include "R3BMusicHitData.h"
#include "R3BMusicMappedData.h"
#include "R3BSofMwpcHitData.h"
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

R3BMusicOnlineSpectra::R3BMusicOnlineSpectra()
    : FairTask("R3BMusicOnlineSpectra", 1)
    , fMappedItemsMus(NULL)
    , fCalItemsMus(NULL)
    , fHitItemsMus(NULL)
    , fHitItemsMwpc0(NULL)
    , fNEvents(0)
{
}

R3BMusicOnlineSpectra::R3BMusicOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItemsMus(NULL)
    , fCalItemsMus(NULL)
    , fHitItemsMus(NULL)
    , fHitItemsMwpc0(NULL)
    , fNEvents(0)
{
}

R3BMusicOnlineSpectra::~R3BMusicOnlineSpectra()
{
    LOG(INFO) << "R3BMusicOnlineSpectra::Delete instance";
    if (fMappedItemsMus)
        delete fMappedItemsMus;
    if (fCalItemsMus)
        delete fCalItemsMus;
    if (fHitItemsMus)
        delete fHitItemsMus;
    if (fHitItemsMwpc0)
	delete fHitItemsMwpc0;
}

InitStatus R3BMusicOnlineSpectra::Init()
{

    LOG(INFO) << "R3BMusicOnlineSpectra::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(FATAL) << "R3BMusicOnlineSpectra::Init FairRootManager not found";
    // header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // get access to mapped data of the MUSIC
    fMappedItemsMus = (TClonesArray*)mgr->GetObject("MusicMappedData");
    if (!fMappedItemsMus)
    {
        return kFATAL;
    }

    // get access to cal data of the MUSIC
    fCalItemsMus = (TClonesArray*)mgr->GetObject("MusicCalData");
    if (!fCalItemsMus)
        LOG(WARNING) << "R3BMusicOnlineSpectra: MusicCalData not found";

    // get access to hit data of the MUSIC
    fHitItemsMus = (TClonesArray*)mgr->GetObject("MusicHitData");
    if (!fHitItemsMus)
        LOG(WARNING) << "R3BMusicOnlineSpectra: MusicHitData not found";

    // get access to the hit data of the Mwpc0
    fHitItemsMwpc0 = (TClonesArray*)mgr->GetObject("Mwpc0HitData");
    if (!fHitItemsMwpc0)
	LOG(WARNING) << "R3BMusicOnlineSpectra: Mwpc0HitData not found";
    
    // Create histograms for detectors
    char Name1[255];
    char Name2[255];

    // MUSIC: Map data
    cMusMap_E = new TCanvas("Mus_Emap", "", 10, 10, 800, 700);
    cMusMap_E->Divide(2, 4);

    for (Int_t j = 0; j < NbAnodesMus; j++)
    {
        sprintf(Name1, "fh1_Mus_Emap_a%d", j + 1);
        sprintf(Name2, "Anode %d", j + 1);
        fh1_Musmap_E[j] = new TH1F(Name1, Name2, 8192, 0, 8192);
        fh1_Musmap_E[j]->GetXaxis()->SetTitle("Energy [channels]");
        fh1_Musmap_E[j]->GetYaxis()->SetTitle("Counts");
        fh1_Musmap_E[j]->GetYaxis()->SetTitleOffset(1.1);
        fh1_Musmap_E[j]->GetXaxis()->CenterTitle(true);
        fh1_Musmap_E[j]->GetYaxis()->CenterTitle(true);
        fh1_Musmap_E[j]->GetXaxis()->SetLabelSize(0.045);
        fh1_Musmap_E[j]->GetXaxis()->SetTitleSize(0.045);
        fh1_Musmap_E[j]->GetYaxis()->SetLabelSize(0.045);
        fh1_Musmap_E[j]->GetYaxis()->SetTitleSize(0.045);
        fh1_Musmap_E[j]->SetFillColor(31);
        cMusMap_E->cd(j + 1);
        fh1_Musmap_E[j]->Draw("");
    }

    // Energy vs Drift-times
    cMusMap_EvsDT = new TCanvas("Mus_EvsDT", "", 10, 10, 800, 700);
    cMusMap_EvsDT->Divide(2, 4);

    for (Int_t j = 0; j < NbAnodesMus; j++)
    {
        sprintf(Name1, "fh1_Mus_EvsDT_a%d", j + 1);
        sprintf(Name2, "Anode %d", j + 1);
        fh2_Mus_EneRawVsDriftTime[j] = new TH2F(Name1, Name2, 1000, 0, 8192, 1000, -30000, 30000);
        fh2_Mus_EneRawVsDriftTime[j]->GetXaxis()->SetTitle("Energy [channels]");
        fh2_Mus_EneRawVsDriftTime[j]->GetYaxis()->SetTitle("Drift time [channels]");
        fh2_Mus_EneRawVsDriftTime[j]->GetYaxis()->SetTitleOffset(1.1);
        fh2_Mus_EneRawVsDriftTime[j]->GetXaxis()->CenterTitle(true);
        fh2_Mus_EneRawVsDriftTime[j]->GetYaxis()->CenterTitle(true);
        fh2_Mus_EneRawVsDriftTime[j]->GetXaxis()->SetLabelSize(0.045);
        fh2_Mus_EneRawVsDriftTime[j]->GetXaxis()->SetTitleSize(0.045);
        fh2_Mus_EneRawVsDriftTime[j]->GetYaxis()->SetLabelSize(0.045);
        fh2_Mus_EneRawVsDriftTime[j]->GetYaxis()->SetTitleSize(0.045);
        cMusMap_EvsDT->cd(j + 1);
        fh2_Mus_EneRawVsDriftTime[j]->Draw("col");
    }

    cMus_EsumvsDT = new TCanvas("Mus_EsumvsDT6", "Music: Esum vs DT-anode 6", 10, 10, 800, 700);
    fh2_Mus_ESum_vs_DT =
        new TH2F("fh2_Mus_EsumVsDT6", "Esum versus Drift Time anode 6", 3000, 5000, 25000, 4100, 0, 8192);
    fh2_Mus_ESum_vs_DT->GetXaxis()->SetTitle("Drift time [channels]");
    fh2_Mus_ESum_vs_DT->GetYaxis()->SetTitle("Energy [channels]");
    fh2_Mus_ESum_vs_DT->GetYaxis()->SetTitleOffset(1.1);
    fh2_Mus_ESum_vs_DT->GetXaxis()->CenterTitle(true);
    fh2_Mus_ESum_vs_DT->GetYaxis()->CenterTitle(true);
    fh2_Mus_ESum_vs_DT->GetXaxis()->SetLabelSize(0.045);
    fh2_Mus_ESum_vs_DT->GetXaxis()->SetTitleSize(0.045);
    fh2_Mus_ESum_vs_DT->GetYaxis()->SetLabelSize(0.045);
    fh2_Mus_ESum_vs_DT->GetYaxis()->SetTitleSize(0.045);
    fh2_Mus_ESum_vs_DT->Draw("col");

    // Drift-times
    cMusMap_DT = new TCanvas("Mus_DTmap", "", 10, 10, 800, 700);
    cMusMap_DT->Divide(2, 4);
    cMusDTCorMwpc0 = new TCanvas("MusDT_Mwpc0","Music: Correlation of DT with MWPC0 (X,Y)",10,10,800,700);
    cMusDTCorMwpc0->Divide(3,3);

    for (Int_t j = 0; j < NbAnodesMus; j++)
    {
        sprintf(Name1, "fh1_Mus_DTmap_a%d", j + 1);
        sprintf(Name2, "Anode %d", j + 1);
        fh1_Musmap_DT[j] = new TH1F(Name1, Name2, 6000, -30000, 30000);
        fh1_Musmap_DT[j]->GetXaxis()->SetTitle("Drift time [channels]");
        fh1_Musmap_DT[j]->GetYaxis()->SetTitle("Counts");
        fh1_Musmap_DT[j]->GetYaxis()->SetTitleOffset(1.1);
        fh1_Musmap_DT[j]->GetXaxis()->CenterTitle(true);
        fh1_Musmap_DT[j]->GetYaxis()->CenterTitle(true);
        fh1_Musmap_DT[j]->GetXaxis()->SetLabelSize(0.045);
        fh1_Musmap_DT[j]->GetXaxis()->SetTitleSize(0.045);
        fh1_Musmap_DT[j]->GetYaxis()->SetLabelSize(0.045);
        fh1_Musmap_DT[j]->GetYaxis()->SetTitleSize(0.045);
        fh1_Musmap_DT[j]->SetFillColor(31);
        cMusMap_DT->cd(j + 1);
        fh1_Musmap_DT[j]->Draw("");

	sprintf(Name1, "fh2_MusDT%d_vs_X0",j+1);
	sprintf(Name2, "Music: DRIFT TIME ANODE %d versus MWPC0-X",j+1);
	fh2_MusCorMwpc0_DTvsX0[j] = new TH2F(Name1,Name2,1000,-10,10,1500,0,30000);
        fh2_MusCorMwpc0_DTvsX0[j]->GetYaxis()->SetTitle("Drift time [channels]");
        fh2_MusCorMwpc0_DTvsX0[j]->GetXaxis()->SetTitle("Mwpc0-X [mm]");
        fh2_MusCorMwpc0_DTvsX0[j]->GetYaxis()->SetTitleOffset(1.1);
        fh2_MusCorMwpc0_DTvsX0[j]->GetXaxis()->CenterTitle(true);
        fh2_MusCorMwpc0_DTvsX0[j]->GetYaxis()->CenterTitle(true);
        fh2_MusCorMwpc0_DTvsX0[j]->GetXaxis()->SetLabelSize(0.045);
        fh2_MusCorMwpc0_DTvsX0[j]->GetXaxis()->SetTitleSize(0.045);
        fh2_MusCorMwpc0_DTvsX0[j]->GetYaxis()->SetLabelSize(0.045);
        fh2_MusCorMwpc0_DTvsX0[j]->GetYaxis()->SetTitleSize(0.045);
	cMusDTCorMwpc0->cd(j+1);
	fh2_MusCorMwpc0_DTvsX0[j]->Draw("col");
    }

    // Cal data, position in mm for each anode
    cMusCal_Pos = new TCanvas("Mus_Pos-X", "Pos-X in mm", 10, 10, 800, 700);
    cMusCal_Pos->Divide(2, 4);
    for (Int_t j = 0; j < NbAnodesMus; j++)
    {
        sprintf(Name1, "fh1_Mus_Pos_a%d", j + 1);
        sprintf(Name2, "Anode %d", j + 1);
        fh1_Muscal_Pos[j] = new TH1F(Name1, Name2, 800, -100., 100.);
        fh1_Muscal_Pos[j]->GetXaxis()->SetTitle("Position-X [mm]");
        fh1_Muscal_Pos[j]->GetYaxis()->SetTitle("Counts");
        fh1_Muscal_Pos[j]->GetYaxis()->SetTitleOffset(1.1);
        fh1_Muscal_Pos[j]->GetXaxis()->CenterTitle(true);
        fh1_Muscal_Pos[j]->GetYaxis()->CenterTitle(true);
        fh1_Muscal_Pos[j]->GetXaxis()->SetLabelSize(0.045);
        fh1_Muscal_Pos[j]->GetXaxis()->SetTitleSize(0.045);
        fh1_Muscal_Pos[j]->GetYaxis()->SetLabelSize(0.045);
        fh1_Muscal_Pos[j]->GetYaxis()->SetTitleSize(0.045);
        fh1_Muscal_Pos[j]->SetFillColor(31);
        fh1_Muscal_Pos[j]->SetLineColor(1);
        cMusCal_Pos->cd(j + 1);
        fh1_Muscal_Pos[j]->Draw("");
    }

    // Comparison of drift-times
    char NamAnode1[255];
    char NamAnode2[255];
    cMusMap_DTvsDT = new TCanvas("Mus_DTvsDT", "Mus_DTvsDT", 10, 10, 800, 700);
    cMusMap_DTvsDT->Divide(2, 4);
    for (Int_t j = 0; j < NbAnodesMus - 1; j++)
    {
        sprintf(Name1, "fh2_Mus_DTvsDT_a%dvs%d", j + 1, j + 2);
        sprintf(Name2, "Anode-%d vs Anode-%d", j + 1, j + 2);
        fh2_Mus_DTvsDT[j] = new TH2F(Name1, Name2, 1000, 0, 30000, 1000, 0, 30000);
        sprintf(NamAnode1, "Drift time %d [channels]", j + 1);
        sprintf(NamAnode2, "Drift time %d [channels]", j + 2);
        fh2_Mus_DTvsDT[j]->GetXaxis()->SetTitle(NamAnode1);
        fh2_Mus_DTvsDT[j]->GetYaxis()->SetTitle(NamAnode2);
        fh2_Mus_DTvsDT[j]->GetYaxis()->SetTitleOffset(1.1);
        fh2_Mus_DTvsDT[j]->GetXaxis()->CenterTitle(true);
        fh2_Mus_DTvsDT[j]->GetYaxis()->CenterTitle(true);
        fh2_Mus_DTvsDT[j]->GetXaxis()->SetLabelSize(0.045);
        fh2_Mus_DTvsDT[j]->GetXaxis()->SetTitleSize(0.045);
        fh2_Mus_DTvsDT[j]->GetYaxis()->SetLabelSize(0.045);
        fh2_Mus_DTvsDT[j]->GetYaxis()->SetTitleSize(0.045);
        cMusMap_DTvsDT->cd(j + 1);
        fh2_Mus_DTvsDT[j]->Draw("col");
    }

    // Sum of energies in the anodes
    cMusMap_ESum = new TCanvas("Mus_ESum_1,2", "Mus_ESum_1,2", 10, 10, 800, 700);
    cMusMap_ESum->Divide(1, 2);
    cMusMap_ESum->cd(1);
    fh1_Mus_ESum[0] = new TH1F("fh1_Mus_ESum1", "Music:ESum:first", 8192, 0, 8192);
    fh1_Mus_ESum[0]->GetXaxis()->SetTitle("Energy [channels]");
    fh1_Mus_ESum[0]->GetYaxis()->SetTitle("Counts");
    fh1_Mus_ESum[0]->GetYaxis()->SetTitleOffset(1.1);
    fh1_Mus_ESum[0]->GetXaxis()->CenterTitle(true);
    fh1_Mus_ESum[0]->GetYaxis()->CenterTitle(true);
    fh1_Mus_ESum[0]->GetXaxis()->SetLabelSize(0.045);
    fh1_Mus_ESum[0]->GetXaxis()->SetTitleSize(0.045);
    fh1_Mus_ESum[0]->GetYaxis()->SetLabelSize(0.045);
    fh1_Mus_ESum[0]->GetYaxis()->SetTitleSize(0.045);
    fh1_Mus_ESum[0]->SetFillColor(31);
    fh1_Mus_ESum[0]->Draw("");
    cMusMap_ESum->cd(2);
    fh1_Mus_ESum[1] = new TH1F("fh1_Mus_ESum2", "Music:ESum:Second", 8192, 0, 8192);
    fh1_Mus_ESum[1]->GetXaxis()->SetTitle("Energy [channels]");
    fh1_Mus_ESum[1]->GetYaxis()->SetTitle("Counts");
    fh1_Mus_ESum[1]->GetYaxis()->SetTitleOffset(1.1);
    fh1_Mus_ESum[1]->GetXaxis()->CenterTitle(true);
    fh1_Mus_ESum[1]->GetYaxis()->CenterTitle(true);
    fh1_Mus_ESum[1]->GetXaxis()->SetLabelSize(0.045);
    fh1_Mus_ESum[1]->GetXaxis()->SetTitleSize(0.045);
    fh1_Mus_ESum[1]->GetYaxis()->SetLabelSize(0.045);
    fh1_Mus_ESum[1]->GetYaxis()->SetTitleSize(0.045);
    fh1_Mus_ESum[1]->SetFillColor(31);
    fh1_Mus_ESum[1]->Draw("");

    cMusMap_ESum1 = new TCanvas("Mus_ESum", "Mus_ESum", 10, 10, 800, 700);
    fh1_Mus_ESum[2] = new TH1F("fh1_tMus_ESum", "Music: ESum", 8192, 0, 8192);
    fh1_Mus_ESum[2]->GetXaxis()->SetTitle("Energy [channels]");
    fh1_Mus_ESum[2]->GetYaxis()->SetTitle("Counts");
    fh1_Mus_ESum[2]->GetYaxis()->SetTitleOffset(1.1);
    fh1_Mus_ESum[2]->GetXaxis()->CenterTitle(true);
    fh1_Mus_ESum[2]->GetYaxis()->CenterTitle(true);
    fh1_Mus_ESum[2]->GetXaxis()->SetLabelSize(0.045);
    fh1_Mus_ESum[2]->GetXaxis()->SetTitleSize(0.045);
    fh1_Mus_ESum[2]->GetYaxis()->SetLabelSize(0.045);
    fh1_Mus_ESum[2]->GetYaxis()->SetTitleSize(0.045);
    fh1_Mus_ESum[2]->SetFillColor(32);
    fh1_Mus_ESum[2]->Draw("");

    cMusMap_ESum2 = new TCanvas("Mus_E1vsE2", "Mus_E1vsE2", 10, 10, 800, 700);
    fh2_Mus_ESum = new TH2F("fh2_Mus_ESum", "Music: ESum1 vs Esum2", 2192, 0, 8192, 2192, 0, 8192);
    fh2_Mus_ESum->GetXaxis()->SetTitle("Energy1 [channels]");
    fh2_Mus_ESum->GetYaxis()->SetTitle("Energy2 [channels]");
    fh2_Mus_ESum->GetYaxis()->SetTitleOffset(1.1);
    fh2_Mus_ESum->GetXaxis()->CenterTitle(true);
    fh2_Mus_ESum->GetYaxis()->CenterTitle(true);
    fh2_Mus_ESum->GetXaxis()->SetLabelSize(0.045);
    fh2_Mus_ESum->GetXaxis()->SetTitleSize(0.045);
    fh2_Mus_ESum->GetYaxis()->SetLabelSize(0.045);
    fh2_Mus_ESum->GetYaxis()->SetTitleSize(0.045);
    fh2_Mus_ESum->Draw("col");


    cMusECorMwpc0 = new TCanvas("MusE_Mwpc0","Music: Correlation with MWPC0",10,10,800,700);
    cMusECorMwpc0->Divide(1,2);
    fh2_MusCorMwpc0_EsumVsX0mm = new TH2F("fh2_Mus_EvsX0mm","Music: Esum versus X0 in mm",1000,-50,50,1024,0,8192);
    fh2_MusCorMwpc0_EsumVsX0mm->GetXaxis()->SetTitle("X0 [mm]");
    fh2_MusCorMwpc0_EsumVsX0mm->GetYaxis()->SetTitle("Raw Esum [channel]");
    fh2_MusCorMwpc0_EsumVsX0mm->GetXaxis()->CenterTitle(true);
    fh2_MusCorMwpc0_EsumVsX0mm->GetYaxis()->CenterTitle(true);
    fh2_MusCorMwpc0_EsumVsX0mm->GetXaxis()->SetLabelSize(0.045);
    fh2_MusCorMwpc0_EsumVsX0mm->GetXaxis()->SetTitleSize(0.045);
    fh2_MusCorMwpc0_EsumVsX0mm->GetYaxis()->SetLabelSize(0.045);
    fh2_MusCorMwpc0_EsumVsX0mm->GetYaxis()->SetTitleSize(0.045);
    cMusECorMwpc0->cd(1);
    fh2_MusCorMwpc0_EsumVsX0mm->Draw("col");
    fh2_MusCorMwpc0_EsumVsY0mm = new TH2F("fh2_Mus_EvsY0mm","Music: Esum versus Y0 in mm",1000,-50,50,1024,0,8192);
    fh2_MusCorMwpc0_EsumVsY0mm->GetXaxis()->SetTitle("Y0 [mm]");
    fh2_MusCorMwpc0_EsumVsY0mm->GetYaxis()->SetTitle("Raw Esum [channel]");
    fh2_MusCorMwpc0_EsumVsY0mm->GetXaxis()->CenterTitle(true);
    fh2_MusCorMwpc0_EsumVsY0mm->GetYaxis()->CenterTitle(true);
    fh2_MusCorMwpc0_EsumVsY0mm->GetXaxis()->SetLabelSize(0.045);
    fh2_MusCorMwpc0_EsumVsY0mm->GetXaxis()->SetTitleSize(0.045);
    fh2_MusCorMwpc0_EsumVsY0mm->GetYaxis()->SetLabelSize(0.045);
    fh2_MusCorMwpc0_EsumVsY0mm->GetYaxis()->SetTitleSize(0.045);
    cMusECorMwpc0->cd(2);
    fh2_MusCorMwpc0_EsumVsY0mm->Draw("col");



    TCanvas* cMus_Mult = new TCanvas("Mus_multiplicity", "Music: Multiplicity per anode", 10, 10, 800, 700);
    fh1_Musmap_mult = new TH1F("fh1_Mus_mult", "Music: Multiplicity per anode", NbAnodesMus + 2, 0, NbAnodesMus + 2);
    fh1_Musmap_mult->GetXaxis()->SetTitle("Anode");
    fh1_Musmap_mult->GetYaxis()->SetTitle("Counts");
    fh1_Musmap_mult->GetYaxis()->SetTitleOffset(1.1);
    fh1_Musmap_mult->GetXaxis()->CenterTitle(true);
    fh1_Musmap_mult->GetYaxis()->CenterTitle(true);
    fh1_Musmap_mult->GetXaxis()->SetLabelSize(0.045);
    fh1_Musmap_mult->GetXaxis()->SetTitleSize(0.045);
    fh1_Musmap_mult->GetYaxis()->SetLabelSize(0.045);
    fh1_Musmap_mult->GetYaxis()->SetTitleSize(0.045);
    fh1_Musmap_mult->SetFillColor(31);
    fh1_Musmap_mult->Draw("");

    TCanvas* cMus_Multhit = new TCanvas("Mus_hit_multiplicity", "Music: Hit-multiplicity per anode", 10, 10, 800, 700);
    fh2_Musmap_multhit = new TH2F(
        "fh2_Mus_multhit", "Music: Hit-Multiplicity per anode", NbAnodesMus + 2, 0, NbAnodesMus + 2, 10, 0, 10);
    fh2_Musmap_multhit->GetXaxis()->SetTitle("Anode");
    fh2_Musmap_multhit->GetYaxis()->SetTitle("Number of hits");
    fh2_Musmap_multhit->GetYaxis()->SetTitleOffset(1.1);
    fh2_Musmap_multhit->GetXaxis()->CenterTitle(true);
    fh2_Musmap_multhit->GetYaxis()->CenterTitle(true);
    fh2_Musmap_multhit->GetXaxis()->SetLabelSize(0.045);
    fh2_Musmap_multhit->GetXaxis()->SetTitleSize(0.045);
    fh2_Musmap_multhit->GetYaxis()->SetLabelSize(0.045);
    fh2_Musmap_multhit->GetYaxis()->SetTitleSize(0.045);
    fh2_Musmap_multhit->Draw("col");

    TCanvas* cMus_TRef_TTrigger = new TCanvas("Mus_DRefTrigger", "MUSIC: TRef-TTrigger", 10, 10, 800, 700);
    fh1_Mus_treftrigger = new TH1F("fh1_Mus_tref-ttrigger", "Music: TRef-TTrigger", 1000, -30000, 30000);
    fh1_Mus_treftrigger->GetXaxis()->SetTitle("Anode");
    fh1_Mus_treftrigger->GetYaxis()->SetTitle("Counts");
    fh1_Mus_treftrigger->GetYaxis()->SetTitleOffset(1.1);
    fh1_Mus_treftrigger->GetXaxis()->CenterTitle(true);
    fh1_Mus_treftrigger->GetYaxis()->CenterTitle(true);
    fh1_Mus_treftrigger->GetXaxis()->SetLabelSize(0.045);
    fh1_Mus_treftrigger->GetXaxis()->SetTitleSize(0.045);
    fh1_Mus_treftrigger->GetYaxis()->SetLabelSize(0.045);
    fh1_Mus_treftrigger->GetYaxis()->SetTitleSize(0.045);
    fh1_Mus_treftrigger->Draw("");

    // Esum vs difference DT7 - DT1
    cMusMap_ESum_vs_diffDT = new TCanvas("Mus_ESum_vs_diffDT", "MUSIC: ESum vs. diff-DT (7-1)", 10, 10, 800, 700);
    fh2_Mus_ESum_vs_diffDT =
        new TH2F("fh2_Mus_ESum_vs_diff_DT", "Music:ESum vs: diff-DT", 2000, -2000, 2000, 2000, 0, 8000);
    fh2_Mus_ESum_vs_diffDT->GetXaxis()->SetTitle("Difference DT (7-1)");
    fh2_Mus_ESum_vs_diffDT->GetYaxis()->SetTitle("Energy Sum [channels]");
    fh2_Mus_ESum_vs_diffDT->GetYaxis()->SetTitleOffset(1.1);
    fh2_Mus_ESum_vs_diffDT->GetXaxis()->CenterTitle(true);
    fh2_Mus_ESum_vs_diffDT->GetYaxis()->CenterTitle(true);
    fh2_Mus_ESum_vs_diffDT->GetXaxis()->SetLabelSize(0.045);
    fh2_Mus_ESum_vs_diffDT->GetXaxis()->SetTitleSize(0.045);
    fh2_Mus_ESum_vs_diffDT->GetYaxis()->SetLabelSize(0.045);
    fh2_Mus_ESum_vs_diffDT->GetYaxis()->SetTitleSize(0.045);
    fh2_Mus_ESum_vs_diffDT->Draw("col");

    // Hit data
    TCanvas* cMus_Z = new TCanvas("Mus_charge_z", "Mus: Charge Z", 10, 10, 800, 700);
    fh1_Mushit_z = new TH1F("fh1_Mus_charge_z", "Music: Charge Z", 900, 2, 28);
    fh1_Mushit_z->GetXaxis()->SetTitle("Charge (Z)");
    fh1_Mushit_z->GetYaxis()->SetTitle("Counts");
    fh1_Mushit_z->GetYaxis()->SetTitleOffset(1.1);
    fh1_Mushit_z->GetXaxis()->CenterTitle(true);
    fh1_Mushit_z->GetYaxis()->CenterTitle(true);
    fh1_Mushit_z->GetXaxis()->SetLabelSize(0.045);
    fh1_Mushit_z->GetXaxis()->SetTitleSize(0.045);
    fh1_Mushit_z->GetYaxis()->SetLabelSize(0.045);
    fh1_Mushit_z->GetYaxis()->SetTitleSize(0.045);
    fh1_Mushit_z->SetFillColor(28);
    fh1_Mushit_z->SetLineColor(1);
    fh1_Mushit_z->Draw("");

    TCanvas* cMus_theta = new TCanvas("Mus_theta", "Mus: #theta_{XZ}", 10, 10, 800, 700);
    fh1_Mushit_theta = new TH1F("fh1_Mus_theta", "Music: #theta_{XZ}", 900, -40, 40);
    fh1_Mushit_theta->GetXaxis()->SetTitle("#theta_{XZ} [mrad]");
    fh1_Mushit_theta->GetYaxis()->SetTitle("Counts");
    fh1_Mushit_theta->GetYaxis()->SetTitleOffset(1.1);
    fh1_Mushit_theta->GetXaxis()->CenterTitle(true);
    fh1_Mushit_theta->GetYaxis()->CenterTitle(true);
    fh1_Mushit_theta->GetXaxis()->SetLabelSize(0.045);
    fh1_Mushit_theta->GetXaxis()->SetTitleSize(0.045);
    fh1_Mushit_theta->GetYaxis()->SetLabelSize(0.045);
    fh1_Mushit_theta->GetYaxis()->SetTitleSize(0.045);
    fh1_Mushit_theta->SetFillColor(28);
    fh1_Mushit_theta->SetLineColor(1);
    fh1_Mushit_theta->Draw("");

    TCanvas* cMus_zvstheta = new TCanvas("Mus_charge_vs_theta", "Mus: Charge Z vs #theta_{XZ}", 10, 10, 800, 700);
    fh2_Mushit_zvstheta =
        new TH2F("fh2_Mus_Charge_Z_vs_theta", "Music: Charge Z vs #theta_{XZ}", 900, -40, 40, 900, 2, 28);
    fh2_Mushit_zvstheta->GetXaxis()->SetTitle("#theta_{XZ} [mrad]");
    fh2_Mushit_zvstheta->GetYaxis()->SetTitle("Charge (Z)");
    fh2_Mushit_zvstheta->GetYaxis()->SetTitleOffset(1.1);
    fh2_Mushit_zvstheta->GetXaxis()->CenterTitle(true);
    fh2_Mushit_zvstheta->GetYaxis()->CenterTitle(true);
    fh2_Mushit_zvstheta->GetXaxis()->SetLabelSize(0.045);
    fh2_Mushit_zvstheta->GetXaxis()->SetTitleSize(0.045);
    fh2_Mushit_zvstheta->GetYaxis()->SetLabelSize(0.045);
    fh2_Mushit_zvstheta->GetYaxis()->SetTitleSize(0.045);
    fh2_Mushit_zvstheta->Draw("col");

    // MAIN FOLDER-Music
    TFolder* mainfolMus = new TFolder("MUSIC", "MUSIC info");
    mainfolMus->Add(cMusMap_E);
    mainfolMus->Add(cMusMap_DT);
    mainfolMus->Add(cMus_Mult);
    mainfolMus->Add(cMus_Multhit);
    mainfolMus->Add(cMus_TRef_TTrigger);
    mainfolMus->Add(cMusMap_ESum);
    mainfolMus->Add(cMusMap_ESum1);
    mainfolMus->Add(cMusMap_ESum2);
    mainfolMus->Add(cMusMap_EvsDT);
    mainfolMus->Add(cMusMap_DTvsDT);
    mainfolMus->Add(cMus_EsumvsDT);
    mainfolMus->Add(cMusMap_ESum_vs_diffDT);
    if (fHitItemsMwpc0)
    {
	mainfolMus->Add(cMusECorMwpc0);
	mainfolMus->Add(cMusDTCorMwpc0);
    }
    if (fCalItemsMus)
    {
        mainfolMus->Add(cMusCal_Pos);
    }
    if (fHitItemsMus)
    {
        mainfolMus->Add(cMus_Z);
        mainfolMus->Add(cMus_theta);
        mainfolMus->Add(cMus_zvstheta);
    }
    run->AddObject(mainfolMus);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_MUSIC_HIST", Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

void R3BMusicOnlineSpectra::Reset_Histo()
{
    LOG(INFO) << "R3BMusicOnlineSpectra::Reset_Histo";
    // Map data
    for (Int_t j = 0; j < NbAnodesMus; j++)
    {
        fh1_Musmap_E[j]->Reset();
        fh1_Musmap_DT[j]->Reset();
        fh2_Mus_EneRawVsDriftTime[j]->Reset();
    }
    for (Int_t j = 0; j < NbAnodesMus - 1; j++)
    {
        fh2_Mus_DTvsDT[j]->Reset();
    }
    fh2_Mus_ESum_vs_DT->Reset();
    fh1_Mus_ESum[0]->Reset();
    fh1_Mus_ESum[1]->Reset();
    fh1_Mus_ESum[2]->Reset();
    fh2_Mus_ESum->Reset();
    fh1_Musmap_mult->Reset();
    fh1_Mus_treftrigger->Reset();
    fh2_Musmap_multhit->Reset();
    fh2_Mus_ESum_vs_diffDT->Reset();
    if(fHitItemsMwpc0){
      fh2_MusCorMwpc0_EsumVsX0mm->Reset();
      fh2_MusCorMwpc0_EsumVsY0mm->Reset();
    }    

    // Cal data
    if (fCalItemsMus)
    {
        for (Int_t j = 0; j < NbAnodesMus; j++)
            fh1_Muscal_Pos[j]->Reset();
    }
    // Hit data
    if (fHitItemsMus)
    {
        fh1_Mushit_z->Reset();
        fh1_Mushit_theta->Reset();
        fh2_Mushit_zvstheta->Reset();
    }
}

void R3BMusicOnlineSpectra::Exec(Option_t* option)
{
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(FATAL) << "R3BMusicOnlineSpectra::Exec FairRootManager not found";

    // Fill mapped data
    if (fMappedItemsMus && fMappedItemsMus->GetEntriesFast() > 0)
    {
        Double_t e1 = 0., e2 = 0.;
        Double_t n1 = 0., n2 = 0.;
        for (Int_t i = 0; i < NbAnodesMus; i++)
            fE[i] = 0.; // mult=1 !!!
        for (Int_t i = 0; i < NbAnodesMus + 2; i++)
        {
            fT[i] = 0.; // mult=1 !!!
            multhit[i] = 0;
        }
        Int_t nHits = fMappedItemsMus->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BMusicMappedData* hit = (R3BMusicMappedData*)fMappedItemsMus->At(ihit);
            if (!hit)
                continue;
            multhit[hit->GetAnodeID()]++;
            if (hit->GetAnodeID() < NbAnodesMus)
                fh1_Musmap_E[hit->GetAnodeID()]->Fill(hit->GetEnergy());
            fh1_Musmap_mult->Fill(hit->GetAnodeID());
            if (fE[hit->GetAnodeID()] == 0)
                fE[hit->GetAnodeID()] = hit->GetEnergy(); // mult=1 !!!
            if (fT[hit->GetAnodeID()] == 0)
                fT[hit->GetAnodeID()] = hit->GetTime(); // mult=1 !!!

            if (hit->GetAnodeID() < NbAnodesMus / 2)
            {
                e1 = e1 + hit->GetEnergy();
                n1++;
            }
            else if (hit->GetAnodeID() < NbAnodesMus)
            {
                e2 = e2 + hit->GetEnergy();
                n2++;
            }
        }

        fh1_Mus_treftrigger->Fill(fT[NbAnodesMus] - fT[NbAnodesMus + 1]);

        for (Int_t i = 0; i < NbAnodesMus + 2; i++)
            fh2_Musmap_multhit->Fill(i, multhit[i]);

        // Fill data only if there are TREF signals
        if (multhit[NbAnodesMus] == 1)
        {
            for (Int_t i = 0; i < NbAnodesMus; i++)
            {
                if (multhit[i] == 1)
                {
                    fh1_Musmap_DT[i]->Fill(fT[i] - fT[NbAnodesMus]);
                    fh2_Mus_EneRawVsDriftTime[i]->Fill(fE[i], fT[i] - fT[NbAnodesMus]);
                    if (i < NbAnodesMus / 2)
                    {
                        e1 = e1 + fE[i];
                        n1++;
                    }
                    else if (i < NbAnodesMus)
                    {
                        e2 = e2 + fE[i];
                        n2++;
                    }
                }
            }

            for (Int_t i = 0; i < NbAnodesMus - 1; i++)
                if (multhit[i] == 1 && multhit[i + 1] == 1)
                    fh2_Mus_DTvsDT[i]->Fill(fT[i] - fT[NbAnodesMus], fT[i + 1] - fT[NbAnodesMus]);

            fh1_Mus_ESum[0]->Fill(e1 / n1);
            fh1_Mus_ESum[1]->Fill(e2 / n2);
            fh1_Mus_ESum[2]->Fill((e1 + e2) / (n1 + n2));
            fh2_Mus_ESum->Fill(e1 / n1, e2 / n2);
            fh2_Mus_ESum_vs_DT->Fill(fT[5] - fT[NbAnodesMus], (e1 + e2) / (n1 + n2));
            fh2_Mus_ESum_vs_diffDT->Fill((fT[6] - fT[NbAnodesMus]) - (fT[0] - fT[NbAnodesMus]), (e1 + e2) / (n1 + n2));
        
	    if (fHitItemsMwpc0 && fHitItemsMwpc0->GetEntriesFast() > 0)
	    {
	      for(Int_t ihit=0; ihit<fHitItemsMwpc0->GetEntriesFast(); ihit++)
	      {
		R3BSofMwpcHitData* hit = (R3BSofMwpcHitData*)fHitItemsMwpc0->At(ihit);
		if (!hit) continue;
		fh2_MusCorMwpc0_EsumVsX0mm->Fill(hit->GetX(),(e1+e2)/(n1+n2));
		fh2_MusCorMwpc0_EsumVsY0mm->Fill(hit->GetY(),(e1+e2)/(n1+n2));
		for (Int_t i = 0; i < NbAnodesMus; i++) 
		{
		  if(multhit[NbAnodesMus] == 1 && multhit[i] == 1)
		  {
		    fh2_MusCorMwpc0_DTvsX0[i]->Fill(hit->GetX(), fT[i]-fT[NbAnodesMus]);
		  }
		}
	      
	      }
	    }
	}
    }

    // Fill cal data
    if (fCalItemsMus && fCalItemsMus->GetEntriesFast() > 0)
    {
        Int_t nHits = fCalItemsMus->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BMusicCalData* hit = (R3BMusicCalData*)fCalItemsMus->At(ihit);
            if (!hit)
                continue;
            fh1_Muscal_Pos[hit->GetAnodeID()]->Fill(hit->GetDTime());
        }
    }

    // Fill hit data
    if (fHitItemsMus && fHitItemsMus->GetEntriesFast() > 0)
    {
        Int_t nHits = fHitItemsMus->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BMusicHitData* hit = (R3BMusicHitData*)fHitItemsMus->At(ihit);
            if (!hit)
                continue;
            fh1_Mushit_z->Fill(hit->GetZcharge());
            fh1_Mushit_theta->Fill(hit->GetTheta() * 1000.); // in mrad
            fh2_Mushit_zvstheta->Fill(hit->GetTheta() * 1000., hit->GetZcharge());
        }
    }

    fNEvents += 1;
}

void R3BMusicOnlineSpectra::FinishEvent()
{
    if (fMappedItemsMus)
    {
        fMappedItemsMus->Clear();
    }
    if (fCalItemsMus)
    {
        fCalItemsMus->Clear();
    }
    if (fHitItemsMus)
    {
        fHitItemsMus->Clear();
    }
}

void R3BMusicOnlineSpectra::FinishTask()
{

    if (fMappedItemsMus)
    {
        cMusMap_E->Write();
        cMusMap_DT->Write();
        cMusMap_DTvsDT->Write();
        cMusMap_EvsDT->Write();
        cMus_EsumvsDT->Write();
        cMusMap_ESum_vs_diffDT->Write();
        fh1_Mus_ESum[0]->Write();
        fh1_Mus_ESum[1]->Write();
        fh1_Mus_ESum[2]->Write();
        fh2_Mus_ESum->Write();
        fh1_Musmap_mult->Write();
        fh1_Mus_treftrigger->Write();
        fh2_Musmap_multhit->Write();
    }
    if (fCalItemsMus)
    {
        cMusCal_Pos->Write();
    }
    if (fHitItemsMus)
    {
        fh1_Mushit_z->Write();
        fh1_Mushit_theta->Write();
        fh2_Mushit_zvstheta->Write();
    }
}

ClassImp(R3BMusicOnlineSpectra)
