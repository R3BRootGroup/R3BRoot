// ------------------------------------------------------------
// -----                  R3BOnlineJuelich2023                -----
// -----          Created April  2023 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BOnlineJuelich2023.h"

#include "R3BRoluCalData.h"
#include "R3BRoluMappedData.h"

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
#include <boost/multi_array.hpp>
#include <cassert>
#include <cmath> // NAN

template <class T>
static void init_array(T& array, double init)
{
    // boost::multi_array prefers cascaded for loops over index sets
    // (e.g. python's dict().keys())
    // thus we can not really use a range based for loop
    // and instead wrangle pointers like it is 1980 again.
    for (double* e = array.origin(); e < array.origin() + array.num_elements(); ++e)
        *e = init;
}

#define IS_NAN(x) TMath::IsNaN(x)

using namespace std;

R3BOnlineJuelich2023::R3BOnlineJuelich2023()
    : R3BOnlineJuelich2023("OnlineSpectra", 1)
{
}

R3BOnlineJuelich2023::R3BOnlineJuelich2023(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fSamp(-1)
    , fNEvents(0)
{
}

R3BOnlineJuelich2023::~R3BOnlineJuelich2023() {}

InitStatus R3BOnlineJuelich2023::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(info) << "R3BOnlineJuelich2023::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("EventHeader.");
    FairRunOnline* run = FairRunOnline::Instance();

    run->GetHttpServer()->Register("/Tasks", this);

    // Get objects for detectors on all levels
    fMappedItems_rolu = (TClonesArray*)mgr->GetObject("RoluMapped");
    if (NULL == fMappedItems_rolu)
        LOG(fatal) << "Branch roluMapped not found";

    fCalItems_rolu = (TClonesArray*)mgr->GetObject("RoluCal");
    if (NULL == fCalItems_rolu)
        LOG(fatal) << "Branch roluCal not found";

    fHitItems_rolu = (TClonesArray*)mgr->GetObject("RoluHit");
    if (NULL == fHitItems_rolu)
        LOG(fatal) << "Branch roluHit not found";

    fMappedItems_fi0 = (TClonesArray*)mgr->GetObject("Fi0Mapped");
    if (NULL == fMappedItems_fi0)
        LOG(error) << "Branch Fi0Mapped not found";

    fCalItems_fi0 = (TClonesArray*)mgr->GetObject("Fi0Cal");
    if (NULL == fCalItems_fi0)
        LOG(error) << "Branch Fi0Cal not found";

    fHitItems_fi0 = (TClonesArray*)mgr->GetObject("Fi0Hit");
    if (NULL == fHitItems_fi0)
        LOG(error) << "Branch Fi0Hit not found";

    fMappedItems_fi60 = (TClonesArray*)mgr->GetObject("Fi60Mapped");
    if (NULL == fMappedItems_fi60)
        LOG(error) << "Branch Fi60Mapped not found";

    fCalItems_fi60 = (TClonesArray*)mgr->GetObject("Fi60Cal");
    if (NULL == fCalItems_fi60)
        LOG(error) << "Branch Fi60Cal not found";

    fHitItems_fi60 = (TClonesArray*)mgr->GetObject("Fi60Hit");
    if (NULL == fHitItems_fi60)
        LOG(error) << "Branch Fi60Hit not found";

    fMappedItems_fi61 = (TClonesArray*)mgr->GetObject("Fi61Mapped");
    if (NULL == fMappedItems_fi61)
        LOG(error) << "Branch Fi61Mapped not found";

    fCalItems_fi61 = (TClonesArray*)mgr->GetObject("Fi61Cal");
    if (NULL == fCalItems_fi61)
        LOG(error) << "Branch Fi61Cal not found";

    fHitItems_fi61 = (TClonesArray*)mgr->GetObject("Fi61Hit");
    if (NULL == fHitItems_fi61)
        LOG(error) << "Branch Fi61Hit not found";

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
    //------------------------------------------------------------------------
    // Rolu detector

    if (fMappedItems_rolu)
    {
        TCanvas* cRolu = new TCanvas("Rolu", "Rolu", 10, 10, 800, 400);

        fh_rolu_tot = new TH2F("Rolu_tot", "ROLU ToT", 10, 0, 10, 3000, 0, 300);
        fh_rolu_tot->GetXaxis()->SetTitle("Channel number");
        fh_rolu_tot->GetYaxis()->SetTitle("ToT / ns");

        fh_rolu_channels = new TH1F("Rolu_channels", "ROLU channels", 10, 0, 10);
        fh_rolu_channels->GetXaxis()->SetTitle("Channel number");
        fh_rolu_channels->GetYaxis()->SetTitle("Counts");

        cRolu->Divide(2, 1);
        cRolu->cd(1);
        gPad->SetLogy();
        fh_rolu_channels->Draw();
        cRolu->cd(2);
        gPad->SetLogz();
        fh_rolu_tot->Draw("colz");
        cRolu->cd(0);
        run->AddObject(cRolu);

        run->GetHttpServer()->RegisterCommand("Reset_ROLU", Form("/Tasks/%s/->Reset_ROLU_Histo()", GetName()));
    }

    //---------------------------------------------------------------------------------------------------
    // Fiber0 detector
    if (fMappedItems_fi0)
    {
        Int_t ch = 1050;
        Double_t ds = 1000. / 150. / 16.;

        fh_fi0_fiber = new TH1F("Fi0_fibers", "Fi0 fibers", ch, 0., ch);
        fh_fi0_fiber->GetXaxis()->SetTitle("Fiber");
        fh_fi0_fiber->GetYaxis()->SetTitle("Counts");

        fh_fi0_mult = new TH1F("Fi0_mult", "Fi0 mult", ch, 0., ch);
        fh_fi0_mult->GetXaxis()->SetTitle("mult");
        fh_fi0_mult->GetYaxis()->SetTitle("Counts");

        fh_fi0_Tot = new TH2F("Fi0_ToT", "Fi0 ToT", ch, 0., ch, (int)(500. / ds), 0, 500);
        fh_fi0_Tot->GetXaxis()->SetTitle("Fiber");
        fh_fi0_Tot->GetYaxis()->SetTitle("ToT");

        fh_fi0_Time = new TH2F("Fi0_Time", "Fi0 time", ch, 0., ch, 2500, -1500, 1000);
        fh_fi0_Time->GetXaxis()->SetTitle("Fiber");
        fh_fi0_Time->GetYaxis()->SetTitle("time");

        // with cuts
        fh_fi0_fiber_ave = new TH1F("Fi0_fibers_ave", "Fi0 fibers average", ch, 0., ch);
        fh_fi0_fiber_ave->GetXaxis()->SetTitle("Fiber");
        fh_fi0_fiber_ave->GetYaxis()->SetTitle("Counts");

        fh_fi0_fiber_mc = new TH1F("Fi0_fibers_mc", "Fi0 fibers with cuts", ch, 0., ch);
        fh_fi0_fiber_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi0_fiber_mc->GetYaxis()->SetTitle("Counts");

        fh_fi0_mult_mc = new TH1F("Fi0_mult_mc", "Fi0 mult with cuts", ch, 0., ch);
        fh_fi0_mult_mc->GetXaxis()->SetTitle("mult");
        fh_fi0_mult_mc->GetYaxis()->SetTitle("Counts");

        fh_fi0_Tot_mc = new TH2F("Fi0_ToT_mc", "Fi0 ToT with cuts", ch, 0., ch, (int)(500. / ds), 0, 500);
        fh_fi0_Tot_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi0_Tot_mc->GetYaxis()->SetTitle("ToT");

        fh_fi0_Tot_ave = new TH2F("Fi0_ToT_ave", "Fi0 ToT sum vs. fiber average", ch, 0., ch, (int)(500. / ds), 0, 500);
        fh_fi0_Tot_ave->GetXaxis()->SetTitle("Fiber");
        fh_fi0_Tot_ave->GetYaxis()->SetTitle("ToT");

        fh_fi0_Time_mc = new TH2F("Fi0_Time_mc", "Fi0 time with cuts", ch, 0., ch, (int)(1000. / ds), -500, 500);
        fh_fi0_Time_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi0_Time_mc->GetYaxis()->SetTitle("time");

        fh_fi0_cor = new TH2F("Fi0_cor", "Fi0 correlation plot", ch, 0, ch, ch, 0, ch);
        fh_fi0_cor->GetXaxis()->SetTitle("Fiber");
        fh_fi0_cor->GetYaxis()->SetTitle("Fiber");

        fh_fi0_tot2_tot1 =
            new TH2F("Fi0_ToT1_vs_ToT2", "Fi0 ToT1 vs. ToT2", (int)(60. / ds), 0, 60, (int)(60. / ds), 0, 60);
        fh_fi0_tot2_tot1->GetXaxis()->SetTitle("ToT1");
        fh_fi0_tot2_tot1->GetYaxis()->SetTitle("ToT2");

        fh_fi0_eff = new TH2F("fi0_eff", "Fi0 hits for PDC x vs y", 1500, 0, 1500, 1500, 0, 1500);
        fh_fi0_eff->GetXaxis()->SetTitle("x in mm");
        fh_fi0_eff->GetYaxis()->SetTitle("y in mm");

        fh_fi0_eff_vs_time = new TH2F("fi0_eff_vs_time", "Fi0 eff vs. time", 10000, 0, 10000, 100, 0, 100);
        fh_fi0_eff_vs_time->GetXaxis()->SetTitle("number of hits");
        fh_fi0_eff_vs_time->GetYaxis()->SetTitle("eff");

        TCanvas* cFib0 = new TCanvas("Fib0", "Fiber0 detectors", 50, 50, 500, 500);
        cFib0->Divide(4, 2);

        cFib0->cd(1);
        gPad->SetLogy();
        fh_fi0_fiber->Draw();
        cFib0->cd(2);
        gPad->SetLogy();
        fh_fi0_mult->Draw();
        cFib0->cd(3);
        gPad->SetLogz();
        fh_fi0_Tot->Draw("colz");
        cFib0->cd(4);
        gPad->SetLogz();
        fh_fi0_Time->Draw("colz");

        cFib0->cd(5);
        gPad->SetLogy();
        fh_fi0_fiber_mc->Draw();
        cFib0->cd(6);
        gPad->SetLogy();
        fh_fi0_mult_mc->Draw();
        cFib0->cd(7);
        gPad->SetLogz();
        fh_fi0_Tot_mc->Draw("colz");
        cFib0->cd(8);
        gPad->SetLogz();
        fh_fi0_Time_mc->Draw("colz");

        cFib0->cd(0);
        run->AddObject(cFib0);
        run->GetHttpServer()->RegisterCommand("Reset_Fi0", Form("/Tasks/%s/->Reset_Fi0_Histo()", GetName()));
    }

    //---------------------------------------------------------------------------------------------------
    // Fiber60 detector
    if (fMappedItems_fi60)
    {
        Int_t ch = 1050;
        Double_t ds = 1000. / 150. / 16.;

        fh_fi60_fiber = new TH1F("Fi60_fibers", "Fi60 fibers", ch, 0., ch);
        fh_fi60_fiber->GetXaxis()->SetTitle("Fiber");
        fh_fi60_fiber->GetYaxis()->SetTitle("Counts");

        fh_fi60_mult = new TH1F("Fi60_mult", "Fi60 mult", ch, 0., ch);
        fh_fi60_mult->GetXaxis()->SetTitle("mult");
        fh_fi60_mult->GetYaxis()->SetTitle("Counts");

        fh_fi60_Tot = new TH2F("Fi60_ToT", "Fi60 ToT", ch, 0., ch, (int)(500. / ds), 0, 500);
        fh_fi60_Tot->GetXaxis()->SetTitle("Fiber");
        fh_fi60_Tot->GetYaxis()->SetTitle("ToT");

        fh_fi60_Time = new TH2F("Fi60_Time", "Fi60 time", ch, 0., ch, 2500, -1500, 1000);
        fh_fi60_Time->GetXaxis()->SetTitle("Fiber");
        fh_fi60_Time->GetYaxis()->SetTitle("time");

        // with cuts
        fh_fi60_fiber_ave = new TH1F("Fi60_fibers_ave", "Fi60 fibers average", ch, 0., ch);
        fh_fi60_fiber_ave->GetXaxis()->SetTitle("Fiber");
        fh_fi60_fiber_ave->GetYaxis()->SetTitle("Counts");

        fh_fi60_fiber_mc = new TH1F("Fi60_fibers_mc", "Fi60 fibers with cuts", ch, 0., ch);
        fh_fi60_fiber_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi60_fiber_mc->GetYaxis()->SetTitle("Counts");

        fh_fi60_mult_mc = new TH1F("Fi60_mult_mc", "Fi60 mult with cuts", ch, 0., ch);
        fh_fi60_mult_mc->GetXaxis()->SetTitle("mult");
        fh_fi60_mult_mc->GetYaxis()->SetTitle("Counts");

        fh_fi60_Tot_mc = new TH2F("Fi60_ToT_mc", "Fi60 ToT with cuts", ch, 0., ch, (int)(500. / ds), 0, 500);
        fh_fi60_Tot_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi60_Tot_mc->GetYaxis()->SetTitle("ToT");

        fh_fi60_Tot_ave =
            new TH2F("Fi60_ToT_ave", "Fi60 ToT sum vs. fiber average", ch, 0., ch, (int)(500. / ds), 0, 500);
        fh_fi60_Tot_ave->GetXaxis()->SetTitle("Fiber");
        fh_fi60_Tot_ave->GetYaxis()->SetTitle("ToT");

        fh_fi60_Time_mc = new TH2F("Fi60_Time_mc", "Fi60 time with cuts", ch, 0., ch, (int)(1000. / ds), -500, 500);
        fh_fi60_Time_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi60_Time_mc->GetYaxis()->SetTitle("time");

        fh_fi60_cor = new TH2F("Fi60_cor", "Fi60 correlation plot", ch, 0, ch, ch, 0, ch);
        fh_fi60_cor->GetXaxis()->SetTitle("Fiber");
        fh_fi60_cor->GetYaxis()->SetTitle("Fiber");

        fh_fi60_tot2_tot1 =
            new TH2F("Fi60_ToT1_vs_ToT2", "Fi60 ToT1 vs. ToT2", (int)(60. / ds), 0, 60, (int)(60. / ds), 0, 60);
        fh_fi60_tot2_tot1->GetXaxis()->SetTitle("ToT1");
        fh_fi60_tot2_tot1->GetYaxis()->SetTitle("ToT2");

        fh_fi60_eff = new TH2F("fi60_eff", "Fi60 hits for PDC x vs y", 1500, 0, 1500, 1500, 0, 1500);
        fh_fi60_eff->GetXaxis()->SetTitle("x in mm");
        fh_fi60_eff->GetYaxis()->SetTitle("y in mm");

        fh_fi60_eff_vs_time = new TH2F("fi60_eff_vs_time", "Fi60 eff vs. time", 10000, 0, 10000, 100, 0, 100);
        fh_fi60_eff_vs_time->GetXaxis()->SetTitle("number of hits");
        fh_fi60_eff_vs_time->GetYaxis()->SetTitle("eff");

        TCanvas* cFib60 = new TCanvas("Fib60", "Fiber60 detectors", 50, 50, 500, 500);
        cFib60->Divide(4, 2);

        cFib60->cd(1);
        gPad->SetLogy();
        fh_fi60_fiber->Draw();
        cFib60->cd(2);
        gPad->SetLogy();
        fh_fi60_mult->Draw();
        cFib60->cd(3);
        gPad->SetLogz();
        fh_fi60_Tot->Draw("colz");
        cFib60->cd(4);
        gPad->SetLogz();
        fh_fi60_Time->Draw("colz");

        cFib60->cd(5);
        gPad->SetLogy();
        fh_fi60_fiber_mc->Draw();
        cFib60->cd(6);
        gPad->SetLogy();
        fh_fi60_mult_mc->Draw();
        cFib60->cd(7);
        gPad->SetLogz();
        fh_fi60_Tot_mc->Draw("colz");
        cFib60->cd(8);
        gPad->SetLogz();
        fh_fi60_Time_mc->Draw("colz");

        cFib60->cd(0);
        run->AddObject(cFib60);
        run->GetHttpServer()->RegisterCommand("Reset_Fi60", Form("/Tasks/%s/->Reset_Fi60_Histo()", GetName()));
    }

    //---------------------------------------------------------------------------------------------------
    // Fiber61 detector
    if (fMappedItems_fi61)
    {
        Int_t ch = 1050;
        Double_t ds = 1000. / 150. / 16.;

        fh_fi61_fiber = new TH1F("Fi61_fibers", "Fi61 fibers", ch, 0., ch);
        fh_fi61_fiber->GetXaxis()->SetTitle("Fiber");
        fh_fi61_fiber->GetYaxis()->SetTitle("Counts");

        fh_fi61_mult = new TH1F("Fi61_mult", "Fi61 mult", ch, 0., ch);
        fh_fi61_mult->GetXaxis()->SetTitle("mult");
        fh_fi61_mult->GetYaxis()->SetTitle("Counts");

        fh_fi61_Tot = new TH2F("Fi61_ToT", "Fi61 ToT", ch, 0., ch, (int)(500. / ds), 0, 500);
        fh_fi61_Tot->GetXaxis()->SetTitle("Fiber");
        fh_fi61_Tot->GetYaxis()->SetTitle("ToT");

        fh_fi61_Time = new TH2F("Fi61_Time", "Fi61 time", ch, 0., ch, 2500, -1500, 1000);
        fh_fi61_Time->GetXaxis()->SetTitle("Fiber");
        fh_fi61_Time->GetYaxis()->SetTitle("time");

        // with cuts
        fh_fi61_fiber_ave = new TH1F("Fi61_fibers_ave", "Fi61 fibers average", ch, 0., ch);
        fh_fi61_fiber_ave->GetXaxis()->SetTitle("Fiber");
        fh_fi61_fiber_ave->GetYaxis()->SetTitle("Counts");

        fh_fi61_fiber_mc = new TH1F("Fi61_fibers_mc", "Fi61 fibers with cuts", ch, 0., ch);
        fh_fi61_fiber_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi61_fiber_mc->GetYaxis()->SetTitle("Counts");

        fh_fi61_mult_mc = new TH1F("Fi61_mult_mc", "Fi61 mult with cuts", ch, 0., ch);
        fh_fi61_mult_mc->GetXaxis()->SetTitle("mult");
        fh_fi61_mult_mc->GetYaxis()->SetTitle("Counts");

        fh_fi61_Tot_mc = new TH2F("Fi61_ToT_mc", "Fi61 ToT with cuts", ch, 0., ch, (int)(500. / ds), 0, 500);
        fh_fi61_Tot_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi61_Tot_mc->GetYaxis()->SetTitle("ToT");

        fh_fi61_Tot_ave =
            new TH2F("Fi61_ToT_ave", "Fi61 ToT sum vs. fiber average", ch, 0., ch, (int)(500. / ds), 0, 500);
        fh_fi61_Tot_ave->GetXaxis()->SetTitle("Fiber");
        fh_fi61_Tot_ave->GetYaxis()->SetTitle("ToT");

        fh_fi61_Time_mc = new TH2F("Fi61_Time_mc", "Fi61 time with cuts", ch, 0., ch, (int)(1000. / ds), -500, 500);
        fh_fi61_Time_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi61_Time_mc->GetYaxis()->SetTitle("time");

        fh_fi61_cor = new TH2F("Fi61_cor", "Fi61 correlation plot", ch, 0, ch, ch, 0, ch);
        fh_fi61_cor->GetXaxis()->SetTitle("Fiber");
        fh_fi61_cor->GetYaxis()->SetTitle("Fiber");

        fh_fi61_tot2_tot1 =
            new TH2F("Fi61_ToT1_vs_ToT2", "Fi61 ToT1 vs. ToT2", (int)(60. / ds), 0, 60, (int)(60. / ds), 0, 60);
        fh_fi61_tot2_tot1->GetXaxis()->SetTitle("ToT1");
        fh_fi61_tot2_tot1->GetYaxis()->SetTitle("ToT2");

        fh_fi61_eff = new TH2F("fi61_eff", "Fi61 hits for PDC x vs y", 1500, 0, 1500, 1500, 0, 1500);
        fh_fi61_eff->GetXaxis()->SetTitle("x in mm");
        fh_fi61_eff->GetYaxis()->SetTitle("y in mm");

        fh_fi61_eff_vs_time = new TH2F("fi61_eff_vs_time", "Fi61 eff vs. time", 10000, 0, 10000, 100, 0, 100);
        fh_fi61_eff_vs_time->GetXaxis()->SetTitle("number of hits");
        fh_fi61_eff_vs_time->GetYaxis()->SetTitle("eff");

        TCanvas* cFib61 = new TCanvas("Fib61", "Fiber61 detectors", 50, 50, 500, 500);
        cFib61->Divide(4, 2);

        cFib61->cd(1);
        gPad->SetLogy();
        fh_fi61_fiber->Draw();
        cFib61->cd(2);
        gPad->SetLogy();
        fh_fi61_mult->Draw();
        cFib61->cd(3);
        gPad->SetLogz();
        fh_fi61_Tot->Draw("colz");
        cFib61->cd(4);
        gPad->SetLogz();
        fh_fi61_Time->Draw("colz");

        cFib61->cd(5);
        gPad->SetLogy();
        fh_fi61_fiber_mc->Draw();
        cFib61->cd(6);
        gPad->SetLogy();
        fh_fi61_mult_mc->Draw();
        cFib61->cd(7);
        gPad->SetLogz();
        fh_fi61_Tot_mc->Draw("colz");
        cFib61->cd(8);
        gPad->SetLogz();
        fh_fi61_Time_mc->Draw("colz");

        cFib61->cd(0);
        run->AddObject(cFib61);
        run->GetHttpServer()->RegisterCommand("Reset_Fi61", Form("/Tasks/%s/->Reset_Fi61_Histo()", GetName()));
    }

    //---------------------------------------------------------------------------------------------------
    // Fiber61 vs Fiber60 detector
    if (fMappedItems_fi61 && fMappedItems_fi60)
    {
        Int_t ch = 1050;
        Double_t ds = 1000. / 150. / 16.;

        fh_fi61_vs_fi60_fibers = new TH2F("Fi61_vs_Fi60_fibers", "Fi61 vs. Fi60 fibers", ch, 0., ch, ch, 0., ch);
        fh_fi61_vs_fi60_fibers->GetXaxis()->SetTitle("Fiber Fi60");
        fh_fi61_vs_fi60_fibers->GetYaxis()->SetTitle("Fiber Fi61");

        fh_fi61_vs_fi60_tot =
            new TH2F("Fi61_vs_Fi60_ToT", "Fi61 vs. Fi60 ToT", (int)(500. / ds), 0, 500, (int)(500. / ds), 0, 500);
        fh_fi61_vs_fi60_tot->GetXaxis()->SetTitle("ToT Fi60");
        fh_fi61_vs_fi60_tot->GetYaxis()->SetTitle("ToT Fi61");

        fh_fi60_vs_fi0_fibers = new TH2F("Fi60_vs_Fi0_fibers", "Fi60 vs. Fi0 fibers", ch, 0., ch, ch, 0., ch);
        fh_fi60_vs_fi0_fibers->GetXaxis()->SetTitle("Fiber Fi60");
        fh_fi60_vs_fi0_fibers->GetYaxis()->SetTitle("Fiber Fi0");

        fh_fi61_vs_fi0_fibers = new TH2F("Fi61_vs_Fi0_fibers", "Fi61 vs. Fi0 fibers", ch, 0., ch, ch, 0., ch);
        fh_fi61_vs_fi0_fibers->GetXaxis()->SetTitle("Fiber Fi61");
        fh_fi61_vs_fi0_fibers->GetYaxis()->SetTitle("Fiber Fi0");

        TCanvas* cFib61vsFib60 = new TCanvas("Fib61vsFib60", "Fiber61 vs. Fiber60 detectors", 50, 50, 800, 500);
        cFib61vsFib60->Divide(2, 3);

        cFib61vsFib60->cd(1);
        // gPad->SetLogy();
        fh_fi61_vs_fi60_fibers->Draw("colz");
        cFib61vsFib60->cd(2);
        // gPad->SetLogy();
        fh_fi61_vs_fi60_tot->Draw("colz");
        cFib61vsFib60->cd(3);
        fh_fi60_Tot_ave->Draw("colz");
        cFib61vsFib60->cd(4);
        fh_fi61_Tot_ave->Draw("colz");
        cFib61vsFib60->cd(5);
        fh_fi60_vs_fi0_fibers->Draw("colz");
        cFib61vsFib60->cd(6);
        fh_fi61_vs_fi0_fibers->Draw("colz");

        run->AddObject(cFib61vsFib60);
        run->GetHttpServer()->RegisterCommand("Reset_Fi61vsFi60",
                                              Form("/Tasks/%s/->Reset_Fi61vsFi60_Histo()", GetName()));
    }

    return kSUCCESS;
}

