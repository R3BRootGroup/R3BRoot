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

#include "R3BOptimizeGeometryS494.h"
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

#include "R3BTofdCalData.h"
#include "R3BTofdHitData.h"
#include "R3BTofdMappedData.h"

#include "R3BFiberMAPMTCalData.h"
#include "R3BFiberMAPMTHitData.h"
#include "R3BFiberMAPMTMappedData.h"

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
namespace GlobalChi2 {
R3BTrackingSetup* gSetup;
R3BOptimizeGeometryS494* gThisTask;
}

using namespace GlobalChi2;

R3BOptimizeGeometryS494::R3BOptimizeGeometryS494(const char* name, Bool_t vis, Int_t verbose)
    : FairTask(name, verbose)
    , fFieldPar(NULL)
    , fPropagator(NULL)
    , fArrayMCTracks(NULL)
    , fDetectors(new R3BTrackingSetup())
    , fDetectorsLeft(new R3BTrackingSetup())
    , fDetectorsRight(new R3BTrackingSetup())
    , fArrayFragments(new TClonesArray("R3BTrackingParticle"))
    , fNEvents(0)
    , fVis(vis)
    , fFitter(nullptr)
    , fEnergyLoss(kTRUE)
    , fSimu(kTRUE)
    , fLeft(kFALSE)
{
    // this is the list of detectors (active areas) we use for tracking
    fDetectorsLeft->AddDetector("target", kTarget, "TargetGeoPar");
    fDetectorsLeft->AddDetector("fi23a", kTargetGlad, "fi23aGeoPar", "Fi23aHit");
    fDetectorsLeft->AddDetector("fi23b", kTargetGlad, "fi23bGeoPar", "Fi23bHit");
    fDetectorsLeft->AddDetector("fi30", kAfterGlad, "fi30GeoPar", "Fi30Hit");
    fDetectorsLeft->AddDetector("fi32", kAfterGlad, "fi32GeoPar", "Fi32Hit");
    fDetectorsLeft->AddDetector("tofd", kTof, "tofdGeoPar", "TofdHit");

    fDetectorsRight->AddDetector("target", kTarget, "TargetGeoPar");
    fDetectorsRight->AddDetector("fi23a", kTargetGlad, "fi23aGeoPar", "Fi23aHit");
    fDetectorsRight->AddDetector("fi23b", kTargetGlad, "fi23bGeoPar", "Fi23bHit");
    fDetectorsRight->AddDetector("fi31", kAfterGlad, "fi31GeoPar", "Fi31Hit");
    fDetectorsRight->AddDetector("fi33", kAfterGlad, "fi33GeoPar", "Fi33Hit");
    fDetectorsRight->AddDetector("tofd", kTof, "tofdGeoPar", "TofdHit");
}

