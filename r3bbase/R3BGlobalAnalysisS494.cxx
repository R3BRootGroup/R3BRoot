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

#include "R3BLosCalData.h"
#include "R3BLosHitData.h"
#include "R3BLosMappedData.h"

#include "R3BBeamMonitorMappedData.h"

#include "R3BGlobalAnalysisS494.h"

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
#include "R3BTrack.h"

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
    , fPairs(0)
    , fSimu(0)
    , fB(-1710)
    , fcut_chiX(5000)
    , fcut_chiY(5000)
    , fNEvents(0)
{
}

R3BGlobalAnalysisS494::~R3BGlobalAnalysisS494() {}

InitStatus R3BGlobalAnalysisS494::Init()
{
    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(INFO) << "R3BGlobalAnalysisS494::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline* run = FairRunOnline::Instance();

    // Get objects for detectors on all levels
    fMCTrack = (TClonesArray*)mgr->GetObject("MCTrack");
    fTrack = (TClonesArray*)mgr->GetObject("Track");
    maxevent = mgr->CheckMaxEventNo();

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

    fh_px_He = new TH1F("px_He", " px He", 2000, -500., 500);
    fh_px_He->GetXaxis()->SetTitle("px / MeV/c");
    fh_px_He->GetYaxis()->SetTitle("counts");

    fh_py_He = new TH1F("py_He", " py He", 2000, -500., 500);
    fh_py_He->GetXaxis()->SetTitle("py / MeV/c");
    fh_py_He->GetYaxis()->SetTitle("counts");

    fh_pz_He = new TH1F("pz_He", " pz He", 30000, 0., 30000.);
    fh_pz_He->GetXaxis()->SetTitle("pz / MeV/c");
    fh_pz_He->GetYaxis()->SetTitle("counts");

    fh_p_He = new TH1F("p_He", " p He", 10000, 0., 30000.);
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

    fh_p_C = new TH1F("p_C", " p C", 10000, 0., 30000.);
    fh_p_C->GetXaxis()->SetTitle("pz / MeV/c");
    fh_p_C->GetYaxis()->SetTitle("counts");

    fh_target_xy = new TH2F("target_xy", "target xy ", 500, -5, 5, 500, -5, 5);
    fh_target_xy->GetXaxis()->SetTitle("x / cm");
    fh_target_xy->GetYaxis()->SetTitle("y / cm");

    fh_chi2 = new TH1F("chi2", "chi2 ", 500, 0., 500);
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

    fh_dpxC = new TH1F("tracker_dpxC", "tracker dpx C", 1000, -50, 50);
    fh_dpxC->GetXaxis()->SetTitle("dpx / percent");
    fh_dpxC->GetYaxis()->SetTitle("counts");

    fh_dpyC = new TH1F("tracker_dpyC", "tracker dpy C", 1000, -50, 50);
    fh_dpyC->GetXaxis()->SetTitle("dpy / percent");
    fh_dpyC->GetYaxis()->SetTitle("counts");

    fh_dpzC = new TH1F("tracker_dpzC", "tracker dpz C", 1000, -50, 50);
    fh_dpzC->GetXaxis()->SetTitle("dpz / percent");
    fh_dpzC->GetYaxis()->SetTitle("counts");

    fh_dpC = new TH1F("tracker_dpC", "tracker dp C", 1000, -50, 50);
    fh_dpC->GetXaxis()->SetTitle("dp / percent");
    fh_dpC->GetYaxis()->SetTitle("counts");

    fh_dpxHe = new TH1F("tracker_dpxHe", "tracker Hedpx ", 1000, -50, 50);
    fh_dpxHe->GetXaxis()->SetTitle("dpx / percent");
    fh_dpxHe->GetYaxis()->SetTitle("counts");

    fh_dpyHe = new TH1F("tracker_dpyHe", "tracker Hedpy ", 1000, -50, 50);
    fh_dpyHe->GetXaxis()->SetTitle("dpy / percent");
    fh_dpyHe->GetYaxis()->SetTitle("counts");

    fh_dpzHe = new TH1F("tracker_dpzHe", "tracker dpz He", 1000, -50, 50);
    fh_dpzHe->GetXaxis()->SetTitle("dpz / percent");
    fh_dpzHe->GetYaxis()->SetTitle("counts");

    fh_dpHe = new TH1F("tracker_dpHe", "tracker dp He", 1000, -50, 50);
    fh_dpHe->GetXaxis()->SetTitle("dp / percent");
    fh_dpHe->GetYaxis()->SetTitle("counts");

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

    fh_x_dpx = new TH2F("tracker_x_dpx", "tracker x vs dpx ", 1000, -100, 100, 200, -1., 1.);
    fh_x_dpx->GetXaxis()->SetTitle("dpx / percent");
    fh_x_dpx->GetYaxis()->SetTitle("x / cm");

    fh_y_dpy = new TH2F("tracker_y_dpy", "tracker y vs dpy ", 2000, -200, 200, 200, -1., 1.);
    fh_y_dpy->GetXaxis()->SetTitle("dpy / percent");
    fh_y_dpy->GetYaxis()->SetTitle("y / cm");

    fh_px_px = new TH2F("px_px", "px He vs. px C", 400, -1000, 400, 1000, -1000., 1000.);
    fh_px_px->GetXaxis()->SetTitle("px / MeV/c");
    fh_px_px->GetYaxis()->SetTitle("px / MeV/c");

    fh_py_py = new TH2F("py_py", "py He vs. py C", 400, -1000, 1000, 400, -1000., 1000.);
    fh_py_py->GetXaxis()->SetTitle("py / MeV/c");
    fh_py_py->GetYaxis()->SetTitle("py / MeV/c");

    fh_pz_pz = new TH2F("pz_pz", "pz He vs. pz C", 400, 0, 10000, 800, 0., 20000.);
    fh_pz_pz->GetXaxis()->SetTitle("pz / MeV/c");
    fh_pz_pz->GetYaxis()->SetTitle("pz / MeV/c");

    fh_px_px_cm = new TH2F("px_px_cm", "px He vs. px C", 1000, -1000, 1000, 1000, -1000., 1000.);
    fh_px_px_cm->GetXaxis()->SetTitle("px / MeV/c");
    fh_px_px_cm->GetYaxis()->SetTitle("px / MeV/c");

    fh_py_py_cm = new TH2F("py_py_cm", "py He vs. py C", 1000, -1000, 1000, 1000, -1000., 1000.);
    fh_py_py_cm->GetXaxis()->SetTitle("py / MeV/c");
    fh_py_py_cm->GetYaxis()->SetTitle("py / MeV/c");

    fh_pz_pz_cm = new TH2F("pz_pz_cm", "pz He vs. pz C", 1000, -1000, 1000, 1000, -1000., 1000.);
    fh_pz_pz_cm->GetXaxis()->SetTitle("pz / MeV/c");
    fh_pz_pz_cm->GetYaxis()->SetTitle("pz / MeV/c");

    // Analysis
    fh_chiy_vs_chix_He = new TH2F("chiy_vs_chix_He", "chi y vs. chi x for He", 250, 0., 500., 250, 0., 500.);
    fh_chiy_vs_chix_He->GetXaxis()->SetTitle("chi x");
    fh_chiy_vs_chix_He->GetYaxis()->SetTitle("chi y");

    fh_chiy_vs_chix_C = new TH2F("chiy_vs_chix_C", "chi y vs. chi x for C", 250, 0., 500., 250, 0., 500.);
    fh_chiy_vs_chix_C->GetXaxis()->SetTitle("chi x");
    fh_chiy_vs_chix_C->GetYaxis()->SetTitle("chi y");

    fh_theta26 = new TH1F("theta26", "theta between alpha and carbon ", 500, 0., 5);
    fh_theta26->GetXaxis()->SetTitle("angle / degree");
    fh_theta26->GetYaxis()->SetTitle("counts");

    fh_theta_16O = new TH1F("theta_16O", "theta of excited 16O ", 500, 0., 5);
    fh_theta_16O->GetXaxis()->SetTitle("angle / degree");
    fh_theta_16O->GetYaxis()->SetTitle("counts");

    fh_phi_16O = new TH1F("phi_16O", "phi of excited 16O ", 360, 0., 360);
    fh_phi_16O->GetXaxis()->SetTitle("angle / degree");
    fh_phi_16O->GetYaxis()->SetTitle("counts");

    fh_theta26_cm = new TH1F("theta26_cm", "theta between alpha and carbon in cm-system ", 360, 0, 360.);
    fh_theta26_cm->GetXaxis()->SetTitle("angle / degree");
    fh_theta26_cm->GetYaxis()->SetTitle("counts");

    fh_phi26_cm = new TH1F("phi26_cm", "phi between alpha and carbon in cm-system", 360, 0., 360.);
    fh_phi26_cm->GetXaxis()->SetTitle("angle / degree");
    fh_phi26_cm->GetYaxis()->SetTitle("counts");

    fh_theta_4He_cm = new TH1F("theta_4He_cm", "Theta 4He in cm-system", 180, 0., 180.);
    fh_theta_4He_cm->GetXaxis()->SetTitle("angle / degree");
    fh_theta_4He_cm->GetYaxis()->SetTitle("counts");

    fh_phi_4He_cm = new TH1F("phi_4He_cm", "phi 4He in cm-system", 360, 0., 360.);
    fh_phi_4He_cm->GetXaxis()->SetTitle("angle / degree");
    fh_phi_4He_cm->GetYaxis()->SetTitle("counts");

    fh_theta_bc_cm = new TH1F("theta_bc_cm", "Theta bc in cm-system", 180, 0., 180.);
    fh_theta_bc_cm->GetXaxis()->SetTitle("angle / degree");
    fh_theta_bc_cm->GetYaxis()->SetTitle("counts");

    fh_phi_bc_cm = new TH1F("phi_bc_cm", "phi bc in cm-system", 360, 0., 360.);
    fh_phi_bc_cm->GetXaxis()->SetTitle("angle / degree");
    fh_phi_bc_cm->GetYaxis()->SetTitle("counts");

    fh_phi_bc_cm_polar = new TH2F("phi_bc_cm_polar", "phi_bc_cm_polar", 360, 0., 360., 100, 0., 1000.);
    fh_phi_bc_cm_polar->GetXaxis()->SetTitle("angle / degree");
    fh_phi_bc_cm_polar->GetYaxis()->SetTitle("counts");

    fh_theta_12C_cm = new TH1F("theta_12C_cm", "Theta 12C in cm-system", 180, 0., 180.);
    fh_theta_12C_cm->GetXaxis()->SetTitle("angle / degree");
    fh_theta_12C_cm->GetYaxis()->SetTitle("counts");

    fh_phi_12C_cm = new TH1F("phi_12C_cm", "phi 12C in cm-system", 360, 0., 360.);
    fh_phi_12C_cm->GetXaxis()->SetTitle("angle / degree");
    fh_phi_12C_cm->GetYaxis()->SetTitle("counts");

    fh_theta26_simu = new TH1F("theta26_simu", "theta 26 simulation", 500, 0., 5);
    fh_theta26_simu->GetXaxis()->SetTitle("angle / degree");
    fh_theta26_simu->GetYaxis()->SetTitle("counts");

    fh_Erel = new TH1F("Erel", "Erel ", 1000, 0., 100);
    fh_Erel->GetXaxis()->SetTitle("Erel / MeV");
    fh_Erel->GetYaxis()->SetTitle("counts");

    fh_ErelL = new TH1F("ErelL", "Erel links", 1000, 0., 100);
    fh_ErelL->GetXaxis()->SetTitle("Erel / MeV");
    fh_ErelL->GetYaxis()->SetTitle("counts");

    fh_ErelR = new TH1F("ErelR", "Erel rechts ", 1000, 0., 100);
    fh_ErelR->GetXaxis()->SetTitle("Erel / MeV");
    fh_ErelR->GetYaxis()->SetTitle("counts");

    fh_ErelB = new TH1F("ErelB", "Erel both sides ", 1000, 0., 100);
    fh_ErelB->GetXaxis()->SetTitle("Erel / MeV");
    fh_ErelB->GetYaxis()->SetTitle("counts");

    fh_Erel_simu = new TH1F("Erel_simu", "Erel simulation", 1000, 0., 100);
    fh_Erel_simu->GetXaxis()->SetTitle("Erel / MeV");
    fh_Erel_simu->GetYaxis()->SetTitle("counts");

    fh_dErel_vs_x = new TH2F("Erel_vs_px", "Erel vs. px", 2000, -500., 500, 500, 0, 50.);
    fh_dErel_vs_x->GetXaxis()->SetTitle("px / MeV/c");
    fh_dErel_vs_x->GetYaxis()->SetTitle("Erel / MeV");

    fh_dErel_vs_y = new TH2F("Erel_vs_py", "Erel vs. py", 2000, -500., 500, 500, 0, 50.);
    fh_dErel_vs_y->GetXaxis()->SetTitle("py / MeV/c");
    fh_dErel_vs_y->GetYaxis()->SetTitle(" Erel / MeV");

    fh_p_vs_p = new TH1F("sum_p", "sum_p", 2000, 16500., 18500.);
    fh_p_vs_p->GetXaxis()->SetTitle("p / MeV/c");
    fh_p_vs_p->GetYaxis()->SetTitle("count");

    fh_dErel = new TH1F("dErel", "delta Erel ", 100, -5., 5.);
    fh_dErel->GetYaxis()->SetTitle("counts");
    fh_dErel->GetXaxis()->SetTitle("Delta Erel / MeV");

    fh_dtheta = new TH1F("dphi_bc_cm", "delta phi_bc_cm ", 1000, -5., 5.);
    fh_dtheta->GetYaxis()->SetTitle("counts");
    fh_dtheta->GetXaxis()->SetTitle("Delta phi_bc_cm / deg");

    fh_Erel_vs_pC = new TH2F("Erel_vs_theta", "Erel both sides vs. theta", 500, 0., 5, 500, 0, 50.);
    fh_Erel_vs_pC->GetXaxis()->SetTitle("angle / deg");
    fh_Erel_vs_pC->GetYaxis()->SetTitle("Erel / MeV");

    fh_Erel_vs_thetaMC = new TH2F("Erel_vs_thetaMC", "Erel both sides vs. theta MC", 500, 0., 5, 500, 0, 50.);
    fh_Erel_vs_thetaMC->GetXaxis()->SetTitle("angle / deg");
    fh_Erel_vs_thetaMC->GetYaxis()->SetTitle("Erel / MeV");

    fh_Erel_vs_thetabc = new TH2F("Erel_vs_phibccm", "Erel both sides vs. phi_bc_cm", 1000, 0, 100., 400, 0., 400.);
    fh_Erel_vs_thetabc->GetYaxis()->SetTitle("phi angle bc / deg");
    fh_Erel_vs_thetabc->GetXaxis()->SetTitle("Erel / MeV");

    fh_Erel_vs_thetabcMC =
        new TH2F("Erel_vs_phibccmMC", "Erel both sides vs. phi_bc_cm MCtrack", 1000, 0, 100., 400, 0., 400.);
    fh_Erel_vs_thetabcMC->GetYaxis()->SetTitle("phi angle bc / deg");
    fh_Erel_vs_thetabcMC->GetXaxis()->SetTitle("Erel / MeV");

    TCanvas* check = new TCanvas("CheckingGlobal", "CheckingGlobal", 10, 10, 900, 900);
    check->Divide(4, 4);
    check->cd(1);
    fh_chiy_vs_chix_He->Draw("colz");
    check->cd(2);
    fh_chiy_vs_chix_C->Draw("colz");
    check->cd(3);
    gPad->SetLogz();
    fh_Erel_vs_thetabc->Draw("colz");
    check->cd(4);
    gPad->SetLogz();
    fh_target_xy->Draw("colz");
    check->cd(5);
    fh_p_He->Draw();
    check->cd(6);
    fh_p_C->Draw();
    check->cd(7);
    // fh_ErelB->SetAxisRange(0., 50.,"X");
    fh_ErelB->Draw();
    check->cd(8);
    // fh_Erel->SetAxisRange(0., 50.,"X");
    fh_Erel->Draw();
    check->cd(9);
    fh_theta26->Draw();
    check->cd(10);
    fh_theta_bc_cm->Draw();
    check->cd(11);
    fh_phi_bc_cm->Draw();
    check->cd(12);
    gPad->SetLogz();
    fh_px_px->Draw("colz");
    check->cd(13);
    gPad->SetLogz();
    fh_py_py->Draw("colz");
    check->cd(14);
    gPad->SetLogz();
    fh_pz_pz->Draw("colz");
    check->cd(15);
    gPad->SetLogz();
    fh_p_vs_p->Draw("colz");
    check->cd(16);
    gPad->SetLogz();
    fh_Erel_vs_pC->Draw("colz");

    if (fSimu)
    {
        TCanvas* checkMC = new TCanvas("CheckingGlobalMC", "CheckingGlobalMC", 10, 10, 900, 900);
        checkMC->Divide(3, 3);
        checkMC->cd(1);
        fh_dx->Draw();
        checkMC->cd(2);
        fh_dy->Draw();
        checkMC->cd(3);
        fh_dpxC->Draw();
        checkMC->cd(4);
        fh_dpyC->Draw();
        checkMC->cd(5);
        fh_dpzC->Draw();
        checkMC->cd(6);
        gPad->SetLogz();
        fh_dpy_dpx->Draw("colz");
        checkMC->cd(7);
        fh_dpxHe->Draw();
        checkMC->cd(8);
        fh_dpyHe->Draw();
        checkMC->cd(9);

        fh_dpzHe->Draw();
    }

    return kSUCCESS;
}