void R3BOnlineJuelich2023::Reset_ROLU_Histo()
{
    fh_rolu_channels->Reset();
    fh_rolu_tot->Reset();
}

void R3BOnlineJuelich2023::Reset_Fi0_Histo()
{
    fh_fi0_fiber->Reset();
    fh_fi0_mult->Reset();
    fh_fi0_Tot->Reset();
    fh_fi0_Time->Reset();
}

void R3BOnlineJuelich2023::Reset_Fi60_Histo()
{
    fh_fi60_fiber->Reset();
    fh_fi60_mult->Reset();
    fh_fi60_Tot->Reset();
    fh_fi60_Time->Reset();
}

void R3BOnlineJuelich2023::Reset_Fi61_Histo()
{
    fh_fi61_fiber->Reset();
    fh_fi61_mult->Reset();
    fh_fi61_Tot->Reset();
    fh_fi61_Time->Reset();
}

void R3BOnlineJuelich2023::Reset_Fi61vsFi60_Histo()
{
    fh_fi61_vs_fi60_fibers->Reset();
    fh_fi61_vs_fi60_tot->Reset();
}

void R3BOnlineJuelich2023::Exec(Option_t* option)
{
    fNEvents++;

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        LOG(error) << "FairRootManager not found";
        return;
    }

    //   check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
    {
        // return;
    }

    struct FibHit
    {
        Double_t X;
        Double_t ToT;

        FibHit(Double_t x, Double_t tot)
            : X(x)
            , ToT(tot)
        {
        }
    };

    std::vector<FibHit> Fib60Hits;
    std::vector<FibHit> Fib61Hits;
    std::vector<FibHit> Fib0Hits;

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
    // **************   ROLU *************************************
    if (fMappedItems_rolu)
    {
        auto det = fMappedItems_rolu;
        Int_t nHits = det->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BRoluMappedData* hit = (R3BRoluMappedData*)det->At(ihit);
            if (!hit)
                continue;

            // channel numbers are stored 1-based (1..n)
            Int_t iDet = hit->GetDetector(); // 1..
            Int_t iCha = hit->GetChannel();  // 1..

            if (iDet < 2)
                fh_rolu_channels->Fill(iCha); // ROLU 1
            if (iDet > 1)
                fh_rolu_channels->Fill(iCha + 4); // ROLU 2
        }
    }

    Int_t nParts;
    if (fCalItems_rolu)
    {

        auto det = fCalItems_rolu;
        nParts = det->GetEntriesFast();
        // if (nParts < 1)
        // return;

        Int_t iDet = 0;
        /*
         * Note: double x[nParts][2][4]={NAN};
         * will initialize everything other than x[0][0][0] to 0.0.
         * (and also not compile with our ancient gcc 4.8.5.)
         */
        using A = boost::multi_array<double, 3>;
        auto dims = boost::extents[nParts][2][4];
        A timeRolu_L(dims);
        init_array(timeRolu_L, NAN);
        A timeRolu_T(dims);
        init_array(timeRolu_T, NAN);
        A totRolu(dims);
        init_array(totRolu, NAN);

        for (Int_t iPart = 0; iPart < nParts; iPart++)
        {
            /*
             * nParts is the number of particle passing through detector in one event
             */
            R3BRoluCalData* calData = (R3BRoluCalData*)det->At(iPart);
            iDet = calData->GetDetector();

            for (Int_t iCha = 0; iCha < 4; iCha++)
            {

                if (!(IS_NAN(calData->GetTimeL_ns(iCha))))
                { // TAMEX leading
                    timeRolu_L[iPart][iDet - 1][iCha] = calData->GetTimeL_ns(iCha);
                }
                if (!(IS_NAN(calData->GetTimeT_ns(iCha))))
                { // TAMEX trailing
                    timeRolu_T[iPart][iDet - 1][iCha] = calData->GetTimeT_ns(iCha);
                }
                // cout << "ROLU: " << timeRolu_L[iPart][iDet - 1][iCha] << "  " << timeRolu_T[iPart][iDet - 1][iCha] <<
                // endl;
                if (timeRolu_T[iPart][iDet - 1][iCha] > 0. && timeRolu_L[iPart][iDet - 1][iCha] > 0. &&
                    !(IS_NAN(timeRolu_T[iPart][iDet - 1][iCha])) && !(IS_NAN(timeRolu_L[iPart][iDet - 1][iCha])))
                {
                    while (timeRolu_T[iPart][iDet - 1][iCha] - timeRolu_L[iPart][iDet - 1][iCha] <= 0.)
                    {
                        timeRolu_T[iPart][iDet - 1][iCha] = timeRolu_T[iPart][iDet - 1][iCha] + 2048. * ClockFreq;
                    }

                    totRolu[iPart][iDet - 1][iCha] =
                        timeRolu_T[iPart][iDet - 1][iCha] - timeRolu_L[iPart][iDet - 1][iCha];
                }

                if (iDet < 2)
                    fh_rolu_tot->Fill(iCha + 1, totRolu[iPart][iDet - 1][iCha]);
                if (iDet > 1)
                    fh_rolu_tot->Fill(iCha + 5, totRolu[iPart][iDet - 1][iCha]);
            }

            if (!calData)
            {
                cout << "Rolu !calData" << endl;
                continue; // can this happen?
            }
        }
    }

    //----------------------------------------------------------------------
    // Fiber0 detectors
    //----------------------------------------------------------------------
    if (fMappedItems_fi0)
    {
        Int_t mult_fi0 = 0;
        Int_t iFib = 0;
        Int_t iFib2 = 0;

        auto det = fMappedItems_fi0;
        Int_t nMapped = det->GetEntriesFast();
        for (Int_t imapped = 0; imapped < nMapped; imapped++)
        {
            auto mapped = (R3BBunchedFiberMappedData const*)det->At(imapped);
            if (!mapped)
                continue; // should not happen
            if (mapped->IsMAPMTTrigger())
            {
                triggerCountsFi0++;
            }
            if (!mapped->IsMAPMT())
            {
                continue;
            }
            if (mapped->IsLeading())
            {
                iFib = mapped->GetChannel(); // 1..n
                mult_fi0++;
                fh_fi0_fiber->Fill(iFib);
            }
        }
        // if (mult_fi0_0 > 0)
        {
            totalFi0 += mult_fi0;
            fh_fi0_mult->Fill(mult_fi0);
        }
        // Cal Items
        if (fCalItems_fi0)
        {
        }

        // Hit Items
        Double_t tmin = -15;
        Double_t tmax = 15;
        if (fHitItems_fi0)
        {
            Int_t multi[1024] = { 0 };
            Int_t nHits = fHitItems_fi0->GetEntriesFast();
            Double_t xpos = 0. / 0.;
            Double_t ypos = 0. / 0.;
            Double_t tfib = 0. / 0., tof_fib = 0. / 0.;
            Double_t randx;
            Double_t fib_ave = 0.;
            Double_t tot_sum = 0.;
            Double_t m = 0.;
            Int_t fib_old = 0;

            // cout << "new event: " << nHits << "  " << endl;

            Double_t tMAPMT = 0. / 0.;
            Double_t tMAPMT2 = 0. / 0.;
            Double_t tSPMT1 = 0. / 0.;
            Double_t tSPMT2 = 0. / 0.;
            Double_t tot = 0. / 0.;
            Double_t tot2 = 0. / 0.;

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
                tot = hit->GetEloss() - 200. / 15.;
                fh_fi0_Tot->Fill(iFib, tot);
                fh_fi0_Time->Fill(iFib, tMAPMT);
                // fiber correlation plot. Loop again over all fibers
                Int_t mult = 0;
                /*
                for (Int_t jhit = 0; jhit < nHits; jhit++)
                {
                    R3BBunchedFiberHitData* hit2 = (R3BBunchedFiberHitData*)fHitItems_fi0->At(jhit);
                    if (!hit2)
                        continue;

                    iFib2 = hit2->GetFiberId(); // 1..
                    tot2 = hit2->GetEloss();
                    tMAPMT2 = hit2->GetTime(); //+ 5444.;

                    if (iFib != iFib2)
                    {
                        fh_fi0_cor->Fill(iFib, iFib2);
                        fh_fi0_tot2_tot1->Fill(tot, tot2);
                    }
                    if (tMAPMT2 < tmin || tMAPMT2 > tmax)
                    {
                        mult++;
                    }
                }
*/
                // cuts ***********************************
                // if not in the right time window, go to next
                if (tMAPMT < tmin || tMAPMT > tmax)
                {
                    continue;
                }

                if (tot < 0 || tot > 2500)
                {
                    continue;
                }

                if (mult < 1)
                {
                    // continue;
                }
                // cout << "Fiber: " << iFib << " tot: " << tot << endl;
                if (ihit == 0)
                {
                    fib_ave = (float)iFib;
                    tot_sum = tot;
                    m = 1.;
                    fib_old = iFib;
                }
                Double_t diff = 3.1;
                if (iFib - fib_old < diff && ihit > 0)
                {
                    fib_ave += (float)iFib;
                    tot_sum += tot;
                    m += 1.;
                    fib_old = iFib;
                }

                if (iFib - fib_old > diff)
                {
                    // plot average and reset it
                    // cout << "fill: " << fib_ave << " tot sum: " << tot_sum << endl;
                    Fib0Hits.push_back(FibHit(fib_ave / m, tot_sum));

                    fh_fi0_fiber_ave->Fill(fib_ave / m);
                    fh_fi0_Tot_ave->Fill(fib_ave / m, tot_sum);
                    fib_ave = (float)iFib;
                    tot_sum = tot;
                    m = 1.;
                    fib_old = iFib;
                }
                if (ihit + 1 == nHits)
                {
                    fh_fi0_fiber_ave->Fill(fib_ave / m);
                    fh_fi0_Tot_ave->Fill(fib_ave / m, tot_sum);
                }
                // cout << "average: " << fib_ave / m << " tot sum: " << tot_sum << endl;
                //  cout << "Test: " << tSPMT1 << IS_NAN(tSPMT1) << endl;
                xpos = hit->GetX();
                ypos = dtime;
                multi[iFib - 1]++;
                fh_fi0_Tot_mc->Fill(iFib, tot);
                fh_fi0_Time_mc->Fill(iFib, tMAPMT);
                fh_fi0_fiber_mc->Fill(iFib);

                if (nHits > 0 && nHits < 3)
                {
                    // cout << "Fib0: fiber = " << iFib << endl;
                }

            } // end for(ihit)

            Int_t multi_fi0 = 0;
            for (Int_t i = 0; i < 1024; i++)
            {
                multi_fi0 += multi[i];
            }
            if (multi_fi0 > 0)
            {
                fh_fi0_mult_mc->Fill(multi_fi0);
                detectedFi0++;
            }
            fh_fi0_eff_vs_time->Fill(triggerCountsFi0, (float)detectedFi0 / (float)triggerCountsFi0 * 100.);
        }
    }

    //----------------------------------------------------------------------
    // Fiber60 detectors
    //----------------------------------------------------------------------
    if (fMappedItems_fi60)
    {
        Int_t mult_fi60 = 0;
        Int_t iFib = 0;
        Int_t iFib2 = 0;

        auto det = fMappedItems_fi60;
        Int_t nMapped = det->GetEntriesFast();
        for (Int_t imapped = 0; imapped < nMapped; imapped++)
        {
            auto mapped = (R3BBunchedFiberMappedData const*)det->At(imapped);
            if (!mapped)
                continue; // should not happen
            if (mapped->IsMAPMTTrigger())
            {
                triggerCountsFi60++;
            }
            if (!mapped->IsMAPMT())
            {
                continue;
            }
            if (mapped->IsLeading())
            {
                iFib = mapped->GetChannel(); // 1..n
                mult_fi60++;
                fh_fi60_fiber->Fill(iFib);
            }
        }
        // if (mult_fi0_0 > 0)
        {
            totalFi60 += mult_fi60;
            fh_fi60_mult->Fill(mult_fi60);
        }
        // Cal Items
        if (fCalItems_fi60)
        {
        }

        // Hit Items
        Double_t tmin = -15;
        Double_t tmax = 15;
        if (fHitItems_fi60)
        {
            Int_t multi[1024] = { 0 };
            Int_t nHits = fHitItems_fi60->GetEntriesFast();
            Double_t xpos = 0. / 0.;
            Double_t ypos = 0. / 0.;
            Double_t tfib = 0. / 0., tof_fib = 0. / 0.;
            Double_t randx;
            Double_t fib_ave = 0.;
            Double_t tot_sum = 0.;
            Double_t m = 0.;
            Int_t fib_old = 0;

            // cout << "new event: " << nHits << "  " << endl;

            Double_t tMAPMT = 0. / 0.;
            Double_t tMAPMT2 = 0. / 0.;
            Double_t tSPMT1 = 0. / 0.;
            Double_t tSPMT2 = 0. / 0.;
            Double_t tot = 0. / 0.;
            Double_t tot2 = 0. / 0.;

            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                tMAPMT = 0. / 0.;
                tot = 0. / 0.;
                Double_t dtime = 0. / 0.;
                R3BBunchedFiberHitData* hit = (R3BBunchedFiberHitData*)fHitItems_fi60->At(ihit);
                if (!hit)
                    continue;

                iFib = hit->GetFiberId(); // 1..
                tMAPMT = hit->GetTime();  //+ 5444.;
                tot = hit->GetEloss() - 200. / 15.;
                fh_fi60_Tot->Fill(iFib, tot);
                fh_fi60_Time->Fill(iFib, tMAPMT);

                // fiber correlation plot. Loop again over all fibers
                Int_t mult = 0;
                /*
                for (Int_t jhit = 0; jhit < nHits; jhit++)
                {
                    R3BBunchedFiberHitData* hit2 = (R3BBunchedFiberHitData*)fHitItems_fi60->At(jhit);
                    if (!hit2)
                        continue;

                    iFib2 = hit2->GetFiberId(); // 1..
                    tot2 = hit2->GetEloss();
                    tMAPMT2 = hit2->GetTime(); //+ 5444.;

                    if (iFib != iFib2)
                    {
                        fh_fi60_cor->Fill(iFib, iFib2);
                        fh_fi60_tot2_tot1->Fill(tot, tot2);
                    }
                    if (tMAPMT2 < tmin || tMAPMT2 > tmax)
                    {
                        mult++;
                    }
                }
                */
                // cuts ***********************************
                // if not in the right time window, go to next
                if (tMAPMT < tmin || tMAPMT > tmax)
                {
                    continue;
                }

                if (tot < 0 || tot > 2500)
                {
                    continue;
                }

                if (mult < 1)
                {
                    // continue;
                }
                // cout << "Fiber: " << iFib << " tot: " << tot << endl;
                if (ihit == 0)
                {
                    fib_ave = (float)iFib;
                    tot_sum = tot;
                    m = 1.;
                    fib_old = iFib;
                }
                Double_t diff = 3.1;
                if (iFib - fib_old < diff && ihit > 0)
                {
                    fib_ave += (float)iFib;
                    tot_sum += tot;
                    m += 1.;
                    fib_old = iFib;
                }

                if (iFib - fib_old > diff)
                {
                    // plot average and reset it
                    // cout << "fill: " << fib_ave << " tot sum: " << tot_sum << endl;
                    Fib60Hits.push_back(FibHit(fib_ave / m, tot_sum));
                    fh_fi60_fiber_ave->Fill(fib_ave / m);
                    fh_fi60_Tot_ave->Fill(fib_ave / m, tot_sum);
                    fib_ave = (float)iFib;
                    tot_sum = tot;
                    m = 1.;
                    fib_old = iFib;
                }
                if (ihit + 1 == nHits)
                {
                    fh_fi60_fiber_ave->Fill(fib_ave / m);
                    fh_fi60_Tot_ave->Fill(fib_ave / m, tot_sum);
                }
                // cout << "average: " << fib_ave / m << " tot sum: " << tot_sum << endl;
                //  cout << "Test: " << tSPMT1 << IS_NAN(tSPMT1) << endl;
                xpos = hit->GetX();
                ypos = dtime;
                multi[iFib - 1]++;
                fh_fi60_Tot_mc->Fill(iFib, tot);
                fh_fi60_Time_mc->Fill(iFib, tMAPMT);
                fh_fi60_fiber_mc->Fill(iFib);

                if (nHits > 0 && nHits < 3)
                {
                    // cout << "Fib0: fiber = " << iFib << endl;
                }

            } // end for(ihit)

            Int_t multi_fi60 = 0;
            for (Int_t i = 0; i < 1024; i++)
            {
                multi_fi60 += multi[i];
            }
            if (multi_fi60 > 0)
            {
                fh_fi60_mult_mc->Fill(multi_fi60);
                detectedFi60++;
            }
            fh_fi60_eff_vs_time->Fill(triggerCountsFi60, (float)detectedFi60 / (float)triggerCountsFi60 * 100.);
        }
    }

    //----------------------------------------------------------------------
    // Fiber61 detectors
    //----------------------------------------------------------------------
    if (fMappedItems_fi61)
    {
        Int_t mult_fi61 = 0;
        Int_t iFib = 0;
        Int_t iFib2 = 0;

        auto det = fMappedItems_fi61;
        Int_t nMapped = det->GetEntriesFast();
        for (Int_t imapped = 0; imapped < nMapped; imapped++)
        {
            auto mapped = (R3BBunchedFiberMappedData const*)det->At(imapped);
            if (!mapped)
                continue; // should not happen
            if (mapped->IsMAPMTTrigger())
            {
                triggerCountsFi61++;
            }
            if (!mapped->IsMAPMT())
            {
                continue;
            }
            if (mapped->IsLeading())
            {
                iFib = mapped->GetChannel(); // 1..n
                mult_fi61++;
                fh_fi61_fiber->Fill(iFib);
            }
        }
        // if (mult_fi0_0 > 0)
        {
            totalFi61 += mult_fi61;
            fh_fi61_mult->Fill(mult_fi61);
        }
        // Cal Items
        if (fCalItems_fi61)
        {
        }

        // Hit Items
        Double_t tmin = -15;
        Double_t tmax = 15;
        if (fHitItems_fi61)
        {
            Int_t multi[1024] = { 0 };
            Int_t nHits = fHitItems_fi61->GetEntriesFast();
            Double_t xpos = 0. / 0.;
            Double_t ypos = 0. / 0.;
            Double_t tfib = 0. / 0., tof_fib = 0. / 0.;
            Double_t randx;
            Double_t fib_ave = 0.;
            Double_t tot_sum = 0.;
            Double_t m = 0.;
            Int_t fib_old = 0;

            // cout << "new event: " << nHits << "  " << endl;

            Double_t tMAPMT = 0. / 0.;
            Double_t tMAPMT2 = 0. / 0.;
            Double_t tSPMT1 = 0. / 0.;
            Double_t tSPMT2 = 0. / 0.;
            Double_t tot = 0. / 0.;
            Double_t tot2 = 0. / 0.;

            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                tMAPMT = 0. / 0.;
                tot = 0. / 0.;
                Double_t dtime = 0. / 0.;
                R3BBunchedFiberHitData* hit = (R3BBunchedFiberHitData*)fHitItems_fi61->At(ihit);
                if (!hit)
                    continue;

                iFib = hit->GetFiberId(); // 1..
                tMAPMT = hit->GetTime();  //+ 5444.;
                tot = hit->GetEloss() - 200. / 15.;
                fh_fi61_Tot->Fill(iFib, tot);
                fh_fi61_Time->Fill(iFib, tMAPMT);
                // fiber correlation plot. Loop again over all fibers
                Int_t mult = 0;
                /*
                                for (Int_t jhit = 0; jhit < nHits; jhit++)
                                {
                                    R3BBunchedFiberHitData* hit2 = (R3BBunchedFiberHitData*)fHitItems_fi61->At(jhit);
                                    if (!hit2)
                                        continue;

                                    iFib2 = hit2->GetFiberId(); // 1..
                                    tot2 = hit2->GetEloss();
                                    tMAPMT2 = hit2->GetTime(); //+ 5444.;

                                    if (iFib != iFib2)
                                    {
                                        fh_fi61_cor->Fill(iFib, iFib2);
                                        fh_fi61_tot2_tot1->Fill(tot, tot2);
                                    }
                                    if (tMAPMT2 < tmin || tMAPMT2 > tmax)
                                    {
                                        mult++;
                                    }
                                }
                */
                // cuts ***********************************
                // if not in the right time window, go to next
                if (tMAPMT < tmin || tMAPMT > tmax)
                {
                    continue;
                }

                if (tot < 0 || tot > 2500)
                {
                    continue;
                }

                if (mult < 1)
                {
                    // continue;
                }
                // cout << "Fiber: " << iFib << " tot: " << tot << endl;
                if (ihit == 0)
                {
                    fib_ave = (float)iFib;
                    tot_sum = tot;
                    m = 1.;
                    fib_old = iFib;
                }
                Double_t diff = 3.1;
                if (iFib - fib_old < diff && ihit > 0)
                {
                    fib_ave += (float)iFib;
                    tot_sum += tot;
                    m += 1.;
                    fib_old = iFib;
                }

                if (iFib - fib_old > diff)
                {
                    // plot average and reset it
                    // cout << "fill: " << fib_ave << " tot sum: " << tot_sum << endl;
                    Fib61Hits.push_back(FibHit(fib_ave / m, tot_sum));
                    fh_fi61_fiber_ave->Fill(fib_ave / m);
                    fh_fi61_Tot_ave->Fill(fib_ave / m, tot_sum);
                    fib_ave = (float)iFib;
                    tot_sum = tot;
                    m = 1.;
                    fib_old = iFib;
                }
                if (ihit + 1 == nHits)
                {
                    fh_fi61_fiber_ave->Fill(fib_ave / m);
                    fh_fi61_Tot_ave->Fill(fib_ave / m, tot_sum);
                }
                // cout << "average: " << fib_ave / m << " tot sum: " << tot_sum << endl;
                //  cout << "Test: " << tSPMT1 << IS_NAN(tSPMT1) << endl;
                xpos = hit->GetX();
                ypos = dtime;
                multi[iFib - 1]++;
                fh_fi61_Tot_mc->Fill(iFib, tot);
                fh_fi61_Time_mc->Fill(iFib, tMAPMT);
                fh_fi61_fiber_mc->Fill(iFib);

                if (nHits > 0 && nHits < 3)
                {
                    // cout << "Fib0: fiber = " << iFib << endl;
                }

            } // end for(ihit)

            Int_t multi_fi61 = 0;
            for (Int_t i = 0; i < 1024; i++)
            {
                multi_fi61 += multi[i];
            }
            if (multi_fi61 > 0)
            {
                fh_fi61_mult_mc->Fill(multi_fi61);
                detectedFi61++;
            }
            fh_fi61_eff_vs_time->Fill(triggerCountsFi61, (float)detectedFi61 / (float)triggerCountsFi61 * 100.);
        }

        if (fHitItems_fi61 && fHitItems_fi61)
        {
            for (FibHit& HitFi60 : Fib60Hits)
            {
                for (FibHit& HitFi61 : Fib61Hits)
                {
                    fh_fi61_vs_fi60_tot->Fill(HitFi60.ToT, HitFi61.ToT);
                    fh_fi61_vs_fi60_fibers->Fill(HitFi60.X, HitFi61.X);
                }
            }

            for (FibHit& HitFi0 : Fib0Hits)
            {
                for (FibHit& HitFi60 : Fib60Hits)
                {
                    fh_fi60_vs_fi0_fibers->Fill(HitFi60.X, HitFi0.X);
                }
                for (FibHit& HitFi61 : Fib61Hits)
                {
                    fh_fi61_vs_fi0_fibers->Fill(HitFi61.X, HitFi0.X);
                }
            }
        }
    }
}

