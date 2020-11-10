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

#include "FairLogger.h"
#include "R3BHit.h"
#include "R3BTPropagator.h"
#include "R3BTrackingParticle.h"
#include "R3BTrackingSetup.h"

#include "R3BFragmentFitterChi2S494.h"

using namespace std;

#define SPEED_OF_LIGHT 29.9792458 // cm/ns
#define Amu 0.938272

R3BTrackingParticle* gCandidate;
R3BTrackingSetup* gSetup;
R3BTPropagator* gProp;

Bool_t gEnergyLoss;

double Chi2(const double* xx)
{
    cout << " in function Chi2" << endl;
    // Bool_t result = kFALSE;
    Double_t sdev = 0.;
    Double_t x_l = 0.;
    Double_t y_l = 0.;
    Double_t prev_l = 0.;
    Double_t time = 0.;
    Double_t chi2 = 0.;
    Int_t nchi2 = 0;

    gCandidate->SetMass(xx[0]);
    gCandidate->UpdateMomentum();

    gCandidate->Reset();

    // Propagate through the setup, defined by array of detectors
    for (auto const& det : gSetup->GetArray())
    {
        if (kTarget != det->section)
        {
            /*result = */ gProp->PropagateToDetector(gCandidate, det);

            time += (gCandidate->GetLength() - prev_l) / (gCandidate->GetBeta() * SPEED_OF_LIGHT);
            prev_l = gCandidate->GetLength();
        }

        if (gEnergyLoss)
        {
            if (kTof != det->section)
            {
                Double_t weight = 1.;
                if (kTarget == det->section)
                {
                    weight = 0.5;
                }
                gCandidate->PassThroughDetector(det, weight);
            }
        }

        // Convert global track coordinates into local on the det plane
        det->GlobalToLocal(gCandidate->GetPosition(), x_l, y_l);

        R3BHit* hit =
            gSetup->GetHit(det->GetDetectorName().Data(), gCandidate->GetHitIndexByName(det->GetDetectorName().Data()));

        // X deviation at the last detector
        if (kAfterGlad == det->section)
        {
            sdev += TMath::Power(x_l - hit->GetX(), 2);
        }

        // if(kTarget != det->section)
        // if(kAfterGlad == det->section)
        {
            chi2 += TMath::Power((x_l - hit->GetX()) / det->res_x, 2);
            // LOG(INFO) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
            nchi2 += 1;
        }

        if (kTof == det->section)
        {
            chi2 += TMath::Power((time - hit->GetTime()) / det->res_t, 2);
        }
    }

    // sdev /= 2;
    // sdev = TMath::Sqrt(sdev);

    // chi2 /= nchi2;
    gCandidate->SetChi2(chi2);

    return chi2;
}

double Chi2Beta(const double* xx)
{
    // cout << " in function Chi2Beta" << endl;
    // Bool_t result = kFALSE;
    // Double_t sdev = 0.;
    Double_t x_l = 0.;
    Double_t y_l = 0.;
    // Double_t prev_l = 0.;
    // Double_t time = 0.;
    Double_t chi2 = 0.;
    Int_t nchi2 = 0;

    gCandidate->SetStartBeta(xx[0]);
    gCandidate->UpdateMomentum();

    gCandidate->Reset();

    // Propagate through the setup, defined by array of detectors
    // for (auto const& det : gSetup->GetArray())
    for (Int_t i = 0; i < (gSetup->GetArray().size() - 1); i++)
    {
        auto det = gSetup->GetArray().at(i);
        if (kTarget != det->section)
        {
            /*result = */ gProp->PropagateToDetector(gCandidate, det);

            // time += (gCandidate->GetLength() - prev_l) / (gCandidate->GetBeta() * SPEED_OF_LIGHT);
            // prev_l = gCandidate->GetLength();

            LOG(DEBUG2) << " at " << det->GetDetectorName() << ", momentum:" << gCandidate->GetMomentum().X() << ","
                        << gCandidate->GetMomentum().Y() << gCandidate->GetMomentum().Z();
        }

        if (gEnergyLoss)
        {
            Double_t weight = 1.;
            if (kTarget == det->section)
            {
                weight = 0.5;
            }
            gCandidate->PassThroughDetector(det, weight);
        }

        // Convert global track coordinates into local on the det plane
        det->GlobalToLocal(gCandidate->GetPosition(), x_l, y_l);

        R3BHit* hit =
            gSetup->GetHit(det->GetDetectorName().Data(), gCandidate->GetHitIndexByName(det->GetDetectorName().Data()));

        // if(kTarget != det->section)
        // if(kAfterGlad == det->section)
        if (det->res_x > 0.)
        {
            chi2 += TMath::Power((x_l - hit->GetX()) / det->res_x, 2);
            // LOG(INFO) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
            cout << "Testbeta: " << det->GetDetectorName().Data() << "  " << chi2 << " Position: " << x_l
                 << " now: " << hit->GetX() << ",  dev: " << (x_l - hit->GetX()) << " res: " << det->res_x << endl;
            nchi2 += 1;
        }
        /*
                if(det->res_y > 0.)
                {
                    chi2 += TMath::Power((y_l - hit->GetY()) / det->res_y, 2);
                    cout << "Calculate chi2 for y: " <<  det->GetDetectorName().Data() << "  " << chi2 <<
                    " Position: " << y_l << " now: " << hit->GetY() << ",  dev: " << (y_l - hit->GetY()) <<
                    " res: " << det->res_y << endl;
                    nchi2 += 1;
                }
        */
        //        if(kTof == det->section)
        //        {
        //            chi2 += TMath::Power((time - hit->GetTime()) / det->res_t, 2);
        //        }
    }

    // sdev /= 2;
    // sdev = TMath::Sqrt(sdev);

    // chi2 /= nchi2;
    cout << "chi2beta: " << chi2 << endl;
    gCandidate->SetChi2(chi2);

    return chi2;
}

