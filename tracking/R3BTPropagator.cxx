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

#include "R3BTPropagator.h"
#include "R3BGladFieldMap.h"
#include "R3BTGeoPar.h"
#include "R3BTrackingDetector.h"
#include "R3BTrackingParticle.h"

#include "FairRunAna.h"

#include "FairLogger.h"
#include "FairRKPropagator.h"

#include "TH2F.h"
#include "TLine.h"
#include "TMath.h"

R3BTPropagator::R3BTPropagator(R3BGladFieldMap* field, Bool_t vis)
    : fFairProp(new FairRKPropagator(field))
    , fField(field)
    , fmTofGeo(NULL)
    , fVis(vis)
{
    // Define magnetic field boundaries ------------------------------------
    TVector3 pos(field->GetPositionX(), field->GetPositionY(), field->GetPositionZ());
    Double_t angle = field->GetYAngle() * TMath::DegToRad();
    fPlane1[0] = TVector3(0., 0., field->GetZmin());
    fPlane1[1] = TVector3(field->GetXmax(), field->GetYmax(), field->GetZmin());
    fPlane1[2] = TVector3(-field->GetXmax(), field->GetYmax(), field->GetZmin());
    fPlane2[0] = TVector3(0., 0., field->GetZmax());
    fPlane2[1] = TVector3(field->GetXmax(), field->GetYmax(), field->GetZmax());
    fPlane2[2] = TVector3(-field->GetXmax(), field->GetYmax(), field->GetZmax());
    for (Int_t i = 0; i < 3; i++)
    {
        fPlane1[i].RotateY(angle);
        fPlane1[i] += pos;
        fPlane2[i].RotateY(angle);
        fPlane2[i] += pos;
    }
    fNorm1 = ((fPlane1[1] - fPlane1[0]).Cross(fPlane1[2] - fPlane1[0])).Unit();
    fNorm2 = ((fPlane2[1] - fPlane2[0]).Cross(fPlane2[2] - fPlane2[0])).Unit();
    //----------------------------------------------------------------------
    fVis = kFALSE;
    fc4 = new TCanvas("c4", "", 50, 50, 800, 800);
    TH2F* h3 = new TH2F("h3", "", 180, -450., 450., 180, -100., 800.);
    h3->SetStats(kFALSE);
    h3->Draw();

    if (fVis)
    {

        TLine* l1 = new TLine(-fPlane1[1].X(), fPlane1[1].Z(), -fPlane1[2].X(), fPlane1[2].Z());
        l1->SetLineColor(2);
        l1->Draw();
        TLine* l2 = new TLine(-fPlane2[1].X(), fPlane2[1].Z(), -fPlane2[2].X(), fPlane2[2].Z());
        l2->SetLineColor(2);
        l2->Draw();
        TLine* l3 = new TLine(-fPlane1[1].X(), fPlane1[1].Z(), -fPlane2[1].X(), fPlane2[1].Z());
        l3->SetLineColor(2);
        l3->Draw();
        TLine* l4 = new TLine(-fPlane1[2].X(), fPlane1[2].Z(), -fPlane2[2].X(), fPlane2[2].Z());
        l4->SetLineColor(2);
        l4->Draw();
    }
}

R3BTPropagator::~R3BTPropagator() {}

Bool_t R3BTPropagator::PropagateToDetector(R3BTrackingParticle* particle, R3BTrackingDetector* detector)
{
    //   std::cout<<"In PropagateToDetector: "<<std::endl;
    //   detector->pos0.Print();
    //   detector->pos1.Print();
    //   detector->pos2.Print();

    detector->Draw();

    return PropagateToPlane(particle, detector->pos0, detector->pos1, detector->pos2);
}

Bool_t R3BTPropagator::PropagateToDetectorBackward(R3BTrackingParticle* particle, R3BTrackingDetector* detector)
{
    return PropagateToPlaneBackward(particle, detector->pos0, detector->pos1, detector->pos2);
}

