/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
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
#include "R3BFrsSciTcalData.h"
#include "R3BLosCalData.h"
#include "R3BOnlineSpectraFrsSciVsLos.h"

R3BOnlineSpectraFrsSciVsLos::R3BOnlineSpectraFrsSciVsLos()
    : R3BOnlineSpectraFrsSciVsLos("FrsSciOnlineSpectra", 1)
{
}

R3BOnlineSpectraFrsSciVsLos::R3BOnlineSpectraFrsSciVsLos(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fFrsSci_Tcal(NULL)
    , fNEvents(0)
    , fFrsSciNbDets(1)
    , fFrsSciNbPmts(3)
{
}

R3BOnlineSpectraFrsSciVsLos::~R3BOnlineSpectraFrsSciVsLos()
{
    LOG(debug) << "R3BOnlineSpectraFrsSciVsLos::Destructor";
    if (fFrsSci_Tcal)
        delete fFrsSci_Tcal;
}

InitStatus R3BOnlineSpectraFrsSciVsLos::Init()
{
    LOG(info) << "R3BOnlineSpectraFrsSciVsLos::Init()";

    char Name1[255];

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "R3BOnlineSpectraFrsSciVsLos::Init FairRootManager not found";

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // --- ------------------ --- //
    // --- GET ACCESS TO DATA --- //
    // --- ------------------ --- //

    // === get access to EventHeader for GetTStartMaster()=<T_LOS_VFTX>-TrefMASTER ===//
    fHeader = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));

    // === get access to frssci tcal data ===//
    fFrsSci_Tcal = dynamic_cast<TClonesArray*>(mgr->GetObject("FrsSciTcalData"));
    if (!fFrsSci_Tcal)
    {
        LOG(info) << "R3BOnlineSpectraFrsSciVsLos::Init() :: FrsSciTcal not found";
    }

    // --- ---------------------------- --- //
    // --- DECLARATION TCANVAS + HISTOS --- //
    // --- ---------------------------- --- //

    if (fFrsSci_Tcal && fHeader)
    {
        sprintf(Name1, "TofRaw_TcalLVL");
        cTcal_TofRaw = new TCanvas(Name1, Name1, 10, 10, 800, 700);
        cTcal_TofRaw->Divide(1, fFrsSciNbDets);
        fh1_Tcal1Hit_TofRaw = new TH1D*[fFrsSciNbDets];

        for (UShort_t i = 0; i < fFrsSciNbDets; i++)
        {
            sprintf(Name1, "FrsSci%i_to_Los_TofRaw_MULT1", i + 1);
            fh1_Tcal1Hit_TofRaw[i] = new TH1D(Name1, Name1, 1000, -1000, 1000);
            fh1_Tcal1Hit_TofRaw[i]->GetXaxis()->SetTitle(Form("Raw Tof from FrsSci %i to Los [ns], mult1", i + 1));
            fh1_Tcal1Hit_TofRaw[i]->GetYaxis()->SetTitle("Nb of counts, with Mult1");
            fh1_Tcal1Hit_TofRaw[i]->GetXaxis()->CenterTitle(true);
            fh1_Tcal1Hit_TofRaw[i]->GetYaxis()->CenterTitle(true);
            fh1_Tcal1Hit_TofRaw[i]->GetXaxis()->SetLabelSize(0.05);
            fh1_Tcal1Hit_TofRaw[i]->GetXaxis()->SetTitleSize(0.05);
            fh1_Tcal1Hit_TofRaw[i]->GetYaxis()->SetLabelSize(0.05);
            fh1_Tcal1Hit_TofRaw[i]->GetYaxis()->SetTitleSize(0.05);
            cTcal_TofRaw->cd(i + 1);
            fh1_Tcal1Hit_TofRaw[i]->Draw();
        }
    }

    // --- --------------- --- //
    // --- MAIN FOLDER-Sci --- //
    // --- --------------- --- //
    TFolder* mainfol = new TFolder("FrsSciVsLos", "FrsSci vs Musli info");

    if (fFrsSci_Tcal && fHeader)
    {
        mainfol->Add(cTcal_TofRaw);
    }
    run->AddObject(mainfol);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_FrsSci_Tof_HIST", Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

