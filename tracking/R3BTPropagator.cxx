
#include "R3BTPropagator.h"
#include "R3BGladFieldMap.h"
#include "R3BTGeoPar.h"

#include "FairRKPropagator.h"
#include "FairLogger.h"

#include "TMath.h"
#include "TLine.h"
#include "TH2F.h"

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
        TH2F* h3 = new TH2F("h3", "", 200, -500., 500., 200, -200., 800.);
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

Bool_t R3BTPropagator::PropagateToDetector(const char* name,
                                           const TVector3& posin,
                                           const TVector3& momin,
                                           TVector3& posout,
                                           TVector3& momout)
{
    if (TString(name).EqualTo("mTOF"))
    {
        if (NULL == fmTofGeo)
        {
            return kFALSE;
        }

        TVector3 pos0(0., 0., -fmTofGeo->GetDimZ());
        TVector3 pos1(fmTofGeo->GetDimX(), fmTofGeo->GetDimY(), -fmTofGeo->GetDimZ());
        TVector3 pos2(-fmTofGeo->GetDimX(), fmTofGeo->GetDimY(), -fmTofGeo->GetDimZ());

        pos0.RotateY(fmTofGeo->GetRotY() * TMath::DegToRad());
        pos1.RotateY(fmTofGeo->GetRotY() * TMath::DegToRad());
        pos2.RotateY(fmTofGeo->GetRotY() * TMath::DegToRad());

        TVector3 trans(fmTofGeo->GetPosX(), fmTofGeo->GetPosY(), fmTofGeo->GetPosZ());

        pos0 += trans;
        pos1 += trans;
        pos2 += trans;

        if (fVis)
        {
            TLine* l11 = new TLine(-pos1.X(), pos1.Z(), -pos2.X(), pos2.Z());
            l11->SetLineWidth(3.);
            l11->SetLineColor(3);
            l11->Draw();
        }

        return PropagateToPlane(posin, momin, pos0, pos1, pos2, posout, momout);
    }
    return kFALSE;
}

Bool_t R3BTPropagator::PropagateToPlane(const TVector3& posin,
                                        const TVector3& momin,
                                        const TVector3& v1,
                                        const TVector3& v2,
                                        const TVector3& v3,
                                        TVector3& posout,
                                        TVector3& momout)
{
    TVector3 tpos = posin;
    TVector3 tmom = momin;

    TVector3 norm = ((v2 - v1).Cross(v3 - v1)).Unit();

    TVector3 intersect;
    Bool_t crossed;
    Bool_t result;
    crossed = LineIntersectPlane(posin, momin, fPlane1[0], fNorm1, intersect);
    if (crossed)
    {
        LOG(DEBUG2) << "Starting upstream of magnetic field boundaries..." << FairLogger::endl;
        if ((v1 - posin).Mag() < (fPlane1[0] - posin).Mag())
        {
            LOG(DEBUG2) << "Propagating to end-plane and stop." << FairLogger::endl;
            crossed = LineIntersectPlane(posin, momin, v1, norm, intersect);
            posout = intersect;
            momout = momin;
            return kTRUE;
        }
        LOG(DEBUG2) << "Propagating to entrance of magnetic field." << FairLogger::endl;
        tpos = intersect;
        if (fVis)
        {
            TLine* l1 = new TLine(-posin.X(), posin.Z(), -tpos.X(), tpos.Z());
            l1->Draw();
        }
    }

    crossed = LineIntersectPlane(tpos, tmom, fPlane2[0], fNorm2, intersect);
    if (crossed)
    {
        LOG(DEBUG2) << "Propagating inside of field using RK4..." << FairLogger::endl;
        TVector3 tpos1;
        TVector3 tmom1;
        if ((v1 - tpos).Mag() < (fPlane2[0] - tpos).Mag())
        {
            LOG(DEBUG2) << "Propagating to end-plane using RK4 and stop." << FairLogger::endl;
            result = PropagateToPlaneRK(tpos, tmom, v1, v2, v3, tpos1, tmom1);
            posout = tpos1;
            momout = tmom1;
            return result;
        }
        LOG(DEBUG2) << "Propagating to exit from magnetic field." << FairLogger::endl;
        result = PropagateToPlaneRK(tpos, tmom, fPlane2[0], fPlane2[1], fPlane2[2], tpos1, tmom1);
        if (fVis)
        {
            TLine* l1 = new TLine(-tpos.X(), tpos.Z(), -tpos1.X(), tpos1.Z());
            l1->Draw();
        }
        tpos = tpos1;
        tmom = tmom1;
        if (!result)
        {
            return result;
        }
    }

    crossed = LineIntersectPlane(tpos, tmom, v1, norm, intersect);
    if (crossed)
    {
        LOG(DEBUG2) << "Propagating to end plane. Finish." << FairLogger::endl;
        posout = intersect;
        momout = tmom;
        if (fVis)
        {
            TLine* l1 = new TLine(-tpos.X(), tpos.Z(), -posout.X(), posout.Z());
            l1->Draw();
        }
        LOG(DEBUG2) << FairLogger::endl << FairLogger::endl;
        return kTRUE;
    }

    LOG(ERROR) << "!!! Failed !!!" << FairLogger::endl;
    return kFALSE;
}

