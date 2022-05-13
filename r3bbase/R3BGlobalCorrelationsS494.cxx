// ------------------------------------------------------------
// -----                  R3BOnlineSpectra                -----
// -----          Created April 13th 2016 by M.Heil       -----
// -----          Modified 2021 by A.Kelic-Heil           -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BGlobalCorrelationsS494.h"

#include "R3BTofdCalData.h"
#include "R3BTofdHitData.h"
#include "R3BTofdMappedData.h"

#include "R3BBeamMonitorMappedData.h"
#include "R3BCalifaMappedData.h"

#include "R3BTofiCalData.h"
#include "R3BTofiHitData.h"
#include "R3BTofiMappedData.h"

#include "R3BFiberMAPMTCalData.h"
#include "R3BFiberMAPMTHitData.h"
#include "R3BFiberMAPMTMappedData.h"

#include "R3BRoluCalData.h"
#include "R3BRoluHitData.h"
#include "R3BRoluMappedData.h"

#include "R3BEventHeader.h"
#include "R3BSamplerMappedData.h"
#include "R3BTCalEngine.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRtdbRun.h"
#include "FairRunAna.h"
#include "FairRunIdGenerator.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TGaxis.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THttpServer.h"

#include "TClonesArray.h"
#include "TMath.h"
#include <TRandom3.h>
#include <TRandomGen.h>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

#include <boost/multi_array.hpp>
#include <cassert>
#include <cmath> // NAN

#define IS_NAN(x) TMath::IsNaN(x)
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
using namespace std;
namespace
{
    double c_period = 2048 * 5;
} // namespace
R3BGlobalCorrelationsS494::R3BGlobalCorrelationsS494()
    : FairTask("GlobalCorrelationsS494", 1)
    , fTrigger(-1)
    , fTpat1(-1)
    , fTpat2(-1)
    , fCuts(0)
    , fVeto(false)
    , ftofminFib2x(T_TOF_MIN)
    , ftofmaxFib2x(T_TOF_MAX)
    , ftofminFib3x(T_TOF_MIN)
    , ftofmaxFib3x(T_TOF_MAX)
    , ftofminTofi(T_TOF_MIN)
    , ftofmaxTofi(T_TOF_MAX)
    , fqtofdmin(0)
    , fqtofdmax(200)
    , fNEvents(0)
//    , fMappedItemsCalifa(NULL)

{
}

