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
#include "FairLogger.h"
#include "FairRKPropagator.h"
#include "FairRunAna.h"
#include "R3BGladFieldMap.h"
#include "R3BTGeoPar.h"
#include "R3BTrackingDetector.h"
#include "R3BTrackingParticle.h"
#include "TH2F.h"
#include "TLine.h"
#include "TMath.h"
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

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

    // fVis = kTRUE;
    if (fVis)
    {
        fc4 = new TCanvas("c4", "", 50, 50, 800, 800);
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
    // std::cout<<"In PropagateToDetector: "<<detector->GetDetectorName()<<std::endl;
    // detector->pos0.Print();
    // detector->pos1.Print();
    // detector->pos2.Print();

    if (fVis)
    {
        detector->Draw();
    }

    return PropagateToPlane(particle, detector->pos0, detector->pos1, detector->pos2);
}

Bool_t R3BTPropagator::PropagateToDetectorBackward(R3BTrackingParticle* particle, R3BTrackingDetector* detector, 
                                                TMatrixD& P_cov)
{
    if (fVis)
        detector->Draw();
    return PropagateToPlaneBackward(particle, detector->pos0, detector->pos1, detector->pos2, P_cov);
}
Bool_t R3BTPropagator::PropagateToDetectorForward(R3BTrackingParticle* particle, R3BTrackingDetector* detector, 
                                                TMatrixD& P_cov)
{
    if (fVis)
        detector->Draw();
    return PropagateToPlaneForward(particle, detector->pos0, detector->pos1, detector->pos2, P_cov);
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

    LOG(debug2) << "Entered PropagateToPlane!" << endl;
    LOG(debug2) << "Magnetic field: " << endl;
    //  fPlane1[0].Print();
    //  fPlane2[1].Print();
    //  fPlane2[0].Print();

    TVector3 norm = ((v2 - v1).Cross(v3 - v1)).Unit(); // normalized vector of vector product of (v2-1) and (v3-v1)
    // Check if particle is already on plane
    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
    TVector3 dist = (particle->GetPosition() - intersect);
    Double_t diff = dist.Mag();

    // if (TMath::Abs((particle->GetPosition() - v1).Dot(norm)) < 1e-6)
    if (TMath::Abs(diff) < 1.e-6)
    {
        LOG(debug2) << "Already at plane!" << endl;
        return kTRUE;
    }

    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), fPlane1[0], fNorm1, intersect);

    if (crossed)
    {
        LOG(debug2) << "crossed at glad entrance: " << crossed << " at:" << endl;
        //  intersect.Print();
        //   particle->GetPosition().Print();
        //   particle->GetMomentum().Print();

        LOG(debug2) << "Starting upstream of magnetic field boundaries...";
        if ((v1 - particle->GetPosition()).Mag() < (fPlane1[0] - particle->GetPosition()).Mag())
        {
            LOG(debug2) << "Propagating to end-plane and stop.";
            crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
            step = (intersect - particle->GetPosition()).Mag();
            //   cout<<"Step 1: "<<step<<", intersect & particle: "<<endl;
            // intersect.Print();
            // particle->GetPosition().Print();
            // particle->GetMomentum().Print();

            particle->SetPosition(intersect);
            particle->AddStep(step);
            return kTRUE;
        }

        LOG(debug2) << "Propagating to entrance of magnetic field.";

        // fVis = kTRUE;
        if (fVis)
        {
            //	cout<<"Line 1 to be plotted: xpart= "<<-particle->GetX()<<"; zpart= "<<particle->GetZ()<<", xinter=
            //"<<-intersect.X()<<
            //	", zinter= "<<intersect.Z()<<endl;
            TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
            l1->SetLineColor(1);
            l1->Draw();
        }

        step = (intersect - particle->GetPosition()).Mag();
        // cout<<"Step 2: "<<step<<", intersect: "<<endl;
        // intersect.Print();
        // particle->GetPosition().Print();
        // particle->GetMomentum().Print();

        particle->SetPosition(intersect);
        particle->AddStep(step);
        LOG(debug2) << intersect.X() << " " << intersect.Y() << " " << intersect.Z();
        // particle->GetPosition().Print();
        // particle->GetMomentum().Print();
    }
    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), fPlane2[0], fNorm2, intersect);

    if (crossed)
    {
        LOG(debug2) << "will cross at the end of Bfeld: " << crossed << " at:";
        //	intersect.Print();
        //	particle->GetPosition().Print();
        //    particle->GetMomentum().Print();

        LOG(debug2) << "####### Propagating inside of field using RK4..." << endl;
        TVector3 tpos;
        // cout << "Test1: dist to det: " << (v1 - particle->GetPosition()).Mag() << ", dist to exit: " << (fPlane2[0] -
        // particle->GetPosition()).Mag() << ", xpart: "<<particle->GetX()<<", ypart: "<<particle->GetY()<<", zpart: "<<
        // particle->GetZ()<<endl;
        //  v1.Print();
        //  v2.Print();
        //   v3.Print();
        if ((v1 - particle->GetPosition()).Mag() < (fPlane2[0] - particle->GetPosition()).Mag())
        {
            LOG(debug2) << "Propagating to end-plane using RK4 and stop. Particle before: " << endl;
            tpos = particle->GetPosition();
            // particle->GetPosition().Print();
            // particle->GetMomentum().Print();
            // LOG(info) << particle->GetCharge();
            result = PropagateToPlaneRK(particle, v1, v2, v3);
            //  cout<<"particle after propagaging: "<<endl;
            // particle->GetPosition().Print();
            // particle->GetMomentum().Print();

            //  fVis = kTRUE;

            if (fVis)
            {
                TLine* l1 = new TLine(-tpos.X(), tpos.Z(), -particle->GetX(), particle->GetZ());
                l1->SetLineColor(1);
                l1->Draw();
            }
            // cout<<"Result: "<<result<<endl;
            return result;
        }

        LOG(debug2) << "Propagating to exit from magnetic field.";
        tpos = particle->GetPosition();
        result = PropagateToPlaneRK(particle, fPlane2[0], fPlane2[1], fPlane2[2]);

        //  fVis = true; // kTRUE;
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

        LOG(debug2) << "Propagating to end plane. Finish." << endl;

        //  fVis = kTRUE;
        if (fVis)
        {
            TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
            l1->Draw();
        }

        step = (intersect - particle->GetPosition()).Mag(); // * 0.1;

        // cout<<"Step 3: "<<step<<", interesct: "<<endl;
        // intersect.Print();
        particle->SetPosition(intersect);
        particle->AddStep(step);
        LOG(debug2);
        return kTRUE;
    }
    // LOG(error) << "!!! Failed !!!";
    return kFALSE;
}