double Chi2Backward(const double* xx)
{
    cout << " in function Chi2Backward" << endl;

    // Bool_t result = kFALSE;
    // Double_t sdev = 0.;
    Double_t x_l = 0.;
    Double_t y_l = 0.;
    // Double_t prev_l = 0.;
    // Double_t time = 0.;
    Double_t chi2 = 0.;
    Int_t nchi2 = 0;

    gCandidate->SetMass(xx[0]);
    gCandidate->UpdateMomentum();

    gCandidate->Reset();

    // Propagate through the setup, defined by array of detectors
    for (Int_t i = (gSetup->GetArray().size() - 2); i >= 0; i--)
    {
        auto det = gSetup->GetArray().at(i);

        if (i < (gSetup->GetArray().size() - 2))
        {
            /*result = */ gProp->PropagateToDetectorBackward(gCandidate, det);

            // time += (gCandidate->GetLength() - prev_l) / (gCandidate->GetBeta() * SPEED_OF_LIGHT);
            // prev_l = gCandidate->GetLength();

            LOG(DEBUG2) << " at " << det->GetDetectorName() << ", momentum:" << gCandidate->GetMomentum().X() << ", "
                        << gCandidate->GetMomentum().Y() << ", " << gCandidate->GetMomentum().Z();
        }

        if (gEnergyLoss)
        {
            Double_t weight = 1.;
            if (kTarget == det->section)
            {
                weight = 0.5;
            }
            gCandidate->PassThroughDetectorBackward(det, weight);
        }

        // Convert global track coordinates into local on the det plane
        det->GlobalToLocal(gCandidate->GetPosition(), x_l, y_l);

        R3BHit* hit =
            gSetup->GetHit(det->GetDetectorName().Data(), gCandidate->GetHitIndexByName(det->GetDetectorName().Data()));

        // if(kTarget != det->section)
        // if(kAfterGlad == det->section)
        {
            chi2 += TMath::Power((x_l - hit->GetX()) / det->res_x, 2);
            // LOG(INFO) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
            nchi2 += 1;
        }

        //        if(kTof == det->section)
        //        {
        //            chi2 += TMath::Power((time - hit->GetTime()) / det->res_t, 2);
        //        }
    }

    // sdev /= 2;
    // sdev = TMath::Sqrt(sdev);

    // chi2 /= nchi2;
    gCandidate->SetChi2(chi2);

    return chi2;
}

