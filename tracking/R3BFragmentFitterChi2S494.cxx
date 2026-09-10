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
    // cout << "IN CHI2 FUNCTION" << endl;
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

            // LOG(info) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
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

    LOG(debug3) << "In chi2 forward" << endl;
    LOG(debug3) << "current momentum: " << gCandidate->GetMomentum().X() << "  " << gCandidate->GetMomentum().Y()
                << "  " << gCandidate->GetMomentum().Z() << endl;
    LOG(debug3) << "current position: " << gCandidate->GetPosition().X() << "  " << gCandidate->GetPosition().Y()
                << "  " << gCandidate->GetPosition().Z() << endl;

    if (gCandidate->GetMass() > 110.0)
    {
        cout << "In chi2 forward with mass: " << gCandidate->GetMass() << endl;
        cout << "Starting momentum: " << gCandidate->GetMomentum().X() << "  " << gCandidate->GetMomentum().Y() << "  "
             << gCandidate->GetMomentum().Z() << endl;
        cout << "Starting position: " << gCandidate->GetPosition().X() << "  " << gCandidate->GetPosition().Y() << "  "
             << gCandidate->GetPosition().Z() << endl;
    }
    if(xx[0] <= 0.){
		return 1.e+10;
	}
	
    Double_t x_l = 0.0;
    Double_t y_l = 0.0;
    Double_t chi2 = 0.0;
    Int_t nchi2 = 0;
    Double_t diffx=0., diffy=0.;

    Double_t ptot0 = xx[0];
    Double_t x0 = xx[1];
    Double_t y0 = xx[2];
    Double_t px0beam = (-2.0324+129.288*x0)/1000.0;
    Double_t py0beam = (2.01651+129.268*y0)/1000.0;
    if((17.3915*17.3915-px0beam*px0beam-py0beam*py0beam) <= 0.) return 1.e+10;
    Double_t pz0beam = sqrt(17.3915*17.3915-px0beam*px0beam-py0beam*py0beam);
    Double_t z0 = 0.0;
    Double_t px0=0.,py0=0.,pz0=ptot0;
    
    TVector3 pos23a;
    TVector3 pos23b;
    TVector3 pos23a_loc;
    TVector3 pos23b_loc;
	TVector3 startMomentumChi;
    
    // px, py are given by the positions at fib23a, fib23b and should not be optimized:

    if (gCandidate->GetHitIndexByName("fi23a") > -1 && gCandidate->GetHitIndexByName("fi23b") > -1)
    {

        if (gCandidate->GetMass() > 3.7 && gCandidate->GetMass() < 3.8) // alpha
        {
            // diffx = 0.058115; // test field
            diffx = 0.0093 * 0.996;      // standard field
            diffy = -0.000365 * (0.125); // 0.0002;//(1.9475601 + 0.02056205*pos23b.Y())*1.e-3*pos23b.Z()/pz0 ;
        }
        else //if (gCandidate->GetMass() > 11.1) // && gCandidate->GetMass() < 11.2) // C&& O
        {
            // diffx = 0.058115; //0.058125 test field
            diffx = 0.0093 * 0.996;       // standard field
            diffy = -0.000365 * (-0.226); // 0.0002; //(1.9475601 + 0.02056205*pos23b.Y())*1.e-3*pos23b.Z()/ptot0 ;
        }

        auto fi23a = gSetup->GetByName("fi23a");
        fi23a->LocalToGlobal(pos23a_loc, gSetup->GetHit("fi23a", gCandidate->GetHitIndexByName("fi23a"))->GetX()-diffx-x0, 0.0);

        auto fi23b = gSetup->GetByName("fi23b");
        fi23b->LocalToGlobal(pos23b_loc, 0.0, gSetup->GetHit("fi23b", gCandidate->GetHitIndexByName("fi23b"))->GetY()-diffy-y0);
 
	    //Rotate the position vector INTO the Beam Frame:
	    TVector3 BeamMomentum(px0beam,py0beam,pz0beam);	
	 	TVector3 BeamMomentumDirection = BeamMomentum.Unit();
		TVector3 nominal(0.0, 0.0, 1.0);
		TVector3 rot_axis = nominal.Cross(BeamMomentumDirection);
		double rot_angle  = nominal.Angle(BeamMomentumDirection);
		pos23a = pos23a_loc;
		pos23b = pos23b_loc;
		if (rot_axis.Mag() > 1e-7) {
			pos23a.Rotate(-rot_angle, rot_axis);
			pos23b.Rotate(-rot_angle, rot_axis);
		}
		// Propagate along the beam frame to the detector plane (z = pos23a_loc.Z())
		// In the beam's local frame, the particle travels in a straight line 
		// from the origin to pos_beam. Therefore, the ratio of its coordinates is constant!
		// We scale the vector so that its Z component exactly matches the physical distance pos23a_loc.Z().		
		pos23a *= pos23a_loc.Z() / pos23a.Z(); 
        pos23b *= pos23b_loc.Z() / pos23b.Z(); 
        Double_t pos23z = (pos23a.Z()+pos23b.Z())/2.;
        Double_t radius = sqrt(pos23a.X() * pos23a.X() + 
                      pos23b.Y() * pos23b.Y() + 
                      pos23z * pos23z);
		// Calculate local momentum components 		
		px0 = pos23a.X() / radius * ptot0;
		py0 = pos23b.Y() / radius * ptot0;
		pz0 = pos23z / radius * ptot0;
		startMomentumChi.SetX(px0);
		startMomentumChi.SetY(py0);
		startMomentumChi.SetZ(pz0);
		// rotate back to lab frame
		if (rot_axis.Mag() > 1e-7) {
				startMomentumChi.Rotate(rot_angle, rot_axis);
		}

		//cout<<"px: "<<pos23a.X()<<" "<<pos23a_loc.X()<<" "<<pos23a.Z()<<" "<<pos23a_loc.Z()<<" "<<px0<<" "<<pos23a_loc.X() / pos23a_loc.Z() * pz0<<endl;
        /*
            cout << "fib23a x: " << pos23a.X()<< ", fib23a z: " <<pos23a.Z()  << " x0: " << x0 << " px0: " << px0 <<
              "pz0: "<<pz0<<", mass= "<<gCandidate->GetMass() <<endl;
            cout << "fib23b y: " << pos23b.Y()<< ", fib23b z: " <<pos23b.Z()  << " y0: " << y0 << " py0: " << py0 <<
              "pz0: "<<pz0<<", mass= "<<gCandidate->GetMass() <<endl;
         */
    }

    if (gCandidate->GetMass() > 110.0)
    {
        cout << "in Chi2: Variables momentum: " << startMomentumChi.X() << "  " << startMomentumChi.Y() << "  " << startMomentumChi.Z() << endl;
        cout << "in Chi2: Variables position: " << x0 << "  " << y0 << "  " << z0 << endl;
    }

    TVector3 startPositionChi;
    startPositionChi.SetX(x0);
    startPositionChi.SetY(y0);
    startPositionChi.SetZ(z0);
    
    TVector3 beamMomentumChi;
    beamMomentumChi.SetX(px0beam);
    beamMomentumChi.SetY(py0beam);
    beamMomentumChi.SetZ(pz0beam);

    gCandidate->SetStartPosition(startPositionChi);
    gCandidate->SetStartMomentum(startMomentumChi);
    gCandidate->SetBeamMomentum(beamMomentumChi);
    gCandidate->Reset();

    LOG(debug3) << "Nach setzen der Startwerte" << endl;
    LOG(debug3) << "current momentum: " << gCandidate->GetMomentum().X() << "  " << gCandidate->GetMomentum().Y()
                << "  " << gCandidate->GetMomentum().Z() << endl;
    LOG(debug3) << "*** Current lab position: " << gCandidate->GetPosition().X() << "  "
                << gCandidate->GetPosition().Y() << "  " << gCandidate->GetPosition().Z() << endl;

    // Propagate forward through the setup, defined by array of detectors
    for (Int_t i = 0; i < (gSetup->GetArray().size() - 0); i++)
    {
        auto det = gSetup->GetArray().at(i);

        R3BHit* hit = nullptr;
        Int_t hitIndex = gCandidate->GetHitIndexByName(det->GetDetectorName().Data());

        // cout << "Hit index " << hitIndex << endl;
        if (-1 != hitIndex)
        {
            hit = gSetup->GetHit(det->GetDetectorName().Data(), hitIndex);
        }

        if (!hit)
            continue;

        LOG(debug3) << "***** At detector ****** " << det->GetDetectorName() << endl;

        //   cout << "***** At detector ****** " << det->GetDetectorName() << endl;
        if (i > 0)
        {
            gProp->PropagateToDetector(gCandidate, det);

            // time += (gCandidate->GetLength() - prev_l) / (gCandidate->GetBeta() * SPEED_OF_LIGHT);
            // prev_l = gCandidate->GetLength();

            if (gCandidate->GetMass() > 110.0)
            {
                cout << " At " << det->GetDetectorName() << ", momentum:" << gCandidate->GetMomentum().X() << ", "
                     << gCandidate->GetMomentum().Y() << ", " << gCandidate->GetMomentum().Z() << endl;
                cout << " At " << det->GetDetectorName() << ", position:" << gCandidate->GetPosition().X() << ", "
                     << gCandidate->GetPosition().Y() << ", " << gCandidate->GetPosition().Z() << endl;
            }
        }
        if (gEnergyLoss)
        {
            Double_t weight = 1.;
            if (kTarget == det->section)
            {
                weight = 0.5;
            }
            gCandidate->PassThroughDetector(det, weight);
            // cout <<" At " << det->GetDetectorName() <<", Energy loss: "<< det->GetEnergyLoss(gCandidate)<<endl;
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

        // Take into chi2 only if there is a hit and user specified SigmaX > 0.
        Double_t chi2temp;
        if (hit && det->res_x > 1e-6)
        {
            chi2temp = TMath::Power((x_l - hit->GetX()) / det->res_x, 2);

            if (det->GetDetectorName() == "tofd" )
            {
                if (TMath::Abs(x_l - hit->GetX()) < det->res_x)
                {
                    chi2temp = 0.0;
                }
                else
                {
                    // cout << "Testcase: " << chi2temp << endl;
                    chi2temp = chi2temp * 100.0;
                }
            }
            else
            {
                if (TMath::Abs(x_l - hit->GetX()) < det->res_x)
                {
                    chi2temp = 0.;
                }
                else
                {
                    // cout << "Testcase: " << chi2temp << endl;
                    chi2temp = chi2temp;
                }
            }

            // chi2temp = TMath::Power((x_l - hit->GetX()) / det->res_x, 2);  // only for simu without granularity
            //  cout <<" At " << det->GetDetectorName() <<", chi2_x: "<<chi2temp<<", x_l= "<<x_l<<"; hit->GetX()=
            //  "<<hit->GetX()<<endl;
            chi2 += chi2temp;

            // chi2 += TMath::Power((x_l - hit->GetX()) / det->res_x, 2);
            // LOG(info) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
            // LOG(debug)
            // cout <<" At " << det->GetDetectorName() <<", chi2_x: "<<chi2temp<<", x_l= "<<x_l<<"; hit->GetX()= "
            // <<hit->GetX()<<", diff: "<<x_l - hit->GetX()<<endl;

            nchi2 += 1;
        }
        if (hit && TMath::Abs(det->res_y) > 1e-6)
        {

            Double_t yres = TMath::Abs(det->res_y);
            Double_t yscal = 1.;
            if ((det->GetDetectorName() == "tofd" && det->res_y < 0.))
                yscal = 0.1;

            if (det->GetDetectorName() == "tofd" && gCandidate->GetMass() < 3.8 && gCandidate->GetMass() > 3.7)
                yres = 3.0 * yres;

            chi2temp = TMath::Power((y_l - hit->GetY()) / yres, 2) * yscal;

            if (det->GetDetectorName() == "fi23b")
            {
                if (TMath::Abs(y_l - hit->GetY()) < det->res_y)
                {
                    chi2temp = 0.0;
                }
                  else chi2temp = 10000.;
            }

            //  chi2temp = TMath::Power((y_l - hit->GetY()) / yres, 2);   // only for simu without granularity

            // cout <<" At " << det->GetDetectorName() <<", chi2_y: "<<chi2temp<<", y_l= "<<y_l<<"; hit->GetY()= "
            //  <<hit->GetY()<<", diff: "<<y_l - hit->GetY()<<endl;
            chi2 += chi2temp;

            // chi2 += TMath::Power((y_l - hit->GetY()) / det->res_y, 2);

            // LOG(info) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
            // LOG(debug)
            // cout << "chi2calc y: " << det->GetDetectorName().Data() << " res: " << det->res_y << " pos: " << y_l
            //          << " hit: " << hit->GetY() << " dev: " << y_l - hit->GetY() << " chi2: " << chi2temp << endl;

            nchi2 += 1;
        }
    }

    // gCandidate->SetChi2(chi2);
    LOG(debug3) << "Ende chi2: " << chi2 << endl;
    LOG(debug3) << "current momentum: " << gCandidate->GetMomentum().X() << "  " << gCandidate->GetMomentum().Y()
                << "  " << gCandidate->GetMomentum().Z() << endl;
    LOG(debug3) << "current position: " << gCandidate->GetPosition().X() << "  " << gCandidate->GetPosition().Y()
                << "  " << gCandidate->GetPosition().Z() << endl;
    LOG(debug3) << "current beam momentum: " << gCandidate->GetBeamMomentum().X() << "  " << gCandidate->GetBeamMomentum().Y()
                << "  " << gCandidate->GetBeamMomentum().Z() << endl;

    // gCandidate->Reset();
    if (gCandidate->GetMass() > 110.0)
    {
        cout << "Am Ende chi2 with current chi2= " << chi2 << endl;
        cout << "Am Ende chi2 with current momentum : " << gCandidate->GetMomentum().X() << "  "
             << gCandidate->GetMomentum().Y() << "  " << gCandidate->GetMomentum().Z() << endl;
        cout << "Am Ende chi2 with current position: " << gCandidate->GetPosition().X() << "  "
             << gCandidate->GetPosition().Y() << "  " << gCandidate->GetPosition().Z() << endl;
    }

    return chi2;
}
/*
double Chi2MomentumBackward(const double* xx)
{

    LOG(debug3) << "In chi2 backward" << endl;
    LOG(debug3) << "current momentum: " << gCandidate->GetMomentum().X() << "  " << gCandidate->GetMomentum().Y()
                << "  " << gCandidate->GetMomentum().Z() << endl;
    LOG(debug3) << "current position: " << gCandidate->GetPosition().X() << "  " << gCandidate->GetPosition().Y()
                << "  " << gCandidate->GetPosition().Z() << endl;

    Double_t x_l = 0.;
    Double_t y_l = 0.;
    Double_t chi2 = 0.;
    Int_t nchi2 = 0;
    TVector3 pos2;
    TVector3 pos3;
    // LOG(debug3) << "Test: " << gCandidate->GetHitIndexByName("fi12") << "  " << gCandidate->GetHitIndexByName("fi10")
    // << endl;
    / *
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
    * /
    Double_t px0 = xx[0];
    Double_t py0 = xx[1];
    Double_t pz0 = xx[2];

    double x0 = xx[3];
    double y0 = xx[4];
    double z0 = xx[5];

    LOG(debug3) << "Variables momentum: " << px0 << "  " << py0 << "  " << pz0 << endl;
    LOG(debug3) << "Variables position: " << x0 << "  " << y0 << "  " << z0 << endl;

    TVector3 startPosition(x0, y0, z0);
    TVector3 startMomentum(px0, py0, pz0);

    gCandidate->SetPosition(startPosition);
    gCandidate->SetMomentum(startMomentum);
    // gCandidate->UpdateMomentum();
    // gCandidate->Reset();

    LOG(debug3) << "Nach setzen von Werten" << endl;
    LOG(debug3) << "current momentum: " << gCandidate->GetMomentum().X() << "  " << gCandidate->GetMomentum().Y()
                << "  " << gCandidate->GetMomentum().Z() << endl;
    LOG(debug3) << "current position: " << gCandidate->GetPosition().X() << "  " << gCandidate->GetPosition().Y()
                << "  " << gCandidate->GetPosition().Z() << endl;

    LOG(debug3) << "Test momentum: " << gCandidate->GetMomentum().X() << ", " << gCandidate->GetMomentum().Y() << ", "
                << gCandidate->GetMomentum().Z() << endl;
    LOG(debug3) << "Test position: " << gCandidate->GetPosition().X() << ", " << gCandidate->GetPosition().Y() << ", "
                << gCandidate->GetPosition().Z() << endl;
    LOG(debug3) << "Test charge: " << gCandidate->GetCharge() << endl;

    // Propagate backward through the setup, defined by array of detectors
    for (Int_t i = (gSetup->GetArray().size() - 1); i >= 0; i--)
    {
        auto det = gSetup->GetArray().at(i);

        if (i < (gSetup->GetArray().size() - 1))
        {
            gProp->PropagateToDetectorBackward(gCandidate, det);

            // time += (gCandidate->GetLength() - prev_l) / (gCandidate->GetBeta() * SPEED_OF_LIGHT);
            // prev_l = gCandidate->GetLength();

            LOG(debug3) << " at " << det->GetDetectorName() << ", momentum:" << gCandidate->GetMomentum().X() << ", "
                        << gCandidate->GetMomentum().Y() << ", " << gCandidate->GetMomentum().Z();

            LOG(debug3) << "Vor Energieverlust" << endl;
            LOG(debug3) << "current momentum: " << gCandidate->GetMomentum().X() << "  "
                        << gCandidate->GetMomentum().Y() << "  " << gCandidate->GetMomentum().Z() << endl;
            LOG(debug3) << "current position: " << gCandidate->GetPosition().X() << "  "
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
            LOG(debug3) << "Nach Energieverlust" << endl;
            LOG(debug3) << "current momentum: " << gCandidate->GetMomentum().X() << "  "
                        << gCandidate->GetMomentum().Y() << "  " << gCandidate->GetMomentum().Z() << endl;
            LOG(debug3) << "current position: " << gCandidate->GetPosition().X() << "  "
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
            chi2temp = TMath::Power((x_l - hit->GetX()) / det->res_x, 2);
            if (det->GetDetectorName() == "tofd")
            {
                if (TMath::Abs(x_l - hit->GetX()) < det->res_x)
                {
                    chi2temp = 0.;
                }
                else
                {
                    // cout << "Testcase: " << chi2temp << endl;
                    chi2temp = chi2temp * 100.;
                }
            }
            else
            {
                if (TMath::Abs(x_l - hit->GetX()) < det->res_x)
                {
                    chi2temp = 0.;
                }
            }
            chi2 += chi2temp;

            // chi2 += TMath::Power((x_l - hit->GetX()) / det->res_x, 2);
            // LOG(info) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
            LOG(debug) << "chi2calc x: " << det->GetDetectorName().Data() << " res: " << det->res_x << " pos: " << x_l
                       << " hit: " << hit->GetX() << " dev: " << x_l - hit->GetX() << " chi2: " << chi2temp << endl;

            nchi2 += 1;
        }
        if (hit && det->res_y > 1e-6)
        {

            Double_t yres = det->res_y;

            if (det->GetDetectorName() == "tofd" && gCandidate->GetMass() < 3.8 && gCandidate->GetMass() > 3.7)
                yres = 3. * yres;

            chi2temp = TMath::Power((y_l - hit->GetY()) / yres, 2);

            if (det->GetDetectorName() == "fi23b")
            {
                if (TMath::Abs(y_l - hit->GetY()) < det->res_y)
                {
                    chi2temp = 0.;
                }
                //  else chi2temp = 10000.;
            }
            chi2 += chi2temp;

            // chi2 += TMath::Power((y_l - hit->GetY()) / det->res_y, 2);
            // LOG(info) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
            LOG(debug3) << "chi2calc y: " << det->GetDetectorName().Data() << " res: " << det->res_y << " pos: " << y_l
                        << " hit: " << hit->GetY() << " dev: " << y_l - hit->GetY() << " chi2: " << chi2;

            nchi2 += 1;
        }
    }
    LOG(debug3) << "Ende chi2" << endl;
    LOG(debug3) << "current momentum: " << gCandidate->GetMomentum().X() << "  " << gCandidate->GetMomentum().Y()
                << "  " << gCandidate->GetMomentum().Z() << endl;
    LOG(debug3) << "current position: " << gCandidate->GetPosition().X() << "  " << gCandidate->GetPosition().Y()
                << "  " << gCandidate->GetPosition().Z() << endl;

    gCandidate->SetChi2(chi2);

    return chi2;

    return 0;
}
*/
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

            LOG(debug3) << " at " << det->GetDetectorName() << ", momentum:" << gCandidate->GetMomentum().X() << ","
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
            // LOG(info) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
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

    return 0;
}
/*
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
            / *result = * / gProp->PropagateToDetectorBackward(gCandidate, det);

            // time += (gCandidate->GetLength() - prev_l) / (gCandidate->GetBeta() * SPEED_OF_LIGHT);
            // prev_l = gCandidate->GetLength();

            LOG(debug3) << " at " << det->GetDetectorName() << ", momentum:" << gCandidate->GetMomentum().X() << ", "
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
        // LOG(info) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
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

    return 0;
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

            / *result = * / gProp->PropagateToDetectorBackward(gCandidate, det);

            // time += (gCandidate->GetLength() - prev_l) / (gCandidate->GetBeta() * SPEED_OF_LIGHT);
            // prev_l = gCandidate->GetLength();

            LOG(debug3) << " at " << det->GetDetectorName() << ", momentum:" << gCandidate->GetMomentum().X() << ", "
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
            // LOG(info) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
            LOG(debug3) << "chi2calc x: " << det->GetDetectorName().Data() << " res: " << det->res_x << " pos: " << x_l
                        << " hit: " << hit->GetX() << " dev: " << x_l - hit->GetX() << " chi2: " << chi2;
            nchi2 += 1;
        }
        if (hit && det->res_y > 1e-6)
        {
            chi2 += TMath::Power((y_l - hit->GetY()) / det->res_y, 2);
            // LOG(info) << nchi2 << "  " << chi2 << ",  dev: " << (x_l - det->hit_x);
            LOG(debug3) << "chi2calc y: " << det->GetDetectorName().Data() << " res: " << det->res_y << " pos: " << y_l
                        << " hit: " << hit->GetY() << " dev: " << y_l - hit->GetY() << " chi2: " << chi2;
            nchi2 += 1;
        }
    }
    gCandidate->SetChi2(chi2);
    return chi2;

    return 0;
}
*/
R3BFragmentFitterChi2S494::R3BFragmentFitterChi2S494() {}