R3BOptimizeGeometryS494::~R3BOptimizeGeometryS494() {}

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
InitStatus R3BOptimizeGeometryS494::Init()
{
    FairRootManager* man = FairRootManager::Instance();


    fArrayMCTracks = (TClonesArray*)man->GetObject("MCTrack");
    if (NULL == fArrayMCTracks)
    {
        LOG(ERROR) << "No MC Track array found in input file.";
//        return kERROR;
    }
 
    assert(DET_MAX + 1 == sizeof(fDetectorNames) / sizeof(fDetectorNames[0]));
    printf("Have %d fiber detectors.\n", NOF_FIB_DET);
    for (int det = 0; det < DET_MAX; det++)
    {
        fArrayHits.push_back((TClonesArray*)man->GetObject(Form("%sHit", fDetectorNames[det])));
        //if (det == DET_MAX - 1)
        //    maxevent = man->CheckMaxEventNo();
        if (NULL == fArrayHits.at(det))
        {
            printf("Could not find hit data for '%s'.\n", fDetectorNames[det]);
        }
        else
        {
            printf("Found hit data for '%s'.\n", fDetectorNames[det]);
        }
    }

    man->Register("TrackingParticle", "Tracking", fArrayFragments, kTRUE);

    if (!InitPropagator())
    {
        return kERROR;
    }

    fDetectorsLeft->Init();
    fDetectorsRight->Init();
	fDetectors->Init();

// For the moment we have to change manually between the setup
	//gSetup = fDetectorsLeft;
	gSetup = fDetectorsRight;
	
	gThisTask = this;
	
    fh_mult_fi23a = new TH1F("h_mult_fi23a", "Multiplicity fi23a", 20, -0.5, 19.5);
    fh_mult_fi23b = new TH1F("h_mult_fi23b", "Multiplicity fi23b", 20, -0.5, 19.5);
    fh_mult_fi32 = new TH1F("h_mult_fi32", "Multiplicity fi32", 20, -0.5, 19.5);
    fh_mult_fi31 = new TH1F("h_mult_fi31", "Multiplicity fi31", 20, -0.5, 19.5);
    fh_mult_fi30 = new TH1F("h_mult_fi30", "Multiplicity fi30", 20, -0.5, 19.5);
    fh_mult_fi33 = new TH1F("h_mult_fi33", "Multiplicity fi33", 20, -0.5, 19.5);
    fh_mult_tofd = new TH1F("h_mult_tofd", "Multiplicity TOFd", 20, -0.5, 19.5);
    fh_eloss_fi23a_mc = new TH1F("h_eloss_fi23a_mc", "Energy loss fi23a (MC truth)", 2000, 0., 200.);
    fh_eloss_fi23a = new TH1F("h_eloss_fi23a", "Energy loss fi23a", 2000, 0., 200.);
    fh_eloss_fi23b_mc = new TH1F("h_eloss_fi23b_mc", "Energy loss fi23b (MC truth)", 2000, 0., 500.);
    fh_eloss_fi23b = new TH1F("h_eloss_fi23b", "Energy loss fi23b", 2000, 0., 500.);
    fh_eloss_fi32_mc = new TH1F("h_eloss_fi32_mc", "Energy loss fi32 (MC truth)", 2000, 0., 500.);
    fh_eloss_fi32 = new TH1F("h_eloss_fi32", "Energy loss fi32", 2000, 0., 500.);
    fh_eloss_fi31_mc = new TH1F("h_eloss_fi31_mc", "Energy loss fi31 (MC truth)", 2000, 0., 500.);
    fh_eloss_fi31 = new TH1F("h_eloss_fi31", "Energy loss fi31", 2000, 0., 500.);
    fh_eloss_fi30_mc = new TH1F("h_eloss_fi30_mc", "Energy loss fi30 (MC truth)", 2000, 0., 1000.);
    fh_eloss_fi30 = new TH1F("h_eloss_fi30", "Energy loss fi30", 2000, 0., 1000.);
    fh_eloss_fi33_mc = new TH1F("h_eloss_fi33_mc", "Energy loss fi33 (MC truth)", 2000, 0., 500.);
    fh_eloss_fi33 = new TH1F("h_eloss_fi33", "Energy loss fi33", 2000, 0., 500.);
    fh_ncand = new TH1F("h_ncand", "Number of candidates", 100, -0.5, 99.5);
    fh_A_reco1 = new TH1F("h_A_reco1", "Reconstructed mass, step 1", 2000., 0., 20.);
    fh_A_reco2 = new TH1F("h_A_reco2", "Reconstructed mass, step 2", 2000., 0., 20.);
    fh_mom_res = new TH1F("h_mom_res", "Momentum resolution", 500, -0.05, 0.05);
    fh_mass_res = new TH1F("h_mass_res", "Mass resolution", 1000, -5., 5.);
    fh_chi2 = new TH1F("h_chi2", "Chi2", 1000, 0., 100.);
    fh_vz_res = new TH1F("h_vz_res", "vz - vz_mc", 200, -1., 1.);
    fh_beta_res = new TH1F("h_beta_res", "beta - beta_mc", 200, -0.01, 0.01);
    fh_A_overZ = new TH2F("fh_A_overZ", "particle identification", 100, 0., 10.,100, 1., 3.);
    fh_p = new TH1F("h_p", "momentum p", 2000, 0., 20.);
    fh_px = new TH1F("h_px", "momentum px", 1000, -500., 500.);
    fh_py = new TH1F("h_py", "momentum py", 1000, -500., 500.);
    fh_pz = new TH1F("h_pz", "momentum pz", 2000, 0., 20.);
	fh_residuals = new TH1F("h_residuals", "residuals p", 1000, -500., 500.);
    
    Double_t ranges[] = { 1., 1., 1., 1., 1., 1., 10. };
    for (Int_t i = 0; i < 7; i++)
    {
        fh_x_res[i] = new TH1F(Form("h_x_res%d", i), Form("x residual %d", i), 200, -ranges[i], ranges[i]);
        fh_x_pull[i] = new TH1F(Form("h_x_pull%d", i), Form("x pull %d", i), 40, -10., 10.);
    }
    fFitter->Init(fPropagator, fEnergyLoss);
    return kSUCCESS;
}

InitStatus R3BOptimizeGeometryS494::ReInit()
{
    SetParContainers();

    if (!InitPropagator())
    {
        return kERROR;
    }

    return kSUCCESS;
}

void R3BOptimizeGeometryS494::SetParContainers()
{
    fFieldPar = (R3BFieldPar*)FairRuntimeDb::instance()->getContainer("R3BFieldPar");

    fDetectorsLeft->SetParContainers();
    fDetectorsRight->SetParContainers();
    fDetectors->SetParContainers();
    
}