double Chi2Backward2D(const double* xx)
{
    // cout << "in function Chi2Backward2D" << endl;

    // Bool_t result = kFALSE;
    // Double_t sdev = 0.;
    Double_t x_l = 0.;
    Double_t y_l = 0.;
    // Double_t prev_l = 0.;
    // Double_t time = 0.;
    Double_t chi2 = 0.;
    Int_t nchi2 = 0;

    double mass = xx[0];
    // double x_fi32 = xx[1];
    // double x_fi32 = xx[2];

    gCandidate->SetMass(mass);
    gCandidate->UpdateMomentum();

    // auto fi4 = gSetup->GetByName("fi4");
    auto fi32 = gSetup->GetByName("fi32");
    auto fi30 = gSetup->GetByName("fi30");

    TVector3 pos1;
    TVector3 pos2;
    TVector3 pos3;
    // fi4->LocalToGlobal(pos1, gSetup->GetHit("fi4", gCandidate->GetHitIndexByName("fi4"))->GetX(), 0.);
    fi32->LocalToGlobal(pos2, gSetup->GetHit("fi32", gCandidate->GetHitIndexByName("fi32"))->GetX(), 0.);
    // fi30->LocalToGlobal(pos3, x_fi30, 0.);

    // fi32->LocalToGlobal(pos2, x_fi32, 0.);
    // fi30->LocalToGlobal(pos3, x_fi30, 0.);
    fi30->LocalToGlobal(pos3, gSetup->GetHit("fi30", gCandidate->GetHitIndexByName("fi30"))->GetX(), 0.);

    TVector3 direction0 = (pos2 - pos3).Unit();
    TVector3 pos0 = pos3;
    Double_t mom = gCandidate->GetMass() * gCandidate->GetStartBeta() * gCandidate->GetStartGamma();
    TVector3 startMomentum(mom * direction0.X(), mom * direction0.Y(), mom * direction0.Z());
    gCandidate->SetStartPosition(pos0);
    gCandidate->SetStartMomentum(startMomentum);
    gCandidate->Reset();

    /*
        cout << "direction: " << endl;
        direction0.Print();
        cout << "Momentum: " << mom << endl;
        startMomentum.Print();
    */
    // Propagate through the setup, defined by array of detectors
    for (Int_t i = (gSetup->GetArray().size() - 2); i >= 0; i--)
    {
        auto det = gSetup->GetArray().at(i);

        if (i < (gSetup->GetArray().size() - 2))
        {
            /*result = */ gProp->PropagateToDetectorBackward(gCandidate, det);

            // time += (gCandidate->GetLength() - prev_l) / (gCandidate->GetBeta() * SPEED_OF_LIGHT);
            // prev_l = gCandidate->GetLength();

            LOG(DEBUG2) << " at " << det->GetDetectorName() << ", momentum:" << gCandidate->GetMomentum().X() << ","
                        << gCandidate->GetMomentum().Y() << gCandidate->GetMomentum().Z();
        }

        if (gEnergyLoss)
        {
            Double_t weight = 1.;
            if (kTarget == det->section)
            {
                weight = 0.5;
            }
            gCandidate->PassThroughDetectorBackward(det, weight);
        }

        // Convert global track coordinates into local on the det plane
        det->GlobalToLocal(gCandidate->GetPosition(), x_l, y_l);
        cout << "Candidate position: " << x_l << endl;
        R3BHit* hit =
            gSetup->GetHit(det->GetDetectorName().Data(), gCandidate->GetHitIndexByName(det->GetDetectorName().Data()));

        cout << "Detector: " << det->GetDetectorName().Data() << " Position on detector: " << hit->GetX() << endl;
        // cout << "Start chi2:" << chi2 << endl;
        // if(kTarget != det->section)
        // if(kAfterGlad == det->section)
        {
            chi2 += TMath::Power((x_l - hit->GetX()) / det->res_x, 2);
            cout << "Test: " << nchi2 << "  " << chi2 << ",  dev: " << (x_l - hit->GetX()) << " res: " << det->res_x
                 << endl;
            nchi2 += 1;
        }

        //        if(kTof == det->section)
        //        {
        //            chi2 += TMath::Power((time - hit->GetTime()) / det->res_t, 2);
        //        }
    }

    // sdev /= 2;
    // sdev = TMath::Sqrt(sdev);

    // chi2 /= nchi2;
    cout << "chi2: " << chi2 << endl;
    gCandidate->SetChi2(chi2);

    return chi2;
}

R3BFragmentFitterChi2S494::R3BFragmentFitterChi2S494() {}

R3BFragmentFitterChi2S494::~R3BFragmentFitterChi2S494() {}

void R3BFragmentFitterChi2S494::Init(R3BTPropagator* prop, Bool_t energyLoss)
{

    fPropagator = prop;
    gProp = prop;
    gEnergyLoss = energyLoss;

    fMinimum = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");

    // set tolerance , etc...
    fMinimum->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2
    fMinimum->SetMaxIterations(10000);      // for GSL
    fMinimum->SetTolerance(10.);
    fMinimum->SetPrintLevel(0);
    fMinimum->SetStrategy(0);

    // create funciton wrapper for minmizer
    // a IMultiGenFunction type
    ROOT::Math::Functor* f = new ROOT::Math::Functor(&Chi2Backward2D, 2);

    fMinimum->SetFunction(*f);
}

