// ------------------------------------------------------------
// -----                  R3BOnlineSpectraPdc                -----
// -----          Created July  2020 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BOnlineSpectraPdc.h"

#include "R3BPdcCalData.h"
#include "R3BPdcHitData.h"
#include "R3BPdcMappedData.h"

#include "R3BBunchedFiberCalData.h"
#include "R3BBunchedFiberHitData.h"
#include "R3BBunchedFiberMappedData.h"

#include "R3BEventHeader.h"
#include "R3BTCalEngine.h"

#include "R3BSamplerMappedData.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TGaxis.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THttpServer.h"
#include "TLegend.h"

#include "TClonesArray.h"
#include "TMath.h"
#include <TRandom3.h>
#include <TRandomGen.h>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <assert.h>
#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;

R3BOnlineSpectraPdc::R3BOnlineSpectraPdc()
    : R3BOnlineSpectraPdc("OnlineSpectra", 1)
{
}

R3BOnlineSpectraPdc::R3BOnlineSpectraPdc(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fSamp(-1)
    , fNEvents(0)
{
}

R3BOnlineSpectraPdc::~R3BOnlineSpectraPdc()
{
    //	delete fhTpat ;
    //	delete fhTrigger;
    //	delete fh_SEETRAM;
    /*
    delete fh_fi0_0_fiber;
    delete fh_fi0_1_fiber;
    delete fh_fi0_0_mult;
    delete fh_fi0_1_mult;
    delete fh_fi0_0_Tot;
    delete fh_fi0_1_Tot;
    delete fh_fi0_0_Time;
    delete fh_fi0_1_Time;
    */
    /*
    delete fMappedItems_fi1a;
    delete fHitItems_fi1a;

    delete fMappedItems_fi1b;
    delete fHitItems_fi1b;
    */
    for (int i = 0; i < N_PLANE_MAX_PDC; i++)
    {
        delete fh_Pdc_Wire[i];
        delete fh_Pdc_Tot[i];
        delete fh_Pdc_mult[i];
        delete fh_Pdc_ToF[i];
        delete fh_Pdc_xy[i];
        delete fh_Pdc_x[i];
        delete fh_Pdc_y[i];
        delete fh_Pdc_Time[i];
        delete fh_Pdc_Wire_vs_Events[i];
    }
}

InitStatus R3BOnlineSpectraPdc::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(info) << "R3BOnlineSpectraPdc::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline* run = FairRunOnline::Instance();

    run->GetHttpServer()->Register("/Tasks", this);

    // Get objects for detectors on all levels
    fMappedItems = (TClonesArray*)mgr->GetObject("PdcMapped");
    if (NULL == fMappedItems)
        LOG(fatal) << "Branch PdcMapped not found";

    fCalItems = (TClonesArray*)mgr->GetObject("PdcCal");
    if (NULL == fCalItems)
        LOG(fatal) << "Branch PdcCal not found";

    fHitItems = (TClonesArray*)mgr->GetObject("PdcHit");
    if (NULL == fHitItems)
        LOG(fatal) << "Branch PdcHit not found";

    // Get objects for detectors on all levels
    fMappedItems_fi0 = (TClonesArray*)mgr->GetObject("Fi0Mapped");
    if (NULL == fMappedItems_fi0)
        LOG(fatal) << "Branch Fi0Mapped not found";

    fCalItems_fi0 = (TClonesArray*)mgr->GetObject("Fi0Cal");
    if (NULL == fCalItems_fi0)
        LOG(fatal) << "Branch Fi0Cal not found";

    fHitItems_fi0 = (TClonesArray*)mgr->GetObject("Fi0Hit");
    if (NULL == fHitItems_fi0)
        LOG(fatal) << "Branch Fi0Hit not found";

    // take out fib1a and 1b for the moment (MH)
    /*
        // Get objects for detectors on all levels
        fMappedItems_fi1a = (TClonesArray*)mgr->GetObject("Fi1aMapped");
        if (NULL == fMappedItems_fi1a)
            LOG(fatal) << "Branch Fi1aMapped not found";

        fCalItems_fi1a = (TClonesArray*)mgr->GetObject("Fi1aCal");
        if (NULL == fCalItems_fi1a)
            LOG(fatal) << "Branch Fi1aCal not found";

        fHitItems_fi1a = (TClonesArray*)mgr->GetObject("Fi1aHit");
        if (NULL == fHitItems_fi1a)
            LOG(fatal) << "Branch Fi1aHit not found";

        // Get objects for detectors on all levels
        fMappedItems_fi1b = (TClonesArray*)mgr->GetObject("Fi1bMapped");
        if (NULL == fMappedItems_fi1b)
            LOG(fatal) << "Branch Fi1bMapped not found";

        fCalItems_fi1b = (TClonesArray*)mgr->GetObject("Fi1bCal");
        if (NULL == fCalItems_fi1b)
            LOG(fatal) << "Branch Fi1bCal not found";

        fHitItems_fi1b = (TClonesArray*)mgr->GetObject("Fi1bHit");
        if (NULL == fHitItems_fi1b)
            LOG(fatal) << "Branch Fi1bHit not found";
    */

    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------

    // Trigger and Tpat
    //  TCanvas* cTrigg = new TCanvas("Trigg", "Triggers", 50, 50, 500, 500);
    fhTpat = new TH1F("Tpat", "Tpat", 20, 0, 20);
    fhTpat->GetXaxis()->SetTitle("Tpat value");

    fhTrigger = new TH1F("Trigger", "Trigger all", 20, 0, 20);
    fhTrigger->GetXaxis()->SetTitle("Trigger value");
    /*
        cTrigg->Divide(2, 1);
        cTrigg->cd(1);
        gPad->SetLogy();
        fhTrigger->Draw();
        cTrigg->cd(2);
        gPad->SetLogy();
        fhTpat->Draw();
        cTrigg->cd(0);
    */
    //---------------------------------------------------------------------------------------------------
    // Fiber0_0 and Fiber0_1 detectors
    if (fMappedItems_fi0)
    {
        Int_t ch = 150;
        TCanvas* cFib = new TCanvas("Fib0", "Fiber0 detectors", 50, 50, 500, 500);
        cFib->Divide(4, 2);

        fh_fi0_0_fiber = new TH1F("Fi0_fibers", "Fi0 fibers", ch, 0., ch);
        fh_fi0_0_fiber->GetXaxis()->SetTitle("Fiber");
        fh_fi0_0_fiber->GetYaxis()->SetTitle("Counts");

        fh_fi0_1_fiber = new TH1F("Fi1_fibers", "Fi1 fibers", 128, 0., 128.);
        fh_fi0_1_fiber->GetXaxis()->SetTitle("Fiber");
        fh_fi0_1_fiber->GetYaxis()->SetTitle("Counts");

        fh_fi0_0_mult = new TH1F("Fi0_mult", "Fi0 mult", 140, 0., 140);
        fh_fi0_0_mult->GetXaxis()->SetTitle("mult");
        fh_fi0_0_mult->GetYaxis()->SetTitle("Counts");

        fh_fi0_1_mult = new TH1F("Fi1_mult", "Fi1 mult", 200, 0., 200.);
        fh_fi0_1_mult->GetXaxis()->SetTitle("mult");
        fh_fi0_1_mult->GetYaxis()->SetTitle("Counts");

        fh_fi0_0_Tot = new TH2F("Fi0_ToT", "Fi0 ToT", ch, 0., ch, int(500. / 0.4167), 0, 500);
        fh_fi0_0_Tot->GetXaxis()->SetTitle("Fiber");
        fh_fi0_0_Tot->GetYaxis()->SetTitle("ToT");

        fh_fi0_1_Tot = new TH2F("Fi1_ToT", "Fi1 ToT", 128, 0., 128., 500, 0, 500);
        fh_fi0_1_Tot->GetXaxis()->SetTitle("Fiber");
        fh_fi0_1_Tot->GetYaxis()->SetTitle("ToT");

        fh_fi0_0_Time = new TH2F("Fi0_Time", "Fi0 time", ch, 0., ch, int(200. / 0.4167), -1000, 1000);
        fh_fi0_0_Time->GetXaxis()->SetTitle("Fiber");
        fh_fi0_0_Time->GetYaxis()->SetTitle("time");

        fh_fi0_1_Time = new TH2F("Fi1_Time", "Fi1 time", 128, 0., 128., 1000, -500, 500);
        fh_fi0_1_Time->GetXaxis()->SetTitle("Fiber");
        fh_fi0_1_Time->GetYaxis()->SetTitle("time");

        fh_fi0_tot2_tot1 =
            new TH2F("Fi0_ToT1_vs_ToT2", "Fi0 ToT1 vs. ToT2", int(30. / 0.4167), 0, 30, int(30. / 0.4167), 0, 30);
        fh_fi0_tot2_tot1->GetXaxis()->SetTitle("ToT1");
        fh_fi0_tot2_tot1->GetYaxis()->SetTitle("ToT2");

        fh_fi0_cor = new TH2F("Fi0_cor", "Fi0 correlation plot", 128, 0, 128, 128, 0, 128);
        fh_fi0_cor->GetXaxis()->SetTitle("Fiber");
        fh_fi0_cor->GetYaxis()->SetTitle("Fiber");

        fh_fi0_eff = new TH2F("fi0_eff", "Fi0 hits for PDC x vs y", 1500, 0, 1500, 1500, 0, 1500);
        fh_fi0_eff->GetXaxis()->SetTitle("x in mm");
        fh_fi0_eff->GetYaxis()->SetTitle("y in mm");

        // with cuts
        fh_fi0_0_fiber_mc = new TH1F("Fi0_fibers_mc", "Fi0 fibers with cuts", ch, 0., ch);
        fh_fi0_0_fiber_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi0_0_fiber_mc->GetYaxis()->SetTitle("Counts");

        fh_fi0_1_fiber_mc = new TH1F("Fi1_fibers_mc", "Fi1 fibers with cuts", 128, 0., 128.);
        fh_fi0_1_fiber_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi0_1_fiber_mc->GetYaxis()->SetTitle("Counts");

        fh_fi0_0_mult_mc = new TH1F("Fi0_mult_mc", "Fi0 mult with cuts", 140, 0., 140);
        fh_fi0_0_mult_mc->GetXaxis()->SetTitle("mult");
        fh_fi0_0_mult_mc->GetYaxis()->SetTitle("Counts");

        fh_fi0_1_mult_mc = new TH1F("Fi1_mult_mc", "Fi1 mult with cuts", 140, 0., 140.);
        fh_fi0_1_mult_mc->GetXaxis()->SetTitle("mult");
        fh_fi0_1_mult_mc->GetYaxis()->SetTitle("Counts");

        fh_fi0_0_Tot_mc = new TH2F("Fi0_ToT_mc", "Fi0 ToT with cuts", ch, 0., ch, int(500. / 0.4167), 0, 500);
        fh_fi0_0_Tot_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi0_0_Tot_mc->GetYaxis()->SetTitle("ToT");

        fh_fi0_1_Tot_mc = new TH2F("Fi1_ToT_mc", "Fi1 ToT with cuts", 128, 0., 128., 500, 0, 500);
        fh_fi0_1_Tot_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi0_1_Tot_mc->GetYaxis()->SetTitle("ToT");

        fh_fi0_0_Time_mc = new TH2F("Fi0_Time_mc", "Fi0 time with cuts", ch, 0., ch, int(200. / 0.4167), -1000, 1000);
        fh_fi0_0_Time_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi0_0_Time_mc->GetYaxis()->SetTitle("time");

        fh_fi0_1_Time_mc = new TH2F("Fi1_Time_mc", "Fi1 time with cuts", 128, 0., 128., 1000, -500, 500);
        fh_fi0_1_Time_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi0_1_Time_mc->GetYaxis()->SetTitle("time");

        fh_fi0_0_pdc = new TH2F("Fi0_pdc", "Fi0 vs. PDC", 130, 0, 130, 2000, 200., 600.);
        fh_fi0_0_pdc->GetXaxis()->SetTitle("Fiber0 position ");
        fh_fi0_0_pdc->GetYaxis()->SetTitle("PDC position ");

        fh_fi0_1_pdc = new TH2F("Fi1_pdc", "Fi1 vs. PDC", 130, 0., 130., 5000, 200, 1200);
        fh_fi0_1_pdc->GetXaxis()->SetTitle("Fiber0 position ");
        fh_fi0_1_pdc->GetYaxis()->SetTitle("PDC position in mm");

        fh_fi0_pdc_eloss = new TH2F("Fi0_pdc_eloss", "Fi0 vs. PDC eloss", 200, 0, 100, 3000, 0., 300.);
        fh_fi0_pdc_eloss->GetXaxis()->SetTitle("Fiber0 tot in ns");
        fh_fi0_pdc_eloss->GetYaxis()->SetTitle("PDC tot in ns");

        fh_fi0_pdc_time = new TH2F("Fi0_pdc_time", "Fi0 vs. PDC time", 1600, -800., 800., 1600, -800, 800);
        fh_fi0_pdc_time->GetXaxis()->SetTitle("Fiber0 time in ns");
        fh_fi0_pdc_time->GetYaxis()->SetTitle("PDC time in ns");

        cFib->cd(1);
        fh_fi0_0_fiber->Draw();
        cFib->cd(2);
        fh_fi0_0_mult->Draw();
        cFib->cd(3);
        fh_fi0_0_Tot->Draw("colz");
        cFib->cd(4);
        fh_fi0_0_Time->Draw("colz");
        cFib->cd(5);
        fh_fi0_0_fiber_mc->Draw();
        cFib->cd(6);
        fh_fi0_0_mult_mc->Draw();
        cFib->cd(7);
        fh_fi0_0_Tot_mc->Draw("colz");
        cFib->cd(8);
        fh_fi0_0_Time_mc->Draw("colz");
        /*
                cFib->cd(9);
                fh_fi0_1_fiber->Draw();
                cFib->cd(10);
                fh_fi0_1_mult->Draw();
                cFib->cd(11);
                fh_fi0_1_Tot->Draw("colz");
                cFib->cd(12);
                fh_fi0_1_Time->Draw("colz");
                cFib->cd(13);
                fh_fi0_1_fiber_mc->Draw();
                cFib->cd(14);
                fh_fi0_1_mult_mc->Draw();
                cFib->cd(15);
                fh_fi0_1_Tot_mc->Draw("colz");
                cFib->cd(16);
                fh_fi0_1_Time_mc->Draw("colz");
        */
        cFib->cd(0);
        run->AddObject(cFib);
        run->GetHttpServer()->RegisterCommand("Reset_Fi0", Form("/Tasks/%s/->Reset_Fi0_Histo()", GetName()));
    }

    //---------------------------------------------------------------------------------------------------
    // Fiber1a and Fiber1b detectors
    if (fMappedItems_fi1a)
    {
        Int_t ch = 260;

        fh_fi1a_fiber = new TH1F("Fi1a_fibers", "Fi1a fibers", ch, 0., ch);
        fh_fi1a_fiber->GetXaxis()->SetTitle("Fiber");
        fh_fi1a_fiber->GetYaxis()->SetTitle("Counts");

        fh_fi1b_fiber = new TH1F("Fi1b_fibers", "Fi1b fibers", ch, 0., ch);
        fh_fi1b_fiber->GetXaxis()->SetTitle("Fiber");
        fh_fi1b_fiber->GetYaxis()->SetTitle("Counts");

        fh_fi1a_mult = new TH1F("Fi1a_mult", "Fi1a mult", ch, 0., ch);
        fh_fi1a_mult->GetXaxis()->SetTitle("mult");
        fh_fi1a_mult->GetYaxis()->SetTitle("Counts");

        fh_fi1b_mult = new TH1F("Fi1b_mult", "Fi1b mult", ch, 0., ch);
        fh_fi1b_mult->GetXaxis()->SetTitle("mult");
        fh_fi1b_mult->GetYaxis()->SetTitle("Counts");

        fh_fi1a_Tot = new TH2F("Fi1a_ToT", "Fi1a ToT", ch, 0., ch, 500, 0, 500);
        fh_fi1a_Tot->GetXaxis()->SetTitle("Fiber");
        fh_fi1a_Tot->GetYaxis()->SetTitle("ToT");

        fh_fi1b_Tot = new TH2F("Fi1b_ToT", "Fi1b ToT", ch, 0., ch, 500, 0, 500);
        fh_fi1b_Tot->GetXaxis()->SetTitle("Fiber");
        fh_fi1b_Tot->GetYaxis()->SetTitle("ToT");

        fh_fi1a_Time = new TH2F("Fi1a_Time", "Fi1a time", ch, 0., ch, 2500, -1500, 1000);
        fh_fi1a_Time->GetXaxis()->SetTitle("Fiber");
        fh_fi1a_Time->GetYaxis()->SetTitle("time");

        fh_fi1b_Time = new TH2F("Fi1b_Time", "Fi1b time", ch, 0., ch, 2500, -1500, 1000);
        fh_fi1b_Time->GetXaxis()->SetTitle("Fiber");
        fh_fi1b_Time->GetYaxis()->SetTitle("time");

        // with cuts
        fh_fi1a_fiber_mc = new TH1F("Fi1a_fibers_mc", "Fi1a fibers with cuts", ch, 0., ch);
        fh_fi1a_fiber_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi1a_fiber_mc->GetYaxis()->SetTitle("Counts");

        fh_fi1b_fiber_mc = new TH1F("Fi1b_fibers_mc", "Fi1b fibers with cuts", ch, 0., ch);
        fh_fi1b_fiber_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi1b_fiber_mc->GetYaxis()->SetTitle("Counts");

        fh_fi1a_mult_mc = new TH1F("Fi1a_mult_mc", "Fi1a mult with cuts", ch, 0., ch);
        fh_fi1a_mult_mc->GetXaxis()->SetTitle("mult");
        fh_fi1a_mult_mc->GetYaxis()->SetTitle("Counts");

        fh_fi1b_mult_mc = new TH1F("Fi1b_mult_mc", "Fi1b mult with cuts", ch, 0., ch);
        fh_fi1b_mult_mc->GetXaxis()->SetTitle("mult");
        fh_fi1b_mult_mc->GetYaxis()->SetTitle("Counts");

        fh_fi1a_Tot_mc = new TH2F("Fi1a_ToT_mc", "Fi1a ToT with cuts", ch, 0., ch, 100, 0, 100);
        fh_fi1a_Tot_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi1a_Tot_mc->GetYaxis()->SetTitle("ToT");

        fh_fi1b_Tot_mc = new TH2F("Fi1b_ToT_mc", "Fi1b ToT with cuts", ch, 0., ch, 100, 0, 100);
        fh_fi1b_Tot_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi1b_Tot_mc->GetYaxis()->SetTitle("ToT");

        fh_fi1a_Time_mc = new TH2F("Fi1a_Time_mc", "Fi1a time with cuts", ch, 0., ch, 1000, -500, 500);
        fh_fi1a_Time_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi1a_Time_mc->GetYaxis()->SetTitle("time");

        fh_fi1b_Time_mc = new TH2F("Fi1b_Time_mc", "Fi1b time with cuts", ch, 0., ch, 1000, -500, 500);
        fh_fi1b_Time_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi1b_Time_mc->GetYaxis()->SetTitle("time");

        fh_fi1a_pdc = new TH2F("Fi1a_pdc", "Fi1a vs. PDC", 256, 0, 256, 2000, 400., 800.); // 286, -3.003, 3.003
        fh_fi1a_pdc->GetXaxis()->SetTitle("Fiber1a x-position in cm");
        fh_fi1a_pdc->GetYaxis()->SetTitle("PDC position in cm");

        fh_fi1b_pdc = new TH2F("Fi1b_pdc", "Fi1b vs. PDC", 256, 0, 256, 2000, 200., 600.);
        fh_fi1b_pdc->GetXaxis()->SetTitle("Fiber1b y-position in cm");
        fh_fi1b_pdc->GetYaxis()->SetTitle("PDC position in cm");

        fh_fi1a_pdc_time = new TH2F("Fi1a_pdc_time", "Fi1a vs. PDC time", 1600, -800., 800., 1600, -800, 800);
        fh_fi1a_pdc_time->GetXaxis()->SetTitle("Fiber1a time in ns");
        fh_fi1a_pdc_time->GetYaxis()->SetTitle("PDC time in ns");

        fh_fi1a_pdc_eloss = new TH2F("Fi1a_pdc_eloss", "Fi1a vs. PDC eloss", 200, 0, 100, 3000, 0., 300.);
        fh_fi1a_pdc_eloss->GetXaxis()->SetTitle("Fiber1a tot in ns");
        fh_fi1a_pdc_eloss->GetYaxis()->SetTitle("PDC tot in ns");

        fh_fi1b_pdc_eloss = new TH2F("Fi1b_pdc_eloss", "Fi1b vs. PDC eloss", 200, 0, 100, 3000, 0., 300.);
        fh_fi1b_pdc_eloss->GetXaxis()->SetTitle("Fiber1b tot in ns");
        fh_fi1b_pdc_eloss->GetYaxis()->SetTitle("PDC tot in ns");

        fh_fi1b_pdc_time = new TH2F("Fi1b_pdc_time", "Fi1b vs. PDC time", 1600, -800., 800., 1600, -800, 800);
        fh_fi1b_pdc_time->GetXaxis()->SetTitle("Fiber1b time in ns");
        fh_fi1b_pdc_time->GetYaxis()->SetTitle("PDC time in ns");

        fh_fi1_xy = new TH2F("Fi1xy", "Fi1xy", 286, -3.003, 3.003, 286, -3.003, 3.003);
        fh_fi1_xy->GetXaxis()->SetTitle("Fiber1a x-position in cm");
        fh_fi1_xy->GetYaxis()->SetTitle("Fiber1b y-position in cm");

        fh_fi1_time = new TH2F("Fi1time", "Fi1time", 1600, -800., 800., 1600, -800., 800.);
        fh_fi1_time->GetXaxis()->SetTitle("Fiber1a time in ns");
        fh_fi1_time->GetYaxis()->SetTitle("Fiber1b time in ns");

        fh_fi1_tot = new TH2F("Fi1tot", "Fi1tot", 200, 0, 100., 200, 0., 100.);
        fh_fi1_tot->GetXaxis()->SetTitle("Fiber1a tot in ns");
        fh_fi1_tot->GetYaxis()->SetTitle("Fiber1b tot in ns");

        TCanvas* cFib1_correl = new TCanvas("Fib1_pdc", "Fiber1_pdc detectors", 50, 50, 800, 800);
        cFib1_correl->Divide(3, 4);

        cFib1_correl->cd(1);
        gPad->SetLogz();
        fh_fi0_0_pdc->Draw("colz");
        cFib1_correl->cd(2);
        gPad->SetLogz();
        fh_fi0_pdc_time->Draw("colz");
        cFib1_correl->cd(3);
        gPad->SetLogz();
        fh_fi0_pdc_eloss->Draw("colz");

        cFib1_correl->cd(4);
        gPad->SetLogz();
        fh_fi1a_pdc->Draw("colz");
        cFib1_correl->cd(5);
        gPad->SetLogz();
        fh_fi1a_pdc_time->Draw("colz");
        cFib1_correl->cd(6);
        gPad->SetLogz();
        fh_fi1a_pdc_eloss->Draw("colz");

        cFib1_correl->cd(7);
        gPad->SetLogz();
        fh_fi1b_pdc->Draw("colz");
        cFib1_correl->cd(8);
        gPad->SetLogz();
        fh_fi1b_pdc_time->Draw("colz");
        cFib1_correl->cd(9);
        gPad->SetLogz();
        fh_fi1b_pdc_eloss->Draw("colz");

        cFib1_correl->cd(10);
        gPad->SetLogz();
        fh_fi1_xy->Draw("colz");
        cFib1_correl->cd(11);
        gPad->SetLogz();
        fh_fi1_time->Draw("colz");
        cFib1_correl->cd(12);
        gPad->SetLogz();
        fh_fi1_tot->Draw("colz");

        cFib1_correl->cd(0);
        run->AddObject(cFib1_correl);

        TCanvas* cFib1 = new TCanvas("Fib1", "Fiber1 detectors", 50, 50, 1200, 900);
        cFib1->Divide(4, 4);

        cFib1->cd(1);
        gPad->SetLogy();
        fh_fi1a_fiber->Draw();
        cFib1->cd(2);
        gPad->SetLogy();
        fh_fi1a_mult->Draw();
        cFib1->cd(3);
        gPad->SetLogz();
        fh_fi1a_Tot->Draw("colz");
        cFib1->cd(4);
        gPad->SetLogz();
        fh_fi1a_Time->Draw("colz");

        cFib1->cd(5);
        gPad->SetLogy();
        fh_fi1a_fiber_mc->Draw();
        cFib1->cd(6);
        gPad->SetLogy();
        fh_fi1a_mult_mc->Draw();
        cFib1->cd(7);
        gPad->SetLogz();
        fh_fi1a_Tot_mc->Draw("colz");
        cFib1->cd(8);
        gPad->SetLogz();
        fh_fi1a_Time_mc->Draw("colz");

        cFib1->cd(9);
        gPad->SetLogy();
        fh_fi1b_fiber->Draw();
        cFib1->cd(10);
        gPad->SetLogy();
        fh_fi1b_mult->Draw();
        cFib1->cd(11);
        gPad->SetLogz();
        fh_fi1b_Tot->Draw("colz");
        cFib1->cd(12);
        gPad->SetLogz();
        fh_fi1b_Time->Draw("colz");

        cFib1->cd(13);
        gPad->SetLogy();
        fh_fi1b_fiber_mc->Draw();
        cFib1->cd(14);
        gPad->SetLogy();
        fh_fi1b_mult_mc->Draw();
        cFib1->cd(15);
        gPad->SetLogz();
        fh_fi1b_Tot_mc->Draw("colz");
        cFib1->cd(16);
        gPad->SetLogz();
        fh_fi1b_Time_mc->Draw("colz");

        cFib1->cd(0);
        run->AddObject(cFib1);
        run->GetHttpServer()->RegisterCommand("Reset_Fi1", Form("/Tasks/%s/->Reset_Fi1_Histo()", GetName()));
    }

    //---------------------------------------------------------------------------------------------------
    // PDC detector

    if (fMappedItems)
    {
        TCanvas* cPdc_planes = new TCanvas("PDC_planes", "PDC planes", 50, 50, 800, 1200);
        cPdc_planes->Divide(6, 4);

        TCanvas* cPdc = new TCanvas("PDC", "PDC", 50, 50, 500, 500);
        cPdc->Divide(1, 4);

        fh_pdc_ebene4 = new TH2F("pdc_wire_vs_y_ebene4", "PDC wire vs y eben 4", 150, 0, 150, 1000, 0, 1000);
        fh_pdc_ebene4->GetXaxis()->SetTitle("wire number");
        fh_pdc_ebene4->GetYaxis()->SetTitle("y");

        fh_Pdc_tvst[0] = new TH2F("pdc_t1_vs_t2", "PDC t1 vs t2", 1000, -1000, 1000, 1000, -1000, 1000);
        fh_Pdc_tvst[0]->GetXaxis()->SetTitle("time plane 1 / ns");
        fh_Pdc_tvst[0]->GetYaxis()->SetTitle("time plane 2 / ns");

        fh_Pdc_tvst[1] = new TH2F("pdc_t3_vs_t4", "PDC t3 vs t4", 1000, -1000, 1000, 1000, -1000, 1000);
        fh_Pdc_tvst[1]->GetXaxis()->SetTitle("time plane 3 / ns");
        fh_Pdc_tvst[1]->GetYaxis()->SetTitle("time plane 4 / ns");

        fh_Pdc_evse[0] = new TH2F("pdc_e1_vs_e2", "PDC e1 vs e2", 3000, 0, 300, 3000, 0, 300);
        fh_Pdc_evse[0]->GetXaxis()->SetTitle("eloss plane 1 / ns");
        fh_Pdc_evse[0]->GetYaxis()->SetTitle("eloss plane 2 / ns");

        fh_Pdc_evse[1] = new TH2F("pdc_e2_vs_e4", "PDC e3 vs e4", 3000, 0, 300, 3000, 0, 300);
        fh_Pdc_evse[1]->GetXaxis()->SetTitle("eloss plane 3 / ns");
        fh_Pdc_evse[1]->GetYaxis()->SetTitle("eloss plane 4 / ns");

        for (Int_t j = 0; j < N_PLANE_MAX_PDC; j++)
        {
            char strName1[255];
            sprintf(strName1, "pdc_wire_plane_%d", j + 1);
            char strName2[255];
            sprintf(strName2, "PDC wires plane %d", j + 1);
            fh_Pdc_Wire[j] = new TH1F(strName1, strName2, 150, 0., 150.);
            fh_Pdc_Wire[j]->GetXaxis()->SetTitle("Wire");
            fh_Pdc_Wire[j]->GetYaxis()->SetTitle("Counts");

            char strName11[255];
            sprintf(strName11, "pdc_x_plane_%d", j + 1);
            char strName12[255];
            sprintf(strName12, "PDC x plane %d", j + 1);
            fh_Pdc_x[j] = new TH1F(strName11, strName12, 1500, 0., 1500.);
            fh_Pdc_x[j]->GetXaxis()->SetTitle("x in mm");
            fh_Pdc_x[j]->GetYaxis()->SetTitle("Counts");

            char strName13[255];
            sprintf(strName13, "pdc_y_plane_%d", j + 1);
            char strName14[255];
            sprintf(strName14, "PDC y plane %d", j + 1);
            fh_Pdc_y[j] = new TH1F(strName13, strName14, 1500, 0., 1500.);
            fh_Pdc_y[j]->GetXaxis()->SetTitle("y in mm");
            fh_Pdc_y[j]->GetYaxis()->SetTitle("Counts");

            char strName3[255];
            sprintf(strName3, "pdc_ToT_plane_%d", j + 1);
            char strName4[255];
            sprintf(strName4, "PDC ToT plane %d", j + 1);
            fh_Pdc_Tot[j] = new TH2F(strName3, strName4, 150, 0, 150, 3000, 0., 300.);
            fh_Pdc_Tot[j]->GetXaxis()->SetTitle("Wire");
            fh_Pdc_Tot[j]->GetYaxis()->SetTitle("ToT / ns");

            char strName23[255];
            sprintf(strName23, "pdc_mult_plane_%d", j + 1);
            char strName24[255];
            sprintf(strName24, "PDC multiplicity plane %d", j + 1);
            fh_Pdc_mult[j] = new TH2F(strName23, strName24, 150, 0, 150, 100, 0., 100.);
            fh_Pdc_mult[j]->GetXaxis()->SetTitle("Wire");
            fh_Pdc_mult[j]->GetYaxis()->SetTitle("multiplicity");

            char strName5[255];
            sprintf(strName5, "ped_ToF_plane_%d", j + 1);
            char strName6[255];
            sprintf(strName6, "PDC ToF plane %d", j + 1);
            fh_Pdc_ToF[j] = new TH2F(strName5, strName6, 150, 0, 150, 1000, 0, 100);
            fh_Pdc_ToF[j]->GetXaxis()->SetTitle("Wire");
            fh_Pdc_ToF[j]->GetYaxis()->SetTitle("ToF / ns");

            char strName7[255];
            sprintf(strName7, "pdc_time_plane_%d", j + 1);
            char strName8[255];
            sprintf(strName8, "PDC Time-trigger time plane %d", j + 1);
            fh_Pdc_Time[j] = new TH2F(strName7, strName8, 150, 0, 150, 1000, -1000, 1000);
            fh_Pdc_Time[j]->GetXaxis()->SetTitle("Wire");
            fh_Pdc_Time[j]->GetYaxis()->SetTitle("Time / ns");

            char strName9[255];
            sprintf(strName9, "pdc_xy_plane_%d", j + 1);
            char strName10[255];
            sprintf(strName10, "PDC xy plane %d", j + 1);
            fh_Pdc_xy[j] = new TH2F(strName9, strName10, 1500, 0, 1500, 1000, 0, 1000);
            fh_Pdc_xy[j]->GetXaxis()->SetTitle("x in cm");
            fh_Pdc_xy[j]->GetYaxis()->SetTitle("y in cm");

            char strName15[255];
            sprintf(strName15, "pdc_wire_vs_Event_plane_%d", j + 1);
            char strName16[255];
            sprintf(strName16, "PDC wire vs. events plane %d", j + 1);
            fh_Pdc_Wire_vs_Events[j] = new TH2F(strName15, strName16, 1000, 0, 10000, 150, 0, 150);
            fh_Pdc_Wire_vs_Events[j]->GetXaxis()->SetTitle("Event number");
            fh_Pdc_Wire_vs_Events[j]->GetYaxis()->SetTitle("Wire number");
        }

        fh_Pdc_xvsx = new TH2F("PDC_x_vs_x", "PDC x vs x", 1500, 0, 1500, 1500, 0, 1500);
        fh_Pdc_xvsx->GetXaxis()->SetTitle("x1 in mm");
        fh_Pdc_xvsx->GetYaxis()->SetTitle("x3 in mm");

        fh_Pdc_xvsx_fi0 = new TH2F("PDC_x_vs_x_fi0", "PDC x vs x", 1500, 0, 1500, 1500, 0, 1500);
        fh_Pdc_xvsx_fi0->GetXaxis()->SetTitle("x1 in mm");
        fh_Pdc_xvsx_fi0->GetYaxis()->SetTitle("x3 in mm");

        fh_Pdc_yvsy = new TH2F("PDC_y_vs_y", "PDC y vs y", 1000, 0, 1000, 1000, 0, 1000);
        fh_Pdc_yvsy->GetXaxis()->SetTitle("y2 in mm");
        fh_Pdc_yvsy->GetYaxis()->SetTitle("y4 in mm");

        fh_Pdc_yvsy_fi0 = new TH2F("PDC_y_vs_y_fi0", "PDC y vs y", 1000, 0, 1000, 1000, 0, 1000);
        fh_Pdc_yvsy_fi0->GetXaxis()->SetTitle("y2 in mm");
        fh_Pdc_yvsy_fi0->GetYaxis()->SetTitle("y4 in mm");

        fh_Pdc_xvsy_fi0 = new TH2F("PDC_x_vs_y_fi0", "PDC x vs y", 1500, 0, 1500, 1500, 0, 1500);
        fh_Pdc_xvsy_fi0->GetXaxis()->SetTitle("x in mm");
        fh_Pdc_xvsy_fi0->GetYaxis()->SetTitle("y in mm");

        fh_Pdc_wvsw = new TH2F("PDC_w_vs_w", "PDC w vs w", 130, 0, 130, 130, 0, 130);
        fh_Pdc_wvsw->GetXaxis()->SetTitle("wire p2 ");
        fh_Pdc_wvsw->GetYaxis()->SetTitle("wire p4");

        /*
                for (Int_t j = 0; j < 144; j++)
                {

                    char strName17[255];
                    sprintf(strName17, "pdc_Tot_vs_Time_wire_%d", j + 1);
                    char strName18[255];
                    sprintf(strName18, "PDC ToT vs. Time wire %d", j + 1);
                    fh_Pdc_ToT_vs_Time[j] = new TH2F(strName17, strName18, 2000, -1000, 1000, 300, 0, 300);
                    fh_Pdc_ToT_vs_Time[j]->GetXaxis()->SetTitle("Time in ns");
                    fh_Pdc_ToT_vs_Time[j]->GetYaxis()->SetTitle("ToT in ns");

                    char strName19[255];
                    sprintf(strName19, "pdc_Tot_vs_Hit_wire_%d", j + 1);
                    char strName20[255];
                    sprintf(strName20, "PDC ToT vs. Hit number for wire %d", j + 1);
                    fh_Pdc_ToT_vs_Hit[j] = new TH2F(strName19, strName20, 20, 0, 20, 300, 0, 300);
                    fh_Pdc_ToT_vs_Hit[j]->GetXaxis()->SetTitle("Time in ns");
                    fh_Pdc_ToT_vs_Hit[j]->GetYaxis()->SetTitle("ToT in ns");
                }
        */
        for (Int_t j = 0; j < N_PLANE_MAX_PDC; j++)
        {
            cPdc_planes->cd(j * 6 + 1);
            fh_Pdc_Wire[j]->Draw();
            cPdc_planes->cd(j * 6 + 2);
            fh_Pdc_mult[j]->Draw("colz");
            cPdc_planes->cd(j * 6 + 3);
            fh_Pdc_Tot[j]->Draw("colz");
            cPdc_planes->cd(j * 6 + 4);
            // gPad->SetLogz();
            fh_Pdc_Time[j]->Draw("colz");
            cPdc_planes->cd(j * 6 + 5);
            if (j == 0 || j == 2)
            {
                fh_Pdc_x[j]->Draw();
            }
            else
            {
                fh_Pdc_y[j]->Draw();
                cPdc_planes->cd(j * 6 + 6);
                fh_Pdc_xy[j]->Draw("colz");
            }
            if (j == 0)
            {
                cPdc_planes->cd(j * 6 + 6);
                fh_Pdc_xvsx->Draw("colz");
            }
            if (j == 2)
            {
                cPdc_planes->cd(j * 6 + 6);
                fh_Pdc_yvsy->Draw("colz");
            }

            cPdc->cd(j + 1);
            fh_Pdc_Wire_vs_Events[j]->Draw("colz");
        }

        cPdc_planes->cd(0);
        run->AddObject(cPdc_planes);
        cPdc->cd(0);
        run->AddObject(cPdc);

        run->GetHttpServer()->RegisterCommand("Reset_PDC", Form("/Tasks/%s/->Reset_PDC_Histo()", GetName()));
    }

    return kSUCCESS;
}