void R3BOptimizeGeometryS494::Exec(const Option_t*)
{
	
	
// For the moment one has to swap manually between the setups 
/*
	R3BTrackingDetector* target = fDetectorsLeft->GetByName("target");
    R3BTrackingDetector* fi23a = fDetectorsLeft->GetByName("fi23a");
    R3BTrackingDetector* fi23b = fDetectorsLeft->GetByName("fi23b");
    R3BTrackingDetector* fi30 = fDetectorsLeft->GetByName("fi30");
    R3BTrackingDetector* fi31 = fDetectorsRight->GetByName("fi31");
    R3BTrackingDetector* fi32 = fDetectorsLeft->GetByName("fi32");
    R3BTrackingDetector* fi33 = fDetectorsRight->GetByName("fi33");
    R3BTrackingDetector* tof = fDetectorsLeft->GetByName("tofd");

    if (tof->fArrayHits->GetEntriesFast() < 1)
        return;
    if (fi23a->fArrayHits->GetEntriesFast() < 1)
        return;
    if (fi23b->fArrayHits->GetEntriesFast() < 1)
        return;
    if (fi30->fArrayHits->GetEntriesFast() < 1)
        return;
    if (fi32->fArrayHits->GetEntriesFast() < 1)
        return;

    if (tof->fArrayHits->GetEntriesFast() > 0)
    {
		fDetectorsLeft->CopyToBuffer();		
		cout << "Copy Hits: " 
		<< fi23a->events[fNEvents].size() << "  " << fi23b->events[fNEvents].size() << "  "
		<< fi32->events[fNEvents].size() << "  " << fi30->events[fNEvents].size() << "  " 
		<< tof->events[fNEvents].size() << endl;

        fNEvents += 1;
		cout << "Saved events: " << gThisTask->GetNEvents() << endl;
	}
*/
// For the moment we have to change manually between the setup

	R3BTrackingDetector* target = fDetectorsRight->GetByName("target");
	R3BTrackingDetector* tof = fDetectorsRight->GetByName("tofd");
	R3BTrackingDetector* fi23a = fDetectorsRight->GetByName("fi23a");
	R3BTrackingDetector* fi31 = fDetectorsRight->GetByName("fi31");
	R3BTrackingDetector* fi33 = fDetectorsRight->GetByName("fi33");
	R3BTrackingDetector* fi23b = fDetectorsRight->GetByName("fi23b");
	R3BTrackingDetector* fi32 = fDetectorsLeft->GetByName("fi32");
	R3BTrackingDetector* fi30 = fDetectorsLeft->GetByName("fi30");

    if (tof->fArrayHits->GetEntriesFast() < 1)
        return;
    if (fi23a->fArrayHits->GetEntriesFast() < 1)
        return;
    if (fi23b->fArrayHits->GetEntriesFast() < 1)
        return;
    if (fi31->fArrayHits->GetEntriesFast() < 1)
        return;
    if (fi33->fArrayHits->GetEntriesFast() < 1)
        return;

    if (tof->fArrayHits->GetEntriesFast() > 0)
    {
		fDetectorsRight->CopyToBuffer();		
		cout << "Copy Hits: " 
		<< fi23a->events[fNEvents].size() << "  " << fi23b->events[fNEvents].size() << "  " 
		<< fi31->events[fNEvents].size() << "  " 
		<< fi33->events[fNEvents].size() << "  " << tof->events[fNEvents].size() << endl;

		cout << "Saved events: " << gThisTask->GetNEvents() << endl;

        fNEvents += 1;
	}


}