Int_t R3BFragmentFitterChi2S494::FitTrack(R3BTrackingParticle* particle, R3BTrackingSetup* setup)
{
    cout << " in function FitTrack" << endl;
    gCandidate = particle;

    ROOT::Math::Minimizer* minimum = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");

    // set tolerance , etc...
    minimum->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2
    minimum->SetMaxIterations(10000);      // for GSL
    minimum->SetTolerance(0.0001);
    minimum->SetPrintLevel(0);

    // create funciton wrapper for minmizer
    // a IMultiGenFunction type
    ROOT::Math::Functor f(&Chi2, 1);
    double variable[1] = { particle->GetMass() };
    double step[1] = {
        0.001,
    };

    minimum->SetFunction(f);

    // Set the free variables to be minimized!
    minimum->SetLimitedVariable(0, "m", variable[0], step[0], variable[0] - 0.5, variable[0] + 0.5);

    Int_t status = 0;

    // do the minimization
    gEnergyLoss = kTRUE;

    minimum->Minimize();

    status = minimum->Status();
    if (0 != status)
    {
        return status;
    }

    particle->SetMass(minimum->X()[0]);
    particle->UpdateMomentum();

    particle->Reset();

    delete minimum;

    return status;
}

Int_t R3BFragmentFitterChi2S494::FitTrackBeta(R3BTrackingParticle* particle, R3BTrackingSetup* setup)
{
    gCandidate = particle;

    ROOT::Math::Minimizer* minimum = ROOT::Math::Factory::CreateMinimizer("Minuit2", "");

    // set tolerance , etc...
    minimum->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2
    minimum->SetMaxIterations(10000);      // for GSL
    minimum->SetTolerance(0.001);
    minimum->SetPrintLevel(0);

    // create funciton wrapper for minmizer
    // a IMultiGenFunction type
    ROOT::Math::Functor f(&Chi2Beta, 1);
    double variable[1] = { particle->GetStartBeta() };
    double step[1] = { 0.01 };

    minimum->SetFunction(f);

    // Set the free variables to be minimized!
    minimum->SetLimitedVariable(0, "beta", variable[0], step[0], 0.5, 0.999);

    Int_t status = 0;

    // do the minimization
    gEnergyLoss = kTRUE;

    minimum->Minimize();

    status = minimum->Status();

    cout << "status beta: " << status << endl;
    if (0 != status)
    {
        return status;
    }

    particle->SetStartBeta(minimum->X()[0]);
    particle->UpdateMomentum();

    particle->Reset();

    delete minimum;

    return status;
}

Int_t R3BFragmentFitterChi2S494::FitTrackBackward(R3BTrackingParticle* particle, R3BTrackingSetup* setup)
{
    // fPropagator->SetVis(kTRUE);
    // cout << "in FitTrackBackward" << endl;

    gCandidate = particle;
    gSetup = setup;

    // auto fi4 = gSetup->GetByName("fi4");
    auto fi32 = gSetup->GetByName("fi32");
    auto fi30 = gSetup->GetByName("fi30");
    // auto tof = gSetup->GetFirstByType(kTof);

    double variable[1] = { 16. * amu };
    double step[1] = { 0.01 };

    // Set the free variables to be minimized!
    fMinimum->SetLimitedVariable(0, "m", variable[0], step[0], 125. * amu, 133. * amu);

    TVector3 pos1;
    TVector3 pos2;
    TVector3 pos3;
    // fi4->LocalToGlobal(pos1, gSetup->GetHit("fi4", particle->GetHitIndexByName("fi4"))->GetX(), 0.);
    fi32->LocalToGlobal(pos2, gSetup->GetHit("fi32", particle->GetHitIndexByName("fi32"))->GetX(), 0.);
    fi30->LocalToGlobal(pos3, gSetup->GetHit("fi30", particle->GetHitIndexByName("fi30"))->GetX(), 0.);
    TVector3 direction0 = (pos2 - pos3).Unit();
    TVector3 pos0 = pos3;
    Double_t mom = gCandidate->GetMass() * gCandidate->GetStartBeta() * gCandidate->GetStartGamma();
    TVector3 startMomentum(mom * direction0.X(), mom * direction0.Y(), mom * direction0.Z());
    gCandidate->SetStartPosition(pos0);
    gCandidate->SetStartMomentum(startMomentum);
    gCandidate->Reset();

    // pos1.Print();
    // startMomentum.Print();

    for (Int_t i = 0; i <= (gSetup->GetArray().size() - 2); i++)
    {
        auto det = gSetup->GetArray().at(i);

        if (gEnergyLoss)
        {
            Double_t weight = 1.;
            if (kTarget == det->section)
            {
                weight = 0.5;
            }
            gCandidate->PassThroughDetector(det, weight); // MH Backward?
        }
    }

    // LOG(INFO) << "1 Start beta:" << gCandidate->GetStartBeta()
    //<< ",  Beta:" << gCandidate->GetBeta();

    gCandidate->SetStartBeta(gCandidate->GetBeta());
    gCandidate->SetCharge(-1. * gCandidate->GetCharge());
    gCandidate->UpdateMomentum();

    // Double_t chi2 = Chi2Backward(variable);

    // LOG(INFO) << "2 chi2 = " << chi2;

    // return 0;

    Int_t status = 0;

    // do the minimization
    fMinimum->Minimize();

    gCandidate->SetCharge(-1. * gCandidate->GetCharge());

    status = fMinimum->Status();
    if (0 != status)
    {
        return status;
    }

    particle->SetMass(fMinimum->X()[0]);
    particle->UpdateMomentum();

    fMinimum->Clear();

    // candidate->Reset();

    return status;
}

