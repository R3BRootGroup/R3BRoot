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
// -----                  R3BGlobalAnalysisS454                -----
// -----          Created April 13th 2016 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BLosCalData.h"
#include "R3BLosHitData.h"
#include "R3BLosMappedData.h"

#include "R3BBeamMonitorMappedData.h"

#include "R3BGlobalAnalysisS454.h"

#include "R3BSci8CalData.h"
#include "R3BSci8MappedData.h"

#include "R3BTofdCalData.h"
#include "R3BTofdHitData.h"
#include "R3BTofdMappedData.h"

#include "R3BRoluCalData.h"
#include "R3BRoluMappedData.h"

#include "R3BPaddleCalData.h"

#include "R3BPspxCalData.h"
#include "R3BPspxMappedData.h"

#include "R3BEventHeader.h"
#include "R3BTCalEngine.h"

#include "R3BBunchedFiberCalData.h"
#include "R3BBunchedFiberHitData.h"
#include "R3BBunchedFiberMappedData.h"

#include "R3BMCTrack.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"

#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"

#include "tracker_routines.h"

#include "TClonesArray.h"
#include "TMath.h"
#include <TRandom3.h>
#include <TRandomGen.h>
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;

R3BGlobalAnalysisS454::R3BGlobalAnalysisS454()
    : R3BGlobalAnalysisS454("GlobalAnalysis", 1)
{
}