Bool_t R3BTPropagator::PropagateToPlane(R3BTrackingParticle* particle,
                                        const TVector3& v1,
                                        const TVector3& v2,
                                        const TVector3& v3)
{
    TVector3 intersect;
    Bool_t crossed;
    Double_t step = 0.;
    Bool_t result = false;

    TVector3 norm = ((v2 - v1).Cross(v3 - v1)).Unit(); // normalized vector of vector product of (v2-1) and (v3-v1)
    // Check if particle is already on plane
    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
    TVector3 dist = (particle->GetPosition() - intersect);
    Double_t diff = dist.Mag();

    // if (TMath::Abs((particle->GetPosition() - v1).Dot(norm)) < 1e-6)
    if (diff < 1.e-6)
    {
        return kTRUE;
    }

    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), fPlane1[0], fNorm1, intersect);
    LOG(DEBUG2) << "crossed at glad entrance: " << crossed << " at:" << endl;
    // intersect.Print();
    if (crossed)
    {
        LOG(DEBUG2) << "Starting upstream of magnetic field boundaries...";
        if ((v1 - particle->GetPosition()).Mag() < (fPlane1[0] - particle->GetPosition()).Mag())
        {
            LOG(DEBUG2) << "Propagating to end-plane and stop.";
            crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
            step = (intersect - particle->GetPosition()).Mag();
            //  cout<<"Step 1: "<<step<<", intersect: "<<endl;
            intersect.Print();
            particle->SetPosition(intersect);
            particle->AddStep(step);
            return kTRUE;
        }

        LOG(DEBUG2) << "Propagating to entrance of magnetic field.";
        fVis = kFALSE;
        if (fVis)
        {
            //	cout<<"Line 1 to be plotted: xpart= "<<-particle->GetX()<<"; zpart= "<<particle->GetZ()<<", xinter=
            //"<<-intersect.X()<<
            //	", zinter= "<<intersect.Z()<<endl;
            TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
            l1->SetLineColor(1);
            l1->Draw("same");
        }

        step = (intersect - particle->GetPosition()).Mag();
        //   cout<<"Step 2: "<<step<<", intersect: "<<endl;
        // intersect.Print();

        particle->SetPosition(intersect);
        particle->AddStep(step);
        LOG(DEBUG2) << intersect.X() << " " << intersect.Y() << " " << intersect.Z();
        // particle->GetPosition().Print();
        // particle->GetMomentum().Print();
    }
    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), fPlane2[0], fNorm2, intersect);
    LOG(DEBUG2) << "crossed at the end of Bfeld: " << crossed << " at:";
    // intersect.Print();

    if (crossed)
    {
        LOG(DEBUG2) << "Propagating inside of field using RK4..." << endl;
        TVector3 tpos;
        // cout << "Test1: dist to det: " << (v1 - particle->GetPosition()).Mag() << ", dist to exit: " << (fPlane2[0] -
        // particle->GetPosition()).Mag() << ", xpart: "<<particle->GetX()<<", ypart: "<<particle->GetY()<<", zpart: "<<
        // particle->GetZ()<<", det v1/v2/v3:  "<<endl;
        // v1.Print();
        // v2.Print();
        // v3.Print();
        if ((v1 - particle->GetPosition()).Mag() < (fPlane2[0] - particle->GetPosition()).Mag())
        {
            // LOG(DEBUG2)
            //   cout << "Propagating to end-plane using RK4 and stop."<<endl;
            tpos = particle->GetPosition();
            // particle->GetPosition().Print();
            // particle->GetMomentum().Print();
            // LOG(INFO) << particle->GetCharge();
            result = PropagateToPlaneRK(particle, v1, v2, v3);
            // particle->GetPosition().Print();
            // particle->GetMomentum().Print();
            fVis = kFALSE;
            if (fVis)
            {
                TLine* l1 = new TLine(-tpos.X(), tpos.Z(), -particle->GetX(), particle->GetZ());
                l1->SetLineColor(1);
                l1->Draw();
            }
            return result;
        }

        LOG(DEBUG2) << "Propagating to exit from magnetic field.";
        tpos = particle->GetPosition();
        result = PropagateToPlaneRK(particle, fPlane2[0], fPlane2[1], fPlane2[2]);
        fVis = kFALSE; // kTRUE;
        if (fVis)
        {
            TLine* l1 = new TLine(-tpos.X(), tpos.Z(), -particle->GetX(), particle->GetZ());
            l1->Draw();
        }
        if (!result)
        {
            return result;
        }
    }
    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
    // cout << "crossed 3rd: " << crossed << " at:";
    // intersect.Print();
    if (crossed)
    {

        LOG(DEBUG2) << "Propagating to end plane. Finish." << endl;
        fVis = kFALSE;
        if (fVis)
        {
            TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
            l1->Draw();
        }
        step = (intersect - particle->GetPosition()).Mag();// * 0.1;
        // cout<<"Step 3: "<<step<<", interesct: "<<endl;
        // intersect.Print();
        particle->SetPosition(intersect);
        particle->AddStep(step);
        LOG(DEBUG2);
        return kTRUE;
    }
    // LOG(ERROR) << "!!! Failed !!!";
    return kFALSE;
}

