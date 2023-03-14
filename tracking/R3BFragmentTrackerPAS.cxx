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

#include "R3BFragmentTrackerPAS.h"
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

R3BFragmentTrackerPAS::R3BFragmentTrackerPAS(const char* name, Bool_t vis, Int_t verbose)
    : FairTask(name, verbose)
    , fFieldPar(NULL)
    , fPropagator(NULL)
    , fArrayMCTracks(NULL)
    , fDetectors(new R3BTrackingSetup())
    , fArrayFragments(new TClonesArray("R3BTrackingParticle"))
    , fNEvents(0)
    , fVis(vis)
    , fFitter(nullptr)
    , fEnergyLoss(kTRUE)
    , fSimu(kFALSE)
    , fForward(kTRUE)
    , fBfield(-1710.)
    , fTrackItems(new TClonesArray("R3BTrack"))
    , fNofTrackItems()
    , fTofdHitItems(new TClonesArray("R3BTofdHitData"))
    , fFi23aHitItems(new TClonesArray("R3BFiberMAPMTHitData"))
    , fFi23bHitItems(new TClonesArray("R3BFiberMAPMTHitData"))
    , fFi60HitItems(new TClonesArray("R3BFiberMAPMTHitData"))
    , fFi61HitItems(new TClonesArray("R3BFiberMAPMTHitData"))
    , fFi62HitItems(new TClonesArray("R3BFiberMAPMTHitData"))
    , fFi63HitItems(new TClonesArray("R3BFiberMAPMTHitData"))
    , fNofTofdHitItems(0)
    , fNofFi23aHitItems(0)
    , fNofFi23bHitItems(0)
    , fNofFi60HitItems(0)
    , fNofFi61HitItems(0)
    , fNofFi62HitItems(0)
    , fNofFi63HitItems(0)
    , fWriteOut(kFALSE)
    , fPmax(20000)
    , fPmin(0)
{
    // this is the list of detectors (active areas) we use for tracking
    fDetectors->AddDetector("target", kTarget, "TargetGeoPar");
    fDetectors->AddDetector("fi23a", kTargetGlad, "fi23aGeoPar", "Fi23aHit");
    fDetectors->AddDetector("fi23b", kTargetGlad, "fi23bGeoPar", "Fi23bHit");
    fDetectors->AddDetector("fi60", kAfterGlad, "fi60GeoPar", "Fi60Hit");
    fDetectors->AddDetector("fi61", kAfterGlad, "fi61GeoPar", "Fi61Hit");
    fDetectors->AddDetector("fi62", kAfterGlad, "fi62GeoPar", "Fi62Hit");
    fDetectors->AddDetector("fi63", kAfterGlad, "fi63GeoPar", "Fi63Hit");
    fDetectors->AddDetector("tofd", kTof, "tofdGeoPar", "TofdHit");
}