Bool_t R3BTPropagator::PropagateToPlaneRK(const TVector3& vin,
                                          const TVector3& momin,
                                          const TVector3& v1,
                                          const TVector3& v2,
                                          const TVector3& v3,
                                          TVector3& posout,
                                          TVector3& momout)
{
    Int_t nStep = 0;

    Double_t charge = 50.;
    Double_t vecRKIn[7];
    Double_t vecTemp[7];
    Double_t vecOut[7];
    Double_t p = momin.Mag();

    vecRKIn[0] = vin.X();
    vecRKIn[1] = vin.Y();
    vecRKIn[2] = vin.Z();
    vecRKIn[3] = momin.X() / p;
    vecRKIn[4] = momin.Y() / p;
    vecRKIn[5] = momin.Z() / p;
    vecRKIn[6] = p;

    TVector3 norm = ((v2 - v1).Cross(v3 - v1)).Unit();
    TVector3 intersect;

    posout = vin;

    Double_t step = (v1 - vin).Mag();

    while (kTRUE)
    {
        for (Int_t ii = 0; ii < 7; ii++)
        {
            vecTemp[ii] = vecRKIn[ii];
        }

        fFairProp->OneStepRungeKutta(charge, step, vecRKIn, vecOut);

        for (Int_t ii = 0; ii < 7; ii++)
        {
            vecRKIn[ii] = vecOut[ii];
        }

        posout.SetXYZ(vecOut[0], vecOut[1], vecOut[2]);
        p = vecOut[6];
        momout.SetXYZ(vecOut[3] * p, vecOut[4] * p, vecOut[5] * p);

        nStep += 1;
        if (!LineIntersectPlane(posout, momout, v1, norm, intersect))
        {
            step = (intersect - TVector3(vecTemp[0], vecTemp[1], vecTemp[2])).Mag();
            fFairProp->OneStepRungeKutta(charge, step, vecTemp, vecOut);
            posout.SetXYZ(vecOut[0], vecOut[1], vecOut[2]);
            p = vecOut[6];
            momout.SetXYZ(vecOut[3] * p, vecOut[4] * p, vecOut[5] * p);
            break;
        }

        if (nStep > 1000)
        {
            return kFALSE;
        }
    }

    return kTRUE;
}

Bool_t R3BTPropagator::PropagateToHitOnPlane(const TVector3& vin,
                                             const TVector3& momin,
                                             const TVector3& v1,
                                             const TVector3& v2,
                                             const TVector3& hit,
                                             TVector3& posout,
                                             TVector3& momout)
{
    Double_t charge = 50.;
    Double_t vecRKIn[7];
    Double_t vec1[3] = { v1.X(), v1.Y(), v1.Z() };
    Double_t vec2[3] = { v2.X(), v2.Y(), v2.Z() };
    Double_t vec3[3] = { hit.X(), hit.Y(), hit.Z() };
    Double_t vecOut[7];

    vecRKIn[0] = vin.X();
    vecRKIn[1] = vin.Y();
    vecRKIn[2] = vin.Z();
    vecRKIn[3] = momin.X() / momin.Mag();
    vecRKIn[4] = momin.Y() / momin.Mag();
    vecRKIn[5] = momin.Z() / momin.Mag();
    vecRKIn[6] = momin.Mag();

    fFairProp->PropagatToPlane(charge, vecRKIn, vec1, vec2, vec3, vecOut);

    posout.SetXYZ(vecOut[0], vecOut[1], vecOut[2]);
    momout.SetXYZ(vecOut[3] * vecOut[6], vecOut[4] * vecOut[6], vecOut[5] * vecOut[6]);

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