void R3BOnlineSpectraFrsSciVsLos::Reset_Histo()
{

    LOG(info) << "R3BOnlineSpectraFrsSciVsLos::Reset_Histo";

    if (fFrsSci_Tcal && fHeader)
    {
        for (UShort_t i = 0; i < fFrsSciNbDets; i++)
        {
            fh1_Tcal1Hit_TofRaw[i]->Reset();
        }
    }
}

void R3BOnlineSpectraFrsSciVsLos::Exec(Option_t* option)
{

    UShort_t iFrsSciDet; // 0-based
    UShort_t iFrsSciPmt; // 0-based
    UInt_t multFrsSciTcal[fFrsSciNbDets * fFrsSciNbPmts];
    Double_t FrsSciTraw[fFrsSciNbDets * fFrsSciNbPmts];
    Double_t StartTraw = -1;
    Double_t TofRaw = -1;

    for (UShort_t i = 0; i < fFrsSciNbDets; i++)
    {
        for (UShort_t j = 0; j < fFrsSciNbPmts; j++)
        {
            multFrsSciTcal[i * fFrsSciNbPmts + j] = 0;
            FrsSciTraw[i * fFrsSciNbPmts + j] = -1;
        }
    }

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        LOG(fatal) << "R3BOnlineSpectraFrsSciVsLos::Exec FairRootManager not found";
    }

    UInt_t nHits;
    if (fFrsSci_Tcal && fHeader)
    {
        nHits = fFrsSci_Tcal->GetEntriesFast();
        for (UInt_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BFrsSciTcalData* hitscitcal = dynamic_cast<R3BFrsSciTcalData*>(fFrsSci_Tcal->At(ihit));
            if (!hitscitcal)
                continue;
            iFrsSciDet = hitscitcal->GetDetector() - 1;
            iFrsSciPmt = hitscitcal->GetPmt() - 1;
            multFrsSciTcal[iFrsSciDet * fFrsSciNbPmts + iFrsSciPmt]++;
            FrsSciTraw[iFrsSciDet * fFrsSciNbPmts + iFrsSciPmt] = hitscitcal->GetRawTimeNs();
        } // end of loop over tcal data

        for (UShort_t i = 0; i < fFrsSciNbDets; i++)
        {
            if (multFrsSciTcal[i * fFrsSciNbPmts] == 1 && multFrsSciTcal[i * fFrsSciNbPmts + 1] == 1 &&
                multFrsSciTcal[i * fFrsSciNbPmts + 2] == 1)
            {
                StartTraw = 0.5 * (FrsSciTraw[i * fFrsSciNbPmts] + FrsSciTraw[i * fFrsSciNbPmts + 1]);
                TofRaw = fHeader->GetTStartMaster() - StartTraw + FrsSciTraw[i * fFrsSciNbPmts + 2];
                fh1_Tcal1Hit_TofRaw[i]->Fill(TofRaw);
            }
        }
    }

    fNEvents++;
}

// -----   Public method Finish   -----------------------------------------------
void R3BOnlineSpectraFrsSciVsLos::FinishEvent()
{
    if (fFrsSci_Tcal)
    {
        fFrsSci_Tcal->Clear();
    }
}

void R3BOnlineSpectraFrsSciVsLos::FinishTask()
{

    if (fFrsSci_Tcal && fHeader)
    {
        for (UShort_t i = 0; i < fFrsSciNbDets; i++)
        {
            fh1_Tcal1Hit_TofRaw[i]->Write();
        }
    }
}
ClassImp(R3BOnlineSpectraFrsSciVsLos);