Int_t R3BFragmentFitterChi2S494::FitTrackBackward2D(R3BTrackingParticle* particle, R3BTrackingSetup* setup)
{
    // fPropagator->SetVis(kTRUE);
    // cout << "in FitTrackBackward2D" << endl;
    gCandidate = particle;
    gSetup = setup;

    // auto fi4 = gSetup->GetByName("fi4");
    auto fi32 = gSetup->GetByName("fi32");
    auto fi30 = gSetup->GetByName("fi30");
    // auto tof = gSetup->GetFirstByType(kTof);

    double variable[1] = { 16. * amu };
    double step[1] = { 0.01 };

    /*
        double variable[2] = { 16. * amu, gSetup->GetHit("fi32", particle->GetHitIndexByName("fi32"))->GetX() };
        double step[2] = { 0.01, 0.001 };
    */
    /*
        double variable[3] = { 16. * amu,
             gSetup->GetHit("fi30", particle->GetHitIndexByName("fi30"))->GetX(),
             gSetup->GetHit("fi32", particle->GetHitIndexByName("fi32"))->GetX()};
        double step[3] = { 0.01, 0.001, 0.001 };
    */
    // Set the free variables to be minimized!
    fMinimum->SetLimitedVariable(0, "m", variable[0], step[0], 1. * amu, 50. * amu);
    // fMinimum->SetLimitedVariable(1, "xfi30", variable[1], step[1], -1000., 1000.);
    // fMinimum->SetLimitedVariable(2, "xfi32", variable[2], step[2], -1000., 1000.);

    TVector3 pos1;
    TVector3 pos2;
    TVector3 pos3;
    // fi4->LocalToGlobal(pos1, gSetup->GetHit("fi4", particle->GetHitIndexByName("fi4"))->GetX(), 0.);
    fi32->LocalToGlobal(pos2, gSetup->GetHit("fi32", particle->GetHitIndexByName("fi32"))->GetX(), 0.);
    fi30->LocalToGlobal(pos3, gSetup->GetHit("fi30", particle->GetHitIndexByName("fi30"))->GetX(), 0.);
    TVector3 direction0 = (pos2 - pos3).Unit();
    TVector3 pos0 = pos3;
    Double_t mom = gCandidate->GetMass() * gCandidate->GetStartBeta() * gCandidate->GetStartGamma();
    TVector3 startMomentum(mom * direction0.X(), mom * direction0.Y(), mom * direction0.Z());
    gCandidate->SetStartPosition(pos0);
    gCandidate->SetStartMomentum(startMomentum);
    gCandidate->Reset();

    for (Int_t i = 0; i <= (gSetup->GetArray().size() - 2); i++)
    {
        auto det = gSetup->GetArray().at(i);

        if (gEnergyLoss)
        {
            Double_t weight = 1.;
            if (kTarget == det->section)
            {
                weight = 0.5;
            }
            gCandidate->PassThroughDetector(det, weight);
        }
    }

    // LOG(INFO) << "1 Start beta:" << gCandidate->GetStartBeta()
    //<< ",  Beta:" << gCandidate->GetBeta();

    gCandidate->SetStartBeta(gCandidate->GetBeta());
    gCandidate->SetCharge(-1. * gCandidate->GetCharge());
    gCandidate->UpdateMomentum();

    // Double_t chi2 = Chi2Backward(variable);

    // LOG(INFO) << "2 chi2 = " << chi2;

    // return 0;

    Int_t status = 0;

    // do the minimization
    fMinimum->Minimize();

    gCandidate->SetCharge(-1. * gCandidate->GetCharge());

    status = fMinimum->Status();
    cout << "Status: " << status << endl;
    if (0 != status)
    {
        return status;
    }

    particle->SetMass(fMinimum->X()[0]);
    particle->UpdateMomentum();

    fMinimum->Clear();

    // candidate->Reset();

    return status;
}

/*
 * This function propagates the particle from its starting point to all
 * defined R3BTrackingDetectors.
 *
 * All required information is in fArrayDetectors and in the parameters
 */
