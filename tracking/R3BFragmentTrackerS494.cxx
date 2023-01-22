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

/******************************************************************************
 *****                   Task for fitting a fragment                      *****
 *****     Created Nov 2016 by D. Kresan                                  *****
 *****     Comments to the algorithm implementation from R. Plag          *****
 *****     Implemented ideal fitter, July 2017, D. Kresan                 *****
 ******************************************************************************/

#include "R3BFragmentTrackerS494.h"
#include "FairIon.h"
#include "G4NistManager.hh"
#include "R3BFi4HitItem.h"
#include "R3BFragmentFitterGeneric.h"
#include "R3BGladFieldMap.h"
#include "R3BHit.h"
#include "R3BMCTrack.h"
#include "R3BPspPoint.h"
#include "R3BTGeoPar.h"
#include "R3BTPropagator.h"
#include "R3BTofdPoint.h"
#include "R3BTrack.h"
#include "R3BTrackingDetector.h"
#include "R3BTrackingParticle.h"
#include "R3BTrackingSetup.h"

#include "R3BCalifaMappedData.h"
#include "R3BFiberMAPMTCalData.h"
#include "R3BFiberMAPMTHitData.h"
#include "R3BFiberMAPMTMappedData.h"
#include "R3BTofdCalData.h"
#include "R3BTofdHitData.h"
#include "R3BTofdMappedData.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TArc.h"
#include "TClonesArray.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include <fstream>
#include <iostream>
using namespace std;

#define SPEED_OF_LIGHT 29.9792458 // cm/ns
//#define Amu 0.938272
//#define Fair_Amu 0.931494028

R3BFragmentTrackerS494::R3BFragmentTrackerS494(const char* name, Bool_t vis, Int_t verbose)
    : FairTask(name, verbose)
    , fFieldPar(NULL)
    , fPropagator(NULL)
    , fArrayMCTracks(NULL)
    , fDetectors(new R3BTrackingSetup())
    , fDetectorsLeft(new R3BTrackingSetup())
    , fDetectorsRight(new R3BTrackingSetup())
    , fArrayFragments(new TClonesArray("R3BTrackingParticle"))
    , fNEvents(0)
    , fNEventsLeft(0)
    , fNEventsRight(0)
    , fVis(vis)
    , fFitter(nullptr)
    , fEnergyLoss(kTRUE)
    , fSimu(kFALSE)
    , fForward(kTRUE)
    , fBfield(-1710.)
    , fOptimizeGeometry(kFALSE)
    , fTrackItems(new TClonesArray("R3BTrack"))
    , fNofTrackItems()
    , fTofdHitItems(new TClonesArray("R3BTofdHitData"))
    , fFi23aHitItems(new TClonesArray("R3BFiberMAPMTHitData"))
    , fFi23bHitItems(new TClonesArray("R3BFiberMAPMTHitData"))
    , fFi30HitItems(new TClonesArray("R3BFiberMAPMTHitData"))
    , fFi31HitItems(new TClonesArray("R3BFiberMAPMTHitData"))
    , fFi32HitItems(new TClonesArray("R3BFiberMAPMTHitData"))
    , fFi33HitItems(new TClonesArray("R3BFiberMAPMTHitData"))
    , fNofTofdHitItems(0)
    , fNofFi23aHitItems(0)
    , fNofFi23bHitItems(0)
    , fNofFi30HitItems(0)
    , fNofFi31HitItems(0)
    , fNofFi32HitItems(0)
    , fNofFi33HitItems(0)
    , fWriteOut(kFALSE)
    , fPmax(20000)
    , fPmin(0)
{
    // this is the list of detectors (active areas) we use for tracking
/*    
    fDetectorsLeft->AddDetector("target", kTarget, "TargetGeoPar");
    fDetectorsLeft->AddDetector("fi23a", kTargetGlad, "fi23aGeoPar", "Fi23aHit");
    fDetectorsLeft->AddDetector("fi23b", kTargetGlad, "fi23bGeoPar", "Fi23bHit");
    fDetectorsLeft->AddDetector("fi30", kAfterGlad, "fi30GeoPar", "Fi30Hit");
    fDetectorsLeft->AddDetector("fi32", kAfterGlad, "fi32GeoPar", "Fi32Hit");
    fDetectorsLeft->AddDetector("tofd", kTof, "tofdGeoPar", "TofdHit");

    fDetectorsRight->AddDetector("target", kTarget, "TargetGeoPar");
    fDetectorsRight->AddDetector("fi23a", kTargetGlad, "fi23aGeoPar", "Fi23aHit");
    fDetectorsRight->AddDetector("fi23b", kTargetGlad, "fi23bGeoPar", "Fi23bHit");
    fDetectorsRight->AddDetector("fi31", kAfterGlad, "fi31GeoPar", "Fi31Hit");
    fDetectorsRight->AddDetector("fi33", kAfterGlad, "fi33GeoPar", "Fi33Hit");
    fDetectorsRight->AddDetector("tofd", kTof, "tofdGeoPar", "TofdHit");
*/

    fDetectors->AddDetector("target", kTarget, "TargetGeoPar");
    fDetectors->AddDetector("fi23a", kTargetGlad, "fi23aGeoPar", "Fi23aHit");
    fDetectors->AddDetector("fi23b", kTargetGlad, "fi23bGeoPar", "Fi23bHit");
    fDetectors->AddDetector("fi31", kAfterGlad, "fi31GeoPar", "Fi31Hit");
    fDetectors->AddDetector("fi30", kAfterGlad, "fi30GeoPar", "Fi30Hit");
    fDetectors->AddDetector("fi33", kAfterGlad, "fi33GeoPar", "Fi33Hit");
    fDetectors->AddDetector("fi32", kAfterGlad, "fi32GeoPar", "Fi32Hit");
    fDetectors->AddDetector("tofd", kTof, "tofdGeoPar", "TofdHit");

}

R3BFragmentTrackerS494::~R3BFragmentTrackerS494() {}

/* For the tracking we use a user-defined list of TrackingDetectors,
 * stored in a TClonesArrays. The TrackingDetectors will provide
 * detector properties as well as hit coordinates to which we want to
 * fit the track.
 *
 * The tracker also needs a (measured) time-of-flight. Duh!
 * And a charge.
 * The charge is part of the TOF-wall hit level (or at least: will be)
 * The time-of-flight can be constructed from the hits of
 * start and stop detector, aka LOS and TOFD.
 *
 * Unfortunately, the HIT level data is not universal, hence, we cannot use the
 * same generic code for all detectors. We *could* have X,Y,C,T as basic structure
 * for ALL hit data. But not all detectors provide all variables.
 * We could inherit getters from a common base class and return NAN in the
 * overloaded members. Can fix this later. For now we can manually construct
 * a list of detectors with X,Y and additionaly fetch TOF and charge.
 *
 * Btw: the old tracker required the user to provide the measured positions
 * for the track fitting. That was actually quite convenient. Can this
 * be done in the macro? Nope. Because there is no event loop in the macro.
 * Pity.
 *
 * So we need to do it here in the Exec function. Like:
 * for each psp1_hit
 *   for each psp2_hit
 *     for each mtof_hit
 *          track_fragment()
 *
 * Also, we need to know the materials of the detectors and a way to        !! discuss !! ToDo
 * calc the energy loss (forward and backward).
 *
 */