R3BGlobalCorrelationsS494::R3BGlobalCorrelationsS494(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat1(-1)
    , fTpat2(-1)
    , fCuts(0)
    , fVeto(false)
    , ftofminFib2x(T_TOF_MIN)
    , ftofmaxFib2x(T_TOF_MAX)
    , ftofminFib3x(T_TOF_MIN)
    , ftofmaxFib3x(T_TOF_MAX)
    , ftofminTofi(T_TOF_MIN)
    , ftofmaxTofi(T_TOF_MAX)
    , fqtofdmin(0)
    , fqtofdmax(200)
    , fNEvents(0)
//    , fMappedItemsCalifa(NULL)
{
}

R3BGlobalCorrelationsS494::~R3BGlobalCorrelationsS494() {}

InitStatus R3BGlobalCorrelationsS494::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(INFO) << "R3BGlobalCorrelationsS494::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline* run = FairRunOnline::Instance();
    //   run->GetHttpServer()->Register("/Tasks", this);

    // Get objects for detectors on all levels

    assert(DET_MAX + 1 == sizeof(fDetectorNames) / sizeof(fDetectorNames[0]));
    cout << " I HAVE FOUND " << DET_MAX + 1 << " DETECTORS" << endl;
    printf("Have %d fiber detectors.\n", NOF_FIB_DET);
    for (int det = 0; det < DET_MAX; det++)
    {

        fMappedItems.push_back((TClonesArray*)mgr->GetObject(Form("%sMapped", fDetectorNames[det])));
        if (NULL == fMappedItems.at(det))
        {
            printf("Could not find mapped data for '%s'.\n", fDetectorNames[det]);
        }

        if (det == 7)
            maxevent = mgr->CheckMaxEventNo();
        fCalItems.push_back((TClonesArray*)mgr->GetObject(Form("%sCal", fDetectorNames[det])));
        if (NULL == fCalItems.at(det))
        {
            printf("Could not find Cal data for '%s'.\n", fDetectorNames[det]);
        }

        fHitItems.push_back((TClonesArray*)mgr->GetObject(Form("%sHit", fDetectorNames[det])));
        if (NULL == fHitItems.at(det))
        {
            printf("Could not find hit data for '%s'.\n", fDetectorNames[det]);
        }
    }
    // get access to mapped data of the CALIFA
    //    fMappedItemsCalifa = (TClonesArray*)mgr->GetObject("CalifaMappedData");
    //    if (!fMappedItemsCalifa)
    //    {
    //        LOG(WARNING) << "R3BOnlineSpectra: CalifaMappedData not found";
    //    }

    cout << "*** In init, before creating histogramms ***" << endl;

    //-----------------------------------------------------------------------
    // Fiber Detectors 1-NOF_FIB_DET

    char canvName[255];
    UInt_t Nmax = 1e7;

    Double_t fexp = float(fsens_SEE + 9);
    Double_t fpow = float(pow(10., fexp));
    calib_SEE = 1; // 104457.9 * fpow;
    Int_t Nbin_bmon = reset_time / read_time;
    fh_SEE_spill = new TH1F("SEE_spill", "SEE particles rate in kHz", Nbin_bmon, 0, reset_time);
    fh_SEE_spill->GetXaxis()->SetTitle("time / sec");
    fh_SEE_spill->GetYaxis()->SetTitle("Particles / kHz");

    fh_IC_spill = new TH1F("IC_spill", "IC particles rate in kHz ", Nbin_bmon, 0, reset_time);
    fh_IC_spill->GetXaxis()->SetTitle("time / sec");
    fh_IC_spill->GetYaxis()->SetTitle("Particles / kHz");

    fh_IC_SEE = new TH1F("IC_vs_SEE", "IC vs SEE ", 10000, 0, 10000);
    fh_IC_SEE->GetYaxis()->SetTitle("SEE particles/spill");
    fh_IC_SEE->GetXaxis()->SetTitle("IC counts /spill");

    fh_Tpat = new TH1F("Tpat", "Tpat", 20, 0, 20);
    fh_Tpat->GetXaxis()->SetTitle("Tpat value");

    fh_Trigger = new TH1F("Trigger", "Trigger all", 20, 0, 20);
    fh_Trigger->GetXaxis()->SetTitle("Trigger value");

    TCanvas* cbmon = new TCanvas("Beam_Monitor", "Beam Monitors", 820, 10, 900, 300);
    cbmon->Divide(2, 1);
    cbmon->cd(1);
    // fh_SEE_spill->Draw("hist");
    fh_Trigger->Draw();
    cbmon->cd(2);
    fh_Tpat->Draw();
    // fh_IC_spill->Draw("hist");
    // cbmon->cd(3);
    // fh_IC_SEE->SetMarkerStyle(21);
    // fh_IC_SEE->Draw("hist p");

    cbmon->cd(0);

    //    run->AddObject(cbmon);

    //  run->GetHttpServer()->RegisterCommand("Reset_BMON", Form("/Tasks/%s/->Reset_BMON_Histo()", GetName()));

    // ToFD
    TCanvas* cTofd = new TCanvas("Tofd", "TOFD", 10, 10, 1000, 1200);
    cTofd->Divide(5, 4);

    fh_ztofd = new TH2F("Z_vs_y_tofd", "Z vs y toFD", 1600, -80., 80, 240, 0., 12.);
    fh_ztofd->GetXaxis()->SetTitle("y / cm");
    fh_ztofd->GetYaxis()->SetTitle("Z");

    fh_ztofd_ac = new TH2F("Z_vs_y_tofd_ac", "Z vs y toFD ac", 1600, -80., 80, 240, 0., 12.);
    fh_ztofd_ac->GetXaxis()->SetTitle("y / cm");
    fh_ztofd_ac->GetYaxis()->SetTitle("Z");

    fh_xy_tofd = new TH2F("yToFD_vs_xToFD", "yToFD vs xToFD", 1600, -80., 80., 1600, -80., 80);
    fh_xy_tofd->GetXaxis()->SetTitle("xToFD / cm ");
    fh_xy_tofd->GetYaxis()->SetTitle("yToFD / cm ");

    for (Int_t i = 0; i < 2; i++)
    {
        fh_time_tofd[i] = new TH1F(Form("timeToFD%d", i + 1), Form("TimeToFD%d", i + 1), 2000, -1000., 1000.);
        fh_time_tofd[i]->GetXaxis()->SetTitle("time / ns");
        fh_time_tofd[i]->GetYaxis()->SetTitle("counts");
    }

    fh_Z_vs_x = new TH2F("Z_vs_x", "Z vs x ", 1600, -80., 80, 240, 0., 12.);
    fh_Z_vs_x->GetXaxis()->SetTitle("x / cm");
    fh_Z_vs_x->GetYaxis()->SetTitle("Z");

    fh_Z_vs_time = new TH2F("Z_vs_time", "Z vs time ", 2000, -1000., 1000, 240, 0., 12.);
    fh_Z_vs_time->GetXaxis()->SetTitle("time / ns");
    fh_Z_vs_time->GetYaxis()->SetTitle("Z");

    fh_Z_vs_dthit = new TH2F("Z_vs_dthit", "Z vs dthit", 2400, -400., 800, 480, -12, 12);
    fh_Z_vs_dthit->GetXaxis()->SetTitle("time between two hits / ns");
    fh_Z_vs_dthit->GetYaxis()->SetTitle("dZ two hits");

    fh_dqvsdt_Tofd = new TH2F("tl_tr_Tofd", "time_r-time_l vs dQ ToFD", 2000, -1000., 1000., 480, -12, 12);
    fh_dqvsdt_Tofd->GetXaxis()->SetTitle("tr-tl ToFD / ns");
    fh_dqvsdt_Tofd->GetYaxis()->SetTitle("Zr-Zl");

    fh_ql_vs_qr_Tofd = new TH2F("Zl_vs_Zr_ToFD", "Zl vs Zr ToFD ", 240, 0., 12., 240, 0., 12.);
    fh_ql_vs_qr_Tofd->GetXaxis()->SetTitle("Zright");
    fh_ql_vs_qr_Tofd->GetYaxis()->SetTitle("Zleft");

    fh_xl_vs_xr_Tofd = new TH2F("xl_vs_xr_ToFD", "xl vs xr ToFD ", 1600, -80., 80, 1600, -80., 80.);
    fh_xl_vs_xr_Tofd->GetXaxis()->SetTitle("xright / cm");
    fh_xl_vs_xr_Tofd->GetYaxis()->SetTitle("xleft / cm");

    fh_xy_target = new TH2F("xtarget_vs_ytarget", "xtarget vs ytarget", 1200, -6., 6., 1200, -6., 6);
    fh_xy_target->GetXaxis()->SetTitle("xtarget / cm ");
    fh_xy_target->GetYaxis()->SetTitle("ytarget / cm ");

    fh_ql_vs_qr_Tofd_Zsum8 = new TH2F("Zl_vs_Zr_ToFD_Zsum8", "Zl vs Zr for Zl+Zr=8 ToFD ", 240, 0., 12., 240, 0., 12.);
    fh_ql_vs_qr_Tofd_Zsum8->GetXaxis()->SetTitle("Zright");
    fh_ql_vs_qr_Tofd_Zsum8->GetYaxis()->SetTitle("Zleft");

    fh_dt_Tofd_Zsum8 =
        new TH2F("tl_tr_Tofd_Zsum8", "time_r-time_l for Zl+Zr=8 ToFD", 2000, -1000., 1000., 480, -12, 12);
    fh_dt_Tofd_Zsum8->GetXaxis()->SetTitle("tr-tl ToFD / ns");
    fh_dt_Tofd_Zsum8->GetYaxis()->SetTitle("Zr-Zl");

    fh_xl_vs_xr_Tofd_Zsum8 =
        new TH2F("xl_vs_xr_ToFD_Zsum8", "xl vs xr for Zl+Zr=8 ToFD ", 1600, -80., 80, 1600, -80., 80.);
    fh_xl_vs_xr_Tofd_Zsum8->GetXaxis()->SetTitle("xright / cm");
    fh_xl_vs_xr_Tofd_Zsum8->GetYaxis()->SetTitle("xleft / cm");

    fh_xy_target_Zsum8 =
        new TH2F("xtarget_vs_ytarget_Zsum8", "xtarget vs ytarget for Z1+Z2=8", 1200, -6., 6., 1200, -6., 6);
    fh_xy_target_Zsum8->GetXaxis()->SetTitle("xtarget / cm ");
    fh_xy_target_Zsum8->GetYaxis()->SetTitle("ytarget / cm ");

    fh_qplane1_vs_qplane2_l =
        new TH2F("fh_qplane1_vs_qplane2_l", "Qplane1 vs Qplane2 leftside", 240, 0., 12., 240, 0., 12.);
    fh_qplane1_vs_qplane2_l->GetXaxis()->SetTitle("Zleft plane 1");
    fh_qplane1_vs_qplane2_l->GetYaxis()->SetTitle("Zleft plane 2");

    fh_qplane1_vs_qplane2_r =
        new TH2F("fh_qplane1_vs_qplane2_r", "Qplane1 vs Qplane2 rightside", 240, 0., 12., 240, 0., 12.);
    fh_qplane1_vs_qplane2_r->GetXaxis()->SetTitle("Zright plane 1");
    fh_qplane1_vs_qplane2_r->GetYaxis()->SetTitle("Zright plane 2");

    fh_xr_Tofd = new TH2F("dxr_vs_dtr_ToFD", "tr1-tr2 vs xr1-xr2 ToFD ", 2400, -60., 60, 500, -100., 100);
    fh_xr_Tofd->GetXaxis()->SetTitle("xright2 -xright1 / cm");
    fh_xr_Tofd->GetYaxis()->SetTitle("tright2 - tright1 / ns");

    fh_xl_Tofd = new TH2F("dxl_vs_dtl_ToFD", "tl1-tl2 vs xl1-xl2", 2400, -60., 60, 500, -100., 100);
    fh_xl_Tofd->GetXaxis()->SetTitle("xleft2 -xleft1  / cm");
    fh_xl_Tofd->GetYaxis()->SetTitle("tleft2 - tleft1 / ns");

    fh_qr_Tofd = new TH2F("dqr_vs_dtr_ToFD", "qr1-qr2 vs tr1-tr2 ", 960, -12, 12, 500, -100., 100);
    fh_qr_Tofd->GetXaxis()->SetTitle("qright2 - qright1 ");
    fh_qr_Tofd->GetYaxis()->SetTitle("tright2 - tright1 / ns");

    fh_ql_Tofd = new TH2F("dql_vs_dtl_ToFD", "ql1-ql2 vs tl1-tl2", 960, -12, 12, 500, -100., 100);
    fh_ql_Tofd->GetXaxis()->SetTitle("qleft2 - qleft1");
    fh_ql_Tofd->GetYaxis()->SetTitle("tleft2 - tleft1 / ns ");

    fh_q_vs_tofFib23aTofd = new TH2F("Z_vs_tofTofdFib23a", "Z vs tof Tofd-Fib23a ", 200, -1000., 1000, 24, 0., 12.);
    fh_q_vs_tofFib23aTofd->GetXaxis()->SetTitle("time / ns");
    fh_q_vs_tofFib23aTofd->GetYaxis()->SetTitle("Z");

    cTofd->cd(1);
    gPad->SetLogz();
    fh_xy_tofd->Draw("colz");
    cTofd->cd(2);
    gPad->SetLogy();
    fh_time_tofd[0]->Draw();
    cTofd->cd(3);
    gPad->SetLogy();
    fh_time_tofd[1]->Draw();
    cTofd->cd(4);
    gPad->SetLogz();
    fh_ztofd->Draw("colz");
    cTofd->cd(5);
    gPad->SetLogz();
    fh_Z_vs_x->Draw("colz");
    cTofd->cd(6);
    gPad->SetLogz();
    fh_Z_vs_time->Draw("colz");
    cTofd->cd(7);
    gPad->SetLogz();
    fh_Z_vs_dthit->Draw("colz");
    cTofd->cd(8);
    gPad->SetLogz();
    fh_dqvsdt_Tofd->Draw("colz");
    cTofd->cd(9);
    gPad->SetLogz();
    fh_ql_vs_qr_Tofd->Draw("colz");
    cTofd->cd(10);
    gPad->SetLogz();
    fh_xl_vs_xr_Tofd->Draw("colz");
    cTofd->cd(11);
    gPad->SetLogz();
    fh_xy_target->Draw("colz");
    cTofd->cd(12);
    gPad->SetLogz();
    fh_ql_vs_qr_Tofd_Zsum8->Draw("colz");
    cTofd->cd(13);
    gPad->SetLogz();
    fh_dt_Tofd_Zsum8->Draw("colz");
    cTofd->cd(14);
    gPad->SetLogz();
    fh_xl_vs_xr_Tofd_Zsum8->Draw("colz");
    cTofd->cd(15);
    gPad->SetLogz();
    fh_xy_target_Zsum8->Draw("colz");
    cTofd->cd(16);
    gPad->SetLogz();
    fh_q_vs_tofFib23aTofd->Draw("colz");
    cTofd->cd(17);
    gPad->SetLogz();
    fh_qplane1_vs_qplane2_l->Draw("colz");
    cTofd->cd(18);
    gPad->SetLogz();
    fh_qplane1_vs_qplane2_r->Draw("colz");
    cTofd->cd(19);
    gPad->SetLogz();
    fh_ztofd_ac->Draw("colz");
    cTofd->cd(0);

    // Fibers

    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        // cout<<"In the loop for ifibcout= "<<ifibcount<<endl;

        const char* detName;
        const char* detName2;
        detName = fDetectorNames[DET_FI_FIRST + ifibcount];

        fh_Fib_ToF[ifibcount] =
            new TH1F(Form("%s_tof", detName), Form("%s ToF Tofd to Fiber", detName), 4000, -2000., 2000.);
        fh_Fib_ToF[ifibcount]->GetXaxis()->SetTitle("ToF / ns");
        fh_Fib_ToF[ifibcount]->GetYaxis()->SetTitle("counts");

        if (ifibcount == 0)
            fh_xFib_vs_xTofd[ifibcount] = new TH2F(
                Form("%s_xToFD_vs_xfib", detName), Form("%s xfib vs xToFD ", detName), 480, -60., 60., 120, -6., 6);
        else if (ifibcount == 1)
            fh_xFib_vs_xTofd[ifibcount] = new TH2F(
                Form("%s_xToFD_vs_xfib", detName), Form("%s xfib vs xToFD ", detName), 480, -60., 60., 72, -6., 6);
        else
            fh_xFib_vs_xTofd[ifibcount] = new TH2F(
                Form("%s_xToFD_vs_xfib", detName), Form("%s xfib vs xToFD ", detName), 480, -60., 60., 600, -30., 30);
        fh_xFib_vs_xTofd[ifibcount]->GetXaxis()->SetTitle("x ToFD / cm");
        fh_xFib_vs_xTofd[ifibcount]->GetYaxis()->SetTitle("x Fib / cm");

        if (ifibcount == 0)
            fh_yFib_vs_yTofd[ifibcount] = new TH2F(
                Form("%s_yToFD_vs_yfib", detName), Form("%s yfib vs yToFD", detName), 640, -80., 80., 72, -6., 6);
        else if (ifibcount == 1)
            fh_yFib_vs_yTofd[ifibcount] = new TH2F(
                Form("%s_yToFD_vs_yfib", detName), Form("%s yfib vs yToFD", detName), 640, -80., 80., 120, -6., 6);
        else
            fh_yFib_vs_yTofd[ifibcount] = new TH2F(
                Form("%s_yToFD_vs_yfib", detName), Form("%s yfib vs yToFD", detName), 640, -80., 80., 72, -30., 30);
        fh_yFib_vs_yTofd[ifibcount]->GetXaxis()->SetTitle("y ToFD / cm");
        fh_yFib_vs_yTofd[ifibcount]->GetYaxis()->SetTitle("y Fib / cm");

        fh_qFib_vs_qTofd[ifibcount] =
            new TH2F(Form("%s_qToFD_vs_qfib", detName), Form("%s qfib vs qToFD", detName), 240, 0., 12., 100, 0, 50);
        fh_qFib_vs_qTofd[ifibcount]->GetXaxis()->SetTitle("q ToFD ");
        fh_qFib_vs_qTofd[ifibcount]->GetYaxis()->SetTitle("q Fib");

        fh_ToF_vs_Events[ifibcount] = new TH2F(Form("%s_tof_vs_events", detName),
                                               Form("%s ToF Tofd to Fiber vs event number", detName),
                                               10,
                                               0,
                                               Nmax,
                                               40,
                                               -2000,
                                               2000);
        fh_ToF_vs_Events[ifibcount]->GetYaxis()->SetTitle("ToF / ns");
        fh_ToF_vs_Events[ifibcount]->GetXaxis()->SetTitle("event number");

        fh_x_vs_Events[ifibcount] = new TH2F(Form("%s_x_vs_events", detName),
                                             Form("%s x vs event number", detName),
                                             100, // 10000,
                                             0,
                                             Nmax,
                                             600,
                                             -30,
                                             30);
        if (ifibcount == 1)
            fh_x_vs_Events[ifibcount]->GetYaxis()->SetTitle("y / cm");
        else
            fh_x_vs_Events[ifibcount]->GetYaxis()->SetTitle("x / cm");
        fh_x_vs_Events[ifibcount]->GetXaxis()->SetTitle("event number");

        fh_Fib_ToF_ac[ifibcount] =
            new TH1F(Form("%s_tof_ac", detName), Form("%s ToF Tofd to Fiber after cuts", detName), 4000, -2000., 2000.);
        fh_Fib_ToF_ac[ifibcount]->GetXaxis()->SetTitle("ToF / ns");
        fh_Fib_ToF_ac[ifibcount]->GetYaxis()->SetTitle("counts");

        if (ifibcount == 0)
            fh_xFib_vs_xTofd_ac[ifibcount] = new TH2F(Form("%s_xToFD_vs_xfib_ac", detName),
                                                      Form("%s xfib vs xToFD ac", detName),
                                                      480,
                                                      -60.,
                                                      60.,
                                                      120,
                                                      -6.,
                                                      6);
        else if (ifibcount == 1)
            fh_xFib_vs_xTofd_ac[ifibcount] = new TH2F(
                Form("%s_xToFD_vs_xfib_ac", detName), Form("%s xfib vs xToFD ac", detName), 480, -60., 60., 72, -6., 6);
        else
            fh_xFib_vs_xTofd_ac[ifibcount] = new TH2F(Form("%s_xToFD_vs_xfib_ac", detName),
                                                      Form("%s xfib vs xToFD ac", detName),
                                                      480,
                                                      -60.,
                                                      60.,
                                                      600,
                                                      -30.,
                                                      30);
        fh_xFib_vs_xTofd_ac[ifibcount]->GetXaxis()->SetTitle("x ToFD / cm");
        fh_xFib_vs_xTofd_ac[ifibcount]->GetYaxis()->SetTitle("x Fib / cm");

        if (ifibcount == 0)
            fh_yFib_vs_yTofd_ac[ifibcount] = new TH2F(
                Form("%s_yToFD_vs_yfib_ac", detName), Form("%s yfib vs yToFD ac", detName), 640, -80., 80., 72, -6., 6);
        else if (ifibcount == 1)
            fh_yFib_vs_yTofd_ac[ifibcount] = new TH2F(Form("%s_yToFD_vs_yfib_ac", detName),
                                                      Form("%s yfib vs yToFD ac", detName),
                                                      640,
                                                      -80.,
                                                      80.,
                                                      120,
                                                      -6.,
                                                      6);
        else
            fh_yFib_vs_yTofd_ac[ifibcount] = new TH2F(Form("%s_yToFD_vs_yfib_ac", detName),
                                                      Form("%s yfib vs yToFD ac", detName),
                                                      640,
                                                      -80.,
                                                      80.,
                                                      72,
                                                      -30.,
                                                      30);
        fh_yFib_vs_yTofd_ac[ifibcount]->GetXaxis()->SetTitle("y ToFD / cm");
        fh_yFib_vs_yTofd_ac[ifibcount]->GetYaxis()->SetTitle("y Fib / cm");

        fh_qFib_vs_qTofd_ac[ifibcount] = new TH2F(
            Form("%s_qToFD_vs_qfib_ac", detName), Form("%s qfib vs qToFD ac", detName), 960, 0., 12., 100, 0, 20);
        fh_qFib_vs_qTofd_ac[ifibcount]->GetXaxis()->SetTitle("q ToFD ");
        fh_qFib_vs_qTofd_ac[ifibcount]->SetTitle("q Fib");
    }
    TCanvas* cTofdFib = new TCanvas("TofdFib", "TOFDFib", 10, 10, 1800, 1000);
    cTofdFib->Divide(10, 6);

    for (Int_t ij = 0; ij < NOF_FIB_DET; ij++)
    {
        cTofdFib->cd(1 + 10 * ij);
        gPad->SetLogz();
        fh_Fib_ToF[ij]->Draw("colz");
        cTofdFib->cd(2 + 10 * ij);
        gPad->SetLogz();
        fh_xFib_vs_xTofd[ij]->Draw("colz");
        cTofdFib->cd(3 + 10 * ij);
        gPad->SetLogz();
        fh_yFib_vs_yTofd[ij]->Draw("colz");
        cTofdFib->cd(4 + 10 * ij);
        gPad->SetLogz();
        fh_qFib_vs_qTofd[ij]->Draw("colz");
        cTofdFib->cd(5 + 10 * ij);
        gPad->SetLogz();
        fh_ToF_vs_Events[ij]->Draw("colz");
        cTofdFib->cd(6 + 10 * ij);
        gPad->SetLogz();
        fh_x_vs_Events[ij]->Draw("colz");
        cTofdFib->cd(7 + 10 * ij);
        gPad->SetLogz();
        fh_Fib_ToF_ac[ij]->Draw("colz");
        cTofdFib->cd(8 + 10 * ij);
        gPad->SetLogz();
        fh_xFib_vs_xTofd_ac[ij]->Draw("colz");
        cTofdFib->cd(9 + 10 * ij);
        gPad->SetLogz();
        fh_yFib_vs_yTofd_ac[ij]->Draw("colz");
        cTofdFib->cd(10 + 10 * ij);
        gPad->SetLogz();
        fh_qFib_vs_qTofd_ac[ij]->Draw("colz");
    }
    cTofdFib->cd(0);

    fh_xFib31_vs_xFib33 = new TH2F("xFi31_vs_xFi33", "xFi31 vs xFi33", 600, -30., 30., 600, -30., 30);
    fh_xFib31_vs_xFib33->GetXaxis()->SetTitle("x Fi33 / cm ");
    fh_xFib31_vs_xFib33->GetYaxis()->SetTitle("x Fi31 / cm ");

    fh_yFib31_vs_yFib33 = new TH2F("yFi31_vs_yFi33", "yFi31 vs yFi33", 72, -30., 30., 72, -30., 30);
    fh_yFib31_vs_yFib33->GetXaxis()->SetTitle("y Fi33 / cm ");
    fh_yFib31_vs_yFib33->GetYaxis()->SetTitle("y Fi31 / cm ");

    fh_qFib31_vs_qFib33 = new TH2F("qFi31_vs_qFi33", "qFi31 vs qFi33", 100, 0, 50, 100, 0, 50);
    fh_qFib31_vs_qFib33->GetXaxis()->SetTitle("q Fi33 ");
    fh_qFib31_vs_qFib33->GetYaxis()->SetTitle("q Fi31 ");

    fh_tFib31_diff_tFib33 = new TH2F("tFi31_vs_tFi33", "tFi31 vs tFi33", 200, -100., 100., 200, -100., 100);
    fh_tFib31_diff_tFib33->GetXaxis()->SetTitle("t Fi33 / ns ");
    fh_tFib31_diff_tFib33->GetYaxis()->SetTitle("t Fi31 / ns ");

    fh_xFib31_vs_xFib33_ac = new TH2F("xFi31_vs_xFi33_ac", "xFi31 vs xFi33 ac", 600, -30., 30., 600, -30., 30);
    fh_xFib31_vs_xFib33_ac->GetXaxis()->SetTitle("x Fi33 / cm ");
    fh_xFib31_vs_xFib33_ac->GetYaxis()->SetTitle("x Fi31 / cm ");

    fh_yFib31_vs_yFib33_ac = new TH2F("yFi31_vs_yFi33_ac", "yFi31 vs yFi33 ac", 72, -30., 30., 72, -30., 30);
    fh_yFib31_vs_yFib33_ac->GetXaxis()->SetTitle("y Fi33 / cm ");
    fh_yFib31_vs_yFib33_ac->GetYaxis()->SetTitle("y Fi31 / cm ");

    fh_qFib31_vs_qFib33_ac = new TH2F("qFi31_vs_qFi33_ac", "qFi31 vs qFi33 ac", 100, 0, 50, 100, 0, 50);
    fh_qFib31_vs_qFib33_ac->GetXaxis()->SetTitle("q Fi33 ");
    fh_qFib31_vs_qFib33_ac->GetYaxis()->SetTitle("q Fi31 ");

    fh_tFib31_diff_tFib33_ac = new TH2F("tFi31_vs_tFi33_ac", "tFi31 vs tFi33 ac", 200, -100., 100., 200, -100., 100);
    fh_tFib31_diff_tFib33_ac->GetXaxis()->SetTitle("t Fi33 / ns ");
    fh_tFib31_diff_tFib33_ac->GetYaxis()->SetTitle("t Fi31 / ns ");

    fh_xFib23a_vs_xFib31 = new TH2F("xFi23a_vs_xFi31", "xFi23a vs xFi31", 120, -6., 6., 600, -30., 30);
    fh_xFib23a_vs_xFib31->GetYaxis()->SetTitle("x Fi33 / cm ");
    fh_xFib23a_vs_xFib31->GetXaxis()->SetTitle("x Fi23a / cm ");

    fh_yFib23a_vs_yFib31 = new TH2F("yFi23a_vs_yFi31", "yFi23a vs yFi31", 72, -6., 6., 72, -30., 30);
    fh_yFib23a_vs_yFib31->GetYaxis()->SetTitle("y Fi31 / cm ");
    fh_yFib23a_vs_yFib31->GetXaxis()->SetTitle("y Fi23a / cm ");

    fh_qFib23a_vs_qFib31 = new TH2F("qFi23a_vs_qFi31", "qFi23a vs qFi31", 100, 0, 50, 100, 0, 50);
    fh_qFib23a_vs_qFib31->GetYaxis()->SetTitle("q Fi31 ");
    fh_qFib23a_vs_qFib31->GetXaxis()->SetTitle("q Fi23a ");

    fh_tFib23a_diff_tFib31 = new TH2F("tFi23a_vs_tFi31", "tFi23a vs tFi31", 200, -100., 100., 200, -100., 100);
    fh_tFib23a_diff_tFib31->GetYaxis()->SetTitle("t Fi31 / ns ");
    fh_tFib23a_diff_tFib31->GetXaxis()->SetTitle("t Fi23a / ns ");

    fh_xFib23a_vs_xFib31_ac = new TH2F("xFi23a_vs_xFi31_ac", "xFi23a vs xFi31 ac", 120, -6., 6., 600, -30., 30);
    fh_xFib23a_vs_xFib31_ac->GetYaxis()->SetTitle("x Fi31 / cm ");
    fh_xFib23a_vs_xFib31_ac->GetXaxis()->SetTitle("x Fi23a / cm ");

    fh_yFib23a_vs_yFib31_ac = new TH2F("yFi23a_vs_yFi31_ac", "yFi23a vs yFi31 ac", 72, -6., 6., 72, -30., 30);
    fh_yFib23a_vs_yFib31_ac->GetYaxis()->SetTitle("y Fi31 / cm ");
    fh_yFib23a_vs_yFib31_ac->GetXaxis()->SetTitle("y Fi23a / cm ");

    fh_qFib23a_vs_qFib31_ac = new TH2F("qFi23a_vs_qFi31_ac", "qFi23a vs qFi31 ac", 100, 0, 50, 100, 0, 50);
    fh_qFib23a_vs_qFib31_ac->GetYaxis()->SetTitle("q Fi31 ");
    fh_qFib23a_vs_qFib31_ac->GetXaxis()->SetTitle("q Fi23a ");

    fh_tFib23a_diff_tFib31_ac = new TH2F("tFi23a_vs_tFi31_ac", "tFi23a vs tFi31 ac", 200, -100., 100., 200, -100., 100);
    fh_tFib23a_diff_tFib31_ac->GetYaxis()->SetTitle("t Fi31 / ns ");
    fh_tFib23a_diff_tFib31_ac->GetXaxis()->SetTitle("t Fi23a / ns ");

    fh_xFib23b_vs_xFib31 = new TH2F("xFi23b_vs_xFi31", "xFi23b vs xFi31", 120, -6., 6., 600, -30., 30);
    fh_xFib23b_vs_xFib31->GetYaxis()->SetTitle("x Fi31 / cm ");
    fh_xFib23b_vs_xFib31->GetXaxis()->SetTitle("x Fi23b / cm ");

    fh_yFib23b_vs_yFib31 = new TH2F("yFi23b_vs_yFi31", "yFi23b vs yFi31", 72, -6., 6., 72, -30., 30);
    fh_yFib23b_vs_yFib31->GetYaxis()->SetTitle("y Fi31 / cm ");
    fh_yFib23b_vs_yFib31->GetXaxis()->SetTitle("y Fi23b / cm ");

    fh_qFib23b_vs_qFib31 = new TH2F("qFi23b_vs_qFi31", "qFi23b vs qFi31", 100, 0, 50, 100, 0, 50);
    fh_qFib23b_vs_qFib31->GetYaxis()->SetTitle("q Fi31 ");
    fh_qFib23b_vs_qFib31->GetXaxis()->SetTitle("q Fi23b ");

    fh_tFib23b_diff_tFib31 = new TH2F("tFi23b_vs_tFi31", "tFi23b vs tFi31", 200, -100., 100., 200, -100., 100);
    fh_tFib23b_diff_tFib31->GetYaxis()->SetTitle("t Fi31 / ns ");
    fh_tFib23b_diff_tFib31->GetXaxis()->SetTitle("t Fi23b / ns ");

    fh_xFib23b_vs_xFib31_ac = new TH2F("xFi23b_vs_xFi31_ac", "xFi23b vs xFi31 ac", 120, -6., 6., 600, -30., 30);
    fh_xFib23b_vs_xFib31_ac->GetYaxis()->SetTitle("x Fi31 / cm ");
    fh_xFib23b_vs_xFib31_ac->GetXaxis()->SetTitle("x Fi23b / cm ");

    fh_yFib23b_vs_yFib31_ac = new TH2F("yFi23b_vs_yFi31_ac", "yFi23b vs yFi31 ac", 72, -6., 6., 72, -30., 30);
    fh_yFib23b_vs_yFib31_ac->GetYaxis()->SetTitle("y Fi31 / cm ");
    fh_yFib23b_vs_yFib31_ac->GetXaxis()->SetTitle("y Fi23b / cm ");

    fh_qFib23b_vs_qFib31_ac = new TH2F("qFi23b_vs_qFi31_ac", "qFi23b vs qFi31 ac", 100, 0, 50, 100, 0, 50);
    fh_qFib23b_vs_qFib31_ac->GetYaxis()->SetTitle("q Fi31 ");
    fh_qFib23b_vs_qFib31_ac->GetXaxis()->SetTitle("q Fi23b ");

    fh_tFib23b_diff_tFib31_ac = new TH2F("tFi23b_vs_tFi31_ac", "tFi23b vs tFi31 ac", 200, -100., 100., 200, -100., 100);
    fh_tFib23b_diff_tFib31_ac->GetYaxis()->SetTitle("t Fi31 / ns ");
    fh_tFib23b_diff_tFib31_ac->GetXaxis()->SetTitle("t Fi23b / ns ");

    fh_xFib30_vs_xFib32 = new TH2F("xFi30_vs_xFi32", "xFi30 vs xFi32", 600, -30., 30, 600, -30., 30.);
    fh_xFib30_vs_xFib32->GetXaxis()->SetTitle("x Fi32 / cm ");
    fh_xFib30_vs_xFib32->GetYaxis()->SetTitle("x Fi30 / cm ");

    fh_yFib30_vs_yFib32 = new TH2F("yFi30_vs_yFi32", "yFi30 vs yFi32", 72, -30., 30, 72, -30., 30.);
    fh_yFib30_vs_yFib32->GetXaxis()->SetTitle("y Fi32 / cm ");
    fh_yFib30_vs_yFib32->GetYaxis()->SetTitle("x Fi30 / cm ");

    fh_qFib30_vs_qFib32 = new TH2F("qFi30_vs_qFi32", "qFi30 vs qFi32", 100, 0, 50, 100, 0, 50);
    fh_qFib30_vs_qFib32->GetXaxis()->SetTitle("q Fi32");
    fh_qFib30_vs_qFib32->GetYaxis()->SetTitle("q Fi30 ");

    fh_tFib30_diff_tFib32 = new TH2F("tFi30_vs_tFi32", "tFi30 vs tFi32", 200, -100., 100., 200, -100., 100);
    fh_tFib30_diff_tFib32->GetXaxis()->SetTitle("t Fi32 / ns");
    fh_tFib30_diff_tFib32->GetYaxis()->SetTitle("t Fi30 / ns");

    fh_xFib30_vs_xFib32_ac = new TH2F("xFi30_vs_xFi32_ac", "xFi30 vs xFi32 ac", 600, -30., 30, 600, -30., 30.);
    fh_xFib30_vs_xFib32_ac->GetXaxis()->SetTitle("x Fi32 / cm ");
    fh_xFib30_vs_xFib32_ac->GetYaxis()->SetTitle("x Fi30 / cm ");

    fh_yFib30_vs_yFib32_ac = new TH2F("yFi30_vs_yFi32_ac", "yFi30 vs yFi32 ac", 72, -30., 30, 72, -30., 30.);
    fh_yFib30_vs_yFib32_ac->GetXaxis()->SetTitle("y Fi32 / cm ");
    fh_yFib30_vs_yFib32_ac->GetYaxis()->SetTitle("y Fi30 / cm ");

    fh_qFib30_vs_qFib32_ac = new TH2F("qFi30_vs_qFi32_ac", "qFi30 vs qFi32 ac", 100, 0, 50, 100, 0, 50);
    fh_qFib30_vs_qFib32_ac->GetXaxis()->SetTitle("q Fi32");
    fh_qFib30_vs_qFib32_ac->GetYaxis()->SetTitle("q Fi30 ");

    fh_tFib30_diff_tFib32_ac = new TH2F("tFi30_vs_tFi32_ac", "tFi30 vs tFi32 ac", 200, -100., 100., 200, -100., 100);
    fh_tFib30_diff_tFib32_ac->GetXaxis()->SetTitle("t Fi32 / ns");
    fh_tFib30_diff_tFib32_ac->GetYaxis()->SetTitle("t Fi30 / ns");

    fh_xFib23a_vs_xFib30 = new TH2F("xFi23a_vs_xFi30", "xFi23a vs xFi30", 120, -6., 6., 600, -30., 30);
    fh_xFib23a_vs_xFib30->GetYaxis()->SetTitle("x Fi30 / cm ");
    fh_xFib23a_vs_xFib30->GetXaxis()->SetTitle("x Fi23a / cm ");

    fh_yFib23a_vs_yFib30 = new TH2F("yFi23a_vs_yFi30", "yFi23a vs yFi30", 72, -6., 6., 72, -30., 30);
    fh_yFib23a_vs_yFib30->GetYaxis()->SetTitle("y Fi30 / cm ");
    fh_yFib23a_vs_yFib30->GetXaxis()->SetTitle("y Fi23a / cm ");

    fh_qFib23a_vs_qFib30 = new TH2F("qFi23a_vs_qFi30", "qFi23a vs qFi30", 100, 0, 50, 100, 0, 50);
    fh_qFib23a_vs_qFib30->GetYaxis()->SetTitle("q Fi30 ");
    fh_qFib23a_vs_qFib30->GetXaxis()->SetTitle("q Fi23a ");

    fh_tFib23a_diff_tFib30 = new TH2F("tFi23a_vs_tFi30", "tFi23a vs tFi30", 200, -100., 100., 200, -100., 100);
    fh_tFib23a_diff_tFib30->GetYaxis()->SetTitle("t Fi30 / ns ");
    fh_tFib23a_diff_tFib30->GetXaxis()->SetTitle("t Fi23a / ns ");

    fh_xFib23a_vs_xFib30_ac = new TH2F("xFi23a_vs_xFi30_ac", "xFi23a vs xFi30 ac", 120, -6., 6., 600, -30., 30);
    fh_xFib23a_vs_xFib30_ac->GetYaxis()->SetTitle("x Fi30 / cm ");
    fh_xFib23a_vs_xFib30_ac->GetXaxis()->SetTitle("x Fi23a / cm ");

    fh_yFib23a_vs_yFib30_ac = new TH2F("yFi23a_vs_yFi30_ac", "yFi23a vs yFi30 ac", 72, -6., 6., 72, -30., 30);
    fh_yFib23a_vs_yFib30_ac->GetYaxis()->SetTitle("y Fi30 / cm ");
    fh_yFib23a_vs_yFib30_ac->GetXaxis()->SetTitle("y Fi23a / cm ");

    fh_qFib23a_vs_qFib30_ac = new TH2F("qFi23a_vs_qFi30_ac", "qFi23a vs qFi30 ac", 100, 0, 50, 100, 0, 50);
    fh_qFib23a_vs_qFib30_ac->GetYaxis()->SetTitle("q Fi30 ");
    fh_qFib23a_vs_qFib30_ac->GetXaxis()->SetTitle("q Fi23a ");

    fh_tFib23a_diff_tFib30_ac = new TH2F("tFi23a_vs_tFi30_ac", "tFi23a vs tFi30 ac", 200, -100., 100., 200, -100., 100);
    fh_tFib23a_diff_tFib30_ac->GetYaxis()->SetTitle("t Fi30 / ns ");
    fh_tFib23a_diff_tFib30_ac->GetXaxis()->SetTitle("t Fi23a / ns ");

    fh_xFib23b_vs_xFib30 = new TH2F("xFi23b_vs_xFi30", "xFi23b vs xFi30", 120, -6., 6., 600, -30., 30);
    fh_xFib23b_vs_xFib30->GetYaxis()->SetTitle("x Fi30/ cm ");
    fh_xFib23b_vs_xFib30->GetXaxis()->SetTitle("x Fi23b / cm ");

    fh_yFib23b_vs_yFib30 = new TH2F("yFi23b_vs_yFi30", "yFi23b vs yFi30", 72, -6., 6., 72, -30., 30);
    fh_yFib23b_vs_yFib30->GetYaxis()->SetTitle("y Fi30 / cm ");
    fh_yFib23b_vs_yFib30->GetXaxis()->SetTitle("y Fi23b / cm ");

    fh_qFib23b_vs_qFib30 = new TH2F("qFi23b_vs_qFi30", "qFi23b vs qFi30", 100, 0, 50, 100, 0, 50);
    fh_qFib23b_vs_qFib30->GetYaxis()->SetTitle("q Fi30 ");
    fh_qFib23b_vs_qFib30->GetXaxis()->SetTitle("q Fi23b ");

    fh_tFib23b_diff_tFib30 = new TH2F("tFi23b_vs_tFi30", "tFi23b vs tFi30", 200, -100., 100., 200, -100., 100);
    fh_tFib23b_diff_tFib30->GetYaxis()->SetTitle("t Fi30 / ns ");
    fh_tFib23b_diff_tFib30->GetXaxis()->SetTitle("t Fi23b / ns ");

    fh_xFib23b_vs_xFib30_ac = new TH2F("xFi23b_vs_xFi30_ac", "xFi23b vs xFi30 ac", 120, -6., 6., 600, -30., 30);
    fh_xFib23b_vs_xFib30_ac->GetYaxis()->SetTitle("x Fi30 / cm ");
    fh_xFib23b_vs_xFib30_ac->GetXaxis()->SetTitle("x Fi23b / cm ");

    fh_yFib23b_vs_yFib30_ac = new TH2F("yFi23b_vs_yFi30_ac", "yFi23b vs yFi30 ac", 72, -6., 6., 72, -30., 30);
    fh_yFib23b_vs_yFib30_ac->GetYaxis()->SetTitle("y Fi30 / cm ");
    fh_yFib23b_vs_yFib30_ac->GetXaxis()->SetTitle("y Fi23b / cm ");

    fh_qFib23b_vs_qFib30_ac = new TH2F("qFi23b_vs_qFi30_ac", "qFi23b vs qFi30 ac", 100, 0, 50, 100, 0, 50);
    fh_qFib23b_vs_qFib30_ac->GetYaxis()->SetTitle("q Fi30 ");
    fh_qFib23b_vs_qFib30_ac->GetXaxis()->SetTitle("q Fi23b ");

    fh_tFib23b_diff_tFib30_ac = new TH2F("tFi23b_vs_tFi30_ac", "tFi23b vs tFi30 ac", 200, -100., 100., 200, -100., 100);
    fh_tFib23b_diff_tFib30_ac->GetYaxis()->SetTitle("t Fi30 / ns ");
    fh_tFib23b_diff_tFib30_ac->GetXaxis()->SetTitle("t Fi23b / ns ");

    //   cout<<"****** Finished with spectra def ******* "<<endl;
    //   run->AddObject(cFib);
    //  run->GetHttpServer()->RegisterCommand("Reset_Fib", Form("/Tasks/%s/->Reset_All()", GetName()));

    cout << "*** At the end of init **" << endl;

    return kSUCCESS;
}
/*
void R3BGlobalCorrelationsS494::Reset_BMON_Histo()
{
    fh_SEE_spill->Reset();
    fh_IC_spill->Reset();
    fh_IC_SEE->Reset();
    time_start = -1;
}
void R3BGlobalCorrelationsS494::Reset_All()
{
    for (Int_t i_FIB_DET = 0; i_FIB_DET < NOF_FIB_DET; i_FIB_DET++)
    {

            fh_Fibs_vs_Tofd_ac[i_FIB_DET]->Reset();
            fh_xy_Fib_ac[i_FIB_DET]->Reset();
            fh_Fib_ToF_ac[i_FIB_DET]->Reset();
            fh_ToT_Fib_ac[i_FIB_DET]->Reset();


            fh_xy_Fib[i_FIB_DET]->Reset();
            fh_Fib_ToF[i_FIB_DET]->Reset();
            fh_ToT_Fib[i_FIB_DET]->Reset();
            fh_Fibs_vs_Tofd[i_FIB_DET]->Reset();
            fh_ToF_vs_Events[i_FIB_DET]->Reset();

    }
    fh_xy_target_ac->Reset();
    fh_xy_target->Reset();
    fh_Z_vs_x->Reset();
    fh_ztofd->Reset();
    fh_ztofi->Reset();

    if (fHitItems.at(DET_TOFI))
    {
        fh_Tofi_ToF->Reset();

            fh_ToT_Tofi_ac->Reset();
            fh_xy_Tofi_ac->Reset();
            fh_ToT_Tofi->Reset();
          //  fh_xy_Tofi->Reset();

    }

    if (fCalItems.at(DET_ROLU))
    {
        for (Int_t i = 0; i < 2; i++)
        {
            if (fCuts)
                fh_ToT_Rolu_ac[i]->Reset();
            else
            {
                fh_ToT_Rolu[i]->Reset();
                fh_Rolu_ToF[i]->Reset();
            }
        }
    }
}
*/
void R3BGlobalCorrelationsS494::Exec(Option_t* option)
{

    Bool_t debug2 = false;
    Bool_t counter_reset = false;
    if (debug2)
        cout << "*** At the entrence of exec ***" << endl;

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    time = header->GetTimeStamp();
    //   if (time == 0 && time > 1.e19)
    //  return;

    fNEvents_total += 1;

    fNEvents += 1;
    if (fNEvents / 10000. == (int)fNEvents / 10000)
        std::cout << "\rEvents: " << fNEvents << " / " << maxevent << " (" << (int)(fNEvents * 100. / maxevent)
                  << " %) " << std::flush;

    if (debug2)
        cout << "*** In exec before starting analysis, fnEvent= " << fNEvents << endl;

    if (time_begin == -1 && time > 0 && time_end < 1)
    {
        time_begin = time;
    }

    if (time_start == -1 && time > 0)
    {
        time_start = time;
        fNEvents_start = fNEvents_total;
    }

    Bool_t newSpill = false;
    if (header->GetTrigger() == 12)
    {
        time_spill_start = time; // header->GetTimeStamp();    // spill start in nsec
        cout << "Spill start: " << double(time_spill_start - time_begin) / 1.e9 << " sec " << endl;
        // reset counters:
        see_spill = 0;
        ic_spill = 0;
        newSpill = true;
        spill_on = true;
        in_spill_off = 0;
        fNSpills += 1;
    }
    if (header->GetTrigger() == 13)
    {
        time_spill_end = time; // header->GetTimeStamp();    // spill end  in nsec
        cout << "Spill stop: " << double(time_spill_end - time_begin) / 1.e9 << " sec " << endl;
        spill_on = false;
    }
    if (header->GetTrigger() == 1)
        nAccepted += 1;
    fh_Trigger->Fill(header->GetTrigger());
    Int_t tpatbin;
    for (int i = 0; i < 16; i++)
    {
        tpatbin = (header->GetTpat() & (1 << i));
        if (tpatbin != 0)
            fh_Tpat->Fill(i + 1);
    }

    if (fMappedItems.at(DET_BMON) && fMappedItems.at(DET_BMON)->GetEntriesFast() > 0)
    {
        Bool_t spectra_clear = false;
        Double_t xtime = double(time - time_start) / 1.e9;
        // for reseting spectra
        Int_t icount = iclear_count * reset_time; // reset after reset_time (sec)

        if (xtime > reset_time)
        {
            time_clear = xtime;
            spectra_clear = true;
        }
        long long IC;
        long long SEETRAM;

        Bool_t bmon_read = false;

        auto det = fMappedItems.at(DET_BMON);
        Int_t nHitsbm = det->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nHitsbm; ihit++)
        {
            R3BBeamMonitorMappedData* hit = (R3BBeamMonitorMappedData*)det->At(ihit);
            if (!hit)
                continue;

            IC = hit->GetIC();           // negative values if offset not high enough
            SEETRAM = hit->GetSEETRAM(); // negative values if offset not high enough

            if (fNEvents_total == fNEvents_start)
            {
                see_mem = SEETRAM;
                ic_mem = IC;
                time_mem = time_start;
                see_first = SEETRAM;
                see_start = SEETRAM;
                ic_start = IC;
                time_prev_read = time_start;
            }
            if (newSpill == true)
            {
                seeCount = SEETRAM - see_first;
                nOffSpillParticle += (seeCount - seeLastSpill);
                seeLastSpill = seeCount;
                newSpill = false;
            }
            if (time > 0)
            {

                // Spectra below are filled every read_time (secs)
                if (time_to_read == 0 && (time - time_prev_read) >= read_time * 1000000000) // in nsec
                {
                    time_to_read = time;
                    bmon_read = true;
                }

                if (bmon_read)
                {
                    tdiff = double(time - time_mem) / 1.e9;
                    fNorm = 1.e-3 / (double(time - time_prev_read) / 1.e9); // kHz

                    // IC:
                    int yIC = (IC - ic_start);
                    int yIC_mem = (IC - ic_mem);
                    Double_t yIC_part = ((double)yIC_mem * fNorm) * calib_IC;
                    if (yIC_mem > 0 && yIC_mem_mem > 0)
                        fh_IC_spill->Fill(tdiff, yIC_part);
                    ic_mem = IC;

                    // SEETRAM:SEETRAM
                    int ySEE = (SEETRAM - see_start);
                    int ySEE_mem = (SEETRAM - see_mem);
                    Double_t ySEE_part = ((double)ySEE_mem * fNorm) * calib_SEE;
                    if (ySEE_mem > 0 && ySEE_mem_mem > 0)
                        fh_SEE_spill->Fill(tdiff, ySEE_part);
                    see_mem = SEETRAM;

                    // correlations:
                    if (spill_on)
                    {
                        if (ySEE_mem > 0 && ySEE_mem_mem > 0 && yIC_mem > 0 && yIC_mem_mem > 0)
                        {
                            see_spill += ySEE_part;
                            ic_spill += yIC_part;
                            nBeamParticle += ySEE_part * 1e3; // / spill_length;
                        }
                    }
                    if (!spill_on && time_spill_start > 0 && time_spill_end > 0)
                    {
                        in_spill_off += 1;
                        if (in_spill_off == 1)
                        {
                            Double_t spill_length = double(time_spill_end - time_spill_start) / 1.e9;
                            //  *1e3 while yDet_part is in kHz
                            ic_spill = ic_spill * 1e3;   // / spill_length;
                            see_spill = see_spill * 1e3; // / spill_length;
                            //     cout<<"Spill counts: "<<ic_spill<<", "<<see_spill<<", "<<spill_length<<endl;
                            fh_IC_SEE->Fill(ic_spill, see_spill);
                        }
                    }
                    // nOffSpillParticle += ySEE_part * 1e3; // / spill_length;
                    ySEE_mem_mem = ySEE_mem;
                    yIC_mem_mem = yIC_mem;

                    time_to_read = 0;
                    time_prev_read = time;
                    bmon_read = false;
                }

                if (spectra_clear)
                {
                    fh_IC_spill->Reset("ICESM");
                    fh_SEE_spill->Reset("ICESM");
                    time_mem = time;
                    time_clear = -1.;
                    time_start = -1;
                    iclear_count = iclear_count + 1;
                    spectra_clear = false;
                    see_start = SEETRAM;
                    ic_start = IC;
                }
            }
        }
    }

    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
    {
        counterWrongTrigger++;
        return;
    }
    // fTpat = 1-16; fTpat_bit = 0-15
    Int_t fTpat_bit1 = fTpat1 - 1;
    Int_t fTpat_bit2 = fTpat2 - 1;
    for (int i = 0; i < 16; i++)
    {
        tpatbin = (header->GetTpat() & (1 << i));
        if (tpatbin != 0 && (i < fTpat_bit1 || i > fTpat_bit2))
        {
            counterWrongTpat++;
            return;
        }
    }

    if (debug2)
        cout << "*** In exec after trig and tpat cond, fnEvents= " << fNEvents << endl;

    Int_t fibMaxFi30 = 0, fibMaxFi31 = 0, fibMaxFi32 = 0, fibMaxFi33 = 0, fibMaxFi23a = 0, fibMaxFi23b = 0,
          fibMaxTofi = 0;

    Int_t summ_tofd = 0;
    Int_t summ_tofdr = 0;
    Int_t summ_tofdl = 0;

    Double_t x1[n_det];
    Double_t y1[n_det];
    Double_t z1[n_det];
    Double_t q1[n_det];
    Double_t t1[n_det];
    Double_t x2[n_det];
    Double_t y2[n_det];
    Double_t z2[n_det];
    Double_t q2[n_det];
    Double_t t2[n_det];

    Int_t id, id1, id2;

    Int_t det = 0;
    Int_t det1 = 0;
    Int_t det2 = 0;

    for (int i = 0; i < n_det; i++)
    {

        x1[i] = 0.;
        y1[i] = 0.;
        z1[i] = 0.;
        q1[i] = 0.;
        t1[i] = -1000.;

        x2[i] = 0.;
        y2[i] = 0.;
        z2[i] = 0.;
        q2[i] = 0.;
        t2[i] = -1000.;
    }

    // is also number of ifibcount
    Int_t fi23a = 0;
    Int_t fi23b = 1;
    Int_t fi30 = 2;
    Int_t fi31 = 3;
    Int_t fi32 = 4;
    Int_t fi33 = 5;
    Int_t tofir = 6;
    Int_t tofil = 7;
    Int_t tofd1r = 8;
    Int_t tofd1l = 9;
    Int_t tofd2r = 10;
    Int_t tofd2l = 11;
    Int_t tofdgoodr = 12;
    Int_t tofdgoodl = 13;

    Double_t tof = 0.;
    Double_t tStart = 0.;
    Bool_t first = true;
    Double_t time_tofd_mem = 0. / 0.;
    Double_t randx;
    Bool_t debug_in = false;

    nHitstemp = 0;

    // Software Veto cut:
    if (fVeto && fHitItems.at(DET_ROLU)->GetEntriesFast() > 0)
        return;

    fNEvents_veto += 1;

    auto detTofd = fHitItems.at(DET_TOFD);
    Int_t nHits = detTofd->GetEntriesFast();
    Double_t xtemp[n_det][nHits];
    Double_t ytemp[n_det][nHits];
    Double_t ztemp[n_det][nHits];
    Double_t qtemp[n_det][nHits];
    Double_t ttemp[n_det][nHits];
    Double_t ttraw[n_det][nHits];
    Double_t xl[n_det][nHits], tl[n_det][nHits], yl[n_det][nHits], ql[n_det][nHits];
    Double_t xr[n_det][nHits], tr[n_det][nHits], yr[n_det][nHits], qr[n_det][nHits];
    Double_t xlav[nHits];
    Double_t ylav[nHits];
    Double_t zlav[nHits];
    Double_t qlav[nHits];
    Double_t tlav[nHits];
    Double_t xrav[nHits];
    Double_t yrav[nHits];
    Double_t zrav[nHits];
    Double_t qrav[nHits];
    Double_t trav[nHits];
    for (Int_t k = 0; k < nHits; k++)
    {
        xlav[k] = 0. / 0.;
        ylav[k] = 0. / 0.;
        zlav[k] = 0. / 0.;
        qlav[k] = 0. / 0.;
        tlav[k] = 0. / 0.;
        xrav[k] = 0. / 0.;
        yrav[k] = 0. / 0.;
        zrav[k] = 0. / 0.;
        qrav[k] = 0. / 0.;
        trav[k] = 0. / 0.;
    }
    for (Int_t i = 0; i < n_det; i++)
    {
        for (Int_t k = 0; k < nHits; k++)
        {
            xtemp[i][k] = 0. / 0.;
            ytemp[i][k] = 0. / 0.;
            ztemp[i][k] = 0. / 0.;
            qtemp[i][k] = 0. / 0.;
            ttemp[i][k] = 0. / 0.;
            ttraw[i][k] = 0. / 0.;
            xl[i][k] = 0. / 0.;
            yl[i][k] = 0. / 0.;
            tl[i][k] = 0. / 0.;
            ql[i][k] = 0. / 0.;
            xr[i][k] = 0. / 0.;
            yr[i][k] = 0. / 0.;
            tr[i][k] = 0. / 0.;
            qr[i][k] = 0. / 0.;
        }
    }

    // Check which detector saw hown many events
    if (fHitItems.at(DET_ROLU)->GetEntriesFast() < 1 && fHitItems.at(DET_TOFD)->GetEntriesFast() > 0)
        fNEvents_local += 1;
    if (fHitItems.at(DET_ROLU)->GetEntriesFast() < 1 && fHitItems.at(DET_TOFD)->GetEntriesFast() < 1)
        fNEvents_zeroToFD += 1;
    //    if (fHitItems.at(DET_ROLU)->GetEntriesFast() < 1 && fMappedItemsCalifa && fMappedItemsCalifa->GetEntriesFast()
    //    > 0)
    //        fNEvents_califa += 1;
    if (fHitItems.at(DET_ROLU)->GetEntriesFast() < 1 && fHitItems.at(DET_TOFI)->GetEntriesFast() > 0)
        fNEvents_tofi += 1;
    if (fHitItems.at(DET_ROLU)->GetEntriesFast() < 1 && fHitItems.at(0)->GetEntriesFast() > 0 &&
        fHitItems.at(1)->GetEntriesFast() > 0) // fib23a && 23b
        fNEvents_fibers += 1;
    if (fHitItems.at(DET_ROLU)->GetEntriesFast() < 1 && fHitItems.at(DET_TOFD)->GetEntriesFast() > 3 &&
        fHitItems.at(0)->GetEntriesFast() > 0 && fHitItems.at(1)->GetEntriesFast() > 0 &&
        fHitItems.at(2)->GetEntriesFast() > 0 && fHitItems.at(3)->GetEntriesFast() > 0 &&
        fHitItems.at(4)->GetEntriesFast() > 0 && fHitItems.at(5)->GetEntriesFast() > 0)
        fNEvents_pair += 1;

    if (debug2)
        cout << "*** In exec before ToFD, fnEvents: " << fNEvents << endl;

    if (nHits > 0)
    {
        counterTofd++;
        if (debug_in)
        {
            cout << "********************************" << endl;
            cout << "ToFD hits: " << nHits << endl;
        }
        // }

        if (debug2)
            cout << "*** In exec, in tofd, fnEvents= " << fNEvents << ", nHits= " << nHits << endl;

        // if (nHits > 100)
        //   return;

        // create tofd_ && tofdr hits
        Double_t tmem = 0, zmem = 0;
        Int_t nSumtofd1l = 0, nSumtofd2l = 0, nSumtofd1r = 0, nSumtofd2r = 0;
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BTofdHitData* hitTofd = (R3BTofdHitData*)detTofd->At(ihit);

            if (IS_NAN(hitTofd->GetTime()))
                continue;
            if (debug_in)
            {
                cout << "Hit " << ihit << " of " << nHits << " charge " << hitTofd->GetEloss() << " time "
                     << hitTofd->GetTime() << endl;
            }

            randx = (std::rand() / (float)RAND_MAX) - 0.5;
            Double_t xxx = hitTofd->GetX() + 2.7 * randx;
            Double_t yyy = hitTofd->GetY();
            Double_t ttt = hitTofd->GetTime();
            tStart = ttt;
            Double_t qqq = hitTofd->GetEloss();
            fh_ztofd->Fill(yyy, qqq);
            fh_xy_tofd->Fill(xxx, yyy);

            if (nHits > 1)
                fh_Z_vs_dthit->Fill(ttt - tmem, qqq - zmem);
            tmem = ttt;
            zmem = qqq;
            Bool_t tofd_left = false;
            Bool_t tofd_right = false;
            id2 = hitTofd->GetDetId();

            //   if ((id2 == 2 && hitTofd->GetBarId() == 24) || (id2 == 2 && hitTofd->GetBarId() == 21))
            //     continue;

            if (xxx <= 0.) // hitTofd->GetX() < 0.)
            {
                // tof rechts
                tofd_right = true;
                tofd_left = false;
                counterTofdMultir++;

                if (id2 == 1)
                {
                    det = tofd1r;
                    xtemp[tofd1r][nSumtofd1r] = hitTofd->GetX() + 2.7 * randx;
                    ytemp[tofd1r][nSumtofd1r] = hitTofd->GetY();
                    ttemp[tofd1r][nSumtofd1r] = hitTofd->GetTime();
                    qtemp[tofd1r][nSumtofd1r] = hitTofd->GetEloss();
                    ttraw[tofd1r][nSumtofd1r] = hitTofd->GetTimeRaw();
                    nSumtofd1r += 1;
                    fh_time_tofd[0]->Fill(hitTofd->GetTime());
                }
                else if (id2 == 2)
                {
                    det = tofd2r;
                    xtemp[tofd2r][nSumtofd2r] = hitTofd->GetX() + 2.7 * randx;
                    ytemp[tofd2r][nSumtofd2r] = hitTofd->GetY();
                    ttemp[tofd2r][nSumtofd2r] = hitTofd->GetTime();
                    qtemp[tofd2r][nSumtofd2r] = hitTofd->GetEloss();
                    ttraw[tofd2r][nSumtofd2r] = hitTofd->GetTimeRaw();
                    nSumtofd2r += 1;
                    fh_time_tofd[1]->Fill(hitTofd->GetTime());
                }
                else if (id2 == 12)
                {
                    //  det = tofdgoodr;
                }
            }
            else
            {
                // tof links
                counterTofdMultil++;
                tofd_left = true;
                tofd_right = false;
                if (id2 == 1)
                {
                    det = tofd1l;
                    xtemp[tofd1l][nSumtofd1l] = hitTofd->GetX() + 2.7 * randx;
                    ytemp[tofd1l][nSumtofd1l] = hitTofd->GetY();
                    ttemp[tofd1l][nSumtofd1l] = hitTofd->GetTime();
                    qtemp[tofd1l][nSumtofd1l] = hitTofd->GetEloss();
                    ttraw[tofd1l][nSumtofd1l] = hitTofd->GetTimeRaw();
                    nSumtofd1l += 1;
                    fh_time_tofd[0]->Fill(hitTofd->GetTime());
                }
                else if (id2 == 2)
                {
                    det = tofd2l;
                    xtemp[tofd2l][nSumtofd2l] = hitTofd->GetX() + 2.7 * randx;
                    ytemp[tofd2l][nSumtofd2l] = hitTofd->GetY();
                    ttemp[tofd2l][nSumtofd2l] = hitTofd->GetTime();
                    qtemp[tofd2l][nSumtofd2l] = hitTofd->GetEloss();
                    ttraw[tofd2l][nSumtofd2l] = hitTofd->GetTimeRaw();
                    nSumtofd2l += 1;
                    fh_time_tofd[1]->Fill(hitTofd->GetTime());
                }
                else if (id2 == 12)
                {
                    //  det = tofdgoodl;
                }
            }

            counterTofdMulti++;
        }

        if (debug2)
            cout << "*** In exec, before good tofdl/r, fnEvents= " << fNEvents << ", nHits= " << nHits << endl;
        // create goot tofdl and tofdr hits
        Int_t nSuml = 0;
        for (Int_t il2 = 0; il2 < nSumtofd2l; il2++)
        {
            for (Int_t il1 = 0; il1 < nSumtofd1l; il1++)
            {
                fh_ql_Tofd->Fill(qtemp[tofd2l][il2] - qtemp[tofd1l][il1], ttemp[tofd2l][il2] - ttemp[tofd1l][il1]);
                fh_xl_Tofd->Fill(xtemp[tofd2l][il2] - xtemp[tofd1l][il1], ttemp[tofd2l][il2] - ttemp[tofd1l][il1]);
                fh_qplane1_vs_qplane2_l->Fill(qtemp[tofd1l][il1], qtemp[tofd2l][il2]);

                /*  if (std::abs(ttemp[tofd2l][il2] - ttemp[tofd1l][il1]) < 40. &&
                      std::abs(xtemp[tofd2l][il2] - xtemp[tofd1l][il1]) < 5. &&
                      std::abs(qtemp[tofd2l][il2] - qtemp[tofd1l][il1]) < 1.5)*/

                // select either Z=2 in both planes, or Z=6 in both planes
                //  if (((std::abs(qtemp[tofd2l][il2] - 2.088)) < 4. * 0.148 &&
                //       (std::abs(qtemp[tofd1l][il1] - 2.038)) < 4. * 0.106) ||
                //     ((std::abs(qtemp[tofd2l][il2] - 6.094)) < 4. * 0.219 &&
                //      (std::abs(qtemp[tofd1l][il1] - 6.051)) < 4. * 0.1799))
                if (1 == 1)
                {

                    xl[tofdgoodl][nSuml] = (xtemp[tofd2l][il2] + xtemp[tofd1l][il1]) / 2.;
                    tl[tofdgoodl][nSuml] = (ttemp[tofd2l][il2] + ttemp[tofd1l][il1]) / 2.;
                    yl[tofdgoodl][nSuml] = (ytemp[tofd2l][il2] + ytemp[tofd1l][il1]) / 2.;
                    //  ql[tofdgoodl][nSuml] = (qtemp[tofd2l][il2] + qtemp[tofd1l][il1]) / 2.;
                    ql[tofdgoodl][nSuml] = qtemp[tofd1l][il1];
                    nSuml += 1;
                }
            }
        }

        Int_t nSumr = 0;
        for (Int_t ir2 = 0; ir2 < nSumtofd2r; ir2++)
        {
            for (Int_t ir1 = 0; ir1 < nSumtofd1r; ir1++)
            {
                fh_qr_Tofd->Fill(qtemp[tofd2r][ir2] - qtemp[tofd1r][ir1], ttemp[tofd2r][ir2] - ttemp[tofd1r][ir1]);
                fh_xr_Tofd->Fill(xtemp[tofd2r][ir2] - xtemp[tofd1r][ir1], ttemp[tofd2r][ir2] - ttemp[tofd1r][ir1]);
                fh_qplane1_vs_qplane2_r->Fill(qtemp[tofd1r][ir1], qtemp[tofd2r][ir2]);

                /*if (std::abs(ttemp[tofd2r][ir2] - ttemp[tofd1r][ir1]) < 50. &&
                    std::abs(xtemp[tofd2r][ir2] - xtemp[tofd1r][ir1]) < 5. &&
                    std::abs(qtemp[tofd2r][ir2] - qtemp[tofd1r][ir1]) < 1.5)*/

                // select either Z=2 in both planes, or Z=6 in both planes
                //  if (((std::abs(qtemp[tofd2r][ir2] - 2.094)) < 4. * 0.1187 &&
                //      (std::abs(qtemp[tofd1r][ir1] - 2.091)) < 4. * 0.115) ||
                //    ((std::abs(qtemp[tofd2r][ir2] - 6.068)) < 3.7 * 0.214 &&
                // //    (std::abs(qtemp[tofd1r][ir1] - 6.054)) < 3.7 * 0.213))
                if (1 == 1)
                {

                    xr[tofdgoodr][nSumr] = (xtemp[tofd2r][ir2] + xtemp[tofd1r][ir1]) / 2.;
                    tr[tofdgoodr][nSumr] = (ttemp[tofd2r][ir2] + ttemp[tofd1r][ir1]) / 2.;
                    yr[tofdgoodr][nSumr] = (ytemp[tofd2r][ir2] + ytemp[tofd1r][ir1]) / 2.;
                    //  qr[tofdgoodr][nSumr] = (qtemp[tofd2r][ir2] + qtemp[tofd1r][ir1]) / 2.;
                    qr[tofdgoodr][nSumr] = qtemp[tofd1r][ir1];

                    nSumr += 1;
                }
            }
        }

        if (debug2)
            cout << "*** In exec, good tofdl/r, fnEvents= " << fNEvents << ", nHits= " << nHits << endl;

        // Here should be tofi hits added
        // add to tofdr / tofdl hits tofi hits

        // loop over TOFI
        /*
              if (fHitItems.at(DET_TOFI))
              {
                    auto detHitTofi = fHitItems.at(DET_TOFI);
                    Int_t nHitsTofi = detHitTofi->GetEntriesFast();
                    LOG(DEBUG) << "Tofi hits: " << nHitsTofi << endl;

                    Int_t icl=0, icr=0;
                    for (Int_t ihitTofi = 0; ihitTofi < nHitsTofi; ihitTofi++)
                    {
                        det = tofi;
                        R3BTofiHitData* hitTofi = (R3BTofiHitData*)detHitTofi->At(ihitTofi);
                        randx = (std::rand() / (float)RAND_MAX) - 0.5;
                        x1[det] = hitTofi->GetX() + 0.5 * randx; // cm
                        y1[det] = hitTofi->GetY();               // cm
                        z1[det] = 0.;
                        q1[det] = hitTofi->GetEloss();
                        t1[det] = hitTofi->GetTime()+249.;
                        fh_ztofi->Fill(x1[det],q1[det]);
                        fh_time_tofd[2]->Fill(hitTofi->GetTime());

                    // tof rechts
                        if (x1[det] <= 0.)
                        {
                             xr[nSumr+icr]=x1[det];
                             tr[nSumr+icr]=t1[det];
                             yr[nSumr+icr]=y1[det];
                             qr[nSumr+icr]=q1[det] ;
                             icr += 1;
                         }
                         else
                         {
                             xl[nSuml+icl]=x1[det];
                             tl[nSuml+icl]=t1[det];
                             yl[nSuml+icl]=y1[det];
                             ql[nSuml+icl]=q1[det];
                             icl += 1;
                         }

                      }

                   // nSumr = nSumr+icr;
                   // nSuml = nSuml+icl;
                }
      */

        // Check spectra for tofd
        // Double_t tmem = 0;
        for (Int_t ihitr = 0; ihitr < nSumr; ihitr++)
        {
            fh_Z_vs_x->Fill(xr[tofdgoodr][ihitr], qr[tofdgoodr][ihitr]);
            fh_Z_vs_time->Fill(tr[tofdgoodr][ihitr], qr[tofdgoodr][ihitr]);
            // if (ihitr > 0)
            // fh_Z_vs_dthit->Fill(tr[tofdgoodr][ihitr] - tmem, qr[tofdgoodr][ihitr]);
            // tmem = tr[tofdgoodr][ihitr];
        }
        tmem = 0;
        for (Int_t ihitl = 0; ihitl < nSuml; ihitl++)
        {
            fh_Z_vs_x->Fill(xl[tofdgoodl][ihitl], ql[tofdgoodl][ihitl]);
            fh_Z_vs_time->Fill(tl[tofdgoodl][ihitl], ql[tofdgoodl][ihitl]);
            // if (ihitl > 0)
            //      fh_Z_vs_dthit->Fill(tl[tofdgoodl][ihitl] - tmem, ql[tofdgoodl][ihitl]);
            // tmem = tl[tofdgoodl][ihitl];
        }

        Int_t nSumav = 0;
        // correlations left-right tofd and position at fib23
        for (Int_t ihitr = 0; ihitr < nSumr; ihitr++)
        {
            for (Int_t ihitl = 0; ihitl < nSuml; ihitl++)
            {

                fh_dqvsdt_Tofd->Fill(tr[tofdgoodr][ihitr] - tl[tofdgoodl][ihitl],
                                     qr[tofdgoodr][ihitr] - ql[tofdgoodl][ihitl]);
                if ((tr[tofdgoodr][ihitr] - tl[tofdgoodl][ihitl]) > -10. &&
                    (tr[tofdgoodr][ihitr] - tl[tofdgoodl][ihitl]) < 30.)
                {
                    fh_ql_vs_qr_Tofd->Fill(qr[tofdgoodr][ihitr], ql[tofdgoodl][ihitl]);
                    fh_xl_vs_xr_Tofd->Fill(xr[tofdgoodr][ihitr], xl[tofdgoodl][ihitl]);

                    auto detHit23a = fHitItems.at(DET_FI23A);
                    Int_t nHits23a = detHit23a->GetEntriesFast();
                    for (Int_t ihit23a = 0; ihit23a < nHits23a; ihit23a++)
                    {
                        det = fi23a;
                        R3BFiberMAPMTHitData* hit23a = (R3BFiberMAPMTHitData*)detHit23a->At(ihit23a);
                        randx = (std::rand() / (float)RAND_MAX) - 0.5;
                        x1[det] = hit23a->GetX() + 0.028 * randx; // cm
                        y1[det] = hit23a->GetY();
                        q1[det] = hit23a->GetEloss();
                        t1[det] = hit23a->GetTime();
                        tof = tStart - t1[det];

                        if (fCuts && (x1[det] < -6 || x1[det] > 6))
                            continue;
                        if (fCuts && (t1[det] < -10 || t1[det] > 10))
                            continue;
                        // if (fCuts && (tof < ftofminFib2x || tof > ftofmaxFib2x))
                        //    continue;

                        fh_q_vs_tofFib23aTofd->Fill(tl[tofdgoodl][ihitl] - t1[det], ql[tofdgoodl][ihitl]);
                        fh_q_vs_tofFib23aTofd->Fill(tr[tofdgoodr][ihitr] - t1[det], qr[tofdgoodr][ihitr]);

                        // loop over fiber 23b
                        auto detHit23b = fHitItems.at(DET_FI23B);
                        Int_t nHits23b = detHit23b->GetEntriesFast();
                        LOG(DEBUG) << "Fi23b hits: " << nHits23b << endl;
                        for (Int_t ihit23b = 0; ihit23b < nHits23b; ihit23b++)
                        {
                            det = fi23b;
                            R3BFiberMAPMTHitData* hit23b = (R3BFiberMAPMTHitData*)detHit23b->At(ihit23b);
                            randx = (std::rand() / (float)RAND_MAX) - 0.5;
                            x1[det] = hit23b->GetX(); // cm
                            y1[det] = hit23b->GetY() + 0.028 * randx;
                            z1[det] = 0.;
                            q1[det] = hit23b->GetEloss();
                            t1[det] = hit23b->GetTime();
                            tof = tStart - t1[det];

                            if (fCuts && (y1[det] < -6 || y1[det] > 6))
                                continue;
                            if (fCuts && (t1[det] < -10 || t1[det] > 10))
                                continue;
                            // if (fCuts && (tof < ftofminFib2x || tof > ftofmaxFib2x))
                            //  continue;

                            fh_xy_target->Fill(x1[fi23a], y1[fi23b]);
                        }
                    }

                    // Look at events with ql+qr=8 on Tofd and xy at Fib23
                    if ((qr[tofdgoodr][ihitr] + ql[tofdgoodl][ihitl]) > 7.5 &&
                        (qr[tofdgoodr][ihitr] + ql[tofdgoodl][ihitl]) < 8.5)
                    {

                        // create hits with He and C left and right (events with He and C in t he same half are here
                        // ignored)
                        xlav[nSumav] = xl[tofdgoodl][ihitl];
                        qlav[nSumav] = ql[tofdgoodl][ihitl];
                        zlav[nSumav] = ql[tofdgoodl][ihitl];
                        tlav[nSumav] = tl[tofdgoodl][ihitl];
                        ylav[nSumav] = yl[tofdgoodl][ihitl];

                        xrav[nSumav] = xr[tofdgoodr][ihitr];
                        qrav[nSumav] = qr[tofdgoodr][ihitr];
                        zrav[nSumav] = qr[tofdgoodr][ihitr];
                        trav[nSumav] = tr[tofdgoodr][ihitr];
                        yrav[nSumav] = yr[tofdgoodr][ihitr];

                        nSumav += 1;

                        fh_ql_vs_qr_Tofd_Zsum8->Fill(qr[tofdgoodr][ihitr], ql[tofdgoodl][ihitl]);
                        fh_dt_Tofd_Zsum8->Fill(tr[tofdgoodr][ihitr] - tl[tofdgoodl][ihitl],
                                               qr[tofdgoodr][ihitr] - ql[tofdgoodl][ihitl]);
                        fh_xl_vs_xr_Tofd_Zsum8->Fill(xr[tofdgoodr][ihitr], xl[tofdgoodl][ihitl]);

                        // loop over fiber 23a
                        detHit23a = fHitItems.at(DET_FI23A);
                        nHits23a = detHit23a->GetEntriesFast();
                        for (Int_t ihit23a = 0; ihit23a < nHits23a; ihit23a++)
                        {
                            det = fi23a;
                            R3BFiberMAPMTHitData* hit23a = (R3BFiberMAPMTHitData*)detHit23a->At(ihit23a);
                            randx = (std::rand() / (float)RAND_MAX) - 0.5;
                            x1[det] = hit23a->GetX() + 0.028 * randx; // cm
                            y1[det] = hit23a->GetY();
                            q1[det] = hit23a->GetEloss();
                            t1[det] = hit23a->GetTime();
                            tof = tStart - t1[det];

                            // cout << "Time: " << t1[det] << endl;
                            if (fCuts && (t1[det] < -30 || t1[det] > 30))
                                continue;
                            if (fCuts && (tof < ftofminFib2x || tof > ftofmaxFib2x))
                                continue;

                            // loop over fiber 23b
                            auto detHit23b = fHitItems.at(DET_FI23B);
                            Int_t nHits23b = detHit23b->GetEntriesFast();
                            LOG(DEBUG) << "Fi23b hits: " << nHits23b << endl;
                            for (Int_t ihit23b = 0; ihit23b < nHits23b; ihit23b++)
                            {
                                det = fi23b;
                                R3BFiberMAPMTHitData* hit23b = (R3BFiberMAPMTHitData*)detHit23b->At(ihit23b);
                                randx = (std::rand() / (float)RAND_MAX) - 0.5;
                                x1[det] = hit23b->GetX(); // cm
                                y1[det] = hit23b->GetY() + 0.028 * randx;
                                z1[det] = 0.;
                                q1[det] = hit23b->GetEloss();
                                t1[det] = hit23b->GetTime();
                                tof = tStart - t1[det];

                                if (fCuts && (t1[det] < -30 || t1[det] > 30))
                                    continue;
                                if (fCuts && (tof < ftofminFib2x || tof > ftofmaxFib2x))
                                    continue;

                                fh_xy_target_Zsum8->Fill(x1[fi23a], y1[fi23b]);
                            }
                        }
                    }
                }
            }
        }

        if (debug2)
            cout << "*** In exec, after tofdl/r correl, fnEvents= " << fNEvents << ", nHits= " << nHits << endl;

        Double_t xStart, yStart, qStart;
        // ***********************************************
        // loop over right side
        // ***********************************************
        for (Int_t ihit = 0; ihit < nSumav; ihit++)
        {
            xStart = xrav[ihit];
            qStart = qrav[ihit];
            yStart = yrav[ihit];
            tStart = trav[ihit];
            if (fCuts && (qStart < 0 || qStart > 8))
            {
                // cout << "Cut because of charge: " << qqq << endl;
                continue;
            }
            if (fCuts && (xStart < -65 || xStart > 65))
            {
                // cout << "Cut because of x" << endl;
                continue;
            }
            if (fCuts && (yStart < -80 || yStart > 80))
            {
                // cout << "Cut because of x" << endl;
                continue;
            }
            if (fCuts && (tStart < -50 || tStart > 50))
            {
                // cout << "Cut because of t" << endl;
                continue;
            }
            // cut in ToT for Fibers
            Double_t cutQ = 0.;

            // loop over fiber 31
            auto detHit31 = fHitItems.at(DET_FI31);
            Int_t nHits31 = detHit31->GetEntriesFast();
            LOG(DEBUG) << "Fi31 hits: " << nHits31 << endl;
            for (Int_t ihit31 = 0; ihit31 < nHits31; ihit31++)
            {
                det = fi31;
                R3BFiberMAPMTHitData* hit31 = (R3BFiberMAPMTHitData*)detHit31->At(ihit31);
                randx = (std::rand() / (float)RAND_MAX) - 0.5;
                x1[det] = hit31->GetX() + 0.1 * randx; // cm
                y1[det] = hit31->GetY();               // cm
                z1[det] = 0.;
                q1[det] = hit31->GetEloss();

                t1[det] = hit31->GetTime();
                tof = tStart - t1[det];

                // Fill histograms before cuts
                fh_Fib_ToF[det]->Fill(tof);
                fh_xFib_vs_xTofd[det]->Fill(xStart, x1[det]);
                fh_yFib_vs_yTofd[det]->Fill(yStart, y1[det]);
                fh_qFib_vs_qTofd[det]->Fill(qStart, q1[det]);
                fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                fh_x_vs_Events[det]->Fill(fNEvents, x1[det]);

                hits31bc++;

                // Cuts on Fi31

                if (fCuts && (x1[det] < -30 || x1[det] > 30))
                    continue;
                if (fCuts && (t1[det] < -30 || t1[det] > 30))
                    continue;
                if (fCuts && (tof < ftofminFib3x || tof > ftofmaxFib3x))
                    continue;

                Double_t xtemp31 = -0.732 * xStart - 29.364;
                if (std::abs(xtemp31 - x1[det]) > 3.)
                    continue;

                hits31++;

                // Fill histograms after cuts
                fh_Fib_ToF_ac[det]->Fill(tof);
                fh_xFib_vs_xTofd_ac[det]->Fill(xStart, x1[det]); // /(-27.2252-0.755*xStart));
                fh_yFib_vs_yTofd_ac[det]->Fill(yStart, y1[det]);
                if (qStart > 1.5 && qStart < 2.5)
                    fh_qFib_vs_qTofd_ac[det]->Fill(qStart, q1[det]);

                xr[det][ihit] = x1[det];
                yr[det][ihit] = y1[det];
                qr[det][ihit] = q1[det];
                tr[det][ihit] = t1[det];

                if (debug2)
                    cout << "Fi31: " << ihit31 << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                         << " t1: " << t1[det] << endl;

                // loop over fiber 33
                auto detHit33 = fHitItems.at(DET_FI33);
                Int_t nHits33 = detHit33->GetEntriesFast();
                LOG(DEBUG) << "Fi33 hits: " << nHits33 << endl;
                for (Int_t ihit33 = 0; ihit33 < nHits33; ihit33++)
                {
                    det = fi33;
                    R3BFiberMAPMTHitData* hit33 = (R3BFiberMAPMTHitData*)detHit33->At(ihit33);
                    randx = (std::rand() / (float)RAND_MAX) - 0.5;
                    x1[det] = hit33->GetX() + 0.1 * randx; // cm
                    y1[det] = hit33->GetY();
                    z1[det] = 0.;
                    q1[det] = hit33->GetEloss();
                    t1[det] = hit33->GetTime();
                    tof = tStart - t1[det];

                    // Fill histograms before cuts
                    fh_Fib_ToF[det]->Fill(tof);
                    fh_xFib_vs_xTofd[det]->Fill(xStart, x1[det]);
                    fh_yFib_vs_yTofd[det]->Fill(yStart, y1[det]);
                    fh_qFib_vs_qTofd[det]->Fill(qStart, q1[det]);
                    fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                    fh_x_vs_Events[det]->Fill(fNEvents, x1[det]);

                    fh_xFib31_vs_xFib33->Fill(x1[det], x1[fi31]);
                    fh_yFib31_vs_yFib33->Fill(y1[det], y1[fi31]);
                    fh_qFib31_vs_qFib33->Fill(x1[det], q1[fi31]);
                    fh_tFib31_diff_tFib33->Fill(t1[det], t1[fi31]);

                    hits33bc++;

                    if (fCuts && (x1[det] < -30 || x1[det] > 30))
                        continue;
                    if (fCuts && (t1[det] < -30 || t1[det] > 30))
                        continue;
                    if (fCuts && (tof < ftofminFib3x || tof > ftofmaxFib3x))
                        continue;

                    Double_t x31 = 0.866 * x1[det] - 4.637;
                    if (std::abs(x1[fi31] - x31) > 3.)
                        continue;
                    if (std::abs(t1[det] - t1[fi31]) > 10.)
                        continue;

                    hits33++;

                    // Fill histograms after cuts
                    fh_Fib_ToF_ac[det]->Fill(tof);
                    fh_xFib_vs_xTofd_ac[det]->Fill(xStart, x1[det]); // /(-27.3077-0.6158*xStart));
                    fh_yFib_vs_yTofd_ac[det]->Fill(yStart, y1[det]);
                    if (qStart > 1.5 && qStart < 2.5)
                        fh_qFib_vs_qTofd_ac[det]->Fill(qStart, q1[det]);

                    fh_xFib31_vs_xFib33_ac->Fill(x1[det], x1[fi31]);
                    fh_yFib31_vs_yFib33_ac->Fill(y1[det], y1[fi31]);
                    fh_qFib31_vs_qFib33_ac->Fill(x1[det], q1[fi31]);
                    fh_tFib31_diff_tFib33_ac->Fill(t1[det], t1[fi31]);

                    xr[det][ihit] = x1[det];
                    yr[det][ihit] = y1[det];
                    qr[det][ihit] = q1[det];
                    tr[det][ihit] = t1[det];

                    if (debug2)
                        cout << "Fi33: " << ihit33 << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                             << " t1: " << t1[det] << endl;
                } // end fi33

                // loop over fiber 23a
                auto detHit23a = fHitItems.at(DET_FI23A);
                Int_t nHits23a = detHit23a->GetEntriesFast();
                LOG(DEBUG) << "Fi23a hits: " << nHits23a << endl;
                for (Int_t ihit23a = 0; ihit23a < nHits23a; ihit23a++)
                {
                    det = fi23a;
                    R3BFiberMAPMTHitData* hit23a = (R3BFiberMAPMTHitData*)detHit23a->At(ihit23a);
                    randx = (std::rand() / (float)RAND_MAX) - 0.5;
                    x1[det] = hit23a->GetX() + 0.028 * randx; // cm
                    y1[det] = hit23a->GetY();
                    q1[det] = hit23a->GetEloss();
                    t1[det] = hit23a->GetTime();
                    tof = tStart - t1[det];

                    // Fill histograms before cuts
                    fh_Fib_ToF[det]->Fill(tof);
                    fh_xFib_vs_xTofd[det]->Fill(xStart, x1[det]);
                    fh_yFib_vs_yTofd[det]->Fill(yStart, y1[det]);
                    fh_qFib_vs_qTofd[det]->Fill(qStart, q1[det]);
                    fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                    fh_x_vs_Events[det]->Fill(fNEvents, x1[det]);

                    fh_xFib23a_vs_xFib31->Fill(x1[det], x1[fi31]);
                    fh_yFib23a_vs_yFib31->Fill(y1[det], y1[fi31]);
                    fh_qFib23a_vs_qFib31->Fill(x1[det], q1[fi31]);
                    fh_tFib23a_diff_tFib31->Fill(t1[det], t1[fi31]);

                    if (fCuts && (x1[det] < -6 || x1[det] > 6))
                        continue;
                    if (fCuts && (t1[det] < -30 || t1[det] > 30))
                        continue;
                    if (fCuts && (tof < ftofminFib2x || tof > ftofmaxFib2x))
                        continue;
                    Double_t x31 = -6.625 * x1[det] - 29.29;
                    // if (std::abs(x1[fi31] - x31) > 5.) continue;
                    if (x1[det] > -0.3)
                        continue;
                    if (std::abs(t1[det] - t1[fi31]) > 10.)
                        continue;

                    // Fill histograms
                    fh_Fib_ToF_ac[det]->Fill(tof);
                    fh_xFib_vs_xTofd_ac[det]->Fill(xStart, x1[det]);
                    fh_yFib_vs_yTofd_ac[det]->Fill(yStart, y1[det]);
                    if (qStart > 1.5 && qStart < 2.5)
                        fh_qFib_vs_qTofd_ac[det]->Fill(qStart, q1[det]);

                    fh_xFib23a_vs_xFib31_ac->Fill(x1[det], x1[fi31]);
                    fh_yFib23a_vs_yFib31_ac->Fill(y1[det], y1[fi31]);
                    fh_qFib23a_vs_qFib31_ac->Fill(x1[det], q1[fi31]);
                    fh_tFib23a_diff_tFib31_ac->Fill(t1[det], t1[fi31]);

                    xr[det][ihit] = x1[det];
                    yr[det][ihit] = y1[det];
                    qr[det][ihit] = q1[det];
                    tr[det][ihit] = t1[det];

                    if (debug2)
                        cout << "Fi23a " << ihit23a << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                             << " t1: " << tof << endl;
                } // end fi23a

                // loop over fiber 23b
                auto detHit23b = fHitItems.at(DET_FI23B);
                Int_t nHits23b = detHit23b->GetEntriesFast();
                LOG(DEBUG) << "Fi23b hits: " << nHits23b << endl;
                for (Int_t ihit23b = 0; ihit23b < nHits23b; ihit23b++)
                {
                    det = fi23b;
                    R3BFiberMAPMTHitData* hit23b = (R3BFiberMAPMTHitData*)detHit23b->At(ihit23b);
                    randx = (std::rand() / (float)RAND_MAX) - 0.5;
                    x1[det] = hit23b->GetX() + 0.028 * randx; // cm
                    y1[det] = hit23b->GetY();
                    z1[det] = 0.;
                    q1[det] = hit23b->GetEloss();
                    t1[det] = hit23b->GetTime();
                    tof = tStart - t1[det];

                    // Fill histograms before cuts
                    fh_Fib_ToF[det]->Fill(tof);
                    fh_xFib_vs_xTofd[det]->Fill(xStart, x1[det]);
                    fh_yFib_vs_yTofd[det]->Fill(yStart, y1[det]);
                    fh_qFib_vs_qTofd[det]->Fill(qStart, q1[det]);
                    fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                    fh_x_vs_Events[det]->Fill(fNEvents, y1[det]);

                    fh_xFib23b_vs_xFib31->Fill(x1[det], x1[fi31]);
                    fh_yFib23b_vs_yFib31->Fill(y1[det], y1[fi31]);
                    fh_qFib23b_vs_qFib31->Fill(x1[det], q1[fi31]);
                    fh_tFib23b_diff_tFib31->Fill(t1[det], t1[fi31]);

                    if (fCuts && (t1[det] < -30 || t1[det] > 30))
                        continue;
                    if (fCuts && (y1[det] < -6 || y1[det] > 6))
                        continue;
                    if (fCuts && (tof < ftofminFib2x || tof > ftofmaxFib2x))
                        continue;
                    if (std::abs(t1[det] - t1[fi31]) > 10.)
                        continue;

                    // Fill histograms
                    fh_Fib_ToF_ac[det]->Fill(tof);
                    fh_xFib_vs_xTofd_ac[det]->Fill(xStart, x1[det]);
                    fh_yFib_vs_yTofd_ac[det]->Fill(yStart, y1[det]);
                    if (qStart > 1.5 && qStart < 2.5)
                        fh_qFib_vs_qTofd_ac[det]->Fill(qStart, q1[det]);

                    fh_xFib23b_vs_xFib31_ac->Fill(x1[det], x1[fi31]);
                    fh_yFib23b_vs_yFib31_ac->Fill(y1[det], y1[fi31]);
                    fh_qFib23b_vs_qFib31_ac->Fill(x1[det], q1[fi31]);
                    fh_tFib23b_diff_tFib31_ac->Fill(t1[det], t1[fi31]);

                    xr[det][ihit] = x1[det];
                    yr[det][ihit] = y1[det];
                    qr[det][ihit] = q1[det];
                    tr[det][ihit] = t1[det];

                    if (debug2)
                        cout << "Fi23b " << ihit23b << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                             << " t1: " << tof << endl;
                }
            }
            fh_ztofd_ac->Fill(yStart, qStart);
        }

        if (debug2)
            cout << "*** In exec, after loop over right side, fnEvents= " << fNEvents << ", nHits= " << nHits << endl;

        // ***********************************************
        // loop over left side
        // ***********************************************
        for (Int_t ihit = 0; ihit < nSumav; ihit++)
        {
            xStart = xlav[ihit];
            qStart = qlav[ihit];
            yStart = ylav[ihit];
            tStart = tlav[ihit];
            if (fCuts && (qStart < 0 || qStart > 8))
            {
                // cout << "Cut because of charge: " << qqq << endl;
                continue;
            }
            if (fCuts && (xStart < -65. || xStart > 65.))
            {
                // cout << "Cut because of x" << endl;
                continue;
            }
            if (fCuts && (yStart < -80. || yStart > 80.))
            {
                // cout << "Cut because of x" << endl;
                continue;
            }
            if (fCuts && (tStart < -50. || tStart > 50.))
            {
                // cout << "Cut because of t" << endl;
                continue;
            }
            // cut in ToT for Fibers
            Double_t cutQ = 0.;

            // loop over fiber 30
            auto detHit30 = fHitItems.at(DET_FI30);
            Int_t nHits30 = detHit30->GetEntriesFast();
            LOG(DEBUG) << "Fi30 hits: " << nHits30 << endl;
            for (Int_t ihit30 = 0; ihit30 < nHits30; ihit30++)
            {
                det = fi30;
                R3BFiberMAPMTHitData* hit30 = (R3BFiberMAPMTHitData*)detHit30->At(ihit30);
                randx = (std::rand() / (float)RAND_MAX) - 0.5;
                x1[det] = hit30->GetX() + 0.1 * randx; // cm
                y1[det] = hit30->GetY();               // cm
                z1[det] = 0.;
                q1[det] = hit30->GetEloss();

                t1[det] = hit30->GetTime();
                tof = tStart - t1[det];

                // Fill histograms before cuts
                fh_Fib_ToF[det]->Fill(tof);
                fh_xFib_vs_xTofd[det]->Fill(xStart, x1[det]);
                fh_yFib_vs_yTofd[det]->Fill(yStart, y1[det]);
                fh_qFib_vs_qTofd[det]->Fill(qStart, q1[det]);
                fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                fh_x_vs_Events[det]->Fill(fNEvents, x1[det]);

                hits30bc++;

                // Cuts on Fi30

                if (fCuts && (x1[det] < -30. || x1[det] > 30.))
                    continue;
                if (fCuts && (t1[det] < -30. || t1[det] > 30.))
                    continue;
                if (fCuts && (tof < ftofminFib3x || tof > ftofmaxFib3x))
                    continue;

                Double_t xtemp30 = 0.7796 * xStart - 28.669;
                if (std::abs(xtemp30 - x1[det]) > 3.)
                    continue;

                hits30++;

                // Fill histograms after cuts
                fh_Fib_ToF_ac[det]->Fill(tof);
                fh_xFib_vs_xTofd_ac[det]->Fill(xStart, x1[det]); // /(-28.049+0.8045*xStart));
                fh_yFib_vs_yTofd_ac[det]->Fill(yStart, y1[det]);
                if (qStart > 1.5 && qStart < 2.5)
                    fh_qFib_vs_qTofd_ac[det]->Fill(qStart, q1[det]);
                xr[det][ihit] = x1[det];
                yr[det][ihit] = y1[det];
                qr[det][ihit] = q1[det];
                tr[det][ihit] = t1[det];

                if (debug2)
                    cout << "Fi30: " << ihit30 << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                         << " t1: " << t1[det] << endl;

                // loop over fiber 32
                auto detHit32 = fHitItems.at(DET_FI32);
                Int_t nHits32 = detHit32->GetEntriesFast();
                LOG(DEBUG) << "Fi32 hits: " << nHits32 << endl;
                for (Int_t ihit32 = 0; ihit32 < nHits32; ihit32++)
                {
                    det = fi32;
                    R3BFiberMAPMTHitData* hit32 = (R3BFiberMAPMTHitData*)detHit32->At(ihit32);
                    randx = (std::rand() / (float)RAND_MAX) - 0.5;
                    x1[det] = hit32->GetX() + 0.1 * randx; // cm
                    y1[det] = hit32->GetY();
                    z1[det] = 0.;
                    q1[det] = hit32->GetEloss();
                    t1[det] = hit32->GetTime();
                    tof = tStart - t1[det];

                    // Fill histograms before cuts
                    fh_Fib_ToF[det]->Fill(tof);
                    fh_xFib_vs_xTofd[det]->Fill(xStart, x1[det]);
                    fh_yFib_vs_yTofd[det]->Fill(yStart, y1[det]);
                    fh_qFib_vs_qTofd[det]->Fill(qStart, q1[det]);
                    fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                    fh_x_vs_Events[det]->Fill(fNEvents, x1[det]);

                    fh_xFib30_vs_xFib32->Fill(x1[det], x1[fi30]);
                    fh_yFib30_vs_yFib32->Fill(y1[det], y1[fi30]);
                    fh_qFib30_vs_qFib32->Fill(x1[det], q1[fi30]);
                    fh_tFib30_diff_tFib32->Fill(t1[det], t1[fi30]);

                    hits32bc++;

                    if (fCuts && (x1[det] < -30 || x1[det] > 30))
                        continue;
                    if (fCuts && (t1[det] < -30 || t1[det] > 30))
                        continue;
                    if (fCuts && (tof < ftofminFib3x || tof > ftofmaxFib3x))
                        continue;
                    Double_t x30 = 0.873 * x1[det] - 3.302;
                    if (std::abs(x1[fi30] - x30) > 3.)
                        continue;
                    if (std::abs(t1[det] - t1[fi30]) > 10.)
                        continue;

                    hits32++;

                    // Fill histograms after cuts
                    fh_Fib_ToF_ac[det]->Fill(tof);
                    fh_xFib_vs_xTofd_ac[det]->Fill(xStart, x1[det]); // /(-27.3009+0.6174*xStart));
                    fh_yFib_vs_yTofd_ac[det]->Fill(yStart, y1[det]);
                    if (qStart > 1.5 && qStart < 2.5)
                        fh_qFib_vs_qTofd_ac[det]->Fill(qStart, q1[det]);

                    fh_xFib30_vs_xFib32_ac->Fill(x1[det], x1[fi30]);
                    fh_yFib30_vs_yFib32_ac->Fill(y1[det], y1[fi30]);
                    fh_qFib30_vs_qFib32_ac->Fill(x1[det], q1[fi30]);
                    fh_tFib30_diff_tFib32_ac->Fill(t1[det], t1[fi30]);

                    xr[det][ihit] = x1[det];
                    yr[det][ihit] = y1[det];
                    qr[det][ihit] = q1[det];
                    tr[det][ihit] = t1[det];

                    if (debug2)
                        cout << "Fi32: " << ihit32 << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                             << " t1: " << t1[det] << endl;
                } // end fi30

                // loop over fiber 23a
                auto detHit23a = fHitItems.at(DET_FI23A);
                Int_t nHits23a = detHit23a->GetEntriesFast();
                LOG(DEBUG) << "Fi23a hits: " << nHits23a << endl;
                for (Int_t ihit23a = 0; ihit23a < nHits23a; ihit23a++)
                {
                    det = fi23a;
                    R3BFiberMAPMTHitData* hit23a = (R3BFiberMAPMTHitData*)detHit23a->At(ihit23a);
                    randx = (std::rand() / (float)RAND_MAX) - 0.5;
                    x1[det] = hit23a->GetX() + 0.028 * randx; // cm
                    y1[det] = hit23a->GetY();
                    q1[det] = hit23a->GetEloss();
                    t1[det] = hit23a->GetTime();
                    tof = tStart - t1[det];

                    // Fill histograms before cuts
                    fh_Fib_ToF[det]->Fill(tof);
                    fh_xFib_vs_xTofd[det]->Fill(xStart, x1[det]);
                    fh_yFib_vs_yTofd[det]->Fill(yStart, y1[det]);
                    fh_qFib_vs_qTofd[det]->Fill(qStart, q1[det]);
                    fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                    fh_x_vs_Events[det]->Fill(fNEvents, x1[det]);

                    fh_xFib23a_vs_xFib30->Fill(x1[det], x1[fi30]);
                    fh_yFib23a_vs_yFib30->Fill(y1[det], y1[fi30]);
                    fh_qFib23a_vs_qFib30->Fill(x1[det], q1[fi30]);
                    fh_tFib23a_diff_tFib30->Fill(t1[det], t1[fi30]);

                    if (fCuts && (x1[det] < -6 || x1[det] > 6))
                        continue;
                    if (fCuts && (t1[det] < -30 || t1[det] > 30))
                        continue;
                    if (fCuts && (tof < ftofminFib2x || tof > ftofmaxFib2x))
                        continue;
                    Double_t x30 = 6.498 * x1[det] - 28.93;
                    if (std::abs(x1[fi30] - x30) > 5.)
                        continue;
                    // if(x1[det] < 0.4) continue;
                    if (std::abs(t1[det] - t1[fi30]) > 10.)
                        continue;

                    // Fill histograms
                    fh_Fib_ToF_ac[det]->Fill(tof);
                    fh_xFib_vs_xTofd_ac[det]->Fill(xStart, x1[det]);
                    fh_yFib_vs_yTofd_ac[det]->Fill(yStart, y1[det]);
                    if (qStart > 1.5 && qStart < 2.5)
                        fh_qFib_vs_qTofd_ac[det]->Fill(qStart, q1[det]);

                    fh_xFib23a_vs_xFib30_ac->Fill(x1[det], x1[fi30]);
                    fh_yFib23a_vs_yFib30_ac->Fill(y1[det], y1[fi30]);
                    fh_qFib23a_vs_qFib30_ac->Fill(x1[det], q1[fi30]);
                    fh_tFib23a_diff_tFib30_ac->Fill(t1[det], t1[fi30]);

                    xr[det][ihit] = x1[det];
                    yr[det][ihit] = y1[det];
                    qr[det][ihit] = q1[det];
                    tr[det][ihit] = t1[det];

                    if (debug2)
                        cout << "Fi23a " << ihit23a << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                             << " t1: " << tof << endl;
                } // end fi23a
                // loop over fiber 23b

                auto detHit23b = fHitItems.at(DET_FI23B);
                Int_t nHits23b = detHit23b->GetEntriesFast();
                LOG(DEBUG) << "Fi23b hits: " << nHits23b << endl;
                for (Int_t ihit23b = 0; ihit23b < nHits23b; ihit23b++)
                {
                    det = fi23b;
                    R3BFiberMAPMTHitData* hit23b = (R3BFiberMAPMTHitData*)detHit23b->At(ihit23b);
                    randx = (std::rand() / (float)RAND_MAX) - 0.5;
                    x1[det] = hit23b->GetX() + 0.028 * randx; // cm
                    y1[det] = hit23b->GetY();
                    z1[det] = 0.;
                    q1[det] = hit23b->GetEloss();
                    t1[det] = hit23b->GetTime();
                    tof = tStart - t1[det];

                    // Fill histograms before cuts
                    fh_Fib_ToF[det]->Fill(tof);
                    fh_xFib_vs_xTofd[det]->Fill(xStart, x1[det]);
                    fh_yFib_vs_yTofd[det]->Fill(yStart, y1[det]);
                    fh_qFib_vs_qTofd[det]->Fill(qStart, q1[det]);
                    fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                    fh_x_vs_Events[det]->Fill(fNEvents, y1[det]);

                    fh_xFib23b_vs_xFib30->Fill(x1[det], x1[fi30]);
                    fh_yFib23b_vs_yFib30->Fill(y1[det], y1[fi30]);
                    fh_qFib23b_vs_qFib30->Fill(x1[det], q1[fi30]);
                    fh_tFib23b_diff_tFib30->Fill(t1[det], t1[fi30]);

                    if (fCuts && (t1[det] < -30 || t1[det] > 30))
                        continue;
                    if (fCuts && (y1[det] < -6 || y1[det] > 6))
                        continue;
                    if (fCuts && (tof < ftofminFib2x || tof > ftofmaxFib2x))
                        continue;
                    if (std::abs(t1[det] - t1[fi30]) > 10.)
                        continue;

                    // Fill histograms
                    fh_Fib_ToF_ac[det]->Fill(tof);
                    fh_xFib_vs_xTofd_ac[det]->Fill(xStart, x1[det]);
                    fh_yFib_vs_yTofd_ac[det]->Fill(yStart, y1[det]);
                    if (qStart > 1.5 && qStart < 2.5)
                        fh_qFib_vs_qTofd_ac[det]->Fill(qStart, q1[det]);

                    fh_xFib23b_vs_xFib30_ac->Fill(x1[det], x1[fi30]);
                    fh_yFib23b_vs_yFib30_ac->Fill(y1[det], y1[fi30]);
                    fh_qFib23b_vs_qFib30_ac->Fill(x1[det], q1[fi30]);
                    fh_tFib23b_diff_tFib30_ac->Fill(t1[det], t1[fi30]);

                    xr[det][ihit] = x1[det];
                    yr[det][ihit] = y1[det];
                    qr[det][ihit] = q1[det];
                    tr[det][ihit] = t1[det];

                    if (debug2)
                        cout << "Fi23b " << ihit23b << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                             << " t1: " << tof << endl;
                }
            }
            fh_ztofd_ac->Fill(yStart, qStart);
        }
    } // end of if nHitsTofd > 0
    if (header->GetTimeStamp() > 0)
        time_end = header->GetTimeStamp();
}