Bool_t R3BTPropagator::PropagateToPlaneBackward(R3BTrackingParticle* particle,
                                                const TVector3& v1,
                                                const TVector3& v2,
                                                const TVector3& v3)
{
    TVector3 norm = ((v3 - v1).Cross(v2 - v1)).Unit();
	
    // Check if particle is already on plane
    if (TMath::Abs((particle->GetPosition() - v1).Dot(norm)) < 1e-6)
    {
        return kTRUE;
    }

    TVector3 intersect;
    Bool_t crossed;
    Double_t step = 0.;
    Bool_t result;
    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), fPlane2[0], -fNorm2, intersect);
    if (crossed)
    {
        LOG(DEBUG2) << "Starting downstream of magnetic field boundaries...";
        if ((v1 - particle->GetPosition()).Mag() < (fPlane2[0] - particle->GetPosition()).Mag())
        {
            LOG(DEBUG2) << "Propagating to end-plane and stop.";
            crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
            fVis = kFALSE;
            if (fVis)
            {
                TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
                l1->Draw();
            }
            step = (intersect - particle->GetPosition()).Mag();
            particle->SetPosition(intersect);
            particle->AddStep(step);
            return kTRUE;
        }
        LOG(DEBUG2) << "Propagating to exit of magnetic field.";
        fVis = kFALSE;
        if (fVis)
        {
            TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
            l1->Draw();
        }
        step = (intersect - particle->GetPosition()).Mag();
        particle->SetPosition(intersect);
        particle->AddStep(step);
        LOG(DEBUG2) << intersect.X() << " " << intersect.Y() << " " << intersect.Z();
    }

    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), fPlane1[0], -fNorm1, intersect);
    if (crossed)
    {
        LOG(DEBUG2) << "Propagating inside of field using RK4...";
        TVector3 tpos;
        if ((v1 - particle->GetPosition()).Mag() < (fPlane1[0] - particle->GetPosition()).Mag())
        {
            LOG(DEBUG2) << "Propagating to end-plane using RK4 and stop.";
            tpos = particle->GetPosition();
            result = PropagateToPlaneRK(particle, v1, v3, v2);
            fVis = kFALSE;
            if (fVis)
            {
                TLine* l1 = new TLine(-tpos.X(), tpos.Z(), -particle->GetX(), particle->GetZ());
                l1->Draw();
            }
            LOG(DEBUG2) << particle->GetX() << ", " << particle->GetY() << ", " << particle->GetZ();
            return result;
        }
        LOG(DEBUG2) << "Propagating to entrance of magnetic field.";
        tpos = particle->GetPosition();
        result = PropagateToPlaneRK(particle, fPlane1[0], fPlane1[2], fPlane1[1]);
        fVis = kFALSE;
        if (fVis)
        {
            TLine* l1 = new TLine(-tpos.X(), tpos.Z(), -particle->GetX(), particle->GetZ());
            l1->Draw();
        }
        LOG(DEBUG2) << particle->GetX() << ", " << particle->GetY() << ", " << particle->GetZ();
        if (!result)
        {
            return result;
        }
    }

    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
    if (crossed)
    {
        LOG(DEBUG2) << "Propagating to end plane. Finish.";
        fVis = kFALSE;
        if (fVis)
        {
            TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
            l1->Draw();
        }
        step = (intersect - particle->GetPosition()).Mag();
        particle->SetPosition(intersect);
        particle->AddStep(step);
        LOG(DEBUG2);
        return kTRUE;
    }

    // LOG(ERROR) << "!!! Failed !!!";
    return kFALSE;
}