Double_t R3BFragmentFitterChi2S494::TrackFragment(R3BTrackingParticle* particle,
                                                  Bool_t energyLoss,
                                                  Double_t& devTof,
                                                  Double_t& time,
                                                  Double_t& chi2)
{
    /*    Bool_t result = kFALSE;
        Double_t sdev = 0.;
        Double_t x_l = 0.;
        Double_t y_l = 0.;
        devTof = 0.;
        time = 0.;
        chi2 = 0.;
        Double_t prev_l = 0.;

        // Propagate through the setup, defined by array of detectors
        for (auto const& det : fDetectors->GetArray())
        {
            LOG(DEBUG2);
            LOG(DEBUG2) << "Propagating to z=" << det->pos0.Z();

            if(kTarget != det->section)
            {
                result = fPropagator->PropagateToDetector(particle, det);

                time += (particle->GetLength() - prev_l) / (particle->GetBeta() * SPEED_OF_LIGHT);
                prev_l = particle->GetLength();
            }

            if (energyLoss && kTof != det->section)
            {
                Double_t weight = 1.;
                if(kTarget == det->section)
                {
                    weight = 0.5;
                }
                particle->PassThroughDetector(det, weight);
            }

            if (det->GetDetectorName().EqualTo("psp"))
            { // PSP
                Double_t eloss = det->GetEnergyLoss(particle);
                fh_eloss_psp->Fill(eloss);
            }

            if (det->GetDetectorName().EqualTo("fi4"))
            {                                                        // Fi4
                Double_t eloss = 0.5 * det->GetEnergyLoss(particle); // 0.5 because of 2 layers !!!
                fh_eloss_fi4->Fill(eloss);
            }

            // Convert global track coordinates into local on the det plane
            det->GlobalToLocal(particle->GetPosition(), x_l, y_l);

            R3BHit* hit = fDetectors->GetHit(det->GetDetectorName().Data(),
       particle->GetHitIndexByName(det->GetDetectorName().Data()));

            // X deviation at the last detector
            if (kAfterGlad == det->section)
            {
                sdev += x_l - hit->GetX();
            }

            // X deviation at the last detector
            if (kTof == det->section)
            {
                devTof = x_l - hit->GetX();
            }

            chi2 += TMath::Power((x_l - hit->GetX()) / det->res_x, 2);

            LOG(DEBUG2) << "Track length " << particle->GetLength() << " cm";
        }

        sdev /= 2;

        chi2 /= (fDetectors->GetArray().size() - 2);

        LOG(DEBUG2) << "Deviation at TOF wall : " << sdev;
        LOG(DEBUG2) << "                 chi2 : " << chi2;
        LOG(DEBUG2) << "                 time : " << time;
        LOG(DEBUG2) << "Track length " << particle->GetLength() << " cm";

        return sdev;*/
    return 0.;
}

/**
 * Calculate velocity from track length and time difference between target
 * and TOF wall.
 * !!! NOTE !!! : there is a systematic 0.5 cm offset in track length as compared
 * to Monte Carlo values. Check the placement of TOF plane in the parameter
 * container.
 */
Double_t R3BFragmentFitterChi2S494::Velocity(R3BTrackingParticle* candidate)
{
    /*    fPropagator->SetVis(kFALSE);
        R3BTrackingDetector* tof = fDetectors->GetByName("tofd");

        fPropagator->PropagateToDetector(candidate, tof);
        // TrackFragment(candidate);
        Double_t beta = candidate->GetLength() / (fDetectors->GetHit("tofd",
       candidate->GetHitIndexByName("tofd"))->GetTime() - fDetectors->GetHit("target",
       candidate->GetHitIndexByName("target"))->GetTime()) / SPEED_OF_LIGHT; LOG(INFO) << "Velocity estimation (TOF) "
       << beta << "   length = " << candidate->GetLength();

        return beta;*/
    return 0.;
}

/**
 * Extrapolate two times with dbeta = 0.01 Calculate dx from that.
 * Return dbeta/dx.
 */
Double_t R3BFragmentFitterChi2S494::DbetaDx(R3BTrackingParticle* candidate)
{
    fPropagator->SetVis(kFALSE);
    Double_t devTof;
    Double_t time;
    Double_t chi2;
    Double_t beta1 = candidate->GetStartBeta();
    LOG(DEBUG1) << "beta1 = " << beta1;
    Double_t dev1 = TrackFragment(candidate, kTRUE, devTof, time, chi2);

    Double_t dbeta = 0.001;
    Double_t beta2 = beta1 + dbeta;
    LOG(DEBUG1) << "beta2 = " << beta2;

    candidate->SetStartBeta(beta2);
    candidate->UpdateMomentum();
    candidate->Reset();

    Double_t dev2 = TrackFragment(candidate, kTRUE, devTof, time, chi2);

    Double_t dbetadx = dbeta / (dev2 - dev1);

    LOG(DEBUG1) << "dbeta/dx = " << dbetadx << " 1/cm";

    return dbetadx;
}

