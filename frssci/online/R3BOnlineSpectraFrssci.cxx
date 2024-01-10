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

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TFolder.h"
#include "TH1.h"
#include "TH2.h"
#include "THttpServer.h"
#include "TMath.h"

#include "R3BEventHeader.h"
#include "R3BFrssciMappedData.h"
#include "R3BOnlineSpectraFrssci.h"

R3BOnlineSpectraFrssci::R3BOnlineSpectraFrssci()
    : R3BOnlineSpectraFrssci("FrssciOnlineSpectra", 1)
{
}

R3BOnlineSpectraFrssci::R3BOnlineSpectraFrssci(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMapped(NULL)
    , fNEvents(0)
    , fNbDets(1)
    , fNbPmts(4)
{
}

R3BOnlineSpectraFrssci::~R3BOnlineSpectraFrssci()
{
    LOG(debug) << "R3BOnlineSpectraFrssci::Destructor";
    if (fMapped)
        delete fMapped;
}

InitStatus R3BOnlineSpectraFrssci::Init()
{
    LOG(info) << "R3BOnlineSpectraFrssci::Init()";

    char Name1[255];
    char Name2[255];

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "R3BOnlineSpectraFrssci::Init FairRootManager not found";

    fEventHeader = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    if (!fEventHeader)
        fEventHeader = dynamic_cast<R3BEventHeader*>(mgr->GetObject("R3BEventHeader"));

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // --- ------------ --- //
    // --- MAPPED LEVEL --- //
    // --- ------------ --- //

    // === get access to mapped data ===//
    fMapped = dynamic_cast<TClonesArray*>(mgr->GetObject("FrssciMapped"));
    if (!fMapped)
    {
        LOG(fatal) << "FrssciMapped not found";
        return (kFATAL);
    }

    // === declare and create TCanvas and Histogram === //

    sprintf(Name1, "MapFinetime");
    cMapFT = new TCanvas(Name1, Name1, 10, 10, 800, 700);
    cMapFT->Divide(fNbPmts, fNbDets);
    fh1_finetime = new TH1I*[fNbDets * fNbPmts];

    sprintf(Name1, "MapMult1D");
    cMapMult1D = new TCanvas(Name1, Name1, 10, 10, 800, 700);
    cMapMult1D->Divide(fNbPmts, fNbDets);
    fh1_multMap = new TH1I*[fNbDets * fNbPmts];

    sprintf(Name1, "MapMult2D");
    cMapMult2D = new TCanvas(Name1, Name1, 10, 10, 800, 700);
    cMapMult2D->Divide(2, fNbDets);
    fh2_multMap = new TH2I*[fNbDets];
    fh2_multMap_RvsL = new TH2I*[fNbDets];

    for (Int_t i = 0; i < fNbDets; i++)
    {
        for (Int_t j = 0; j < fNbPmts; j++)
        {
            // === TH1F: fine time === //
            sprintf(Name1, "Frssci%i_FineTimePmt%i", i + 1, j + 1);
            fh1_finetime[i * fNbPmts + j] = new TH1I(Name1, Name1, 1000, 0, 1000);
            fh1_finetime[i * fNbPmts + j]->GetXaxis()->SetTitle("Fine time");
            fh1_finetime[i * fNbPmts + j]->GetYaxis()->SetTitle("Counts per bin");
            fh1_finetime[i * fNbPmts + j]->GetXaxis()->CenterTitle(true);
            fh1_finetime[i * fNbPmts + j]->GetYaxis()->CenterTitle(true);
            fh1_finetime[i * fNbPmts + j]->GetXaxis()->SetLabelSize(0.05);
            fh1_finetime[i * fNbPmts + j]->GetXaxis()->SetTitleSize(0.05);
            fh1_finetime[i * fNbPmts + j]->GetYaxis()->SetLabelSize(0.05);
            fh1_finetime[i * fNbPmts + j]->GetYaxis()->SetTitleSize(0.05);
            cMapFT->cd(i * fNbPmts + j + 1);
            fh1_finetime[i * fNbPmts + j]->Draw("");

            // === TH1I: 1D-mult at map level === //
            sprintf(Name1, "Frssci%i_Pmt%i_MultPerEvent_Mapped", i + 1, j + 1);
            sprintf(Name2,
                    "Frssci%i_Pmt%i_MultPerEvent_Mapped (blue no condition on TPAT, red condition on TPAT = 1 or 2)",
                    i + 1,
                    j + 1);
            fh1_multMap[i * fNbPmts + j] = new TH1I(Name1, Name1, 70, -0.5, 69.5);
            fh1_multMap[i * fNbPmts + j]->GetXaxis()->SetTitle("Multiplicity per event");
            fh1_multMap[i * fNbPmts + j]->GetYaxis()->SetTitle("Counts");
            fh1_multMap[i * fNbPmts + j]->GetXaxis()->CenterTitle(true);
            fh1_multMap[i * fNbPmts + j]->GetYaxis()->CenterTitle(true);
            fh1_multMap[i * fNbPmts + j]->GetXaxis()->SetLabelSize(0.05);
            fh1_multMap[i * fNbPmts + j]->GetXaxis()->SetTitleSize(0.05);
            fh1_multMap[i * fNbPmts + j]->GetYaxis()->SetLabelSize(0.05);
            fh1_multMap[i * fNbPmts + j]->GetYaxis()->SetTitleSize(0.05);
            fh1_multMap[i * fNbPmts + j]->SetLineWidth(2);
            fh1_multMap[i * fNbPmts + j]->SetLineColor(kBlue);
            cMapMult1D->cd(i * fNbPmts + j + 1);
            gPad->SetLogy();
            fh1_multMap[i * fNbPmts + j]->Draw("");
        }

        // === TH1F: multiplicity per event and channel at mapped level === //
        sprintf(Name1, "Frssci%i_MultPerEvent", i + 1);
        fh2_multMap[i] = new TH2I(Name1, Name1, fNbPmts + 1, -0.5, fNbPmts + 0.5, 20, -0.5, 19.5);
        fh2_multMap[i]->GetXaxis()->SetTitle("channel: 1=PMT R,    2=PMT L,    3=COMMON REF, 4 = RESERVED FOR SYNC");
        fh2_multMap[i]->GetYaxis()->SetTitle("multiplicity per event");
        fh2_multMap[i]->GetXaxis()->CenterTitle(true);
        fh2_multMap[i]->GetYaxis()->CenterTitle(true);
        fh2_multMap[i]->GetXaxis()->SetLabelSize(0.05);
        fh2_multMap[i]->GetXaxis()->SetTitleSize(0.05);
        fh2_multMap[i]->GetYaxis()->SetLabelSize(0.05);
        fh2_multMap[i]->GetYaxis()->SetTitleSize(0.05);
        cMapMult2D->cd(i * 2 + 1);
        fh2_multMap[i]->Draw("COL");

        // === TH1F: multiplicity per event and channel at mapped level === //
        sprintf(Name1, "Frssci%i_MultPerEvent_RvsL", i + 1);
        fh2_multMap_RvsL[i] = new TH2I(Name1, Name1, 40, -1.5, 38.5, 40, -1.5, 38.5);
        fh2_multMap_RvsL[i]->GetXaxis()->SetTitle("Multiplicity per event on the Left Pmt");
        fh2_multMap_RvsL[i]->GetYaxis()->SetTitle("Multiplicity per event on the Right Pmt");
        fh2_multMap_RvsL[i]->GetXaxis()->CenterTitle(true);
        fh2_multMap_RvsL[i]->GetYaxis()->CenterTitle(true);
        fh2_multMap_RvsL[i]->GetXaxis()->SetLabelSize(0.05);
        fh2_multMap_RvsL[i]->GetXaxis()->SetTitleSize(0.05);
        fh2_multMap_RvsL[i]->GetYaxis()->SetLabelSize(0.05);
        fh2_multMap_RvsL[i]->GetYaxis()->SetTitleSize(0.05);
        cMapMult2D->cd(i * 2 + 2);
        fh2_multMap_RvsL[i]->Draw("COL");

    } // end of loop over fNbDets

    // --- --------------- --- //
    // --- MAIN FOLDER-Sci --- //
    // --- --------------- --- //
    TFolder* mainfolMap = new TFolder("FrssciMap", "Frssci Map info");
    mainfolMap->Add(cMapFT);
    mainfolMap->Add(cMapMult1D);
    mainfolMap->Add(cMapMult2D);

    run->AddObject(mainfolMap);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_FRSSCI_HIST", Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

void R3BOnlineSpectraFrssci::Reset_Histo()
{
    LOG(info) << "R3BOnlineSpectraFrssci::Reset_Histo";
    for (Int_t i = 0; i < fNbDets; i++)
    {
        fh2_multMap[i]->Reset();
        fh2_multMap_RvsL[i]->Reset();

        // === FINE TIME === //
        for (Int_t j = 0; j < fNbPmts; j++)
        {
            fh1_finetime[i * fNbPmts + j]->Reset();
            fh1_multMap[i * fNbPmts + j]->Reset();
        }
    }
}

void R3BOnlineSpectraFrssci::Exec(Option_t* option)
{

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "R3BOnlineSpectraFrssci::Exec FairRootManager not found";

    Int_t nHits;
    Int_t iDet; // 0-based
    Int_t iPmt; // 0-based

    Int_t multMap[fNbDets * fNbPmts];

    for (Int_t i = 0; i < fNbDets; i++)
    {
        for (Int_t j = 0; j < fNbPmts; j++)
        {
            multMap[i * fNbPmts + j] = 0;
        }
    }

    if (fMapped && fMapped->GetEntriesFast() > 0)
    {
        // --- ---------------- --- //
        // --- read mapped data --- //
        // --- ---------------- --- //
        nHits = fMapped->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BFrssciMappedData* hitmapped = dynamic_cast<R3BFrssciMappedData*>(fMapped->At(ihit));
            if (!hitmapped)
                continue;
            iDet = hitmapped->GetDetector() - 1;
            iPmt = hitmapped->GetPmt() - 1;
            multMap[iDet * fNbPmts + iPmt]++;
            fh1_finetime[iDet * fNbPmts + iPmt]->Fill(hitmapped->GetTimeFine());
        } // end of loop over mapped data

        // --- ----------------------------------------- --- //
        // --- filling some histogramms outside the loop --- //
        // --- ----------------------------------------- --- //
        for (Int_t i = 0; i < fNbDets; i++)
        {
            fh2_multMap_RvsL[i]->Fill(multMap[i * fNbPmts + 1], multMap[i * fNbPmts]);

            for (Int_t j = 0; j < fNbPmts; j++)
            {
                fh1_multMap[i * fNbPmts + j]->Fill(multMap[i * fNbPmts + j]);
                fh2_multMap[i]->Fill(j + 1, multMap[i * fNbPmts + j]);
            }
        }

    } // --- end of if Mapped data --- //

    fNEvents++;
}

// -----   Public method Reset   ------------------------------------------------
void R3BOnlineSpectraFrssci::Reset() {}

// -----   Public method Finish   -----------------------------------------------
void R3BOnlineSpectraFrssci::FinishEvent()
{
    if (fMapped)
    {
        fMapped->Clear();
    }
}

void R3BOnlineSpectraFrssci::FinishTask()
{
    for (Int_t i = 0; i < fNbDets; i++)
    {
        if (fMapped)
        {
            fh2_multMap[i]->Write();
            fh2_multMap_RvsL[i]->Write();
            for (Int_t j = 0; j < fNbPmts; j++)
            {
                fh1_finetime[i * fNbPmts + j]->Write();
                fh1_multMap[i * fNbPmts + j]->Write();
            }
        }
    } // end of loop over fNbDets
}
ClassImp(R3BOnlineSpectraFrssci);