void R3BOnlineSpectraPdc::Reset_PDC_Histo()
{
    for (int i = 0; i < N_PLANE_MAX_PDC; i++)
    {
        fh_Pdc_Wire[i]->Reset();
        fh_Pdc_Time[i]->Reset();
        fh_Pdc_ToF[i]->Reset();
        fh_Pdc_Tot[i]->Reset();
        fh_Pdc_xy[i]->Reset();
        fh_Pdc_x[i]->Reset();
        fh_Pdc_y[i]->Reset();
        fh_Pdc_Wire_vs_Events[i]->Reset();
    }
}

void R3BOnlineSpectraPdc::Reset_Fi0_Histo()
{
    fh_fi0_0_fiber->Reset();
    fh_fi0_1_fiber->Reset();
    fh_fi0_0_mult->Reset();
    fh_fi0_1_mult->Reset();
    fh_fi0_0_Tot->Reset();
    fh_fi0_1_Tot->Reset();
    fh_fi0_0_Time->Reset();
    fh_fi0_1_Time->Reset();
}

void R3BOnlineSpectraPdc::Reset_Fi1_Histo()
{
    fh_fi1a_fiber->Reset();
    fh_fi1b_fiber->Reset();
    fh_fi1a_mult->Reset();
    fh_fi1b_mult->Reset();
    fh_fi1a_Tot->Reset();
    fh_fi1b_Tot->Reset();
    fh_fi1a_Time->Reset();
    fh_fi1b_Time->Reset();
}

