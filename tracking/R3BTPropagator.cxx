/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
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

    if (fVis)
    {
        fc4 = new TCanvas("c4", "", 900, 10, 600, 600);
        TH2F* h3 = new TH2F("h3", "", 180, -450., 450., 180, -100., 800.);
        h3->SetStats(kFALSE);
        h3->Draw();

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
    TVector3 norm = ((v2 - v1).Cross(v3 - v1)).Unit();

    // Check if particle is already on plane
    if (TMath::Abs((particle->GetPosition() - v1).Dot(norm)) < 1e-6)
    {
        return kTRUE;
    }

    TVector3 intersect;
    Bool_t crossed;
    Double_t step = 0.;
    Bool_t result;
    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), fPlane1[0], fNorm1, intersect);
    // LOG(info) << "crossed: " << crossed;
    if (crossed)
    {
        LOG(debug2) << "Starting upstream of magnetic field boundaries...";
        if ((v1 - particle->GetPosition()).Mag() < (fPlane1[0] - particle->GetPosition()).Mag())
        {
            LOG(debug2) << "Propagating to end-plane and stop.";
            crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
            step = (intersect - particle->GetPosition()).Mag();
            particle->SetPosition(intersect);
            particle->AddStep(step);
            return kTRUE;
        }
        LOG(debug2) << "Propagating to entrance of magnetic field.";
        if (fVis)
        {
            TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
            l1->Draw();
        }
        step = (intersect - particle->GetPosition()).Mag();
        particle->SetPosition(intersect);
        particle->AddStep(step);
        LOG(debug2) << intersect.X() << " " << intersect.Y() << " " << intersect.Z();
        // particle->GetPosition().Print();
        // particle->GetMomentum().Print();
    }

    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), fPlane2[0], fNorm2, intersect);
    if (crossed)
    {
        LOG(debug2) << "Propagating inside of field using RK4...";
        TVector3 tpos;
        if ((v1 - particle->GetPosition()).Mag() < (fPlane2[0] - particle->GetPosition()).Mag())
        {
            LOG(debug2) << "Propagating to end-plane using RK4 and stop.";
            tpos = particle->GetPosition();
            // particle->GetPosition().Print();
            // particle->GetMomentum().Print();
            // LOG(info) << particle->GetCharge();
            result = PropagateToPlaneRK(particle, v1, v2, v3);
            // particle->GetPosition().Print();
            // particle->GetMomentum().Print();
            if (fVis)
            {
                TLine* l1 = new TLine(-tpos.X(), tpos.Z(), -particle->GetX(), particle->GetZ());
                l1->Draw();
            }
            return result;
        }
        LOG(debug2) << "Propagating to exit from magnetic field.";
        tpos = particle->GetPosition();
        result = PropagateToPlaneRK(particle, fPlane2[0], fPlane2[1], fPlane2[2]);
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
    if (crossed)
    {
        LOG(debug2) << "Propagating to end plane. Finish.";
        if (fVis)
        {
            TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
            l1->Draw();
        }
        step = (intersect - particle->GetPosition()).Mag();
        particle->SetPosition(intersect);
        particle->AddStep(step);
        LOG(debug2);
        return kTRUE;
    }

    LOG(error) << "!!! Failed !!!";
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
        LOG(debug2) << "Starting downstream of magnetic field boundaries...";
        if ((v1 - particle->GetPosition()).Mag() < (fPlane2[0] - particle->GetPosition()).Mag())
        {
            LOG(debug2) << "Propagating to end-plane and stop.";
            crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
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
        LOG(debug2) << "Propagating to exit of magnetic field.";
        if (fVis)
        {
            TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
            l1->Draw();
        }
        step = (intersect - particle->GetPosition()).Mag();
        particle->SetPosition(intersect);
        particle->AddStep(step);
        LOG(debug2) << intersect.X() << " " << intersect.Y() << " " << intersect.Z();
    }

    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), fPlane1[0], -fNorm1, intersect);
    if (crossed)
    {
        LOG(debug2) << "Propagating inside of field using RK4...";
        TVector3 tpos;
        if ((v1 - particle->GetPosition()).Mag() < (fPlane1[0] - particle->GetPosition()).Mag())
        {
            LOG(debug2) << "Propagating to end-plane using RK4 and stop.";
            tpos = particle->GetPosition();
            result = PropagateToPlaneRK(particle, v1, v3, v2);
            if (fVis)
            {
                TLine* l1 = new TLine(-tpos.X(), tpos.Z(), -particle->GetX(), particle->GetZ());
                l1->Draw();
            }
            return result;
        }
        LOG(debug2) << "Propagating to entrance of magnetic field.";
        tpos = particle->GetPosition();
        result = PropagateToPlaneRK(particle, fPlane1[0], fPlane1[2], fPlane1[1]);
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
    if (crossed)
    {
        LOG(debug2) << "Propagating to end plane. Finish.";
        if (fVis)
        {
            TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
            l1->Draw();
        }
        step = (intersect - particle->GetPosition()).Mag();
        particle->SetPosition(intersect);
        particle->AddStep(step);
        LOG(debug2);
        return kTRUE;
    }

    LOG(error) << "!!! Failed !!!";
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

    particle->GetPosition(vecRKIn);
    particle->GetCosines(&vecRKIn[3]);

    TVector3 norm = ((v2 - v1).Cross(v3 - v1)).Unit();
    TVector3 intersect;

    Double_t step = (v1 - particle->GetPosition()).Mag() * 0.99;
    Double_t length = 0.;

    while (kTRUE)
    {
        length = fFairProp->OneStepRungeKutta(particle->GetCharge(), step, vecRKIn, vecOut);

        for (Int_t ii = 0; ii < 7; ii++)
        {
            vecRKIn[ii] = vecOut[ii];
        }

        particle->SetPosition(vecOut);
        particle->SetCosines(&vecOut[3]);
        particle->AddStep(length);

        nStep += 1;
        if (LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect))
        {
            for (Int_t ii = 0; ii < 7; ii++)
            {
                vecTemp[ii] = vecOut[ii];
            }
            step = (intersect - TVector3(vecTemp[0], vecTemp[1], vecTemp[2])).Mag() * 0.99;
            length = fFairProp->OneStepRungeKutta(particle->GetCharge(), step, vecTemp, vecOut);
            particle->SetPosition(vecOut);
            particle->SetCosines(&vecOut[3]);
            particle->AddStep(length);
            break;
        }

        step = (v1 - particle->GetPosition()).Mag();
        if (step < 1e-5)
        {
            break;
        }

        if (nStep > 1000)
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
