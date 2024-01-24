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
#include "R3BFrsSciMappedData.h"
#include "R3BFrsSciTcalData.h"
#include "R3BOnlineSpectraFrsSci.h"

R3BOnlineSpectraFrsSci::R3BOnlineSpectraFrsSci()
    : R3BOnlineSpectraFrsSci("FrsSciOnlineSpectra", 1)
{
}

R3BOnlineSpectraFrsSci::R3BOnlineSpectraFrsSci(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMapped(NULL)
    , fTcal(NULL)
    , fNEvents(0)
    , fNbDets(3)
    , fNbPmts(3)
    , fNbTofs(3)
{
}

R3BOnlineSpectraFrsSci::~R3BOnlineSpectraFrsSci()
{
    LOG(debug) << "R3BOnlineSpectraFrsSci::Destructor";
    if (fMapped)
        delete fMapped;
    if (fTcal)
        delete fTcal;
}

InitStatus R3BOnlineSpectraFrsSci::Init()
{
    LOG(info) << "R3BOnlineSpectraFrsSci::Init()";

    char Name1[255];
    char Name2[255];

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "R3BOnlineSpectraFrsSci::Init FairRootManager not found";

    fEventHeader = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    if (!fEventHeader)
        fEventHeader = dynamic_cast<R3BEventHeader*>(mgr->GetObject("R3BEventHeader"));

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // --- ------------ --- //
    // --- MAPPED LEVEL --- //
    // --- ------------ --- //

    // === get access to mapped data ===//
    fMapped = dynamic_cast<TClonesArray*>(mgr->GetObject("FrsSciMappedData"));
    if (!fMapped)
    {
        LOG(fatal) << "FrsSciMapped not found";
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

    for (UShort_t i = 0; i < fNbDets; i++)
    {
        for (UShort_t j = 0; j < fNbPmts; j++)
        {
            // === TH1F: fine time === //
            sprintf(Name1, "FrsSci%i_FineTimePmt%i", i + 1, j + 1);
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
            sprintf(Name1, "FrsSci%i_Pmt%i_MultPerEvent_Mapped", i + 1, j + 1);
            sprintf(Name2,
                    "FrsSci%i_Pmt%i_MultPerEvent_Mapped (blue no condition on TPAT, red condition on TPAT = 1 or 2)",
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
        sprintf(Name1, "FrsSci%i_MultPerEvent", i + 1);
        fh2_multMap[i] = new TH2I(Name1, Name1, fNbPmts + 1, -0.5, fNbPmts + 0.5, 20, -0.5, 19.5);
        fh2_multMap[i]->GetXaxis()->SetTitle("channel: 1=PMT R,    2=PMT L,    3=COMMON REF");
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
        sprintf(Name1, "FrsSci%i_MultPerEvent_RvsL", i + 1);
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

    // --- ------------ --- //
    // --- TCAL LEVEL --- //
    // --- ------------ --- //

    // === get access to tcal data ===//
    fTcal = dynamic_cast<TClonesArray*>(mgr->GetObject("FrsSciTcalData"));
    if (!fTcal)
    {
        LOG(fatal) << "FrsSciTcal not found";
        return (kFATAL);
    }

    // === declare and create TCanvas and Histogram === //

    sprintf(Name1, "TcalPosRaw_1Hit");
    cTcalPos = new TCanvas(Name1, Name1, 10, 10, 800, 700);
    cTcalPos->Divide(1, fNbDets);
    fh1_PosRawTcal1Hit = new TH1D*[fNbDets * fNbPmts];

    sprintf(Name1, "TcalTofRaw_1Hit");
    cTcalTof = new TCanvas(Name1, Name1, 10, 10, 800, 700);
    cTcalTof->Divide(1, fNbTofs);
    fh1_TofRawTcal1Hit = new TH1D*[fNbTofs];

    for (UShort_t i = 0; i < fNbDets; i++)
    {
        // === TH1F: Raw Position in Ns if mult1 RIGHT and LEFT === //
        sprintf(Name1, "FrsSci%i_PosRaw_MULT1", i + 1);
        fh1_PosRawTcal1Hit[i] = new TH1D(Name1, Name1, 1000, -5000, 5000);
        fh1_PosRawTcal1Hit[i]->GetXaxis()->SetTitle("Raw Positon [ns] if mult1 at L and R");
        fh1_PosRawTcal1Hit[i]->GetYaxis()->SetTitle("number of counts with mult1");
        fh1_PosRawTcal1Hit[i]->GetXaxis()->CenterTitle(true);
        fh1_PosRawTcal1Hit[i]->GetYaxis()->CenterTitle(true);
        fh1_PosRawTcal1Hit[i]->GetXaxis()->SetLabelSize(0.05);
        fh1_PosRawTcal1Hit[i]->GetXaxis()->SetTitleSize(0.05);
        fh1_PosRawTcal1Hit[i]->GetYaxis()->SetLabelSize(0.05);
        fh1_PosRawTcal1Hit[i]->GetYaxis()->SetTitleSize(0.05);
        cTcalPos->cd(i + 1);
        fh1_PosRawTcal1Hit[i]->Draw();
    }

    if (fNbTofs > 0)
    {
        UShort_t cpt = 0;
        for (UShort_t sta = 1; sta < fNbDets; sta++)
        {
            for (UShort_t sto = sta + 1; sto <= fNbDets; sto++)
            {
                sprintf(Name1, "TofRaw_FrsSci%i_to_FrsSci%i_MULT1", sta, sto);
                fh1_TofRawTcal1Hit[cpt] = new TH1D(Name1, Name1, 10000, -50000, 50000);
                fh1_TofRawTcal1Hit[cpt]->GetXaxis()->SetTitle("Raw Tof [ns] if mult1 at L, R and Tref");
                fh1_TofRawTcal1Hit[cpt]->GetYaxis()->SetTitle("number of counts with mult1");
                fh1_TofRawTcal1Hit[cpt]->GetXaxis()->CenterTitle(true);
                fh1_TofRawTcal1Hit[cpt]->GetYaxis()->CenterTitle(true);
                fh1_TofRawTcal1Hit[cpt]->GetXaxis()->SetLabelSize(0.05);
                fh1_TofRawTcal1Hit[cpt]->GetXaxis()->SetTitleSize(0.05);
                fh1_TofRawTcal1Hit[cpt]->GetYaxis()->SetLabelSize(0.05);
                fh1_TofRawTcal1Hit[cpt]->GetYaxis()->SetTitleSize(0.05);
                cTcalTof->cd(cpt + 1);
                fh1_TofRawTcal1Hit[cpt]->Draw();
                cpt++;
            }
        }
    }

    // --- --------------- --- //
    // --- MAIN FOLDER-Sci --- //
    // --- --------------- --- //
    TFolder* mainfolMap = new TFolder("FrsSciMap", "FrsSci Map info");
    mainfolMap->Add(cMapFT);
    mainfolMap->Add(cMapMult1D);
    mainfolMap->Add(cMapMult2D);

    TFolder* mainfolTcal = new TFolder("FrsSciTcal", "FrsSci Tcal info");
    mainfolTcal->Add(cTcalPos);
    mainfolTcal->Add(cTcalTof);

    run->AddObject(mainfolMap);
    run->AddObject(mainfolTcal);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_FRSSCI_HIST", Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

void R3BOnlineSpectraFrsSci::Reset_Histo()
{
    LOG(info) << "R3BOnlineSpectraFrsSci::Reset_Histo";
    for (UShort_t i = 0; i < fNbDets; i++)
    {
        fh2_multMap[i]->Reset();
        fh2_multMap_RvsL[i]->Reset();

        // === FINE TIME === //
        for (UShort_t j = 0; j < fNbPmts; j++)
        {
            fh1_finetime[i * fNbPmts + j]->Reset();
            fh1_multMap[i * fNbPmts + j]->Reset();
        }

        fh1_PosRawTcal1Hit[i]->Reset();
    }

    for (UShort_t i = 0; i < fNbTofs; i++)
    {
        fh1_TofRawTcal1Hit[i]->Reset();
    }
}

void R3BOnlineSpectraFrsSci::Exec(Option_t* option)
{

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        LOG(fatal) << "R3BOnlineSpectraFrsSci::Exec FairRootManager not found";
    }
    UInt_t nHits;
    UShort_t iDet; // 0-based
    UShort_t iPmt; // 0-based
    UShort_t cpt;

    UInt_t multMap[fNbDets * fNbPmts];
    UInt_t multTcal[fNbDets * fNbPmts];
    Double_t Traw[fNbDets * fNbPmts];

    for (UShort_t i = 0; i < fNbDets; i++)
    {
        for (UShort_t j = 0; j < fNbPmts; j++)
        {
            multMap[i * fNbPmts + j] = 0;
            multTcal[i * fNbPmts + j] = 0;
        }
    }

    if (fMapped)
    {
        if (fMapped->GetEntriesFast() > 0)
        {
            // --- ---------------- --- //
            // --- read mapped data --- //
            // --- ---------------- --- //
            nHits = fMapped->GetEntriesFast();
            for (UInt_t ihit = 0; ihit < nHits; ihit++)
            {
                R3BFrsSciMappedData* hitmapped = dynamic_cast<R3BFrsSciMappedData*>(fMapped->At(ihit));
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
            for (UShort_t i = 0; i < fNbDets; i++)
            {
                if (fNbPmts > 1)
                    fh2_multMap_RvsL[i]->Fill(multMap[i * fNbPmts + 1], multMap[i * fNbPmts]);

                for (UShort_t j = 0; j < fNbPmts; j++)
                {
                    fh1_multMap[i * fNbPmts + j]->Fill(multMap[i * fNbPmts + j]);
                    fh2_multMap[i]->Fill(j + 1, multMap[i * fNbPmts + j]);
                }
            }

        } // --- end of if (fMapped->GetEntries() > 0) --- //

        if (fTcal)
        {
            if (fTcal->GetEntries() > 0)
            {
                // --- -------------- --- //
                // --- read tcal data --- //
                // --- -------------- --- //
                nHits = fTcal->GetEntriesFast();
                for (UInt_t ihit = 0; ihit < nHits; ihit++)
                {
                    R3BFrsSciTcalData* hittcal = dynamic_cast<R3BFrsSciTcalData*>(fTcal->At(ihit));
                    if (!hittcal)
                        continue;
                    iDet = hittcal->GetDetector() - 1;
                    iPmt = hittcal->GetPmt() - 1;
                    multTcal[iDet * fNbPmts + iPmt]++;
                    Traw[iDet * fNbPmts + iPmt] = hittcal->GetRawTimeNs();
                } // end of loop over tcal data

                // --- ------------------- --- //
                // --- filling histogramms --- //
                // --- ------------------- --- //
                if (fNbPmts > 1 && fNbDets > 0)
                {
                    // PosRaw [ns] = Tright - Tleft -> x increasing from right to left
                    for (UShort_t i = 0; i < fNbDets; i++)
                    {
                        if (multTcal[i * fNbPmts] == 1 && multTcal[i * fNbPmts + 1] == 1)
                            fh1_PosRawTcal1Hit[i]->Fill(Traw[i * fNbPmts] - Traw[i * fNbPmts + 1]);
                    }
                    // TofRaw [ns] relatively to Tref
                    if (fNbPmts > 2 && fNbDets > 1 && fNbTofs > 0)
                    {
                        cpt = 0;
                        for (UShort_t sta = 0; sta < fNbDets - 1; sta++)
                        {
                            for (UShort_t sto = sta + 1; sto < fNbDets; sto++)
                            {
                                if (multTcal[sta * fNbPmts] == 1 && multTcal[sta * fNbPmts + 1] == 1 &&
                                    multTcal[sta * fNbPmts + 2] == 1 && multTcal[sto * fNbPmts] == 1 &&
                                    multTcal[sto * fNbPmts + 1] == 1 && multTcal[sto * fNbPmts + 2] == 1)
                                {
                                    fh1_TofRawTcal1Hit[cpt]->Fill(
                                        0.5 * (Traw[sto * fNbPmts] + Traw[sto * fNbPmts + 1]) -
                                        Traw[sto * fNbPmts + 2] -
                                        0.5 * (Traw[sta + fNbPmts] + Traw[sta * fNbPmts + 1]) +
                                        Traw[sta * fNbPmts + 2]);
                                    cpt++;
                                } // end of if mult = 1
                            }     // end of loop over stop FrsSci
                        }         // end of loop over start FrsSco
                    }
                }
            } // --- end of if Tcal->GetEntries() >0 --- //
        }     // end of if fTcal

        fNEvents++;
    } // end of if fMapped
}
// -----   Public method Finish   -----------------------------------------------
void R3BOnlineSpectraFrsSci::FinishEvent()
{
    if (fMapped)
    {
        fMapped->Clear();
    }
    if (fTcal)
    {
        fTcal->Clear();
    }
}

void R3BOnlineSpectraFrsSci::FinishTask()
{
    if (fMapped)
    {
        for (UShort_t i = 0; i < fNbDets; i++)
        {
            fh2_multMap[i]->Write();
            fh2_multMap_RvsL[i]->Write();
            for (UShort_t j = 0; j < fNbPmts; j++)
            {
                fh1_finetime[i * fNbPmts + j]->Write();
                fh1_multMap[i * fNbPmts + j]->Write();
            }
        } // end of loop over fNbDets
        if (fTcal)
        {
            for (UShort_t i = 0; i < fNbDets; i++)
            {
                fh1_PosRawTcal1Hit[i]->Write();
            }
            for (UShort_t i = 0; i < fNbTofs; i++)
            {
                fh1_TofRawTcal1Hit[i]->Write();
            }
        }
    }
}
ClassImp(R3BOnlineSpectraFrsSci);