R3BFragmentFitterChi2S494::~R3BFragmentFitterChi2S494() {}

void R3BFragmentFitterChi2S494::Init(R3BTPropagator* prop, Bool_t energyLoss)
{
    LOG(debug3) << "Call of init of R3BFragmentFitterChi2S494 !!!!!!!!!!!!!!" << endl;
    fPropagator = prop;
    gProp = prop;
    gEnergyLoss = energyLoss;

    // BACKWARD TRACKING:
/*
    // fMinimum = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
    fMinimum = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Simplex");

    // set tolerance , etc...
    fMinimum->SetMaxFunctionCalls(100000); // for Minuit/Minuit2
    fMinimum->SetMaxIterations(10000);     // for GSL
    fMinimum->SetTolerance(0.1);
    fMinimum->SetPrintLevel(0);
    // set minimization quality level for low (0), medium (1) and high (2) quality
    fMinimum->SetStrategy(1);
    // create funciton wrapper for minmizer
    // a IMultiGenFunction type
    ROOT::Math::Functor* f = new ROOT::Math::Functor(&Chi2Backward2D, 1);
    fMinimum->SetFunction(*f);
*/
    // FORWARD TRACKING:  
      
    //  minimum_m = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Simplex");
    // minimum_m = ROOT::Math::Factory::CreateMinimizer("Minuit", "Minimize"); 
      minimum_m = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
    // minimum_m = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Scan");
    // minimum_m = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Seek");

    // minimum_m = ROOT::Math::Factory::CreateMinimizer("GSLMultiMin","ConjugateFR");
    // minimum_m = ROOT::Math::Factory::CreateMinimizer("GSLMultiMin","BFGS");

    // set tolerance , etc...
    minimum_m->SetMaxFunctionCalls(100000); // for Minuit/Minuit2
    minimum_m->SetMaxIterations(100000);    // for GSL
    minimum_m->SetTolerance(0.1);           // 0.1);
    minimum_m->SetPrintLevel(0);
    minimum_m->SetStrategy(1);
    // create funciton wrapper for minmizer
    ROOT::Math::Functor* fm = new ROOT::Math::Functor(&Chi2MomentumForward, 3);
    minimum_m->SetFunction(*fm);
 /*   
 // global       
    minimum_m = ROOT::Math::Factory::CreateMinimizer("GSLSimAn", "");
    minimum_m->SetMaxFunctionCalls(100000);
    // Increase default search steps from 200 to 1000 for tough data fits
	ROOT::Math::GenAlgoOptions simanOpt;
	simanOpt.SetValue("n_tries", 100); // default 200
	simanOpt.SetValue("iters_per_tmp", 10); // default 10
	simanOpt.SetValue("t_initial", 10); // defaul 10

	ROOT::Math::MinimizerOptions opt;
	opt.SetExtraOptions(simanOpt);
	minimum_m->SetOptions(opt);

    ROOT::Math::Functor fg(&Chi2MomentumForward, 3);
    minimum_m->SetFunction(fg);
    */
}