R3BFragmentTrackerPAS::~R3BFragmentTrackerPAS() {}

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
InitStatus R3BFragmentTrackerPAS::Init()
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
    man->Register("Fi60Hit", "Land", fFi60HitItems, kTRUE);
    man->Register("Fi61Hit", "Land", fFi61HitItems, kTRUE);
    man->Register("Fi62Hit", "Land", fFi62HitItems, kTRUE);
    man->Register("Fi63Hit", "Land", fFi63HitItems, kTRUE);

    man->Register("TrackingParticle", "Tracking", fArrayFragments, kTRUE);
    man->Register("Track", "Land", fTrackItems, kTRUE);

    if (!InitPropagator())
    {
        return kERROR;
    }

    fDetectors->Init();
    fh_mult_fi23a = new TH1F("h_mult_fi23a", "Multiplicity fi23a", 20, -0.5, 19.5);
    fh_mult_fi23b = new TH1F("h_mult_fi23b", "Multiplicity fi23b", 20, -0.5, 19.5);
    fh_y_vs_x = new TH2F("h_y_vs_x", "Fi23b y vs. Fi23a x", 100, -5., 5., 100, -5., 5.);
    fh_mult_fi60 = new TH1F("h_mult_fi60", "Multiplicity fi60", 20, -0.5, 19.5);
    fh_mult_fi61 = new TH1F("h_mult_fi61", "Multiplicity fi61", 20, -0.5, 19.5);
    fh_mult_fi62 = new TH1F("h_mult_fi62", "Multiplicity fi62", 20, -0.5, 19.5);
    fh_mult_fi63 = new TH1F("h_mult_fi63", "Multiplicity fi63", 20, -0.5, 19.5);
    fh_mult_tofd = new TH1F("h_mult_tofd", "Multiplicity TOFd", 20, -0.5, 19.5);
    fh_eloss_fi23a_mc = new TH1F("h_eloss_fi23a_mc", "Energy loss fi23a (MC truth)", 2000, 0., 50.);
    fh_eloss_fi23a = new TH1F("h_eloss_fi23a", "Energy loss fi23a", 2000, 0., 50.);
    fh_eloss_fi23b_mc = new TH1F("h_eloss_fi23b_mc", "Energy loss fi23b (MC truth)", 2000, 0., 50.);
    fh_eloss_fi23b = new TH1F("h_eloss_fi23b", "Energy loss fi23b", 2000, 0., 50.);
    fh_eloss_fi60_mc = new TH1F("h_eloss_fi60_mc", "Energy loss fi60 (MC truth)", 2000, 0., 50.);
    fh_eloss_fi60 = new TH1F("h_eloss_fi60", "Energy loss fi60", 2000, 0., 50.);
    fh_eloss_fi61_mc = new TH1F("h_eloss_fi61_mc", "Energy loss fi61 (MC truth)", 2000, 0., 50.);
    fh_eloss_fi61 = new TH1F("h_eloss_fi61", "Energy loss fi61", 2000, 0., 50.);
    fh_eloss_fi62_mc = new TH1F("h_eloss_fi62_mc", "Energy loss fi62 (MC truth)", 2000, 0., 50.);
    fh_eloss_fi62 = new TH1F("h_eloss_fi62", "Energy loss fi62", 2000, 0., 50.);
    fh_eloss_fi63_mc = new TH1F("h_eloss_fi63_mc", "Energy loss fi63 (MC truth)", 2000, 0., 50.);
    fh_eloss_fi63 = new TH1F("h_eloss_fi63", "Energy loss fi63", 2000, 0., 50.);
    fh_ncand = new TH1F("h_ncand", "Number of candidates", 100, -0.5, 99.5);
    fh_A_reco1 = new TH1F("h_A_reco1", "Reconstructed mass, step 1", 2000., 0., 20.);
    fh_A_reco2 = new TH1F("h_A_reco2", "Reconstructed mass, step 2", 2000., 0., 20.);

    fh_mom_res = new TH1F("h_mom_res", "Momentum resolution", 2000, -10., 10.);
    fh_mom_res->GetXaxis()->SetTitle("dp in %");
    fh_mom_res->GetYaxis()->SetTitle("counts");

    fh_mom_res_x = new TH1F("h_mom_res_x", "Momentum resolution px in %", 2000, -10., 10.);
    fh_mom_res_x->GetXaxis()->SetTitle("dpx in %");
    fh_mom_res_x->GetYaxis()->SetTitle("counts");

    fh_mom_res_y = new TH1F("h_mom_res_y", "Momentum resolution py in %", 2000, -10., 10.);
    fh_mom_res_y->GetXaxis()->SetTitle("dpy in %");
    fh_mom_res_y->GetYaxis()->SetTitle("counts");

    fh_mom_res_z = new TH1F("h_mom_res_z", "Momentum resolution pz in %", 2000, -10., 10.);
    fh_mom_res_z->GetXaxis()->SetTitle("dpz in %");
    fh_mom_res_z->GetYaxis()->SetTitle("counts");

    fh_mass_res = new TH1F("h_mass_res", "Mass resolution", 10, -5.5, 4.5);

    fh_chi2 = new TH1F("h_chi2", "Chi2", 100, 0., 10.);
    fh_chi2->GetXaxis()->SetTitle("chi2");
    fh_chi2->GetYaxis()->SetTitle("counts");

    fh_vz_res = new TH1F("h_vz_res", "vz - vz_mc", 200, -1., 1.);
    fh_beta_res = new TH1F("h_beta_res", "beta - beta_mc", 200, -0.1, 0.1);
    fh_p = new TH1F("h_p", "momentum p", 2000, 0., 2.);
    fh_p->GetXaxis()->SetTitle("pz in GeV/c");
    fh_p->GetYaxis()->SetTitle("counts");

    fh_px = new TH1F("h_px", "momentum px", 2000, -100., 100.);
    fh_px->GetXaxis()->SetTitle("px in MeV/c");
    fh_px->GetYaxis()->SetTitle("counts");

    fh_py = new TH1F("h_py", "momentum py", 2000, -100., 100.);
    fh_py->GetXaxis()->SetTitle("py in MeV/c");
    fh_py->GetYaxis()->SetTitle("counts");

    fh_pz = new TH1F("h_pz", "momentum pz", 2000, 0., 2.);
    fh_pz->GetXaxis()->SetTitle("pz in GeV/c");
    fh_pz->GetYaxis()->SetTitle("counts");

    Double_t ranges[] = { 1., 5., 5., 10., 10., 10., 10., 50 };
    Int_t bins[] = { 1000, 1000, 1000, 2000, 2000, 2000, 2000, 2000 };
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

