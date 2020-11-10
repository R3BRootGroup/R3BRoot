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

R3BFragmentTrackerS494::R3BFragmentTrackerS494(const char* name, Bool_t vis, Int_t verbose)
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
{
    // this is the list of detectors (active areas) we use for tracking
    fDetectors->AddDetector("target", kTarget, "TargetGeoPar");
    fDetectors->AddDetector("fi23a", kTargetGlad, "fi23aGeoPar", "Fi23aHit");
    // fDetectors->AddDetector("fi23b", kTargetGlad, "fi23bGeoPar", "Fi23bHit");
    fDetectors->AddDetector("fi32", kAfterGlad, "fi32GeoPar", "Fi32Hit");
    fDetectors->AddDetector("fi30", kAfterGlad, "fi30GeoPar", "Fi30Hit");
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
    FairRootManager* man = FairRootManager::Instance();

    fArrayMCTracks = (TClonesArray*)man->GetObject("MCTrack");
    if (NULL == fArrayMCTracks)
    {
        LOG(ERROR) << "No MC Track array found in input file.";
        return kERROR;
    }

    man->Register("TrackingParticle", "Tracking", fArrayFragments, kTRUE);

    if (!InitPropagator())
    {
        return kERROR;
    }

    fDetectors->Init();

    fh_mult_fi23a = new TH1F("h_mult_fi23a", "Multiplicity fi23a", 20, -0.5, 19.5);
    fh_mult_fi23b = new TH1F("h_mult_fi23b", "Multiplicity fi23b", 20, -0.5, 19.5);
    fh_mult_fi32 = new TH1F("h_mult_fi32", "Multiplicity fi32", 20, -0.5, 19.5);
    fh_mult_tofd = new TH1F("h_mult_tofd", "Multiplicity TOFd", 20, -0.5, 19.5);
    fh_eloss_fi23a_mc = new TH1F("h_eloss_fi23a_mc", "Energy loss fi23a (MC truth)", 2000, 0., 200.);
    fh_eloss_fi23a = new TH1F("h_eloss_fi23a", "Energy loss fi23a", 2000, 0., 200.);
    fh_eloss_fi23b_mc = new TH1F("h_eloss_fi23b_mc", "Energy loss fi23b (MC truth)", 2000, 0., 500.);
    fh_eloss_fi23b = new TH1F("h_eloss_fi23b", "Energy loss fi23b", 2000, 0., 500.);
    fh_eloss_fi32_mc = new TH1F("h_eloss_fi32_mc", "Energy loss fi32 (MC truth)", 2000, 0., 500.);
    fh_eloss_fi32 = new TH1F("h_eloss_fi32", "Energy loss fi32", 2000, 0., 500.);
    fh_eloss_fi30_mc = new TH1F("h_eloss_fi30_mc", "Energy loss fi30 (MC truth)", 2000, 0., 1000.);
    fh_eloss_fi30 = new TH1F("h_eloss_fi30", "Energy loss fi30", 2000, 0., 1000.);
    fh_ncand = new TH1F("h_ncand", "Number of candidates", 100, -0.5, 99.5);
    fh_A_reco1 = new TH1F("h_A_reco1", "Reconstructed mass, step 1", 200., 120., 140.);
    fh_A_reco2 = new TH1F("h_A_reco2", "Reconstructed mass, step 2", 200., 120., 140.);
    fh_mom_res = new TH1F("h_mom_res", "Momentum resolution", 500, -0.05, 0.05);
    fh_mass_res = new TH1F("h_mass_res", "Mass resolution", 10, -5.5, 4.5);
    fh_chi2 = new TH1F("h_chi2", "Chi2", 100, 0., 10.);
    fh_vz_res = new TH1F("h_vz_res", "vz - vz_mc", 200, -1., 1.);
    fh_beta_res = new TH1F("h_beta_res", "beta - beta_mc", 200, -0.1, 0.1);

    Double_t ranges[] = { 1., 1., 1., 1., 1., 10. };
    for (Int_t i = 0; i < 6; i++)
    {
        fh_x_res[i] = new TH1F(Form("h_x_res%d", i), Form("x residual %d", i), 200, -ranges[i], ranges[i]);
        fh_x_pull[i] = new TH1F(Form("h_x_pull%d", i), Form("x pull %d", i), 40, -10., 10.);
    }

    fFitter->Init(fPropagator, fEnergyLoss);

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

    fDetectors->SetParContainers();
}