void R3BOnlineJuelich2023::FinishEvent()
{
    if (fMappedItems_fi0)
        fMappedItems_fi0->Clear();
    if (fCalItems_fi0)
        fCalItems_fi0->Clear();
    if (fHitItems_fi0)
        fHitItems_fi0->Clear();

    if (fMappedItems_fi60)
        fMappedItems_fi60->Clear();
    if (fCalItems_fi60)
        fCalItems_fi60->Clear();
    if (fHitItems_fi60)
        fHitItems_fi60->Clear();

    if (fMappedItems_fi61)
        fMappedItems_fi61->Clear();
    if (fCalItems_fi61)
        fCalItems_fi61->Clear();
    if (fHitItems_fi61)
        fHitItems_fi61->Clear();
    if (fHitItems_fi61 && fHitItems_fi60)
    {
        fh_fi61_vs_fi60_tot->Clear();
        fh_fi61_vs_fi60_fibers->Clear();
        fh_fi60_vs_fi0_fibers->Clear();
        fh_fi61_vs_fi0_fibers->Clear();
    }
}

void R3BOnlineJuelich2023::FinishTask()
{
    cout << "Total Fi60: " << totalFi60 << endl;
    cout << "trigger Fi60: " << triggerCountsFi60 << endl;
    cout << "Total Fi61: " << totalFi61 << endl;
    cout << "trigger Fi61: " << triggerCountsFi61 << endl;

    if (fMappedItems_rolu)
    {
        fh_rolu_channels->Write();
        fh_rolu_tot->Write();
    }

    if (fMappedItems_fi0)
    {
        fh_fi0_fiber->Write();
        fh_fi0_mult->Write();
        fh_fi0_Tot->Write();
        fh_fi0_Time->Write();
        fh_fi0_fiber_mc->Write();
        fh_fi0_fiber_ave->Write();
        fh_fi0_mult_mc->Write();
        fh_fi0_Tot_mc->Write();
        fh_fi0_Tot_ave->Write();
        fh_fi0_Time_mc->Write();
        fh_fi0_tot2_tot1->Write();
        fh_fi0_cor->Write();
        fh_fi0_eff->Write();
        fh_fi0_eff_vs_time->Write();
    }

    if (fMappedItems_fi60)
    {
        fh_fi60_fiber->Write();
        fh_fi60_mult->Write();
        fh_fi60_Tot->Write();
        fh_fi60_Time->Write();
        fh_fi60_fiber_mc->Write();
        fh_fi60_fiber_ave->Write();
        fh_fi60_mult_mc->Write();
        fh_fi60_Tot_mc->Write();
        fh_fi60_Tot_ave->Write();
        fh_fi60_Time_mc->Write();
        fh_fi60_tot2_tot1->Write();
        fh_fi60_cor->Write();
        fh_fi60_eff->Write();
        fh_fi60_eff_vs_time->Write();
    }
    if (fMappedItems_fi61)
    {
        fh_fi61_fiber->Write();
        fh_fi61_mult->Write();
        fh_fi61_Tot->Write();
        fh_fi61_Time->Write();
        fh_fi61_fiber_mc->Write();
        fh_fi61_fiber_ave->Write();
        fh_fi61_mult_mc->Write();
        fh_fi61_Tot_mc->Write();
        fh_fi61_Tot_ave->Write();
        fh_fi61_Time_mc->Write();
        fh_fi61_tot2_tot1->Write();
        fh_fi61_cor->Write();
        fh_fi61_eff->Write();
        fh_fi61_eff_vs_time->Write();
    }
    if (fHitItems_fi61 && fHitItems_fi60)
    {
        fh_fi61_vs_fi60_tot->Write();
        fh_fi61_vs_fi60_fibers->Write();
        fh_fi60_vs_fi0_fibers->Write();
        fh_fi61_vs_fi0_fibers->Write();
    }
}

ClassImp(R3BOnlineJuelich2023)
