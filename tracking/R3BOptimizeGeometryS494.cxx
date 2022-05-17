/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum f�r Schwerionenforschung GmbH    *
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
namespace GlobalChi2
{
    R3BTrackingSetup* gSetup;
    R3BOptimizeGeometryS494* gThisTask;
} // namespace GlobalChi2

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
        // if (det == DET_MAX - 1)
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
    // gSetup = fDetectorsLeft;
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
    fh_A_overZ = new TH2F("fh_A_overZ", "particle identification", 100, 0., 10., 100, 1., 3.);
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
    fFitter->Init(fPropagator, fEnergyLoss);
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
            cout << "Copy Hits: " << fi23a->events[fNEvents].size() << "  " << fi23b->events[fNEvents].size() << "  "
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
        cout << "Copy Hits: " << fi23a->events[fNEvents].size() << "  " << fi23b->events[fNEvents].size() << "  "
             << fi31->events[fNEvents].size() << "  " << fi33->events[fNEvents].size() << "  "
             << tof->events[fNEvents].size() << endl;

        cout << "Saved events: " << gThisTask->GetNEvents() << endl;

        fNEvents += 1;
    }
}

double Chi2AllEvents(const double* xx)
{

    // For the moment ...
    // Bool_t fLeft = kFALSE;
    Bool_t fLeft = kTRUE;
    Double_t chi2_all = 0;
    Int_t nof = 0;
    cout.precision(7);
    cout << "new correction: " << xx[0] << "  " << xx[1] << "  " << xx[2] << "  " << xx[3] << "  " << xx[4] << "  "
         << xx[5] << "  " << xx[6] << "  " << xx[7] << "  " << xx[8] << "  " << xx[9] << "  " << xx[10] << "  "
         << xx[11] << endl;

    ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleY(xx[7]);
    ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionZ(xx[8]);
    ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionY(xx[6]);
    //( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionX(xx[10]);
    //( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(xx[10]);
    //( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(xx[11]);

    //( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.02);
    //( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.01);
    //( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(0.03);
    //( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(0.02);

    R3BTrackingDetector* fi23a = gSetup->GetByName("fi23a");

    fi23a->pos0 = TVector3(0., 0., 0.);
    fi23a->pos1 = TVector3(5., 5., 0.);
    fi23a->pos2 = TVector3(-5., 5., 0.);

    fi23a->pos0.RotateY(0. * TMath::DegToRad());
    fi23a->pos1.RotateY(0. * TMath::DegToRad());
    fi23a->pos2.RotateY(0. * TMath::DegToRad());

    TVector3 trans3b(xx[10], 0., xx[11]);

    fi23a->pos0 += trans3b;
    fi23a->pos1 += trans3b;
    fi23a->pos2 += trans3b;
    fi23a->norm = ((fi23a->pos1 - fi23a->pos0).Cross(fi23a->pos2 - fi23a->pos0)).Unit();

    if (fLeft)
    {
        R3BTrackingDetector* fi32 = gSetup->GetByName("fi32");

        fi32->pos0 = TVector3(0., 0., 0.);
        fi32->pos1 = TVector3(25., 25., 0.);
        fi32->pos2 = TVector3(-25., 25., 0.);

        fi32->pos0.RotateY(xx[5] * TMath::DegToRad());
        fi32->pos1.RotateY(xx[5] * TMath::DegToRad());
        fi32->pos2.RotateY(xx[5] * TMath::DegToRad());

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
        // cout << "Rotation: " << xx[4] << endl;
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
        // cout << "Rotation: " << xx[5] << endl;

        TVector3 trans13(xx[2], 0., xx[3]);

        fi33->pos0 += trans13;
        fi33->pos1 += trans13;
        fi33->pos2 += trans13;
        fi33->norm = ((fi33->pos1 - fi33->pos0).Cross(fi33->pos2 - fi33->pos0)).Unit();

        /*
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
        */
    }

    for (Int_t iev = 0; iev < gThisTask->GetNEvents(); iev++)
    {
        gSetup->TakeHitsFromBuffer(iev);
        Double_t fieldScale;

        ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(xx[9]);

        /*
                if(fLeft)
                {
                    if(iev == 0)
                    //if(iev == -1)
                    {
                        Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                        Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field:" << field << " scale: " << scale << endl;
                        fieldScale	= -1560.0 / 3583.81 / scale * xx[9];	//run395
                        cout << "Setting field to " << 	fieldScale << endl;
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
           )->SetTrackerCorrectionScale(fieldScale); field = ((R3BGladFieldMap *)
           FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field after:" << 	field << endl;

                    }
                    if(iev == 1000)
                    //if(iev == -1)
                    {
                        Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                        Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field:" << field << " scale: " << scale<< endl;
                        fieldScale	= -1410.0 / 3583.81 / scale * xx[9];	//run391
                        cout << "Setting field to " << 	fieldScale << endl;
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
           )->SetTrackerCorrectionScale(fieldScale); field = ((R3BGladFieldMap *)
           FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field after:" << 	field << endl;
                    }
                    if(iev == 2000)
                    //if(iev == -1)
                    {
                        Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                        Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field:" << field << " scale: " << scale << endl;
                        fieldScale	= -1260.0 / 3583.81 / scale * xx[9];	//run389
                        cout << "Setting field to " << 	fieldScale << endl;
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
           )->SetTrackerCorrectionScale(fieldScale); field = ((R3BGladFieldMap *)
           FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field after:" << 	field << endl;
                    }
                    if(iev == 3000)
                    //if(iev == 0)
                    {
                        Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                        Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field:" << field << " scale: " << scale << endl;
                        fieldScale	= -1110.0 / 3583.81 / scale * xx[9];	//run389
                        cout << "Setting field to " << 	fieldScale << endl;
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
           )->SetTrackerCorrectionScale(fieldScale); field = ((R3BGladFieldMap *)
           FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field after:" << 	field << endl;
                    }
                }
                else
                {
                    if(iev == 0)
                    //if(iev == -1)
                    {
                        Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                        Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field:" << field << " scale: " << scale << endl;
                        fieldScale	= -2310.0/ 3583.81 / scale * xx[9]; 	//run399
                        cout << "Setting field to " << 	fieldScale << endl;
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
           )->SetTrackerCorrectionScale(fieldScale); field = ((R3BGladFieldMap *)
           FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field after:" << 	field << endl;

                    }
                    if(iev == 1000)
                    //if(iev == -1)
                    {
                        Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                        Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field:" << field << " scale: " << scale<< endl;
                        fieldScale	= -2160.0/ 3583.81 / scale * xx[9];//run401
                        cout << "Setting field to " << 	fieldScale << endl;
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
           )->SetTrackerCorrectionScale(fieldScale); field = ((R3BGladFieldMap *)
           FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field after:" << 	field << endl;
                    }
                    if(iev == 2000)
                    //if(iev == -1)
                    {
                        Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                        Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field:" << field << " scale: " << scale << endl;
                        fieldScale	= -2010.0/ 3583.81 / scale * xx[9];//run405
                        cout << "Setting field to " << 	fieldScale << endl;
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
           )->SetTrackerCorrectionScale(fieldScale); field = ((R3BGladFieldMap *)
           FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field after:" << 	field << endl;
                    }
                    if(iev == 3000)
                    //if(iev == -1)
                    {
                        Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                        Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field:" << field << " scale: " << scale << endl;
                        fieldScale	= -1860.0/ 3583.81 / scale * xx[9];//run405
                        cout << "Setting field to " << 	fieldScale << endl;
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
           )->SetTrackerCorrectionScale(fieldScale); field = ((R3BGladFieldMap *)
           FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field after:" << 	field << endl;
                    }
                }
        */

        // cout << "Test: " << gSetup->GetArray().size() << endl;

        Double_t chi2 = gThisTask->Chi2();
        // cout << "Chi2 value: " << chi2 << endl;
        if (chi2 > 0)
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
    R3BTrackingDetector* fi32 = gSetup->GetByName("fi32");
    R3BTrackingDetector* fi30 = gSetup->GetByName("fi30");
    R3BTrackingDetector* fi31 = fDetectorsRight->GetByName("fi31");
    R3BTrackingDetector* fi33 = fDetectorsRight->GetByName("fi33");

    /*
        R3BTrackingDetector* fi32 = fDetectorsLeft->GetByName("fi32");
        R3BTrackingDetector* fi30 = fDetectorsLeft->GetByName("fi30");
        R3BTrackingDetector* fi31 = gSetup->GetByName("fi31");
        R3BTrackingDetector* fi33 = gSetup->GetByName("fi33");
    */

    if (target->hits.size() < 1)
        target->hits.push_back(new R3BHit(0, 0., 0., 0., 0., 0));

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
    R3BTrackingParticle* particle = new R3BTrackingParticle(10., x0, y0, z0, px0, py0, pz0, beta, mass);

    // try to fit all possible combination of hits.

    fPropagator->SetVis(kFALSE);

    Int_t nCand = 0;

    Double_t mChi2 = -1;
    Double_t pChi2 = -1;
    Double_t xChi2 = -1;
    Double_t yChi2 = -1;
    Double_t zChi2 = -1;

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
    Double_t Charge = 0;
    // Int_t charge_requested = 8;
    Int_t charge_requested = 6;
    // Int_t charge_requested = 2;
    Double_t m0 = 0.;
    Double_t p0 = 0.;

    // Loop over all combinations of hits

    for (Int_t i = 0; i < tof->hits.size(); i++) // loop over all ToFD hits
    // do // tof wall
    {

        // cout << "Eloss: " << sqrt(tof->hits.at(i)->GetEloss()) * 26.76 << endl;
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

        // if (charge != charge_requested)
        // if (charge != 2 && charge != 6)
        if (charge != 6)
            continue;

        // cout << "Charge: " << charge << " requested charge: " << charge_requested << endl;

        Double_t beta0 = 0.7593; // velocity could eventually be calculated from ToF
        tof->res_t = 0.03;
        // Double_t m0 = charge * 2. * 0.931494028; // First guess of mass

        // Masse wird nicht getreckt, nur momentum
        if (charge == 8)
        {
            m0 = 15.0124; // has to have the same value as what geant uses
            // p0 = 17.3915; // in GeV/c2 without target
            // p0 = 17.38887; // in GeV/c2 with Pb target
            p0 = 17.38675; // in GeV/c2 with C target
        }
        if (charge == 6)
        {
            m0 = 11.17486339;
            // p0 = 13.043625; // in GeV/c2 without target
            p0 = 13.04165; // in GeV/c2 with Pb target
            // p0 = 13.04006; // in GeV/c2 with C target
        }
        if (charge == 2)
        {
            m0 = 3.7284;
            // p0 = 4.347875; // in GeV/c2 without target
            p0 = 4.34722; // in GeV/c2 with Pb target
            // p0 = 4.34669; // in GeV/c2 with C target
        }

        // cout << "Mass: " << m0 << endl;
        // cout << "Position on TofD: " << tof->hits.at(i)->GetX() << endl;
        if (tof->hits.at(i)->GetX() > 0 && fi30->hits.size() > 0 && fi32->hits.size() > 0)
        {
            // left branch in beam direction, don't consider hits in the detectors of the other side
            do // fi30
            {
                do // fi32
                {
                    do // fi23b
                    {
                        do // fi23a
                        {
                            // Create object for particle which will be fitted
                            R3BTrackingParticle* candidate =
                                new R3BTrackingParticle(charge, 0., 0., 0., 0., 0., p0, beta0, m0); // 17.39

                            // cout << "left side of setup" << endl;
                            // cout << "Hit Tofd # " << i << " x: " << tof->hits.at(i)->GetX()
                            //	<< " y: " << tof->hits.at(i)->GetY() << endl;
                            // if(ifi30 > -1) cout << " fi30 # " <<  ifi30 << " x: "<< fi30->hits.at(ifi30)->GetX() <<
                            // endl; if(ifi32 > -1) cout << " fi32 # " <<  ifi32 << " x: "<<
                            // fi32->hits.at(ifi32)->GetX()
                            // << endl; cout << "Hit target # " << " x: " << target->hits.at(0)->GetX() << endl;
                            //                               cout << "fi23b  # " << " x: " << fi23b->hits.at(0)->GetX()
                            //                               << endl;

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
                                if (forward)
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
                                    // cout << "chi2: " << candidate->GetChi2() << endl;
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
        if (tof->hits.at(i)->GetX() < 0 && fi31->hits.size() > 0 && fi33->hits.size() > 0)
        {
            // right branch in beam direction, don't consider hits in the detectors of the other side
            do // fi33
            {
                if (ifi33 >= 0)
                    fh_eloss_fi33_mc->Fill(1000. * fi33->hits.at(ifi33)->GetEloss()); // MeV
                do                                                                    // fi31
                {
                    if (ifi31 >= 0)
                        fh_eloss_fi31_mc->Fill(1000. * fi31->hits.at(ifi31)->GetEloss()); // MeV
                    do                                                                    // fi23b
                    {
                        if (ifi23b >= 0)
                            fh_eloss_fi23b_mc->Fill(1000. * fi23b->hits.at(ifi23b)->GetEloss()); // MeV

                        do // fi23a
                        {
                            if (ifi23a >= 0)
                                fh_eloss_fi23a_mc->Fill(1000. * fi23a->hits.at(ifi23a)->GetEloss()); // MeV

                            // Create object for particle which will be fitted
                            R3BTrackingParticle* candidate =
                                new R3BTrackingParticle(charge, 0., 0., 0., 0., 0., p0, beta0, m0); // 17.39

                            // cout << "right side of setup" << endl;
                            // cout << "Hit Tofd # " << i << " x: " << tof->hits.at(i)->GetX()
                            //	<< " y: " << tof->hits.at(i)->GetY() << endl;
                            // if(ifi33 > -1) cout << "fi33 # " <<  ifi33 << " x: "<< fi33->hits.at(ifi33)->GetX() <<
                            // endl; if(ifi31 > -1) cout << "fi31 # " <<  ifi31 << " x: "<< fi31->hits.at(ifi31)->GetX()
                            // << endl; cout << "Hit target # " << " x: " << target->hits.at(0)->GetX() << endl;

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
                                if (forward)
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
                                    // cout << "chi2: " << candidate->GetChi2() << endl;
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
                // cout << "New min chi2: " << minChi2 << endl;
                // cout << "Corresponding Mass   : " << x->GetMass() << endl;
                // cout << "Corresponding Mass   : " << candidate->GetMass() << endl;
            }
        }

        // if(minChi2 > 1.e5) continue;
        pChi2 = 1. * (candidate->GetStartMomentum().Mag() - p0) * (candidate->GetStartMomentum().Mag() - p0) /
                (p0 * p0 * 0.0001 * 0.0001);
        mChi2 = candidate->GetChi2();
        zChi2 = (candidate->GetStartMomentum().Z() - p0) * (candidate->GetStartMomentum().Z() - p0) /
                (p0 * p0 * 0.001 * 0.001);
        yChi2 = (candidate->GetStartMomentum().Y() - 0.) * (candidate->GetStartMomentum().Y() - 0.) / (0.01 * 0.01);
        xChi2 = (candidate->GetStartMomentum().X() - 0.) * (candidate->GetStartMomentum().X() - 0.) / (0.01 * 0.01);

        mChi2 = mChi2 + pChi2;
        // mChi2 = mChi2 + xChi2 + yChi2 + zChi2;

        // mChi2 = sqrt(candidate->GetChi2() * candidate->GetChi2() + pChi2 * pChi2);
        // mChi2 = pChi2;
        // cout << "pchi: " << pChi2 << " mchi " << candidate->GetChi2() << " total " << mChi2 << endl;
        LOG(INFO) << "Results after tracking mass:";
        LOG(INFO) << "Position x: " << candidate->GetStartPosition().X() << " y: " << candidate->GetStartPosition().Y()
                  << " z: " << candidate->GetStartPosition().Z();
        LOG(INFO) << "Momentum : " << candidate->GetStartMomentum().Mag()
                  << " px : " << candidate->GetStartMomentum().X() << " py: " << candidate->GetStartMomentum().Y()
                  << " pz: " << candidate->GetStartMomentum().Z() << endl;
        LOG(INFO) << "Mass   : " << candidate->GetMass();
        LOG(INFO) << "Beta   : " << candidate->GetStartBeta();
        LOG(INFO) << "chi2: " << candidate->GetChi2() << endl;
        LOG(INFO) << "mchi2: " << mChi2 << endl;
        LOG(INFO) << "test p0 p: " << p0 << "  " << candidate->GetStartMomentum().Mag() << endl;
        LOG(INFO) << "test pchi: " << pChi2 << endl;

        fh_residuals->Fill(candidate->GetStartMomentum().Mag() - p0);

    } // end if(fFragments.size() > 0

    delete particle;
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

    if (mChi2 < 1000.)
    {
        return mChi2;
    }
    return -1.;
}

void R3BOptimizeGeometryS494::Finish()
{
    cout << "Total chi2 for mass: " << totalChi2Mass << endl;
    cout << "Total chi2 for momentum: " << totalChi2P << endl;
    // Optimize global Chi2

    // Place here Minuit part for
    // minimization of Chi2AllEvents()

    ROOT::Math::Minimizer* mini = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
    // ROOT::Math::Minimizer* mini = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Scan");
    // ROOT::Math::Minimizer* mini = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Simplex");
    // Minimum = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
    // Migrad, Simplex, Scan,
    // set tolerance , etc...
    mini->SetMaxFunctionCalls(10000); // for Minuit/Minuit2
    mini->SetMaxIterations(1000);     // for GSL
    mini->SetTolerance(10.);
    mini->SetPrintLevel(1);

    // create funciton wrapper for minmizer
    // a IMultiGenFunction type
    ROOT::Math::Functor fm1(&Chi2AllEvents, 12);

    if (fLeft)
    {
        // 12C und 4He from simulation
        // Double_t variable[9] = {-62.8664, 570.7671, -82.8087,  632.4669,  -14., -14.,  1.0,  -14. , 177.6};
        // //chi=329.8 Double_t variable[9] = {-62.6664, 570.7671, -82.8087,  632.4669,  -14., -14.,  1.4,  -13.6 ,
        // 173.6025}; //chi=329.8 Double_t variable[10] = {-62.6664, 570.9671, -82.8087,  632.4669,  -14., -14.,  1.5,
        // -13.6 , 173.6025, 1.}; //chi=329.8 Double_t variable[11] = {-62.6664, 570.7671, -82.8087,  632.4669,  -14.,
        // -14.,  1.5,  -14. , 172.5, 1., 0.}; //chi=329.8 Double_t variable[11] = {-62.6664, 570.7671, -82.8087,
        // 632.4669,  -14., -14.,  1.625196,  -13.9750 , 172.8753, 0.997496, 0.025}; //chi=329.8 *Double_t variable[11]
        // =
        //{-62.6664, 570.7671, -82.8087,  632.4669,  -14., -14.,  1.625196,  -13.9750 , 172.8753, 0.997496, 0.025};
        ////chi=329.8 Double_t variable[11] = {-62.6664, 570.7671, -82.8087,  632.4669,  -14., -14.,  1.630196, -13.8750
        // , 172.8753, 0.997496, 0.025}; //chi=3.0188

        // Ohne Rotation des Magnetfeldes, fixed variable 10
        // Double_t variable[12] = {-62.6664, 570.7671, -82.8087,  632.4669,  -14., -14.,  1.630196,  -13.8750 ,
        // 172.8753, 0.997496, 0., 91.2}; //chi=3.0188

        // mit breakup data
        // Double_t variable[12] = {-62.6664, 570.7671, -82.7987,  632.4669,  -13.9900, -13.9500,  1.630196,  -13.8750 ,
        // 172.8753, 0.997496, 0.0, 0.}; //chi=3.0188

        // Mit x, y, z Drehung von Glad
        // Double_t variable[12] = {-62.82355, 570.8905, -82.87761,  632.5903,  -13.87659, -13.87659,  1.159864,
        // -13.8398 , 175.1890, 1.002706, 0.0201793, 0.027073}; // Double_t variable[12] = {-62.80855, 571.3955,
        // -82.86261,  632.5953,  -13.87159, -13.87159,  1.160364,  -13.83930 , 173.0050, 0.9987061, 0.02067930,
        // 0.027573}; // mit echten Daten Double_t variable[12] = {-62.82293, 571.4378, -82.78851,  632.6376, -13.82937,
        // -13.84923,  1.200,  -13.79943 , 173.1065, 0.9966701, 0.024889, 0.03178}; // Double_t variable[12] =
        // {-62.81546, 571.4552, -82.78104,  632.6950,  -13.82190, -13.84176,  1.200747,  -13.79196 , 173.1072,
        // 0.9981290, 0.0053298, 91.05174}; //chi=2.80829
        // jetzt gleiche Paramter für Magnetfeld links und rechts
        // Double_t variable[12] = {-62.72401, 571.4876, -82.64887,  633.0008,  -13.63535, -13.61740,  1.223193,
        // -13.40954 , 171.0334, 0.99, 0.0054, 91.052}; //chi=2.520121 Double_t variable[12] = {-62.72401, 571.4876,
        // -82.64887,  633.0008,  -13.63535, -13.61740,  1.223193,  -13.40954 , 171.0334, 0.99, 0.0054, 91.052};
        // //chi=2.520121 Double_t variable[12] = {-62.15724, 572.2269, -82.12618,  633.9725,  -13.00364,
        // -11.43203,  1.5,  -13.5 , 173.5, 0.99, 0.0054, 91.052}; //chi=2.277084679
        //**Double_t variable[12] = {-62.15502, 572.2391, -82.12785,  633.9731,  -13.00142, -11.42981,  1.3,  -13.5 ,
        // 173.7, 0.991, 0.0054, 91.052}; //chi=2.277084679 Double_t variable[12] = {-62.15502, 572.2391, -82.12785,
        // 633.9731,  -13.00142, -11.42981,  1.3,  -13.5 , 173.7, 0.991, 0.0054, 91.052}; //chi=2.277084679

        // Double_t variable[12] = {-62.8664, 570.7671, -82.8087,  632.4669,  -14., -14., 1.87, -13.46 , 177.36, 1.013,
        // 0.0052, 91.06}; //chi=329.8

        // Double_t variable[12] = {-62.8664, 570.7671, -82.8087,  632.4669,  -14., -14., 1.5,   -14. , 175.2631, 1.0,
        // 0, 91.06}; //chi=195.72315 Double_t variable[12] = {-62.68456, 571.0268, -82.78089,  632.6474,  -14.20422,
        // -14.02416, 1.9,   -14.02416 , 175.2631, 1.0, 0, 91.06}; //chi=

        // Double_t variable[12] = {-62.61640, 570.7671, -82.6087,  632.4669,  -14.0000, -14.0000, 1.6252,   -13.975 ,
        // 172.87530, 0.9975, 0, 91.06}; //chi=5.78805 Double_t variable[12] = {-62.6039, 570.7796, -82.70868, 632.4794,
        // -13.98750, -13.9875, 1.62645,   -13.96250 , 174.8766, 1.0012, 0.00125, 91.06125}; //chi=4.691445300 Double_t
        // variable[12] = {-62.58520, 570.8430, -82.70848,  632.5428,  -13.92333, -13.92333, 1.690075,   -13.94990 ,
        // 173.9708, 1.001513, 0.000037787, 91.19050}; //chi=76.58731558 Double_t variable[12] = {-62.58520, 570.8430,
        // -82.70848,  632.5428,  -13.92333, -13.92333, 1.690075,   -13.94990 , 173.9708, 1.001513,
        // 0.000037787, 91.29050}; //chi=73.0314937255
        Double_t variable[12] = {
            -62.58520, 570.8430,  -82.70848, 632.5428, -13.92333,   -13.92333,
            1.690075,  -13.94990, 175.9708,  1.001513, 0.000037787, 91.29050
        }; // chi=73.0314937255

        // richtiger Abstand von Magnet
        // Double_t variable[11] = {-62.64088, 570.9361, -82.77324,  632.6359,  -13.83102, -13.83102,  1.05,  -14.0 ,
        // 174.950, 1.0, 0.}; //chi=5.55378 Double_t variable[11] = {-62.15025, 571.2975, -82.28261,  632.7194,
        // -13.62326, -13.43957,  1.05,  -14.0 , 174.950, 1.0, 0.}; //chi=5.55378 Double_t variable[11] = {-62.16052,
        // 571.3229, -82.26855,  632.7448,  -13.59789, -13.41420,  1.074675,  -13.98766 , 175.0117, 0.999948, 0.};
        // //chi=5.55378

        // einzelne runs fitten:
        // Double_t variable[11] = {-62.8664, 570.7671, -82.8087,  632.4669,  -14., -14.,  1.07,  -14. , 174.950, 1.0,
        // 0.}; //chi= Double_t variable[11] = {-62.7, 570.7671, -83.0,  632.4669,  -14., -14.,  1.07,  -14. ,
        // 174.950, 1.0, 0.}; //chi= Double_t variable[11] = {-62.7, 570.7671, -83.0,  632.4669,  -14., -14.,  1.07,
        // -14. , 174.950, 1.01, 0.}; //chi=

        // both sides individually
        // Double_t variable[11] = {-62.69650, 570.7806, -83.00851,  632.4804,  -13.98649, -13.98649,  1.083501,
        // -13.98650 , 174.9635, 1.009699, 0.}; //chi= Double_t variable[11] = {-62.69650, 570.7806, -83.00851,
        // 632.4804, -13.98649, -13.98649,  1.083501,  -13.98650 , 174.9635, 1.009699, 0.}; //chi= Double_t variable[11]
        // =
        // {-62.69650, 570.7806, -83.00851,  632.4804,  -13.98649, -13.93649,  1.083501,  -13.98650 ,
        // 174.9635, 1.009699, 0.}; //chi=16.387 Double_t variable[11] = {-62.69650, 570.7806, -83.00851,  632.4804,
        // -13.98649, -13.93649,  1.083501,  -13.98650 , 174.9635, 1.009699, 0.}; //chi= anschliesend linke und rechte
        // Seite zusammen Double_t variable[11] = {-62.70, 570.80, -83.00,  632.50,  -13.95, -13.90,  1.1,  -13.90 ,
        // 174.9635, 1.009, 0.}; //chi= Double_t variable[11] = {-62.70, 570.80, -83.00,  632.50,  -13.95, -13.90,  1.1,
        // -13.80 , 174.9635, 1.009, 0.}; //chi=5.477 Double_t variable[11] = {-62.70, 570.80, -83.00,  632.50,  -13.94,
        // -13.90,  1.205,  -13.80 , 174.9635, 1.009, 0.}; //chi=5.395

        // together with oter side
        // Double_t variable[11] = {-62.70, 570.80, -83.00,  632.50,  -13.94, -13.90,  1.205,  -13.80 , 174.9635, 1.009,
        // 0.}; //chi=5.395

        // chi2 nur auf den Gesamtimpuls p0 = 17.3915*1.01
        // Double_t variable[11] = {-62.6910, 570.8190, -82.97099,  632.5190,  -13.92099, -13.88099,  1.217251,
        // -13.78100 , 174.9758, 1.016998, 0.}; //chi=2.9125524

        // chi2 nur auf den Gesamtimpuls p0 = 17.3915*1.0
        // Double_t variable[11] = {-62.64873, 570.8673, -82.93272,  632.5673, -13.82272, -13.63265,  1.249078,
        // -13.73465, 175.0103, 1.005960, 0.}; //chi=2.76379

        // chi2 nur auf den Gesamtimpuls p0 = 17.3915*0.99
        // Double_t variable[11] = {-62.60059, 570.9154, -82.97283,  632.8154, -13.82458, -13.58451,  1.297090,
        // -13.71065, 175.1315, 0.9963587, 0.}; //chi=3.51779127

        Double_t step[12] = { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.1, 1.5, 0.01, 0.1, 0.1 };
        // Double_t step[12] = {0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.1, 0.5, 0.01, 0.1, 0.1};
        // Double_t step[12] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.01, 0.01};
        // Double_t step[12] = {0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.005, 0.05, 0.005, 0.005, 0.005, 0.005};
        // Double_t step[12] = {0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.001, 0.001, 0.01, 0.001, 0.001, 0.001};
        // Double_t step[12] = {0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.5, 0.02, 0.02, 0.02};
        // Double_t step[12] = {0.2, 0.5, 0.2, 0.5, 0.2, 0.2, 0.5, 0.5, 0.5, 0.01, 0.05, 0.05};

        mini->SetFunction(fm1);

        // Set the free variables to be minimized!
        mini->SetLimitedVariable(0, "x30", variable[0], step[0], variable[0] - 2.5, variable[0] + 2.5);
        mini->SetLimitedVariable(1, "z30", variable[1], step[1], variable[1] - 2.5, variable[1] + 2.5);
        mini->SetLimitedVariable(2, "x32", variable[2], step[2], variable[2] - 2.5, variable[2] + 2.5);
        mini->SetLimitedVariable(3, "z32", variable[3], step[3], variable[3] - 2.5, variable[3] + 2.5);
        mini->SetLimitedVariable(4, "dangle30", variable[4], step[4], variable[4] - 1.5, variable[4] + 1.5);
        mini->SetLimitedVariable(5, "dangle32", variable[5], step[5], variable[5] - 1.5, variable[5] + 1.5);
        mini->SetLimitedVariable(6, "ypos", variable[6], step[6], variable[6] - 2., variable[6] + 2.);
        mini->SetLimitedVariable(7, "angleY", variable[7], step[7], variable[7] - 2., variable[7] + 2.);
        mini->SetLimitedVariable(8, "zpos", variable[8], step[8], variable[8] - 10., variable[8] + 10.);
        mini->SetLimitedVariable(9, "field", variable[9], step[9], variable[9] - 0.1, variable[9] + 0.1);
        mini->SetLimitedVariable(10, "angleX", variable[10], step[10], variable[10] - 0.5, variable[10] + 0.5);
        mini->SetLimitedVariable(11, "angleZ", variable[11], step[11], variable[11] - 0.5, variable[11] + 0.5);
        // mini->FixVariable(0);
        // mini->FixVariable(1);
        // mini->FixVariable(2);
        // mini->FixVariable(3);
        // mini->FixVariable(4);
        // mini->FixVariable(5);
        // mini->FixVariable(6);
        // mini->FixVariable(7);
        // mini->FixVariable(8);
        // mini->FixVariable(9);
        // mini->FixVariable(10);
        // mini->FixVariable(11);
    }
    else
    {
        // Double_t variable[9] = { -111.1057,  537.8183,  -131.0650,  599.5703,  -194.,  -194.,  1.994056,   -13.62947
        // , 175.2631}; // chi=7.534 Double_t variable[9] = { -111.1057,  537.8183,  -131.0650,  599.5703,  -194.,
        // -194., 1.994056,   -13.62947 , 176.7631}; // chi=7.534 Double_t variable[9] = { -111.1057,  537.8183,
        // -131.0650, 599.5703,  -194.,  -194., 1.927387, -13.56280, 177.8298}; // chi=7.534 Double_t variable[9] = {
        // -111.1057, 537.8183,  -131.0650,  599.5703,  -194.,  -194., 1.4,  -13.6 , 173.6025}; // chi=7.534 Double_t
        // variable[10] = { -111.2816,  537.8088,  -130.9717,  599.6506,  -193.9370,  -193.9600, 0.8032,  -14.19709 ,
        // 176.9028, 1.}; // chi=7.534 Double_t variable[10] = { -111.2476,  537.8921,  -131.0065,  599.5336, -193.8243,
        // -193.8473, 0.9147, -14.13736, 177.1437, 0.9962}; // chi=7.534 Double_t variable[11] = { -111.1057,  537.8183,
        // -131.0650, 599.5703,  -194.,  -194., 1.000, -14., 172.5, 1.0, 0.}; // chi=7.534 Double_t variable[11] = {
        // -111.1057, 537.8183,  -131.0650,  599.5703,  -194.,  -194., 2.041127, -13.88482, 178.7732, 1.002188,
        // -0.3535}; // chi=7.534 *Double_t variable[11] = { -111.1057,  537.8183,  -131.0650,  599.5703,  -194.,
        // -194., 2.041127,
        //-13.88482, 178.7732, 1.002188, -0.3535}; // chi=7.534 Double_t variable[11] = { -111.1257,  537.8731,
        // -131.0910,  599.6022,  -193.8672,  -193.9172, 2.122505, -13.86857, 179.0529, 1.001629, -0.3535}; //
        // chi=2.547957

        // Ohne Rotation des Magnetfeldes, fixed variable 10
        // Double_t variable[11] = { -111.1257,  537.8731,  -131.0910,  599.6022,  -193.8672,  -193.9172, 2.122505,
        // -13.86857, 179.0529, 1.001629, 0.}; // chi=2.547957

        // Jetzt mit breakup Daten und Fokus auf 4He
        // Double_t variable[12] = { -111.2143,  537.8894,  -131.0247,  599.6185,  -193.8517,  -193.9009, 2.124099,
        // -13.85344, 179.0557, 1.003588, 0.00009375, 91.18513}; // chi=5.77

        // Mit x, y, z Drehung von Glad
        // Double_t variable[12] = { -111.0853,  537.8589,  -130.9761,  599.7052,  -193.8651,  -193.8651,  0.9016831,
        // -13.77554 , 177.2066, 0.9955284, 0.00538, 0.01569}; // chi=7.534 Double_t variable[12] = { -111.2014,
        // 537.8158,  -130.7929,  599.9453,  -193.7693,  -194.3303,  0.8279273,  -13.38015 , 173.6426, 0.9821933,
        // 0.01777889, 0.03423889}; // chi=2.16326
        // Jetzt mit echten Daten
        // Double_t variable[12] = { -111.1967,  537.8205,  -130.7382,  599.9500,  -193.7546,  -194.3256,  0.8284064,
        // -13.27686 , 173.6445, 0.9844438, 0.005479167, 91.05247}; // chi=2.4988
        // Jetzt gleiche Parameter für Magnetfeld für links und rechts
        // Double_t variable[12] = { -111.3371,  537.7993,  -130.9237,  599.8100,  -193.9877,  -193.9409,  0.8808368,
        // -12.89503 , 174.7896, 0.99, 0.0054, 91.052}; // chi=2.43289 Double_t variable[12] = { -111.3371,  537.7993,
        // -130.9237,  599.8100,  -193.9877,  -193.9409,  0.8808368,  -12.89503 , 174.7896, 0.994, 0.0054, 91.052}; //
        // chi=2.43289 Double_t variable[12] = { -111.5882,  537.3350,  -131.3205,  599.7711,  -193.7239,
        // -192.6849,  1.5,  -13.50 , 173.5, 0.99, 0.0054, 91.052}; // chi=2.23704168
        //**Double_t variable[12] = { -111.5849,  537.3483,  -131.3305,  599.7744,  -193.7206,  -192.6716,  1.3,  -13.50
        //, 173.7, 0.991, 0.0054, 91.052}; // chi=2.23704168 Double_t variable[12] = { -111.5849,  537.3483,  -131.3305,
        // 599.7744,  -193.2206,  -192.6716,  1.3,  -13.50 , 173.7, 0.991, 0.0054, 91.052}; // chi=2.23704168

        // Double_t variable[12] = { -111.1057,  537.8183,  -131.0650,  599.5703,  -194.,  -194.,  1.87, -13.46 ,
        // 177.36, 1.013, 0.0052, 91.06}; // chi=7.534

        // Double_t variable[12] = { -111.1057,  537.8183,  -131.0650,  599.5703,  -194.,  -194.,  1.5,   -14. ,
        // 175.2631, 1.0, 0, 91.06}; // chi=123.16 Double_t variable[12] = { -110.2538,  537.4911,  -130.2718, 599.9729,
        // -194.3020,  -193.6182,  1.5,   -14. , 175.2631, 1.0, 0, 91.06}; // chi=121.0383 Double_t variable[12] = {
        // -110.2538,  537.4911,  -130.2318,  599.9729,  -194.2820,  -193.5782,  1.975152,   -14.35 , 175.2631, 1.0,
        // 0, 91.11}; // chi=121.0383
        Double_t variable[12] = { -111.1056, 537.81830, -131.065, 599.57030, -194.0000,  -194.000,
                                  2.04113,   -13.88482, 178.7732, 1.00219,   0.01460567, 91.12461 }; // chi=

        // Double_t variable[11] = { -111.1057,  537.8183,  -131.0650,  599.5703,  -194.,  -194., 1.0, -14., 172.5, 1.,
        // 0.}; // chi=7.534 Double_t variable[11] = { -111.4118,  537.9929,  -130.8408,  599.8507,  -193.4780,
        // -193.8677, 1.0, -14., 172.5, 0.9797757, 0.}; // chi=7.534 Double_t variable[11] = { -111.4118,  537.9929,
        // -130.8408,  599.8507,  -193.4780,  -193.8677, 1.0, -14., 172.5, 0.9797757, 0.}; // chi=7.534 Double_t
        // variable[11] = { -111.4118,  537.9929,  -130.8408,  599.8507,  -193.4780,  -193.8677, 1.05, -14., 172.5,
        // 0.9797757, 0.}; // chi=7.534 Double_t variable[11] = { -111.4118,  537.9929,  -130.8408,  599.8507,
        // -193.4280, -193.8677, 1.05, -14., 172.5, 0.9797757, 0.}; // chi=7.534

        // richtiger Abstand Magnet
        // Double_t variable[11] = { -111.4118,  537.9929,  -130.8408,  599.8507,  -194.,  -194., 1.0, -14., 174.95, 1.,
        // 0.}; // chi=5.1532138 Double_t variable[11] = { -112.1990,  538.2636,  -132.3246,  600.9256,  -193.9895,
        // -193.4368, 1.0, -14., 174.95, 1., 0.}; // chi=5.1532138 Double_t variable[11] = { -112.2356,  538.2684,
        // -132.3929,  600.9053,  -194.0040,  -193.3735, 1.074349, -13.95020, 175.2253, 0.9971670, 0.}; // chi=5.1532138

        // einzelne runs fitten
        // Double_t variable[11] = { -111.1057,  537.8183,  -131.0650,  599.5703,  -194.,  -194.,  1.07,   -14. ,
        // 174.95, 1., 0.}; // chi=7.534 Double_t variable[11] = { -111.2,  537.8183,  -130.85,  599.5703,  -194.,
        // -194., 1.07,   -14. , 174.95, 1., 0.}; // chi=7.534 Double_t variable[11] = { -111.2133,  537.8370,
        // -130.8812, 599.5432,  -193.9240,  -193.9240,  1.079929,   -14.04407 , 175.0259, 0.9883258, 0.}; // chi=7.534
        // Double_t variable[11] = { -111.2133,  537.8370,  -130.7812,  599.5432,  -193.9240,  -193.9240,  1.079929,
        // -14.04407 , 175.0259, 0.9883258, 0.}; // chi=7.534 anschliesend linke und rechte Seite zusammen Double_t
        // variable[11] = { -111.2133,  537.8370,  -130.7812,  599.5432,  -193.9240,  -193.9240,  1.579929, -14.04407 ,
        // 175.0259, 0.9883258, 0.}; // chi=12.4501 Double_t variable[11] = { -111.1806,  537.8837, -130.7788, 599.5708,
        // -193.7783,  -193.7783,  1.72047,   -14.00583 , 175.4381, 0.9894962, 0.}; // chi=5.100 Double_t variable[11] =
        // { -111.1806,  537.8837,  -130.7788,  599.5708,  -193.6783,  -193.7783,  1.72047, -14.00583 , 175.4381,
        // 0.9894962, 0.}; // chi=4,902 Double_t variable[11] = { -111.1806,  537.8837,  -130.7788, 599.5708, -193.6783,
        // -193.7783,  1.72047,   -14.00583 , 175.4381, 0.9894962, 0.}; // chi=4,902

        // chi2 nur auf den Gesamtimpuls p0 = 17.3915*1.01
        // Double_t variable[11] = { -111.1728,  537.8965,  -130.7660,  599.6336,  -193.5655,  -193.7655,  1.721745,
        // -13.99308 , 175.4394, 0.9972455, 0.}; // chi=2.238658

        // chi2 nur auf den Gesamtimpuls p0 = 17.3915*1.0
        // Double_t variable[11] = { -111.1519,  537.9124,  -130.7511,  599.6495,  -193.4496,  -193.7496,  1.742336,
        // -13.97821 , 175.4601, 0.9879454, 0.}; // chi=2.0697

        // together with oter side
        // Double_t variable[11] = { -111.1806,  537.8837,  -130.7788,  599.5708,  -193.6783,  -193.7783,  1.205, -13.80
        // , 174.9635, 1.009, 0.}; // chi=58.6125 Double_t variable[11] = { -111.0986,  538.0398,  -130.8728,  599.4673,
        // -193.5222,  -193.6222,  1.205,  -13.80 , 174.9635, 1.009, 0.}; // chi=4,902

        // Double_t variable[11] = { -111.0809,  537.8615,  -131.0399,  599.626,  -193.4688,  -193.8688, 1., -14.,
        // 179.1, 1.0011, 0.}; // chi= Double_t variable[11] = { -111.0701,  537.8723,  -131.0391,  599.6368, -193.4580,
        // -193.8580, 1.010751, -13.98925, 179.1108, 1.001350, 0.}; // chi=5.88859

        // Double_t step[11] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.1, 1.5, 0.01, 0.1};
        // Double_t step[11] = {0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.5, 0.01, 0.1};
        // Double_t step[11] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.01, 0.01};
        // Double_t step[12] = {0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.005, 0.05, 0.005, 0.005, 0.005, 0.005};
        // Double_t step[12] = {0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.001, 0.001, 0.01, 0.001, 0.001, 0.001};
        // Double_t step[12] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.01, 0.01, 0.1, 0.01, 0.01, 0.01};
        // Double_t step[12] = {0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.5, 0.02, 0.02, 0.02};
        Double_t step[12] = { 0.2, 0.5, 0.2, 0.5, 0.2, 0.2, 0.5, 0.5, 0.5, 0.01, 0.05, 0.05 };

        mini->SetFunction(fm1);

        // Set the free variables to be minimized!
        mini->SetLimitedVariable(0, "x31", variable[0], step[0], variable[0] - 2.5, variable[0] + 2.5);
        mini->SetLimitedVariable(1, "z31", variable[1], step[1], variable[1] - 2.5, variable[1] + 2.5);
        mini->SetLimitedVariable(2, "x33", variable[2], step[2], variable[2] - 2.5, variable[2] + 2.5);
        mini->SetLimitedVariable(3, "z33", variable[3], step[3], variable[3] - 2.5, variable[3] + 2.5);
        mini->SetLimitedVariable(4, "dangle31", variable[4], step[4], variable[4] - 1.5, variable[4] + 1.5);
        mini->SetLimitedVariable(5, "dangle33", variable[5], step[5], variable[5] - 1.5, variable[5] + 1.5);
        mini->SetLimitedVariable(6, "ypos", variable[6], step[6], -2., 2.);
        mini->SetLimitedVariable(7, "angleY", variable[7], step[7], variable[7] - 2., variable[7] + 2.);
        mini->SetLimitedVariable(8, "zpos", variable[8], step[8], variable[8] - 10., variable[8] + 10.);
        mini->SetLimitedVariable(9, "field", variable[9], step[9], variable[9] - 0.1, variable[9] + 0.1);
        mini->SetLimitedVariable(10, "x23", variable[10], step[10], variable[10] - 0.5, variable[10] + 0.5);
        mini->SetLimitedVariable(11, "z23", variable[11], step[11], variable[11] - 0.5, variable[11] + 0.5);
        // mini->FixVariable(0);
        // mini->FixVariable(1);
        // mini->FixVariable(2);
        // mini->FixVariable(3);
        // mini->FixVariable(4);
        // mini->FixVariable(5);
        // mini->FixVariable(6);
        // mini->FixVariable(7);
        // mini->FixVariable(8);
        // mini->FixVariable(9);
        // mini->FixVariable(10);
        // mini->FixVariable(11);
    }

    Int_t status = 0;

    // do the minimization
    mini->Minimize();

    status = mini->Status();
    cout << "Status: " << status << endl;
    cout << "optimized values: " << endl;
    cout.precision(7);
    if (fLeft)
    {
        cout << "dx30: " << mini->X()[0] << " dz30 " << mini->X()[1] << endl;
        cout << "dx32: " << mini->X()[2] << " dz32 " << mini->X()[3] << endl;
        cout << "dangle30: " << mini->X()[4] << " dangle32: " << mini->X()[5] << endl;
    }
    else
    {
        cout << "dx31: " << mini->X()[0] << " dz31 " << mini->X()[1] << endl;
        cout << "dx33: " << mini->X()[2] << " dz33 " << mini->X()[3] << endl;
        cout << "dangle31: " << mini->X()[4] << " dangle33: " << mini->X()[5] << endl;
    }
    cout << "PosY: " << mini->X()[6] << endl;
    cout << "PosZ " << mini->X()[8] << endl;
    cout << "Glad angle Y: " << mini->X()[7] << endl;
    // cout << "Glad angle Z: " << mini->X()[10]  << endl;
    cout << "angleX: " << mini->X()[10] << endl;
    cout << "angleZ: " << mini->X()[11] << endl;
    cout << "field: " << mini->X()[9] << endl;
    mini->Clear();

    if (fVis)
    {
        for (auto const& det : fDetectors->GetArray())
        {
            // det->Draw();
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