R3BGlobalAnalysisS454::R3BGlobalAnalysisS454(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fCuts(0)
    , fNEvents(0)
{
}

R3BGlobalAnalysisS454::~R3BGlobalAnalysisS454()
{
    for (int i = 0; i < NOF_FIB_DET; i++)
    {
        delete fh_xy_Fib[i];
        delete fh_mult_Fib[i];
        delete fh_ToT_Fib[i];
    }
}

InitStatus R3BGlobalAnalysisS454::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(INFO) << "R3BGlobalAnalysisS454::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";
    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline* run = FairRunOnline::Instance();

    // Get objects for detectors on all levels
    fMCTrack = (TClonesArray*)mgr->GetObject("MCTrack");

    assert(DET_MAX + 1 == sizeof(fDetectorNames) / sizeof(fDetectorNames[0]));
    printf("Have %d fiber detectors.\n", NOF_FIB_DET);
    fMappedItems.push_back((TClonesArray*)mgr->GetObject(Form("%sMappedData", fDetectorNames[0])));
    if (NULL == fMappedItems.at(0))
    {
        printf("Could not find mapped data for '%s'.\n", fDetectorNames[0]);
    }
    fCalItems.push_back((TClonesArray*)mgr->GetObject(Form("%sCrystalCalData", fDetectorNames[0])));
    if (NULL == fCalItems.at(0))
    {
        printf("Could not find Cal data for '%s'.\n", fDetectorNames[0]);
    }
    fHitItems.push_back((TClonesArray*)mgr->GetObject(Form("%sHit", fDetectorNames[0])));
    if (NULL == fHitItems.at(0))
    {
        printf("Could not find hit data for '%s'.\n", fDetectorNames[0]);
    }
    for (int det = 1; det < DET_MAX; det++)
    {
        fMappedItems.push_back((TClonesArray*)mgr->GetObject(Form("%sMapped", fDetectorNames[det])));
        if (NULL == fMappedItems.at(det))
        {
            printf("Could not find mapped data for '%s'.\n", fDetectorNames[det]);
        }
        if (det == 9)
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

    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------

    char strNameC[255];
    sprintf(strNameC, "Cave_C_position");
    fh_Cave_position = new TH2F(strNameC, "", 2100, -100., 2000., 1000, -500., 500.);

    //-----------------------------------------------------------------------
    // BeamMonitor

    // get the theoretical calib factors for SEETRAM
    Double_t fexp = float(fsens_SEE + 9);
    Double_t fpow = float(pow(10., fexp));
    calib_SEE = 135641.7786 * fpow;
    LOG(DEBUG) << fsens_SEE << ", " << fexp << ", " << fpow << ", " << calib_SEE << endl;

    fh_Tpat = new TH1F("Tpat", "Tpat", 20, 0, 20);
    fh_Tpat->GetXaxis()->SetTitle("Tpat value");

    fh_Trigger = new TH1F("Trigger", "Trigger all", 20, 0, 20);
    fh_Trigger->GetXaxis()->SetTitle("Trigger value");

    fh_IC = new TH1F("IC", "IC ", 1000, 0, 1000);
    fh_IC->GetXaxis()->SetTitle("spill number");
    fh_IC->GetYaxis()->SetTitle("IC counts");

    fh_SEE = new TH1F("SEETRAM", "SEETRAM ", 1000, 0, 1000);
    fh_SEE->GetXaxis()->SetTitle("spill number");
    fh_SEE->GetYaxis()->SetTitle("SEETRAM counts");

    fh_TOFDOR = new TH1F("TOFDOR", "TOFDOR ", 1000, 0, 1000);
    fh_TOFDOR->GetXaxis()->SetTitle("spill number");
    fh_TOFDOR->GetYaxis()->SetTitle("TOFDOR counts");

    //-----------------------------------------------------------------------
    // compare against MC Simulations

    fh_target_px = new TH1F("target_px", "target px ", 2000, -500., 500);
    fh_target_px->GetXaxis()->SetTitle("px / MeV/c");
    fh_target_px->GetYaxis()->SetTitle("counts");

    fh_target_py = new TH1F("target_py", "target py ", 2000, -500., 500);
    fh_target_py->GetXaxis()->SetTitle("py / MeV/c");
    fh_target_py->GetYaxis()->SetTitle("counts");

    fh_target_pz = new TH1F("target_pz", "target pz ", 30000, 0., 30000.);
    fh_target_pz->GetXaxis()->SetTitle("pz / MeV/c");
    fh_target_pz->GetYaxis()->SetTitle("counts");

    fh_target_p = new TH1F("target_p", "target p ", 30000, 0., 30000.);
    fh_target_p->GetXaxis()->SetTitle("pz / MeV/c");
    fh_target_p->GetYaxis()->SetTitle("counts");

    fh_target_xy = new TH2F("target_xy", "target xy ", 1000, -0.5, 0.5, 1000, -0.5, 0.5);
    fh_target_xy->GetXaxis()->SetTitle("x / cm");
    fh_target_xy->GetYaxis()->SetTitle("y / cm");

    fh_chi2 = new TH1F("chi2", "chi2 ", 1000, 0., 100);
    fh_chi2->GetXaxis()->SetTitle("Chi2");
    fh_chi2->GetYaxis()->SetTitle("counts");

    fh_dx = new TH1F("tracker_dx", "tracker dx ", 2000, -5., 5);
    fh_dx->GetXaxis()->SetTitle("dx / cm");
    fh_dx->GetYaxis()->SetTitle("counts");

    fh_dy = new TH1F("tracker_dy", "tracker dy ", 2000, -5., 5.);
    fh_dy->GetXaxis()->SetTitle("dy / cm");
    fh_dy->GetYaxis()->SetTitle("counts");

    fh_dz = new TH1F("tracker_dz", "tracker dz ", 2000, -5., 5.);
    fh_dz->GetXaxis()->SetTitle("dz / cm");
    fh_dz->GetYaxis()->SetTitle("counts");

    fh_dpx = new TH1F("tracker_dpx", "tracker dpx ", 2000, -10, 10);
    fh_dpx->GetXaxis()->SetTitle("dpx / percent");
    fh_dpx->GetYaxis()->SetTitle("counts");

    fh_dpy = new TH1F("tracker_dpy", "tracker dpy ", 2000, -10, 10);
    fh_dpy->GetXaxis()->SetTitle("dpy / percent");
    fh_dpy->GetYaxis()->SetTitle("counts");

    fh_dpz = new TH1F("tracker_dpz", "tracker dpz ", 2000, -10, 10);
    fh_dpz->GetXaxis()->SetTitle("dpz / percent");
    fh_dpz->GetYaxis()->SetTitle("counts");

    fh_dp = new TH1F("tracker_dp", "tracker dp ", 2000, -10, 10);
    fh_dp->GetXaxis()->SetTitle("dp / percent");
    fh_dp->GetYaxis()->SetTitle("counts");

    fh_thetax_dpx = new TH2F("tracker_thethax_dpx", "tracker theta_x vs dpx ", 1000, -50, 50, 200, -100., 100.);
    fh_thetax_dpx->GetXaxis()->SetTitle("dpx / percent");
    fh_thetax_dpx->GetYaxis()->SetTitle("theta_x / mrad");

    fh_dpy_dpx = new TH2F("tracker_dpy_dpx", "tracker dpy vs dpx ", 200, -100, 100, 200, -100., 100.);
    fh_dpy_dpx->GetXaxis()->SetTitle("dpx / percent");
    fh_dpy_dpx->GetYaxis()->SetTitle("dpy / percent");

    fh_thetay_dpy = new TH2F("tracker_thethay_dpy", "tracker theta_y vs dpy ", 1000, -50, 50, 200, -100., 100.);
    fh_thetay_dpy->GetXaxis()->SetTitle("dpy / percent");
    fh_thetay_dpy->GetYaxis()->SetTitle("theta_y / mrad");

    fh_thetax_dpy = new TH2F("tracker_thethax_dpy", "tracker theta_x vs dpy ", 1000, -50, 50, 200, -100., 100.);
    fh_thetax_dpy->GetXaxis()->SetTitle("dpy / percent");
    fh_thetax_dpy->GetYaxis()->SetTitle("theta_x / mrad");

    fh_thetay_dpx = new TH2F("tracker_thethay_dpx", "tracker theta_y vs dpx ", 1000, -50, 50, 200, -100., 100.);
    fh_thetay_dpx->GetXaxis()->SetTitle("dpx / percent");
    fh_thetay_dpx->GetYaxis()->SetTitle("theta_y / mrad");

    fh_thetax_dpx_abs =
        new TH2F("tracker_thethax_dpx_abs", "tracker theta_x vs dpx abs ", 1000, -50, 50, 200, -100., 100.);
    fh_thetax_dpx_abs->GetXaxis()->SetTitle("dpx");
    fh_thetax_dpx_abs->GetYaxis()->SetTitle("theta_x / mrad");

    fh_x_dpx = new TH2F("tracker_x_dpx", "tracker x vs dpx ", 1000, -50, 50, 200, -10., 10.);
    fh_x_dpx->GetXaxis()->SetTitle("dpx / percent");
    fh_x_dpx->GetYaxis()->SetTitle("x / cm");

    fh_y_dpy = new TH2F("tracker_y_dpy", "tracker y vs dpy ", 1000, -50, 50, 200, -10., 10.);
    fh_y_dpy->GetXaxis()->SetTitle("dpy / percent");
    fh_y_dpy->GetYaxis()->SetTitle("y / cm");

    //-----------------------------------------------------------------------
    // Fiber Detectors 1-NOF_FIB_DET

    char canvName[255];
    UInt_t Nmax = 1e7;
    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {

        if (fCalItems.at(DET_FI_FIRST + ifibcount) || fHitItems.at(DET_FI_FIRST + ifibcount))
        {

            const char* detName;
            const char* detName2;
            detName = fDetectorNames[DET_FI_FIRST + ifibcount];

            LOG(DEBUG) << "I am creating canvas " << detName << endl;

            // xy:
            fh_xy_Fib[ifibcount] =
                new TH2F(Form("%s_xy", detName), Form("%s xy", detName), 1000, -50., 50., 2000, -1000., 1000.);
            fh_xy_Fib[ifibcount]->GetXaxis()->SetTitle("x / cm ");
            fh_xy_Fib[ifibcount]->GetYaxis()->SetTitle("y / cm");

            fh_xy_Fib_ac[ifibcount] = new TH2F(
                Form("%s_xy_ac", detName), Form("%s xy after cuts", detName), 1000, -50., 50., 2000, -100., 100.);
            fh_xy_Fib_ac[ifibcount]->GetXaxis()->SetTitle("x / cm ");
            fh_xy_Fib_ac[ifibcount]->GetYaxis()->SetTitle("y / cm");

            // Multiplicity (number of hit fibers):
            fh_mult_Fib[ifibcount] = new TH1F(Form("%s_mult", detName), Form("%s # of fibers", detName), 500, 0., 500.);
            fh_mult_Fib[ifibcount]->GetXaxis()->SetTitle("Multiplicity");
            fh_mult_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            fh_mult_Fib_ac[ifibcount] =
                new TH1F(Form("%s_mult_ac", detName), Form("%s # of fibers after cuts", detName), 500, 0., 500.);
            fh_mult_Fib_ac[ifibcount]->GetXaxis()->SetTitle("Multiplicity");
            fh_mult_Fib_ac[ifibcount]->GetYaxis()->SetTitle("Counts");

            // ToT MAPMT:
            fh_ToT_Fib[ifibcount] =
                new TH2F(Form("%s_tot_m", detName), Form("%s ToT of MAPMT", detName), 1000, -50., 50, 4000, 0., 400.);
            fh_ToT_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber x / cm");
            fh_ToT_Fib[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            fh_ToT_Fib_ac[ifibcount] = new TH2F(Form("%s_tot_m_ac", detName),
                                                Form("%s ToT of MAPMT after cuts", detName),
                                                1000,
                                                -50.,
                                                50,
                                                4000,
                                                0.,
                                                400.);
            fh_ToT_Fib_ac[ifibcount]->GetXaxis()->SetTitle("Fiber x / cm");
            fh_ToT_Fib_ac[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            // ToF Tofd -> Fiber:
            fh_Fib_ToF[ifibcount] = new TH2F(
                Form("%s_tof", detName), Form("%s ToF Tofd to Fiber", detName), 600, -30., 30, 10000, -1000., 1000.);
            fh_Fib_ToF[ifibcount]->GetYaxis()->SetTitle("ToF / ns");
            fh_Fib_ToF[ifibcount]->GetXaxis()->SetTitle("x / cm");

            fh_Fib_ToF_ac[ifibcount] = new TH2F(Form("%s_tof_ac", detName),
                                                Form("%s ToF Tofd to Fiber after cuts", detName),
                                                600,
                                                -30.,
                                                30,
                                                10000,
                                                -1000.,
                                                1000.);
            fh_Fib_ToF_ac[ifibcount]->GetYaxis()->SetTitle("ToF / ns");
            fh_Fib_ToF_ac[ifibcount]->GetXaxis()->SetTitle("x / cm");

            // ToF Tofd -> Fiber vs. event number:
            fh_ToF_vs_Events[ifibcount] = new TH2F(Form("%s_tof_vs_events", detName),
                                                   Form("%s ToF Tofd to Fiber vs event number", detName),
                                                   10000,
                                                   0,
                                                   Nmax,
                                                   2200,
                                                   -5100,
                                                   5100);
            fh_ToF_vs_Events[ifibcount]->GetYaxis()->SetTitle("ToF / ns");
            fh_ToF_vs_Events[ifibcount]->GetXaxis()->SetTitle("event number");

            fh_ToF_vs_Events_ac[ifibcount] = new TH2F(Form("%s_tof_vs_events_ac", detName),
                                                      Form("%s ToF Tofd to Fiber vs event number after cuts", detName),
                                                      10000,
                                                      0,
                                                      Nmax,
                                                      2200,
                                                      -5100,
                                                      5100);
            fh_ToF_vs_Events_ac[ifibcount]->GetYaxis()->SetTitle("ToF / ns");
            fh_ToF_vs_Events_ac[ifibcount]->GetXaxis()->SetTitle("event number");

            // hit fiber number vs. event number:
            fh_Fib_vs_Events[ifibcount] = new TH2F(Form("%s_fib_vs_event", detName),
                                                   Form("%s Fiber # vs. Event #", detName),
                                                   10000,
                                                   0,
                                                   Nmax,
                                                   200,
                                                   -100.,
                                                   100.);
            fh_Fib_vs_Events[ifibcount]->GetYaxis()->SetTitle("Fiber x / cm");
            fh_Fib_vs_Events[ifibcount]->GetXaxis()->SetTitle("Event number");

            fh_Fib_vs_Events_ac[ifibcount] = new TH2F(Form("%s_fib_vs_event_ac", detName),
                                                      Form("%s Fiber # vs. Event # after cuts", detName),
                                                      10000,
                                                      0,
                                                      Nmax,
                                                      200,
                                                      -100.,
                                                      100.);
            fh_Fib_vs_Events_ac[ifibcount]->GetYaxis()->SetTitle("Fiber x / cm");
            fh_Fib_vs_Events_ac[ifibcount]->GetXaxis()->SetTitle("Event number");

            // hit fiber number vs. TofD position:
            fh_Fibs_vs_Tofd[ifibcount] = new TH2F(Form("%s_fib_vs_TofdX", detName),
                                                  Form("%s Fiber # vs. Tofd x-pos", detName),
                                                  200,
                                                  -100,
                                                  100,
                                                  200,
                                                  -100.,
                                                  100.);
            fh_Fibs_vs_Tofd[ifibcount]->GetYaxis()->SetTitle("Fiber x / cm");
            fh_Fibs_vs_Tofd[ifibcount]->GetXaxis()->SetTitle("Tofd x / cm");

            fh_Fibs_vs_Tofd_ac[ifibcount] = new TH2F(Form("%s_fib_vs_TofdX_ac", detName),
                                                     Form("%s Fiber # vs. Tofd x-pos after cuts", detName),
                                                     200,
                                                     -100,
                                                     100,
                                                     200,
                                                     -100.,
                                                     100.);
            fh_Fibs_vs_Tofd_ac[ifibcount]->GetYaxis()->SetTitle("Fiber x / cm");
            fh_Fibs_vs_Tofd_ac[ifibcount]->GetXaxis()->SetTitle("Tofd x / cm");

            // hit fiber number vs. TofD position:
            for (Int_t j = ifibcount + 1; j < NOF_FIB_DET; j++)
            {
                detName2 = fDetectorNames[DET_FI_FIRST + j];
                fh_Fib_vs_Fib[ifibcount][j] = new TH2F(Form("fib%s_vs_fib%s", detName, detName2),
                                                       Form("Fiber %s vs. Fiber %s", detName, detName2),
                                                       1000,
                                                       -50,
                                                       50,
                                                       1000,
                                                       -50.,
                                                       50.);
                fh_Fib_vs_Fib[ifibcount][j]->GetYaxis()->SetTitle(Form("Fiber%s", detName2));
                fh_Fib_vs_Fib[ifibcount][j]->GetXaxis()->SetTitle(Form("Fiber%s", detName));
                ;

                detName2 = fDetectorNames[DET_FI_FIRST + j];
                fh_Fib_vs_Fib_ac[ifibcount][j] = new TH2F(Form("fib%s_vs_fib%s_ac", detName, detName2),
                                                          Form("Fiber %s vs. Fiber %s after cuts", detName, detName2),
                                                          1000,
                                                          -50,
                                                          50,
                                                          1000,
                                                          -50.,
                                                          50.);
                fh_Fib_vs_Fib_ac[ifibcount][j]->GetYaxis()->SetTitle(Form("Fiber%s", detName2));
                fh_Fib_vs_Fib_ac[ifibcount][j]->GetXaxis()->SetTitle(Form("Fiber%s", detName));
                ;
            }

            // dx between fibers vs x
            for (Int_t j = ifibcount + 1; j < NOF_FIB_DET; j++)
            {
                detName2 = fDetectorNames[DET_FI_FIRST + j];
                fh_Fib_dx[ifibcount][j] = new TH2F(Form("fib%s_fib%s_dx", detName, detName2),
                                                   Form("dx of Fiber %s and Fiber %s", detName2, detName),
                                                   1000,
                                                   -50,
                                                   50,
                                                   1000,
                                                   -50.,
                                                   50.);
                fh_Fib_dx[ifibcount][j]->GetYaxis()->SetTitle("dx");
                fh_Fib_dx[ifibcount][j]->GetXaxis()->SetTitle(Form("x %s / cm", detName));
                ;

                detName2 = fDetectorNames[DET_FI_FIRST + j];
                fh_Fib_dx_ac[ifibcount][j] = new TH2F(Form("fib%s_fib%s_dx_ac", detName, detName2),
                                                      Form("dx of Fiber %s and Fiber %s after cuts", detName2, detName),
                                                      1000,
                                                      -50,
                                                      50,
                                                      1000,
                                                      -50.,
                                                      50.);
                fh_Fib_dx_ac[ifibcount][j]->GetYaxis()->SetTitle("dx");
                fh_Fib_dx_ac[ifibcount][j]->GetXaxis()->SetTitle(Form("x %s / cm", detName));
                ;
            }

        } // end if(Mapped)

    } // end for(ifibcount)

    //---------------------------------------------------------------------------------------------------
    // TofD detector

    if (fHitItems.at(DET_TOFD) || fCalItems.at(DET_TOFD))
    {

        // xy:
        fh_xy_tofd = new TH2F("tofd_xy", "tofd xy", 2000, -100., 100., 2000, -100., 100.);
        fh_xy_tofd->GetXaxis()->SetTitle("x / cm ");
        fh_xy_tofd->GetYaxis()->SetTitle("y / cm");

        fh_xy_tofd_ac = new TH2F("tofd_xy_ac", "tofd xy after cuts", 2000, -100., 100., 2000, -100., 100.);
        fh_xy_tofd_ac->GetXaxis()->SetTitle("x / cm ");
        fh_xy_tofd_ac->GetYaxis()->SetTitle("y / cm");

        fh_tofd_charge = new TH1F("tofd_Q", "Charge of Tofd", 200, 0., 20.);
        fh_tofd_charge->GetXaxis()->SetTitle("x / cm ");
        fh_tofd_charge->GetYaxis()->SetTitle("y / cm");

        fh_tofd_charge_ac = new TH1F("tofd_Q_ac", "Charge of Tofd after cuts", 200, 0., 20.);
        fh_tofd_charge_ac->GetXaxis()->SetTitle("x / cm ");
        fh_tofd_charge_ac->GetYaxis()->SetTitle("y / cm");

        fh_tofd_mult = new TH1F("tofd_mult", "ToFD multiplicits ", 100, 0, 100);
        fh_tofd_mult->GetXaxis()->SetTitle("multiplicity");
        fh_tofd_mult->GetYaxis()->SetTitle("counts");

        fh_tofd_mult_ac = new TH1F("tofd_mult_ac", "ToFD multiplicits after cuts", 100, 0, 100);
        fh_tofd_mult_ac->GetXaxis()->SetTitle("multiplicity");
        fh_tofd_mult_ac->GetYaxis()->SetTitle("counts");

        fh_TimePreviousEvent = new TH1F("TimePreviousEvent", "Time between 2 particles ", 300000, 0, 3000);
        fh_TimePreviousEvent->GetXaxis()->SetTitle("time / ns");
        fh_TimePreviousEvent->GetYaxis()->SetTitle("counts");
    }

    // -------------------------------------------------------------------------
    // Rene's tracker.

    if (tracker)
        init_from_cpp_();

    return kSUCCESS;
}

void R3BGlobalAnalysisS454::Exec(Option_t* option)
{
    if (fNEvents / 10000. == (int)fNEvents / 10000)
        std::cout << "\rEvents: " << fNEvents << " / " << maxevent << " (" << (int)(fNEvents * 100. / maxevent)
                  << " %) " << std::flush;

    // cout << "New event ******************************" << endl;

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    if (header)
    {
        time = header->GetTimeStamp();

        if (time_start == 0 && time > 0)
        {
            time_start = time;
            fNEvents_start = fNEvents;
        }

        if (header->GetTrigger() == 12)
        {
            // spill start in nsec
            cout << "spill start" << endl;
            num_spills++;
        }
        if (header->GetTrigger() == 13)
        {
            // spill end  in nsec
            cout << "spill stop" << endl;
        }

        fh_Trigger->Fill(header->GetTrigger());
        //   check for requested trigger (Todo: should be done globablly / somewhere else)
        if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
            return;

        Int_t tpatbin;
        for (int i = 0; i < 16; i++)
        {
            tpatbin = (header->GetTpat() & (1 << i));
            if (tpatbin != 0)
                fh_Tpat->Fill(i + 1);
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

        // check for requested trigger (Todo: should be done globablly / somewhere else)
        if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
            return;
    }

    // cout << "Spill: " << num_spills << endl;

    if (fMappedItems.at(DET_BMON))
    {
        unsigned long IC;
        unsigned long SEETRAM_raw;
        Double_t SEETRAM;
        unsigned long TOFDOR;

        auto detBmon = fMappedItems.at(DET_BMON);
        Int_t nHitsbm = detBmon->GetEntriesFast();
        // cout<<"Bmon hits: "<<nHitsbm<<endl;

        for (Int_t ihit = 0; ihit < nHitsbm; ihit++)
        {
            R3BBeamMonitorMappedData* hit = (R3BBeamMonitorMappedData*)detBmon->At(ihit);
            if (!hit)
                continue;

            IC = hit->GetIC(); // negative values if offset not high enough
            counts_IC += (double)IC;

            SEETRAM_raw = hit->GetSEETRAM();           // raw counts
            SEETRAM = (double)SEETRAM_raw * calib_SEE; // calibrated SEETRAM counts
            counts_SEE += SEETRAM;

            TOFDOR = hit->GetTOFDOR(); // only positive values possible
            counts_TofD += TOFDOR;

            if (fNEvents == fNEvents_start)
            {
                see_start = SEETRAM;
                ic_start = IC;
                tofdor_start = TOFDOR;
            }

            /*
            cout << "time " << time << endl;
            cout << "IC   " << IC << "  " << counts_IC << "  " << endl;
            cout << "SEE  " << SEETRAM_raw << "  " << counts_SEE << "  " << SEETRAM << endl;
            cout << "TofD " << TOFDOR << "  " << counts_TofD << "  " << endl;
            */

            // IC:
            Int_t yIC = IC - ic_start;
            fh_IC->Fill(num_spills, yIC);

            // SEETRAM:
            Int_t ySEE = SEETRAM - see_start;
            fh_SEE->Fill(num_spills, ySEE);
            // Double_t ySEE_part = (SEETRAM-see_mem)*fNorm*1.e+3-see_offset*calib_SEE;

            // TOFDOR:
            Int_t yTOFDOR = TOFDOR - tofdor_start;
            fh_TOFDOR->Fill(num_spills, yTOFDOR);
        }
    }

    Bool_t RoluCut = false;
    if (fMappedItems.at(DET_ROLU))
    {
        // rolu
        auto detRolu = fMappedItems.at(DET_ROLU);
        Int_t nHitsRolu = detRolu->GetEntriesFast();
        // cout<<"ROLU hits: "<<nHitsRolu<<endl;

        for (Int_t ihit = 0; ihit < nHitsRolu; ihit++)
        {
            R3BRoluMappedData* hitRolu = (R3BRoluMappedData*)detRolu->At(ihit);
            if (!hitRolu)
                continue;

            // channel numbers are stored 1-based (1..n)
            Int_t iDet = hitRolu->GetDetector(); // 1..
            Int_t iCha = hitRolu->GetChannel();  // 1..
            RoluCut = true;
        }
    }
    if (RoluCut)
    {
        cout << "ROLU cut applied !!!" << endl;
        return;
    }

    if (fMCTrack)
    {
        // read in Monte Carlo Track parameter

        Int_t nHitsMCTrack = fMCTrack->GetEntriesFast();
        // cout << "MCTrack hits: " << nHitsMCTrack << endl;

        for (Int_t l = 0; l < nHitsMCTrack; l++)
        {
            R3BMCTrack* aTrack = (R3BMCTrack*)fMCTrack->At(l);

            Int_t PID = aTrack->GetPdgCode();
            Int_t mother = aTrack->GetMotherId();
            if (mother < 0)
            {
                Xf = aTrack->GetStartX();
                Yf = aTrack->GetStartY();
                Zf = aTrack->GetStartZ();
                Tf = aTrack->GetStartT();

                Pxf = aTrack->GetPx() * 1000.;
                Pyf = aTrack->GetPy() * 1000.;
                Pzf = aTrack->GetPz() * 1000.;
                Pf_tot = sqrt((Pxf * Pxf) + (Pyf * Pyf) + (Pzf * Pzf));

                LOG(DEBUG) << "******************************************" << endl;
                LOG(DEBUG) << "Track In "
                           << "x " << Xf << " y " << Yf << " z " << Zf << endl;
                ;
                LOG(DEBUG) << "Track In "
                           << "Pxf " << Pxf << " Pyf " << Pyf << " Pzf " << Pzf << " Pf_tot " << Pf_tot << endl;
                ;
            }
        }
    }

    Int_t n_det = 8;
    Double_t x[n_det];
    Double_t y[n_det];
    Double_t z[n_det];
    Double_t q[n_det];
    Double_t t[n_det];
    Bool_t ptn[n_det];
    Int_t det;

    Double_t track_parameter[6];        // x,y,z, px, py, pz
    Double_t chi_single_parameter[2];   // chi2, chi2_red
    Double_t track_parameter_best_l[6]; // x,y,z, px, py, pz
    Double_t track_parameter_best_r[6]; // x,y,z, px, py, pz
    Double_t chi2Minl[2];
    Double_t chi2Minr[2];

    for (int i = 0; i < n_det; i++)
    {
        x[i] = 0.;
        y[i] = 0.;
        z[i] = 0.;
        q[i] = 0.;
        t[i] = 0.;
        ptn[i] = kFALSE;
    }

    // is also number of ifibcount
    Int_t fi3a = 0;
    Int_t fi3b = 1;
    Int_t fi10 = 2;
    Int_t fi11 = 3;
    Int_t fi12 = 4;
    Int_t fi13 = 5;
    Int_t tofdr = 6;
    Int_t tofdl = 7;

    Double_t tof = 0.;
    Double_t qMax10 = -1;
    Double_t iMax10 = -1;
    Double_t qMax11 = -1;
    Double_t iMax11 = -1;
    Double_t qMax12 = -1;
    Double_t iMax12 = -1;
    Double_t qMax13 = -1;
    Double_t iMax13 = -1;

    auto detTofd = fHitItems.at(DET_TOFD);
    Int_t nHits = detTofd->GetEntriesFast();
    LOG(DEBUG) << "ToFD hits: " << nHits << endl;

    if (nHits > 0)
        fh_tofd_mult->Fill(nHits);

    if (nHits > 2)
        return;

    // loop over ToFD
    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
        R3BTofdHitData* hitTofd = (R3BTofdHitData*)detTofd->At(ihit);
        if (!hitTofd)
            continue; // should not happen
        if (hitTofd->GetX() / 100. <= -0.007)
        {
            // tof rechts
            det = tofdr;
            ptn[tofdl] = false;
            chi2Minr[0] = 1000.;
        }
        else
        {
            // tof links
            det = tofdl;
            ptn[tofdr] = false;
            chi2Minl[0] = 1000.;
        }
        x[det] = hitTofd->GetX() / 100.;
        y[det] = hitTofd->GetY() / 100.;
        z[det] = 0.;
        q[det] = hitTofd->GetEloss();
        t[det] = hitTofd->GetTime();
        Int_t id = hitTofd->GetDetId();
        // get ID for hit 1= hit in plane 1, 2= hit in plane 2, 12 = hit in both

        LOG(DEBUG) << "ToFD: " << ihit << " x: " << x[det] << " y: " << y[det] << " q: " << q[det] << " t: " << t[det]
                   << endl;

        fh_xy_tofd->Fill(x[det] * 100., y[det] * 100.);
        fh_tofd_charge->Fill(q[det]);

        // Cuts on TofD
        //        if (fCuts && id != 12)
        //            continue;
        if (fCuts && q[det] > 12.)
            continue;
        if (fCuts && q[det] < 5.)
            continue;
        if (fCuts && (t[det] < 0. || t[det] > 8192. * 5.))
            continue;
        if (IS_NAN(t[det]))
            continue;

        ptn[det] = true;

        // fill histograms for ToFD
        fh_tofd_mult_ac->Fill(nHits);
        fh_xy_tofd_ac->Fill(x[det] * 100., y[det] * 100.);
        fh_tofd_charge_ac->Fill(q[det]);
        fh_Cave_position->Fill(1000., x[det] * 100.);

        // differ between left and right half of ToFD
        if (ptn[tofdr])
        { // right half in direction of beam

            // look for hits in Fi3a, Fi11 and Fi13
            // auto detMapped3a = fMappedItems.at(DET_FI3A);
            auto detHit3a = fHitItems.at(DET_FI3A);
            Int_t nHits3a = detHit3a->GetEntriesFast();
            LOG(DEBUG) << "Fi3a hits: " << nHits3a << endl;
            Int_t mult3a = 0;
            fh_mult_Fib[fi3a]->Fill(nHits3a);
            for (Int_t ihit3a = 0; ihit3a <= nHits3a; ihit3a++)
            {
                if (nHits3a == 0)
                {
                    // no hit in 3a but can still be tracked

                    det = fi3a;
                    x[det] = 0.;
                    y[det] = 0.;
                    z[det] = 0.;
                    q[det] = 0.;
                    t[det] = 0.;
                    ptn[det] = false;
                }
                else
                {
                    R3BBunchedFiberHitData* hit3a = (R3BBunchedFiberHitData*)detHit3a->At(ihit3a);
                    if (!hit3a)
                        continue;

                    det = fi3a;

                    x[det] = hit3a->GetX() / 100.;
                    y[det] = hit3a->GetY() / 100.;
                    z[det] = 0.;
                    q[det] = hit3a->GetEloss();
                    t[det] = hit3a->GetTime();

                    // "Push" the Fib times in the same cycle with ToFD:
                    // ToFD coarse counter has 8192 channels with 5 ns each
                    // SPMT coarse counter has 2048 channels with 5 ns each

                    while (t[tofdr] - t[det] < 2048. * 5. / 2.)
                    {
                        t[det] -= 2048. * 5.;
                    }
                    while (t[tofdr] - t[det] > 2048. * 5. / 2.)
                    {
                        t[det] += 2048. * 5.;
                    }

                    tof = t[tofdr] - t[det];

                    // Fill histograms before cuts
                    fh_Fib_ToF[det]->Fill(x[det] * 100., tof);
                    fh_xy_Fib[det]->Fill(x[det] * 100., y[det] * 100.);
                    fh_mult_Fib[det]->Fill(nHits3a);
                    fh_ToT_Fib[det]->Fill(x[det] * 100., q[det]);
                    fh_Fibs_vs_Tofd[det]->Fill(x[tofdr] * 100., x[det] * 100.);
                    fh_Fib_vs_Events[det]->Fill(fNEvents, x[det] * 100.);
                    fh_ToF_vs_Events[det]->Fill(fNEvents, tof);

                    // Cuts on Fi3a
                    if (fCuts && q[det] < 1.)
                        continue;

                    mult3a++;

                    //                    if (fCuts && mult3a > 50)
                    //                        continue;

                    // Fill histograms
                    fh_Fib_ToF_ac[det]->Fill(x[det] * 100., tof);
                    fh_xy_Fib_ac[det]->Fill(x[det] * 100., y[det] * 100.);
                    fh_mult_Fib_ac[det]->Fill(mult3a);
                    fh_ToT_Fib_ac[det]->Fill(x[det] * 100., q[det]);
                    fh_Fibs_vs_Tofd_ac[det]->Fill(x[tofdr] * 100., x[det] * 100.);
                    fh_Fib_vs_Events_ac[det]->Fill(fNEvents, x[det] * 100.);
                    fh_ToF_vs_Events_ac[det]->Fill(fNEvents, tof);

                    ptn[det] = true;
                    LOG(DEBUG) << "Fi3a: " << ihit3a << " x: " << x[det] << " y: " << y[det] << " q: " << q[det]
                               << " t: " << t[det] << endl;
                }
                // auto detMapped11 = fMappedItems.at(DET_FI11);
                auto detHit11 = fHitItems.at(DET_FI11);
                Int_t nHits11 = detHit11->GetEntriesFast();
                // cout<<"Fi11 hits: "<<nHits11<<endl;
                Int_t mult11 = 0;
                qMax11 = -1;
                iMax11 = -1;
                for (Int_t ihit11 = 0; ihit11 < nHits11; ihit11++)
                {
                    R3BBunchedFiberHitData* hit11 = (R3BBunchedFiberHitData*)detHit11->At(ihit11);
                    if (!hit11)
                        continue;
                    // find maximum light output
                    Double_t qq = hit11->GetEloss();
                    if (qq > qMax11)
                    {
                        qMax11 = qq;
                        iMax11 = ihit11;
                    }
                }
                for (Int_t ihit11 = 0; ihit11 < nHits11; ihit11++)
                {
                    R3BBunchedFiberHitData* hit11 = (R3BBunchedFiberHitData*)detHit11->At(ihit11);
                    if (!hit11)
                        continue;

                    det = fi11;

                    x[det] = hit11->GetX() / 100.;
                    y[det] = hit11->GetY() / 100.;
                    z[det] = 0.;
                    q[det] = hit11->GetEloss();
                    t[det] = hit11->GetTime();

                    // "Push" two times in the same clock cycle:
                    while (t[tofdr] - t[det] < 2048. * 5. / 2.)
                    {
                        t[det] -= 2048. * 5.;
                    }
                    while (t[tofdr] - t[det] > 2048. * 5. / 2.)
                    {
                        t[det] += 2048. * 5.;
                    }

                    tof = t[tofdr] - t[det];

                    // Fill histograms before cuts
                    fh_Fib_ToF[det]->Fill(x[det] * 100., tof);
                    fh_xy_Fib[det]->Fill(x[det] * 100., y[det] * 100);
                    fh_mult_Fib[det]->Fill(nHits11);
                    fh_ToT_Fib[det]->Fill(x[det] * 100., q[det]);
                    fh_Fibs_vs_Tofd[det]->Fill(x[tofdr] * 100., x[det] * 100.);
                    fh_Fib_vs_Events[det]->Fill(fNEvents, x[det] * 100.);
                    fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                    fh_Fib_vs_Fib[fi3a][fi11]->Fill(x[fi3a] * 100., x[fi11] * 100.);
                    fh_Fib_dx[fi3a][fi11]->Fill(x[fi3a] * 100., x[fi11] * 100. - x[fi3a] * 100.);

                    // Cuts on Fi11
                    if (fCuts && ihit11 != iMax11)
                        continue;
                    if (fCuts && y[det] * 100. > 50.)
                        continue;
                    if (fCuts && y[det] * 100. < -50.)
                        continue;
                    if (fCuts && q[det] < 1)
                        continue;
                    if (fCuts && (tof < -300 || tof > 0))
                        continue;

                    mult11++;

                    //                    if (fCuts && mult11 > 50)
                    //                        continue;

                    // Fill histograms
                    fh_Fib_ToF_ac[det]->Fill(x[det] * 100., tof);
                    fh_xy_Fib_ac[det]->Fill(x[det] * 100., y[det] * 100);
                    fh_mult_Fib_ac[det]->Fill(mult11);
                    fh_ToT_Fib_ac[det]->Fill(x[det] * 100., q[det]);
                    fh_Fibs_vs_Tofd_ac[det]->Fill(x[tofdr] * 100., x[det] * 100.);
                    fh_Fib_vs_Events_ac[det]->Fill(fNEvents, x[det] * 100.);
                    fh_ToF_vs_Events_ac[det]->Fill(fNEvents, tof);
                    fh_Fib_vs_Fib_ac[fi3a][fi11]->Fill(x[fi3a] * 100., x[fi11] * 100.);
                    fh_Fib_dx_ac[fi3a][fi11]->Fill(x[fi3a] * 100., x[fi11] * 100. - x[fi3a] * 100.);

                    ptn[det] = true;
                    LOG(DEBUG) << "Fi11: " << ihit11 << " x: " << x[det] << " y: " << y[det] << " q: " << q[det]
                               << " t: " << t[det] << endl;

                    // auto detMapped13 = fMappedItems.at(DET_FI13);
                    auto detHit13 = fHitItems.at(DET_FI13);
                    Int_t nHits13 = detHit13->GetEntriesFast();
                    // cout<<"Fi13 hits: "<<nHits13<<endl;
                    Int_t mult13 = 0;
                    qMax13 = -1;
                    iMax13 = -1;
                    for (Int_t ihit13 = 0; ihit13 < nHits13; ihit13++)
                    {
                        R3BBunchedFiberHitData* hit13 = (R3BBunchedFiberHitData*)detHit13->At(ihit13);
                        if (!hit13)
                            continue;
                        // find maximum light output
                        Double_t qq = hit13->GetEloss();
                        if (qq > qMax13)
                        {
                            qMax13 = qq;
                            iMax13 = ihit13;
                        }
                    }
                    for (Int_t ihit13 = 0; ihit13 < nHits13; ihit13++)
                    {
                        R3BBunchedFiberHitData* hit13 = (R3BBunchedFiberHitData*)detHit13->At(ihit13);
                        if (!hit13)
                            continue;

                        det = fi13;

                        x[det] = hit13->GetX() / 100.;
                        y[det] = hit13->GetY() / 100.;
                        z[det] = 0.;
                        q[det] = hit13->GetEloss();
                        t[det] = hit13->GetTime();

                        // "Push" two times in the same clock cycle:
                        while (t[tofdr] - t[det] < 2048. * 5. / 2.)
                        {
                            t[det] -= 2048. * 5.;
                        }
                        while (t[tofdr] - t[det] > 2048. * 5. / 2.)
                        {
                            t[det] += 2048. * 5.;
                        }

                        tof = t[tofdr] - t[det];

                        // Fill histograms before cuts
                        fh_Fib_ToF[det]->Fill(x[det] * 100., tof);
                        fh_xy_Fib[det]->Fill(x[det] * 100., y[det] * 100.);
                        fh_mult_Fib[det]->Fill(nHits13);
                        fh_ToT_Fib[det]->Fill(x[det] * 100., q[det]);
                        fh_Fibs_vs_Tofd[det]->Fill(x[tofdr] * 100., x[det] * 100.);
                        fh_Fib_vs_Events[det]->Fill(fNEvents, x[det] * 100.);
                        fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                        fh_Fib_vs_Fib[fi11][fi13]->Fill(x[fi11] * 100., x[fi13] * 100.);
                        fh_Fib_dx[fi11][fi13]->Fill(x[fi11] * 100., x[fi13] * 100. - x[fi11] * 100.);

                        // Cuts on Fi13
                        if (fCuts && ihit13 != iMax13)
                            continue;
                        if (fCuts && y[det] * 100. > 50.)
                            continue;
                        if (fCuts && y[det] * 100. < -50.)
                            continue;
                        if (fCuts && q[det] < 1)
                            continue;
                        if (fCuts && (tof < -200 || tof > 0))
                            continue;

                        mult13++;

                        //						if (fCuts && mult13 > 50)
                        //							continue;

                        // Fill histograms
                        fh_Fib_ToF_ac[det]->Fill(x[det] * 100., tof);
                        fh_xy_Fib_ac[det]->Fill(x[det] * 100., y[det] * 100.);
                        fh_mult_Fib_ac[det]->Fill(mult13);
                        fh_ToT_Fib_ac[det]->Fill(x[det] * 100., q[det]);
                        fh_Fibs_vs_Tofd_ac[det]->Fill(x[tofdr] * 100., x[det] * 100.);
                        fh_Fib_vs_Events_ac[det]->Fill(fNEvents, x[det] * 100.);
                        fh_ToF_vs_Events_ac[det]->Fill(fNEvents, tof);
                        fh_Fib_vs_Fib_ac[fi11][fi13]->Fill(x[fi11] * 100., x[fi13] * 100.);
                        fh_Fib_dx_ac[fi11][fi13]->Fill(x[fi11] * 100., x[fi13] * 100. - x[fi11] * 100.);

                        ptn[det] = true;

                        LOG(DEBUG) << "Fi13: " << ihit13 << " x: " << x[det] << " y: " << y[det] << " q: " << q[det]
                                   << " t: " << t[det] << endl;

                        Bool_t det_coord = true;

                        ptn[1] = false;
                        ptn[2] = false;
                        ptn[4] = false;

                        // here call tracker
                        if (tracker)
                        {
                            single_track_from_cpp_(
                                &n_det, &det_coord, x, y, z, ptn, track_parameter, chi_single_parameter);
                            // make output
                            if (chi_single_parameter[0] < chi2Minr[0])
                            {
                                chi2Minr[0] = chi_single_parameter[0];
                                for (Int_t i = 0; i < 6; i++)
                                {
                                    track_parameter_best_r[i] = track_parameter[i];
                                }
                            }
                            cout << "chi2 " << chi_single_parameter[0] << "  " << chi_single_parameter[1] << endl;
                            //                            if (chi_single_parameter[0] < 1.)
                            //                                Output(track_parameter, chi_single_parameter);
                        }

                    } // end of loop Fi13

                } // end of loop Fi11
            }     // end of loop Fi3a
        }
        else
        { // left half in direction of beam

            // look for hits in Fi3b, Fi12 and Fi10
            // auto detMapped3b = fMappedItems.at(DET_FI3B);
            auto detHit3b = fHitItems.at(DET_FI3B);
            Int_t nHits3b = detHit3b->GetEntriesFast();
            // cout<<"Fi3b hits: "<<nHits3b<<endl;
            Int_t mult3b = 0;
            for (Int_t ihit3b = 0; ihit3b <= nHits3b; ihit3b++)
            {
                if (nHits3b == 0)
                {
                    // no hit in 3b but can still be tracked
                    det = fi3b;
                    x[det] = 0.;
                    y[det] = 0.;
                    z[det] = 0.;
                    q[det] = 0.;
                    t[det] = 0.;
                    ptn[det] = false;
                }
                else
                {
                    R3BBunchedFiberHitData* hit3b = (R3BBunchedFiberHitData*)detHit3b->At(ihit3b);
                    if (!hit3b)
                        continue;

                    det = fi3b;

                    x[det] = hit3b->GetX() / 100.;
                    y[det] = hit3b->GetY() / 100.;
                    z[det] = 0.;
                    q[det] = hit3b->GetEloss();
                    t[det] = hit3b->GetTime();

                    // "Push" two times in the same clock cycle:
                    while (t[tofdl] - t[det] < 2048. * 5. / 2.)
                    {
                        t[det] -= 2048. * 5.;
                    }
                    while (t[tofdl] - t[det] > 2048. * 5. / 2.)
                    {
                        t[det] += 2048. * 5.;
                    }

                    tof = t[tofdl] - t[det];

                    // Fill histograms before cuts
                    fh_Fib_ToF[det]->Fill(x[det] * 100., tof);
                    fh_xy_Fib[det]->Fill(x[det] * 100., y[det] * 100.);
                    fh_mult_Fib[det]->Fill(nHits3b);
                    fh_ToT_Fib[det]->Fill(x[det] * 100., q[det]);
                    fh_Fibs_vs_Tofd[det]->Fill(x[tofdl] * 100., x[det] * 100.);
                    fh_Fib_vs_Events[det]->Fill(fNEvents, x[det] * 100.);
                    fh_ToF_vs_Events[det]->Fill(fNEvents, tof);

                    // Cuts on Fi3b
                    if (fCuts && q[det] < 1)
                        continue;

                    mult3b++;

                    //                    if (fCuts && mult3b > 50)
                    //                        continue;

                    // Fill histograms
                    fh_Fib_ToF_ac[det]->Fill(x[det] * 100., tof);
                    fh_xy_Fib_ac[det]->Fill(x[det] * 100., y[det] * 100.);
                    fh_mult_Fib_ac[det]->Fill(mult3b);
                    fh_ToT_Fib_ac[det]->Fill(x[det] * 100., q[det]);
                    fh_Fibs_vs_Tofd_ac[det]->Fill(x[tofdl] * 100., x[det] * 100.);
                    fh_Fib_vs_Events_ac[det]->Fill(fNEvents, x[det] * 100.);
                    fh_ToF_vs_Events_ac[det]->Fill(fNEvents, tof);

                    LOG(DEBUG) << "Fi3b: " << ihit3b << " x: " << x[det] << " y: " << y[det] << " q: " << q[det]
                               << " t: " << t[det] << endl;
                    ptn[det] = true;
                }

                // auto detMapped12 = fMappedItems.at(DET_FI12);
                auto detHit12 = fHitItems.at(DET_FI12);
                Int_t nHits12 = detHit12->GetEntriesFast();
                // cout<<"Fi12 hits: "<<nHits12<<endl;
                Int_t mult12 = 0;
                qMax12 = -1;
                iMax12 = -1;
                for (Int_t ihit12 = 0; ihit12 < nHits12; ihit12++)
                {
                    R3BBunchedFiberHitData* hit12 = (R3BBunchedFiberHitData*)detHit12->At(ihit12);
                    if (!hit12)
                        continue;
                    // find maximum light output
                    Double_t qq = hit12->GetEloss();
                    if (qq > qMax12)
                    {
                        qMax12 = qq;
                        iMax12 = ihit12;
                    }
                }
                for (Int_t ihit12 = 0; ihit12 < nHits12; ihit12++)
                {
                    R3BBunchedFiberHitData* hit12 = (R3BBunchedFiberHitData*)detHit12->At(ihit12);
                    if (!hit12)
                        continue;

                    det = fi12;

                    x[det] = hit12->GetX() / 100.;
                    y[det] = hit12->GetY() / 100.;
                    z[det] = 0.;
                    q[det] = hit12->GetEloss();
                    t[det] = hit12->GetTime();

                    // "Push" two times in the same clock cycle:
                    while (t[tofdl] - t[det] < 2048. * 5. / 2.)
                    {
                        t[det] -= 2048. * 5.;
                    }
                    while (t[tofdl] - t[det] > 2048. * 5. / 2.)
                    {
                        t[det] += 2048. * 5.;
                    }

                    tof = t[tofdl] - t[det];

                    // Fill histograms before cuts
                    fh_Fib_ToF[det]->Fill(x[det] * 100., tof);
                    fh_xy_Fib[det]->Fill(x[det] * 100., y[det] * 100.);
                    fh_mult_Fib[det]->Fill(nHits12);
                    fh_ToT_Fib[det]->Fill(x[det] * 100., q[det]);
                    fh_Fibs_vs_Tofd[det]->Fill(x[tofdl] * 100., x[det] * 100.);
                    fh_Fib_vs_Events[det]->Fill(fNEvents, x[det] * 100.);
                    fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                    fh_Fib_vs_Fib[fi3b][fi12]->Fill(x[fi3b] * 100., x[fi12] * 100.);
                    fh_Fib_dx[fi3b][fi12]->Fill(x[fi3b] * 100., x[fi12] * 100. - x[fi3b] * 100.);

                    // Cuts on Fi12
                    if (fCuts && ihit12 != iMax12)
                        continue;
                    if (fCuts && q[det] < 1)
                        continue;
                    if (fCuts && y[det] * 100. > 50.)
                        continue;
                    if (fCuts && y[det] * 100. < -50.)
                        continue;
                    if (fCuts && (tof < -100 || tof > 0))
                        continue;

                    mult12++;

                    //                    if (fCuts && mult12 > 50)
                    //                        continue;

                    // Fill histograms
                    fh_Fib_ToF_ac[det]->Fill(x[det] * 100., tof);
                    fh_xy_Fib_ac[det]->Fill(x[det] * 100., y[det] * 100.);
                    fh_mult_Fib_ac[det]->Fill(mult12);
                    fh_ToT_Fib_ac[det]->Fill(x[det] * 100., q[det]);
                    fh_Fibs_vs_Tofd_ac[det]->Fill(x[tofdl] * 100., x[det] * 100.);
                    fh_Fib_vs_Events_ac[det]->Fill(fNEvents, x[det] * 100.);
                    fh_ToF_vs_Events_ac[det]->Fill(fNEvents, tof);
                    fh_Fib_vs_Fib_ac[fi3b][fi12]->Fill(x[fi3b] * 100., x[fi12] * 100.);
                    fh_Fib_dx_ac[fi3b][fi12]->Fill(x[fi3b] * 100., x[fi12] * 100. - x[fi3b] * 100.);

                    LOG(DEBUG) << "Fi12: " << ihit12 << " x: " << x[det] << " y: " << y[det] << " q: " << q[det]
                               << " t: " << t[det] << endl;
                    ptn[det] = true;

                    // auto detMapped10 = fMappedItems.at(DET_FI10);
                    auto detHit10 = fHitItems.at(DET_FI10);
                    Int_t nHits10 = detHit10->GetEntriesFast();
                    // cout<<"Fi10 hits: "<<nHits10<<endl;
                    Int_t mult10 = 0;
                    qMax10 = -1;
                    iMax10 = -1;
                    for (Int_t ihit10 = 0; ihit10 < nHits10; ihit10++)
                    {
                        R3BBunchedFiberHitData* hit10 = (R3BBunchedFiberHitData*)detHit10->At(ihit10);
                        if (!hit10)
                            continue;
                        // find maximum light output
                        Double_t qq = hit10->GetEloss();
                        if (qq > qMax10)
                        {
                            qMax10 = qq;
                            iMax10 = ihit10;
                        }
                    }
                    for (Int_t ihit10 = 0; ihit10 < nHits10; ihit10++)
                    {
                        R3BBunchedFiberHitData* hit10 = (R3BBunchedFiberHitData*)detHit10->At(ihit10);
                        if (!hit10)
                            continue;

                        det = fi10;

                        x[det] = hit10->GetX() / 100.;
                        y[det] = hit10->GetY() / 100.;
                        z[det] = 0.;
                        q[det] = hit10->GetEloss();
                        t[det] = hit10->GetTime();

                        // "Push" two times in the same clock cycle:
                        while (t[tofdl] - t[det] < 2048. * 5. / 2.)
                        {
                            t[det] -= 2048. * 5.;
                        }
                        while (t[tofdl] - t[det] > 2048. * 5. / 2.)
                        {
                            t[det] += 2048. * 5.;
                        }

                        tof = t[tofdl] - t[det];

                        // Fill histograms before cuts
                        fh_Fib_ToF[det]->Fill(x[det] * 100., tof);
                        fh_xy_Fib[det]->Fill(x[det] * 100., y[det] * 100.);
                        fh_mult_Fib[det]->Fill(nHits10);
                        fh_ToT_Fib[det]->Fill(x[det] * 100., q[det]);
                        fh_Fibs_vs_Tofd[det]->Fill(x[tofdl] * 100., x[det] * 100.);
                        fh_Fib_vs_Events[det]->Fill(fNEvents, x[det] * 100.);
                        fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                        fh_Fib_vs_Fib[fi10][fi12]->Fill(x[fi12] * 100., x[fi10] * 100.);
                        fh_Fib_dx[fi10][fi12]->Fill(x[fi12] * 100., x[fi10] * 100. - x[fi12] * 100.);

                        // Cuts on Fi10
                        if (fCuts && ihit10 != iMax10)
                            continue;
                        if (fCuts && q[det] < 1)
                            continue;
                        if (fCuts && y[det] * 100. > 50.)
                            continue;
                        if (fCuts && y[det] * 100. < -50.)
                            continue;
                        if (fCuts && (tof < -100 || tof > 0))
                            continue;

                        mult10++;

                        //						if (fCuts && mult10 > 50)
                        //							continue;

                        // Fill histograms
                        fh_Fib_ToF_ac[det]->Fill(x[det] * 100., tof);
                        fh_xy_Fib_ac[det]->Fill(x[det] * 100., y[det] * 100.);
                        fh_mult_Fib_ac[det]->Fill(mult10);
                        fh_ToT_Fib_ac[det]->Fill(x[det] * 100., q[det]);
                        fh_Fibs_vs_Tofd_ac[det]->Fill(x[tofdl] * 100., x[det] * 100.);
                        fh_Fib_vs_Events_ac[det]->Fill(fNEvents, x[det] * 100.);
                        fh_ToF_vs_Events_ac[det]->Fill(fNEvents, tof);
                        fh_Fib_vs_Fib_ac[fi10][fi12]->Fill(x[fi12] * 100., x[fi10] * 100.);
                        fh_Fib_dx_ac[fi10][fi12]->Fill(x[fi12] * 100., x[fi10] * 100. - x[fi12] * 100.);

                        LOG(DEBUG) << "Fi10: " << ihit10 << " x: " << x[det] << " y: " << y[det] << " q: " << q[det]
                                   << " t: " << t[det] << endl;
                        ptn[det] = true;

                        // here call tracker
                        Bool_t det_coord = true;

                        ptn[0] = false;
                        ptn[3] = false;
                        ptn[5] = false;

                        // here call tracker
                        if (tracker)
                        {
                            single_track_from_cpp_(
                                &n_det, &det_coord, x, y, z, ptn, track_parameter, chi_single_parameter);
                            if (chi_single_parameter[0] < chi2Minl[0])
                            {
                                chi2Minl[0] = chi_single_parameter[0];
                                chi2Minl[1] = chi_single_parameter[1];
                                for (Int_t i = 0; i < 6; i++)
                                {
                                    track_parameter_best_l[i] = track_parameter[i];
                                }
                            }
                            cout << "chi2 " << chi_single_parameter[0] << "  " << chi_single_parameter[1] << endl;

                            //                            if (chi_single_parameter[0] < 1.)
                            //                                Output(track_parameter, chi_single_parameter);
                        }

                    } // end of loop Fi13

                } // end of loop Fi11
            }     // end of loop Fi3a

        } // end if left half

        if (ptn[tofdl] && chi2Minl[0] < 10.)
            Output(track_parameter_best_l, chi2Minl);
        if (ptn[tofdr] && chi2Minr[0] < 10.)
            Output(track_parameter_best_r, chi2Minr);

    } // end loop over ToFD

    fNEvents += 1;
}
void R3BGlobalAnalysisS454::Output(Double_t track_parameter[6], Double_t chi_single_parameter[2])
{
    Double_t q = 8.;
    LOG(DEBUG) << "******************************************" << endl;
    LOG(DEBUG) << "chi_single_parameter " << chi_single_parameter[0] << "  " << chi_single_parameter[1] << endl;
    LOG(DEBUG) << "xyz: " << track_parameter[0] * 100. << "  " << track_parameter[1] * 100. << "  "
               << track_parameter[2] * 100. << endl;
    LOG(DEBUG) << "p: " << track_parameter[3] * q << "  " << track_parameter[4] * q << "  " << track_parameter[5] * q
               << endl;
    Double_t p_tot = q * sqrt(track_parameter[3] * track_parameter[3] + track_parameter[4] * track_parameter[4] +
                              track_parameter[5] * track_parameter[5]);
    Double_t thetax_simu = Pxf / Pzf * 1000.;
    Double_t thetay_simu = Pyf / Pzf * 1000.;

    fh_target_xy->Fill(track_parameter[0] * 100., track_parameter[1] * 100.);
    fh_target_px->Fill(track_parameter[3] * q);
    fh_target_py->Fill(track_parameter[4] * q);
    fh_target_pz->Fill(track_parameter[5] * q);
    fh_target_p->Fill(p_tot);
    fh_chi2->Fill(chi_single_parameter[0]);

    fh_thetax_dpx->Fill((Pxf - track_parameter[3] * q) / Pxf * 100., thetax_simu);
    fh_thetax_dpx_abs->Fill((Pxf - track_parameter[3] * q), thetax_simu);
    fh_thetay_dpy->Fill((Pyf - track_parameter[4] * q) / Pyf * 100., thetay_simu);

    fh_thetax_dpy->Fill((Pyf - track_parameter[4] * q) / Pyf * 100., thetax_simu);
    fh_thetay_dpx->Fill((Pxf - track_parameter[3] * q) / Pxf * 100., thetay_simu);
    fh_dpy_dpx->Fill((Pxf - track_parameter[3] * q) / Pxf * 100., (Pyf - track_parameter[4] * q) / Pyf * 100.);

    fh_x_dpx->Fill((Pxf - track_parameter[3] * q) / Pxf * 100., Xf);
    fh_y_dpy->Fill((Pyf - track_parameter[4] * q) / Pyf * 100., Yf);
    fh_dx->Fill(Xf - track_parameter[0] * 100.);
    fh_dy->Fill(Yf - track_parameter[1] * 100.);
    fh_dz->Fill(Zf - track_parameter[2] * 100.);
    fh_dpx->Fill((Pxf - track_parameter[3] * q) / Pxf * 100.);
    fh_dpy->Fill((Pyf - track_parameter[4] * q) / Pyf * 100.);
    fh_dpz->Fill((Pzf - track_parameter[5] * q) / Pzf * 100.);
    fh_dp->Fill((Pf_tot - p_tot) / Pf_tot * 100.);
}
void R3BGlobalAnalysisS454::FinishEvent()
{

    for (Int_t det = 0; det < DET_MAX; det++)
    {
        if (fMappedItems.at(det))
        {
            fMappedItems.at(det)->Clear();
        }
        if (fCalItems.at(det))
        {
            fCalItems.at(det)->Clear();
        }
        if (fHitItems.at(det))
        {
            fHitItems.at(det)->Clear();
        }
    }
}

void R3BGlobalAnalysisS454::FinishTask()
{
    fh_Tpat->Write();
    fh_Trigger->Write();
    fh_Cave_position->Write();
    fh_TOFDOR->Write();
    fh_SEE->Write();
    fh_IC->Write();

    fh_target_xy->Write();
    fh_target_px->Write();
    fh_target_py->Write();
    fh_target_pz->Write();
    fh_target_p->Write();
    fh_chi2->Write();

    fh_dx->Write();
    fh_dy->Write();
    fh_dz->Write();
    fh_dpx->Write();
    fh_dpy->Write();
    fh_dpz->Write();
    fh_dp->Write();
    fh_thetax_dpx->Write();
    fh_thetax_dpx_abs->Write();
    fh_thetay_dpy->Write();
    fh_thetax_dpy->Write();
    fh_thetay_dpx->Write();
    fh_x_dpx->Write();
    fh_y_dpy->Write();
    fh_dpy_dpx->Write();

    if (fHitItems.at(DET_TOFD))
    {
        fh_xy_tofd->Write();
        fh_xy_tofd_ac->Write();
        fh_tofd_charge->Write();
        fh_tofd_charge_ac->Write();
        fh_TimePreviousEvent->Write();
        fh_tofd_mult->Write();
        fh_tofd_mult_ac->Write();
    }

    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        if (fMappedItems.at(ifibcount + DET_FI_FIRST))
        {
            fh_xy_Fib[ifibcount]->Write();
            fh_xy_Fib_ac[ifibcount]->Write();
            fh_mult_Fib[ifibcount]->Write();
            fh_mult_Fib_ac[ifibcount]->Write();
            fh_ToT_Fib[ifibcount]->Write();
            fh_ToT_Fib_ac[ifibcount]->Write();
            fh_Fib_vs_Events[ifibcount]->Write();
            fh_Fib_vs_Events_ac[ifibcount]->Write();
            fh_Fibs_vs_Tofd[ifibcount]->Write();
            fh_Fibs_vs_Tofd_ac[ifibcount]->Write();
            fh_Fib_ToF[ifibcount]->Write();
            fh_Fib_ToF_ac[ifibcount]->Write();
            fh_ToF_vs_Events[ifibcount]->Write();
            fh_ToF_vs_Events_ac[ifibcount]->Write();
        }
    }

    for (Int_t i = 0; i < NOF_FIB_DET; i++)
    {
        for (Int_t j = i + 1; j < NOF_FIB_DET; j++)
        {
            if (fHitItems.at(i + DET_FI_FIRST) && fHitItems.at(j + DET_FI_FIRST))
            {
                fh_Fib_vs_Fib[i][j]->Write();
                fh_Fib_vs_Fib_ac[i][j]->Write();
                fh_Fib_dx[i][j]->Write();
                fh_Fib_dx_ac[i][j]->Write();
            }
        }
    }
}

ClassImp(R3BGlobalAnalysisS454)
