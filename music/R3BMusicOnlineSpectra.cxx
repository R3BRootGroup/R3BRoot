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
#include "R3BMusicMappedData.h"
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
    , fNEvents(0)
{
}

R3BMusicOnlineSpectra::R3BMusicOnlineSpectra(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItemsMus(NULL)
    , fNEvents(0)
{
}

R3BMusicOnlineSpectra::~R3BMusicOnlineSpectra()
{
    LOG(INFO) << "R3BMusicOnlineSpectra::Delete instance";
    if (fMappedItemsMus)
        delete fMappedItemsMus;
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

    // Drift-times
    cMusMap_DT = new TCanvas("Mus_DTmap", "", 10, 10, 800, 700);
    cMusMap_DT->Divide(2, 4);

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
        cMusMap_DT->cd(j + 1);
        fh1_Musmap_DT[j]->Draw("");
    }

    cMusMap_ESum = new TCanvas("Mus_ESum_1,2", "Mus_ESum_1,2", 10, 10, 800, 700);
    cMusMap_ESum->Divide(1, 2);
    cMusMap_ESum->cd(1);
    fh1_Mus_ESum[0] = new TH1F("fh1_Mus_ESum1", "Mus:ESum:first", 8192, 0, 8192);
    fh1_Mus_ESum[0]->GetXaxis()->SetTitle("Energy [channels]");
    fh1_Mus_ESum[0]->GetYaxis()->SetTitle("Counts");
    fh1_Mus_ESum[0]->GetYaxis()->SetTitleOffset(1.1);
    fh1_Mus_ESum[0]->GetXaxis()->CenterTitle(true);
    fh1_Mus_ESum[0]->GetYaxis()->CenterTitle(true);
    fh1_Mus_ESum[0]->GetXaxis()->SetLabelSize(0.045);
    fh1_Mus_ESum[0]->GetXaxis()->SetTitleSize(0.045);
    fh1_Mus_ESum[0]->GetYaxis()->SetLabelSize(0.045);
    fh1_Mus_ESum[0]->GetYaxis()->SetTitleSize(0.045);
    fh1_Mus_ESum[0]->Draw("");
    cMusMap_ESum->cd(2);
    fh1_Mus_ESum[1] = new TH1F("fh1_Mus_ESum2", "Mus:ESum:Second", 8192, 0, 8192);
    fh1_Mus_ESum[1]->GetXaxis()->SetTitle("Energy [channels]");
    fh1_Mus_ESum[1]->GetYaxis()->SetTitle("Counts");
    fh1_Mus_ESum[1]->GetYaxis()->SetTitleOffset(1.1);
    fh1_Mus_ESum[1]->GetXaxis()->CenterTitle(true);
    fh1_Mus_ESum[1]->GetYaxis()->CenterTitle(true);
    fh1_Mus_ESum[1]->GetXaxis()->SetLabelSize(0.045);
    fh1_Mus_ESum[1]->GetXaxis()->SetTitleSize(0.045);
    fh1_Mus_ESum[1]->GetYaxis()->SetLabelSize(0.045);
    fh1_Mus_ESum[1]->GetYaxis()->SetTitleSize(0.045);
    fh1_Mus_ESum[1]->Draw("");

    cMusMap_ESum1 = new TCanvas("Mus_ESum", "Mus_ESum", 10, 10, 800, 700);
    fh1_Mus_ESum[2] = new TH1F("fh1_tMus_ESum", "Mus:ESum", 8192, 0, 8192);
    fh1_Mus_ESum[2]->GetXaxis()->SetTitle("Energy [channels]");
    fh1_Mus_ESum[2]->GetYaxis()->SetTitle("Counts");
    fh1_Mus_ESum[2]->GetYaxis()->SetTitleOffset(1.1);
    fh1_Mus_ESum[2]->GetXaxis()->CenterTitle(true);
    fh1_Mus_ESum[2]->GetYaxis()->CenterTitle(true);
    fh1_Mus_ESum[2]->GetXaxis()->SetLabelSize(0.045);
    fh1_Mus_ESum[2]->GetXaxis()->SetTitleSize(0.045);
    fh1_Mus_ESum[2]->GetYaxis()->SetLabelSize(0.045);
    fh1_Mus_ESum[2]->GetYaxis()->SetTitleSize(0.045);
    fh1_Mus_ESum[2]->Draw("");

    cMusMap_ESum2 = new TCanvas("Mus_E1vsE2", "Mus_E1vsE2", 10, 10, 800, 700);
    fh2_Mus_ESum = new TH2F("fh2_Mus_ESum", "Mus: ESum1 vs Esum2", 2192, 0, 8192, 2192, 0, 8192);
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

    TCanvas* cMus_Mult = new TCanvas("Mus_multiplicity", "MUSIC: Multiplicity per anode", 10, 10, 800, 700);
    fh1_Musmap_mult = new TH1F("fh1_Mus_mult", "Music:Multiplicity per anode", NbAnodesMus + 2, 0, NbAnodesMus + 2);
    fh1_Musmap_mult->GetXaxis()->SetTitle("Anode");
    fh1_Musmap_mult->GetYaxis()->SetTitle("Counts");
    fh1_Musmap_mult->GetYaxis()->SetTitleOffset(1.1);
    fh1_Musmap_mult->GetXaxis()->CenterTitle(true);
    fh1_Musmap_mult->GetYaxis()->CenterTitle(true);
    fh1_Musmap_mult->GetXaxis()->SetLabelSize(0.045);
    fh1_Musmap_mult->GetXaxis()->SetTitleSize(0.045);
    fh1_Musmap_mult->GetYaxis()->SetLabelSize(0.045);
    fh1_Musmap_mult->GetYaxis()->SetTitleSize(0.045);
    fh1_Musmap_mult->Draw("");

    TCanvas* cMus_Multhit = new TCanvas("Mus_hit_multiplicity", "MUSIC: Hit-multiplicity per anode", 10, 10, 800, 700);
    fh2_Musmap_multhit =
        new TH2F("fh2_Mus_multhit", "Music:Hit-Multiplicity per anode", NbAnodesMus + 2, 0, NbAnodesMus + 2, 10, 0, 10);
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

    // MAIN FOLDER-Music
    TFolder* mainfolMus = new TFolder("MUSIC", "MUSIC info");
    mainfolMus->Add(cMusMap_E);
    mainfolMus->Add(cMusMap_DT);
    mainfolMus->Add(cMus_Mult);
    mainfolMus->Add(cMus_Multhit);
    mainfolMus->Add(cMusMap_ESum);
    mainfolMus->Add(cMusMap_ESum1);
    mainfolMus->Add(cMusMap_ESum2);
    mainfolMus->Add(cMusMap_EvsDT);
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
    fh1_Mus_ESum[0]->Reset();
    fh1_Mus_ESum[1]->Reset();
    fh1_Mus_ESum[2]->Reset();
    fh2_Mus_ESum->Reset();
    fh1_Musmap_mult->Reset();
    fh2_Musmap_multhit->Reset();
}