void R3BGlobalAnalysisS494::Exec(Option_t* option)
{
    if (fNEvents / 100000. == (int)fNEvents / 100000)
        std::cout << "\rEvents: " << fNEvents << " / " << maxevent << " (" << (int)(fNEvents * 100. / maxevent)
                  << " %) " << std::flush;

    //    cout << "New event " << fNEvents << endl;

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

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
    Double_t theta_16, theta_26;
    Double_t costh26;
    Double_t chiHex, chiHey = 1.e+36, chiCx = 1.e+36, chiCy = 1.e+36;

    Bool_t is_alpha = false;
    Bool_t is_carbon = false;
    Bool_t is_tracked = false;

    Int_t nHitsTrack = 0;
    Int_t nHitsMCTrack = 0;

    if (fTrack)
    {

        // read in Track parameter
        nHitsTrack = fTrack->GetEntriesFast();
        // cout << "Track hits: " << nHitsTrack << endl;
        for (Int_t l = 0; l < nHitsTrack; l++)
        {
            R3BTrack* aTrack = (R3BTrack*)fTrack->At(l);

            LOG(DEBUG) << "Charge " << aTrack->GetQ() << endl;

            if (aTrack->GetQ() == 2)
            {
                // 4He
                XHe = aTrack->GetX();
                YHe = aTrack->GetY();
                ZHe = aTrack->GetZ();

                pHex = aTrack->GetPx();
                pHey = aTrack->GetPy();
                pHez = aTrack->GetPz();

                chiHex = aTrack->GetChix();
                chiHey = aTrack->GetChiy();

                is_alpha = true;
                alpha.SetPxPyPzE(pHex, pHey, pHez, sqrt(pow(pHex, 2) + pow(pHey, 2) + pow(pHez, 2) + pow(mHe, 2)));
                /*
                   cout << "******************************************" << endl;
                   cout << "Track In 4He"
                             << "x " << XHe << " y " << YHe << " z " << ZHe << endl;
                   cout << "px " << pHex << " py " << pHey << " z " << pHez << endl;
                 */
            }
            if (aTrack->GetQ() == 6)
            {
                // 12C
                XC = aTrack->GetX();
                YC = aTrack->GetY();
                ZC = aTrack->GetZ();

                pCx = aTrack->GetPx();
                pCy = aTrack->GetPy();
                pCz = aTrack->GetPz();

                chiCx = aTrack->GetChix();
                chiCy = aTrack->GetChiy();

                is_carbon = true;
                carbon.SetPxPyPzE(pCx, pCy, pCz, sqrt(pow(pCx, 2) + pow(pCy, 2) + pow(pCz, 2) + pow(mC, 2)));
                /*
                  cout << "******************************************" << endl;
                  cout << "Track In 12C"
                            << "x " << XC << " y " << YC << " z " << ZC << endl;
                  cout << "px " << pCx << " py " << pCy << " z " << pCz << endl;
                 */
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

                // cout << "******************************************" << endl;
                // cout << "Track In 16O"
                //           << "x " << Xf << " y " << Yf << " z " << Zf << endl;
                // cout << "px " << Pxf << " py " << Pyf << " z " << Pzf << endl;
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
            // cout << "Original MC Data is analyzed" << endl;
            R3BMCTrack* aTrack = (R3BMCTrack*)fMCTrack->At(l);

            Int_t PID = aTrack->GetPdgCode();
            Int_t mother = aTrack->GetMotherId();
            LOG(DEBUG) << "PID " << PID << endl;
            if (mother < 0)
            {
                if (PID == 1000020040)
                {
                    // 4He
                    XHe_mc = aTrack->GetStartX();
                    YHe_mc = aTrack->GetStartY();
                    ZHe_mc = aTrack->GetStartZ();
                    THe_mc = aTrack->GetStartT();

                    pHex_mc = aTrack->GetPx() * 1000.;
                    pHey_mc = aTrack->GetPy() * 1000.;
                    pHez_mc = aTrack->GetPz() * 1000.;

                    is_alpha = true;
                    alphaMC.SetPxPyPzE(pHex_mc,
                                       pHey_mc,
                                       pHez_mc,
                                       sqrt(pow(pHex_mc, 2) + pow(pHey_mc, 2) + pow(pHez_mc, 2) + pow(mHe, 2)));

                    LOG(DEBUG) << "******************************************" << endl;
                    LOG(DEBUG) << "Track In 4He"
                               << "x " << XHe_mc << " y " << YHe_mc << " z " << ZHe_mc << endl;
                    LOG(DEBUG) << "px " << pHex_mc << " py " << pHey_mc << " pz " << pHez_mc << endl;
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

                    LOG(DEBUG) << "******************************************" << endl;
                    LOG(DEBUG) << "Track In 12C"
                               << "x " << XC_mc << " y " << YC_mc << " z " << ZC_mc << endl;
                    LOG(DEBUG) << "px " << pCx_mc << " py " << pCy_mc << " pz " << pCz_mc << endl;
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
                    Pf_tot_mc = sqrt((Pxf_mc * Pxf_mc) + (Pyf_mc * Pyf_mc) + (Pzf_mc * Pzf_mc));

                    LOG(DEBUG) << "******************************************" << endl;
                    LOG(DEBUG) << "Track In 16O"
                               << "x " << Xf_mc << " y " << Yf_mc << " z " << Zf_mc << endl;
                    LOG(DEBUG) << "px " << Pxf_mc << " py " << Pyf_mc << " z " << Pzf_mc << endl;
                }
            }
        }
    }

    // new version of analysis
    if (is_carbon && is_alpha)
    {
        fh_chiy_vs_chix_He->Fill(chiHex, chiHey);
        fh_chiy_vs_chix_C->Fill(chiCx, chiCy);
        fh_chi2->Fill(chiHex + chiHey);
        fh_chi2->Fill(chiCx + chiCy);

        if (chiHex < fcut_chiX && chiHey < fcut_chiY && chiCx < fcut_chiX && chiCy < fcut_chiY)
        //			&&((alpha.Px() > 0 && carbon.Px() < 0) || (alpha.Px() < 0 && carbon.Px() > 0)))
        {
            is_tracked = true;

            fh_target_xy->Fill(XHe * 100., YHe * 100.);
            fh_target_xy->Fill(XC * 100., YC * 100.);

            fh_px_He->Fill(alpha.Px());
            fh_py_He->Fill(alpha.Py());
            fh_pz_He->Fill(alpha.Pz());

            fh_px_C->Fill(carbon.Px());
            fh_py_C->Fill(carbon.Py());
            fh_pz_C->Fill(carbon.Pz());

            fh_px_px->Fill(alpha.Px(), carbon.Px());
            fh_py_py->Fill(alpha.Py(), carbon.Py());
            fh_pz_pz->Fill(alpha.Pz(), carbon.Pz());

            pa = alpha.Vect();
            fh_p_He->Fill(pa.Mag());
            pc = carbon.Vect();
            fh_p_C->Fill(pc.Mag());

            // Calculate angle between alpha and C
            if (alpha.Pz() == 0 || carbon.Pz() == 0)
                return;
            /*  if (alpha.Pz() < 3700 || alpha.Pz() > 5000)
                  return;
              if (carbon.Pz() < 12700 || carbon.Pz() > 13300)
                  return;*/

            fh_p_vs_p->Fill(pa.Mag() + pc.Mag());
            theta_26 = alpha.Angle(carbon.Vect()) * TMath::RadToDeg(); /// angle alpha carbon (theta)
            fh_theta26->Fill(theta_26);

            /** Calculate invariant mass and relative energy **/
            m_inva = (alpha + carbon).M(); // invariant mass
            Erel = m_inva - mHe - mC;      // relative Energy
            fh_Erel->Fill(Erel);           // relative Energy plot

            if (alpha.Px() < 0. && carbon.Px() < 0.)
                fh_ErelR->Fill(Erel);
            if (alpha.Px() > 0. && carbon.Px() > 0.)
                fh_ErelL->Fill(Erel);

            fh_Erel_vs_pC->Fill(theta_26, Erel);

            if ((alpha.Px() > 0. && carbon.Px() < 0.) || (alpha.Px() < 0. && carbon.Px() > 0.))
            {

                // if (theta_26 > 0.7)
                fh_ErelB->Fill(Erel);
            }
            fh_dErel_vs_x->Fill(carbon.Px(), Erel);
            fh_dErel_vs_y->Fill(carbon.Py(), Erel);
            fh_dErel_vs_x->Fill(alpha.Px(), Erel);
            fh_dErel_vs_y->Fill(alpha.Py(), Erel);

            /** transfer to cm system and rotate **/

            oxygen = alpha + carbon; // excited oxygen
            // cout<<"gamma "<<oxygen.Gamma()<<endl;
            Double_t theta_16O = oxygen.Theta() * TMath::RadToDeg();
            fh_theta_16O->Fill(theta_16O); // theta oxygen
            Double_t phi_16O = oxygen.Phi() * TMath::RadToDeg();
            if (phi_16O < 0)
                phi_16O += 360.;
            fh_phi_16O->Fill(phi_16O);

            // photon.SetPxPyPzE(oxygen.Px, oxygen.Py, 0., sqrt(pow(oxygen.Px, 2) + pow(oxygen.Py, 2)));

            // transfer to cm system and make some rotations
            // boost them to centre of mass
            alpha_cm = alpha;
            carbon_cm = carbon;
            oxygen_cm = oxygen;
            alpha_cm.Boost(-oxygen.BoostVector());
            carbon_cm.Boost(-oxygen.BoostVector());
            oxygen_cm.Boost(-oxygen.BoostVector());
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

            Double_t phi = (alpha_cm.Phi() - carbon_cm.Phi()) * TMath::RadToDeg();
            fh_phi26_cm->Fill(phi); // phi between alpha + carbon in cm
            Double_t theta = alpha_cm.Angle(carbon_cm.Vect()) * TMath::RadToDeg();
            fh_theta26_cm->Fill(theta); // angle between alpha + carbon in cm

            fh_theta_4He_cm->Fill(alpha_cm.Theta() * TMath::RadToDeg());
            fh_phi_4He_cm->Fill(alpha_cm.Phi() * TMath::RadToDeg());
            fh_theta_12C_cm->Fill(carbon_cm.Theta() * TMath::RadToDeg());
            fh_phi_12C_cm->Fill(carbon_cm.Phi() * TMath::RadToDeg());

            fh_theta_bc_cm->Fill(alpha_cm.Theta() * TMath::RadToDeg());

            phi_bc_cm = (oxygen.Phi() - alpha_cm.Phi()) * TMath::RadToDeg();
            if (phi_bc_cm < 0)
                phi_bc_cm += 360.;
            fh_phi_bc_cm->Fill(phi_bc_cm);
            fh_phi_bc_cm_polar->Fill(phi_bc_cm, 1);
            fh_Erel_vs_thetabc->Fill(Erel, phi_bc_cm);

            fh_px_px_cm->Fill(alpha_cm.Px(), carbon_cm.Px());
            fh_py_py_cm->Fill(alpha_cm.Py(), carbon_cm.Py());
            fh_pz_pz_cm->Fill(alpha_cm.Pz(), carbon_cm.Pz());

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

    if (fSimu && nHitsMCTrack > 0)
    {

        paMC = alphaMC.Vect();
        pcMC = carbonMC.Vect();
        Double_t theta_26MC = alphaMC.Angle(carbonMC.Vect()) * TMath::RadToDeg(); /// angle alpha carbon (theta)
        /** Calculate invariant mass and relative energy **/
        m_invaMC = (alphaMC + carbonMC).M(); // invariant mass
        ErelMC = m_invaMC - mHe - mC;        // relative Energy
        fh_Erel_vs_thetaMC->Fill(theta_26MC, ErelMC);

        /** transfer to cm system and rotate **/

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

        phiMC_bc_cm = (oxygenMC.Phi() - alphaMC_cm.Phi()) * TMath::RadToDeg();
        if (phiMC_bc_cm < 0)
            phiMC_bc_cm += 360.;
        fh_Erel_vs_thetabcMC->Fill(ErelMC, phiMC_bc_cm);
        fh_Erel_simu->Fill(ErelMC);

        if (is_tracked)
        {
            fh_dErel->Fill(ErelMC - Erel);
            fh_dtheta->Fill(phiMC_bc_cm - phi_bc_cm);

            fh_dx->Fill((XHe_mc - XHe) * 100.);
            fh_dx->Fill((XC_mc - XC) * 100.);
            fh_dy->Fill((YHe_mc - YHe) * 100.);
            fh_dy->Fill((YC_mc - YC) * 100.);

            fh_dpxC->Fill((pCx_mc - pCx) / pCx_mc * 100.);
            fh_dpxHe->Fill((pHex_mc - pHex) / pHex_mc * 100.);
            fh_dpyC->Fill((pCy_mc - pCy) / pCy_mc * 100.);
            fh_dpyHe->Fill((pHey_mc - pHey) / pHey_mc * 100.);
            fh_dpzC->Fill((pCz_mc - pCz) / pCz_mc * 100.);
            fh_dpzHe->Fill((pHez_mc - pHez) / pHez_mc * 100.);
            fh_dpC->Fill((pcMC.Mag() - pc.Mag()) / pcMC.Mag() * 100.);
            fh_dpHe->Fill((paMC.Mag() - pa.Mag()) / paMC.Mag() * 100.);

            fh_x_dpx->Fill((pCx_mc - pCx) / pCx_mc * 100., XC);
            fh_x_dpx->Fill((pHex_mc - pHex) / pHex_mc * 100., XHe);
            fh_y_dpy->Fill((pCy_mc - pCy) / pCy_mc * 100., YC);
            fh_y_dpy->Fill((pHey_mc - pHey) / pHey_mc * 100., YHe);
            fh_dpy_dpx->Fill((pCx_mc - pCx) / pCx_mc * 100., (pCy_mc - pCy) / pCy_mc * 100.);
            fh_dpy_dpx->Fill((pHex_mc - pHex) / pHex_mc * 100., (pHey_mc - pHey) / pHey_mc * 100.);
        }
    }

    // second, old version of analysis
    if (is_carbon && is_alpha && 1 == 0)
    {
        fh_chiy_vs_chix_He->Fill(chiHex, chiHey);
        fh_chiy_vs_chix_C->Fill(chiCx, chiCy);

        if (chiHex < 1e10 && chiHey < 1e10 && chiCx < 1e10 && chiCy < 1e10)
        {
            fh_target_xy->Fill(XHe * 100., YHe * 100.);

            fh_px_He->Fill(pHex);
            fh_py_He->Fill(pHey);
            fh_pz_He->Fill(pHez);

            fh_px_C->Fill(pCx);
            fh_py_C->Fill(pCy);
            fh_pz_C->Fill(pCz);

            fh_px_px->Fill(pHex, pCx);
            fh_py_py->Fill(pHey, pCy);
            fh_pz_pz->Fill(pHez, pCz);

            // Calculate angle between alphs and C
            if (pCz == 0 || pHez == 0)
                return;

            costh26 = (pHex * pCx + pHey * pCy + pHez * pCz) /
                      (sqrt(pHex * pHex + pHey * pHey + pHez * pHez) * sqrt(pCx * pCx + pCy * pCy + pCz * pCz));

            theta_26 = acos(costh26) * 180. / 3.14159; // opening angle

            fh_theta26->Fill(theta_26);

            Double_t pHe = sqrt(pow(pHex, 2) + pow(pHey, 2) + pow(pHez, 2));
            Double_t pC = sqrt(pow(pCx, 2) + pow(pCy, 2) + pow(pCz, 2));
            Double_t eHe = sqrt(pow(pHe, 2) + pow(mHe, 2)) - mHe;
            Double_t eC = sqrt(pow(pC, 2) + pow(mC, 2)) - mC;

            fh_p_He->Fill(pHe);
            fh_p_C->Fill(pC);

            Double_t Erela = sqrt(pow((mHe + mC + eHe + eC), 2) - pow(pHe, 2) - pow(pC, 2) -
                                  2 * pHe * pC * cos(theta_26 * 3.1415 / 180.)) -
                             (mHe + mC); // Erel

            Double_t ptota = sqrt(pow(pHe, 2) + pow(pC, 2) + 2 * pHe * pC * cos(theta_26 * 3.1415 / 180.));

            Double_t m_inv = sqrt(mHe * mHe + mC * mC + 2. * sqrt(pC * pC + mC * mC) * sqrt(pHe * pHe + mHe * mHe) -
                                  2. * pHe * pC * cos(theta_26 * 3.1415 / 180.));
            Double_t Erelb = m_inv - mHe - mC;

            fh_Erel->Fill(Erela);
            if (pHex < 0. && pCx < 0.)
                fh_ErelR->Fill(Erela);
            if (pHex > 0. && pCx > 0.)
                fh_ErelL->Fill(Erela);
            if ((pHex > 0. && pCx < 0.) || (pHex < 0. && pCx > 0.))
                fh_ErelB->Fill(Erela);

            //	fh_dErel_vs_x->Fill(xTest * 100., Erela - 1.);
            //	fh_dErel_vs_y->Fill(yTest * 100., Erela - 1.);
            // cout << "Theta 26: " << theta_26 << " Erel: " << Erela << " " << Erelb << endl;

            // transfer to cm system and make some rotations

            px = pHex + pCx;
            py = pHey + pCy;
            pz = pHez + pCz;

            if (pz != 0.)
            {
                theta_16 = atan2(sqrt(pow(px, 2) + pow(py, 2)), pz) * 180. / 3.14159265;
            }

            fh_theta_16O->Fill(theta_16);

            Double_t n2[10], eaa, vaa, g2, v2[10], lt2[10][10];
            Double_t tmp1[10], tmp2[10], tmp3[10], tmp4[10];
            Double_t pxO_1, pyO_1, pzO_1, esum_3;
            Double_t psum_1, thcm, phcm, tanphicm;

            esum_3 = sqrt(pow((sqrt(pow(pHe, 2) + pow(pC, 2) + 2 * pHe * pC * cos(theta_26 * 3.14159 / 180.))), 2) +
                          pow(mO, 2)) -
                     mO;

            psum_1 = sqrt(pow(px, 2) + pow(py, 2) + pow(pz, 2));
            thcm = acos(pz / psum_1);
            tanphicm = py / px;

            if (px > 0)
            {
                phcm = atan(tanphicm) * 180. / 3.14159265;
            }

            if (px < 0. && py > 0.)
            {
                phcm = (atan(tanphicm) * 180. / 3.14159265) + 180.;
            }

            if (px < 0. && py < 0.)
            {
                phcm = (atan(tanphicm) * 180. / 3.14159265) - 180.;
            }

            if (phcm < 0)
            {
                phcm = phcm + 360.;
            }
            phcm = phcm * 3.14159265 / 180.;

            //            fh_theta_16O_cm->Fill(thcm);
            //            fh_phi_16O_cm->Fill(phcm);

            n2[1] = px / ptota;
            n2[2] = py / ptota;
            n2[3] = pz / ptota;

            //       eaa   = esum_3;
            //     velocity of 6Li
            //       vaa=sqrt(eaa*eaa+2*mO*eaa)/(eaa+mO);

            //       cout<< " va1 "<< vaa<<endl;

            eaa = sqrt(mO * mO + psum_1 * psum_1);
            vaa = psum_1 / eaa;

            //       cout<< " va2 "<< vaa<<endl;

            g2 = 1.0 / sqrt(1.0 - vaa * vaa);

            v2[1] = vaa * n2[1];
            v2[2] = vaa * n2[2];
            v2[3] = vaa * n2[3];

            // Lorentz transformation
            lt2[1][1] = g2;
            lt2[1][2] = -1. * g2 * v2[1];
            lt2[1][3] = -1. * g2 * v2[2];
            lt2[1][4] = -1. * g2 * v2[3];
            lt2[2][1] = lt2[1][2];
            lt2[2][2] = 1.0 + (g2 - 1.0) * n2[1] * n2[1];
            lt2[2][3] = (g2 - 1.0) * n2[1] * n2[2];
            lt2[2][4] = (g2 - 1.0) * n2[1] * n2[3];
            lt2[3][1] = lt2[1][3];
            lt2[3][2] = lt2[2][3];
            lt2[3][3] = 1.0 + (g2 - 1.0) * n2[2] * n2[2];
            lt2[3][4] = (g2 - 1.0) * n2[2] * n2[3];
            lt2[4][1] = lt2[1][4];
            lt2[4][2] = lt2[2][4];
            lt2[4][3] = lt2[3][4];
            lt2[4][4] = 1.0 + (g2 - 1.0) * n2[3] * n2[3];

            //      four momentum of nucleus b
            tmp1[1] = sqrt(mHe * mHe + pHe * pHe);
            tmp1[2] = pHex;
            tmp1[3] = pHey;
            tmp1[4] = pHez;

            //     four momentum of nucleus c
            tmp2[1] = sqrt(mC * mC + pC * pC);
            tmp2[2] = pCx;
            tmp2[3] = pCy;
            tmp2[4] = pCz;

            //     four momentum of nucleus a for a check of the transformation
            tmp4[1] = sqrt(mO * mO + psum_1 * psum_1);
            tmp4[2] = px;
            tmp4[3] = py;
            tmp4[4] = pz;
            //       cout <<"compare " << tmp4[1]<<"  "<< esum_3+mO<<endl;

            /*
                            cout <<"in " << tmp4[1]<<endl;
                            cout <<"in " << tmp4[2]<<endl;
                            cout <<"in " << tmp4[3]<<endl;
                            cout <<"in " << tmp4[4]<<endl;
            */
            for (Int_t i1 = 1; i1 < 5; i1++)
            {
                tmp3[i1] = 0.0;
                for (Int_t i2 = 1; i2 < 5; i2++)
                {
                    tmp3[i1] = tmp3[i1] + lt2[i1][i2] * tmp1[i2];
                }
            }

            Double_t pHex_1, pHey_1, pHez_1, pHe_1, pCx_1, pCy_1, pCz_1, pC_1;

            pHex_1 = tmp3[2];
            pHey_1 = tmp3[3];
            pHez_1 = tmp3[4];

            for (Int_t i1 = 1; i1 < 5; i1++)
            {
                tmp3[i1] = 0.0;
                for (Int_t i2 = 1; i2 < 5; i2++)
                {
                    tmp3[i1] = tmp3[i1] + lt2[i1][i2] * tmp2[i2];
                }
            }

            pCx_1 = tmp3[2];
            pCy_1 = tmp3[3];
            pCz_1 = tmp3[4];

            for (Int_t i1 = 1; i1 < 5; i1++)
            {
                tmp3[i1] = 0.0;
                for (Int_t i2 = 1; i2 < 5; i2++)
                {
                    tmp3[i1] = tmp3[i1] + lt2[i1][i2] * tmp4[i2];
                }
            }

            pxO_1 = tmp3[2];
            pyO_1 = tmp3[3];
            pzO_1 = tmp3[4];

            /*
                            cout <<"out " << tmp3[1]<<endl;
                            cout <<"out " << tmp3[2]<<endl;
                            cout <<"out " << tmp3[3]<<endl;
                            cout <<"out " << tmp3[4]<<endl;
            */

            Double_t pHex_cm, pHey_cm, pHez_cm, pHe_cm;
            Double_t pCx_cm, pCy_cm, pCz_cm, pC_cm;
            Double_t thet_cm_He, thet_cm_C, thet;
            Double_t theta_cm_He, theta_cm_C, theta;
            Double_t pxyd_cm, costhetacm_He, tanphi_d, phicm_He, pxyhe_cm;
            Double_t costhetacm_C, tanphi_he, phicm_C, theta_cm_26;
            Double_t phi_cm_26, pxO_cm, pyO_cm, pzO_cm;

            //     Vector for rotation
            n2[1] = py / sqrt(px * px + py * py);
            n2[2] = -px / sqrt(px * px + py * py);
            n2[3] = 0;

            //       cout <<"winkel " << thcm<<"  "<<theta6/180.*3.14159265<<endl;

            lt2[1][1] = cos(thcm) + n2[1] * n2[1] * (1. - cos(thcm));
            lt2[1][2] = n2[1] * n2[2] * (1. - cos(thcm)) - n2[3] * sin(thcm);
            lt2[1][3] = n2[1] * n2[3] * (1. - cos(thcm)) + n2[2] * sin(thcm);
            lt2[2][1] = n2[2] * n2[1] * (1. - cos(thcm)) + n2[3] * sin(thcm);
            lt2[2][2] = cos(thcm) + n2[2] * n2[2] * (1. - cos(thcm));
            lt2[2][3] = n2[2] * n2[3] * (1. - cos(thcm)) - n2[1] * sin(thcm);
            lt2[3][1] = n2[3] * n2[1] * (1. - cos(thcm)) - n2[2] * sin(thcm);
            lt2[3][2] = n2[3] * n2[2] * (1. - cos(thcm)) + n2[1] * sin(thcm);
            lt2[3][3] = cos(thcm) + n2[3] * n2[3] * (1. - cos(thcm));

            //      four momentum of nucleus b
            tmp1[1] = pHex_1;
            tmp1[2] = pHey_1;
            tmp1[3] = pHez_1;

            //     four momentum of nucleus c
            tmp2[1] = pCx_1;
            tmp2[2] = pCy_1;
            tmp2[3] = pCz_1;

            //     four momentum of nucleus a for a check of the transformation
            tmp4[1] = px;
            tmp4[2] = py;
            tmp4[3] = pz;
            //       cout <<"compare " << tmp4[1]<<"  "<< esum_3+mO<<endl;

            /*
                            cout <<"in1 " << tmp4[1]<<endl;
                            cout <<"in1 " << tmp4[2]<<endl;
                            cout <<"in1 " << tmp4[3]<<endl;
            */
            Double_t pHex_2, pHey_2, pHez_2, pHe_2, pCx_2, pCy_2, pCz_2, pC_2;
            Double_t pxO_2, pyO_2, pzO_2;

            for (Int_t i1 = 1; i1 < 4; i1++)
            {
                tmp3[i1] = 0.0;
                for (Int_t i2 = 1; i2 < 4; i2++)
                {
                    tmp3[i1] = tmp3[i1] + lt2[i1][i2] * tmp1[i2];
                }
            }

            pHex_cm = tmp3[1];
            pHey_cm = tmp3[2];
            pHez_cm = tmp3[3];

            for (Int_t i1 = 1; i1 < 4; i1++)
            {
                tmp3[i1] = 0.0;
                for (Int_t i2 = 1; i2 < 4; i2++)
                {
                    tmp3[i1] = tmp3[i1] + lt2[i1][i2] * tmp2[i2];
                }
            }

            pCx_cm = tmp3[1];
            pCy_cm = tmp3[2];
            pCz_cm = tmp3[3];

            for (Int_t i1 = 1; i1 < 4; i1++)
            {
                tmp3[i1] = 0.0;
                for (Int_t i2 = 1; i2 < 4; i2++)
                {
                    tmp3[i1] = tmp3[i1] + lt2[i1][i2] * tmp4[i2];
                }
            }

            pxO_cm = tmp3[1];
            pyO_cm = tmp3[2];
            pzO_cm = tmp3[3];

            /*
                            cout <<"out1 " << tmp3[1]<<endl;
                            cout <<"out1 " << tmp3[2]<<endl;
                            cout <<"out1 " << tmp3[3]<<endl;
            */

            // 4He fragment

            pHe_cm = sqrt(pHex_cm * pHex_cm + pHey_cm * pHey_cm + pHez_cm * pHez_cm);

            costhetacm_He = pHez_cm / pHe_cm;
            theta_cm_He = acos(costhetacm_He) * 180. / 3.14159265;
            tanphi_d = pHey_cm / pHex_cm;

            if (pHex_cm > 0)
            {
                phicm_He = atan(tanphi_d) * 180. / 3.14159265;
            }

            if (pHex_cm < 0. && pHey_cm > 0.)
            {
                phicm_He = (atan(tanphi_d) * 180. / 3.14159265) + 180.;
            }

            if (pHex_cm < 0. && pHey_cm < 0.)
            {
                phicm_He = (atan(tanphi_d) * 180. / 3.14159265) - 180.;
            }

            if (phicm_He < 0)
            {
                phicm_He = phicm_He + 360.;
            }
            fh_theta_4He_cm->Fill(theta_cm_He);
            fh_phi_4He_cm->Fill(phicm_He);

            // 12C fragment

            pC_cm = sqrt(pCx_cm * pCx_cm + pCy_cm * pCy_cm + pCz_cm * pCz_cm);

            costhetacm_C = pCz_cm / pC_cm;
            theta_cm_C = acos(costhetacm_C) * 180. / 3.14159265;
            tanphi_he = pCy_cm / pCx_cm;

            if (pCx_cm > 0.)
            {
                phicm_C = atan(tanphi_he) * 180. / 3.14159265;
            }

            if (pCx_cm < 0. && pCy_cm > 0.)
            {
                phicm_C = (atan(tanphi_he) * 180. / 3.14159265) + 180.;
            }

            if (pCx_cm < 0. && pCy_cm < 0.)
            {
                phicm_C = (atan(tanphi_he) * 180. / 3.14159265) - 180.;
            }

            if (phicm_C < 0.)
            {
                phicm_C = phicm_C + 360.;
            }
            fh_theta_12C_cm->Fill(theta_cm_C);
            fh_phi_12C_cm->Fill(phicm_C);

            theta_cm_26 = theta_cm_He + theta_cm_C;
            phi_cm_26 = phicm_He - phicm_C;

            fh_theta26_cm->Fill(theta_cm_26);
            fh_phi26_cm->Fill(phi_cm_26);

            fh_px_px_cm->Fill(pHex_cm, pCx_cm);
            fh_py_py_cm->Fill(pHey_cm, pCy_cm);
            fh_pz_pz_cm->Fill(pHez_cm, pCz_cm);

        } // end if chi2
    }     // end if trackHits>1

    fNEvents += 1;
    return;
}

void R3BGlobalAnalysisS494::FinishEvent()
{
    fTrack->Clear();
    if (fMCTrack)
        fMCTrack->Clear();
}

void R3BGlobalAnalysisS494::FinishTask()
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

    fh_px_He->Write();
    fh_py_He->Write();
    fh_pz_He->Write();
    fh_p_He->Write();
    fh_px_C->Write();
    fh_py_C->Write();
    fh_pz_C->Write();
    fh_p_C->Write();

    fh_chi2->Write();

    fh_dx->Write();
    fh_dy->Write();
    fh_dz->Write();
    fh_dpxC->Write();
    fh_dpyC->Write();
    fh_dpzC->Write();
    fh_dpC->Write();
    fh_dpxHe->Write();
    fh_dpyHe->Write();
    fh_dpzHe->Write();
    fh_dpHe->Write();

    fh_thetax_dpx->Write();
    fh_thetax_dpx_abs->Write();
    fh_thetay_dpy->Write();
    fh_thetax_dpy->Write();
    fh_thetay_dpx->Write();
    fh_x_dpx->Write();
    fh_y_dpy->Write();
    fh_dpy_dpx->Write();

    fh_theta26->Write();
    fh_theta26_cm->Write();
    fh_theta_16O->Write();
    fh_phi_16O->Write();
    fh_phi26_cm->Write();
    fh_theta_4He_cm->Write();
    fh_phi_4He_cm->Write();
    fh_theta_bc_cm->Write();
    fh_phi_bc_cm->Write();
    fh_phi_bc_cm_polar->Write();
    fh_theta_12C_cm->Write();
    fh_phi_12C_cm->Write();
    fh_Erel->Write();
    fh_ErelB->Write();
    fh_ErelR->Write();
    fh_ErelL->Write();
    fh_theta26_simu->Write();
    fh_Erel_simu->Write();
    fh_chiy_vs_chix_He->Write();
    fh_chiy_vs_chix_C->Write();
    fh_dErel_vs_x->Write();
    fh_dErel_vs_y->Write();
    fh_px_px->Write();
    fh_py_py->Write();
    fh_pz_pz->Write();
    fh_px_px_cm->Write();
    fh_py_py_cm->Write();
    fh_pz_pz_cm->Write();
    fh_Erel_vs_pC->Write();
    fh_Erel_vs_thetaMC->Write();
    fh_dErel->Write();
    fh_dtheta->Write();
    fh_p_vs_p->Write();
    fh_Erel_vs_thetabc->Write();
    fh_Erel_vs_thetabcMC->Write();
}
ClassImp(R3BGlobalAnalysisS494)