void R3BGlobalCorrelationsS494::FinishEvent()
{

    for (Int_t det = 0; det < DET_MAX; det++)
    {
        if (fHitItems.at(det))
        {
            fHitItems.at(det)->Clear();
        }
        if (fCalItems.at(det))
        {
            fCalItems.at(det)->Clear();
        }
    }
}

void R3BGlobalCorrelationsS494::FinishTask()
{

    cout << "Statistics:" << endl;
    cout << "Total number of events             : " << fNEvents_total << endl;
    cout << "Events with correct trigger & tpat : " << fNEvents << endl;
    cout << "Events w/o Rolu                    : " << fNEvents_veto << endl;
    cout << "Events w/o Rolu and with ToFD      : " << fNEvents_local << endl;
    cout << "Events w/o Rolu and w/o ToFD       : " << fNEvents_zeroToFD << endl;
    //    cout << "Events w/o Rolu and with Califa    : " << fNEvents_califa << endl;
    cout << "Events w/o Rolu and with ToFI      : " << fNEvents_tofi << endl;
    cout << "Events w/o Rolu and with fibers23  : " << fNEvents_fibers << endl;
    cout << "Events with two particles          : " << fNEvents_pair << endl;
    cout << "Time duration                      : " << (time_end - time_begin) / 1.e9 << " sec" << endl;
    cout << "testing: " << time_end << ", " << time_begin << endl;
    cout << "nSpill                             : " << fNSpills << endl;
    cout << "Total num of 18O                   : " << nBeamParticle << endl;
    cout << "Total SEE counts                   : " << nOffSpillParticle << endl;
    cout << "Total with accepted triggers       : " << nAccepted << endl;
    cout << "Dead-time / %                      : "
         << ((double)nAccepted - (double)nOffSpillParticle) / (double)nAccepted * 100. << endl;
    cout << "  " << endl;
    cout << "Wrong Trigger: " << counterWrongTrigger << endl;
    cout << "Wrong Tpat: " << counterWrongTpat << endl;
    cout << "TofD: " << counterTofd << endl;
    cout << "TofD multi: " << counterTofdMulti << endl;
    cout << "TofDr multi: " << counterTofdMultir << endl;
    cout << "TofDl multi: " << counterTofdMultil << endl;

    ofstream outfile(fcounter_file.c_str(), ios_base::out | ios_base::app);
    //    ofstream outfile(fcounter_file.c_str());
    if (!(outfile.is_open()))
        cout << "Warning: unable to open outpu file " << fcounter_file << endl;

    if (outfile.is_open())
    {
        outfile << setprecision(10) << "Total number of events             : " << fNEvents_total << endl;
        outfile << setprecision(10) << "Events with correct trigger & tpat : " << fNEvents << endl;
        outfile << setprecision(10) << "Events w/o Rolu                    : " << fNEvents_veto << endl;
        outfile << setprecision(10) << "Events w/o Rolu and with ToFD      : " << fNEvents_local << endl;
        outfile << setprecision(10) << "Events w/o Rolu and w/o ToFD       : " << fNEvents_zeroToFD << endl;
        //        outfile << setprecision(10) << "Events w/o Rolu and with Califa    : " << fNEvents_califa << endl;
        outfile << setprecision(10) << "Events w/o Rolu and with ToFI      : " << fNEvents_tofi << endl;
        outfile << setprecision(10) << "Events w/o Rolu and with fibers23  : " << fNEvents_fibers << endl;
        outfile << setprecision(10) << "Events with two particles          : " << fNEvents_pair << endl;
        outfile << setprecision(10) << "Time duration                      : " << (time_end - time_begin) / 1.e9
                << " sec" << endl;
        outfile << setprecision(10) << "nSpill                             : " << fNSpills << endl;
        outfile << setprecision(10) << "Total num of 18O                   : " << nBeamParticle << endl;
        outfile << setprecision(10) << "Total off-spill particles          : " << nOffSpillParticle << endl;
        outfile << setprecision(10) << "Wrong Trigger                      : " << counterWrongTrigger << endl;
        outfile << setprecision(10) << "Wrong Tpat                         : " << counterWrongTpat << endl;
    }

    outfile.close();

    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        if (fHitItems.at(DET_FI_FIRST + ifibcount))
        {

            fh_Fib_ToF[ifibcount]->Write();
            fh_xFib_vs_xTofd[ifibcount]->Write();
            fh_yFib_vs_yTofd[ifibcount]->Write();
            fh_qFib_vs_qTofd[ifibcount]->Write();
            fh_ToF_vs_Events[ifibcount]->Write();
            fh_x_vs_Events[ifibcount]->Write();
            fh_Fib_ToF_ac[ifibcount]->Write();
            fh_xFib_vs_xTofd_ac[ifibcount]->Write();
            fh_yFib_vs_yTofd_ac[ifibcount]->Write();
            fh_qFib_vs_qTofd_ac[ifibcount]->Write();
        }
    }
    fh_xFib31_vs_xFib33->Write();
    fh_yFib31_vs_yFib33->Write();
    fh_qFib31_vs_qFib33->Write();
    fh_tFib31_diff_tFib33->Write();
    fh_xFib31_vs_xFib33_ac->Write();
    fh_yFib31_vs_yFib33_ac->Write();
    fh_qFib31_vs_qFib33_ac->Write();
    fh_tFib31_diff_tFib33_ac->Write();

    fh_xFib23a_vs_xFib31->Write();
    fh_yFib23a_vs_yFib31->Write();
    fh_qFib23a_vs_qFib31->Write();
    fh_tFib23a_diff_tFib31->Write();

    fh_xFib23a_vs_xFib31_ac->Write();
    fh_yFib23a_vs_yFib31_ac->Write();
    fh_qFib23a_vs_qFib31_ac->Write();
    fh_tFib23a_diff_tFib31_ac->Write();

    fh_xFib23b_vs_xFib31->Write();
    fh_yFib23b_vs_yFib31->Write();
    fh_qFib23b_vs_qFib31->Write();
    fh_tFib23b_diff_tFib31->Write();

    fh_xFib23b_vs_xFib31_ac->Write();
    fh_yFib23b_vs_yFib31_ac->Write();
    fh_qFib23b_vs_qFib31_ac->Write();
    fh_tFib23b_diff_tFib31_ac->Write();

    fh_xFib30_vs_xFib32->Write();
    fh_yFib30_vs_yFib32->Write();
    fh_qFib30_vs_qFib32->Write();
    fh_tFib30_diff_tFib32->Write();

    fh_xFib30_vs_xFib32_ac->Write();
    fh_yFib30_vs_yFib32_ac->Write();
    fh_qFib30_vs_qFib32_ac->Write();
    fh_tFib30_diff_tFib32_ac->Write();

    fh_xFib23a_vs_xFib30->Write();
    fh_yFib23a_vs_yFib30->Write();
    fh_qFib23a_vs_qFib30->Write();
    fh_tFib23a_diff_tFib30->Write();

    fh_xFib23a_vs_xFib30_ac->Write();
    fh_yFib23a_vs_yFib30_ac->Write();
    fh_qFib23a_vs_qFib30_ac->Write();
    fh_tFib23a_diff_tFib30_ac->Write();

    fh_xFib23b_vs_xFib30->Write();
    fh_yFib23b_vs_yFib30->Write();
    fh_qFib23b_vs_qFib30->Write();
    fh_tFib23b_diff_tFib30->Write();

    fh_xFib23b_vs_xFib30_ac->Write();
    fh_yFib23b_vs_yFib30_ac->Write();
    fh_qFib23b_vs_qFib30_ac->Write();
    fh_tFib23b_diff_tFib30_ac->Write();

    fh_ztofd->Write();
    fh_ztofd_ac->Write();
    fh_xy_tofd->Write();
    fh_time_tofd[0]->Write();
    fh_time_tofd[1]->Write();
    fh_Z_vs_x->Write();
    fh_Z_vs_time->Write();
    fh_Z_vs_dthit->Write();
    fh_dqvsdt_Tofd->Write();
    fh_ql_vs_qr_Tofd->Write();
    fh_xl_vs_xr_Tofd->Write();
    fh_xy_target->Write();
    fh_ql_vs_qr_Tofd_Zsum8->Write();
    fh_dt_Tofd_Zsum8->Write();
    fh_xl_vs_xr_Tofd_Zsum8->Write();
    fh_xy_target_Zsum8->Write();
    fh_qplane1_vs_qplane2_l->Write();
    fh_qplane1_vs_qplane2_r->Write();
    fh_qr_Tofd->Write();
    fh_xr_Tofd->Write();
    fh_ql_Tofd->Write();
    fh_xl_Tofd->Write();
    fh_q_vs_tofFib23aTofd->Write();

    fh_Trigger->Write();
    fh_Tpat->Write();
}

ClassImp(R3BGlobalCorrelationsS494)