Bool_t R3BTPropagator::PropagateToPlaneForward(R3BTrackingParticle* particle,
                                        const TVector3& v1,
                                        const TVector3& v2,
                                        const TVector3& v3, 
                                        TMatrixD& P_cov)
{
// Propagation forward in a setup: field start -> target -> fibers -> field end -> tofd
    TVector3 pos_start = particle->GetPosition();
    TVector3 intersect;
    TVector3 pos_end;
    Double_t step = 0.;
    Double_t delta_z = 0.;
    Bool_t crossed;
    Bool_t result = false;
    Bool_t writeout = false;
    
    TMatrixD F_linear(5, 5);
    TMatrixD F_accumulator(5, 5);
	F_accumulator.UnitMatrix();
    
    
    if(writeout) cout << "**** Entered PropagateToPlaneForward ******" << endl;
    
    TVector3 norm = ((v2 - v1).Cross(v3 - v1)).Unit(); // normalized vector of vector product of (v2-1) and (v3-v1)
    if(writeout) cout<<"norm vector: "<<norm.X()<<" "<<norm.Y()<<" "<<norm.Z()<<endl;
    if(writeout) cout<<"v1 vector: "<<v1.X()<<" "<<v1.Y()<<" "<<v1.Z()<<endl;
    
    // Check if particle is already on plane
    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
    TVector3 dist = (particle->GetPosition() - intersect);
    Double_t diff = dist.Mag();
    if(writeout) cout<<"Distance to plane: "<<diff<<endl;
    
    // if (TMath::Abs((particle->GetPosition() - v1).Dot(norm)) < 1e-6)
    if (TMath::Abs(diff) < 1.e-6)
    {
        LOG(debug2) << "Already at plane!" << endl;
        TMatrixD F_unit;
        F_unit.UnitMatrix();
        P_cov = F_unit * P_cov;
        return kTRUE;
    }
		
    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), fPlane1[0], fNorm1, intersect);
    if(writeout) cout<<"Will field entrance be crossed? (this should not happen!)"<<endl;
    if(writeout) cout<<"crossed= "<<crossed<<endl;
    if(writeout) cout<<"Plane: "<<fPlane1[0].X()<<" "<<fPlane1[0].Y()<<" "<<fPlane1[0].Z()<<endl;
    if(writeout) cout<<"Planes norm: "<<fNorm1.X()<<" "<<fNorm1.Y()<<" "<<fNorm1.Z()<<endl;
    if(writeout) cout<<"Particle position: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
    if(writeout) cout<<"Particle momentum: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
    if(writeout) cout<<"Particle charge: "<<particle->GetCharge()<<endl;
    if(writeout) cout<<"Intersect point: "<<intersect.X()<<" "<<intersect.Y()<<" "<<intersect.Z()<<endl;
    
    // Here crossed == 0, as field map starts before the target, fPlane1[0].Z()~ -116cm; if crossed == 1, something is wrong.
    if (crossed)
    {
        if(writeout) cout<<"In front of glad - should not happen!: "<<particle->GetPosition().Z()<<", "<<particle->GetMomentum().Z()<<endl;
          
        if ((v1 - particle->GetPosition()).Mag() < (fPlane1[0] - particle->GetPosition()).Mag())
        {
            if(writeout) cout << "Propagating to detector plane in front of field - should not happen!! ";
            crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
            step = (intersect - particle->GetPosition()).Mag();
            particle->SetPosition(intersect);
            particle->AddStep(step);
            if(writeout) cout<<"After step= "<<step<<endl;
			if(writeout) cout<<"Particle position: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
			if(writeout) cout<<"Particle momentum: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
			if(writeout) cout<<"Particle charge: "<<particle->GetCharge()<<endl;
    
            
            // Linear Covariance Update for the step to detector in front of the magnet
            pos_end = particle->GetPosition();
			TVector3 pos_diff = pos_end - pos_start;
			TVector3 pos_def_local = TransformGlobalToLocalPlane(pos_diff, v1, v2, v3); 
			F_linear.UnitMatrix();
			F_linear(0, 2) = pos_def_local.Z();
			F_linear(1, 3) = pos_def_local.Z();
        
			P_cov = F_linear * P_cov * TMatrixD(TMatrixD::kTransposed, F_linear);
           
            return kTRUE;
        }

        if(writeout)  cout << "Propagating from detector to entrance of magnetic field. Should not happen!" <<endl;

        // fVis = kTRUE;
        if (fVis)
        {
            TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
            l1->SetLineColor(1);
            l1->Draw();
        }

        step = (intersect - particle->GetPosition()).Mag();
        particle->SetPosition(intersect);
        particle->AddStep(step);
		if(writeout) cout<<"After step= "<<step<<endl;
		if(writeout) cout<<"Particle position: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
		if(writeout) cout<<"Particle momentum: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
		if(writeout) cout<<"Particle charge: "<<particle->GetCharge()<<endl;
    
        // Linear Covariance Update for the step from detector to entrance
        pos_end = particle->GetPosition();
		TVector3 pos_diff = pos_end - pos_start;
		TVector3 pos_def_local = TransformGlobalToLocalPlane(pos_diff, v1, v2, v3);
        F_linear.UnitMatrix();
        F_linear(0, 2) = pos_def_local.Z();
        F_linear(1, 3) = pos_def_local.Z(); 
               
	    P_cov = F_linear * P_cov * TMatrixD(TMatrixD::kTransposed, F_linear); 
            
    }
    
    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), fPlane2[0], fNorm2, intersect);
	if(writeout) cout<<"Check the field exit plane"<<endl;
    if(writeout) cout<<"crossed= "<<crossed<<endl;
    if(writeout) cout<<"Plane: "<<fPlane2[0].X()<<" "<<fPlane2[0].Y()<<" "<<fPlane2[0].Z()<<endl;
    if(writeout) cout<<"Planes norm: "<<fNorm2.X()<<" "<<fNorm2.Y()<<" "<<fNorm2.Z()<<endl;
    if(writeout) cout<<"Particle position: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
    if(writeout) cout<<"Particle momentum: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
    if(writeout) cout<<"Particle charge: "<<particle->GetCharge()<<endl;
    if(writeout) cout<<"Intersect point: "<<intersect.X()<<" "<<intersect.Y()<<" "<<intersect.Z()<<endl;
  
    if (crossed)
    {
        if(writeout) cout<<"Particle going toward the exit plane of field: " << endl;
		
        TVector3 tpos;

        if ((v1 - particle->GetPosition()).Mag() < (fPlane2[0] - particle->GetPosition()).Mag())
        {
            if(writeout) cout<< "Propagating to detector-plane using RK4. " << endl;
            tpos = particle->GetPosition();
        
            result = PropagateToPlaneRKForward(particle, v1, v2, v3, F_accumulator);
        
    	    P_cov = F_accumulator * P_cov * TMatrixD(TMatrixD::kTransposed, F_accumulator);
	    
            if(writeout) cout<<"Particle momentum after RK: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
			if(writeout) cout<<"Particle momentum after RK: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
			if(writeout) cout<<"Particle charge after Rk: "<<particle->GetCharge()<<endl;
			
            //  fVis = kTRUE;

            if (fVis)
            {
                TLine* l1 = new TLine(-tpos.X(), tpos.Z(), -particle->GetX(), particle->GetZ());
                l1->SetLineColor(1);
                l1->Draw();
            }
           
            return result;
        }

        if(writeout) cout<<"Propagating to the exit of magnetic field."<<endl;
        tpos = particle->GetPosition();
        
        result = PropagateToPlaneRKForward(particle, fPlane2[0], fPlane2[1], fPlane2[2], F_accumulator);
        
	    P_cov = F_accumulator * P_cov * TMatrixD(TMatrixD::kTransposed, F_accumulator);
	    
		if(writeout) cout<<"Particle position after RK: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
		if(writeout) cout<<"Particle momentum after RK: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
		if(writeout) cout<<"Particle charge after RK: "<<particle->GetCharge()<<endl;

        //  fVis = true; // kTRUE;
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
	if(writeout) cout<<"Check behind the field"<<endl;
    if(writeout) cout<<"crossed= "<<crossed<<endl;
    if(writeout) cout<<"Plane: "<<v1.X()<<" "<<v1.Y()<<" "<<v1.Z()<<endl;
    if(writeout) cout<<"Planes norm: "<<norm.X()<<" "<<norm.Y()<<" "<<norm.Z()<<endl;
    if(writeout) cout<<"Particle position: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
    if(writeout) cout<<"Particle momentum: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
    if(writeout) cout<<"Particle charge: "<<particle->GetCharge()<<endl;
    if(writeout) cout<<"Intersect point: "<<intersect.X()<<" "<<intersect.Y()<<" "<<intersect.Z()<<endl;
   
   
    if (crossed)
    {

        if(writeout) cout<< "Propagating from magnet exit to the last detector" << endl;

        //  fVis = kTRUE;
        if (fVis)
        {
            TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
            l1->Draw();
        }

        step = (intersect - particle->GetPosition()).Mag(); // * 0.1;

        particle->SetPosition(intersect);
        particle->AddStep(step);
		if(writeout) cout<<"Particle position: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
		if(writeout) cout<<"Particle momentum at the end: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
		if(writeout) cout<<"Particle charge at the end: "<<particle->GetCharge()<<endl;
        
    
        pos_end = particle->GetPosition();
		TVector3 pos_diff = pos_end - pos_start;
		TVector3 pos_def_local = TransformGlobalToLocalPlane(pos_diff, v1, v2, v3);
        F_linear.UnitMatrix();
        F_linear(0, 2) = pos_def_local.Z();
        F_linear(1, 3) = pos_def_local.Z();
        
	    P_cov = F_linear * P_cov * TMatrixD(TMatrixD::kTransposed, F_linear);
        
        return kTRUE;
    }
    // LOG(error) << "!!! Failed !!!";
    return kFALSE;
}