Int_t R3BFragmentFitterChi2S494::FitTrack(R3BTrackingParticle* particle, R3BTrackingSetup* setup)
{
    gCandidate = particle;

    // ROOT::Math::Minimizer* minimum = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
    ROOT::Math::Minimizer* minimum = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Minimize");

    // set tolerance , etc...
    minimum->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2
    minimum->SetMaxIterations(10000);      // for GSL
    minimum->SetTolerance(0.01);
    minimum->SetStrategy(2);
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

Int_t R3BFragmentFitterChi2S494::FitTrackMomentumForwardMinuit(R3BTrackingParticle* particle, R3BTrackingSetup* setup)
{

    fPropagator->SetVis(kFALSE);

    Bool_t bsecond = false;

    LOG(debug3) << "In track momentum forward" << endl;
    gCandidate = particle;
    gSetup = setup;
    Double_t pbeam = 17.3915;
    Double_t pzbeam_optimized = 17.3915;
    Double_t theta0 = 0.0;
    Double_t phi0 = 0.0;
    Double_t dy_tof = 0.0, dy_fi23b = 0.0;
	Double_t pzmin = 0., pzmax = 0.;
    TVector3 pos0;
    TVector3 pos1;
    TVector3 pos2;
    TVector3 pos23a;
    TVector3 pos23b;
    TVector3 pos3;
    TVector3 pos3shift;
    TVector3 postofd;
    TVector3 direction0;
    TVector3 inputMomentum;
    TVector3 inputPosition;
    TVector3 startMomentum;
    TVector3 startPosition;
    TVector3 startPosOptimized;
    TVector3 nominal(0.0, 0.0, 1.0);
	
    Double_t x0 = gCandidate->GetStartPosition().X();
    Double_t y0 = gCandidate->GetStartPosition().Y();
    Double_t z0 = gCandidate->GetStartPosition().Z();
    Double_t px0 = gCandidate->GetStartMomentum().X();
    Double_t py0 = gCandidate->GetStartMomentum().Y();
    Double_t pz0 = gCandidate->GetStartMomentum().Z();
    Double_t ptot0 = sqrt(px0 * px0 + py0 * py0 + pz0 * pz0);
    Double_t mass = gCandidate->GetMass();
    Double_t x0start = x0;
    Double_t y0start = y0;    
    //Double_t pxbeam = gCandidate->GetBeamMomentum().X();
    //Double_t pybeam = gCandidate->GetBeamMomentum().Y();
    Double_t pxbeam = (-2.0324+129.288*x0)/1000.0;
    Double_t pybeam = (2.01651+129.268*y0)/1000.0;
    Double_t pzbeam = sqrt(pbeam*pbeam-pxbeam*pxbeam-pybeam*pybeam);
	Double_t px0_cand = px0, py0_cand = py0, pz0_cand = pz0;
	Double_t zfi23 = 91.2;
    LOG(debug3) << "In FitTrackMomentumForward" << endl;
    LOG(debug3) << "Start Momenta: " << px0 << ", " << py0 << ", " << pz0 << endl;
    LOG(debug3) << "Start Position: " << x0 << "; " << y0 << ", " << z0 << endl;
    
    // Construct the true, tilted beam direction vector 
	TVector3 inputBeamMomentum(pxbeam,pybeam,pzbeam); 
	TVector3 inputBeamDirection = inputBeamMomentum.Unit(); 
	gCandidate->SetBeamMomentum(inputBeamMomentum);    
    TVector3 input_rot_axis = nominal.Cross(inputBeamDirection);
	Double_t input_rot_angle  = nominal.Angle(inputBeamDirection);

    Double_t diffx = 0.0, diffy = 0.0; // correction for deviation in the magnetic field for protons 1 GeV

    if (gCandidate->GetMass() > 3.7 && gCandidate->GetMass() < 3.8) // alpha
    {
        diffx = 0.0093 * 0.996;      // standard field
        diffy = -0.000365 * (0.125); // 0.0002;// (1.9475601 + 0.02056205*pos23b.Y())*1.e-3*pos23b.Z()/pz0 ;
    }
    else //if (gCandidate->GetMass() > 11.1) //&& gCandidate->GetMass() < 11.2) // C && O
    {
        diffx = 0.0093 * 0.996;       // standard field
        diffy = -0.000365 * (-0.226); // 0.0002;//(1.9475601 + 0.02056205*pos23b.Y())*1.e-3*pos23b.Z()/pz0 ;
    }

    if (gCandidate->GetHitIndexByName("fi23a") > -1)
    {
        auto fi23a = gSetup->GetByName("fi23a");
        fi23a->LocalToGlobal(pos23a, gSetup->GetHit("fi23a", gCandidate->GetHitIndexByName("fi23a"))->GetX(), 0.0);
    }

    if (gCandidate->GetHitIndexByName("fi23b") > -1)
    {
        auto fi23b = gSetup->GetByName("fi23b");
        fi23b->LocalToGlobal(pos23b, 0.0, gSetup->GetHit("fi23b", gCandidate->GetHitIndexByName("fi23b"))->GetY());
        dy_fi23b = fi23b->res_y;
    }

    if (gCandidate->GetHitIndexByName("fi23a") > -1 && gCandidate->GetHitIndexByName("fi23b") > -1)
    {
       // auto fi23a = gSetup->GetByName("fi23a");

      //  Double_t xfi23b = x0 + (pos23a.X() - x0 - diffx) * pos23b.Z() / pos23a.Z();

      //  auto fi23b = gSetup->GetByName("fi23b");
      //  fi23b->LocalToGlobal(
      //      pos3, xfi23b, gSetup->GetHit("fi23b", gCandidate->GetHitIndexByName("fi23b"))->GetY() - diffy);
        pos3.SetX(pos23a.X()-x0-diffx);
        pos3.SetY(pos23b.Y()-y0-diffy);
        pos3.SetZ((pos23a.Z()+pos23b.Z()/2.));    
        if (input_rot_axis.Mag() > 1e-7) {
			pos3.Rotate(-input_rot_angle, input_rot_axis);
		}
	// Propagate along the beam frame to the detector plane (z = zfi23)
    // In the beam's local frame, the particle travels in a straight line 
    // from the origin to pos_beam. Therefore, the ratio of its coordinates is constant!
    // We scale the vector so that its Z component exactly matches the physical distance zfi23.
		zfi23 = (pos23a.Z()+pos23b.Z())/2.;
		double scale_factor = zfi23 / pos3.Z();
		pos3 *= scale_factor;           
    }
    Double_t radius0 = sqrt(pos3.X() * pos3.X() + 
                      pos3.Y() * pos3.Y() + 
                      pos3.Z() * pos3.Z());
                      
  // calculate momentum components in the beam frame 
    px0 = pos3.X() / radius0 * ptot0; 
	py0 = pos3.Y() / radius0 * ptot0; 
    pz0 = pos3.Z() / radius0 * ptot0;
    
/*    
    pos0.SetX(x0);
    pos0.SetY(x0);
    pos0.SetZ(z0);

    direction0 = pos3 - pos0;
    direction0.SetMag(ptot0);
    px0 = direction0.X();
    py0 = direction0.Y();
    pz0 = direction0.Z();
*/
    inputMomentum.SetX(px0);
    inputMomentum.SetY(py0);
    inputMomentum.SetZ(pz0);
    // Rotate the momentum vector BACK into the Lab Frame
	if (input_rot_axis.Mag() > 1e-7) {
		inputMomentum.Rotate(input_rot_angle, input_rot_axis);
	}

    inputPosition.SetX(x0);
    inputPosition.SetY(y0);
    inputPosition.SetZ(z0);

    gCandidate->SetStartPosition(inputPosition);

    gCandidate->SetStartMomentum(inputMomentum);

    gCandidate->Reset(); 
 
    if (x0 < -1.5)
        x0 = -1.5;
    if (y0 < -1.5)
        y0 = -1.5;
    if (x0 > 1.5)
        x0 = 1.5;
    if (y0 > 1.5)
        y0 = 1.5;

    LOG(debug) << "Start values momentum:     " << px0 << "  " << py0 << "  " << pz0 << endl;
    LOG(debug) << "Start values position:     " << x0 << "  " << y0 << "  " << z0 << endl;
    LOG(debug3) << "Pos3 x,y,z pos:           " << pos3.X() << "  " << pos3.Y() << "  " << pos3.Z() << endl;
    LOG(debug3) << "Direction x,y,z pos:      " << pos3.X() << "  " << pos3.Y() << "  " << pos3.Z() << endl;
    LOG(debug) << "Beam momentum:     " << pxbeam << "  " << pybeam << "  " << pzbeam << endl;
 /*      
    cout << "Start values momentum:     " << px0 << "  " << py0 << "  " << pz0 << endl;
    cout << "Start values position:     " << x0 << "  " << y0 << "  " << z0 << endl;
    cout << "Beam momentum:     " << pxbeam << "  " << pybeam << "  " << pzbeam << endl;
 */      
    pzmin = gCandidate->GetStartMomentum().Mag() * 0.7;
    pzmax = gCandidate->GetStartMomentum().Mag() * 1.3;
  
    Double_t variable[3] = { ptot0, x0, y0};
    Double_t stepmig[3] = { 0.001, 0.02, 0.02}; 
    Double_t lower[3] = {pzmin, -1.5, -1.5};
    Double_t upper[3] = {pzmax, 1.5, 1.5};

/*
 // GSL too slow
    Double_t pz0_stepgsl = (pzmax - pzmin) * 0.15;
    Double_t stepgsl[5] = { pz0_stepgsl, 0.45, 0.45, 0.08, 0.08 }; // for gsl    
    minimum_g->SetLimitedVariable(0, "pzgsl",     variable[0], stepgsl[0], lower[0], upper[0]);
    minimum_g->SetLimitedVariable(1, "x0gsl",     variable[1], stepgsl[1], lower[1], upper[1]);
    minimum_g->SetLimitedVariable(2, "y0gsl",     variable[2], stepgsl[2], lower[2], upper[2]);
    minimum_g->SetLimitedVariable(3, "pxbeamgsl", variable[3], stepgsl[3], lower[3], upper[3]);  
    minimum_g->SetLimitedVariable(4, "pybeamgsl", variable[4], stepgsl[4], lower[4], upper[4]); 
    
    if ((mass < 3.8 && mass > 3.7) ) // 4He
    {
        minimum_g->FixVariable(1); // x fixed
        minimum_g->FixVariable(3); 
        minimum_g->FixVariable(4); 
    }
   // if(mass > 4. && !(x0start == 0.)) minimum_g->FixVariable(1); // x fixed for iretrack==1
    minimum_g->FixVariable(2); 
    minimum_g->FixVariable(3); 
    minimum_g->FixVariable(4); 
    
// Start candidate = Init candidate, as we haven't changed anything in between
    py0_cand = gCandidate->GetStartMomentum().Y();
    px0_cand = gCandidate->GetStartMomentum().X();
    pz0_cand = gCandidate->GetStartMomentum().Z();
    
    startMomentum.SetX(px0_cand);
    startMomentum.SetY(py0_cand);
    startMomentum.SetZ(pz0_cand);

    startPosition.SetX(gCandidate->GetStartPosition().X());
    startPosition.SetY(gCandidate->GetStartPosition().Y());
    startPosition.SetZ(gCandidate->GetStartPosition().Z());
    LOG(debug) << "Start Position:     " << gCandidate->GetStartPosition().X() << "  Start Momentum: " << pz0_cand << endl;
   
    gCandidate->SetStartPosition(startPosition);

    gCandidate->SetStartMomentum(startMomentum);
        
    gCandidate->Reset();
    
    Double_t oldChi = 0.0;
    Double_t ptot0_cand;
    Int_t status0 = 0;
    
	// do the minimization
	minimum_g->Minimize();

	status0 = minimum_g->Status();
	LOG(debug3) << "Status0: " << status0 << endl;
	
	px0_cand = (pos23a.X() - minimum_g->X()[1] - diffx) / pos23a.Z() * minimum_g->X()[0];
	py0_cand = (pos23b.Y() - minimum_g->X()[2] - diffy) / pos23b.Z() * minimum_g->X()[0];
	//correction for incomming-beam angle:
	px0_cand = px0_cand - gCandidate->GetMass() / 15.01235 * minimum_g->X()[3]; 
	py0_cand = py0_cand - gCandidate->GetMass() / 15.01235 * minimum_g->X()[4];;
	pz0_cand = minimum_g->X()[0];

	LOG(debug3) << "optimized momentum: " << px0_cand << "  " << py0_cand << "  " << pz0_cand << endl;
	LOG(debug3) << "optimized position: " << minimum_g->X()[1] << "  " << minimum_g->X()[2] << "  "
				<< "0. " << endl;
	pzbeam_optimized = sqrt(pbeam*pbeam - minimum_g->X()[3] * minimum_g->X()[3] -
									  minimum_g->X()[4] *minimum_g->X()[4]); 
	LOG(debug3) << "optimized beam momentum: " << minimum_g->X()[3] << "  " << minimum_g->X()[4] << " " <<pzbeam_optimized<<endl;
	
	TVector3 startPositionOptimized(minimum_g->X()[1], minimum_g->X()[2], 0.);
	TVector3 startMomentumOptimized(px0_cand, py0_cand, pz0_cand);
	TVector3 startBeamMomentumOptimized(minimum_g->X()[3],minimum_g->X()[4],pzbeam_optimized);

	oldChi = minimum_g->MinValue();
	gCandidate->SetChi2(oldChi);
	gCandidate->SetStartPosition(startPositionOptimized);
	gCandidate->SetStartMomentum(startMomentumOptimized);
	gCandidate->SetBeamMomentum(startBeamMomentumOptimized);
	gCandidate->Reset();
	
	minimum_g->Clear();
	
	if(oldChi > 100.){
		return 20;
	}
*/         
// GSL too slow, we will do only migrad:
    Int_t status = 0;
    Double_t ptot0minuit = gCandidate->GetStartMomentum().Mag();
    Double_t x0minuit = gCandidate->GetStartPosition().X();
    Double_t y0minuit = gCandidate->GetStartPosition().Y();
    Double_t pxbeamminuit = gCandidate->GetBeamMomentum().X();
    Double_t pybeamminuit = gCandidate->GetBeamMomentum().Y();
   
    pxbeamminuit = (-2.0324+129.288*x0minuit)/1000.0;
    pybeamminuit = (2.01651+129.268*y0minuit)/1000.0;
    minimum_m->SetLimitedVariable(0, "ptot", ptot0minuit, stepmig[0], lower[0], upper[0]);
    minimum_m->SetLimitedVariable(1, "x0", x0minuit, stepmig[1], lower[1], upper[1]);    
    minimum_m->SetLimitedVariable(2, "y0", y0minuit, stepmig[2], lower[2], upper[2]);
 
// y0 fixed to y0geom 
// x0 fixed to 0 for iretrack=0, and to xcorrected for iretrack=1 
    if(!(x0start == 0.0)) minimum_m->FixVariable(1); // x fixed
    //minimum_m->FixVariable(1);
    minimum_m->FixVariable(2); // y fixed   

    if ((mass < 3.8 && mass > 3.7) ) // 4He
    {        
		minimum_m->FixVariable(1); 
        minimum_m->FixVariable(2);
    }
        
    minimum_m->Minimize();

	status = minimum_m->Status();
	LOG(debug3) << "Status: " << status << endl;
    Double_t px0beam_optimized = (-2.0324+129.288*minimum_m->X()[1])*1.e-3;//minimum_m->X()[3];
    Double_t py0beam_optimized = (2.01651+129.268*minimum_m->X()[2])*1.e-3;//minimum_m->X()[4];
// with option b, we get from tracker momenta in lab frame, thus no correction for beam angle here
	pzbeam_optimized = sqrt(pbeam*pbeam - px0beam_optimized * px0beam_optimized -
										  py0beam_optimized * py0beam_optimized); 
										  
 // tranform from angled-beam system to lab system:  	
    TVector3 FinalstartBeamMomentumOptimized(px0beam_optimized,py0beam_optimized,pzbeam_optimized);
    TVector3 FinalBeamDirection = FinalstartBeamMomentumOptimized.Unit();
    TVector3 fragment_local(pos23a.X() - minimum_m->X()[1] - diffx,pos23b.Y() - minimum_m->X()[2] - diffy,zfi23);
	
    TVector3 rot_axis = nominal.Cross(FinalBeamDirection);
	double rot_angle  = nominal.Angle(FinalBeamDirection);
	if (rot_axis.Mag() > 1e-7) {
		fragment_local.Rotate(-rot_angle, rot_axis);
	}									  
	double scale_factor = zfi23 / fragment_local.Z();
	fragment_local *= scale_factor;
	
	Double_t radius = sqrt(fragment_local.X() * fragment_local.X() + 
                      fragment_local.Y() * fragment_local.Y() + 
                      fragment_local.Z() * fragment_local.Z());
				
	px0_cand = fragment_local.X() / radius * minimum_m->X()[0];
	py0_cand = fragment_local.Y() / radius * minimum_m->X()[0];
	pz0_cand = fragment_local.Z() / radius * minimum_m->X()[0];
  
	TVector3 FinalstartMomentumOptimized(px0_cand, py0_cand, pz0_cand);
	// rotate back to lab frame
	if (rot_axis.Mag() > 1e-7) {
			FinalstartMomentumOptimized.Rotate(rot_angle, rot_axis);
	}

	LOG(debug3) << "local optimized momentum: " << px0_cand << "  " << py0_cand << "  " << pz0_cand << endl;
	LOG(debug3) << "lab optimized momentum: " << fragment_local.X() << "  " << fragment_local.Y() << "  " << fragment_local.Z() << endl;
	LOG(debug3) << "optimized position: " << minimum_m->X()[1] << "  " << minimum_m->X()[2] << "  "
				<< "0. " << endl;
	LOG(debug3) << "optimized beam momentum: " << px0beam_optimized << "  " << py0beam_optimized << " " <<pzbeam_optimized<<endl;
	 
	TVector3 FinalstartPositionOptimized(minimum_m->X()[1], minimum_m->X()[2], 0.);

	Double_t newChi = minimum_m->MinValue();
	gCandidate->SetChi2(newChi);
	gCandidate->SetStartPosition(FinalstartPositionOptimized);
	gCandidate->SetStartMomentum(FinalstartMomentumOptimized);
	gCandidate->SetBeamMomentum(FinalstartBeamMomentumOptimized);
	gCandidate->Reset();

  /*   
	cout << "local optimized momentum: " << px0_cand << "  " << py0_cand << "  " << pz0_cand << endl;
	cout << "lab optimized momentum: " << fragment_local.X() << "  " << fragment_local.Y() << "  " << fragment_local.Z() <<endl;
	cout << "rotation angle: " << rot_angle * TMath::RadToDeg() <<" deg"<<endl;
	cout << "optimized position: " << minimum_m->X()[1] << "  " << minimum_m->X()[2] << endl;
	cout << "optimized beam momentum: " << px0beam_optimized << "  " << py0beam_optimized << " " <<pzbeam_optimized<<endl;
	cout << "current chi: " << newChi << endl;
*/	
    
    if (mass > 40.)
    {
       // cout << "For gsl parameters: " << pz0minuit << ", " << x0minuit << ", " << y0minuit
       //      << ", and chi2: " << oldChi << endl;
		cout << "For migrad parameters: " << minimum_m->X()[0] << ", " << minimum_m->X()[1] << ", " << minimum_m->X()[2]
             << ", and chi2: " << newChi << endl;
        cout << "Correlation between pz and x0: " << minimum_m->Correlation(0, 1)
             << ", pz and y0: " << minimum_m->Correlation(0, 2) << ", x0 and y0: " << minimum_m->Correlation(1, 2)
             << endl;
    }
    
   
    minimum_m->Clear();

    return status;
}
// ***********************************************************************************************************
// =========================================================================
// HELPER LAMBDAS
// =========================================================================
auto enforceSymmetry = [](TMatrixD& P) {
    for (Int_t i = 0; i < 5; ++i) {
        for (Int_t j = i + 1; j < 5; ++j) {
            Double_t avg = 0.5 * (P(i, j) + P(j, i));
            P(i, j) = avg;
            P(j, i) = avg;
        }
    }
};

auto computeProcessNoise = [](TMatrixD& Q, Double_t L_over_X0, Double_t qOverP_abs, 
                              Double_t beta, Double_t p_tot_GeV, Double_t mass, 
                              Double_t sigma_E_sq, Double_t ux, Double_t uy, 
                              Double_t det_thick) {
    Q.Zero();
    if (L_over_X0 <= 1e-7 || beta <= 0.0 || p_tot_GeV <= 0.0) return;

    Double_t theta_scat = (0.0136 * qOverP_abs / beta) * TMath::Sqrt(L_over_X0) 
                          * (1.0 + 0.038 * TMath::Log(L_over_X0));
    Double_t var_scat = theta_scat * theta_scat;

    Double_t xi = 1.0 + ux * ux + uy * uy;
    Double_t var_ux = (1.0 + ux * ux) * xi * var_scat;
    Double_t var_uy = (1.0 + uy * uy) * xi * var_scat;

    Double_t var_pos = (det_thick * det_thick / 3.0) * var_scat;
    Double_t cov_pos_slope = (det_thick / 2.0) * var_scat;

    Double_t E_tot = TMath::Sqrt(p_tot_GeV * p_tot_GeV + mass * mass);
    Double_t sigma_p_sq = (E_tot / p_tot_GeV) * (E_tot / p_tot_GeV) * sigma_E_sq;
    Double_t var_qOverP = (qOverP_abs * qOverP_abs) * (sigma_p_sq / (p_tot_GeV * p_tot_GeV));

    Q(0, 0) = var_pos;          Q(1, 1) = var_pos;
    Q(2, 2) = var_ux;           Q(3, 3) = var_uy;
    Q(0, 2) = cov_pos_slope;    Q(2, 0) = cov_pos_slope;
    Q(1, 3) = cov_pos_slope;    Q(3, 1) = cov_pos_slope;
    Q(4, 4) = var_qOverP;
};

auto josephFormUpdate = [](TMatrixD& P, const TMatrixD& K, const TMatrixD& H, const TMatrixD& R, auto& enforceSym) {
    TMatrixD I(5, 5); I.UnitMatrix();
    TMatrixD I_minus_KH = I - (K * H);
    TMatrixD I_minus_KH_T(TMatrixD::kTransposed, I_minus_KH);
    TMatrixD K_T(TMatrixD::kTransposed, K);

    P = (I_minus_KH * P * I_minus_KH_T) + (K * R * K_T);
    enforceSym(P);
};

auto calculatePulls = [](TMatrixD& Pull, const TMatrixD& residual, const TMatrixD& S_mat) {
    Int_t measDim = residual.GetNrows(); 
    Double_t var_x = S_mat(0, 0);
    if (var_x > 0.0) Pull(0,0) = residual(0,0) / TMath::Sqrt(var_x);
    if (measDim == 2) {
        Double_t var_y = S_mat(1, 1);
        if (var_y > 0.0) Pull(1,0) = residual(1,0) / TMath::Sqrt(var_y);
    }
};

Int_t R3BFragmentFitterChi2S494::FitTrackMomentumKalmanFilter(R3BTrackingParticle* particle, R3BTrackingSetup* setup)
{
    gCandidate = particle;
    gSetup = setup;
    Int_t status = 0;
    Int_t measDim = 1;
    Double_t chi2 = 0., chi2_first = 0., chi2_second = 0., chi2_third = 0.;
    Double_t beta_target = 0.;
    Double_t x0_start=0., y0_start=0.;
    TVector3 pos0, pos2, pos3, pos23a, pos23b, direction0, inputPosition, inputMomentum;
    TVector3 pos_target, mom_target, p_new_global, pos_new_global, p_l, inputMomentumLocal;
    Double_t y_tp = 0.;
    Double_t yfi30=-100.,yfi31=-100.,yfi32=-100.,yfi33=-100.;
    Double_t foffset=0.441;
    Double_t fslope=1.;
    Double_t ltofd=0.;
    Double_t ztp = 174.5851;
    Double_t x_l = 0., y_l = 0.;
    
    TMatrixD x_state(5, 1);
    TMatrixD P_cov(5, 5);
    TMatrixD Q_mat(5, 5);

    Bool_t dumpResiduesStep1 = false, dumpResiduesStep2 = false, dumpResiduesStep3 = false, dumpResiduesStep4 = true;
    Double_t mass = gCandidate->GetMass();
    Double_t charge = gCandidate->GetCharge();
    Bool_t helium = (charge == 2.0);

    Double_t x0 = gCandidate->GetStartPosition().X();
    Double_t y0 = gCandidate->GetStartPosition().Y();
    Double_t z0 = gCandidate->GetStartPosition().Z();
    Double_t px0 = gCandidate->GetStartMomentum().X();
    Double_t py0 = gCandidate->GetStartMomentum().Y();
    Double_t pz0 = gCandidate->GetStartMomentum().Z();
    Double_t ptot0 = TMath::Sqrt(px0 * px0 + py0 * py0 + pz0 * pz0);
    
    x0_start = x0;
	y0_start = y0;
	  
    Double_t diffx = 0.0093 * 0.996;
    Double_t diffy = 0.000365 * (-0.125);

    if (gCandidate->GetHitIndexByName("fi23a") > -1) {
        auto fi23a = gSetup->GetByName("fi23a");
        fi23a->LocalToGlobal(pos23a, gSetup->GetHit("fi23a", gCandidate->GetHitIndexByName("fi23a"))->GetX() - diffx, 0.0);
    }
    if (gCandidate->GetHitIndexByName("fi23b") > -1) {
        auto fi23b = gSetup->GetByName("fi23b");
        fi23b->LocalToGlobal(pos23b, 0.0, gSetup->GetHit("fi23b", gCandidate->GetHitIndexByName("fi23b"))->GetY() - diffy);
    } 

    pos0.SetXYZ(x0, y0, z0);
    pos3.SetXYZ(pos23a.X(), pos23b.Y(), (pos23a.Z() + pos23b.Z()) / 2.0);   
    direction0 = (pos3 - pos0).Unit() * ptot0;

    inputMomentum = direction0;
    inputPosition = pos0;
    gCandidate->SetStartPosition(inputPosition);
    gCandidate->SetStartMomentum(inputMomentum);   
    gCandidate->Reset();    

/*
    if (gCandidate->GetHitIndexByName("tofd") > -1)
    {
        auto tofd = gSetup->GetByName("tofd");
        tofd->LocalToGlobal(pos0,
                            gSetup->GetHit("tofd", gCandidate->GetHitIndexByName("tofd"))->GetX(),
                            gSetup->GetHit("tofd", gCandidate->GetHitIndexByName("tofd"))->GetY());
        ltofd = sqrt((pos0.Z()-ztp)*(pos0.Z()-ztp) + pos0.X()*pos0.X()) ;
    }
    
    if (gCandidate->GetHitIndexByName("fi23b") > -1)
    {
        auto fi23b = gSetup->GetByName("fi23b");
        fi23b->LocalToGlobal(pos23b, 0.0, gSetup->GetHit("fi23b", gCandidate->GetHitIndexByName("fi23b"))->GetY());
    }
 
 Double_t diffx = 0.;
 // Extrapolate y position between fi23b and tofd to fi3x:   
    if (gCandidate->GetHitIndexByName("fi32") > -1 && gCandidate->GetHitIndexByName("fi30") > -1)
    {
		diffx = 0.;//-0.7;
        auto fi32 = gSetup->GetByName("fi32");
        auto fi30 = gSetup->GetByName("fi30");
        fi32->LocalToGlobal(pos2, gSetup->GetHit("fi32", gCandidate->GetHitIndexByName("fi32"))->GetX()-diffx, 0.);
        fi30->LocalToGlobal(pos3, gSetup->GetHit("fi30", gCandidate->GetHitIndexByName("fi30"))->GetX(), 0.);
        
        fslope = 0.883;
        y_tp = (pos0.Y()-foffset+fslope*ltofd*pos23b.Y()/(ztp-pos23b.Z()))/
														 (1.+fslope*ltofd/(ztp-pos23b.Z()));  
	    Double_t lfi30 = sqrt((pos3.Z()-ztp)*(pos3.Z()-ztp) + pos3.X()*pos3.X()) ;			
		yfi30 = y_tp + lfi30/(ztp-pos23b.Z())*(y_tp-pos23b.Y())*fslope + foffset; 
		pos3.SetY(yfi30);
	    
	    Double_t lfi32 = sqrt((pos2.Z()-ztp)*(pos2.Z()-ztp) + pos2.X()*pos2.X()) ;			
		yfi32 = y_tp + lfi32/(ztp-pos23b.Z())*(y_tp-pos23b.Y())*fslope + foffset;
		pos2.SetY(yfi32);
    }
    else if (gCandidate->GetHitIndexByName("fi31") > -1 && gCandidate->GetHitIndexByName("fi33") > -1)
    {
        diffx = 0.;//0.53;
        auto fi31 = gSetup->GetByName("fi31");
        auto fi33 = gSetup->GetByName("fi33");
        fi33->LocalToGlobal(pos2, gSetup->GetHit("fi33", gCandidate->GetHitIndexByName("fi33"))->GetX()-diffx, 0.);
        fi31->LocalToGlobal(pos3, gSetup->GetHit("fi31", gCandidate->GetHitIndexByName("fi31"))->GetX(), 0.);
        
        fslope = 0.901;
        y_tp = (pos0.Y()-foffset+fslope*ltofd*pos23b.Y()/(ztp-pos23b.Z()))/
														 (1.+fslope*ltofd/(ztp-pos23b.Z()));  
	    Double_t lfi31 = sqrt((pos3.Z()-ztp)*(pos3.Z()-ztp) + pos3.X()*pos3.X()) ;			
		yfi31 = y_tp + lfi31/(ztp-pos23b.Z())*(y_tp-pos23b.Y())*fslope + foffset; 
		pos3.SetY(yfi31);
	    
	    Double_t lfi33 = sqrt((pos2.Z()-ztp)*(pos2.Z()-ztp) + pos2.X()*pos2.X()) ;			
		yfi33 = y_tp + lfi33/(ztp-pos23b.Z())*(y_tp-pos23b.Y())*fslope + foffset;
		pos2.SetY(yfi33);
    }
    else
    {
        return 10;
    }
      Double_t ygeom = pos23b.Y() - pos23b.Z() * (y_tp - pos23b.Y()) / (ztp - pos23b.Z());

       // Pass inital candidate through detectors, correct for energy loss, and get start momentum at tofd:	
		
	for (Int_t i = 0; i < gSetup->GetArray().size();  i++) 
	{
		auto det = gSetup->GetArray().at(i); 
		R3BHit* hit = nullptr; 
		Int_t hitIndex = gCandidate->GetHitIndexByName(det->GetDetectorName().Data());
		
		if(hitIndex < 0) continue;
		
		if (hitIndex >= 0)
		{
			hit = gSetup->GetHit(det->GetDetectorName().Data(), hitIndex);
		}

		if (!hit) continue;
		
		Double_t weight = 1.;
		if (kTarget == det->section || det->GetDetectorName() == "tofd")
		{
			weight = 0.5;
		}
	   // Update particle's momentum is due to energy loss in a detector:
		gCandidate->PassThroughDetector(det, weight); 			
	} 
	
	Double_t mom = gCandidate->GetMomentum().Mag();
	direction0 = (pos2 - pos0);
	
	direction0 = direction0.Unit();
	
	direction0.SetMag(mom);
	
	inputMomentum = direction0;

	px0 = inputMomentum.X();
	py0 = inputMomentum.Y();
	pz0 = inputMomentum.Z();

	x0 = pos0.X();
	y0 = pos0.Y();
	z0 = pos0.Z();
	
	gCandidate->SetCharge(-charge);
	gCandidate->SetStartPosition(pos0);
	gCandidate->SetStartMomentum(inputMomentum);
	gCandidate->Reset();
	
	if (gCandidate->GetHitIndexByName("tofd") > -1)
	{
		auto tofd = gSetup->GetByName("tofd");
		tofd->GlobalToLocalMomentum(inputMomentum,inputMomentumLocal);
		tofd->GlobalToLocal(pos0,x_l,y_l);
	}
	
	// --- BEFORE THE DETECTOR LOOP ---
	// Initialize local state at the tofd
	x_state(0,0) = x_l; // initial x 
	x_state(1,0) = y_l; // initial y
	x_state(2,0) = inputMomentumLocal.X() / inputMomentumLocal.Z(); // initial slope dx/dz
	x_state(3,0) = inputMomentumLocal.Y() / inputMomentumLocal.Z(); // initial slope dy/dz
	x_state(4,0) = abs(gCandidate->GetCharge()) / inputMomentumLocal.Mag(); // q/p 
	
	
	// Initialize starting uncertainties (high values mean "I trust the hits more than my guess")
	Double_t x_err_targ = (helium ? 0.01 : 1.5);
	Double_t y_err_targ = (helium ? 0.01 : 1.5);
	P_cov.Zero();
	Double_t x_err = 2.025 * 2. / sqrt(12.);
	P_cov(0,0) = x_err * x_err; 
	Double_t y_err = (helium ? 1. * 3. : 1.);
	P_cov(1,1) = y_err * y_err; 
	P_cov(2,2) = 5.e-2 * 5.e-2;//0.05 * 0.05 ;     
	P_cov(3,3) = 5.e-2 * 5.e-2;//0.05 * 0.05 ;     
	Double_t qp_err = (helium ? 0.03 : 0.01) * charge / inputMomentumLocal.Mag();
    P_cov(4,4) = qp_err * qp_err;
*/
    
    x_state(0,0) = x0;
    x_state(1,0) = y0;
    x_state(2,0) = inputMomentum.X() / inputMomentum.Z();
    x_state(3,0) = inputMomentum.Y() / inputMomentum.Z();
    x_state(4,0) = charge / inputMomentum.Mag();
	
    P_cov.Zero();
    TMatrixD R_target(2,2); R_target.Zero();
	Double_t x_err_targ = 0.5, y_err_targ = 0.5;
	Double_t qp_err = (helium ? 0.03 : 0.01) * charge / inputMomentum.Mag();
	if (!helium) 
	{
		x_err_targ = 0.5;
		y_err_targ = 0.5;
		P_cov(0,0) = x_err_targ * x_err_targ;
		P_cov(1,1) = y_err_targ * y_err_targ;
		P_cov(2,2) = 5.e-1 * 5.e-1;
		P_cov(3,3) = 5.e-1 * 5.e-1;
		P_cov(4,4) = qp_err * qp_err;
		
		R_target(0,0) = P_cov(0,0);
		R_target(1,1) = P_cov(1,1);
		R_target(0,1) = 0.;
		R_target(1,0) = 0.;
		
	}
	else
	{
		//cout<<"fHasCarbonVertexCov: "<<fHasCarbonVertexCov<<endl;
		//cout<<"fbestCarbonTargetCov: "<<fbestCarbonTargetCov(0,0)<<" "<<fbestCarbonTargetCov(1,1)<<" "<<
		//fbestCarbonTargetCov(2,2)<<" "<<fbestCarbonTargetCov(3,3)<<" "<<fbestCarbonTargetCov(4,4)<<endl;
		if (fHasCarbonVertexCov) {
			// Set spatial uncertainties directly from Carbon's stored vertex error
			P_cov(0, 0) = fbestCarbonTargetCov(0, 0); // Var(x)
			P_cov(0, 1) = fbestCarbonTargetCov(0, 1); // Cov(x, y)
			P_cov(1, 0) = fbestCarbonTargetCov(1, 0); // Cov(y, x)
			P_cov(1, 1) = fbestCarbonTargetCov(1, 1); // Var(y)

			// Set initial direction / momentum variances
			P_cov(2,2) = 5.e-1 * 5.e-1;
			P_cov(3,3) = 5.e-1 * 5.e-1;
			P_cov(4,4) = qp_err * qp_err;

			// Target measurement noise R for Helium
			R_target(0, 0) = fbestCarbonTargetCov(0, 0);
			R_target(0, 1) = fbestCarbonTargetCov(0, 1);
			R_target(1, 0) = fbestCarbonTargetCov(1, 0);
			R_target(1, 1) = fbestCarbonTargetCov(1, 1);
		}
	}
	
    // --- STEP 1: FORWARD PASS ---

    //cout<<"FIRST STEP FOR CHARGE: "<<charge<<endl;
    
    for (Int_t i = 0; i < (Int_t)gSetup->GetArray().size(); i++) {
        auto det = gSetup->GetArray().at(i);          
        Int_t hitIndex = gCandidate->GetHitIndexByName(det->GetDetectorName().Data());        
        if (hitIndex < 0) continue;
        R3BHit* hit = gSetup->GetHit(det->GetDetectorName().Data(), hitIndex);
        if (!hit) continue;        

        measDim = (det->GetDetectorName() == "tofd" || kTarget == det->section) ? 2 : 1;
        TMatrixD z_meas(measDim, 1), H_mat(measDim, 5), R_noise(measDim, measDim);
        z_meas.Zero(); H_mat.Zero(); R_noise.Zero();

        if (det->GetDetectorName() == "tofd") {
            H_mat(0,0) = 1.0; H_mat(1,1) = 1.0;
            R_noise(0,0) = det->res_x * det->res_x * 4./12.;
            R_noise(1,1) = det->res_y * det->res_y * (helium ? 9.0 : 1.0);
            z_meas(0,0) = hit->GetX(); z_meas(1,0) = hit->GetY();
        } else if (kTarget == det->section) {
            H_mat(0,0) = 1.0; H_mat(1,1) = 1.0;
            R_noise(0,0) = R_target(0,0);
			R_noise(0,1) = R_target(0,1);
			R_noise(1,1) = R_target(1,1);
			R_noise(1,0) = R_target(1,0);
			z_meas(0,0) = gCandidate->GetStartPosition().X();
            z_meas(1,0) = gCandidate->GetStartPosition().Y();             
        } else if (det->GetDetectorName() == "fi23b") {
            H_mat(0,1) = 1.0;
            R_noise(0,0) = det->res_y * det->res_y * 4./12.;
            z_meas(0,0) = hit->GetY();
        } else {
            H_mat(0,0) = 1.0; 
            R_noise(0,0) = det->res_x * det->res_x * 4./12.;
            z_meas(0,0) = hit->GetX();
        }         

        gProp->PropagateToDetectorForward(gCandidate, det, P_cov);         
        Double_t weight = (kTarget == det->section || det->GetDetectorName() == "tofd") ? 0.5 : 1.0;
        Double_t sigma_E_squared = 0.0;
        if (gEnergyLoss) {
            sigma_E_squared = weight * det->GetEnergyLossStraggling(gCandidate);
            gCandidate->PassThroughDetector(det, weight); 
        }                                       

        det->GlobalToLocal(gCandidate->GetPosition(), x_l, y_l);
        det->GlobalToLocalMomentum(gCandidate->GetMomentum(), p_l);     
        x_state(0,0) = x_l; 
        x_state(1,0) = y_l;
        x_state(2,0) = p_l.X() / p_l.Z(); 
        x_state(3,0) = p_l.Y() / p_l.Z();
        x_state(4,0) = TMath::Abs(charge) / p_l.Mag();
        
        if (std::abs(x_state(4,0)) < 1e-6 || std::abs(x_state(2,0)) > 5.0 || std::abs(x_state(3,0)) > 5.0) {
			return 10;
		}
        enforceSymmetry(P_cov);

        Double_t L_over_X0 = weight * det->thickness / rad_length;
        Double_t p_tot = gCandidate->GetMomentum().Mag();
        Double_t E_tot = TMath::Sqrt(p_tot * p_tot + mass * mass);
        computeProcessNoise(Q_mat, L_over_X0, x_state(4,0), p_tot / E_tot, p_tot, mass, 
                            sigma_E_squared, x_state(2,0), x_state(3,0), det->thickness);
        P_cov += Q_mat;     

        TMatrixD S_mat = H_mat * P_cov * TMatrixD(TMatrixD::kTransposed, H_mat) + R_noise;                     
        TMatrixD residual = z_meas - (H_mat * x_state);         
        TMatrixD pulls(measDim, 1);
        calculatePulls(pulls, residual, S_mat);     

        if (dumpResiduesStep1) {
            if (measDim == 2) {
                gCandidate->SetResidual(det->GetDetectorName(), residual(0,0), residual(1,0), pulls(0,0), pulls(1,0));
            } else {
                if (det->GetDetectorName() == "fi23b") gCandidate->SetResidual(det->GetDetectorName(), 0.0, residual(0,0), 0., pulls(0,0));
                else gCandidate->SetResidual(det->GetDetectorName(), residual(0,0), 0.0, pulls(0,0), 0.);
            }
        }           

        TMatrixD S_inv = S_mat; 
        // Perform safe inversion check
		if (!SafeInvertMatrix(S_inv)) {
			// Inversion failed due to degenerate/noisy data in this event!
			// Safely skip this update step, drop this track candidate, or return false.
			continue; 
		}
        //S_inv.Invert();
        TMatrixD K_gain = P_cov * TMatrixD(TMatrixD::kTransposed, H_mat) * S_inv;
        x_state += (K_gain * residual);
         if (std::abs(x_state(4,0)) < 1e-6 || std::abs(x_state(2,0)) > 5.0 || std::abs(x_state(3,0)) > 5.0) {
			return 10;
		}
        josephFormUpdate(P_cov, K_gain, H_mat, R_noise, enforceSymmetry);

        //TMatrixD chi2_inc = TMatrixD(TMatrixD::kTransposed, residual) * S_inv * residual;
        TMatrixD R_inv = R_noise; 
        // Perform safe inversion check
		if (!SafeInvertMatrix(R_inv)) {
			continue; 
		}
		//R_inv.Invert();
        TMatrixD residual_updated = z_meas - (H_mat * x_state);         
        TMatrixD chi2_inc = TMatrixD(TMatrixD::kTransposed, residual_updated) * R_inv * residual_updated;
        chi2_first += chi2_inc(0,0);         

        Double_t ptot_new = TMath::Abs(charge) / x_state(4,0);
        Double_t ux_new = x_state(2,0), uy_new = x_state(3,0);
        Double_t pz_new_local = ptot_new / TMath::Sqrt(1.0 + ux_new * ux_new + uy_new * uy_new);
        TVector3 p_new_local(ux_new * pz_new_local, uy_new * pz_new_local, pz_new_local);         

        det->LocalToGlobalMomentum(p_new_global, p_new_local); 
        det->LocalToGlobal(pos_new_global, x_state(0,0), x_state(1,0));         
        gCandidate->SetPosition(pos_new_global);
        gCandidate->SetMomentum(p_new_global);
        gCandidate->SetBeta(p_new_global.Mag() / TMath::Sqrt(p_new_global.Mag() * p_new_global.Mag() + mass * mass));  
    } 
    
    //cout<<"SECOND STEP FOR CHARGE: "<<charge<<endl;  

    // --- STEP 2: BACKWARD PASS ---
    gCandidate->SetMomentum(-p_new_global);
    gCandidate->SetPosition(pos_new_global);
    gCandidate->SetCharge(-charge);        

    for (Int_t i = (Int_t)gSetup->GetArray().size() - 1; i >= 0; i--) {
        auto det = gSetup->GetArray().at(i);  
        Int_t hitIndex = gCandidate->GetHitIndexByName(det->GetDetectorName().Data());      
        if (hitIndex < 0) continue;
        R3BHit* hit = gSetup->GetHit(det->GetDetectorName().Data(), hitIndex);
        if (!hit) continue;     

        gProp->PropagateToDetectorBackward(gCandidate, det, P_cov);         
        Double_t weight = (kTarget == det->section || det->GetDetectorName() == "tofd") ? 0.5 : 1.0;
        Double_t sigma_E_squared = 0.0;
        if (gEnergyLoss) {
            sigma_E_squared = weight * det->GetEnergyLossStraggling(gCandidate);
            gCandidate->PassThroughDetectorBackward(det, weight);               
        }                                                   

        det->GlobalToLocal(gCandidate->GetPosition(), x_l, y_l);
        det->GlobalToLocalMomentum(gCandidate->GetMomentum(), p_l);         
        x_state(0,0) = x_l; x_state(1,0) = y_l;
        x_state(2,0) = p_l.X() / p_l.Z(); x_state(3,0) = p_l.Y() / p_l.Z();
        x_state(4,0) = TMath::Abs(charge) / p_l.Mag();
         if (std::abs(x_state(4,0)) < 1e-6 || std::abs(x_state(2,0)) > 5.0 || std::abs(x_state(3,0)) > 5.0) {
			return 10;
		}
        enforceSymmetry(P_cov);                 

        Double_t L_over_X0 = weight * det->thickness / rad_length;
        Double_t p_tot = gCandidate->GetMomentum().Mag();
        Double_t E_tot = TMath::Sqrt(p_tot * p_tot + mass * mass);
        computeProcessNoise(Q_mat, L_over_X0, x_state(4,0), p_tot / E_tot, p_tot, mass, 
                            sigma_E_squared, x_state(2,0), x_state(3,0), det->thickness);           
        P_cov += Q_mat;                             

        measDim = (i == 0 || det->GetDetectorName() == "tofd") ? 2 : 1;
        TMatrixD z_meas(measDim, 1), H_mat(measDim, 5), R_noise(measDim, measDim);
        z_meas.Zero(); H_mat.Zero(); R_noise.Zero();

        if (det->GetDetectorName() == "tofd") {
            H_mat(0,0) = 1.0; H_mat(1,1) = 1.0;
            R_noise(0,0) = det->res_x * det->res_x * 4./12.;
            R_noise(1,1) = det->res_y * det->res_y * (helium ? 9.0 : 1.0);
            z_meas(0,0) = hit->GetX(); z_meas(1,0) = hit->GetY();
        } else if (kTarget == det->section) {
            H_mat(0,0) = 1.0; H_mat(1,1) = 1.0;
			R_noise(0,0) = R_target(0,0);
			R_noise(0,1) = R_target(0,1);
			R_noise(1,1) = R_target(1,1);
			R_noise(1,0) = R_target(1,0);
            z_meas(0,0) = x0_start; z_meas(1,0) = y0_start;
        } else if (det->GetDetectorName() == "fi23b") {
            H_mat(0,1) = 1.0;
            R_noise(0,0) = det->res_y * det->res_y * 4./12.;
            z_meas(0,0) = hit->GetY();
        } else {
            H_mat(0,0) = 1.0;
            R_noise(0,0) = det->res_x * det->res_x * 4./12.;
            z_meas(0,0) = hit->GetX();
        }           

        TMatrixD S_mat = H_mat * P_cov * TMatrixD(TMatrixD::kTransposed, H_mat) + R_noise;                     
        TMatrixD residual = z_meas - (H_mat * x_state);                                 
        TMatrixD pulls(measDim, 1);
        calculatePulls(pulls, residual, S_mat);     

        if (dumpResiduesStep2) {
            if (measDim == 2) {
                gCandidate->SetResidual(det->GetDetectorName(), residual(0,0), residual(1,0), pulls(0,0), pulls(1,0));
            } else {
                if (det->GetDetectorName() == "fi23b") gCandidate->SetResidual(det->GetDetectorName(), 0.0, residual(0,0), 0., pulls(0,0));
                else gCandidate->SetResidual(det->GetDetectorName(), residual(0,0), 0.0, pulls(0,0), 0.);
            }
        }                       

        TMatrixD S_inv = S_mat; 
        // Perform safe inversion check
		if (!SafeInvertMatrix(S_inv)) {
			// Inversion failed due to degenerate/noisy data in this event!
			// Safely skip this update step, drop this track candidate, or return false.
			continue;
		}
        //S_inv.Invert();
        TMatrixD K_gain = P_cov * TMatrixD(TMatrixD::kTransposed, H_mat) * S_inv;
        x_state += (K_gain * residual);
         if (std::abs(x_state(4,0)) < 1e-6 || std::abs(x_state(2,0)) > 5.0 || std::abs(x_state(3,0)) > 5.0) {
			return 10;
		}
        josephFormUpdate(P_cov, K_gain, H_mat, R_noise, enforceSymmetry);

        //TMatrixD chi2_inc = TMatrixD(TMatrixD::kTransposed, residual) * S_inv * residual;          
        TMatrixD R_inv = R_noise; 
        // Perform safe inversion check
		if (!SafeInvertMatrix(R_inv)) {
			continue; 
		}
		//R_inv.Invert();
        TMatrixD residual_updated = z_meas - (H_mat * x_state);         
        TMatrixD chi2_inc = TMatrixD(TMatrixD::kTransposed, residual_updated) * R_inv * residual_updated;
        chi2_second += chi2_inc(0,0);           

        Double_t ptot_new = TMath::Abs(charge) / x_state(4,0);
        Double_t ux_new = x_state(2,0), uy_new = x_state(3,0);
        Double_t pz_new_local = ptot_new / TMath::Sqrt(1.0 + ux_new * ux_new + uy_new * uy_new);
        TVector3 p_new_local(ux_new * pz_new_local, uy_new * pz_new_local, pz_new_local);     

        det->LocalToGlobalMomentum(p_new_global, p_new_local);          
        det->LocalToGlobal(pos_new_global, x_state(0,0), x_state(1,0));
        Double_t beta = p_new_global.Mag() / TMath::Sqrt(p_new_global.Mag() * p_new_global.Mag() + mass * mass);
        gCandidate->SetPosition(pos_new_global);
        gCandidate->SetMomentum(-p_new_global); 
        gCandidate->SetBeta(beta);          

        if (i == 0) {
            if(!helium) pos_target = pos_new_global;
            if(helium) pos_target.SetXYZ(x0_start,y0_start,0.);
            mom_target = p_new_global;
            beta_target = beta;
        }                           
    }
    
   // cout<<"THIRD STEP FOR CHARGE: "<<charge<<endl;
    
    // --- RE-INITIALIZE AT TARGET (CLEAN PASS 3 & 4) ---
    gCandidate->SetStartMomentum(mom_target);
    gCandidate->SetStartPosition(pos_target);
    gCandidate->SetStartBeta(beta_target);
    gCandidate->SetCharge(charge);
    gCandidate->Reset();

    // Reset covariances safely (diagonal constraint + clear correlations)
    P_cov.Zero(); R_target.Zero();
    qp_err = (helium ? 0.03 : 0.01) * charge / gCandidate->GetMomentum().Mag();
 /*   
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 5; ++c) {
            if (r != c) P_cov(r, c) = 0.0;
        }
    }
*/
    if (!helium) 
	{
		x_err_targ = 0.1;
		y_err_targ = 0.1;
		P_cov(0,0) = x_err_targ * x_err_targ;
		P_cov(1,1) = y_err_targ * y_err_targ;
		P_cov(2,2) = 1.e-1 * 1.e-1;
		P_cov(3,3) = 1.e-1 * 1.e-1;
		P_cov(4,4) = qp_err * qp_err;
		
		R_target(0,0) = P_cov(0,0);
		R_target(1,1) = P_cov(1,1);
		R_target(0,1) = 0.;
		R_target(1,0) = 0.;
		
	}
	else
	{
		// Set spatial uncertainties directly from Carbon's stored vertex error
		if (fHasCarbonVertexCov) {
			P_cov(0, 0) = fbestCarbonTargetCov(0, 0); // Var(x)
			P_cov(0, 1) = fbestCarbonTargetCov(0, 1); // Cov(x, y)
			P_cov(1, 0) = fbestCarbonTargetCov(1, 0); // Cov(y, x)
			P_cov(1, 1) = fbestCarbonTargetCov(1, 1); // Var(y)

			// Set initial direction / momentum variances
			P_cov(2,2) = 1.e-1 * 1.e-1;
			P_cov(3,3) = 1.e-1 * 1.e-1;
			P_cov(4,4) = qp_err * qp_err;

			// Target measurement noise R for Helium
			R_target(0, 0) = fbestCarbonTargetCov(0, 0);
			R_target(0, 1) = fbestCarbonTargetCov(0, 1);
			R_target(1, 0) = fbestCarbonTargetCov(1, 0);
			R_target(1, 1) = fbestCarbonTargetCov(1, 1);
		}
	}

    // --- STEP 3: FORWARD FILTER ---
    for (Int_t i = 0; i < (Int_t)gSetup->GetArray().size(); ++i) {
        auto det = gSetup->GetArray().at(i);  
        Int_t hitIndex = gCandidate->GetHitIndexByName(det->GetDetectorName().Data());
        if (hitIndex < 0) continue;
        R3BHit* hit = gSetup->GetHit(det->GetDetectorName().Data(), hitIndex);
        if (!hit) continue;

        gProp->PropagateToDetectorForward(gCandidate, det, P_cov);
        Double_t weight = (kTarget == det->section || det->GetDetectorName() == "tofd") ? 0.5 : 1.0;
        Double_t sigma_E_squared = 0.0;
        if (gEnergyLoss) {
            sigma_E_squared = weight * det->GetEnergyLossStraggling(gCandidate);
            gCandidate->PassThroughDetector(det, weight); 
        }

        det->GlobalToLocal(gCandidate->GetPosition(), x_l, y_l);
        det->GlobalToLocalMomentum(gCandidate->GetMomentum(), p_l);
        x_state(0,0) = x_l; x_state(1,0) = y_l;
        x_state(2,0) = p_l.X() / p_l.Z(); x_state(3,0) = p_l.Y() / p_l.Z();
        x_state(4,0) = TMath::Abs(charge) / p_l.Mag();
         if (std::abs(x_state(4,0)) < 1e-6 || std::abs(x_state(2,0)) > 5.0 || std::abs(x_state(3,0)) > 5.0) {
			return 10;
		}
        enforceSymmetry(P_cov);

        Double_t L_over_X0 = weight * det->thickness / rad_length;
        Double_t p_tot = gCandidate->GetMomentum().Mag();
        Double_t E_tot = TMath::Sqrt(p_tot * p_tot + mass * mass);
        computeProcessNoise(Q_mat, L_over_X0, x_state(4,0), p_tot / E_tot, p_tot, mass, 
                            sigma_E_squared, x_state(2,0), x_state(3,0), det->thickness);
        P_cov += Q_mat;

        measDim = (det->GetDetectorName() == "tofd" || kTarget == det->section) ? 2 : 1;
        TMatrixD z_meas(measDim, 1), H_mat(measDim, 5), R_noise(measDim, measDim);
        z_meas.Zero(); H_mat.Zero(); R_noise.Zero();

        if (det->GetDetectorName() == "tofd") {
            H_mat(0,0) = 1.0; H_mat(1,1) = 1.0;
            R_noise(0,0) = det->res_x * det->res_x * 4.0 / 12.0;
            R_noise(1,1) = det->res_y * det->res_y * (helium ? 9.0 : 1.0);
            z_meas(0,0) = hit->GetX(); z_meas(1,0) = hit->GetY();
        } else if (kTarget == det->section) {
            H_mat(0,0) = 1.0; H_mat(1,1) = 1.0;
            R_noise(0,0) = R_target(0,0);
			R_noise(0,1) = R_target(0,1);
			R_noise(1,1) = R_target(1,1);
			R_noise(1,0) = R_target(1,0);
            z_meas(0,0) = gCandidate->GetStartPosition().X();
            z_meas(1,0) = gCandidate->GetStartPosition().Y();             
        } else if (det->GetDetectorName() == "fi23b") {
            H_mat(0,1) = 1.0;
            R_noise(0,0) = det->res_y * det->res_y * 4.0 / 12.0;
            z_meas(0,0) = hit->GetY();
        } else {
            H_mat(0,0) = 1.0;
            R_noise(0,0) = det->res_x * det->res_x * 4.0 / 12.0;
            z_meas(0,0) = hit->GetX();
        }

        TMatrixD S_mat = H_mat * P_cov * TMatrixD(TMatrixD::kTransposed, H_mat) + R_noise; 
        TMatrixD residual = z_meas - (H_mat * x_state);  
        TMatrixD pulls(measDim, 1);
        calculatePulls(pulls, residual, S_mat);     

        if (dumpResiduesStep3) {
            if (measDim == 2) {
                gCandidate->SetResidual(det->GetDetectorName(), residual(0,0), residual(1,0), pulls(0,0), pulls(1,0));
            } else {
                if (det->GetDetectorName() == "fi23b") gCandidate->SetResidual(det->GetDetectorName(), 0.0, residual(0,0), 0., pulls(0,0));
                else gCandidate->SetResidual(det->GetDetectorName(), residual(0,0), 0.0, pulls(0,0), 0.);
            }
        }

        TMatrixD S_inv = S_mat; 
        // Perform safe inversion check
		if (!SafeInvertMatrix(S_inv)) {
			// Inversion failed due to degenerate/noisy data in this event!
			// Safely skip this update step, drop this track candidate, or return false.
			continue;
		}
        //S_inv.Invert();
        TMatrixD K_gain = P_cov * TMatrixD(TMatrixD::kTransposed, H_mat) * S_inv;          
        x_state += (K_gain * residual);
         if (std::abs(x_state(4,0)) < 1e-6 || std::abs(x_state(2,0)) > 5.0 || std::abs(x_state(3,0)) > 5.0) {
			return 10;
		}
        josephFormUpdate(P_cov, K_gain, H_mat, R_noise, enforceSymmetry);

        //TMatrixD chi2_inc = TMatrixD(TMatrixD::kTransposed, residual) * S_inv * residual;
        TMatrixD R_inv = R_noise;
        // Perform safe inversion check
		if (!SafeInvertMatrix(R_inv)) {
			continue; 
		}
		//R_inv.Invert();
        TMatrixD residual_updated = z_meas - (H_mat * x_state);         
        TMatrixD chi2_inc = TMatrixD(TMatrixD::kTransposed, residual_updated) * R_inv * residual_updated;
        chi2_third += chi2_inc(0,0);

        Double_t ptot_new = TMath::Abs(charge) / x_state(4,0);
        Double_t ux_new = x_state(2,0), uy_new = x_state(3,0);
        Double_t pz_new_local = ptot_new / TMath::Sqrt(1.0 + ux_new * ux_new + uy_new * uy_new);
        TVector3 p_new_local(ux_new * pz_new_local, uy_new * pz_new_local, pz_new_local);

        det->LocalToGlobalMomentum(p_new_global, p_new_local); 
        det->LocalToGlobal(pos_new_global, x_state(0,0), x_state(1,0));
        gCandidate->SetPosition(pos_new_global);
        gCandidate->SetMomentum(p_new_global);
        gCandidate->SetBeta(p_new_global.Mag() / TMath::Sqrt(p_new_global.Mag() * p_new_global.Mag() + mass * mass));
    }
    
   // cout<<"FOURTH STEP FOR CHARGE: "<<charge<<endl;
    
    // --- STEP 4: BACKWARD FILTER ---
    gCandidate->SetMomentum(-p_new_global);
    gCandidate->SetPosition(pos_new_global);
    gCandidate->SetBeta(p_new_global.Mag() / TMath::Sqrt(p_new_global.Mag() * p_new_global.Mag() + mass * mass));
    gCandidate->SetCharge(-charge);
    TMatrixD P_cov_at_target(5,5); P_cov_at_target.Zero();

    for (Int_t i = (Int_t)gSetup->GetArray().size() - 1; i >= 0; --i) {
        auto det = gSetup->GetArray().at(i);  
        Int_t hitIndex = gCandidate->GetHitIndexByName(det->GetDetectorName().Data());
        if (hitIndex < 0) continue;
        R3BHit* hit = gSetup->GetHit(det->GetDetectorName().Data(), hitIndex);
        if (!hit) continue;

        gProp->PropagateToDetectorBackward(gCandidate, det, P_cov);
        Double_t weight = (kTarget == det->section || det->GetDetectorName() == "tofd") ? 0.5 : 1.0;
        Double_t sigma_E_squared = 0.0;
        if (gEnergyLoss) {
            sigma_E_squared = weight * det->GetEnergyLossStraggling(gCandidate);
            gCandidate->PassThroughDetectorBackward(det, weight); 
        }

        det->GlobalToLocal(gCandidate->GetPosition(), x_l, y_l);
        det->GlobalToLocalMomentum(gCandidate->GetMomentum(), p_l);
        x_state(0,0) = x_l; x_state(1,0) = y_l;
        x_state(2,0) = p_l.X() / p_l.Z(); x_state(3,0) = p_l.Y() / p_l.Z();
        x_state(4,0) = TMath::Abs(charge) / p_l.Mag();
         if (std::abs(x_state(4,0)) < 1e-6 || std::abs(x_state(2,0)) > 5.0 || std::abs(x_state(3,0)) > 5.0) {
			return 10;
		}
        enforceSymmetry(P_cov);

        Double_t L_over_X0 = weight * det->thickness / rad_length;
        Double_t p_tot_GeV = gCandidate->GetMomentum().Mag();
        Double_t E_tot = TMath::Sqrt(p_tot_GeV * p_tot_GeV + mass * mass);
        computeProcessNoise(Q_mat, L_over_X0, x_state(4,0), p_tot_GeV / E_tot, p_tot_GeV, mass, 
                            sigma_E_squared, x_state(2,0), x_state(3,0), det->thickness);
        P_cov += Q_mat;

        measDim = (det->GetDetectorName() == "tofd" || kTarget == det->section) ? 2 : 1;
        TMatrixD z_meas(measDim, 1), H_mat(measDim, 5), R_noise(measDim, measDim);
        z_meas.Zero(); H_mat.Zero(); R_noise.Zero();

        if (det->GetDetectorName() == "tofd") {
            H_mat(0,0) = 1.0; H_mat(1,1) = 1.0;
            R_noise(0,0) = det->res_x * det->res_x * 4.0 / 12.0;
            R_noise(1,1) = det->res_y * det->res_y * (helium ? 9.0 : 1.0);
            z_meas(0,0) = hit->GetX(); z_meas(1,0) = hit->GetY();
        } else if (kTarget == det->section) {
            H_mat(0,0) = 1.0; H_mat(1,1) = 1.0;
            R_noise(0,0) = R_target(0,0);
			R_noise(0,1) = R_target(0,1);
			R_noise(1,1) = R_target(1,1);
			R_noise(1,0) = R_target(1,0);
            z_meas(0,0) = pos_target.X(); z_meas(1,0) = pos_target.Y();
        } else if (det->GetDetectorName() == "fi23b") {
            H_mat(0,1) = 1.0;
            R_noise(0,0) = det->res_y * det->res_y * 4.0 / 12.0;
            z_meas(0,0) = hit->GetY();
        } else {
            H_mat(0,0) = 1.0;
            R_noise(0,0) = det->res_x * det->res_x * 4.0 / 12.0;
            z_meas(0,0) = hit->GetX();
        }

        TMatrixD S_mat = H_mat * P_cov * TMatrixD(TMatrixD::kTransposed, H_mat) + R_noise; 
        TMatrixD residual = z_meas - (H_mat * x_state);
        TMatrixD pulls(measDim, 1);
        calculatePulls(pulls, residual, S_mat);     

        if (dumpResiduesStep4) {
            if (measDim == 2) {
                gCandidate->SetResidual(det->GetDetectorName(), residual(0,0), residual(1,0), pulls(0,0), pulls(1,0));
               // cout<<"Residuals at detector: "<<det->GetDetectorName()<<" "<<residual(0,0)<<" "<< residual(1,0)<<" "<< pulls(0,0)<<" "<< pulls(1,0)<<endl;
            } else {
                if (det->GetDetectorName() == "fi23b") 
                {
					gCandidate->SetResidual(det->GetDetectorName(), 0.0, residual(0,0), 0., pulls(0,0));
					//cout<<"Residuals at detector: "<<det->GetDetectorName()<<" "<<residual(0,0)<<" "<< pulls(0,0)<<endl;
				}
                else 
                {
					gCandidate->SetResidual(det->GetDetectorName(), residual(0,0), 0.0, pulls(0,0), 0.);
					//cout<<"Residuals at detector: "<<det->GetDetectorName()<<" "<<residual(0,0)<<" "<< pulls(0,0)<<endl;
				}
            }
        }

        TMatrixD S_inv = S_mat; 
        // Perform safe inversion check
		if (!SafeInvertMatrix(S_inv)) {
			// Inversion failed due to degenerate/noisy data in this event!
			// Safely skip this update step, drop this track candidate, or return false.
			continue; 
		}
        //S_inv.Invert();
        	
	//Update		
        TMatrixD K_gain = P_cov * TMatrixD(TMatrixD::kTransposed, H_mat) * S_inv;          
        x_state += (K_gain * residual);
         if (std::abs(x_state(4,0)) < 1e-6 || std::abs(x_state(2,0)) > 5.0 || std::abs(x_state(3,0)) > 5.0) {
			return 10;
		}
        josephFormUpdate(P_cov, K_gain, H_mat, R_noise, enforceSymmetry);
        
        //TMatrixD chi2_inc = TMatrixD(TMatrixD::kTransposed, residual) * S_inv * residual;
        TMatrixD R_inv = R_noise; 
        // Perform safe inversion check
		if (!SafeInvertMatrix(R_inv)) {
			continue; 
		}
		//R_inv.Invert();
        TMatrixD residual_updated = z_meas - (H_mat * x_state);         
        TMatrixD chi2_inc = TMatrixD(TMatrixD::kTransposed, residual_updated) * R_inv * residual_updated;
        chi2 += chi2_inc(0,0);
       	
        Double_t chi2temp = 0.;
        TMatrixD residual1 = z_meas - (H_mat * x_state);
        if (det->GetDetectorName() == "tofd" || kTarget == det->section)
        {
            chi2temp = TMath::Power(residual1(0,0),2) / R_noise(0,0);
            chi2temp = chi2temp + TMath::Power(residual1(1,0),2) / R_noise(1,1);
         }
         else
         {
			chi2temp = TMath::Power(residual1(0,0),2) / R_noise(0,0);
		 }
 //       chi2 += chi2temp;

        Double_t ptot_new = TMath::Abs(charge) / x_state(4,0);
        Double_t ux_new = x_state(2,0), uy_new = x_state(3,0);
        Double_t pz_new_local = ptot_new / TMath::Sqrt(1.0 + ux_new * ux_new + uy_new * uy_new);
        TVector3 p_new_local(ux_new * pz_new_local, uy_new * pz_new_local, pz_new_local);

        det->LocalToGlobalMomentum(p_new_global, p_new_local); 
        det->LocalToGlobal(pos_new_global, x_state(0,0), x_state(1,0));
        Double_t beta = p_new_global.Mag() / TMath::Sqrt(p_new_global.Mag() * p_new_global.Mag() + mass * mass);
        gCandidate->SetPosition(pos_new_global);
        gCandidate->SetMomentum(-p_new_global); 
        gCandidate->SetBeta(beta);

        if (i == 0) {
            pos_target = pos_new_global;
            mom_target = p_new_global;
            beta_target = beta;
            P_cov_at_target = P_cov;
        }
    }
    gCandidate->SetTargetCovariance(P_cov_at_target);
   // cout<<"P_cov_at_target: "<<P_cov_at_target(0,0)<<" "<<P_cov_at_target(1,1)<<" "<<P_cov_at_target(2,2)<<" "<<
   // P_cov_at_target(3,3)<<" "<<P_cov_at_target(4,4)<<endl;

    // --- FINAL REGISTRATION ---
    gCandidate->SetStartPosition(pos_target);
    gCandidate->SetStartMomentum(mom_target);
    gCandidate->SetCharge(TMath::Abs(charge));
    gCandidate->SetStartBeta(beta_target);
    gCandidate->SetChi2(chi2);
    gCandidate->Reset();
/*	
	cout<<"*** New fragment ****** "<<endl;
    cout<<"*** Charge: "<<gCandidate->GetCharge()<<endl;
    cout<<"*** Position: "<<gCandidate->GetStartPosition().X()<<", "<<gCandidate->GetStartPosition().Y()<<", "<<gCandidate->GetStartPosition().Z()<<endl;
    cout<<"*** Momentum: "<<gCandidate->GetStartMomentum().X()<<", "<<gCandidate->GetStartMomentum().Y()<<", "<<gCandidate->GetStartMomentum().Z()<<endl;
    cout<<"*** Beta: "<<gCandidate->GetStartBeta()<<endl;
    cout<<"*** Chi2: "<<chi2<<", "<<chi2_first<<", "<<chi2_second<<", "<<chi2_third<<endl;
*/
    //cout<<"FINISHED IN KF FOR CHARGE: "<<charge<<endl;
    if (chi2 > 1.e6) status = 10;
    return status;
}

// ***********************************************************************************************************

Int_t R3BFragmentFitterChi2S494::FitTrackMomentumForward(R3BTrackingParticle* particle, R3BTrackingSetup* setup)
{
    gCandidate = particle;
    gSetup = setup;
    Bool_t writeout = false;

    TVector3 pos0;
    TVector3 pos3;
    TVector3 pos23a;
    TVector3 pos23b;
    TVector3 direction0;
    TVector3 inputPosition;
    TVector3 inputMomentum;
    TVector3 p_new_global; 
    TVector3 pos_new_global;
			
    TVector3 p_l;
    Int_t status = 0;
    Double_t chi2 = 0., chi2_back=0.;
    Double_t x_new_local = 0.;
	Double_t y_new_local = 0.;
	Double_t ux_new = 0.;
	Double_t uy_new = 0.;
	Double_t pz_new_local = 0., ptot_new=0.;
	Double_t x_l = 0., y_l = 0.;
	Double_t dx_tofd = 2.025, dy_tofd = 3.;
    
    Double_t x0 = gCandidate->GetStartPosition().X();
    Double_t y0 = gCandidate->GetStartPosition().Y();
    Double_t z0 = gCandidate->GetStartPosition().Z();
    Double_t px0 = gCandidate->GetStartMomentum().X();
    Double_t py0 = gCandidate->GetStartMomentum().Y();
    Double_t pz0 = gCandidate->GetStartMomentum().Z();
    Double_t ptot0 = sqrt(px0 * px0 + py0 * py0 + pz0 * pz0);
    Double_t mass = gCandidate->GetMass();
    
    Double_t diffx = 0.0093 * 0.996 ;      // standard field
    Double_t diffy = 0.000365 * (-0.125) ; 
    
    Bool_t helium = false;
    if(gCandidate->GetCharge() == 4) helium = true;
    
    if (gCandidate->GetHitIndexByName("fi23a") > -1)
    {
        auto fi23a = gSetup->GetByName("fi23a");
        fi23a->LocalToGlobal(pos23a,gSetup->GetHit("fi23a", gCandidate->GetHitIndexByName("fi23a"))->GetX()-diffx,0.0);
    }
    if (gCandidate->GetHitIndexByName("fi23b") > -1)
    {
        auto fi23b = gSetup->GetByName("fi23b");
        fi23b->LocalToGlobal(pos23b, 0.0, gSetup->GetHit("fi23b", gCandidate->GetHitIndexByName("fi23b"))->GetY()-diffy);
    }
    
    pos0.SetX(x0);
    pos0.SetY(y0);
    pos0.SetZ(z0);
    
    pos3.SetX(pos23a.X());
    pos3.SetY(pos23b.Y());
    Double_t zfi23 = (pos23a.Z() + pos23b.Z()) / 2.;
    pos3.SetZ(zfi23);
    
    direction0 = pos3 - pos0;   
    cout<<"**** Tracking particle with charge = "<<gCandidate->GetCharge()<<endl;
	direction0 = direction0.Unit();
    
    direction0.SetMag(ptot0);
    
    px0 = direction0.X();
    py0 = direction0.Y();
    pz0 = direction0.Z();

    inputMomentum.SetX(px0);
    inputMomentum.SetY(py0);
    inputMomentum.SetZ(pz0);

    inputPosition.SetX(x0);
    inputPosition.SetY(y0);
    inputPosition.SetZ(z0);

    gCandidate->SetStartPosition(inputPosition);
    gCandidate->SetStartMomentum(inputMomentum);
    
	gCandidate->Reset();
    
    px0 = gCandidate->GetMomentum().X();
    py0 = gCandidate->GetMomentum().Y();
    pz0 =gCandidate->GetMomentum().Z();

    x0 = gCandidate->GetPosition().X();
    y0 = gCandidate->GetPosition().Y();
    z0 = gCandidate->GetPosition().Z();
    
    if(writeout){
		cout << "Start values momentum lab: " << px0 << "  " << py0 << "  " << pz0 <<endl;
		cout << "Start values position lab: " << x0 << "  " << y0 << "  " << z0 <<endl;
	}
    
    // We start at the target, thus local = global
    x_l = x0;
    y_l = y0;
    TVector3 startMomentumLocal = inputMomentum;
    
    // --- BEFORE THE DETECTOR LOOP ---
	TMatrixD x_state(5, 1);  // 5x1 Column Vector
	TMatrixD P_cov(5, 5);    // 5x5 Covariance Matrix
	TMatrixD Q_mat(5, 5);    // 5x5 noise Matrix
	// Initialize local state at the target
	x_state(0,0) = x_l; // initial x 
	x_state(1,0) = y_l; // initial y
	x_state(2,0) = startMomentumLocal.X() / startMomentumLocal.Z(); // initial slope dx/dz
	x_state(3,0) = startMomentumLocal.Y() / startMomentumLocal.Z(); // initial slope dy/dz
	x_state(4,0) = gCandidate->GetCharge() / startMomentumLocal.Mag(); // q/p 

	// Initialize starting uncertainties (high values mean "I trust the hits more than my guess")
	P_cov.Zero();
	P_cov(0,0) = 1.5 * 1.5 ;//0.01 * 0.01;   // Allow the filter to shift X by cms if needed
	P_cov(1,1) = 1.5 * 1.5 ;//0.01 * 0.01;   // Allow it to shift Y
	P_cov(2,2) = 50.e-3 * 50.e-3;//0.05 * 0.05;   // Open up the injection slopes completely!
	P_cov(3,3) = 10.e-3 * 10.e-3;//0.05 * 0.05;   
	if(!helium) P_cov(4,4) = (0.01 * gCandidate->GetCharge() / startMomentumLocal.Mag()) *
	             (0.01 * gCandidate->GetCharge() / startMomentumLocal.Mag());// 0.07 * 0.07;
	if(helium) P_cov(4,4) = (0.03 * gCandidate->GetCharge() / startMomentumLocal.Mag()) *
	             (0.03 * gCandidate->GetCharge() / startMomentumLocal.Mag());// 0.07 * 0.07;
	
	if(writeout){
		cout<<"Forward tracking starting values:"<<endl;
		cout<<"Charge: "<<gCandidate->GetCharge()<<endl;
		cout<<"Position: "<<x0<<" "<<y0<<" "<<z0<<endl;
		cout<<"Momentum: "<<inputMomentum.X()<<" "<<inputMomentum.Y()<<" "<<inputMomentum.Z()<<endl;
		cout<<"State values at init: "<<x_state(0,0)<<" "<<x_state(1,0)<<" "<<x_state(2,0)<<" "<<x_state(3,0)<<x_state(4,0)<<endl;
		cout<<"P_cov values at init: "<<P_cov(0,0)<<" "<<P_cov(1,1)<<" "<<P_cov(2,2)<<" "<<P_cov(3,3)<<" "<<P_cov(4,4)<<endl;
	}
	
	Double_t sigma_E_squared = 0.;
    Int_t measDim = 1;
    Double_t dxerror = 1., dyerror = 1.; 	
	Int_t n_ghost_track = 0;
	for (Int_t i = 0 ; i < gSetup->GetArray().size() ; i++) 
	{
        auto det = gSetup->GetArray().at(i);  
        
       // hit data at a given detector; contains: detector Id, x and y positions of a particle 
       // at the detector, as well asparticle's charge and time
        R3BHit* hit = nullptr; 
        Int_t hitIndex = gCandidate->GetHitIndexByName(det->GetDetectorName().Data());
        
		if(hitIndex < 0) continue;
		
        if (hitIndex >= 0)
        {
            hit = gSetup->GetHit(det->GetDetectorName().Data(), hitIndex);
        }

        if (!hit) continue;
        
		// ==========================================
		// 1. THE PREDICT STEP (Extrapolation)
		// ==========================================
		// Your existing Runge-Kutta engine propagates the physical state backward!
		TVector3 pos_before = gCandidate->GetPosition(); // Before propagation
		
		   
		   if(det->GetDetectorName() == "tofd" || kTarget == det->section) measDim = 2;
		   else measDim = 1;
		
		    // We have to distinguish between detectors measuring only x, y or both
			// Measurement vector z (what the detector actually saw)
			TMatrixD z_meas(measDim, 1); z_meas.Zero();
			// Measurement projection matrix H (maps 5D state down to the measured components x, y)
			TMatrixD H_mat(measDim, 5); H_mat.Zero();
			// Detector resolution matrix R
			TMatrixD R_noise(measDim, measDim); R_noise.Zero();
		
			if (det->GetDetectorName() == "tofd") // x and y measured
			{
				H_mat(0,0) = 1.0; 
				H_mat(1,1) = 1.0;
				// det->res_x is fibre_width/2.; for KF R_noise ist width/sqrt(12)
				R_noise(0,0) = det->res_x * det->res_x * 4./12.;
				R_noise(1,1) = det->res_y * det->res_y ;
				if(helium) R_noise(1,1) = R_noise(1,1) * 9.;
				z_meas(0,0) = hit->GetX();
				z_meas(1,0) = hit->GetY();
			}
			else if(kTarget == det->section)
			{
				H_mat(0,0) = 1.0; 
				H_mat(1,1) = 1.0;
				R_noise(0,0) = 1.5 * 1.5 * 4./12.;
				R_noise(1,1) = 1.5 * 1.5 * 4./12.;
				z_meas(0,0) = gCandidate->GetStartPosition().X();
				z_meas(1,0) = gCandidate->GetStartPosition().Y();				
			}			
			else if (det->GetDetectorName() == "fi23b")  // only y position measured
			{
				H_mat(0,1) = 1.0; // Maps to the y parameter of x_state
				R_noise(0,0) = det->res_y * det->res_y * 4./12.;
				z_meas(0,0) = hit->GetY();
			}
			else // fi3X and fi23a
			{
				H_mat(0,0) = 1.0; 
				R_noise(0,0) = det->res_x * det->res_x * 4./12.;
				z_meas(0,0) = hit->GetX();
			}
        		
		if(i >= 0)
		{ 
			if(writeout){
					cout<<"#### Propagation forward to det: "<<det->GetDetectorName()<<" will be started ##"<<endl;
					}
				
			gProp->PropagateToDetectorForward(gCandidate, det, P_cov);
			
			if(writeout){
					cout<<"Det: "<<det->GetDetectorName()<<" has been reached"<<endl;
					cout<<"Position: "<<gCandidate->GetPosition().X()<<" "<<gCandidate->GetPosition().Y()<<" "<<gCandidate->GetPosition().Z()<<endl;
					cout<<"Momentum: "<<gCandidate->GetMomentum().X()<<" "<<gCandidate->GetMomentum().Y()<<" "<<gCandidate->GetMomentum().Z()<<endl;
					det->GlobalToLocal(gCandidate->GetPosition(), x_l, y_l);
					cout<<"State values: "<<x_l<<" "<<y_l<<endl;
					cout<<"P_cov values: "<<P_cov(0,0)<<" "<<P_cov(1,1)<<" "<<P_cov(2,2)<<" "<<P_cov(3,3)<<" "<<P_cov(4,4)<<endl;
				}
			Double_t weight = 1.;
			if (gEnergyLoss ) // correct for energy loss in detector
			{
				if (kTarget == det->section || det->GetDetectorName() == "tofd")
				{
					weight = 0.5;
				}
				sigma_E_squared = weight * det->GetEnergyLossStraggling(gCandidate);
			   // Update particle's momentum is due to energy loss in a detector:
				gCandidate->PassThroughDetector(det, weight); 
			 }
										 
			// Convert the newly predicted gCandidate back into our 5D local state matrix
			det->GlobalToLocal(gCandidate->GetPosition(), x_l, y_l);
			det->GlobalToLocalMomentum(gCandidate->GetMomentum(),p_l);
		
		// update local state with values after propagation through a detector	
			x_state(0,0) = x_l;
			x_state(1,0) = y_l;
			x_state(2,0) = p_l.X() / p_l.Z();
			x_state(3,0) = p_l.Y() / p_l.Z();
			x_state(4,0) = gCandidate->GetCharge() / p_l.Mag();
			// Force exact symmetry after propagation step
			for (Int_t ij = 0; ij < 5; ++ij) {
				for (Int_t jj = ij + 1; jj < 5; ++jj) {
					Double_t avg = 0.5 * (P_cov(ij, jj) + P_cov(jj, ij));
					P_cov(ij, jj) = avg;
					P_cov(jj, ij) = avg;
				}
			}
			if(writeout)
			{
					cout<<"After going through det: "<<det->GetDetectorName()<<endl;
					cout<<"Energy loss: "<<det->GetEnergyLoss(gCandidate)<<", sigma_E_squared: "<<sigma_E_squared<<endl;
					cout<<"Position: "<<gCandidate->GetPosition().X()<<" "<<gCandidate->GetPosition().Y()<<" "<<gCandidate->GetPosition().Z()<<endl;
					cout<<"Momentum: "<<gCandidate->GetMomentum().X()<<" "<<gCandidate->GetMomentum().Y()<<" "<<gCandidate->GetMomentum().Z()<<endl;
					cout<<"State values: "<<x_state(0,0)<<" "<<x_state(1,0)<<" "<<x_state(2,0)<<" "<<x_state(3,0)<<" "<<x_state(4,0)<<endl;
					cout<<"P_cov values: "<<P_cov(0,0)<<" "<<P_cov(1,1)<<" "<<P_cov(2,2)<<" "<<P_cov(3,3)<<" "<<P_cov(4,4)<<endl;
			}
				
			// ==========================================
			// 2. MULTIPLE COULOMB SCATTERING AND ENERGY-LOSS STRAGGLING (Noise Q)
			// ==========================================
			// Calculate scattering angle using the Highland Formula based on detector material thickness
			Double_t L_over_X0 = weight * det->thickness / rad_length; 
			Double_t p_tot = gCandidate->GetMomentum().Mag(); // in MeV/c or GeV/c
			mass  = gCandidate->GetMass();          
			// Calculate beta = p / E
			Double_t E_tot = TMath::Sqrt(p_tot * p_tot + mass * mass);
			Double_t beta  = p_tot / E_tot;
			Double_t theta_scat = (0.0136 * x_state(4,0)/beta) * TMath::Sqrt(L_over_X0) * (1.0 + 0.038 * TMath::Log(L_over_X0));
			Double_t var_scat = theta_scat * theta_scat;
            
            // 2. Convert Energy Variance (\sigma_E^2) to q/p Variance (\sigma_{q/p}^2)
			// Using relativistic kinematics: E^2 = p^2 + m^2 -> dE = (p/E) * dp
			// dp = (E/p) * dE
			Double_t sigma_p_squared = (E_tot / p_tot) * (E_tot / p_tot) * sigma_E_squared;

			// Since state variable is q/p, d(q/p)/dp = -q / p^2
			// Therefore: var(q/p) = (q / p^2)^2 * var(p)
			Double_t qOverP = gCandidate->GetCharge() / p_tot;
			Double_t sigma_qOverP_squared = TMath::Power(qOverP / p_tot, 2) * sigma_p_squared;
			
		 // Inflate the predictive uncertainty because of scattering, except at tofd		   
			Q_mat.Zero(); // clear matrix
			Q_mat(2, 2) = var_scat;
			Q_mat(3, 3) = var_scat;
			Q_mat(4, 4) = sigma_qOverP_squared; 
			// Add process noise to the track covariance matrix
			P_cov = P_cov + Q_mat;
			
		// ==========================================
		// 3. THE UPDATE STEP (Filtering the Hit)
		// ==========================================
        
			// Compute residual and its covariance matrix S 
			// S = H * P * H^T + R
			TMatrixD S_mat(measDim, measDim);
			S_mat = H_mat * P_cov * TMatrixD(TMatrixD::kTransposed, H_mat) + R_noise;
						
			// Residual: 
			TMatrixD residual(measDim, 1);
			residual = z_meas - (H_mat * x_state);
			
			// Scaling the Kalman Gain if residual < det_res: instead of forcing a zero, 
			// we damp the residual smoothly when it falls deep inside the resolution boundary. 
			// This prevents mathematical discontinuities
/*
			Double_t res_val, resolution, pull, damping_factor;
			if(measDim == 1)
			{				
				res_val = residual(0,0);
				if(det->GetDetectorName() == "fi23b") resolution = det->res_y;
				else resolution = det->res_x;

				// Calculate how many "sigmas" the track is from the hit
				pull = TMath::Abs(res_val) / resolution;

				if (pull < 1.0) 
				{
					// If inside 1 sigma of detector resolution, smoothly damp the residual.
					// At pull = 0, factor is 0. At pull = 1, factor is 1.
					damping_factor = pull * pull; // Smooth parabolic scaling
					residual(0,0) = res_val * damping_factor;
				}
			}
			else
			{
				res_val = residual(0,0);
				resolution = sqrt(R_noise(0,0));//det->res_x;
				// Calculate how many "sigmas" the track is from the hit
				pull = TMath::Abs(res_val) / resolution;
				if (pull < 1.0) 
				{
					// If inside 1 sigma of detector resolution, smoothly damp the residual.
					// At pull = 0, factor is 0. At pull = 1, factor is 1.
					damping_factor = pull * pull; // Smooth parabolic scaling
					residual(0,0) = res_val * damping_factor;
				}
				
				res_val = residual(1,0);
				resolution = sqrt(R_noise(1,1));//det->res_y;
				// Calculate how many "sigmas" the track is from the hit
				pull = TMath::Abs(res_val) / resolution;
				if (pull < 1.0) 
				{
					// If inside 1 sigma of detector resolution, smoothly damp the residual.
					// At pull = 0, factor is 0. At pull = 1, factor is 1.
					damping_factor = pull * pull; // Smooth parabolic scaling
					residual(1,0) = res_val * damping_factor;
				}	
			}		
*/
			// Extract Pulls for measured coordinates
			Double_t pull_x = 0.0;
			Double_t pull_y = 0.0;

			if (measDim == 1) {
				Double_t var_x = S_mat(0, 0); // Total variance in X
				if (var_x > 0.0) {
					pull_x = residual(0,0) / sqrt(var_x);
				}
			}
			else {
				Double_t var_x = S_mat(0, 0); // Total variance in X
				if (var_x > 0.0) {
					pull_x = residual(0,0) / sqrt(var_x);
				}
				Double_t var_y = S_mat(1, 1); // Total variance in y
				if (var_y > 0.0) {
					pull_y = residual(1,0) / sqrt(var_y);
				}				
			}
		
			if (measDim == 2) {
				gCandidate->SetResidual(det->GetDetectorName(), residual(0,0), residual(1,0), pull_x, pull_y);
			} 
			else {
				// 1D measurement (determine whether it's X or Y based on det type)
				if (det->GetDetectorName() == "fi23b") {
					gCandidate->SetResidual(det->GetDetectorName(), 0.0, residual(0,0), 0., pull_x);
				} 
				else {
					gCandidate->SetResidual(det->GetDetectorName(), residual(0,0), 0.0, pull_x, 0.);
				}
			}

			// Compute Kalman Gain: K = P * H^T * S^-1
			TMatrixD K_gain(5, measDim);
			TMatrixD S_inv = S_mat; S_inv.Invert();
			K_gain = P_cov * TMatrixD(TMatrixD::kTransposed, H_mat) * S_inv;
			
			// Update local state
			 x_state = x_state + (K_gain * residual);
			
			if(writeout){
				cout<<"Det: "<<det->GetDetectorName()<<" after noise"<<endl;
				if(kTarget == det->section || det->GetDetectorName() == "tofd")
				{
					cout <<"Residues: "<<residual(0,0)<<", "<<residual(1,0)<<endl;
					cout <<"Measurem: "<<z_meas(0,0)<<", "<<z_meas(1,0)<<endl;
				}
				else
				{
					cout <<"Residues: "<<residual(0,0)<<endl;
					cout <<"Measurem: "<<z_meas(0,0)<<endl;
				}
								
				cout<<"Noise: "<<var_scat<<", "<<sigma_qOverP_squared<<endl;
				cout<<"K_gain: "<<K_gain(0,0)<<" "<<K_gain(1,0)<<" "<<K_gain(2,0)<<" "<<K_gain(3,0)<<" "<<K_gain(4,0)<<endl;
				cout<<"P_cov values with noise: "<<P_cov(0,0)<<" "<<P_cov(1,1)<<" "<<P_cov(2,2)<<" "<<P_cov(3,3)<<" "<<P_cov(4,4)<<endl;
				cout<<"Updated state values: "<<x_state(0,0)<<" "<<x_state(1,0)<<" "<<x_state(2,0)<<" "<<x_state(3,0)<<" "<<x_state(4,0)<<endl;
			}	
	        
	        // Update the Covariance Matrix: P = (I - K * H) * P
			// 1. Construct Identity Matrix
			TMatrixD I(5, 5);
			I.UnitMatrix();

			// 2. Compute (I - K * H)
			TMatrixD I_minus_KH = I - (K_gain * H_mat);

			// 3. Compute Transpose (I - K * H)^T
			TMatrixD I_minus_KH_T(TMatrixD::kTransposed, I_minus_KH);

			// 4. Compute First Term: (I - KH) * P * (I - KH)^T
			TMatrixD P_updated = I_minus_KH * P_cov * I_minus_KH_T;

			// 5. Compute Transpose of K_gain
			TMatrixD K_gain_T(TMatrixD::kTransposed, K_gain);

			// 6. Compute Second Term: K * R * K^T
			TMatrixD KRK_T = K_gain * R_noise * K_gain_T;

			// 7. Complete Joseph Form Update
			P_cov = P_updated + KRK_T;

			// 8. Explicitly Enforce Matrix Symmetry (Numerical Safeguard)
			for (Int_t ij = 0; ij; ++ij) {
				for (Int_t jj = ij + 1; jj < 5; ++jj) {
					Double_t avg = 0.5 * (P_cov(ij, jj) + P_cov(jj, ij));
					P_cov(ij, jj) = avg;
					P_cov(jj, ij) = avg;
				}
			}
			if(writeout) cout<<"Updated P_cov values: "<<P_cov(0,0)<<" "<<P_cov(1,1)<<" "<<P_cov(2,2)<<" "<<P_cov(3,3)<<" "<<P_cov(4,4)<<endl;
	
			// Increment local chi2 safely using the measurement covariance matrix
			TMatrixD chi2_increment = TMatrixD(TMatrixD::kTransposed, residual) * S_inv * residual;
			chi2 += chi2_increment(0,0);
			
			// CRITICAL: Push the freshly updated Kalman position and slopes back into gCandidate 
			// so the next Runge-Kutta step propagates the corrected trajectory!
			
			// 1. Reconstruct the new local momentum vector from the updated slopes
			// We use the fact that p_x = ux * p_z and p_y = uy * p_z
			ptot_new = gCandidate->GetCharge() / x_state(4,0);

			// Use simple geometry to find the new local p_z component
			x_new_local = x_state(0,0);
			y_new_local = x_state(1,0);
			ux_new = x_state(2,0);
			uy_new = x_state(3,0);
			pz_new_local = ptot_new / TMath::Sqrt(1.0 + ux_new*ux_new + uy_new*uy_new);

			TVector3 p_new_local(ux_new * pz_new_local, uy_new * pz_new_local, pz_new_local);
			
			// 2. Rotate the local momentum vector BACK into the global lab frame
			
			det->LocalToGlobalMomentum(p_new_global, p_new_local); 
			det->LocalToGlobal(pos_new_global,x_new_local,y_new_local);
			
			gCandidate->SetPosition(pos_new_global);
			gCandidate->SetMomentum(p_new_global);
			E_tot = TMath::Sqrt(p_new_global.Mag() * p_new_global.Mag() + mass * mass);
			beta  = p_new_global.Mag() / E_tot;
			gCandidate->SetBeta(beta);
			if(writeout)
			{
				cout<<"Updated candidate at det: "<<det->GetDetectorName()<<endl;
				cout<<"Position: "<<gCandidate->GetPosition().X()<<" "<<gCandidate->GetPosition().Y()<<" "<<gCandidate->GetPosition().Z()<<endl;
				cout<<"Momentum: "<<gCandidate->GetMomentum().X()<<" "<<gCandidate->GetMomentum().Y()<<" "<<gCandidate->GetMomentum().Z()<<endl;
			    cout<<"chi2: "<<chi2_increment(0,0)<<endl;
			}	
		}
	}
	
	// We have arrived at the the tofd, and now we track back to get momentum of the helium at the target.
	// On the way back, we just propagate helium. The core concept of a Kalman Filter is that it accumulates 
	// information as it moves forward. When the forward filter reaches tofd, that final state vector x_state 
	// is not just a raw measurement from tofd. It is a mathematically optimized state that already contains 
	// the combined information, constraints, and lever-arms of the target and all fiber detectors. 
	// Because tofd sits after the GLAD magnet, it has the maximum possible knowledge about the particle's 
	// bending radius. Therefore, the momentum value at tofd is already the absolute best estimate of the 
	// helium's momentum that the entire detector setup can possibly compute. Therefore, on the way back to
	// the target we just simply propagate He.
		gCandidate->SetMomentum(-p_new_global);
		gCandidate->SetPosition(pos_new_global);
		Double_t E_tot_new = TMath::Sqrt(p_new_global.Mag() * p_new_global.Mag() + mass * mass);
		Double_t beta_new  = p_new_global.Mag() / E_tot_new;
		gCandidate->SetBeta(beta_new);
		Double_t charge = gCandidate->GetCharge();
        gCandidate->SetCharge(-charge);
		
		if(writeout){
			cout<<"!!! Turning back toward target:"<<endl;
			cout<<"Charge: "<<charge<<endl;
			cout<<"Position: "<<gCandidate->GetPosition().X()<<" "<<gCandidate->GetPosition().Y()<<" "<<gCandidate->GetPosition().Z()<<endl;
			cout<<"Momentum: "<<gCandidate->GetMomentum().X()<<" "<<gCandidate->GetMomentum().Y()<<" "<<gCandidate->GetMomentum().Z()<<endl;
		}
		measDim = 2;
		dxerror = 1.;
	    dyerror = 1.; 
		sigma_E_squared = 0.;	
		n_ghost_track = 0;	
		Double_t chi2_loc = 0.;
		for (Int_t i = gSetup->GetArray().size() - 1; i >= 0; i--) 
		{
			auto det = gSetup->GetArray().at(i);  
			//cout<<"PROPAGATE TO DETECTOR: "<<det->GetDetectorName()<<endl;
		   // hit data at a given detector; contains: detector Id, x and y positions of a particle 
		   // at the detector, as well asparticle's charge and time
			R3BHit* hit = nullptr; 
			Int_t hitIndex = gCandidate->GetHitIndexByName(det->GetDetectorName().Data());
			
			if(hitIndex < 0) continue;
			
			if (hitIndex >= 0)
			{
				hit = gSetup->GetHit(det->GetDetectorName().Data(), hitIndex);
			}

			if (!hit) continue;
			
			// ==========================================
			// 1. THE PREDICT STEP (Extrapolation)
			// ==========================================
			// Your existing Runge-Kutta engine propagates the physical state backward!
			TVector3 pos_before = gCandidate->GetPosition(); // Before propagation
			
			if(writeout)
			{
				cout<<"Backward propagation to det: "<<det->GetDetectorName()<<" will be started ##"<<endl;
			}
	   
			if(i < gSetup->GetArray().size())
			{
				gProp->PropagateToDetectorBackward(gCandidate, det, P_cov);
				
				if(writeout)
				{
					cout<<"Det: "<<det->GetDetectorName()<<" has been reached"<<endl;
					cout<<"Position: "<<gCandidate->GetPosition().X()<<" "<<gCandidate->GetPosition().Y()<<" "<<gCandidate->GetPosition().Z()<<endl;
					cout<<"Momentum: "<<gCandidate->GetMomentum().X()<<" "<<gCandidate->GetMomentum().Y()<<" "<<gCandidate->GetMomentum().Z()<<endl;
					det->GlobalToLocal(gCandidate->GetPosition(), x_l, y_l);
					cout<<"State values: "<<x_l<<" "<<y_l<<endl;
					cout<<"P_cov values: "<<P_cov(0,0)<<" "<<P_cov(1,1)<<" "<<P_cov(2,2)<<" "<<P_cov(3,3)<<" "<<P_cov(4,4)<<endl;
				}
				 Double_t weight = 1.;
				if (gEnergyLoss ) // correct for energy loss in detector
				{
					if (kTarget == det->section || det->GetDetectorName() == "tofd")
					{
						weight = 0.5;
					}
					sigma_E_squared = weight * det->GetEnergyLossStraggling(gCandidate);
				   // Update particle's momentum is due to energy loss in a detector:
					gCandidate->PassThroughDetectorBackward(det, weight); 				
				 }
															 
				// Convert the newly predicted gCandidate back into our 5D local state matrix
				det->GlobalToLocal(gCandidate->GetPosition(), x_l, y_l);
				det->GlobalToLocalMomentum(gCandidate->GetMomentum(),p_l);
				
				x_state(0,0) = x_l;
				x_state(1,0) = y_l;
				x_state(2,0) = p_l.X() / p_l.Z();
				x_state(3,0) = p_l.Y() / p_l.Z();
				x_state(4,0) = abs(gCandidate->GetCharge()) / p_l.Mag();
				// Force exact symmetry after propagation step
				for (Int_t ij = 0; ij < 5; ++ij) {
					for (Int_t jj = ij + 1; jj < 5; ++jj) {
						Double_t avg = 0.5 * (P_cov(ij, jj) + P_cov(jj, ij));
						P_cov(ij, jj) = avg;
						P_cov(jj, ij) = avg;
					}
				}
				if(writeout)
				{
					cout<<"Backward: Det: "<<det->GetDetectorName()<<" passed through"<<endl;
					cout<<"Energy loss: "<<det->GetEnergyLoss(gCandidate)<<", sigma_E_squared: "<<sigma_E_squared<<endl;
					cout<<"Position: "<<gCandidate->GetPosition().X()<<" "<<gCandidate->GetPosition().Y()<<" "<<gCandidate->GetPosition().Z()<<endl;
					cout<<"Momentum: "<<gCandidate->GetMomentum().X()<<" "<<gCandidate->GetMomentum().Y()<<" "<<gCandidate->GetMomentum().Z()<<endl;
					cout<<"State values: "<<x_state(0,0)<<" "<<x_state(1,0)<<" "<<x_state(2,0)<<" "<<x_state(3,0)<<" "<<x_state(4,0)<<endl;
					cout<<"P_cov values: "<<P_cov(0,0)<<" "<<P_cov(1,1)<<" "<<P_cov(2,2)<<" "<<P_cov(3,3)<<" "<<P_cov(4,4)<<endl;
				}			
				
				// ==========================================
				// 2. MULTIPLE COULOMB SCATTERING (Noise Q)
				// ==========================================
				// Calculate scattering angle using the Highland Formula based on detector material thickness
				Double_t L_over_X0 = weight * det->thickness / rad_length; 
				Double_t p_tot = gCandidate->GetMomentum().Mag(); // in MeV/c or GeV/c
				mass  = gCandidate->GetMass();          
				// Calculate beta = p / E
				Double_t E_tot = TMath::Sqrt(p_tot * p_tot + mass * mass);
				Double_t beta  = p_tot / E_tot;
				Double_t theta_scat = (0.0136 * x_state(4,0)/beta) * TMath::Sqrt(L_over_X0) * (1.0 + 0.038 * TMath::Log(L_over_X0));
				Double_t var_scat = theta_scat * theta_scat;
				
				// 2. Convert Energy Variance (\sigma_E^2) to q/p Variance (\sigma_{q/p}^2)
				// Using relativistic kinematics: E^2 = p^2 + m^2 -> dE = (p/E) * dp
				// dp = (E/p) * dE
				Double_t sigma_p_squared = (E_tot / p_tot) * (E_tot / p_tot) * sigma_E_squared;

				// Since state variable is q/p, d(q/p)/dp = -q / p^2
				// Therefore: var(q/p) = (q / p^2)^2 * var(p)
				Double_t qOverP = abs(gCandidate->GetCharge()) / p_tot;
				Double_t sigma_qOverP_squared = TMath::Power(qOverP / p_tot, 2) * sigma_p_squared;
					   
				Q_mat.Zero(); // clear matrix
				Q_mat(2, 2) = var_scat;
				Q_mat(3, 3) = var_scat;
				Q_mat(4, 4) = sigma_qOverP_squared;
				// Add process noise to the track covariance matrix
				P_cov = P_cov + Q_mat;
								
				// We have to distinguish between detectors measuring only x, y or both
				// Measurement vector z (what the detector actually saw)
				if(i == 0 || det->GetDetectorName() == "tofd") measDim = 2;
				else measDim = 1;
	
				TMatrixD z_meas(measDim, 1); z_meas.Zero();
				// Measurement projection matrix H (maps 5D state down to the measured components x, y)
				TMatrixD H_mat(measDim, 5); H_mat.Zero();
				// Detector resolution matrix R
				TMatrixD R_noise(measDim, measDim); R_noise.Zero();
			
				if (det->GetDetectorName() == "tofd") // x and y measured
				{
					H_mat(0,0) = 1.0; 
					H_mat(1,1) = 1.0;
					R_noise(0,0) = det->res_x * det->res_x * 4./12.;
					R_noise(1,1) = det->res_y * det->res_y;
					if(helium) R_noise(1,1) = R_noise(1,1) * 9.;
					z_meas(0,0) = hit->GetX();
					z_meas(1,0) = hit->GetY();
				}
				else if(kTarget == det->section)
				{
					H_mat(0,0) = 1.0; 
					H_mat(1,1) = 1.0;
					R_noise(0,0) = 1.5 * 1.5 * 4./12.;
					R_noise(1,1) = 1.5 * 1.5 * 4./12.;
					z_meas(0,0) = gCandidate->GetStartPosition().X();
					z_meas(1,0) = gCandidate->GetStartPosition().Y();				
				}			
				else if (det->GetDetectorName() == "fi23b")  // only y position measured
				{
					H_mat(0,1) = 1.0; // Maps to the y parameter of x_state
					R_noise(0,0) = det->res_y * det->res_y  * 4./12.;
					z_meas(0,0) = hit->GetY();
				}
				else  // only x position measured
				{
					H_mat(0,0) = 1.0; // Maps to the y parameter of x_state
					R_noise(0,0) = det->res_x * det->res_x  * 4./12.;
					z_meas(0,0) = hit->GetX();
				}
				

			// ==========================================
			// 3. THE UPDATE STEP (Filtering the Hit)
			// ==========================================
					
			// Compute residual and its covariance matrix S 
			// S = H * P * H^T + R
			TMatrixD S_mat(measDim, measDim);
			S_mat = H_mat * P_cov * TMatrixD(TMatrixD::kTransposed, H_mat) + R_noise;
						
			// Residual: 
			TMatrixD residual(measDim, 1);
			residual = z_meas - (H_mat * x_state);
/*				
			Double_t res_val, resolution, pull, damping_factor;
			if(measDim == 1)
			{				
				res_val = residual(0,0);
				if(det->GetDetectorName() == "fi23b") resolution = det->res_y;
				else resolution = det->res_x;

				// Calculate how many "sigmas" the track is from the hit
				pull = TMath::Abs(res_val) / resolution;

				if (pull < 1.0) 
				{
					// If inside 1 sigma of detector resolution, smoothly damp the residual.
					// At pull = 0, factor is 0. At pull = 1, factor is 1.
					damping_factor = pull * pull; // Smooth parabolic scaling
					residual(0,0) = res_val * damping_factor;
				}
			}
			else
			{
				res_val = residual(0,0);
				resolution = sqrt(R_noise(0,0));//det->res_x;
				// Calculate how many "sigmas" the track is from the hit
				pull = TMath::Abs(res_val) / resolution;
				if (pull < 1.0) 
				{
					// If inside 1 sigma of detector resolution, smoothly damp the residual.
					// At pull = 0, factor is 0. At pull = 1, factor is 1.
					damping_factor = pull * pull; // Smooth parabolic scaling
					residual(0,0) = res_val * damping_factor;
				}
				
				res_val = residual(1,0);
				resolution = sqrt(R_noise(1,1));//det->res_y;
				// Calculate how many "sigmas" the track is from the hit
				pull = TMath::Abs(res_val) / resolution;
				if (pull < 1.0) 
				{
					// If inside 1 sigma of detector resolution, smoothly damp the residual.
					// At pull = 0, factor is 0. At pull = 1, factor is 1.
					damping_factor = pull * pull; // Smooth parabolic scaling
					residual(1,0) = res_val * damping_factor;
				}	
			}
*/			

/*
			// Extract Pulls for measured coordinates
			Double_t pull_x = 0.0;
			Double_t pull_y = 0.0;

			if (measDim == 1) {
				Double_t var_x = S_mat(0, 0); // Total variance in X
				if (var_x > 0.0) {
					pull_x = residual(0,0) / sqrt(var_x);
				}
			}
			else {
				Double_t var_x = S_mat(0, 0); // Total variance in X
				if (var_x > 0.0) {
					pull_x = residual(0,0) / sqrt(var_x);
				}
				Double_t var_y = S_mat(1, 1); // Total variance in y
				if (var_y > 0.0) {
					pull_y = residual(1,0) / sqrt(var_y);
				}				
			}
		
			if (measDim == 2) {
				gCandidate->SetResidual(det->GetDetectorName(), residual(0,0), residual(1,0), pull_x, pull_y);
			} 
			else {
				// 1D measurement (determine whether it's X or Y based on det type)
				if (det->GetDetectorName() == "fi23b") {
					gCandidate->SetResidual(det->GetDetectorName(), 0.0, residual(0,0), 0., pull_x);
				} 
				else {
					gCandidate->SetResidual(det->GetDetectorName(), residual(0,0), 0.0, pull_x, 0.);
				}
			}
			
			*/
			
				// Compute Kalman Gain: K = P * H^T * S^-1
				TMatrixD K_gain(5, measDim);
				TMatrixD S_inv = S_mat; S_inv.Invert();
				K_gain = P_cov * TMatrixD(TMatrixD::kTransposed, H_mat) * S_inv;
				
				// Update local state
				x_state = x_state + (K_gain * residual);	
				
				if(writeout){
					cout<<"Det: "<<det->GetDetectorName()<<" after noise"<<endl;
					if(kTarget == det->section || det->GetDetectorName() == "tofd")
					{
						cout <<"Residues: "<<residual(0,0)<<", "<<residual(1,0)<<endl;
						cout <<"Measurem: "<<z_meas(0,0)<<", "<<z_meas(1,0)<<endl;
					}
					else
					{
						cout <<"Residues: "<<residual(0,0)<<endl;
						cout <<"Measurem: "<<z_meas(0,0)<<endl;
					}
					
					cout<<"Noise: "<<var_scat<<", "<<sigma_qOverP_squared<<endl;
					cout<<"K_gain: "<<K_gain(0,0)<<" "<<K_gain(1,0)<<" "<<K_gain(2,0)<<" "<<K_gain(3,0)<<" "<<K_gain(4,0)<<endl;
					cout<<"P_cov values with noise: "<<P_cov(0,0)<<" "<<P_cov(1,1)<<" "<<P_cov(2,2)<<" "<<P_cov(3,3)<<" "<<P_cov(4,4)<<endl;
					cout<<"Updated state values: "<<x_state(0,0)<<" "<<x_state(1,0)<<" "<<x_state(2,0)<<" "<<x_state(3,0)<<" "<<x_state(4,0)<<endl;
				}
				
		
				// Update the Covariance Matrix: P = (I - K * H) * P
			// 1. Construct Identity Matrix
			TMatrixD I(5, 5);
			I.UnitMatrix();

			// 2. Compute (I - K * H)
			TMatrixD I_minus_KH = I - (K_gain * H_mat);

			// 3. Compute Transpose (I - K * H)^T
			TMatrixD I_minus_KH_T(TMatrixD::kTransposed, I_minus_KH);

			// 4. Compute First Term: (I - KH) * P * (I - KH)^T
			TMatrixD P_updated = I_minus_KH * P_cov * I_minus_KH_T;

			// 5. Compute Transpose of K_gain
			TMatrixD K_gain_T(TMatrixD::kTransposed, K_gain);

			// 6. Compute Second Term: K * R * K^T
			TMatrixD KRK_T = K_gain * R_noise * K_gain_T;

			// 7. Complete Joseph Form Update
			P_cov = P_updated + KRK_T;

			// 8. Explicitly Enforce Matrix Symmetry (Numerical Safeguard)
			for (Int_t ij = 0; ij; ++ij) {
				for (Int_t jj = ij + 1; jj < 5; ++jj) {
					Double_t avg = 0.5 * (P_cov(ij, jj) + P_cov(jj, ij));
					P_cov(ij, jj) = avg;
					P_cov(jj, ij) = avg;
				}
			}
				if(writeout) cout<<"Updated P_cov values: "<<P_cov(0,0)<<" "<<P_cov(1,1)<<" "<<P_cov(2,2)<<" "<<P_cov(3,3)<<" "<<P_cov(4,4)<<endl;
		
				// Increment local chi2 safely using the measurement covariance matrix
				TMatrixD chi2_increment = TMatrixD(TMatrixD::kTransposed, residual) * S_inv * residual;
				//cout<<"chi2_increment: "<<chi2_increment(0,0)<<endl;
								
				chi2_loc += chi2_increment(0,0);
				
				// CRITICAL: Push the freshly updated Kalman position and slopes back into gCandidate 
				// so the next Runge-Kutta step propagates the corrected trajectory!
				
				// 1. Reconstruct the new local momentum vector from the updated slopes
				// We use the fact that p_x = ux * p_z and p_y = uy * p_z
				ptot_new = abs(gCandidate->GetCharge()) / x_state(4,0);

				// Use simple geometry to find the new local p_z component
				x_new_local = x_state(0,0);
				y_new_local = x_state(1,0);
				ux_new = x_state(2,0);
				uy_new = x_state(3,0);
				pz_new_local = ptot_new / TMath::Sqrt(1.0 + ux_new*ux_new + uy_new*uy_new);

				TVector3 p_new_local(ux_new * pz_new_local, uy_new * pz_new_local, pz_new_local);
				
				// 2. Rotate the local momentum vector BACK into the global lab frame
				det->LocalToGlobalMomentum(p_new_global, p_new_local); 
				
				det->LocalToGlobal(pos_new_global,x_new_local,y_new_local);
				E_tot = TMath::Sqrt(p_new_global.Mag() * p_new_global.Mag() + mass * mass);
				beta  = p_new_global.Mag() / E_tot;
				gCandidate->SetPosition(pos_new_global);
				gCandidate->SetMomentum(-p_new_global);  // this is ok; slopes are preserved
				gCandidate->SetBeta(beta);
				
				
				if(writeout)
				{
					cout<<"Updated candidate position: "<<gCandidate->GetPosition().X()<<" "<<gCandidate->GetPosition().Y()<<" "<<gCandidate->GetPosition().Z()<<endl;
					cout<<"Updated candidate momentum: "<<gCandidate->GetMomentum().X()<<" "<<gCandidate->GetMomentum().Y()<<" "<<gCandidate->GetMomentum().Z()<<endl;				    
				    cout<<"chi2_det: "<<chi2_increment(0,0)<<endl;
				}				
			}
		}

	gCandidate->SetStartPosition(gCandidate->GetPosition());
	gCandidate->SetStartMomentum(-gCandidate->GetMomentum());
	gCandidate->SetCharge(abs(gCandidate->GetCharge()));
	gCandidate->SetStartBeta(gCandidate->GetBeta());
	gCandidate->SetChi2(chi2);
	gCandidate->Reset();
   
    cout<<"*** New fragment ****** "<<endl;
    cout<<"*** Charge: "<<gCandidate->GetCharge()<<endl;
    cout<<"*** Position: "<<gCandidate->GetStartPosition().X()<<", "<<gCandidate->GetStartPosition().Y()<<", "<<gCandidate->GetStartPosition().Z()<<endl;
    cout<<"*** Momentum: "<<gCandidate->GetStartMomentum().X()<<", "<<gCandidate->GetStartMomentum().Y()<<", "<<gCandidate->GetStartMomentum().Z()<<endl;
    cout<<"*** Beta: "<<gCandidate->GetStartBeta()<<endl;
    cout<<"*** Chi2: "<<chi2<<", "<<chi2_loc<<endl;
    if(chi2 > 1.e6) status = 10;

    return status;
}

Int_t R3BFragmentFitterChi2S494::FitTrackMomentumBackward(R3BTrackingParticle* particle, R3BTrackingSetup* setup)
{
    LOG(debug3) << "In track momentum backward" << endl;
    gCandidate = particle;
    gSetup = setup;

    Int_t status = 0;
    Double_t px0 = 0.;
    Double_t py0 = 0.;
    Double_t pz0 = 0.;
    Double_t x0 = 0.;
    Double_t y0 = 0.;
    Double_t z0 = 0.;
    Double_t y_tp = 0.;
    Double_t yfi30=-100.,yfi31=-100.,yfi32=-100.,yfi33=-100.;
    Double_t foffset=0.441;
    Double_t fslope=1.;
    Double_t ltofd=0.;
    Double_t ztp = 174.5851;
    Double_t x_l=0., y_l=0.;
	Double_t pz_new_local = 0., ptot_new=0.;
    Double_t chi2 = 0.;
    Double_t beta_target;
    Bool_t writeout = false;
    TVector3 pos0;
    TVector3 pos1;
    TVector3 pos2;
    TVector3 pos3;
    TVector3 pos23b;
    TVector3 direction0;
    TVector3 startPosition;
    TVector3 startPosOptimized;
    TVector3 startPositionOptimized;
    TVector3 startMomentumOptimized;
    TVector3 startMomentumLocal;
    TVector3 pos_target;
    TVector3 mom_target;
    TVector3 p_l;
    TMatrixD x_state(5, 1);  // 5x1 Column Vector
	TMatrixD P_cov(5, 5);    // 5x5 Covariance Matrix
	TMatrixD Q_mat(5,5);  // 5x5 Noise matrix
	
	Double_t x0true = gCandidate->GetStartPosition().X();
	Double_t y0true = gCandidate->GetStartPosition().Y();
	Double_t z0true = gCandidate->GetStartPosition().Z();
	
    
    if (gCandidate->GetHitIndexByName("tofd") > -1)
    {
        auto tofd = gSetup->GetByName("tofd");
        tofd->LocalToGlobal(pos0,
                            gSetup->GetHit("tofd", gCandidate->GetHitIndexByName("tofd"))->GetX(),
                            gSetup->GetHit("tofd", gCandidate->GetHitIndexByName("tofd"))->GetY());
        ltofd = sqrt((pos0.Z()-ztp)*(pos0.Z()-ztp) + pos0.X()*pos0.X()) ;
    }
    
    if (gCandidate->GetHitIndexByName("fi23b") > -1)
    {
        auto fi23b = gSetup->GetByName("fi23b");
        fi23b->LocalToGlobal(pos23b, 0.0, gSetup->GetHit("fi23b", gCandidate->GetHitIndexByName("fi23b"))->GetY());
    }
 
 Double_t diffx = 0.;
 // Extrapolate y position between fi23b and tofd to fi3x:   
    if (gCandidate->GetHitIndexByName("fi32") > -1 && gCandidate->GetHitIndexByName("fi30") > -1)
    {
		diffx = -0.7;
        auto fi32 = gSetup->GetByName("fi32");
        auto fi30 = gSetup->GetByName("fi30");
        fi32->LocalToGlobal(pos2, gSetup->GetHit("fi32", gCandidate->GetHitIndexByName("fi32"))->GetX()-diffx, 0.);
        fi30->LocalToGlobal(pos3, gSetup->GetHit("fi30", gCandidate->GetHitIndexByName("fi30"))->GetX(), 0.);
        
        fslope = 0.883;
        y_tp = (pos0.Y()-foffset+fslope*ltofd*pos23b.Y()/(ztp-pos23b.Z()))/
														 (1.+fslope*ltofd/(ztp-pos23b.Z()));  
	    Double_t lfi30 = sqrt((pos3.Z()-ztp)*(pos3.Z()-ztp) + pos3.X()*pos3.X()) ;			
		yfi30 = y_tp + lfi30/(ztp-pos23b.Z())*(y_tp-pos23b.Y())*fslope + foffset; 
		pos3.SetY(yfi30);
	    
	    Double_t lfi32 = sqrt((pos2.Z()-ztp)*(pos2.Z()-ztp) + pos2.X()*pos2.X()) ;			
		yfi32 = y_tp + lfi32/(ztp-pos23b.Z())*(y_tp-pos23b.Y())*fslope + foffset;
		pos2.SetY(yfi32);
    }
    else if (gCandidate->GetHitIndexByName("fi31") > -1 && gCandidate->GetHitIndexByName("fi33") > -1)
    {
        diffx = 0.53;
        auto fi31 = gSetup->GetByName("fi31");
        auto fi33 = gSetup->GetByName("fi33");
        fi33->LocalToGlobal(pos2, gSetup->GetHit("fi33", gCandidate->GetHitIndexByName("fi33"))->GetX()-diffx, 0.);
        fi31->LocalToGlobal(pos3, gSetup->GetHit("fi31", gCandidate->GetHitIndexByName("fi31"))->GetX(), 0.);
        
        fslope = 0.901;
        y_tp = (pos0.Y()-foffset+fslope*ltofd*pos23b.Y()/(ztp-pos23b.Z()))/
														 (1.+fslope*ltofd/(ztp-pos23b.Z()));  
	    Double_t lfi31 = sqrt((pos3.Z()-ztp)*(pos3.Z()-ztp) + pos3.X()*pos3.X()) ;			
		yfi31 = y_tp + lfi31/(ztp-pos23b.Z())*(y_tp-pos23b.Y())*fslope + foffset; 
		pos3.SetY(yfi31);
	    
	    Double_t lfi33 = sqrt((pos2.Z()-ztp)*(pos2.Z()-ztp) + pos2.X()*pos2.X()) ;			
		yfi33 = y_tp + lfi33/(ztp-pos23b.Z())*(y_tp-pos23b.Y())*fslope + foffset;
		pos2.SetY(yfi33);
    }
    else
    {
        return 10;
    }
      Double_t ygeom = pos23b.Y() - pos23b.Z() * (y_tp - pos23b.Y()) / (ztp - pos23b.Z());

       // Pass inital candidate through detectors, correct for energy loss, and get start momentum at tofd:	
		
	for (Int_t i = 0; i < gSetup->GetArray().size();  i++) 
	{
		auto det = gSetup->GetArray().at(i); 
		R3BHit* hit = nullptr; 
		Int_t hitIndex = gCandidate->GetHitIndexByName(det->GetDetectorName().Data());
		
		if(hitIndex < 0) continue;
		
		if (hitIndex >= 0)
		{
			hit = gSetup->GetHit(det->GetDetectorName().Data(), hitIndex);
		}

		if (!hit) continue;
		
		Double_t weight = 1.;
		if (kTarget == det->section || det->GetDetectorName() == "tofd")
		{
			weight = 0.5;
		}
	   // Update particle's momentum is due to energy loss in a detector:
		gCandidate->PassThroughDetector(det, weight); 
			
	} 
	Double_t mom = gCandidate->GetMomentum().Mag();
	direction0 = (pos2 - pos0);
	
	direction0 = direction0.Unit();
	
	direction0.SetMag(mom);
	
	TVector3 startMomentum(direction0.X(), direction0.Y(), direction0.Z());

	px0 = direction0.X();
	py0 = direction0.Y();
	pz0 = direction0.Z();

	x0 = pos0.X();
	y0 = pos0.Y();
	z0 = pos0.Z();
	
	Double_t charge = gCandidate->GetCharge();
	gCandidate->SetCharge(-charge);
	gCandidate->SetStartPosition(pos0);
	gCandidate->SetStartMomentum(startMomentum);
	gCandidate->Reset();
	
	if (gCandidate->GetHitIndexByName("tofd") > -1)
	{
		auto tofd = gSetup->GetByName("tofd");
		tofd->GlobalToLocalMomentum(startMomentum,startMomentumLocal);
		tofd->GlobalToLocal(pos0,x_l,y_l);
	}
	
	// --- BEFORE THE DETECTOR LOOP ---
	// Initialize local state at the tofd
	x_state(0,0) = x_l; // initial x 
	x_state(1,0) = y_l; // initial y
	x_state(2,0) = startMomentumLocal.X() / startMomentumLocal.Z(); // initial slope dx/dz
	x_state(3,0) = startMomentumLocal.Y() / startMomentumLocal.Z(); // initial slope dy/dz
	x_state(4,0) = abs(gCandidate->GetCharge()) / startMomentumLocal.Mag(); // q/p 
	
	
	// Initialize starting uncertainties (high values mean "I trust the hits more than my guess")
	P_cov.Zero();
	P_cov(0,0) = 2. * 2. * 4./12.; 
	P_cov(1,1) = 1. * 1.; 
	P_cov(2,2) = 5.e-2 * 5.e-2;//0.05 * 0.05 ;     
	P_cov(3,3) = 5.e-2 * 5.e-2;//0.05 * 0.05 ;     
	P_cov(4,4) = (0.005 * abs(gCandidate->GetCharge()) / startMomentumLocal.Mag()) *
	           (0.005 * abs(gCandidate->GetCharge()) / startMomentumLocal.Mag());//0.07 * 0.07;     
	
	if(writeout)
	{
		cout<<"Backward tracking starting values:"<<endl;
		cout<<"Position: "<<pos0.X()<<" "<<pos0.Y()<<" "<<pos0.Z()<<endl;
		cout<<"Momentum: "<<startMomentum.X()<<" "<<startMomentum.Y()<<" "<<startMomentum.Z()<<endl;
		cout<<"State values at init: "<<x_state(0,0)<<" "<<x_state(1,0)<<" "<<x_state(2,0)<<" "<<x_state(3,0)<<x_state(4,0)<<endl;
		cout<<"P_cov values at init: "<<P_cov(0,0)<<" "<<P_cov(1,1)<<" "<<P_cov(2,2)<<" "<<P_cov(3,3)<<" "<<P_cov(4,4)<<endl;
	}
	
	Int_t measDim = 2;
	Double_t dxerror = 1., dyerror = 1.; 
	Double_t sigma_E_squared = 0.;	
	Int_t n_ghost_track = 0;	
	for (Int_t i = gSetup->GetArray().size() - 1; i >= 0; i--) 
	{
		auto det = gSetup->GetArray().at(i);  
	   // hit data at a given detector; contains: detector Id, x and y positions of a particle 
	   // at the detector, as well asparticle's charge and time
		R3BHit* hit = nullptr; 
		Int_t hitIndex = gCandidate->GetHitIndexByName(det->GetDetectorName().Data());
		
		if(hitIndex < 0) continue;
		
		if (hitIndex >= 0)
		{
			hit = gSetup->GetHit(det->GetDetectorName().Data(), hitIndex);
		}

		if (!hit) continue;
		
		// ==========================================
		// 1. THE PREDICT STEP (Extrapolation)
		// ==========================================
		// Your existing Runge-Kutta engine propagates the physical state backward!
		TVector3 pos_before = gCandidate->GetPosition(); // Before propagation
		
		if(writeout)
		{
			cout<<"#### Propagation to det: "<<det->GetDetectorName()<<" will be started ##"<<endl;
			cout<<"Position: "<<gCandidate->GetPosition().X()<<" "<<gCandidate->GetPosition().Y()<<" "<<gCandidate->GetPosition().Z()<<endl;
			cout<<"Momentum: "<<gCandidate->GetMomentum().X()<<" "<<gCandidate->GetMomentum().Y()<<" "<<gCandidate->GetMomentum().Z()<<endl;
			cout<<"State values: "<<x_state(0,0)<<" "<<x_state(1,0)<<" "<<x_state(2,0)<<" "<<x_state(3,0)<<" "<<x_state(4,0)<<endl;
			cout<<"P_cov values: "<<P_cov(0,0)<<" "<<P_cov(1,1)<<" "<<P_cov(2,2)<<" "<<P_cov(3,3)<<" "<<P_cov(4,4)<<endl;
		}
   
		if(i < gSetup->GetArray().size() )
		{
			gProp->PropagateToDetectorBackward(gCandidate, det, P_cov);
			
			Double_t weight = 1.;
			if (gEnergyLoss ) // correct for energy loss in detector
			{
				if (kTarget == det->section || det->GetDetectorName() == "tofd")
				{
					weight = 0.5;
				}
				sigma_E_squared = weight * det->GetEnergyLossStraggling(gCandidate);
			   // Update particle's momentum is due to energy loss in a detector:
				gCandidate->PassThroughDetectorBackward(det, weight); 				
			 }
														 
			// Convert the newly predicted gCandidate back into our 5D local state matrix
			det->GlobalToLocal(gCandidate->GetPosition(), x_l, y_l);
			det->GlobalToLocalMomentum(gCandidate->GetMomentum(),p_l);
			
			x_state(0,0) = x_l;
			x_state(1,0) = y_l;
			x_state(2,0) = p_l.X() / p_l.Z();
			x_state(3,0) = p_l.Y() / p_l.Z();
			x_state(4,0) = abs(gCandidate->GetCharge()) / p_l.Mag();
			// Force exact symmetry after propagation step
			for (Int_t ij = 0; ij < 5; ++ij) {
				for (Int_t jj = ij + 1; jj < 5; ++jj) {
					Double_t avg = 0.5 * (P_cov(ij, jj) + P_cov(jj, ij));
					P_cov(ij, jj) = avg;
					P_cov(jj, ij) = avg;
				}
			}
			if(writeout)
			{
				cout<<"Det: "<<det->GetDetectorName()<<" passed through"<<endl;
				cout<<"Position: "<<gCandidate->GetPosition().X()<<" "<<gCandidate->GetPosition().Y()<<" "<<gCandidate->GetPosition().Z()<<endl;
				cout<<"Momentum: "<<gCandidate->GetMomentum().X()<<" "<<gCandidate->GetMomentum().Y()<<" "<<gCandidate->GetMomentum().Z()<<endl;
				cout<<"State values: "<<x_state(0,0)<<" "<<x_state(1,0)<<" "<<x_state(2,0)<<" "<<x_state(3,0)<<" "<<x_state(4,0)<<endl;
				cout<<"P_cov values: "<<P_cov(0,0)<<" "<<P_cov(1,1)<<" "<<P_cov(2,2)<<" "<<P_cov(3,3)<<" "<<P_cov(4,4)<<endl;
			}
		
			
			// ==========================================
			// 2. MULTIPLE COULOMB SCATTERING (Noise Q)
			// ==========================================
				   
			Q_mat.Zero(); // Clear process noise matrix

			Double_t p_tot_GeV = gCandidate->GetMomentum().Mag(); // Make sure this is GeV/c
			Double_t mass = gCandidate->GetMass(); // GeV/c^2       
			Double_t E_tot     = TMath::Sqrt(p_tot_GeV * p_tot_GeV + mass * mass);
			Double_t beta      = p_tot_GeV / E_tot;
					
			if (!(det->thickness <= 0.0 || rad_length <= 0.0 || p_tot_GeV <= 0.0)) 
			{
				// Effective radiation length step taking tilt/weight into account
				Double_t L_over_X0 = weight * det->thickness / rad_length; 
				
				// Protect against log of very small numbers
				if (L_over_X0 > 1e-7) 
				{
					
					// 1. Highland Formula for Multiple Scattering Angle (radians)
					Double_t qOverP_abs = TMath::Abs(x_state(4,0)); // |q/p| in 1/GeV
					Double_t theta_scat = (0.0136 * qOverP_abs / beta) * TMath::Sqrt(L_over_X0) * (1.0 + 0.038 * TMath::Log(L_over_X0));
					Double_t var_scat   = theta_scat * theta_scat;

					// 2. Slope variance projection
					Double_t ux = x_state(2,0);
					Double_t uy = x_state(3,0);
					Double_t xi = 1.0 + ux * ux + uy * uy;
					Double_t var_ux = (1.0 + ux * ux) * xi * var_scat;
					Double_t var_uy = (1.0 + uy * uy) * xi * var_scat;

					// 3. Finite detector thickness correlations
					Double_t var_pos = (det->thickness * det->thickness / 3.0) * var_scat;
					Double_t cov_pos_slope = (det->thickness / 2.0) * var_scat;
					
					// 4. Energy straggling conversion from sigma_p^2 to Var(q/p)
					Double_t sigma_p_squared = (E_tot / p_tot_GeV) * (E_tot / p_tot_GeV) * sigma_E_squared;
					Double_t var_qOverP      = (qOverP_abs * qOverP_abs) * (sigma_p_squared / (p_tot_GeV * p_tot_GeV));

					// Populate 5x5 Q matrix
					Q_mat(0, 0) = var_pos;
					Q_mat(1, 1) = var_pos;
					Q_mat(2, 2) = var_ux;
					Q_mat(3, 3) = var_uy;
					
					Q_mat(0, 2) = cov_pos_slope;
					Q_mat(2, 0) = cov_pos_slope;
					Q_mat(1, 3) = cov_pos_slope;
					Q_mat(3, 1) = cov_pos_slope;
					
					Q_mat(4, 4) = var_qOverP;
				}
			}
			// Add process noise to the track covariance matrix
			P_cov = P_cov + Q_mat;
							
			// We have to distinguish between detectors measuring only x, y or both
			// Measurement vector z (what the detector actually saw)
			if(i == 0 || det->GetDetectorName() == "tofd") measDim = 2;
			else measDim = 1;
		
			TMatrixD z_meas(measDim, 1); z_meas.Zero();
			// Measurement projection matrix H (maps 5D state down to the measured components x, y)
			TMatrixD H_mat(measDim, 5); H_mat.Zero();
			// Detector resolution matrix R
			TMatrixD R_noise(measDim, measDim); R_noise.Zero();
		
			if (det->GetDetectorName() == "tofd") // x and y measured
			{
				H_mat(0,0) = 1.0; 
				H_mat(1,1) = 1.0;
				R_noise(0,0) = det->res_x * det->res_x  * 4./12.;
				R_noise(1,1) = det->res_y * det->res_y;
				z_meas(0,0) = hit->GetX();
				z_meas(1,0) = hit->GetY();
			}
			else if(kTarget == det->section) // target
			{
				H_mat(0,0) = 1.0; 
				H_mat(1,1) = 1.0;
				R_noise(0,0) = 0.5 * 0.5;
				R_noise(1,1) = 0.5 * 0.5;
				z_meas(0,0) = x0true;
				z_meas(1,0) = y0true;
			}			
			else if (det->GetDetectorName() == "fi23b")  // only y position measured
			{
				H_mat(0,1) = 1.0; // Maps to the y parameter of x_state
				R_noise(0,0) = det->res_y * det->res_y * 4./12.;
				z_meas(0,0) = hit->GetY();
			}
			else  // only x position measured
			{
				H_mat(0,0) = 1.0; // Maps to the y parameter of x_state
				R_noise(0,0) = det->res_x * det->res_x  * 4./12.;
				z_meas(0,0) = hit->GetX();
			}			

			// ==========================================
			// 3. THE UPDATE STEP (Filtering the Hit)
			// ==========================================
					
			// Compute residual and its covariance matrix S 
			// S = H * P * H^T + R
			TMatrixD S_mat(measDim, measDim);
			S_mat = H_mat * P_cov * TMatrixD(TMatrixD::kTransposed, H_mat) + R_noise;
						
			// Residual: 
			TMatrixD residual(measDim, 1);
			residual = z_meas - (H_mat * x_state);
									
			// Extract Pulls for measured coordinates
			Double_t pull_x = 0.0;
			Double_t pull_y = 0.0;

			if (measDim == 1) {
				Double_t var_x = S_mat(0, 0); // Total variance in X
				if (var_x > 0.0) {
					pull_x = residual(0,0) / sqrt(var_x);
				}
			}
			else {
				Double_t var_x = S_mat(0, 0); // Total variance in X
				if (var_x > 0.0) {
					pull_x = residual(0,0) / sqrt(var_x);
				}
				Double_t var_y = S_mat(1, 1); // Total variance in y
				if (var_y > 0.0) {
					pull_y = residual(1,0) / sqrt(var_y);
				}				
			}
		
			if (measDim == 2) {
				gCandidate->SetResidual(det->GetDetectorName(), residual(0,0), residual(1,0), pull_x, pull_y);
			} 
			else {
				// 1D measurement (determine whether it's X or Y based on det type)
				if (det->GetDetectorName() == "fi23b") {
					gCandidate->SetResidual(det->GetDetectorName(), 0.0, residual(0,0), 0., pull_x);
				} 
				else {
					gCandidate->SetResidual(det->GetDetectorName(), residual(0,0), 0.0, pull_x, 0.);
				}
			}

			// Compute Kalman Gain: K = P * H^T * S^-1
			TMatrixD K_gain(5, measDim);
			TMatrixD S_inv = S_mat; S_inv.Invert();
			K_gain = P_cov * TMatrixD(TMatrixD::kTransposed, H_mat) * S_inv;
			
			// Update local state
			x_state = x_state + (K_gain * residual);	
			
			if(writeout)
			{
				cout<<"Det: "<<det->GetDetectorName()<<" after noise"<<endl;
				if(kTarget == det->section || det->GetDetectorName() == "tofd")
				{
					cout <<"Residues: "<<residual(0,0)<<", "<<residual(1,0)<<endl;
					cout <<"Measurem: "<<z_meas(0,0)<<", "<<z_meas(1,0)<<endl;
				}
				else
				{
					cout <<"Residues: "<<residual(0,0)<<endl;
					cout <<"Measurem: "<<z_meas(0,0)<<endl;
				}
				
				//cout<<"Noise: "<<var_scat<<", "<<sigma_qOverP_squared<<endl;
				cout<<"K_gain: "<<K_gain(0,0)<<" "<<K_gain(1,0)<<" "<<K_gain(2,0)<<" "<<K_gain(3,0)<<" "<<K_gain(4,0)<<endl;
				cout<<"Position: "<<gCandidate->GetPosition().X()<<" "<<gCandidate->GetPosition().Y()<<" "<<gCandidate->GetPosition().Z()<<endl;
				cout<<"Momentum: "<<gCandidate->GetMomentum().X()<<" "<<gCandidate->GetMomentum().Y()<<" "<<gCandidate->GetMomentum().Z()<<endl;
				cout<<"Updated state values: "<<x_state(0,0)<<" "<<x_state(1,0)<<" "<<x_state(2,0)<<" "<<x_state(3,0)<<" "<<x_state(4,0)<<endl;
				cout<<"P_cov values with noise: "<<P_cov(0,0)<<" "<<P_cov(1,1)<<" "<<P_cov(2,2)<<" "<<P_cov(3,3)<<" "<<P_cov(4,4)<<endl;
			}
	
			// Update the Covariance Matrix: P = (I - K * H) * P
			// 1. Construct Identity Matrix
			TMatrixD I(5, 5);
			I.UnitMatrix();

			// 2. Compute (I - K * H)
			TMatrixD I_minus_KH = I - (K_gain * H_mat);

			// 3. Compute Transpose (I - K * H)^T
			TMatrixD I_minus_KH_T(TMatrixD::kTransposed, I_minus_KH);

			// 4. Compute First Term: (I - KH) * P * (I - KH)^T
			TMatrixD P_updated = I_minus_KH * P_cov * I_minus_KH_T;

			// 5. Compute Transpose of K_gain
			TMatrixD K_gain_T(TMatrixD::kTransposed, K_gain);

			// 6. Compute Second Term: K * R * K^T
			TMatrixD KRK_T = K_gain * R_noise * K_gain_T;

			// 7. Complete Joseph Form Update
			P_cov = P_updated + KRK_T;

			// 8. Explicitly Enforce Matrix Symmetry (Numerical Safeguard)
			for (Int_t ij = 0; ij < 5; ++ij) {
				for (Int_t jj = ij + 1; jj < 5; ++jj) {
					Double_t avg = 0.5 * (P_cov(ij, jj) + P_cov(jj, ij));
					P_cov(ij, jj) = avg;
					P_cov(jj, ij) = avg;
				}
			}
			if(writeout) cout<<"Updated P_cov values: "<<P_cov(0,0)<<" "<<P_cov(1,1)<<" "<<P_cov(2,2)<<" "<<P_cov(3,3)<<" "<<P_cov(4,4)<<endl;
	
			// Increment local chi2 safely using the measurement covariance matrix
			TMatrixD chi2_increment = TMatrixD(TMatrixD::kTransposed, residual) * S_inv * residual;
			
			chi2 += chi2_increment(0,0);
			
			// CRITICAL: Push the freshly updated Kalman position and slopes back into gCandidate 
			// so the next Runge-Kutta step propagates the corrected trajectory!
			
			// 1. Reconstruct the new local momentum vector from the updated slopes
			// We use the fact that p_x = ux * p_z and p_y = uy * p_z
			ptot_new = abs(gCandidate->GetCharge()) / x_state(4,0);

			// Use simple geometry to find the new local p_z component
			Double_t x_new_local = x_state(0,0);
			Double_t y_new_local = x_state(1,0);
			Double_t ux_new = x_state(2,0);
			Double_t uy_new = x_state(3,0);
			pz_new_local = ptot_new / TMath::Sqrt(1.0 + ux_new*ux_new + uy_new*uy_new);

			TVector3 p_new_local(ux_new * pz_new_local, uy_new * pz_new_local, pz_new_local);
			
			// 2. Rotate the local momentum vector BACK into the global lab frame
			TVector3 p_new_global;
			det->LocalToGlobalMomentum(p_new_global, p_new_local); 
			
			TVector3 pos_new_global;
			det->LocalToGlobal(pos_new_global,x_new_local,y_new_local);
			E_tot = TMath::Sqrt(p_new_global.Mag() * p_new_global.Mag() + mass * mass);
			beta  = p_new_global.Mag() / E_tot;
			gCandidate->SetPosition(pos_new_global);
			gCandidate->SetMomentum(-p_new_global);  // this is ok; slopes are preserved
			gCandidate->SetBeta(beta);
			
			if(i == 0)
			{
				pos_target = pos_new_global;
				mom_target = p_new_global;
				beta_target = beta;
			}
			
			if(writeout)
			{
				cout<<"Updated candidate position: "<<gCandidate->GetPosition().X()<<" "<<gCandidate->GetPosition().Y()<<" "<<gCandidate->GetPosition().Z()<<endl;
				cout<<"Updated candidate momentum: "<<gCandidate->GetMomentum().X()<<" "<<gCandidate->GetMomentum().Y()<<" "<<gCandidate->GetMomentum().Z()<<endl;				    
			}				
		}
	}

	gCandidate->SetStartPosition(pos_target);
	gCandidate->SetStartMomentum(mom_target);
	gCandidate->SetCharge(abs(gCandidate->GetCharge()));
	gCandidate->SetStartBeta(beta_target);
	gCandidate->SetChi2(chi2);
	gCandidate->Reset();
								
    cout<<"*** New fragment ****** "<<endl;
    cout<<"*** Charge: "<<gCandidate->GetCharge()<<endl;
    cout<<"*** Position: "<<gCandidate->GetStartPosition().X()<<", "<<gCandidate->GetStartPosition().Y()<<", "<<gCandidate->GetStartPosition().Z()<<endl;
    cout<<"*** y0geom: "<<ygeom<<endl;
	cout<<"*** Target true position: "<<x0true<<" "<<y0true<<endl;
    cout<<"*** Momentum: "<<gCandidate->GetStartMomentum().X()<<", "<<gCandidate->GetStartMomentum().Y()<<", "<<gCandidate->GetStartMomentum().Z()<<endl;
    cout<<"*** Beta: "<<gCandidate->GetStartBeta()<<endl;
    cout<<"*** Chi2: "<<chi2<<endl;
    cout<<"*********************** "<<endl;
    if(chi2 > 1.e6) status = 10;

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

    // LOG(info) << "1 Start beta:" << gCandidate->GetStartBeta()
    //<< ",  Beta:" << gCandidate->GetBeta();

    gCandidate->SetStartBeta(gCandidate->GetBeta());
    gCandidate->SetCharge(-1. * gCandidate->GetCharge());
    gCandidate->UpdateMomentum();

    // Double_t chi2 = Chi2Backward(variable);

    // LOG(info) << "2 chi2 = " << chi2;

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

    return 0;
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
        LOG(debug2) << "Fi 32 hit index " << particle->GetHitIndexByName("fi32") << " out of " << fi32->hits.size();
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

    // LOG(info) << "1 Start beta:" << gCandidate->GetStartBeta()
    //<< ",  Beta:" << gCandidate->GetBeta();

    gCandidate->SetStartBeta(gCandidate->GetBeta());
    gCandidate->SetCharge(-1. * gCandidate->GetCharge());
    gCandidate->UpdateMomentum();

    // Double_t chi2 = Chi2Backward(variable);

    // LOG(info) << "2 chi2 = " << chi2;

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

    return 0;
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
    /*
            Bool_t result = kFALSE;
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
                LOG(debug2);
                LOG(debug2) << "Propagating to z=" << det->pos0.Z();

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

                LOG(debug2) << "Track length " << particle->GetLength() << " cm";
            }

            sdev /= 2;

            chi2 /= (fDetectors->GetArray().size() - 2);

            LOG(debug2) << "Deviation at TOF wall : " << sdev;
            LOG(debug2) << "                 chi2 : " << chi2;
            LOG(debug2) << "                 time : " << time;
            LOG(debug2) << "Track length " << particle->GetLength() << " cm";

            return sdev;
    */
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
       candidate->GetHitIndexByName("target"))->GetTime()) / SPEED_OF_LIGHT; LOG(info) << "Velocity estimation (TOF) "
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
    LOG(debug1) << "beta1 = " << beta1;
    Double_t dev1 = TrackFragment(candidate, kTRUE, devTof, time, chi2);

    Double_t dbeta = 0.001;
    Double_t beta2 = beta1 + dbeta;
    LOG(debug1) << "beta2 = " << beta2;

    candidate->SetStartBeta(beta2);
    candidate->UpdateMomentum();
    candidate->Reset();

    Double_t dev2 = TrackFragment(candidate, kTRUE, devTof, time, chi2);

    Double_t dbetadx = dbeta / (dev2 - dev1);

    LOG(debug1) << "dbeta/dx = " << dbetadx << " 1/cm";

    return dbetadx;
}