Bool_t R3BTPropagator::PropagateToPlaneRK(R3BTrackingParticle* particle,
                                          const TVector3& v1,
                                          const TVector3& v2,
                                          const TVector3& v3)
{
    Int_t nStep = 0;

    Double_t vecRKIn[7];
    Double_t vecTemp[7];
    Double_t vecOut[7];
    TVector3 dist;
    Double_t diff;
    TVector3 intersect;
    Bool_t crossed;
    Double_t distance;

    particle->GetPosition(vecRKIn);
    particle->GetCosines(&vecRKIn[3]);

    TVector3 norm = ((v2 - v1).Cross(v3 - v1)).Unit();

    // dist = particle->GetPosition() - v1;
    // diff = dist.Dot(norm);
    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
    dist = (particle->GetPosition() - intersect);
    diff = dist.Mag();
    // cout << "diff: " << diff << endl;

    // Double_t step = TMath::Abs(diff) / 400.;// / 100.;
    Double_t step = 0.1;
    Double_t length = 0.;
    Double_t res = 10000.;
    Double_t res_old = 10000.;

    // cout << "Step: " << step << endl;

    while (kTRUE)
    {
        length = fFairProp->OneStepRungeKutta(particle->GetCharge(), step, vecRKIn, vecOut);
        // length = length / 100.;
        // cout << "Length: " << length << endl;

        for (Int_t ii = 0; ii < 7; ii++)
        {
            vecRKIn[ii] = vecOut[ii];
        }

        particle->SetPosition(vecOut);
        particle->SetCosines(&vecOut[3]);
        particle->AddStep(length);

        nStep += 1;

        //   dist = particle->GetPosition() - v1;
        //   distance = (TVector3(dist.X() * norm.X(), dist.Y() * norm.Y(), dist.Z() * norm.Z())).Mag();

        crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
        dist = (particle->GetPosition() - intersect);
        distance = dist.Mag();

        res = TMath::Abs(distance); // / diff);

        if (res < 0.1 || res > res_old)
        {

            // cout << "break############" << endl;
            // cout << "Res: " << res << "  " << res_old << "  " << nStep << "  " << distance << endl;
            // cout<<"Intersection point: "<<intersect.X()<<", "<<intersect.Y()<<", "<<intersect.Z()<<endl;
            
            break;
        }
        else
        {
            res_old = res;
            step = 1.;
            
			TVector3 pos = particle->GetPosition();
			Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(pos.X(),pos.Y(),pos.Z());
			
			step = 20. / TMath::Abs(field); //10.

			//cout << "x: " << pos.X() << " y: " << pos.Y() << " z: " << pos.Z() << 
			//"Field:" << field << " step: " << 1./ TMath::Abs(field) << endl;
			
			if (step > TMath::Abs(distance))
			{
				step = TMath::Abs(distance)/2.;
				
			}
			if (step > 50.) //20
			{
				step = 50.; //20
			}
			if (step < 0.5) //0.1
			{
				step = 0.5; //0.1
			}

        }

        if (nStep > 100000)
        {
            return kFALSE;
        }
    }

    return kTRUE;
}

Bool_t R3BTPropagator::LineIntersectPlane(const TVector3& pos,
                                          const TVector3& mom,
                                          const TVector3& v1,
                                          const TVector3& normal,
                                          TVector3& intersect)
{
    TVector3 delta = v1 - pos;
    Double_t pn = mom.Dot(normal);
    if (pn == 0)
    {
        return kFALSE;
    }
    Double_t t = delta.Dot(normal) / pn;
    if (t < 0)
    {
        return kFALSE;
    }
    else
    {
        intersect = pos + mom * t;
    }
    return kTRUE;
}

ClassImp(R3BTPropagator)