InitStatus R3BFragmentTrackerS494::Init()
{
    cout << "ENETRING TRACKER INIT" << endl;

    FairRootManager* man = FairRootManager::Instance();

    // Get objects for detectors on all levels
    fArrayMCTracks = (TClonesArray*)man->GetObject("MCTrack");
    if (fArrayMCTracks)
        man->Register("MCTrack", "Monte Carlo Tracks", fArrayMCTracks, kTRUE);

    if (NULL == fArrayMCTracks)
    {
        LOG(info) << "No MC Track array found in input file.";
        // return kERROR;
    }
    assert(DET_MAX + 1 == sizeof(fDetectorNames) / sizeof(fDetectorNames[0]));
    printf("Have %d fiber detectors.\n", NOF_FIB_DET);

    for (int det = 0; det < DET_MAX; det++)
    {
        fArrayHits.push_back((TClonesArray*)man->GetObject(Form("%sHit", fDetectorNames[det])));

        if (det == DET_MAX - 1)
            maxevent = man->CheckMaxEventNo();

        if (NULL == fArrayHits.at(det))
        {
            printf("Could not find hit data for '%s'.\n", fDetectorNames[det]);
        }
        else
        {
            printf("Found hit data for '%s'.\n", fDetectorNames[det]);
        }
    }
    // CALIFA
    fMappedItems.push_back((TClonesArray*)man->GetObject(Form("%sMappedData", fDetectorNames[0])));
    if (NULL == fMappedItems.at(0))
    {
        printf("Could not find mapped data for '%s'.\n", fDetectorNames[0]);
    }
    fCalItems.push_back((TClonesArray*)man->GetObject(Form("%sCrystalCalData", fDetectorNames[0])));
    if (NULL == fCalItems.at(0))
    {
        printf("Could not find Cal data for '%s'.\n", fDetectorNames[0]);
    }

    man->Register("TofdHit", "Land", fTofdHitItems, kTRUE);
    man->Register("Fi23aHit", "Land", fFi23aHitItems, kTRUE);
    man->Register("Fi23bHit", "Land", fFi23bHitItems, kTRUE);
    man->Register("Fi30Hit", "Land", fFi30HitItems, kTRUE);
    man->Register("Fi31Hit", "Land", fFi31HitItems, kTRUE);
    man->Register("Fi32Hit", "Land", fFi32HitItems, kTRUE);
    man->Register("Fi33Hit", "Land", fFi33HitItems, kTRUE);

    man->Register("TrackingParticle", "Tracking", fArrayFragments, kTRUE);
    man->Register("Track", "Land", fTrackItems, kTRUE);

    if (!InitPropagator())
    {
        return kERROR;
    }

    //fDetectorsLeft->Init();
    //fDetectorsRight->Init();
    fDetectors->Init();
    if (fMappedItems.at(DET_CALIFA))
    {
        fh_califa_energy = new TH2F("fh_califa_energy", "Califa E vs crystal id", 2000, 0, 2000, 1000, 0., 1000.);
        fh_califa_energy->GetYaxis()->SetTitle("Energy / MeV");
        fh_califa_energy->GetXaxis()->SetTitle("Crystal #");

        fh_califa_energy_select =
            new TH2F("fh_califa_energy_select", "Califa E vs crystal id selected", 2000, 0, 2000, 1000, 0., 1000.);
        fh_califa_energy_select->GetYaxis()->SetTitle("Energy / MeV");
        fh_califa_energy_select->GetXaxis()->SetTitle("Crystal #");
    }
    fh_mult_fi23a = new TH1F("h_mult_fi23a", "Multiplicity fi23a", 20, -0.5, 19.5);
    fh_mult_fi23b = new TH1F("h_mult_fi23b", "Multiplicity fi23b", 20, -0.5, 19.5);
    fh_mult_fi30 = new TH1F("h_mult_fi30", "Multiplicity fi30", 20, -0.5, 19.5);
    fh_mult_fi31 = new TH1F("h_mult_fi31", "Multiplicity fi31", 20, -0.5, 19.5);
    fh_mult_fi32 = new TH1F("h_mult_fi32", "Multiplicity fi32", 20, -0.5, 19.5);
    fh_mult_fi33 = new TH1F("h_mult_fi33", "Multiplicity fi33", 20, -0.5, 19.5);
    fh_mult_tofd = new TH1F("h_mult_tofd", "Multiplicity TOFd", 20, -0.5, 19.5);
    fh_eloss_fi23a_mc = new TH1F("h_eloss_fi23a_mc", "Energy loss fi23a (MC truth)", 2000, 0., 50.);
    fh_eloss_fi23a = new TH1F("h_eloss_fi23a", "Energy loss fi23a", 2000, 0., 50.);
    fh_eloss_fi23b_mc = new TH1F("h_eloss_fi23b_mc", "Energy loss fi23b (MC truth)", 2000, 0., 50.);
    fh_eloss_fi23b = new TH1F("h_eloss_fi23b", "Energy loss fi23b", 2000, 0., 50.);
    fh_eloss_fi30_mc = new TH1F("h_eloss_fi30_mc", "Energy loss fi30 (MC truth)", 2000, 0., 50.);
    fh_eloss_fi30 = new TH1F("h_eloss_fi30", "Energy loss fi30", 2000, 0., 50.);
    fh_eloss_fi31_mc = new TH1F("h_eloss_fi31_mc", "Energy loss fi31 (MC truth)", 2000, 0., 50.);
    fh_eloss_fi31 = new TH1F("h_eloss_fi31", "Energy loss fi31", 2000, 0., 50.);
    fh_eloss_fi32_mc = new TH1F("h_eloss_fi32_mc", "Energy loss fi32 (MC truth)", 2000, 0., 50.);
    fh_eloss_fi32 = new TH1F("h_eloss_fi32", "Energy loss fi32", 2000, 0., 50.);
    fh_eloss_fi33_mc = new TH1F("h_eloss_fi33_mc", "Energy loss fi33 (MC truth)", 2000, 0., 50.);
    fh_eloss_fi33 = new TH1F("h_eloss_fi33", "Energy loss fi33", 2000, 0., 50.);
    fh_ncand = new TH1F("h_ncand", "Number of candidates", 100, -0.5, 99.5);
    fh_A_reco1 = new TH1F("h_A_reco1", "Reconstructed mass, step 1", 2000., 0., 20.);
    fh_Erel = new TH1F("h_Erel", "Erel", 100., 0., 10.);
    fh_A_reco2 = new TH1F("h_A_reco2", "Reconstructed mass, step 2", 2000., 0., 20.);
    fh_mom_res = new TH1F("h_mom_res", "Momentum resolution", 2000, -100., 100.);
    fh_mom_res_x = new TH1F("h_mom_res_x", "Momentum resolution px in %", 2000, -100., 100.);
    fh_mom_res_y = new TH1F("h_mom_res_y", "Momentum resolution py in %", 2000, -100., 100.);
    fh_mom_res_z = new TH1F("h_mom_res_z", "Momentum resolution pz in %", 2000, -100., 100.);
    fh_mass_res = new TH1F("h_mass_res", "Mass resolution", 10, -5.5, 4.5);
    fh_chi2 = new TH1F("h_chi2", "Chi2", 10000, 0., 1000.);
    fh_vz_res = new TH1F("h_vz_res", "vz - vz_mc", 200, -1., 1.);
    fh_beta_res = new TH1F("h_beta_res", "beta - beta_mc", 200, -0.1, 0.1);
    fh_A_overZ = new TH2F("fh_A_overZ", "particle identification", 100, 1., 4., 10, 0., 10.);
    fh_A_overZ->GetXaxis()->SetTitle("A/Z");
    fh_A_overZ->GetYaxis()->SetTitle("Z");
    fh_p = new TH1F("h_p", "momentum p", 2000, 0., 20.);
    fh_px = new TH1F("h_px", "momentum px", 2000, -100., 100.);
    fh_py = new TH1F("h_py", "momentum py", 2000, -100., 100.);
    fh_pz = new TH1F("h_pz", "momentum pz", 2000, 0., 20.);
    fh_px_l = new TH1F("h_px_l", "momentum px left", 1000, -500., 500.);
    fh_py_l = new TH1F("h_py_l", "momentum py left", 1000, -500., 500.);
    fh_pz_l = new TH1F("h_pz_l", "momentum pz left", 2000, 0., 20.);
    fh_px_r = new TH1F("h_px_r", "momentum px right", 1000, -500., 500.);
    fh_py_r = new TH1F("h_py_r", "momentum py right", 1000, -500., 500.);
    fh_pz_r = new TH1F("h_pz_r", "momentum pz right", 2000, 0., 20.);

    fh_Erel = new TH1F("h_Erel", "Erel / MeV", 200, 0., 20);
    fh_psum = new TH1F("h_psum", "psum / MeV/c", 500, 16000., 18000.);
    fh_theta = new TH1F("h_theta", "theta / deg", 500, 0., 5);

    fh_p_vs_ch2 = new TH2F("h_p_vs_chi2", "p.Mag vs chi2", 2000, 0., 200., 2000, 0., 20.);
    fh_mass_vs_ch2 = new TH2F("h_mass_vs_chi2", "mass vs chi2", 2000, 0., 200., 400, 0., 20.);

    fh_theta_16O = new TH2F("theta_16O", "theta of 6O ", 100, 0., 5., 2000, 16000, 18000);
    fh_theta_16O->GetXaxis()->SetTitle("angle / degree");
    fh_theta_16O->GetYaxis()->SetTitle("pz / MeV/c");

    fh_phi_16O = new TH2F("phi_16O", "phi of 16O ", 360, 0., 360, 2000, 16000, 18000);
    fh_phi_16O->GetXaxis()->SetTitle("angle / degree");
    fh_phi_16O->GetYaxis()->SetTitle("pz / MeV/c");

    Double_t ranges[] = { 10., 10., 10., 10., 10., 10., 10., 50 };
    Int_t bins[] = { 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000 };
    for (Int_t i = 0; i < 8; i++)
    {
        fh_x_res[i] = new TH1F(Form("h_x_res%d", i), Form("x residual %d", i), bins[i], -ranges[i], ranges[i]);
        fh_x_pull[i] = new TH1F(Form("h_x_pull%d", i), Form("x pull %d", i), 40, -10., 10.);
        fh_y_res[i] = new TH1F(Form("h_y_res%d", i), Form("y residual %d", i), bins[i], -ranges[i], ranges[i]);
        fh_y_pull[i] = new TH1F(Form("h_y_pull%d", i), Form("ypull %d", i), 40, -10., 10.);
    }

    fh_xy_target_C = new TH2F("h_xy_target_track_C", "xy target track C", 200, -1, 1, 200, -1, 1);
    fh_xy_target_C->GetXaxis()->SetTitle("xtarget / cm");
    fh_xy_target_C->GetYaxis()->SetTitle("ytarget / cm");

    fh_dxdy = new TH2F("h_dxdy_track_MC", "dxdy track vs MC", 200, -1, 1, 200, -1, 1);
    fh_dxdy->GetXaxis()->SetTitle("xMC-xtarget / cm");
    fh_dxdy->GetYaxis()->SetTitle("yMC-ytarget / cm");

    fh_xy_target_He = new TH2F("h_xy_target_track_He", "xy target track He", 400, -2, 2, 400, -2, 2);
    fh_xy_target_He->GetXaxis()->SetTitle("xtarget / cm");
    fh_xy_target_He->GetYaxis()->SetTitle("ytarget / cm");

    fh_mass_corel = new TH2F("h_mass_corel", "mass He vs mass C", 1000, 0, 20, 500, 0, 10);
    fh_mass_corel->GetXaxis()->SetTitle("mass C / amu");
    fh_mass_corel->GetYaxis()->SetTitle("mass He / amu");

    fh_xfi23a_target_track =
        new TH2F("h_xfi23a_target_track", "xFi23a vs xtarget track", 429, -6.006, 6.006, 429, -6.006, 6.006);
    fh_xfi23a_target_track->GetXaxis()->SetTitle("xtarget / cm");
    fh_xfi23a_target_track->GetYaxis()->SetTitle("xFi23a / cm");

    fh_yfi23b_target_track =
        new TH2F("h_yfi23b_target_track", "yFi23b vs ytarget track", 429, -6.006, 6.006, 429, -6.006, 6.006);
    fh_yfi23b_target_track->GetXaxis()->SetTitle("ytarget / cm");
    fh_yfi23b_target_track->GetYaxis()->SetTitle("yFi23b / cm");

    fh_xfi30_fi23a_track = new TH2F("h_xFi30vsFi23a_track", "xFi23a vs xFi30 track", 429, -6.006, 6.006, 600, -30, 30);
    fh_xfi30_fi23a_track->GetXaxis()->SetTitle("xFi23a / cm");
    fh_xfi30_fi23a_track->GetYaxis()->SetTitle("xFi30 / cm");

    fh_xfi30_fi32_track = new TH2F("h_xFi30vsFi32_track", "xFi32 vs xFi30 track", 600, -30, 30, 600, -30, 30);
    fh_xfi30_fi32_track->GetXaxis()->SetTitle("xFi30 / cm");
    fh_xfi30_fi32_track->GetYaxis()->SetTitle("xFi32 / cm");

    fh_xfi30_tofd_track = new TH2F("h_xFi30vsToFD_track", "xToFD track vs xFi30", 89, -60, 60.15, 600, -30, 30);
    fh_xfi30_tofd_track->GetXaxis()->SetTitle("xToFD / cm");
    fh_xfi30_tofd_track->GetYaxis()->SetTitle("xFi30 / cm");

    fh_xfi31_fi23a_track = new TH2F("h_xFi31vsFi23a_track", "xFi23a vs xFi31 track", 429, -6.006, 6.006, 600, -30, 30);
    fh_xfi31_fi23a_track->GetXaxis()->SetTitle("xFi23a / cm");
    fh_xfi31_fi23a_track->GetYaxis()->SetTitle("xFi31 / cm");

    fh_xfi31_fi33_track = new TH2F("h_xFi31vsFi33_track", "xFi33 vs xFi31 track", 600, -30, 30, 600, -30, 30);
    fh_xfi31_fi33_track->GetXaxis()->SetTitle("xFi31 / cm");
    fh_xfi31_fi33_track->GetYaxis()->SetTitle("xFi33 / cm");

    fh_xfi31_tofd_track = new TH2F("h_xFi31vsToFD_track", "xToFD vs xFi31 track", 89, -60, 60.15, 600, -30, 30);
    fh_xfi31_tofd_track->GetXaxis()->SetTitle("xToFD / cm");
    fh_xfi31_tofd_track->GetYaxis()->SetTitle("xFi31 / cm");

    fh_xfi30_fi23a_exp = new TH2F("h_xFi30vsFi23a_exp", "xFi23a vs xFi30 exp", 429, -6.006, 6.006, 600, -30, 30);
    fh_xfi30_fi23a_exp->GetXaxis()->SetTitle("xFi23a / cm");
    fh_xfi30_fi23a_exp->GetYaxis()->SetTitle("xFi30 / cm");

    fh_xfi30_fi32_exp = new TH2F("h_xFi30vsFi32_exp", "xFi32 vs xFi30 exp", 600, -30, 30, 600, -30, 30);
    fh_xfi30_fi32_exp->GetXaxis()->SetTitle("xFi30 / cm");
    fh_xfi30_fi32_exp->GetYaxis()->SetTitle("xFi32 / cm");

    fh_xfi30_tofd_exp = new TH2F("h_xFi30vsToFD_exp", "xToFD vs xFi30 exp", 89, -60, 60.15, 600, -30, 30);
    fh_xfi30_tofd_exp->GetXaxis()->SetTitle("xToFD / cm");
    fh_xfi30_tofd_exp->GetYaxis()->SetTitle("xFi30 / cm");

    fh_xfi30_fi23a_exp_select =
        new TH2F("h_xFi30vsFi23a_exp_select", "xFi23a vs xFi30 exp for A/Z=2", 429, -6.006, 6.006, 600, -30, 30);
    fh_xfi30_fi23a_exp_select->GetXaxis()->SetTitle("xFi23a / cm");
    fh_xfi30_fi23a_exp_select->GetYaxis()->SetTitle("xFi30 / cm");

    fh_xfi30_fi32_exp_select =
        new TH2F("h_xFi30vsFi32_exp_select", "xFi32 vs xFi30 exp for A/Z=2", 600, -30, 30, 600, -30, 30);
    fh_xfi30_fi32_exp_select->GetXaxis()->SetTitle("xFi30 / cm");
    fh_xfi30_fi32_exp_select->GetYaxis()->SetTitle("xFi32 / cm");

    fh_xfi30_tofd_exp_select =
        new TH2F("h_xFi30vsToFD_exp_select", "xToFD vs xFi30 exp for A/Z=2", 89, -60, 60.15, 600, -30, 30);
    fh_xfi30_tofd_exp_select->GetXaxis()->SetTitle("xToFD / cm");
    fh_xfi30_tofd_exp_select->GetYaxis()->SetTitle("xFi30 / cm");

    fh_fi23b_track_exp =
        new TH2F("h_yFi23b_track_vs_exp", "yFi23b track vs yFi23b exp", 429, -6.006, 6.006, 429, -6.006, 6.006);
    fh_fi23b_track_exp->GetXaxis()->SetTitle("yFib23b track / cm");
    fh_fi23b_track_exp->GetYaxis()->SetTitle("yFi23b exp / cm");

    fh_tofd_track_exp = new TH2F("h_yToFD_track_vs_exp", "yToFD track vs yTofd exp", 120, -60, 60, 120, -60, 60);
    fh_tofd_track_exp->GetXaxis()->SetTitle("yToFD track / cm");
    fh_tofd_track_exp->GetYaxis()->SetTitle("yTofd exp / cm");

    fh_yFi23b_tofd_track = new TH2F("h_yTofdvsFi23b_track", "yFi23b vs yTofd track", 120, -60, 60, 429, -6.006, 6.006);
    fh_yFi23b_tofd_track->GetYaxis()->SetTitle("yFi23b / cm");
    fh_yFi23b_tofd_track->GetXaxis()->SetTitle("yTofd / cm");

    fh_yFi23b_tofd_exp = new TH2F("h_yTofdvsFi23b_exp", "yFi23b vs yTofd exp", 120, -60, 60, 429, -6.006, 6.006);
    fh_yFi23b_tofd_exp->GetYaxis()->SetTitle("yFi23b / cm");
    fh_yFi23b_tofd_exp->GetXaxis()->SetTitle("yTofd / cm");

    fh_xfi31_fi23a_exp = new TH2F("h_xFi31vsFi23a_exp", "xFi23a vs xFi31 exp", 429, -6.006, 6.006, 600, -30, 30);
    fh_xfi31_fi23a_exp->GetXaxis()->SetTitle("xFi23a / cm");
    fh_xfi31_fi23a_exp->GetYaxis()->SetTitle("xFi31 / cm");

    fh_xfi31_fi33_exp = new TH2F("h_xFi31vsFi33_exp", "xFi33 vs xFi31 exp", 600, -30, 30, 600, -30, 30);
    fh_xfi31_fi33_exp->GetXaxis()->SetTitle("xFi31 / cm");
    fh_xfi31_fi33_exp->GetYaxis()->SetTitle("xFi33 / cm");

    fh_xfi31_tofd_exp = new TH2F("h_xFi31vsToFD_exp", "xToFD vs xFi31 exp", 120, -60, 60, 600, -30, 30);
    fh_xfi31_tofd_exp->GetXaxis()->SetTitle("xToFD / cm");
    fh_xfi31_tofd_exp->GetYaxis()->SetTitle("xFi31/ cm");

    fh_yFi23b_tofd_exp_select =
        new TH2F("h_yTofdvsFi23b_exp_select", "yFi23b vs yTofd exp for A/Z=2", 120, -60, 60, 429, -6.006, 6.006);
    fh_yFi23b_tofd_exp_select->GetYaxis()->SetTitle("yFi23b / cm");
    fh_yFi23b_tofd_exp_select->GetXaxis()->SetTitle("yTofd / cm");

    fh_xfi31_fi23a_exp_select =
        new TH2F("h_xFi31vsFi23a_exp_select", "xFi23a vs xFi31 exp for A/Z=2", 429, -6.006, 6.006, 600, -30, 30);
    fh_xfi31_fi23a_exp_select->GetXaxis()->SetTitle("xFi23a / cm");
    fh_xfi31_fi23a_exp_select->GetYaxis()->SetTitle("xFi31 / cm");

    fh_xfi31_fi33_exp_select =
        new TH2F("h_xFi31vsFi33_exp_select", "xFi33 vs xFi31 exp for A/Z=2", 600, -30, 30, 600, -30, 30);
    fh_xfi31_fi33_exp_select->GetXaxis()->SetTitle("xFi31 / cm");
    fh_xfi31_fi33_exp_select->GetYaxis()->SetTitle("xFi33 / cm");

    fh_xfi31_tofd_exp_select =
        new TH2F("h_xFi31vsToFD_exp_select", "xToFD vs xFi31 exp for A/Z=2", 120, -60, 60, 600, -30, 30);
    fh_xfi31_tofd_exp_select->GetXaxis()->SetTitle("xToFD / cm");
    fh_xfi31_tofd_exp_select->GetYaxis()->SetTitle("xFi31/ cm");

    fh_yC_vs_yHe_Tofd = new TH2F("h_yC_vs_yHe_Tofd", "yC_vs_yHe_Tofd track", 120, -60, 60, 120, -60, 60);
    fh_yC_vs_yHe_Tofd->GetXaxis()->SetTitle("yC / cm");
    fh_yC_vs_yHe_Tofd->GetYaxis()->SetTitle("yHe / cm");

    fh_yC_vs_yHe_Tofd_exp = new TH2F("h_yC_vs_yHe_Tofd_exp", "yC_vs_yHe_Tofd exp", 120, -60, 60, 120, -60, 60);
    fh_yC_vs_yHe_Tofd_exp->GetXaxis()->SetTitle("yC / cm");
    fh_yC_vs_yHe_Tofd_exp->GetYaxis()->SetTitle("yHe / cm");

    fh_yC_vs_yHe_fib23 = new TH2F("h_yC_vs_yHe_fib23", "yC_vs_yHe_fib23 track", 429, -6.006, 6.006, 429, -6.006, 6.006);
    fh_yC_vs_yHe_fib23->GetXaxis()->SetTitle("yC / cm");
    fh_yC_vs_yHe_fib23->GetYaxis()->SetTitle("yHe / cm");

    fh_yC_vs_yHe_fib23_exp =
        new TH2F("h_yC_vs_yHe_fib23_exp", "yC_vs_yHe_fib23 exp", 429, -6.006, 6.006, 429, -6.006, 6.006);
    fh_yC_vs_yHe_fib23_exp->GetXaxis()->SetTitle("yC / cm");
    fh_yC_vs_yHe_fib23_exp->GetYaxis()->SetTitle("yHe / cm");

    fh_xC_vs_xHe_fib23 = new TH2F("h_xC_vs_xHe_fib23", "xC_vs_xHe_fib23 track", 429, -6.006, 6.006, 429, -6.006, 6.006);
    fh_xC_vs_xHe_fib23->GetXaxis()->SetTitle("xC / cm");
    fh_xC_vs_xHe_fib23->GetYaxis()->SetTitle("xHe / cm");

    fh_xC_vs_xHe_fib23_exp =
        new TH2F("h_xC_vs_xHe_fib23_exp", "xC_vs_xHe_fib23 exp", 429, -6.006, 6.006, 429, -6.006, 6.006);
    fh_xC_vs_xHe_fib23_exp->GetXaxis()->SetTitle("xC / cm");
    fh_xC_vs_xHe_fib23_exp->GetYaxis()->SetTitle("xHe / cm");

    fh_pyC_vs_pyHe = new TH2F("h_pyC_vs_pyHe", "pyC_vs_pyHe", 1000, -500, 500, 1000, -500, 500);
    fh_pyC_vs_pyHe->GetXaxis()->SetTitle("pyC / MeV/c");
    fh_pyC_vs_pyHe->GetYaxis()->SetTitle("pyHe / MeV/c");

    px_vs_x = new TH2F("h_px_vs_xtofd", "px_vs_xtofd", 89, -60, 60.15, 1000, -500, 500);
    px_vs_x->GetXaxis()->SetTitle("x tofd / cm");
    px_vs_x->GetYaxis()->SetTitle("px / MeV/c");

    py_vs_x = new TH2F("h_py_vs_xtofd", "py_vs_xtofd", 89, -60, 60.15, 1000, -500, 500);
    py_vs_x->GetXaxis()->SetTitle("x tofd / cm");
    py_vs_x->GetYaxis()->SetTitle("py / MeV/c");

    pz_vs_x = new TH2F("h_pz_vs_xtofd", "pz_vs_xtofd", 89, -60, 60.15, 2000, 0., 20.);
    pz_vs_x->GetXaxis()->SetTitle("x tofd / cm");
    pz_vs_x->GetYaxis()->SetTitle("pz / GeV/c");

    p_vs_x = new TH2F("h_p_vs_xtofd", "p_vs_xtofd", 89, -60, 60.15, 2000, 0., 20.);
    p_vs_x->GetXaxis()->SetTitle("x tofd / cm");
    p_vs_x->GetYaxis()->SetTitle("psum / GeV/c");

    px_vs_y = new TH2F("h_px_vs_ytofd", "px_vs_ytofd", 120, -60, 60, 1000, -500, 500);
    px_vs_y->GetXaxis()->SetTitle("y tofd / cm");
    px_vs_y->GetYaxis()->SetTitle("px / MeV/c");

    py_vs_y = new TH2F("h_py_vs_ytofd", "py_vs_ytofd", 120, -60, 60, 1000, -500, 500);
    py_vs_y->GetXaxis()->SetTitle("y tofd / cm");
    py_vs_y->GetYaxis()->SetTitle("py / MeV/c");

    pz_vs_y = new TH2F("h_pz_vs_ytofd", "pz_vs_ytofd", 120, -60, 60, 2000, 0., 20.);
    pz_vs_y->GetXaxis()->SetTitle("y tofd / cm");
    pz_vs_y->GetYaxis()->SetTitle("pz / GeV/c");

    p_vs_y = new TH2F("h_p_vs_ytofd", "p_vs_ytofd", 120, -60, 60, 2000, 0., 20.);
    p_vs_y->GetXaxis()->SetTitle("y tofd / cm");
    p_vs_y->GetYaxis()->SetTitle("p / GeV/c");

    fh_px_p = new TH2F("h_p_vs_px", "p_vs_px", 200, -1, 1, 2000, 0., 20.);
    fh_px_p->GetXaxis()->SetTitle("px / GeV/c");
    fh_px_p->GetYaxis()->SetTitle("p / GeV/c");

    fh_py_p = new TH2F("h_p_vs_py", "p_vs_py", 200, -1, 1, 2000, 0., 20.);
    fh_py_p->GetXaxis()->SetTitle("py / GeV/c");
    fh_py_p->GetYaxis()->SetTitle("p / GeV/c");

    fh_Erel_vs_x =
        new TH2F("h_Erel_vs_x", "Erel_vs_x", 429, -6.006, 6.006, 200, 0., 20.); // 89, -60, 60.15, 200, 0., 20.);
    fh_Erel_vs_x->GetXaxis()->SetTitle("x fib23a / cm");
    fh_Erel_vs_x->GetYaxis()->SetTitle("Erel / MeV");

    fh_Erel_vs_y =
        new TH2F("h_Erel_vs_y", "Erel_vs_y", 429, -6.006, 6.006, 200, 0., 20.); // 89, -60, 60.15, 200, 0., 20.);
    fh_Erel_vs_y->GetXaxis()->SetTitle("y fib23b / cm");
    fh_Erel_vs_y->GetYaxis()->SetTitle("Erel / MeV");

    fh_Erel_vs_nhits23a = new TH2F("h_Erel_vs_nhits23a", "Erel_vs_nhits23a", 20, 0, 20, 200, 0., 20.);
    fh_Erel_vs_nhits23a->GetXaxis()->SetTitle("nhits23a");
    fh_Erel_vs_nhits23a->GetYaxis()->SetTitle("Erel / MeV");

    fh_Erel_vs_nhits23b = new TH2F("h_Erel_vs_nhits23b", "Erel_vs_nhits23b", 20, 0, 20, 200, 0., 20.);
    fh_Erel_vs_nhits23b->GetXaxis()->SetTitle("nhits23b");
    fh_Erel_vs_nhits23b->GetYaxis()->SetTitle("Erel / MeV");

    fh_theta_vs_x = new TH2F("h_theta_vs_x", "theta_vs_x", 429, -6.006, 6.006, 125, 0., 5.);
    fh_theta_vs_x->GetXaxis()->SetTitle("x fib23a / cm");
    fh_theta_vs_x->GetYaxis()->SetTitle("theta / deg");

    fh_theta_vs_y = new TH2F("h_theta_vs_y", "theta_vs_y", 429, -6.006, 6.006, 125, 0., 5.);
    fh_theta_vs_y->GetXaxis()->SetTitle("y fib23b / cm");
    fh_theta_vs_y->GetYaxis()->SetTitle("theta / deg");

    fFitter->Init(fPropagator, fEnergyLoss);

    Double_t scale = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetScale();
    Double_t field = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(0., 0., 240.);
    cout << "Field:" << field << " scale: " << scale << endl;

    return kSUCCESS;
}