Double_t R3BFragmentFitterChi2S494::DbetaChi2(R3BTrackingParticle* candidate)
{
    fPropagator->SetVis(kFALSE);

    Double_t devTof;
    Double_t time;
    Double_t chi2_1, chi2_2;
    Double_t beta1 = candidate->GetStartBeta();
    LOG(debug1) << "beta1 = " << beta1;
    TrackFragment(candidate, kTRUE, devTof, time, chi2_1);

    Double_t dbeta = 0.001;
    Double_t beta2 = beta1 + dbeta;
    LOG(debug1) << "beta2 = " << beta2;

    candidate->SetStartBeta(beta2);
    candidate->UpdateMomentum();
    candidate->Reset();

    TrackFragment(candidate, kTRUE, devTof, time, chi2_2);

    Double_t dbetachi2 = dbeta / (chi2_2 - chi2_1);

    LOG(debug1) << "dbeta/dchi2 = " << dbetachi2 << " 1/cm";

    return dbetachi2;
}

Double_t R3BFragmentFitterChi2S494::DbetaDt(R3BTrackingParticle* candidate)
{
    fPropagator->SetVis(kFALSE);

    Double_t devTof;
    Double_t time1, time2;
    Double_t chi2;
    Double_t beta1 = candidate->GetStartBeta();
    LOG(debug1) << "beta1 = " << beta1;
    TrackFragment(candidate, kTRUE, devTof, time1, chi2);

    Double_t dbeta = 0.001;
    Double_t beta2 = beta1 + dbeta;
    LOG(debug1) << "beta2 = " << beta2;

    candidate->SetStartBeta(beta2);
    candidate->UpdateMomentum();
    candidate->Reset();

    TrackFragment(candidate, kTRUE, devTof, time2, chi2);

    Double_t dbetadt = dbeta / (time2 - time1);

    LOG(debug1) << "dbeta/dt = " << dbetadt << " 1/ns";

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
    LOG(debug1) << "m1 = " << m1;
    Double_t dev1 = TrackFragment(candidate, energy_loss, devTof, time, chi2);

    Double_t dm = 0.01;
    Double_t m2 = m1 + dm;
    LOG(debug1) << "m2 = " << m2;

    candidate->SetMass(m2);
    candidate->UpdateMomentum();
    candidate->Reset();

    Double_t dev2 = TrackFragment(candidate, energy_loss, devTof, time, chi2);

    Double_t dmdx = dm / (dev2 - dev1);

    LOG(debug1) << "dm/dx = " << dmdx << " GeV/cm";

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
    LOG(debug1) << "m1 = " << m1;
    TrackFragment(candidate, energy_loss, devTof1, time, chi2);

    Double_t dm = 0.5;
    Double_t m2 = m1 + dm;
    LOG(debug1) << "m2 = " << m2;

    candidate->SetMass(m2);
    candidate->UpdateMomentum();
    candidate->Reset();

    TrackFragment(candidate, energy_loss, devTof2, time, chi2);

    Double_t dmdx = dm / (devTof2 - devTof1);

    LOG(debug1) << "dm/dx = " << dmdx << " GeV/cm";

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
    LOG(debug1) << "m1 = " << m1;
    TrackFragment(candidate, energy_loss, devTof, time1, chi2);

    Double_t dm = 0.5;
    Double_t m2 = m1 + dm;
    LOG(debug1) << "m2 = " << m2;

    candidate->SetMass(m2);
    candidate->UpdateMomentum();
    candidate->Reset();

    TrackFragment(candidate, energy_loss, devTof, time2, chi2);

    Double_t dmdt = dm / (time2 - time1);

    LOG(debug1) << "dm/dt = " << dmdt << " GeV/ns";

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
        LOG(debug1) << "Starting iteration " << (i + 1) << "   mass = " << mass << "   beta = " << beta
                   ;

        // Always start an iteration from the target
        candidate->Reset();

        //fPropagator->SetVis(kTRUE);

        // Calculate deviation of track from a measured TOF hit
        sdev = TrackFragment(candidate, kTRUE, devTof, time, chi2);

        LOG(debug1) << sdev << "  " << (time - tof->hit_time) << "  chi2=" << chi2;

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