void R3BOnlineSpectraPdc::Exec(Option_t* option)
{
    fNEvents++;
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        // FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
        LOG(error) << "FairRootManager not found";
        return;
    }
    /*
        if (fMappedItems)
        {
            fhTrigger->Fill(header->GetTrigger());
        }

        //   check for requested trigger (Todo: should be done globablly / somewhere else)
        if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
            return;

        if (fMappedItems)
        {
            Int_t tpatbin;
            for (int i = 0; i < 16; i++)
            {
                tpatbin = (header->GetTpat() & (1 << i));
                if (tpatbin != 0)
                    fhTpat->Fill(i + 1);
            }
        }

        // fTpat = 1-16; fTpat_bit = 0-15
        Int_t fTpat_bit = fTpat - 1;
        Int_t itpat;
        Int_t tpatvalue;
        if (fTpat_bit >= 0)
        {
            itpat = header->GetTpat();
            tpatvalue = (itpat & (1 << fTpat_bit)) >> fTpat_bit;
            if (tpatvalue == 0)
                return;
        }
    */

    //----------------------------------------------------------------------
    // PDC
    //----------------------------------------------------------------------

    // cout << endl;
    // cout << "new Event" << endl;
    if (fMappedItems)
    {
        Int_t mult[5][145] = { 0 };

        auto det = fMappedItems;
        Int_t nMapped = det->GetEntriesFast();
        for (Int_t imapped = 0; imapped < nMapped; imapped++)
        {
            auto mapped = (R3BPdcMappedData const*)det->At(imapped);
            if (!mapped)
                continue; // should not happen

            Int_t iPlane = mapped->GetPlaneId(); // 1..n
            Int_t iWire = mapped->GetWireId();   // 1..n
            Int_t iEdge = mapped->GetEdgeId();
            // cout << "Plane: " << iPlane << " Wire: " << iWire << " Edge: " << iEdge << endl;
            Int_t wire = iWire;

            // cout << "Plane: " << iPlane << " wire: " << iWire << endl;
            mult[iPlane - 1][iWire - 1]++;
            if (iPlane <= N_PLANE_MAX_PDC)
            {
                fh_Pdc_Wire[iPlane - 1]->Fill(wire);
                fh_Pdc_Wire_vs_Events[iPlane - 1]->Fill(fNEvents, wire);
            }

            auto det1 = fMappedItems;
            Int_t nMapped1 = det1->GetEntriesFast();
            for (Int_t imapped1 = 0; imapped1 < nMapped1; imapped1++)
            {
                auto mapped1 = (R3BPdcMappedData const*)det1->At(imapped1);
                if (!mapped1)
                    continue; // should not happen

                Int_t iPlane1 = mapped1->GetPlaneId(); // 1..n
                Int_t iWire1 = mapped1->GetWireId();   // 1..n
                Int_t iEdge1 = mapped1->GetEdgeId();
                // cout << "Plane1: " << iPlane1 << " Wire1: " << iWire1 << " Edge1: " << iEdge1 << endl;

                if (iPlane == 2 && iPlane1 == 4)
                {
                    fh_Pdc_wvsw->Fill(iWire, iWire1);
                }
            }
        }

        for (Int_t p = 0; p < N_PLANE_MAX_PDC; p++)
        {
            for (Int_t w = 0; w < 144; w++)
            {
                // fh_Pdc_mult[p]->Fill(w, mult[p][w]);

                if (mult[p][w] > 0)
                {
                    fh_Pdc_mult[p]->Fill(w + 1, mult[p][w] / 2, 1); // divided by 2 for leading and trailing
                }
            }
        }
    }

    if (fCalItems)
    {
        // cout << "CalItems" << endl;
        auto det = fCalItems;
        Int_t nCals = det->GetEntriesFast();

        for (Int_t ical = 0; ical < nCals; ical++)
        {
            auto cal = (R3BPdcCalData const*)det->At(ical);
            if (!cal)
                continue; // should not happen

            Int_t plane = cal->GetPlaneId();
            Double_t wire = cal->GetWireId();
            Double_t edge = cal->GetEdgeId();
            Double_t t = cal->GetTime_ns();

            // cout << "Plane: " << plane << " Wire: " << wire << " edge: " << edge << endl;
            // fh_Pdc_Time[plane-1]->Fill(wire,t);
        }
    }

    Double_t yPdc2 = -1000;
    Double_t xPdc1 = -1000;
    Double_t yPdc4test = -1000;
    Double_t xPdc3test = -1000;
    Double_t yPdc4 = -1000;
    Double_t xPdc3 = -1000;
    Double_t t0 = -10000.;
    Double_t tPair[4];
    Double_t x[4], y[4], eloss[4], t[4];
    Int_t ipl[4];
    Int_t nmlt = 10;
    Double_t ppl[4][nmlt], tpl[4][nmlt], epl[4][nmlt];
    for (Int_t ip = 0; ip < 4; ip++)
    {
        for (Int_t ic = 0; ic < nmlt; ic++)
        {
            ppl[ip][ic] = -1000;
            tpl[ip][ic] = -1000;
            epl[ip][ic] = -1000;
        }
        ipl[ip] = -1;
    }

    Int_t mult_pdc4 = 0;
    Int_t mult_pdc3 = 0;

    if (fHitItems)
    {
        Int_t mult[5][145] = { 0 };
        auto det = fHitItems;
        Int_t nHits = det->GetEntriesFast();
        // cout << "HitItems: " << nHits << endl;

        if (nHits == 4)
        {
            Int_t plane = 0;
            Int_t wire = 0;
            for (Int_t i = 0; i < 4; i++)
            {
                x[i] = -10000.;
                y[i] = -10000;
                eloss[i] = 0.;
                t[i] = -10000.;
            }

            Double_t x_prev = -10.;
            Double_t y_prev = -10.;
            Double_t xPair[4];
            Double_t yPair[4];
            Double_t TotMax[4];
            for (Int_t i = 0; i < 4; i++)
            {
                xPair[i] = -10.;
                yPair[i] = -10.;
                tPair[i] = -10000.;
            }

            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                auto hit = (R3BPdcHitData const*)det->At(ihit);
                if (!hit)
                    continue; // should not happen
                if (t0 == -10000.)
                {
                    t0 = hit->GetTime();
                }

                plane = hit->GetDetId();
                wire = hit->GetWireId();
                x[plane - 1] = hit->GetX();
                y[plane - 1] = hit->GetY();
                eloss[plane - 1] = hit->GetEloss();
                t[plane - 1] = hit->GetTime();
                fh_Pdc_Tot[plane - 1]->Fill(wire, eloss[plane - 1]);
                fh_Pdc_Time[plane - 1]->Fill(wire, t[plane - 1]);

                // cout << "Plane: " << plane << " wire: " << wire << " x: " << x[plane - 1]
                //	<< " y: " << y[plane - 1] << " eloss: " << eloss[plane - 1] << endl;

                if (plane == 3)
                {
                    mult_pdc3++;
                }
                if (plane == 4)
                {
                    mult_pdc4++;
                }

                mult[plane - 1][wire]++;

                // if (x[plane - 1] > 0 && eloss[plane - 1] > TotMax[plane - 1])
                if (x[plane - 1] > 0)
                {

                    // cout << "Plane: " << plane << " wire: " << wire << " x: " << x[plane - 1] << endl;

                    fh_Pdc_x[plane - 1]->Fill(x[plane - 1]);
                    xPair[plane - 1] = x[plane - 1];
                    TotMax[plane - 1] = eloss[plane - 1];
                }

                // if (y[plane - 1] > 0 && eloss[plane - 1] > TotMax[plane - 1])
                if (y[plane - 1] > 0)
                {

                    // cout << "Plane: " << plane << " wire: " << wire << " y: " << y[plane - 1] << endl;

                    fh_Pdc_y[plane - 1]->Fill(y[plane - 1]);
                    yPair[plane - 1] = y[plane - 1];
                    TotMax[plane - 1] = eloss[plane - 1];
                }

                if (plane == 4 && y[plane - 1] > 350. && y[plane - 1] < 390.)
                {
                    yPdc4test = y[plane - 1];
                }
            }

            Bool_t true1 = false, true2 = false, true3 = false;

            // Choose correlated x1 vs x3 hits
            if (abs(xPair[0] - xPair[2]) < 45 && xPair[0] > 0 && xPair[2] > 0)
            {
                ipl[0] += 1;
                ipl[2] += 1;
                ppl[0][ipl[0]] = xPair[0];
                ppl[2][ipl[2]] = xPair[2];
                tpl[0][ipl[0]] = t[0];
                tpl[2][ipl[2]] = t[2];
                epl[0][ipl[0]] = eloss[0];
                epl[2][ipl[2]] = eloss[2];
                true1 = true;
            }
            // For correct x3 hits, choose proper y4 hits
            if (ppl[2][ipl[2]] > 0 && yPair[3] > 0 && true1)
            {
                ipl[3] += 1;
                ppl[3][ipl[3]] = yPair[3];
                tpl[3][ipl[3]] = t[3];
                epl[3][ipl[3]] = eloss[3];
                true2 = true;
            }

            // For correct y4 hits, choose correlated y2 hits; y2 is chose as last, as one card is missing in this plane
            // and it should not influence hits in other planes:
            if (yPair[1] > 0 && abs(yPair[1] - ppl[3][ipl[3]]) < 45 && true2)
            {
                ipl[1] += 1;
                ppl[1][ipl[1]] = yPair[1];
                tpl[1][ipl[1]] = t[1];
                epl[1][ipl[1]] = eloss[1];
                true3 = true;
            }

            //   if(true1 && true2) cout<<"Checking: "<<abs(ppl[0][ipl[0]] - ppl[2][ipl[2]]) <<", "<<ppl[0][ipl[0]]<<",
            //   "<< ppl[2][ipl[2]] <<", "<<
            //                    abs(ppl[1][ipl[1]] - ppl[3][ipl[3]]) <<", "<< ppl[1][ipl[1]] <<", "<<
            //                    ppl[3][ipl[3]]<<endl;

            if (true1 && true2)
            {
                xPdc1 = ppl[0][ipl[0]];
                yPdc2 = ppl[1][ipl[1]];
                xPdc3 = ppl[2][ipl[2]];
                yPdc4 = ppl[3][ipl[3]];
                for (Int_t ia = 0; ia < 4; ia++)
                {
                    tPair[ia] = tpl[ia][ipl[ia]];
                    eloss[ia] = epl[ia][ipl[ia]];
                }
                fh_Pdc_tvst[0]->Fill(tPair[0], tPair[1]);
                fh_Pdc_evse[0]->Fill(eloss[0], eloss[1]);
                fh_Pdc_tvst[1]->Fill(tPair[2], tPair[3]);
                fh_Pdc_evse[1]->Fill(eloss[2], eloss[3]);
                fh_Pdc_xy[1]->Fill(xPdc1, yPdc2);
                fh_Pdc_xy[3]->Fill(xPdc3, yPdc4);
            }
            if (xPdc1 > 0. && xPdc3 > 0. && yPdc2 > 0. && yPdc4 > 0.)
            {

                // cout << "Test: " << xPdc1 << "  " << yPdc2 << "  " << xPdc3 << "  " << yPdc4 << endl;
                fh_Pdc_xvsx->Fill(xPdc1, xPdc3);
                fh_Pdc_yvsy->Fill(yPdc2, yPdc4);
                fh_Pdc_xvsy_fi0->Fill((xPdc1 + xPdc3) / 2., (yPdc2 + yPdc4) / 2.);
            }
        }
    }
    if (ipl[0] > 1 || ipl[1] > 1 || ipl[2] > 1 || ipl[3] > 1)
        cout << "Multipl: " << ipl[0] << ", " << ipl[1] << "; " << ipl[2] << ", " << ipl[3] << endl;

    //----------------------------------------------------------------------
    // Fiber0 detectors
    //----------------------------------------------------------------------
    if (fMappedItems_fi0)
    {
        // cout << "in Fiber0" << endl;
        Int_t mult_fi0_0 = 0;
        Int_t mult_fi0_1 = 0;
        Int_t iFib = 0;
        Int_t iFib2 = 0;

        auto det = fMappedItems_fi0;
        Int_t nMapped = det->GetEntriesFast();
        for (Int_t imapped = 0; imapped < nMapped; imapped++)
        {
            auto mapped = (R3BBunchedFiberMappedData const*)det->At(imapped);
            if (!mapped)
                continue; // should not happen
            if (!mapped->IsMAPMT())
            {
                continue;
            }

            iFib = mapped->GetChannel(); // 1..n
            mult_fi0_0++;
            fh_fi0_0_fiber->Fill(iFib);
        }
        // if (mult_fi0_0 > 0)
        {
            fh_fi0_0_mult->Fill(mult_fi0_0 / 2.); // divided by 2 for leading and trailing edges
        }
        if (mult_fi0_1 > 0)
        {
            fh_fi0_1_mult->Fill(mult_fi0_1 / 2.);
        }

        // Cal Items
        if (fCalItems_fi0)
        {
        }

        // Hit Items
        if (fHitItems_fi0)
        {
            Int_t multi[128] = { 0 };
            Int_t nHits = fHitItems_fi0->GetEntriesFast();
            Double_t xpos = 0. / 0.;
            Double_t ypos = 0. / 0.;
            Double_t tfib = 0. / 0., tof_fib = 0. / 0.;
            Double_t randx;
            // cout<<"new event: "<< nHits<<"  "<<ifibcount<<endl;

            Double_t tMAPMT = 0. / 0.;
            Double_t tSPMT1 = 0. / 0.;
            Double_t tSPMT2 = 0. / 0.;
            Double_t tot = 0. / 0.;

            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                // Loop first time to get time of single PMT
                tMAPMT = 0. / 0.;
                R3BBunchedFiberHitData* hit = (R3BBunchedFiberHitData*)fHitItems_fi0->At(ihit);
                if (!hit)
                    continue;

                iFib = hit->GetFiberId(); // 1..
                tMAPMT = hit->GetTime();  //+ 5444.;
                tot = hit->GetEloss();

                fh_fi0_0_Tot->Fill(iFib, tot);
                fh_fi0_0_Time->Fill(iFib, tMAPMT);
            }
            Double_t tot1 = 0.;
            Double_t tot2 = 0.;
            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                tMAPMT = 0. / 0.;
                tot = 0. / 0.;
                Double_t dtime = 0. / 0.;
                R3BBunchedFiberHitData* hit = (R3BBunchedFiberHitData*)fHitItems_fi0->At(ihit);
                if (!hit)
                    continue;

                iFib = hit->GetFiberId(); // 1..
                tMAPMT = hit->GetTime();  //+ 5444.;
                tot = hit->GetEloss();

                // fiber correlation plot. Loop again over all fibers
                for (Int_t jhit = 0; jhit < nHits; jhit++)
                {
                    R3BBunchedFiberHitData* hit2 = (R3BBunchedFiberHitData*)fHitItems_fi0->At(jhit);
                    if (!hit2)
                        continue;

                    iFib2 = hit2->GetFiberId(); // 1..
                    if (iFib != iFib2)
                    {
                        fh_fi0_cor->Fill(iFib, iFib2);
                    }
                }

                // cuts ***********************************
                // if not in the right time window, go to next
                if (tMAPMT < -160 || tMAPMT > -120)
                {
                    continue;
                }

                if (nHits == 2 && ihit == 0)
                    tot1 = tot;
                if (nHits == 2 && ihit == 1)
                    tot2 = tot;

                if (tot < 0 || tot > 2500)
                {
                    continue;
                }
                // cout << "Test: " << tSPMT1 << IS_NAN(tSPMT1) << endl;
                xpos = hit->GetX();
                ypos = dtime;
                multi[iFib - 1]++;
                fh_fi0_0_Tot_mc->Fill(iFib, tot);
                fh_fi0_0_Time_mc->Fill(iFib, tMAPMT);
                fh_fi0_0_fiber_mc->Fill(iFib);

                // if (yPdc4 > 0. && eloss[3] > 35 && nHits == 1)
                if (nHits > 0 && nHits < 3)
                {
                    // cout << "Fib0: fiber = " << iFib << endl;
                }

                // if (yPdc4test > 0. && nHits > 0 && nHits < 2 )
                // if (yPdc4 > 0. && nHits > 0 && nHits < 2 )
                // if (yPdc4 > 0. && nHits > 0 && mult_pdc4 == 1)
                // if (yPdc4 > 0. && nHits > 0 )
                if (yPdc4test > 0. && nHits > 0)
                {
                    // fh_fi0_0_pdc->Fill(iFib, yPdc4);
                    fh_fi0_0_pdc->Fill(iFib, yPdc4test);
                    fh_fi0_pdc_time->Fill(tMAPMT, tPair[3]);
                    fh_fi0_pdc_eloss->Fill(tot, eloss[3]);
                }
                // if (xPdc3test > 0. && nHits > 0 && nHits < 2 )
                // if (xPdc1 > 0. && nHits > 0 && nHits < 2 )
                if (yPdc2 > 0. && nHits > 0)
                {
                    fh_fi0_1_pdc->Fill(iFib, yPdc2);
                    // fh_fi0_1_pdc->Fill(iFib, xPdc3test);
                }
            } // end for(ihit)
            if (xPdc1 > 0 && xPdc3 > 0 && yPdc2 > 0 && yPdc4 > 0 && nHits > 0)
            {
                fh_Pdc_xvsx_fi0->Fill(xPdc1, xPdc3);
                fh_Pdc_yvsy_fi0->Fill(yPdc2, yPdc4);
                fh_fi0_eff->Fill((xPdc1 + xPdc3) / 2., (yPdc2 + yPdc4) / 2.);
            }

            fh_fi0_tot2_tot1->Fill(tot1, tot2);
            Int_t multi_fi0 = 0;
            Int_t multi_fi1 = 0;
            for (Int_t i = 0; i < 128; i++)
            {
                multi_fi0 = multi_fi0 + multi[i];
            }
            if (multi_fi0 > 0)
                fh_fi0_0_mult_mc->Fill(multi_fi0);
        }
    }

    //----------------------------------------------------------------------
    // Fiber1 detectors
    //----------------------------------------------------------------------
    /*
        if (fMappedItems_fi1a)
        {
            Int_t mult_fi1a = 0;
            Int_t iFib = 0;

            auto det1a = fMappedItems_fi1a;
            Int_t nMapped1a = det1a->GetEntriesFast();
            for (Int_t imapped = 0; imapped < nMapped1a; imapped++)
            {
                auto mapped = (R3BBunchedFiberMappedData const*)det1a->At(imapped);
                if (!mapped)
                    continue; // should not happen
                if (!mapped->IsMAPMT())
                {
                    continue;
                }

                if (mapped->IsMAPMT())
                {
                    iFib = mapped->GetChannel(); // 1..n
                    if (mapped->IsLeading())
                    {
                        mult_fi1a++;
                        fh_fi1a_fiber->Fill(iFib);
                    }
                }
            }
            if (mult_fi1a > 0)
            {
                fh_fi1a_mult->Fill(mult_fi1a);
            }
        }

        // Cal Items

        if (fCalItems_fi1a)
        {
        }

        // Hit Items
        if (fHitItems_fi1a)
        {
            if (1 == 1)
            {
                Int_t multi[256] = { 0 };
                Int_t iFib;
                Int_t nHits = fHitItems_fi1a->GetEntriesFast();

                Double_t xpos = 0. / 0.;
                Double_t ypos = 0. / 0.;
                Double_t tfib = 0. / 0., tof_fib = 0. / 0.;
                Double_t randx;

                // if(nHits>0) cout<<"new event Fi1a: "<< nHits<<endl;

                Double_t tMAPMT = 0. / 0.;
                Double_t tSPMT1 = 0. / 0.;
                Double_t tSPMT2 = 0. / 0.;
                Double_t tot = 0. / 0.;


                for (Int_t ihit = 0; ihit < nHits; ihit++)
                {
                    tMAPMT = 0. / 0.;
                    tot = 0. / 0.;
                    Double_t dtime = 0. / 0.;
                    R3BBunchedFiberHitData* hit = (R3BBunchedFiberHitData*)fHitItems_fi1a->At(ihit);
                    if (!hit)
                        continue;

                    iFib = hit->GetFiberId(); // 1..
                    tMAPMT = hit->GetTime();
                    tot = hit->GetEloss(); // hit->GetMAPMTToT_ns();

                    // cout << "Hit: " << ihit << " Fib: " << iFib << " t: " << tMAPMT
                    //	 << " ToT: " << tot << endl;

                    fh_fi1a_Tot->Fill(iFib, tot);
                    fh_fi1a_Time->Fill(iFib, tMAPMT);

                    // cuts ***********************************
                    // if not in the right time window, go to next

                    if (tMAPMT < -25 || tMAPMT > 30)
                    {
                        continue;
                    }
                    if (tot > 20)
                    {
                        // continue;
                    }
                    // cout << "Test: " << tSPMT1 << IS_NAN(tSPMT1) << endl;
                    xpos = hit->GetX();
                    ypos = hit->GetY();
                    Double_t ttt = hit->GetTime();
                    multi[iFib - 1]++;
                    fh_fi1a_Tot_mc->Fill(iFib, tot);
                    fh_fi1a_Time_mc->Fill(iFib, tMAPMT);
                    fh_fi1a_fiber_mc->Fill(iFib);
                    if (xPdc1 > 0. && eloss[0] > 35. && nHits == 1)
                    {
                        fh_fi1a_pdc->Fill(iFib, xPdc1);
                        fh_fi1a_pdc_time->Fill(tMAPMT, tPair[0]);
                        fh_fi1a_pdc_eloss->Fill(tot, eloss[0]);
                    }

                    // fh_fi1a_pdc->Fill(iFib * 0.2, yPdc);
                } // end for(ihit)

                Int_t multi_fi1a = 0;
                for (Int_t i = 0; i < 256; i++)
                {
                    multi_fi1a = multi_fi1a + multi[i];
                }
                if (multi_fi1a > 0)
                    fh_fi1a_mult_mc->Fill(multi_fi1a);
            }
        }
        if (fMappedItems_fi1b)
        {
            Int_t mult_fi1b = 0;
            Int_t iFib = 0;

            auto det1b = fMappedItems_fi1b;
            Int_t nMapped1b = det1b->GetEntriesFast();
            for (Int_t imapped = 0; imapped < nMapped1b; imapped++)
            {
                auto mapped = (R3BBunchedFiberMappedData const*)det1b->At(imapped);
                if (!mapped)
                    continue; // should not happen
                if (!mapped->IsMAPMT())
                {
                    continue;
                }
                if (mapped->IsMAPMT())
                {
                    iFib = mapped->GetChannel(); // 1..n
                    if (mapped->IsLeading())
                    {
                        mult_fi1b++;
                        fh_fi1b_fiber->Fill(iFib);
                    }
                }
            }
            if (mult_fi1b > 0)
            {
                fh_fi1b_mult->Fill(mult_fi1b);
            }
        }

        // Cal Items

        if (fCalItems_fi1b)
        {
        }

        // Hit Items
        if (fHitItems_fi1b)
        {
            if (1 == 1)
            {
                Int_t multib[256] = { 0 };
                Int_t iFib;
                Int_t nHits = fHitItems_fi1b->GetEntriesFast();
                Double_t xpos = 0. / 0.;
                Double_t ypos = 0. / 0.;
                Double_t tfib = 0. / 0., tof_fib = 0. / 0.;
                Double_t randx;

                // if(nHits>0) cout<<"new event Fi1b: "<< nHits<<endl;

                Double_t tMAPMT = 0. / 0.;
                Double_t tSPMT1 = 0. / 0.;
                Double_t tSPMT2 = 0. / 0.;
                Double_t tot = 0. / 0.;

                for (Int_t ihit = 0; ihit < nHits; ihit++)
                {
                    tMAPMT = 0. / 0.;
                    tot = 0. / 0.;
                    Double_t dtime = 0. / 0.;
                    R3BBunchedFiberHitData* hit = (R3BBunchedFiberHitData*)fHitItems_fi1b->At(ihit);
                    if (!hit)
                        continue;

                    iFib = hit->GetFiberId(); // 1..
                    tMAPMT = hit->GetTime();
                    tot = hit->GetEloss(); // hit->GetMAPMTToT_ns();
                    xpos = hit->GetX();
                    ypos = hit->GetY();
                    Double_t ttt = hit->GetTime();

                    // cout << "Hit: " << ihit << " Fib: " << iFib << " t: " << tMAPMT
                    //	 << " ToT: " << tot << endl;

                    fh_fi1b_Tot->Fill(iFib, tot);
                    fh_fi1b_Time->Fill(iFib, tMAPMT);

                    Int_t nHitsa = fHitItems_fi1a->GetEntriesFast();
                    Double_t xposa = 0. / 0.;
                    Double_t tfiba = 0. / 0.;
                    Double_t tota = 0. / 0.;

                    // cuts ***********************************
                    // if not in the right time window, go to next
                    if (tMAPMT < -25 || tMAPMT > 30)
                    {
                        continue;
                    }
                    if (tot > 25)
                    {
                        // continue;
                    }
                    // cout << "Test: " << tSPMT1 << IS_NAN(tSPMT1) << endl;
                    for (Int_t ihita = 0; ihita < nHitsa; ihita++)
                    {
                        R3BBunchedFiberHitData* hita = (R3BBunchedFiberHitData*)fHitItems_fi1a->At(ihita);
                        if (!hita)
                            continue;

                        xposa = hita->GetX();
                        tfiba = hita->GetTime();
                        tota = hita->GetEloss();
                        if (tfiba < -25 || tfiba > 30)
                        {
                            continue;
                        }
                        fh_fi1_xy->Fill(xposa, ypos);
                        fh_fi1_tot->Fill(tota, tot);
                        fh_fi1_time->Fill(tfiba, ttt);
                    }
                    multib[iFib - 1]++;
                    fh_fi1b_Tot_mc->Fill(iFib, tot);
                    fh_fi1b_Time_mc->Fill(iFib, tMAPMT);
                    fh_fi1b_fiber_mc->Fill(iFib);
                    // fh_fi1b_pdc->Fill(iFib * 0.2, xPdc);

                    if (yPdc4 > 0. && eloss[3] > 35. && nHits == 1)
                    {
                        fh_fi1b_pdc->Fill(iFib, yPdc4);
                        fh_fi1b_pdc_time->Fill(tMAPMT, tPair[3]);
                        fh_fi1b_pdc_eloss->Fill(tot, eloss[3]);
                    }
                } // end for(ihit)
                Int_t multi_fi1b = 0;
                for (Int_t i = 0; i < 256; i++)
                {
                    multi_fi1b = multi_fi1b + multib[i];
                }
                if (multi_fi1b > 0)
                    fh_fi1b_mult_mc->Fill(multi_fi1b);
            }
        }
    */
    fNEvents += 1;
}

