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
// -----                  R3BTrackerTestS494                -----
// -----          Created Mai 28th 2020 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BTrackerTestS494.h"
#include "R3BFibPoint.h"
#include "R3BMCTrack.h"
#include "R3BTofdPoint.h"
#include "R3BTrack.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"

#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"

#include "TCutG.h"
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

R3BTrackerTestS494::R3BTrackerTestS494()
    : R3BTrackerTestS494("Track", 1)
{
}

R3BTrackerTestS494::R3BTrackerTestS494(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fCuts(0)
    , fGraphCuts(0)
    , fPairs(0)
    , fB(-1672)
    , fSimu(0)
    , fNEvents(0)
    , fTrackItems(new TClonesArray("R3BTrack"))
    , fNofTrackItems()
{
}

R3BTrackerTestS494::~R3BTrackerTestS494()
{
    for (int i = 0; i < NOF_FIB_DET; i++)
    {
        delete fh_xy_Fib[i];
        delete fh_mult_Fib[i];
        delete fh_ToT_Fib[i];
    }
    delete fTrackItems;
}

InitStatus R3BTrackerTestS494::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(INFO) << "R3BTrackerTestS494::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    FairRunOnline* run = FairRunOnline::Instance();

    fTofdPoints = (TClonesArray*)mgr->GetObject("TOFdPoint");
    fFi23aPoints = (TClonesArray*)mgr->GetObject("Fi23aPoint");
    fFi23bPoints = (TClonesArray*)mgr->GetObject("Fi23bPoint");
    fFi30Points = (TClonesArray*)mgr->GetObject("Fi30Point");
    fFi31Points = (TClonesArray*)mgr->GetObject("Fi31Point");
    fFi32Points = (TClonesArray*)mgr->GetObject("Fi32Point");
    fFi33Points = (TClonesArray*)mgr->GetObject("Fi33Point");

    fMCTrack = (TClonesArray*)mgr->GetObject("MCTrack");
    if (fMCTrack)
        mgr->Register("MCTrack", "Monte Carlo Tracks", fMCTrack, kTRUE);

    mgr->Register("Track", "Land", fTrackItems, kTRUE);

    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------

    char strNameC[255];
    sprintf(strNameC, "Cave_C_position");
    fh_Cave_position = new TH2F(strNameC, "", 210, -100., 200., 1000, -500., 500.);

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

    fh_px_He = new TH1F("px_He", " px He", 2000, -500., 500);
    fh_px_He->GetXaxis()->SetTitle("px / MeV/c");
    fh_px_He->GetYaxis()->SetTitle("counts");

    fh_py_He = new TH1F("py_He", " py He", 2000, -500., 500);
    fh_py_He->GetXaxis()->SetTitle("py / MeV/c");
    fh_py_He->GetYaxis()->SetTitle("counts");

    fh_pz_He = new TH1F("pz_He", " pz He", 30000, 0., 30000.);
    fh_pz_He->GetXaxis()->SetTitle("pz / MeV/c");
    fh_pz_He->GetYaxis()->SetTitle("counts");

    fh_p_He = new TH1F("p_He", " p He", 30000, 0., 30000.);
    fh_p_He->GetXaxis()->SetTitle("pz / MeV/c");
    fh_p_He->GetYaxis()->SetTitle("counts");

    fh_px_C = new TH1F("px_C", " px C", 2000, -500., 500);
    fh_px_C->GetXaxis()->SetTitle("px / MeV/c");
    fh_px_C->GetYaxis()->SetTitle("counts");

    fh_py_C = new TH1F("py_C", " py C", 2000, -500., 500);
    fh_py_C->GetXaxis()->SetTitle("py / MeV/c");
    fh_py_C->GetYaxis()->SetTitle("counts");

    fh_pz_C = new TH1F("pz_C", " pz C", 30000, 0., 30000.);
    fh_pz_C->GetXaxis()->SetTitle("pz / MeV/c");
    fh_pz_C->GetYaxis()->SetTitle("counts");

    fh_p_C = new TH1F("p_C", " p C", 30000, 0., 30000.);
    fh_p_C->GetXaxis()->SetTitle("pz / MeV/c");
    fh_p_C->GetYaxis()->SetTitle("counts");

    fh_dpx_He = new TH1F("dpx_He", "dpx He", 400, -20., 20.);
    fh_dpx_He->GetXaxis()->SetTitle("dpx / percent");
    fh_dpx_He->GetYaxis()->SetTitle("counts");

    fh_dpy_He = new TH1F("dpy_He", "dpy He", 400, -20., 20.);
    fh_dpy_He->GetXaxis()->SetTitle("dpy / percent");
    fh_dpy_He->GetYaxis()->SetTitle("counts");

    fh_dpz_He = new TH1F("dpz_He", "dpz He", 100, -5., 5.);
    fh_dpz_He->GetXaxis()->SetTitle("dpz / percent");
    fh_dpz_He->GetYaxis()->SetTitle("counts");

    fh_dp_He = new TH1F("dp_He", "dp He", 100, -5., 5.);
    fh_dp_He->GetXaxis()->SetTitle("dp / percent");
    fh_dp_He->GetYaxis()->SetTitle("counts");

    fh_dpx_C = new TH1F("dpx_C", "dpx C", 400, -20., 20.);
    fh_dpx_C->GetXaxis()->SetTitle("dpx / percent");
    fh_dpx_C->GetYaxis()->SetTitle("counts");

    fh_dpy_C = new TH1F("dpy_C", "dpy C", 400, -20., 20.);
    fh_dpy_C->GetXaxis()->SetTitle("dpy / percent");
    fh_dpy_C->GetYaxis()->SetTitle("counts");

    fh_dpz_C = new TH1F("dpz_C", "dpz C", 100, -5., 5.);
    fh_dpz_C->GetXaxis()->SetTitle("dpz / percent");
    fh_dpz_C->GetYaxis()->SetTitle("counts");

    fh_dp_C = new TH1F("dp_C", "dp C", 100, -5., 5.);
    fh_dp_C->GetXaxis()->SetTitle("dp / percent");
    fh_dp_C->GetYaxis()->SetTitle("counts");

    fh_target_xy = new TH2F("target_xy", "target xy ", 100, -0.5, 0.5, 100, -0.5, 0.5);
    fh_target_xy->GetXaxis()->SetTitle("x / cm");
    fh_target_xy->GetYaxis()->SetTitle("y / cm");

    fh_target_dxdy = new TH2F("target_dxdy", "target dx dy ", 100, -0.5, 0.5, 100, -0.5, 0.5);
    fh_target_dxdy->GetXaxis()->SetTitle("x / cm");
    fh_target_dxdy->GetYaxis()->SetTitle("y / cm");

    fh_chi2 = new TH1F("chi2", "chi2 ", 1000, 0., 100);
    fh_chi2->GetXaxis()->SetTitle("Chi2");
    fh_chi2->GetYaxis()->SetTitle("counts");

    fh_dx = new TH1F("tracker_dx", "tracker dx ", 200, -5., 5);
    fh_dx->GetXaxis()->SetTitle("dx / cm");
    fh_dx->GetYaxis()->SetTitle("counts");

    fh_dy = new TH1F("tracker_dy", "tracker dy ", 200, -5., 5.);
    fh_dy->GetXaxis()->SetTitle("dy / cm");
    fh_dy->GetYaxis()->SetTitle("counts");

    fh_dz = new TH1F("tracker_dz", "tracker dz ", 200, -5., 5.);
    fh_dz->GetXaxis()->SetTitle("dz / cm");
    fh_dz->GetYaxis()->SetTitle("counts");

    fh_dpx = new TH1F("tracker_dpx", "tracker dpx ", 200, -10, 10);
    fh_dpx->GetXaxis()->SetTitle("dpx / percent");
    fh_dpx->GetYaxis()->SetTitle("counts");

    fh_dpy = new TH1F("tracker_dpy", "tracker dpy ", 200, -10, 10);
    fh_dpy->GetXaxis()->SetTitle("dpy / percent");
    fh_dpy->GetYaxis()->SetTitle("counts");

    fh_dpz = new TH1F("tracker_dpz", "tracker dpz ", 200, -10, 10);
    fh_dpz->GetXaxis()->SetTitle("dpz / percent");
    fh_dpz->GetYaxis()->SetTitle("counts");

    fh_dp = new TH1F("tracker_dp", "tracker dp ", 200, -10, 10);
    fh_dp->GetXaxis()->SetTitle("dp / percent");
    fh_dp->GetYaxis()->SetTitle("counts");

    fh_thetax_dpx = new TH2F("tracker_thethax_dpx", "tracker theta_x vs dpx ", 100, -50, 50, 200, -100., 100.);
    fh_thetax_dpx->GetXaxis()->SetTitle("dpx / percent");
    fh_thetax_dpx->GetYaxis()->SetTitle("theta_x / mrad");

    fh_dpy_dpx = new TH2F("tracker_dpy_dpx", "tracker dpy vs dpx ", 200, -100, 100, 200, -100., 100.);
    fh_dpy_dpx->GetXaxis()->SetTitle("dpx / percent");
    fh_dpy_dpx->GetYaxis()->SetTitle("dpy / percent");

    fh_thetay_dpy = new TH2F("tracker_thethay_dpy", "tracker theta_y vs dpy ", 100, -50, 50, 200, -100., 100.);
    fh_thetay_dpy->GetXaxis()->SetTitle("dpy / percent");
    fh_thetay_dpy->GetYaxis()->SetTitle("theta_y / mrad");

    fh_thetax_dpy = new TH2F("tracker_thethax_dpy", "tracker theta_x vs dpy ", 100, -50, 50, 200, -100., 100.);
    fh_thetax_dpy->GetXaxis()->SetTitle("dpy / percent");
    fh_thetax_dpy->GetYaxis()->SetTitle("theta_x / mrad");

    fh_thetay_dpx = new TH2F("tracker_thethay_dpx", "tracker theta_y vs dpx ", 100, -50, 50, 200, -100., 100.);
    fh_thetay_dpx->GetXaxis()->SetTitle("dpx / percent");
    fh_thetay_dpx->GetYaxis()->SetTitle("theta_y / mrad");

    fh_thetax_dpx_abs =
        new TH2F("tracker_thethax_dpx_abs", "tracker theta_x vs dpx abs ", 100, -50, 50, 200, -100., 100.);
    fh_thetax_dpx_abs->GetXaxis()->SetTitle("dpx");
    fh_thetax_dpx_abs->GetYaxis()->SetTitle("theta_x / mrad");

    fh_x_dpx = new TH2F("tracker_x_dpx", "tracker x vs dpx ", 100, -50, 50, 200, -10., 10.);
    fh_x_dpx->GetXaxis()->SetTitle("dpx / percent");
    fh_x_dpx->GetYaxis()->SetTitle("x / cm");

    fh_y_dpy = new TH2F("tracker_y_dpy", "tracker y vs dpy ", 100, -50, 50, 200, -10., 10.);
    fh_y_dpy->GetXaxis()->SetTitle("dpy / percent");
    fh_y_dpy->GetYaxis()->SetTitle("y / cm");

    // histograms for track hits
    for (Int_t i = 0; i < ndet; i++)
    {

        fh_res_xA[i] = new TH1F(Form("resA_x_%i", i), Form("x-residuals of Det %i", i), 1000, -1, 1);
        fh_res_xA[i]->GetXaxis()->SetTitle("x-residuals / cm");
        fh_res_xA[i]->GetYaxis()->SetTitle("Counts");

        fh_res_yA[i] = new TH1F(Form("resA_y_%i", i), Form("y-residuals of Det %i", i), 1000, -1, 1);
        fh_res_yA[i]->GetXaxis()->SetTitle("y-residuals / cm");
        fh_res_yA[i]->GetYaxis()->SetTitle("Counts");
		
        fh_res_xC[i] = new TH1F(Form("resC_x_%i", i), Form("x-residuals of Det %i", i), 1000, -1, 1);
        fh_res_xC[i]->GetXaxis()->SetTitle("x-residuals / cm");
        fh_res_xC[i]->GetYaxis()->SetTitle("Counts");

        fh_res_yC[i] = new TH1F(Form("resC_y_%i", i), Form("y-residuals of Det %i", i), 1000, -1, 1);
        fh_res_yC[i]->GetXaxis()->SetTitle("y-residuals / cm");
        fh_res_yC[i]->GetYaxis()->SetTitle("Counts");

        fh_xy[i] = new TH2F(Form("xy_%i", i), Form("xy of Det %i", i), 600, -30, 30, 1200, -60., 60.);
        fh_xy[i]->GetXaxis()->SetTitle("x / cm");
        fh_xy[i]->GetYaxis()->SetTitle("y / cm");

        fh_p_vs_x[i] = new TH2F(Form("PvsX%i", i), Form("pz vs. x of Det %i", i), 1200, -60, 60, 4000, 0., 40000.);
        fh_p_vs_x[i]->GetXaxis()->SetTitle("x position / cm");
        fh_p_vs_x[i]->GetYaxis()->SetTitle("p / MeV/c");

        fh_p_vs_x_test[i] =
            new TH2F(Form("PvsX_test%i", i), Form("p vs. x of Det %i test", i), 1200, -60, 60, 4000, 0., 40000.);
        fh_p_vs_x_test[i]->GetXaxis()->SetTitle("x position / cm");
        fh_p_vs_x_test[i]->GetYaxis()->SetTitle("p / MeV/c");
    }
    //-----------------------------------------------------------------------
    // Fiber Detectors 1-NOF_FIB_DET

    char canvName[255];
    UInt_t Nmax = 1e7;
    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {

        const char* detName;
        const char* detName2;
        detName = fDetectorNames[DET_FI_FIRST + ifibcount];

        LOG(DEBUG) << "I am creating canvas " << detName << endl;

        // xy:
        fh_xy_Fib[ifibcount] =
            new TH2F(Form("%s_xy", detName), Form("%s xy", detName), 600, -30., 30., 200, -1000., 1000.);
        fh_xy_Fib[ifibcount]->GetXaxis()->SetTitle("x / cm ");
        fh_xy_Fib[ifibcount]->GetYaxis()->SetTitle("y / cm");

        fh_xy_Fib_ac[ifibcount] =
            new TH2F(Form("%s_xy_ac", detName), Form("%s xy after cuts", detName), 600, -30., 30., 200, -100., 100.);
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
            new TH2F(Form("%s_tot_m", detName), Form("%s ToT of MAPMT", detName), 600, -30., 30, 400, 0., 400.);
        fh_ToT_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber x / cm");
        fh_ToT_Fib[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

        fh_ToT_Fib_ac[ifibcount] = new TH2F(
            Form("%s_tot_m_ac", detName), Form("%s ToT of MAPMT after cuts", detName), 600, -30., 30, 400, 0., 400.);
        fh_ToT_Fib_ac[ifibcount]->GetXaxis()->SetTitle("Fiber x / cm");
        fh_ToT_Fib_ac[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

        // ToF Tofd -> Fiber:
        fh_Fib_ToF[ifibcount] = new TH2F(
            Form("%s_tof", detName), Form("%s ToF Tofd to Fiber", detName), 600, -30., 30, 10000, -1100., 1100.);
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

        // Time:
        fh_Fib_Time[ifibcount] =
            new TH2F(Form("%s_time", detName), Form("%s Time", detName), 600, -30., 30, 4000, -4000., 4000.);
        fh_Fib_Time[ifibcount]->GetYaxis()->SetTitle("Time / ns");
        fh_Fib_Time[ifibcount]->GetXaxis()->SetTitle("x / cm");

        fh_Fib_Time_ac[ifibcount] = new TH2F(
            Form("%s_time_ac", detName), Form("%s Time after cuts", detName), 600, -30., 30, 4000, -4000., 4000.);
        fh_Fib_Time_ac[ifibcount]->GetYaxis()->SetTitle("Time / ns");
        fh_Fib_Time_ac[ifibcount]->GetXaxis()->SetTitle("x / cm");

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
        fh_Fib_vs_Events[ifibcount] = new TH2F(
            Form("%s_fib_vs_event", detName), Form("%s Fiber # vs. Event #", detName), 10000, 0, Nmax, 600, -30., 30.);
        fh_Fib_vs_Events[ifibcount]->GetYaxis()->SetTitle("Fiber x / cm");
        fh_Fib_vs_Events[ifibcount]->GetXaxis()->SetTitle("Event number");

        fh_Fib_vs_Events_ac[ifibcount] = new TH2F(Form("%s_fib_vs_event_ac", detName),
                                                  Form("%s Fiber # vs. Event # after cuts", detName),
                                                  10000,
                                                  0,
                                                  Nmax,
                                                  600,
                                                  -30.,
                                                  30.);
        fh_Fib_vs_Events_ac[ifibcount]->GetYaxis()->SetTitle("Fiber x / cm");
        fh_Fib_vs_Events_ac[ifibcount]->GetXaxis()->SetTitle("Event number");

        // hit fiber number vs. TofD position:
        fh_Fibs_vs_Tofd[ifibcount] = new TH2F(Form("%s_fib_vs_TofdX", detName),
                                              Form("%s Fiber # vs. Tofd x-pos", detName),
                                              200,
                                              -100,
                                              100,
                                              600,
                                              -30.,
                                              30.);
        fh_Fibs_vs_Tofd[ifibcount]->GetYaxis()->SetTitle("Fiber x / cm");
        fh_Fibs_vs_Tofd[ifibcount]->GetXaxis()->SetTitle("Tofd x / cm");

        fh_Fibs_vs_Tofd_ac[ifibcount] = new TH2F(Form("%s_fib_vs_TofdX_ac", detName),
                                                 Form("%s Fiber # vs. Tofd x-pos after cuts", detName),
                                                 200,
                                                 -100,
                                                 100,
                                                 600,
                                                 -30.,
                                                 30.);
        fh_Fibs_vs_Tofd_ac[ifibcount]->GetYaxis()->SetTitle("Fiber x / cm");
        fh_Fibs_vs_Tofd_ac[ifibcount]->GetXaxis()->SetTitle("Tofd x / cm");

        // hit fiber vs. fiber position:

    } // end for(ifibcount)
    fh_Fib33_vs_Fib31 = new TH2F("fib33_vs_fib31", "Fiber 33 vs. Fiber 31", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib33_vs_Fib31->GetYaxis()->SetTitle("Fiber33");
    fh_Fib33_vs_Fib31->GetXaxis()->SetTitle("Fiber31");

    fh_Fib31_vs_Fib23a = new TH2F("fib31_vs_Fib23a", "Fiber 31 vs. Fiber 23a", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib31_vs_Fib23a->GetYaxis()->SetTitle("Fiber31");
    fh_Fib31_vs_Fib23a->GetXaxis()->SetTitle("Fiber3a");

    fh_Fib30_vs_Fib32 = new TH2F("fib30_vs_fib32", "Fiber 30 vs. Fiber 32", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib30_vs_Fib32->GetYaxis()->SetTitle("Fiber30");
    fh_Fib30_vs_Fib32->GetXaxis()->SetTitle("Fiber32");

    fh_Fib32_vs_Fib23b = new TH2F("fib32_vs_Fib23b", "Fiber 32 vs. Fiber 23b", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib32_vs_Fib23b->GetYaxis()->SetTitle("Fiber32");
    fh_Fib32_vs_Fib23b->GetXaxis()->SetTitle("Fiber3b");

    // dx between fibers vs x
    fh_Fib33_vs_Fib31_dx = new TH2F("fib33_fib31_dx", "dx of Fiber 33 and Fiber 31", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib33_vs_Fib31_dx->GetYaxis()->SetTitle("xFi33 - xFi31 / cm");
    fh_Fib33_vs_Fib31_dx->GetXaxis()->SetTitle("x Fi31 / cm");

    fh_Fib31_vs_Fib23a_dx = new TH2F("fib31_Fib23a_dx", "dx of Fiber 31 and Fiber 23a", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib31_vs_Fib23a_dx->GetYaxis()->SetTitle("xFi31 - xFi23a / cm");
    fh_Fib31_vs_Fib23a_dx->GetXaxis()->SetTitle("x Fi23a / cm");

    fh_Fib30_vs_Fib32_dx = new TH2F("fib30_fib32_dx", "dx of Fiber 30 and Fiber 32", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib30_vs_Fib32_dx->GetYaxis()->SetTitle("xFi30 - xFi32 / cm");
    fh_Fib30_vs_Fib32_dx->GetXaxis()->SetTitle("x Fi32 / cm");

    fh_Fib32_vs_Fib23b_dx = new TH2F("fib32_Fib23b_dx", "dx of Fiber 32 and Fiber 23b", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib32_vs_Fib23b_dx->GetYaxis()->SetTitle("xFi32 - xFi23b / cm");
    fh_Fib32_vs_Fib23b_dx->GetXaxis()->SetTitle("x Fi23b / cm");

    fh_Fib33_vs_Fib31_back =
        new TH2F("fib33_vs_fib31_back", "Fiber 33 vs. Fiber 31 back", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib33_vs_Fib31_back->GetYaxis()->SetTitle("Fiber33");
    fh_Fib33_vs_Fib31_back->GetXaxis()->SetTitle("Fiber31");

    fh_Fib31_vs_Fib23a_back =
        new TH2F("fib31_vs_Fib23a_back", "Fiber 31 vs. Fiber 23a back", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib31_vs_Fib23a_back->GetYaxis()->SetTitle("Fiber31");
    fh_Fib31_vs_Fib23a_back->GetXaxis()->SetTitle("Fiber3a");

    fh_Fib30_vs_Fib32_back =
        new TH2F("fib30_vs_fib32_back", "Fiber 30 vs. Fiber 32 back", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib30_vs_Fib32_back->GetYaxis()->SetTitle("Fiber30");
    fh_Fib30_vs_Fib32_back->GetXaxis()->SetTitle("Fiber32");

    fh_Fib32_vs_Fib23b_back =
        new TH2F("fib32_vs_Fib23b_back", "Fiber 32 vs. Fiber 23b back", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib32_vs_Fib23b_back->GetYaxis()->SetTitle("Fiber32");
    fh_Fib32_vs_Fib23b_back->GetXaxis()->SetTitle("Fiber3b");

    // dx between fibers vs x
    fh_Fib33_vs_Fib31_dx_back =
        new TH2F("fib33_fib31_dx_back", "dx of Fiber 33 and Fiber 31 back", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib33_vs_Fib31_dx_back->GetYaxis()->SetTitle("xFi33 - xFi31 / cm");
    fh_Fib33_vs_Fib31_dx_back->GetXaxis()->SetTitle("x Fi31 / cm");

    fh_Fib31_vs_Fib23a_dx_back =
        new TH2F("fib31_Fib23a_dx_back", "dx of Fiber 31 and Fiber 23a back", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib31_vs_Fib23a_dx_back->GetYaxis()->SetTitle("xFi31 - xFi23a / cm");
    fh_Fib31_vs_Fib23a_dx_back->GetXaxis()->SetTitle("x Fi23a / cm");

    fh_Fib30_vs_Fib32_dx_back =
        new TH2F("fib30_fib32_dx_back", "dx of Fiber 30 and Fiber 32 back", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib30_vs_Fib32_dx_back->GetYaxis()->SetTitle("xFi30 - xFi32 / cm");
    fh_Fib30_vs_Fib32_dx_back->GetXaxis()->SetTitle("x Fi32 / cm");

    fh_Fib32_vs_Fib23b_dx_back =
        new TH2F("fib32_Fib23b_dx_back", "dx of Fiber 32 and Fiber 23b back", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib32_vs_Fib23b_dx_back->GetYaxis()->SetTitle("xFi32 - xFi23b / cm");
    fh_Fib32_vs_Fib23b_dx_back->GetXaxis()->SetTitle("x Fi23b / cm");

    //---------------------------------------------------------------------------------------------------
    // TofD detector

    // xy:
    fh_xy_tofd = new TH2F("tofd_xy", "tofd xy", 200, -100., 100., 200, -100., 100.);
    fh_xy_tofd->GetXaxis()->SetTitle("x / cm ");
    fh_xy_tofd->GetYaxis()->SetTitle("y / cm");

    fh_xy_tofd_ac = new TH2F("tofd_xy_ac", "tofd xy after cuts", 200, -100., 100., 200, -100., 100.);
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

    fh_TimePreviousEvent = new TH1F("TimePreviousEvent", "Time between 2 particles ", 3000, 0, 3000);
    fh_TimePreviousEvent->GetXaxis()->SetTitle("time / ns");
    fh_TimePreviousEvent->GetYaxis()->SetTitle("counts");

    fh_tofd_time = new TH1F("tofd_time", "Tofd times ", 40000, -2000, 2000);
    fh_tofd_time->GetXaxis()->SetTitle("time / ns");
    fh_tofd_time->GetYaxis()->SetTitle("counts");

    fh_tofd_time_ac = new TH1F("tofd_time_ac", "Tofd times after cut", 40000, -2000, 2000);
    fh_tofd_time_ac->GetXaxis()->SetTitle("time / ns");
    fh_tofd_time_ac->GetYaxis()->SetTitle("counts");

    fh_tofd_q2_vs_q1 = new TH2F("tofd_q2_vs_q1", "tofd q2 vs. q1", 500, 0., 50., 500, 0., 50.);
    fh_tofd_q2_vs_q1->GetXaxis()->SetTitle("q1");
    fh_tofd_q2_vs_q1->GetYaxis()->SetTitle("q2");

    fh_tofd_q2_vs_q1_ac = new TH2F("tofd_q2_vs_q1_ac", "tofd q2 vs. q1 after cut", 500, 0., 50., 500, 0., 50.);
    fh_tofd_q2_vs_q1_ac->GetXaxis()->SetTitle("q1");
    fh_tofd_q2_vs_q1_ac->GetYaxis()->SetTitle("q2");

    // Analysis
    fh_chiy_vs_chix = new TH2F("chiy_vs_chix", "chi y vs. x", 500, 0., 500., 500, 0., 500.);
    fh_chiy_vs_chix->GetXaxis()->SetTitle("chi x");
    fh_chiy_vs_chix->GetYaxis()->SetTitle("chi y");

    fh_chi2_vs_Erel = new TH2F("chi2_vs_Erel", "Chi2 vs. Erel", 100, 0., 10., 500, 0., 500.);
    fh_chi2_vs_Erel->GetXaxis()->SetTitle("Erel");
    fh_chi2_vs_Erel->GetYaxis()->SetTitle("chi2");

    fh_theta26 = new TH1F("theta26", "theta 26 ", 500, 0., 5);
    fh_theta26->GetXaxis()->SetTitle("angle / degree");
    fh_theta26->GetYaxis()->SetTitle("counts");

    fh_theta26_simu = new TH1F("theta26_simu", "theta 26 simulation", 500, 0., 5);
    fh_theta26_simu->GetXaxis()->SetTitle("angle / degree");
    fh_theta26_simu->GetYaxis()->SetTitle("counts");

    fh_Erel = new TH1F("Erel", "Erel ", 5000, 0., 100);
    fh_Erel->GetXaxis()->SetTitle("Erel / MeV");
    fh_Erel->GetYaxis()->SetTitle("counts");

    fh_totalPz = new TH1F("totalPz", "total Pz ", 5000, 17000., 18000.);
    fh_totalPz->GetXaxis()->SetTitle("Pz / MeV/c");
    fh_totalPz->GetYaxis()->SetTitle("counts");

    fh_totalPz_reco = new TH1F("totalPz_reco", "total Pz reconstructed ", 5000, 17000., 18000.);
    fh_totalPz_reco->GetXaxis()->SetTitle("Pz / MeV/c");
    fh_totalPz_reco->GetYaxis()->SetTitle("counts");

    fh_Erel_simu = new TH1F("Erel_simu", "Erel simulation", 6000, -10., 50);
    fh_Erel_simu->GetXaxis()->SetTitle("Erel / MeV");
    fh_Erel_simu->GetYaxis()->SetTitle("counts");

    fh_dErel_vs_x = new TH2F("dErel_vs_x", "delta Erel vs. x", 200, 0., 200., 100, 0., 10.);
    fh_dErel_vs_x->GetXaxis()->SetTitle("delta / cm");
    fh_dErel_vs_x->GetYaxis()->SetTitle("Erel / MeV");

    fh_dErel_vs_y = new TH2F("dErel_vs_y", "delta Erel vs. y", 200, -100., 100., 100, 0., 10.);
    fh_dErel_vs_y->GetXaxis()->SetTitle("TofD y / cm");
    fh_dErel_vs_y->GetYaxis()->SetTitle("Delta Erel / MeV");

    // -------------------------------------------------------------------------
    // Rene's tracker.

    if (tracker)
        init_from_cpp_();

    return kSUCCESS;
}

void R3BTrackerTestS494::Exec(Option_t* option)
{
    if (fNEvents / 10000. == (int)fNEvents / 10000)
        std::cout << "\rEvents: " << fNEvents << " / " << maxevent << " (" << (int)(fNEvents * 100. / maxevent)
                  << " %) "
                  << " Tofd: " << counterTofd << " tracked: " << counter2 << " chix: " << counter3
                  << " chiy: " << counter4 << std::flush;
    fNEvents += 1;

    cout << "New event ****************************** " << fNEvents << endl;

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    Bool_t debug = true;
    Bool_t debug1 = true;

    Double_t xTest = 0.;
    Double_t yTest = 0.;

    Int_t max = 10000;
    Int_t detector[max];
    Int_t detector_s[max];
    Int_t detector_ss[max];
    Double_t xdet[max];
    Double_t xdet_s[max];
    Double_t xdet_ss[max];
    Double_t ydet[max];
    Double_t ydet_s[max];
    Double_t ydet_ss[max];
    Double_t zdet[max];
    Double_t zdet_s[max];
    Double_t zdet_ss[max];
    Int_t qdet[max + 2];
    Int_t qdet_s[max + 2];
    Int_t qdet_ss[max + 2];

    if (fPairs)
    {
        qdet[max] = 2;
        qdet[max + 1] = 6;
    }
    if (!fPairs)
    {
        qdet[max] = 6;
        qdet[max + 1] = 6;
    }

    Double_t IDdet[max];

    Double_t xFi33[max];
    Double_t yFi33[max];
    Double_t qFi33[max];
    Double_t tFi33[max];
    Bool_t fFi33[max];
    Double_t xFi32[max];
    Double_t yFi32[max];
    Double_t qFi32[max];
    Double_t tFi32[max];
    Bool_t fFi32[max];
    Double_t xFi31[max];
    Double_t yFi31[max];
    Double_t qFi31[max];
    Double_t tFi31[max];
    Bool_t fFi31[max];
    Double_t xFi30[max];
    Double_t yFi30[max];
    Double_t qFi30[max];
    Double_t tFi30[max];
    Bool_t fFi30[max];
    Double_t xFi23a[max];
    Double_t yFi23a[max];
    Double_t qFi23a[max];
    Double_t tFi23a[max];
    Bool_t fFi23a[max];
    Double_t xFi23b[max];
    Double_t yFi23b[max];
    Double_t qFi23b[max];
    Double_t tFi23b[max];
    Bool_t fFi23b[max];
    for (int i = 0; i < max; i++)
    {
        fFi33[i] = false;
        fFi32[i] = false;
        fFi31[i] = false;
        fFi30[i] = false;
        fFi23a[i] = false;
        fFi23b[i] = false;
    }

    countdet = 0;

    Double_t track[12];   // x,y,z, px, py, pz
    Double_t track_s[12]; // x,y,z, px, py, pz
    Double_t chi[6];      // x,y,z, px, py, pz
    Double_t chi_s[6];    // x,y,z, px, py, pz

    Int_t n_det = 10;

    Double_t res1_det_x[n_det];
    Double_t res1_det_y[n_det];
    Double_t res1_det_z[n_det];
    Double_t res2_det_x[n_det];
    Double_t res2_det_y[n_det];
    Double_t res2_det_z[n_det];
    Double_t res1_lab_x[n_det];
    Double_t res1_lab_y[n_det];
    Double_t res1_lab_z[n_det];
    Double_t res2_lab_x[n_det];
    Double_t res2_lab_y[n_det];
    Double_t res2_lab_z[n_det];

    Double_t x[n_det];
    Double_t y[n_det];
    Double_t z[n_det];
    Double_t q[n_det];
    Double_t t[n_det];
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
    Double_t xMax[n_det];
    Double_t yMax[n_det];
    Double_t zMax[n_det];
    Double_t qMax[n_det];
    Double_t tMax[n_det];

    Double_t xTrack[n_det];
    Double_t yTrack[n_det];
    Double_t zTrack[n_det];
    Double_t qTrack[n_det];

    Bool_t pat1[2 * n_det];
    Bool_t pat2[2 * n_det];

    Int_t id, id1, id2;

    Int_t det = 0;
    Int_t det1 = 0;
    Int_t det2 = 0;

    Double_t target[3];
    Double_t track1_best[6]; // x,y,z, px, py, pz
    Double_t track2_best[6]; // x,y,z, px, py, pz
    Double_t chi_best[6];    // chi2, chi2_red

    Double_t chi2;
    Double_t chi2_best;

    target[0] = 0.;
    target[1] = 0.;
    target[2] = -0.162;

    for (int i = 0; i < n_det; i++)
    {
        x[i] = 0.;
        y[i] = 0.;
        z[i] = 0.;
        q[i] = 0.;
        t[i] = -1000.;

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

        xMax[i] = -1000.;
        yMax[i] = -1000.;
        zMax[i] = -1000.;
        qMax[i] = -1000.;
        tMax[i] = -1000.;
    }
    pHex = 0.;
    pHey = 0.;
    pHez = 0.;
    pCx = 0.;
    pCy = 0.;
    pCz = 0.;

    // is also number of ifibcount
    Int_t fi23a = 0;
    Int_t fi23b = 1;
    Int_t fi30 = 2;
    Int_t fi31 = 3;
    Int_t fi32 = 4;
    Int_t fi33 = 5;
    Int_t tofd1r = 6;
    Int_t tofd1l = 7;
    Int_t tofd2r = 8;
    Int_t tofd2l = 9;

    Double_t tof = 0.;
    Bool_t pair = false;
    Bool_t single = false;
    Double_t tStart = 0.;
    Bool_t first = true;
    Bool_t alpha = false;
    Bool_t carbon = false;

    for (Int_t i = 0; i < 10; i++)
    {
        tPrev[i] = -1000.;
        detPrev[i] = -1;
    }

    Bool_t coinc = false;

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
            LOG(DEBUG) << "PID " << PID << endl;
            if (mother < 0)
            {
                if (PID == 1000020040)
                {
                    // 4He
                    XHes = aTrack->GetStartX();
                    YHes = aTrack->GetStartY();
                    ZHes = aTrack->GetStartZ();
                    THes = aTrack->GetStartT();

                    pHexs = aTrack->GetPx() * 1000.;
                    pHeys = aTrack->GetPy() * 1000.;
                    pHezs = aTrack->GetPz() * 1000.;
                    pHes = sqrt((pHexs * pHexs) + (pHeys * pHeys) + (pHezs * pHezs));

                    LOG(DEBUG) << "******************************************" << endl;
                    LOG(DEBUG) << "Track In 4He"
                               << "x " << XHes << " y " << YHes << " z " << ZHes << endl;
                    LOG(DEBUG) << "px " << pHexs << " py " << pHeys << " z " << pHezs << endl;
                }
                if (PID == 1000060120)
                {
                    // 12C
                    XCs = aTrack->GetStartX();
                    YCs = aTrack->GetStartY();
                    ZCs = aTrack->GetStartZ();
                    TCs = aTrack->GetStartT();

                    pCxs = aTrack->GetPx() * 1000.;
                    pCys = aTrack->GetPy() * 1000.;
                    pCzs = aTrack->GetPz() * 1000.;
                    pCs = sqrt((pCxs * pCxs) + (pCys * pCys) + (pCzs * pCzs));

                    LOG(DEBUG) << "******************************************" << endl;
                    LOG(DEBUG) << "Track In 12C"
                               << "x " << XCs << " y " << YCs << " z " << ZCs << endl;
                    LOG(DEBUG) << "px " << pCxs << " py " << pCys << " z " << pCzs << endl;
                }
                //                if (PID == 1000080160)
                //                if (PID == 1000020040)
                if (PID == 1000060120)
                {
                    // 16C
                    Xf = aTrack->GetStartX();
                    Yf = aTrack->GetStartY();
                    Zf = aTrack->GetStartZ();

                    Pxf = aTrack->GetPx() * 1000.;
                    Pyf = aTrack->GetPy() * 1000.;
                    Pzf = aTrack->GetPz() * 1000.;
                    Pf_tot = sqrt((Pxf * Pxf) + (Pyf * Pyf) + (Pzf * Pzf));

                    LOG(DEBUG) << "******************************************" << endl;
                    LOG(DEBUG) << "Track In 16O"
                               << "x " << Xf << " y " << Yf << " z " << Zf << endl;
                    LOG(DEBUG) << "px " << Pxf << " py " << Pyf << " z " << Pzf << endl;
                }
            }
        }
	fh_totalPz->Fill(pCzs + pHezs);
    }

    Double_t randx;
    randx = (std::rand() / (float)RAND_MAX) * 2.8 - 1.4;
    Double_t x_4He = 0.;
    Double_t y_4He = 0.;
    Double_t z_4He = 0.;
    Double_t x_12C = 0.;
    Double_t y_12C = 0.;
    Double_t z_12C = 0.;

    Int_t qqq = 0;

    Int_t nHits = fTofdPoints->GetEntriesFast();

    Int_t mult23a = 0;
    Int_t mult23b = 0;
    Int_t mult30 = 0;
    Int_t mult31 = 0;
    Int_t mult32 = 0;
    Int_t mult33 = 0;

    Bool_t C_left = false;
    Bool_t C_right = false;
    Bool_t A_left = false;
    Bool_t A_right = false;
    for (Int_t i = 0; i < nHits; i++)
    {
        R3BTofdPoint* hit = (R3BTofdPoint*)fTofdPoints->At(i);
        Int_t ID = hit->GetDetectorID();
        //		if ((ID == 122 || ID == 123 || ID == 124 || ID == 222 || ID == 223))
        //			&& TMath::Abs((hit->GetYIn() + hit->GetYOut()) / 2.) < 5.)
        //			continue;

        //        qqq = sqrt(hit->GetEnergyLoss()) * 23.211 + 0.5; // For 300 AMeV
        qqq = sqrt(hit->GetEnergyLoss()) * 26.62 + 0.5; // For 500 AMeV

        if (ID <= 122)
        {
            detector[countdet] = 6;
            if (qqq == 6)
                C_right = true;
            if (qqq == 2)
                A_right = true;
        }
        if (ID >= 122 && ID < 145)
        {
            detector[countdet] = 7;
            if (qqq == 6)
                C_left = true;
            if (qqq == 2)
                A_left = true;
        }
        if (ID > 200 && ID <= 222)
        {
            detector[countdet] = 8;
            if (qqq == 6)
                C_left = true;
            if (qqq == 2)
                A_left = true;
        }
        if (ID > 222)
        {
            detector[countdet] = 9;
            if (qqq == 6)
                C_right = true;
            if (qqq == 2)
                A_right = true;
        }

        if (qqq < 2)
            continue;
        xdet[countdet] = (hit->GetXIn() + hit->GetXOut()) / 2. / 100.;
        ydet[countdet] = (hit->GetYIn() + hit->GetYOut()) / 2. / 100.;
        zdet[countdet] = (hit->GetZIn() + hit->GetZOut()) / 2. / 100.;
        qdet[countdet] = qqq;
        IDdet[countdet] = ID;
        if (qqq == 6)
        {
            cout << "charge C: " << qqq << endl;
            carbon = true;
            x_12C = xdet[countdet];
            y_12C = ydet[countdet];
            z_12C = zdet[countdet];
        }
        if (qqq == 2)
        {
            cout << "charge He: " << qqq << endl;
            alpha = true;
            x_4He = xdet[countdet];
            y_4He = ydet[countdet];
            z_4He = zdet[countdet];
        }

        if (debug1)
            cout << "det: " << detector[countdet] << " x: " << xdet[countdet] << " y: " << ydet[countdet]
                 << " z: " << zdet[countdet] << " q: " << qdet[countdet] << " id: " << ID << endl;
        countdet++;
    }
    cout << "alpha: " << alpha << "  carbon: " << carbon << endl;
    if (fPairs && (!alpha || !carbon))
    {
        cout << "No coincidence!" << endl;
        return;
    }

    delta = sqrt((x_12C - x_4He) * (x_12C - x_4He) + (y_12C - y_4He) * (y_12C - y_4He));
    cout << "Delta: " << delta << "  x_12C: " << x_12C << "  x_4He: " << x_4He << "  y_12C: " << y_12C
         << "  y_4He: " << y_4He << endl;

    if (nHits > 0)
    {
        fh_tofd_mult->Fill(nHits);
        counterTofd++;
    }

    nHits = fFi23aPoints->GetEntriesFast();
    for (Int_t i = 0; i < nHits; i++)
    {
        R3BFibPoint* hit = (R3BFibPoint*)fFi23aPoints->At(i);
        Int_t ID = hit->GetDetectorID();
        qqq = sqrt(hit->GetEnergyLoss()) * 120.53 + 0.5;
        if (qqq >= 5)
            qqq = 6;
        if (qqq >= 1 && qqq < 5)
            qqq = 2;
        if (qqq > 6)
            qqq = 8;
        // if(qqq<2) continue;
        /*
                Double_t ddd = sqrt((hit->GetXIn() + hit->GetXOut()) / 2. * (hit->GetXIn() + hit->GetXOut()) / 2. +
                    (hit->GetYIn() + hit->GetYOut()) / 2.*(hit->GetYIn() + hit->GetYOut()) / 2.);
                if(ddd < 0.3)
                    continue;
        */
        detector[countdet] = 0;
        xdet[countdet] = (hit->GetXIn() + hit->GetXOut()) / 2. / 100.;
        ydet[countdet] = (hit->GetYIn() + hit->GetYOut()) / 2. / 100.;
        zdet[countdet] = (hit->GetZIn() + hit->GetZOut()) / 2. / 100.;
        qdet[countdet] = qqq;
        IDdet[countdet] = ID;

        // xdet[countdet] = (int)(xdet[countdet] / 0.00025 ) * 0.00025;

        fh_xy_Fib[fi23a]->Fill(xdet[countdet], ydet[countdet]);

        if (debug1)
            cout << "det: " << detector[countdet] << " x: " << xdet[countdet] << " y: " << ydet[countdet]
                 << " z: " << zdet[countdet] << " q: " << qdet[countdet] << " id: " << ID << endl;
        countdet++;
        mult23a++;
    }

    nHits = fFi23bPoints->GetEntriesFast();
    for (Int_t i = 0; i < nHits; i++)
    {
        R3BFibPoint* hit = (R3BFibPoint*)fFi23bPoints->At(i);
        Int_t ID = hit->GetDetectorID();
        qqq = sqrt(hit->GetEnergyLoss()) * 120.53 + 0.5;
        if (qqq >= 5)
            qqq = 6;
        if (qqq >= 1 && qqq < 5)
            qqq = 2;
        if (qqq > 6)
            qqq = 8;
        // if(qqq<2) continue;
        detector[countdet] = 1;
        xdet[countdet] = (hit->GetXIn() + hit->GetXOut()) / 2. / 100.;
        ydet[countdet] = (hit->GetYIn() + hit->GetYOut()) / 2. / 100.;
        zdet[countdet] = (hit->GetZIn() + hit->GetZOut()) / 2. / 100.;
        qdet[countdet] = qqq;
        IDdet[countdet] = ID;
        // xdet[countdet] = (int)(xdet[countdet] / 0.00025 ) * 0.00025;

        if (debug1)
            cout << "det: " << detector[countdet] << " x: " << xdet[countdet] << " y: " << ydet[countdet]
                 << " z: " << zdet[countdet] << " q: " << qdet[countdet] << " id: " << ID << endl;

        countdet++;
        mult23b++;
    }

    nHits = fFi30Points->GetEntriesFast();
    for (Int_t i = 0; i < nHits; i++)
    {
        R3BFibPoint* hit = (R3BFibPoint*)fFi30Points->At(i);
        Int_t ID = hit->GetDetectorID();
        qqq = sqrt(hit->GetEnergyLoss()) * 55.12 + 0.5;
        if (qqq >= 5)
            qqq = 6;
        if (qqq >= 1 && qqq < 5)
            qqq = 2;
        if (qqq > 6)
            qqq = 8;
        // if(qqq<2) continue;
        detector[countdet] = 2;
        xdet[countdet] = (hit->GetXIn() + hit->GetXOut()) / 2. / 100.;
        ydet[countdet] = (hit->GetYIn() + hit->GetYOut()) / 2. / 100.;
        zdet[countdet] = (hit->GetZIn() + hit->GetZOut()) / 2. / 100.;
        qdet[countdet] = qqq;
        IDdet[countdet] = ID;
        // xdet[countdet] = (int)(xdet[countdet] / 0.0005 ) * 0.0005;

        if (debug1)
            cout << "det: " << detector[countdet] << " x: " << xdet[countdet] << " y: " << ydet[countdet]
                 << " z: " << zdet[countdet] << " q: " << qdet[countdet] << " id: " << ID << endl;
        countdet++;
        mult30++;
    }

    nHits = fFi31Points->GetEntriesFast();
    for (Int_t i = 0; i < nHits; i++)
    {
        R3BFibPoint* hit = (R3BFibPoint*)fFi31Points->At(i);
        Int_t ID = hit->GetDetectorID();
        qqq = sqrt(hit->GetEnergyLoss()) * 55.12 + 0.5;
        if (qqq >= 5)
            qqq = 6;
        if (qqq >= 1 && qqq < 5)
            qqq = 2;
        if (qqq > 6)
            qqq = 8;
        // if(qqq<2) continue;
        detector[countdet] = 3;
        xdet[countdet] = (hit->GetXIn() + hit->GetXOut()) / 2. / 100.;
        ydet[countdet] = (hit->GetYIn() + hit->GetYOut()) / 2. / 100.;
        zdet[countdet] = (hit->GetZIn() + hit->GetZOut()) / 2. / 100.;
        qdet[countdet] = qqq;
        IDdet[countdet] = ID;
        // xdet[countdet] = (int)(xdet[countdet] / 0.0005 ) * 0.0005;

        if (debug1)
            cout << "det: " << detector[countdet] << " x: " << xdet[countdet] << " y: " << ydet[countdet]
                 << " z: " << zdet[countdet] << " q: " << qdet[countdet] << " id: " << ID << endl;
        countdet++;
        mult31++;
    }

    nHits = fFi32Points->GetEntriesFast();
    for (Int_t i = 0; i < nHits; i++)
    {
        R3BFibPoint* hit = (R3BFibPoint*)fFi32Points->At(i);
        Int_t ID = hit->GetDetectorID();
        qqq = sqrt(hit->GetEnergyLoss()) * 55.12 + 0.5;
        if (qqq >= 5)
            qqq = 6;
        if (qqq >= 1 && qqq < 5)
            qqq = 2;
        if (qqq > 6)
            qqq = 8;
        // if(qqq<2) continue;
        detector[countdet] = 4;
        xdet[countdet] = (hit->GetXIn() + hit->GetXOut()) / 2. / 100.;
        ydet[countdet] = (hit->GetYIn() + hit->GetYOut()) / 2. / 100.;
        zdet[countdet] = (hit->GetZIn() + hit->GetZOut()) / 2. / 100.;
        qdet[countdet] = qqq;
        IDdet[countdet] = ID;
        // xdet[countdet] = (int)(xdet[countdet] / 0.0005 ) * 0.0005;

        if (debug1)
            cout << "det: " << detector[countdet] << " x: " << xdet[countdet] << " y: " << ydet[countdet]
                 << " z: " << zdet[countdet] << " q: " << qdet[countdet] << " id: " << ID << endl;
        countdet++;
        mult32++;
    }

    nHits = fFi33Points->GetEntriesFast();
    for (Int_t i = 0; i < nHits; i++)
    {
        R3BFibPoint* hit = (R3BFibPoint*)fFi33Points->At(i);
        Int_t ID = hit->GetDetectorID();
        qqq = sqrt(hit->GetEnergyLoss()) * 55.12 + 0.5;
        if (qqq >= 5)
            qqq = 6;
        if (qqq >= 1 && qqq < 5)
            qqq = 2;
        if (qqq > 6)
            qqq = 8;
        // if(qqq<2) continue;
        detector[countdet] = 5;
        xdet[countdet] = (hit->GetXIn() + hit->GetXOut()) / 2. / 100.;
        ydet[countdet] = (hit->GetYIn() + hit->GetYOut()) / 2. / 100.;
        zdet[countdet] = (hit->GetZIn() + hit->GetZOut()) / 2. / 100.;
        qdet[countdet] = qqq;
        IDdet[countdet] = ID;
        // xdet[countdet] = (int)(xdet[countdet] / 0.0005 ) * 0.0005;

        if (debug1)
            cout << "det: " << detector[countdet] << " x: " << xdet[countdet] << " y: " << ydet[countdet]
                 << " z: " << zdet[countdet] << " q: " << qdet[countdet] << " id: " << ID << endl;
        countdet++;
        mult33++;
    }

    /*
        // Plots of correlations of Fiber detectors and register events for tracker
        for (Int_t i = 0; i < mult31; i++)
        {
            for (Int_t j = 0; j < mult33; j++)
            {
                if (xFi31[i] > -100 && xFi33[j] > -100)
                {
                    fh_Fib33_vs_Fib31->Fill(xFi31[i] * 100., xFi33[j] * 100.);
                    fh_Fib33_vs_Fib31_dx->Fill(xFi31[i] * 100., xFi33[j] * 100. - xFi31[i] * 100.);
                    if (!maxWerte)
                    {
                        if (!fFi31[i])
                        {
                            detector[countdet] = fi31;
                            xdet[countdet] = xFi31[i];
                            ydet[countdet] = yFi31[i];
                            zdet[countdet] = 0.;
                            qdet[countdet] = qFi31[i];
                            countdet++;
                            fFi31[i] = true;
                        }
                        if (!fFi33[j])
                        {
                            detector[countdet] = fi33;
                            xdet[countdet] = xFi33[j];
                            ydet[countdet] = yFi33[j];
                            zdet[countdet] = 0.;
                            qdet[countdet] = qFi33[j];
                            countdet++;
                            fFi33[j] = true;
                        }
                    }
                }
            }
            for (Int_t j = 0; j < mult23a; j++)
            {
                if (xFi23a[j] > -100 && xFi31[i] > -100)
                {
                    if (fGraphCuts && !cut_fi31_fi23a->IsInside(xFi23a[j] * 100., xFi31[i] * 100.))
                        continue;
                    fh_Fib31_vs_Fib23a->Fill(xFi23a[j] * 100., xFi31[i] * 100.);
                    fh_Fib31_vs_Fib23a_dx->Fill(xFi23a[j] * 100., xFi31[i] * 100. - xFi23a[j] * 100.);
                    if (!maxWerte)
                    {
                        if (!fFi23a[j])
                        {
                            detector[countdet] = fi23a;
                            xdet[countdet] = xFi23a[j];
                            ydet[countdet] = yFi23a[j];
                            zdet[countdet] = 0.;
                            qdet[countdet] = qFi23a[j];
                            countdet++;
                            fFi23a[j] = true;
                        }
                    }
                }
            }
        }

        for (Int_t i = 0; i < mult32; i++)
        {
            for (Int_t j = 0; j < mult30; j++)
            {
                if (xFi32[i] > -100 && xFi30[j] > -100)
                {
                    fh_Fib30_vs_Fib32->Fill(xFi32[i] * 100., xFi30[j] * 100.);
                    fh_Fib30_vs_Fib32_dx->Fill(xFi32[i] * 100., xFi30[j] * 100. - xFi32[i] * 100.);
                    if (!maxWerte)
                    {
                        if (!fFi32[i])
                        {
                            detector[countdet] = fi32;
                            xdet[countdet] = xFi32[i];
                            ydet[countdet] = yFi32[i];
                            zdet[countdet] = 0.;
                            qdet[countdet] = qFi32[i];
                            countdet++;
                            fFi32[i] = true;
                        }
                        if (!fFi30[j])
                        {
                            detector[countdet] = fi30;
                            xdet[countdet] = xFi30[j];
                            ydet[countdet] = yFi30[j];
                            zdet[countdet] = 0.;
                            qdet[countdet] = qFi30[j];
                            countdet++;
                            fFi30[j] = true;
                        }
                    }
                }
            }
            for (Int_t j = 0; j < mult23b; j++)
            {
                if (xFi23b[j] > -100 && xFi32[i] > -100)
                {
                    if (fGraphCuts && !cut_fi32_fi23b->IsInside(xFi23b[j] * 100., xFi32[i] * 100.))
                        continue;
                    fh_Fib32_vs_Fib23b->Fill(xFi23b[j] * 100., xFi32[i] * 100.);
                    fh_Fib32_vs_Fib23b_dx->Fill(xFi23b[j] * 100., xFi32[i] * 100. - xFi23b[j] * 100.);
                    if (!maxWerte)
                    {
                        if (!fFi23b[j])
                        {
                            detector[countdet] = fi23b;
                            xdet[countdet] = xFi23b[j];
                            ydet[countdet] = yFi23b[j];
                            zdet[countdet] = 0.;
                            qdet[countdet] = qFi23b[j];
                            countdet++;
                            fFi23b[j] = true;
                        }
                    }
                }
            }
        }
    */
    if (countdet > 50)
    {
        cout << "Too many hits!!!" << endl;
        return;
    }

    // here call tracker
    chi2 = 1.E100;

    if (debug)
    {
        cout << "# of points: " << countdet << endl;
        for (Int_t i = 0; i < countdet; i++)
        {
            cout << "#" << i << " Det: " << detector[i] << " x: " << xdet[i] << " y: " << ydet[i] << " z: " << zdet[i]
                 << " q: " << qdet[i] << " ID: " << IDdet[i] << endl;
            //			if (detector[i] < 6) qdet[i] = 0;
        }

        for (Int_t i = 0; i < ndet; i++)
        {
            LOG(DEBUG2) << "Max Det: " << i << " max x: " << xMax[i] << " max y: " << yMax[i] << " max q: " << qMax[i]
                        << endl;
        }
    }
	Bool_t hit_he[n_det];
	Bool_t hit_c[n_det];
	for (Int_t i = 0; i < n_det; i++)
	{
		hit_he[i] = 0.;
		hit_c[i] = 0.;
	}
    // fiber detectors can't measure charge!
    for (Int_t i = 0; i < countdet; i++)
    {
		if(qdet[i] == 2)
		{
			hit_he[detector[i]] = true;
			//cout << "Test he: " << detector[i] << "  " << hit_he[detector[i]] << endl;
		}
		if(qdet[i] == 6)
		{
			hit_c[detector[i]] = true;
			//cout << "Test c: " << detector[i] << "  " << hit_c[detector[i]] << endl;
		}
        if (detector[i] < 6)
        {
            //qdet[i] = 0;
        }
		if (detector[i] == 0)
		{
			ydet[i] = 0;
		}
		if (detector[i] == 1 )
		{
			xdet[i] = 0;
		}
		if (detector[i] > 1 && detector[i] < 6)
		{
			ydet[i] = 0;
		}
		if (detector[i] > 5 )
		{
			//ydet[i] = 0;
		}
	
		// introduce granularity		
		Bool_t granularity = true;
		
		if(granularity)
		{
			//cout << "Before: " << i << "  " << xdet[i] << "  " << ydet[i] << endl;
			Int_t xtemp = 0;
			Int_t ytemp = 0;
			if (detector[i] < 2)
			{
				xtemp = (int)(xdet[i] * 4000.);
				ytemp = (int)(ydet[i] * 4000.);
				xdet[i] = xtemp / 4000.;
				ydet[i] = ytemp / 4000.;
			}
			if (detector[i] > 1 && detector[i] < 6)
			{
				xtemp = (int)(xdet[i] * 1000.);
				xdet[i] = xtemp / 1000.;
			}
			//cout << "After: " << i << "  " << xdet[i] << "  " << ydet[i] << endl;
		}

        // Fill temp array
        xdet_s[i] = xdet[i];
        ydet_s[i] = ydet[i];
        zdet_s[i] = zdet[i];
        qdet_s[i] = qdet[i];
        detector_s[i] = detector[i];
    }
    countdet_s = countdet;

    if (C_right && mult23a == 0)
        miss23a++;
    if (C_right && mult31 == 0)
        miss31++;
    if (C_right && mult33 == 0)
        miss33++;

    if (C_left > 0 && mult23b == 0)
        miss23b++;
    if (C_left > 0 && mult30 == 0)
        miss30++;
    if (C_left > 0 && mult32 == 0)
        miss32++;

    // here call tracker
    chi2 = 1.E100;
    counter2++;

    Bool_t twice = false;
    Double_t chiMax = 1.E10;
    if (tracker && fPairs && twice)
    {
        // two times single track single track
        counter2++;
        Bool_t first = false;
        Bool_t second = false;
        Bool_t det_coord = false;
        Bool_t dt = false;
		target[0] = 0.;
		target[1] = 0.;
		target[2] = -0.162;

        // first track carbon
        if (debug)
        {
            cout << "# of points first particle: " << countdet << endl;
            for (Int_t i = 0; i < countdet; i++)
            {
                cout << "#" << i << " Det: " << detector[i] << " x: " << xdet[i] << " y: " << ydet[i]
                     << " z: " << zdet[i] << " q: " << qdet[i] << endl;
            }
        }
        qdet[max] = 6;
        qdet[max + 1] = 6;

        multi_track_extended_output_from_cpp_(&max,
                                              &countdet,
                                              &det_coord,
                                              &dt,
                                              target,
                                              detector,
                                              qdet,
                                              xdet,
                                              ydet,
                                              zdet,
                                              track,
                                              chi,
                                              pat1,
                                              pat2,
                                              res1_det_x,
                                              res1_det_y,
                                              res1_det_z,
                                              res2_det_x,
                                              res2_det_y,
                                              res2_det_z,
                                              res1_lab_x,
                                              res1_lab_y,
                                              res1_lab_z,
                                              res2_lab_x,
                                              res2_lab_y,
                                              res2_lab_z);

        chi2 = chi[0] + chi[1];
        fh_chiy_vs_chix->Fill(chi[0], chi[1]);
        fh_chi2->Fill(chi2);

		cout << "Test after tracking: " << chi[0] << endl;
		
        if (chi[0] < chiMax)
            counter3++;
        if (chi[1] < chiMax)
            counter4++;
        if (chi[0] < chiMax && chi[1] < chiMax)
        {
            // fill histograms
            Output2(track, chi);
        }
        if (debug && chi[0] < chiMax)
        {
            cout << "******************************************" << endl;
            cout << "single track #1: " << track[0] << "  " << track[1] << "  " << track[2] << "  " << track[3] << "  "
                 << track[4] << "  " << track[5] << endl;

            cout << "chi: " << chi[0] << "  " << chi[1] << "  " << chi[2] << "  " << chi[3] << "  " << chi[4] << "  "
                 << chi[5] << endl;

            cout << "Track In 12C"
                 << "px " << pCxs << " py " << pCys << " z " << pCzs << endl;
        }

        track_s[6] = track[0];
        track_s[7] = track[1];
        track_s[8] = track[2];
        track_s[9] = track[3];
        track_s[10] = track[4];
        track_s[11] = track[5];
        chi_s[2] = chi[0];
        chi_s[3] = chi[1];

        if (chi[0] < chiMax && chi[1] < chiMax)
        {
            counterTracker++;
            // we have a hit
            first = true;
            for (Int_t i = 0; i < ndet; i++)
            {
                xTrack[i] = -1000.;
                yTrack[i] = -1000.;
                zTrack[i] = -1000.;
                qTrack[i] = -1000.;
            }
            Int_t charge = 0;
            cout << "# of points back" << countdet << endl;
            for (Int_t i = 0; i < countdet; i++)
            {

                cout << "back #" << i << " Det: " << detector[i] << " x: " << xdet[i] << " y: " << ydet[i]
                     << " q: " << qdet[i] << endl;
                xTrack[detector[i]] = xdet[i];
                yTrack[detector[i]] = ydet[i];
                zTrack[detector[i]] = zdet[i];
                qTrack[detector[i]] = qdet[i];
                if (qdet[i] > charge)
                    charge = qdet[i];
            }
            // plot hits of the track
            for (Int_t i = 0; i < countdet; i++)
            {
                fh_xy[i]->Fill(xTrack[i] * 100., yTrack[i] * 100.);
                fh_p_vs_x[i]->Fill(xTrack[i] * 100., track[5]);
                fh_p_vs_x_test[i]->Fill(xTrack[i] * 100.,
                                        sqrt(track[3] * track[3] + track[4] * track[4] + track[5] * track[5]));
				if(i == 0 || i > 1)
				{
					fh_res_xC[i]->Fill(res2_det_x[i] * 100.);
				}
				if(i == 1)
				{
					fh_res_yC[i]->Fill(res2_det_y[i] * 100.);
				}
            }
            // Plots of correlations of Fiber detectors

            fh_Fib33_vs_Fib31_back->Fill(xTrack[3] * 100., xTrack[5] * 100.);
            fh_Fib33_vs_Fib31_dx_back->Fill(xTrack[3] * 100., xTrack[5] * 100. - xTrack[3] * 100.);
            fh_Fib31_vs_Fib23a_back->Fill(xTrack[0] * 100., xTrack[3] * 100.);
            fh_Fib31_vs_Fib23a_dx_back->Fill(xTrack[0] * 100., xTrack[3] * 100. - xTrack[0] * 100.);
            fh_Fib30_vs_Fib32_back->Fill(xTrack[4] * 100., xTrack[2] * 100.);
            fh_Fib30_vs_Fib32_dx_back->Fill(xTrack[4] * 100., xTrack[2] * 100. - xTrack[4] * 100.);
            fh_Fib32_vs_Fib23b_back->Fill(xTrack[1] * 100., xTrack[4] * 100.);
            fh_Fib32_vs_Fib23b_dx_back->Fill(xTrack[1] * 100., xTrack[4] * 100. - xTrack[1] * 100.);

            // store hits in track level
            new ((*fTrackItems)[fNofTrackItems++])
                R3BTrack(track[0], track[1], track[2], track[3], track[4], track[5], charge, 2, chi[0], chi[1], 0);

			target[0] = track[0];
			target[1] = track[1];
			target[2] = track[2];
        }

        // now track second particle (alpha)
        // remove hits which were used to track first particle
        Bool_t used = false;
        countdet_ss = 0;
        for (Int_t i = 0; i < countdet_s; i++)
        {
            used = false;
            for (Int_t j = 0; j < countdet; j++)
            {
                //                    if(detector_s[i] == detector[j] && abs(xdet_s[i] - xdet[j]) < 0.000001 &&
                //                    abs(ydet_s[i] - ydet[j]) < 0.000001)
                if (detector_s[i] == detector[j] && abs(xdet_s[i] - xdet[j]) < 0.000001)
                {
                    used = true;
                }
            }
            if (!used)
            {
                xdet_ss[countdet_ss] = xdet_s[i];
                ydet_ss[countdet_ss] = ydet_s[i];
                zdet_ss[countdet_ss] = zdet_s[i];
                qdet_ss[countdet_ss] = qdet_s[i];
                detector_ss[countdet_ss] = detector_s[i];
                countdet_ss++;
            }
        }
        // now track second particle (alpha)
        countdet = countdet_ss;
        for (Int_t i = 0; i < countdet; i++)
        {
            // Fill temp array
            xdet[i] = xdet_ss[i];
            ydet[i] = ydet_ss[i];
            zdet[i] = zdet_ss[i];
            qdet[i] = qdet_ss[i];
            detector[i] = detector_ss[i];
        }
        for (Int_t i = 0; i < 12; i++)
        {
            track[i] = 0.;
        }
        if (debug)
        {
            cout << "# of points second particle" << countdet << endl;
            for (Int_t i = 0; i < countdet; i++)
            {
                cout << "#" << i << " Det: " << detector[i] << " x: " << xdet[i] << " y: " << ydet[i]
                     << " z: " << zdet[i] << " q: " << qdet[i] << endl;
            }
        }

        qdet[max] = 2;
        qdet[max + 1] = 2;

        multi_track_extended_output_from_cpp_(&max,
                                              &countdet,
                                              &det_coord,
                                              &dt,
                                              target,
                                              detector,
                                              qdet,
                                              xdet,
                                              ydet,
                                              zdet,
                                              track,
                                              chi,
                                              pat1,
                                              pat2,
                                              res1_det_x,
                                              res1_det_y,
                                              res1_det_z,
                                              res2_det_x,
                                              res2_det_y,
                                              res2_det_z,
                                              res1_lab_x,
                                              res1_lab_y,
                                              res1_lab_z,
                                              res2_lab_x,
                                              res2_lab_y,
                                              res2_lab_z);

        chi2 = chi[0] + chi[1];
        fh_chiy_vs_chix->Fill(chi[0], chi[1]);
        fh_chi2->Fill(chi2);

        if (chi[0] < chiMax)
            counter3++;
        if (chi[1] < chiMax)
            counter4++;
        if (chi[0] < chiMax && chi[1] < chiMax)
        {
            // fill histograms
            second = true;
            Output2(track, chi);
        }

        track_s[0] = track[0];
        track_s[1] = track[1];
        track_s[2] = track[2];
        track_s[3] = track[3];
        track_s[4] = track[4];
        track_s[5] = track[5];
        chi_s[0] = chi[0];
        chi_s[1] = chi[1];

        if (debug && chi[0] < chiMax)
        {
            cout << "******************************************" << endl;
            cout << "single track #2: " << track[0] << "  " << track[1] << "  " << track[2] << "  " << track[3] << "  "
                 << track[4] << "  " << track[5] << endl;

            cout << "chi: " << chi[0] << "  " << chi[1] << "  " << chi[2] << "  " << chi[3] << "  " << chi[4] << "  "
                 << chi[5] << endl;

            cout << "Track In 4He"
                 << "px " << pHexs << " py " << pHeys << " z " << pHezs << endl;
        }

        if (chi_s[0] + chi_s[0] + chi_s[0] + chi_s[0] < chiMax * 4. && first && second)
        {
            //			if(sqrt((track_s[0] - track_s[6]) * (track_s[0] - track_s[6]) +
            //				(track_s[1] * track_s[7]) * (track_s[1] * track_s[7])) < 0.0005)
            {
                counterTracker++;
                Output1(track_s, chi_s);
            }
        }

        if (chi[0] < chiMax && chi[1] < chiMax)
        {
            counterTracker++;
            // we have a hit
            for (Int_t i = 0; i < ndet; i++)
            {
                xTrack[i] = -1000.;
                yTrack[i] = -1000.;
                zTrack[i] = -1000.;
                qTrack[i] = -1000.;
            }
            Int_t charge = 0;
            LOG(DEBUG2) << "# of points back" << countdet << endl;
            for (Int_t i = 0; i < countdet; i++)
            {

                // cout << "back #" << i << " Det: " << detector[i] << " x: " << xdet[i]
                //     << " y: " << ydet[i] << " q: " << qdet[i] << endl;
                xTrack[detector[i]] = xdet[i];
                yTrack[detector[i]] = ydet[i];
                zTrack[detector[i]] = zdet[i];
                qTrack[detector[i]] = qdet[i];
                if (qdet[i] > charge)
                    charge = qdet[i];
            }
            // plot hits of the track
            for (Int_t i = 0; i < countdet; i++)
            {
                fh_xy[i]->Fill(xTrack[i] * 100., yTrack[i] * 100.);
                fh_p_vs_x[i]->Fill(xTrack[i] * 100., track[5]);
                fh_p_vs_x_test[i]->Fill(xTrack[i] * 100.,
                                        sqrt(track[3] * track[3] + track[4] * track[4] + track[5] * track[5]));
				if(i == 0 || i > 1)
				{
					fh_res_xA[i]->Fill(res1_det_x[i] * 100.);
				}
				if(i == 1)
				{
					fh_res_yA[i]->Fill(res1_det_y[i] * 100.);
				}
            }
            // Plots of correlations of Fiber detectors

            fh_Fib33_vs_Fib31_back->Fill(xTrack[3] * 100., xTrack[5] * 100.);
            fh_Fib33_vs_Fib31_dx_back->Fill(xTrack[3] * 100., xTrack[5] * 100. - xTrack[3] * 100.);
            fh_Fib31_vs_Fib23a_back->Fill(xTrack[0] * 100., xTrack[3] * 100.);
            fh_Fib31_vs_Fib23a_dx_back->Fill(xTrack[0] * 100., xTrack[3] * 100. - xTrack[0] * 100.);
            fh_Fib30_vs_Fib32_back->Fill(xTrack[4] * 100., xTrack[2] * 100.);
            fh_Fib30_vs_Fib32_dx_back->Fill(xTrack[4] * 100., xTrack[2] * 100. - xTrack[4] * 100.);
            fh_Fib32_vs_Fib23b_back->Fill(xTrack[1] * 100., xTrack[4] * 100.);
            fh_Fib32_vs_Fib23b_dx_back->Fill(xTrack[1] * 100., xTrack[4] * 100. - xTrack[1] * 100.);

            // store hits in track level
            new ((*fTrackItems)[fNofTrackItems++])
                R3BTrack(track[0], track[1], track[2], track[3], track[4], track[5], charge, 2, chi[0], chi[1], 0);
        }
        countdet = countdet_s;
        for (Int_t i = 0; i < countdet; i++)
        {
            // Fill temp array
            xdet[i] = xdet_s[i];
            ydet[i] = ydet_s[i];
            zdet[i] = zdet_s[i];
            qdet[i] = qdet_s[i];
            detector[i] = detector_s[i];
        }
        for (Int_t i = 0; i < 12; i++)
        {
            track[i] = 0.;
        }
    }

    if (tracker && fPairs && !twice && countdet > 3 &&
		(hit_c[0] && hit_c[1] && ((hit_c[2] && hit_c[4]) || (hit_c[3] && hit_c[5]))) &&
		(hit_he[0] && hit_he[1] && ((hit_he[2] && hit_he[4]) || (hit_he[3] && hit_he[5]))) )
    {
        // double track
        counter2++;
        Bool_t det_coord = false; // 
        Bool_t dt = true;
		target[0] = 0.;
		target[1] = 0.;
		target[2] = -0.162;

        
        multi_track_extended_output_from_cpp_(&max,
                                              &countdet,
                                              &det_coord,
                                              &dt,
                                              target,
                                              detector,
                                              qdet,
                                              xdet,
                                              ydet,
                                              zdet,
                                              track,
                                              chi,
                                              pat1,
                                              pat2,
                                              res1_det_x,
                                              res1_det_y,
                                              res1_det_z,
                                              res2_det_x,
                                              res2_det_y,
                                              res2_det_z,
                                              res1_lab_x,
                                              res1_lab_y,
                                              res1_lab_z,
                                              res2_lab_x,
                                              res2_lab_y,
                                              res2_lab_z);

        chi2 = chi[4] + chi[5];
        fh_chiy_vs_chix->Fill(chi[0], chi[1]);
        fh_chiy_vs_chix->Fill(chi[2], chi[3]);
        fh_chi2->Fill(chi2);
		Double_t chi_cut = 10.;
        if (debug && chi2 < chi_cut)
        {
            cout << "******************************************" << endl;
            cout << "double track1: " << track[0] << "  " << track[1] << "  " << track[2] << "  " << track[3] << "  "
                 << track[4] << "  " << track[5] << endl;
            cout << "double track2: " << track[6] << "  " << track[7] << "  " << track[8] << "  " << track[9] << "  "
                 << track[10] << "  " << track[11] << endl;

            cout << "chi: " << chi[0] << "  " << chi[1] << "  " << chi[2] << "  " << chi[3] << "  " << chi[4] << "  "
                 << chi[5] << endl;

            cout << "Track In 4He"
                 << "px " << pHexs << " py " << pHeys << " z " << pHezs << endl;
            cout << "Track In 12C"
                 << "px " << pCxs << " py " << pCys << " z " << pCzs << endl;
        }

        if (chi[0] < chi_cut)
            counter3++;
        if (chi[1] < chi_cut)
            counter4++;

        if (chi[0] < chi_cut && chi[1] < chi_cut)
        {
            counterTracker++;
            Output1(track, chi);
        }
        // we have a hit
        for (Int_t i = 0; i < ndet; i++)
        {
            xTrack[i] = -1000.;
            yTrack[i] = -1000.;
            zTrack[i] = -1000.;
            qTrack[i] = -1000.;
        }
        Int_t charge = 0;
        if (debug)
            cout << "# of points back" << countdet << endl;
        for (Int_t i = 0; i < countdet; i++)
        {

            if (debug)
            {
                cout << "back #" << i << " Det: " << detector[i] << " x: " << xdet[i] << " y: " << ydet[i]
                     << " q: " << qdet[i] << endl;
            }
            if (qdet[i] == 2)
            {
                xTrack[detector[i]] = xdet[i];
                yTrack[detector[i]] = ydet[i];
                zTrack[detector[i]] = zdet[i];
                qTrack[detector[i]] = qdet[i];
            }
            // plot hits of the track
			if (qdet[i] == 2)
			{
				fh_res_xA[i]->Fill(res1_det_x[i] * 100.);
				fh_res_yA[i]->Fill(res1_det_y[i] * 100.);
			}
			if (qdet[i] == 6)
			{
				fh_res_xC[i]->Fill(res1_det_x[i] * 100.);
				fh_res_yC[i]->Fill(res1_det_y[i] * 100.);
			}
			fh_xy[detector[i]]->Fill(xdet[i] * 100., ydet[i] * 100.);
			fh_p_vs_x[detector[i]]->Fill(xdet[i] * 100. + randx, track[5]);
			fh_p_vs_x_test[detector[i]]->Fill(
				xdet[i] * 100., sqrt(track[3] * track[3] + track[4] * track[4] + track[5] * track[5]));
			fh_p_vs_x_test[detector[i]]->Fill(
				xdet[i] * 100., sqrt(track[9] * track[9] + track[10] * track[10] + track[11] * track[11]));
            if (sqrt(track[3] * track[3] + track[4] * track[4] + track[5] * track[5]) > 4450)
            {
                cout << " ***************************** high momentum ***********************************" << endl;
            }
        }
        // Plots of correlations of Fiber detectors

        fh_Fib33_vs_Fib31_back->Fill(xTrack[3] * 100., xTrack[5] * 100.);
        fh_Fib33_vs_Fib31_dx_back->Fill(xTrack[3] * 100., xTrack[5] * 100. - xTrack[3] * 100.);
        fh_Fib31_vs_Fib23a_back->Fill(xTrack[0] * 100., xTrack[3] * 100.);
        fh_Fib31_vs_Fib23a_dx_back->Fill(xTrack[0] * 100., xTrack[3] * 100. - xTrack[0] * 100.);
        fh_Fib30_vs_Fib32_back->Fill(xTrack[4] * 100., xTrack[2] * 100.);
        fh_Fib30_vs_Fib32_dx_back->Fill(xTrack[4] * 100., xTrack[2] * 100. - xTrack[4] * 100.);
        fh_Fib32_vs_Fib23b_back->Fill(xTrack[1] * 100., xTrack[4] * 100.);
        fh_Fib32_vs_Fib23b_dx_back->Fill(xTrack[1] * 100., xTrack[4] * 100. - xTrack[1] * 100.);

        if (chi[0] < chi_cut && chi[1] < chi_cut)
        {
            // sorted, lowest Charge first
            new ((*fTrackItems)[fNofTrackItems++])
                R3BTrack(track[0], track[1], track[2], track[3], track[4], track[5], 2, 2, chi[0], chi[1], 0);
            new ((*fTrackItems)[fNofTrackItems++])
                R3BTrack(track[6], track[7], track[8], track[9], track[10], track[11], 6, 2, chi[2], chi[3], 0);
        }
    }
    if (tracker && !fPairs && !twice)
    {
        // single track
        counter2++;
        Bool_t det_coord = false;
        Bool_t dt = false;
        // multi_track_from_cpp_(
        //    &max, &countdet, &det_coord, &dt, target, detector, qdet, xdet, ydet, zdet, track, chi);

        multi_track_extended_output_from_cpp_(&max,
                                              &countdet,
                                              &det_coord,
                                              &dt,
                                              target,
                                              detector,
                                              qdet,
                                              xdet,
                                              ydet,
                                              zdet,
                                              track,
                                              chi,
                                              pat1,
                                              pat2,
                                              res1_det_x,
                                              res1_det_y,
                                              res1_det_z,
                                              res2_det_x,
                                              res2_det_y,
                                              res2_det_z,
                                              res1_lab_x,
                                              res1_lab_y,
                                              res1_lab_z,
                                              res2_lab_x,
                                              res2_lab_y,
                                              res2_lab_z);

        chi2 = chi[0] + chi[1];
        fh_chiy_vs_chix->Fill(chi[0], chi[1]);
        fh_chi2->Fill(chi2);

        if (chi[0] < 1.e10)
            counter3++;
        if (chi[1] < 1.e10)
            counter4++;
        if (chi[0] < 1.e10 && chi[1] < 1.e10)
        {
            // fill histograms
            Output2(track, chi);
        }
        if (debug)
        {
            cout << "track1: " << track[0] << "  " << track[1] << "  " << track[2] << "  " << track[3] << "  "
                 << track[4] << "  " << track[5] << endl;

            cout << "chi: " << chi[0] << "  " << chi[1] << "  " << chi[2] << "  " << chi[3] << "  " << chi[4] << "  "
                 << chi[5] << endl;

            cout << "******************************************" << endl;
            LOG(DEBUG) << "Track In 16O"
                       << "x " << Xf << " y " << Yf << " z " << Zf << endl;
            LOG(DEBUG) << "px " << Pxf << " py " << Pyf << " z " << Pzf << endl;
        }

        if (chi[0] < 1.e10 && chi[1] < 1.e10)
        {
            counterTracker++;
            // cout << "track1: " << track[0] << "  " << track[1] << "  " << track[2] << endl;
            // cout << "track1: " << track[3] << "  " << track[4] << "  " << track[5] << endl;
            // cout << "chi: " << chi[0] << "  " << chi[1] << "  " << chi[2] << "  " << chi[3] << "  "
            //     << chi[4] << "  " << chi[5] << endl;

            // we have a hit
            for (Int_t i = 0; i < ndet; i++)
            {
                xTrack[i] = -1000.;
                yTrack[i] = -1000.;
                zTrack[i] = -1000.;
                qTrack[i] = -1000.;
            }
            Int_t charge = 0;
            LOG(DEBUG2) << "# of points back" << countdet << endl;
            for (Int_t i = 0; i < countdet; i++)
            {

                // cout << "back #" << i << " Det: " << detector[i] << " x: " << xdet[i]
                //     << " y: " << ydet[i] << " q: " << qdet[i] << endl;
                xTrack[detector[i]] = xdet[i];
                yTrack[detector[i]] = ydet[i];
                zTrack[detector[i]] = zdet[i];
                qTrack[detector[i]] = qdet[i];
                if (qdet[i] > charge)
                    charge = qdet[i];
            }
            // plot hits of the track
            for (Int_t i = 0; i < ndet; i++)
            {
				if (qdet[i] == 2)
				{
					fh_res_xA[i]->Fill(res1_det_x[i] * 100.);
					fh_res_yA[i]->Fill(res1_det_y[i] * 100.);
                }
				if (qdet[i] == 6)
				{
					fh_res_xC[i]->Fill(res1_det_x[i] * 100.);
					fh_res_yC[i]->Fill(res1_det_y[i] * 100.);
                }
                fh_xy[i]->Fill(xTrack[i] * 100., yTrack[i] * 100.);
                fh_p_vs_x[i]->Fill(xTrack[i] * 100., track[5]);
                fh_p_vs_x_test[i]->Fill(xTrack[i] * 100.,
                                        sqrt(track[3] * track[3] + track[4] * track[4] + track[5] * track[5]));
            }
            // Plots of correlations of Fiber detectors

            fh_Fib33_vs_Fib31_back->Fill(xTrack[3] * 100., xTrack[5] * 100.);
            fh_Fib33_vs_Fib31_dx_back->Fill(xTrack[3] * 100., xTrack[5] * 100. - xTrack[3] * 100.);
            fh_Fib31_vs_Fib23a_back->Fill(xTrack[0] * 100., xTrack[3] * 100.);
            fh_Fib31_vs_Fib23a_dx_back->Fill(xTrack[0] * 100., xTrack[3] * 100. - xTrack[0] * 100.);
            fh_Fib30_vs_Fib32_back->Fill(xTrack[4] * 100., xTrack[2] * 100.);
            fh_Fib30_vs_Fib32_dx_back->Fill(xTrack[4] * 100., xTrack[2] * 100. - xTrack[4] * 100.);
            fh_Fib32_vs_Fib23b_back->Fill(xTrack[1] * 100., xTrack[4] * 100.);
            fh_Fib32_vs_Fib23b_dx_back->Fill(xTrack[1] * 100., xTrack[4] * 100. - xTrack[1] * 100.);

            // store hits in track level
            new ((*fTrackItems)[fNofTrackItems++])
                R3BTrack(track[0], track[1], track[2], track[3], track[4], track[5], charge, 2, chi[0], chi[1], 0);
        }
    }
    chi2_best = 1.E10;
    pHex = 0.;
    pHey = 0.;
    pHez = 0.;
    pCx = 0.;
    pCy = 0.;
    pCz = 0.;

    if (fMCTrack)
    {
        Double_t costh26s =
            (pHexs * pCxs + pHeys * pCys + pHezs * pCzs) /
            (sqrt(pHexs * pHexs + pHeys * pHeys + pHezs * pHezs) * sqrt(pCxs * pCxs + pCys * pCys + pCzs * pCzs));

        Double_t theta_26s = acos(costh26s) * 180. / 3.14159; // opening angle

        fh_theta26_simu->Fill(theta_26s);

        Double_t eHes = sqrt(pow(pHes, 2) + pow(mHe, 2)) - mHe;
        Double_t eCs = sqrt(pow(pCs, 2) + pow(mC, 2)) - mC;

        Double_t Erelas = sqrt(pow((mHe + mC + eHes + eCs), 2) - pow(pHes, 2) - pow(pCs, 2) -
                               2 * pHes * pCs * cos(theta_26s * 3.1415 / 180.)) -
                          (mHe + mC); // Erel

        Double_t m_invs = sqrt(mHe * mHe + mC * mC + 2. * sqrt(pCs * pCs + mC * mC) * sqrt(pHes * pHes + mHe * mHe) -
                               2. * pHes * pCs * cos(theta_26s * 3.1415 / 180.));
        Double_t Erelbs = m_invs - mHe - mC;

        fh_Erel_simu->Fill(Erelbs);
        LOG(DEBUG) << "Theta 26 simu: " << theta_26s << " Erel simu: " << Erelas << " " << Erelbs << endl;
    }

    for (Int_t i = 0; i < 12; i++)
    {
        track[i] = 0.;
    }

    //    if (multTofd > 0)
    //        fh_tofd_mult_ac->Fill(multTofd);
}
void R3BTrackerTestS494::Output1(Double_t track[12], Double_t chi[6])
{

    pHex = track[3];
    pHey = track[4];
    pHez = track[5];

    pCx = track[9];
    pCy = track[10];
    pCz = track[11];

    LOG(DEBUG) << "He: " << pHex << "  " << pHey << "  " << pHez << endl;
    LOG(DEBUG) << "C: " << pCx << "  " << pCy << "  " << pCz << endl;

    fh_target_xy->Fill(track[0] * 100., track[1] * 100.);
    fh_target_dxdy->Fill((track[0] - track[6]) * 100., (track[1] - track[7]) * 100.);
    fh_px_He->Fill(pHex);
    fh_py_He->Fill(pHey);
    fh_pz_He->Fill(pHez);

    fh_px_C->Fill(pCx);
    fh_py_C->Fill(pCy);
    fh_pz_C->Fill(pCz);

    fh_dpx_He->Fill((pHex - pHexs) / pHexs * 100.);
    fh_dpy_He->Fill((pHey - pHeys) / pHeys * 100.);
    fh_dpz_He->Fill((pHez - pHezs) / pHezs * 100.);

    fh_dpx_C->Fill((pCx - pCxs) / pCxs * 100.);
    fh_dpy_C->Fill((pCy - pCys) / pCys * 100.);
    fh_dpz_C->Fill((pCz - pCzs) / pCzs * 100.);

	fh_totalPz_reco->Fill(pCz + pHez);

    // Calculate angle between alphs and C
    if (pCz == 0 || pHez == 0)
        return;

    Double_t costh26 = (pHex * pCx + pHey * pCy + pHez * pCz) /
                       (sqrt(pHex * pHex + pHey * pHey + pHez * pHez) * sqrt(pCx * pCx + pCy * pCy + pCz * pCz));

    Double_t theta_26 = acos(costh26) * 180. / 3.14159; // opening angle

    fh_theta26->Fill(theta_26);

    Double_t pHe = sqrt(pow(pHex, 2) + pow(pHey, 2) + pow(pHez, 2));
    Double_t pC = sqrt(pow(pCx, 2) + pow(pCy, 2) + pow(pCz, 2));
    Double_t eHe = sqrt(pow(pHe, 2) + pow(mHe, 2)) - mHe;
    Double_t eC = sqrt(pow(pC, 2) + pow(mC, 2)) - mC;

    fh_p_He->Fill(pHe);
    fh_p_C->Fill(pC);
    fh_dp_He->Fill((pHe - pHes) / pHes * 100.);
    fh_dp_C->Fill((pC - pCs) / pCs * 100.);

    Double_t Erela =
        sqrt(pow((mHe + mC + eHe + eC), 2) - pow(pHe, 2) - pow(pC, 2) - 2 * pHe * pC * cos(theta_26 * 3.1415 / 180.)) -
        (mHe + mC); // Erel

    Double_t m_inv = sqrt(mHe * mHe + mC * mC + 2. * sqrt(pC * pC + mC * mC) * sqrt(pHe * pHe + mHe * mHe) -
                          2. * pHe * pC * cos(theta_26 * 3.1415 / 180.));
    Double_t Erelb = m_inv - mHe - mC;

    fh_Erel->Fill(Erelb);
    fh_dErel_vs_x->Fill(delta * 100., Erela);
    fh_chi2_vs_Erel->Fill(Erelb, chi[0] + chi[1] + chi[2] + chi[3]);

    //	fh_dErel_vs_y->Fill(yTest * 100., Erela - 1.);
    if (1 == 1)
    {
        cout << " ****************************************************************" << endl;
        cout << "Theta 26: " << theta_26 << " Erel: " << Erela << " " << Erelb << endl;
    }
}

void R3BTrackerTestS494::Output2(Double_t track_parameter[12], Double_t chi_single_parameter[6])
{
    // compare

    LOG(DEBUG) << "******************************************" << endl;
    LOG(DEBUG) << "chi_sqingle_parameter " << chi_single_parameter[0] << "  " << chi_single_parameter[1] << endl;
    LOG(DEBUG) << "xyz: " << track_parameter[0] * 100. << "  " << track_parameter[1] * 100. << "  "
               << track_parameter[2] * 100. << endl;
    LOG(DEBUG) << "p: " << track_parameter[3] << "  " << track_parameter[4] << "  " << track_parameter[5] << endl;
    Double_t p_tot = sqrt(track_parameter[3] * track_parameter[3] + track_parameter[4] * track_parameter[4] +
                          track_parameter[5] * track_parameter[5]);

    fh_target_xy->Fill(track_parameter[0] * 100., track_parameter[1] * 100.);
    fh_target_px->Fill(track_parameter[3]);
    fh_target_py->Fill(track_parameter[4]);
    fh_target_pz->Fill(track_parameter[5]);
    fh_target_p->Fill(p_tot);
    fh_chi2->Fill(chi_single_parameter[0]);

    // Comparison with simulation
    Double_t thetax_simu = Pxf / Pzf * 1000.;
    Double_t thetay_simu = Pyf / Pzf * 1000.;

    fh_thetax_dpx->Fill((Pxf - track_parameter[3]) / Pxf * 100., thetax_simu);
    fh_thetax_dpx_abs->Fill((Pxf - track_parameter[3]), thetax_simu);
    fh_thetay_dpy->Fill((Pyf - track_parameter[4]) / Pyf * 100., thetay_simu);

    fh_thetax_dpy->Fill((Pyf - track_parameter[4]) / Pyf * 100., thetax_simu);
    fh_thetay_dpx->Fill((Pxf - track_parameter[3]) / Pxf * 100., thetay_simu);
    fh_dpy_dpx->Fill((Pxf - track_parameter[3]) / Pxf * 100., (Pyf - track_parameter[4]) / Pyf * 100.);

    fh_x_dpx->Fill((Pxf - track_parameter[3]) / Pxf * 100., Xf);
    fh_y_dpy->Fill((Pyf - track_parameter[4]) / Pyf * 100., Yf);
    fh_dx->Fill(Xf - track_parameter[0] * 100.);
    fh_dy->Fill(Yf - track_parameter[1] * 100.);
    fh_dz->Fill(Zf - track_parameter[2] * 100.);
    fh_dpx->Fill((Pxf - track_parameter[3]) / Pxf * 100.);
    fh_dpy->Fill((Pyf - track_parameter[4]) / Pyf * 100.);
    fh_dpz->Fill((Pzf - track_parameter[5]) / Pzf * 100.);
    fh_dp->Fill((Pf_tot - p_tot) / Pf_tot * 100.);
}
void R3BTrackerTestS494::FinishEvent()
{

    fTrackItems->Clear();
    fNofTrackItems = 0;
}

void R3BTrackerTestS494::FinishTask()
{

    cout << "Statistics:" << endl;
    cout << "Events: " << fNEvents << endl;
    cout << "Wrong Trigger: " << counterWrongTrigger << endl;
    cout << "Wrong Tpat: " << counterWrongTpat << endl;
    cout << "ROLU veto: " << counterRolu << endl;
    cout << "Califa veto: " << counterCalifa << endl;
    cout << "TofD: " << counterTofd << endl;
    cout << "TofD multi: " << counterTofdMulti << endl;
    cout << "Tracker: " << counterTracker << endl;
    cout << "missing 23a: " << miss23a << " missing 33b: " << miss23b << endl;
    cout << "missing 31: " << miss31 << " missing 32: " << miss32 << endl;
    cout << "missing 33: " << miss33 << " missing 30: " << miss30 << endl;

    fh_Tpat->Write();
    fh_Trigger->Write();
    fh_Cave_position->Write();
    fh_TOFDOR->Write();
    fh_SEE->Write();
    fh_IC->Write();

    fh_target_xy->Write();
    fh_target_dxdy->Write();
    fh_target_px->Write();
    fh_target_py->Write();
    fh_target_pz->Write();
    fh_target_p->Write();

    fh_px_He->Write();
    fh_py_He->Write();
    fh_pz_He->Write();
    fh_p_He->Write();
    fh_px_C->Write();
    fh_py_C->Write();
    fh_pz_C->Write();
    fh_p_C->Write();

    fh_dpx_He->Write();
    fh_dpy_He->Write();
    fh_dpz_He->Write();
    fh_dp_He->Write();
    fh_dpx_C->Write();
    fh_dpy_C->Write();
    fh_dpz_C->Write();
    fh_dp_C->Write();

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

    fh_xy_tofd->Write();
    fh_xy_tofd_ac->Write();
    fh_tofd_charge->Write();
    fh_tofd_charge_ac->Write();
    fh_TimePreviousEvent->Write();
    fh_tofd_time->Write();
    fh_tofd_time_ac->Write();
    fh_tofd_mult->Write();
    fh_tofd_mult_ac->Write();
    fh_tofd_q2_vs_q1->Write();
    fh_tofd_q2_vs_q1_ac->Write();

    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
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
        fh_Fib_Time[ifibcount]->Write();
        fh_Fib_Time_ac[ifibcount]->Write();
    }

    fh_Fib33_vs_Fib31->Write();
    fh_Fib33_vs_Fib31_dx->Write();
    fh_Fib31_vs_Fib23a->Write();
    fh_Fib31_vs_Fib23a_dx->Write();
    fh_Fib30_vs_Fib32->Write();
    fh_Fib30_vs_Fib32_dx->Write();
    fh_Fib32_vs_Fib23b->Write();
    fh_Fib32_vs_Fib23b_dx->Write();

    fh_Fib33_vs_Fib31_back->Write();
    fh_Fib33_vs_Fib31_dx_back->Write();
    fh_Fib31_vs_Fib23a_back->Write();
    fh_Fib31_vs_Fib23a_dx_back->Write();
    fh_Fib30_vs_Fib32_back->Write();
    fh_Fib30_vs_Fib32_dx_back->Write();
    fh_Fib32_vs_Fib23b_back->Write();
    fh_Fib32_vs_Fib23b_dx_back->Write();

    fh_theta26->Write();
    fh_Erel->Write();
    fh_theta26_simu->Write();
    fh_Erel_simu->Write();
    fh_chiy_vs_chix->Write();
    fh_chi2_vs_Erel->Write();
    fh_dErel_vs_x->Write();
    fh_dErel_vs_y->Write();
    fh_totalPz->Write();
    fh_totalPz_reco->Write();

    for (Int_t i = 0; i < ndet; i++)
    {
        fh_res_xA[i]->Write();
        fh_res_xC[i]->Write();
        fh_res_yA[i]->Write();
        fh_res_yC[i]->Write();
        fh_xy[i]->Write();
        fh_p_vs_x[i]->Write();
        fh_p_vs_x_test[i]->Write();
    }
}

ClassImp(R3BTrackerTestS494)