double Chi2AllEvents(const double* xx)
{
	// For the moment ...
	Bool_t fLeft = kFALSE;
	//Bool_t fLeft = kTRUE;
    Double_t chi2_all = 0;
	Int_t nof = 0;
	cout.precision(7);
	cout << "new correction: " << xx[0] << "  " << xx[1] << "  " << xx[2] 
	<< "  " << xx[3] << "  " << xx[4] << "  " << xx[5] << "  " << xx[6] 
	<< "  " << xx[7] << "  " << xx[8]<< endl;

	if(fLeft)
	{
		R3BTrackingDetector* fi32 = gSetup->GetByName("fi32");
		
		fi32->pos0 = TVector3(0., 0., 0.);
		fi32->pos1 = TVector3(25., 25., 0.);
		fi32->pos2 = TVector3(-25., 25., 0.);

		fi32->pos0.RotateY( xx[5] * TMath::DegToRad());
		fi32->pos1.RotateY( xx[5] * TMath::DegToRad());
		fi32->pos2.RotateY( xx[5] * TMath::DegToRad());

		TVector3 trans12(xx[2], 0., xx[3]);

		fi32->pos0 += trans12;
		fi32->pos1 += trans12;
		fi32->pos2 += trans12;
		fi32->norm = ((fi32->pos1 - fi32->pos0).Cross(fi32->pos2 - fi32->pos0)).Unit();

		
		R3BTrackingDetector* fi30 = gSetup->GetByName("fi30");
		fi30->pos0 = TVector3(0., 0., 0.);
		fi30->pos1 = TVector3(25., 25., 0.);
		fi30->pos2 = TVector3(-25., 25., 0.);

		fi30->pos0.RotateY(xx[4] * TMath::DegToRad());
		fi30->pos1.RotateY(xx[4] * TMath::DegToRad());
		fi30->pos2.RotateY(xx[4] * TMath::DegToRad());

		TVector3 trans10(xx[0], 0., xx[1]);

		fi30->pos0 += trans10;
		fi30->pos1 += trans10;
		fi30->pos2 += trans10;
		fi30->norm = ((fi30->pos1 - fi30->pos0).Cross(fi30->pos2 - fi30->pos0)).Unit();

		R3BTrackingDetector* fi23a = gSetup->GetByName("fi23a");
		
		fi23a->pos0 = TVector3(0., 0., 0.);
		fi23a->pos1 = TVector3(5., 5., 0.);
		fi23a->pos2 = TVector3(-5., 5., 0.);

		fi23a->pos0.RotateY( 0. * TMath::DegToRad());
		fi23a->pos1.RotateY( 0. * TMath::DegToRad());
		fi23a->pos2.RotateY( 0. * TMath::DegToRad());

		TVector3 trans3b(xx[7], 0., xx[8]);

		fi23a->pos0 += trans3b;
		fi23a->pos1 += trans3b;
		fi23a->pos2 += trans3b;
		fi23a->norm = ((fi23a->pos1 - fi23a->pos0).Cross(fi23a->pos2 - fi23a->pos0)).Unit();

	}
	else
	{
	
		R3BTrackingDetector* fi31 = gSetup->GetByName("fi31");
		
		fi31->pos0 = TVector3(0., 0., 0.);
		fi31->pos1 = TVector3(25., 25., 0.);
		fi31->pos2 = TVector3(-25., 25., 0.);

		fi31->pos0.RotateY(xx[4] * TMath::DegToRad());
		fi31->pos1.RotateY(xx[4] * TMath::DegToRad());
		fi31->pos2.RotateY(xx[4] * TMath::DegToRad());
		//cout << "Rotation: " << xx[4] << endl;
		TVector3 trans11(xx[0], 0., xx[1]);

		fi31->pos0 += trans11;
		fi31->pos1 += trans11;
		fi31->pos2 += trans11;
		fi31->norm = ((fi31->pos1 - fi31->pos0).Cross(fi31->pos2 - fi31->pos0)).Unit();

		
		R3BTrackingDetector* fi33 = gSetup->GetByName("fi33");
		fi33->pos0 = TVector3(0., 0., 0.);
		fi33->pos1 = TVector3(25., 25., 0.);
		fi33->pos2 = TVector3(-25., 25., 0.);

		fi33->pos0.RotateY(xx[5] * TMath::DegToRad());
		fi33->pos1.RotateY(xx[5] * TMath::DegToRad());
		fi33->pos2.RotateY(xx[5] * TMath::DegToRad());
		//cout << "Rotation: " << xx[5] << endl;

		TVector3 trans13(xx[2], 0., xx[3]);

		fi33->pos0 += trans13;
		fi33->pos1 += trans13;
		fi33->pos2 += trans13;
		fi33->norm = ((fi33->pos1 - fi33->pos0).Cross(fi33->pos2 - fi33->pos0)).Unit();

		R3BTrackingDetector* fi23a = gSetup->GetByName("fi23a");
		
		fi23a->pos0 = TVector3(0., 0., 0.);
		fi23a->pos1 = TVector3(5., 5., 0.);
		fi23a->pos2 = TVector3(-5., 5., 0.);

		fi23a->pos0.RotateY( 180. * TMath::DegToRad());
		fi23a->pos1.RotateY( 180. * TMath::DegToRad());
		fi23a->pos2.RotateY( 180. * TMath::DegToRad());

		TVector3 trans3a(xx[7], 0., xx[8]);

		fi23a->pos0 += trans3a;
		fi23a->pos1 += trans3a;
		fi23a->pos2 += trans3a;
		fi23a->norm = ((fi23a->pos1 - fi23a->pos0).Cross(fi23a->pos2 - fi23a->pos0)).Unit();
	}

    for(Int_t iev = 0; iev < gThisTask->GetNEvents(); iev++)
    {
        gSetup->TakeHitsFromBuffer(iev);
		Double_t fieldScale;
/*
		Double_t fieldScale	= -1710.0/3583.81 * 1.0;	//standard
		Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
		Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
		//cout << "Field:" << field << " scale: " << scale << endl;
		
		fieldScale	= -1710.0/3583.81 / scale * xx[6];	
		//cout << "Setting field to " << 	fieldScale << endl;
		( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrection(fieldScale);						
		field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
		//cout << "Field after:" << 	field << endl;
*/

		if(fLeft)
		{
			if(iev == 0) 
			{			
				Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
				Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
				cout << "Field:" << field << " scale: " << scale << endl;			
				fieldScale	= -1560.0 / 3583.81 / scale * xx[6];	//run395
				cout << "Setting field to " << 	fieldScale << endl;
				( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrection(fieldScale);						
				field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
				cout << "Field after:" << 	field << endl;
		
			}
			if(iev == 100) 
			//if(iev == 97) 
			{				
				Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
				Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
				cout << "Field:" << field << " scale: " << scale<< endl;
				fieldScale	= -1410.0 / 3583.81 / scale * xx[6];	//run391
				cout << "Setting field to " << 	fieldScale << endl;
				( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrection(fieldScale);						
				field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
				cout << "Field after:" << 	field << endl;
			}
			if(iev == 200) 
			//if(iev == 196) 
			{				
				Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
				Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
				cout << "Field:" << field << " scale: " << scale << endl;
				fieldScale	= -1260.0 / 3583.81 / scale * xx[6];	//run389
				cout << "Setting field to " << 	fieldScale << endl;
				( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrection(fieldScale);									
				field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
				cout << "Field after:" << 	field << endl;
			}
			if(iev == 300) 
			//if(iev == 296) 
			{				
				Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
				Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
				cout << "Field:" << field << " scale: " << scale << endl;
				fieldScale	= -1110.0 / 3583.81 / scale * xx[6];	//run389
				cout << "Setting field to " << 	fieldScale << endl;
				( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrection(fieldScale);									
				field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
				cout << "Field after:" << 	field << endl;
			}
		}
		else
		{
			if(iev == 0) 
			{			
				Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
				Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
				cout << "Field:" << field << " scale: " << scale << endl;			
				fieldScale	= -2310.0/ 3583.81 * xx[6] / scale;	//run399
				cout << "Setting field to " << 	fieldScale << endl;
				( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrection(fieldScale);						
				field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
				cout << "Field after:" << 	field << endl;
		
			}
			if(iev == 100) 
			{				
				Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
				Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
				cout << "Field:" << field << " scale: " << scale<< endl;
				fieldScale	= -2160.0/ 3583.81 * xx[6] / scale;	//run401
				cout << "Setting field to " << 	fieldScale << endl;
				( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrection(fieldScale);						
				field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
				cout << "Field after:" << 	field << endl;
			}
			if(iev == 200) 
			{				
				Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
				Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
				cout << "Field:" << field << " scale: " << scale << endl;
				fieldScale	= -2010.0/ 3583.81 * xx[6] / scale;	//run405
				cout << "Setting field to " << 	fieldScale << endl;
				( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrection(fieldScale);									
				field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
				cout << "Field after:" << 	field << endl;
			}
			if(iev == 300) 
			{				
				Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
				Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
				cout << "Field:" << field << " scale: " << scale << endl;
				fieldScale	= -1860.0/ 3583.81 * xx[6] / scale;	//run405
				cout << "Setting field to " << 	fieldScale << endl;
				( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrection(fieldScale);									
				field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
				cout << "Field after:" << 	field << endl;
			}
		}

        Double_t chi2 = gThisTask->Chi2();
        //cout << "Chi2 value: " << chi2 << endl;
        if(chi2 > 0)
        {
            chi2_all += chi2;
            nof++;
        }

    }
    chi2_all = chi2_all / nof;
    cout << "Chi2 all: " << chi2_all << endl;
    return chi2_all;
}

double R3BOptimizeGeometryS494::Chi2()
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

    R3BTrackingDetector* target = gSetup->GetByName("target");
    R3BTrackingDetector* fi23a = gSetup->GetByName("fi23a");
    R3BTrackingDetector* fi23b = gSetup->GetByName("fi23b");
    R3BTrackingDetector* tof = gSetup->GetByName("tofd");

// For the moment we have to change manually between the setup
/*
    R3BTrackingDetector* fi32 = gSetup->GetByName("fi32");
    R3BTrackingDetector* fi30 = gSetup->GetByName("fi30");
    R3BTrackingDetector* fi31 = fDetectorsRight->GetByName("fi31");
    R3BTrackingDetector* fi33 = fDetectorsRight->GetByName("fi33");
*/

    R3BTrackingDetector* fi32 = fDetectorsLeft->GetByName("fi32");
    R3BTrackingDetector* fi30 = fDetectorsLeft->GetByName("fi30");
    R3BTrackingDetector* fi31 = gSetup->GetByName("fi31");
    R3BTrackingDetector* fi33 = gSetup->GetByName("fi33");



    if(target->hits.size() < 1) target->hits.push_back(new R3BHit(0, 0., 0., 0., 0., 0));

	Double_t beta = 0.;
	Double_t x0 = 0.;
	Double_t y0 = 0.;
	Double_t z0 = 0.;
	Double_t px0 = 0;
	Double_t py0 = 0.;
	Double_t pz0 = 0.;
	Double_t p = 0.;
	Double_t mass = 0.;
	
    
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

    // try to fit all possible combination of hits. 
    
    fPropagator->SetVis(kFALSE);

    Int_t nCand = 0;
	Double_t mChi2 = -1;
	Double_t pChi2 = -1;
    Int_t ifi23a = 0;
    Int_t ifi23b = 0;
    Int_t ifi32 = 0;
    Int_t ifi31 = 0;
    Int_t ifi30 = 0;
    Int_t ifi33 = 0;
    Int_t itof = 0;
    if (0 == fi23a->hits.size())
        ifi23a = -1;
    if (0 == fi23b->hits.size())
        ifi23b = -1;
    if (0 == fi32->hits.size())
        ifi32 = -1;
    if (0 == fi31->hits.size())
        ifi31 = -1;
    if (0 == fi30->hits.size())
        ifi30 = -1;
    if (0 == fi33->hits.size())
        ifi33 = -1;
    if (0 == tof->hits.size())
        itof = -1;

    
    Int_t charge = 0;
    Double_t Charge =0;
	Int_t charge_requested = 8;
	//Int_t charge_requested = 6;
	Double_t m0 = 0.;
	Double_t p0 = 0.;

	
    // Loop over all combinations of hits
	for(Int_t i = 0; i < tof->hits.size(); i++) // loop over all ToFD hits
	//do // tof wall 
	{

		//cout << "Eloss: " << sqrt(tof->hits.at(i)->GetEloss()) * 26.76 << endl;
		if (fSimu)
		{
			// For tracking of simulations:
			charge = sqrt(tof->hits.at(i)->GetEloss()) * 26.76 + 0.5;
			Charge = sqrt(tof->hits.at(i)->GetEloss()) * 26.76;
		}
		else
		{
			// For tracking of exp. data:
			charge = tof->hits.at(i)->GetEloss();
			Charge = tof->hits.at(i)->GetEloss();
		}

		if (charge != charge_requested)
			continue;
		
		//cout << "Charge: " << charge << " requested charge: " << charge_requested << endl;

		Double_t beta0 = 0.7593; // velocity could eventually be calculated from ToF
		tof->res_t = 0.03;
		// Double_t m0 = charge * 2. * 0.931494028; // First guess of mass

		// Masse wird nicht getreckt, nur momentum
		if (charge == 8)
		{
			m0 = 15.0124; // has to have the same value as what geant uses
			p0 = 17.3915; // in GeV/c2
		}
		if (charge == 6)
		{
			m0 = 11.1749;
			p0 = 13.043625;
		}
		if (charge == 2)
		{
			m0 = 3.7284;
			p0 = 4.347875;
		}

		//cout << "Mass: " << m0 << endl;
		//cout << "Position on TofD: " << tof->hits.at(i)->GetX() << endl;
		if(tof->hits.at(i)->GetX() > 0 && fi30->hits.size() > 0 && fi32->hits.size() > 0){
			// left branch in beam direction, don't consider hits in the detectors of the other side 
			do // fi30
			{
				do // fi32
				{
					do //fi23b
					{
						do // fi23a
						{
							// Create object for particle which will be fitted
							R3BTrackingParticle* candidate = new R3BTrackingParticle(
								charge, 0., 0., 0., 0., 0., p0, beta0, m0); // 17.39

							//cout << "left side of setup" << endl;
							//cout << "Hit Tofd # " << i << " x: " << tof->hits.at(i)->GetX() 
							//	<< " y: " << tof->hits.at(i)->GetY() << endl;
							//if(ifi30 > -1) cout << " fi30 # " <<  ifi30 << " x: "<< fi30->hits.at(ifi30)->GetX() << endl;
							//if(ifi32 > -1) cout << " fi32 # " <<  ifi32 << " x: "<< fi32->hits.at(ifi32)->GetX() << endl;
							//cout << "Hit target # " << " x: " << target->hits.at(0)->GetX() << endl;
					 //                               cout << "fi23b  # " << " x: " << fi23b->hits.at(0)->GetX() << endl;

							candidate->AddHit("target", 0);
							candidate->AddHit("tofd", i);
							candidate->AddHit("fi23a", ifi23a);
							candidate->AddHit("fi23b", ifi23b);
							candidate->AddHit("fi32", ifi32);
							candidate->AddHit("fi30", ifi30);

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
							nCand += 1;
							
							if (TMath::IsNaN(candidate->GetMomentum().Z()))
							{
								delete candidate;
								continue;
							}

							if (10 > status)
							{
								if(forward)
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
									//cout << "chi2: " << candidate->GetChi2() << endl;
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

					ifi32 += 1;
				} while (ifi32 < fi32->hits.size());
				ifi32 = 0;
				if (0 == fi32->hits.size())
					ifi32 = -1;

				ifi30 += 1;
			} while (ifi30 < fi30->hits.size());
			ifi30 = 0;
			if (0 == fi30->hits.size())
				ifi30 = -1;

		} // end if left branch
		if(tof->hits.at(i)->GetX() < 0 && fi31->hits.size() > 0 && fi33->hits.size() > 0){
			// right branch in beam direction, don't consider hits in the detectors of the other side 
			do // fi33
			{
				if (ifi33 >= 0)
					fh_eloss_fi33_mc->Fill(1000. * fi33->hits.at(ifi33)->GetEloss()); // MeV		
				do // fi31
				{
					if (ifi31 >= 0)
						fh_eloss_fi31_mc->Fill(1000. * fi31->hits.at(ifi31)->GetEloss()); // MeV
					do // fi23b
					{
						if (ifi23b >= 0)
							fh_eloss_fi23b_mc->Fill(1000. * fi23b->hits.at(ifi23b)->GetEloss()); // MeV

						do // fi23a
						{
							if (ifi23a >= 0)
								fh_eloss_fi23a_mc->Fill(1000. * fi23a->hits.at(ifi23a)->GetEloss()); // MeV

							// Create object for particle which will be fitted
							R3BTrackingParticle* candidate = new R3BTrackingParticle(
								charge, 0., 0., 0., 0., 0., p0, beta0, m0); // 17.39
							
							//cout << "right side of setup" << endl;
							//cout << "Hit Tofd # " << i << " x: " << tof->hits.at(i)->GetX() 
							//	<< " y: " << tof->hits.at(i)->GetY() << endl;
							//if(ifi33 > -1) cout << "fi33 # " <<  ifi33 << " x: "<< fi33->hits.at(ifi33)->GetX() << endl;
							//if(ifi31 > -1) cout << "fi31 # " <<  ifi31 << " x: "<< fi31->hits.at(ifi31)->GetX() << endl;
							//cout << "Hit target # " << " x: " << target->hits.at(0)->GetX() << endl;

							candidate->AddHit("target", 0);
							candidate->AddHit("tofd", i);
							candidate->AddHit("fi23a", ifi23a);
							candidate->AddHit("fi23b", ifi23b);
							candidate->AddHit("fi31", ifi31);
							candidate->AddHit("fi33", ifi33);

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
							nCand += 1;
							
							if (TMath::IsNaN(candidate->GetMomentum().Z()))
							{
								delete candidate;
								continue;
							}

							if (10 > status)
							{
								if(forward)
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
									//cout << "chi2: " << candidate->GetChi2() << endl;
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

					ifi31 += 1;
				} while (ifi31 < fi31->hits.size());
				ifi31 = 0;
				if (0 == fi31->hits.size())
					ifi31 = -1;

				ifi33 += 1;
			} while (ifi33 < fi33->hits.size());
			ifi33 = 0;
			if (0 == fi33->hits.size())
				ifi33 = -1;
		} // end if right branch
		
	} // end for TofD

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
				//cout << "New min chi2: " << minChi2 << endl;
				//cout << "Corresponding Mass   : " << x->GetMass() << endl;
				//cout << "Corresponding Mass   : " << candidate->GetMass() << endl;
			}
		}

		//if(minChi2 > 1.e5) continue;
		pChi2 = 10.*(candidate->GetStartMomentum().Mag() - p0) * (candidate->GetStartMomentum().Mag() - p0) / 
		(p0*p0*0.001*0.001);		
		mChi2 = candidate->GetChi2();
		mChi2 = mChi2 + pChi2;
		//mChi2 = sqrt(candidate->GetChi2() * candidate->GetChi2() + pChi2 * pChi2);
		//mChi2 = pChi2;
		//cout << "pchi: " << pChi2 << " mchi " << candidate->GetChi2() << " total " << mChi2 << endl;
		LOG(INFO) << "Results after tracking mass:";
		LOG(INFO) << "Position x: " << candidate->GetStartPosition().X() << 
			" y: " << candidate->GetStartPosition().Y() << " z: " << candidate->GetStartPosition().Z();
		//cout << "Momentum : " << candidate->GetStartMomentum().Mag() << " px : " << candidate->GetStartMomentum().X() << 
		//	" py: " << candidate->GetStartMomentum().Y() << " pz: " << candidate->GetStartMomentum().Z() << endl;
		LOG(INFO) << "Mass   : " << candidate->GetMass();
		LOG(INFO) << "Beta   : " << candidate->GetStartBeta();
		LOG(INFO) << "chi2: " << candidate->GetChi2() << endl;
		LOG(INFO) << "mchi2: " << mChi2 << endl;
		
		fh_residuals->Fill(candidate->GetStartMomentum().Mag() - p0);
		
			
	}// end if(fFragments.size() > 0
		
    delete particle;

    if(mChi2 < 10000.)
    {
        return mChi2;
    }
    return  -1.;

}

void R3BOptimizeGeometryS494::Finish()
{
	cout << "Total chi2 for mass: " << totalChi2Mass << endl;
	cout << "Total chi2 for momentum: " << totalChi2P << endl;
    // Optimize global Chi2
    
    // Place here Minuit part for
    // minimization of Chi2AllEvents()

    //ROOT::Math::Minimizer* mini = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
    //ROOT::Math::Minimizer* mini = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Scan");
    ROOT::Math::Minimizer* mini = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Simplex");
    //Minimum = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
	// Migrad, Simplex, Scan, 
    // set tolerance , etc...
    mini->SetMaxFunctionCalls(10000); // for Minuit/Minuit2
    mini->SetMaxIterations(1000);      // for GSL
    mini->SetTolerance(10.);
    mini->SetPrintLevel(1);

    // create funciton wrapper for minmizer
    // a IMultiGenFunction type
    ROOT::Math::Functor fm1(&Chi2AllEvents, 9);
	if(fLeft)
	{
		// 12C und 4He from simulation
		//Double_t variable[9] = {-62.9, 570.8, -82.8,  632.5,  -14.0, -14.,  1.0,  0.0 , 91.2}; //chi=329.8
		
		//Double_t variable[9] = {-62.09, 571.4, -82.42,  633.2,  -14.14, -13.91, 1.016, 0.0, 91.2}; //chi=69.8 scan
		//Double_t variable[9] = {-61.97496, 571.9778, -82.27509,  633.5024,  -14.07784, -13.76490, 1.008, 0.0, 91.2}; //chi=23.35 mit p
		Double_t variable[9] = {-61.55449, 572.4289, -81.88424,  634.2852,  -13.71719, -13.60719, 1.006815, 0.0, 91.2}; //chi=23.35 mit p
		

		//Double_t step[9] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.02, 0.1, 0.1}; 
		//Double_t step[9] = {0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.01, 0.02, 0.02}; 
		Double_t step[9] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.01, 0.01, 0.01}; 
		//Double_t step[9] = {0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.005, 0.005, 0.005}; 
		//Double_t step[9] = {0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.001, 0.001, 0.001}; 

		mini->SetFunction(fm1);

		// Set the free variables to be minimized!
		mini->SetLimitedVariable(0, "x30", variable[0], step[0], variable[0] - 0.5, variable[0] + 0.5);
		mini->SetLimitedVariable(1, "z30", variable[1], step[1], variable[1] - 0.5, variable[1] + 0.5);
		mini->SetLimitedVariable(2, "x32", variable[2], step[2], variable[2] - 0.5, variable[2] + 0.5);
		mini->SetLimitedVariable(3, "z32", variable[3], step[3], variable[3] - 0.5, variable[3] + 0.5);
		mini->SetLimitedVariable(4, "dangle30", variable[4], step[4], variable[4] - 0.5, variable[4] + 0.5);
		mini->SetLimitedVariable(5, "dangle32", variable[5], step[5], variable[5] - 0.5, variable[5] + 0.5);
		mini->SetLimitedVariable(6, "dmag", variable[6], step[6], variable[6] - 0.1, variable[6] + 0.1);
		mini->SetLimitedVariable(7, "x23a", variable[7], step[7], variable[7] - 0.1, variable[7] + 0.1);
		mini->SetLimitedVariable(8, "z23a", variable[8], step[8], variable[8] - 0.1, variable[8] + 0.1);

	}
	else
	{
		Double_t variable[9] = { -111.1,  537.8,  -131.1,  599.6,  -194.,  -194.,  1.,  0.,  91.2}; // chi=7.534
		
		//Double_t variable[9] = { -110.8,  537.9,  -131.1,  599.5,  -193.9,  -193.8,  1.007,  0.,  91.2}; // chi=110.7 scan  
		
		//best Double_t variable[9] = { -110.7436,  538.7448,  -130.7746,  599.6543,  -192.9494,  -193.3667,  1.007038,  0.0,  91.2}; // chi=56.64 mit p  
		//Double_t variable[9] = { -110.7436,  536.7448,  -130.7746,  597.6543,  -192.9494,  -193.3667,  1.007038,  0.0,  91.2}; // chi=56.64 mit p  

		//Double_t variable[9] = { -110.8116,  540.6786,  -130.7119,  601.8127,  -192.8380,  -193.2083,  0.9978,  0.0,  91.2}; // chi=56.64 mit p  

		//Double_t step[9] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.02, 0.1, 0.1}; 
		Double_t step[9] = {0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.02, 0.02, 0.02}; 
		//Double_t step[9] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.01, 0.01, 0.01}; 
		//Double_t step[9] = {0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.005, 0.005, 0.005}; 
		//Double_t step[9] = {0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.001, 0.001, 0.001}; 
		//Double_t step[9] = {0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.0001, 0.0001, 0.0001}; 

		mini->SetFunction(fm1);
		
		// Set the free variables to be minimized!
		mini->SetLimitedVariable(0, "x31", variable[0], step[0], variable[0] - 1.5, variable[0] + 1.5);
		mini->SetLimitedVariable(1, "z31", variable[1], step[1], variable[1] - 1.5, variable[1] + 1.5);
		mini->SetLimitedVariable(2, "x33", variable[2], step[2], variable[2] - 1.5, variable[2] + 1.5);
		mini->SetLimitedVariable(3, "z33", variable[3], step[3], variable[3] - 1.5, variable[3] + 1.5);
		mini->SetLimitedVariable(4, "dangle31", variable[4], step[4], variable[4] - 1.5, variable[4] + 1.5);
		mini->SetLimitedVariable(5, "dangle33", variable[5], step[5], variable[5] - 1.5, variable[5] + 1.5);
		mini->SetLimitedVariable(6, "dmag", variable[6], step[6], variable[6] - 0.1, variable[6] + 0.1);
		mini->SetLimitedVariable(7, "x23a", variable[7], step[7], variable[7] - 0.0001, variable[7] + 0.0001);
		mini->SetLimitedVariable(8, "z23a", variable[8], step[8], variable[8] - 0.0001, variable[8] + 0.0001);
	}
    Int_t status = 0;

    // do the minimization
    mini->Minimize();

    status = mini->Status();
    cout << "Status: " << status << endl;
	cout << "optimized values: " << endl;
	cout.precision(7);
	if(fLeft)
	{
		cout << "dx30: " << mini->X()[0] << " dz30 " << mini->X()[1] << endl;
		cout << "dx32: " << mini->X()[2] << " dz32 " << mini->X()[3] << endl;
		cout << "dangle30: " << mini->X()[4] << " dangle32: " << mini->X()[5] << endl; 
		cout << "mag field: " << mini->X()[6] << endl;
		cout << "dx23a: " << mini->X()[7] << " dz23a " << mini->X()[8] << endl;
	}	
	else
	{
		cout << "dx31: " << mini->X()[0] << " dz31 " << mini->X()[1] << endl;
		cout << "dx33: " << mini->X()[2] << " dz33 " << mini->X()[3] << endl;
		cout << "dangle31: " << mini->X()[4] << " dangle33: " << mini->X()[5] << endl; 
		cout << "mag field: " << mini->X()[6] << endl;
		cout << "dx23a: " << mini->X()[7] << " dz23a " << mini->X()[8] << endl;
	}
    mini->Clear();
    

    	
    if (fVis)
    {
        for (auto const& det : fDetectors->GetArray())
        {
            det->Draw();
        }

        TCanvas* c11 = new TCanvas("c11", "", 50, 50, 400, 400);
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

        new TCanvas("c2", "", 50, 50, 400, 400);
        fh_eloss_fi23a_mc->SetLineStyle(2);
        fh_eloss_fi23a_mc->SetLineColor(2);
        fh_eloss_fi23a_mc->Draw();
        fh_eloss_fi23a->Draw("same");

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

        LOG(INFO) << fwhm / fh_A_reco2->GetMean();

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

Bool_t R3BOptimizeGeometryS494::InitPropagator()
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

ClassImp(R3BOptimizeGeometryS494)