InitStatus R3BFragmentTrackerS494::ReInit()
{
    SetParContainers();

    if (!InitPropagator())
    {
        return kERROR;
    }

    return kSUCCESS;
}

void R3BFragmentTrackerS494::SetParContainers()
{
    fFieldPar = (R3BFieldPar*)FairRuntimeDb::instance()->getContainer("R3BFieldPar");

    //fDetectorsLeft->SetParContainers();
    //fDetectorsRight->SetParContainers();
    fDetectors->SetParContainers();
}

void R3BFragmentTrackerS494::Exec(const Option_t*)
{
    //  ofstream outfile("event_numbers.dat",ios_base::out|ios_base::app);

    if (fNEvents / 1000. == (int)fNEvents / 1000)
        std::cout << "\rEvents: " << fNEvents << " / " << maxevent << " (" << (int)(fNEvents * 100. / maxevent)
                  << " %) " << std::endl;

    fNEvents += 1;

    fArrayFragments->Clear();
    if (fFragments.size() > 0)
    {
        for (auto const& x : fFragments)
        {
            delete x;
        }
        fFragments.clear();
    }

<<<<<<< HEAD
    Bool_t debug_loopout = false;
    Bool_t debug_loopin = false;
    Bool_t bestevents = false;

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
=======
    Bool_t debug = true;
    Int_t fNevOut = 0;
    if (fNEvents == 147041 || fNEvents == 184888)
        fNevOut = fNEvents;
    if (fNEvents == fNevOut)
        debug = true;
>>>>>>> working on the optimization of geometry

    /* this part needs to be adopted to each experiment / setup
     *
     * First thing is to tell each detector which hit we want to use
     * for the track fitting. This means: manually copy the hit we want to
     * use for tracking from the TClonesArray into the single hit structure.
     *
     * Then call fit_fragment() to optimize the track and calculate
     * particle properties.
     */
<<<<<<< HEAD
    fDetectorsLeft->CopyHits();
    fDetectorsRight->CopyHits();

    // R3BTrackingDetector* target = fDetectorsLeft->GetByName("target");
    R3BTrackingDetector* fi23a = fDetectorsLeft->GetByName("fi23a");
    R3BTrackingDetector* fi23b = fDetectorsLeft->GetByName("fi23b");
    R3BTrackingDetector* fi30 = fDetectorsLeft->GetByName("fi30");
    R3BTrackingDetector* fi31 = fDetectorsRight->GetByName("fi31");
    R3BTrackingDetector* fi32 = fDetectorsLeft->GetByName("fi32");
    R3BTrackingDetector* fi33 = fDetectorsRight->GetByName("fi33");
    R3BTrackingDetector* tof = fDetectorsLeft->GetByName("tofd");

    // R3BHit(Int_t detId, Double_t x, Double_t y, Double_t eloss, Double_t time, Int_t hitId)
    // target->hits.push_back(new R3BHit(0, 0., 0., 0., 0., 0));
=======
    //fDetectorsLeft->CopyHits();
    //fDetectorsRight->CopyHits();
    fDetectors->CopyHits();
    
    R3BTrackingDetector* target = fDetectors->GetByName("target");
    R3BTrackingDetector* fi23a = fDetectors->GetByName("fi23a");
    R3BTrackingDetector* fi23b = fDetectors->GetByName("fi23b");
    R3BTrackingDetector* fi31 = fDetectors->GetByName("fi31");
    R3BTrackingDetector* fi30 = fDetectors->GetByName("fi30");
    R3BTrackingDetector* fi33 = fDetectors->GetByName("fi33");
    R3BTrackingDetector* fi32 = fDetectors->GetByName("fi32");
    R3BTrackingDetector* tof = fDetectors->GetByName("tofd");

    target->hits.push_back(new R3BHit(0, 0., 0., 0., 0., 0));
>>>>>>> working on the optimization of geometry

    if (debug_loopout)
        // cout << "*************** NEW EVENT ****" << fNEvents << ", " << fNEvents_nonull << endl;

        if (tof->hits.size() > 0 && debug_loopout)
        {
            cout << "*************** NEW EVENT ****" << fNEvents << ", " << fNEvents_nonull << endl;
            cout << "Hits ToFD: " << tof->hits.size() << endl;
            cout << "Hits right: " << fi23a->hits.size() << "  " << fi23b->hits.size() << "  " << fi31->hits.size()
                 << "  " << fi33->hits.size() << endl;

            cout << "Hits left: " << fi23a->hits.size() << "  " << fi23b->hits.size() << "  " << fi32->hits.size()
                 << "  " << fi30->hits.size() << endl;
        }

    if (abs(fBfield) == 1710.0)
    {
        if (tof->hits.size() < 2)
            return;
        if (fi23a->hits.size() < 2)
            return;
        if (fi23b->hits.size() < 2)
            return;
        if ((fi30->hits.size() + fi31->hits.size() + fi32->hits.size() + fi33->hits.size()) < 4)
            return;
    }
    else
    {
        if (tof->hits.size() < 1)
            return;
    }

    /*
        if (tof->hits.size() != 4)
            return;
        if (fi23a->hits.size() != 2)
            return;
        if (fi23b->hits.size() != 2)
            return;
        if (fi30->hits.size() != 1)
            return;
        if (fi31->hits.size() != 1)
            return;
        if (fi32->hits.size() != 1)
            return;
        if (fi33->hits.size() != 1)
            return;
    */
    fNEvents_nonull += 1;
    //   cout<<"Selected num events: "<<fNEvents_nonull<<endl;

    // Start values
    Double_t beta = 0.;
    Double_t beta0 = 0.;
    Double_t x0 = 0.;
    Double_t y0 = 0.;
    Double_t z0 = 0.;
    Double_t px0 = 0;
    Double_t py0 = 0.;
    Double_t pz0 = 0.;
    Double_t p = 0.;
    Double_t mass = 0.;

    Double_t betaHe = 0.;
    Double_t x0He = 0.;
    Double_t y0He = 0.;
    Double_t z0He = 0.;
    Double_t px0He = 0;
    Double_t py0He = 0.;
    Double_t pz0He = 0.;
    Double_t pHe = 0.;
    Double_t massHe = 0.;

    Double_t betaC = 0.;
    Double_t x0C = 0.;
    Double_t y0C = 0.;
    Double_t z0C = 0.;
    Double_t px0C = 0;
    Double_t py0C = 0.;
    Double_t pz0C = 0.;
    Double_t pC = 0.;
    Double_t massC = 0.;

    Double_t betaO = 0.;
    Double_t x0O = 0.;
    Double_t y0O = 0.;
    Double_t z0O = 0.;
    Double_t px0O = 0;
    Double_t py0O = 0.;
    Double_t pz0O = 0.;
    Double_t pO = 0.;
    Double_t massO = 0.;

    Double_t pBeamz = 17.3915;
    Double_t pxmem = 0.;
    Double_t pymem = 0.;
    Double_t pzmem = 0.;
    Double_t xmem = 0.;
    Double_t ymem = 0.;
    Double_t zmem = 0.;
    Double_t yC = -1000;
    Double_t yCexp = -1000;
    Double_t yCfib = -1000;
    Double_t yCfibexp = -1000;
    Double_t xCfib = -1000;
    Double_t xCfibexp = -1000;
    Double_t masmem = -1000;
    Double_t chargemem = -1000.;
    Double_t AoverZmem = -1000.;
    Double_t pCx = -1000., pCy = -1000., pCz = -10000.;
    Double_t Erel = -10;
    Double_t theta_26 = -10;
    Double_t psum = -10;
    Double_t pxmemtarget = 0.;
    Double_t pymemtarget = 0.;
    Double_t pzmemtarget = 0.;
    Double_t ptot = 0;

    Double_t AoverZ = -1000.;
    Int_t iAoverZ = -1;
    Int_t iAoverZmem = -1;

    Double_t dfib = 0.09;

    if (fArrayMCTracks)
    {
        Int_t nHitsMCTrack = fArrayMCTracks->GetEntriesFast();
        for (Int_t l = 0; l < nHitsMCTrack; l++)
        {
            R3BMCTrack* ion = (R3BMCTrack*)fArrayMCTracks->At(l);

            Int_t PID = ion->GetPdgCode();
            Int_t mother = ion->GetMotherId();
            LOG(debug) << "PID " << PID << endl;
            if (mother < 0)
            {
                if (PID == 1000020040)
                {
                    // 4He
                    betaHe = 1. / TMath::Sqrt(1 + TMath::Power(ion->GetMass() / ion->GetP(), 2));
                    x0He = ion->GetStartX(); // cm
                    y0He = ion->GetStartY(); // cm
                    z0He = ion->GetStartZ(); // cm
                    px0He = ion->GetPx();    // GeV/c
                    py0He = ion->GetPy();    // GeV/c
                    pz0He = ion->GetPz();    // GeV/c
                    pHe = ion->GetP();
                    massHe = ion->GetMass();
                    LOG(debug) << "MC ************ 4He **************";
                    LOG(debug) << "MC position x: " << x0He << " y: " << y0He << " z: " << z0He;
                    LOG(debug) << "MC momentum p: " << pHe << " px " << px0He << " py " << py0He << " pz " << pz0He;
                    LOG(debug) << "MC mass 4He: " << massHe << " beta: " << betaHe << endl;
                }
                if (PID == 1000060120)
                {
                    // 12C
                    betaC = 1. / TMath::Sqrt(1 + TMath::Power(ion->GetMass() / ion->GetP(), 2));
                    x0C = ion->GetStartX();
                    y0C = ion->GetStartY();
                    z0C = ion->GetStartZ();
                    px0C = ion->GetPx();
                    py0C = ion->GetPy();
                    pz0C = ion->GetPz();
                    pC = ion->GetP();
                    massC = ion->GetMass();
                    LOG(debug) << "MC ************ 12C **************";
                    LOG(debug) << "MC position x: " << x0C << " y: " << y0C << " z: " << z0C;
                    LOG(debug) << "MC momentum p: " << pC << " px " << px0C << " py " << py0C << " pz " << pz0C;
                    LOG(debug) << "MC mass 12C: " << massC << " beta: " << betaC << endl;
                }
                if (PID == 1000080160)
                {
                    // 16O
                    betaO = 1. / TMath::Sqrt(1 + TMath::Power(ion->GetMass() / ion->GetP(), 2));
                    x0O = ion->GetStartX();
                    y0O = ion->GetStartY();
                    z0O = ion->GetStartZ();
                    px0O = ion->GetPx();
                    py0O = ion->GetPy();
                    pz0O = ion->GetPz();
                    pO = ion->GetP();
                    massO = ion->GetMass();
                    LOG(debug) << "MC ************ 16O **************";
                    LOG(debug) << "MC position x: " << x0O << " y: " << y0O << " z: " << z0O;
                    LOG(debug) << "MC momentum p: " << pO << " px " << px0O << " py " << py0O << " pz " << pz0O;
                    LOG(debug) << "MC mass: " << massO << " beta: " << betaO;
                }
            }
        }
    }
    /*
        // Important: Set charge and initial position and momentum of the particle
        R3BTrackingParticle* particle = new R3BTrackingParticle(10.,
                                                                x0,
                                                                y0,
                                                                z0,
                                                                px0,
                                                                py0,
                                                                pz0,
                                                                beta,
                                                                mass);

    */

    fh_mult_fi23a->Fill(fi23a->hits.size());
    fh_mult_fi23b->Fill(fi23b->hits.size());
    fh_mult_fi30->Fill(fi30->hits.size());
    fh_mult_fi31->Fill(fi31->hits.size());
    fh_mult_fi32->Fill(fi32->hits.size());
    fh_mult_fi33->Fill(fi33->hits.size());
    fh_mult_tofd->Fill(tof->hits.size());

    /* Note:
     * We DO need to know the beta of the particle in order to calculate
     * its energy loss in matter. For that reason only, we need to separate
     * the momentum into mass and beta and we need to vary these two
     * independently. Pity.
     */

    // try to fit all possible combination of hits.

    fPropagator->SetVis(kFALSE);

    Int_t nCand = 0;

    Int_t ifi23a = 0;
    Int_t ifi23b = 0;
    Int_t ifi30 = 0;
    Int_t ifi31 = 0;
    Int_t ifi32 = 0;
    Int_t ifi33 = 0;
    Int_t itof = 0;
    if (0 == fi23a->hits.size())
    {
        ifi23a = -1;
    }
    if (0 == fi23b->hits.size())
    {
        ifi23b = -1;
    }
    if (0 == fi30->hits.size())
    {
        ifi30 = -1;
    }
    if (0 == fi31->hits.size())
    {
        ifi31 = -1;
    }
    if (0 == fi32->hits.size())
    {
        ifi32 = -1;
    }
    if (0 == fi33->hits.size())
    {
        ifi33 = -1;
    }
    if (0 == tof->hits.size())
    {
        itof = -1;
    }
    if (fi23a->hits.size() * fi23b->hits.size() * fi30->hits.size() * fi32->hits.size() * tof->hits.size() > 10000)
    {
        return;
    }
    if (fi23a->hits.size() * fi23b->hits.size() * fi31->hits.size() * fi33->hits.size() * tof->hits.size() > 10000)
    {
        return;
    }
    Int_t charge_mem = 0;
    Bool_t alpha = kFALSE;
    Bool_t carbon = kFALSE;
    Bool_t oxygen = kFALSE;
    Bool_t writeOutC = kFALSE;

    R3BTrackingParticle* candidate;
    // The idea is to loop twice over the ToF wall hits.
    // First we want to look for 12C particle and then for 4He

    Int_t Icountleft = 0;
    Int_t Icountright = 0;

    Int_t lmin;
    Int_t lmax;
    if (fBfield != -1710.0)
    {
        lmin = 0;
        lmax = 1;
    }
    else
    {
        lmin = 1;
        lmax = 3;
    }

    if (debug_loopout)
    {
        for (Int_t i = 0; i < fi23a->hits.size(); i++)
        {
            cout << "Fib23a hits: " << i << ", x: " << fi23a->hits.at(i)->GetX()
                 << ", Q: " << fi23a->hits.at(i)->GetEloss() << endl;
        }
        for (Int_t i = 0; i < fi23b->hits.size(); i++)
        {
            cout << "Fib23b hits: " << i << ", " << fi23b->hits.at(i)->GetY()
                 << ", Q: " << fi23b->hits.at(i)->GetEloss() << endl;
        }
        for (Int_t i = 0; i < fi30->hits.size(); i++)
        {
            cout << "Fib30 hits: " << i << ", x: " << fi30->hits.at(i)->GetX()
                 << ", Q: " << fi30->hits.at(i)->GetEloss() << endl;
        }
        for (Int_t i = 0; i < fi32->hits.size(); i++)
        {
            cout << "Fib32 hits: " << i << ", x: " << fi32->hits.at(i)->GetX()
                 << ", Q: " << fi32->hits.at(i)->GetEloss() << endl;
        }
        for (Int_t i = 0; i < fi31->hits.size(); i++)
        {
            cout << "Fib31 hits: " << i << ", x: " << fi31->hits.at(i)->GetX()
                 << ", Q: " << fi31->hits.at(i)->GetEloss() << endl;
        }
        for (Int_t i = 0; i < fi33->hits.size(); i++)
        {
            cout << "Fib33 hits: " << i << ", x: " << fi33->hits.at(i)->GetX()
                 << ", Q: " << fi33->hits.at(i)->GetEloss() << endl;
        }

        for (Int_t i = 0; i < tof->hits.size(); i++)
        {
            cout << "Tofd hits  : " << i << ", " << tof->hits.at(i)->GetX() << ", " << tof->hits.at(i)->GetY()
                 << ", Q: " << tof->hits.at(i)->GetEloss() << endl;
        }
    }

    for (Int_t l = lmin; l < lmax; l++)
    {
        // l = 0: 16O
        // l = 1: 12C
        // l = 1: 4He

        Int_t charge = 0;
        Int_t charge_requested = 0;
        Double_t Charge = 0;
        Double_t m0 = 0.;
        Double_t p0 = 0.;

        if (l == 0)
        {
            charge_requested = 8;
        }
        else if (l == 1)
        {
            charge_requested = 6;
        }
        else if (l == 2)
        {
            charge_requested = 2;
            if (!carbon)
            {
                if (debug_loopout)
                    cout << "No 12C found, tracking of 4He will not be done" << endl;
                return;
            }
        }

        // Loop over all combinations of hits
        for (Int_t i = 0; i < tof->hits.size(); i++) // loop over all ToFD hits
        {
            if (fSimu)
            {
                // For tracking of simulations:
                //   charge = sqrt(tof->hits.at(i)->GetEloss()) * 26.76 + 0.5;
                //   Charge = sqrt(tof->hits.at(i)->GetEloss()) * 26.76;

                // if digi used
                Charge = tof->hits.at(i)->GetEloss();
                charge = (int)(Charge + 0.5);

                // if digiHit used
                //  charge = (int)(tof->hits.at(i)->GetEloss() * 88.196293 + 1.5024036 + 0.5);
                //  Charge = tof->hits.at(i)->GetEloss() * 88.196293 + 1.5024036;
            }
            else
            {
                // For tracking of exp. data:
                charge = (int)(tof->hits.at(i)->GetEloss() + 0.5);
                Charge = tof->hits.at(i)->GetEloss();
            }
            if (debug_loopout)
                cout << "Charge: " << charge << " requested charge: " << charge_requested << endl;

            if (charge != charge_requested)
                continue;

            beta0 = 0.7593; // velocity could eventually be calculated from ToF
            tof->res_t = 0.03;
            // Double_t m0 = charge * 2. * 0.931494028; // First guess of mass

            // Masse wird nicht getreckt, nur momentum
            if (charge == 8)
            {
                m0 = 15.0124; // has to have the same value as what geant uses
                p0 = 17.3915; // in GeV/c2
            }
            if (charge == 6)
            {
                m0 = 11.17486339;
                // m0 = 11.178;
                p0 = 13.043625;
                if (!fSimu)
                    pC = p0;
                massC = m0;
                if (fSimu)
                {
                    //  x0 = x0C;
                    // y0 = y0C;
                }
            }
            if (charge == 2)
            {
                m0 = 3.728401291;
                p0 = 4.347875;
                if (!fSimu)
                    pHe = p0;
                massHe = m0;
                if (fSimu)
                {
                    //  x0 = x0He;
                    // y0 = y0He;
                }
            }

            // Create object for particle which will be fitted
            // R3BTrackingParticle* candidate =
            //                      new R3BTrackingParticle(charge, x0, y0, z0, 0., 0., p0, beta0, m0);

<<<<<<< HEAD
            if (debug_loopin)
                cout << "Mass: " << m0 << endl;
            if (debug_loopin)
=======
            if (debug){
                cout << "Mass: " << m0 << endl;
>>>>>>> working on the optimization of geometry
                cout << "Position on TofD: " << tof->hits.at(i)->GetX() << endl;
			}
            if (!tof->free_hit[i]) // if the hit was used already, continue
            {
                if (debug_loopin)
                    cout << "ToFD hit already used" << endl;
                continue;
            }

            if (debug_loopin)
                cout << "AT START: "
                     << "Mass: " << m0 << ", Momentum: " << p0 << endl;

            if (tof->hits.at(i)->GetX() > 0 && fi30->hits.size() > 0 && fi32->hits.size() > 0 &&
                fi23a->hits.size() > 0 && fi23b->hits.size() > 0)
            {
                // left branch in beam direction, don't consider hits in the detectors of the other side

                if (!fSimu)
                {
                    //( (R3BGladFieldMap*) FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(1.002);

                    // start values:
                    //	( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleY(-14.);
                    //     ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionZ(172.5);
                    //     ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionY(1.5);
                    //     ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(0.);
                    //     ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(1.000);
                    //    ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.0);
                    // first optimization, right and left same:
                    /* ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleY(-14.);
                      ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionZ(179.1);
                      ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionY(1.);
                      ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(0.);
                      ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(1.0011);
                      ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.0);*/
                    // second optimization
                    //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleY(-13.9750);
                    //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionZ(172.8753);
                    //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionY(1.625196);
                    //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleZ(0.025);
                    //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(0.9974960);
                    //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleX(0.0);
                    // second optimization angleZ=0
                    /*( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleY(-13.875);
                    ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionZ(172.8753);
                    ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionY(1.630196);
                    ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(0.0);
                    ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionScale(0.997496);
                    ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.0);	*/
                    // third optimization
                    /*	( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleY(-13.8);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionZ(174.9635);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionY(1.205);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(0.);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionScale(1.009);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.0);*/
                    // fourth optimization
                    /*	( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                       )->SetTrackerCorrectionAngleY(-13.73465); ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                       )->SetTrackerCorrectionZ(175.0103); ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                       )->SetTrackerCorrectionY(1.24908); ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                       )->SetTrackerCorrectionAngleZ(0.); ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                       )->SetTrackerCorrectionScale(1.00596); ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                       )->SetTrackerCorrectionAngleX(0.0);*/
                    // fifth optimization
                    /*( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleY(-13.83939);
                    ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionZ(173.005);
                    ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionY(1.16036);
                    ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(0.027573);
                    ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionScale(0.99871);
                    ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.02068);	*/
                    // seventh optimization
                    /*	( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleY(-13.5);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionZ(173.5);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionY(1.5);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(0.02);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionScale(0.99);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.03);*/
                    // seventh optimization test
                    /*	( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleY(-13.5);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionZ(173.7);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionY(1.3);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(0.02);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionScale(0.991);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.03);*/
                }
                // simu from 01/02/22:
                if (fSimu)
                {
                    /*   ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleY(-14.);
                       ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionZ(174.95);
                       ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionY(1.08);
                       ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleZ(0.);
                       ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(1.00);*/
                }

<<<<<<< HEAD
                R3BTrackingDetector* target = fDetectorsLeft->GetByName("target");
                target->hits.clear();
                if (fNEventsLeft == 0)
=======
                //R3BTrackingDetector* target = fDetectorsLeft->GetByName("target");
                //if (fNEventsLeft == 0)
>>>>>>> working on the optimization of geometry
                {
                    target->hits.push_back(new R3BHit(0, 0.0, 0.0, 0., 0., 0));
                }

                do // fi32
                {
                    if (ifi32 >= 0)
                        fh_eloss_fi32_mc->Fill(1000. * fi32->hits.at(ifi32)->GetEloss()); // MeV

                    if ((ifi32 >= 0 && !fi32->free_hit[ifi32]) ||
                        charge_requested != fi32->hits.at(ifi32)->GetEloss()) // if the hit was used already, continue
                    {
                        if (debug_loopin)
                            cout << "Fi32 hit already used or not correct charge " << charge_requested << ", "
                                 << fi32->hits.at(ifi32)->GetEloss() << endl;
                        ifi32 += 1;
                        continue;
                    }
                    do // fi30
                    {
                        if (ifi30 >= 0)
                            fh_eloss_fi30_mc->Fill(1000. * fi30->hits.at(ifi30)->GetEloss()); // MeV
                        if ((ifi30 >= 0 && !fi30->free_hit[ifi30]) ||
                            charge_requested !=
                                fi30->hits.at(ifi30)->GetEloss()) // if the hit was used already, continue
                        {
                            if (debug_loopin)
                                cout << "Fi30 hit already used or not correct charge " << charge_requested << ", "
                                     << fi30->hits.at(ifi30)->GetEloss() << endl;
                            ifi30 += 1;
                            continue;
                        }
                        do // fi23b
                        {
                            if (ifi23b >= 0)
                                fh_eloss_fi23b_mc->Fill(1000. * fi23b->hits.at(ifi23b)->GetEloss()); // MeV
                            if ((l < 2 && ifi23b >= 0 && !fi23b->free_hit[ifi23b]) ||
                                (l == 2 && ((ifi23b >= 0 && !fi23b->free_hit[ifi23b]) ||
                                            std::abs(det_hit_yC[2] - fi23b->hits.at(ifi23b)->GetY()) <
                                                dfib))) // if the hit was used already, continue
                            {
                                if (debug_loopin)
                                    cout << "left - Fi23b hit : " << fi23b->hits.at(ifi23b)->GetY() << " already used"
                                         << ", " << det_hit_yC[2] << endl;
                                ifi23b += 1;
                                continue;
                            }

                            do // fi23a
                            {
                                if (ifi23a >= 0)
                                    fh_eloss_fi23a_mc->Fill(1000. * fi23a->hits.at(ifi23a)->GetEloss()); // MeV
                                if ((l < 2 && ifi23a >= 0 && !fi23a->free_hit[ifi23a]) ||
                                    (l == 2 && ((ifi23a >= 0 && !fi23a->free_hit[ifi23a]) ||
                                                std::abs(det_hit_xC[1] - fi23a->hits.at(ifi23a)->GetX()) <
                                                    dfib))) //(ifi23a >= 0 && !fi23a->free_hit[ifi23a]) // if the hit
                                                            // was used already, continue
                                {
                                    if (debug_loopin)
                                        cout << "Fi23a hit already used" << endl;
                                    ifi23a += 1;
                                    continue;
                                }
                                z0 = 0;

                                // Create object for particle which will be fitted
                                if (l < 2)
<<<<<<< HEAD
                                {
                                    candidate = new R3BTrackingParticle(charge, x0, y0, z0, 0., 0., p0, beta0, m0);
                                }
                                else if (l == 2)
                                {
                                    candidate = new R3BTrackingParticle(
                                        charge, xmem, ymem, zmem, 0., 0., pBeamz - pzmem, beta0, m0);
                                }
=======
                                    ixtmax = 1;
                                if (l == 2)
                                    ixtmax = 1;
                                for (Int_t ixt = 0; ixt < ixtmax; ixt++)
                                {
                                    if (ixt == 0)
                                    {
                                        x0 = 0.0;
                                        y0 = 0.0;
                                    }
                                    else if (ixt == 1)
                                    {
                                        x0 = 0.1;
                                        y0 = 0.1;
                                    }
                                    else if (ixt == 2)
                                    {
                                        x0 = 0.1;
                                        y0 = -0.1;
                                    }
                                    else if (ixt == 3)
                                    {
                                        x0 = -0.1;
                                        y0 = 0.1;
                                    }
                                    else if (ixt == 4)
                                    {
                                        x0 = -0.1;
                                        y0 = -0.1;
                                    }
                                    z0 = 0;

                                    // Create object for particle which will be fitted
                                    if (l < 2)
                                    {
                                        candidate = new R3BTrackingParticle(charge, x0, y0, z0, 0., 0., p0, beta0, m0);
                                    }
                                    else
                                    {
                                        candidate =
                                            new R3BTrackingParticle(charge, xmem, ymem, zmem, 0., 0., p0, beta0, m0);
                                    }
>>>>>>> working on the optimization of geometry

                                if (debug_loopin)
                                {
                                    cout << "left side of setup" << endl;
                                    cout << "Charge requested: " << charge_requested << endl;
                                    cout << "Start values to fit, x0: " << x0 << " y0: " << y0 << " z0: " << z0
                                         << " p0: " << p0 << " beta0: " << beta0 << " m0: " << m0 << endl;
                                    cout << "Hit Tofd # " << i << " x: " << tof->hits.at(i)->GetX()
                                         << " y: " << tof->hits.at(i)->GetY() << endl;
                                    if (ifi23a > -1)
                                        cout << " Fi23a left # " << ifi23a << " x: " << fi23a->hits.at(ifi23a)->GetX()
                                             << endl;
                                    if (ifi23b > -1)
                                        cout << " left Fi23b # " << ifi23b << " y: " << fi23b->hits.at(ifi23b)->GetY()
                                             << endl;
<<<<<<< HEAD
                                    if (ifi30 > -1)
                                        cout << " fi30 # " << ifi30 << " x: " << fi30->hits.at(ifi30)->GetX()
                                             << ", q: " << fi30->hits.at(ifi30)->GetEloss() << endl;
                                    if (ifi32 > -1)
                                        cout << " fi32 # " << ifi32 << " x: " << fi32->hits.at(ifi32)->GetX()
                                             << ", q: " << fi32->hits.at(ifi32)->GetEloss() << endl;
                                    cout << "Hit target # "
                                         << " x: " << target->hits.at(0)->GetX() << endl;
                                }
                                // add points through which tracker has to go:
                                candidate->AddHit("target", 0);
                                candidate->AddHit("tofd", i);
                                candidate->AddHit("fi23a", ifi23a);
                                candidate->AddHit("fi23b", ifi23b);
                                candidate->AddHit("fi32", ifi32);
                                candidate->AddHit("fi30", ifi30);

                                fDetectors = fDetectorsLeft;

                                Int_t status = 10;
                                if (fForward)
                                {
                                    status = fFitter->FitTrackMomentumForward(candidate, fDetectors);
                                }
                                else
                                {
                                    // status = fFitter->FitTrackBackward2D(candidate, fDetectors);
                                    status = fFitter->FitTrackMomentumBackward(candidate, fDetectors);
                                }
                                if (debug_loopin)
                                    cout << " Chi: " << candidate->GetChi2() << "  "
                                         << candidate->GetStartMomentum().Mag() << "  "
                                         << 1000. * (candidate->GetStartMomentum().Mag() - p0) *
                                                (candidate->GetStartMomentum().Mag() - p0)
                                         << endl;
                                if (debug_loopin)
                                    cout << "--------------------------------" << endl;
                                nCand += 1;
                                Icountleft += 1;

                                //     cout << fNEvents<< ", LEFT SIDE: Charge: "<< charge<<", Momentum: " <<
                                //     candidate->GetMomentum().Mag()<<
                                //  ", Momentum Z: "<<candidate->GetMomentum().Z() <<
                                //   ", Momentum X: "<<candidate->GetMomentum().X()<< endl;

                                if (TMath::IsNaN(candidate->GetMomentum().Z()))
                                {
                                    delete candidate;
                                    continue;
                                }

                                if (10 > status)
                                {
=======
                                    if (ifi30 > -1 && debug)
                                        cout << " fi30 # " << ifi30 << " x: " << fi30->hits.at(ifi30)->GetX() << endl;
                                    if (ifi32 > -1 && debug)
                                        cout << " fi32 # " << ifi32 << " x: " << fi32->hits.at(ifi32)->GetX() << endl;
                                    if (debug)
                                        cout << "Hit target # "
                                             << " x: " << target->hits.at(ixt)->GetX() << endl;
                                    // add points through which tracker has to go:
                                    candidate->AddHit("target", 0);
                                    candidate->AddHit("tofd", i);
                                    candidate->AddHit("fi23a", ifi23a);
                                    candidate->AddHit("fi23b", ifi23b);
                                    candidate->AddHit("fi32", ifi32);
                                    candidate->AddHit("fi30", ifi30);

                                    //fDetectors = fDetectorsLeft;

                                    Int_t status = 10;
>>>>>>> working on the optimization of geometry
                                    if (fForward)
                                    {
                                        candidate->Reset();
                                    }
                                    else
                                    {
                                        candidate->SetStartPosition(candidate->GetPosition());
                                        candidate->SetStartMomentum(-1. * candidate->GetMomentum());
                                        candidate->SetStartBeta(beta0);
                                        candidate->UpdateMomentum();

                                        // candidate->SetStartPosition(candidate->GetPosition()); // @target
                                        // candidate->SetStartMomentum(-1. * candidate->GetMomentum());
                                        // candidate->SetStartBeta(0.8328);
                                        // candidate->SetStartBeta(beta0);
                                        // candidate->UpdateMomentum();
                                        candidate->Reset();

                                        // candidate->GetStartPosition().Print();
                                        // candidate->GetStartMomentum().Print();
                                        // cout << "chi2: " << candidate->GetChi2() << endl;
                                        // status = FitFragment(candidate);
                                    }
                                    if (10 > status)
                                    {
                                        // if(candidate->GetChi2() < 3.)
                                        {
<<<<<<< HEAD
                                            fFragments.push_back(candidate);
=======
                                            candidate->Reset();
                                        }
                                        else
                                        {
                                            //candidate->SetStartPosition(candidate->GetPosition());
                                            //candidate->SetStartMomentum(-1. * candidate->GetMomentum());
                                            //candidate->SetStartBeta(beta0);
                                            //candidate->UpdateMomentum();

                                            // candidate->SetStartPosition(candidate->GetPosition()); // @target
                                            // candidate->SetStartMomentum(-1. * candidate->GetMomentum());
                                            // candidate->SetStartBeta(0.8328);
                                            // candidate->SetStartBeta(beta0);
                                            // candidate->UpdateMomentum();
                                            candidate->Reset();

                                            // candidate->GetStartPosition().Print();
                                            // candidate->GetStartMomentum().Print();
                                            // cout << "chi2: " << candidate->GetChi2() << endl;
                                            // status = FitFragment(candidate);
                                        }
                                        if (10 > status)
                                        {
                                            // if(candidate->GetChi2() < 3.)
                                            {
                                                fFragments.push_back(candidate);
                                            }
                                        }
                                        else
                                        {
                                            delete candidate;
>>>>>>> working on the optimization of geometry
                                        }
                                    }
                                    else
                                    {
                                        delete candidate;
                                    }
                                }
                                else
                                {
                                    delete candidate;
                                }
                                //   } // for target y0

                                // return;
                                ifi23a += 1;
                            } while (ifi23a < fi23a->hits.size());
                            ifi23a = 0;
                            if (0 == fi23a->hits.size())
                                ifi23a = -1;

                            ifi23b += 1;
                        } while (ifi23b < fi23b->hits.size());
                        ifi23b = 0;
                        if (0 == fi23b->hits.size())
                            ifi23b = -1;

                        ifi30 += 1;
                    } while (ifi30 < fi30->hits.size());
                    ifi30 = 0;
                    if (0 == fi30->hits.size())
                        ifi30 = -1;

                    ifi32 += 1;
                } while (ifi32 < fi32->hits.size());
                ifi32 = 0;
                if (0 == fi32->hits.size())
                    ifi32 = -1;
            } // end if left branch

            if (tof->hits.at(i)->GetX() < 0 && fi31->hits.size() > 0 && fi33->hits.size() > 0 &&
                fi23a->hits.size() > 0 && fi23b->hits.size() > 0)
            {
                // right branch in beam direction, don't consider hits in the detectors of the other side

                if (!fSimu)
                {
                    //( (R3BGladFieldMap*) FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(1.004);

                    // start values:
                    //	( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleY(-14.);
                    //     ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionZ(172.5);
                    //     ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionY(1.5);
                    //     ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(0.);
                    //     ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(1.00000);
                    //    ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.0);
                    // first optimization, right and left same:
                    /*	( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleY(-14.);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionZ(179.1);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionY(1.);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(0.);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(1.0011);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.0);*/
                    // second optimization
                    //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleY(-13.88482);
                    //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionZ(178.7732);
                    //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionY(2.041127);
                    //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleZ(-0.3535);
                    //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(1.002188);
                    //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleX(0.0);
                    // second optimization angleZ=0
                    /*	( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                       )->SetTrackerCorrectionAngleY(-13.86857); ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                       )->SetTrackerCorrectionZ(179.0529); ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                       )->SetTrackerCorrectionY(2.122505); ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                       )->SetTrackerCorrectionAngleZ(-0.3535); ( (R3BGladFieldMap*)
                       FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(1.001629); ( (R3BGladFieldMap*)
                       FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.0);*/
                    // third optimization
                    /*( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleY(-14.00583);
                    ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionZ(175.4381);
                    ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionY(1.72047);
                    ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(0.);
                    ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(0.9895);
                    ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.0);*/
                    // fourth optimization
                    /*  ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                      )->SetTrackerCorrectionAngleY(-13.97821); ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                      )->SetTrackerCorrectionZ(175.4601); ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                      )->SetTrackerCorrectionY(1.74234); ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                      )->SetTrackerCorrectionAngleZ(0.); ( (R3BGladFieldMap*)
                      FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(0.98795); ( (R3BGladFieldMap*)
                      FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.0);*/
                    // fifth optimization
                    /*	( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                       )->SetTrackerCorrectionAngleY(-13.38015); ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                       )->SetTrackerCorrectionZ(173.6426); ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                       )->SetTrackerCorrectionY(0.87293); ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                       )->SetTrackerCorrectionAngleZ(0.03423889); ( (R3BGladFieldMap*)
                       FairRunAna::Instance()->GetField() )->SetTrackerCorrectionScale(0.98219); ( (R3BGladFieldMap*)
                       FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.01777889);	*/
                    // seventh optimization
                    /*	( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleY(-13.5);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionZ(173.5);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionY(1.5);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(0.02);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionScale(0.99);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.03);*/
                    // seventh optimization test
                    /*	( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleY(-13.5);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionZ(173.7);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionY(1.3);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(0.02);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionScale(0.991);
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.03);
                     */
                }
                // simu from 01/02/22:
                if (fSimu)
                {
                    /*  ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleY(-14.);
                      ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionZ(174.95);
                      ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionY(1.08);
                      ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleZ(0.);
                      ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(1.00);*/
                }

<<<<<<< HEAD
                R3BTrackingDetector* target = fDetectorsRight->GetByName("target");
                target->hits.clear();
                if (fNEventsRight == 0)
=======
                //R3BTrackingDetector* target = fDetectorsRight->GetByName("target");
                //if (fNEventsRight == 0)
>>>>>>> working on the optimization of geometry
                {
                    // R3BHit(detId, x,  y, eloss,  time, hitId = -1);
                    target->hits.push_back(new R3BHit(0, 0.0, 0.0, 0., 0., 0));
                }

                do // fi33
                {
                    if (ifi33 >= 0)
                        fh_eloss_fi33_mc->Fill(1000. * fi33->hits.at(ifi33)->GetEloss()); // MeV
                    if ((ifi33 >= 0 && !fi33->free_hit[ifi33]) ||
                        charge_requested != fi33->hits.at(ifi33)->GetEloss()) // if the hit was used already, continue
                    {
                        if (debug_loopin)
                            cout << "Fi33 hit already used or not correct charge " << charge_requested << ", "
                                 << fi33->hits.at(ifi33)->GetEloss() << endl;
                        ifi33 += 1;
                        continue;
                    }
                    do // fi31
                    {
                        if (ifi31 >= 0)
                            fh_eloss_fi31_mc->Fill(1000. * fi31->hits.at(ifi31)->GetEloss()); // MeV
                        if ((ifi31 >= 0 && !fi31->free_hit[ifi31]) ||
                            charge_requested !=
                                fi31->hits.at(ifi31)->GetEloss()) // if the hit was used already, continue
                        {
                            if (debug_loopin)
                                cout << "Fi31 hit already used or not correct charge " << charge_requested << ", "
                                     << fi31->hits.at(ifi31)->GetEloss() << endl;
                            ifi31 += 1;
                            continue;
                        }
                        do // fi23b
                        {
                            if (ifi23b >= 0)
                                fh_eloss_fi23b_mc->Fill(1000. * fi23b->hits.at(ifi23b)->GetEloss()); // MeV
                            if ((l < 2 && ifi23b >= 0 && !fi23b->free_hit[ifi23b]) ||
                                (l == 2 && ((ifi23b >= 0 && !fi23b->free_hit[ifi23b]) ||
                                            std::abs(det_hit_yC[2] - fi23b->hits.at(ifi23b)->GetY()) <
                                                dfib))) // if the hit was used already, continue
                            {
                                if (debug_loopin)
                                    cout << "right Fi23b hit : " << fi23b->hits.at(ifi23b)->GetY() << " already used"
                                         << ", " << det_hit_yC[2] << endl;
                                ifi23b += 1;
                                continue;
                            }
                            do // fi23a
                            {
                                if (ifi23a >= 0)
                                    fh_eloss_fi23a_mc->Fill(1000. * fi23a->hits.at(ifi23a)->GetEloss()); // MeV
                                if ((l < 2 && ifi23a >= 0 && !fi23a->free_hit[ifi23a]) ||
                                    (l == 2 && ((ifi23a >= 0 && !fi23a->free_hit[ifi23a]) ||
                                                std::abs(det_hit_xC[1] - fi23a->hits.at(ifi23a)->GetX()) <
<<<<<<< HEAD
                                                    dfib))) //(ifi23a >= 0 && !fi23a->free_hit[ifi23a]) // if the hit
=======
                                                    0.03))) //(ifi23a >= 0 && !fi23a->free_hit[ifi23a]) // if the hit
>>>>>>> working on the optimization of geometry
                                                            // was used already, continue
                                {
                                    if (debug_loopin)
                                        cout << "Fi23a hit already used" << endl;
                                    ifi23a += 1;
                                    continue;
                                }

                                z0 = 0;

                                // Create object for particle which will be fitted
                                if (l < 2)
<<<<<<< HEAD
                                {
                                    candidate = new R3BTrackingParticle(charge, x0, y0, z0, 0., 0., p0, beta0, m0);
                                }
                                else if (l == 2)
                                {
                                    candidate = new R3BTrackingParticle(
                                        charge, xmem, ymem, zmem, 0., 0., pBeamz - pzmem, beta0, m0);
                                }
=======
                                    ixtmax = 1;
                                if (l == 2)
                                    ixtmax = 1;
                                for (Int_t ixt = 0; ixt < ixtmax; ixt++)
                                {
                                    if (ixt == 0)
                                    {
                                        x0 = 0.0; 
                                        y0 = 0.0;
                                    }
                                    else if (ixt == 1)
                                    {
                                        x0 = 0.1;
                                        y0 = 0.1;
                                    }
                                    else if (ixt == 2)
                                    {
                                        x0 = 0.1;
                                        y0 = -0.1;
                                    }
                                    else if (ixt == 3)
                                    {
                                        x0 = -0.1;
                                        y0 = 0.1;
                                    }
                                    else if (ixt == 4)
                                    {
                                        x0 = -0.1;
                                        y0 = -0.1;
                                    }
                                    z0 = 0;

                                    // Create object for particle which will be fitted
                                    if (l < 2)
                                    {
                                        candidate = new R3BTrackingParticle(charge, x0, y0, z0, 0., 0., p0, beta0, m0);
                                    }
                                    else
                                    {
                                        candidate =
                                            new R3BTrackingParticle(charge, xmem, ymem, zmem, 0., 0., p0, beta0, m0);
                                    }
>>>>>>> working on the optimization of geometry

                                if (debug_loopin)
                                {
                                    cout << "right side of setup" << endl;
                                    cout << "Charge requested: " << charge_requested << endl;
                                    cout << "Start values to fit, x0: " << x0 << " y0: " << y0 << " z0: " << z0
                                         << " p0: " << p0 << " beta0: " << beta0 << " m0: " << m0 << endl;
                                    cout << "Hit Tofd # " << i << " x: " << tof->hits.at(i)->GetX()
                                         << " y: " << tof->hits.at(i)->GetY() << endl;

                                    if (ifi23a > -1)
                                        cout << " Fi23a right # " << ifi23a << " x: " << fi23a->hits.at(ifi23a)->GetX()
                                             << ", q: " << fi23a->hits.at(ifi23a)->GetEloss() << endl;
                                    if (ifi23b > -1)
                                        cout << "right Fi23b # " << ifi23b << " y: " << fi23b->hits.at(ifi23b)->GetY()
<<<<<<< HEAD
                                             << ", q: " << fi23b->hits.at(ifi23b)->GetEloss() << endl;
                                    if (ifi33 > -1)
                                        cout << "Fi33 # " << ifi33 << " x: " << fi33->hits.at(ifi33)->GetX()
                                             << ", q: " << fi33->hits.at(ifi33)->GetEloss() << endl;
                                    if (ifi31 > -1)
                                        cout << "Fi31  # " << ifi31 << " x: " << fi31->hits.at(ifi31)->GetX()
                                             << ", q: " << fi31->hits.at(ifi31)->GetEloss() << endl;
                                    cout << "Hit target # "
                                         << " x: " << target->hits.at(0)->GetX() << endl;
                                }

                                candidate->AddHit("target", 0);
                                candidate->AddHit("tofd", i);
                                candidate->AddHit("fi23a", ifi23a);
                                candidate->AddHit("fi23b", ifi23b);
                                candidate->AddHit("fi31", ifi31);
                                candidate->AddHit("fi33", ifi33);

                                fDetectors = fDetectorsRight;

                                Int_t status = 10;
                                if (fForward)
                                {
                                    status = fFitter->FitTrackMomentumForward(candidate, fDetectors);
                                }
                                else
                                {
                                    // status = fFitter->FitTrackBackward2D(candidate, fDetectors);
                                    status = fFitter->FitTrackMomentumBackward(candidate, fDetectors);
                                }
                                if (debug_loopin)
                                    cout << "Chi: " << candidate->GetChi2() << "  pstart.Mag "
                                         << candidate->GetStartMomentum().Mag() << " dp.Mag "
                                         << 1000. * (candidate->GetStartMomentum().Mag() - p0) *
                                                (candidate->GetStartMomentum().Mag() - p0)
                                         << endl;
                                if (debug_loopin)
                                    cout << "--------------------------------" << endl;
                                nCand += 1;
                                Icountright += 1;

                                //    cout <<fNEvents<<", RIGHT SIDE: Charge: "<< charge<<", Momentum: " <<
                                //    candidate->GetMomentum().Mag()<<", Momentum Z:
                                //    "<<candidate->GetMomentum().Z()<<
                                //   ", Momentum X: "<<candidate->GetMomentum().X() << " status: "<<status<<endl;

                                if (TMath::IsNaN(candidate->GetMomentum().Z()))
                                {
                                    delete candidate;
                                    continue;
                                }

                                if (10 > status)
                                {
=======
                                             << endl;
                                    if (ifi33 > -1 && debug)
                                        cout << "Fi33 # " << ifi33 << " x: " << fi33->hits.at(ifi33)->GetX() << endl;
                                    if (ifi31 > -1 && debug)
                                        cout << "Fi31  # " << ifi31 << " x: " << fi31->hits.at(ifi31)->GetX() << endl;
                                    if (debug)
                                        cout << "Hit target # "
                                             << " x: " << target->hits.at(ixt)->GetX() << endl;

                                    candidate->AddHit("target", 0);
                                    candidate->AddHit("tofd", i);
                                    candidate->AddHit("fi23a", ifi23a);
                                    candidate->AddHit("fi23b", ifi23b);
                                    candidate->AddHit("fi31", ifi31);
                                    candidate->AddHit("fi33", ifi33);

                                    //fDetectors = fDetectorsRight;

                                    Int_t status = 10;
>>>>>>> working on the optimization of geometry
                                    if (fForward)
                                    {
                                        candidate->Reset();
                                    }
                                    else
                                    {
                                        candidate->SetStartPosition(candidate->GetPosition());
                                        candidate->SetStartMomentum(-1. * candidate->GetMomentum());
                                        candidate->SetStartBeta(beta0);
                                        candidate->UpdateMomentum();

                                        // candidate->SetStartPosition(candidate->GetPosition());
                                        // candidate->SetStartMomentum(-1. * candidate->GetMomentum());
                                        // candidate->SetStartBeta(0.8328);
                                        // candidate->SetStartBeta(beta0);
                                        // candidate->UpdateMomentum();

                                        candidate->Reset();

                                        // candidate->GetStartPosition().Print();
                                        // candidate->GetStartMomentum().Print();
                                        // cout << "chi2: " << candidate->GetChi2() << endl;
                                        // status = FitFragment(candidate);
                                    }
                                    if (10 > status)
                                    {
<<<<<<< HEAD
                                        // if(candidate->GetChi2() < 3.)
=======
                                        if (fForward)
                                        {
                                            candidate->Reset();
                                        }
                                        else
                                        {
                                            //candidate->SetStartPosition(candidate->GetPosition());
                                            //candidate->SetStartMomentum(-1. * candidate->GetMomentum());
                                            //candidate->SetStartBeta(beta0);
                                            //candidate->UpdateMomentum();

                                            // candidate->SetStartPosition(candidate->GetPosition());
                                            // candidate->SetStartMomentum(-1. * candidate->GetMomentum());
                                            // candidate->SetStartBeta(0.8328);
                                            // candidate->SetStartBeta(beta0);
                                            // candidate->UpdateMomentum();

                                            candidate->Reset();

                                            // candidate->GetStartPosition().Print();
                                            // candidate->GetStartMomentum().Print();
                                            // cout << "chi2: " << candidate->GetChi2() << endl;
                                            // status = FitFragment(candidate);
                                        }
                                        if (10 > status)
                                        {
                                            // if(candidate->GetChi2() < 3.)
                                            {
                                                fFragments.push_back(candidate);
                                            }
                                        }
                                        else
>>>>>>> working on the optimization of geometry
                                        {
                                            fFragments.push_back(candidate);
                                        }
                                    }
                                    else
                                    {
                                        delete candidate;
                                    }
                                }
                                else
                                {
                                    delete candidate;
                                }
                                //  } // for target x0 y0

                                // return;
                                ifi23a += 1;
                            } while (ifi23a < fi23a->hits.size());
                            ifi23a = 0;
                            if (0 == fi23a->hits.size())
                                ifi23a = -1;

                            ifi23b += 1;
                        } while (ifi23b < fi23b->hits.size());
                        ifi23b = 0;
                        if (0 == fi23b->hits.size())
                            ifi23b = -1;

                        ifi31 += 1;
                    } while (ifi31 < fi31->hits.size());
                    ifi31 = 0;
                    if (0 == fi31->hits.size())
                        ifi31 = -1;

                    ifi33 += 1;
                } while (ifi33 < fi33->hits.size());
                ifi33 = 0;
                if (0 == fi33->hits.size())
                    ifi33 = -1;
            } // end if right branch

            charge_mem = charge;

        } // end for TofD

        charge = charge_mem;
        Charge = float(charge_mem);

        // if (candidate->GetSize() > 0 && !fSimu)
        {
            // candidate->Clear();
        }

        //   if(candidate) delete candidate;

        fh_ncand->Fill(nCand);

        R3BTrackingParticle* bestcandidate =
            new R3BTrackingParticle(-1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000);
        minChi2 = 1e10;
        Double_t pChi2 = 0.;
        Double_t parChi2 = 0.;
        Double_t xChi2 = 0.;

        if (fFragments.size() > 0)
        {
            for (auto const& x : fFragments)
            {
                pChi2 = 10. * (candidate->GetStartMomentum().Mag() - p0) * (candidate->GetStartMomentum().Mag() - p0) /
                        (p0 * p0 * 0.01 * 0.01);
                xChi2 = x->GetChi2();

                // parChi2 = sqrt(pChi2 * pChi2 + xChi2 * xChi2);
                parChi2 = xChi2;
                if (parChi2 < minChi2)
                {
<<<<<<< HEAD
                    bestcandidate = x;
                    minChi2 = parChi2;
                    // cout << "New min chi2: " << minChi2 << endl;
                    // cout << "Corresponding Mass   : " << x->GetMass() << endl;
                    // cout << "Corresponding Mass   : " << bestcandidate->GetMass() << endl;
=======
                    if (parChi2 < minChi2)
                    {
                        bestcandidate = x;
                        minChi2 = parChi2;
                        pzmem = x->GetStartMomentum().Z();
                        xmem = x->GetStartPosition().X();
                        ymem = x->GetStartPosition().Y();
                        zmem = x->GetStartPosition().Z();
                        if (l == 1)
                        {
                            pCx = x->GetStartMomentum().X() * 1000.;
                            pCy = x->GetStartMomentum().Y() * 1000.;
                            pCz = x->GetStartMomentum().Z() * 1000.;
                            carbonP.SetPxPyPzE(
                                pCx, pCy, pCz, sqrt(pow(pCx, 2) + pow(pCy, 2) + pow(pCz, 2) + pow(mC, 2)));
                            p12C = carbonP.Vect();
                            // cout << "For event: "<<fNEvents_nonull<<" new min chi2 for C: " << minChi2 <<endl;
                            minChi2_12C = minChi2;
                        }
                        // cout << "New min chi2: " << minChi2 << endl;
                        // cout << "Corresponding Mass   : " << x->GetMass() << endl;
                        // cout << "Corresponding Mass   : " << bestcandidate->GetMass() << endl;
                    }
                }
                if (l == 2) // 4He
                {
                    Double_t pHex = x->GetStartMomentum().X() * 1000.;
                    Double_t pHey = x->GetStartMomentum().Y() * 1000.;
                    Double_t pHez = x->GetStartMomentum().Z() * 1000.;
                    alphaP.SetPxPyPzE(pHex, pHey, pHez, sqrt(pow(pHex, 2) + pow(pHey, 2) + pow(pHez, 2) + pow(mHe, 2)));
                    Double_t theta_26 = alphaP.Angle(carbonP.Vect()) * TMath::RadToDeg(); // angle alpha carbon (theta)
                    Double_t m_inva = (alphaP + carbonP).M();                             // invariant mass
                    Erel = m_inva - mHe - mC;                                             // relative Energy
                    Double_t Erel_check = 3.87359e-01 - 3.55075e-01 * theta_26 + 6.57182e-01 * theta_26 * theta_26;
					fh_Erel->Fill(Erel);
                    // cout<<"4He selecting: "<<endl;
                    // cout << "Erel: " << Erel << "; chi2 C/He: " << minChi2_12C <<", "<<minChi2 << endl;
                    // cout<<"candidate: "<<x->GetStartMomentum().Z()<<", "<<x->GetStartPosition().X()<<", "<<
                    // x->GetStartPosition().Y()<<endl;

                    // cout << "For event: "<<fNEvents_nonull<<" possible He: " << parChi2 << ", Erel: "<<Erel<<",
                    // "<<Erel_check<<endl;

                    if (parChi2 < minChi2)
                    // if (abs(Erel-Erel_check) < 1.1  && parChi2 < minChi2)
                    {
                        // if (parChi2 < minChi2)
                        {
                            bestcandidate = x;
                            minChi2 = parChi2;

                            // cout << "For event: "<<fNEvents_nonull<<" new min chi2 for He: " << minChi2 << ", Erel:
                            // "<<Erel<<", "<<Erel_check<<endl;

                            // cout << "Corresponding Mass   : " << x->GetMass() << endl;
                            // cout << "Corresponding Mass   : " << bestcandidate->GetMass() << endl;
                        }
                    }
                    //	cout<<"selected: "<<bestcandidate->GetStartMomentum().Z()<<",
                    //"<<bestcandidate->GetStartPosition().X()<<", "<< 	bestcandidate->GetStartPosition().Y()<<",
                    //"<<minChi2<<", "<<Erel<<endl;
>>>>>>> working on the optimization of geometry
                }
            }

            if (minChi2 > 1.e5)
                continue;

            if (bestcandidate->GetStartMomentum().X() < 0)
            {
                fi23a->free_hit[bestcandidate->GetHitIndexByName("fi23a")] = false;
                fi23b->free_hit[bestcandidate->GetHitIndexByName("fi23b")] = false;
                fi31->free_hit[bestcandidate->GetHitIndexByName("fi31")] = false;
                fi33->free_hit[bestcandidate->GetHitIndexByName("fi33")] = false;
            }
            else
            {
                fi23a->free_hit[bestcandidate->GetHitIndexByName("fi23a")] = false;
                fi23b->free_hit[bestcandidate->GetHitIndexByName("fi23b")] = false;
                fi30->free_hit[bestcandidate->GetHitIndexByName("fi30")] = false;
                fi32->free_hit[bestcandidate->GetHitIndexByName("fi32")] = false;
            }
            tof->free_hit[bestcandidate->GetHitIndexByName("tofd")] = false;

            Double_t x0soll = 0.;
            Double_t y0soll = 0.;
            Double_t z0soll = 0.;
            Double_t psoll = 0.;
            Double_t px0soll = 0.;
            Double_t py0soll = 0.;
            Double_t pz0soll = 0.;
            Double_t beta0soll = 0.;
            Double_t m0soll = 0.;

            if (l == 0)
            {
                if (debug_loopout)
                    cout << "16O" << endl;
                ;
                oxygen = kTRUE;
                if (fSimu)
                {
                    x0soll = x0O;
                    y0soll = y0O;
                    z0soll = z0O;
                    px0soll = px0O;
                    py0soll = py0O;
                    pz0soll = pz0O;
                    psoll = pO;
                    m0soll = massO;
                    beta0soll = betaO;
                }
                else
                {
                    x0soll = x0;
                    y0soll = y0;
                    z0soll = z0;
                    px0soll = 0.;
                    py0soll = 0.;
                    pz0soll = 17.3915;
                    psoll = 17.3915;
                    m0soll = m0;
                    beta0soll = 0.7593209;
                }
            }

            if (l == 1)
            {
                if (debug_loopout)
                    cout << "12C" << endl;

                carbon = kTRUE;
                x0soll = x0C;
                y0soll = y0C;
                z0soll = z0C;
                px0soll = px0C;
                py0soll = py0C;
                pz0soll = pz0C;
                psoll = pC;
                m0soll = massC;
                beta0soll = betaC;
                counterC += 1;
                chargemem = Charge;
                minChi2_12C = minChi2;
            }

            if (l == 2)
            {
                if (debug_loopout)
                    cout << "4He" << endl;
                alpha = kTRUE;
                x0soll = xmem; // x0He;
                y0soll = ymem; // y0He;
                z0soll = zmem; // z0He;
                px0soll = px0He;
                py0soll = py0He;
                pz0soll = pz0He;
                psoll = pHe;
                m0soll = massHe;
                beta0soll = betaHe;
                counterHe += 1;
            }
            if (debug_loopout)
            {
                cout << "Results after tracking :" << endl;
                cout << "Charge   : " << charge << endl;
                cout << "Position (soll) x: " << x0soll << " y: " << y0soll << " z: " << z0soll << endl;
                cout << "Position (ist)  x: " << bestcandidate->GetStartPosition().X()
                     << " y: " << bestcandidate->GetStartPosition().Y()
                     << " z: " << bestcandidate->GetStartPosition().Z() << endl;

                cout << "Momentum (soll): " << psoll << " px : " << px0soll << " py: " << py0soll << " pz: " << pz0soll
                     << endl;
                cout << "Momentum (ist) : " << bestcandidate->GetStartMomentum().Mag()
                     << " px : " << bestcandidate->GetStartMomentum().X()
                     << " py: " << bestcandidate->GetStartMomentum().Y()
                     << " pz: " << bestcandidate->GetStartMomentum().Z() << endl;

                //   cout << "Beta   : " << bestcandidate->GetStartBeta() << endl;
                // LOG(debug) << "chi2: " << bestcandidate->GetChi2() << endl;
                cout << "chi: " << minChi2 << endl;
            }
            /*
              cout<<"Momentum (ist) : " << bestcandidate->GetStartMomentum().Mag()
                       << " px : " << bestcandidate->GetStartMomentum().X() << " py: " <<
                       bestcandidate->GetStartMomentum().Y()
                     << " pz: " << bestcandidate->GetStartMomentum().Z()<<", Mass   : " << bestcandidate->GetMass()<<
                     ", chi2: " << minChi2 <<". Position (ist)  x: " << bestcandidate->GetStartPosition().X()
                      << " y: " << bestcandidate->GetStartPosition().Y()
                      << " z: " << bestcandidate->GetStartPosition().Z()<<endl;
            */
            fh_A_reco1->Fill(bestcandidate->GetMass() / amu);
            if (minChi2 < 1.e4)
            {
                totalChi2Mass += minChi2;
                totalEvents++;
            }
            /*
                        // Now we know the mass. We can set here the exact mass if wanted and then track the momentum
                        Double_t m =  bestcandidate->GetMass();
                        if(TMath::Abs(bestcandidate->GetMass() - 14.8951) < 1.5)
                            m = 14.8951; // 16O
                        if(TMath::Abs(bestcandidate->GetMass() - 11.1749) < 1.5)
                            m = 11.1749; // 4He
                        if(TMath::Abs(bestcandidate->GetMass() - 3.7284) < 1.5)
                            m = 3.7284; // 4He

                        //bestcandidate->SetMass(m);
                        bestcandidate->UpdateMomentum();

                        fDetectors = fDetectorsLeft;

                        if(bestcandidate->GetStartMomentum().X() < 0 )
                        {
                            fDetectors = fDetectorsRight;
                        }
                        Int_t status = fFitter->FitTrackMomentum(bestcandidate, fDetectors);
                        cout << "status: " << status << endl;
                        if(status > 9) return;


                        LOG(info) << "Final result:";
                        LOG(info) << "Position x: " << bestcandidate->GetStartPosition().X() <<
                            " y: " << bestcandidate->GetStartPosition().Y() << " z: " <<
               bestcandidate->GetStartPosition().Z(); LOG(info) << "Momentum : " <<
               bestcandidate->GetStartMomentum().Mag() << " px : " << bestcandidate->GetStartMomentum().X() << " py: "
               << bestcandidate->GetStartMomentum().Y() << " pz: " << bestcandidate->GetStartMomentum().Z(); LOG(info)
               << "Mass   : " << bestcandidate->GetMass(); LOG(info) << "Beta : " << bestcandidate->GetStartBeta();
               LOG(info) << "chi2: " << bestcandidate->GetChi2() << endl;

                        fh_A_overZ->Fill(Charge, bestcandidate->GetMass() / Charge / amu);
                        fh_mom_res->Fill((bestcandidate->GetStartMomentum().Mag() - particle->GetStartMomentum().Mag())
               / particle->GetStartMomentum().Mag()); fh_chi2->Fill(bestcandidate->GetChi2());
                        fh_mass_res->Fill(bestcandidate->GetMass() - particle->GetMass());
                        fh_vz_res->Fill(bestcandidate->GetStartPosition().X() - particle->GetStartPosition().X());
                        fh_beta_res->Fill((bestcandidate->GetStartBeta() - particle->GetStartBeta()) /
               particle->GetStartBeta());
                        // bestcandidate->GetStartPosition().Print();
                        // LOG(info) << (bestcandidate->GetMass() / amu);
            */
            totalChi2P += minChi2;
            fh_p_vs_ch2->Fill(minChi2, bestcandidate->GetStartMomentum().Mag());

            if (minChi2 < 1000)
            {

                fh_p->Fill(bestcandidate->GetStartMomentum().Mag());
                fh_px->Fill(bestcandidate->GetStartMomentum().X() * 100.);
                fh_py->Fill(bestcandidate->GetStartMomentum().Y() * 100.);
                fh_pz->Fill(bestcandidate->GetStartMomentum().Z());
                if (bestcandidate->GetStartMomentum().X() < 0)
                {
                    fh_px_r->Fill(bestcandidate->GetStartMomentum().X() * 100.);
                    fh_py_r->Fill(bestcandidate->GetStartMomentum().Y() * 100.);
                    fh_pz_r->Fill(bestcandidate->GetStartMomentum().Z());
                }
                else
                {
                    fh_px_l->Fill(bestcandidate->GetStartMomentum().X() * 100.);
                    fh_py_l->Fill(bestcandidate->GetStartMomentum().Y() * 100.);
                    fh_pz_l->Fill(bestcandidate->GetStartMomentum().Z());
                }

                if (l == 1) // 12C
                {
                    pxmem = bestcandidate->GetStartMomentum().X();
                    pymem = bestcandidate->GetStartMomentum().Y();
                    pzmem = bestcandidate->GetStartMomentum().Z();
                    xmem = bestcandidate->GetStartPosition().X();
                    ymem = bestcandidate->GetStartPosition().Y();
                    zmem = bestcandidate->GetStartPosition().Z();

                    pCx = bestcandidate->GetStartMomentum().X() * 1000.;
                    pCy = bestcandidate->GetStartMomentum().Y() * 1000.;
                    pCz = bestcandidate->GetStartMomentum().Z() * 1000.;
                    carbonP.SetPxPyPzE(pCx, pCy, pCz, sqrt(pow(pCx, 2) + pow(pCy, 2) + pow(pCz, 2) + pow(mC, 2)));
                    p12C = carbonP.Vect();
                }
                if (l == 2) // 4He
                {
                    Double_t pHex = bestcandidate->GetStartMomentum().X() * 1000.;
                    Double_t pHey = bestcandidate->GetStartMomentum().Y() * 1000.;
                    Double_t pHez = bestcandidate->GetStartMomentum().Z() * 1000.;
                    alphaP.SetPxPyPzE(pHex, pHey, pHez, sqrt(pow(pHex, 2) + pow(pHey, 2) + pow(pHez, 2) + pow(mHe, 2)));
                    p4He = alphaP.Vect();
                    if (carbon)
                    {
                        theta_26 = alphaP.Angle(carbonP.Vect()) * TMath::RadToDeg(); // angle alpha carbon (theta)
                        Double_t m_inva = (alphaP + carbonP).M();                    // invariant mass
                        Erel = m_inva - mHe - mC;                                    // relative Energy
                        psum = (p12C + p4He).Mag();
                    }
                }

                if (l == 0) // 16O
                {
                    Oxygen.SetPxPyPzE(bestcandidate->GetStartMomentum().X() * 1000.,
                                      bestcandidate->GetStartMomentum().Y() * 1000.,
                                      bestcandidate->GetStartMomentum().Z() * 1000.,
                                      sqrt(pow(bestcandidate->GetStartMomentum().X() * 1000., 2) +
                                           pow(bestcandidate->GetStartMomentum().Y() * 1000., 2) +
                                           pow(bestcandidate->GetStartMomentum().Z() * 1000., 2) +
                                           pow(15.99065084 * 931.49410242, 2)));

                    Double_t theta_16O = Oxygen.Theta() * TMath::RadToDeg();
                    Double_t phi_16O = Oxygen.Phi() * TMath::RadToDeg();
                    Double_t pz_16 = bestcandidate->GetStartMomentum().Z() * 1000.;
                    fh_theta_16O->Fill(theta_16O, pz_16);
                    fh_phi_16O->Fill(phi_16O, pz_16);
                    // cout<<"abgles;: "<<	bestcandidate->GetStartMomentum().Z()* 1000.<<", "<<theta_16O<<",
                    // "<<phi_16O<<endl;
                }
            }
            Double_t beta_beam = 0.7593209;
            Double_t gamma_beam = 1. / sqrt(1. - beta_beam * beta_beam);
            Double_t mass_check = bestcandidate->GetStartMomentum().Mag() / beta_beam / gamma_beam / amu;
            // cout<<"mass_check: "<<mass_check/ amu<<", "<<bestcandidate->GetMass()/ amu<<endl;
            // bestcandidate->SetMass(mass_check);
            // cout<<"new mass: "<<bestcandidate->GetMass()/ amu<<endl;
            fh_A_reco2->Fill(mass_check); // bestcandidate->GetMass() / amu);
            AoverZ = mass_check / Charge;
            fh_A_overZ->Fill(AoverZ, Charge);
            if (l == 1)
            {
                AoverZmem = AoverZ;
                if (AoverZ > 1.9 && AoverZ < 2.1)
                    iAoverZ = 2;
                iAoverZmem = iAoverZ;
            }
            if (l == 2)
            {
                if (AoverZ > 1.7 && AoverZ < 2.4)
                    iAoverZ = 2;
            }
            fh_mass_vs_ch2->Fill(minChi2, mass_check);
            if (iAoverZ == 2)
            {
                fh_px_p->Fill(bestcandidate->GetStartMomentum().X(), bestcandidate->GetStartMomentum().Mag());
                fh_py_p->Fill(bestcandidate->GetStartMomentum().Y(), bestcandidate->GetStartMomentum().Mag());
            }

            fh_mom_res->Fill((bestcandidate->GetStartMomentum().Mag() - psoll) / psoll * 100.);
            fh_mom_res_x->Fill((bestcandidate->GetStartMomentum().X() - px0soll) / px0soll * 100.);
            fh_mom_res_y->Fill((bestcandidate->GetStartMomentum().Y() - py0soll) / py0soll * 100.);
            fh_mom_res_z->Fill((bestcandidate->GetStartMomentum().Z() - pz0soll) / pz0soll * 100.);

            fh_chi2->Fill(minChi2);
            fh_mass_res->Fill(bestcandidate->GetMass() - m0soll);
            fh_vz_res->Fill(bestcandidate->GetStartPosition().X() - x0soll);
            fh_beta_res->Fill((bestcandidate->GetStartBeta() - beta0soll) / beta0soll);
            if (l == 1)
            {
                fh_xy_target_C->Fill(bestcandidate->GetStartPosition().X(), bestcandidate->GetStartPosition().Y());
                if (fSimu)
                {
                    fh_dxdy->Fill(x0C - bestcandidate->GetStartPosition().X(),
                                  y0C - bestcandidate->GetStartPosition().Y());
                }
            }
            if (l == 2)
            {
                fh_xy_target_He->Fill(bestcandidate->GetStartPosition().X(), bestcandidate->GetStartPosition().Y());
            }
            if (l == 0)
                fh_xy_target_C->Fill(bestcandidate->GetStartPosition().X(), bestcandidate->GetStartPosition().Y());

            // bestcandidate->GetStartPosition().Print();
            // LOG(info) << (bestcandidate->GetMass() / amu);

            if (charge_requested == 6)
            {
                masmem = mass_check;
            }
            if (charge_requested == 2)
                fh_mass_corel->Fill(masmem, mass_check);

            // store hits in track level

            if (l == 2) //&& iAoverZ == 2 && iAoverZmem == 2)
            {
                if (carbon)
                {

                    /*
                    if(fi23a->hits.size() == 2 && fi23b->hits.size() == 2 &&
                       Erel > 3.9 && Erel < 4.8 &&
                       theta_26 > 2.64 && theta_26 < 2.80 &&
                       psum > 17380. && psum < 17450. &&
                       iAoverZ == 2 && iAoverZmem == 2 &&
                       sqrt(minChi2*minChi2 + minChi2_12C*minChi2_12C) < 5) bestevents = true;
                */
                    // if (iAoverZ == 2 && iAoverZmem == 2 && sqrt(minChi2 * minChi2 + minChi2_12C * minChi2_12C) < 5 &&
                    //  psum > 17380. && psum < 17450.)
                    // bestevents = true;

                    if (sqrt(minChi2 * minChi2 + minChi2_12C * minChi2_12C) < 5)
                        bestevents = true;
                }

                new ((*fTrackItems)[fNofTrackItems++]) R3BTrack(xmem, // cm
                                                                ymem, // cm
                                                                zmem, // cm
                                                                pCx,  // MeV/c
                                                                pCy,  // MeV/c
                                                                pCz,  // MeV/c
                                                                chargemem,
                                                                AoverZmem,
                                                                minChi2_12C,
                                                                minChi2_12C,
                                                                0);
                new ((*fTrackItems)[fNofTrackItems++]) R3BTrack(bestcandidate->GetStartPosition().X(),         // cm
                                                                bestcandidate->GetStartPosition().Y(),         // cm
                                                                bestcandidate->GetStartPosition().Z(),         // cm
                                                                bestcandidate->GetStartMomentum().X() * 1000., // MeV/c
                                                                bestcandidate->GetStartMomentum().Y() * 1000., // MeV/c
                                                                bestcandidate->GetStartMomentum().Z() * 1000., // MeV/c
                                                                charge_requested,
                                                                AoverZ,
                                                                minChi2,
                                                                minChi2,
                                                                0);
            }
            if (l == 0)
            {
                new ((*fTrackItems)[fNofTrackItems++]) R3BTrack(bestcandidate->GetStartPosition().X(),         // cm
                                                                bestcandidate->GetStartPosition().Y(),         // cm
                                                                bestcandidate->GetStartPosition().Z(),         // cm
                                                                bestcandidate->GetStartMomentum().X() * 1000., // MeV/c
                                                                bestcandidate->GetStartMomentum().Y() * 1000., // MeV/c
                                                                bestcandidate->GetStartMomentum().Z() * 1000., // MeV/c
                                                                charge_requested,
                                                                AoverZ,
                                                                minChi2,
                                                                minChi2,
                                                                0);
            }

            pzmemtarget = bestcandidate->GetStartMomentum().Z();
            pxmemtarget = bestcandidate->GetStartMomentum().X();
            pymemtarget = bestcandidate->GetStartMomentum().Y();
            ptot = sqrt(pxmemtarget * pxmemtarget + pymemtarget * pymemtarget + pzmemtarget * pzmemtarget);

            Int_t iDet = 0;
            Double_t pHeycheck = -10000;
            if (l == 2)
                pHeycheck = 1.6862972 + 0.32212 * pymem * 1000.;

            for (int ic = 0; ic < 8; ic++)
            {
                x_l[ic] = -10000.;
                y_l[ic] = -10000.;
                det_hit_x[ic] = -10000.;
                det_hit_y[ic] = -10000.;
                eloss_hit[ic] = 0;
                if (l == 1)
                {
                    det_hit_xC[ic] = -10000.;
                    det_hit_yC[ic] = -10000.;
                }
                if (l == 2)
                {
                    det_hit_xHe[ic] = -10000.;
                    det_hit_yHe[ic] = -10000.;
                }
            }

            fPropagator->SetVis(fVis);

            bestcandidate->Reset();

            for (auto const& det : fDetectors->GetArray())
            {
                if (debug_loopout)
                    cout << "Best track at detector: " << det->GetDetectorName() << endl;
                if (kTarget != det->section)
                {
                    if (debug_loopout)
                        cout << "Propagate to " << det->GetDetectorName() << endl;
                    fPropagator->PropagateToDetector(bestcandidate, det);
                }

                if (det->GetDetectorName().EqualTo("fi23a"))
                { // fi23a
                    Double_t eloss = det->GetEnergyLoss(bestcandidate);
                    fh_eloss_fi23a->Fill(eloss);
                    iDet = 1;
                }

                if (det->GetDetectorName().EqualTo("fi23b"))
                { // fi23b
                    Double_t eloss = det->GetEnergyLoss(bestcandidate);
                    fh_eloss_fi23b->Fill(eloss);
                    iDet = 2;
                }

                if (det->GetDetectorName().EqualTo("fi30"))
                { // fi30
                    Double_t eloss = det->GetEnergyLoss(bestcandidate);
                    fh_eloss_fi30->Fill(eloss);
                    iDet = 3;
                }

                if (det->GetDetectorName().EqualTo("fi31"))
                { // fi31
                    Double_t eloss = det->GetEnergyLoss(bestcandidate);
                    fh_eloss_fi31->Fill(eloss);
                    iDet = 4;
                }

                if (det->GetDetectorName().EqualTo("fi32"))
                { // fi32
                    Double_t eloss = det->GetEnergyLoss(bestcandidate);
                    fh_eloss_fi32->Fill(eloss);
                    iDet = 5;
                }

                if (det->GetDetectorName().EqualTo("fi33"))
                { // fi33
                    Double_t eloss = det->GetEnergyLoss(bestcandidate);
                    fh_eloss_fi33->Fill(eloss);
                    iDet = 6;
                }

                if (det->GetDetectorName().EqualTo("tofd"))
                { // tofd
                    iDet = 7;
                }

                if (fEnergyLoss)
                {
                    if (kTof != det->section)
                    {
                        Double_t weight = 1.;
                        if (kTarget == det->section)
                        {
                            weight = 0.5;
                        }
                        bestcandidate->PassThroughDetector(det, weight);
                    }
                }

                // Convert global track coordinates into local on the det plane
                det->GlobalToLocal(bestcandidate->GetPosition(), x_l[iDet], y_l[iDet]);
                R3BHit* hit = nullptr;
                Int_t hitIndex = bestcandidate->GetHitIndexByName(det->GetDetectorName().Data());
                if (hitIndex >= 0)
                    hit = fDetectors->GetHit(det->GetDetectorName().Data(), hitIndex);

                if (hit)
                {
                    det_hit_x[iDet] = hit->GetX();
                    det_hit_y[iDet] = hit->GetY();
                    eloss_hit[iDet] = hit->GetEloss();
                }
                if (hit && l == 1)
                {
                    det_hit_xC[iDet] = hit->GetX();
                    det_hit_yC[iDet] = hit->GetY();
                }
                if (hit && l == 2)
                {
                    det_hit_xHe[iDet] = hit->GetX();
                    det_hit_yHe[iDet] = hit->GetY();
                }
                if (hit && iDet == 7 && l == 1)
                {
                    yC = y_l[iDet];
                    yCexp = det_hit_y[iDet];
                }
                if (hit && iDet == 1 && l == 1)
                {
                    xCfibexp = det_hit_x[iDet];
                    xCfib = x_l[iDet];
                }
                if (hit && iDet == 1 && bestevents)
                {
                    fh_xC_vs_xHe_fib23->Fill(xCfib, x_l[iDet]);
                    fh_xC_vs_xHe_fib23_exp->Fill(xCfibexp, det_hit_x[iDet]);
                    fh_Erel_vs_x->Fill(det_hit_x[iDet], Erel);
                    fh_theta_vs_x->Fill(det_hit_x[iDet], theta_26);
                    fh_Erel_vs_nhits23a->Fill(fi23a->hits.size(), Erel);
                }
                if (hit && iDet == 2 && l == 1)
                {
                    yCfibexp = det_hit_y[iDet];
                    yCfib = y_l[iDet];
                }
                if (hit && iDet == 2 && bestevents)
                {
                    fh_yC_vs_yHe_fib23->Fill(yCfib, y_l[iDet]);
                    fh_yC_vs_yHe_fib23_exp->Fill(yCfibexp, det_hit_y[iDet]);
                    fh_Erel_vs_y->Fill(det_hit_y[iDet], Erel);
                    fh_theta_vs_y->Fill(det_hit_y[iDet], theta_26);
                    fh_Erel_vs_nhits23b->Fill(fi23b->hits.size(), Erel);
                }
                if (hit && iDet == 7 && l == 2 && bestevents)
                {
                    fh_yC_vs_yHe_Tofd->Fill(yC, y_l[iDet]);
                    fh_pyC_vs_pyHe->Fill(pCy, bestcandidate->GetStartMomentum().Y() * 1000.);
                    fh_yC_vs_yHe_Tofd_exp->Fill(yCexp, det_hit_y[iDet]);
                }

                if (hit && det->res_x > 1e-6)
                {
                    Double_t xres = x_l[iDet] - det_hit_x[iDet];
                    if (TMath::Abs(x_l[iDet] - det_hit_x[iDet]) < det->res_x)
                    {
                        xres = 0.;
                    }
                    if (debug_loopout)
                        cout << "For charge: " << charge_requested
                             << ", current lab-x position: " << bestcandidate->GetPosition().X() << "  "
                             << bestcandidate->GetPosition().Y() << "  " << bestcandidate->GetPosition().Z() << endl;

                    if (debug_loopout)
                        cout << "Residuals x - iDet: " << iDet << "; Det: " << det->GetDetectorName().Data()
                             << ", tracked local-x: " << x_l[iDet] << ", local hit_x: " << det_hit_x[iDet]
                             << ", dev: " << xres << endl;
                    if (debug_loopout)
                        cout << "bestcandidate momentum - Px: " << bestcandidate->GetStartMomentum().X()
                             << ", Py: " << bestcandidate->GetStartMomentum().Y()
                             << ", Pz: " << bestcandidate->GetStartMomentum().Z() << endl;
                    if (debug_loopout)
                        cout << "selected particle had Q= " << eloss_hit[iDet] << endl;

                    fh_x_res[iDet]->Fill(xres);
                    fh_x_pull[iDet]->Fill(xres / det->res_x);
                    if (iDet == 7)
                    {
                        if (iAoverZ == 2)
                        {
                            pz_vs_x->Fill(det_hit_x[iDet], pzmemtarget);
                            px_vs_x->Fill(det_hit_x[iDet], pxmemtarget * 1000.);
                            py_vs_x->Fill(det_hit_x[iDet], pymemtarget * 1000.);
                            p_vs_x->Fill(det_hit_x[iDet], psum / 1000.);
                        }
                    }
                }
                if (hit && det->res_y > 1e-6)
                {
                    Double_t yres = y_l[iDet] - det_hit_y[iDet];
                    if (TMath::Abs(y_l[iDet] - det_hit_y[iDet]) < det->res_y)
                    {
                        yres = 0.;
                    }
                    if (iDet == 7)
                        yres = TMath::Power((y_l[iDet] - det_hit_y[iDet]) / det->res_y, 2);
                    if (debug_loopout)
                        cout << "For charge: " << charge_requested
                             << ", current lab-y position: " << bestcandidate->GetPosition().X() << "  "
                             << bestcandidate->GetPosition().Y() << "  " << bestcandidate->GetPosition().Z() << endl;

                    if (debug_loopout)
                        cout << "Residuals y - iDet: " << iDet << ", Det: " << det->GetDetectorName().Data()
                             << ", tracked local-y:  " << y_l[iDet] << ", local hit_y: " << det_hit_y[iDet]
                             << ", dev: " << yres << endl;
                    if (debug_loopout)
                        cout << "selected particle had Q= " << eloss_hit[iDet] << endl;

                    fh_y_res[iDet]->Fill(yres);
                    fh_y_pull[iDet]->Fill(yres / det->res_y);
                    if (iDet == 7)
                    {
                        if (iAoverZ == 2)
                        {
                            pz_vs_y->Fill(det_hit_y[iDet], pzmemtarget);
                            px_vs_y->Fill(det_hit_y[iDet], pxmemtarget * 1000.);
                            py_vs_y->Fill(det_hit_y[iDet], pymemtarget * 1000.);
                            p_vs_y->Fill(det_hit_y[iDet], ptot);
                        }
                    }
                }
            }

            bestcandidate->Reset();

            if (l < 3)
            {
                fh_xfi23a_target_track->Fill(x_l[0], x_l[1]);
                fh_yfi23b_target_track->Fill(y_l[0], y_l[2]);
                fh_xfi30_fi23a_track->Fill(x_l[1], x_l[3]);
                fh_xfi30_fi32_track->Fill(x_l[3], x_l[5]);
                fh_xfi30_tofd_track->Fill(x_l[7], x_l[5]);
                fh_xfi31_fi23a_track->Fill(x_l[1], x_l[4]);
                fh_xfi31_fi33_track->Fill(x_l[4], x_l[6]);
                fh_xfi31_tofd_track->Fill(x_l[7], x_l[4]);
                fh_yFi23b_tofd_track->Fill(y_l[7], y_l[2]);

                fh_xfi30_fi23a_exp->Fill(det_hit_x[1], det_hit_x[3]);
                fh_xfi30_fi32_exp->Fill(det_hit_x[3], det_hit_x[5]);
                fh_xfi30_tofd_exp->Fill(det_hit_x[7], det_hit_x[5]);
                fh_xfi31_fi23a_exp->Fill(det_hit_x[1], det_hit_x[4]);
                fh_xfi31_fi33_exp->Fill(det_hit_x[4], det_hit_x[6]);
                fh_xfi31_tofd_exp->Fill(det_hit_x[7], det_hit_x[4]);
                fh_yFi23b_tofd_exp->Fill(det_hit_y[7], det_hit_y[2]);

                fh_tofd_track_exp->Fill(y_l[7], det_hit_y[7]);
                fh_fi23b_track_exp->Fill(y_l[2], det_hit_y[2]);
            }
            if (bestevents)
            {
                fh_xfi30_fi23a_exp_select->Fill(det_hit_xC[1], det_hit_xC[3]);
                fh_xfi30_fi23a_exp_select->Fill(det_hit_xHe[1], det_hit_xHe[3]);

                fh_xfi30_fi32_exp_select->Fill(det_hit_xC[3], det_hit_xC[5]);
                fh_xfi30_fi32_exp_select->Fill(det_hit_xHe[3], det_hit_xHe[5]);

                fh_xfi30_tofd_exp_select->Fill(det_hit_xC[7], det_hit_xC[5]);
                fh_xfi30_tofd_exp_select->Fill(det_hit_xHe[7], det_hit_xHe[5]);

                fh_xfi31_fi23a_exp_select->Fill(det_hit_xC[1], det_hit_xC[4]);
                fh_xfi31_fi23a_exp_select->Fill(det_hit_xHe[1], det_hit_xHe[4]);

                fh_xfi31_fi33_exp_select->Fill(det_hit_xC[4], det_hit_xC[6]);
                fh_xfi31_fi33_exp_select->Fill(det_hit_xHe[4], det_hit_xHe[6]);

                fh_xfi31_tofd_exp_select->Fill(det_hit_xC[7], det_hit_xC[4]);
                fh_xfi31_tofd_exp_select->Fill(det_hit_xHe[7], det_hit_xHe[4]);

                fh_yFi23b_tofd_exp_select->Fill(det_hit_yC[7], det_hit_yC[2]);
                fh_yFi23b_tofd_exp_select->Fill(det_hit_yHe[7], det_hit_yHe[2]);
            }

            if (fWriteOut)
            {

                /*
                                 if (l > 0)
                                {
                                    fNwriteout++;
                                    if (1 == 1)// fNwriteout < 1001)
                                    {
                                        new ((*fFi23aHitItems)[fNofFi23aHitItems++])
                                            R3BFiberMAPMTHitData(0, det_hit_x[1], det_hit_y[1], 2, 0, 0, 0, 0., 0, 0.);

                                        new ((*fFi23bHitItems)[fNofFi23bHitItems++])
                                            R3BFiberMAPMTHitData(0, det_hit_x[2], det_hit_y[2], 2, 0, 0, 0, 0., 0, 0.);

                                        if (det_hit_x[3] > -1000)
                                            new ((*fFi30HitItems)[fNofFi30HitItems++])
                                                R3BFiberMAPMTHitData(0, det_hit_x[3], det_hit_y[3], 2, 0, 0, 0, 0., 0,
                   0.);

                                        if (det_hit_x[4] > -1000)
                                            new ((*fFi31HitItems)[fNofFi31HitItems++])
                                                R3BFiberMAPMTHitData(0, det_hit_x[4], det_hit_y[4], 2, 0, 0, 0, 0., 0,
                   0.);

                                        if (det_hit_x[5] > -1000)
                                            new ((*fFi32HitItems)[fNofFi32HitItems++])
                                                R3BFiberMAPMTHitData(0, det_hit_x[5], det_hit_y[5], 2, 0, 0, 0, 0., 0,
                   0.);

                                        if (det_hit_x[6] > -1000)
                                            new ((*fFi33HitItems)[fNofFi33HitItems++])
                                                R3BFiberMAPMTHitData(0, det_hit_x[6], det_hit_y[6], 2, 0, 0, 0, 0., 0,
                   0.);

                                        new ((*fTofdHitItems)[fNofTofdHitItems++])
                                            R3BTofdHitData(0, det_hit_x[7], det_hit_y[7], 2, 0, 2, 1, 1, 0);
                                    }
                                }
                 */
                // if (l == 2 && p12C.Mag() >= 13030. && p12C.Mag() <= 13050. &&
                //    bestcandidate->GetStartMomentum().Mag() * 1000. >= 4340. &&
                //   bestcandidate->GetStartMomentum().Mag() * 1000. <= 4360. && carbon && alpha )
                if (bestevents)
                //&& p12C.Mag() >= 12980. && p12C.Mag() <= 13070. &&
                // bestcandidate->GetStartMomentum().Mag() * 1000. >= 4300. &&
                // bestcandidate->GetStartMomentum().Mag() * 1000. <= 4420. && carbon && alpha)
                {
                    fNwriteout++;
                    if (1 == 1) // fNwriteout < 1001)
                    {
                        // outfile<<fNEvents<<endl;

                        // cout<<"Number of written event: "<<fNwriteout<<endl;

                        new ((*fFi23aHitItems)[fNofFi23aHitItems++])
                            R3BFiberMAPMTHitData(0, det_hit_xC[1], det_hit_yC[1], 6, 0, 0, 0, 0., 0, 0.);

                        new ((*fFi23bHitItems)[fNofFi23bHitItems++])
                            R3BFiberMAPMTHitData(0, det_hit_xC[2], det_hit_yC[2], 6, 0, 0, 0, 0., 0, 0.);

                        if (det_hit_xC[3] > -1000)
                            new ((*fFi30HitItems)[fNofFi30HitItems++])
                                R3BFiberMAPMTHitData(0, det_hit_xC[3], det_hit_yC[3], 6, 0, 0, 0, 0., 0, 0.);

                        if (det_hit_xC[4] > -1000)
                            new ((*fFi31HitItems)[fNofFi31HitItems++])
                                R3BFiberMAPMTHitData(0, det_hit_xC[4], det_hit_yC[4], 6, 0, 0, 0, 0., 0, 0.);

                        if (det_hit_xC[5] > -1000)
                            new ((*fFi32HitItems)[fNofFi32HitItems++])
                                R3BFiberMAPMTHitData(0, det_hit_xC[5], det_hit_yC[5], 6, 0, 0, 0, 0., 0, 0.);

                        if (det_hit_xC[6] > -1000)
                            new ((*fFi33HitItems)[fNofFi33HitItems++])
                                R3BFiberMAPMTHitData(0, det_hit_xC[6], det_hit_yC[6], 6, 0, 0, 0, 0., 0, 0.);

                        new ((*fTofdHitItems)[fNofTofdHitItems++])
                            R3BTofdHitData(0, det_hit_xC[7], det_hit_yC[7], 6, 0, 6, 1, 1, 0);

                        new ((*fFi23aHitItems)[fNofFi23aHitItems++])
                            R3BFiberMAPMTHitData(0, det_hit_x[1], det_hit_y[1], 2, 0, 0, 0, 0., 0, 0.);

                        new ((*fFi23bHitItems)[fNofFi23bHitItems++])
                            R3BFiberMAPMTHitData(0, det_hit_x[2], det_hit_y[2], 2, 0, 0, 0, 0., 0, 0.);

                        if (det_hit_x[3] > -1000)
                            new ((*fFi30HitItems)[fNofFi30HitItems++])
                                R3BFiberMAPMTHitData(0, det_hit_x[3], det_hit_y[3], 2, 0, 0, 0, 0., 0, 0.);

                        if (det_hit_x[4] > -1000)
                            new ((*fFi31HitItems)[fNofFi31HitItems++])
                                R3BFiberMAPMTHitData(0, det_hit_x[4], det_hit_y[4], 2, 0, 0, 0, 0., 0, 0.);

                        if (det_hit_x[5] > -1000)
                            new ((*fFi32HitItems)[fNofFi32HitItems++])
                                R3BFiberMAPMTHitData(0, det_hit_x[5], det_hit_y[5], 2, 0, 0, 0, 0., 0, 0.);

                        if (det_hit_x[6] > -1000)
                            new ((*fFi33HitItems)[fNofFi33HitItems++])
                                R3BFiberMAPMTHitData(0, det_hit_x[6], det_hit_y[6], 2, 0, 0, 0, 0., 0, 0.);

                        new ((*fTofdHitItems)[fNofTofdHitItems++])
                            R3BTofdHitData(0, det_hit_x[7], det_hit_y[7], 2, 0, 2, 1, 1, 0);
                    }
                }

                if (l == 0 && bestcandidate->GetStartMomentum().Mag() * 1000. > fPmin &&
                    bestcandidate->GetStartMomentum().Mag() * 1000. < fPmax && (int)(AoverZ + 0.5) == 2)
                {
                    counter_wo++;
                    if (counter_wo == 100)
                    {
                        sumwrite++;
                        counter_wo = 0;

                        if (sumwrite < 1001)
                        {
                            cout << "Written out: " << fNEvents_nonull << ", " << sumwrite << ", " << endl;

                            new ((*fFi23aHitItems)[fNofFi23aHitItems++])
                                R3BFiberMAPMTHitData(0, det_hit_x[1], det_hit_y[1], 8, 0, 0, 0, 0., 0, 0.);

                            new ((*fFi23bHitItems)[fNofFi23bHitItems++])
                                R3BFiberMAPMTHitData(0, det_hit_x[2], det_hit_y[2], 8, 0, 0, 0, 0., 0, 0.);

                            if (det_hit_x[3] > -1000)
                                new ((*fFi30HitItems)[fNofFi30HitItems++])
                                    R3BFiberMAPMTHitData(0, det_hit_x[3], det_hit_y[3], 8, 0, 0, 0, 0., 0, 0.);

                            if (det_hit_x[4] > -1000)
                                new ((*fFi31HitItems)[fNofFi31HitItems++])
                                    R3BFiberMAPMTHitData(0, det_hit_x[4], det_hit_y[4], 8, 0, 0, 0, 0., 0, 0.);

                            if (det_hit_x[5] > -1000)
                                new ((*fFi32HitItems)[fNofFi32HitItems++])
                                    R3BFiberMAPMTHitData(0, det_hit_x[5], det_hit_y[5], 8, 0, 0, 0, 0., 0, 0.);

                            if (det_hit_x[6] > -1000)
                                new ((*fFi33HitItems)[fNofFi33HitItems++])
                                    R3BFiberMAPMTHitData(0, det_hit_x[6], det_hit_y[6], 8, 0, 0, 0, 0., 0, 0.);

                            new ((*fTofdHitItems)[fNofTofdHitItems++])
                                R3BTofdHitData(0, det_hit_x[7], det_hit_y[7], 8, 0, 8, 1, 1, 0);
                        }
                    }
                }
            }
        }
        // delete all stored fragments
        fArrayFragments->Clear();
        if (fFragments.size() > 0)
        {
            for (auto const& x : fFragments)
            {
                delete x;
            }
            fFragments.clear();
        }
        // cout << "Delete  " << fFragments.size() << endl;

        //} // end for TofD
    } // end for two particle (12C and 4He)

    if ((alpha && carbon && fBfield == -1710.0))
    {
        counter1++;

        LOG(info) << "Found Tracks: " << counter1 << " with chi2 He/C= " << minChi2 << " / " << minChi2_12C
                  << ", and Erel/MeV: " << Erel << ", from selected NEvents: " << fNEvents_nonull
                  << ", num total events: " << fNEvents << ", max Events: " << maxevent << endl;
        if (sqrt(minChi2 * minChi2 + minChi2_12C * minChi2_12C) < 5)
        {
            fh_Erel->Fill(Erel);
            fh_psum->Fill(psum);
            fh_theta->Fill(theta_26);
        }
    }

    if ((oxygen && fBfield != -1710.0))
    {
        counter1++;

        LOG(info) << "Found Tracks: " << counter1 << " with chi2 O = " << minChi2
                  << ", from selected NEvents: " << fNEvents_nonull << ", num total events: " << fNEvents
                  << ", max Events: " << maxevent << endl;
    }
    // delete particle;
    //  outfile.close();
}

