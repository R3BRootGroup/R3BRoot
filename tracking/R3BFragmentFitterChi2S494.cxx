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
    cout << "IN CHI2 FUNCTION" << endl;
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
    gProp->SetVis(kFALSE);

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
            Double_t chi2temp;

            if (TMath::Abs(x_l - hit->GetX()) < det->res_x)
            {
                chi2temp = 0.;
            }
            else
            {
                chi2temp = TMath::Power((x_l - hit->GetX()) / det->res_x, 2);
            }
            chi2 += chi2temp;

            // chi2 += TMath::Power((x_l - hit->GetX()) / det->res_x, 2);

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

double Chi2MomentumForward(const double* xx)
{

    /*
        auto fi31 = gSetup->GetByName("fi31");
        TVector3 fib31(-111.1, 0., 537.8);
        Double_t xxx, yyy;
        fi31->GlobalToLocal(fib31, xxx, yyy);
        cout << "***************** " << xxx << "  " << yyy << endl;

        TVector3 out;
        fi31->LocalToGlobal(out, -21.40, 0.);
        cout<<"++++++++++++++++++ " << out.X() << "  " << out.Y() << "  " << out.Z() << endl;
    */

    LOG(DEBUG3) << "In chi2 forward" << endl;
    LOG(DEBUG3) << "current momentum: " << gCandidate->GetMomentum().X() << "  " << gCandidate->GetMomentum().Y()
                << "  " << gCandidate->GetMomentum().Z() << endl;
    LOG(DEBUG3) << "current position: " << gCandidate->GetPosition().X() << "  " << gCandidate->GetPosition().Y()
                << "  " << gCandidate->GetPosition().Z() << endl;

    Double_t x_l = 0.;
    Double_t y_l = 0.;
    Double_t chi2 = 0.;
    Int_t nchi2 = 0;

    Double_t px0 = xx[0];
    Double_t py0 = xx[1];
    Double_t pz0 = xx[2];

    double x0 = xx[3];
    double y0 = xx[4];
    double z0 = xx[5];
    //	double x0 = 0.;
    //	double y0 = 0.;
    //	double z0 = 0.;

    // px is given by the position of fib23a and should not be optimized:
    TVector3 pos23a;
    if (gCandidate->GetHitIndexByName("fi23a") > -1)
    {
        auto fi23a = gSetup->GetByName("fi23a");
        fi23a->LocalToGlobal(pos23a, gSetup->GetHit("fi23a", gCandidate->GetHitIndexByName("fi23a"))->GetX(), 0.);
        px0 = (pos23a.X() - x0) / pos23a.Z() * pz0;

        //	cout << "fib23a x: " << pos23a.X()<< "fib23a z: " <<pos23a.Z()  << " x0: " << x0 << " px0: " << px0 << "
        //pz0: "<<pz0<<endl;
    }

    // py is given by the position of fib23b and should not be optimized:
    TVector3 pos23b;
    if (gCandidate->GetHitIndexByName("fi23b") > -1)
    {
        auto fi23b = gSetup->GetByName("fi23b");
        fi23b->LocalToGlobal(pos23b, 0., gSetup->GetHit("fi23b", gCandidate->GetHitIndexByName("fi23b"))->GetY());
        py0 = (pos23b.Y() - y0) / pos23b.Z() * pz0;

        //	cout << "fib23b y: " << pos23b.Y()<< "fib23b z: " <<pos23b.Z()<< " y0: " << y0 << " py0: " << py0<< " pz0:
        //"<<pz0 << endl;
    }

    LOG(DEBUG3) << "Variables momentum: " << px0 << "  " << py0 << "  " << pz0 << endl;
    LOG(DEBUG3) << "Variables position: " << x0 << "  " << y0 << "  " << z0 << endl;

    TVector3 startPosition(x0, y0, z0);
    TVector3 startMomentum(px0, py0, pz0);

    gCandidate->SetStartPosition(startPosition);
    gCandidate->SetStartMomentum(startMomentum);
    // gCandidate->UpdateMomentum();
    gCandidate->Reset();

    LOG(DEBUG3) << "Nach setzen der Startwerte" << endl;
    LOG(DEBUG3) << "current momentum: " << gCandidate->GetMomentum().X() << "  " << gCandidate->GetMomentum().Y()
                << "  " << gCandidate->GetMomentum().Z() << endl;
    LOG(DEBUG3) << "*** Current lab position: " << gCandidate->GetPosition().X() << "  "
                << gCandidate->GetPosition().Y() << "  " << gCandidate->GetPosition().Z() << endl;

    // Propagate forward through the setup, defined by array of detectors
    for (Int_t i = 0; i < (gSetup->GetArray().size() - 0); i++)
    {
        auto det = gSetup->GetArray().at(i);

        LOG(DEBUG3) << "***** At detector ****** " << det->GetDetectorName() << endl;
        if (i > 0)
        {
            gProp->PropagateToDetector(gCandidate, det);

            // time += (gCandidate->GetLength() - prev_l) / (gCandidate->GetBeta() * SPEED_OF_LIGHT);
            // prev_l = gCandidate->GetLength();

            LOG(DEBUG3) << " At " << det->GetDetectorName() << ", momentum:" << gCandidate->GetMomentum().X() << ", "
                        << gCandidate->GetMomentum().Y() << ", " << gCandidate->GetMomentum().Z();
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

        //  cout<<"In fragment_fitter_chi2 Global2Local: "<<endl;
        //  cout<<det->GetDetectorName().Data()<<", "<<gCandidate->GetPosition().X()<<",
        //  "<<gCandidate->GetPosition().Y()<<
        //  ", "<<gCandidate->GetPosition().Z()<<
        //  ", "<<x_l<<", "<<y_l<<endl;

        // TVector3 out;
        // det->LocalToGlobal(out, x_l, y_l);
        // cout<<"In fragment_fitter_chi2 LocalToGlobal: "<<endl;
        // cout<< out.X() << "  " << out.Y() << "  " << out.Z() << ", "<<x_l<<", "<<y_l<<endl;

        R3BHit* hit = nullptr;
        Int_t hitIndex = gCandidate->GetHitIndexByName(det->GetDetectorName().Data());

        // cout << "Hit index " << hitIndex << endl;
        if (-1 != hitIndex)
        {
            hit = gSetup->GetHit(det->GetDetectorName().Data(), hitIndex);
        }
        // Take into chi2 only if there is a hit and user specified SigmaX > 0.
        Double_t chi2temp;
        if (hit && det->res_x > 1e-6)
        {
            chi2temp = TMath::Power((x_l - hit->GetX()) / det->res_x, 2);
            if (TMath::Abs(x_l - hit->GetX()) < det->res_x)
            {
                chi2temp = 0.;
            }
            chi2 += chi2temp;

            // chi2 += TMath::Power((x_l - hit->GetX()) / det->res_x, 2);
            // LOG(INFO) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
            LOG(DEBUG3) << "chi2calc x: " << det->GetDetectorName().Data() << " res: " << det->res_x << " pos: " << x_l
                        << " hit: " << hit->GetX() << " dev: " << x_l - hit->GetX() << " chi2: " << chi2temp << endl;

            nchi2 += 1;
        }
        if (hit && det->res_y > 1e-6)
        {
            chi2temp = TMath::Power((y_l - hit->GetY()) / det->res_y, 2);
            if (TMath::Abs(y_l - hit->GetY()) < det->res_y &&
                (det->GetDetectorName() == "fi23b" || kTarget == det->section))
            {
                chi2temp = 0.;
            }
            chi2 += chi2temp;

            // chi2 += TMath::Power((y_l - hit->GetY()) / det->res_y, 2);

            // LOG(INFO) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
            LOG(DEBUG3) << "chi2calc y: " << det->GetDetectorName().Data() << " res: " << det->res_y << " pos: " << y_l
                        << " hit: " << hit->GetY() << " dev: " << y_l - hit->GetY() << " chi2: " << chi2temp << endl;

            nchi2 += 1;
        }
    }

    gCandidate->SetChi2(chi2);
    LOG(DEBUG3) << "Ende chi2" << endl;
    LOG(DEBUG3) << "current momentum: " << gCandidate->GetMomentum().X() << "  " << gCandidate->GetMomentum().Y()
                << "  " << gCandidate->GetMomentum().Z() << endl;
    LOG(DEBUG3) << "current position: " << gCandidate->GetPosition().X() << "  " << gCandidate->GetPosition().Y()
                << "  " << gCandidate->GetPosition().Z() << endl;
    LOG(DEBUG3) << "Ende chi2" << endl;
    LOG(DEBUG3) << "current momentum: " << gCandidate->GetMomentum().X() << "  " << gCandidate->GetMomentum().Y()
                << "  " << gCandidate->GetMomentum().Z() << endl;
    LOG(DEBUG3) << "current position: " << gCandidate->GetPosition().X() << "  " << gCandidate->GetPosition().Y()
                << "  " << gCandidate->GetPosition().Z() << endl;

    return chi2;
}

double Chi2MomentumBackward(const double* xx)
{
    LOG(DEBUG3) << "In chi2 backward" << endl;
    LOG(DEBUG3) << "current momentum: " << gCandidate->GetMomentum().X() << "  " << gCandidate->GetMomentum().Y()
                << "  " << gCandidate->GetMomentum().Z() << endl;
    LOG(DEBUG3) << "current position: " << gCandidate->GetPosition().X() << "  " << gCandidate->GetPosition().Y()
                << "  " << gCandidate->GetPosition().Z() << endl;

    Double_t x_l = 0.;
    Double_t y_l = 0.;
    Double_t chi2 = 0.;
    Int_t nchi2 = 0;
    TVector3 pos2;
    TVector3 pos3;
    // LOG(DEBUG3) << "Test: " << gCandidate->GetHitIndexByName("fi12") << "  " << gCandidate->GetHitIndexByName("fi10")
    // << endl;
    /*
        if (gCandidate->GetHitIndexByName("fi30") > -1)
        {
            auto fi30 = gSetup->GetByName("fi30");
            fi30->LocalToGlobal(pos3, gSetup->GetHit("fi30", gCandidate->GetHitIndexByName("fi30"))->GetX(), 0.);
        }
        else if (gCandidate->GetHitIndexByName("fi31") > -1 && gCandidate->GetHitIndexByName("fi33") > -1)
        {
            auto fi33 = gSetup->GetByName("fi33");
            fi33->LocalToGlobal(pos3, gSetup->GetHit("fi33", gCandidate->GetHitIndexByName("fi33"))->GetX(), 0.);
        }
        else
        {
            return 10;
        }
    */
    Double_t px0 = xx[0];
    Double_t py0 = xx[1];
    Double_t pz0 = xx[2];

    double x0 = xx[3];
    double y0 = xx[4];
    double z0 = xx[5];

    LOG(DEBUG3) << "Variables momentum: " << px0 << "  " << py0 << "  " << pz0 << endl;
    LOG(DEBUG3) << "Variables position: " << x0 << "  " << y0 << "  " << z0 << endl;

    TVector3 startPosition(x0, y0, z0);
    TVector3 startMomentum(px0, py0, pz0);

    gCandidate->SetPosition(startPosition);
    gCandidate->SetMomentum(startMomentum);
    // gCandidate->UpdateMomentum();
    // gCandidate->Reset();

    LOG(DEBUG3) << "Nach setzen von Werten" << endl;
    LOG(DEBUG3) << "current momentum: " << gCandidate->GetMomentum().X() << "  " << gCandidate->GetMomentum().Y()
                << "  " << gCandidate->GetMomentum().Z() << endl;
    LOG(DEBUG3) << "current position: " << gCandidate->GetPosition().X() << "  " << gCandidate->GetPosition().Y()
                << "  " << gCandidate->GetPosition().Z() << endl;

    /*
        LOG(DEBUG3) << "Test momentum: " << gCandidate->GetMomentum().X() << ", "
            << gCandidate->GetMomentum().Y() << ", " << gCandidate->GetMomentum().Z() << endl;
        LOG(DEBUG3) << "Test position: " << gCandidate->GetPosition().X() << ", "
            << gCandidate->GetPosition().Y() << ", " << gCandidate->GetPosition().Z() << endl;
        LOG(DEBUG3) << "Test charge: " << gCandidate->GetCharge() << endl;
    */

    // Propagate backward through the setup, defined by array of detectors
    for (Int_t i = (gSetup->GetArray().size() - 1); i >= 0; i--)
    {
        auto det = gSetup->GetArray().at(i);

        if (i < (gSetup->GetArray().size() - 1))
        {
            gProp->PropagateToDetectorBackward(gCandidate, det);

            // time += (gCandidate->GetLength() - prev_l) / (gCandidate->GetBeta() * SPEED_OF_LIGHT);
            // prev_l = gCandidate->GetLength();

            LOG(DEBUG3) << " at " << det->GetDetectorName() << ", momentum:" << gCandidate->GetMomentum().X() << ", "
                        << gCandidate->GetMomentum().Y() << ", " << gCandidate->GetMomentum().Z();

            LOG(DEBUG3) << "Vor Energieverlust" << endl;
            LOG(DEBUG3) << "current momentum: " << gCandidate->GetMomentum().X() << "  "
                        << gCandidate->GetMomentum().Y() << "  " << gCandidate->GetMomentum().Z() << endl;
            LOG(DEBUG3) << "current position: " << gCandidate->GetPosition().X() << "  "
                        << gCandidate->GetPosition().Y() << "  " << gCandidate->GetPosition().Z() << endl;

            if (gEnergyLoss)
            {
                Double_t weight = 1.;
                if (kTarget == det->section)
                {
                    weight = 0.5;
                }
                gCandidate->PassThroughDetectorBackward(det, weight);
            }
            LOG(DEBUG3) << "Nach Energieverlust" << endl;
            LOG(DEBUG3) << "current momentum: " << gCandidate->GetMomentum().X() << "  "
                        << gCandidate->GetMomentum().Y() << "  " << gCandidate->GetMomentum().Z() << endl;
            LOG(DEBUG3) << "current position: " << gCandidate->GetPosition().X() << "  "
                        << gCandidate->GetPosition().Y() << "  " << gCandidate->GetPosition().Z() << endl;
        }

        // Convert global track coordinates into local on the det plane
        det->GlobalToLocal(gCandidate->GetPosition(), x_l, y_l);

        R3BHit* hit = nullptr;
        Int_t hitIndex = gCandidate->GetHitIndexByName(det->GetDetectorName().Data());
        if (-1 != hitIndex)
            hit = gSetup->GetHit(det->GetDetectorName().Data(), hitIndex);

        // Take into chi2 only if there is a hit and user specified SigmaX > 0.
        Double_t chi2temp;
        if (hit && det->res_x > 1e-6)

        {
            if (TMath::Abs(x_l - hit->GetX()) < det->res_x)
            {
                chi2temp = 0.;
            }
            else
            {
                chi2temp = TMath::Power((x_l - hit->GetX()) / det->res_x, 2);
            }
            chi2 += chi2temp;

            // chi2 += TMath::Power((x_l - hit->GetX()) / det->res_x, 2);
            // LOG(INFO) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
            LOG(DEBUG3) << "chi2calc x: " << det->GetDetectorName().Data() << " res: " << det->res_x << " pos: " << x_l
                        << " hit: " << hit->GetX() << " dev: " << x_l - hit->GetX() << " chi2: " << chi2;

            nchi2 += 1;
        }
        if (hit && det->res_y > 1e-6)
        {

            if (TMath::Abs(y_l - hit->GetY()) < det->res_y)
            {
                chi2temp = 0.;
            }
            else
            {
                chi2temp = TMath::Power((y_l - hit->GetY()) / det->res_y, 2);
            }
            chi2 += chi2temp;

            // chi2 += TMath::Power((y_l - hit->GetY()) / det->res_y, 2);
            // LOG(INFO) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
            LOG(DEBUG3) << "chi2calc y: " << det->GetDetectorName().Data() << " res: " << det->res_y << " pos: " << y_l
                        << " hit: " << hit->GetY() << " dev: " << y_l - hit->GetY() << " chi2: " << chi2;

            nchi2 += 1;
        }
    }
    LOG(DEBUG3) << "Ende chi2" << endl;
    LOG(DEBUG3) << "current momentum: " << gCandidate->GetMomentum().X() << "  " << gCandidate->GetMomentum().Y()
                << "  " << gCandidate->GetMomentum().Z() << endl;
    LOG(DEBUG3) << "current position: " << gCandidate->GetPosition().X() << "  " << gCandidate->GetPosition().Y()
                << "  " << gCandidate->GetPosition().Z() << endl;

    gCandidate->SetChi2(chi2);

    return chi2;
}

double Chi2Beta(const double* xx)
{
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
    for (auto const& det : gSetup->GetArray())
    {
        if (kTarget != det->section)
        {
            /*result = */ gProp->PropagateToDetector(gCandidate, det);

            // time += (gCandidate->GetLength() - prev_l) / (gCandidate->GetBeta() * SPEED_OF_LIGHT);
            // prev_l = gCandidate->GetLength();

            LOG(DEBUG3) << " at " << det->GetDetectorName() << ", momentum:" << gCandidate->GetMomentum().X() << ","
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

        R3BHit* hit = nullptr;
        Int_t hitIndex = gCandidate->GetHitIndexByName(det->GetDetectorName().Data());
        if (hitIndex >= 0)
            hit = gSetup->GetHit(det->GetDetectorName().Data(), hitIndex);

        // if(kTarget != det->section)
        // if(kAfterGlad == det->section)
        if (hit && det->res_x > 1e-6)
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

double Chi2Backward(const double* xx)
{
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

            LOG(DEBUG3) << " at " << det->GetDetectorName() << ", momentum:" << gCandidate->GetMomentum().X() << ", "
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
        // {
        //     chi2 += TMath::Power((x_l - hit->GetX()) / det->res_x, 2);
        // LOG(INFO) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
        //    nchi2 += 1;
        //}
        Double_t chi2temp;

        if (TMath::Abs(x_l - hit->GetX()) < det->res_x)
        {
            chi2temp = 0.;
        }
        else
        {
            chi2temp = TMath::Power((x_l - hit->GetX()) / det->res_x, 2);
        }
        chi2 += chi2temp;
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
    // Require fi30 and fi32  or fi31 and fi33 to have hit
    // for the initial position and direction

    TVector3 pos2;
    TVector3 pos3;

    if (gCandidate->GetHitIndexByName("fi30") > -1 && gCandidate->GetHitIndexByName("fi32") > -1)
    {
        auto fi30 = gSetup->GetByName("fi30");
        auto fi32 = gSetup->GetByName("fi32");
        fi30->LocalToGlobal(pos2,
                            gSetup->GetHit("fi30", gCandidate->GetHitIndexByName("fi30"))->GetX(),
                            gSetup->GetHit("fi30", gCandidate->GetHitIndexByName("fi30"))->GetY());
        fi32->LocalToGlobal(pos3,
                            gSetup->GetHit("fi32", gCandidate->GetHitIndexByName("fi32"))->GetX(),
                            gSetup->GetHit("fi32", gCandidate->GetHitIndexByName("fi32"))->GetY());
    }
    else if (gCandidate->GetHitIndexByName("fi31") > -1 && gCandidate->GetHitIndexByName("fi33") > -1)
    {
        auto fi31 = gSetup->GetByName("fi31");
        auto fi33 = gSetup->GetByName("fi33");
        fi31->LocalToGlobal(pos2,
                            gSetup->GetHit("fi31", gCandidate->GetHitIndexByName("fi31"))->GetX(),
                            gSetup->GetHit("fi31", gCandidate->GetHitIndexByName("fi31"))->GetY());
        fi33->LocalToGlobal(pos3,
                            gSetup->GetHit("fi33", gCandidate->GetHitIndexByName("fi33"))->GetX(),
                            gSetup->GetHit("fi33", gCandidate->GetHitIndexByName("fi33"))->GetY());
    }
    else
    {
        return 1e10;
    }

    Double_t x_l = 0.;
    Double_t y_l = 0.;
    Double_t chi2 = 0.;
    Int_t nchi2 = 0;

    double mass = xx[0];

    // cout << "Set mass: " << mass << endl;
    gCandidate->SetMass(mass);
    gCandidate->UpdateMomentum();

    TVector3 direction0 = (pos2 - pos3).Unit();
    TVector3 pos0 = pos3;
    Double_t mom = gCandidate->GetMass() * gCandidate->GetStartBeta() * gCandidate->GetStartGamma();
    TVector3 startMomentum(mom * direction0.X(), mom * direction0.Y(), mom * direction0.Z());
    gCandidate->SetStartPosition(pos0);
    gCandidate->SetStartMomentum(startMomentum);
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

            LOG(DEBUG3) << " at " << det->GetDetectorName() << ", momentum:" << gCandidate->GetMomentum().X() << ", "
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

        R3BHit* hit = nullptr;
        Int_t hitIndex = gCandidate->GetHitIndexByName(det->GetDetectorName().Data());
        if (-1 != hitIndex)
            hit = gSetup->GetHit(det->GetDetectorName().Data(), hitIndex);

        // Take into chi2 only if there is a hit and user specified SigmaX > 0.
        if (hit && det->res_x > 1e-6)
        {
            chi2 += TMath::Power((x_l - hit->GetX()) / det->res_x, 2);
            // LOG(INFO) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
            LOG(DEBUG3) << "chi2calc x: " << det->GetDetectorName().Data() << " res: " << det->res_x << " pos: " << x_l
                        << " hit: " << hit->GetX() << " dev: " << x_l - hit->GetX() << " chi2: " << chi2;
            nchi2 += 1;
        }
        if (hit && det->res_y > 1e-6)
        {
            chi2 += TMath::Power((y_l - hit->GetY()) / det->res_y, 2);
            // LOG(INFO) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
            LOG(DEBUG3) << "chi2calc y: " << det->GetDetectorName().Data() << " res: " << det->res_y << " pos: " << y_l
                        << " hit: " << hit->GetY() << " dev: " << y_l - hit->GetY() << " chi2: " << chi2;
            nchi2 += 1;
        }
    }
    gCandidate->SetChi2(chi2);
    return chi2;
}

R3BFragmentFitterChi2S494::R3BFragmentFitterChi2S494() {}

R3BFragmentFitterChi2S494::~R3BFragmentFitterChi2S494() {}

void R3BFragmentFitterChi2S494::Init(R3BTPropagator* prop, Bool_t energyLoss)
{
    LOG(DEBUG3) << "Call of init of R3BFragmentFitterChi2S494 !!!!!!!!!!!!!!" << endl;
    fPropagator = prop;
    gProp = prop;
    gEnergyLoss = energyLoss;

    fMinimum = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");

    // set tolerance , etc...
    fMinimum->SetMaxFunctionCalls(100000); // for Minuit/Minuit2
    fMinimum->SetMaxIterations(10000);     // for GSL
    fMinimum->SetTolerance(10.);
    fMinimum->SetPrintLevel(0);
    // set minimization quality level for low (0), medium (1) and high (2) quality
    fMinimum->SetStrategy(1);
    // create funciton wrapper for minmizer
    // a IMultiGenFunction type
    ROOT::Math::Functor* f = new ROOT::Math::Functor(&Chi2Backward2D, 1);
    fMinimum->SetFunction(*f);

    minimum_m = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Simplex");
    // minimum_m = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Minimize");
    // minimum_m = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
    // minimum_m = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Scan");
    // minimum_m = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Seek");

    // set tolerance , etc...
    minimum_m->SetMaxFunctionCalls(10000); // for Minuit/Minuit2
    minimum_m->SetMaxIterations(1000);     // for GSL
    minimum_m->SetTolerance(10.);
    minimum_m->SetPrintLevel(0);
    minimum_m->SetStrategy(1);
    // create funciton wrapper for minmizer
    // a IMultiGenFunction type
    ROOT::Math::Functor* fm = new ROOT::Math::Functor(&Chi2MomentumForward, 6);
    minimum_m->SetFunction(*fm);
}

Int_t R3BFragmentFitterChi2S494::FitTrack(R3BTrackingParticle* particle, R3BTrackingSetup* setup)
{
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

Int_t R3BFragmentFitterChi2S494::FitTrackMomentumForward(R3BTrackingParticle* particle,
                                                         R3BTrackingSetup* setup,
                                                         Double_t pin)
{

    fPropagator->SetVis(kFALSE);

    LOG(DEBUG3) << "In track momentum forward" << endl;
    gCandidate = particle;
    gSetup = setup;
    Double_t pbeam = 17.3915; // 16.8;

    Double_t px0 = gCandidate->GetStartMomentum().X();
    Double_t py0 = gCandidate->GetStartMomentum().Y();
    Double_t pz0 = gCandidate->GetStartMomentum().Z();

    Double_t theta0 = 0;

    Double_t x0 = gCandidate->GetStartPosition().X();
    Double_t y0 = gCandidate->GetStartPosition().Y();
    Double_t z0 = gCandidate->GetStartPosition().Z();
    Double_t mass = gCandidate->GetMass();

    LOG(DEBUG3) << "In FitTrackMomentumForward" << endl;
    LOG(DEBUG3) << setprecision(10) << "Start Momenta: " << px0 << ", " << py0 << ", " << pz0 << endl;
    LOG(DEBUG3) << setprecision(10) << "Start Position: " << x0 << "; " << y0 << ", " << z0 << endl;

    TVector3 pos0;
    TVector3 pos3;

    TVector3 pos23a;
    if (gCandidate->GetHitIndexByName("fi23a") > -1)
    {
        auto fi23a = gSetup->GetByName("fi23a");
        fi23a->LocalToGlobal(pos23a, gSetup->GetHit("fi23a", gCandidate->GetHitIndexByName("fi23a"))->GetX(), 0.);
    }

    TVector3 pos23b;
    if (gCandidate->GetHitIndexByName("fi23b") > -1)
    {
        auto fi23b = gSetup->GetByName("fi23b");
        fi23b->LocalToGlobal(pos23b, 0., gSetup->GetHit("fi23b", gCandidate->GetHitIndexByName("fi23b"))->GetY());
    }

    if (gCandidate->GetHitIndexByName("fi23a") > -1 && gCandidate->GetHitIndexByName("fi23b") > -1)
    {
        auto fi23a = gSetup->GetByName("fi23a");

        Double_t xfi23b = x0 + (pos23a.X() - x0) * pos23b.Z() / pos23a.Z();

        auto fi23b = gSetup->GetByName("fi23b");
        fi23b->LocalToGlobal(pos3, xfi23b, gSetup->GetHit("fi23b", gCandidate->GetHitIndexByName("fi23b"))->GetY());
    }

    pos0.SetX(x0);
    pos0.SetY(y0);
    pos0.SetZ(z0);

    TVector3 direction0 = pos3 - pos0;
    direction0.SetMag(pz0);

    px0 = direction0.X();
    py0 = direction0.Y();
    pz0 = direction0.Z();

    if (x0 < -1.5)
        x0 = -1.5;
    if (y0 < -1.5)
        y0 = -1.5;
    if (x0 > 1.5)
        x0 = 1.5;
    if (y0 > 1.5)
        y0 = 1.5;

    LOG(DEBUG3) << "Start values momentum: " << px0 << "  " << py0 << "  " << pz0 << endl;
    LOG(DEBUG3) << "Start values position: " << x0 << "  " << y0 << "  " << z0 << endl;
    LOG(DEBUG3) << "Pos3 x,y,z pos       : " << pos3.X() << "  " << pos3.Y() << "  " << pos3.Z() << endl;
    LOG(DEBUG3) << "Direction x,y,z pos  : " << pos3.X() << "  " << pos3.Y() << "  " << pos3.Z() << endl;

    //    double variable[3] = { gCandidate->GetMomentum().X(), gCandidate->GetMomentum().Y(),
    //    gCandidate->GetMomentum().Z()}; double step[3] = { 0.01, 0.01, 0.01 };
    double variable[6] = { px0, py0, pz0, x0, y0, z0 };
    double step[6] = { 0.001, 0.001, 0.1, 0.01, 0.01, 0.01 };
    //    double variable[5] = { px0, py0, pz0, x0, y0};
    //    double step[5] = { 0.1, 0.1, 0.1, 0.01, 0.01 };
    //    double variable[3] = { px0, py0, pz0 };
    //    double step[3] = { 0.01, 0.01, 0.1 };

    // Set the free variables to be minimized!

    //   cout<<"MOMENTA: "<<gCandidate->GetStartMomentum().X()<<"; "<<gCandidate->GetStartMomentum().Y()<<";
    //   "<<gCandidate->GetStartMomentum().Z()<<endl;

    minimum_m->SetLimitedVariable(0,
                                  "px",
                                  variable[0],
                                  step[0],
                                  gCandidate->GetStartMomentum().X() - 0.0005,
                                  gCandidate->GetStartMomentum().X() + 0.0005);

    minimum_m->SetLimitedVariable(1,
                                  "py",
                                  variable[1],
                                  step[1],
                                  gCandidate->GetStartMomentum().Y() - 0.0005,
                                  gCandidate->GetStartMomentum().Y() + 0.0005);

    minimum_m->SetLimitedVariable(2,
                                  "pz",
                                  variable[2],
                                  step[2],
                                  gCandidate->GetStartMomentum().Z() - 10.05,
                                  gCandidate->GetStartMomentum().Z() + 10.05);
    //}

    minimum_m->SetLimitedVariable(3, "x0", variable[3], step[3], -1.5, 1.5);
    minimum_m->SetLimitedVariable(4, "y0", variable[4], step[4], -1.5, 1.5);
    minimum_m->SetLimitedVariable(5, "z0", variable[5], step[5], z0 - 0.005, z0 + 0.005); // z0 - 1., z0 + 1.);

    // Zum testen
    minimum_m->FixVariable(0);
    minimum_m->FixVariable(1);
    // minimum_m->FixVariable(4);
    minimum_m->FixVariable(5);
    if (mass < 3.8 && mass > 3.7) // 4He
    {
        minimum_m->FixVariable(3);
        minimum_m->FixVariable(4);
    }

    Double_t py0_cand =
        (pos23b.Y() - gCandidate->GetStartPosition().Y()) / pos23b.Z() * gCandidate->GetStartMomentum().Z();
    Double_t px0_cand =
        (pos23a.X() - gCandidate->GetStartPosition().X()) / pos23a.Z() * gCandidate->GetStartMomentum().Z();

    //    TVector3 startMomentum(
    //       gCandidate->GetStartMomentum().X(), gCandidate->GetStartMomentum().Y(),
    //       gCandidate->GetStartMomentum().Z());

    TVector3 startMomentum(px0_cand, py0_cand, gCandidate->GetStartMomentum().Z());

    TVector3 startPosition(
        gCandidate->GetStartPosition().X(), gCandidate->GetStartPosition().Y(), gCandidate->GetStartPosition().Z());

    gCandidate->SetStartPosition(startPosition);

    gCandidate->SetStartMomentum(startMomentum);

    gCandidate->Reset();

    Int_t status = 0;

    // do the minimization
    minimum_m->Minimize();

    status = minimum_m->Status();
    // LOG(DEBUG3) << "Status: " << status << endl;

    LOG(DEBUG3) << "optimized momentum: " << minimum_m->X()[0] << "  " << minimum_m->X()[1] << "  " << minimum_m->X()[2]
                << endl;
    LOG(DEBUG3) << "optimized position: " << minimum_m->X()[3] << "  " << minimum_m->X()[4] << "  " << minimum_m->X()[5]
                << endl;

    py0_cand = (pos23b.Y() - minimum_m->X()[4]) / pos23b.Z() * minimum_m->X()[2];
    px0_cand = (pos23a.X() - minimum_m->X()[3]) / pos23a.Z() * minimum_m->X()[2];

    TVector3 startPositionOptimized(minimum_m->X()[3], minimum_m->X()[4], minimum_m->X()[5]);
    TVector3 startMomentumOptimized(px0_cand, py0_cand, minimum_m->X()[2]);
    // TVector3 startMomentumOptimized(minimum_m->X()[0], minimum_m->X()[1], minimum_m->X()[2]);

    gCandidate->SetStartPosition(startPositionOptimized);
    gCandidate->SetStartMomentum(startMomentumOptimized);
    gCandidate->Reset();

    minimum_m->Clear();

    LOG(DEBUG3) << "current momentum: " << gCandidate->GetMomentum().X() << "  " << gCandidate->GetMomentum().Y()
                << "  " << gCandidate->GetMomentum().Z();
    LOG(DEBUG3) << "current position: " << gCandidate->GetPosition().X() << "  " << gCandidate->GetPosition().Y()
                << "  " << gCandidate->GetPosition().Z();

    // delete minimum_m;

    return status;
}

Int_t R3BFragmentFitterChi2S494::FitTrackMomentumBackward(R3BTrackingParticle* particle, R3BTrackingSetup* setup)
{
    // LOG(DEBUG3)
    cout << "In track momentum backward" << endl;
    gCandidate = particle;
    gSetup = setup;

    Double_t px0 = 0.;
    Double_t py0 = 0.;
    Double_t pz0 = 0.;
    Double_t x0 = 0.;
    Double_t y0 = 0.;
    Double_t z0 = 0.;

    TVector3 pos0;
    TVector3 pos2;
    TVector3 pos3;

    if (gCandidate->GetHitIndexByName("fi32") > -1 && gCandidate->GetHitIndexByName("fi30") > -1)
    {
        auto fi32 = gSetup->GetByName("fi32");
        auto fi30 = gSetup->GetByName("fi30");
        fi32->LocalToGlobal(pos2, gSetup->GetHit("fi32", gCandidate->GetHitIndexByName("fi32"))->GetX(), 0.);
        fi30->LocalToGlobal(pos3, gSetup->GetHit("fi30", gCandidate->GetHitIndexByName("fi30"))->GetX(), 0.);
    }
    else if (gCandidate->GetHitIndexByName("fi31") > -1 && gCandidate->GetHitIndexByName("fi33") > -1)
    {
        auto fi31 = gSetup->GetByName("fi31");
        auto fi33 = gSetup->GetByName("fi33");
        fi33->LocalToGlobal(pos2, gSetup->GetHit("fi33", gCandidate->GetHitIndexByName("fi33"))->GetX(), 0.);
        fi31->LocalToGlobal(pos3, gSetup->GetHit("fi31", gCandidate->GetHitIndexByName("fi31"))->GetX(), 0.);
    }
    else
    {
        return 10;
    }

    if (gCandidate->GetHitIndexByName("tofd") > -1)
    {
        auto tofd = gSetup->GetByName("tofd");
        tofd->LocalToGlobal(pos0,
                            gSetup->GetHit("tofd", gCandidate->GetHitIndexByName("tofd"))->GetX(),
                            gSetup->GetHit("tofd", gCandidate->GetHitIndexByName("tofd"))->GetY());
    }

    TVector3 direction0 = (pos3 - pos2).Unit();
    // TVector3 direction0 = (pos2 - pos0).Unit();

    Double_t mom = gCandidate->GetMass() * gCandidate->GetStartBeta() * gCandidate->GetStartGamma();
    direction0.SetMag(mom);

    TVector3 startMomentum(direction0.X(), direction0.Y(), direction0.Z());

    // LOG(DEBUG3)
    cout << "mom: " << mom << endl;

    px0 = direction0.X();
    py0 = direction0.Y();
    pz0 = direction0.Z();

    x0 = pos0.X();
    y0 = pos0.Y();
    z0 = pos0.Z();

    gCandidate->Reset();
    gCandidate->SetPosition(pos0);
    gCandidate->SetMomentum(startMomentum);

    // LOG(DEBUG3)
    cout << "Start values momentum lab: " << px0 << "  " << py0 << "  " << pz0 << endl;
    // LOG(DEBUG3)
    cout << "Start values position lab: " << x0 << "  " << y0 << "  " << z0 << endl;

    Double_t x_l, y_l;
    if (gCandidate->GetHitIndexByName("tofd") > -1)
    {
        auto tofd = gSetup->GetByName("tofd");
        tofd->GlobalToLocal(gCandidate->GetPosition(), x_l, y_l);
        cout << "Start values position ToFd recalc: " << x_l << "  " << y_l << endl;
    }

    double variable[6] = { px0, py0, pz0, x0, y0, z0 };
    double step[6] = { 0.01, 0.01, 0.01, 0.1, 0.1, 0.1 };

    //    ROOT::Math::Minimizer* minimum_a = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
    ROOT::Math::Minimizer* minimum_a = ROOT::Math::Factory::CreateMinimizer("Minuit2", "SIMPLEX");

    // set tolerance , etc...
    minimum_a->SetMaxFunctionCalls(10000); // for Minuit/Minuit2
    minimum_a->SetMaxIterations(1000);     // for GSL
    minimum_a->SetTolerance(10.0);
    minimum_a->SetPrintLevel(0);

    // create funciton wrapper for minmizer
    // a IMultiGenFunction type
    ROOT::Math::Functor fa(&Chi2MomentumBackward, 6);
    minimum_a->SetFunction(fa);

    // Set the free variables to be minimized!
    minimum_a->SetLimitedVariable(0, "px", variable[0], step[0], px0 - 10., px0 + 10.);
    minimum_a->SetLimitedVariable(1, "py", variable[1], step[1], -30., 30.);
    minimum_a->SetLimitedVariable(
        2, "pz", variable[2], step[2], gCandidate->GetMomentum().Z() - 20., gCandidate->GetMomentum().Z() + 20.);

    minimum_a->SetLimitedVariable(3, "x0", variable[3], step[3], x0 - 5., x0 + 5.);
    minimum_a->SetLimitedVariable(4, "y0", variable[4], step[4], y0 - 5., y0 + 5.);
    minimum_a->SetLimitedVariable(5, "z0", variable[5], step[5], z0 - 1., z0 + 1.);

    gCandidate->SetStartBeta(gCandidate->GetBeta());
    gCandidate->SetCharge(-1. * gCandidate->GetCharge());
    gCandidate->UpdateMomentum();

    Int_t status = 0;

    // do the minimization
    minimum_a->Minimize();

    status = minimum_a->Status();
    // LOG(DEBUG3) << "Status: " << status ;

    // LOG(DEBUG3)
    cout << "optimized momentum: " << minimum_a->X()[0] << "  " << minimum_a->X()[1] << "  " << minimum_a->X()[2]
         << endl;
    // LOG(DEBUG3)
    cout << "optimized position: " << minimum_a->X()[3] << "  " << minimum_a->X()[4] << "  " << minimum_a->X()[5]
         << endl;

    TVector3 startMomentumOptimized(minimum_a->X()[0], minimum_a->X()[1], minimum_a->X()[2]);
    TVector3 startPosOptimized(minimum_a->X()[3], minimum_a->X()[4], minimum_a->X()[5]);

    gCandidate->Reset();
    gCandidate->SetPosition(startPosOptimized);
    gCandidate->SetMomentum(startMomentumOptimized);

    // Propagate through the setup, defined by array of detectors
    for (Int_t i = (gSetup->GetArray().size() - 1); i >= 0; i--)
    {
        auto det = gSetup->GetArray().at(i);
        if (i < (gSetup->GetArray().size() - 1))
        {
            // LOG(DEBUG3)
            cout << " at " << det->GetDetectorName() << ", momentum:" << gCandidate->GetMomentum().X() << ", "
                 << gCandidate->GetMomentum().Y() << ", " << gCandidate->GetMomentum().Z() << endl;

            gProp->PropagateToDetectorBackward(gCandidate, det);

            if (gEnergyLoss)
            {
                Double_t weight = 1.;
                if (kTarget == det->section)
                {
                    weight = 0.5;
                }

                gCandidate->PassThroughDetectorBackward(det, weight);
            }
        }
    }

    // particle->UpdateMomentum();
    LOG(DEBUG3) << "nach Energieverlust und invertieren";
    LOG(DEBUG3) << "current momentum: " << -1. * gCandidate->GetMomentum().X() << "  "
                << -1. * gCandidate->GetMomentum().Y() << "  " << -1. * gCandidate->GetMomentum().Z();
    LOG(DEBUG3) << "current position: " << gCandidate->GetPosition().X() << "  " << gCandidate->GetPosition().Y()
                << "  " << gCandidate->GetPosition().Z();

    Double_t px_inv = (-1.) * gCandidate->GetMomentum().X();
    Double_t py_inv = (-1.) * gCandidate->GetMomentum().Y();
    Double_t pz_inv = (-1.) * gCandidate->GetMomentum().Y();
    TVector3 pinv(px_inv, py_inv, pz_inv);
    gCandidate->SetStartMomentum(pinv);

    //  gCandidate->SetStartMomentum(-1. * gCandidate->GetMomentum());
    gCandidate->SetStartPosition(gCandidate->GetPosition());
    gCandidate->SetCharge(-1. * gCandidate->GetCharge());
    gCandidate->Reset();

    minimum_a->Clear();
    // delete minimum_a;
    // candidate->Reset();

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
    double step[1] = { 0.0001 };

    minimum->SetFunction(f);

    // Set the free variables to be minimized!
    minimum->SetLimitedVariable(0, "beta", variable[0], step[0], 0.5, 0.999);

    Int_t status = 0;

    // do the minimization
    gEnergyLoss = kTRUE;

    minimum->Minimize();

    status = minimum->Status();
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
    fPropagator->SetVis(kFALSE);

    gCandidate = particle;
    gSetup = setup;

    auto fi23b = gSetup->GetByName("fi23b");
    auto fi30 = gSetup->GetByName("fi30");
    auto fi32 = gSetup->GetByName("fi32");
    // auto tof = gSetup->GetFirstByType(kTof);

    double variable[1] = { 132. * amu };
    double step[1] = { 0.01 };

    // Set the free variables to be minimized!
    fMinimum->SetLimitedVariable(0, "m", variable[0], step[0], 125. * amu, 133. * amu);

    TVector3 pos1;
    TVector3 pos2;
    TVector3 pos3;
    fi23b->LocalToGlobal(pos1, gSetup->GetHit("fi23b", particle->GetHitIndexByName("fi23b"))->GetX(), 0.);
    fi30->LocalToGlobal(pos2, gSetup->GetHit("fi30", particle->GetHitIndexByName("fi30"))->GetX(), 0.);
    fi32->LocalToGlobal(pos3, gSetup->GetHit("fi32", particle->GetHitIndexByName("fi32"))->GetX(), 0.);
    /*Int_t np = 3;
    Double_t x[] = {pos1.X(), pos2.X(), pos3.X()};
    Double_t xe[] = {fi23b->res_x, fi30->res_x, fi32->res_x};
    Double_t z[] = {pos1.Z(), pos2.Z(), pos3.Z()};
    TGraphErrors* gr = new TGraphErrors(np, x, z, xe, 0);
    TF1* f1 = new TF1("f1", "[0]*x + [1]", -500., 500.);
    TF1* f2 = new TF1("f1", "[0]*x + [1]", 0., 1000.);
    f1->SetParameters(-1., 0.);
    gr->Fit(f1, "QN");
    f2->SetParameters(1./f1->GetParameter(0), -1.*f1->GetParameter(1)/f1->GetParameter(0));
    TVector3 direction0 = ( TVector3(fi30->hit_x, 0., f1->Eval(fi30->hit_x)) - TVector3(fi32->hit_x, 0.,
    f1->Eval(fi32->hit_x)) ).Unit(); TVector3 pos0(f2->Eval(fi32->pos0.Z()), 0., fi32->pos0.Z());*/
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

    // Require fi30 and fi32  or fi31 and fi33 to have hit
    // for the initial position and direction
    gCandidate = particle;
    gSetup = setup;

    TVector3 pos2;
    TVector3 pos3;
    if (gCandidate->GetHitIndexByName("fi30") > -1 && gCandidate->GetHitIndexByName("fi32") > -1)
    {
        auto fi30 = gSetup->GetByName("fi30");
        auto fi32 = gSetup->GetByName("fi32");
        fi30->LocalToGlobal(pos2,
                            gSetup->GetHit("fi30", gCandidate->GetHitIndexByName("fi30"))->GetX(),
                            gSetup->GetHit("fi30", gCandidate->GetHitIndexByName("fi30"))->GetY());
        fi32->LocalToGlobal(pos3,
                            gSetup->GetHit("fi32", gCandidate->GetHitIndexByName("fi32"))->GetX(),
                            gSetup->GetHit("fi32", gCandidate->GetHitIndexByName("fi32"))->GetY());
        LOG(DEBUG2) << "Fi 32 hit index " << particle->GetHitIndexByName("fi32") << " out of " << fi32->hits.size();
    }
    else if (gCandidate->GetHitIndexByName("fi31") > -1 && gCandidate->GetHitIndexByName("fi33") > -1)
    {
        auto fi31 = gSetup->GetByName("fi31");
        auto fi33 = gSetup->GetByName("fi33");
        fi31->LocalToGlobal(pos2,
                            gSetup->GetHit("fi31", gCandidate->GetHitIndexByName("fi31"))->GetX(),
                            gSetup->GetHit("fi31", gCandidate->GetHitIndexByName("fi31"))->GetY());
        fi33->LocalToGlobal(pos3,
                            gSetup->GetHit("fi33", gCandidate->GetHitIndexByName("fi33"))->GetX(),
                            gSetup->GetHit("fi33", gCandidate->GetHitIndexByName("fi33"))->GetY());
    }
    else
    {
        return 10;
    }
    double variable[1] = { gCandidate->GetMass() };
    double step[1] = { 0.01 };

    // Set the free variables to be minimized!
    fMinimum->SetLimitedVariable(0, "m", variable[0], step[0], 2., 20.);
    // fMinimum->SetLimitedVariable(1, "xfi32", variable[1], step[1], -200., 200.);

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
    // cout << "Status: " << status << endl;
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

            if (det->GetDetectorName().EqualTo("fi23b"))
            {                                                        // fi23b
                Double_t eloss = 0.5 * det->GetEnergyLoss(particle); // 0.5 because of 2 layers !!!
                fh_eloss_fi23b->Fill(eloss);
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