void R3BMusicOnlineSpectra::Exec(Option_t* option)
{
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(FATAL) << "R3BMusicOnlineSpectra::Exec FairRootManager not found";

    // Fill mapped data
    if (fMappedItemsMus && fMappedItemsMus->GetEntriesFast())
    {
        Double_t e1 = 0., e2 = 0., E[NbAnodesMus];
        Double_t n1 = 0., n2 = 0.;
        for (Int_t i = 0; i < NbAnodesMus; i++)
            E[i] = 0.; // mult=1 !!!
        Double_t DT[NbAnodesMus + 2];
        Int_t multhit[NbAnodesMus + 2];
        for (Int_t i = 0; i < NbAnodesMus + 1; i++)
        {
            DT[i] = 0.; // mult=1 !!!
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
            if (E[hit->GetAnodeID()] == 0)
                E[hit->GetAnodeID()] = hit->GetEnergy(); // mult=1 !!!
            if (DT[hit->GetAnodeID()] == 0)
                DT[hit->GetAnodeID()] = hit->GetTime(); // mult=1 !!!

            if (hit->GetAnodeID() < 4)
            {
                e1 = e1 + hit->GetEnergy();
                n1++;
            }
            else
            {
                e2 = e2 + hit->GetEnergy();
                n2++;
            }
        }

        for (Int_t i = 0; i < NbAnodesMus; i++)
        {
            fh1_Musmap_DT[i]->Fill(DT[i] - DT[NbAnodesMus]);
            if (i < 4)
                fh2_Mus_EneRawVsDriftTime[i]->Fill(E[i], DT[i] - DT[NbAnodesMus]);
            else
                fh2_Mus_EneRawVsDriftTime[i]->Fill(E[i], DT[i] - DT[NbAnodesMus + 1]);
        }
        for (Int_t i = 0; i < NbAnodesMus + 2; i++)
            fh2_Musmap_multhit->Fill(i, multhit[i]);

        fh1_Mus_ESum[0]->Fill(e1 / n1);
        fh1_Mus_ESum[1]->Fill(e2 / n2);
        fh1_Mus_ESum[2]->Fill((e1 + e2) / (n1 + n2));
        fh2_Mus_ESum->Fill(e1 / n1, e2 / n2);
    }

    fNEvents += 1;
}

void R3BMusicOnlineSpectra::FinishEvent()
{
    if (fMappedItemsMus)
    {
        fMappedItemsMus->Clear();
    }
}

void R3BMusicOnlineSpectra::FinishTask()
{

    if (fMappedItemsMus)
    {
        cMusMap_E->Write();
        cMusMap_DT->Write();
        cMusMap_EvsDT->Write();
        fh1_Mus_ESum[0]->Write();
        fh1_Mus_ESum[1]->Write();
        fh1_Mus_ESum[2]->Write();
        fh2_Mus_ESum->Write();
        fh1_Musmap_mult->Write();
        fh2_Musmap_multhit->Write();
    }
}

ClassImp(R3BMusicOnlineSpectra)
