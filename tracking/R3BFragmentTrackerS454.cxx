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

#include "R3BFragmentTrackerS454.h"
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

R3BFragmentTrackerS454::R3BFragmentTrackerS454(const char* name, Bool_t vis, Int_t verbose)
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
    , fOptimizeGeometry(kFALSE)
    , fTrackItems(new TClonesArray("R3BTrack"))
    , fNofTrackItems()
{
    // this is the list of detectors (active areas) we use for tracking
    fDetectorsLeft->AddDetector("target", kTarget, "TargetGeoPar");
    fDetectorsLeft->AddDetector("fi3b", kTargetGlad, "fi3bGeoPar", "Fi3bHit");
    fDetectorsLeft->AddDetector("fi12", kAfterGlad, "fi12GeoPar", "Fi12Hit");
    fDetectorsLeft->AddDetector("fi10", kAfterGlad, "fi10GeoPar", "Fi10Hit");
    fDetectorsLeft->AddDetector("tofd", kTof, "tofdGeoPar", "TofdHit");

    fDetectorsRight->AddDetector("target", kTarget, "TargetGeoPar");
    fDetectorsRight->AddDetector("fi3a", kTargetGlad, "fi3aGeoPar", "Fi3aHit");
    fDetectorsRight->AddDetector("fi11", kAfterGlad, "fi11GeoPar", "Fi11Hit");
    fDetectorsRight->AddDetector("fi13", kAfterGlad, "fi13GeoPar", "Fi13Hit");
    fDetectorsRight->AddDetector("tofd", kTof, "tofdGeoPar", "TofdHit");
}

