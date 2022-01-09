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
// -----                  R3BTrackDoubleS494                     ----
// -----          Created April 13th 2016 by M.Heil       -----
// -----          Changed in Jan 2022 by A.Kelic-Heil to  -----
// -----         preselected hit-level data (pre-track).  -----
// -----         If you want to use full hit-level data, ------
//------         use R3BTrackS494.cxx                    ------
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BCalifaMappedData.h"
#include "R3BLosCalData.h"
#include "R3BLosHitData.h"
#include "R3BLosMappedData.h"

#include "R3BBeamMonitorMappedData.h"

#include "R3BTrackDoubleS494.h"

#include "R3BSci8CalData.h"
#include "R3BSci8MappedData.h"

#include "R3BTofdCalData.h"
#include "R3BTofdHitData.h"
#include "R3BTofdMappedData.h"

#include "R3BFiberMAPMTCalData.h"
#include "R3BFiberMAPMTHitData.h"
#include "R3BFiberMAPMTMappedData.h"

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
#include "TFile.h"
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

R3BTrackDoubleS494::R3BTrackDoubleS494()
    : R3BTrackDoubleS494("Track", 1)
{
}

R3BTrackDoubleS494::R3BTrackDoubleS494(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat1(-1)
    , fTpat2(-1)
    , fCuts(0)
    , fGraphCuts(0)
    , fPairs(0)
    , fB(-1250)
    , fSimu(0)
    , fNEvents(0)
    , fTrackItems(new TClonesArray("R3BTrack"))
    , fNofTrackItems()
{
}

R3BTrackDoubleS494::~R3BTrackDoubleS494()
{
    for (int i = 0; i < NOF_FIB_DET; i++)
    {
        delete fh_xy_Fib[i];
        delete fh_mult_Fib[i];
        delete fh_ToT_Fib[i];
    }
    delete fTrackItems;
}

InitStatus R3BTrackDoubleS494::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(INFO) << "R3BTrackDoubleS494::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline* run = FairRunOnline::Instance();

    // Get objects for detectors on all levels
    fMCTrack = (TClonesArray*)mgr->GetObject("MCTrack");
    if (fMCTrack)
        mgr->Register("MCTrack", "Monte Carlo Tracks", fMCTrack, kTRUE);

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

    mgr->Register("Track", "Land", fTrackItems, kTRUE);

    //------------------------------------------------------------------------
    // graphical cuts
    //------------------------------------------------------------------------
    if (fGraphCuts)
    {
        cut_fi31_fi23a = NULL;
        cut_fi30_fi23b = NULL;
        cut_Fi33vsFi31 = NULL;
        cut_Fi30vsFi32 = NULL;

        TFile* f = TFile::Open("fiberCuts.root", "read");
        cut_Fi33vsFi31 = dynamic_cast<TCutG*>(f->Get("cut_dx_Fi33_Fi31"));
        cut_Fi30vsFi32 = dynamic_cast<TCutG*>(f->Get("cut_dx_Fi30_Fi32"));

        TFile* f23a = TFile::Open("myCutsFi23a.root", "read");
        cut_fi31_fi23a = dynamic_cast<TCutG*>(f23a->Get("cut_fi31_fi23a"));

        TFile* f23b = TFile::Open("myCutsFi23b.root", "read");
        cut_fi30_fi23b = dynamic_cast<TCutG*>(f23b->Get("cut_fi32_fi23b"));
    }
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

    fh_p_He = new TH1F("p_He", " p He meanSoll 4348 MeV/c", 30000, 0, 30000.);
    fh_p_He->GetXaxis()->SetTitle("p / MeV/c");
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

    fh_p_C = new TH1F("p_C", " p C meanSoll 13044 MeV/c", 30000, 0, 30000.);
    fh_p_C->GetXaxis()->SetTitle("p / MeV/c");
    fh_p_C->GetYaxis()->SetTitle("counts");

    fh_dpx_He = new TH1F("dpx_He", "dpx He", 200, -10., 10.);
    fh_dpx_He->GetXaxis()->SetTitle("dpx / percent");
    fh_dpx_He->GetYaxis()->SetTitle("counts");

    fh_dpy_He = new TH1F("dpy_He", "dpy He", 200, -10., 10.);
    fh_dpy_He->GetXaxis()->SetTitle("dpy / percent");
    fh_dpy_He->GetYaxis()->SetTitle("counts");

    fh_dpz_He = new TH1F("dpz_He", "dpz He", 100, -5., 5.);
    fh_dpz_He->GetXaxis()->SetTitle("dpz / percent");
    fh_dpz_He->GetYaxis()->SetTitle("counts");

    fh_dp_He = new TH1F("dp_He", "dp He", 100, -5., 5.);
    fh_dp_He->GetXaxis()->SetTitle("dp / percent");
    fh_dp_He->GetYaxis()->SetTitle("counts");

    fh_dpx_C = new TH1F("dpx_C", "dpx C", 200, -10., 10.);
    fh_dpx_C->GetXaxis()->SetTitle("dpx / percent");
    fh_dpx_C->GetYaxis()->SetTitle("counts");

    fh_dpy_C = new TH1F("dpy_C", "dpy C", 200, -10., 10.);
    fh_dpy_C->GetXaxis()->SetTitle("dpy / percent");
    fh_dpy_C->GetYaxis()->SetTitle("counts");

    fh_dpz_C = new TH1F("dpz_C", "dpz C", 100, -5., 5.);
    fh_dpz_C->GetXaxis()->SetTitle("dpz / percent");
    fh_dpz_C->GetYaxis()->SetTitle("counts");

    fh_dp_C = new TH1F("dp_C", "dp C", 100, -5., 5.);
    fh_dp_C->GetXaxis()->SetTitle("dp / percent");
    fh_dp_C->GetYaxis()->SetTitle("counts");

    fh_target_xy = new TH2F("target_xy", "target xy ", 5000, -2.5, 2.5, 5000, -2.5, 2.5);
    fh_target_xy->GetXaxis()->SetTitle("x / cm");
    fh_target_xy->GetYaxis()->SetTitle("y / cm");

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

    fh_px_px = new TH2F("px_px", "simulation px 4He vs. px 12C", 300, -300, 300, 300, -300., 300.);
    fh_px_px->GetXaxis()->SetTitle("px 12C");
    fh_px_px->GetYaxis()->SetTitle("py 4He");

    fh_p_p = new TH2F("p_p", "p 4He vs. p 12C", 1000, 12000, 14000, 1000, 3500., 5500.);
    fh_p_p->GetXaxis()->SetTitle("p 12C");
    fh_p_p->GetYaxis()->SetTitle("p 4He");

    fh_sum_p = new TH1F("sum_p", "p 4He + p 12C", 2000, 16500, 18500);
    fh_sum_p->GetXaxis()->SetTitle("p 12C + 4He");
    fh_sum_p->GetYaxis()->SetTitle("counts");

    fh_p_p_simu = new TH2F("p_p_simu", "simulation p 4He vs. p 12C", 1000, 12000, 14000, 1000, 3500., 5500.);
    fh_p_p_simu->GetXaxis()->SetTitle("p 12C");
    fh_p_p_simu->GetYaxis()->SetTitle("p 4He");

    fh_sum_p_simu = new TH1F("sum_p_simu", "simulation p 4He + p 12C", 1000, 17000, 18000);
    fh_sum_p_simu->GetXaxis()->SetTitle("p 12C + 4He");
    fh_sum_p_simu->GetYaxis()->SetTitle("counts");

    // histograms for track hits
    for (Int_t i = 0; i < ndet; i++)
    {

        fh_res_xA[i] = new TH1F(Form("resA_x_%i", i), Form("x-residuals of Det %i", i), 5000, -10, 10);
        fh_res_xA[i]->GetXaxis()->SetTitle("x-residuals / cm");
        fh_res_xA[i]->GetYaxis()->SetTitle("Counts");

        fh_res_yA[i] = new TH1F(Form("resA_y_%i", i), Form("y-residuals of Det %i", i), 5000, -10, 10);
        fh_res_yA[i]->GetXaxis()->SetTitle("y-residuals / cm");
        fh_res_yA[i]->GetYaxis()->SetTitle("Counts");

        fh_res_xC[i] = new TH1F(Form("resC_x_%i", i), Form("x-residuals of Det %i", i), 5000, -10, 10);
        fh_res_xC[i]->GetXaxis()->SetTitle("x-residuals / cm");
        fh_res_xC[i]->GetYaxis()->SetTitle("Counts");

        fh_res_yC[i] = new TH1F(Form("resC_y_%i", i), Form("y-residuals of Det %i", i), 5000, -10, 10);
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
        if (fCalItems.at(DET_FI_FIRST + ifibcount) || fHitItems.at(DET_FI_FIRST + ifibcount))
        {

            const char* detName;
            const char* detName2;
            detName = fDetectorNames[DET_FI_FIRST + ifibcount];

            LOG(DEBUG) << "I am creating canvas " << detName << endl;

            // xy:
            fh_xy_Fib[ifibcount] =
                new TH2F(Form("%s_xy", detName), Form("%s xy", detName), 600, -30., 30., 600, -30., 30.);
            fh_xy_Fib[ifibcount]->GetXaxis()->SetTitle("x / cm ");
            fh_xy_Fib[ifibcount]->GetYaxis()->SetTitle("y / cm");

            // Multiplicity (number of hit fibers):
            fh_mult_Fib[ifibcount] = new TH1F(Form("%s_mult", detName), Form("%s # of fibers", detName), 500, 0., 500.);
            fh_mult_Fib[ifibcount]->GetXaxis()->SetTitle("Multiplicity");
            fh_mult_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // ToT MAPMT:
            fh_ToT_Fib[ifibcount] =
                new TH2F(Form("%s_tot_m", detName), Form("%s ToT of MAPMT", detName), 600, -30., 30, 400, 0., 400.);
            fh_ToT_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber x / cm");
            fh_ToT_Fib[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            // ToF Tofd -> Fiber:
            fh_Fib_ToF[ifibcount] = new TH2F(
                Form("%s_tof", detName), Form("%s ToF Tofd to Fiber", detName), 600, -30., 30, 2000, -1000., 1000.);
            fh_Fib_ToF[ifibcount]->GetYaxis()->SetTitle("ToF / ns");
            fh_Fib_ToF[ifibcount]->GetXaxis()->SetTitle("x / cm");

            // Time:
            fh_Fib_Time[ifibcount] =
                new TH2F(Form("%s_time", detName), Form("%s Time", detName), 600, -30., 30, 1000, -1000., 1000.);
            fh_Fib_Time[ifibcount]->GetYaxis()->SetTitle("Time / ns");
            fh_Fib_Time[ifibcount]->GetXaxis()->SetTitle("x / cm");

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

            // hit fiber number vs. event number:
            fh_Fib_vs_Events[ifibcount] = new TH2F(Form("%s_fib_vs_event", detName),
                                                   Form("%s Fiber # vs. Event #", detName),
                                                   10000,
                                                   0,
                                                   Nmax,
                                                   600,
                                                   -30.,
                                                   30.);
            fh_Fib_vs_Events[ifibcount]->GetYaxis()->SetTitle("Fiber x / cm");
            fh_Fib_vs_Events[ifibcount]->GetXaxis()->SetTitle("Event number");

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

            // hit fiber vs. fiber position:

        } // end if(Mapped)

    } // end for(ifibcount)

    fh_Fib33_vs_Fib31_back =
        new TH2F("fib33_vs_fib31_back", "Fiber 33 vs. Fiber 31 back", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib33_vs_Fib31_back->GetYaxis()->SetTitle("Fiber33");
    fh_Fib33_vs_Fib31_back->GetXaxis()->SetTitle("Fiber31");

    fh_Fib31_vs_Fib23a_back =
        new TH2F("fib31_vs_fib23a_back", "Fiber 31 vs. Fiber 23a back", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib31_vs_Fib23a_back->GetYaxis()->SetTitle("Fiber31");
    fh_Fib31_vs_Fib23a_back->GetXaxis()->SetTitle("Fiber23a");

    fh_Fib32_vs_Fib30_back =
        new TH2F("fib32_vs_fib30_back", "Fiber 32 vs. Fiber 30 back", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib32_vs_Fib30_back->GetYaxis()->SetTitle("Fiber32");
    fh_Fib32_vs_Fib30_back->GetXaxis()->SetTitle("Fiber30");

    fh_Fib30_vs_Fib23b_back =
        new TH2F("fib30_vs_fib23b_back", "Fiber 30 vs. Fiber 23b back", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib30_vs_Fib23b_back->GetYaxis()->SetTitle("Fiber30");
    fh_Fib30_vs_Fib23b_back->GetXaxis()->SetTitle("Fiber23b");

    // dx between fibers vs x
    fh_Fib23a_vs_Fib23b_back =
        new TH2F("fib23a_fib23b_back", "x of Fiber 23a and y Fiber 23b back", 240, -6, 6, 240, -6., 6.);
    fh_Fib23a_vs_Fib23b_back->GetYaxis()->SetTitle("y Fi23b / cm");
    fh_Fib23a_vs_Fib23b_back->GetXaxis()->SetTitle("x Fi23a / cm");

    fh_Fib31_vs_Fib23a_dx_back =
        new TH2F("fib31_fib23a_dx_back", "dx of Fiber 31 and Fiber 23a back", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib31_vs_Fib23a_dx_back->GetYaxis()->SetTitle("xFi31 - xFi23a / cm");
    fh_Fib31_vs_Fib23a_dx_back->GetXaxis()->SetTitle("x Fi23a / cm");

    fh_Fib32_vs_Fib30_dx_back =
        new TH2F("fib32_fib30_dx_back", "dx of Fiber 32 and Fiber 30 back", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib32_vs_Fib30_dx_back->GetYaxis()->SetTitle("xFi32 - xFi30 / cm");
    fh_Fib32_vs_Fib30_dx_back->GetXaxis()->SetTitle("x Fi30 / cm");

    fh_Fib30_vs_Fib23b_dx_back =
        new TH2F("fib30_fib23b_dx_back", "dx of Fiber 30 and Fiber 23b back", 1000, -50, 50, 1000, -50., 50.);
    fh_Fib30_vs_Fib23b_dx_back->GetYaxis()->SetTitle("xFi30 - xFi23b / cm");
    fh_Fib30_vs_Fib23b_dx_back->GetXaxis()->SetTitle("x Fi23b / cm");

    //---------------------------------------------------------------------------------------------------
    // TofD detector

    if (fHitItems.at(DET_TOFD) || fCalItems.at(DET_TOFD))
    {

        // xy:
        fh_xy_tofd = new TH2F("tofd_xy", "tofd xy", 200, -100., 100., 200, -100., 100.);
        fh_xy_tofd->GetXaxis()->SetTitle("x / cm ");
        fh_xy_tofd->GetYaxis()->SetTitle("y / cm");

        fh_tofd_charge = new TH1F("tofd_Q", "Charge of Tofd", 200, 0., 20.);
        fh_tofd_charge->GetXaxis()->SetTitle("x / cm ");
        fh_tofd_charge->GetYaxis()->SetTitle("y / cm");

        fh_tofd_mult = new TH1F("tofd_mult", "ToFD multiplicits ", 100, 0, 100);
        fh_tofd_mult->GetXaxis()->SetTitle("multiplicity");
        fh_tofd_mult->GetYaxis()->SetTitle("counts");

        fh_TimePreviousEvent = new TH1F("TimePreviousEvent", "Time between 2 particles ", 3000, 0, 3000);
        fh_TimePreviousEvent->GetXaxis()->SetTitle("time / ns");
        fh_TimePreviousEvent->GetYaxis()->SetTitle("counts");

        fh_tofd_time = new TH1F("tofd_time", "Tofd times ", 40000, -2000, 2000);
        fh_tofd_time->GetXaxis()->SetTitle("time / ns");
        fh_tofd_time->GetYaxis()->SetTitle("counts");

        fh_tofd_q2_vs_q1 = new TH2F("tofd_q2_vs_q1", "tofd q2 vs. q1", 500, 0., 50., 500, 0., 50.);
        fh_tofd_q2_vs_q1->GetXaxis()->SetTitle("q1");
        fh_tofd_q2_vs_q1->GetYaxis()->SetTitle("q2");
    }

    if (fMappedItems.at(DET_CALIFA))
    {
        fh_califa_energy = new TH2F("fh_califa_energy", "Califa E vs crystal id", 2000, 0, 2000, 1000, 0., 1000.);
        fh_califa_energy->GetYaxis()->SetTitle("Energy / MeV");
        fh_califa_energy->GetXaxis()->SetTitle("Crystal #");
    }

    // Analysis
    fh_chiy_vs_chix = new TH2F("chiy_vs_chix", "chi y vs. q1", 2000, 0., 200., 2000, 0., 200.);
    fh_chiy_vs_chix->GetXaxis()->SetTitle("chi x");
    fh_chiy_vs_chix->GetYaxis()->SetTitle("chi y");

    fh_theta26 = new TH1F("theta26", "theta 26 ", 500, 0., 5);
    fh_theta26->GetXaxis()->SetTitle("angle / degree");
    fh_theta26->GetYaxis()->SetTitle("counts");

    fh_theta26_simu = new TH1F("theta26_simu", "theta 26 simulation", 500, 0., 5);
    fh_theta26_simu->GetXaxis()->SetTitle("angle / degree");
    fh_theta26_simu->GetYaxis()->SetTitle("counts");

    fh_Erel = new TH1F("Erel", "Erel ", 5000, 0., 100);
    fh_Erel->GetXaxis()->SetTitle("Erel / MeV");
    fh_Erel->GetYaxis()->SetTitle("counts");

    fh_Erel_simu = new TH1F("Erel_simu", "Erel simulation", 6000, -10., 50);
    fh_Erel_simu->GetXaxis()->SetTitle("Erel / MeV");
    fh_Erel_simu->GetYaxis()->SetTitle("counts");

    fh_dErel = new TH1F("dErel", "delta Erel ", 500, -10., -10);
    fh_dErel->GetYaxis()->SetTitle("counts");
    fh_dErel->GetXaxis()->SetTitle("Delta Erel / MeV");

    fh_dtheta = new TH1F("dtheta", "dtheta", 500, -5., 5);
    fh_dtheta->GetXaxis()->SetTitle("Delta theta");
    fh_dtheta->GetYaxis()->SetTitle("counts");

    fh_x4He_vs_x12C = new TH2F("x4He_vs_x12C", " x4He vs. x12C measured on ToFD", 120, -60., 60., 120, -60., 60.);
    fh_x4He_vs_x12C->GetXaxis()->SetTitle("x 12C / cm");
    fh_x4He_vs_x12C->GetYaxis()->SetTitle("x 4He / cm");

    /*
       TCanvas* check = new TCanvas("CheckingMom", "CheckingMom", 10, 10, 900, 900);
       check->Divide(3,3);
       check->cd(1);gPad->SetLogz();
       fh_chiy_vs_chix->Draw("colz");
       check->cd(2);
       fh_chi2->Draw();
       check->cd(3);gPad->SetLogz();
       fh_p_p->Draw("colz");
       check->cd(4);gPad->SetLogz();
       fh_p_vs_x_test[0]->Draw("colz");
       check->cd(5);
       fh_sum_p->Draw();
       check->cd(6);
       fh_p_C->Draw();
       check->cd(7);
       fh_p_He->Draw();
       check->cd(8);gPad->SetLogz();
       fh_target_xy->Draw("colz");
       check->cd(9);gPad->SetLogz();
       fh_x4He_vs_x12C->Draw("colz");


       TCanvas* checkR = new TCanvas("CheckingRes", "CheckingRes", 10, 10, 300,900);
       checkR->Divide(4,5);
       checkR->cd(1);//gPad->SetLogy();
       fh_res_xA[0]->Draw();
       checkR->cd(2);//gPad->SetLogy();
       fh_res_xC[0]->Draw();
       checkR->cd(3);//gPad->SetLogy();
       fh_res_xA[2]->Draw();
       checkR->cd(4);//gPad->SetLogy();
       fh_res_xC[2]->Draw();
       checkR->cd(5);//gPad->SetLogy();
       fh_res_xA[3]->Draw();
       checkR->cd(6);//gPad->SetLogy();
       fh_res_xC[3]->Draw();
       checkR->cd(7);//gPad->SetLogy();
       fh_res_xA[4]->Draw();
       checkR->cd(8);//gPad->SetLogy();
       fh_res_xC[4]->Draw();
       checkR->cd(9);//gPad->SetLogy();
       fh_res_xA[5]->Draw();
       checkR->cd(10);//gPad->SetLogy();
       fh_res_xC[5]->Draw();
       checkR->cd(11);//gPad->SetLogy();
       fh_res_yA[1]->Draw();
       checkR->cd(12);//gPad->SetLogy();
       fh_res_yC[1]->Draw();
       checkR->cd(13);//gPad->SetLogy();
       fh_res_yA[6]->Draw();
       checkR->cd(14);//gPad->SetLogy();
       fh_res_yC[6]->Draw();
       checkR->cd(15);//gPad->SetLogy();
       fh_res_yA[7]->Draw();
       checkR->cd(16);//gPad->SetLogy();
       fh_res_yC[7]->Draw();
       checkR->cd(17);//gPad->SetLogy();
       fh_res_yA[8]->Draw();
       checkR->cd(18);//gPad->SetLogy();
       fh_res_yC[8]->Draw();
       checkR->cd(19);//gPad->SetLogy();
       fh_res_yA[9]->Draw();
       checkR->cd(20);//gPad->SetLogy();
       fh_res_yC[9]->Draw();
      */
    // -------------------------------------------------------------------------
    // Rene's tracker.

    if (tracker)
        init_from_cpp_();

    return kSUCCESS;
}

void R3BTrackDoubleS494::Exec(Option_t* option)
{
    if (fNEvents / 10000. == (int)fNEvents / 10000)
        std::cout << "\rEvents: " << fNEvents << " / " << maxevent << " (" << (int)(fNEvents * 100. / maxevent)
                  << " %) "
                  << " Tofd: " << counterTofd << " tracked: " << counter2 << ", chix calculated: " << counter3
                  << " times, and chiy: " << counter4 << std::flush;
    // cout << "Event: " << fNEvents << endl;
    fNEvents += 1;

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    Bool_t CalifaHit = false;
    if (fMappedItems.at(DET_CALIFA))
    {
        // CALIFA
        auto detCalifa = fMappedItems.at(DET_CALIFA);
        Int_t nHitsCalifa = detCalifa->GetEntriesFast();
        // cout<<"Califa hits: "<<nHitsCalifa<<endl;

        for (Int_t ihit = 0; ihit < nHitsCalifa; ihit++)
        {
            R3BCalifaMappedData* hitCalifa = (R3BCalifaMappedData*)detCalifa->At(ihit);
            if (!hitCalifa)
                continue;

            Int_t Crystal = hitCalifa->GetCrystalId();
            Int_t Energy = hitCalifa->GetEnergy();
            // cout << "Califa: " << Crystal << " Energy: " << Energy << endl;
            if (Energy > 0)
            {
                fh_califa_energy->Fill(Crystal, Energy);
                CalifaHit = true;
            }
        }
    }
    if (CalifaHit)
    {
        counterCalifa++;
        //		return;
    }
    if (fMCTrack) // for simulated data
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
                if (PID == 1000080160)
                {
                    // 12C
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
        fh_px_px->Fill(pCxs, pHexs);
        fh_p_p_simu->Fill(pCs, pHes);
        // fh_sum_p_simu->Fill(pCs+pHes);

        Double_t dx = pHexs + pCxs;
        Double_t dy = pHeys + pCys;
        Double_t dz = pHezs + pCzs;
        Double_t deltaP = sqrt(dx * dx + dy * dy + dz * dz); // - 12850;
        fh_sum_p_simu->Fill(deltaP);
    }

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
    qdet[max] = 2;
    qdet[max + 1] = 6;
    Double_t xFi33[max];
    Double_t yFi33[max];
    Double_t qFi33[max];
    Double_t tFi33[max];
    Double_t timeFi33[max];
    Bool_t fFi33[max];
    Double_t xFi32[max];
    Double_t yFi32[max];
    Double_t qFi32[max];
    Double_t tFi32[max];
    Double_t timeFi32[max];
    Bool_t fFi32[max];
    Double_t xFi31[max];
    Double_t yFi31[max];
    Double_t qFi31[max];
    Double_t tFi31[max];
    Double_t timeFi31[max];
    Bool_t fFi31[max];
    Double_t xFi30[max];
    Double_t yFi30[max];
    Double_t qFi30[max];
    Double_t tFi30[max];
    Double_t timeFi30[max];
    Bool_t fFi30[max];
    Double_t xFi23a[max];
    Double_t yFi23a[max];
    Double_t qFi23a[max];
    Double_t tFi23a[max];
    Double_t timeFi23a[max];
    Bool_t fFi23a[max];
    Double_t xFi23b[max];
    Double_t yFi23b[max];
    Double_t qFi23b[max];
    Double_t tFi23b[max];
    Double_t timeFi23b[max];
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

    Double_t xTrackA[n_det];
    Double_t yTrackA[n_det];
    Double_t zTrackA[n_det];
    Double_t qTrackA[n_det];

    Double_t xTrackC[n_det];
    Double_t yTrackC[n_det];
    Double_t zTrackC[n_det];
    Double_t qTrackC[n_det];

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

    Double_t chi2 = 1.e30;
    ;
    Double_t chi2_best;

    // SET THE OPTIONS ***************
    target[0] = 0.;
    target[1] = 0.;
    target[2] = -0.162;

    Bool_t debug = false;
    Bool_t debug2 = false;
    Bool_t debug_in = false;
    Bool_t debug3 = false;
    Bool_t twice = false; // true: one part after another tracked; false: both part traacked simult.
    Bool_t fibCuts = true;
    // if fibCuts true, dx1 and dx2 used for cuts in xfib correlations
    Double_t dx1 = 3;    // cm
    Double_t dx2 = 5.;   // cm
    Double_t dx3 = 4.;   // cm
    Double_t dtft = 10.; // ns
                         // END CHOOSING OPTIONS **********

    for (int i = 0; i < n_det; i++)
    {
        x[i] = -1000.;
        y[i] = -1000.;
        z[i] = 0.;
        q[i] = 0.;
        t[i] = -1000.;

        x1[i] = -1000.;
        y1[i] = -1000.;
        z1[i] = 0.;
        q1[i] = 0.;
        t1[i] = -1000.;

        x2[i] = -1000.;
        y2[i] = -1000.;
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
    Double_t x_4He = 0.;
    Double_t y_4He = 0.;
    Double_t z_4He = 0.;
    Double_t x_12C = 0.;
    Double_t y_12C = 0.;
    Double_t z_12C = 0.;

    for (Int_t i = 0; i < 10; i++)
    {
        tPrev[i] = -1000.;
        detPrev[i] = -1;
    }

    auto detTofd = fHitItems.at(DET_TOFD);
    Int_t nHits = detTofd->GetEntriesFast();

    if (nHits > 0)
    {
        fh_tofd_mult->Fill(nHits);
        counterTofd++;
        if (debug_in)
        {
            cout << "********************************" << endl;
            cout << "ToFD hits: " << nHits << endl;
        }
    }

    if (nHits > 100)
        return;

    Int_t multTofd = 0;

    // loop over ToFD
    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {

        R3BTofdHitData* hitTofd = (R3BTofdHitData*)detTofd->At(ihit);
        pair = false;

        if (IS_NAN(hitTofd->GetTime()))
            continue;
        if (debug_in)
        {
            cout << "Hit " << ihit << " of " << nHits << " charge " << hitTofd->GetEloss() << " time "
                 << hitTofd->GetTime() << endl;
        }
        Double_t ttt = hitTofd->GetTime();
        fh_tofd_time->Fill(ttt);
        Double_t qqq = hitTofd->GetEloss(); // / 1.132;
        fh_tofd_charge->Fill(qqq);

        Double_t xxx = hitTofd->GetX();
        Double_t yyy = hitTofd->GetY();

        Double_t y_corr = 0.;
        Double_t randx;
        Double_t randy;
        randx = (std::rand() / (float)RAND_MAX) * 2.8 - 1.4;
        randy = (std::rand() / (float)RAND_MAX) * 2.8 - 1.4;
        fh_xy_tofd->Fill(xxx + randx, yyy);

        // first looking for the right charge
        if (fB == -1250 || fB == -1710)
        {

            if (!fPairs && (qqq < 7.5 || qqq > 8.5)) // fPairs given in macro, true 2 particles, false 16O tracking
            {
                if (debug_in)
                    cout << "Not the right charge! Charge <> 8" << endl;
                continue;
            }
            if (fPairs && !(qqq > 1.5 && qqq < 2.5) && !(qqq > 5.5 && qqq < 6.5))
            {
                if (debug_in)
                    cout << "Not the right charge! Charge = " << qqq << " Charge <> 2 and Charge <> 6" << endl;
                continue;
            }
            y_corr = 0.0;
        }

        id2 = hitTofd->GetDetId();
        if (xxx <= 0.)
        {
            // tof rechts
            if (id2 == 1)
            {
                det2 = tofd1r;
            }
            else if (id2 == 2)
            {
                det2 = tofd2r;
            }
        }
        else
        {
            // tof links
            if (id2 == 1)
            {
                det2 = tofd1l;
            }
            else if (id2 == 2)
            {
                det2 = tofd2l;
            }
        }
        x2[det2] = xxx / 100.; // for tracker everything in meters
        y2[det2] = yyy / 100. + y_corr;
        if (y2[det2] < -0.8 || y2[det2] > 0.8) // in meters!
        {
            if (debug_in)
                cout << "Wrong ytoFD-position! " << y2[det2] << " for charge " << q2[det2] << endl;
            continue;
        }

        // Achtung, nur kurzfristig verwenden
        /*
                if (id2 > 1)
                    continue;
                if ( x2[det2] * 100. < 7. || x2[det2] * 100. > 9.)
                    continue;
        */

        z2[det2] = 0.;
        q2[det2] = qqq;
        t2[det2] = hitTofd->GetTime();

        // register hits for tracker as long a time is in the coincidence window
        if ((abs(t2[det2] - t1[det1]) < 50.) || first) // change back to 5.
        {
            if (debug_in)
                cout << "2 particle within 5 ns   " << first << endl;
            // register point for tracker
            detector[countdet] = det2;
            xdet[countdet] = x2[det2];
            ydet[countdet] = y2[det2];
            zdet[countdet] = z2[det2];
            qdet[countdet] = (int)(q2[det2] + 0.5); // q for tracker must be integer
            if (debug_in)
            {
                cout << "registered"
                     << " x: " << x2[det2] << " y: " << y2[det2] << " q: " << q2[det2] << " t: " << t2[det2] << " ID "
                     << id2 << endl;
            }
            if (abs(qdet[countdet] - 2.) < 0.5)
            {
                alpha = true;
                /*                cout << "alpha: " << alpha << endl;
                                cout << "registered"
                                     << " x: " << x2[det2] << " y: " << y2[det2] << " q: " << q2[det2] << " t: " <<
                   t2[det2] << " ID "
                                     << id2 << endl;
                */
                x_4He = xdet[countdet];
                y_4He = ydet[countdet];
                z_4He = zdet[countdet];
            }
            if (abs(qdet[countdet] - 6.) < 0.5)
            {
                carbon = true;
                /*                cout << "carbon: " << carbon << endl;
                                cout << "registered"
                                     << " x: " << x2[det2] << " y: " << y2[det2] << " q: " << q2[det2] << " t: " <<
                   t2[det2] << " ID "
                                     << id2 << endl;
                */
                x_12C = xdet[countdet];
                y_12C = ydet[countdet];
                z_12C = zdet[countdet];
            }
            countdet++;
            single = true;
            first = false;
            tStart = t2[det2];

            det1 = det2;
            x1[det1] = x2[det2];
            y1[det1] = y2[det2];
            z1[det1] = 0.; // this is coordinate
            q1[det1] = q2[det2];
            t1[det1] = t2[det2];
            id1 = id2;

            // since we had a coincidence, continue with next event, if not last event.
            if (ihit < nHits - 1)
                continue;
        }
        else
        {
            x2[det2] = -1000.;
            y2[det2] = -1000.;
            z2[det2] = 0.;
            q2[det2] = 0.;
            t2[det2] = -1000.;
        }

        if (!single)
            continue;

        if (fPairs && !(alpha && carbon))
            continue;
        alpha = false;
        carbon = false;
        //        cout << "Found pair!!!!!! " << endl;

        delta = sqrt((x_12C - x_4He) * (x_12C - x_4He) + (y_12C - y_4He) * (y_12C - y_4He));
        //		cout << "Delta: " << delta << endl;
        fh_x4He_vs_x12C->Fill(x_12C * 100. + randx, x_4He * 100. + randy);

        if (debug2)
        {
            cout << "found good event, x_12C " << x_12C * 100. << " y_12C " << y_12C * 100. << endl;
            cout << "found good event, x_4He " << x_4He * 100. << " y_4He " << y_4He * 100. << endl;
            cout << "Delta: " << delta << endl;
        }
        hits1++;

        single = false;
        first = true;

        if (ihit < nHits - 1)
            ihit--;

        det1 = det2;
        x1[det1] = x2[det2];
        y1[det1] = y2[det2];
        z1[det1] = 0.;
        q1[det1] = q2[det2];
        t1[det1] = t2[det2];
        id1 = id2;

        multTofd++;

        counterTofdMulti++;

        if (debug_in)
            cout << "start fiber analysis" << endl;

        // loop over fiber 33
        auto detHit33 = fHitItems.at(DET_FI33);
        Int_t nHits33 = detHit33->GetEntriesFast();
        LOG(DEBUG) << "Fi33 hits: " << nHits33 << endl;

        Int_t mult33 = 0;
        for (Int_t ihit33 = 0; ihit33 < nHits33; ihit33++)
        {
            det = fi33;
            R3BFiberMAPMTHitData* hit33 = (R3BFiberMAPMTHitData*)detHit33->At(ihit33);
            x1[det] = hit33->GetX() / 100.;
            y1[det] = hit33->GetY() / 100.;
            z1[det] = 0.;
            q1[det] = hit33->GetEloss();

            t1[det] = hit33->GetTime();
            tof = tStart - t1[det];

            // Fill histograms before cuts
            fh_Fib_ToF[det]->Fill(x1[det] * 100., tof);
            fh_xy_Fib[det]->Fill(x1[det] * 100., y1[det] * 100.);
            fh_mult_Fib[det]->Fill(nHits33);
            fh_ToT_Fib[det]->Fill(x1[det] * 100., q1[det]);
            fh_Fibs_vs_Tofd[det]->Fill(x1[tofd1r] * 100. + randx, x1[det] * 100.);
            fh_Fib_vs_Events[det]->Fill(fNEvents, x1[det] * 100.);
            fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
            fh_Fib_Time[det]->Fill(x1[det] * 100., t1[det]);

            hits33++;

            if (debug3)
                cout << "Fi33: " << ihit33 << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                     << " t1: " << t1[det] << endl;

            detector[countdet] = det;
            xdet[countdet] = x1[det];
            ydet[countdet] = y1[det];
            zdet[countdet] = z1[det];
            qdet[countdet] = q1[det];
            countdet++;
        }

        // loop over fiber 31
        auto detHit31 = fHitItems.at(DET_FI31);
        Int_t nHits31 = detHit31->GetEntriesFast();
        LOG(DEBUG) << "Fi31 hits: " << nHits31 << endl;
        Int_t mult31 = 0;
        for (Int_t ihit31 = 0; ihit31 < nHits31; ihit31++)
        {
            det = fi31;
            R3BFiberMAPMTHitData* hit31 = (R3BFiberMAPMTHitData*)detHit31->At(ihit31);
            x1[det] = hit31->GetX() / 100.;
            y1[det] = hit31->GetY() / 100.;
            z1[det] = 0.;
            q1[det] = hit31->GetEloss();

            t1[det] = hit31->GetTime();
            tof = tStart - t1[det];

            // Fill histograms before cuts
            fh_Fib_ToF[det]->Fill(x1[det] * 100., tof);
            fh_xy_Fib[det]->Fill(x1[det] * 100., y1[det] * 100);
            fh_mult_Fib[det]->Fill(nHits31);
            fh_ToT_Fib[det]->Fill(x1[det] * 100., q1[det]);
            fh_Fibs_vs_Tofd[det]->Fill(x1[tofd1r] * 100. + randx, x1[det] * 100.);
            fh_Fib_vs_Events[det]->Fill(fNEvents, x1[det] * 100.);
            fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
            fh_Fib_Time[det]->Fill(x1[det] * 100., t1[det]);

            hits31++;

            if (debug3)
                cout << "Fi31: " << ihit31 << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                     << " t1: " << t1[det] << endl;

            detector[countdet] = det;
            xdet[countdet] = x1[det];
            ydet[countdet] = y1[det];
            zdet[countdet] = z1[det];
            qdet[countdet] = q1[det];
            countdet++;
        }

        // loop over fiber 32
        auto detHit32 = fHitItems.at(DET_FI32);
        Int_t nHits32 = detHit32->GetEntriesFast();
        LOG(DEBUG) << "Fi32 hits: " << nHits32 << endl;
        Int_t mult32 = 0;
        for (Int_t ihit32 = 0; ihit32 < nHits32; ihit32++)
        {
            det = fi32;
            R3BFiberMAPMTHitData* hit32 = (R3BFiberMAPMTHitData*)detHit32->At(ihit32);
            x1[det] = hit32->GetX() / 100.;
            y1[det] = hit32->GetY() / 100.;
            z1[det] = 0.;
            q1[det] = hit32->GetEloss();

            t1[det] = hit32->GetTime();
            tof = tStart - t1[det];

            // Fill histograms before cuts
            fh_Fib_ToF[det]->Fill(x1[det] * 100., tof);
            fh_xy_Fib[det]->Fill(x1[det] * 100., y1[det] * 100.);
            fh_mult_Fib[det]->Fill(nHits32);
            fh_ToT_Fib[det]->Fill(x1[det] * 100., q1[det]);
            fh_Fibs_vs_Tofd[det]->Fill(x1[tofd1l] * 100. + randx, x1[det] * 100.);
            fh_Fib_vs_Events[det]->Fill(fNEvents, x1[det] * 100.);
            fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
            fh_Fib_Time[det]->Fill(x1[det] * 100., t1[det]);

            hits32++;

            if (debug3)
                cout << "Fi32: " << ihit32 << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                     << " t1: " << t1[det] << endl;

            detector[countdet] = det;
            xdet[countdet] = x1[det];
            ydet[countdet] = y1[det];
            zdet[countdet] = z1[det];
            qdet[countdet] = q1[det];
            countdet++;
        }

        // loop over fiber 30
        auto detHit30 = fHitItems.at(DET_FI30);
        Int_t nHits30 = detHit30->GetEntriesFast();
        LOG(DEBUG) << "Fi30 hits: " << nHits30 << endl;
        Int_t mult30 = 0;
        for (Int_t ihit30 = 0; ihit30 < nHits30; ihit30++)
        {
            det = fi30;
            R3BFiberMAPMTHitData* hit30 = (R3BFiberMAPMTHitData*)detHit30->At(ihit30);
            x1[det] = hit30->GetX() / 100.;
            y1[det] = hit30->GetY() / 100.;
            z1[det] = 0.;
            q1[det] = hit30->GetEloss();

            t1[det] = hit30->GetTime();
            tof = tStart - t1[det];

            // Fill histograms before cuts
            fh_Fib_ToF[det]->Fill(x1[det] * 100., tof);
            fh_xy_Fib[det]->Fill(x1[det] * 100., y1[det] * 100.);
            fh_mult_Fib[det]->Fill(nHits30);
            fh_ToT_Fib[det]->Fill(x1[det] * 100., q1[det]);
            fh_Fibs_vs_Tofd[det]->Fill(x1[tofd1l] * 100. + randx, x1[det] * 100.);
            fh_Fib_vs_Events[det]->Fill(fNEvents, x1[det] * 100.);
            fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
            fh_Fib_Time[det]->Fill(x1[det] * 100., t1[det]);

            hits30++;

            if (debug3)
                cout << "Fi30: " << ihit30 << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                     << " t1: " << t1[det] << endl;

            detector[countdet] = det;
            xdet[countdet] = x1[det];
            ydet[countdet] = y1[det];
            zdet[countdet] = z1[det];
            qdet[countdet] = q1[det];
            countdet++;
        }

        // loop over fiber 23a
        auto detHit23a = fHitItems.at(DET_FI23A);
        Int_t nHits23a = detHit23a->GetEntriesFast();
        LOG(DEBUG) << "Fi23a hits: " << nHits23a << endl;
        Int_t mult23a = 0;
        // fh_mult_Fib[fi23a]->Fill(nHits23a);
        for (Int_t ihit23a = 0; ihit23a < nHits23a; ihit23a++)
        {
            det = fi23a;
            R3BFiberMAPMTHitData* hit23a = (R3BFiberMAPMTHitData*)detHit23a->At(ihit23a);
            x1[det] = hit23a->GetX() / 100.;
            y1[det] = hit23a->GetY() / 100.;
            z1[det] = 0.;
            q1[det] = hit23a->GetEloss();

            t1[det] = hit23a->GetTime();
            tof = tStart - t1[det];

            // Fill histograms before cuts
            fh_Fib_ToF[det]->Fill(x1[det] * 100., tof);
            fh_xy_Fib[det]->Fill(x1[det] * 100., y1[det] * 100.);
            fh_mult_Fib[det]->Fill(nHits23a);
            fh_ToT_Fib[det]->Fill(x1[det] * 100., q1[det]);
            fh_Fibs_vs_Tofd[det]->Fill(x1[tofd1r] * 100. + randx, x1[det] * 100.);
            fh_Fib_vs_Events[det]->Fill(fNEvents, x1[det] * 100.);
            fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
            fh_Fib_Time[det]->Fill(x1[det] * 100., t1[det]);

            if (debug3)
                cout << "Fi23a " << ihit23a << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                     << " t1: " << tof << endl;

            detector[countdet] = det;
            xdet[countdet] = x1[det];
            ydet[countdet] = y1[det];
            zdet[countdet] = z1[det];
            qdet[countdet] = q1[det];
            countdet++;
        }

        // loop over fiber 23b
        auto detHit23b = fHitItems.at(DET_FI23B);
        Int_t nHits23b = detHit23b->GetEntriesFast();
        LOG(DEBUG) << "Fi23b hits: " << nHits23b << endl;
        Int_t mult23b = 0;
        //   fh_mult_Fib[fi23b]->Fill(nHits23b);
        for (Int_t ihit23b = 0; ihit23b < nHits23b; ihit23b++)
        {
            det = fi23b;
            R3BFiberMAPMTHitData* hit23b = (R3BFiberMAPMTHitData*)detHit23b->At(ihit23b);
            x1[det] = hit23b->GetX() / 100.;
            y1[det] = hit23b->GetY() / 100.;
            z1[det] = 0.;
            q1[det] = hit23b->GetEloss();

            t1[det] = hit23b->GetTime();
            tof = tStart - t1[det];

            // Fill histograms before cuts
            fh_Fib_ToF[det]->Fill(y1[det] * 100., tof);
            fh_xy_Fib[det]->Fill(x1[det] * 100., y1[det] * 100.);
            fh_mult_Fib[det]->Fill(nHits23b);
            fh_ToT_Fib[det]->Fill(y1[det] * 100., q1[det]);
            fh_Fibs_vs_Tofd[det]->Fill(y1[tofd1l] * 100., y1[det] * 100.);
            fh_Fib_vs_Events[det]->Fill(fNEvents, y1[det] * 100.);
            fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
            fh_Fib_Time[det]->Fill(y1[det] * 100., t1[det]);

            if (debug3)
                cout << "Fi23b " << ihit23b << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                     << " t1: " << tof << endl;

            detector[countdet] = det;
            xdet[countdet] = x1[det];
            ydet[countdet] = y1[det];
            zdet[countdet] = z1[det];
            qdet[countdet] = q1[det];
            countdet++;
        }

        Bool_t cond1 = kFALSE;
        Bool_t cond2 = kFALSE;
        Bool_t cond3 = kFALSE;
        Bool_t cond4 = kFALSE;
        Bool_t cond5 = kFALSE;
        Bool_t cond6 = kFALSE;

        Bool_t tempFi33 = false;
        Bool_t tempFi32 = false;
        Bool_t tempFi23al = false;
        Bool_t tempFi23bl = false;
        Bool_t tempFi23ar = false;
        Bool_t tempFi23br = false;

        for (Int_t i = 0; i < countdet; i++)
        {
            if (detector[i] < 6)
            {
                // qdet[i] = 0;  // Q for fibers will not be considered in tracker
            }
            if (detector[i] == 0)
            {
                ydet[i] = 0;
            }
            if (detector[i] == 1)
            {
                xdet[i] = 0;
            }
            if (detector[i] > 1 && detector[i] < 6)
            {
                ydet[i] = 0;
            }
            if (detector[i] > 5)
            {
                // ydet[i] = 0;
            }
        }

        // DOUBLETRACK
        if (tracker && fPairs && !twice)
        {
            // double track
            fNeventselect += 1;
            counter2++;
            qdet[max] = 2;
            qdet[max + 1] = 6;
            Bool_t det_coord = true;
            Bool_t st = true; // if true -> double track
            for (Int_t i = 0; i < 6; i++)
            {
                chi[i] = 0.;
            }
            Int_t attempt = 0;

            Double_t deltaP = 100.;
            while (attempt < 1 && abs(deltaP) > 40. && deltaP > -10000.)
            // while( attempt < 1 )
            {

                multi_track_extended_output_from_cpp_(&max,
                                                      &countdet,
                                                      &det_coord,
                                                      &st,
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

                attempt++;

                chi2 = chi[4] + chi[5];                // alle x abw. beide teilchen, alle y...
                fh_chiy_vs_chix->Fill(chi[0], chi[1]); // x, y Abweichung 1 Teilchen
                fh_chiy_vs_chix->Fill(chi[2], chi[3]); // x, y Abweichung 2 Teilchen
                fh_chi2->Fill(chi2);

                /*
                Double_t p4He = sqrt(track[3] * track[3] + track[4] * track[4] + track[5] * track[5]);
                Double_t pC = sqrt(track[9] * track[9] + track[10] * track[10] + track[11] * track[11]);

                deltaP = p4He + pC - 12850.;
                */

                /*
                 * track[0] = x, track[1] = y, track[2] = z, track[3] = px, track[4] = py, track[5] = pz von 1. Teilchen
                 * track[6] = x, track[7] = y, track[8] = z, track[9] = px, track[10] = py, track[11] = pz von 2.
                 * Teilchen
                 */

                Double_t dx = track[3] + track[9];
                Double_t dy = track[4] + track[10];
                Double_t dz = track[5] + track[11];
                deltaP = sqrt(dx * dx + dy * dy + dz * dz) - 17392.; // 12860;
                fh_sum_p->Fill(deltaP + 17392.);

                if (debug)
                {
                    cout << " ###############################################" << endl;
                    cout << "Attempt: " << attempt << " deltaP: " << deltaP << endl;
                    // cout << "delta4He: " << p4He << " deltaC: " << pC << endl;
                    cout << "track1: " << track[0] << "  " << track[1] << "  " << track[2] << "  " << track[3] << "  "
                         << track[4] << "  " << track[5] << endl;
                    cout << "track2: " << track[6] << "  " << track[7] << "  " << track[8] << "  " << track[9] << "  "
                         << track[10] << "  " << track[11] << endl;

                    cout << "chi: " << chi[0] << "  " << chi[1] << "  " << chi[2] << "  " << chi[3] << "  " << chi[4]
                         << "  " << chi[5] << endl;
                    if (fSimu)
                    {
                        cout << "******************************************" << endl;
                        cout << "Track In 4He "
                             << "x " << XHes << " y " << YHes << " z " << ZHes << endl;
                        cout << "px " << pHexs << " py " << pHeys << " z " << pHezs << endl;

                        cout << "Track In 12C "
                             << "x " << XCs << " y " << YCs << " z " << ZCs << endl;
                        cout << "px " << pCxs << " py " << pCys << " z " << pCzs << endl;
                    }
                }
            }
            if (chi[4] < 1e6)
                counter3++;
            if (chi[5] < 1e6)
                counter4++;

            Double_t chi2cut = 5000.0;
            deltaP = 0.;
            if (chi[4] < chi2cut && chi[5] < chi2cut * 10000. && abs(deltaP) < 400.)
            {
                counterTracker++;
                Output1(track, chi);

                // we have a hit
                for (Int_t i = 0; i < ndet; i++)
                {
                    xTrackA[i] = -1000.;
                    yTrackA[i] = -1000.;
                    zTrackA[i] = -1000.;
                    qTrackA[i] = -1000.;
                    xTrackC[i] = -1000.;
                    yTrackC[i] = -1000.;
                    zTrackC[i] = -1000.;
                    qTrackC[i] = -1000.;
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
                        cout << "residuals: " << res1_det_x[i] * 100. << "  " << res1_det_y[i] * 100. << "  "
                             << res2_det_x[i] * 100. << "  " << res2_det_y[i] * 100. << endl;
                    }
                    fh_xy[detector[i]]->Fill(xdet[i] * 100., ydet[i] * 100.);

                    if (res1_det_x[i] != 0)
                    {
                        fh_res_xA[i]->Fill(res1_det_x[i] * 100.);
                    }
                    if (res1_det_y[i] != 0)
                    {
                        fh_res_yA[i]->Fill(res1_det_y[i] * 100.);
                    }
                    if (res2_det_x[i] != 0)
                    {
                        fh_res_xC[i]->Fill(res2_det_x[i] * 100.);
                    }
                    if (res2_det_y[i] != 0)
                    {
                        fh_res_yC[i]->Fill(res2_det_y[i] * 100.);
                    }

                    if (qdet[i] == 2)
                    {
                        xTrackA[detector[i]] = xdet[i];
                        yTrackA[detector[i]] = ydet[i];
                        zTrackA[detector[i]] = zdet[i];
                        qTrackA[detector[i]] = qdet[i];

                        Int_t cdet = detector[i];
                        fh_p_vs_x[cdet]->Fill(xdet[i] * 100., track[5]);
                        Double_t ptemp = sqrt(track[3] * track[3] + track[4] * track[4] + track[5] * track[5]);
                        fh_p_vs_x_test[cdet]->Fill(xdet[i] * 100., ptemp);
                    }
                    if (qdet[i] == 6)
                    {
                        xTrackC[detector[i]] = xdet[i];
                        yTrackC[detector[i]] = ydet[i];
                        zTrackC[detector[i]] = zdet[i];
                        qTrackC[detector[i]] = qdet[i];

                        Int_t cdet = detector[i];
                        fh_p_vs_x[cdet]->Fill(xdet[i] * 100., track[11]);
                        Double_t ptemp = sqrt(track[9] * track[9] + track[10] * track[10] + track[11] * track[11]);
                        fh_p_vs_x_test[cdet]->Fill(xdet[i] * 100., ptemp);
                    }
                }
                // Plots of correlations of Fiber detectors
                fh_Fib33_vs_Fib31_back->Fill(xTrackA[3] * 100., xTrackA[5] * 100.);
                fh_Fib31_vs_Fib23a_back->Fill(xTrackA[0] * 100., xTrackA[3] * 100.);
                fh_Fib32_vs_Fib30_back->Fill(xTrackA[2] * 100., xTrackA[4] * 100.);
                fh_Fib30_vs_Fib23b_back->Fill(xTrackA[1] * 100., xTrackA[2] * 100.);
                fh_Fib23a_vs_Fib23b_back->Fill(xTrackA[0] * 100., yTrackA[1] * 100.);

                fh_Fib31_vs_Fib23a_dx_back->Fill(xTrackA[0] * 100., xTrackA[3] * 100. - xTrackA[0] * 100.);
                fh_Fib32_vs_Fib30_dx_back->Fill(xTrackA[2] * 100., xTrackA[4] * 100. - xTrackA[2] * 100.);
                fh_Fib30_vs_Fib23b_dx_back->Fill(xTrackA[1] * 100., xTrackA[2] * 100. - xTrackA[1] * 100.);

                fh_Fib33_vs_Fib31_back->Fill(xTrackC[3] * 100., xTrackC[5] * 100.);
                fh_Fib23a_vs_Fib23b_back->Fill(xTrackA[0] * 100., yTrackA[1] * 100.);
                fh_Fib31_vs_Fib23a_back->Fill(xTrackC[0] * 100., xTrackC[3] * 100.);
                fh_Fib31_vs_Fib23a_dx_back->Fill(xTrackC[0] * 100., xTrackC[3] * 100. - xTrackC[0] * 100.);
                fh_Fib32_vs_Fib30_back->Fill(xTrackC[2] * 100., xTrackC[4] * 100.);
                fh_Fib32_vs_Fib30_dx_back->Fill(xTrackC[2] * 100., xTrackC[4] * 100. - xTrackC[2] * 100.);
                fh_Fib30_vs_Fib23b_back->Fill(xTrackC[1] * 100., xTrackC[2] * 100.);
                fh_Fib30_vs_Fib23b_dx_back->Fill(xTrackC[1] * 100., xTrackC[2] * 100. - xTrackC[1] * 100.);

                // sorted, lowest Charge first
                new ((*fTrackItems)[fNofTrackItems++])
                    R3BTrack(track[0], track[1], track[2], track[3], track[4], track[5], 2, 2, chi[0], chi[1], 0);
                new ((*fTrackItems)[fNofTrackItems++])
                    R3BTrack(track[6], track[7], track[8], track[9], track[10], track[11], 6, 2, chi[2], chi[3], 0);
            }
        }
        // END DOUBLETRACK

        if (tracker && !fPairs && !twice && ((mult30 > 0 && mult32 > 0) || (mult31 > 0 && mult33 > 0))) // für Eichruns
        {
            // single track
            qdet[max] = 8;
            qdet[max + 1] = 8;
            counter2++;
            Bool_t det_coord = true;
            Bool_t st = false;
            // multi_track_from_cpp_(
            //    &max, &countdet, &det_coord, &st, target, detector, qdet, xdet, ydet, zdet, track, chi);
            multi_track_extended_output_from_cpp_(&max,
                                                  &countdet,
                                                  &det_coord,
                                                  &st,
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

            cout << "back from tracker!" << endl;

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
                cout << "track1: " << track[0] << "  " << track[1] << "  " << track[2] << endl;
                cout << "track1: " << track[3] << "  " << track[4] << "  " << track[5] << endl;
                cout << "chi: " << chi[0] << "  " << chi[1] << endl;
            }

            if (chi[0] < 1.e10 && chi[1] < 1.e10)
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
                    fh_res_xA[i]->Fill(res1_det_x[i] * 100.);
                    fh_res_yA[i]->Fill(res1_det_y[i] * 100.);
                    fh_res_xC[i]->Fill(res1_det_x[i] * 100.);
                    fh_res_yC[i]->Fill(res1_det_y[i] * 100.);
                    fh_xy[detector[i]]->Fill(xdet[i] * 100., ydet[i] * 100.);
                    fh_p_vs_x[detector[i]]->Fill(xdet[i] * 100. + randx, track[5]);
                    fh_p_vs_x_test[detector[i]]->Fill(
                        xdet[i] * 100., sqrt(track[3] * track[3] + track[4] * track[4] + track[5] * track[5]));
                }

                // Plots of correlations of Fiber detectors

                fh_Fib33_vs_Fib31_back->Fill(xTrack[3] * 100., xTrack[5] * 100.);
                fh_Fib23a_vs_Fib23b_back->Fill(xTrack[0] * 100., yTrack[1] * 100.);
                fh_Fib31_vs_Fib23a_back->Fill(xTrack[0] * 100., xTrack[3] * 100.);
                fh_Fib31_vs_Fib23a_dx_back->Fill(xTrack[0] * 100., xTrack[3] * 100. - xTrack[0] * 100.);
                fh_Fib32_vs_Fib30_back->Fill(xTrack[2] * 100., xTrack[4] * 100.);
                fh_Fib32_vs_Fib30_dx_back->Fill(xTrack[2] * 100., xTrack[4] * 100. - xTrack[2] * 100.);
                fh_Fib30_vs_Fib23b_back->Fill(xTrack[1] * 100., xTrack[2] * 100.);
                fh_Fib30_vs_Fib23b_dx_back->Fill(xTrack[1] * 100., xTrack[2] * 100. - xTrack[1] * 100.);

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

            Double_t m_invs =
                sqrt(mHe * mHe + mC * mC + 2. * sqrt(pCs * pCs + mC * mC) * sqrt(pHes * pHes + mHe * mHe) -
                     2. * pHes * pCs * cos(theta_26s * 3.1415 / 180.));
            Double_t Erelbs = m_invs - mHe - mC;

            fh_Erel_simu->Fill(Erelbs);
            //  cout << "Theta 26 simu: " << theta_26s << " Erel simu: " << Erelas << " " << Erelbs << endl;
        }

        for (Int_t i = 0; i < 12; i++)
        {
            track[i] = 0.;
        }
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

    } // end ToFD loop
}
void R3BTrackDoubleS494::Output1(Double_t track[12], Double_t chi[6])
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
    // fh_target_xy->Fill(track[7] * 100., track[8] * 100.);
    fh_px_He->Fill(pHex);
    fh_py_He->Fill(pHey);
    fh_pz_He->Fill(pHez);

    fh_px_C->Fill(pCx);
    fh_py_C->Fill(pCy);
    fh_pz_C->Fill(pCz);

    if (pHexs != 0.)
        fh_dpx_He->Fill((pHex - pHexs) / pHexs * 100.);
    if (pHeys != 0.)
        fh_dpy_He->Fill((pHey - pHeys) / pHeys * 100.);
    if (pHezs != 0.)
        fh_dpz_He->Fill((pHez - pHezs) / pHezs * 100.);

    if (pCxs != 0.)
        fh_dpx_C->Fill((pCx - pCxs) / pCxs * 100.);
    if (pCys != 0.)
        fh_dpy_C->Fill((pCy - pCys) / pCys * 100.);
    if (pCzs != 0.)
        fh_dpz_C->Fill((pCz - pCzs) / pCzs * 100.);

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

    if (pHes != 0.)
        fh_dp_He->Fill((pHe - pHes) / pHes * 100.);
    if (pCs != 0.)
        fh_dp_C->Fill((pC - pCs) / pCs * 100.);

    // cout<<"pC, pHe: "<<pC<<", "<<pHe<<endl;
    fh_p_p->Fill(pC, pHe);
    // fh_sum_p->Fill(pC + pHe);

    Double_t Erela =
        sqrt(pow((mHe + mC + eHe + eC), 2) - pow(pHe, 2) - pow(pC, 2) - 2 * pHe * pC * cos(theta_26 * 3.1415 / 180.)) -
        (mHe + mC); // Erel

    Double_t m_inv = sqrt(mHe * mHe + mC * mC + 2. * sqrt(pC * pC + mC * mC) * sqrt(pHe * pHe + mHe * mHe) -
                          2. * pHe * pC * cos(theta_26 * 3.1415 / 180.));
    Double_t Erelb = m_inv - mHe - mC;

    if (pHes > 0. && pCs > 0.)
    {
        Double_t costh26s =
            (pHexs * pCxs + pHeys * pCys + pHezs * pCzs) /
            (sqrt(pHexs * pHexs + pHeys * pHeys + pHezs * pHezs) * sqrt(pCxs * pCxs + pCys * pCys + pCzs * pCzs));
        Double_t theta_26s = acos(costh26s) * 180. / 3.14159; // opening angle
        Double_t m_invs = sqrt(mHe * mHe + mC * mC + 2. * sqrt(pCs * pCs + mC * mC) * sqrt(pHes * pHes + mHe * mHe) -
                               2. * pHes * pCs * cos(theta_26s * 3.1415 / 180.));
        Double_t Erelbs = m_invs - mHe - mC;
        fh_dErel->Fill(Erelb - Erelbs);
        fh_dtheta->Fill(theta_26 - theta_26s);
    }

    if (Erelb < 0.6)
    {
        cout << "Wrong Event: " << fNEvents << endl;
    }

    fh_Erel->Fill(Erelb);

    //  cout << "Theta 26: " << theta_26 << " Erel: " << Erela << " " << Erelb << endl;
}

void R3BTrackDoubleS494::Output2(Double_t track_parameter[12], Double_t chi_single_parameter[6])
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
void R3BTrackDoubleS494::FinishEvent()
{

    fTrackItems->Clear();
    fNofTrackItems = 0;

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

void R3BTrackDoubleS494::FinishTask()
{
    finish_from_cpp_();
    cout << "Statistics:" << endl;
    cout << "Events: " << fNEvents << endl;
    cout << "Events after selection: " << fNeventselect << endl;
    cout << "Events to tracker: " << fNeventstrack << endl;
    cout << "Wrong Trigger: " << counterWrongTrigger << endl;
    cout << "Wrong Tpat: " << counterWrongTpat << endl;
    cout << "ROLU veto: " << counterRolu << endl;
    cout << "Califa veto: " << counterCalifa << endl;
    cout << "TofD: " << counterTofd << endl;
    cout << "TofD multi: " << counterTofdMulti << endl;
    cout << "Tracker: " << counterTracker << endl;

    cout << "Hits TofD " << hits1 << endl;
    cout << "Eff. Fi30 min: " << hits30 << "  " << hits30 / hits1 << endl;
    cout << "Eff. Fi31 min: " << hits31 << "  " << hits31 / hits1 << endl;
    cout << "Eff. Fi32 min: " << hits32 << "  " << hits32 / hits1 << endl;
    cout << "Eff. Fi33 min: " << hits33 << "  " << hits33 / hits1 << endl;

    fh_Tpat->Write();
    fh_Trigger->Write();
    if (fMappedItems.at(DET_BMON))
    {
        fh_TOFDOR->Write();
        fh_SEE->Write();
        fh_IC->Write();
    }

    if (fMappedItems.at(DET_CALIFA))
    {
        fh_califa_energy->Write();
    }

    if (fHitItems.at(DET_TOFD))
    {
        fh_xy_tofd->Write();
        fh_tofd_charge->Write();
        fh_TimePreviousEvent->Write();
        fh_tofd_time->Write();
        fh_tofd_mult->Write();
        fh_tofd_q2_vs_q1->Write();
    }

    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        if (fCalItems.at(ifibcount + DET_FI_FIRST) || fHitItems.at(ifibcount + DET_FI_FIRST))
        {
            fh_xy_Fib[ifibcount]->Write();
            fh_mult_Fib[ifibcount]->Write();
            fh_ToT_Fib[ifibcount]->Write();
            fh_Fib_vs_Events[ifibcount]->Write();
            fh_Fibs_vs_Tofd[ifibcount]->Write();
            fh_Fib_ToF[ifibcount]->Write();
            fh_ToF_vs_Events[ifibcount]->Write();
            fh_Fib_Time[ifibcount]->Write();
        }
    }

    if (tracker)
    {
        fh_Fib33_vs_Fib31_back->Write();
        fh_Fib23a_vs_Fib23b_back->Write();
        fh_Fib31_vs_Fib23a_back->Write();
        fh_Fib31_vs_Fib23a_dx_back->Write();
        fh_Fib32_vs_Fib30_back->Write();
        fh_Fib32_vs_Fib30_dx_back->Write();
        fh_Fib30_vs_Fib23b_back->Write();
        fh_Fib30_vs_Fib23b_dx_back->Write();

        fh_target_xy->Write();
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
        fh_px_px->Write();
        fh_p_p->Write();
        fh_sum_p->Write();
        fh_p_p_simu->Write();
        fh_sum_p_simu->Write();
        fh_theta26->Write();
        fh_Erel->Write();
        fh_chiy_vs_chix->Write();
        fh_dErel->Write();
        fh_dtheta->Write();

        if (fMCTrack)
        {
            fh_theta26_simu->Write();
            fh_Erel_simu->Write();
        }

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
        fh_x4He_vs_x12C->Write();
    }
}

ClassImp(R3BTrackDoubleS494)