InitStatus R3BFragmentTrackerPAS::ReInit()
{
    SetParContainers();

    if (!InitPropagator())
    {
        return kERROR;
    }

    return kSUCCESS;
}

void R3BFragmentTrackerPAS::SetParContainers()
{
    fFieldPar = (R3BFieldPar*)FairRuntimeDb::instance()->getContainer("R3BFieldPar");

    fDetectors->SetParContainers();
}

void R3BFragmentTrackerPAS::Exec(const Option_t*)
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

    Bool_t debug_loopout = false;
    Bool_t debug_loopin = false;
    Bool_t bestevents = false;

    /* this part needs to be adopted to each experiment / setup
     *
     * First thing is to tell each detector which hit we want to use
     * for the track fitting. This means: manually copy the hit we want to
     * use for tracking from the TClonesArray into the single hit structure.
     *
     * Then call fit_fragment() to optimize the track and calculate
     * particle properties.
     */
    fDetectors->CopyHits();

    R3BTrackingDetector* target = fDetectors->GetByName("target");
    R3BTrackingDetector* fi23a = fDetectors->GetByName("fi23a");
    R3BTrackingDetector* fi23b = fDetectors->GetByName("fi23b");
    R3BTrackingDetector* fi60 = fDetectors->GetByName("fi60");
    R3BTrackingDetector* fi61 = fDetectors->GetByName("fi61");
    R3BTrackingDetector* fi62 = fDetectors->GetByName("fi62");
    R3BTrackingDetector* fi63 = fDetectors->GetByName("fi63");
    R3BTrackingDetector* tof = fDetectors->GetByName("tofd");

    // R3BHit(Int_t detId, Double_t x, Double_t y, Double_t eloss, Double_t time, Int_t hitId)
    target->hits.push_back(new R3BHit(0, 0.0, 0.0, 0., 0., 0));

    if (tof->hits.size() > 0 && debug_loopout)
    {
        cout << "*************** NEW EVENT ****" << fNEvents << endl;
        cout << "Hits ToFD: " << tof->hits.size() << endl;
        cout << "Hits: " << fi23a->hits.size() << "  " << fi23b->hits.size() << "  " << fi60->hits.size() << "  "
             << fi61->hits.size() << "  " << fi62->hits.size() << "  " << fi63->hits.size() << endl;
    }

    if (fi23a->hits.size() > 0 && fi23b->hits.size() > 0 && tof->hits.size() > 0)
    {
        // we have a proton in the entrance channel
        counter1++;
    }

    if (tof->hits.size() < 1)
        return;
    if (fi23a->hits.size() < 1)
        return;
    if (fi23b->hits.size() < 1)
        return;
    if (fi60->hits.size() + fi61->hits.size() + fi62->hits.size() < 2)
        return;
    if (fi63->hits.size() < 1)
        return;
    if (tof->hits.size() < 1)
        return;

    // All detectors had at least one hit
    counter2++;

    // Start values
    Double_t betasim = 0.;
    Double_t xsim = 0.;
    Double_t ysim = 0.;
    Double_t zsim = 0.;
    Double_t pxsim = 0;
    Double_t pysim = 0.;
    Double_t pzsim = 0.;
    Double_t psim = 0.;
    Double_t masssim = 0.;

    Double_t beta0 = 0.;
    Double_t x0 = 0.;
    Double_t y0 = 0.;
    Double_t z0 = 0.;
    Double_t px0 = 0;
    Double_t py0 = 0.;
    Double_t pz0 = 0.;
    Double_t p0 = 0.;
    Double_t mass0 = 0.;

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
                if (PID == 2212)
                {
                    // proton
                    betasim = 1. / TMath::Sqrt(1 + TMath::Power(ion->GetMass() / ion->GetP(), 2));
                    xsim = ion->GetStartX(); // cm
                    ysim = ion->GetStartY(); // cm
                    zsim = ion->GetStartZ(); // cm
                    pxsim = ion->GetPx();    // GeV/c
                    pysim = ion->GetPy();    // GeV/c
                    pzsim = ion->GetPz();    // GeV/c
                    psim = ion->GetP();
                    masssim = ion->GetMass();
                    LOG(debug) << "MC ************ Proton **************";
                    LOG(debug) << "MC position x: " << xsim << " y: " << ysim << " z: " << zsim;
                    LOG(debug) << "MC momentum p: " << psim << " px " << pxsim << " py " << pysim << " pz " << pzsim;
                    LOG(debug) << "MC mass: " << masssim << " beta: " << betasim << endl;
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
    fh_mult_fi60->Fill(fi60->hits.size());
    fh_mult_fi61->Fill(fi61->hits.size());
    fh_mult_fi62->Fill(fi62->hits.size());
    fh_mult_fi63->Fill(fi63->hits.size());
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
    Int_t ifi60 = 0;
    Int_t ifi61 = 0;
    Int_t ifi62 = 0;
    Int_t ifi63 = 0;
    Int_t itof = 0;
    if (0 == fi23a->hits.size())
    {
        ifi23a = -1;
    }
    if (0 == fi23b->hits.size())
    {
        ifi23b = -1;
    }
    if (0 == fi60->hits.size())
    {
        ifi60 = -1;
    }
    if (0 == fi61->hits.size())
    {
        ifi61 = -1;
    }
    if (0 == fi62->hits.size())
    {
        ifi62 = -1;
    }
    if (0 == fi63->hits.size())
    {
        ifi63 = -1;
    }
    if (0 == tof->hits.size())
    {
        itof = -1;
    }
    Bool_t writeOutC = kFALSE;

    R3BTrackingParticle* candidate;
    // The idea is to loop twice over the ToF wall hits.
    // First we want to look for 12C particle and then for 4He

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
        for (Int_t i = 0; i < fi60->hits.size(); i++)
        {
            cout << "Fib60 hits: " << i << ", x: " << fi60->hits.at(i)->GetX()
                 << ", Q: " << fi60->hits.at(i)->GetEloss() << endl;
        }
        for (Int_t i = 0; i < fi61->hits.size(); i++)
        {
            cout << "Fib61 hits: " << i << ", x: " << fi61->hits.at(i)->GetX()
                 << ", Q: " << fi61->hits.at(i)->GetEloss() << endl;
        }
        for (Int_t i = 0; i < fi62->hits.size(); i++)
        {
            cout << "Fib62 hits: " << i << ", x: " << fi62->hits.at(i)->GetX()
                 << ", Q: " << fi62->hits.at(i)->GetEloss() << endl;
        }
        for (Int_t i = 0; i < fi63->hits.size(); i++)
        {
            cout << "Fib63 hits: " << i << ", x: " << fi63->hits.at(i)->GetX()
                 << ", Q: " << fi63->hits.at(i)->GetEloss() << endl;
        }

        for (Int_t i = 0; i < tof->hits.size(); i++)
        {
            cout << "Tofd hits  : " << i << ", " << tof->hits.at(i)->GetX() << ", " << tof->hits.at(i)->GetY()
                 << ", Q: " << tof->hits.at(i)->GetEloss() << endl;
        }
    }

    Int_t charge = 0; // integer charge
    Int_t charge_requested = 1;
    Double_t Charge = 0; // measured charge from the experiment, no interger!
    Double_t AoverZ = 1;

    // Loop over all combinations of hits
    for (Int_t i = 0; i < tof->hits.size(); i++) // loop over all ToFD hits
    {
        if (fSimu)
        {
            // For tracking of simulations:
            //   charge = sqrt(tof->hits.at(i)->GetEloss()) * 26.76 + 0.5;
            //   Charge = sqrt(tof->hits.at(i)->GetEloss()) * 26.76;

            // if digi used
            // Charge = tof->hits.at(i)->GetEloss();
            // charge = (int)(Charge + 0.5);

            // if digiHit used
            charge = (int)(tof->hits.at(i)->GetEloss() + 0.5);
            Charge = tof->hits.at(i)->GetEloss() * 1000.;
            // cout << "Test: " <<  tof->hits.at(i)->GetEloss() << endl;
        }
        else
        {
            // For tracking of exp. data:
            charge = (int)(tof->hits.at(i)->GetEloss() + 0.5);
            Charge = tof->hits.at(i)->GetEloss();
        }
        if (debug_loopout)
            cout << "Charge: " << charge << " requested charge: " << charge_requested << endl;

        charge = 1;
        if (charge != charge_requested)
            continue;

        beta0 = 0.8760237; // velocity could eventually be calculated from ToF
        // beta0 = 0.756986; // velocity could eventually be calculated from ToF
        // tof->res_t = 0.03;
        // Double_t mass0 = charge * 2. * 0.931494028; // First guess of mass

        // Masse wird nicht getreckt, nur momentum
        mass0 = 0.938272; // has to have the same value as what geant uses
        p0 = 1.6920410;   // in GeV/c2
        // p0 =  1.086969; // in GeV/c2

        // Create object for particle which will be fitted
        // R3BTrackingParticle* candidate =
        //                      new R3BTrackingParticle(charge, x0, y0, z0, 0., 0., p0, beta0, mass0);

        if (debug_loopin)
            cout << "Position on TofD: " << tof->hits.at(i)->GetX() << endl;
        if (!tof->free_hit[i]) // if the hit was used already, continue
        {
            if (debug_loopin)
                cout << "ToFD hit already used" << endl;
            continue;
        }

        if (debug_loopin)
            cout << "AT START: "
                 << "Mass: " << mass0 << ", Momentum: " << p0 << endl;

        do // fi63
        {
            if (ifi63 >= 0)
                fh_eloss_fi63_mc->Fill(1000. * fi63->hits.at(ifi63)->GetEloss()); // MeV
            if ((ifi63 >= 0 && !fi63->free_hit[ifi63])) // if the hit was used already, continue
            {
                if (debug_loopin)
                    cout << "Fi63 hit already used or not correct charge " << charge_requested << ", "
                         << fi63->hits.at(ifi63)->GetEloss() << endl;
                ifi63 += 1;
                continue;
            }
            do // fi62
            {
                if (ifi62 >= 0)
                    fh_eloss_fi62_mc->Fill(1000. * fi62->hits.at(ifi62)->GetEloss()); // MeV
                if ((ifi62 >= 0 && !fi62->free_hit[ifi62])) // if the hit was used already, continue
                {
                    if (debug_loopin)
                        cout << "Fi62 hit already used or not correct charge " << charge_requested << ", "
                             << fi62->hits.at(ifi62)->GetEloss() << endl;
                    ifi62 += 1;
                    continue;
                }
                do // fi61
                {
                    if (ifi61 >= 0)
                        fh_eloss_fi61_mc->Fill(1000. * fi61->hits.at(ifi61)->GetEloss()); // MeV
                    if ((ifi61 >= 0 && !fi61->free_hit[ifi61])) // if the hit was used already, continue
                    {
                        if (debug_loopin)
                            cout << "Fi61 hit already used or not correct charge " << charge_requested << ", "
                                 << fi61->hits.at(ifi61)->GetEloss() << endl;
                        ifi61 += 1;
                        continue;
                    }
                    do // fi60
                    {
                        if (ifi60 >= 0)
                            fh_eloss_fi60_mc->Fill(1000. * fi60->hits.at(ifi60)->GetEloss()); // MeV
                        if ((ifi60 >= 0 && !fi60->free_hit[ifi60])) // if the hit was used already, continue
                        {
                            if (debug_loopin)
                                cout << "Fi60 hit already used or not correct charge " << charge_requested << ", "
                                     << fi60->hits.at(ifi60)->GetEloss() << endl;
                            ifi60 += 1;
                            continue;
                        }
                        do // fi23b
                        {
                            if (ifi23b >= 0)
                                fh_eloss_fi23b_mc->Fill(1000. * fi23b->hits.at(ifi23b)->GetEloss()); // MeV
                            if (ifi23b >= 0 && !fi23b->free_hit[ifi23b]) // if the hit was used already, continue
                            {
                                if (debug_loopin)
                                    cout << "Fi23b hit : " << fi23b->hits.at(ifi23b)->GetY() << " already used" << endl;
                                ifi23b += 1;
                                continue;
                            }
                            do // fi23a
                            {
                                if (ifi23a >= 0)
                                    fh_eloss_fi23a_mc->Fill(1000. * fi23a->hits.at(ifi23a)->GetEloss()); // MeV
                                if (ifi23a >= 0 && !fi23a->free_hit[ifi23a])                             // if the hit
                                                                             // was used already, continue
                                {
                                    if (debug_loopin)
                                        cout << "Fi23a hit already used" << endl;
                                    ifi23a += 1;
                                    continue;
                                }

                                // Create object for particle which will be fitted
                                candidate = new R3BTrackingParticle(charge, x0, y0, z0, 0., 0., p0, beta0, mass0);

                                if (debug_loopin)
                                {
                                    cout << "Charge requested: " << charge_requested << endl;
                                    cout << "Start values to fit, x0: " << x0 << " y0: " << y0 << " z0: " << z0
                                         << " p0: " << p0 << " beta0: " << beta0 << " mass0: " << mass0 << endl;
                                    cout << "Hit Tofd # " << i << " x: " << tof->hits.at(i)->GetX()
                                         << " y: " << tof->hits.at(i)->GetY() << endl;

                                    cout << "Hit target # "
                                         << " x: " << target->hits.at(0)->GetX() << endl;
                                    if (ifi23a > -1)
                                        cout << " Fi23a right # " << ifi23a << " x: " << fi23a->hits.at(ifi23a)->GetX()
                                             << ", q: " << fi23a->hits.at(ifi23a)->GetEloss() << endl;
                                    if (ifi23b > -1)
                                        cout << "right Fi23b # " << ifi23b << " y: " << fi23b->hits.at(ifi23b)->GetY()
                                             << ", q: " << fi23b->hits.at(ifi23b)->GetEloss() << endl;
                                    if (ifi60 > -1)
                                        cout << "Fi60  # " << ifi60 << " x: " << fi60->hits.at(ifi60)->GetX()
                                             << ", q: " << fi60->hits.at(ifi60)->GetEloss() << endl;
                                    if (ifi61 > -1)
                                        cout << "Fi61 # " << ifi61 << " x: " << fi61->hits.at(ifi61)->GetX()
                                             << ", q: " << fi61->hits.at(ifi61)->GetEloss() << endl;
                                    if (ifi62 > -1)
                                        cout << "Fi62 # " << ifi62 << " x: " << fi62->hits.at(ifi62)->GetX()
                                             << ", q: " << fi62->hits.at(ifi62)->GetEloss() << endl;
                                    if (ifi63 > -1)
                                        cout << "Fi63 # " << ifi63 << " x: " << fi63->hits.at(ifi63)->GetX()
                                             << ", q: " << fi63->hits.at(ifi63)->GetEloss() << endl;
                                }

                                candidate->AddHit("target", 0);
                                candidate->AddHit("tofd", i);
                                candidate->AddHit("fi23a", ifi23a);
                                candidate->AddHit("fi23b", ifi23b);
                                candidate->AddHit("fi60", ifi60);
                                candidate->AddHit("fi61", ifi61);
                                candidate->AddHit("fi62", ifi62);
                                candidate->AddHit("fi63", ifi63);

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

                                //    cout <<fNEvents<<", RIGHT SIDE: Charge: "<< charge<<", Momentum: " <<
                                //    candidate->GetMomentum()Mag()<<", Momentum Z:
                                //    "<<candidate->GetMomentum().Z()<<
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
                                        candidate->Reset();
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

                        ifi60 += 1;
                    } while (ifi60 < fi60->hits.size());
                    ifi60 = 0;
                    if (0 == fi60->hits.size())
                        ifi60 = -1;

                    ifi61 += 1;
                } while (ifi61 < fi61->hits.size());
                ifi61 = 0;
                if (0 == fi61->hits.size())
                    ifi61 = -1;

                ifi62 += 1;
            } while (ifi62 < fi62->hits.size());
            ifi62 = 0;
            if (0 == fi62->hits.size())
                ifi62 = -1;

            ifi63 += 1;
        } while (ifi63 < fi63->hits.size());
        ifi63 = 0;
        if (0 == fi63->hits.size())
            ifi63 = -1;

    } // end for TofD

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
                bestcandidate = x;
                minChi2 = parChi2;
                // cout << "New min chi2: " << minChi2 << endl;
                // cout << "Corresponding Mass   : " << x->GetMass() << endl;
                // cout << "Corresponding Mass   : " << bestcandidate->GetMass() << endl;
            }
        }

        fi23a->free_hit[bestcandidate->GetHitIndexByName("fi23a")] = false;
        fi23b->free_hit[bestcandidate->GetHitIndexByName("fi23b")] = false;
        fi60->free_hit[bestcandidate->GetHitIndexByName("fi60")] = false;
        fi61->free_hit[bestcandidate->GetHitIndexByName("fi61")] = false;
        fi62->free_hit[bestcandidate->GetHitIndexByName("fi62")] = false;
        fi63->free_hit[bestcandidate->GetHitIndexByName("fi63")] = false;
        tof->free_hit[bestcandidate->GetHitIndexByName("tofd")] = false;

        Double_t x0soll = 0.;
        Double_t y0soll = 0.;
        Double_t z0soll = 0.;
        Double_t psoll = 0.;
        Double_t px0soll = 0.;
        Double_t py0soll = 0.;
        Double_t pz0soll = 0.;
        Double_t beta0soll = 0.;
        Double_t mass0soll = 0.;

        x0soll = x0;
        y0soll = y0;
        z0soll = z0;
        // from Simu
        if (fSimu)
        {
            x0soll = xsim;
            y0soll = ysim;
            z0soll = zsim;
            px0soll = pxsim;
            py0soll = pysim;
            pz0soll = pzsim;
            psoll = psim;
            mass0soll = masssim;
            beta0soll = betasim;
        }

        LOG(debug) << "Results after tracking :";
        LOG(debug) << "Charge   : " << charge << endl;
        LOG(debug) << "Position (soll) x: " << x0soll << " y: " << y0soll << " z: " << z0soll;
        LOG(debug) << "Position (ist)  x: " << bestcandidate->GetStartPosition().X()
                   << " y: " << bestcandidate->GetStartPosition().Y()
                   << " z: " << bestcandidate->GetStartPosition().Z();

        LOG(debug) << "Momentum (soll): " << psoll << " px : " << px0soll << " py: " << py0soll << " pz: " << pz0soll;
        LOG(debug) << "Momentum (ist) : " << bestcandidate->GetStartMomentum().Mag()
                   << " px : " << bestcandidate->GetStartMomentum().X()
                   << " py: " << bestcandidate->GetStartMomentum().Y()
                   << " pz: " << bestcandidate->GetStartMomentum().Z();

        //   cout << "Beta   : " << bestcandidate->GetStartBeta() << endl;
        // LOG(debug) << "chi2: " << bestcandidate->GetChi2() << endl;
        LOG(debug) << "chi: " << minChi2;

        fh_A_reco1->Fill(bestcandidate->GetMass() / amu);
        if (minChi2 < 1.e4)
        {
            totalChi2Mass += minChi2;
            totalEvents++;
        }
        totalChi2P += minChi2;

        if (minChi2 < 1000)
        {
            fh_p->Fill(bestcandidate->GetStartMomentum().Mag());
            fh_px->Fill(bestcandidate->GetStartMomentum().X() * 1000.);
            fh_py->Fill(bestcandidate->GetStartMomentum().Y() * 1000.);
            fh_pz->Fill(bestcandidate->GetStartMomentum().Z());
        }

        if (minChi2 < 20)
        {
            // good tracking
            counter3++;
        }

        fh_mom_res->Fill((bestcandidate->GetStartMomentum().Mag() - psoll) / psoll * 100.);
        fh_mom_res_x->Fill((bestcandidate->GetStartMomentum().X() - px0soll) / px0soll * 100.);
        fh_mom_res_y->Fill((bestcandidate->GetStartMomentum().Y() - py0soll) / py0soll * 100.);
        fh_mom_res_z->Fill((bestcandidate->GetStartMomentum().Z() - pz0soll) / pz0soll * 100.);

        fh_chi2->Fill(minChi2);
        fh_mass_res->Fill(bestcandidate->GetMass() - mass0soll);
        fh_vz_res->Fill(bestcandidate->GetStartPosition().X() - x0soll);
        fh_beta_res->Fill((bestcandidate->GetStartBeta() - beta0soll) / beta0soll);

        // store hits in track level

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

        Int_t iDet = 0;

        for (int ic = 0; ic < 8; ic++)
        {
            x_l[ic] = -10000.;
            y_l[ic] = -10000.;
            det_hit_x[ic] = -10000.;
            det_hit_y[ic] = -10000.;
            eloss_hit[ic] = 0;
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

            if (det->GetDetectorName().EqualTo("fi60"))
            { // fi60
                Double_t eloss = det->GetEnergyLoss(bestcandidate);
                fh_eloss_fi60->Fill(eloss);
                iDet = 3;
            }

            if (det->GetDetectorName().EqualTo("fi61"))
            { // fi61
                Double_t eloss = det->GetEnergyLoss(bestcandidate);
                fh_eloss_fi61->Fill(eloss);
                iDet = 4;
            }

            if (det->GetDetectorName().EqualTo("fi62"))
            { // fi62
                Double_t eloss = det->GetEnergyLoss(bestcandidate);
                fh_eloss_fi62->Fill(eloss);
                iDet = 5;
            }

            if (det->GetDetectorName().EqualTo("fi63"))
            { // fi63
                Double_t eloss = det->GetEnergyLoss(bestcandidate);
                fh_eloss_fi63->Fill(eloss);
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
            if (hit && det->res_x > 1e-6)
            {
                Double_t xres = x_l[iDet] - det_hit_x[iDet];
                if (TMath::Abs(x_l[iDet] - det_hit_x[iDet]) < det->res_x)
                {
                    // xres = 0.;
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
            }
        }

        bestcandidate->Reset();
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
}

void R3BFragmentTrackerPAS::FinishEvent()
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
    fNofFi60HitItems = 0;
    fFi60HitItems->Clear();
    fNofFi61HitItems = 0;
    fFi61HitItems->Clear();
    fNofFi62HitItems = 0;
    fFi62HitItems->Clear();
    fNofFi63HitItems = 0;
    fFi63HitItems->Clear();
}

