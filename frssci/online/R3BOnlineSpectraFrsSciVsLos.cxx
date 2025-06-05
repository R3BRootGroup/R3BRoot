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
#include "R3BFrsSciCalPar.h"
#include "R3BFrsSciPosCalData.h"
#include "R3BFrsSciTcalData.h"
#include "R3BIncomingIDPar.h"
#include "R3BLosHitData.h"
#include "R3BLosTCalData.h"
#include "R3BOnlineSpectraFrsSciVsLos.h"

R3BOnlineSpectraFrsSciVsLos::R3BOnlineSpectraFrsSciVsLos()
    : R3BOnlineSpectraFrsSciVsLos("R3BOnlineSpectraFrsSciVsLos", 1)
{
}

R3BOnlineSpectraFrsSciVsLos::R3BOnlineSpectraFrsSciVsLos(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fFrsSci_Tcal(NULL)
    , fFrsSci_PosCal(NULL)
    , fLos_Tcal(NULL)
    , fLos_Hit(NULL)
    , fIncomingID_Par(NULL)
    , fCalPar(NULL)
    , fNEvents(0)
    , fFrsSciNbDets(1)
    , fFrsSciNbPmts(3)
    , fLosNbDets(1)
    , fpos_range_min(-5)
    , fpos_range_max(5)
{
    fTof2InvV_p0 = new TArrayF(fFrsSciNbDets);
    fTof2InvV_p1 = new TArrayF(fFrsSciNbDets);
    fDispersionS2 = new TArrayF(fFrsSciNbDets);
    fBrho0_S2toCC = new TArrayF(fFrsSciNbDets);
    fPosCal_Gain = new TArrayF(fFrsSciNbDets);
    fPosCal_Offset = new TArrayF(fFrsSciNbDets);
}

R3BOnlineSpectraFrsSciVsLos::~R3BOnlineSpectraFrsSciVsLos()
{
    LOG(debug) << "R3BOnlineSpectraFrsSciVsLos::Destructor";
    if (fFrsSci_Tcal)
        delete fFrsSci_Tcal;
    if (fFrsSci_PosCal)
        delete fFrsSci_PosCal;
    if (fLos_Tcal)
        delete fLos_Tcal;
    if (fLos_Hit)
        delete fLos_Hit;
}

void R3BOnlineSpectraFrsSciVsLos::SetParContainers()
{
    // Reading IncomingIDPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
        LOG(fatal) << "FairRuntimeDb not found";

    fCalPar = dynamic_cast<R3BFrsSciCalPar*>(rtdb->getContainer("FrsSciCalPar"));
    if (!fCalPar)
        LOG(fatal) << "Couldn't get handle on FrsSciCalPar container";
    else
        LOG(info) << "FrsSciCalPar container was found";

    fIncomingID_Par = dynamic_cast<R3BIncomingIDPar*>(rtdb->getContainer("IncomingIDPar"));
    if (!fIncomingID_Par)
        LOG(fatal) << "Couldn't get handle on IncomingIDPar container";
    else
        LOG(info) << "IncomingIDPar container was found";

    return;
}

void R3BOnlineSpectraFrsSciVsLos::SetParameters()
{
    for (Int_t i = 1; i < fFrsSciNbDets + 1; i++)
    {
        fTof2InvV_p0->AddAt(fIncomingID_Par->GetTof2InvV_p0(i), i - 1);
        fTof2InvV_p1->AddAt(fIncomingID_Par->GetTof2InvV_p1(i), i - 1);
        fDispersionS2->AddAt(fIncomingID_Par->GetDispersionS2(i), i - 1);
        fBrho0_S2toCC->AddAt(fIncomingID_Par->GetBrho0_S2toCC(i), i - 1);
        fPosCal_Gain->AddAt(fCalPar->GetPosCalGainAtRank(i - 1), i - 1);
        fPosCal_Offset->AddAt(fCalPar->GetPosCalOffsetAtRank(i - 1), i - 1);
    }
    return;
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
    // === get access to pos cal data ===//
    fFrsSci_PosCal = dynamic_cast<TClonesArray*>(mgr->GetObject("FrsSciPosCalData"));
    if (!fFrsSci_PosCal)
    {
        LOG(info) << "R3BOnlineSpectraFrsSciVsLos::Init() :: FrsSciPosCal not found";
    }
    // === get access to los cal data ===//
    fLos_Tcal = dynamic_cast<TClonesArray*>(mgr->GetObject("LosTCal"));
    if (!fLos_Tcal)
    {
        LOG(info) << "R3BOnlineSpectraFrsSciVsLos::Init() :: LosTCal not found";
    }
    // === get access to los hit data ===//
    fLos_Hit = dynamic_cast<TClonesArray*>(mgr->GetObject("LosHit"));
    if (!fLos_Hit)
    {
        LOG(info) << "R3BOnlineSpectraFrsSciVsLos::Init() :: LosHit not found";
    }

    // --- -------------------- --- //
    // --- GET PARAMETERS VALUE --- //
    // --- -------------------- --- //
    SetParameters();

    // --- ---------------------------- --- //
    // --- DECLARATION TCANVAS + HISTOS --- //
    // --- ---------------------------- --- //

    if (fFrsSci_Tcal && fHeader)
    {
        // TofRaw
        sprintf(Name1, "TofRaw_TcalLVL");
        cTcal_TofRaw = new TCanvas(Name1, Name1, 10, 10, 800, 700);
        cTcal_TofRaw->Divide(2, fFrsSciNbDets);
        fh1_Tcal1Hit_TofRaw = new TH1D*[fFrsSciNbDets];
        fh1_TofRaw = new TH1D*[fFrsSciNbDets];

        sprintf(Name1, "multTofRaw");
        cTcal_multTofRaw = new TCanvas(Name1, Name1, 10, 10, 800, 700);
        cTcal_multTofRaw->Divide(2, fFrsSciNbDets);
        fh1_multTofRaw = new TH1I*[fFrsSciNbDets];
        fh1_multTofRaw_Zgt3 = new TH1I*[fFrsSciNbDets];

        // AoQ
        sprintf(Name1, "AoQ");
        cAoQ = new TCanvas(Name1, Name1, 10, 10, 800, 700);
        cAoQ->Divide(2, fFrsSciNbDets);
        fh1_Tcal1Hit_AoQraw = new TH1D*[fFrsSciNbDets];
        fh1_AoQcal = new TH1D*[fFrsSciNbDets];

        sprintf(Name1, "AoQ_vs_PosS2");
        cAoQ_vs_PosS2 = new TCanvas(Name1, Name1, 10, 10, 800, 700);
        cAoQ_vs_PosS2->Divide(1, fFrsSciNbDets);
        fh2_AoQcal_vs_PosS2 = new TH2D*[fFrsSciNbDets];

        sprintf(Name1, "Zlos_vs_AoQ");
        cZ_vs_AoQ = new TCanvas(Name1, Name1, 10, 10, 800, 700);
        cZ_vs_AoQ->Divide(1, fFrsSciNbDets);
        fh2_Z_vs_AoQcal = new TH2D*[fFrsSciNbDets];

        sprintf(Name1, "los_mult");
        cLos_Mult = new TCanvas(Name1, Name1, 10, 10, 800, 700);
        cLos_Mult->Divide(2, fLosNbDets);
        fh2_MultLos_LE = new TH2I*[fLosNbDets];
        fh2_MultLos_TE = new TH2I*[fLosNbDets];

        sprintf(Name1, "los_Mean_ToT");
        cLos_ToT = new TCanvas(Name1, Name1, 10, 10, 800, 700);
        cLos_ToT->Divide(1, fLosNbDets);
        fh1_MeanToT = new TH1D*[fLosNbDets];

        for (UShort_t i = 0; i < fFrsSciNbDets; i++)
        {
            // TofRaw
            sprintf(Name1, "FrsSci%i_to_Los_TofRaw_MULT1_TPAT1", i + 1);
            fh1_Tcal1Hit_TofRaw[i] = new TH1D(Name1, Name1, 2000000, -10000, 10000);
            fh1_Tcal1Hit_TofRaw[i]->GetXaxis()->SetTitle(Form("Raw Tof from FrsSci %i to Los [ns], mult1", i + 1));
            fh1_Tcal1Hit_TofRaw[i]->GetYaxis()->SetTitle("Nb of counts, with Mult1");
            fh1_Tcal1Hit_TofRaw[i]->GetXaxis()->CenterTitle(true);
            fh1_Tcal1Hit_TofRaw[i]->GetYaxis()->CenterTitle(true);
            fh1_Tcal1Hit_TofRaw[i]->GetXaxis()->SetLabelSize(0.05);
            fh1_Tcal1Hit_TofRaw[i]->GetXaxis()->SetTitleSize(0.05);
            fh1_Tcal1Hit_TofRaw[i]->GetYaxis()->SetLabelSize(0.05);
            fh1_Tcal1Hit_TofRaw[i]->GetYaxis()->SetTitleSize(0.05);
            cTcal_TofRaw->cd(2 * i + 1);
            fh1_Tcal1Hit_TofRaw[i]->Draw();

            sprintf(Name1, "FrsSci%i_to_Los_GoodTofRaw_MULT1GoodTof_TPAT1", i + 1);
            fh1_TofRaw[i] = new TH1D(Name1, Name1, 1000000, -5000, 5000);
            fh1_TofRaw[i]->GetXaxis()->SetTitle(Form("Raw Tof from FrsSci %i to Los [ns]", i + 1));
            fh1_TofRaw[i]->GetYaxis()->SetTitle("Nb of counts");
            fh1_TofRaw[i]->GetXaxis()->CenterTitle(true);
            fh1_TofRaw[i]->GetYaxis()->CenterTitle(true);
            fh1_TofRaw[i]->GetXaxis()->SetLabelSize(0.05);
            fh1_TofRaw[i]->GetXaxis()->SetTitleSize(0.05);
            fh1_TofRaw[i]->GetYaxis()->SetLabelSize(0.05);
            fh1_TofRaw[i]->GetYaxis()->SetTitleSize(0.05);
            cTcal_TofRaw->cd(2 * i + 2);
            fh1_TofRaw[i]->Draw();

            // mult GoodTofRaw
            sprintf(Name1, "MultFrsSci%i_to_Los_GoodTofRaw_allMULT_TPAT1", i + 1);
            fh1_multTofRaw[i] = new TH1I(Name1, Name1, 10, -0.5, 9.5);
            fh1_multTofRaw[i]->GetXaxis()->SetTitle(Form("GoodTofRaw multiplicity(FrsSci%i-Los)", i + 1));
            fh1_multTofRaw[i]->GetYaxis()->SetTitle("Nb of counts");
            fh1_multTofRaw[i]->GetXaxis()->CenterTitle(true);
            fh1_multTofRaw[i]->GetYaxis()->CenterTitle(true);
            fh1_multTofRaw[i]->GetXaxis()->SetLabelSize(0.05);
            fh1_multTofRaw[i]->GetXaxis()->SetTitleSize(0.05);
            fh1_multTofRaw[i]->GetYaxis()->SetLabelSize(0.05);
            fh1_multTofRaw[i]->GetYaxis()->SetTitleSize(0.05);
            cTcal_multTofRaw->cd(2 * i + 1);
            fh1_multTofRaw[i]->Draw();

            sprintf(Name1, "MultFrsSci%i_to_Los_GoodTofRaw_allMULT_TPAT1_Zgt3", i + 1);
            fh1_multTofRaw_Zgt3[i] = new TH1I(Name1, Name1, 10, -0.5, 9.5);
            fh1_multTofRaw_Zgt3[i]->GetXaxis()->SetTitle(
                Form("GoodTofRaw multiplicity(FrsSci%i-Los) && Zlos>3", i + 1));
            fh1_multTofRaw_Zgt3[i]->GetYaxis()->SetTitle("Nb of counts");
            fh1_multTofRaw_Zgt3[i]->GetXaxis()->CenterTitle(true);
            fh1_multTofRaw_Zgt3[i]->GetYaxis()->CenterTitle(true);
            fh1_multTofRaw_Zgt3[i]->GetXaxis()->SetLabelSize(0.05);
            fh1_multTofRaw_Zgt3[i]->GetXaxis()->SetTitleSize(0.05);
            fh1_multTofRaw_Zgt3[i]->GetYaxis()->SetLabelSize(0.05);
            fh1_multTofRaw_Zgt3[i]->GetYaxis()->SetTitleSize(0.05);
            cTcal_multTofRaw->cd(2 * i + 2);
            fh1_multTofRaw_Zgt3[i]->Draw();

            // AoQraw
            sprintf(Name1, "FrsSci%i_AoQraw_MULT1_TPAT1", i + 1);
            fh1_Tcal1Hit_AoQraw[i] = new TH1D(Name1, Name1, 10000, 2.0, 3.0);
            fh1_Tcal1Hit_AoQraw[i]->GetXaxis()->SetTitle("A/Q raw (no dispersion, mult1");
            fh1_Tcal1Hit_AoQraw[i]->GetYaxis()->SetTitle("Nb of counts, with Mult1");
            fh1_Tcal1Hit_AoQraw[i]->GetXaxis()->CenterTitle(true);
            fh1_Tcal1Hit_AoQraw[i]->GetYaxis()->CenterTitle(true);
            fh1_Tcal1Hit_AoQraw[i]->GetXaxis()->SetLabelSize(0.05);
            fh1_Tcal1Hit_AoQraw[i]->GetXaxis()->SetTitleSize(0.05);
            fh1_Tcal1Hit_AoQraw[i]->GetYaxis()->SetLabelSize(0.05);
            fh1_Tcal1Hit_AoQraw[i]->GetYaxis()->SetTitleSize(0.05);
            cAoQ->cd(2 * i + 1);
            fh1_Tcal1Hit_AoQraw[i]->Draw();

            if (fFrsSci_PosCal)
            {
                // AoQcal
                sprintf(Name1, "FrsSci%i_AoQcal_TPAT1_GoodTofMult1", i + 1);
                fh1_AoQcal[i] = new TH1D(Name1, Name1, 10000, 2.1, 3.1);
                fh1_AoQcal[i]->GetXaxis()->SetTitle("A/Q cal");
                fh1_AoQcal[i]->GetYaxis()->SetTitle("Nb of counts, with Mult1");
                fh1_AoQcal[i]->GetXaxis()->CenterTitle(true);
                fh1_AoQcal[i]->GetYaxis()->CenterTitle(true);
                fh1_AoQcal[i]->GetXaxis()->SetLabelSize(0.05);
                fh1_AoQcal[i]->GetXaxis()->SetTitleSize(0.05);
                fh1_AoQcal[i]->GetYaxis()->SetLabelSize(0.05);
                fh1_AoQcal[i]->GetYaxis()->SetTitleSize(0.05);
                cAoQ->cd(2 * i + 2);
                fh1_AoQcal[i]->Draw();

                // AoQcal vs PosS2
                sprintf(Name1, "FrsSci%i_AoQcal_vs_PosS2_TPAT1_GoodTofMult1", i + 1);
                fh2_AoQcal_vs_PosS2[i] = new TH2D(Name1, Name1, 1100, -110, 110, 900, 2.1, 3.0);
                fh2_AoQcal_vs_PosS2[i]->GetXaxis()->SetTitle(Form("Position [mm] at FrsSci%i", i + 1));
                fh2_AoQcal_vs_PosS2[i]->GetYaxis()->SetTitle(Form("A/Q from FrsSci%i to Los", i + 1));
                fh2_AoQcal_vs_PosS2[i]->GetXaxis()->CenterTitle(true);
                fh2_AoQcal_vs_PosS2[i]->GetYaxis()->CenterTitle(true);
                fh2_AoQcal_vs_PosS2[i]->GetXaxis()->SetLabelSize(0.05);
                fh2_AoQcal_vs_PosS2[i]->GetXaxis()->SetTitleSize(0.05);
                fh2_AoQcal_vs_PosS2[i]->GetYaxis()->SetLabelSize(0.05);
                fh2_AoQcal_vs_PosS2[i]->GetYaxis()->SetTitleSize(0.05);
                cAoQ_vs_PosS2->cd(i + 1);
                fh2_AoQcal_vs_PosS2[i]->Draw("col");

                // AoQcal vs Z
                if (fLos_Hit)
                {
                    sprintf(Name1, "FrsSci%i_Z_vs_AoQcal_TPAT1_GoodTofMult1", i + 1);
                    fh2_Z_vs_AoQcal[i] = new TH2D(Name1, Name1, 1500, 2.1, 3.6, 1900, 0.5, 19.5);
                    fh2_Z_vs_AoQcal[i]->GetXaxis()->SetTitle(Form("A/Q cal from FrsSci %i to Los, mult1", i + 1));
                    fh2_Z_vs_AoQcal[i]->GetYaxis()->SetTitle("nuclear charge from LOS");
                    fh2_Z_vs_AoQcal[i]->GetXaxis()->CenterTitle(true);
                    fh2_Z_vs_AoQcal[i]->GetYaxis()->CenterTitle(true);
                    fh2_Z_vs_AoQcal[i]->GetXaxis()->SetLabelSize(0.05);
                    fh2_Z_vs_AoQcal[i]->GetXaxis()->SetTitleSize(0.05);
                    fh2_Z_vs_AoQcal[i]->GetYaxis()->SetLabelSize(0.05);
                    fh2_Z_vs_AoQcal[i]->GetYaxis()->SetTitleSize(0.05);
                    cZ_vs_AoQ->cd(i + 1);
                    fh2_Z_vs_AoQcal[i]->Draw("col");
                }
            } // end of if fFrsSci_PosCal
        }     // end of loop over the fFrsSciNbDets

        if (fLos_Tcal)
        {
            for (UShort_t i = 0; i < fLosNbDets; i++)
            {
                // MultLos_LE
                sprintf(Name1, "Los%i_LE_Mult_TPAT1", i + 1);
                fh2_MultLos_LE[i] = new TH2I(Name1, Name1, 10, -0.5, 9.5, 20, 0, 20);
                fh2_MultLos_LE[i]->GetXaxis()->SetTitle(Form("LOS%i_LE PMT", i + 1));
                fh2_MultLos_LE[i]->GetYaxis()->SetTitle("Mult");
                fh2_MultLos_LE[i]->GetXaxis()->CenterTitle(true);
                fh2_MultLos_LE[i]->GetYaxis()->CenterTitle(true);
                fh2_MultLos_LE[i]->GetXaxis()->SetLabelSize(0.05);
                fh2_MultLos_LE[i]->GetXaxis()->SetTitleSize(0.05);
                fh2_MultLos_LE[i]->GetYaxis()->SetLabelSize(0.05);
                fh2_MultLos_LE[i]->GetYaxis()->SetTitleSize(0.05);
                cLos_Mult->cd(2 * i + 1);
                fh2_MultLos_LE[i]->Draw("colz");
                // MultLos_TE
                sprintf(Name1, "Los%i_TE_Mult_TPAT1", i + 1);
                fh2_MultLos_TE[i] = new TH2I(Name1, Name1, 10, -0.5, 9.5, 20, 0, 20);
                fh2_MultLos_TE[i]->GetXaxis()->SetTitle(Form("LOS%i_TE PMT", i + 1));
                fh2_MultLos_TE[i]->GetYaxis()->SetTitle("Mult");
                fh2_MultLos_TE[i]->GetXaxis()->CenterTitle(true);
                fh2_MultLos_TE[i]->GetYaxis()->CenterTitle(true);
                fh2_MultLos_TE[i]->GetXaxis()->SetLabelSize(0.05);
                fh2_MultLos_TE[i]->GetXaxis()->SetTitleSize(0.05);
                fh2_MultLos_TE[i]->GetYaxis()->SetLabelSize(0.05);
                fh2_MultLos_TE[i]->GetYaxis()->SetTitleSize(0.05);
                cLos_Mult->cd(2 * i + 2);
                fh2_MultLos_TE[i]->Draw("colz");
                // Mean Time-over-Threshold
                sprintf(Name1, "Los%i_MeanToT_MULT1atLOS_TPAT1", i + 1);
                fh1_MeanToT[i] = new TH1D(Name1, Name1, 5000, 0, 1000);
                fh1_MeanToT[i]->GetXaxis()->SetTitle(Form("Mean Time-over_Threshold LOS%i [ns], mult1", i + 1));
                fh1_MeanToT[i]->GetYaxis()->SetTitle("Nb of counts, with Mult1");
                fh1_MeanToT[i]->GetXaxis()->CenterTitle(true);
                fh1_MeanToT[i]->GetYaxis()->CenterTitle(true);
                fh1_MeanToT[i]->GetXaxis()->SetLabelSize(0.05);
                fh1_MeanToT[i]->GetXaxis()->SetTitleSize(0.05);
                fh1_MeanToT[i]->GetYaxis()->SetLabelSize(0.05);
                fh1_MeanToT[i]->GetYaxis()->SetTitleSize(0.05);
                cLos_ToT->cd(i + 1);
                fh1_MeanToT[i]->Draw();
            } // end of loop over the number of Los detectors
        }     // end of if (fLos_Tcal)
    }

    // --- --------------- --- //
    // --- MAIN FOLDER-Sci --- //
    // --- --------------- --- //
    TFolder* mainfol = new TFolder("FrsSciVsLos", "FrsSci vs Musli info");

    if (fFrsSci_Tcal && fHeader)
    {
        mainfol->Add(cTcal_TofRaw);
        mainfol->Add(cTcal_multTofRaw);
        mainfol->Add(cAoQ);
        mainfol->Add(cAoQ_vs_PosS2);
        mainfol->Add(cZ_vs_AoQ);
        mainfol->Add(cLos_Mult);
        mainfol->Add(cLos_ToT);
    }
    run->AddObject(mainfol);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_FrsSci_LOS_HIST", Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

void R3BOnlineSpectraFrsSciVsLos::Reset_Histo()
{

    LOG(info) << "R3BOnlineSpectraFrsSciVsLos::Reset_Histo";

    if (fFrsSci_Tcal)
    {
        for (UShort_t i = 0; i < fFrsSciNbDets; i++)
        {
            fh1_Tcal1Hit_TofRaw[i]->Reset();
            fh1_TofRaw[i]->Reset();
            fh1_multTofRaw[i]->Reset();
            fh1_multTofRaw_Zgt3[i]->Reset();
            fh1_Tcal1Hit_AoQraw[i]->Reset();
            if (fFrsSci_PosCal)
            {
                fh1_AoQcal[i]->Reset();
                fh2_AoQcal_vs_PosS2[i]->Reset();
            }

            if (fLos_Hit)
            {
                fh2_Z_vs_AoQcal[i]->Reset();
            }
        }
        for (UShort_t i = 0; i < fLosNbDets; i++)
        {
            if (fLos_Tcal)
            {
                fh2_MultLos_LE[i]->Reset();
                fh2_MultLos_TE[i]->Reset();
                fh1_MeanToT[i]->Reset();
            }
        }
    }
}

void R3BOnlineSpectraFrsSciVsLos::Exec(Option_t* option)
{

    UShort_t iDet; // 0-based
    UShort_t iPmt; // 0-based
    UInt_t multFrsSciTcal[fFrsSciNbDets * fFrsSciNbPmts];
    UInt_t multFrsSciPosCal[fFrsSciNbDets];
    UInt_t multTofRaw[fFrsSciNbDets];
    UInt_t multLosTcal_LE[fLosNbDets * 8];
    UInt_t multLosTcal_TE[fLosNbDets * 8];
    UInt_t multLosHit[fFrsSciNbDets];
    UInt_t multTotLos_LE[fLosNbDets];
    UInt_t multTotLos_TE[fLosNbDets];
    Bool_t kLosMult2[fLosNbDets];
    Bool_t kFlag_LosOk[fLosNbDets];
    Double_t FrsSciTraw[fFrsSciNbDets * fFrsSciNbPmts][64];
    Double_t LosTcal_LE[fLosNbDets * 8];
    Double_t LosTcal_TE[fLosNbDets * 8];
    Double_t StartTraw_atTcal = -1;
    Double_t StartTraw_atCal[fFrsSciNbDets];
    Double_t TofRaw = -1;
    Double_t PosRaw = -100000;
    Double_t PosCal = -100000;
    Double_t Velocity;
    Double_t Beta;
    Double_t Gamma;
    Double_t AoQraw;
    Double_t AoQcal;
    Double_t Brho;
    Double_t PosCal_mm[fFrsSciNbDets];
    Double_t Zcharge[fFrsSciNbDets];

    Int_t indexl = -1;
    Int_t indexr = -1;
    for (UShort_t i = 0; i < fFrsSciNbDets; i++)
    {
        for (UShort_t j = 0; j < fFrsSciNbPmts; j++)
        {
            multFrsSciTcal[i * fFrsSciNbPmts + j] = 0;
            for (UShort_t k = 0; k < 63; k++)
                FrsSciTraw[i * fFrsSciNbPmts + j][k] = -1;
        }
        multTofRaw[i] = 0;
        multFrsSciPosCal[i] = 0;
        multLosHit[i] = 0;
        PosCal_mm[i] = -10000;
        StartTraw_atCal[i] = -10000;
        Zcharge[i] = -10000;
    }
    for (UShort_t i = 0; i < fLosNbDets; i++)
    {
        for (UShort_t j = 0; j < 8; j++)
        {
            multLosTcal_LE[i * 8 + j] = 0;
            multLosTcal_TE[i * 8 + j] = 0;
            LosTcal_LE[i * 8 + j] = -1;
            LosTcal_TE[i * 8 + j] = -1;
        }
        multTotLos_LE[i] = 0;
        multTotLos_TE[i] = 0;
        kLosMult2[i] = false;
        kFlag_LosOk[i] = false;
    }

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        LOG(fatal) << "R3BOnlineSpectraFrsSciVsLos::Exec FairRootManager not found";
    }

    UInt_t nHits;
    if (fFrsSci_Tcal && fHeader)
    {
        if (fHeader->GetTpat() == 1)
        {
            nHits = fFrsSci_Tcal->GetEntriesFast();
            for (UInt_t ihit = 0; ihit < nHits; ihit++)
            {
                R3BFrsSciTcalData* hitscitcal = dynamic_cast<R3BFrsSciTcalData*>(fFrsSci_Tcal->At(ihit));
                if (!hitscitcal)
                    continue;
                iDet = hitscitcal->GetDetector() - 1;
                iPmt = hitscitcal->GetPmt() - 1;
                FrsSciTraw[iDet * fFrsSciNbPmts + iPmt][multFrsSciTcal[iDet * fFrsSciNbPmts + iPmt]] =
                    hitscitcal->GetRawTimeNs();
                multFrsSciTcal[iDet * fFrsSciNbPmts + iPmt]++;
            } // end of loop over tcal data

            if (fLos_Tcal)
            {
                nHits = fLos_Tcal->GetEntriesFast();
                for (UInt_t ihit = 0; ihit < nHits; ihit++)
                {
                    R3BLosTCalData* hitlostcal = dynamic_cast<R3BLosTCalData*>(fLos_Tcal->At(ihit));
                    if (!hitlostcal)
                        continue;
                    iDet = hitlostcal->GetDetector() - 1;
                    iPmt = hitlostcal->GetChannel() - 1;
                    // fType : 0 (VFTX), 1 (TAMEX leading), 2 (TAMEX trailing)
                    if (hitlostcal->GetType() == 0)
                        continue;
                    else if (hitlostcal->GetType() == 1)
                    {
                        multLosTcal_LE[iDet * 8 + iPmt]++;
                        LosTcal_LE[iDet * 8 + iPmt] = hitlostcal->GetRawTimeNs();
                        if (multLosTcal_LE[iDet * 8 + iPmt] > 1)
                            kLosMult2[iDet] = true;
                        multTotLos_LE[iDet]++;
                    }
                    else if (hitlostcal->GetType() == 2)
                    {
                        multLosTcal_TE[iDet * 8 + iPmt]++;
                        LosTcal_TE[iDet * 8 + iPmt] = hitlostcal->GetRawTimeNs();
                        ;
                        if (multLosTcal_TE[iDet * 8 + iPmt] > 1)
                            kLosMult2[iDet] = true;
                        multTotLos_TE[iDet]++;
                    }
                    else
                    {
                        LOG(info) << "R3BOnlineSpectraFrsSciVsLos::Exec TAMEX LOS DATA HAS A TYPE "
                                  << hitlostcal->GetType();
                    }
                } // end of loop over tcal data

                for (UShort_t i = 0; i < fLosNbDets; i++)
                {
                    for (UShort_t pmt = 0; pmt < 8; pmt++)
                    {
                        fh2_MultLos_LE[i]->Fill(pmt, multLosTcal_LE[i * 8 + pmt]);
                        fh2_MultLos_TE[i]->Fill(pmt, multLosTcal_TE[i * 8 + pmt]);
                    }
                    if (multTotLos_LE[i] == 8 && multTotLos_TE[i] == 8 && kLosMult2[i] == false)
                        kFlag_LosOk[i] = true;
                }

            } // end of if fLos_Tcal

            if (fFrsSci_PosCal)
            {
                nHits = fFrsSci_PosCal->GetEntriesFast();
                for (UInt_t ihit = 0; ihit < nHits; ihit++)
                {
                    R3BFrsSciPosCalData* hitposcal = dynamic_cast<R3BFrsSciPosCalData*>(fFrsSci_PosCal->At(ihit));
                    if (!hitposcal)
                        continue;
                    iDet = hitposcal->GetDetector() - 1;
                    multFrsSciPosCal[iDet]++;
                    PosCal_mm[iDet] = hitposcal->GetCalPosMm();
                    StartTraw_atCal[iDet] = hitposcal->GetRawTimeNsWithTref();
                } // end of loop over tcal data
            }
            if (fLos_Hit)
            {
                nHits = fLos_Hit->GetEntriesFast();
                for (UInt_t ihit = 0; ihit < nHits; ihit++)
                {
                    R3BLosHitData* hitloshit = dynamic_cast<R3BLosHitData*>(fLos_Hit->At(ihit));
                    if (!hitloshit)
                        continue;
                    iDet = hitloshit->GetDetector() - 1;
                    multLosHit[iDet]++;
                    Zcharge[iDet] = hitloshit->GetZ();
                } // end of loop over tcal data
            }

            if (kFlag_LosOk[0] == true)
            {

                for (UShort_t i = 0; i < fFrsSciNbDets; i++)
                {
                    // Calculate the ToT from first Los Detector
                    Double_t MeanToT = 0;
                    for (UShort_t pmt = 0; pmt < 8; pmt++)
                    {
                        MeanToT += (LosTcal_TE[pmt] - LosTcal_LE[pmt]);
                    }
                    MeanToT = MeanToT / 8.;
                    fh1_MeanToT[0]->Fill(MeanToT);

                    // Raw time of flight to find the limits
                    if (multFrsSciTcal[i * fFrsSciNbPmts] == 1 && multFrsSciTcal[i * fFrsSciNbPmts + 1] == 1 &&
                        multFrsSciTcal[i * fFrsSciNbPmts + 2] == 1)
                    {
                        // Calculate the A/Q
                        StartTraw_atTcal =
                            0.5 * (FrsSciTraw[i * fFrsSciNbPmts][0] + FrsSciTraw[i * fFrsSciNbPmts + 1][0]) -
                            FrsSciTraw[i * fFrsSciNbPmts + 2][0];
                        TofRaw = fHeader->GetTStartMaster() - StartTraw_atTcal;

                        Velocity = 1. / (fTof2InvV_p0->GetAt(i) + fTof2InvV_p1->GetAt(i) * TofRaw);
                        Beta = Velocity / 0.299792458;
                        Gamma = 1. / TMath::Sqrt(1. - Beta * Beta);
                        AoQraw = fBrho0_S2toCC->GetAt(i) / (3.10716 * Beta * Gamma);
                        fh1_Tcal1Hit_TofRaw[i]->Fill(TofRaw);
                        fh1_Tcal1Hit_AoQraw[i]->Fill(AoQraw);
                    } // end of if mult1 in FrsSci and First Los Detector

                    for (UShort_t hitr = 0; hitr < multFrsSciTcal[i * fFrsSciNbPmts]; hitr++)
                    {
                        for (UShort_t hitl = 0; hitl < multFrsSciTcal[i * fFrsSciNbPmts + i]; hitl++)
                        {
                            StartTraw_atTcal =
                                0.5 * (FrsSciTraw[i * fFrsSciNbPmts][hitr] + FrsSciTraw[i * fFrsSciNbPmts + 1][hitl]) -
                                FrsSciTraw[i * fFrsSciNbPmts + 2][0];
                            TofRaw = fHeader->GetTStartMaster() - StartTraw_atTcal;
                            PosRaw = FrsSciTraw[i * fFrsSciNbPmts][hitr] - FrsSciTraw[i * fFrsSciNbPmts + 1][hitl];
                            if (ftof_range_min < TofRaw && TofRaw < ftof_range_max && fpos_range_min < PosRaw &&
                                PosRaw < fpos_range_max)
                            {
                                indexr = hitr;
                                indexl = hitl;
                                multTofRaw[i]++;
                            }
                        }
                    }
                    fh1_multTofRaw[i]->Fill(multTofRaw[i]);
                    if (Zcharge[i] > 3.5)
                        fh1_multTofRaw_Zgt3[i]->Fill(multTofRaw[i]);
                    if (indexr >= 0 && indexl >= 0 && multTofRaw[i] == 1)
                    {
                        StartTraw_atTcal =
                            0.5 * (FrsSciTraw[i * fFrsSciNbPmts][indexr] + FrsSciTraw[i * fFrsSciNbPmts + 1][indexl]) -
                            FrsSciTraw[i * fFrsSciNbPmts + 2][0];
                        TofRaw = fHeader->GetTStartMaster() - StartTraw_atTcal;
                        PosRaw = FrsSciTraw[i * fFrsSciNbPmts][indexr] - FrsSciTraw[i * fFrsSciNbPmts + 1][indexl];
                        PosCal = fPosCal_Gain->GetAt(i) * PosRaw + fPosCal_Offset->GetAt(i);
                        Velocity = 1. / (fTof2InvV_p0->GetAt(i) + fTof2InvV_p1->GetAt(i) * TofRaw);
                        Beta = Velocity / 0.299792458;
                        Gamma = 1. / TMath::Sqrt(1. - Beta * Beta);
                        Brho = fBrho0_S2toCC->GetAt(i) * (1 + PosCal / fDispersionS2->GetAt(i));
                        AoQcal = Brho / (3.10716 * Beta * Gamma);
                        fh1_TofRaw[i]->Fill(TofRaw);
                        fh1_AoQcal[i]->Fill(AoQcal);
                        fh2_AoQcal_vs_PosS2[i]->Fill(PosCal_mm[i], AoQcal);
                        if (Zcharge[i] > 0)
                        {
                            {
                                fh2_Z_vs_AoQcal[i]->Fill(AoQcal, Zcharge[i]);
                            }
                        }
                    }
                    else
                    {
                        AoQcal = 4;
                    }
                } // end of loop over the FrsSci detectors
            }     // end of good data at LOS
            fNEvents++;
        } // end of if On Spill TPat
    }     // end of if (fFrsSci_Tcal)
}

// -----   Public method Finish   -----------------------------------------------
void R3BOnlineSpectraFrsSciVsLos::FinishEvent()
{
    if (fFrsSci_Tcal)
    {
        fFrsSci_Tcal->Clear();
    }
    if (fFrsSci_PosCal)
    {
        fFrsSci_PosCal->Clear();
    }
    if (fLos_Tcal)
    {
        fLos_Tcal->Clear();
    }
    if (fLos_Hit)
    {
        fLos_Hit->Clear();
    }
}

void R3BOnlineSpectraFrsSciVsLos::FinishTask()
{

    if (fFrsSci_Tcal && fHeader)
    {
        for (UShort_t i = 0; i < fFrsSciNbDets; i++)
        {
            fh1_Tcal1Hit_TofRaw[i]->Write();
            fh1_TofRaw[i]->Write();
            fh1_multTofRaw[i]->Write();
            fh1_multTofRaw_Zgt3[i]->Write();
            fh1_Tcal1Hit_AoQraw[i]->Write();
            if (fFrsSci_PosCal)
            {
                fh1_AoQcal[i]->Write();
                fh2_AoQcal_vs_PosS2[i]->Write();
            }
            if (fLos_Hit)
            {
                fh2_Z_vs_AoQcal[i]->Write();
            }
        }
        for (UShort_t i = 0; i < fLosNbDets; i++)
        {
            if (fLos_Tcal)
            {
                fh1_MeanToT[i]->Write();
            }
        }
    }
}
ClassImp(R3BOnlineSpectraFrsSciVsLos)
