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

#include "R3BTofdCalData.h"
#include "R3BTofdHitData.h"
#include "R3BTofdMappedData.h"

#include "R3BFiberMAPMTCalData.h"
#include "R3BFiberMAPMTHitData.h"
#include "R3BFiberMAPMTMappedData.h"

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
    , fSimu(kTRUE)
    , fForward(kTRUE)
    , fBfield(-1710.)
    , fOptimizeGeometry(kFALSE)
    , fTrackItems(new TClonesArray("R3BTrack"))
    , fNofTrackItems()
{
    // this is the list of detectors (active areas) we use for tracking
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
    FairRootManager* man = FairRootManager::Instance();

    // Get objects for detectors on all levels
    fArrayMCTracks = (TClonesArray*)man->GetObject("MCTrack");
    if (fArrayMCTracks)
        man->Register("MCTrack", "Monte Carlo Tracks", fArrayMCTracks, kTRUE);

    if (NULL == fArrayMCTracks)
    {
        LOG(INFO) << "No MC Track array found in input file.";
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
    man->Register("TrackingParticle", "Tracking", fArrayFragments, kTRUE);
    man->Register("Track", "Land", fTrackItems, kTRUE);

    if (!InitPropagator())
    {
        return kERROR;
    }

    fDetectorsLeft->Init();
    fDetectorsRight->Init();
    fDetectors->Init();
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
    fh_A_reco2 = new TH1F("h_A_reco2", "Reconstructed mass, step 2", 2000., 0., 20.);
    fh_mom_res = new TH1F("h_mom_res", "Momentum resolution", 10000, -100., 100.);
    fh_mom_res_x = new TH1F("h_mom_res_x", "Momentum resolution px in %", 1000, -10., 10.);
    fh_mom_res_y = new TH1F("h_mom_res_y", "Momentum resolution py in %", 1000, -10., 10.);
    fh_mom_res_z = new TH1F("h_mom_res_z", "Momentum resolution pz in %", 1000, -10., 10.);
    fh_mass_res = new TH1F("h_mass_res", "Mass resolution", 10, -5.5, 4.5);
    fh_chi2 = new TH1F("h_chi2", "Chi2", 10000, 0., 1000.);
    fh_vz_res = new TH1F("h_vz_res", "vz - vz_mc", 200, -1., 1.);
    fh_beta_res = new TH1F("h_beta_res", "beta - beta_mc", 200, -0.1, 0.1);
    fh_A_overZ = new TH2F("fh_A_overZ", "particle identification", 100, 0., 10., 100, 1., 3.);
    fh_p = new TH1F("h_p", "momentum p", 2000, 0., 20.);
    fh_px = new TH1F("h_px", "momentum px", 1000, -500., 500.);
    fh_py = new TH1F("h_py", "momentum py", 1000, -500., 500.);
    fh_pz = new TH1F("h_pz", "momentum pz", 2000, 0., 20.);
    fh_px_l = new TH1F("h_px_l", "momentum px left", 1000, -500., 500.);
    fh_py_l = new TH1F("h_py_l", "momentum py left", 1000, -500., 500.);
    fh_pz_l = new TH1F("h_pz_l", "momentum pz left", 2000, 0., 20.);
    fh_px_r = new TH1F("h_px_r", "momentum px right", 1000, -500., 500.);
    fh_py_r = new TH1F("h_py_r", "momentum py right", 1000, -500., 500.);
    fh_pz_r = new TH1F("h_pz_r", "momentum pz right", 2000, 0., 20.);

    fh_p_vs_ch2 = new TH2F("h_p_vs_chi2", "p.Mag vs chi2", 2000, 0., 200., 2000, 0., 20.);

    Double_t ranges[] = { 10., 10., 10., 10., 10., 10., 10., 50 };
    Int_t bins[] = { 2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000 };
    for (Int_t i = 0; i < 8; i++)
    {
        fh_x_res[i] = new TH1F(Form("h_x_res%d", i), Form("x residual %d", i), bins[i], -ranges[i], ranges[i]);
        fh_x_pull[i] = new TH1F(Form("h_x_pull%d", i), Form("x pull %d", i), 40, -10., 10.);
        fh_y_res[i] = new TH1F(Form("h_y_res%d", i), Form("y residual %d", i), bins[i], -ranges[i], ranges[i]);
        fh_y_pull[i] = new TH1F(Form("h_y_pull%d", i), Form("ypull %d", i), 40, -10., 10.);
    }

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

    fDetectorsLeft->SetParContainers();
    fDetectorsRight->SetParContainers();
    fDetectors->SetParContainers();
}

void R3BFragmentTrackerS494::Exec(const Option_t*)
{

    if (fNEvents / 100. == (int)fNEvents / 100)
        std::cout << "\rEvents: " << fNEvents << " / " << maxevent << " (" << (int)(fNEvents * 100. / maxevent)
                  << " %) " << std::flush;

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

    Bool_t debug = true;

    /* this part needs to be adopted to each experiment / setup
     *
     * First thing is to tell each detector which hit we want to use
     * for the track fitting. This means: manually copy the hit we want to
     * use for tracking from the TClonesArray into the single hit structure.
     *
     * Then call fit_fragment() to optimize the track and calculate
     * particle properties.
     */
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

    // target->hits.push_back(new R3BHit(0, 0., 0., 0., 0., 0));


    if (fBfield == -1710.0)
    {
        if (tof->hits.size() < 2)
            return;
        if (fi23a->hits.size() < 2)
            return;
        if (fi23b->hits.size() < 2)
            return;
        if (fi30->hits.size() < 1)
            return;
        if (fi31->hits.size() < 1)
            return;
        if (fi32->hits.size() < 1)
            return;
        if (fi33->hits.size() < 1)
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

    if (debug)
        cout << "*************** NEW EVENT *******************" << endl;
    if (tof->hits.size() > 0 && debug)
    {
        cout << "Hits ToFD: " << tof->hits.size() << endl;
        cout << "Hits right: " << fi23a->hits.size() << "  " << fi23b->hits.size() << "  " << fi31->hits.size() << "  "
             << fi33->hits.size() << endl;

        cout << "Hits left: " << fi23a->hits.size() << "  " << fi23b->hits.size() << "  " << fi32->hits.size() << "  "
             << fi30->hits.size() << endl;
    }

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

    if (fArrayMCTracks)
    {
        Int_t nHitsMCTrack = fArrayMCTracks->GetEntriesFast();
        for (Int_t l = 0; l < nHitsMCTrack; l++)
        {
            R3BMCTrack* ion = (R3BMCTrack*)fArrayMCTracks->At(l);

            Int_t PID = ion->GetPdgCode();
            Int_t mother = ion->GetMotherId();
            LOG(DEBUG) << "PID " << PID << endl;
            if (mother < 0)
            {
                if (PID == 1000020040)
                {
                    // 4He
                    betaHe = 1. / TMath::Sqrt(1 + TMath::Power(ion->GetMass() / ion->GetP(), 2));
                    x0He = ion->GetStartX();
                    y0He = ion->GetStartY();
                    z0He = ion->GetStartZ();
                    px0He = ion->GetPx();
                    py0He = ion->GetPy();
                    pz0He = ion->GetPz();
                    pHe = ion->GetP();
                    massHe = ion->GetMass();
                    LOG(DEBUG) << "MC ************ 4He **************";
                    LOG(DEBUG) << "MC position x: " << x0He << " y: " << y0He << " z: " << z0He;
                    LOG(DEBUG) << "MC momentum p: " << pHe << " px " << px0He << " py " << py0He << " pz " << pz0He;
                    LOG(DEBUG) << "MC mass 4He: " << massHe << " beta: " << betaHe;
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
                    LOG(DEBUG) << "MC ************ 12C **************";
                    LOG(DEBUG) << "MC position x: " << x0C << " y: " << y0C << " z: " << z0C;
                    LOG(DEBUG) << "MC momentum p: " << pC << " px " << px0C << " py " << py0C << " pz " << pz0C;
                    LOG(DEBUG) << "MC mass 12C: " << massC << " beta: " << betaC;
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
                    LOG(DEBUG) << "MC ************ 16O **************";
                    LOG(DEBUG) << "MC position x: " << x0O << " y: " << y0O << " z: " << z0O;
                    LOG(DEBUG) << "MC momentum p: " << pO << " px " << px0O << " py " << py0O << " pz " << pz0O;
                    LOG(DEBUG) << "MC mass: " << massO << " beta: " << betaO;
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

    fPropagator->SetVis(kTRUE);

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
        }

        // Loop over all combinations of hits
        for (Int_t i = 0; i < tof->hits.size(); i++) // loop over all ToFD hits
        {
            if (fSimu)
            {
                // For tracking of simulations:
                charge = sqrt(tof->hits.at(i)->GetEloss()) * 26.76 + 0.5;
                Charge = sqrt(tof->hits.at(i)->GetEloss()) * 26.76;
            }
            else
            {
                // For tracking of exp. data:
                charge = tof->hits.at(i)->GetEloss();
                Charge = tof->hits.at(i)->GetEloss();
            }
            if (debug)
                cout << "Charge: " << charge << " requested charge: " << charge_requested << endl;


            if (charge != charge_requested)
                continue;
            if (debug)
                cout << "Charge: " << charge << " requested charge: " << charge_requested << endl;

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
            }
            if (charge == 2)
            {
                m0 = 3.728401291;
                p0 = 4.347875;
                if (!fSimu)
                    pHe = p0;
                massHe = m0;
            }

            if (debug)
                cout << "Mass: " << m0 << endl;
            if (debug)
                cout << "Position on TofD: " << tof->hits.at(i)->GetX() << endl;
            if (!tof->free_hit[i]) // if the hit was used already, continue
            {
                if (debug)
                    cout << "ToFD hit already used" << endl;
                continue;
            }

            if (debug)
                cout << "AT START: "
                     << "Mass: " << m0 << ", Momentum: " << p0 << endl;

            if (tof->hits.at(i)->GetX() > 0 && fi30->hits.size() > 0 && fi32->hits.size() > 0 &&
                fi23a->hits.size() > 0 && fi23b->hits.size() > 0)
            {
                // left branch in beam direction, don't consider hits in the detectors of the other side
                R3BTrackingDetector* target = fDetectorsLeft->GetByName("target");
                if (fNEventsLeft == 0)
                    target->hits.push_back(new R3BHit(0, 0., 0., 0., 0., 0));

/*

                Double_t fieldScale = fBfield / 3583.81 * 1.0; // standard
                Double_t scale = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetScale();
                Double_t field = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(0., 0., 240.);
                if (debug)
                    cout << "Field left:" << field << " scale: " << scale << endl;

                fieldScale = fBfield / 3583.81 / scale * 1.;
                if (debug)
                    cout << "Setting field to " << fieldScale << endl;
                ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrection(fieldScale);
                field = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(0., 0., 240.);
                if (debug)
                    cout << "Field left after:" << field << endl;
*/
                do // fi30
                {
                    if (ifi30 >= 0)
                        fh_eloss_fi30_mc->Fill(1000. * fi30->hits.at(ifi30)->GetEloss()); // MeV
                    if (ifi30 >= 0 && !fi30->free_hit[ifi30]) // if the hit was used already, continue
                    {
                        if (debug)
                            cout << "Fi30 hit already used" << endl;
                        ifi30 += 1;
                        continue;
                    }
                    do // fi32
                    {
                        if (ifi32 >= 0)
                            fh_eloss_fi32_mc->Fill(1000. * fi32->hits.at(ifi32)->GetEloss()); // MeV
                        if (ifi32 >= 0 && !fi32->free_hit[ifi32]) // if the hit was used already, continue
                        {
                            if (debug)
                                cout << "Fi32 hit already used" << endl;
                            ifi32 += 1;
                            continue;
                        }
                        do // fi23b
                        {
                            if (ifi23b >= 0)
                                fh_eloss_fi23b_mc->Fill(1000. * fi23b->hits.at(ifi23b)->GetEloss()); // MeV
                            if (ifi23b >= 0 && !fi23b->free_hit[ifi23b]) // if the hit was used already, continue
                            {
                                if (debug)
                                    cout << "Fi23b hit already used" << endl;
                                ifi23b += 1;
                                continue;
                            }

                            do // fi23a
                            {
                                if (ifi23a >= 0)
                                    fh_eloss_fi23a_mc->Fill(1000. * fi23a->hits.at(ifi23a)->GetEloss()); // MeV
                                if (ifi23a >= 0 && !fi23a->free_hit[ifi23a]) // if the hit was used already, continue
                                {
                                    if (debug)
                                        cout << "Fi23a hit already used" << endl;
                                    ifi23a += 1;
                                    continue;
                                }

                                // Create object for particle which will be fitted
                                R3BTrackingParticle* candidate =
                                    new R3BTrackingParticle(charge, x0, y0, z0, 0., 0., p0, beta0, m0);

                                if (debug)
                                {
                                    cout << "left side of setup" << endl;
                                    cout << "Charge requested: " << charge_requested << endl;
                                    cout << "Hit Tofd # " << i << " x: " << tof->hits.at(i)->GetX()
                                         << " y: " << tof->hits.at(i)->GetY() << endl;
                                }
                                if (ifi23a > -1 && debug)
                                    cout << " Fi23a left # " << ifi23a << " x: " << fi23a->hits.at(ifi23a)->GetX()
                                         << endl;
                                if (ifi23b > -1 && debug)
                                    cout << " Fi23b # " << ifi23b << " x: " << fi23b->hits.at(ifi23b)->GetX() << endl;
                                if (ifi30 > -1 && debug)
                                    cout << " fi30 # " << ifi30 << " x: " << fi30->hits.at(ifi30)->GetX() << endl;
                                if (ifi32 > -1 && debug)
                                    cout << " fi32 # " << ifi32 << " x: " << fi32->hits.at(ifi32)->GetX() << endl;
                                if (debug)
                                    cout << "Hit target # "
                                         << " x: " << target->hits.at(0)->GetX() << endl;
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
                                if (debug)
                                    cout << " Chi: " << candidate->GetChi2() << "  "
                                         << candidate->GetStartMomentum().Mag() << "  "
                                         << 1000. * (candidate->GetStartMomentum().Mag() - p0) *
                                                (candidate->GetStartMomentum().Mag() - p0)
                                         << endl;
                                if (debug)
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
                                    if (fForward)
                                    {
                                        candidate->Reset();
                                    }
                                    else
                                    {
                                        //candidate->SetStartPosition(candidate->GetPosition()); // @target
                                        //candidate->SetStartMomentum(-1. * candidate->GetMomentum());
                                        // candidate->SetStartBeta(0.8328);
                                        //candidate->SetStartBeta(beta0);
                                        //candidate->UpdateMomentum();
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
                                    }
                                }
                                else
                                {
                                    delete candidate;
                                }

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

                        ifi32 += 1;
                    } while (ifi32 < fi32->hits.size());
                    ifi32 = 0;
                    if (0 == fi32->hits.size())
                        ifi32 = -1;

                    ifi30 += 1;
                } while (ifi30 < fi30->hits.size());
                ifi30 = 0;
                if (0 == fi30->hits.size())
                    ifi30 = -1;
            } // end if left branch

            if (tof->hits.at(i)->GetX() < 0 && fi31->hits.size() > 0 && fi33->hits.size() > 0 &&
                fi23a->hits.size() > 0 && fi23b->hits.size() > 0)
            {
                // right branch in beam direction, don't consider hits in the detectors of the other side
                R3BTrackingDetector* target = fDetectorsRight->GetByName("target");
                if (fNEventsRight == 0)
                    target->hits.push_back(new R3BHit(0, 0., 0., 0., 0., 0));

/*
                Double_t fieldScale = fBfield / 3583.81 * 1.0; // standard
                Double_t scale = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetScale();
                Double_t field = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(0., 0., 240.);
                if (debug)
                    cout << "Field right:" << field << " scale: " << scale << endl;

                fieldScale = fBfield / 3583.81 / scale * 1.;
                if (debug)
                    cout << "Setting field to " << fieldScale << endl;
                ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrection(fieldScale);
                field = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(0., 0., 240.);
                if (debug)
                    cout << "Field right after:" << field << endl;
*/
                do // fi33
                {
                    if (ifi33 >= 0)
                        fh_eloss_fi33_mc->Fill(1000. * fi33->hits.at(ifi33)->GetEloss()); // MeV
                    if (ifi33 >= 0 && !fi33->free_hit[ifi33]) // if the hit was used already, continue
                    {
                        if (debug)
                            cout << "Fi33 hit already used" << endl;
                        ifi33 += 1;
                        continue;
                    }
                    do // fi31
                    {
                        if (ifi31 >= 0)
                            fh_eloss_fi31_mc->Fill(1000. * fi31->hits.at(ifi31)->GetEloss()); // MeV
                        if (ifi31 >= 0 && !fi31->free_hit[ifi31]) // if the hit was used already, continue
                        {
                            if (debug)
                                cout << "Fi11 hit already used" << endl;
                            ifi31 += 1;
                            continue;
                        }
                        do // fi23b
                        {
                            if (ifi23b >= 0)
                                fh_eloss_fi23b_mc->Fill(1000. * fi23b->hits.at(ifi23b)->GetEloss()); // MeV
                            if (ifi23b >= 0 && !fi23b->free_hit[ifi23b]) // if the hit was used already, continue
                            {
                                if (debug)
                                    cout << "Fi23b hit already used" << endl;
                                ifi23b += 1;
                                continue;
                            }
                            do // fi23a
                            {
                                if (ifi23a >= 0)
                                    fh_eloss_fi23a_mc->Fill(1000. * fi23a->hits.at(ifi23a)->GetEloss()); // MeV
                                if (ifi23a >= 0 && !fi23a->free_hit[ifi23a]) // if the hit was used already, continue
                                {
                                    if (debug)
                                        cout << "Fi3a hit already used" << endl;
                                    ifi23a += 1;
                                    continue;
                                }

                                // Create object for particle which will be fitted
                                R3BTrackingParticle* candidate =
                                    new R3BTrackingParticle(charge, x0, y0, z0, 0., 0., p0, beta0, m0);

                                if (debug)
                                {
                                    cout << "right side of setup" << endl;
                                    cout << "Charge requested: " << charge_requested << endl;
                                    cout << "Start values to fit, x0: " << x0 << " y0: " << y0 << " z0: " << z0
                                         << " p0: " << p0 << " beta0: " << beta0 << " m0: " << m0 << endl;
                                    cout << "Hit Tofd # " << i << " x: " << tof->hits.at(i)->GetX()
                                         << " y: " << tof->hits.at(i)->GetY() << endl;
                                }
                                if (ifi23a > -1 && debug)
                                    cout << " Fi23a # " << ifi23a << " x: " << fi23a->hits.at(ifi23a)->GetX() << endl;
                                if (ifi23b > -1 && debug)
                                    cout << " Fi23b # " << ifi23b << " x: " << fi23b->hits.at(ifi23b)->GetX() << endl;
                                if (ifi33 > -1 && debug)
                                    cout << "Fi33 # " << ifi33 << " x: " << fi33->hits.at(ifi33)->GetX() << endl;
                                if (ifi31 > -1 && debug)
                                    cout << "Fi31  # " << ifi31 << " x: " << fi31->hits.at(ifi31)->GetX() << endl;
                                if (debug)
                                    cout << "Hit target # "
                                         << " x: " << target->hits.at(0)->GetX() << endl;

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
                                if (debug)
                                    cout << "Chi2: " << candidate->GetChi2() << "  pstart.Mag "
                                         << candidate->GetStartMomentum().Mag() << " dp.Mag "
                                         << 1000. * (candidate->GetStartMomentum().Mag() - p0) *
                                                (candidate->GetStartMomentum().Mag() - p0)
                                         << endl;
                                if (debug)
                                    cout << "--------------------------------" << endl;
                                nCand += 1;
                                Icountright += 1;

                                //    cout <<fNEvents<<", RIGHT SIDE: Charge: "<< charge<<", Momentum: " <<
                                //    candidate->GetMomentum().Mag()<<", Momentum Z: "<<candidate->GetMomentum().Z()<<
                                //   ", Momentum X: "<<candidate->GetMomentum().X() << " status: "<<status<<endl;

                                if (TMath::IsNaN(candidate->GetMomentum().Z()))
                                {
                                    delete candidate;
                                    continue;
                                }

                                if (10 > status)
                                {
                                    if (fForward)
                                    {
                                        candidate->Reset();
                                    }
                                    else
                                    {
                                        //candidate->SetStartPosition(candidate->GetPosition());
                                        //candidate->SetStartMomentum(-1. * candidate->GetMomentum());
                                        // candidate->SetStartBeta(0.8328);
                                        //candidate->SetStartBeta(beta0);
                                        //candidate->UpdateMomentum();
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
                                    }
                                }
                                else
                                {
                                    delete candidate;
                                }

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

        fh_ncand->Fill(nCand);

        R3BTrackingParticle* candidate;
        Double_t minChi2 = 1e10;
        Double_t pChi2 = 0.;
        Double_t parChi2 = 0.;
        Double_t xChi2 = 0.;

        if (fFragments.size() > 0)
        {
            for (auto const& x : fFragments)
            {
                pChi2 = 10000. * (x->GetStartMomentum().Mag() - p0) * (x->GetStartMomentum().Mag() - p0);
                xChi2 = x->GetChi2();
                // parChi2 = sqrt(pChi2 * pChi2 + xChi2 * xChi2);
                parChi2 = xChi2;

                // if (x->GetChi2() < minChi2)
                if (parChi2 < minChi2)
                {
                    candidate = x;
                    // minChi2 = x->GetChi2();
                    minChi2 = parChi2;

                    // cout << "New min chi2: " << minChi2 << endl;
                    // cout << "Corresponding Mass   : " << x->GetMass() << endl;
                    // cout << "Corresponding Mass   : " << candidate->GetMass() << endl;
                }
            }

            if (minChi2 > 1.e5)
                continue;

            if (candidate->GetStartMomentum().X() < 0)
            {
                fi23a->free_hit[candidate->GetHitIndexByName("fi23a")] = false;
                fi23b->free_hit[candidate->GetHitIndexByName("fi23b")] = false;
                fi31->free_hit[candidate->GetHitIndexByName("fi31")] = false;
                fi33->free_hit[candidate->GetHitIndexByName("fi33")] = false;
            }
            else
            {
                fi23a->free_hit[candidate->GetHitIndexByName("fi23a")] = false;
                fi23b->free_hit[candidate->GetHitIndexByName("fi23b")] = false;
                fi30->free_hit[candidate->GetHitIndexByName("fi30")] = false;
                fi32->free_hit[candidate->GetHitIndexByName("fi32")] = false;
            }
            tof->free_hit[candidate->GetHitIndexByName("tofd")] = false;

            Double_t x0soll, y0soll, z0soll, psoll, px0soll, py0soll, pz0soll, beta0soll, m0soll;

            if (l == 0)
            {
                LOG(DEBUG) << "16O";
                oxygen = kTRUE;
                if(fSimu){
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
				else{
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
                LOG(DEBUG) << "12C";
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
            }

            if (l == 2)
            {
                LOG(DEBUG) << "4He";
                alpha = kTRUE;
                x0soll = x0He;
                y0soll = y0He;
                z0soll = z0He;
                px0soll = px0He;
                py0soll = py0He;
                pz0soll = pz0He;
                psoll = pHe;
                m0soll = massHe;
                beta0soll = betaHe;
            }

            LOG(DEBUG) << "Results after tracking mass:";
            LOG(DEBUG) << "Position (soll) x: " << x0soll << " y: " << y0soll << " z: " << z0soll;
            LOG(DEBUG) << "Position (ist)  x: " << candidate->GetStartPosition().X()
                       << " y: " << candidate->GetStartPosition().Y() << " z: " << candidate->GetStartPosition().Z();

            LOG(DEBUG) << "Momentum (soll): " << psoll << " px : " << px0soll << " py: " << py0soll
                       << " pz: " << pz0soll;
            LOG(DEBUG) << "Momentum (ist) : " << candidate->GetStartMomentum().Mag()
                       << " px : " << candidate->GetStartMomentum().X() << " py: " << candidate->GetStartMomentum().Y()
                       << " pz: " << candidate->GetStartMomentum().Z();

            LOG(DEBUG) << "Mass   : " << candidate->GetMass();
            LOG(DEBUG) << "Beta   : " << candidate->GetStartBeta();
            // LOG(DEBUG) << "chi2: " << candidate->GetChi2() << endl;
            LOG(DEBUG) << "chi: " << minChi2 << endl;

            // cout<<"Momentum (ist) : " << candidate->GetStartMomentum().Mag()
            //          << " px : " << candidate->GetStartMomentum().X() << " py: " << candidate->GetStartMomentum().Y()
            //        << " pz: " << candidate->GetStartMomentum().Z()<<", Mass   : " << candidate->GetMass()<<", chi2: "
            //        << minChi2 << endl;

            fh_A_reco1->Fill(candidate->GetMass() / amu);
            if (minChi2 < 1.e4)
                totalChi2Mass += minChi2;

            /*
                        // Now we know the mass. We can set here the exact mass if wanted and then track the momentum
                        Double_t m =  candidate->GetMass();
                        if(TMath::Abs(candidate->GetMass() - 14.8951) < 1.5)
                            m = 14.8951; // 16O
                        if(TMath::Abs(candidate->GetMass() - 11.1749) < 1.5)
                            m = 11.1749; // 4He
                        if(TMath::Abs(candidate->GetMass() - 3.7284) < 1.5)
                            m = 3.7284; // 4He

                        //candidate->SetMass(m);
                        candidate->UpdateMomentum();

                        fDetectors = fDetectorsLeft;

                        if(candidate->GetStartMomentum().X() < 0 )
                        {
                            fDetectors = fDetectorsRight;
                        }
                        Int_t status = fFitter->FitTrackMomentum(candidate, fDetectors);
                        cout << "status: " << status << endl;
                        if(status > 9) return;


                        LOG(INFO) << "Final result:";
                        LOG(INFO) << "Position x: " << candidate->GetStartPosition().X() <<
                            " y: " << candidate->GetStartPosition().Y() << " z: " << candidate->GetStartPosition().Z();
                        LOG(INFO) << "Momentum : " << candidate->GetStartMomentum().Mag() << " px : " <<
               candidate->GetStartMomentum().X() << " py: " << candidate->GetStartMomentum().Y() << " pz: " <<
               candidate->GetStartMomentum().Z(); LOG(INFO) << "Mass   : " << candidate->GetMass(); LOG(INFO) << "Beta
               : " << candidate->GetStartBeta(); LOG(INFO) << "chi2: " << candidate->GetChi2() << endl;

                        fh_A_overZ->Fill(Charge, candidate->GetMass() / Charge / amu);
                        fh_mom_res->Fill((candidate->GetStartMomentum().Mag() - particle->GetStartMomentum().Mag()) /
               particle->GetStartMomentum().Mag()); fh_chi2->Fill(candidate->GetChi2());
                        fh_mass_res->Fill(candidate->GetMass() - particle->GetMass());
                        fh_vz_res->Fill(candidate->GetStartPosition().X() - particle->GetStartPosition().X());
                        fh_beta_res->Fill((candidate->GetStartBeta() - particle->GetStartBeta()) /
               particle->GetStartBeta());
                        // candidate->GetStartPosition().Print();
                        // LOG(INFO) << (candidate->GetMass() / amu);
            */
            totalChi2P += minChi2;

            fh_p_vs_ch2->Fill(minChi2, candidate->GetStartMomentum().Mag());

            if (minChi2 < 1000)
            {
                fh_p->Fill(candidate->GetStartMomentum().Mag());
                fh_px->Fill(candidate->GetStartMomentum().X() * 100.);
                fh_py->Fill(candidate->GetStartMomentum().Y() * 100.);
                fh_pz->Fill(candidate->GetStartMomentum().Z());
                if (candidate->GetStartMomentum().X() < 0)
                {
                    fh_px_r->Fill(candidate->GetStartMomentum().X() * 100.);
                    fh_py_r->Fill(candidate->GetStartMomentum().Y() * 100.);
                    fh_pz_r->Fill(candidate->GetStartMomentum().Z());
                }
                else
                {
                    fh_px_l->Fill(candidate->GetStartMomentum().X() * 100.);
                    fh_py_l->Fill(candidate->GetStartMomentum().Y() * 100.);
                    fh_pz_l->Fill(candidate->GetStartMomentum().Z());
                }
            }
            fh_A_reco2->Fill(candidate->GetMass() / amu);
            fh_A_overZ->Fill(Charge, candidate->GetMass() / Charge / amu);
            fh_mom_res->Fill((candidate->GetStartMomentum().Mag() - psoll) / psoll * 100.);
            fh_mom_res_x->Fill((candidate->GetStartMomentum().X() - px0soll) / px0soll * 100.);
            fh_mom_res_y->Fill((candidate->GetStartMomentum().Y() - py0soll) / py0soll * 100.);
            fh_mom_res_z->Fill((candidate->GetStartMomentum().Z() - pz0soll) / pz0soll * 100.);

            fh_chi2->Fill(minChi2);
            fh_mass_res->Fill(candidate->GetMass() - m0soll);
            fh_vz_res->Fill(candidate->GetStartPosition().X() - x0soll);
            fh_beta_res->Fill((candidate->GetStartBeta() - beta0soll) / beta0soll);
            // candidate->GetStartPosition().Print();
            // LOG(INFO) << (candidate->GetMass() / amu);

            // store hits in track level
            new ((*fTrackItems)[fNofTrackItems++]) R3BTrack(candidate->GetStartPosition().X() / 100.,
                                                            candidate->GetStartPosition().Y() / 100.,
                                                            candidate->GetStartPosition().Z() / 100.,
                                                            candidate->GetStartMomentum().X() * 1000.,
                                                            candidate->GetStartMomentum().Y() * 1000.,
                                                            candidate->GetStartMomentum().Z() * 1000.,
                                                            charge_requested,
                                                            2,
                                                            minChi2,
                                                            minChi2,
                                                            0);

            Double_t x_l = 0.;
            Double_t y_l = 0.;
            Int_t iDet = 0;
            fPropagator->SetVis();

            candidate->Reset();

            for (auto const& det : fDetectors->GetArray())
            {
                // cout << "At detector: " << det->GetDetectorName() << endl;
                if (kTarget != det->section)
                {
                    LOG(DEBUG1) << "Propagate to " << det->GetDetectorName();
                    fPropagator->PropagateToDetector(candidate, det);
                }

                if (det->GetDetectorName().EqualTo("fi23a"))
                { // fi23a
                    Double_t eloss = det->GetEnergyLoss(candidate);
                    fh_eloss_fi23a->Fill(eloss);
                    iDet = 1;
                }

                if (det->GetDetectorName().EqualTo("fi23b"))
                { // fi23b
                    Double_t eloss = det->GetEnergyLoss(candidate);
                    fh_eloss_fi23b->Fill(eloss);
                    iDet = 2;
                }

                if (det->GetDetectorName().EqualTo("fi30"))
                { // fi30
                    Double_t eloss = det->GetEnergyLoss(candidate);
                    fh_eloss_fi30->Fill(eloss);
                    iDet = 3;
                }

                if (det->GetDetectorName().EqualTo("fi31"))
                { // fi31
                    Double_t eloss = det->GetEnergyLoss(candidate);
                    fh_eloss_fi31->Fill(eloss);
                    iDet = 4;
                }

                if (det->GetDetectorName().EqualTo("fi32"))
                { // fi32
                    Double_t eloss = det->GetEnergyLoss(candidate);
                    fh_eloss_fi32->Fill(eloss);
                    iDet = 5;
                }

                if (det->GetDetectorName().EqualTo("fi33"))
                { // fi33
                    Double_t eloss = det->GetEnergyLoss(candidate);
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
                        candidate->PassThroughDetector(det, weight);
                    }
                }

                // Convert global track coordinates into local on the det plane
                det->GlobalToLocal(candidate->GetPosition(), x_l, y_l);
                R3BHit* hit = nullptr;
                Int_t hitIndex = candidate->GetHitIndexByName(det->GetDetectorName().Data());
                if (hitIndex >= 0)
                    hit = fDetectors->GetHit(det->GetDetectorName().Data(), hitIndex);
                if (hit && det->res_x > 1e-6)
                {
                    if (debug)
                        cout << "current x position: " << candidate->GetPosition().X() << "  "
                             << candidate->GetPosition().Y() << "  " << candidate->GetPosition().Z() << endl;
                    Double_t det_hit_x = hit->GetX();
                    if (debug)
                        cout << "Residuals x " << iDet << "; " << det->GetDetectorName().Data() << "  " << x_l << "  "
                             << det_hit_x << endl;

                    fh_x_res[iDet]->Fill(x_l - det_hit_x);
                    fh_x_pull[iDet]->Fill((x_l - det_hit_x) / det->res_x);
                }
                if (hit && det->res_y > 1e-6)
                {
                    if (debug)
                        cout << "current y position: " << candidate->GetPosition().X() << "  "
                             << candidate->GetPosition().Y() << "  " << candidate->GetPosition().Z() << endl;
                    Double_t det_hit_y = hit->GetY();
                    if (debug)
                        cout << "Residuals y " << iDet << ", " << det->GetDetectorName().Data() << "  " << y_l << "  "
                             << det_hit_y << endl;

                    fh_y_res[iDet]->Fill(y_l - det_hit_y);
                    fh_y_pull[iDet]->Fill((y_l - det_hit_y) / det->res_y);
                }
            }
            candidate->Reset();
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
    if ((alpha && carbon && fBfield == -1710.0) || (oxygen && fBfield != -1710.0))
    {
        counter1++;

        cout << "Found Tracks: " << counter1 << " from selected NEvents: " << fNEvents_nonull
             << "num total events: " << fNEvents << endl;
        ;
    }

    // delete particle;
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
}

void R3BFragmentTrackerS494::Finish()
{
    cout << "Total chi2 for mass: " << totalChi2Mass << endl;
    cout << "Total chi2 for momentum: " << totalChi2P << endl;

    cout << "found pairs: " << counter1 << endl;

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

        LOG(DEBUG) << fwhm / fh_A_reco2->GetMean();

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
        LOG(ERROR) << "Unsupported type of field.";
        return kFALSE;
    }
    return kTRUE;
}

ClassImp(R3BFragmentTrackerS494)