void R3BFragmentTrackerPAS::Finish()
{
    cout << "Total chi2: " << totalChi2Mass / totalEvents << endl;
    //    cout << "Total chi2 for momentum: " << totalChi2P / totalEvents << endl;

    cout << "Protons on Fi23 and ToFD: " << counter1 << endl;
    cout << "Hits on all detectors: " << counter2 << endl;
    cout << "Good tracks: " << counter3 << endl;
    cout << "Efficiency tracking in %: " << (float)counter3 / (float)counter1 * 100. << endl;

    fh_mult_fi23a->Write();
    fh_mult_fi23b->Write();
    fh_mult_tofd->Write();
    fh_eloss_fi23a_mc->Write();
    fh_eloss_fi23a->Write();
    fh_eloss_fi23b_mc->Write();
    fh_eloss_fi23b->Write();
    fh_eloss_fi60_mc->Write();
    fh_eloss_fi60->Write();
    fh_eloss_fi61_mc->Write();
    fh_eloss_fi61->Write();
    fh_eloss_fi62_mc->Write();
    fh_eloss_fi62->Write();
    fh_eloss_fi63_mc->Write();
    fh_eloss_fi63->Write();
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
    fh_p->Write();
    fh_px->Write();
    fh_py->Write();
    fh_pz->Write();

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

        for (auto const& det : fDetectors->GetArray())
        {
            det->Draw();
        }

        TCanvas* c11 = new TCanvas("c11", "", 10, 10, 1000, 1000);
        c11->Divide(2, 2);
        c11->cd(1);
        fh_mult_fi23b->Draw();
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

Bool_t R3BFragmentTrackerPAS::InitPropagator()
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

ClassImp(R3BFragmentTrackerPAS)