Bool_t R3BTPropagator::PropagateToPlaneBackward(R3BTrackingParticle* particle,
                                                const TVector3& v1,
                                                const TVector3& v2,
                                                const TVector3& v3,
                                                TMatrixD& P_cov)
{
// Backward propagation for setup: tofd -> exit glad -> fi32/33 -> fi30/31 -> fi23b -> fi23a -> target 
	
    // Define the normal to the detector plane (oriented backward toward the target)
    TVector3 norm = ((v2 - v1).Cross(v3 - v1)).Unit(); 
    
    // Check if the particle is already on the target plane to avoid division by zero
    if (TMath::Abs((particle->GetPosition() - v1).Dot(norm)) < 1e-6)
    {
        TMatrixD F_unit;
        F_unit.UnitMatrix();
        P_cov = F_unit * P_cov;
        return kTRUE;
    }

    TVector3 pos_start = particle->GetPosition();
    TVector3 intersect;
    TVector3 pos_end;
    Double_t step = 0.;
    Double_t delta_z = 0.;
    Bool_t crossed = kFALSE;
    Bool_t result = kFALSE;
    
    TMatrixD F_linear(5, 5);
    TMatrixD F_accumulator(5, 5);
	F_accumulator.UnitMatrix();
    
    Bool_t writeout = false;
    //if(abs(particle->GetCharge()) == 2) writeout = true;
    
    if(writeout) cout << "**** Entered PropagateToPlaneBackward ******" << endl;
    if(writeout) cout<<"norm vector: "<<norm.X()<<" "<<norm.Y()<<" "<<norm.Z()<<endl;
    if(writeout) cout<<"v1 vector: "<<v1.X()<<" "<<v1.Y()<<" "<<v1.Z()<<endl;

    // =====================================================================
    // STEP 1: MAGNET EXIT BOUNDARY CHECK (fPlane2)
    // =====================================================================
    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), fPlane2[0], -fNorm2, intersect);
    
    if(writeout) cout<<"Will field exit be crossed? "<<endl;
    if(writeout) cout<<"crossed= "<<crossed<<endl;
    if(writeout) cout<<"Plane: "<<fPlane2[0].X()<<" "<<fPlane2[0].Y()<<" "<<fPlane2[0].Z()<<endl;
    if(writeout) cout<<"Planes norm: "<<-fNorm2.X()<<" "<<-fNorm2.Y()<<" "<<-fNorm2.Z()<<endl;
    if(writeout) cout<<"Particle position: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
    if(writeout) cout<<"Particle momentum: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
    if(writeout) cout<<"Particle charge: "<<particle->GetCharge()<<endl;
    if(writeout) cout<<"Intersect point: "<<intersect.X()<<" "<<intersect.Y()<<" "<<intersect.Z()<<endl;
   
    if (crossed)
    {
         if(writeout) cout<< "Particle going upstream to field entrance."<<endl;
        
        // If the destination detector is upstream of the magnet exit, propagate linearly to the exit first
        if ((v1 - particle->GetPosition()).Mag() > (fPlane2[0] - particle->GetPosition()).Mag())
        {
            if(writeout) cout<< "Propagating upstream to field : "<<endl;
			crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), fPlane2[0], -fNorm2, intersect);
			if(writeout) cout<<"crossed= "<<crossed<<endl;
			if(writeout) cout<<"Plane: "<<fPlane2[0].X()<<" "<<fPlane2[0].Y()<<" "<<fPlane2[0].Z()<<endl;
			if(writeout) cout<<"Planes norm: "<<-fNorm2.X()<<" "<<-fNorm2.Y()<<" "<<-fNorm2.Z()<<endl;
			if(writeout) cout<<"Particle position: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
			if(writeout) cout<<"Particle momentum: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
			if(writeout) cout<<"Particle charge: "<<particle->GetCharge()<<endl;
			if(writeout) cout<<"Intersect point: "<<intersect.X()<<" "<<intersect.Y()<<" "<<intersect.Z()<<endl;
			
			
			if (fVis)
			{
				TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
				l1->Draw();
			}
			
			pos_start = particle->GetPosition();
			step = (intersect - particle->GetPosition()).Mag();
			particle->SetPosition(intersect);
			particle->AddStep(step);
			if(writeout) cout<<"After step= "<<step<< " particle reached field"<< endl;
			if(writeout) cout<<"Particle position: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
			if(writeout) cout<<"Particle momentum: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
			if(writeout) cout<<"Particle charge: "<<particle->GetCharge()<<endl;  
				 
			// Linear Covariance Update for the step to the magnet boundary
			pos_end = particle->GetPosition();
			TVector3 pos_diff = pos_end - pos_start;
			TVector3 pos_def_local = TransformGlobalToLocalPlane(pos_diff, v1, v2, v3);
			F_linear.UnitMatrix();
			F_linear(0, 2) = pos_def_local.Z();
			F_linear(1, 3) = pos_def_local.Z();
		    P_cov = F_linear * P_cov * TMatrixD(TMatrixD::kTransposed, F_linear);
			if(writeout) cout<<"P_cov(0,0): "<<P_cov(0,0)<<endl;
		}
	}
    /*    
        if(writeout) cout << "Propagating  upstream  to the exit of magnetic field."<<endl;
		if (fVis)
		{
			TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
			l1->Draw();
		}
		
		step = (intersect - particle->GetPosition()).Mag();
		particle->SetPosition(intersect);
		particle->AddStep(step);
		if(writeout) cout<<"After step= "<<step<<endl;
		if(writeout) cout<<"Particle position: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
		if(writeout) cout<<"Particle momentum: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
		if(writeout) cout<<"Particle charge: "<<particle->GetCharge()<<endl;           
	   
		// Linear Covariance Update for the step to the magnet boundary
		pos_end = particle->GetPosition();
		TVector3 pos_diff = pos_end - pos_start;
        TVector3 pos_def_local = TransformGlobalToLocalPlane(pos_diff, v1, v2, v3);
		F_linear.UnitMatrix();
		F_linear(0, 2) = pos_def_local.Z();
		F_linear(1, 3) = pos_def_local.Z();
		P_cov = F_linear * P_cov * TMatrixD(TMatrixD::kTransposed, F_linear);     
		
		if(writeout) cout<<"P_cov(0,0): "<<P_cov(0,0)<<endl;
		
        return kTRUE;        
    */

    // =====================================================================
    // MAGNETIC FIELD INTERIOR (Runge-Kutta Engine)
    // =====================================================================
    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), fPlane1[0], -fNorm1, intersect);
    
    if(writeout) cout<<"Is particle going upstream toward the entrance of the field? "<<endl;
    if(writeout) cout<<"crossed= "<<crossed<<endl;
    if(writeout) cout<<"Plane: "<<fPlane1[0].X()<<" "<<fPlane1[0].Y()<<" "<<fPlane1[0].Z()<<endl;
    if(writeout) cout<<"Planes norm: "<<-fNorm1.X()<<" "<<-fNorm1.Y()<<" "<<-fNorm1.Z()<<endl;
    if(writeout) cout<<"Particle position: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
    if(writeout) cout<<"Particle momentum: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
    if(writeout) cout<<"Particle charge: "<<particle->GetCharge()<<endl;
    if(writeout) cout<<"Intersect point: "<<intersect.X()<<" "<<intersect.Y()<<" "<<intersect.Z()<<endl;
       
    if (crossed)
    {
        if(writeout) cout << "Propagating inside of field using RK4..."<<endl;
        TVector3 tpos = particle->GetPosition();
        
        // If the detector plane resides inside the magnet volume
        if ((v1 - particle->GetPosition()).Mag() < (fPlane1[0] - particle->GetPosition()).Mag())
        {
            if(writeout) cout << "Propagating to detector-plane using RK4 .";
            result = PropagateToPlaneRKBackward(particle, v1, v2, v3, F_accumulator);
            
		    P_cov = F_accumulator * P_cov * TMatrixD(TMatrixD::kTransposed, F_accumulator);
		    
			if(writeout) cout<<"Particle position after RK: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
			if(writeout) cout<<"Particle momentum after RK: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
			if(writeout) cout<<"Particle charge after RK: "<<particle->GetCharge()<<endl;
			if(writeout) cout<<"P_cov(0,0): "<<P_cov(0,0)<<endl;
            
            if (fVis)
            {
                TLine* l1 = new TLine(-tpos.X(), tpos.Z(), -particle->GetX(), particle->GetZ());
                l1->Draw();
            }
            

            return result;
        }
        
        // Otherwise, propagate all the way through to the field entrance plane, this never happens, as field goes beyond target.
        if(writeout) cout << "Propagating to entrance of magnetic field. This should not happen!!!";
        result = PropagateToPlaneRKBackward(particle, fPlane1[0], fPlane1[1], fPlane1[2], F_accumulator);
        
		P_cov = F_accumulator * P_cov * TMatrixD(TMatrixD::kTransposed, F_accumulator);
		    
		if(writeout) cout<<"Particle position after RK: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
		if(writeout) cout<<"Particle momentum after RK: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
		if(writeout) cout<<"Particle charge after RK: "<<particle->GetCharge()<<endl;
		if(writeout) cout<<"P_cov(0,0): "<<P_cov(0,0)<<endl;
		
        if (fVis)
        {
            TLine* l1 = new TLine(-tpos.X(), tpos.Z(), -particle->GetX(), particle->GetZ());
            l1->Draw();
        }
        LOG(debug2) << particle->GetX() << ", " << particle->GetY() << ", " << particle->GetZ();
        
        if (!result)
        {
            return result;
        }
    }

    // =====================================================================
    // STEP 3: MAGNET ENTRANCE TO TARGET VERTEXT (Field-Free Region), this doesn't happen
    // =====================================================================
    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, -norm, intersect);
    
    if(writeout) cout<<"Magnet entrance to target vetrex? Should not happen, field map goes upstream of the target! "<<endl;
    if(writeout) cout<<"crossed= "<<crossed<<endl;
    if(writeout) cout<<"Plane: "<<v1.X()<<" "<<v1.Y()<<" "<<v1.Z()<<endl;
    if(writeout) cout<<"Planes norm: "<<-norm.X()<<" "<<-norm.Y()<<" "<<-norm.Z()<<endl;
    if(writeout) cout<<"Particle position: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
    if(writeout) cout<<"Particle momentum: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
    if(writeout) cout<<"Particle charge: "<<particle->GetCharge()<<endl;
    if(writeout) cout<<"Intersect point: "<<intersect.X()<<" "<<intersect.Y()<<" "<<intersect.Z()<<endl;
    
    if (crossed)
    {
        LOG(debug2) << "Propagating to end plane. Finish.";
        if (fVis)
        {
            TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
            l1->Draw();
        }
        
        pos_start = particle->GetPosition();
        step = (intersect - particle->GetPosition()).Mag();
        particle->SetPosition(intersect);
        particle->AddStep(step);
		if(writeout) cout<<"After step= "<<step<<endl;
		if(writeout) cout<<"Particle position: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
		if(writeout) cout<<"Particle momentum: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
		if(writeout) cout<<"Particle charge: "<<particle->GetCharge()<<endl;           
	   
         
        pos_end = particle->GetPosition();
        TVector3 pos_diff = pos_end - pos_start;
        TVector3 pos_def_local = TransformGlobalToLocalPlane(pos_diff, v1, v2, v3);

        F_linear.UnitMatrix();
        F_linear(0, 2) = pos_def_local.Z();
        F_linear(1, 3) = pos_def_local.Z();
		P_cov = F_linear * P_cov * TMatrixD(TMatrixD::kTransposed, F_linear);
		    
		if(writeout) cout<<"P_cov(0,0): "<<P_cov(0,0)<<endl;

        return kTRUE;
    }
    
    LOG(error) << "!!! Backward Propagation Failed to Intersect Target Plane !!!";
    return kFALSE;
}
/*
Bool_t R3BTPropagator::PropagateToPlaneBackward(R3BTrackingParticle* particle,
                                                const TVector3& v1,
                                                const TVector3& v2,
                                                const TVector3& v3,
                                                TMatrixD& P_cov)
{
    TVector3 norm = ((v2 - v1).Cross(v3 - v1)).Unit(); // this is -norm for backward tracking
    // Check if particle is already on plane
    if (TMath::Abs((particle->GetPosition() - v1).Dot(norm)) < 1e-6)
    {
        return kTRUE;
    }
    TVector3 pos_start = particle->GetPosition();
    TVector3 intersect;
    Bool_t crossed;
    Double_t step = 0.;
    Bool_t result;
    TVector3 pos_end;
    Double_t total_ds=0.;
    TMatrixD F_linear(5, 5);
			
    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), fPlane2[0], -fNorm2, intersect);
    if (crossed)
    {
        LOG(debug2) << "Starting at the end of the setup...";
        if ((v1 - particle->GetPosition()).Mag() > (fPlane2[0] - particle->GetPosition()).Mag())
        {
			LOG(debug2) << "Propagating to exit of magnetic field.";
			fVis = kFALSE;
			if (fVis)
			{
				TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
				l1->Draw();
			}
			step = (intersect - particle->GetPosition()).Mag();
			particle->SetPosition(intersect);
			particle->AddStep(step);
			
			pos_end = particle->GetPosition();
			total_ds = (pos_end - pos_start).Mag();
			F_linear.UnitMatrix();
			F_linear(0, 2) = -total_ds; // x = x + ux * ds
			F_linear(1, 3) = -total_ds; // y = y + uy * ds
			// P = F_linear * P * F_linear^T
			P_cov = F_linear * P_cov * TMatrixD(TMatrixD::kTransposed, F_linear);
				
			LOG(debug2) << intersect.X() << " " << intersect.Y() << " " << intersect.Z();
        }
        LOG(debug2) << "Propagating from exit back to fi3x detector";
		crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
		// fVis = kFALSE;
		if (fVis)
		{
			TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
			l1->Draw();
		}
		
		pos_start = particle->GetPosition();
		step = (intersect - particle->GetPosition()).Mag();
		particle->SetPosition(intersect);
		particle->AddStep(step);
			
		pos_end = particle->GetPosition();
		total_ds = (pos_end - pos_start).Mag();
		F_linear.UnitMatrix();
		F_linear(0, 2) = -total_ds; // x = x + ux * ds
		F_linear(1, 3) = -total_ds; // y = y + uy * ds
		// P = F_linear * P * F_linear^T
		P_cov = F_linear * P_cov * TMatrixD(TMatrixD::kTransposed, F_linear);
		
		return kTRUE;       
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
            result = PropagateToPlaneRKBackward(particle, v1, v3, v2, P_cov);
            fVis = kFALSE;
            if (fVis)
            {
                TLine* l1 = new TLine(-tpos.X(), tpos.Z(), -particle->GetX(), particle->GetZ());
                l1->Draw();
            }
            LOG(debug2) << particle->GetX() << ", " << particle->GetY() << ", " << particle->GetZ();

            return result;
        }
        LOG(debug2) << "Propagating to entrance of magnetic field.";
        tpos = particle->GetPosition();
        result = PropagateToPlaneRKBackward(particle, fPlane1[0], fPlane1[2], fPlane1[1], P_cov);
        fVis = kFALSE;
        if (fVis)
        {
            TLine* l1 = new TLine(-tpos.X(), tpos.Z(), -particle->GetX(), particle->GetZ());
            l1->Draw();
        }
        LOG(debug2) << particle->GetX() << ", " << particle->GetY() << ", " << particle->GetZ();
        
        if (!result)
        {
            return result;
        }
    }
    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
   
    if (crossed)
    {
        LOG(debug2) << "Propagating to end plane. Finish.";
        fVis = kFALSE;
        if (fVis)
        {
            TLine* l1 = new TLine(-particle->GetX(), particle->GetZ(), -intersect.X(), intersect.Z());
            l1->Draw();
        }
                
        pos_start = particle->GetPosition(); // Capture start
		step = (intersect - particle->GetPosition()).Mag();
		particle->SetPosition(intersect);
		particle->AddStep(step);
	   
		pos_end = particle->GetPosition();
		total_ds = (pos_end - pos_start).Mag();
		F_linear.UnitMatrix();
		F_linear(0, 2) = -total_ds;
		F_linear(1, 3) = -total_ds;
		P_cov = F_linear * P_cov * TMatrixD(TMatrixD::kTransposed, F_linear);
       
        return kTRUE;
    }

    // LOG(error) << "!!! Failed !!!";
    return kFALSE;
}
*/
Bool_t R3BTPropagator::PropagateToPlaneRKBackward(R3BTrackingParticle* particle,
                                          const TVector3& v1,
                                          const TVector3& v2,
                                          const TVector3& v3,
                                          TMatrixD& F_accumulator)
{
    Int_t nStep = 0;

    Double_t vecRKIn[7];
    Double_t vecTemp[7];
    Double_t vecOut[7];
    TVector3 dist;
    Double_t diff;
    TVector3 intersect;
    Bool_t crossed;
    Bool_t writeout = false;
   // if(abs(particle->GetCharge()) == 2) writeout = true;
    Double_t distance;

	F_accumulator.UnitMatrix();
    
    particle->GetPosition(vecRKIn);
    particle->GetCosines(&vecRKIn[3]);
  
    TVector3 norm = ((v2 - v1).Cross(v3 - v1)).Unit(); 
    
    if(writeout) cout<<"Will start with RK backwards with particle: "<<endl;
	if(writeout) cout<<"Particle position: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
	if(writeout) cout<<"Particle momentum: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
	if(writeout) cout<<"Particle charge: "<<particle->GetCharge()<<endl;  
	if(writeout) cout<<"norm vector: "<<-norm.X()<<" "<<-norm.Y()<<" "<<-norm.Z()<<endl;

    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, -norm, intersect);
    if(writeout) cout<<"crossed= "<<crossed<<endl;
    if(writeout) cout<<"Intersect point: "<<intersect.X()<<" "<<intersect.Y()<<" "<<intersect.Z()<<endl;
    
    dist = (particle->GetPosition() - intersect);
    diff = dist.Mag();
    if(writeout) cout<<"Distance to plane: "<<dist.X()<<" "<<dist.Y()<<" "<<dist.Z()<<" "<<diff<<endl;

    // Double_t step = TMath::Abs(diff) / 400.;// / 100.;
    Double_t step = 0.1;
    Double_t length = 0.;
    Double_t res = 10000.;
    Double_t res_old = 10000.;
    while (kTRUE)
    {
      	
	// Start shadow particle at the EXACT same position as the real particle BEFORE this RK step   
        Double_t vecRKShadowIn[7];
        for (Int_t ii = 0; ii < 7; ii++)
        {
            vecRKShadowIn[ii] = vecRKIn[ii];
        }
        
        TVector3 pos_before = particle->GetPosition();
       
        if (step > diff)
            step = diff;
         
        if(writeout) cout<<"RK step: "<<step<<endl; 
		if(writeout) cout<<"Particle position before RK step: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
		if(writeout) cout<<"Particle momentum before RK step: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
		if(writeout) cout<<"Particle charge before RK step: "<<particle->GetCharge()<<endl;     
        
        length = fFairProp->OneStepRungeKutta(particle->GetCharge(), step, vecRKIn, vecOut);
        
		
		Double_t dz_step = vecOut[2] - vecRKIn[2];
        
        for (Int_t ii = 0; ii < 7; ii++)
        {
            vecRKIn[ii] = vecOut[ii];
        }
                
        TVector3 xyz = particle->GetPosition();
        TVector3 mom = particle->GetMomentum();
        Double_t bx = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBx(xyz.X(), xyz.Y(), xyz.Z());
		Double_t by = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(xyz.X(), xyz.Y(), xyz.Z());
		Double_t bz = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBz(xyz.X(), xyz.Y(), xyz.Z());
		TVector3 bf(bx,by,bz);
		
        particle->SetPosition(vecOut);
        particle->SetCosines(&vecOut[3]);
        particle->AddStep(length);
        if(writeout) cout<<"RK length: "<<length<<endl; 
		if(writeout) cout<<"Particle position after RK length: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
		if(writeout) cout<<"Particle momentum  after RK length: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
		if(writeout) cout<<"Particle charge  after RK length: "<<particle->GetCharge()<<endl;     
       
        TVector3 pos_after = particle->GetPosition();
       
        // =====================================================================
        // KALMAN MICROSTEPPING DEPLOYMENT WITH NUMERICAL MOMENTUM COUPLING
        // =====================================================================
        
        TVector3 pos_diff = pos_after - pos_before;
        TVector3 pos_def_local = TransformGlobalToLocalPlane(pos_diff, v1, v2, v3);

		// 1. Core tracking parameters
		Double_t qOverP_real   = abs(particle->GetCharge()) / mom.Mag();
		Double_t qOverP_shadow = qOverP_real * 1.01;
		Double_t dqOverP       = qOverP_real - qOverP_shadow;

		Double_t vecShadowOut[7];

		Double_t shadow_charge_factor = particle->GetCharge() * 1.01; 
		fFairProp->OneStepRungeKutta(shadow_charge_factor, step, vecRKShadowIn, vecShadowOut);

		// 2. Extract local spatial step components
		TVector3 global_pos_delta(vecOut[0]-vecShadowOut[0], vecOut[1]-vecShadowOut[1], vecOut[2]-vecShadowOut[2]);
		TVector3 local_pos_delta = TransformGlobalToLocalPlane(global_pos_delta, v1, v2, v3);
		
		// Transform global momenta to local frame to get correct tracking slopes
		TVector3 local_mom_real = TransformGlobalToLocalPlane(particle->GetMomentum(), v1, v2, v3);			
		TVector3 shadow_mom_global(vecShadowOut[3], vecShadowOut[4], vecShadowOut[5]); // Rebuild shadow mom
		// Check if the real particle is moving backward globally (p_z < 0)
		// If it is, force the shadow vector's magnitude to be negative to keep it pointing upstream!
		Double_t p_mag_real = particle->GetMomentum().Mag(); // e.g., 12.9 GeV/c
		Double_t p_mag_shadow = p_mag_real / 1.01; // The correct scalar value for the shadow track
		Double_t p_mag_signed = p_mag_shadow;
		if (particle->GetMomentum().Z() < 0 && shadow_mom_global.Z() > 0.) {
			p_mag_signed = -p_mag_shadow;
		}		
		// Scale the shadow vector with the signed magnitude
		shadow_mom_global = shadow_mom_global.Unit() * (p_mag_signed / 1.01);
		TVector3 local_mom_shadow = TransformGlobalToLocalPlane(shadow_mom_global, v1, v2, v3);

		// Compute exact local slopes (u_x = p_x / p_z, u_y = p_y / p_z)
		Double_t slopeX_real   = local_mom_real.X() / local_mom_real.Z();
		Double_t slopeY_real   = local_mom_real.Y() / local_mom_real.Z();
		Double_t slopeX_shadow = local_mom_shadow.X() / local_mom_shadow.Z();
		Double_t slopeY_shadow = local_mom_shadow.Y() / local_mom_shadow.Z();

		Double_t dSlopeX_dqOverP = (slopeX_real - slopeX_shadow) / dqOverP;
		Double_t dSlopeY_dqOverP = (slopeY_real - slopeY_shadow) / dqOverP;

		// Build the microstep matrix for this individual small step
		TMatrixD F_micro(5, 5);
		F_micro.UnitMatrix();
		F_micro(0, 2) = pos_def_local.Z(); // Local coordinate step delta-z
		F_micro(1, 3) = pos_def_local.Z();

		if (TMath::Abs(by) > 0.01) 
		{
			F_micro(0, 4) = local_pos_delta.X() / dqOverP; 
			F_micro(1, 4) = local_pos_delta.Y() / dqOverP;
			F_micro(2, 4) = dSlopeX_dqOverP; 
			F_micro(3, 4) = dSlopeY_dqOverP;
		}

		// Accumulate the microstep into the total transport matrix chain
		F_accumulator = F_micro * F_accumulator;
		
        nStep += 1;
       
        // dist = particle->GetPosition() - v1;
        // distance = (TVector3(dist.X() * norm.X(), dist.Y() * norm.Y(), dist.Z() * norm.Z())).Mag();

        crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, -norm, intersect);
        
        if(writeout) cout<<"How far away is particle from the plane after RK step: "<<endl;
        if(writeout) cout<<"crossed: "<<crossed<<endl;
		if(writeout) cout<<"Intersect point: "<<intersect.X()<<" "<<intersect.Y()<<" "<<intersect.Z()<<endl;

        dist = (particle->GetPosition() - intersect);
        distance = dist.Mag();
        if(writeout) cout<<"Distance: "<<dist.X()<<" "<<dist.Y()<<" "<<dist.Z()<<" "<<distance<<endl;
       
        res = TMath::Abs(distance); // / diff);

        if (res < 0.1 || res > res_old)
        {
             if(writeout) cout<<"RK backward propagations reached the plane"<<endl;
             if(writeout) cout<<"Intersection point: "<<intersect.X()<<", "<<intersect.Y()<<", "<<intersect.Z()<<endl;

            break;
        }
        else
        {
            res_old = res;
            
            // 1. Calculate the physics-based step size solely from the local field intensity
			TVector3 pos = particle->GetPosition();
			Double_t field = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(pos.X(), pos.Y(), pos.Z());
			Double_t abs_field = TMath::Abs(field);

			// Define standard operating boundaries
			Double_t max_step = 10.0; // Reduced from 50.0 to guarantee linear tracking in GLAD field
			Double_t min_step = 1.0;  // Never drop below 1 cm based on field alone

			if (abs_field > 0.01) // Protect against division by zero in zero-field regions
			{
				step = 20. / abs_field; 
			}
			else 
			{
				step = max_step; // In weak/zero field, take a standard stable step
			}

			// Clamp the step size so it stays within safe numerical boundaries
			if (step > max_step) step = max_step;
			if (step < min_step) step = min_step;

			// TRAP PREVENTION: Only shrink the step if it would physically overshoot the plane!
			if (step > distance)
			{
				step = distance; // Take exactly the remaining distance to hit the plane dead-on
			}

           if(writeout) cout<<"Calculating step in RK field= "<<field<<", 20/field= "<<20. / abs_field<<", distance= "<<distance<<", step= "<<step<<endl;
        }

        if (nStep > 100000)
        {
            return kFALSE;
        }
    }
    /*
    // Propagate the error matrix with full momentum couplings intact
        P_cov = F_total * P_cov * TMatrixD(TMatrixD::kTransposed, F_total);
        // Add a tiny bit of diagonal process noise 
		for(Int_t i=0; i<5; i++) {
			P_cov(i,i) += 1e-6; 
		}
   */
    return kTRUE;
}