R3BFragmentTrackerS454::~R3BFragmentTrackerS454() { delete fTrackItems; }

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
InitStatus R3BFragmentTrackerS454::Init()
{
    FairRootManager* man = FairRootManager::Instance();

    fArrayMCTracks = (TClonesArray*)man->GetObject("MCTrack");
    if (NULL == fArrayMCTracks)
    {
        LOG(error) << "No MC Track array found in input file.";
        //        return kERROR;
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
    fh_mult_fi3a = new TH1F("h_mult_fi3a", "Multiplicity fi3a", 20, -0.5, 19.5);
    fh_mult_fi3b = new TH1F("h_mult_fi3b", "Multiplicity fi3b", 20, -0.5, 19.5);
    fh_mult_fi12 = new TH1F("h_mult_fi12", "Multiplicity fi12", 20, -0.5, 19.5);
    fh_mult_fi11 = new TH1F("h_mult_fi11", "Multiplicity fi11", 20, -0.5, 19.5);
    fh_mult_fi10 = new TH1F("h_mult_fi10", "Multiplicity fi10", 20, -0.5, 19.5);
    fh_mult_fi13 = new TH1F("h_mult_fi13", "Multiplicity fi13", 20, -0.5, 19.5);
    fh_mult_tofd = new TH1F("h_mult_tofd", "Multiplicity TOFd", 20, -0.5, 19.5);
    fh_eloss_fi3a_mc = new TH1F("h_eloss_fi3a_mc", "Energy loss fi3a (MC truth)", 2000, 0., 200.);
    fh_eloss_fi3a = new TH1F("h_eloss_fi3a", "Energy loss fi3a", 2000, 0., 200.);
    fh_eloss_fi3b_mc = new TH1F("h_eloss_fi3b_mc", "Energy loss fi3b (MC truth)", 2000, 0., 500.);
    fh_eloss_fi3b = new TH1F("h_eloss_fi3b", "Energy loss fi3b", 2000, 0., 500.);
    fh_eloss_fi12_mc = new TH1F("h_eloss_fi12_mc", "Energy loss fi12 (MC truth)", 2000, 0., 500.);
    fh_eloss_fi12 = new TH1F("h_eloss_fi12", "Energy loss fi12", 2000, 0., 500.);
    fh_eloss_fi11_mc = new TH1F("h_eloss_fi11_mc", "Energy loss fi11 (MC truth)", 2000, 0., 500.);
    fh_eloss_fi11 = new TH1F("h_eloss_fi11", "Energy loss fi11", 2000, 0., 500.);
    fh_eloss_fi10_mc = new TH1F("h_eloss_fi10_mc", "Energy loss fi10 (MC truth)", 2000, 0., 1000.);
    fh_eloss_fi10 = new TH1F("h_eloss_fi10", "Energy loss fi10", 2000, 0., 1000.);
    fh_eloss_fi13_mc = new TH1F("h_eloss_fi13_mc", "Energy loss fi13 (MC truth)", 2000, 0., 500.);
    fh_eloss_fi13 = new TH1F("h_eloss_fi13", "Energy loss fi13", 2000, 0., 500.);
    fh_ncand = new TH1F("h_ncand", "Number of candidates", 100, -0.5, 99.5);
    fh_A_reco1 = new TH1F("h_A_reco1", "Reconstructed mass, step 1", 2000., 0., 20.);
    fh_A_reco2 = new TH1F("h_A_reco2", "Reconstructed mass, step 2", 2000., 0., 20.);
    fh_mom_res = new TH1F("h_mom_res", "Momentum resolution", 500, -0.05, 0.05);
    fh_mass_res = new TH1F("h_mass_res", "Mass resolution", 1000, -5., 5.);
    fh_chi2 = new TH1F("h_chi2", "Chi2", 1000, 0., 100.);
    fh_vz_res = new TH1F("h_vz_res", "vz - vz_mc", 200, -1., 1.);
    fh_beta_res = new TH1F("h_beta_res", "beta - beta_mc", 200, -0.01, 0.01);
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

    Double_t ranges[] = { 10., 10., 10., 10., 10., 10., 10. };
    for (Int_t i = 0; i < 7; i++)
    {
        fh_x_res[i] = new TH1F(Form("h_x_res%d", i), Form("x residual %d", i), 200, -ranges[i], ranges[i]);
        fh_x_pull[i] = new TH1F(Form("h_x_pull%d", i), Form("x pull %d", i), 40, -10., 10.);
    }

    fFitter->Init(fPropagator, fEnergyLoss);

    Double_t scale = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetScale();
    Double_t field = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(0., 0., 240.);
    cout << "Field:" << field << " scale: " << scale << endl;

    return kSUCCESS;
}

InitStatus R3BFragmentTrackerS454::ReInit()
{
    SetParContainers();

    if (!InitPropagator())
    {
        return kERROR;
    }

    return kSUCCESS;
}

void R3BFragmentTrackerS454::SetParContainers()
{
    fFieldPar = (R3BFieldPar*)FairRuntimeDb::instance()->getContainer("R3BFieldPar");

    fDetectorsLeft->SetParContainers();
    fDetectorsRight->SetParContainers();
    fDetectors->SetParContainers();
}

void R3BFragmentTrackerS454::Exec(const Option_t*)
{
    fArrayFragments->Clear();
    if (fFragments.size() > 0)
    {
        for (auto const& x : fFragments)
        {
            delete x;
        }
        fFragments.clear();
    }
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

    // R3BTrackingDetector* target = fDetectorsRight->GetByName("target");
    // if(fNEvents == 0) target->hits.push_back(new R3BHit(0, 0., 0., 0., 0., 0));
    // R3BTrackingDetector* target = fDetectorsLeft->GetByName("target");
    // if(fNEvents == 0) target->hits.push_back(new R3BHit(0, 0., 0., 0., 0., 0));
    R3BTrackingDetector* fi3a = fDetectorsRight->GetByName("fi3a");
    R3BTrackingDetector* fi3b = fDetectorsLeft->GetByName("fi3b");
    R3BTrackingDetector* fi12 = fDetectorsLeft->GetByName("fi12");
    R3BTrackingDetector* fi11 = fDetectorsRight->GetByName("fi11");
    R3BTrackingDetector* fi10 = fDetectorsLeft->GetByName("fi10");
    R3BTrackingDetector* fi13 = fDetectorsRight->GetByName("fi13");
    R3BTrackingDetector* tof = fDetectorsLeft->GetByName("tofd");

    // if(fNEvents == 0) target->hits.push_back(new R3BHit(0, 0., 0., 0., 0., 0));

    if (tof->hits.size() > 0)
    {
        cout << "Hits ToFD: " << tof->hits.size() << endl;
        cout << "Hits right: " << fi3a->hits.size() << "  " << fi11->hits.size() << "  " << fi13->hits.size() << endl;

        cout << "Hits left: " << fi3b->hits.size() << "  " << fi12->hits.size() << "  " << fi10->hits.size() << endl;
    }

    Double_t beta = 0.;
    Double_t x0 = 0.;
    Double_t y0 = 0.;
    Double_t z0 = 0.;
    Double_t px0 = 0;
    Double_t py0 = 0.;
    Double_t pz0 = 0.;
    Double_t p = 0.;
    Double_t mass = 0.;

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
                    beta = 1. / TMath::Sqrt(1 + TMath::Power(ion->GetMass() / ion->GetP(), 2));
                    x0 = ion->GetStartX();
                    y0 = ion->GetStartY();
                    z0 = ion->GetStartZ();
                    px0 = ion->GetPx();
                    py0 = ion->GetPy();
                    pz0 = ion->GetPz();
                    p = ion->GetP();
                    mass = ion->GetMass();
                    LOG(info) << "MC ************ 4He **************";
                    LOG(info) << "MC position x: " << x0 << " y: " << y0 << " z: " << z0;
                    LOG(info) << "MC momentum p: " << p << " px " << px0 << " py " << py0 << " pz " << pz0;
                    LOG(info) << "MC mass: " << mass << " beta: " << beta;
                }
                if (PID == 1000060120)
                {
                    // 12C
                    beta = 1. / TMath::Sqrt(1 + TMath::Power(ion->GetMass() / ion->GetP(), 2));
                    x0 = ion->GetStartX();
                    y0 = ion->GetStartY();
                    z0 = ion->GetStartZ();
                    px0 = ion->GetPx();
                    py0 = ion->GetPy();
                    pz0 = ion->GetPz();
                    p = ion->GetP();
                    mass = ion->GetMass();
                    LOG(info) << "MC ************ 12C **************";
                    LOG(info) << "MC position x: " << x0 << " y: " << y0 << " z: " << z0;
                    LOG(info) << "MC momentum p: " << p << " px " << px0 << " py " << py0 << " pz " << pz0;
                    LOG(info) << "MC mass: " << mass << " beta: " << beta;
                }
                if (PID == 1000080160)
                {
                    // 16O
                    beta = 1. / TMath::Sqrt(1 + TMath::Power(ion->GetMass() / ion->GetP(), 2));
                    x0 = ion->GetStartX();
                    y0 = ion->GetStartY();
                    z0 = ion->GetStartZ();
                    px0 = ion->GetPx();
                    py0 = ion->GetPy();
                    pz0 = ion->GetPz();
                    p = ion->GetP();
                    mass = ion->GetMass();
                    LOG(info) << "MC ************ 16O **************";
                    LOG(info) << "MC position x: " << x0 << " y: " << y0 << " z: " << z0;
                    LOG(info) << "MC momentum p: " << p << " px " << px0 << " py " << py0 << " pz " << pz0;
                    LOG(info) << "MC mass: " << mass << " beta: " << beta;
                }
            }
        }
    }

    // Important: Set charge and initial position and momentum of the particle
    R3BTrackingParticle* particle = new R3BTrackingParticle(10., x0, y0, z0, px0, py0, pz0, beta, mass);

    fh_mult_fi3a->Fill(fi3a->hits.size());
    fh_mult_fi3b->Fill(fi3b->hits.size());
    fh_mult_fi12->Fill(fi12->hits.size());
    fh_mult_fi11->Fill(fi11->hits.size());
    fh_mult_fi10->Fill(fi10->hits.size());
    fh_mult_fi13->Fill(fi13->hits.size());
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

    Int_t ifi3a = 0;
    Int_t ifi3b = 0;
    Int_t ifi12 = 0;
    Int_t ifi11 = 0;
    Int_t ifi10 = 0;
    Int_t ifi13 = 0;
    Int_t itof = 0;
    if (0 == fi3a->hits.size())
        ifi3a = -1;
    if (0 == fi3b->hits.size())
        ifi3b = -1;
    if (0 == fi12->hits.size())
        ifi12 = -1;
    if (0 == fi11->hits.size())
        ifi11 = -1;
    if (0 == fi10->hits.size())
        ifi10 = -1;
    if (0 == fi13->hits.size())
        ifi13 = -1;
    if (0 == tof->hits.size())
        itof = -1;

    if (fi3b->hits.size() * fi12->hits.size() * fi10->hits.size() * tof->hits.size() > 1000)
        return;
    if (fi3a->hits.size() * fi11->hits.size() * fi13->hits.size() * tof->hits.size() > 1000)
        return;

    Bool_t alpha = kFALSE;
    Bool_t carbon = kFALSE;
    // The idea is to loop twice over the ToF wall hits.
    // First we want to look for 12C particle and then for 4He
    for (Int_t l = 1; l < 3; l++)
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
                charge = sqrt(tof->hits.at(i)->GetEloss()) / 0.037675 + 0.5;
                Charge = sqrt(tof->hits.at(i)->GetEloss()) / 0.037675;
            }
            else
            {
                // For tracking of exp. data:
                charge = tof->hits.at(i)->GetEloss();
                Charge = tof->hits.at(i)->GetEloss();
            }
            cout << "Charge: " << charge << " requested charge: " << charge_requested << endl;
            if (charge != charge_requested)
                continue;

            Double_t beta0 = 0.76; // velocity could eventually be calculated from ToF
            tof->res_t = 0.03;
            // Double_t m0 = charge * 2. * 0.931494028; // First guess of mass

            if (charge == 8)
            {
                m0 = 15.0124;
                p0 = 17.390;
            }
            if (charge == 6)
            {
                m0 = 11.1749;
                p0 = 13.044;
            }
            if (charge == 2)
            {
                m0 = 3.7284;
                p0 = 4.348;
            }

            cout << "Mass: " << m0 << endl;
            cout << "Position on TofD: " << tof->hits.at(i)->GetX() << endl;
            if (!tof->free_hit[i]) // if the hit was used already, continue
            {
                cout << "ToFD hit already used" << endl;
                continue;
            }
            if (tof->hits.at(i)->GetX() > 0 && fi10->hits.size() > 0 && fi12->hits.size() > 0)
            {
                // left branch in beam direction, don't consider hits in the detectors of the other side
                R3BTrackingDetector* target = fDetectorsLeft->GetByName("target");
                if (fNEventsLeft == 0)
                    target->hits.push_back(new R3BHit(0, 0., 0., 0., 0., 0));

                Double_t fieldScale = 1672.0 / 3584. * 1.0; // standard
                Double_t scale = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetScale();
                Double_t field = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(0., 0., 240.);
                cout << "Field:" << field << " scale: " << scale << endl;

                fieldScale = -1672.0 / 3584. / scale * 1.018; // run395
                cout << "Setting field to " << fieldScale << endl;
                ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrection(fieldScale);
                field = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(0., 0., 240.);
                cout << "Field after:" << field << endl;

                do // fi10
                {
                    if (ifi10 >= 0)
                        fh_eloss_fi10_mc->Fill(1000. * fi10->hits.at(ifi10)->GetEloss()); // MeV
                    if (ifi10 >= 0 && !fi10->free_hit[ifi10]) // if the hit was used already, continue
                    {
                        cout << "Fi10 hit already used" << endl;
                        ifi10 += 1;
                        continue;
                    }
                    do // fi12
                    {
                        if (ifi12 >= 0)
                            fh_eloss_fi12_mc->Fill(1000. * fi12->hits.at(ifi12)->GetEloss()); // MeV
                        if (ifi12 >= 0 && !fi12->free_hit[ifi12]) // if the hit was used already, continue
                        {
                            cout << "Fi12 hit already used" << endl;
                            ifi12 += 1;
                            continue;
                        }
                        do // fi3b
                        {
                            if (ifi3b >= 0)
                                fh_eloss_fi3b_mc->Fill(1000. * fi3b->hits.at(ifi3b)->GetEloss()); // MeV
                            if (ifi3b >= 0 && !fi3b->free_hit[ifi3b]) // if the hit was used already, continue
                            {
                                cout << "Fi3b hit already used" << endl;
                                ifi3b += 1;
                                continue;
                            }
                            // Create object for particle which will be fitted
                            R3BTrackingParticle* candidate =
                                new R3BTrackingParticle(charge, 0., 0., 0., 0., 0., p0, beta0, m0); // 17.39

                            cout << "left side of setup" << endl;
                            cout << "Hit Tofd # " << i << " x: " << tof->hits.at(i)->GetX()
                                 << " y: " << tof->hits.at(i)->GetY() << endl;
                            if (ifi3b > -1)
                                cout << " Fi3b # " << ifi3b << " x: " << fi3b->hits.at(ifi3b)->GetX() << endl;
                            if (ifi10 > -1)
                                cout << " Fi10 # " << ifi10 << " x: " << fi10->hits.at(ifi10)->GetX() << endl;
                            if (ifi12 > -1)
                                cout << " Fi12 # " << ifi12 << " x: " << fi12->hits.at(ifi12)->GetX() << endl;
                            cout << "Hit target # "
                                 << " x: " << target->hits.at(0)->GetX() << endl;

                            candidate->AddHit("target", 0);
                            candidate->AddHit("tofd", i);
                            candidate->AddHit("fi3b", ifi3b);
                            candidate->AddHit("fi12", ifi12);
                            candidate->AddHit("fi10", ifi10);

                            fDetectors = fDetectorsLeft;
                            Bool_t forward = kTRUE;
                            Int_t status = 10;
                            if (forward)
                            {
                                status = fFitter->FitTrackMomentumForward(candidate, fDetectors);
                            }
                            else
                            {
                                status = fFitter->FitTrackBackward2D(candidate, fDetectors);
                            }
                            cout << "Chi2: " << candidate->GetChi2() << "  " << candidate->GetStartMomentum().Mag()
                                 << "  "
                                 << 100000. * (candidate->GetStartMomentum().Mag() - p0) *
                                        (candidate->GetStartMomentum().Mag() - p0)
                                 << endl;
                            cout << "--------------------------------" << endl;
                            nCand += 1;

                            // cout << "Momentum: " << candidate->GetMomentum().Z() << endl;
                            if (TMath::IsNaN(candidate->GetMomentum().Z()))
                            {
                                delete candidate;
                                continue;
                            }

                            if (10 > status)
                            {
                                if (forward)
                                {
                                    candidate->Reset();
                                }
                                else
                                {
                                    candidate->SetStartPosition(candidate->GetPosition());
                                    candidate->SetStartMomentum(-1. * candidate->GetMomentum());
                                    // candidate->SetStartBeta(0.8328);
                                    candidate->SetStartBeta(beta0);
                                    candidate->UpdateMomentum();
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

                            ifi3b += 1;
                        } while (ifi3b < fi3b->hits.size());
                        ifi3b = 0;
                        if (0 == fi3b->hits.size())
                            ifi3b = -1;

                        ifi12 += 1;
                    } while (ifi12 < fi12->hits.size());
                    ifi12 = 0;
                    if (0 == fi12->hits.size())
                        ifi12 = -1;

                    ifi10 += 1;
                } while (ifi10 < fi10->hits.size());
                ifi10 = 0;
                if (0 == fi10->hits.size())
                    ifi10 = -1;

                fNEventsLeft += 1;

            } // end if left branch
            if (tof->hits.at(i)->GetX() < 0 && fi11->hits.size() > 0 && fi13->hits.size() > 0)
            {
                // right branch in beam direction, don't consider hits in the detectors of the other side
                R3BTrackingDetector* target = fDetectorsRight->GetByName("target");
                if (fNEventsRight == 0)
                    target->hits.push_back(new R3BHit(0, 0., 0., 0., 0., 0));

                Double_t fieldScale = 1672.0 / 3584. * 1.0; // standard
                Double_t scale = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetScale();
                Double_t field = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(0., 0., 240.);
                cout << "Field:" << field << " scale: " << scale << endl;

                fieldScale = -1672.0 / 3584. / scale * 0.9776; // run395
                cout << "Setting field to " << fieldScale << endl;
                ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrection(fieldScale);
                field = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(0., 0., 240.);
                cout << "Field after:" << field << endl;

                do // fi13
                {
                    if (ifi13 >= 0)
                        fh_eloss_fi13_mc->Fill(1000. * fi13->hits.at(ifi13)->GetEloss()); // MeV
                    if (ifi13 >= 0 && !fi13->free_hit[ifi13]) // if the hit was used already, continue
                    {
                        cout << "Fi13 hit already used" << endl;
                        ifi13 += 1;
                        continue;
                    }
                    do // fi11
                    {
                        if (ifi11 >= 0)
                            fh_eloss_fi11_mc->Fill(1000. * fi11->hits.at(ifi11)->GetEloss()); // MeV
                        if (ifi11 >= 0 && !fi11->free_hit[ifi11]) // if the hit was used already, continue
                        {
                            cout << "Fi11 hit already used" << endl;
                            ifi11 += 1;
                            continue;
                        }
                        do // fi3a
                        {
                            if (ifi3a >= 0)
                                fh_eloss_fi3a_mc->Fill(1000. * fi3a->hits.at(ifi3a)->GetEloss()); // MeV
                            if (ifi3a >= 0 && !fi3a->free_hit[ifi3a]) // if the hit was used already, continue
                            {
                                cout << "Fi3a hit already used" << endl;
                                ifi3a += 1;
                                continue;
                            }
                            // Create object for particle which will be fitted
                            R3BTrackingParticle* candidate =
                                new R3BTrackingParticle(charge, 0., 0., 0., 0., 0., p0, beta0, m0); // 17.39

                            cout << "right side of setup" << endl;
                            cout << "Hit Tofd # " << i << " x: " << tof->hits.at(i)->GetX()
                                 << " y: " << tof->hits.at(i)->GetY() << endl;
                            if (ifi3a > -1)
                                cout << " Fi3a # " << ifi3a << " x: " << fi3a->hits.at(ifi3a)->GetX() << endl;
                            if (ifi13 > -1)
                                cout << "Fi13 # " << ifi13 << " x: " << fi13->hits.at(ifi13)->GetX() << endl;
                            if (ifi11 > -1)
                                cout << "Fi11 # " << ifi11 << " x: " << fi11->hits.at(ifi11)->GetX() << endl;
                            cout << "Hit target # "
                                 << " x: " << target->hits.at(0)->GetX() << endl;

                            candidate->AddHit("target", 0);
                            candidate->AddHit("tofd", i);
                            candidate->AddHit("fi3a", ifi3a);
                            candidate->AddHit("fi11", ifi11);
                            candidate->AddHit("fi13", ifi13);

                            fDetectors = fDetectorsRight;
                            Bool_t forward = kTRUE;
                            Int_t status = 10;
                            if (forward)
                            {
                                status = fFitter->FitTrackMomentumForward(candidate, fDetectors);
                            }
                            else
                            {
                                status = fFitter->FitTrackBackward2D(candidate, fDetectors);
                            }
                            cout << "Chi2: " << candidate->GetChi2() << "  " << candidate->GetStartMomentum().Mag()
                                 << "  "
                                 << 100000. * (candidate->GetStartMomentum().Mag() - p0) *
                                        (candidate->GetStartMomentum().Mag() - p0)
                                 << endl;
                            cout << "--------------------------------" << endl;
                            nCand += 1;

                            // cout << "Momentum: " << candidate->GetMomentum().Z() << endl;
                            if (TMath::IsNaN(candidate->GetMomentum().Z()))
                            {
                                delete candidate;
                                continue;
                            }

                            if (10 > status)
                            {
                                if (forward)
                                {
                                    candidate->Reset();
                                }
                                else
                                {
                                    candidate->SetStartPosition(candidate->GetPosition());
                                    candidate->SetStartMomentum(-1. * candidate->GetMomentum());
                                    // candidate->SetStartBeta(0.8328);
                                    candidate->SetStartBeta(beta0);
                                    candidate->UpdateMomentum();
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
                            ifi3a += 1;
                        } while (ifi3a < fi3a->hits.size());
                        ifi3a = 0;
                        if (0 == fi3a->hits.size())
                            ifi3a = -1;

                        ifi11 += 1;
                    } while (ifi11 < fi11->hits.size());
                    ifi11 = 0;
                    if (0 == fi11->hits.size())
                        ifi11 = -1;

                    ifi13 += 1;
                } while (ifi13 < fi13->hits.size());
                ifi13 = 0;
                if (0 == fi13->hits.size())
                    ifi13 = -1;

                fNEventsRight += 1;

            } // end if right branch

        } // end for TofD

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
                parChi2 = sqrt(pChi2 * pChi2 + xChi2 * xChi2);

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
                fi3a->free_hit[candidate->GetHitIndexByName("fi3a")] = false;
                fi11->free_hit[candidate->GetHitIndexByName("fi11")] = false;
                fi13->free_hit[candidate->GetHitIndexByName("fi13")] = false;
            }
            else
            {
                fi3b->free_hit[candidate->GetHitIndexByName("fi3b")] = false;
                fi10->free_hit[candidate->GetHitIndexByName("fi10")] = false;
                fi12->free_hit[candidate->GetHitIndexByName("fi12")] = false;
            }
            tof->free_hit[candidate->GetHitIndexByName("tofd")] = false;

            if (l == 1)
            {
                LOG(info) << "12C";
                carbon = kTRUE;
            }

            if (l == 2)
            {
                LOG(info) << "4He";
                alpha = kTRUE;
            }
            LOG(info) << "Results after tracking mass:";
            LOG(info) << "Position x: " << candidate->GetStartPosition().X()
                      << " y: " << candidate->GetStartPosition().Y() << " z: " << candidate->GetStartPosition().Z();
            LOG(info) << "Momentum : " << candidate->GetStartMomentum().Mag()
                      << " px : " << candidate->GetStartMomentum().X() << " py: " << candidate->GetStartMomentum().Y()
                      << " pz: " << candidate->GetStartMomentum().Z();
            LOG(info) << "Mass   : " << candidate->GetMass();
            LOG(info) << "Beta   : " << candidate->GetStartBeta();
            // LOG(info) << "chi2: " << candidate->GetChi2() << endl;
            LOG(info) << "chi2: " << minChi2 << endl;

            fh_A_reco1->Fill(candidate->GetMass());
            if (minChi2 < 1.e4)
                totalChi2Mass += minChi2;
            /*
                        // Now we know the mass. We can set here the exact mass if wanted and then track the momentum
                        Double_t m =  candidate->GetMass();
                        if(TMath::Abs(candidate->GetMass() - 14.8951) < 1.5)
                            //m = 14.8951; // 16O
                            m = 15.0124; // 16O
                        if(TMath::Abs(candidate->GetMass() - 11.1749) < 1.5)
                            m = 11.1749; // 4He
                        if(TMath::Abs(candidate->GetMass() - 3.7284) < 1.5)
                            m = 3.7284; // 4He
                        m = 15.0124;
                        candidate->SetMass(m);
                        candidate->UpdateMomentum();
            */

            /*
                        fDetectors = fDetectorsLeft;
                        if(candidate->GetStartMomentum().X() < 0 )
                        {
                            fDetectors = fDetectorsRight;
                        }
            */
            /*
                        cout << "Test1: " << candidate->GetStartMomentum().X() << endl;
                        cout << "Test2: " << fDetectors->GetArray().size() << "  " <<
                            fDetectorsLeft->GetArray().size() << "  " <<
                            fDetectorsRight->GetArray().size() << endl;

                        cout << "Test3: " << candidate->GetHitIndexByName("fi12") << "  " <<
               candidate->GetHitIndexByName("fi10") << endl; cout << "Test4: " << candidate->GetHitIndexByName("fi11")
               << "  " << candidate->GetHitIndexByName("fi13") << endl;

                        Int_t status1 = fFitter->FitTrackMomentumBackward(candidate, fDetectors);
                        cout << "status: " << status1 << endl;
                        if(status1 > 9) return;

                        LOG(info) << "Track backward:";
                        LOG(info) << "Position x: " << candidate->GetStartPosition().X() <<
                            " y: " << candidate->GetStartPosition().Y() << " z: " << candidate->GetStartPosition().Z();
                        LOG(info) << "Momentum : " << candidate->GetStartMomentum().Mag() << " px : " <<
               candidate->GetStartMomentum().X() << " py: " << candidate->GetStartMomentum().Y() << " pz: " <<
               candidate->GetStartMomentum().Z(); LOG(info) << "Mass   : " << candidate->GetMass(); LOG(info) << "Beta
               : " << candidate->GetStartBeta(); LOG(info) << "chi2: " << candidate->GetChi2() << endl;
            */

            // Int_t status2 = fFitter->FitTrackMomentumForward(candidate, fDetectors);
            // cout << "status: " << status2 << endl;
            // if(status2 > 9) return;

            /*
                        LOG(info) << "Track forward:";
                        LOG(info) << "Position x: " << candidate->GetStartPosition().X() <<
                            " y: " << candidate->GetStartPosition().Y() << " z: " << candidate->GetStartPosition().Z();
                        LOG(info) << "Momentum : " << candidate->GetStartMomentum().Mag() << " px : " <<
               candidate->GetStartMomentum().X() << " py: " << candidate->GetStartMomentum().Y() << " pz: " <<
               candidate->GetStartMomentum().Z(); LOG(info) << "Mass   : " << candidate->GetMass(); LOG(info) << "Beta
               : " << candidate->GetStartBeta(); LOG(info) << "chi2: " << candidate->GetChi2() << endl;
            */
            totalChi2P += minChi2;
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
            fh_A_reco2->Fill(candidate->GetMass());
            fh_A_overZ->Fill(Charge, candidate->GetMass() / Charge / amu);
            fh_mom_res->Fill((candidate->GetStartMomentum().Mag() - particle->GetStartMomentum().Mag()) /
                             particle->GetStartMomentum().Mag());
            fh_chi2->Fill(minChi2);
            fh_mass_res->Fill(candidate->GetMass() - particle->GetMass());
            fh_vz_res->Fill(candidate->GetStartPosition().X() - particle->GetStartPosition().X());
            fh_beta_res->Fill((candidate->GetStartBeta() - particle->GetStartBeta()) / particle->GetStartBeta());
            // candidate->GetStartPosition().Print();
            // LOG(info) << (candidate->GetMass() / amu);

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
                    LOG(debug1) << "Propagate to " << det->GetDetectorName();
                    fPropagator->PropagateToDetector(candidate, det);
                }

                if (det->GetDetectorName().EqualTo("fi3a"))
                { // fi3a
                    Double_t eloss = det->GetEnergyLoss(candidate);
                    fh_eloss_fi3a->Fill(eloss);
                    iDet = 0;
                }

                if (det->GetDetectorName().EqualTo("fi3b"))
                { // fi3b
                    Double_t eloss = det->GetEnergyLoss(candidate);
                    fh_eloss_fi3b->Fill(eloss);
                    iDet = 1;
                    // cout << "Eloss fi3b: " << eloss << endl;
                }

                if (det->GetDetectorName().EqualTo("fi10"))
                { // fi10
                    Double_t eloss = det->GetEnergyLoss(candidate);
                    fh_eloss_fi10->Fill(eloss);
                    iDet = 2;
                    // cout << "Eloss fi10: " << eloss << endl;
                }

                if (det->GetDetectorName().EqualTo("fi11"))
                { // fi11
                    Double_t eloss = det->GetEnergyLoss(candidate);
                    fh_eloss_fi11->Fill(eloss);
                    iDet = 3;
                    // cout << "Eloss fi11: " << eloss << endl;
                }

                if (det->GetDetectorName().EqualTo("fi12"))
                { // fi12
                    Double_t eloss = det->GetEnergyLoss(candidate);
                    fh_eloss_fi12->Fill(eloss);
                    iDet = 4;
                    // cout << "Eloss fi12: " << eloss << endl;
                }

                if (det->GetDetectorName().EqualTo("fi13"))
                { // fi13
                    Double_t eloss = det->GetEnergyLoss(candidate);
                    fh_eloss_fi13->Fill(eloss);
                    iDet = 5;
                    // cout << "Eloss fi13: " << eloss << endl;
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
                    cout << "current position: " << candidate->GetPosition().X() << "  " << candidate->GetPosition().Y()
                         << "  " << candidate->GetPosition().Z() << endl;
                    Double_t det_hit_x = hit->GetX();
                    cout << "Residuals " << det->GetDetectorName().Data() << "  " << x_l << "  " << det_hit_x << endl;
                    fh_x_res[iDet]->Fill(x_l - det_hit_x);
                    fh_x_pull[iDet]->Fill((x_l - det_hit_x) / det->res_x);
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
    if (alpha && carbon)
    {
        counter1++;
    }

    if (0 == (fNEvents % 1000))
    {
        LOG(info) << "Event: " << fNEvents;
    }

    fNEvents += 1;

    delete particle;
}
void R3BFragmentTrackerS454::FinishEvent()
{
    fTrackItems->Clear();
    fNofTrackItems = 0;
}
void R3BFragmentTrackerS454::Finish()
{
    cout << "Total chi2 for mass: " << totalChi2Mass << endl;
    cout << "Total chi2 for momentum: " << totalChi2P << endl;

    cout << "found pairs: " << counter1 << endl;

    fh_mult_fi3a->Write();
    fh_mult_fi3b->Write();
    fh_mult_fi10->Write();
    fh_mult_fi11->Write();
    fh_mult_fi12->Write();
    fh_mult_fi13->Write();
    fh_mult_tofd->Write();
    fh_eloss_fi3a_mc->Write();
    fh_eloss_fi3a->Write();
    fh_eloss_fi3b_mc->Write();
    fh_eloss_fi3b->Write();
    fh_eloss_fi10_mc->Write();
    fh_eloss_fi10->Write();
    fh_eloss_fi11_mc->Write();
    fh_eloss_fi11->Write();
    fh_eloss_fi12_mc->Write();
    fh_eloss_fi12->Write();
    fh_eloss_fi13_mc->Write();
    fh_eloss_fi13->Write();
    fh_ncand->Write();
    fh_A_reco1->Write();
    fh_A_reco2->Write();
    fh_mom_res->Write();
    fh_mass_res->Write();
    fh_chi2->Write();
    for (Int_t i = 0; i < 6; i++)
    {
        fh_x_res[i]->Write();
        fh_x_pull[i]->Write();
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

    if (fVis)
    {
        for (auto const& det : fDetectors->GetArray())
        {
            det->Draw();
        }

        TCanvas* c11 = new TCanvas("c11", "", 50, 50, 400, 400);
        c11->Divide(2, 2);
        c11->cd(1);
        fh_mult_fi3b->Draw();
        c11->cd(2);
        fh_mult_fi12->Draw();
        c11->cd(3);
        fh_mult_fi3a->Draw();
        c11->cd(4);
        fh_mult_tofd->Draw();
        c11->cd(0);

        new TCanvas("c2", "", 50, 50, 400, 400);
        fh_eloss_fi3a_mc->SetLineStyle(2);
        fh_eloss_fi3a_mc->SetLineColor(2);
        fh_eloss_fi3a_mc->Draw();
        fh_eloss_fi3a->Draw("same");

        //        TCanvas* c3 = new TCanvas("c3", "", 200, 10, 1000, 500);
        //        c3->Divide(2, 1);
        //        c3->cd(1);
        //        c3->cd(2);
        //        c3->cd(0);

        TCanvas* c5 = new TCanvas("c5", "", 50, 50, 400, 400);
        c5->Divide(2, 1);
        c5->cd(1);
        fh_mom_res->Draw();
        c5->cd(2);
        fh_mass_res->Draw();
        c5->cd(0);

        new TCanvas("c6", "", 50, 50, 400, 400);
        fh_A_reco2->Draw();
        fh_A_reco2->GetXaxis()->SetRangeUser(1., 20.);
        //        fh_A_reco2->SetLineColor(2);
        //        fh_A_reco2->Draw("same");

        Int_t bin1 = fh_A_reco2->FindFirstBinAbove(fh_A_reco2->GetMaximum() / 2.);
        Int_t bin2 = fh_A_reco2->FindLastBinAbove(fh_A_reco2->GetMaximum() / 2.);
        Double_t fwhm = fh_A_reco2->GetBinCenter(bin2) - fh_A_reco2->GetBinCenter(bin1);

        LOG(info) << fwhm / fh_A_reco2->GetMean();

        new TCanvas("c7", "", 50, 50, 400, 400);
        fh_chi2->Draw();

        TCanvas* c8 = new TCanvas("c8", "", 50, 50, 400, 400);
        c8->Divide(3, 2);
        for (Int_t i = 0; i < 6; i++)
        {
            c8->cd(i + 1);
            fh_x_res[i]->Draw();
        }
        c8->cd(0);

        TCanvas* c9 = new TCanvas("c9", "", 50, 50, 400, 400);
        c9->Divide(3, 2);
        for (Int_t i = 0; i < 6; i++)
        {
            c9->cd(i + 1);
            fh_x_pull[i]->Draw();
        }
        c9->cd(0);
        /*
                new TCanvas("c10", "", 50, 50, 400, 400);
                fh_vz_res->Draw();

                new TCanvas("c12", "", 600, 10, 500, 500);
                fh_ncand->Draw();

                new TCanvas("c12", "", 10, 10, 500, 500);
                fh_beta_res->Draw();
        */
    }
}

Bool_t R3BFragmentTrackerS454::InitPropagator()
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

ClassImp(R3BFragmentTrackerS454)