Double_t R3BFragmentFitterChi2S494::DbetaChi2(R3BTrackingParticle* candidate)
{
    fPropagator->SetVis(kFALSE);

    Double_t devTof;
    Double_t time;
    Double_t chi2_1, chi2_2;
    Double_t beta1 = candidate->GetStartBeta();
    LOG(DEBUG1) << "beta1 = " << beta1;
    TrackFragment(candidate, kTRUE, devTof, time, chi2_1);

    Double_t dbeta = 0.001;
    Double_t beta2 = beta1 + dbeta;
    LOG(DEBUG1) << "beta2 = " << beta2;

    candidate->SetStartBeta(beta2);
    candidate->UpdateMomentum();
    candidate->Reset();

    TrackFragment(candidate, kTRUE, devTof, time, chi2_2);

    Double_t dbetachi2 = dbeta / (chi2_2 - chi2_1);

    LOG(DEBUG1) << "dbeta/dchi2 = " << dbetachi2 << " 1/cm";

    return dbetachi2;
}

Double_t R3BFragmentFitterChi2S494::DbetaDt(R3BTrackingParticle* candidate)
{
    fPropagator->SetVis(kFALSE);

    Double_t devTof;
    Double_t time1, time2;
    Double_t chi2;
    Double_t beta1 = candidate->GetStartBeta();
    LOG(DEBUG1) << "beta1 = " << beta1;
    TrackFragment(candidate, kTRUE, devTof, time1, chi2);

    Double_t dbeta = 0.001;
    Double_t beta2 = beta1 + dbeta;
    LOG(DEBUG1) << "beta2 = " << beta2;

    candidate->SetStartBeta(beta2);
    candidate->UpdateMomentum();
    candidate->Reset();

    TrackFragment(candidate, kTRUE, devTof, time2, chi2);

    Double_t dbetadt = dbeta / (time2 - time1);

    LOG(DEBUG1) << "dbeta/dt = " << dbetadt << " 1/ns";

    return dbetadt;
}

/**
 * Extrapolate two times with dm = 1. Calculate dx from that. Return dm/dx.
 */
Double_t R3BFragmentFitterChi2S494::DmDx(R3BTrackingParticle* candidate, Bool_t energy_loss)
{
    fPropagator->SetVis(kFALSE);

    Double_t devTof;
    Double_t time;
    Double_t chi2;
    Double_t m1 = candidate->GetMass();
    LOG(DEBUG1) << "m1 = " << m1;
    Double_t dev1 = TrackFragment(candidate, energy_loss, devTof, time, chi2);

    Double_t dm = 0.01;
    Double_t m2 = m1 + dm;
    LOG(DEBUG1) << "m2 = " << m2;

    candidate->SetMass(m2);
    candidate->UpdateMomentum();
    candidate->Reset();

    Double_t dev2 = TrackFragment(candidate, energy_loss, devTof, time, chi2);

    Double_t dmdx = dm / (dev2 - dev1);

    LOG(DEBUG1) << "dm/dx = " << dmdx << " GeV/cm";

    return dmdx;
}

/**
 * Extrapolate two times with dm = 1. Calculate dx from that. Return dm/dx.
 */
Double_t R3BFragmentFitterChi2S494::DmDxTof(R3BTrackingParticle* candidate, Bool_t energy_loss)
{
    fPropagator->SetVis(kFALSE);

    Double_t devTof1, devTof2;
    Double_t time;
    Double_t chi2;
    Double_t m1 = candidate->GetMass();
    LOG(DEBUG1) << "m1 = " << m1;
    TrackFragment(candidate, energy_loss, devTof1, time, chi2);

    Double_t dm = 0.5;
    Double_t m2 = m1 + dm;
    LOG(DEBUG1) << "m2 = " << m2;

    candidate->SetMass(m2);
    candidate->UpdateMomentum();
    candidate->Reset();

    TrackFragment(candidate, energy_loss, devTof2, time, chi2);

    Double_t dmdx = dm / (devTof2 - devTof1);

    LOG(DEBUG1) << "dm/dx = " << dmdx << " GeV/cm";

    return dmdx;
}

/**
 * Extrapolate two times with dm = 1. Calculate dt from that. Return dm/dt.
 */
Double_t R3BFragmentFitterChi2S494::DmDt(R3BTrackingParticle* candidate, Bool_t energy_loss)
{
    fPropagator->SetVis(kFALSE);

    Double_t devTof;
    Double_t time1;
    Double_t time2;
    Double_t chi2;
    Double_t m1 = candidate->GetMass();
    LOG(DEBUG1) << "m1 = " << m1;
    TrackFragment(candidate, energy_loss, devTof, time1, chi2);

    Double_t dm = 0.5;
    Double_t m2 = m1 + dm;
    LOG(DEBUG1) << "m2 = " << m2;

    candidate->SetMass(m2);
    candidate->UpdateMomentum();
    candidate->Reset();

    TrackFragment(candidate, energy_loss, devTof, time2, chi2);

    Double_t dmdt = dm / (time2 - time1);

    LOG(DEBUG1) << "dm/dt = " << dmdt << " GeV/ns";

    return dmdt;
}