void R3BFragmentTrackerS494::FinishEvent()
{
    fTrackItems->Clear();
    fNofTrackItems = 0;
    for (Int_t det = 0; det < DET_MAX; det++)
    {
        if (fArrayHits.at(det))
        {
            fArrayHits.at(det)->Clear();
        }
    }

    fNofTofdHitItems = 0;
    fTofdHitItems->Clear();
    fNofFi23aHitItems = 0;
    fFi23aHitItems->Clear();
    fNofFi23bHitItems = 0;
    fFi23bHitItems->Clear();
    fNofFi30HitItems = 0;
    fFi30HitItems->Clear();
    fNofFi31HitItems = 0;
    fFi31HitItems->Clear();
    fNofFi32HitItems = 0;
    fFi32HitItems->Clear();
    fNofFi33HitItems = 0;
    fFi33HitItems->Clear();
}

void R3BFragmentTrackerS494::Finish()
{
    cout << "Total chi2 for mass: " << totalChi2Mass / totalEvents << endl;
    //    cout << "Total chi2 for momentum: " << totalChi2P / totalEvents << endl;
    if (fBfield == -1710.0)
    {
        cout << "found pairs: " << counter1 << endl;
        cout << "found 4He:   " << counterHe << endl;
        cout << "found 12C:   " << counterC << endl;
    }
    if (fBfield != -1710.0)
    {
        cout << "found 16O: " << counter1 << endl;
    }
    cout << "from selected NEvents: " << fNEvents_nonull << "num total events: " << fNEvents << endl;

    fh_mult_fi23a->Write();
    fh_mult_fi23b->Write();
    fh_mult_fi30->Write();
    fh_mult_tofd->Write();
    fh_eloss_fi23a_mc->Write();
    fh_eloss_fi23a->Write();
    fh_eloss_fi23b_mc->Write();
    fh_eloss_fi23b->Write();
    fh_eloss_fi30_mc->Write();
    fh_eloss_fi30->Write();
    fh_eloss_fi32_mc->Write();
    fh_eloss_fi32->Write();
    fh_eloss_fi31_mc->Write();
    fh_eloss_fi31->Write();
    fh_eloss_fi33_mc->Write();
    fh_eloss_fi33->Write();
    fh_ncand->Write();
    fh_A_reco1->Write();
    fh_A_reco2->Write();
    fh_mom_res->Write();
    fh_mom_res_x->Write();
    fh_mom_res_y->Write();
    fh_mom_res_z->Write();
    fh_mass_res->Write();
    fh_chi2->Write();
    for (Int_t i = 0; i < 8; i++)
    {
        fh_x_res[i]->Write();
        fh_x_pull[i]->Write();
        fh_y_res[i]->Write();
        fh_y_pull[i]->Write();
    }
    fh_vz_res->Write();
    fh_beta_res->Write();
    fh_A_overZ->Write();
    fh_p->Write();
    fh_px->Write();
    fh_py->Write();
    fh_pz->Write();
    fh_px_l->Write();
    fh_py_l->Write();
    fh_pz_l->Write();
    fh_px_r->Write();
    fh_py_r->Write();
    fh_pz_r->Write();
    fh_p_vs_ch2->Write();
    fh_mass_vs_ch2->Write();
    fh_xfi30_fi23a_track->Write();
    fh_xfi30_fi32_track->Write();
    fh_xfi30_tofd_track->Write();
    fh_tofd_track_exp->Write();
    fh_xfi31_fi23a_track->Write();
    fh_xfi31_fi33_track->Write();
    fh_xfi31_tofd_track->Write();

    fh_xfi30_fi23a_exp->Write();
    fh_xfi30_fi32_exp->Write();
    fh_xfi30_tofd_exp->Write();
    fh_xfi31_fi23a_exp->Write();
    fh_xfi31_fi33_exp->Write();
    fh_xfi31_tofd_exp->Write();
    fh_yFi23b_tofd_exp->Write();

    fh_xfi30_fi23a_exp_select->Write();
    fh_xfi30_fi32_exp_select->Write();
    fh_xfi30_tofd_exp_select->Write();
    fh_xfi31_fi23a_exp_select->Write();
    fh_xfi31_fi33_exp_select->Write();
    fh_xfi31_tofd_exp_select->Write();
    fh_yFi23b_tofd_exp_select->Write();

    fh_yC_vs_yHe_Tofd->Write();
    fh_yC_vs_yHe_Tofd_exp->Write();
    fh_yC_vs_yHe_fib23->Write();
    fh_yC_vs_yHe_fib23_exp->Write();
    fh_xC_vs_xHe_fib23->Write();
    fh_xC_vs_xHe_fib23_exp->Write();
    fh_yFi23b_tofd_track->Write();
    fh_yFi23b_tofd_exp->Write();
    fh_fi23b_track_exp->Write();
    fh_pyC_vs_pyHe->Write();
    fh_theta_16O->Write();
    fh_phi_16O->Write();
    fh_xfi23a_target_track->Write();
    fh_yfi23b_target_track->Write();
    fh_xy_target_C->Write();
    fh_xy_target_He->Write();
    fh_dxdy->Write();
    fh_mass_corel->Write();
    fh_Erel->Write();
<<<<<<< HEAD
    fh_psum->Write();
    fh_theta->Write();
    px_vs_x->Write();
    py_vs_x->Write();
    pz_vs_x->Write();
    p_vs_x->Write();
    px_vs_y->Write();
    py_vs_y->Write();
    pz_vs_y->Write();
    p_vs_y->Write();
    fh_px_p->Write();
    fh_py_p->Write();
    fh_Erel_vs_x->Write();
    fh_Erel_vs_y->Write();
    fh_theta_vs_x->Write();
    fh_theta_vs_y->Write();
    fh_Erel_vs_nhits23a->Write();
    fh_Erel_vs_nhits23b->Write();

    if (fMappedItems.at(DET_CALIFA))
    {
        fh_califa_energy->Write();
        fh_califa_energy_select->Write();
    }
=======
>>>>>>> working on the optimization of geometry

    if (fVis)
    {
        TCanvas* cdif = new TCanvas("cSum", "Summed", 10, 10, 1100, 550);
        cdif->Divide(4, 3);
        cdif->cd(1);
        fh_chi2->Draw("hist");
        cdif->cd(2);
        fh_p->Draw("hist");
        cdif->cd(3);
        fh_px->Draw("hist");
        cdif->cd(4);
        fh_py->Draw("hist");
        cdif->cd(5);
        fh_pz->Draw("hist");
        cdif->cd(6);
        fh_px_l->Draw("hist");
        cdif->cd(7);
        fh_py_l->Draw("hist");
        cdif->cd(8);
        fh_pz_l->Draw("hist");
        cdif->cd(9);
        fh_px_r->Draw("hist");
        cdif->cd(10);
        fh_py_r->Draw("hist");
        cdif->cd(11);
        fh_pz_r->Draw("hist");
        cdif->cd(12);
        gPad->SetLogz();
        fh_p_vs_ch2->Draw("colz");

        for (auto const& det : fDetectors->GetArray())
        {
            det->Draw();
        }

        TCanvas* c11 = new TCanvas("c11", "", 10, 10, 1000, 1000);
        c11->Divide(2, 2);
        c11->cd(1);
        fh_mult_fi23b->Draw();
        c11->cd(2);
        fh_mult_fi30->Draw();
        c11->cd(3);
        fh_mult_fi23a->Draw();
        c11->cd(4);
        fh_mult_tofd->Draw();
        c11->cd(0);

        new TCanvas("c2", "", 10, 10, 500, 500);
        fh_eloss_fi23a_mc->SetLineStyle(2);
        fh_eloss_fi23a_mc->SetLineColor(2);
        fh_eloss_fi23a_mc->Draw();
        fh_eloss_fi23a->Draw("same");

        //        TCanvas* c3 = new TCanvas("c3", "", 200, 10, 1000, 500);
        //        c3->Divide(2, 1);
        //        c3->cd(1);
        //        c3->cd(2);
        //        c3->cd(0);

        TCanvas* c5 = new TCanvas("c5", "", 400, 10, 1000, 500);
        c5->Divide(2, 1);
        c5->cd(1);
        fh_mom_res->Draw();
        c5->cd(2);
        fh_mass_res->Draw();
        c5->cd(0);

        new TCanvas("c6", "", 500, 300, 500, 500);
        fh_A_reco2->Draw();
        fh_A_reco2->GetXaxis()->SetRangeUser(126., 132.);
        //        fh_A_reco2->SetLineColor(2);
        //        fh_A_reco2->Draw("same");

        Int_t bin1 = fh_A_reco2->FindFirstBinAbove(fh_A_reco2->GetMaximum() / 2.);
        Int_t bin2 = fh_A_reco2->FindLastBinAbove(fh_A_reco2->GetMaximum() / 2.);
        Double_t fwhm = fh_A_reco2->GetBinCenter(bin2) - fh_A_reco2->GetBinCenter(bin1);

        LOG(debug) << fwhm / fh_A_reco2->GetMean();

        new TCanvas("c7", "", 500, 600, 500, 500);
        fh_chi2->Draw();

        TCanvas* c8 = new TCanvas("c8", "", 10, 10, 1200, 1200);
        c8->Divide(4, 4);
        for (Int_t i = 0; i < 8; i++)
        {
            c8->cd(i + 1);
            fh_x_res[i]->Draw();
        }
        for (Int_t i = 0; i < 8; i++)
        {
            c8->cd(i + 9);
            fh_y_res[i]->Draw();
        }
        c8->cd(0);

        TCanvas* c9 = new TCanvas("c9", "", 10, 10, 1200, 1200);
        c9->Divide(4, 4);
        for (Int_t i = 0; i < 8; i++)
        {
            c9->cd(i + 1);
            fh_x_pull[i]->Draw();
        }
        for (Int_t i = 0; i < 8; i++)
        {
            c9->cd(i + 9);
            fh_y_pull[i]->Draw();
        }
        c9->cd(0);
        /*
                new TCanvas("c10", "", 10, 10, 500, 500);
                fh_vz_res->Draw();

                new TCanvas("c12", "", 600, 10, 500, 500);
                fh_ncand->Draw();

                new TCanvas("c12", "", 10, 10, 500, 500);
                fh_beta_res->Draw();
        */
    }
}

Bool_t R3BFragmentTrackerS494::InitPropagator()
{
    FairField* fairField = FairRunAna::Instance()->GetField();
    R3BGladFieldMap* gladField = NULL;
    fFieldPar->printParams();
    if (2 == fFieldPar->GetType())
    {
        gladField = (R3BGladFieldMap*)fairField;

        if (fPropagator)
        {
            delete fPropagator;
        }
        fPropagator = new R3BTPropagator(gladField, fVis);
    }
    else
    {
        LOG(error) << "Unsupported type of field.";
        return kFALSE;
    }
    return kTRUE;
}

ClassImp(R3BFragmentTrackerS494)
