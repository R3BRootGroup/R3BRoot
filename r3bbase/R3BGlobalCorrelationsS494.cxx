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
    , fwindow_mv(10000)
    , fxmin(-1000)
    , fxmax(1000)
    , ftofminFib2x(T_TOF_MIN)
    , ftofmaxFib2x(T_TOF_MAX)
    , ftofminFib3x(T_TOF_MIN)
    , ftofmaxFib3x(T_TOF_MAX)
    , ftofminTofi(T_TOF_MIN)
    , ftofmaxTofi(T_TOF_MAX)
    , fqtofdmin(0)
    , fqtofdmax(200)
    , fNEvents(0)
    , fMappedItemsCalifa(NULL)

{
}

R3BGlobalCorrelationsS494::R3BGlobalCorrelationsS494(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat1(-1)
    , fTpat2(-1)
    , fCuts(0)
    , fVeto(false)
    , fwindow_mv(10000)
    , fxmin(-1000)
    , fxmax(1000)
    , ftofminFib2x(T_TOF_MIN)
    , ftofmaxFib2x(T_TOF_MAX)
    , ftofminFib3x(T_TOF_MIN)
    , ftofmaxFib3x(T_TOF_MAX)
    , ftofminTofi(T_TOF_MIN)
    , ftofmaxTofi(T_TOF_MAX)
    , fqtofdmin(0)
    , fqtofdmax(200)
    , fNEvents(0)
    , fMappedItemsCalifa(NULL)
{
}

R3BGlobalCorrelationsS494::~R3BGlobalCorrelationsS494()
{
    for (int i = 0; i < NOF_FIB_DET; i++)
    {
        delete fh_ToT_Fib[i];
        delete fh_xy_Fib[i];
        delete fh_xy_Fib_ac[i];
        delete fh_Fib_ToF[i];
        delete fh_Fib_ToF_ac[i];
        delete fh_ToT_Fib[i];
        delete fh_ToT_Fib_ac[i];
        delete fh_Fibs_vs_Tofd[i];
        delete fh_Fibs_vs_Tofd_ac[i];
        delete fh_ToF_vs_Events[i];
    }

    delete fh_Tofi_ToF;
    delete fh_ToT_Tofi_ac;
    delete fh_xy_Tofi_ac;
    delete fh_ToT_Tofi;
    for (Int_t i = 0; i < 7; i++)
    {
        delete fh_xy_Tofi[i];
    }

    for (Int_t i = 0; i < 2; i++)
    {
        delete fh_ToT_Rolu_ac[i];
        delete fh_ToT_Rolu[i];
        delete fh_Rolu_ToF[i];
    }
}

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
    fMappedItemsCalifa = (TClonesArray*)mgr->GetObject("CalifaMappedData");
    if (!fMappedItemsCalifa)
    {
        LOG(WARNING) << "R3BOnlineSpectra: CalifaMappedData not found";
    }

    //-----------------------------------------------------------------------
    // Fiber Detectors 1-NOF_FIB_DET

    char canvName[255];
    UInt_t Nmax = 1e7;
    TCanvas* cFib = new TCanvas("Fib", "Fib", 10, 10, 910, 910);
    cFib->Divide(8, 4);

    Double_t fexp = float(fsens_SEE + 9);
    Double_t fpow = float(pow(10., fexp));
    calib_SEE = 104457.9 * fpow;
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

    TCanvas* cbmon = new TCanvas("Beam_Monitor", "Beam Monitors", 820, 10, 900, 300);
    cbmon->Divide(3, 1);
    cbmon->cd(1);
    fh_SEE_spill->Draw("hist");
    cbmon->cd(2);
    fh_IC_spill->Draw("hist");
    cbmon->cd(3);
    fh_IC_SEE->SetMarkerStyle(21);
    fh_IC_SEE->Draw("hist p");

    cbmon->cd(0);

    //    run->AddObject(cbmon);

    //  run->GetHttpServer()->RegisterCommand("Reset_BMON", Form("/Tasks/%s/->Reset_BMON_Histo()", GetName()));

    if (fHitItems.at(DET_TOFI))
    {
        // ToF Tofd -> Fiber:
        fh_Tofi_ToF = new TH2F("t1_t2", "t1-t2", 2000, -1000., 1000., 400, 0, 20);
        fh_Tofi_ToF->GetXaxis()->SetTitle("t1-t2 / ns");
        fh_Tofi_ToF->GetYaxis()->SetTitle("Z1+Z2");

        fh_Tofi_ToF_ac = new TH1F("t1_t2_ac", "t1-t2 after cuts", 2000, -1000., 1000.);
        fh_Tofi_ToF_ac->GetXaxis()->SetTitle("t1-t2 / ns");
        fh_Tofi_ToF_ac->GetYaxis()->SetTitle("counts");

        fh_ToT_Tofi = new TH2F("Z1_vs_Z2", "Z1 vs Z2 ", 400, 0., 20., 400, 0., 20.);
        fh_ToT_Tofi->GetXaxis()->SetTitle("Z1");
        fh_ToT_Tofi->GetYaxis()->SetTitle("Z2");

        fh_ToT_Tofi_ac = new TH2F("Zsum_eq_8", "Z1 + Z2 = 8", 400, 0., 20., 400, 0., 20.);
        fh_ToT_Tofi_ac->GetXaxis()->SetTitle("Z1");
        fh_ToT_Tofi_ac->GetYaxis()->SetTitle("Z2");

        for (Int_t i = 0; i < 7; i++)
        {
            fh_xy_Tofi[i] =
                new TH2F(Form("X_vs_Dx_Z%d", i + 2), Form("x vs Dx for z%d", i + 2), 1600, -80., 80., 1200, -60., 60.);
            fh_xy_Tofi[i]->GetYaxis()->SetTitle("Dx fib3x / cm ");
            fh_xy_Tofi[i]->GetXaxis()->SetTitle("x / cm");
        }

        fh_xy_Tofi_ac = new TH2F("Z_vs_Dx_x", "Z vs Dx/x", 400, 0., 20., 2000, -1., 1.);
        fh_xy_Tofi_ac->GetYaxis()->SetTitle("Dx/x fib3x");
        fh_xy_Tofi_ac->GetXaxis()->SetTitle("Nuclear charge ");

        fh_Z_vs_x = new TH2F("Z_vs_x", "Z vs x ", 1600, -80., 80, 400, 0., 20.);
        fh_Z_vs_x->GetXaxis()->SetTitle("x / cm");
        fh_Z_vs_x->GetYaxis()->SetTitle("Z");

        fh_Z_vs_dthit = new TH2F("Z_vs_dthit", "Z vs dthit", 2400, -400., 800, 400, 0., 20.);
        fh_Z_vs_dthit->GetXaxis()->SetTitle("time between two hits / ns");
        fh_Z_vs_dthit->GetYaxis()->SetTitle("Z");

        fh_ztofi = new TH2F("Z_vs_x_tofi", "Z vs x toFI", 200, -10., 10, 400, 0., 20.);
        fh_ztofi->GetXaxis()->SetTitle("x / cm");
        fh_ztofi->GetYaxis()->SetTitle("Z");
    }
    fh_ztofd = new TH2F("Z_vs_y_tofd", "Z vs y toFD", 1600, -80., 80, 400, 0., 20.);
    fh_ztofd->GetXaxis()->SetTitle("y / cm");
    fh_ztofd->GetYaxis()->SetTitle("Z");

    for (Int_t i = 0; i < 3; i++)
    {
        fh_time_tofd[i] = new TH1F(Form("timeToFD%d", i + 1), Form("TimeToFD%d", i + 1), 2000, -1000., 1000.);
        fh_time_tofd[i]->GetXaxis()->SetTitle("time / ns");
        fh_time_tofd[i]->GetYaxis()->SetTitle("counts");
    }

    fh_yy = new TH2F("yToFD_vs_yFib23b", "yToFD vs yFib23b", 320, -80., 80., 1600, -80., 80);
    fh_yy->GetXaxis()->SetTitle("yFib23b / cm ");
    fh_yy->GetYaxis()->SetTitle("yToFD / cm ");

    fh_xy_tofd = new TH2F("yToFD_vs_xToFD", "yToFD vs xToFD", 1600, -80., 80., 1600, -80., 80);
    fh_xy_tofd->GetXaxis()->SetTitle("xToFD / cm ");
    fh_xy_tofd->GetYaxis()->SetTitle("yToFD / cm ");

    if (fCalItems.at(DET_ROLU))
    {
        for (Int_t i = 0; i < 2; i++)
        {
            fh_Rolu_ToF[i] =
                new TH2F(Form("timeRolu%d", i + 1), Form("timeRolu%d", i + 1), 5, 0, 5, 12000, -6000., 6000.);
            fh_Rolu_ToF[i]->GetYaxis()->SetTitle("Time / ns");
            fh_Rolu_ToF[i]->GetXaxis()->SetTitle("Channel");

            fh_ToT_Rolu[i] = new TH2F(
                Form("totRolu%d_vs_Ztofd", i + 1), Form("totRolu%d vs Ztofd", i + 1), 200, 0., 20., 600, 0., 300.);
            fh_ToT_Rolu[i]->GetXaxis()->SetTitle("Nuclear charge ToFD");
            fh_ToT_Rolu[i]->GetYaxis()->SetTitle("Rolu ToT / ns");

            fh_ToT_Rolu_ac[i] = new TH2F(Form("totRolu%d_vs_Ztofd_ac", i + 1),
                                         Form("totRolu%d vs Ztofd ac", i + 1),
                                         200,
                                         0.,
                                         20.,
                                         600,
                                         0.,
                                         300.);
            fh_ToT_Rolu_ac[i]->GetXaxis()->SetTitle("Nuclear charge ToFD");
            fh_ToT_Rolu_ac[i]->GetYaxis()->SetTitle("Rolu ToT / ns");
        }
    }

    fh_xy_target_ac =
        new TH2F("xtarget_vs_ytarget_Zsum8", "xtarget vs ytarget for Z1+Z2=8", 1200, -6., 6., 1200, -6., 6);
    fh_xy_target_ac->GetXaxis()->SetTitle("xtarget / cm ");
    fh_xy_target_ac->GetYaxis()->SetTitle("ytarget / cm ");

    fh_xy_target = new TH2F("xtarget_vs_ytarget", "xtarget vs ytarget", 1200, -6., 6., 1200, -6., 6);
    fh_xy_target->GetXaxis()->SetTitle("xtarget / cm ");
    fh_xy_target->GetYaxis()->SetTitle("ytarget / cm ");

    fh_Fib32_ToF = new TH2F("tfib32_vs_ttofd", "tfib32 vs ttofd", 1000, -500, 500., 1000, -500., 500);
    fh_Fib32_ToF->GetXaxis()->SetTitle("time fi32 /ns");
    fh_Fib32_ToF->GetYaxis()->SetTitle("time tofd / ns ");

    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        // cout<<"In the loop for ifibcout= "<<ifibcount<<endl;

        const char* detName;
        const char* detName2;
        detName = fDetectorNames[DET_FI_FIRST + ifibcount];

        // xy:
        fh_xy_Fib[ifibcount] =
            new TH2F(Form("%s_xToFDZfib", detName), Form("%s Zfib vs xToFD", detName), 1200, -60., 60., 200, 0., 20.);
        fh_xy_Fib[ifibcount]->GetXaxis()->SetTitle("xToFD / cm ");
        fh_xy_Fib[ifibcount]->GetYaxis()->SetTitle("Nuclear charge fiber");

        fh_xy_Fib_ac[ifibcount] = new TH2F(Form("%s_xToFDZfib_ac", detName),
                                           Form("%s Zfib vs xToFD after cuts", detName),
                                           1200,
                                           -60.,
                                           60.,
                                           200,
                                           0.,
                                           20.);
        fh_xy_Fib_ac[ifibcount]->GetXaxis()->SetTitle("xToFD / cm ");
        fh_xy_Fib_ac[ifibcount]->GetYaxis()->SetTitle("Nuclear charge fiber");

        // ToT vs xToFD:
        fh_ToT_Fib[ifibcount] =
            new TH2F(Form("%s_Zfib_vs_Ztofd", detName), Form("%s Zfib vs Ztofd", detName), 200, 0., 20., 200, 0., 20.);
        fh_ToT_Fib[ifibcount]->GetXaxis()->SetTitle("Nuclear charge ToFD");
        fh_ToT_Fib[ifibcount]->GetYaxis()->SetTitle("Nuclear charge fiber");

        fh_ToT_Fib_ac[ifibcount] = new TH2F(Form("%s_Zfib_vs_Ztofd_ac", detName),
                                            Form("%s Zfib vs Ztofd after cuts", detName),
                                            200,
                                            0.,
                                            20.,
                                            200,
                                            0.,
                                            20.);
        fh_ToT_Fib_ac[ifibcount]->GetXaxis()->SetTitle("Nuclear charge ToFD");
        fh_ToT_Fib_ac[ifibcount]->GetYaxis()->SetTitle("Nuclear charge fiber");

        fh_ToT_TOF_Fib_ac[ifibcount] = new TH2F(Form("%s_TOF_vs_xFib_ac", detName),
                                                Form("%s TOF vs xFib after cuts", detName),
                                                600,
                                                -30.,
                                                30.,
                                                1200,
                                                -100.,
                                                500.);
        fh_ToT_TOF_Fib_ac[ifibcount]->GetXaxis()->SetTitle("xFib / cm");
        fh_ToT_TOF_Fib_ac[ifibcount]->GetYaxis()->SetTitle("ToF / ns");

        // ToF Tofd -> Fiber:
        fh_Fib_ToF[ifibcount] =
            new TH1F(Form("%s_tof", detName), Form("%s ToF Tofd to Fiber", detName), 4000, -2000., 2000.);
        fh_Fib_ToF[ifibcount]->GetXaxis()->SetTitle("ToF / ns");
        fh_Fib_ToF[ifibcount]->GetYaxis()->SetTitle("counts");

        fh_Fib_ToF_ac[ifibcount] =
            new TH1F(Form("%s_tof_ac", detName), Form("%s ToF Tofd to Fiber after cuts", detName), 4000, -2000., 2000.);
        fh_Fib_ToF_ac[ifibcount]->GetXaxis()->SetTitle("ToF / ns");
        fh_Fib_ToF_ac[ifibcount]->GetYaxis()->SetTitle("counts");

        // ToF Tofd -> Fiber vs. event number:
        fh_ToF_vs_Events[ifibcount] = new TH2F(Form("%s_tof_vs_events", detName),
                                               Form("%s ToF Tofd to Fiber vs event number", detName),
                                               10000,
                                               0,
                                               Nmax,
                                               4000,
                                               -2000,
                                               2000);
        fh_ToF_vs_Events[ifibcount]->GetYaxis()->SetTitle("ToF / ns");
        fh_ToF_vs_Events[ifibcount]->GetXaxis()->SetTitle("event number");

        // xfib vs. TofD position:
        if (ifibcount < 2)
            fh_Fibs_vs_Tofd[ifibcount] = new TH2F(Form("%s_fib_vs_TofdX", detName),
                                                  Form("%s Fiber # vs. Tofd x-pos", detName),
                                                  400,
                                                  -60,
                                                  60,
                                                  120,
                                                  -6.,
                                                  6.);
        else
            fh_Fibs_vs_Tofd[ifibcount] = new TH2F(Form("%s_fib_vs_TofdX", detName),
                                                  Form("%s Fiber # vs. Tofd x-pos", detName),
                                                  400,
                                                  -60,
                                                  60,
                                                  600,
                                                  -30.,
                                                  30.);

        fh_Fibs_vs_Tofd[ifibcount]->GetYaxis()->SetTitle("Fiber x / cm");
        fh_Fibs_vs_Tofd[ifibcount]->GetXaxis()->SetTitle("Tofd x / cm");

        if (ifibcount < 2)
            fh_Fibs_vs_Tofd_ac[ifibcount] = new TH2F(Form("%s_fib_vs_TofdX_ac", detName),
                                                     Form("%s Fiber # vs. Tofd x-pos after cuts", detName),
                                                     400,
                                                     -60,
                                                     60,
                                                     120,
                                                     -6.,
                                                     6.);
        else
            fh_Fibs_vs_Tofd_ac[ifibcount] = new TH2F(Form("%s_fib_vs_TofdX_ac", detName),
                                                     Form("%s Fiber # vs. Tofd x-pos after cuts", detName),
                                                     400,
                                                     -60,
                                                     60,
                                                     600,
                                                     -30.,
                                                     30.);

        fh_Fibs_vs_Tofd_ac[ifibcount]->GetYaxis()->SetTitle("Fiber x / cm");
        fh_Fibs_vs_Tofd_ac[ifibcount]->GetXaxis()->SetTitle("Tofd x / cm");

    } // end for(ifibcount)

    fhTpat = new TH1F("Tpat", "Tpat", 20, 0, 20);
    fhTpat->GetXaxis()->SetTitle("Tpat value");

    cFib->cd(1);
    // gPad->SetLogy();
    fh_Fib_ToF[0]->Draw();
    cFib->cd(9);
    gPad->SetLogz();
    fh_xy_Tofi[0]->Draw("colz");
    cFib->cd(17);
    gPad->SetLogz();
    fh_Tofi_ToF->Draw("colz");
    gPad->SetLogz();
    cFib->cd(25);
    gPad->SetLogz();
    fh_ztofd->Draw("colz");

    cFib->cd(2);
    // gPad->SetLogy();
    fh_Fib_ToF[1]->Draw();
    cFib->cd(10);
    gPad->SetLogz();
    fh_xy_Tofi[1]->Draw("colz");
    cFib->cd(18);
    gPad->SetLogz();
    fh_Tofi_ToF_ac->Draw();
    cFib->cd(26);
    gPad->SetLogz();
    fh_yy->Draw("colz");

    cFib->cd(3);
    //  gPad->SetLogy();
    fh_Fib_ToF[2]->Draw();
    cFib->cd(11);
    gPad->SetLogz();
    fh_xy_Tofi[2]->Draw("colz");
    cFib->cd(19);
    // gPad->SetLogz();
    fh_time_tofd[0]->Draw();
    cFib->cd(27);
    gPad->SetLogz();
    fh_xy_tofd->Draw("colz");

    cFib->cd(4);
    // gPad->SetLogy();
    fh_Fib_ToF[3]->Draw();
    cFib->cd(12);
    gPad->SetLogz();
    fh_xy_Tofi[3]->Draw("colz");
    cFib->cd(20);
    gPad->SetLogz();
    // gPad->SetLogz();
    fh_time_tofd[1]->Draw();
    cFib->cd(28);

    cFib->cd(5);
    //  gPad->SetLogy();
    fh_Fib_ToF[4]->Draw();
    cFib->cd(13);
    gPad->SetLogz();
    fh_xy_Tofi[4]->Draw("colz");
    cFib->cd(21);
    // gPad->SetLogz();
    fh_time_tofd[2]->Draw();
    cFib->cd(29);

    cFib->cd(6);
    //  gPad->SetLogy();
    fh_Fib_ToF[5]->Draw();
    cFib->cd(14);
    gPad->SetLogz();
    fh_xy_Tofi[5]->Draw("colz");
    cFib->cd(22);
    gPad->SetLogz();
    /*  if (fCuts)
          fh_Fibs_vs_Tofd_ac[5]->Draw("colz");
      else
          fh_Fibs_vs_Tofd[5]->Draw("colz");
      cFib->cd(30);
      fh_counter_fi33->Draw("colz");*/

    cFib->cd(7);
    gPad->SetLogz();
    fh_ToT_Tofi->Draw("colz");
    cFib->cd(15);
    gPad->SetLogz();
    fh_xy_Tofi[6]->Draw("colz");
    cFib->cd(23);
    gPad->SetLogz();
    fh_xy_Tofi_ac->Draw("colz");
    cFib->cd(31);
    gPad->SetLogz();
    fh_xy_target->Draw("colz");

    cFib->cd(8);
    gPad->SetLogz();
    fh_ToT_Tofi_ac->Draw("colz");
    cFib->cd(16);
    gPad->SetLogz();
    fh_Z_vs_x->Draw("colz");
    cFib->cd(24);
    gPad->SetLogz();
    fh_Z_vs_dthit->Draw("colz");
    cFib->cd(32);
    gPad->SetLogz();
    fh_xy_target_ac->Draw("colz");

    cFib->cd(0);

    //   cout<<"****** Finished with spectra def ******* "<<endl;
    //   run->AddObject(cFib);
    //  run->GetHttpServer()->RegisterCommand("Reset_Fib", Form("/Tasks/%s/->Reset_All()", GetName()));

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

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    time = header->GetTimeStamp();
    //   if (time == 0 && time > 1.e19)
    //  return;

    fNEvents_total += 1;

    if (time_begin == -1 && time > 0 && time_end < 1)
    {
        time_begin = time;
    }

    if (time_start == -1 && time > 0)
    {
        time_start = time;
        fNEvents_start = fNEvents_total;
    }

    if (header->GetTrigger() == 12)
    {
        time_spill_start = time; // header->GetTimeStamp();    // spill start in nsec
        cout << "Spill start: " << double(time_spill_start - time_begin) / 1.e9 << " sec " << endl;
        // reset counters:
        see_spill = 0;
        ic_spill = 0;
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

    Int_t tpatbin;
    for (int i = 0; i < 16; i++)
    {
        tpatbin = (header->GetTpat() & (1 << i));
        if (tpatbin != 0)
            fhTpat->Fill(i + 1);
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
                    nOffSpillParticle += ySEE_part * 1e3; // / spill_length;
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
    Int_t tofi = 6;
    Int_t tofd1r = 7;
    Int_t tofd1l = 8;
    Int_t tofd2r = 9;
    Int_t tofd2l = 10;
    Int_t tofdgoodl = 11;
    Int_t tofdgoodr = 12;

    Double_t tof = 0.;
    Double_t tStart = 0.;
    Bool_t first = true;
    Double_t time_tofd_mem = 0. / 0.;
    Double_t randx;
    Bool_t debug_in = false;

    nHitstemp = 0;

    fNEvents += 1;
    //  if (fNEvents / 10000. == (int)fNEvents / 10000)
    //      std::cout << "\rEvents: " << fNEvents << " / " << maxevent << " (" << (int)(fNEvents * 100. / maxevent)
    //               << " %) " << std::flush;

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
        }
    }

    //    cout<<"TOFD nHits: "<<nHits<<endl;

    if (fHitItems.at(DET_ROLU)->GetEntriesFast() < 1 && fHitItems.at(DET_TOFD)->GetEntriesFast() > 0)
        fNEvents_local += 1;
    if (fHitItems.at(DET_ROLU)->GetEntriesFast() < 1 && fHitItems.at(DET_TOFD)->GetEntriesFast() < 1)
        fNEvents_zeroToFD += 1;
    if (fHitItems.at(DET_ROLU)->GetEntriesFast() < 1 && fMappedItemsCalifa && fMappedItemsCalifa->GetEntriesFast() > 0)
        fNEvents_califa += 1;
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

    if (nHits > 0)
    {
        counterTofd++;
        if (debug_in)
        {
            cout << "********************************" << endl;
            cout << "ToFD hits: " << nHits << endl;
        }
        // }

        // if (nHits > 100)
        //   return;
        // create tofd_ && tofdr hits

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

                //	if (fCuts && (t1[det] < -20 || t1[det] > 20))
                //	continue;
                if ((hit23b->GetFiberId() > 188 && hit23b->GetFiberId() < 197))
                    continue;

                if (fCuts && (tof < ftofminFib2x || tof > ftofmaxFib2x))
                    continue;
            }

            Bool_t tofd_left = false;
            Bool_t tofd_right = false;
            id2 = hitTofd->GetDetId();

            if ((id2 == 2 && hitTofd->GetBarId() == 24) || (id2 == 2 && hitTofd->GetBarId() == 21))
                continue;

            if (xxx <= 0.) // hitTofd->GetX() < 0.)
            {
                // tof rechts
                tofd_right = true;
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
                    det = tofdgoodr;
                }
            }
            else
            {
                // tof links
                counterTofdMultil++;
                tofd_left = true;
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
                    det = tofdgoodl;
                }
            }

            counterTofdMulti++;
        }
        Double_t xl[100], tl[100], yl[100], ql[100];
        Double_t xr[100], tr[100], yr[100], qr[100];
        for (Int_t i = 0; i < 100; i++)
        {
            xl[i] = 0. / 0.;
            yl[i] = 0. / 0.;
            tl[i] = 0. / 0.;
            ql[i] = 0. / 0.;
            xr[i] = 0. / 0.;
            yr[i] = 0. / 0.;
            tr[i] = 0. / 0.;
            qr[i] = 0. / 0.;
        }

        // create goot tofdl and tofdr hits
        Int_t nSuml = 0;
        for (Int_t il2 = 0; il2 < tofd2l; il2++)
        {
            for (Int_t il1 = 0; il1 < tofd1l; il1++)
            {
                if (std::abs(ttemp[tofd2l][il2] - ttemp[tofd1l][il1]) < 20. &&
                    std::abs(xtemp[tofd2l][il2] - xtemp[tofd1l][il1]) < 2. &&
                    std::abs(qtemp[tofd2l][il2] - qtemp[tofd1l][il1]) < 1.)
                {
                    xl[nSuml] = (xtemp[tofd2l][il2] + xtemp[tofd1l][il1]) / 2.;
                    tl[nSuml] = (ttemp[tofd2l][il2] + ttemp[tofd1l][il1]) / 2.;
                    yl[nSuml] = (ytemp[tofd2l][il2] + ytemp[tofd1l][il1]) / 2.;
                    ql[nSuml] = (qtemp[tofd2l][il2] + qtemp[tofd1l][il1]) / 2.;
                    nSuml += 1;
                }
            }
        }
        Int_t nSumr = 0;
        for (Int_t ir2 = 0; ir2 < tofd2r; ir2++)
        {
            for (Int_t ir1 = 0; ir1 < tofd1r; ir1++)
            {
                if (std::abs(ttemp[tofd2r][ir2] - ttemp[tofd1r][ir1]) < 20. &&
                    std::abs(xtemp[tofd2r][ir2] - xtemp[tofd1r][ir1]) < 2. &&
                    std::abs(qtemp[tofd2r][ir2] - qtemp[tofd1r][ir1]) < 1. && xtemp[tofd2r][ir2] <= 0. &&
                    xtemp[tofd1r][ir1] <= 0.)
                {
                    xr[nSumr] = (xtemp[tofd2r][ir2] + xtemp[tofd1r][ir1]) / 2.;
                    tr[nSumr] = (ttemp[tofd2r][ir2] + ttemp[tofd1r][ir1]) / 2.;
                    yr[nSumr] = (ytemp[tofd2r][ir2] + ytemp[tofd1r][ir1]) / 2.;
                    qr[nSumr] = (qtemp[tofd2r][ir2] + qtemp[tofd1r][ir1]) / 2.;
                    nSumr += 1;
                }
            }
        }
        // add to tofdr / tofdl hits tofi hits

        // loop over TOFI
        /*     if (fHitItems.at(DET_TOFI))
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
                  nSumr = nSumr+icr;
                  nSuml = nSuml+icl;
              }*/
        Double_t tmem = 0;
        for (Int_t ihitr = 0; ihitr < nSumr; ihitr++)
        {
            fh_Z_vs_x->Fill(xr[ihitr], qr[ihitr]);
            if (ihitr > 0)
                fh_Z_vs_dthit->Fill(tr[ihitr] - tmem, qr[ihitr]);
            tmem = tr[ihitr];
        }
        tmem = 0;
        for (Int_t ihitl = 0; ihitl < nSuml; ihitl++)
        {
            fh_Z_vs_x->Fill(xl[ihitl], ql[ihitl]);
            if (ihitl > 0)
                fh_Z_vs_dthit->Fill(tl[ihitl] - tmem, ql[ihitl]);
            tmem = tl[ihitl];
        }

        // correlations left-right tofd / fib23
        for (Int_t ihitr = 0; ihitr < nSumr; ihitr++)
        {
            for (Int_t ihitl = 0; ihitl < nSuml; ihitl++)
            {
                // fh_Tofi_ToF->Fill(tr[ihitr]-tl[ihitl],qr[ihitr]+ql[ihitl]);
                fh_Tofi_ToF->Fill(ttraw[tofd1r][ihitr] - ttraw[tofd1l][ihitl], qr[ihitr] + ql[ihitl]);
                if ((tr[ihitr] - tl[ihitl]) > -1500. && (tr[ihitr] - tl[ihitl]) < 3000)
                {
                    fh_ToT_Tofi->Fill(qr[ihitr], ql[ihitl]);

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

                        //	if (fCuts && (t1[det] < -20 || t1[det] > 20))
                        //	continue;
                        if ((hit23a->GetFiberId() > 188 && hit23a->GetFiberId() < 197))
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

                            //	if (fCuts && (t1[det] < -20 || t1[det] > 20))
                            //	continue;
                            if ((hit23b->GetFiberId() > 188 && hit23b->GetFiberId() < 197))
                                continue;
                            if (fCuts && (tof < ftofminFib2x || tof > ftofmaxFib2x))
                                continue;

                            fh_xy_target->Fill(x1[fi23a], y1[fi23b]);
                        }
                    }

                    if ((qr[ihitr] + ql[ihitl]) > 7.5 && (qr[ihitr] + ql[ihitl]) < 8.5)
                    {
                        fh_ToT_Tofi_ac->Fill(qr[ihitr], ql[ihitl]);
                        fh_Tofi_ToF_ac->Fill(tr[ihitr] - tl[ihitl]);
                        // fh_xy_Tofi_ac->Fill(xr[ihitr],xl[ihitl]);
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
                            //	if (fCuts && (t1[det] < -20 || t1[det] > 20))
                            //	continue;
                            if ((hit23a->GetFiberId() > 188 && hit23a->GetFiberId() < 197))
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

                                //	if (fCuts && (t1[det] < -20 || t1[det] > 20))
                                //	continue;
                                if ((hit23b->GetFiberId() > 188 && hit23b->GetFiberId() < 197))
                                    continue;
                                if (fCuts && (tof < ftofminFib2x || tof > ftofmaxFib2x))
                                    continue;

                                fh_xy_target_ac->Fill(x1[fi23a], y1[fi23b]);
                            }
                        }
                    }
                }
            }
        }

        Double_t xxx, yyy, qqq;
        // ***********************************************
        // loop over right side
        // ***********************************************
        for (Int_t ihit = 0; ihit < nSumr; ihit++)
        {
            xxx = xr[ihit];
            qqq = qr[ihit];
            yyy = yr[ihit];
            tStart = tr[ihit];
            if (fCuts && (qqq < 0 || qqq > 20))
            {
                // cout << "Cut because of charge: " << qqq << endl;
                continue;
            }
            if (fCuts && (xxx < -10000 || xxx > 10000))
            {
                // cout << "Cut because of x" << endl;
                continue;
            }

            // cut in ToT for Fibers
            Double_t cutQ = 0.;

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
                y1[det] = hit33->GetY();               // cm
                z1[det] = 0.;
                q1[det] = hit33->GetEloss();

                t1[det] = hit33->GetTime();
                tof = tStart - t1[det];

                // Fill histograms before cuts
                fh_Fib_ToF[det]->Fill(tof);
                fh_Fib32_ToF->Fill(t1[det], tStart);
                fh_xy_Fib[det]->Fill(xxx, q1[det]);
                fh_ToT_Fib[det]->Fill(qqq, q1[det]);
                fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                fh_Fibs_vs_Tofd[det]->Fill(xxx, x1[det]);

                hits33bc++;

                // Cuts on Fi33

                /*   if (fCuts && (x1[det] * 100. < -10000 || x1[det] * 100. > 10000))
                        continue;
                    if (fCuts && (y1[det] < -10000 || y1[det] > 10000))
                        continue; */
                // if (fCuts && std::abs(q1[det]-qqq) > 2.)
                //       continue;
                if (fCuts && (tof < ftofminFib3x || tof > ftofmaxFib3x))
                    continue;

                hits33++;

                // Fill histograms after cuts
                fh_Fib_ToF_ac[det]->Fill(tof);
                fh_xy_Fib_ac[det]->Fill(xxx, q1[det]);
                fh_ToT_Fib_ac[det]->Fill(qqq, q1[det]);
                fh_Fibs_vs_Tofd_ac[det]->Fill(xxx, x1[det]);
                fh_ToT_TOF_Fib_ac[det]->Fill(x1[det], t1[det]); // tof);

                if (debug2)
                    cout << "Fi33: " << ihit33 << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                         << " t1: " << t1[det] << endl;

                if (qqq > 1.5 && qqq < 2.5)
                    fh_yy->Fill(y1[fi33], yyy);

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
                    y1[det] = hit31->GetY();
                    z1[det] = 0.;
                    q1[det] = hit31->GetEloss();
                    t1[det] = hit31->GetTime();
                    tof = tStart - t1[det];

                    // Fill histograms before cuts
                    fh_xy_Fib[det]->Fill(xxx, q1[det]);
                    fh_Fib_ToF[det]->Fill(tof);
                    fh_ToT_Fib[det]->Fill(qqq, q1[det]);
                    fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                    fh_Fibs_vs_Tofd[det]->Fill(xxx, x1[det]);
                    hits31bc++;

                    // Cuts on Fi31

                    /*   if (fCuts && (x1[det] * 100. < -10000 || x1[det] * 100. > 10000))
                            continue;
                        if (fCuts && (y1[det] < -10000 || y1[det] > 10000))
                            continue; */
                    // if (fCuts && std::abs(q1[det]-qqq) > 2.)
                    //       continue;
                    if (fCuts && (tof < ftofminFib3x || tof > ftofmaxFib3x))
                        continue;

                    hits31++;

                    // Fill histograms
                    fh_Fib_ToF_ac[det]->Fill(tof);
                    fh_xy_Fib_ac[det]->Fill(xxx, q1[det]);
                    fh_ToT_Fib_ac[det]->Fill(qqq, q1[det]);
                    fh_Fibs_vs_Tofd_ac[det]->Fill(xxx, x1[det]);
                    fh_ToT_TOF_Fib_ac[det]->Fill(x1[det], tof);

                    if (debug2)
                        cout << "Fi31: " << ihit31 << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                             << " t1: " << t1[det] << endl;

                    for (Int_t iCharge = 0; iCharge < 7; iCharge++)
                    {
                        if (qr[ihit] > (float(iCharge) + 1.5) && qr[ihit] < (float(iCharge) + 2.5))
                            fh_xy_Tofi[iCharge]->Fill(x1[fi33], x1[fi33] - x1[fi31]);
                    }
                    fh_xy_Tofi_ac->Fill(qr[ihit], (x1[fi33] - x1[fi31]) / x1[fi33]);

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
                        fh_xy_Fib[det]->Fill(xxx, q1[det]);
                        fh_ToT_Fib[det]->Fill(qqq, q1[det]);
                        fh_Fibs_vs_Tofd[det]->Fill(xxx, x1[det]);
                        fh_ToF_vs_Events[det]->Fill(fNEvents, tof);

                        // cout << "Time: " << t1[det] << endl;
                        //  if (fCuts && (t1[det] < -20 || t1[det] > 20))
                        //    continue;
                        if ((hit23a->GetFiberId() > 188 && hit23a->GetFiberId() < 197))
                            continue;

                        /*   if (fCuts && (x1[det] * 100. < -10000 || x1[det] * 100. > 10000))
                                continue;
                            if (fCuts && (y1[det] < -10000 || y1[det] > 10000))
                                continue; */

                        if (fCuts && (tof < ftofminFib2x || tof > ftofmaxFib2x))
                            continue;

                        // Fill histograms
                        fh_Fib_ToF_ac[det]->Fill(tof);
                        fh_xy_Fib_ac[det]->Fill(xxx, q1[det]);
                        fh_ToT_Fib_ac[det]->Fill(qqq, q1[det]);
                        fh_Fibs_vs_Tofd_ac[det]->Fill(xxx, x1[det]);
                        fh_ToT_TOF_Fib_ac[det]->Fill(x1[det], tof);

                        if (debug2)
                            cout << "Fi23a " << ihit23a << " x1: " << x1[det] << " y1: " << y1[det]
                                 << " q1: " << q1[det] << " t1: " << tof << endl;

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
                            fh_xy_Fib[det]->Fill(xxx, q1[det]);
                            fh_ToT_Fib[det]->Fill(qqq, q1[det]);
                            fh_Fibs_vs_Tofd[det]->Fill(xxx, y1[det]);
                            fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                            //  if (fCuts && (t1[det] < -20 || t1[det] > 20))
                            //    continue;
                            if ((hit23b->GetFiberId() > 188 && hit23b->GetFiberId() < 197))
                                continue;

                            if (fCuts && (tof < ftofminFib2x || tof > ftofmaxFib2x))
                                continue;

                            // Fill histograms
                            fh_Fib_ToF_ac[det]->Fill(tof);
                            fh_xy_Fib_ac[det]->Fill(xxx, q1[det]);
                            fh_ToT_Fib_ac[det]->Fill(qqq, q1[det]);
                            fh_Fibs_vs_Tofd_ac[det]->Fill(xxx, y1[det]);
                            fh_ToT_TOF_Fib_ac[det]->Fill(y1[det], tof);

                            if (debug2)
                                cout << "Fi23b " << ihit23b << " x1: " << x1[det] << " y1: " << y1[det]
                                     << " q1: " << q1[det] << " t1: " << tof << endl;
                        }
                    }
                }
            }
        }

        // ***********************************************
        // loop over left side
        // ***********************************************
        for (Int_t ihit = 0; ihit < nSuml; ihit++)
        {
            xxx = xl[ihit];
            qqq = ql[ihit];
            yyy = yl[ihit];
            tStart = tl[ihit];
            if (fCuts && (qqq < 0 || qqq > 20))
            {
                // cout << "Cut because of charge: " << qqq << endl;
                continue;
            }
            if (fCuts && (xxx < -10000 || xxx > 10000))
            {
                // cout << "Cut because of x" << endl;
                continue;
            }

            // cut in ToT for Fibers
            Double_t cutQ = 0.;

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
                y1[det] = hit32->GetY();               // cm
                z1[det] = 0.;
                q1[det] = hit32->GetEloss();

                t1[det] = hit32->GetTime();
                tof = tStart - t1[det];

                // Fill histograms before cuts
                fh_Fib_ToF[det]->Fill(tof);
                fh_xy_Fib[det]->Fill(xxx, q1[det]);
                fh_ToT_Fib[det]->Fill(qqq, q1[det]);
                fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                fh_Fibs_vs_Tofd[det]->Fill(xxx, x1[det]);

                hits32bc++;

                // Cuts on Fi32

                /*   if (fCuts && (x1[det] * 100. < -10000 || x1[det] * 100. > 10000))
                        continue;
                    if (fCuts && (y1[det] < -10000 || y1[det] > 10000))
                        continue; */
                //   if (fCuts && std::abs(q1[det]-qqq) > 2.)
                //         continue;
                if (fCuts && (tof < ftofminFib3x || tof > ftofmaxFib3x))
                    continue;

                hits32++;

                // Fill histograms after cuts
                fh_Fib_ToF_ac[det]->Fill(tof);
                fh_xy_Fib_ac[det]->Fill(xxx, q1[det]);
                fh_ToT_Fib_ac[det]->Fill(qqq, q1[det]);
                fh_Fibs_vs_Tofd_ac[det]->Fill(xxx, x1[det]);
                fh_ToT_TOF_Fib_ac[det]->Fill(x1[det], t1[det]); // tof);

                if (qqq > 1.5 && qqq < 2.5)
                    fh_yy->Fill(y1[fi32], yyy);

                if (debug2)
                    cout << "Fi32: " << ihit32 << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                         << " t1: " << t1[det] << endl;

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
                    y1[det] = hit30->GetY();
                    z1[det] = 0.;
                    q1[det] = hit30->GetEloss();
                    t1[det] = hit30->GetTime();
                    tof = tStart - t1[det];

                    // Fill histograms before cuts
                    fh_xy_Fib[det]->Fill(xxx, q1[det]);
                    fh_Fib_ToF[det]->Fill(tof);
                    fh_ToT_Fib[det]->Fill(qqq, q1[det]);
                    fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                    fh_Fibs_vs_Tofd[det]->Fill(xxx, x1[det]);
                    hits30bc++;

                    // Cuts on Fi30

                    /*   if (fCuts && (x1[det] * 100. < -10000 || x1[det] * 100. > 10000))
                            continue;
                        if (fCuts && (y1[det] < -10000 || y1[det] > 10000))
                            continue; */
                    //    if (fCuts && std::abs(q1[det]-qqq) > 2.)
                    //          continue;
                    if (fCuts && (tof < ftofminFib3x || tof > ftofmaxFib3x))
                        continue;

                    hits30++;

                    // Fill histograms
                    fh_Fib_ToF_ac[det]->Fill(tof);
                    fh_xy_Fib_ac[det]->Fill(xxx, q1[det]);
                    fh_ToT_Fib_ac[det]->Fill(qqq, q1[det]);
                    fh_Fibs_vs_Tofd_ac[det]->Fill(xxx, x1[det]);
                    fh_ToT_TOF_Fib_ac[det]->Fill(x1[det], tof);

                    if (debug2)
                        cout << "Fi30: " << ihit30 << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                             << " t1: " << t1[det] << endl;

                    for (Int_t iCharge = 0; iCharge < 7; iCharge++)
                    {
                        if (ql[ihit] > (float(iCharge) + 1.5) && ql[ihit] < (float(iCharge) + 2.5))
                            fh_xy_Tofi[iCharge]->Fill(x1[fi32], x1[fi32] - x1[fi30]);
                    }
                    fh_xy_Tofi_ac->Fill(ql[ihit], (x1[fi32] - x1[fi30]) / x1[fi32]);
                    // cout<<x1[fi32]-x1[fi30]/x1[fi32]<<endl;

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
                        fh_xy_Fib[det]->Fill(xxx, q1[det]);
                        fh_ToT_Fib[det]->Fill(qqq, q1[det]);
                        fh_Fibs_vs_Tofd[det]->Fill(xxx, x1[det]);
                        fh_ToF_vs_Events[det]->Fill(fNEvents, tof);

                        // cout << "Time: " << t1[det] << endl;
                        //  if (fCuts && (t1[det] < -20 || t1[det] > 20))
                        //    continue;
                        if ((hit23a->GetFiberId() > 188 && hit23a->GetFiberId() < 197))
                            continue;

                        /*   if (fCuts && (x1[det] * 100. < -10000 || x1[det] * 100. > 10000))
                                continue;
                            if (fCuts && (y1[det] < -10000 || y1[det] > 10000))
                                continue; */

                        if (fCuts && (tof < ftofminFib2x || tof > ftofmaxFib2x))
                            continue;

                        // Fill histograms
                        fh_Fib_ToF_ac[det]->Fill(tof);
                        fh_xy_Fib_ac[det]->Fill(xxx, q1[det]);
                        fh_ToT_Fib_ac[det]->Fill(qqq, q1[det]);
                        fh_Fibs_vs_Tofd_ac[det]->Fill(xxx, x1[det]);
                        fh_ToT_TOF_Fib_ac[det]->Fill(x1[det], tof);

                        if (debug2)
                            cout << "Fi23a " << ihit23a << " x1: " << x1[det] << " y1: " << y1[det]
                                 << " q1: " << q1[det] << " t1: " << tof << endl;
                    }

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
                        fh_xy_Fib[det]->Fill(xxx, q1[det]);
                        fh_ToT_Fib[det]->Fill(qqq, q1[det]);
                        fh_Fibs_vs_Tofd[det]->Fill(xxx, y1[det]);
                        fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                        //   if (fCuts && (t1[det] < -20 || t1[det] > 20))
                        //     continue;
                        if ((hit23b->GetFiberId() > 188 && hit23b->GetFiberId() < 197))
                            continue;

                        // Cuts on Fi23b
                        /*   if (fCuts && (x1[det] * 100. < -10000 || x1[det] * 100. > 10000))
                                continue;
                            if (fCuts && (y1[det] < -10000 || y1[det] > 10000))
                                continue; */

                        if (fCuts && (tof < ftofminFib2x || tof > ftofmaxFib2x))
                            continue;

                        // Fill histograms
                        fh_Fib_ToF_ac[det]->Fill(tof);
                        fh_xy_Fib_ac[det]->Fill(xxx, q1[det]);
                        fh_ToT_Fib_ac[det]->Fill(qqq, q1[det]);
                        fh_Fibs_vs_Tofd_ac[det]->Fill(xxx, y1[det]);
                        fh_ToT_TOF_Fib_ac[det]->Fill(y1[det], tof);

                        if (debug2)
                            cout << "Fi23b " << ihit23b << " x1: " << x1[det] << " y1: " << y1[det]
                                 << " q1: " << q1[det] << " t1: " << tof << endl;
                    }
                }
            }
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
    cout << "Events w/o Rolu and with Califa    : " << fNEvents_califa << endl;
    cout << "Events w/o Rolu and with ToFI      : " << fNEvents_tofi << endl;
    cout << "Events w/o Rolu and with fibers23  : " << fNEvents_fibers << endl;
    cout << "Events with two particles          : " << fNEvents_pair << endl;
    cout << "Time duration                      : " << (time_end - time_begin) / 1.e9 << " sec" << endl;
    cout << "testing: " << time_end << ", " << time_begin << endl;
    cout << "nSpill                             : " << fNSpills << endl;
    cout << "Total num of 18O                   : " << nBeamParticle << endl;
    cout << "Total off-spill particles          : " << nOffSpillParticle - nBeamParticle << endl;
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
        outfile << setprecision(10) << "Events w/o Rolu and with Califa    : " << fNEvents_califa << endl;
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
            fh_xy_Fib_ac[ifibcount]->Write();
            fh_ToT_Fib_ac[ifibcount]->Write();
            fh_Fibs_vs_Tofd_ac[ifibcount]->Write();
            fh_Fib_ToF_ac[ifibcount]->Write();
            fh_ToT_TOF_Fib_ac[ifibcount]->Write();
            fh_xy_Fib[ifibcount]->Write();
            fh_ToT_Fib[ifibcount]->Write();
            fh_Fibs_vs_Tofd[ifibcount]->Write();
            fh_Fib_ToF[ifibcount]->Write();
            fh_ToF_vs_Events[ifibcount]->Write();
        }
    }
    fh_xy_target_ac->Write();
    fh_xy_target->Write();
    fh_Z_vs_x->Write();
    fh_Z_vs_dthit->Write();
    fh_ztofd->Write();
    fh_ztofi->Write();
    fh_yy->Write();
    if (fHitItems.at(DET_TOFI))
    {

        fh_ToT_Tofi_ac->Write();
        fh_xy_Tofi_ac->Write();
        for (Int_t i = 0; i < 7; i++)
        {
            fh_xy_Tofi[i]->Write();
        }

        fh_ToT_Tofi->Write();
        fh_Tofi_ToF->Write();
    }
    if (fCalItems.at(DET_ROLU))
    {
        for (Int_t i = 0; i < 2; i++)
        {
            fh_Rolu_ToF[i]->Write();
            if (fCuts)
                fh_ToT_Rolu_ac[i]->Write();
            else
            {
                fh_Rolu_ToF[i]->Write();
                fh_ToT_Rolu[i]->Write();
            }
        }
    }
}

ClassImp(R3BGlobalCorrelationsS494)