/* This function modifies the particle properties such that the track
 * fits the measured hits best.
 *
 * All required information is in fArrayDetectors and in the parameters.
 * Note: momin is only a first guess which will be recalculated while
 * fitting
 *//*
void R3BFragmentTracker::FitFragment(R3BTrackingParticle* candidate)
{
    Int_t maxIter = 100;

    // Todo:
    // Initial start parameters for the fragment:
    //   position  = measured target position
    //   direction = vector from pos on target to pos on PSP
    //   velocity  = a mean value, valid at center of target
    //               (considering the energy loss of half of the target
    //                thickness, provided by the user)
    //   charge    = as provided by the user
    //   mass      = as provided by the user
    //
    // For now, we have only three points (target, PSP, TofWall).
    // We keep fixed (for now):
    // - start position and
    // - start direction of the particle
    // - charge
    //
    // So we need to vary mass and (if needed) velocity. Note that we
    // cannot combine mass and velocity into a single momentum parameter
    // of the particle because we need the velocity to calculate the
    // energy loss at each detector (later).
    //
    // Ideally we would pre-calculated dm/dx which means:
    // how far moves the beam on the tof wall if the mass is changed?
    // or viceversa: "how much do we need to change the mass if we are
    // x centimeters off on the tof wall?".
    // One could also calculate dx/dbeta. Combination with dm/dx leads
    // to dm/dbeta.
    //
    // In the old tracker, I used only dm/dx. I had a rough estimate for
    // dm/dx hard coded in the code. Then after the second iteration
    // (after the particle was tracked twice with different m0), I
    // recalculated dm/dx using the actual data.

    Double_t sdev = 0.;
    Double_t devTof;
    Double_t time;
    Double_t chi2;
    Double_t beta = candidate->GetStartBeta();
    Double_t mass = candidate->GetMass();
    Double_t dbetadt;
    Double_t dbetadx;
    Double_t dmdx;

    Bool_t stopHere = kFALSE;
    
    fAfterGladResolution = fDetectors->GetAfterGladResolution();
    
    R3BTrackingDetector* tof = fDetectors->GetFirstByType(kTof);

    for (Int_t i = 0; i < maxIter; i++)
    {
        LOG(DEBUG1) << "Starting iteration " << (i + 1) << "   mass = " << mass << "   beta = " << beta
                   ;

        // Always start an iteration from the target
        candidate->Reset();

        //fPropagator->SetVis(kTRUE);

        // Calculate deviation of track from a measured TOF hit
        sdev = TrackFragment(candidate, kTRUE, devTof, time, chi2);

        LOG(DEBUG1) << sdev << "  " << (time - tof->hit_time) << "  chi2=" << chi2;

        stopHere = kTRUE;

        candidate->Reset();

        // Calculate dm/dx by changing mass by 1 GeV and measuring deviation of
        // track on the TOF wall
        //if(0 == i || TMath::Abs(time - tof->hit_time) > 0.5)
//        {
//            dbetadt = DbetaDt(candidate);
//            // Reset the mass to original value, reset the track
//            candidate->SetStartBeta(beta);
//            candidate->UpdateMomentum();
//            candidate->Reset();
//        }

        // Calculate dm/dx by changing mass by 1 GeV and measuring deviation of
        // track on the TOF wall
        //if(0 == i || TMath::Abs(sdev) > 1.)
        {
            dmdx = DmDx(candidate, kTRUE);
            // Reset the mass to original value, reset the track
            candidate->SetMass(mass);
            candidate->UpdateMomentum();
            candidate->Reset();
        }

//        if (TMath::Abs(time - tof->hit_time) > tof->res_t)
//        {
//            // Correct the mass to achieve better extrapolation to hit
//            Double_t dbeta = dbetadt * (time - tof->hit_time);
//            beta -= dbeta;
//            // Set the mass and recalculate momentum
//            candidate->SetStartBeta(beta);
//            candidate->UpdateMomentum();
//            stopHere = kFALSE;
//        }
        if (TMath::Abs(sdev) > fAfterGladResolution)
        {
            // Correct the mass to achieve better extrapolation to hit
            mass -= dmdx * sdev;
            // Set the mass and recalculate momentum
            candidate->SetMass(mass);
            candidate->UpdateMomentum();
            stopHere = kFALSE;
        }

        if (stopHere)
        {
            break;
        }
    }

    candidate->Reset();
    candidate->SetChi2(chi2);

    fh_A_reco1->Fill(candidate->GetMass() / 0.9314940954);
}
*/

ClassImp(R3BFragmentFitterChi2S494)