void R3BFragmentTrackerS494::Exec(const Option_t*)
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
    fDetectors->CopyHits();

    R3BTrackingDetector* target = fDetectors->GetByName("target");
    R3BTrackingDetector* fi23a = fDetectors->GetByName("fi23a");
    // R3BTrackingDetector* fi23b = fDetectors->GetByName("fi23b");
    R3BTrackingDetector* fi32 = fDetectors->GetByName("fi32");
    R3BTrackingDetector* fi30 = fDetectors->GetByName("fi30");
    R3BTrackingDetector* tof = fDetectors->GetByName("tofd");

    cout << "Hits: " << fi23a->hits.size() << "  " //<< fi23b->hits.size() << "  "
         << fi32->hits.size() << "  " << fi30->hits.size() << "  " << tof->hits.size() << endl;

    // remember: in this test, target hast no data
    // if (target->hits->GetEntriesFast()==0) return; // no error, can always happen
    if (fi23a->hits.size() == 0)
        return; // no error, can always happen
    // if (fi23b->hits.size() == 0)
    // return; // no error, can always happen
    if (fi32->hits.size() == 0)
        return; // no error, can always happen
    if (fi30->hits.size() == 0)
        return; // no error, can always happen
    if (tof->hits.size() == 0)
        return; // no error, can always happen

    // fetch start pos, default momentum and charge from the simulation
    // (just for this test!)
    R3BMCTrack* ion = (R3BMCTrack*)fArrayMCTracks->At(0);
    // Check if primary
    if (ion->GetMotherId() != -1)
    {
        // error(); !!!
        // exit(0);
        return;
    }

    // Important: Set charge and initial position and momentum of the particle
    Double_t beta = 1. / TMath::Sqrt(1 + TMath::Power(ion->GetMass() / ion->GetP(), 2));

    Double_t px0 = ion->GetPx();
    Double_t py0 = ion->GetPy();
    Double_t pz0 = ion->GetPz();
    Double_t mass0 = ion->GetMass();

    R3BTrackingParticle* particle = new R3BTrackingParticle(8.,
                                                            ion->GetStartX(),
                                                            ion->GetStartY(),
                                                            ion->GetStartZ(),
                                                            ion->GetPx(),
                                                            ion->GetPy(),
                                                            ion->GetPz(),
                                                            beta,
                                                            ion->GetMass());
    LOG(INFO) << "MC mass " << ion->GetMass();
    LOG(INFO) << "MC momentum "
              << sqrt(ion->GetPx() * ion->GetPx() + ion->GetPy() * ion->GetPy() + ion->GetPz() * ion->GetPz()) /
                     ion->GetMass() / amu;
    LOG(INFO) << "MC px " << ion->GetPx();
    LOG(INFO) << "MC py " << ion->GetPy();
    LOG(INFO) << "MC pz " << ion->GetPz();
    LOG(INFO) << "MC beta " << beta;

    fh_mult_fi23a->Fill(fi23a->hits.size());
    // fh_mult_fi23b->Fill(fi23b->hits.size());
    fh_mult_fi32->Fill(fi32->hits.size());
    fh_mult_tofd->Fill(tof->hits.size());

    /* Note:
     * We DO need to know the beta of the particle in order to calculate
     * its energy loss in matter. For that reason only, we need to separate
     * the momentum into mass and beta and we need to vary these two
     * independently. Pity.
     */

    // try to fit all possible combination of hits. (For now just one)
    // for (all fi23a hits)
    //    for (all tof hits)
    fPropagator->SetVis(kFALSE);

    Int_t nCand = 0;

    {
        target->hits.push_back(new R3BHit(0, 0., 0., 0., 0., 0));
        target->res_x = 0.1000;
        target->res_y = 0.1000;

        for (auto const& xfi23a : fi23a->hits)
        {
            if (xfi23a->GetEloss() < 0.)
            {
                continue;
            }
            fi23a->res_x = 0.02500 / sqrt(12.);
            fi23a->res_y = 0.02500 / sqrt(12.);
            fh_eloss_fi23a_mc->Fill(xfi23a->GetEloss()); // MeV
            cout << "Eloss MC fi23a: " << xfi23a->GetEloss() << endl;
            // for (auto const& xfi23b : fi23b->hits)
            //{
            // fh_eloss_fi23b_mc->Fill(xfi23b->GetEloss()); // MeV
            // fi23b->res_x = 0.0200;

            for (auto const& xfi32 : fi32->hits)
            {
                fi32->res_x = 0.100 / sqrt(12.);
                fi32->res_y = 0.;
                cout << "Eloss MC fi32: " << xfi32->GetEloss() << endl;

                for (auto const& xfi30 : fi30->hits)
                {
                    fi30->res_x = 0.100 / sqrt(12.);
                    fi30->res_y = 0.;
                    cout << "Eloss MC fi30: " << xfi30->GetEloss() << endl;

                    for (auto const& xtof : tof->hits)
                    {
                        tof->res_x = 2.7 / sqrt(12.);
                        tof->res_y = 1. / sqrt(12.);
                        tof->res_t = 0.03;

                        Double_t velocity0 = 0.66;

                        // Create object for particle which will be fitted
                        R3BTrackingParticle* candidate =
                            new R3BTrackingParticle(8, 0., 0., 0., px0, py0, pz0, velocity0, mass0);

                        candidate->AddHit("target", 0);
                        candidate->AddHit("fi23a", xfi23a->GetHitId());
                        // candidate->AddHit("fi23b", xfi23b->GetHitId());
                        candidate->AddHit("fi32", xfi32->GetHitId());
                        candidate->AddHit("fi30", xfi30->GetHitId());
                        candidate->AddHit("tofd", xtof->GetHitId());

                        // find momentum
                        // momin is only a first guess
                        cout << "start backward tracking" << endl;
                        Int_t status = fFitter->FitTrackBackward2D(candidate, fDetectors);

                        nCand += 1;

                        if (TMath::IsNaN(candidate->GetMomentum().Z()))
                        {
                            delete candidate;
                            continue;
                        }

                        if (0 == status)
                        {
                            candidate->SetStartPosition(candidate->GetPosition());
                            candidate->SetStartMomentum(-1. * candidate->GetMomentum());
                            // candidate->SetStartBeta(0.8328);
                            candidate->SetStartBeta(velocity0);
                            candidate->UpdateMomentum();
                            candidate->Reset();

                            // candidate->GetStartPosition().Print();
                            // candidate->GetStartMomentum().Print();
                            cout << "chi2: " << candidate->GetChi2() << endl;
                            // status = FitFragment(candidate);

                            if (0 == status)
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
                    }
                }
            }
            //} // end fi23b
        }
    }

    fh_ncand->Fill(nCand);

    R3BTrackingParticle* candidate;
    Double_t minChi2 = 1e10;

    if (fFragments.size() > 0)
    {
        for (auto const& x : fFragments)
        {
            if (x->GetChi2() < minChi2)
            {
                candidate = x;
                minChi2 = x->GetChi2();
                cout << "New min chi2: " << minChi2 << endl;
                cout << "Corresponding Mass   : " << x->GetMass() << endl;
                cout << "Corresponding Mass   : " << candidate->GetMass() << endl;
            }
        }

        // Find out the mass
        const double mass =
            G4NistManager::Instance()->GetIsotopeMass(candidate->GetCharge(), TMath::Nint(candidate->GetMass() / amu)) /
            CLHEP::GeV;
        cout << "Test: " << candidate->GetCharge() << "  " << TMath::Nint(candidate->GetMass() / amu) << "  " << mass
             << endl;

        // fh_A_reco2->Fill(candidate->GetMass() / Fair_Amu);
        fh_A_reco2->Fill(candidate->GetMass() / amu);
        // candidate->SetMass(TMath::Nint(candidate->GetMass() / Fair_Amu) * Fair_Amu);
        cout << "Set integer mass: " << TMath::Nint(candidate->GetMass() / amu)
             << " before: " << candidate->GetMass() / amu << endl;

        // candidate->SetMass(TMath::Nint(candidate->GetMass() / amu) * amu);
        // candidate->SetMass(mass0);

        candidate->UpdateMomentum();
        cout << "Fit Beta, start: " << candidate->GetStartBeta() << endl;
        fFitter->FitTrackBeta(candidate, fDetectors);
        cout << "Fit Beta, end: " << candidate->GetStartBeta() << endl;

        Double_t momentum0 = candidate->GetStartMomentum().Mag();
        LOG(DEBUG1);
        LOG(DEBUG1) << "Momentum : " << momentum0;
        LOG(DEBUG1) << "Momentum px: " << candidate->GetStartMomentum().X();
        LOG(DEBUG1) << "Momentum py: " << candidate->GetStartMomentum().Y();
        LOG(DEBUG1) << "Momentum pz: " << candidate->GetStartMomentum().Z();

        LOG(DEBUG1) << "Truth  : " << particle->GetMomentum().Mag();
        LOG(DEBUG1) << "Truth px: " << px0;
        LOG(DEBUG1) << "Truth py: " << py0;
        LOG(DEBUG1) << "Truth pz: " << pz0;

        LOG(DEBUG1) << "Resolution: " << (momentum0 - particle->GetMomentum().Mag()) / particle->GetMomentum().Mag();
        LOG(DEBUG1) << "Mass   : " << candidate->GetMass();
        LOG(DEBUG1) << "Truth  : " << particle->GetMass() << "  " << mass0;
        LOG(DEBUG1) << "Mass resolution : " << (candidate->GetMass() - particle->GetMass()) / particle->GetMass();
        LOG(DEBUG1) << "Beta   : " << candidate->GetStartBeta();
        LOG(DEBUG1) << "Truth  : " << particle->GetStartBeta();
        LOG(DEBUG1) << "Beta resolution : "
                    << (candidate->GetStartBeta() - particle->GetStartBeta()) / particle->GetStartBeta();

        fh_mom_res->Fill((momentum0 - particle->GetStartMomentum().Mag()) / particle->GetStartMomentum().Mag());
        fh_chi2->Fill(candidate->GetChi2());
        cout << "chi2: " << minChi2 << endl;
        int imass = TMath::Nint(candidate->GetMass() / amu);
        int imass_mc = TMath::Nint(particle->GetMass() / amu);
        fh_mass_res->Fill(imass - imass_mc);
        fh_vz_res->Fill(candidate->GetStartPosition().X() - particle->GetStartPosition().X());
        fh_beta_res->Fill((candidate->GetStartBeta() - particle->GetStartBeta()) / particle->GetStartBeta());
        // candidate->GetStartPosition().Print();
        // LOG(INFO) << (candidate->GetMass() / amu);

        Double_t x_l = 0.;
        Double_t y_l = 0.;
        Int_t iDet = 0;
        fPropagator->SetVis();
        for (auto const& det : fDetectors->GetArray())
        {
            if (kTarget != det->section)
            {
                LOG(DEBUG2) << "Propagate to " << det->GetDetectorName();
                fPropagator->PropagateToDetector(candidate, det);
            }

            if (det->GetDetectorName().EqualTo("fi23a"))
            { // fi23a
                Double_t eloss = det->GetEnergyLoss(candidate);
                fh_eloss_fi23a->Fill(eloss);
                cout << "Eloss fi23a: " << eloss << endl;
            }

            if (det->GetDetectorName().EqualTo("fi23b"))
            { // fi23b
                Double_t eloss = det->GetEnergyLoss(candidate);
                fh_eloss_fi23b->Fill(eloss);
                cout << "Eloss fi23b: " << eloss << endl;
            }

            if (det->GetDetectorName().EqualTo("fi32"))
            { // fi32
                Double_t eloss = det->GetEnergyLoss(candidate);
                fh_eloss_fi32->Fill(eloss);
                cout << "Eloss fi32: " << eloss << endl;
            }

            if (det->GetDetectorName().EqualTo("fi30"))
            { // fi30
                Double_t eloss = det->GetEnergyLoss(candidate);
                fh_eloss_fi30->Fill(eloss);
                cout << "Eloss fi30: " << eloss << endl;
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
            Double_t det_hit_x =
                fDetectors
                    ->GetHit(det->GetDetectorName().Data(), candidate->GetHitIndexByName(det->GetDetectorName().Data()))
                    ->GetX();
            fh_x_res[iDet]->Fill(x_l - det_hit_x);
            fh_x_pull[iDet]->Fill((x_l - det_hit_x) / det->res_x);
            iDet++;
        }
        candidate->Reset();

        //        for (auto const& x : fFragments)
        //        {
        //            delete x;
        //        }
        //        fFragments.clear();
    }

    if (0 == (fNEvents % 10))
    {
        LOG(INFO) << "Event: " << fNEvents;
    }

    fNEvents += 1;

    delete particle;
}

void R3BFragmentTrackerS494::Finish()
{
    fh_mult_fi23a->Write();
    fh_mult_fi23b->Write();
    fh_mult_fi32->Write();
    fh_mult_tofd->Write();
    fh_eloss_fi23a_mc->Write();
    fh_eloss_fi23a->Write();
    fh_eloss_fi23b_mc->Write();
    fh_eloss_fi23b->Write();
    fh_eloss_fi32_mc->Write();
    fh_eloss_fi32->Write();
    fh_eloss_fi30_mc->Write();
    fh_eloss_fi30->Write();
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

    if (fVis)
    {
        for (auto const& det : fDetectors->GetArray())
        {
            det->Draw();
        }

        TCanvas* c11 = new TCanvas("c11", "", 10, 10, 1000, 1000);
        c11->Divide(2, 2);
        c11->cd(1);
        fh_mult_fi23b->Draw();
        c11->cd(2);
        fh_mult_fi32->Draw();
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

        LOG(INFO) << fwhm / fh_A_reco2->GetMean();

        new TCanvas("c7", "", 500, 600, 500, 500);
        fh_chi2->Draw();

        TCanvas* c8 = new TCanvas("c8", "", 10, 10, 1500, 500);
        c8->Divide(3, 2);
        for (Int_t i = 0; i < 6; i++)
        {
            c8->cd(i + 1);
            fh_x_res[i]->Draw();
        }
        c8->cd(0);

        TCanvas* c9 = new TCanvas("c9", "", 10, 10, 1500, 500);
        c9->Divide(3, 2);
        for (Int_t i = 0; i < 6; i++)
        {
            c9->cd(i + 1);
            fh_x_pull[i]->Draw();
        }
        c9->cd(0);

        new TCanvas("c10", "", 10, 10, 500, 500);
        fh_vz_res->Draw();

        new TCanvas("c12", "", 600, 10, 500, 500);
        fh_ncand->Draw();

        new TCanvas("c12", "", 10, 10, 500, 500);
        fh_beta_res->Draw();
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