void R3BOnlineSpectraPdc::FinishEvent()
{

    if (fMappedItems)
        fMappedItems->Clear();
    if (fCalItems)
        fCalItems->Clear();
    if (fHitItems)
        fHitItems->Clear();

    if (fMappedItems_fi0)
        fMappedItems_fi0->Clear();
    if (fCalItems_fi0)
        fCalItems_fi0->Clear();
    if (fHitItems_fi0)
        fHitItems_fi0->Clear();
    /* for the moment fib1a and gib1b are taken out (MH)
        if (fMappedItems_fi1a)
            fMappedItems_fi1a->Clear();
        if (fCalItems_fi1a)
            fCalItems_fi1a->Clear();
        if (fHitItems_fi1a)
            fHitItems_fi1a->Clear();

        if (fMappedItems_fi1b)
            fMappedItems_fi1b->Clear();
        if (fCalItems_fi1b)
            fCalItems_fi1b->Clear();
        if (fHitItems_fi1b)
            fHitItems_fi1b->Clear();
    */
}

void R3BOnlineSpectraPdc::FinishTask()
{
    if (fMappedItems_fi0)
    {
        fh_fi0_0_fiber->Write();
        fh_fi0_1_fiber->Write();
        fh_fi0_0_mult->Write();
        fh_fi0_1_mult->Write();
        fh_fi0_0_Tot->Write();
        fh_fi0_1_Tot->Write();
        fh_fi0_0_Time->Write();
        fh_fi0_1_Time->Write();
        fh_fi0_0_fiber_mc->Write();
        fh_fi0_1_fiber_mc->Write();
        fh_fi0_0_mult_mc->Write();
        fh_fi0_1_mult_mc->Write();
        fh_fi0_0_Tot_mc->Write();
        fh_fi0_1_Tot_mc->Write();
        fh_fi0_0_Time_mc->Write();
        fh_fi0_1_Time_mc->Write();
        fh_fi0_0_pdc->Write();
        fh_fi0_1_pdc->Write();
        fh_fi0_pdc_time->Write();
        fh_fi0_pdc_eloss->Write();
        fh_fi0_tot2_tot1->Write();
        fh_fi0_cor->Write();
        fh_fi0_eff->Write();
    }

    if (fMappedItems_fi1a)
    {
        fh_fi1a_fiber->Write();
        fh_fi1b_fiber->Write();
        fh_fi1a_mult->Write();
        fh_fi1b_mult->Write();
        fh_fi1a_Tot->Write();
        fh_fi1b_Tot->Write();
        fh_fi1a_Time->Write();
        fh_fi1b_Time->Write();
        fh_fi1a_fiber_mc->Write();
        fh_fi1b_fiber_mc->Write();
        fh_fi1a_mult_mc->Write();
        fh_fi1b_mult_mc->Write();
        fh_fi1a_Tot_mc->Write();
        fh_fi1b_Tot_mc->Write();
        fh_fi1a_Time_mc->Write();
        fh_fi1b_Time_mc->Write();
        fh_fi1a_pdc->Write();
        fh_fi1b_pdc->Write();
        fh_fi1b_pdc_time->Write();
        fh_fi1a_pdc_time->Write();
        fh_fi1b_pdc_eloss->Write();
        fh_fi1a_pdc_eloss->Write();
        fh_fi1_time->Write();
        fh_fi1_xy->Write();
        fh_fi1_tot->Write();
    }

    if (fMappedItems)
    {
        fh_Pdc_xvsx_fi0->Write();
        fh_Pdc_xvsx->Write();
        fh_Pdc_yvsy_fi0->Write();
        fh_Pdc_xvsy_fi0->Write();
        fh_Pdc_yvsy->Write();
        fh_Pdc_wvsw->Write();
        fh_pdc_ebene4->Write();
        fh_Pdc_tvst[0]->Write();
        fh_Pdc_tvst[1]->Write();
        fh_Pdc_evse[0]->Write();
        fh_Pdc_evse[1]->Write();
        /*        for (Int_t i = 0; i < 144; i++)
                {
                    fh_Pdc_ToT_vs_Time[i]->Write();
                    fh_Pdc_ToT_vs_Hit[i]->Write();
                }*/
        for (Int_t i = 0; i < 4; i++)
        {
            // fh_Pdc_ToF[i]->Write();
            fh_Pdc_x[i]->Write();
            fh_Pdc_y[i]->Write();
            fh_Pdc_xy[i]->Write();
            fh_Pdc_Wire[i]->Write();
            fh_Pdc_mult[i]->Write();
            fh_Pdc_Tot[i]->Write();
            fh_Pdc_Time[i]->Write();
            fh_Pdc_Wire_vs_Events[i]->Write();
        }
    }
}

ClassImp(R3BOnlineSpectraPdc)