Bool_t R3BTPropagator::PropagateToPlaneRKForward(R3BTrackingParticle* particle,
                                          const TVector3& v1,
                                          const TVector3& v2,
                                          const TVector3& v3,
                                          TMatrixD& F_accumulator)
{
    Int_t nStep = 0;

    Double_t vecRKIn[7];
    Double_t vecTemp[7];
    Double_t vecOut[7];
    TVector3 dist;
    Double_t diff;
    TVector3 intersect;
    Bool_t crossed;
    Bool_t writeout= false;
    Double_t distance;

	F_accumulator.UnitMatrix();
    
    particle->GetPosition(vecRKIn);

    particle->GetCosines(&vecRKIn[3]);

    TVector3 norm = ((v2 - v1).Cross(v3 - v1)).Unit();
        
    if(writeout) cout<<"Will start with RK forward with particle: "<<endl;
	if(writeout) cout<<"Particle position: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
	if(writeout) cout<<"Particle momentum: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
	if(writeout) cout<<"Particle charge: "<<particle->GetCharge()<<endl;  
	if(writeout) cout<<"norm vector: "<<norm.X()<<" "<<norm.Y()<<" "<<norm.Z()<<endl;

    crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
    dist = (particle->GetPosition() - intersect);
    diff = dist.Mag();
    if(writeout) cout<<"crossed= "<<crossed<<endl;
    if(writeout) cout<<"Intersect point: "<<intersect.X()<<" "<<intersect.Y()<<" "<<intersect.Z()<<endl;
    if(writeout) cout<<"Distance to plane: "<<dist.X()<<" "<<dist.Y()<<" "<<dist.Z()<<" "<<diff<<endl;
    
    Double_t step = 0.1;
    Double_t length = 0.;
    Double_t res = 10000.;
    Double_t res_old = 10000.;
    
    while (kTRUE)
    {
         // Start shadow particle at the EXACT same position as the real particle BEFORE this RK step   
        Double_t vecRKShadowIn[7];
        for (Int_t ii = 0; ii < 7; ii++)
        {
            vecRKShadowIn[ii] = vecRKIn[ii];
        }
        if (step > diff)
            step = diff;
                   
        if(writeout) cout<<"RK step: "<<step<<endl; 
		if(writeout) cout<<"Particle position before RK step: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
		if(writeout) cout<<"Particle momentum before RK step: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
		if(writeout) cout<<"Particle charge before RK step: "<<particle->GetCharge()<<endl;     
               
        TVector3 pos_before = particle->GetPosition();
            
        length = fFairProp->OneStepRungeKutta(particle->GetCharge(), step, vecRKIn, vecOut);
        
        if(writeout) cout<<"RK length: "<<length<<endl; 
		if(writeout) cout<<"Particle position after RK length: "<<particle->GetPosition().X()<<" "<<particle->GetPosition().Y()<<" "<<particle->GetPosition().Z()<<endl;
		if(writeout) cout<<"Particle momentum  after RK length: "<<particle->GetMomentum().X()<<" "<<particle->GetMomentum().Y()<<" "<<particle->GetMomentum().Z()<<endl;
		if(writeout) cout<<"Particle charge  after RK length: "<<particle->GetCharge()<<endl; 
        
         // 4. Calculate the true longitudinal dz step for scaling
        Double_t dz_step = vecOut[2] - vecRKIn[2];
            
        for (Int_t ii = 0; ii < 7; ii++)
        {
            vecRKIn[ii] = vecOut[ii];
        }
      
        TVector3 xyz = particle->GetPosition();
        TVector3 mom = particle->GetMomentum();
        Double_t bx = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBx(xyz.X(), xyz.Y(), xyz.Z());
		Double_t by = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(xyz.X(), xyz.Y(), xyz.Z());
		Double_t bz = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBz(xyz.X(), xyz.Y(), xyz.Z());
		TVector3 bf(bx,by,bz);
	
        particle->SetPosition(vecOut);
        particle->SetCosines(&vecOut[3]);
        particle->AddStep(length);
        
        TVector3 pos_after = particle->GetPosition();
        
        // =====================================================================
        // KALMAN MICROSTEPPING DEPLOYMENT WITH NUMERICAL MOMENTUM COUPLING
        // =====================================================================
        
        TVector3 pos_diff = pos_after - pos_before;
        TVector3 pos_def_local = TransformGlobalToLocalPlane(pos_diff, v1, v2, v3);

		// 1. Core tracking parameters
		Double_t qOverP_real   = particle->GetCharge() / mom.Mag();
		Double_t qOverP_shadow = qOverP_real * 1.01;
		Double_t dqOverP       = qOverP_real - qOverP_shadow;

		Double_t vecShadowOut[7];

		Double_t shadow_charge_factor = particle->GetCharge() * 1.01; 
		fFairProp->OneStepRungeKutta(shadow_charge_factor, step, vecRKShadowIn, vecShadowOut);

		// 2. Extract local spatial step components
		TVector3 global_pos_delta(vecOut[0]-vecShadowOut[0], vecOut[1]-vecShadowOut[1], vecOut[2]-vecShadowOut[2]);
		TVector3 local_pos_delta = TransformGlobalToLocalPlane(global_pos_delta, v1, v2, v3);
		
		// Transform global momenta to local frame to get correct tracking slopes
		TVector3 local_mom_real = TransformGlobalToLocalPlane(particle->GetMomentum(), v1, v2, v3);			
		TVector3 shadow_mom_global(vecShadowOut[3], vecShadowOut[4], vecShadowOut[5]); // Rebuild shadow mom
		Double_t p_mag_real = particle->GetMomentum().Mag(); 
		Double_t p_mag_shadow = p_mag_real / 1.01; // The correct scalar value for the shadow track
		
		// Scale the shadow vector with the signed magnitude
		shadow_mom_global = shadow_mom_global.Unit() * (p_mag_shadow / 1.01);
		TVector3 local_mom_shadow = TransformGlobalToLocalPlane(shadow_mom_global, v1, v2, v3);

		// Compute exact local slopes (u_x = p_x / p_z, u_y = p_y / p_z)
		Double_t slopeX_real   = local_mom_real.X() / local_mom_real.Z();
		Double_t slopeY_real   = local_mom_real.Y() / local_mom_real.Z();
		Double_t slopeX_shadow = local_mom_shadow.X() / local_mom_shadow.Z();
		Double_t slopeY_shadow = local_mom_shadow.Y() / local_mom_shadow.Z();

		Double_t dSlopeX_dqOverP = (slopeX_real - slopeX_shadow) / dqOverP;
		Double_t dSlopeY_dqOverP = (slopeY_real - slopeY_shadow) / dqOverP;

		// Build the microstep matrix for this individual small step
		TMatrixD F_micro(5, 5);
		F_micro.UnitMatrix();
		F_micro(0, 2) = pos_def_local.Z(); // Local coordinate step delta-z
		F_micro(1, 3) = pos_def_local.Z();

		if (TMath::Abs(by) > 0.01) 
		{
			F_micro(0, 4) = local_pos_delta.X() / dqOverP; 
			F_micro(1, 4) = local_pos_delta.Y() / dqOverP;
			F_micro(2, 4) = dSlopeX_dqOverP; 
			F_micro(3, 4) = dSlopeY_dqOverP;
		}

		// Accumulate the microstep into the total transport matrix chain
		F_accumulator = F_micro * F_accumulator;
 
        nStep += 1;

        crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
        
        if(writeout) cout<<"How far away is particle from the plane after RK step: "<<endl;
        if(writeout) cout<<"crossed: "<<crossed<<endl;
		if(writeout) cout<<"Intersect point: "<<intersect.X()<<" "<<intersect.Y()<<" "<<intersect.Z()<<endl;

        dist = (particle->GetPosition() - intersect);
        distance = dist.Mag();
        if(writeout) cout<<"Distance: "<<dist.X()<<" "<<dist.Y()<<" "<<dist.Z()<<" "<<distance<<endl;

        res = TMath::Abs(distance); 

        if (res < 0.1 || res > res_old)
        { 
             if(writeout) cout<<"residuals: "<<res<<" "<<res_old<<endl;
             if(writeout) cout<<"RK forward propagations reached the plane"<<endl;
             if(writeout) cout<<"Intersection point: "<<intersect.X()<<", "<<intersect.Y()<<", "<<intersect.Z()<<endl;

            break;
        }
        else
        {
            res_old = res;

            // step = 1.;

            TVector3 pos = particle->GetPosition();
            Double_t field = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(pos.X(), pos.Y(), pos.Z());
			Double_t abs_field = TMath::Abs(field);

			// Define standard operating boundaries
			Double_t max_step = 10.0; // Reduced from 50.0 to guarantee linear tracking in GLAD field
			Double_t min_step = 1.0;  // Never drop below 1 cm based on field alone

			if (abs_field > 0.01) // Protect against division by zero in zero-field regions
			{
				step = 20. / abs_field; 
			}
			else 
			{
				step = max_step; // In weak/zero field, take a standard stable step
			}

			// Clamp the step size so it stays within safe numerical boundaries
			if (step > max_step) step = max_step;
			if (step < min_step) step = min_step;

			// TRAP PREVENTION: Only shrink the step if it would physically overshoot the plane!
			if (step > distance)
			{
				step = distance; // Take exactly the remaining distance to hit the plane dead-on
			}

           if(writeout) cout<<"Calculating step in RK field= "<<field<<", 20/field= "<<20. / abs_field<<", distance= "<<distance<<", step= "<<step<<endl;
           
        }

        if (nStep > 100000)
        {
            return kFALSE;
        }
    }
    /*
       // Propagate the error matrix with full momentum couplings intact
        P_cov = F_total * P_cov * TMatrixD(TMatrixD::kTransposed, F_total);
        // Add a tiny bit of diagonal process noise 
		for(Int_t i=0; i<5; i++) {
			P_cov(i,i) += 1e-6; 
		}*/
    return kTRUE;
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

    // cout<<"In R3BTPropagator::PropagateToPlaneRK: "<<endl;
    // particle->GetPosition().Print();

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

        //  cout<<"TEST0: "<<particle->GetPosition().Z()<<", "<<step<<"; "<<diff<<endl;

        if (step > diff)
            step = diff;
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
        
        
        //   particle->GetPosition().Print();

        nStep += 1;

        // dist = particle->GetPosition() - v1;
        // distance = (TVector3(dist.X() * norm.X(), dist.Y() * norm.Y(), dist.Z() * norm.Z())).Mag();

        crossed = LineIntersectPlane(particle->GetPosition(), particle->GetMomentum(), v1, norm, intersect);
        // cout<<"In propagatRK intersect/v1/particle: "<<endl;
        // intersect.Print();
        // v1.Print();
        // particle->GetPosition().Print();

        dist = (particle->GetPosition() - intersect);
        distance = dist.Mag();
        // cout<<"Distance: "<<distance<<endl;
        // dist.Print();

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

            // step = 1.;

            TVector3 pos = particle->GetPosition();
            Double_t field = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(pos.X(), pos.Y(), pos.Z());

            // particle->GetPosition().Print();
            // cout<<"By: "<<field<<endl;

            step = 20. / TMath::Abs(field); // 20.
            // if(step < 0.1) step = 0.1;

            //  cout << "x: " << pos.X() << " y: " << pos.Y() << " z: " << pos.Z() <<
            // " Field:" << field << " step from Bfield: " << 10./ TMath::Abs(field) << endl;

            //    cout <<  pos.X()<<" " <<  pos.Y()<<" " << pos.Z()<<" " << field <<  endl;

            if (step > TMath::Abs(distance))
            {
                step = TMath::Abs(distance) / 2.;
            }
            if (step > 50.) // 20
            {
                step = 50.; // 20
            }
            /*
            if (step < 0.2) //0.1
            {
                step = 0.2; //0.1
            }
            * */
            if (TMath::Abs(distance) < 0.25) // 0.1
            {
                step = TMath::Abs(distance); // 0.1
            }
            //   step = 0.1;

            //  cout<<"Step final: "<<step<<endl;

            //  cout<<"TEST1: "<<particle->GetPosition().Z()<<", "<<step<<"; "<<diff<<endl;
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
    if (pn == 0)// particle is moving perfectly paralel to the plane, and will not intercept it
    {
        return kFALSE;
    }
    Double_t t = delta.Dot(normal) / pn;
    if (t < 0)// particle has already passed through plane
    {
        return kFALSE;
    }
    else// particle will hit the plane at intersect position
    {
        intersect = pos + mom * t;
    }
    return kTRUE;
}
TVector3 R3BTPropagator::TransformGlobalToLocalPlane(const TVector3& globalVector,
                                                     const TVector3& v1,
                                                     const TVector3& v2,
                                                     const TVector3& v3)
{
    // 1. Define the local axis unit vectors of the target plane based on its 3 geometry points
    // Local X-axis: pointing along the direction from v1 to v2
    TVector3 localX = (v2 - v1).Unit();
    
    // Local Z-axis (Normal vector): perpendicular to the plane surface
    TVector3 localZ = ((v2 - v1).Cross(v3 - v1)).Unit();
    
    // Local Y-axis: mutually perpendicular to close the right-handed coordinate system
    TVector3 localY = localZ.Cross(localX);

    // 2. Project the global displacement vector onto these local axes using dot products
    Double_t localDx = globalVector.Dot(localX);
    Double_t localDy = globalVector.Dot(localY);
    Double_t localDz = globalVector.Dot(localZ);

    // Return the vector converted into the detector's local frame coordinates
    return TVector3(localDx, localDy, localDz);
}

ClassImp(R3BTPropagator)
