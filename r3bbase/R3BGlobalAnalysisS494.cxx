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
// -----                  R3BGlobalAnalysisS494                -----
// -----          Created April 13th 2016 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BCalifaClusterData.h"
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaMappedData.h"

#include "R3BGlobalAnalysisS494.h"

#include "R3BTofdHitData.h"

#include "R3BFiberMAPMTHitData.h"

#include "R3BEventHeader.h"
#include "R3BTCalEngine.h"

#include "R3BFiberMAPMTCalData.h"
#include "R3BFiberMAPMTHitData.h"
#include "R3BFiberMAPMTMappedData.h"

#include "R3BBunchedFiberCalData.h"
#include "R3BBunchedFiberHitData.h"
#include "R3BBunchedFiberMappedData.h"

#include "R3BMCTrack.h"
#include "R3BTrack.h"
#include "R3BTrackData.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "R3BLogger.h"

#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"

#include "TFile.h"

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

R3BGlobalAnalysisS494::R3BGlobalAnalysisS494()
    : R3BGlobalAnalysisS494("GlobalAnalysis", 1)
{
}

R3BGlobalAnalysisS494::R3BGlobalAnalysisS494(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fCuts(0)
    , fGhost(0)
    , fPairs(1)
    , fSimu(0)
    , fGraphCuts(0)
    , fB(-1710)
    , fcut_chiX(5000)
    , fcut_chiY(5000)
    , fThetaGrazing(0.79)
    , fxfibcut(0.1)
    , fyfibcut(0.1)
    , fvis(0)
    , fEvsE("")
    , fNEvents(0)
    , fHitItemsCalifa(NULL)
{
}

R3BGlobalAnalysisS494::~R3BGlobalAnalysisS494() {}

InitStatus R3BGlobalAnalysisS494::Init()
{
    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    cout << "R3BGlobalAnalysisS494::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    if (header)
        R3BLOG(info, "EventHeader. was found");
    else
        R3BLOG(info, "EventHeader. was not found");

    FairRunAna* run = FairRunAna::Instance();

    // Get objects for detectors on all levels
    fMCTrack = (TClonesArray*)mgr->GetObject("MCTrack");
    fTrack = (TClonesArray*)mgr->GetObject("TrackData");
    maxevent = mgr->CheckMaxEventNo();

    // get access to data of Califa
    fHitItemsCalifa = (TClonesArray*)mgr->GetObject("CalifaClusterData");
    if (!fHitItemsCalifa)
        LOG(warning) << "GlobalAnalysis: CalifaClusterData not found";

    //------------------------------------------------------------------------
    // graphical cuts
    //------------------------------------------------------------------------
    if (fGraphCuts)
    {
        LOG(info) << "READING GRAPHICAL CUTS " << endl;
        cut_EHe_EC = NULL;
        if (!fEvsE)
        {
            LOG(warn) << "No file with graphical cuts loaded!" << endl;
        }
        else
        {
            TFile* fevse = TFile::Open(fEvsE, "read");
            cut_EHe_EC = dynamic_cast<TCutG*>(fevse->Get("Cut_EHeEC"));

            LOG(info) << "GRAPHICAL CUTS ARE READ" << endl;
        }
    }
    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------

    //    char strNameC[255];
    //    sprintf(strNameC, "Cave_C_position");
    //    fh_Cave_position = new TH2F(strNameC, "", 2100, -100., 2000., 1000, -500., 500.);

    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //

    R3BLOG(info, "Starting to define histograms");
    fh_px_He = new TH1F("px_He", " px He", 500, -500., 500);
    fh_px_He->GetXaxis()->SetTitle("px / MeV/c");
    fh_px_He->GetYaxis()->SetTitle("counts");

    fh_py_He = new TH1F("py_He", " py He", 500, -500., 500);
    fh_py_He->GetXaxis()->SetTitle("py / MeV/c");
    fh_py_He->GetYaxis()->SetTitle("counts");

    fh_pz_He = new TH1F("pz_He", " pz He", 1000, 2000., 5000.);
    fh_pz_He->GetXaxis()->SetTitle("pz / MeV/c");
    fh_pz_He->GetYaxis()->SetTitle("counts");

    fh_p_He = new TH1F("p_He", " p He", 1000, 2000., 5000.);
    fh_p_He->GetXaxis()->SetTitle("pz / MeV/c");
    fh_p_He->GetYaxis()->SetTitle("counts");

    fh_px_C = new TH1F("px_C", " px C", 500, -500., 500);
    fh_px_C->GetXaxis()->SetTitle("px / MeV/c");
    fh_px_C->GetYaxis()->SetTitle("counts");

    fh_py_C = new TH1F("py_C", " py C", 500, -500., 500);
    fh_py_C->GetXaxis()->SetTitle("py / MeV/c");
    fh_py_C->GetYaxis()->SetTitle("counts");

    fh_pz_C = new TH1F("pz_C", " pz C", 2000, 10000., 16000.);
    fh_pz_C->GetXaxis()->SetTitle("pz / MeV/c");
    fh_pz_C->GetYaxis()->SetTitle("counts");

    fh_p_C = new TH1F("p_C", " p C", 200, 10000., 16000.);
    fh_p_C->GetXaxis()->SetTitle("pz / MeV/c");
    fh_p_C->GetYaxis()->SetTitle("counts");

    fh_px_O = new TH1F("px_O", " px O", 500, -500., 500);
    fh_px_O->GetXaxis()->SetTitle("px / MeV/c");
    fh_px_O->GetYaxis()->SetTitle("counts");

    fh_py_O = new TH1F("py_O", " py O", 500, -500., 500);
    fh_py_O->GetXaxis()->SetTitle("py / MeV/c");
    fh_py_O->GetYaxis()->SetTitle("counts");

    fh_pz_O = new TH1F("pz_O", " pz O", 1000, 16000., 19000.);
    fh_pz_O->GetXaxis()->SetTitle("pz / MeV/c");
    fh_pz_O->GetYaxis()->SetTitle("counts");

    fh_p_O = new TH1F("p_O", " p O", 1000, 16000., 19000.);
    fh_p_O->GetXaxis()->SetTitle("pz / MeV/c");
    fh_p_O->GetYaxis()->SetTitle("counts");

    fh_px_He_nc = new TH1F("px_He_nc", " px He all chi2", 500, -500., 500);
    fh_px_He_nc->GetXaxis()->SetTitle("px / MeV/c");
    fh_px_He_nc->GetYaxis()->SetTitle("counts");

    fh_py_He_nc = new TH1F("py_He_nc", " py He all chi2", 500, -500., 500);
    fh_py_He_nc->GetXaxis()->SetTitle("py / MeV/c");
    fh_py_He_nc->GetYaxis()->SetTitle("counts");

    fh_pz_He_nc = new TH1F("pz_He_nc", " pz He all chi2", 1000, 2000., 5000.);
    fh_pz_He_nc->GetXaxis()->SetTitle("pz / MeV/c");
    fh_pz_He_nc->GetYaxis()->SetTitle("counts");

    fh_p_He_nc = new TH1F("p_He_nc", " p He all chi2", 1000, 2000., 5000.);
    fh_p_He_nc->GetXaxis()->SetTitle("pz / MeV/c");
    fh_p_He_nc->GetYaxis()->SetTitle("counts");

    fh_px_C_nc = new TH1F("px_C_nc", " px C all chi2", 500, -500., 500);
    fh_px_C_nc->GetXaxis()->SetTitle("px / MeV/c");
    fh_px_C_nc->GetYaxis()->SetTitle("counts");

    fh_py_C_nc = new TH1F("py_C_nc", " py C all chi2", 500, -500., 500);
    fh_py_C_nc->GetXaxis()->SetTitle("py / MeV/c");
    fh_py_C_nc->GetYaxis()->SetTitle("counts");

    fh_pz_C_nc = new TH1F("pz_C_nc", " pz C all chi2", 2000, 10000., 16000.);
    fh_pz_C_nc->GetXaxis()->SetTitle("pz / MeV/c");
    fh_pz_C_nc->GetYaxis()->SetTitle("counts");

    fh_p_C_nc = new TH1F("p_C_nc", " p C all chi2", 2000, 10000., 16000.);
    fh_p_C_nc->GetXaxis()->SetTitle("pz / MeV/c");
    fh_p_C_nc->GetYaxis()->SetTitle("counts");

    fh_px_O_nc = new TH1F("px_O_nc", " px O all chi2", 500, -500., 500);
    fh_px_O_nc->GetXaxis()->SetTitle("px / MeV/c");
    fh_px_O_nc->GetYaxis()->SetTitle("counts");

    fh_py_O_nc = new TH1F("py_O_nc", " py O all chi2", 500, -500., 500);
    fh_py_O_nc->GetXaxis()->SetTitle("py / MeV/c");
    fh_py_O_nc->GetYaxis()->SetTitle("counts");

    fh_pz_O_nc = new TH1F("pz_O_nc", " pz O all chi2", 1000, 16000., 19000.);
    fh_pz_O_nc->GetXaxis()->SetTitle("pz / MeV/c");
    fh_pz_O_nc->GetYaxis()->SetTitle("counts");

    fh_p_O_nc = new TH1F("p_O_nc", " p O all chi2", 1000, 16000., 19000.);
    fh_p_O_nc->GetXaxis()->SetTitle("pz / MeV/c");
    fh_p_O_nc->GetYaxis()->SetTitle("counts");

    fh_target_xy_nc = new TH2F("target_xy_nc", "target xy all chi2", 3, -1.5, 1.5, 3, -1.5, 1.5);
    fh_target_xy_nc->GetXaxis()->SetTitle("x / cm");
    fh_target_xy_nc->GetYaxis()->SetTitle("y / cm");

    fh_target_xx = new TH1F("target_xHe_xC", "diff target xC vs xHe ", 3, -1.5, 1.5);
    fh_target_xx->GetXaxis()->SetTitle("xHe-xC / cm");
    fh_target_xx->GetYaxis()->SetTitle("counts");

    fh_target_yy = new TH1F("target_yHe_yC", "target yC vs yHe ", 3, -1.5, 1.5);
    fh_target_yy->GetXaxis()->SetTitle("yHe-yC / cm");
    fh_target_yy->GetYaxis()->SetTitle("counts");

    fh_target_xy = new TH2F("target_xy", "target xy ", 2, -1, 1, 2, -1, 1);
    fh_target_xy->GetXaxis()->SetTitle("x / cm");
    fh_target_xy->GetYaxis()->SetTitle("y / cm");

    if (fSimu)
    {
        fh_target_xy_MC = new TH2F("target_xy_MC", "target xy MC", 3, -1.5, 1.5, 3, -1.5, 1.5);
        fh_target_xy_MC->GetXaxis()->SetTitle("x / cm");
        fh_target_xy_MC->GetYaxis()->SetTitle("y / cm");

        fh_px_px_mc = new TH2F("px_px_MC", "px He vs. px C MC", 400, -500, 500, 400, -500., 500.);
        fh_px_px_mc->GetXaxis()->SetTitle("px / MeV/c");
        fh_px_px_mc->GetYaxis()->SetTitle("px / MeV/c");

        fh_py_py_mc = new TH2F("py_py_MC", "py He vs. py C MC", 400, -500, 500, 400, -500., 500.);
        fh_py_py_mc->GetXaxis()->SetTitle("py / MeV/c");
        fh_py_py_mc->GetYaxis()->SetTitle("py / MeV/c");

        fh_pz_pz_mc = new TH2F("pz_pz_MC", "pz He vs. pz C MC", 400, 2000, 6000, 1000, 10000., 20000.);
        fh_pz_pz_mc->GetXaxis()->SetTitle("pz / MeV/c");
        fh_pz_pz_mc->GetYaxis()->SetTitle("pz / MeV/c");

        fh_dx = new TH1F("tracker_dx", "tracker dx ", 200, -5., 5);
        fh_dx->GetXaxis()->SetTitle("dx / cm");
        fh_dx->GetYaxis()->SetTitle("counts");

        fh_dy = new TH1F("tracker_dy", "tracker dy ", 200, -5., 5.);
        fh_dy->GetXaxis()->SetTitle("dy / cm");
        fh_dy->GetYaxis()->SetTitle("counts");

        fh_dz = new TH1F("tracker_dz", "tracker dz ", 200, -5., 5.);
        fh_dz->GetXaxis()->SetTitle("dz / cm");
        fh_dz->GetYaxis()->SetTitle("counts");

        fh_dpxO = new TH1F("tracker_dpxO", "tracker dpx O", 1000, -50, 50);
        fh_dpxO->GetXaxis()->SetTitle("dpx / percent");
        fh_dpxO->GetYaxis()->SetTitle("counts");

        fh_dpyO = new TH1F("tracker_dpyO", "tracker dpy O", 1000, -50, 50);
        fh_dpyO->GetXaxis()->SetTitle("dpy / percent");
        fh_dpyO->GetYaxis()->SetTitle("counts");

        fh_dpzO = new TH1F("tracker_dpzO", "tracker dpz O", 1000, -50, 50);
        fh_dpzO->GetXaxis()->SetTitle("dpz / percent");
        fh_dpzO->GetYaxis()->SetTitle("counts");

        fh_dpO = new TH1F("tracker_dpO", "tracker dp O", 1000, -50, 50);
        fh_dpO->GetXaxis()->SetTitle("dp / percent");
        fh_dpO->GetYaxis()->SetTitle("counts");

        fh_dpxC = new TH1F("tracker_dpxC", "tracker dpx C", 2000, -100, 100);
        fh_dpxC->GetXaxis()->SetTitle("dpx / percent");
        fh_dpxC->GetYaxis()->SetTitle("counts");

        fh_dpyC = new TH1F("tracker_dpyC", "tracker dpy C", 2000, -100, 100);
        fh_dpyC->GetXaxis()->SetTitle("dpy / percent");
        fh_dpyC->GetYaxis()->SetTitle("counts");

        fh_dpzC = new TH1F("tracker_dpzC", "tracker dpzC", 2000, -100, 100);
        fh_dpzC->GetXaxis()->SetTitle("dpz / percent");
        fh_dpzC->GetYaxis()->SetTitle("counts");

        fh_dpC = new TH1F("tracker_dpC", "tracker dpC", 400, -20, 20);
        fh_dpC->GetXaxis()->SetTitle("dp / percent");
        fh_dpC->GetYaxis()->SetTitle("counts");

        fh_dp = new TH1F("tracker_dp", "tracker dp", 400, -20, 20);
        fh_dp->GetXaxis()->SetTitle("dp / percent");
        fh_dp->GetYaxis()->SetTitle("counts");

        fh_dpxHe = new TH1F("tracker_dpxHe", "tracker Hedpx ", 2000, -100, 100);
        fh_dpxHe->GetXaxis()->SetTitle("dpx / percent");
        fh_dpxHe->GetYaxis()->SetTitle("counts");

        fh_dpyHe = new TH1F("tracker_dpyHe", "tracker Hedpy ", 2000, -100, 100);
        fh_dpyHe->GetXaxis()->SetTitle("dpy / percent");
        fh_dpyHe->GetYaxis()->SetTitle("counts");

        fh_dpzHe = new TH1F("tracker_dpzHe", "tracker dpz He", 400, -20, 20);
        fh_dpzHe->GetXaxis()->SetTitle("dpz / percent");
        fh_dpzHe->GetYaxis()->SetTitle("counts");

        fh_dpHe = new TH1F("tracker_dpHe", "tracker dp He", 400, -20, 20);
        fh_dpHe->GetXaxis()->SetTitle("dp / percent");
        fh_dpHe->GetYaxis()->SetTitle("counts");

        fh_dpy_dpx = new TH2F("tracker_dpy_dpx", "tracker dpy vs dpx ", 200, -100, 100, 200, -100., 100.);
        fh_dpy_dpx->GetXaxis()->SetTitle("dpx / percent");
        fh_dpy_dpx->GetYaxis()->SetTitle("dpy / percent");

        fh_thetax_dpx_C =
            new TH2F("tracker_thethax_dpx_C", "tracker theta_x vs dpx C", 2000, -200, 200, 200, -50., 50.);
        fh_thetax_dpx_C->GetXaxis()->SetTitle("dpx / MeV/c");
        fh_thetax_dpx_C->GetYaxis()->SetTitle("theta_x / mrad ");

        fh_thetay_dpy_C =
            new TH2F("tracker_thethay_dpy_C", "tracker theta_y vs dpy_C ", 2000, -200, 200, 200, -50., 50.);
        fh_thetay_dpy_C->GetXaxis()->SetTitle("dpy / MeV/c");
        fh_thetay_dpy_C->GetYaxis()->SetTitle("theta_y / mrad ");

        fh_thetax_dpx_He =
            new TH2F("tracker_thethax_dpx_He", "tracker theta_x vs dpx He", 2000, -200, 200, 200, -50., 50.);
        fh_thetax_dpx_He->GetXaxis()->SetTitle("dpx / MeV/c");
        fh_thetax_dpx_He->GetYaxis()->SetTitle("theta_x / mrad ");

        fh_thetay_dpy_He =
            new TH2F("tracker_thethay_dpy_He", "tracker theta_y vs dpy_He ", 2000, -200, 200, 200, -50., 50.);
        fh_thetay_dpy_He->GetXaxis()->SetTitle("dpy / MeV/c");
        fh_thetay_dpy_He->GetYaxis()->SetTitle("theta_y / mrad ");

        fh_thetax_dpy = new TH2F("tracker_thethax_dpy", "tracker theta_x vs dpy ", 2000, -200, 200, 200, -50., 50.);
        fh_thetax_dpy->GetXaxis()->SetTitle("dpy / MeV/c");
        fh_thetax_dpy->GetYaxis()->SetTitle("theta_x / mrad ");

        fh_thetay_dpx = new TH2F("tracker_thethay_dpx", "tracker theta_y vs dpx ", 2000, -200, 200, 200, -50., 50.);
        fh_thetay_dpx->GetXaxis()->SetTitle("dpx / MeV/c");
        fh_thetay_dpx->GetYaxis()->SetTitle("theta_y / mrad ");

        fh_x_dpx = new TH2F("tracker_px_dpx", "tracker px vs dpx ", 1000, -50, 50, 400, -500., 500.);
        fh_x_dpx->GetXaxis()->SetTitle("dpx / percent");
        fh_x_dpx->GetYaxis()->SetTitle("px / MeV/c");

        fh_y_dpy = new TH2F("tracker_py_dpy", "tracker py vs dpy ", 1000, -50, 50, 400, -500., 500.);
        fh_y_dpy->GetXaxis()->SetTitle("dpy / percent");
        fh_y_dpy->GetYaxis()->SetTitle("py / MeV/c");

        fh_phiMC_bc_cm = new TH1F("phiMC_bc_cm", "phiMC bc in cm-system", 360, 0., 360.);
        fh_phiMC_bc_cm->GetXaxis()->SetTitle("angle / degree");
        fh_phiMC_bc_cm->GetYaxis()->SetTitle("counts");

        fh_phiMC_bc_cm_polar = new TH2F("phiMC_bc_cm_polar", "phiMC_bc_cm_polar", 360, 0., 360., 100, 0., 10.);
        fh_phiMC_bc_cm_polar->GetXaxis()->SetTitle("angle / degree");
        fh_phiMC_bc_cm_polar->GetYaxis()->SetTitle("counts");

        fh_theta26_simu = new TH1F("theta26_simu", "theta 26 simulation", 500, 0., 5);
        fh_theta26_simu->GetXaxis()->SetTitle("angle / degree");
        fh_theta26_simu->GetYaxis()->SetTitle("counts");

        fh_minv_simu = new TH1F("minv_MC", "minv_MC ", 500, 14900, 14920);
        fh_minv_simu->GetXaxis()->SetTitle("minv / MeV");
        fh_minv_simu->GetYaxis()->SetTitle("counts");

        fh_psum_MC = new TH1F("sum_p_MC", "sum_p_MC", 2000, 16000., 18000.);
        fh_psum_MC->GetXaxis()->SetTitle("p / MeV/c");
        fh_psum_MC->GetYaxis()->SetTitle("count");

        fh_pzsum_MC = new TH1F("sum_pz_MC", "sum_pz_MC", 2000, 16000., 18000.);
        fh_pzsum_MC->GetXaxis()->SetTitle("pz / MeV/c");
        fh_pzsum_MC->GetYaxis()->SetTitle("count");

        fh_dtheta = new TH1F("dtheta26_cm", "delta theta ", 1000, -5., 5.);
        fh_dtheta->GetYaxis()->SetTitle("counts");
        fh_dtheta->GetXaxis()->SetTitle("Delta theta / deg");

        fh_Erel_vs_thetaMC = new TH2F("Erel_vs_thetaMC", "Erel vs. theta MC", 125, 0., 5, 200, 0, 20.);
        fh_Erel_vs_thetaMC->GetXaxis()->SetTitle("angle / deg");
        fh_Erel_vs_thetaMC->GetYaxis()->SetTitle("Erel / MeV");

        fh_Erel_vs_phibcMC = new TH2F("Erel_vs_phibccmMC", "Erel vs. phi_bc_cm MCtrack", 200, 0, 20., 400, 0., 400.);
        fh_Erel_vs_phibcMC->GetYaxis()->SetTitle("phi angle bc / deg");
        fh_Erel_vs_phibcMC->GetXaxis()->SetTitle("Erel / MeV");

        fh_Erel_simu = new TH1F("Erel_simu", "Erel simulation", 2000, 0., 20);
        fh_Erel_simu->GetXaxis()->SetTitle("Erel / MeV");
        fh_Erel_simu->GetYaxis()->SetTitle("counts");
    }
    fh_dErel = new TH1F("dErel", "delta Erel ", 400, 0, 1); // 400, -100, 100.);
    fh_dErel->GetYaxis()->SetTitle("counts");
    fh_dErel->GetXaxis()->SetTitle("Delta Erel / %");

    fh_thetax_px = new TH2F("thethax_px", "theta_x vs px ", 500, -500, 500, 200, -100., 100.);
    fh_thetax_px->GetXaxis()->SetTitle("px / MeV/c");
    fh_thetax_px->GetYaxis()->SetTitle("theta_x ");

    fh_thetay_py = new TH2F("thethay_py", "theta_y vs py ", 500, -500, 500, 200, -100., 100.);
    fh_thetay_py->GetXaxis()->SetTitle("py / MeV/c");
    fh_thetay_py->GetYaxis()->SetTitle("theta_y ");

    fh_thetax_py = new TH2F("thethax_py", "theta_x vs py ", 200, -100, 100, 200, -100., 100.);
    fh_thetax_py->GetXaxis()->SetTitle("py / MeV/c");
    fh_thetax_py->GetYaxis()->SetTitle("theta_x ");

    fh_thetay_px = new TH2F("thethay_px", "theta_y vs px ", 200, -100, 100, 200, -100., 100.);
    fh_thetay_px->GetXaxis()->SetTitle("px / MeV/c");
    fh_thetay_px->GetYaxis()->SetTitle("theta_y ");

    fh_px_px = new TH2F("px_px", "px He vs. px C", 400, -500, 500, 400, -500., 500.);
    fh_px_px->GetXaxis()->SetTitle("px / MeV/c");
    fh_px_px->GetYaxis()->SetTitle("px / MeV/c");

    fh_py_py = new TH2F("py_py", "py He vs. py C", 400, -500, 500, 400, -500., 500.);
    fh_py_py->GetXaxis()->SetTitle("py / MeV/c");
    fh_py_py->GetYaxis()->SetTitle("py / MeV/c");

    fh_pz_pz = new TH2F("pz_pz", "pz He vs. pz C", 400, 0, 10000, 400, 10000., 20000.);
    fh_pz_pz->GetXaxis()->SetTitle("pz / MeV/c");
    fh_pz_pz->GetYaxis()->SetTitle("pz / MeV/c");

    fh_p_p = new TH2F("p_p", "p He vs. p C", 400, 0, 10000, 400, 10000., 20000.);
    fh_p_p->GetXaxis()->SetTitle("p / MeV/c");
    fh_p_p->GetYaxis()->SetTitle("p / MeV/c");

    fh_px_px_nc = new TH2F("px_px_nc", "px He vs. px C all chi2", 400, -500, 500, 400, -500., 500.);
    fh_px_px_nc->GetXaxis()->SetTitle("px / MeV/c");
    fh_px_px_nc->GetYaxis()->SetTitle("px / MeV/c");

    fh_py_py_nc = new TH2F("py_py_nc", "py He vs. py C all chi2", 400, -500, 500, 400, -500., 500.);
    fh_py_py_nc->GetXaxis()->SetTitle("py / MeV/c");
    fh_py_py_nc->GetYaxis()->SetTitle("py / MeV/c");

    fh_pz_pz_nc = new TH2F("pz_pz_nc", "pz He vs. pz C all chi2", 400, 2000, 6000, 500, 10000., 20000.);
    fh_pz_pz_nc->GetXaxis()->SetTitle("pz / MeV/c");
    fh_pz_pz_nc->GetYaxis()->SetTitle("pz / MeV/c");

    // Analysis
    fh_chi2 = new TH1F("chi2", "chi2 ", 500, 0., 500);
    fh_chi2->GetXaxis()->SetTitle("Chi2");
    fh_chi2->GetYaxis()->SetTitle("counts");

    fh_chiy_vs_chix_nc = new TH2F("chiC_vs_chiHe", "chi2 C vs chi2 He", 1000, 0., 50000., 1000, 0., 50000.);
    fh_chiy_vs_chix_nc->GetXaxis()->SetTitle("chi2 He");
    fh_chiy_vs_chix_nc->GetYaxis()->SetTitle("chi2 C");

    fh_chiy_vs_chix = new TH2F("chiC_vs_chiHe_after_cut", "chi2 C vs chi2 He after cut", 50, 0., 500., 50, 0., 500.);
    fh_chiy_vs_chix->GetXaxis()->SetTitle("chi2 He");
    fh_chiy_vs_chix->GetYaxis()->SetTitle("chi2 C");

    fh_theta26 = new TH1F("theta26", "theta between alpha and carbon ", 125, 0., 5);
    fh_theta26->GetXaxis()->SetTitle("angle / degree");
    fh_theta26->GetYaxis()->SetTitle("counts");

    fh_theta26_nc = new TH1F("theta26_nc", "theta between alpha and carbon all chi2 ", 125, 0., 5);
    fh_theta26_nc->GetXaxis()->SetTitle("angle / degree");
    fh_theta26_nc->GetYaxis()->SetTitle("counts");

    fh_theta26_vs_chi = new TH2F("theta_vs_chi", "theta vs chi", 500, 0., 500., 125, 0., 5);
    fh_theta26_vs_chi->GetXaxis()->SetTitle("chi ");
    fh_theta26_vs_chi->GetYaxis()->SetTitle("angle / degree");

    fh_theta_16O = new TH1F("theta_16O", "theta of excited 16O ", 125, 0., 5);
    fh_theta_16O->GetXaxis()->SetTitle("angle / degree");
    fh_theta_16O->GetYaxis()->SetTitle("counts");

    fh_phi_16O = new TH1F("phi_16O", "phi of excited 16O ", 360, 0., 360);
    fh_phi_16O->GetXaxis()->SetTitle("angle / degree");
    fh_phi_16O->GetYaxis()->SetTitle("counts");

    fh_theta_bc_cm = new TH2F("theta_bc_cm", "Theta bc in cm-system", 180, 0., 180., 2000, 0, 20);
    fh_theta_bc_cm->GetXaxis()->SetTitle("angle / degree");
    fh_theta_bc_cm->GetYaxis()->SetTitle("Erel / MeV");

    fh_theta_bc_cm_nc = new TH1F("theta_bc_cm_nc", "Theta bc in cm-system all chi2", 180, 0., 180.);
    fh_theta_bc_cm_nc->GetXaxis()->SetTitle("angle / degree");
    fh_theta_bc_cm_nc->GetYaxis()->SetTitle("counts");

    fh_phi_bc_cm_nc = new TH1F("phi_bc_cm_nc", "phi bc in cm-system all chi2", 360, 0., 360.);
    fh_phi_bc_cm_nc->GetXaxis()->SetTitle("angle / degree");
    fh_phi_bc_cm_nc->GetYaxis()->SetTitle("counts");

    fh_phi_bc_cm = new TH1F("phi_bc_cm", "phi bc in cm-system", 360, 0., 360.);
    fh_phi_bc_cm->GetXaxis()->SetTitle("angle / degree");
    fh_phi_bc_cm->GetYaxis()->SetTitle("counts");

    fh_phi_bc_cm_polar = new TH2F("phi_bc_cm_polar", "phi_bc_cm_polar", 360, 0., 360., 100, 0., 10.);
    fh_phi_bc_cm_polar->GetXaxis()->SetTitle("angle / degree");
    fh_phi_bc_cm_polar->GetYaxis()->SetTitle("counts");

    fh_minv = new TH1F("minv", "minv ", 2000, 10, 30);
    fh_minv->GetXaxis()->SetTitle("minv / MeV");
    fh_minv->GetYaxis()->SetTitle("counts");

    fh_Erel = new TH1F("Erel", "Erel ", 2000, 0., 20);
    fh_Erel->GetXaxis()->SetTitle("Erel / MeV");
    fh_Erel->GetYaxis()->SetTitle("counts");

    fh_Erel_vs_chi = new TH2F("Erel_vs_chi", "Erel vs chi", 50, 0, 50, 300, 0., 30);
    fh_Erel_vs_chi->GetYaxis()->SetTitle("Erel / MeV");
    fh_Erel_vs_chi->GetXaxis()->SetTitle("chi2");

    fh_phi26_vs_chi = new TH2F("phi26_vs_chi", "phi26 vs chi", 50, 0, 50, 360, 0., 360);
    fh_phi26_vs_chi->GetYaxis()->SetTitle("phi / deg");
    fh_phi26_vs_chi->GetXaxis()->SetTitle("chi2");

    fh_psum_vs_theta26 = new TH2F("psum_vs_theta26", "psum_vs_theta26", 125, 0., 5, 600, 16000., 19000);
    fh_psum_vs_theta26->GetYaxis()->SetTitle("theta26 / deg");
    fh_psum_vs_theta26->GetXaxis()->SetTitle("psum / MeV/c");

    fh_Erel_nc = new TH1F("Erel_nc", "Erel all chi2", 2000, 0., 20);
    fh_Erel_nc->GetXaxis()->SetTitle("Erel / MeV");
    fh_Erel_nc->GetYaxis()->SetTitle("counts");

    fh_ErelL = new TH1F("ErelL", "Erel links", 2000, 0., 20);
    fh_ErelL->GetXaxis()->SetTitle("Erel / MeV");
    fh_ErelL->GetYaxis()->SetTitle("counts");

    fh_ErelR = new TH1F("ErelR", "Erel rechts ", 2000, 0., 20);
    fh_ErelR->GetXaxis()->SetTitle("Erel / MeV");
    fh_ErelR->GetYaxis()->SetTitle("counts");

    fh_ErelB = new TH1F("ErelB", "Erel both sides ", 2000, 0., 20);
    fh_ErelB->GetXaxis()->SetTitle("Erel / MeV");
    fh_ErelB->GetYaxis()->SetTitle("counts");

    fh_ErelB_nc = new TH1F("ErelB_nc", "Erel both sides all chi2", 2000, 0., 20);
    fh_ErelB_nc->GetXaxis()->SetTitle("Erel / MeV");
    fh_ErelB_nc->GetYaxis()->SetTitle("counts");

    fh_Erel_vs_psum = new TH2F("Erel_vs_psum", "Erel vs. psum", 500, 16000., 18000., 100, 0, 10.);
    fh_Erel_vs_psum->GetXaxis()->SetTitle("psum / MeV/c");
    fh_Erel_vs_psum->GetYaxis()->SetTitle(" Erel / MeV");

    fh_mass_nc = new TH2F("mass_nc", "mass He vs mass C all chi2", 200, 10, 20, 500, 0, 5);
    fh_mass_nc->GetXaxis()->SetTitle("mass C");
    fh_mass_nc->GetYaxis()->SetTitle(" mass H");

    fh_mass = new TH2F("mass", "mass He vs mass C selected chi2", 200, 10, 20, 500, 0, 5);
    fh_mass->GetXaxis()->SetTitle("mass C");
    fh_mass->GetYaxis()->SetTitle(" mass H");

    fh_energy_nc = new TH2F("energy_nc", "energy He vs energy C all chi2", 1000, 10, 20, 1000, 0, 10);
    fh_energy_nc->GetXaxis()->SetTitle("energy C / GeV");
    fh_energy_nc->GetYaxis()->SetTitle(" energy H / GeV");

    fh_energy = new TH2F("energy", "energy He vs energy C selected chi2", 1000, 10, 20, 1000, 0, 10);
    fh_energy->GetXaxis()->SetTitle("energy C / GeV");
    fh_energy->GetYaxis()->SetTitle(" energy H / GeV");

    fh_psum = new TH1F("sum_p", "ptot", 2000, 16000., 18000.);
    fh_psum->GetXaxis()->SetTitle("p / MeV/c");
    fh_psum->GetYaxis()->SetTitle("count");

    fh_pzsum = new TH1F("sum_pz", "pztot", 2000, 16000., 18000.);
    fh_pzsum->GetXaxis()->SetTitle("pz / MeV/c");
    fh_pzsum->GetYaxis()->SetTitle("count");

    fh_psum_nc = new TH1F("sum_p_nc", "ptot all chi2", 2000, 16000., 18000.);
    fh_psum_nc->GetXaxis()->SetTitle("p / MeV/c");
    fh_psum_nc->GetYaxis()->SetTitle("count");

    fh_psum_vs_chi = new TH2F("sum_p_vs_chi", "ptot vs chi2", 50, 0, 50, 400, 16800., 17800.);
    fh_psum_vs_chi->GetYaxis()->SetTitle("p / MeV/c");
    fh_psum_vs_chi->GetXaxis()->SetTitle("chi2");

    fh_psum_vs_event = new TH2F("psum_vs_event", "psum vs. event number", 100, 0, 1e6, 80, 17000., 17800.);
    fh_psum_vs_event->GetXaxis()->SetTitle("Event number");
    fh_psum_vs_event->GetYaxis()->SetTitle("p / MeV/c");

    fh_psum_vs_event_nc =
        new TH2F("psum_vs_event_nc", "psum vs. event number all chi2", 100, 0, 1e6, 80, 17000., 17800.);
    fh_psum_vs_event_nc->GetXaxis()->SetTitle("Event number");
    fh_psum_vs_event_nc->GetYaxis()->SetTitle("p / MeV/c");

    fh_Erel_vs_theta16O = new TH2F("Erel_vs_theta16O", "Erel vs. theta 16O*", 125, 0., 5, 2000, 0, 20.);
    fh_Erel_vs_theta16O->GetXaxis()->SetTitle("angle / deg");
    fh_Erel_vs_theta16O->GetYaxis()->SetTitle("Erel / MeV");

    fh_ErelB_vs_theta16O = new TH2F("ErelB_vs_theta16O", "ErelB vs. theta 16O*", 125, 0., 5, 2000, 0, 20.);
    fh_ErelB_vs_theta16O->GetXaxis()->SetTitle("angle / deg");
    fh_ErelB_vs_theta16O->GetYaxis()->SetTitle("ErelB / MeV");

    fh_Erel_vs_theta16O_3He12C =
        new TH2F("Erel_vs_theta16O_3He12C", "Erel vs. theta 16O* for 3He+12C", 125, 0., 5, 500, 0, 20.);
    fh_Erel_vs_theta16O_3He12C->GetXaxis()->SetTitle("angle / deg");
    fh_Erel_vs_theta16O_3He12C->GetYaxis()->SetTitle("Erel / MeV");

    fh_pHe_vs_theta26 = new TH2F("pHe_vs_theta", "theta vs pHe", 400, 2000, 6000, 200, 0, 100.);
    fh_pHe_vs_theta26->GetYaxis()->SetTitle("angle / mrad");
    fh_pHe_vs_theta26->GetXaxis()->SetTitle("p / MeV/c");

    fh_ErelB_vs_phibc_bg =
        new TH2F("ErelB_vs_phibccm_bg", "ErelB vs. phi_bc_cm below grazing", 2000, 0, 20., 400, 0., 400.);
    fh_ErelB_vs_phibc_bg->GetYaxis()->SetTitle("phi angle bc / deg");
    fh_ErelB_vs_phibc_bg->GetXaxis()->SetTitle("Erel / MeV");

    fh_ErelB_vs_theta26_bg = new TH2F("b", "ErelB vs. theta below grazing", 125, 0., 5, 2000, 0, 20.);
    fh_ErelB_vs_theta26_bg->GetXaxis()->SetTitle("angle / deg");
    fh_ErelB_vs_theta26_bg->GetYaxis()->SetTitle("Erel / MeV");

    fh_erelB_vs_ptransHe_bg =
        new TH2F("erelB_vs_ptransHe_bg", "ErelB vs. ptransHe below grazing", 2000, -1000, 1000, 150, 0., 15);
    fh_erelB_vs_ptransHe_bg->GetXaxis()->SetTitle("ptrans / MeV/c");
    fh_erelB_vs_ptransHe_bg->GetYaxis()->SetTitle("Erel / MeV");

    fh_erelB_vs_ptransC_bg =
        new TH2F("erelB_vs_ptransC_bg", "ErelB vs. ptransC below grazing", 2000, -1000, 1000, 150, 0., 15);
    fh_erelB_vs_ptransC_bg->GetXaxis()->SetTitle("ptrans / MeV/c");
    fh_erelB_vs_ptransC_bg->GetYaxis()->SetTitle("Erel / MeV");

    fh_thetaB_4He_cm_bg =
        new TH2F("thetaB_4He_cm_bg", "ThetaB 4He in cm-system below grazing", 200, -1., 1., 2000, 0, 20);
    fh_thetaB_4He_cm_bg->GetXaxis()->SetTitle("cos(theta_cm)");
    fh_thetaB_4He_cm_bg->GetYaxis()->SetTitle("ErelB / MeV");

    fh_phiB_4He_cm_bg = new TH2F("phiB_4He_cm_bg", "phiB 4He in cm-system below grazing", 360, 0., 360., 2000, 0, 20);
    fh_phiB_4He_cm_bg->GetXaxis()->SetTitle("angle / degree");
    fh_phiB_4He_cm_bg->GetYaxis()->SetTitle("Erel / MeV");

    fh_thetaB_12C_cm_bg =
        new TH2F("thetaB_12C_cm_bg", "ThetaB 12C in cm-system below grazing", 200, -1, 1., 2000, 0, 20);
    fh_thetaB_12C_cm_bg->GetXaxis()->SetTitle("cos(theta_cm)");
    fh_thetaB_12C_cm_bg->GetYaxis()->SetTitle("Erel / MeV");

    fh_phiB_12C_cm_bg = new TH2F("phiB_12C_cm_bg", "phiB 12C in cm-system below grazing", 360, 0., 360., 2000, 0, 20);
    fh_phiB_12C_cm_bg->GetXaxis()->SetTitle("angle / degree");
    fh_phiB_12C_cm_bg->GetYaxis()->SetTitle("Erel / MeV");

    fh_ErelB_vs_phibc_ag =
        new TH2F("ErelB_vs_phibccm_ag", "ErelB vs. phi_bc_cm above grazing", 2000, 0, 20., 400, 0., 400.);
    fh_ErelB_vs_phibc_ag->GetYaxis()->SetTitle("phi angle bc / deg");
    fh_ErelB_vs_phibc_ag->GetXaxis()->SetTitle("Erel / MeV");

    fh_ErelB_vs_theta26_ag = new TH2F("ErelB_vs_theta_ag", "ErelB vs. theta above grazing", 125, 0., 5, 2000, 0, 20.);
    fh_ErelB_vs_theta26_ag->GetXaxis()->SetTitle("angle / deg");
    fh_ErelB_vs_theta26_ag->GetYaxis()->SetTitle("Erel / MeV");

    fh_erelB_vs_ptransHe_ag =
        new TH2F("erelB_vs_ptransHe_ag", "ErelB vs. ptransHe above grazing", 2000, -1000, 1000, 150, 0., 15);
    fh_erelB_vs_ptransHe_ag->GetXaxis()->SetTitle("ptrans / MeV/c");
    fh_erelB_vs_ptransHe_ag->GetYaxis()->SetTitle("Erel / MeV");

    fh_erelB_vs_ptransC_ag =
        new TH2F("erelB_vs_ptransC_ag", "ErelB vs. ptransC above grazing", 2000, -1000, 1000, 150, 0., 15);
    fh_erelB_vs_ptransC_ag->GetXaxis()->SetTitle("ptrans / MeV/c");
    fh_erelB_vs_ptransC_ag->GetYaxis()->SetTitle("Erel / MeV");

    fh_thetaB_4He_cm_ag =
        new TH2F("thetaB_4He_cm_ag", "ThetaB 4He in cm-system above grazing", 200, -1., 1., 2000, 0, 20);
    fh_thetaB_4He_cm_ag->GetXaxis()->SetTitle("cos(theta_cm)");
    fh_thetaB_4He_cm_ag->GetYaxis()->SetTitle("ErelB / MeV");

    fh_phiB_4He_cm_ag = new TH2F("phiB_4He_cm_ag", "phiB 4He in cm-system above grazing", 360, 0., 360., 2000, 0, 20);
    fh_phiB_4He_cm_ag->GetXaxis()->SetTitle("angle / degree");
    fh_phiB_4He_cm_ag->GetYaxis()->SetTitle("Erel / MeV");

    fh_thetaB_12C_cm_ag =
        new TH2F("thetaB_12C_cm_ag", "ThetaB 12C in cm-system above grazing", 200, -1, 1., 2000, 0, 20);
    fh_thetaB_12C_cm_ag->GetXaxis()->SetTitle("cos(theta_cm)");
    fh_thetaB_12C_cm_ag->GetYaxis()->SetTitle("Erel / MeV");

    fh_phiB_12C_cm_ag = new TH2F("phiB_12C_cm_ag", "phiB 12C in cm-system above grazing", 360, 0., 360., 2000, 0, 20);
    fh_phiB_12C_cm_ag->GetXaxis()->SetTitle("angle / degree");
    fh_phiB_12C_cm_ag->GetYaxis()->SetTitle("Erel / MeV");

    fh_Erel_vs_phibc_bg =
        new TH2F("Erel_vs_phibccm_bg", "Erel vs. phi_bc_cm below grazing", 2000, 0, 20., 400, 0., 400.);
    fh_Erel_vs_phibc_bg->GetYaxis()->SetTitle("phi angle bc / deg");
    fh_Erel_vs_phibc_bg->GetXaxis()->SetTitle("Erel / MeV");

    fh_Erel_vs_theta26_bg = new TH2F("Erel_vs_theta_bg", "Erel vs. theta below grazing", 125, 0., 5, 2000, 0, 20.);
    fh_Erel_vs_theta26_bg->GetXaxis()->SetTitle("angle / deg");
    fh_Erel_vs_theta26_bg->GetYaxis()->SetTitle("Erel / MeV");

    fh_erel_vs_ptransHe_bg =
        new TH2F("erel_vs_ptransHe_bg", "ErelB vs. ptransHe below grazing", 2000, -1000, 1000, 150, 0., 15);
    fh_erel_vs_ptransHe_bg->GetXaxis()->SetTitle("ptrans / MeV/c");
    fh_erel_vs_ptransHe_bg->GetYaxis()->SetTitle("Erel / MeV");

    fh_erel_vs_ptransC_bg =
        new TH2F("erel_vs_ptransC_bg", "Erel vs. ptransC below grazing", 2000, -1000, 1000, 150, 0., 15);
    fh_erel_vs_ptransC_bg->GetXaxis()->SetTitle("ptrans / MeV/c");
    fh_erel_vs_ptransC_bg->GetYaxis()->SetTitle("Erel / MeV");

    fh_theta_4He_cm_bg = new TH2F("theta_4He_cm_bg", "Theta 4He in cm-system below grazing", 200, -1., 1., 2000, 0, 20);
    fh_theta_4He_cm_bg->GetXaxis()->SetTitle("cos(theta_cm)");
    fh_theta_4He_cm_bg->GetYaxis()->SetTitle("Erel / MeV");

    fh_phi_4He_cm_bg = new TH2F("phi_4He_cm_bg", "phi 4He in cm-system below grazing", 360, 0., 360., 2000, 0, 20);
    fh_phi_4He_cm_bg->GetXaxis()->SetTitle("angle / degree");
    fh_phi_4He_cm_bg->GetYaxis()->SetTitle("Erel / MeV");

    fh_theta_12C_cm_bg = new TH2F("theta_12C_cm_bg", "Theta 12C in cm-system below grazing", 200, -1, 1., 2000, 0, 20);
    fh_theta_12C_cm_bg->GetXaxis()->SetTitle("cos(theta_cm)");
    fh_theta_12C_cm_bg->GetYaxis()->SetTitle("Erel / MeV");

    fh_phi_12C_cm_bg = new TH2F("phi_12C_cm_bg", "phi 12C in cm-system below grazing", 360, 0., 360., 2000, 0, 20);
    fh_phi_12C_cm_bg->GetXaxis()->SetTitle("angle / degree");
    fh_phi_12C_cm_bg->GetYaxis()->SetTitle("Erel / MeV");

    fh_Erel_vs_phibc_ag =
        new TH2F("Erel_vs_phibccm_ag", "Erel vs. phi_bc_cm above grazing", 2000, 0, 20., 400, 0., 400.);
    fh_Erel_vs_phibc_ag->GetYaxis()->SetTitle("phi angle bc / deg");
    fh_Erel_vs_phibc_ag->GetXaxis()->SetTitle("Erel / MeV");

    fh_Erel_vs_theta26_ag = new TH2F("Erel_vs_theta_ag", "Erel vs. theta above grazing", 125, 0., 5, 2000, 0, 20.);
    fh_Erel_vs_theta26_ag->GetXaxis()->SetTitle("angle / deg");
    fh_Erel_vs_theta26_ag->GetYaxis()->SetTitle("Erel / MeV");

    fh_erel_vs_ptransHe_ag =
        new TH2F("erel_vs_ptransHe_ag", "Erel vs. ptransHe above grazing", 2000, -1000, 1000, 150, 0., 15);
    fh_erel_vs_ptransHe_ag->GetXaxis()->SetTitle("ptrans / MeV/c");
    fh_erel_vs_ptransHe_ag->GetYaxis()->SetTitle("Erel / MeV");

    fh_erel_vs_ptransC_ag =
        new TH2F("erel_vs_ptransC_ag", "Erel vs. ptransC above grazing", 2000, -1000, 1000, 150, 0., 15);
    fh_erel_vs_ptransC_ag->GetXaxis()->SetTitle("ptrans / MeV/c");
    fh_erel_vs_ptransC_ag->GetYaxis()->SetTitle("Erel / MeV");

    fh_theta_4He_cm_ag = new TH2F("theta_4He_cm_ag", "Theta 4He in cm-system above grazing", 200, -1., 1., 2000, 0, 20);
    fh_theta_4He_cm_ag->GetXaxis()->SetTitle("cos(theta_cm)");
    fh_theta_4He_cm_ag->GetYaxis()->SetTitle("Erel / MeV");

    fh_phi_4He_cm_ag = new TH2F("phi_4He_cm_ag", "phi 4He in cm-system above grazing", 360, 0., 360., 2000, 0, 20);
    fh_phi_4He_cm_ag->GetXaxis()->SetTitle("angle / degree");
    fh_phi_4He_cm_ag->GetYaxis()->SetTitle("Erel / MeV");

    fh_theta_12C_cm_ag = new TH2F("theta_12C_cm_ag", "Theta 12C in cm-system above grazing", 200, -1, 1., 2000, 0, 20);
    fh_theta_12C_cm_ag->GetXaxis()->SetTitle("cos(theta_cm)");
    fh_theta_12C_cm_ag->GetYaxis()->SetTitle("Erel / MeV");

    fh_phi_12C_cm_ag = new TH2F("phi_12C_cm_ag", "phi 12C in cm-system above grazing", 360, 0., 360., 2000, 0, 20);
    fh_phi_12C_cm_ag->GetXaxis()->SetTitle("angle / degree");
    fh_phi_12C_cm_ag->GetYaxis()->SetTitle("Erel / MeV");

    fh_psum_vs_theta26_nc = new TH2F("psum_vs_theta_nc", "theta vs psum nc", 125, 0., 5, 600, 16000., 19000);
    fh_psum_vs_theta26_nc->GetYaxis()->SetTitle("angle / deg");
    fh_psum_vs_theta26_nc->GetXaxis()->SetTitle("p / MeV/c");

    Int_t nbins = 100;
    Double_t binmin = 0, binmax = 10;
    for (Int_t det = 0; det < N_DET_MAX; det++)
    {
        if (det < 2)
        {
            nbins = 429;
            binmin = -6.006;
            binmax = 6.006;
        }
        else if (det > 1 && det < 6)
        {
            nbins = 120;
            binmin = -30;
            binmax = 30;
        }
        else
        {
            nbins = 240;   // 89;
            binmin = -60.; // 075;
            binmax = 60.;  // 075;
        }
        fh_Erel_vs_xdet[det] = new TH2F(Form("Erel_vs_x%s", fDetectorNames[det]),
                                        Form("Erel vs x%s", fDetectorNames[det]),
                                        nbins,
                                        binmin,
                                        binmax,
                                        200,
                                        0,
                                        20);

        fh_Erel_vs_ydet[det] = new TH2F(Form("Erel_vs_y%s", fDetectorNames[det]),
                                        Form("Erel vs y%s", fDetectorNames[det]),
                                        nbins,
                                        binmin,
                                        binmax,
                                        200,
                                        0,
                                        20);
    }
    fh_yfi23_vs_ytofd_bc = new TH2F("yfi23_vs_ytofd_bc", " yFi23b vs yTofd bc", 120, -60, 60, 429, -6.006, 6.006);
    fh_yfi23_vs_ytofd = new TH2F("yfi23_vs_ytofd", " yFi23b vs yTofd", 120, -60, 60, 429, -6.006, 6.006);
    fh_xfi23_vs_xtofd_bc =
        new TH2F("xfi23_vs_xtofd_bc", " xFi23b vs xTofd bc", 89, -60.075, 60.075, 429, -6.006, 6.006);
    fh_xfi23_vs_xtofd = new TH2F("xfi23_vs_xtofd", " xFi23b vs xTofd", 89, -60.075, 60.075, 429, -6.006, 6.006);
    fh_xy_fib23_bc = new TH2F("xy_fi23_bc", " xFi23b vs yfi23 bc", 429, -6.006, 6.006, 429, -6.006, 6.006);
    fh_xy_fib23 = new TH2F("xy_fi23", " xFi23b vs yfi23", 429, -6.006, 6.006, 429, -6.006, 6.006);
    fh_dt_fib23_bc = new TH1F("dt_fib23_bc", " tFi23b - tfi23a bc", 120, -30, 30);

    if (fHitItemsCalifa)
    {
        fh_Erel_withCalifa_barrel = new TH1F("Erel_withCalifa_barrel", "Erel with is_califa_barrel", 2000, 0., 20);
        fh_Erel_withCalifa_barrel->GetXaxis()->SetTitle("Erel / MeV");
        fh_Erel_withCalifa_barrel->GetYaxis()->SetTitle("counts");

        fh_Erel_withCalifa_2d_barrel =
            new TH2F("Erel_vs_numCrystalCalifa_barrel", "Erel vs Numb of Crystal_barrel", 40, 0, 40, 200, 0., 20);
        fh_Erel_withCalifa_2d_barrel->GetYaxis()->SetTitle("Erel / MeV");
        fh_Erel_withCalifa_2d_barrel->GetXaxis()->SetTitle("No crystals in cluster");

        fh_califaenergy_2d_barrel = new TH2F(
            "Ecalifa_vs_numCrystalCalifa_barrel", "Ecalifa vs Numb of Crystal_barrel", 40, 0, 40, 1000, 0., 10000.);
        fh_califaenergy_2d_barrel->GetYaxis()->SetTitle("Ecalifa / keV");
        fh_califaenergy_2d_barrel->GetXaxis()->SetTitle("No crystals in cluster");

        fh_Erel_vs_theta16O_withcalifa_barrel = new TH2F(
            "Erel_vs_theta16O_withcalifa_barrel", "Erel vs. theta 16O* with califa_barrel", 125, 0., 5, 200, 0, 20.);
        fh_Erel_vs_theta16O_withcalifa_barrel->GetXaxis()->SetTitle("angle / deg");
        fh_Erel_vs_theta16O_withcalifa_barrel->GetYaxis()->SetTitle("Erel / MeV");

        fh_califa_hitenergy_ag_barrel = new TH2F("califa_hit_energy_vs_erel_ag_barrel",
                                                 "Califa hitE_barrel vs Erel above grazing",
                                                 2000,
                                                 0,
                                                 20,
                                                 1000,
                                                 0.,
                                                 10000.);
        fh_califa_hitenergy_ag_barrel->GetYaxis()->SetTitle("Califa energy / keV");
        fh_califa_hitenergy_ag_barrel->GetXaxis()->SetTitle("Erel / MeV");

        fh_califa_hitenergy_bg_barrel = new TH2F("califa_hit_energy_vs_erel_bg_barrel",
                                                 "Califa hitE_barrel vs Erel below grazing",
                                                 2000,
                                                 0,
                                                 20,
                                                 1000,
                                                 0.,
                                                 10000.);
        fh_califa_hitenergy_bg_barrel->GetYaxis()->SetTitle("Califa energy / keV");
        fh_califa_hitenergy_bg_barrel->GetXaxis()->SetTitle("Erel / MeV");

        fh_califa_hitenergy_boost_barrel =
            new TH1F("hit_califa_energy_DC_barrel", "Califa hitE_barrel is_track true DopCorr", 1000, 0., 10000.);
        fh_califa_hitenergy_boost_barrel->GetYaxis()->SetTitle("Counts");
        fh_califa_hitenergy_boost_barrel->GetXaxis()->SetTitle("Energy / keV");

        fh_Erel_withCalifa_iphos = new TH1F("Erel_withCalifa_iphos", "Erel with is_califa_iphos", 2000, 0., 20);
        fh_Erel_withCalifa_iphos->GetXaxis()->SetTitle("Erel / MeV");
        fh_Erel_withCalifa_iphos->GetYaxis()->SetTitle("counts");

        fh_Erel_withCalifa_2d_iphos =
            new TH2F("Erel_vs_numCrystalCalifa_iphos", "Erel vs Numb of Crystal_iphos", 40, 0, 40, 200, 0., 20);
        fh_Erel_withCalifa_2d_iphos->GetYaxis()->SetTitle("Erel / MeV");
        fh_Erel_withCalifa_2d_iphos->GetXaxis()->SetTitle("No crystals in cluster");

        fh_califaenergy_2d_iphos = new TH2F(
            "Ecalifa_vs_numCrystalCalifa_iphos", "Ecalifa vs Numb of Crystal_iphos", 40, 0, 40, 1000, 0., 10000.);
        fh_califaenergy_2d_iphos->GetYaxis()->SetTitle("Ecalifa / keV");
        fh_califaenergy_2d_iphos->GetXaxis()->SetTitle("No crystals in cluster");

        fh_Erel_vs_theta16O_withcalifa_iphos = new TH2F(
            "Erel_vs_theta16O_withcalifa_iphos", "Erel vs. theta 16O* with califa_iphos", 125, 0., 5, 200, 0, 20.);
        fh_Erel_vs_theta16O_withcalifa_iphos->GetXaxis()->SetTitle("angle / deg");
        fh_Erel_vs_theta16O_withcalifa_iphos->GetYaxis()->SetTitle("Erel / MeV");

        fh_califa_hitenergy_ag_iphos = new TH2F("califa_hit_energy_vs_erel_ag_iphos",
                                                "Califa hitE_iphos vs Erel above grazing",
                                                2000,
                                                0,
                                                20,
                                                1000,
                                                0.,
                                                10000.);
        fh_califa_hitenergy_ag_iphos->GetYaxis()->SetTitle("Califa energy / keV");
        fh_califa_hitenergy_ag_iphos->GetXaxis()->SetTitle("Erel / MeV");

        fh_califa_hitenergy_bg_iphos = new TH2F("califa_hit_energy_vs_erel_bg_iphos",
                                                "Califa hitE_iphos vs Erel below grazing",
                                                2000,
                                                0,
                                                20,
                                                1000,
                                                0.,
                                                10000.);
        fh_califa_hitenergy_bg_iphos->GetYaxis()->SetTitle("Califa energy / keV");
        fh_califa_hitenergy_bg_iphos->GetXaxis()->SetTitle("Erel / MeV");

        fh_califa_hitenergy_boost_iphos =
            new TH1F("hit_califa_energy_DC_iphos", "Califa hitE_iphos is_track true DopCorr", 1000, 0., 10000.);
        fh_califa_hitenergy_boost_iphos->GetYaxis()->SetTitle("Counts");
        fh_califa_hitenergy_boost_iphos->GetXaxis()->SetTitle("Energy / keV");

        fh_Erel_withCalifa_tof = new TH2F("Erel_vs_tofCalifa", "Erel vs tof Califa", 1500, 1000., 4000, 200, 0., 20);
        fh_Erel_withCalifa_tof->GetYaxis()->SetTitle("Erel / MeV");
        fh_Erel_withCalifa_tof->GetXaxis()->SetTitle("tof / ns");

        fh_Erel_withCalifa_motherId =
            new TH2F("Erel_vs_motherId", "Erel vs motherId Califa", 1600, 900., 2500, 200, 0., 20);
        fh_Erel_withCalifa_motherId->GetYaxis()->SetTitle("Erel / MeV");
        fh_Erel_withCalifa_motherId->GetXaxis()->SetTitle("motherId");

        fh_theta_vs_theta = new TH2F("theta_vs_theta", "Califa theta vs 16O* theta", 125, 0, 5, 360, -180, 180);
        fh_theta_vs_theta->GetYaxis()->SetTitle("theta Califa / deg");
        fh_theta_vs_theta->GetXaxis()->SetTitle("theta 16O* / deg");

        fh_califa_hitenergy_select =
            new TH1F("hit_califa_energy_select", "Califa hitE if is_track true", 1000, 0., 10000.);
        fh_califa_hitenergy_select->GetYaxis()->SetTitle("Counts");
        fh_califa_hitenergy_select->GetXaxis()->SetTitle("Energy / keV");

        fh_califa_tofd = new TH2F("fh_califa_tofd", "Califa time vs tofd time", 1500, 1000., 4000., 20000, -1000, 1000);
        fh_califa_tofd->GetYaxis()->SetTitle("tofd time / ns");
        fh_califa_tofd->GetXaxis()->SetTitle("califa tome / ns");
    }
    if (fvis)
    {
        if (fPairs)
        {
            TCanvas* check1 = new TCanvas("SelectedChi2", "SelectedChi2", 10, 10, 900, 900);
            check1->Divide(4, 3);
            check1->cd(1);
            gPad->SetLogz();
            fh_px_px->Draw("colz");
            check1->cd(2);
            gPad->SetLogz();
            fh_py_py->Draw("colz");
            check1->cd(3);
            gPad->SetLogz();
            fh_pz_pz->Draw("colz");
            check1->cd(4);
            fh_p_He->Draw();
            check1->cd(5);
            fh_p_C->Draw();
            check1->cd(6);
            fh_psum->Draw();
            check1->cd(7);
            fh_Erel->Draw();
            check1->cd(8);
            fh_theta26->Draw();
            check1->cd(9);
            fh_Erel_vs_theta26_bg->Draw("colz");
            check1->cd(10);
            fh_erel_vs_ptransHe_bg->Draw("colz");
            check1->cd(11);
            fh_Erel_vs_theta16O->Draw("colz");
            check1->cd(12);
            fh_Erel_vs_phibc_bg->Draw("colz");
        }
        else
        {
            TCanvas* check = new TCanvas("SelectedChi2", "SelectedChi2", 10, 10, 900, 900);
            check->Divide(4, 4);
            check->cd(1);
            fh_chi2->Draw();
            check->cd(2);
            gPad->SetLogz();
            fh_psum_vs_chi->Draw("colz");
            check->cd(3);
            gPad->SetLogz();
            fh_target_xy_nc->Draw("colz");
            check->cd(4);
            gPad->SetLogz();
            fh_psum_vs_event_nc->Draw("colz");
            check->cd(5);
            fh_p_O_nc->Draw();
            check->cd(6);
            fh_px_O_nc->Draw();
            check->cd(7);
            fh_py_O_nc->Draw();
            check->cd(8);
            fh_pz_O_nc->Draw();
            check->cd(11);
            gPad->SetLogz();
            fh_target_xy->Draw("colz");
            check->cd(12);
            gPad->SetLogz();
            fh_psum_vs_event->Draw("colz");
            check->cd(13);
            fh_p_O->Draw();
            check->cd(14);
            fh_px_O->Draw();
            check->cd(15);
            fh_py_O->Draw();
            check->cd(16);
            fh_pz_O->Draw();
        }

        if (fSimu)
        {
            TCanvas* checkMC = new TCanvas("CheckingGlobalMC", "CheckingGlobalMC", 10, 10, 1200, 900);

            if (fPairs)
            {
                checkMC->Divide(4, 3);
                checkMC->cd(1);
                fh_dx->Draw();
                checkMC->cd(2);
                fh_dy->Draw();
                checkMC->cd(3);
                fh_dp->Draw();
                checkMC->cd(4);
                gPad->SetLogz();
                fh_dpy_dpx->Draw("colz");
                checkMC->cd(5);
                fh_x_dpx->Draw("colz");
                checkMC->cd(6);
                fh_y_dpy->Draw("colz");
                checkMC->cd(7);
                checkMC->cd(8);
                fh_target_xy->Draw("colz");
                checkMC->cd(9);
                fh_target_xy_MC->Draw("colz");
                checkMC->cd(10);
                gPad->SetLogz();
                fh_thetay_dpy_C->Draw("colz");
                checkMC->cd(11);
                gPad->SetLogz();
                fh_thetax_dpx_He->Draw("colz");
                checkMC->cd(12);
                gPad->SetLogz();
                fh_thetay_dpy_He->Draw("colz");
            }
            else
            {
                checkMC->Divide(3, 3);
                checkMC->cd(1);
                fh_dx->Draw();
                checkMC->cd(2);
                fh_dy->Draw();
                checkMC->cd(3);
                fh_dpxO->Draw();
                checkMC->cd(4);
                fh_dpyO->Draw();
                checkMC->cd(5);
                fh_dpzO->Draw();
                checkMC->cd(6);
                fh_dpO->Draw();
                checkMC->cd(7);
                gPad->SetLogz();
                fh_dpy_dpx->Draw("colz");
            }
        }
    } // for fvis

    return kSUCCESS;
}

void R3BGlobalAnalysisS494::Exec(Option_t* option)
{
    if (fNEvents / 100000. == (int)fNEvents / 100000)
        std::cout << "\rEvents: " << fNEvents << " / " << maxevent << " (" << (int)(fNEvents * 100. / maxevent)
                  << " %) " << std::flush;

    // cout << "New event " << fNEvents << endl;

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    if (header)
    {
        timeTS = header->GetTimeStamp();
        eventTS = header->GetEventno();
    }
    //  cout<<"header time eventnum and time: "<<eventTS<<", "<<timeTS<<endl;

    pHex = -100.;
    pHey = -100.;
    pHez = -100.;
    pCx = -100.;
    pCy = -100.;
    pCz = -100.;
    XHe = -100;
    YHe = -100;
    ZHe = -100;
    XC = -100;
    YC = -100;
    ZC = -100;
    mtrackHe = -100;
    mtrackC = -100;
    pHex_mc = -100.;
    pHey_mc = -100.;
    pHez_mc = -100.;
    pCx_mc = -100.;
    pCy_mc = -100.;
    pCz_mc = -100.;
    XHe_mc = -100;
    YHe_mc = -100;
    ZHe_mc = -100;
    XC_mc = -100;
    YC_mc = -100;
    ZC_mc = -100;
    Double_t px, py, pz;
    Double_t theta_16O = 0., theta_26 = 0.;
    Double_t costh26 = 0.;
    Double_t chiHex = 1.e+36, chiHey = 1.e+36, chiCx = 1.e+36, chiCy = 1.e+36;
    Double_t chix = 1.e+36, chiy = 1.e+36;
    pbeam.SetXYZ(0, 0, 17391.5);

    Double_t beta_beam = 0.7593209;
    Double_t gamma_beam = 1.536768;

    Bool_t is_alpha = false;
    Bool_t is_carbon = false;
    Bool_t is_oxygen = false;
    Bool_t is_tracked = false;

    Int_t nTracksFound = 0;
    Int_t nHitsTrack = 0;
    Int_t nHitsMCTrack = 0;

    vector<Double_t> posdetHe;
    vector<Double_t> posdetC;

    Double_t tHetofd = -10000., tCtofd = -10000.;

    if (fTrack)
    {

        // read in Track parameter
        nHitsTrack = fTrack->GetEntriesFast();

        //  if(nHitsTrack < 1) return;

        // cout << "Track hits: " << nHitsTrack << endl;
        for (Int_t l = 0; l < nHitsTrack; l++)
        {
            R3BTrackData* aTrack = (R3BTrackData*)fTrack->At(l);

            if (aTrack->GetQ() == 2)
            {
                // 4He
                XHe = aTrack->GetX(); // cm
                YHe = aTrack->GetY(); // cm
                ZHe = aTrack->GetZ(); // cm

                pHex = aTrack->GetPx(); //  + ((std::rand() / (float)RAND_MAX) - 0.5)/100.* aTrack->GetPx(); // MeV/c
                pHey = aTrack->GetPy(); // + ((std::rand() / (float)RAND_MAX) - 0.5)/100.* aTrack->GetPx(); // MeV/c
                pHez = aTrack->GetPz(); // MeV/c
                mtrackHe = aTrack->GetAoZ() * aTrack->GetQ();

                chiHex = aTrack->GetChix();
                chiHey = aTrack->GetChiy();
                posdetHe = aTrack->GetDetPos();
                tHetofd = aTrack->GetTime();

                is_alpha = true;
                alpha.SetPxPyPzE(pHex, pHey, pHez, sqrt(pow(pHex, 2) + pow(pHey, 2) + pow(pHez, 2) + pow(mHe, 2)));
                if (mtrackHe > 2.4 && mtrackHe < 3.6)
                    helium3.SetPxPyPzE(
                        pHex, pHey, pHez, sqrt(pow(pHex, 2) + pow(pHey, 2) + pow(pHez, 2) + pow(m3He, 2)));

                LOG(debug) << "******************************************" << endl;
                LOG(debug) << "Track In 4He"
                           << "x " << XHe << " y " << YHe << " z " << ZHe << endl;
                LOG(debug) << "px " << pHex << " py " << pHey << " z " << pHez << endl;
                LOG(debug) << "chiHex " << chiHex << ", " << chiHey << endl;
            }
            if (aTrack->GetQ() == 6)
            {
                // 12C
                XC = aTrack->GetX();
                YC = aTrack->GetY();
                ZC = aTrack->GetZ();
                mtrackC = aTrack->GetAoZ() * aTrack->GetQ();

                pCx = aTrack->GetPx(); // + ((std::rand() / (float)RAND_MAX) - 0.5)/100.* aTrack->GetPx();
                pCy = aTrack->GetPy(); // + ((std::rand() / (float)RAND_MAX) - 0.5)/100.* aTrack->GetPx();
                pCz = aTrack->GetPz();

                chiCx = aTrack->GetChix();
                chiCy = aTrack->GetChiy();

                posdetC = aTrack->GetDetPos();
                tCtofd = aTrack->GetTime();

                is_carbon = true;
                carbon.SetPxPyPzE(pCx, pCy, pCz, sqrt(pow(pCx, 2) + pow(pCy, 2) + pow(pCz, 2) + pow(mC, 2)));

                LOG(debug) << "******************************************" << endl;
                LOG(debug) << "Track In 12C"
                           << "x " << XC << " y " << YC << " z " << ZC << endl;
                LOG(debug) << "px " << pCx << " py " << pCy << " z " << pCz << endl;
                LOG(debug) << "chiCx " << chiCx << ", " << chiCy << endl;
            }
            if (aTrack->GetQ() == 8)
            {
                // 160
                Xf = aTrack->GetX();
                Yf = aTrack->GetY();
                Zf = aTrack->GetZ();

                Pxf = aTrack->GetPx();
                Pyf = aTrack->GetPy();
                Pzf = aTrack->GetPz();
                is_oxygen = true;
                Pf_tot = sqrt((Pxf * Pxf) + (Pyf * Pyf) + (Pzf * Pzf));

                chix = aTrack->GetChix();
                chiy = aTrack->GetChiy();

                LOG(debug) << "******************************************" << endl;
                LOG(debug) << "Track In 16O"
                           << "x " << Xf << " y " << Yf << " z " << Zf << endl;
                LOG(debug) << "px " << Pxf << " py " << Pyf << " z " << Pzf << endl;
            }
        }
    }
    if (fMCTrack && fSimu)
    {
        // read in Monte Carlo Track parameter

        nHitsMCTrack = fMCTrack->GetEntriesFast();
        //  cout << "MCTrack hits: " << nHitsMCTrack << endl;

        for (Int_t l = 0; l < nHitsMCTrack; l++)
        {
            //  cout << "Original MC Data is analyzed" << endl;
            R3BMCTrack* aTrack = (R3BMCTrack*)fMCTrack->At(l);

            Int_t PID = aTrack->GetPdgCode();
            Int_t mother = aTrack->GetMotherId();
            LOG(debug) << "PID " << PID << endl;
            if (mother < 0)
            {
                if (PID == 1000020040)
                {
                    // 4He
                    XHe_mc = aTrack->GetStartX(); // cm
                    YHe_mc = aTrack->GetStartY(); // cm
                    ZHe_mc = aTrack->GetStartZ(); // cm
                    THe_mc = aTrack->GetStartT(); // cm

                    pHex_mc = aTrack->GetPx() * 1000.; // MeV/c
                    pHey_mc = aTrack->GetPy() * 1000.;
                    pHez_mc = aTrack->GetPz() * 1000.;

                    is_alpha = true;
                    alphaMC.SetPxPyPzE(pHex_mc,
                                       pHey_mc,
                                       pHez_mc,
                                       sqrt(pow(pHex_mc, 2) + pow(pHey_mc, 2) + pow(pHez_mc, 2) + pow(mHe, 2)));

                    LOG(debug) << "******************************************" << endl;
                    LOG(debug) << "MC Track In 4He"
                               << "x " << XHe_mc << " y " << YHe_mc << " z " << ZHe_mc << endl;
                    LOG(debug) << "px " << pHex_mc << " py " << pHey_mc << " pz " << pHez_mc << endl;
                }
                if (PID == 1000060120)
                {
                    // 12C
                    XC_mc = aTrack->GetStartX();
                    YC_mc = aTrack->GetStartY();
                    ZC_mc = aTrack->GetStartZ();
                    TC_mc = aTrack->GetStartT();

                    pCx_mc = aTrack->GetPx() * 1000.;
                    pCy_mc = aTrack->GetPy() * 1000.;
                    pCz_mc = aTrack->GetPz() * 1000.;

                    is_carbon = true;
                    carbonMC.SetPxPyPzE(
                        pCx_mc, pCy_mc, pCz_mc, sqrt(pow(pCx_mc, 2) + pow(pCy_mc, 2) + pow(pCz_mc, 2) + pow(mC, 2)));

                    //  LOG(debug) << "******************************************" << endl;
                    //  LOG(debug) << "MC Track In 12C"
                    //             << "x " << XC_mc << " y " << YC_mc << " z " << ZC_mc << endl;
                    //  LOG(debug) << "px " << pCx_mc << " py " << pCy_mc << " pz " << pCz_mc << endl;
                }
                if (PID == 1000080160)
                {
                    // 16O
                    Xf_mc = aTrack->GetStartX();
                    Yf_mc = aTrack->GetStartY();
                    Zf_mc = aTrack->GetStartZ();

                    Pxf_mc = aTrack->GetPx() * 1000.;
                    Pyf_mc = aTrack->GetPy() * 1000.;
                    Pzf_mc = aTrack->GetPz() * 1000.;

                    is_oxygen = true;
                    Pf_tot_mc = sqrt((Pxf_mc * Pxf_mc) + (Pyf_mc * Pyf_mc) + (Pzf_mc * Pzf_mc));

                    LOG(debug) << "******************************************" << endl;
                    LOG(debug) << "MC Track In 16O"
                               << "x " << Xf_mc << " y " << Yf_mc << " z " << Zf_mc << endl;
                    LOG(debug) << "px " << Pxf_mc << " py " << Pyf_mc << " z " << Pzf_mc << endl;
                }
            }
        }
    }

    // new version of analysis
    if (fTrack && is_carbon && is_alpha && fPairs && chiHex < 1.e+36) // 17364.240
    {
        fNeventstrack += 1;

        // (0,1) = (xfi23a,yfi23a), .. (12,13) = (xtofd,ytofd)
        fh_xy_fib23_bc->Fill(posdetHe.at(0), posdetHe.at(3));
        fh_xy_fib23_bc->Fill(posdetC.at(0), posdetC.at(3));
        fh_xfi23_vs_xtofd_bc->Fill(posdetHe.at(12), posdetHe.at(0));
        fh_xfi23_vs_xtofd_bc->Fill(posdetC.at(12), posdetC.at(0));
        fh_yfi23_vs_ytofd_bc->Fill(posdetHe.at(13), posdetHe.at(3));
        fh_yfi23_vs_ytofd_bc->Fill(posdetC.at(13), posdetC.at(3));

        Double_t sq_chi = sqrt(chiHex * chiHex + chiCx * chiCx);
        fh_chi2->Fill(sq_chi);
        fh_chiy_vs_chix_nc->Fill(chiHex, chiCx);

        LOG(debug) << "Entering Pair analysis***" << endl;
        // if (chiHex < fcut_chiX && chiHey < fcut_chiY && chiCx < fcut_chiX && chiCy < fcut_chiY)
        //		&&((alpha.Px() > 0 && carbon.Px() < 0) || (alpha.Px() < 0 && carbon.Px() > 0)))
        if (alpha.Pz() > 0 && carbon.Pz() > 0)
        {
            //   if(abs(XHe-XC)*100. > 0.03) return;
            //   if(abs(YHe-YC)*100. > 0.03) return;

            //  if(abs(XHe-XC)*100. > 0.2) return;
            //  if(abs(YHe-YC)*100. > 0.2) return;

            pa = alpha.Vect();
            pc = carbon.Vect();

            // Calculate angle between alpha and C
            if (alpha.Pz() == 0 || carbon.Pz() == 0)
                return;
            /*
                        if (alpha.Pz() < 3900 || alpha.Pz() > 4800)
                           return;
                       if (carbon.Pz() < 12600 || carbon.Pz() > 13500)
                           return;
           */
            theta_26 = alpha.Angle(carbon.Vect()) * TMath::RadToDeg(); // angle alpha carbon (theta)
            Double_t phi_26 = (alpha.Phi() - carbon.Phi()) * TMath::RadToDeg();

            // if(theta_26 < 0.5) return;

            /* * Calculate invariant mass and relative energy * */
            m_inva = (alpha + carbon).M(); // invariant mass

            Erel = m_inva - mHe - mC; // relative Energy

            /*
                        Double_t costh26_test = (alpha.Px() * carbon.Px() + alpha.Py() * carbon.Py() + alpha.Pz() *
               carbon.Pz()) / (sqrt(alpha.Px() * alpha.Px() + alpha.Py() * alpha.Py() + alpha.Pz() * alpha.Pz()) *
                               sqrt(carbon.Px() * carbon.Px() + carbon.Py() * carbon.Py() + carbon.Pz() * carbon.Pz()));

                        Double_t test_theta = acos(costh26_test) * TMath::RadToDeg(); // opening angle
                        Double_t m_inv_test = sqrt(mHe * mHe + mC * mC + 2. * sqrt(pc.Mag() * pc.Mag() + mC * mC) *
                                              sqrt(pa.Mag() * pa.Mag() + mHe * mHe) -
                                              2. * pa.Mag() * pc.Mag() * cos(test_theta * TMath::DegToRad()));

                        cout<<"Checking minv: "<<m_inva<<", "<<m_inv_test<<endl;
            */

            /* * transfer to cm system and rotate * */

            oxygen = alpha + carbon; // excited oxygen
            poexc = oxygen.Vect();

            // cout<<"gamma "<<oxygen.Gamma()<<endl;
            theta_16O = oxygen.Theta() * TMath::RadToDeg();
            Double_t phi_16O = oxygen.Phi() * TMath::RadToDeg();
            if (phi_16O < 0)
                phi_16O += 360.;

            // photon.SetPxPyPzE(oxygen.Px, oxygen.Py, 0., sqrt(pow(oxygen.Px, 2) + pow(oxygen.Py, 2)));

            // transfer to cm system and make some rotations
            // boost them to centre of mass
            alpha_cm = alpha;
            carbon_cm = carbon;
            oxygen_cm = oxygen;
            alpha_cm.Boost(-oxygen.BoostVector());
            carbon_cm.Boost(-oxygen.BoostVector());
            oxygen_cm.Boost(-oxygen.BoostVector());

            pa_cm = alpha_cm.Vect();
            pc_cm = carbon_cm.Vect();

            phi_bc_cm = (oxygen.Phi() - alpha_cm.Phi()) * TMath::RadToDeg();
            if (phi_bc_cm < 0)
                phi_bc_cm += 360.;

            TVector3 norm;
            Double_t normx = (-1.) * pbeam.Z() * poexc.Y();
            Double_t normy = (-1.) * pbeam.Z() * poexc.X();
            norm.SetXYZ(normx, normy, 0); // vector perpendicular to (pbeam,p16O*)
            Double_t thetanorm_alpha = pa_cm.Theta();
            Double_t thetanorm_carbon = pc_cm.Theta();
            Double_t pinplaneHe = pa_cm.Perp(norm); // project to plane defined by pbeam and p16O*
            Double_t pinplaneC = pc_cm.Perp(norm);  // project to plane defined by pbeam and p16O*
            Double_t ptransHe = pinplaneHe * pa_cm.Phi();
            Double_t ptransC = pinplaneC * pc_cm.Phi();

            // cout << " px: " << alpha_cm.Px() + carbon_cm.Px() << "  py:  " << alpha_cm.Py() + carbon_cm.Py()
            //     << "  pz:  " << alpha_cm.Pz() + carbon_cm.Pz() << endl;
            /*
                        cout << "lab " << alpha.Px() << "  " << alpha.Py() << "  " << alpha.Pz() << endl;
                        cout << "lab " << carbon.Px() << "  " << carbon.Py() << "  " << carbon.Pz() << endl;
                        cout << "lab " << oxygen.Px() << "  " << oxygen.Py() << "  " << oxygen.Pz() << endl;

                        cout << "cm " << alpha_cm.Px() << "  " << alpha_cm.Py() << "  " << alpha_cm.Pz() << endl;
                        cout << "cm " << carbon_cm.Px() << "  " << carbon_cm.Py() << "  " << carbon_cm.Pz() << endl;
                        cout << "cm " << oxygen_cm.Px() << "  " << oxygen_cm.Py() << "  " << oxygen_cm.Pz() << endl;
            */

            Double_t tthetaxC = pCx / pCz;    // * 1000.;
            Double_t tthetayC = pCy / pCz;    // * 1000.;
            Double_t tthetaxHe = pHex / pHez; // * 1000.;
            Double_t tthetayHe = pHey / pHez; // * 1000.;

            Double_t thetaxC = atan(tthetaxC) * 1000.;
            Double_t thetayC = atan(tthetayC) * 1000.;
            Double_t thetaxHe = atan(tthetaxHe) * 1000.;
            Double_t thetayHe = atan(tthetayHe) * 1000.;

            Double_t Ea = mHe * sqrt(1. + (pa.Mag() / mHe) * (pa.Mag() / mHe)) / 1000.; // in GeV
            Double_t Ec = mC * sqrt(1. + (pc.Mag() / mC) * (pc.Mag() / mC)) / 1000.;    // in GeV
            /*
                   cout<<setprecision(10)<<"12C einzelne: "<<pCx<<" - "<<carbon.Px()<<"; "<<pCy<<" - "<<carbon.Py()<<";
               "<<pCz<<" - "<<carbon.Pz()<<endl; cout<<setprecision(10)<<"12C total: "<<pc.Mag()<<" -
               "<<sqrt(pCx*pCx+pCy*pCy+pCz*pCz)<<endl; cout<<setprecision(10)<<"4He einzelne: "<<pHex<<" -
               "<<alpha.Px()<<"; "<<pHey<<" - "<<alpha.Py()<<"; "<<pHez<<" - "<<alpha.Pz()<<endl;
                   cout<<setprecision(10)<<"4He total: "<<pa.Mag()<<" - "<<sqrt(pHex*pHex+pHey*pHey+pHez*pHez)<<endl;
                   cout<<setprecision(10)<<"Sum of two: "<<(pa+pc).Mag()<<",
               "<<sqrt((pHex+pCx)*(pHex+pCx)+(pHey+pCy)*(pHey+pCy)+(pHez+pCz)*(pHez+pCz))<<", "<<
                                       sqrt(pa.Mag()*pa.Mag()+pc.Mag()*pc.Mag()+2. * pa.Mag() * pc.Mag() * cos(theta_26
               * TMath::DegToRad()))<<endl;
             */

            fh_target_xy_nc->Fill(XHe, YHe);
            fh_target_xy_nc->Fill(XC, YC);

            fh_mass_nc->Fill(mtrackC, mtrackHe);
            fh_energy_nc->Fill(Ec, Ea);
            fh_px_He_nc->Fill(alpha.Px());
            fh_py_He_nc->Fill(alpha.Py());
            fh_pz_He_nc->Fill(alpha.Pz());
            fh_p_He_nc->Fill(pa.Mag());

            fh_px_C_nc->Fill(carbon.Px());
            fh_py_C_nc->Fill(carbon.Py());
            fh_pz_C_nc->Fill(carbon.Pz());
            fh_p_C_nc->Fill(pc.Mag());

            fh_px_px_nc->Fill(alpha.Px(), carbon.Px());
            fh_py_py_nc->Fill(alpha.Py(), carbon.Py());
            fh_pz_pz_nc->Fill(alpha.Pz(), carbon.Pz());

            fh_psum_nc->Fill((pa + pc).Mag());
            fh_theta26_nc->Fill(theta_26);
            fh_Erel_nc->Fill(Erel); // relative Energy plot

            fh_psum_vs_event_nc->Fill(fNeventstrack, (pa + pc).Mag());
            fh_psum_vs_chi->Fill(sq_chi, (pa + pc).Mag());

            fh_theta26_vs_chi->Fill(sq_chi, theta_26);
            fh_Erel_vs_chi->Fill(sq_chi, Erel);
            fh_phi26_vs_chi->Fill(sq_chi, phi_26);
            fh_psum_vs_theta26_nc->Fill(theta_26, (pa + pc).Mag());

            if ((alpha.Px() > 0. && carbon.Px() < 0.) || (alpha.Px() < 0. && carbon.Px() > 0.))
            {

                fh_ErelB_nc->Fill(Erel);
            }

            fh_theta_bc_cm_nc->Fill(alpha_cm.Theta() * TMath::RadToDeg());
            fh_phi_bc_cm_nc->Fill(phi_bc_cm);

            fh_chiy_vs_chix->Fill(chiHex, chiCx);

            is_tracked = false;

            if (mtrackHe > 2.5 && mtrackHe < 3.5 && mtrackC > 11.5 && mtrackC < 12.5)
            {
                Double_t m_invaHe3 = (helium3 + carbon).M(); // invariant mass
                Double_t ErelHe3 = m_invaHe3 - m3He - mC;
                Double_t theta_15O = (helium3 + carbon).Theta() * TMath::RadToDeg();
                fh_Erel_vs_theta16O_3He12C->Fill(theta_15O, ErelHe3);
            }

            //  Double_t Eacheck = 17.1514-0.665768*Ec;
            Double_t Eacheck = -75.6469 + 10.2155 * Ec - 0.318913 * Ec * Ec;
            Bool_t becut = false;
            if (fGraphCuts)
            {
                if (cut_EHe_EC->IsInside(Ec, Ea))
                    becut = true;
            }
            else
            {
                if (abs(Ea - Eacheck) < 0.12)
                    becut = true;
            }
            // Dima's tracker only one chi2;&& becut
            if (sq_chi < (fcut_chiX + fcut_chiY) && mtrackHe > 3.5 && mtrackHe < 4.5 && mtrackC > 11.5 &&
                mtrackC < 12.5 && becut &&
                ((alpha.Px() > 0. && carbon.Px() < 0.) ||
                 (alpha.Px() < 0. && carbon.Px() > 0.))) // abs(carbon.Pz()+alpha.Pz()-17390.) < 400. )
            {

                // (0,1) = (xfi23a,yfi23a), .. (12,13) = (xtofd,ytofd)
                fh_xy_fib23->Fill(posdetHe.at(0), posdetHe.at(3));
                fh_xy_fib23->Fill(posdetC.at(0), posdetC.at(3));
                fh_xfi23_vs_xtofd->Fill(posdetHe.at(12), posdetHe.at(0));
                fh_xfi23_vs_xtofd->Fill(posdetC.at(12), posdetC.at(0));
                fh_yfi23_vs_ytofd->Fill(posdetHe.at(13), posdetHe.at(3));
                fh_yfi23_vs_ytofd->Fill(posdetC.at(13), posdetC.at(3));

                for (Int_t idet = 0; idet < N_DET_MAX; idet++)
                {
                    fh_Erel_vs_xdet[idet]->Fill(posdetC.at(2 * idet), Erel);
                    fh_Erel_vs_xdet[idet]->Fill(posdetHe.at(2 * idet), Erel);
                    fh_Erel_vs_ydet[idet]->Fill(posdetC.at(2 * idet + 1), Erel);
                    fh_Erel_vs_ydet[idet]->Fill(posdetHe.at(2 * idet + 1), Erel);
                }

                if (fHitItemsCalifa)
                {
                    // CALIFA
                    auto detCalifa = fHitItemsCalifa;
                    Int_t nHitsCalifaH = detCalifa->GetEntriesFast();

                    for (Int_t ihitC = 0; ihitC < nHitsCalifaH; ihitC++)
                    {
                        R3BCalifaClusterData* hitCalifa = (R3BCalifaClusterData*)detCalifa->At(ihitC);
                        if (!hitCalifa)
                            continue;

                        ULong64_t timeCalifa = hitCalifa->GetTime();
                        Double_t timerelCalifa = (double)(timeCalifa - timeTS);
                        Double_t tof = timerelCalifa - tCtofd;
                        Int_t CrystalNb = hitCalifa->GetCrystalList().size();
                        Double_t Energy = hitCalifa->GetEnergy();
                        Double_t theta = hitCalifa->GetTheta();
                        Double_t Energy_dc = Energy * GAMMA * (1 - BETA * TMath::Cos(theta));

                        /*
                            Double_t GetEnergy() const { return fEnergy; }
                            Double_t GetNf() const { return fNf; }
                            Double_t GetNs() const { return fNs; }
                            Double_t GetTheta() const { return fTheta; }
                            Double_t GetPhi() const { return fPhi; }
                            ULong64_t GetTime() const { return fTime; }
                            uint32_t GetClusterType() const { return fClusterType; }
                            Int_t GetNbOfCrystalHits()  const { return fCrystalList.size(); }
                            std::vector<Int_t> GetCrystalList() const {return fCrystalList; }
                            Int_t GetMotherCrystal() const {return fCrystalList.at(0); }
                        */

                        if (hitCalifa->GetClusterType() == 1 &&
                            !(IS_NAN(Energy))) // gammas (IS_NAN(Energy) == overflow)
                        {
                            fh_Erel_withCalifa_motherId->Fill(hitCalifa->GetMotherCrystal(), Erel);
                            fh_theta_vs_theta->Fill(theta_16O, hitCalifa->GetTheta() * TMath::RadToDeg());
                            fh_califa_hitenergy_select->Fill(Energy_dc);
                            fh_califa_tofd->Fill(timerelCalifa, tCtofd);
                            fh_Erel_withCalifa_tof->Fill(timerelCalifa - tCtofd, Erel);

                            if (hitCalifa->GetMotherCrystal() > 927 && hitCalifa->GetMotherCrystal() < 1953) // barrel
                            {
                                fh_Erel_withCalifa_2d_barrel->Fill(hitCalifa->GetCrystalList().size(), Erel);
                                fh_califaenergy_2d_barrel->Fill(hitCalifa->GetCrystalList().size(), Energy_dc);
                                if (hitCalifa->GetCrystalList().size() > 1 && hitCalifa->GetCrystalList().size() < 5)
                                {
                                    fh_califa_hitenergy_boost_barrel->Fill(Energy_dc);
                                    fh_Erel_withCalifa_barrel->Fill(Erel);
                                    fh_Erel_vs_theta16O_withcalifa_barrel->Fill(theta_16O, Erel);
                                    if (theta_16O < fThetaGrazing)
                                        fh_califa_hitenergy_bg_barrel->Fill(Erel, Energy_dc);
                                    else
                                        fh_califa_hitenergy_ag_barrel->Fill(Erel, Energy_dc);
                                }
                            }

                            if (hitCalifa->GetMotherCrystal() > 1952 && hitCalifa->GetMotherCrystal() < 2433) // iphos
                            {
                                fh_Erel_withCalifa_2d_iphos->Fill(hitCalifa->GetCrystalList().size(), Erel);
                                fh_califaenergy_2d_iphos->Fill(hitCalifa->GetCrystalList().size(), Energy_dc);
                                if (hitCalifa->GetCrystalList().size() > 1 && hitCalifa->GetCrystalList().size() < 5)
                                {
                                    fh_califa_hitenergy_boost_iphos->Fill(Energy_dc);
                                    fh_Erel_withCalifa_iphos->Fill(Erel);
                                    fh_Erel_vs_theta16O_withcalifa_iphos->Fill(theta_16O, Erel);
                                    fh_Erel_withCalifa_2d_iphos->Fill(hitCalifa->GetCrystalList().size(), Erel);
                                    if (theta_16O < fThetaGrazing)
                                        fh_califa_hitenergy_bg_iphos->Fill(Erel, Energy_dc);
                                    else
                                        fh_califa_hitenergy_ag_iphos->Fill(Erel, Energy_dc);
                                }
                            }
                        }
                    }
                }

                is_tracked = true;
                nTracksFound += 1;
                fh_energy->Fill(Ec, Ea);

                Double_t mche = mHe * mC / mO;
                Double_t deltaTheta26 =
                    0.028 / 91.2 * cos(theta_26 / TMath::RadToDeg()) * cos(theta_26 / TMath::RadToDeg());
                Double_t deltaErel = 2. * mche * sqrt(Ec / mC * Ea / mHe) * sin(theta_26 / TMath::RadToDeg()) *
                                     deltaTheta26 * 1000.; // Ea/Ec are in GeV
                //    cout<<"deltaErel: "<<deltaErel<<", deltaTheta26: "<<deltaTheta26<<endl;
                fh_dErel->Fill(deltaErel);

                fh_minv->Fill(Ec / 1000. + Ea / 1000.);

                //  fh_chiy_vs_chix->Fill(chiHex,chiCx);
                fh_target_xy->Fill(XHe, YHe);
                fh_target_xy->Fill(XC, YC);

                fh_target_xx->Fill(XHe - XC);
                fh_target_yy->Fill(YHe - YC);

                fh_mass->Fill(mtrackC, mtrackHe);

                fh_psum_vs_event->Fill(fNeventstrack, (pa + pc).Mag());

                fh_px_He->Fill(alpha.Px());
                fh_py_He->Fill(alpha.Py());
                fh_pz_He->Fill(alpha.Pz());
                fh_p_He->Fill(pa.Mag());

                fh_px_C->Fill(carbon.Px());
                fh_py_C->Fill(carbon.Py());
                fh_pz_C->Fill(carbon.Pz());
                fh_p_C->Fill(pc.Mag());

                // if (Erel < 0.4)
                {
                    fh_px_px->Fill(alpha.Px(), carbon.Px());
                    fh_py_py->Fill(alpha.Py(), carbon.Py());
                    fh_pz_pz->Fill(alpha.Pz(), carbon.Pz());
                    fh_p_p->Fill(pa.Mag(), pc.Mag());
                }

                fh_psum->Fill((pa + pc).Mag());
                fh_pzsum->Fill(alpha.Pz() + carbon.Pz());

                fh_theta26->Fill(theta_26);
                fh_Erel->Fill(Erel); // relative Energy plot

                fh_phi_bc_cm->Fill(phi_bc_cm);

                if (alpha.Px() < 0. && carbon.Px() < 0.)
                    fh_ErelR->Fill(Erel);
                if (alpha.Px() > 0. && carbon.Px() > 0.)
                    fh_ErelL->Fill(Erel);
                if ((alpha.Px() > 0. && carbon.Px() < 0.) || (alpha.Px() < 0. && carbon.Px() > 0.))
                {
                    // if (theta_26 > 0.7)
                    fh_ErelB->Fill(Erel);
                    fh_ErelB_vs_theta16O->Fill(theta_16O, Erel);
                }

                fh_Erel_vs_theta16O->Fill(theta_16O, Erel);
                fh_pHe_vs_theta26->Fill(pa.Mag(), theta_26 * TMath::DegToRad() * 1000.);
                fh_psum_vs_theta26->Fill(theta_26, (pa + pc).Mag());

                fh_Erel_vs_psum->Fill((pa + pc).Mag(), Erel);
                fh_theta_16O->Fill(theta_16O); // theta oxygen
                fh_phi_16O->Fill(phi_16O);

                Double_t alpha_cm_Phi = alpha_cm.Phi() * TMath::RadToDeg();
                if (alpha_cm_Phi < 0.)
                    alpha_cm_Phi += 360;

                Double_t carbon_cm_Phi = carbon_cm.Phi() * TMath::RadToDeg();
                if (carbon_cm_Phi < 0.)
                    carbon_cm_Phi += 360;

                fh_theta_bc_cm->Fill(alpha_cm.Theta() * TMath::RadToDeg(), Erel);

                fh_phi_bc_cm_polar->Fill(phi_bc_cm, 1);

                if ((alpha.Px() > 0. && carbon.Px() < 0.) || (alpha.Px() < 0. && carbon.Px() > 0.))
                {
                    if (theta_16O < fThetaGrazing)
                    {
                        fh_ErelB_vs_phibc_bg->Fill(Erel, phi_bc_cm);
                        fh_ErelB_vs_theta26_bg->Fill(theta_26, Erel);
                        fh_erelB_vs_ptransHe_bg->Fill(ptransHe, Erel);
                        fh_erelB_vs_ptransC_bg->Fill(ptransC, Erel);
                        fh_thetaB_4He_cm_bg->Fill(alpha_cm.CosTheta(), Erel);
                        fh_phiB_4He_cm_bg->Fill(alpha_cm_Phi, Erel);
                        fh_thetaB_12C_cm_bg->Fill(carbon_cm.CosTheta(), Erel);
                        fh_phiB_12C_cm_bg->Fill(carbon_cm_Phi, Erel);
                    }
                    else
                    {
                        fh_ErelB_vs_phibc_ag->Fill(Erel, phi_bc_cm);
                        fh_ErelB_vs_theta26_ag->Fill(theta_26, Erel);
                        fh_erelB_vs_ptransHe_ag->Fill(ptransHe, Erel);
                        fh_erelB_vs_ptransC_ag->Fill(ptransC, Erel);
                        fh_thetaB_4He_cm_ag->Fill(alpha_cm.CosTheta(), Erel);
                        fh_phiB_4He_cm_ag->Fill(alpha_cm_Phi, Erel);
                        fh_thetaB_12C_cm_ag->Fill(carbon_cm.CosTheta(), Erel);
                        fh_phiB_12C_cm_ag->Fill(carbon_cm_Phi, Erel);
                    }
                }

                if (theta_16O < fThetaGrazing)
                {
                    fh_Erel_vs_phibc_bg->Fill(Erel, phi_bc_cm);
                    fh_Erel_vs_theta26_bg->Fill(theta_26, Erel);
                    fh_erel_vs_ptransHe_bg->Fill(ptransHe, Erel);
                    fh_erel_vs_ptransC_bg->Fill(ptransC, Erel);
                    fh_theta_4He_cm_bg->Fill(alpha_cm.CosTheta(), Erel);
                    fh_phi_4He_cm_bg->Fill(alpha_cm_Phi, Erel);
                    fh_theta_12C_cm_bg->Fill(carbon_cm.CosTheta(), Erel);
                    fh_phi_12C_cm_bg->Fill(carbon_cm_Phi, Erel);
                }
                else
                {
                    fh_Erel_vs_phibc_ag->Fill(Erel, phi_bc_cm);
                    fh_Erel_vs_theta26_ag->Fill(theta_26, Erel);
                    fh_erel_vs_ptransHe_ag->Fill(ptransHe, Erel);
                    fh_erel_vs_ptransC_ag->Fill(ptransC, Erel);
                    fh_theta_4He_cm_ag->Fill(alpha_cm.CosTheta(), Erel);
                    fh_phi_4He_cm_ag->Fill(alpha_cm_Phi, Erel);
                    fh_theta_12C_cm_ag->Fill(carbon_cm.CosTheta(), Erel);
                    fh_phi_12C_cm_ag->Fill(carbon_cm_Phi, Erel);
                }

                fh_thetax_px->Fill(pHex, thetaxHe);
                fh_thetax_px->Fill(pCx, thetaxC);

                fh_thetay_py->Fill(pHey, thetayHe);
                fh_thetay_py->Fill(pCy, thetayC);

                fh_thetax_py->Fill(pHey, thetaxHe);
                fh_thetax_py->Fill(pCy, thetaxC);

                fh_thetay_px->Fill(pHex, thetayHe);
                fh_thetay_px->Fill(pCx, thetayC);
            }

            /*
                        // rotation in oxygen direction
                        //TLorentzRotation transform;
                        //transform.RotateX(oxygen.Px()).RotateY(oxygen.Py()).RotateZ(0.);
                        TVector3 transform = oxygen.Vect(); /// TODO: check if it needs to be normed
                        alpha *= transform; // apply transformation vector
                        carbon *= transform;

                        if(abs(alpha.Phi()*TMath::RadToDeg()) > 88 && abs(alpha.Phi()*TMath::RadToDeg()) < 92)
                        {
                        fh_target_xy->Fill(XHe * 100., YHe * 100.);

                        fh_px_He->Fill(pHex);
                        fh_py_He->Fill(pHey);
                        fh_pz_He->Fill(pHez);

                        fh_px_C->Fill(pCx);
                        fh_py_C->Fill(pCy);
                        fh_pz_C->Fill(pCz);

                            fh_theta26->Fill(theta26); ///angle alpha carbon (theta)
                            fh_theta_16O->Fill(theta16O); // theta oxygen


                            fh_Erel->Fill(Erel); // relative Energy plot
                        fh_cs_cd->Fill(Erel,Erel);


            */

        } // end if chi2
    }     // end if trackHits>1

    LOG(debug) << "finished with track" << endl;

    if (fTrack && is_oxygen && !fPairs)
    {

        fh_chi2->Fill(chix);
        fh_psum_vs_chi->Fill(chix, Pf_tot);
        fh_target_xy_nc->Fill(Xf, Yf);
        fh_px_O_nc->Fill(Pxf);
        fh_py_O_nc->Fill(Pyf);
        fh_pz_O_nc->Fill(Pzf);
        fh_p_O_nc->Fill(Pf_tot);
        fh_psum_vs_event_nc->Fill(fNEvents, Pf_tot);
        Double_t Radius = sqrt(Xf * Xf + Yf * Yf);

        if (chix < fcut_chiX && chiy < fcut_chiY)
        {
            is_tracked = true;

            fh_target_xy->Fill(Xf, Yf);

            fh_px_O->Fill(Pxf);
            fh_py_O->Fill(Pyf);
            fh_pz_O->Fill(Pzf);
            fh_p_O->Fill(Pf_tot);
            fh_psum_vs_event->Fill(fNEvents, Pf_tot);

        } // end if chi2
    }     // end if trackHits>1

    if (fSimu && nHitsMCTrack > 0)
    {
        if (fPairs) //&& is_tracked)
        {

            paMC = alphaMC.Vect();
            pcMC = carbonMC.Vect();
            Double_t theta_26MC = alphaMC.Angle(carbonMC.Vect()) * TMath::RadToDeg(); /// angle alpha carbon (theta)
            fh_theta26_simu->Fill(theta_26MC);
            // Double_t randtheta = (std::rand() / (float)RAND_MAX) - 0.5;
            //** Calculate invariant mass and relative energy **/
            m_invaMC = (alphaMC + carbonMC).M(); // invariant mass
            ErelMC = m_invaMC - mHe - mC;        // relative Energy
            fh_Erel_vs_thetaMC->Fill(theta_26MC, ErelMC);

            /* * transfer to cm system and rotate * */

            oxygenMC = alphaMC + carbonMC; // excited oxygen
            // cout<<"gamma "<<oxygen.Gamma()<<endl;
            Double_t thetaMC_16O = oxygenMC.Theta() * TMath::RadToDeg();
            Double_t phiMC_16O = oxygenMC.Phi() * TMath::RadToDeg();
            if (phiMC_16O < 0)
                phiMC_16O += 360.;

            // transfer to cm system and make some rotations
            // boost them to centre of mass
            alphaMC_cm = alphaMC;
            carbonMC_cm = carbonMC;
            oxygenMC_cm = oxygenMC;
            alphaMC_cm.Boost(-oxygenMC.BoostVector());
            carbonMC_cm.Boost(-oxygenMC.BoostVector());
            oxygenMC_cm.Boost(-oxygenMC.BoostVector());

            Double_t phiMC = (alphaMC_cm.Phi() - carbonMC_cm.Phi()) * TMath::RadToDeg();
            Double_t thetaMC = alphaMC_cm.Angle(carbonMC_cm.Vect()) * TMath::RadToDeg();

            fh_psum_MC->Fill((paMC + pcMC).Mag());
            fh_pzsum_MC->Fill(alphaMC.Pz() + carbonMC.Pz());

            phiMC_bc_cm = (oxygenMC.Phi() - alphaMC_cm.Phi()) * TMath::RadToDeg();
            if (phiMC_bc_cm < 0)
                phiMC_bc_cm += 360.;
            fh_phiMC_bc_cm->Fill(phiMC_bc_cm);

            fh_phiMC_bc_cm_polar->Fill(phiMC_bc_cm, 1);

            fh_minv_simu->Fill(m_invaMC);
            fh_Erel_vs_phibcMC->Fill(ErelMC, phiMC_bc_cm);
            fh_Erel_simu->Fill(ErelMC);

            fh_px_px_mc->Fill(pHex_mc, pCx_mc);
            fh_py_py_mc->Fill(pHey_mc, pCy_mc);
            fh_pz_pz_mc->Fill(pHez_mc, pCz_mc);

            if (is_tracked)
            {
                fh_dtheta->Fill(theta_26MC - theta_26);

                fh_dx->Fill(XHe_mc - XHe);
                fh_dx->Fill(XC_mc - XC);
                fh_dy->Fill(YHe_mc - YHe);
                fh_dy->Fill(YC_mc - YC);
                fh_target_xy_MC->Fill(XHe_mc, YHe_mc);
                fh_target_xy_MC->Fill(XC_mc, YC_mc);

                //   cout << "X-momenta: MC_He: " << pHex_mc << ", tracked He: " << pHex << endl;
                //   cout << "X-momenta: MC_C: " << pCx_mc << ", tracked C: " << pCx << endl;
                //   cout << "Y-momenta: MC_He: " << pHey_mc << ", tracked He: " << pHey << endl;
                //   cout << "Y-momenta: MC_C: " << pCy_mc << ", tracked C: " << pCy << endl;
                //   cout << "Momenta: MC_He: " << paMC.Mag() << ", tracked He: " << pa.Mag() << endl;
                //   cout << "Momenta: MC_C: " << pcMC.Mag() << ", tracked C: " << pc.Mag() << endl;

                fh_dpxC->Fill((pCx_mc - pCx) / pCx_mc * 100.);
                fh_dpxHe->Fill((pHex_mc - pHex) / pHex_mc * 100.);
                fh_dpyC->Fill((pCy_mc - pCy) / pCy_mc * 100.);
                fh_dpyHe->Fill((pHey_mc - pHey) / pHey_mc * 100.);
                fh_dpzC->Fill((pCz_mc - pCz) / pCz_mc * 100.);
                fh_dpzHe->Fill((pHez_mc - pHez) / pHez_mc * 100.);
                fh_dpC->Fill((pcMC.Mag() - pc.Mag()) / pcMC.Mag() * 100.);
                fh_dpHe->Fill((paMC.Mag() - pa.Mag()) / paMC.Mag() * 100.);
                fh_dp->Fill(((paMC + pcMC).Mag() - (pa + pc).Mag()) / ((paMC + pcMC).Mag()) * 100.);

                Double_t diff =
                    sqrt(pCx_mc * pCx_mc + pCy_mc * pCy_mc + pCz_mc * pCz_mc) - sqrt(pCx * pCx + pCy * pCy + pCz * pCz);
                //	cout<<"Difference: "<<pcMC.Mag() - pc.Mag()<<", "<<diff<<endl;

                fh_x_dpx->Fill((pCx_mc - pCx) / pCx_mc * 100., pCx);
                fh_x_dpx->Fill((pHex_mc - pHex) / pHex_mc * 100., pHex);
                fh_y_dpy->Fill((pCy_mc - pCy) / pCy_mc * 100., pCy);
                fh_y_dpy->Fill((pHey_mc - pHey) / pHey_mc * 100., pHey);
                fh_dpy_dpx->Fill((pCx_mc - pCx) / pCx_mc * 100., (pCy_mc - pCy) / pCy_mc * 100.);
                fh_dpy_dpx->Fill((pHex_mc - pHex) / pHex_mc * 100., (pHey_mc - pHey) / pHey_mc * 100.);

                // Comparison with simulation
                Double_t tthetaxC_simu = pCx_mc / pCz_mc;    // * 1000.;
                Double_t tthetayC_simu = pCy_mc / pCz_mc;    // * 1000.;
                Double_t tthetaxHe_simu = pHex_mc / pHez_mc; // * 1000.;
                Double_t tthetayHe_simu = pHey_mc / pHez_mc; // *  1000.;

                Double_t thetaxC_simu = atan(tthetaxC_simu) * 1000.;
                Double_t thetayC_simu = atan(tthetayC_simu) * 1000.;
                Double_t thetaxHe_simu = atan(tthetaxHe_simu) * 1000.;
                Double_t thetayHe_simu = atan(tthetayHe_simu) * 1000.;

                fh_thetax_dpx_He->Fill((pHex_mc - pHex), thetaxHe_simu);
                fh_thetax_dpx_C->Fill((pCx_mc - pCx), thetaxC_simu);

                fh_thetay_dpy_He->Fill((pHey_mc - pHey), thetayHe_simu);
                fh_thetay_dpy_C->Fill((pCy_mc - pCy), thetayC_simu);

                fh_thetax_dpy->Fill((pHey_mc - pHey), thetaxHe_simu);
                fh_thetax_dpy->Fill((pCy_mc - pCy), thetaxC_simu);

                fh_thetay_dpx->Fill((pHex_mc - pHex), thetayHe_simu);
                fh_thetay_dpx->Fill((pCx_mc - pCx), thetayC_simu);
            }
        }
        else
        {

            if (is_tracked)
            {

                fh_dx->Fill((Xf_mc - Xf));
                fh_dy->Fill((Yf_mc - Yf));

                fh_dpxO->Fill((Pxf_mc - Pxf) / Pxf_mc * 100.);
                fh_dpyO->Fill((Pyf_mc - Pyf) / Pyf_mc * 100.);
                fh_dpzO->Fill((Pzf_mc - Pzf) / Pzf_mc * 100.);
                // fh_dpO->Fill((Pf_tot_mc - Pf_tot) / Pf_tot_mc * 100.);

                fh_x_dpx->Fill((Pxf_mc - Pxf) / Pxf_mc * 100., Xf);
                fh_y_dpy->Fill((Pyf_mc - Pyf) / Pyf_mc * 100., Yf);
                // fh_dpy_dpx->Fill((Pxf_mc - Pxf) / Pxf_mc * 100., (Pyf_mc - Pyf) / Pyf_mc * 100.);
            }
        }
    }

    fNEvents += 1;

    return;
}

void R3BGlobalAnalysisS494::FinishEvent()
{
    fTrack->Clear();
    if (fMCTrack)
        fMCTrack->Clear();
    if (fHitItemsCalifa)
        fHitItemsCalifa->Clear();
}

void R3BGlobalAnalysisS494::FinishTask()
{
    fh_target_xy->Write();
    fh_target_xx->Write();
    fh_target_yy->Write();
    fh_target_xy_nc->Write();

    fh_chi2->Write();
    fh_psum_vs_chi->Write();
    fh_Erel_vs_chi->Write();
    fh_phi26_vs_chi->Write();
    fh_psum_vs_theta26->Write();
    fh_chiy_vs_chix->Write();
    fh_chiy_vs_chix_nc->Write();
    fh_theta26_vs_chi->Write();
    fh_px_He_nc->Write();
    fh_py_He_nc->Write();
    fh_pz_He_nc->Write();
    fh_p_He_nc->Write();
    fh_px_C_nc->Write();
    fh_py_C_nc->Write();
    fh_pz_C_nc->Write();
    fh_p_C_nc->Write();
    fh_px_O_nc->Write();
    fh_py_O_nc->Write();
    fh_pz_O_nc->Write();
    fh_p_O_nc->Write();
    fh_px_px_nc->Write();
    fh_py_py_nc->Write();
    fh_pz_pz_nc->Write();
    fh_psum_vs_event_nc->Write();
    fh_psum_nc->Write();

    fh_theta26_nc->Write();
    fh_ErelB_nc->Write();
    fh_Erel_nc->Write();
    fh_theta_bc_cm_nc->Write();
    fh_phi_bc_cm_nc->Write();

    fh_px_He->Write();
    fh_py_He->Write();
    fh_pz_He->Write();
    fh_p_He->Write();
    fh_px_C->Write();
    fh_py_C->Write();
    fh_pz_C->Write();
    fh_p_C->Write();
    fh_px_O->Write();
    fh_py_O->Write();
    fh_pz_O->Write();
    fh_p_O->Write();
    fh_px_px->Write();
    fh_py_py->Write();
    fh_pz_pz->Write();
    fh_p_p->Write();
    fh_psum->Write();
    fh_pzsum->Write();
    fh_psum_vs_event->Write();
    fh_pHe_vs_theta26->Write();
    fh_psum_vs_theta26_nc->Write();

    fh_theta26->Write();
    fh_minv->Write();
    fh_Erel->Write();
    fh_ErelB->Write();
    fh_ErelR->Write();
    fh_ErelL->Write();
    fh_Erel_vs_psum->Write();
    fh_Erel_vs_theta16O->Write();
    fh_ErelB_vs_theta16O->Write();
    fh_Erel_vs_theta16O_3He12C->Write();
    fh_phi_bc_cm->Write();

    fh_thetax_px->Write();
    fh_thetay_py->Write();
    fh_thetax_py->Write();
    fh_thetay_px->Write();

    fh_theta_16O->Write();
    fh_phi_16O->Write();
    fh_theta_bc_cm->Write();
    fh_phi_bc_cm_polar->Write();

    fh_ErelB_vs_phibc_bg->Write();
    fh_ErelB_vs_theta26_bg->Write();
    fh_erelB_vs_ptransHe_bg->Write();
    fh_erelB_vs_ptransC_bg->Write();
    fh_thetaB_4He_cm_bg->Write();
    fh_phiB_4He_cm_bg->Write();
    fh_thetaB_12C_cm_bg->Write();
    fh_phiB_12C_cm_bg->Write();
    fh_ErelB_vs_phibc_ag->Write();
    fh_ErelB_vs_theta26_ag->Write();
    fh_erelB_vs_ptransHe_ag->Write();
    fh_erelB_vs_ptransC_ag->Write();
    fh_thetaB_4He_cm_ag->Write();
    fh_phiB_4He_cm_ag->Write();
    fh_thetaB_12C_cm_ag->Write();
    fh_phiB_12C_cm_ag->Write();
    fh_Erel_vs_phibc_bg->Write();
    fh_Erel_vs_theta26_bg->Write();
    fh_erel_vs_ptransHe_bg->Write();
    fh_erel_vs_ptransC_bg->Write();
    fh_theta_4He_cm_bg->Write();
    fh_phi_4He_cm_bg->Write();
    fh_theta_12C_cm_bg->Write();
    fh_phi_12C_cm_bg->Write();
    fh_Erel_vs_phibc_ag->Write();
    fh_Erel_vs_theta26_ag->Write();
    fh_erel_vs_ptransHe_ag->Write();
    fh_erel_vs_ptransC_ag->Write();
    fh_theta_4He_cm_ag->Write();
    fh_phi_4He_cm_ag->Write();
    fh_theta_12C_cm_ag->Write();
    fh_phi_12C_cm_ag->Write();

    fh_mass_nc->Write();
    fh_mass->Write();
    fh_energy_nc->Write();
    fh_energy->Write();

    if (fSimu)
    {
        fh_target_xy_MC->Write();
        fh_dx->Write();
        fh_dy->Write();
        fh_dz->Write();
        fh_dpxO->Write();
        fh_dpyO->Write();
        fh_dpzO->Write();
        fh_dpO->Write();
        fh_dpxC->Write();
        fh_dpyC->Write();
        fh_dpzC->Write();
        fh_dpC->Write();
        fh_dpxHe->Write();
        fh_dpyHe->Write();
        fh_dpzHe->Write();
        fh_dpHe->Write();
        fh_dp->Write();
        fh_thetax_dpx_C->Write();
        fh_thetay_dpy_C->Write();
        fh_thetax_dpx_He->Write();
        fh_thetay_dpy_He->Write();
        fh_thetax_dpy->Write();
        fh_thetay_dpx->Write();
        fh_x_dpx->Write();
        fh_y_dpy->Write();
        fh_dpy_dpx->Write();
        fh_Erel_vs_thetaMC->Write();
        fh_dtheta->Write();
        fh_psum_MC->Write();
        fh_pzsum_MC->Write();
        fh_theta26_simu->Write();
        fh_Erel_simu->Write();
        fh_phiMC_bc_cm->Write();
        fh_phiMC_bc_cm_polar->Write();
        fh_Erel_vs_phibcMC->Write();
        fh_px_px_mc->Write();
        fh_py_py_mc->Write();
        fh_pz_pz_mc->Write();
        fh_minv_simu->Write();
    }
    fh_dErel->Write();

    if (fHitItemsCalifa)
    {
        fh_Erel_withCalifa_barrel->Write();
        fh_califa_hitenergy_boost_barrel->Write();
        fh_califa_hitenergy_ag_barrel->Write();
        fh_califa_hitenergy_bg_barrel->Write();
        fh_Erel_withCalifa_2d_barrel->Write();
        fh_califaenergy_2d_barrel->Write();
        fh_califaenergy_2d_iphos->Write();
        fh_Erel_vs_theta16O_withcalifa_barrel->Write();

        fh_Erel_withCalifa_iphos->Write();
        fh_califa_hitenergy_boost_iphos->Write();
        fh_califa_hitenergy_ag_iphos->Write();
        fh_califa_hitenergy_bg_iphos->Write();
        fh_Erel_withCalifa_2d_iphos->Write();
        fh_Erel_vs_theta16O_withcalifa_iphos->Write();

        fh_califa_hitenergy_select->Write();
        fh_Erel_withCalifa_tof->Write();
        fh_Erel_withCalifa_motherId->Write();
        fh_theta_vs_theta->Write();
        fh_califa_tofd->Write();
    }
    for (int det = 0; det < N_DET_MAX; det++)
    {
        fh_Erel_vs_xdet[det]->Write();
        fh_Erel_vs_ydet[det]->Write();
    }
    fh_yfi23_vs_ytofd_bc->Write();
    fh_yfi23_vs_ytofd->Write();
    fh_xfi23_vs_xtofd_bc->Write();
    fh_xfi23_vs_xtofd->Write();
    fh_xy_fib23->Write();
    fh_xy_fib23_bc->Write();
    fh_dt_fib23_bc->Write();
}
ClassImp(R3BGlobalAnalysisS494)
