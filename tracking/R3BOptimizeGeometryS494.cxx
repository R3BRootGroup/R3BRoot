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
                cout << "Copy Hits: " << fi23a->events[fNEvents].size() << "  " << fi23b->events[fNEvents].size() << " "
                     << fi32->events[fNEvents].size() << "  " << fi30->events[fNEvents].size() << "  "
                     << tof->events[fNEvents].size() << endl;

                cout << "Saved events: " << gThisTask->GetNEvents() << endl;

                fNEvents += 1;
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
    Bool_t fLeft = kFALSE;
    // Bool_t fLeft = kTRUE;
    Double_t chi2_all = 0;
    Int_t nof = 0;
    cout.precision(7);
    cout << "new correction: " << xx[0] << "  " << xx[1] << "  " << xx[2] << "  " << xx[3] << "  " << xx[4] << "  "
         << xx[5] << "  " << xx[6] << "  " << xx[7] << "  " << xx[8] << "  " << xx[9] << endl;

    ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(xx[6]);

    //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionY(xx[6]);
    //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleY(xx[7]);
    //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionZ(xx[8]);
    //((R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionX(xx[9]);
    //((R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(xx[10]);
    //((R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(xx[11]);

    //( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.02);
    //( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleX(0.01);
    //( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(0.03);
    //( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrectionAngleZ(0.02);

    R3BTrackingDetector* fi23a = gSetup->GetByName("fi23a");

    fi23a->pos0 = TVector3(0., 0., 0.);
    fi23a->pos1 = TVector3(5., 5., 0.);
    fi23a->pos2 = TVector3(-5., 5., 0.);

    fi23a->pos0.RotateY(xx[8] * TMath::DegToRad());
    fi23a->pos1.RotateY(xx[8] * TMath::DegToRad());
    fi23a->pos2.RotateY(xx[8] * TMath::DegToRad());

    // TVector3 trans3b(xx[7],  xx[8], xx[9]);
    TVector3 trans3a(xx[7], 0., xx[9] - 0.015);

    fi23a->pos0 += trans3a;
    fi23a->pos1 += trans3a;
    fi23a->pos2 += trans3a;
    fi23a->norm = ((fi23a->pos1 - fi23a->pos0).Cross(fi23a->pos2 - fi23a->pos0)).Unit();

    R3BTrackingDetector* fi23b = gSetup->GetByName("fi23b");

    fi23b->pos0 = TVector3(0., 0., 0.);
    fi23b->pos1 = TVector3(5., 5., 0.);
    fi23b->pos2 = TVector3(-5., 5., 0.);

    fi23b->pos0.RotateY(xx[8] * TMath::DegToRad());
    fi23b->pos1.RotateY(xx[8] * TMath::DegToRad());
    fi23b->pos2.RotateY(xx[8] * TMath::DegToRad());

    // TVector3 trans3b(xx[7],  xx[8], xx[9]);
    TVector3 trans3b(0., xx[8], xx[9] + 0.015);

    fi23b->pos0 += trans3b;
    fi23b->pos1 += trans3b;
    fi23b->pos2 += trans3b;
    fi23b->norm = ((fi23b->pos1 - fi23b->pos0).Cross(fi23b->pos2 - fi23b->pos0)).Unit();

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

        // für break-up events wieder rein

        //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(xx[6]);
        // Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
        // Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
        // cout << "Field:" << field << " scale: " << scale << endl;

        // Braucht man nur für Magnetfeld runs
        /*
                //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(1.000376);
                //Double_t sf = 1.000376;
                //Double_t sf = 1.0;
                Double_t sf = xx[6];
                Double_t cur0 = 3583.81;
                Double_t cur1 = -1560.64563;
                Double_t cur2 = -1410.63245;
                Double_t cur3 = -1260.59814;
                Double_t cur4 = -1110.54639;
                Double_t cur5 = -2310.30298;
                Double_t cur6 = -2160.40796;
                Double_t cur7 = -2010.51855;
                Double_t cur8 = -1860.59241;

        // After measuring the magnetic field we found out that the scaling has to be corrected
        // I_theory           I_corrected         I_corrected/I_theory
        //   2310.00000       2310.37329       1.00013113
        //   2160.00000       2160.45117       1.00020885
        //   2010.00000       2010.55872       1.00027800
        //   1860.00000       1860.62976       1.00033855
        //   1710.00000       1710.66785       1.00039053
        //   1560.00000       1560.67688       1.00043392
        //   1410.00000       1410.66077       1.00046861
        //   1260.00000       1260.62329       1.00049472
        //   1110.00000       1110.56860       1.00051224


                        if(fLeft)
                        {
                            if(iev == 0)
                            //if(iev == -1)
                            {
                                Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                                Double_t field = ((R3BGladFieldMap *)
        FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field:" << field << " scale: " << scale <<
        endl; fieldScale	= cur1 / cur0 / scale * sf;// xx[9];	//run395 cout << "Setting field to " <<
        fieldScale << endl; ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                   )->SetTrackerCorrectionScale(fieldScale); field = ((R3BGladFieldMap *)
                   FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field after:" << 	field << endl;

                            }
                            if(iev == 1000)
                            //if(iev == 800)
                            {
                                Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                                Double_t field = ((R3BGladFieldMap *)
        FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field:" << field << " scale: " << scale<< endl;
                                fieldScale	= cur2 / cur0 / scale * sf ;// xx[9];	//run391
                                cout << "Setting field to " << 	fieldScale << endl;
                                ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                   )->SetTrackerCorrectionScale(fieldScale); field = ((R3BGladFieldMap *)
                   FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field after:" << 	field << endl;
                            }
                            if(iev == 2000)
                            //if(iev == 1600)
                            {
                                Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                                Double_t field = ((R3BGladFieldMap *)
        FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field:" << field << " scale: " << scale <<
        endl; fieldScale	= cur3 / cur0 / scale * sf ;// xx[9];	//run389 cout << "Setting field to " <<
        fieldScale << endl; ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                   )->SetTrackerCorrectionScale(fieldScale); field = ((R3BGladFieldMap *)
                   FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field after:" << 	field << endl;
                            }
                            if(iev == 3000)
                            //if(iev == 2400)
                            {
                                Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                                Double_t field = ((R3BGladFieldMap *)
        FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field:" << field << " scale: " << scale <<
        endl; fieldScale	= cur4 / cur0 / scale * sf ;// xx[9];	//run389 cout << "Setting field to " <<
        fieldScale << endl; ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
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
                                Double_t field = ((R3BGladFieldMap *)
        FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field:" << field << " scale: " << scale <<
        endl; fieldScale	= cur5 / cur0 / scale * sf ;// xx[9]; 	//run399 cout << "Setting field to " <<
        fieldScale << endl; ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                   )->SetTrackerCorrectionScale(fieldScale); field = ((R3BGladFieldMap *)
                   FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field after:" << 	field << endl;

                            }
                            if(iev == 1000)
                            //if(iev == 0)
                            //if(iev == -1)
                            {
                                Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                                Double_t field = ((R3BGladFieldMap *)
        FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field:" << field << " scale: " << scale<< endl;
                                fieldScale	= cur6 / cur0 / scale * sf ;// xx[9];//run401
                                cout << "Setting field to " << 	fieldScale << endl;
                                ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                   )->SetTrackerCorrectionScale(fieldScale); field = ((R3BGladFieldMap *)
                   FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field after:" << 	field << endl;
                            }
                            if(iev == 2000)
                            //if(iev == 200)
                            //if(iev == -1)
                            {
                                Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                                Double_t field = ((R3BGladFieldMap *)
        FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field:" << field << " scale: " << scale <<
        endl; fieldScale	= cur7 / cur0 / scale * sf ;// xx[9];//run405
                                cout << "Setting field to " << 	fieldScale << endl;
                                ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                   )->SetTrackerCorrectionScale(fieldScale); field = ((R3BGladFieldMap *)
                   FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field after:" << 	field << endl;
                            }
                            if(iev == 3000)
                            //if(iev == 300)
                            //if(iev == -1)
                            {
                                Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                                Double_t field = ((R3BGladFieldMap *)
        FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field:" << field << " scale: " << scale <<
        endl; fieldScale	= cur8 / cur0 / scale * sf ;// xx[9];//run405
                                cout << "Setting field to " << 	fieldScale << endl;
                                ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField()
                   )->SetTrackerCorrectionScale(fieldScale); field = ((R3BGladFieldMap *)
                   FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.); cout << "Field after:" << 	field << endl;
                            }
                        }
        */
        // Braucht man nur für Magnetfeld runs - Ende

        // cout << "Test: " << gSetup->GetArray().size() << endl;

        Double_t chi2 = gThisTask->Chi2();
        // cout << "Chi2 value: " << chi2 << endl;
        if (chi2 > 0)
        {
            chi2_all += chi2;
            nof++;
        }
    }
    // cout << "Test: " << nof << endl;
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
    Bool_t debug = false;

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

    if (tof->hits.size() > 0 && debug)
    {
        // cout << "*************** NEW EVENT ****" << fNEvents << ", " << fNEvents_nonull << endl;
        cout << "Hits ToFD: " << tof->hits.size() << endl;
        cout << "Hits right: " << fi23a->hits.size() << "  " << fi23b->hits.size() << "  " << fi31->hits.size() << "  "
             << fi33->hits.size() << endl;

        cout << "Hits left: " << fi23a->hits.size() << "  " << fi23b->hits.size() << "  " << fi32->hits.size() << "  "
             << fi30->hits.size() << endl;
    }

    if (debug)
    {
        for (Int_t i = 0; i < fi23a->hits.size(); i++)
        {
            cout << "Fib23a hits: " << i << ", " << fi23a->hits.at(i)->GetX() << endl;
        }
        for (Int_t i = 0; i < fi23b->hits.size(); i++)
        {
            cout << "Fib23b hits: " << i << ", " << fi23b->hits.at(i)->GetY() << endl;
        }
        for (Int_t i = 0; i < tof->hits.size(); i++)
        {
            cout << "Tofd hits  : " << i << ", " << tof->hits.at(i)->GetX() << ", " << tof->hits.at(i)->GetY() << endl;
        }
    }

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
    // Change for magnetic field runs
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
        if (charge != 2 && charge != 6)
            // if (charge != 6)
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

        if (debug)
            cout << "AT START: "
                 << "Mass: " << m0 << ", Momentum: " << p0 << endl;

        // cout << "Mass m0: " << m0 << endl;
        // cout << "Momentum p0: " << p0 << endl;
        // cout << "Charge z0: " << charge << endl;
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
                            if (debug)
                            {
                                cout << "left side of setup" << endl;
                                cout << "Charge requested: " << charge_requested << endl;
                                cout << "Hit Tofd # " << i << " x: " << tof->hits.at(i)->GetX()
                                     << " y: " << tof->hits.at(i)->GetY() << endl;
                            }
                            if (ifi23a > -1 && debug)
                                cout << " Fi23a left # " << ifi23a << " x: " << fi23a->hits.at(ifi23a)->GetX() << endl;
                            if (ifi23b > -1 && debug)
                                cout << " left Fi23b # " << ifi23b << " y: " << fi23b->hits.at(ifi23b)->GetY() << endl;
                            if (ifi30 > -1 && debug)
                                cout << " fi30 # " << ifi30 << " x: " << fi30->hits.at(ifi30)->GetX() << endl;
                            if (ifi32 > -1 && debug)
                                cout << " fi32 # " << ifi32 << " x: " << fi32->hits.at(ifi32)->GetX() << endl;
                            if (debug)
                                cout << "Hit target # "
                                     << " x: " << target->hits.at(0)->GetX() << endl;

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

                            if (debug)
                                cout << " Chi: " << candidate->GetChi2() << "  " << candidate->GetStartMomentum().Mag()
                                     << "  "
                                     << 1000. * (candidate->GetStartMomentum().Mag() - p0) *
                                            (candidate->GetStartMomentum().Mag() - p0)
                                     << endl;
                            if (debug)
                                cout << "--------------------------------" << endl;

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
                            // << endl; if(ifi23a > -1) cout << "fi23a # " <<  ifi23a << " x: "<<
                            // fi23a->hits.at(ifi23a)->GetX() << endl; if(ifi23a > -1) cout << "fi23a # " <<  ifi23a << "
                            // x: "<< fi23a->hits.at(ifi23a)->GetX() << endl; if(ifi23b > -1) cout << "fi23b # " <<
                            // ifi23b << " y: "<< fi23b->hits.at(ifi23b)->GetY() << endl;

                            // cout << "Hit target # " << " x: " << target->hits.at(0)->GetX() << endl;
                            if (debug)
                            {
                                cout << "right side of setup" << endl;
                                cout << "Charge requested: " << charge_requested << endl;
                                cout << "Start values to fit, x0: " << x0 << " y0: " << y0 << " z0: " << z0
                                     << " p0: " << p0 << " beta0: " << beta0 << " m0: " << m0 << endl;
                                cout << "Hit Tofd # " << i << " x: " << tof->hits.at(i)->GetX()
                                     << " y: " << tof->hits.at(i)->GetY() << endl;
                            }
                            if (ifi23a > -1 && debug)
                                cout << " Fi23a # " << ifi23a << " x: " << fi23a->hits.at(ifi23a)->GetX() << endl;
                            if (ifi23b > -1 && debug)
                                cout << "right Fi23b # " << ifi23b << " y: " << fi23b->hits.at(ifi23b)->GetY() << endl;
                            if (ifi33 > -1 && debug)
                                cout << "Fi33 # " << ifi33 << " x: " << fi33->hits.at(ifi33)->GetX() << endl;
                            if (ifi31 > -1 && debug)
                                cout << "Fi31  # " << ifi31 << " x: " << fi31->hits.at(ifi31)->GetX() << endl;
                            if (debug)
                                cout << "Hit target # "
                                     << " x: " << target->hits.at(0)->GetX() << endl;

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

                            if (debug)
                                cout << "Chi: " << candidate->GetChi2() << "  pstart.Mag "
                                     << candidate->GetStartMomentum().Mag() << " dp.Mag "
                                     << 1000. * (candidate->GetStartMomentum().Mag() - p0) *
                                            (candidate->GetStartMomentum().Mag() - p0)
                                     << endl;
                            if (debug)
                                cout << "--------------------------------" << endl;

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
                (p0 * p0 * 0.01 * 0.01);
        mChi2 = candidate->GetChi2() + pChi2; // Achtung!
        zChi2 = (candidate->GetStartMomentum().Z() - p0) * (candidate->GetStartMomentum().Z() - p0) /
                (p0 * p0 * 0.01 * 0.01);
        yChi2 = (candidate->GetStartMomentum().Y() - 0.) * (candidate->GetStartMomentum().Y() - 0.) / (0.01 * 0.01);
        xChi2 = (candidate->GetStartMomentum().X() - 0.) * (candidate->GetStartMomentum().X() - 0.) / (0.01 * 0.01);

        // cout << "mChi2: " << mChi2 << endl;
        // cout << "pChi2: " << pChi2 << endl;
        // mChi2 = mChi2 + pChi2;
        // cout << "mChi2: " << mChi2 << endl;

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

    if (mChi2 < 10000.)
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

    // ROOT::Math::Minimizer* mini = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
    // ROOT::Math::Minimizer* mini = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Scan");
    ROOT::Math::Minimizer* mini = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Simplex");
    // Minimum = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
    // Migrad, Simplex, Scan,
    // set tolerance , etc...
    mini->SetMaxFunctionCalls(10000); // for Minuit/Minuit2
    mini->SetMaxIterations(1000);     // for GSL
    mini->SetTolerance(10.);
    mini->SetPrintLevel(1);

    // create funciton wrapper for minmizer
    // a IMultiGenFunction type
    ROOT::Math::Functor fm1(&Chi2AllEvents, 10);

    if (fLeft)
    {

        Double_t variable[10] =
            //{-62.58520, 570.8430, -82.70848, 632.5428, -14.000, -14.000, 0.0 }; // Startwerte
            //{-62.23755, 571.3730, -82.47672, 633.1141, -13.97303, -14.16004, 0.081043 }; // chi=7.533950
            //{-62.23755, 571.3730, -82.47672, 633.1141, -13.97303, -14.16004, 0.081043, 0., 0., 91.05 }; // chi=5.95577
            //{-62.23755, 571.3730, -82.47672, 633.1141, -13.97303, -14.16004, 0.081043, 0.1, 0., 91.05 }; //
            //chi=5.95577
            //{-62.62859, 571.6278, -82.71948, 633.3689, -13.9223, -14.10931, 0.1317587, -0.07913441,
            //0.05071571, 91.10072 }; // chi=
            //{-63.29942, 573.5622, -83.94180, 631.7081, -13.42387, -13.83432, 0.9991695, 1.186133, -0.3350816, 91.0253
            //}; // chi=46.5 with sigmap=0.01
            //{-63.28558, 571.0375, -84.09037, 634.0597, -13.95823, -14.27242, 0.4031335, 0.331566,
            //-0.03871824, 91.26594 }; // chi=1469 with sigmap=0.001
            //{-63.25367, 571.1016, -83.86361, 634.0761, -13.94912, -14.26561, 0.4152935, -0.6209402,
            //0.1947092, 91.38979 }; // chi=410 with sigmap=0.001 and tracked data
            //{-63.45233, 570.8596, -84.10561, 634.2469, -13.90920, -14.19762, 0.250, -0.65, 0.1, 91.37 }; // chi=426.3
            //with sigmap=0.001 and tracked data  Fib23

            //{-62.58520, 570.8430, -82.70848, 632.5428, -14.000, -14.000, 0., 0., 0., 91.2  }; // Startwerte
            //{-62.58520, 570.9581, -82.70848, 632.4312, -12.61012, -15.45907, 0., 0., 0., 91.2  }; // chi=3.4442
            //without momentum contribution
            //{-62.59022, 571.0172, -82.70850, 632.4312, -12.60802, -15.45762, 0., 0., 0., 91.2  }; // chi=4.3627  with
            //momentum contribution and sigma=0.001
            //{-62.59034, 571.0075, -82.70805, 632.3565, -13.64436, -15.45762, 1., 0., 0., 91.2  }; // chi=4.0276  with
            //momentum contribution and sigmap=0.001 and 1000 events
            //{-62.59034, 571.0075, -82.70805, 632.3565, -13.64436, -15.44762, 1., 0., 0., 91.2  }; // chi=3.1195 with
            //scale and with momentum contribution and sigmap=0.001 and 1000 events
            //{-62.59034, 571.0075, -82.70805, 632.3565, -13.64436, -15.43762, 1., 0., 0., 91.2  }; // chi=11.2971 with
            //scale and with momentum contribution and sigmap=0.0005 and 1000 events

            // after better settings for step size of px, py, pz
            //{-62.23128, 571.2587, -82.39341, 632.5904, -13.70977, -15.45659, 1., 0., 0., 91.2  }; // chi=82.2890
            //without scale and with momentum contribution and sigmap=0.0005 and 1000 events
            //{-62.23128, 571.2587, -82.39341, 632.5904, -13.70977, -15.45659, 1., 0., 0., 91.2  }; // chi=77.85605 with
            //scale and with momentum contribution and sigmap=0.0005 and 1000 events

            //{-62.58520, 570.8430, -82.70848, 632.5428, -14.000, -14.000, 1., 0., 0., 91.2  }; // Startwerte
            //{-62.18069, 570.5933, -82.41713, 632.9688, -13.98726, -14.02967, 1., 0., 0., 91.2  };  // chi=79.366
            //without scale and with momentum contribution and sigmap=0.0005 and 1000 events
            //{-62.18069, 570.5933, -82.41713, 632.9688, -13.98726, -14.02967, 1., 0., 0., 91.2  };  // chi=79.366 with
            //scale and with momentum contribution and sigmap=0.0005 and 1000 events

            // so far best
            //{-62.18069, 570.5933, -82.41713, 632.9688, -13.68626, -14.02967, 1., 0., 0., 91.2  };  // chi=79.1549
            //without scale and with momentum contribution and sigmap=0.0005 and 1000 events
            // now with exp data
            //{-62.18069, 570.6933, -82.41713, 632.9688, -13.68626, -14.02967, 1., 0.0, 0.0, 91.04  };  // chi=79.1549
            //without scale and with momentum contribution and sigmap=0.0005 and 1000 events

            // now magnetic field runs with tracks which hit fi23a
            //{-62.58520, 570.8430, -82.70848, 632.5428, -14.000, -14.000, 1., 0., 0., 91.2  }; // Startwerte
            //{-62.49292, 570.8348, -82.64447, 632.4315, -13.98908, -13.96955, 1.00024, -0.419508, 0., 91.394  }; //
            //chi2=89.59149

            // after taking out py of optimization
            //{-62.58520, 570.8430, -82.70848, 632.5428, -14.000, -14.000, 1., 0., 0., 91.2  }; // Startwerte
            //{-62.48242, 570.9792, -82.69180, 632.6790, -13.99638, -13.98638, 1., 0., 0., 91.2  }; // chi2=2.032
            //sigmap=0.001
            //{-62.48242, 570.9792, -82.69180, 632.6790, -13.99638, -13.98638, 1., 0., 0., 91.2  }; // chi2=2.032
            //sigmap=0.001
            //{-62.27230, 571.1321, -82.16564, 632.4996, -14.05917, -14.02692, 1., 0., 0., 91.2  }; // chi2=79.71
            //sigmap=0.0005

            //{-63.12573, 571.2747, -83.23229, 632.6543, -13.96104, -13.98078, 1.012128, 0., 0., 91.2  }; //
            //chi2=37.3593 sigmap=0.0001
            //{-63.97522, 571.6246, -84.23016, 632.9943, -13.97081, -13.91154, 1.0, -0.1067407, 0.16940, 91.13455  }; //
            //chi2=37.144176 sigmap=0.0001
            //{-63.99189, 571.6302, -84.20793, 633.0165, -13.94859, -13.90932, 1.0, -0.1067407, 0.16940, 91.13455  }; //
            //chi2= 35.50894 sigmap=0.0001

            //{-62.23466, 570.6938, -82.50767, 632.9448, -13.68123, -14.02354, 1., 0.1106492, 0.0369036, 91.06051  }; //
            //chi=38.5 sigmap=0.001
            //{-62.21163, 570.7021, -82.64561, 632.9570, -13.68240, -14.01811, 1., 0.2439544, 0.03561416, 91.06078  };
            //// chi=307.583 sigmap=0.001

            //{-62.58520, 570.8430, -82.84179, 632.5095, -13.999, -13.98667, 1., 0.1997498, 0., 91.12499  }; //
            //chi=303.085, einzeln optimiert

            //{-62.54769, 570.8805, -82.80428, 632.5470, -13.99525, -13.98292, 1., 0.2372632, 0., 90.92524  }; //
            //chi=1.979364
            //{-62.52268, 571.0055, -82.77927, 632.5720, -13.99275, -13.97042, 1., 0.1372632, 0., 90.95025  }; //
            //chi=5.18
            //{-62.52268, 571.0055, -82.77927, 632.5720, -13.98275, -13.97042, 1., 0.1372632, 0., 90.95025  }; //
            //chi=16.2856 sigmap=0.01

            //{-62.62268, 571.0341, -82.65069, 632.6006, -13.97989, -13.96756, 0.9934388, 0., 0., 90.97883  }; //
            //chi=864 sigmap=0.01

            //{-62.86640, 570.7671, -82.80870, 632.4669, -14.000, -14.000, 1., 0., 0., 91.2  }; // Startwerte
            //{-62.58676, 570.8731, -82.85948, 632.5588, -13.99137, -13.99137, 1.000863, 0., 0., 91.16132  }; //
            //chi=955.8960 sigmap=0.001
            //{-62.72252, 570.9484, -82.96545, 632.6482, -13.98946, -13.98946, 1.000625, 0., 0., 91.30539  }; //
            //chi=66.1772 sigmap=0.005
            //{-62.72252, 570.9484, -82.96545, 632.6482, -13.98946, -13.98946, 1.000625, 0., 0., 91.30539  }; //
            //chi=36.9737 sigmap=0.01
            //{-62.72252, 570.9484, -82.96545, 632.6482, -13.97946, -13.98946, 1.000625, 0., 0., 91.30539  }; //
            //chi=22.2219 ohne sigmap

            // Now with the same scaling factor for left and right
            //{-62.72252, 570.9484, -82.96545, 632.6482, -13.98946, -13.98946, 1.000625, 0., 0., 91.30539  }; //
            //chi=36.97114 sigmap=0.01
            //{-62.72252, 570.9584, -82.96545, 632.6482, -13.98946, -13.98946, 1.000625, 0., 0., 91.18721  }; //
            //chi=35.7339 sigmap=0.01
            //{-62.72252, 570.9584, -82.96545, 632.6482, -13.98946, -13.98946, 1.000625, 0.000, 0.005, 91.2  }; //
            //chi=35.7339 sigmap=0.01
            //{-62.71252, 570.9584, -82.96545, 632.6482, -13.98946, -13.98946, 1.000625, 0.000, 0.005, 91.2  }; //
            //chi= 37.8560 sigmap=0.01
            //{-62.71252, 570.9584, -82.96545, 632.6482, -13.98946, -13.98946, 1.000625, 0.000, 0.005, 91.3  }; //
            //chi=36.75182 sigmap=0.01
            //{-62.71252, 570.9584, -82.96545, 632.6482, -13.98946, -13.98946, 1.000625, 0.000, 0.005, 91.31  }; //
            //chi=36.67675095 sigmap=0.01

            // now px, py fix and target point same for C and 4He, deltaxy=0.1
            //{-62.71252, 570.9584, -82.96545, 632.6482, -13.98946, -13.98946, 1.000625, 0.000, 0.005, 91.31  }; //
            //chi=36.67675095 sigmap=0.01

            //{-62.18069, 570.7933, -82.41713, 632.9688, -13.68626, -14.02967, 1., 0.0, 0.0, 91.04  };  // chi=1.6174741
            //without sigmap and 1000 events
            //{-62.18069, 570.7933, -82.41713, 632.9688, -13.68626, -14.02967, 1., 0.0, 0.0, 91.04  };  //
            //chi=10.6018455 sigmap=0.01 and 1000 events
            //{-62.15068, 570.8233, -82.38712, 632.9988, -13.68326, -14.02667, 1.0003, 0.03, 0.03, 90.84025  };  //
            //chi=8.44264287295 sigmap=0.01 and 1000 events
            //{-62.15211, 570.8251, -82.38671, 633.0006, -13.68308, -14.02649, 1.001, 0.0, 0.0, 91.04184  };  //
            //chi=8.7640679 sigmap=0.01 and 1000 events
            //{-62.15211, 570.8251, -82.38671, 633.0006, -13.68308, -14.02549, 1.001, 0.0, 0.0, 91.04184  };  // ***
            //chi=8.654978 sigmap=0.01 and 1000 events
            //{-62.15211, 570.8251, -82.37671, 633.0006, -13.68308, -14.02549, 1.002, 0.0, 0.0, 91.04184  };  //
            //chi=8.7443146 sigmap=0.01 and 1000 events

            // deltaxy=0.01
            //{-62.15211, 570.8251, -82.37671, 633.0006, -13.67308, -14.02549, 1.002, 0.0, 0.0, 91.04184  };  //
            //chi=11.20842577 sigmap=0.01 and 1000 events
            //{-62.15211, 570.8251, -82.37671, 633.0006, -13.67308, -14.02549, 1.002, 0.1, 0.0, 91.04184  };  //
            //chi=10.802843 sigmap=0.01 and 1000 events

            //{-62.16032, 570.8370, -82.39240, 633.0125, -13.67189, -14.02330, 1.003, 0.0, 0.0, 91.04613 };  //
            //chi=11.3757596 sigmap=0.01 and 1000 events

            //{-62.17822, 570.7069, -82.42824, 632.9824, -13.68490, -14.02831, 1., 0.01358, 0.01358, 91.05359  };  //
            //chi=9.2844436 without scale and with momentum contribution and sigmap=0.0005 and 1000 events
            //{-62.17622, 570.7189, -82.43824, 632.9944, -13.68370, -14.02711, 1.00012, 0.02558493, 0.02558493, 91.06559
            //};  // chi=9.152384725 without scale and with momentum contribution and sigmap=0.0005 and 1000 events
            //{-62.17322, 570.7219, -82.45824, 632.9974, -13.68340, -14.02681, 1.00015, 0.02858494, 0.02858494, 91.06859
            //};  // chi= 9.059510129 without scale and with momentum contribution and sigmap=0.0005 and 1000 events
            //{-62.17322, 570.7219, -82.45824, 632.9974, -13.68340, -14.02681, 1.00015, 0.03858494, 0.02858494, 91.06859
            //};  // chi= 8.99686 without scale and with momentum contribution and sigmap=0.0005 and 1000 events
            //{-62.18322, 570.7252, -82.45491, 633.0007, -13.68307, -14.02648, 1.001, 0.014, 0.02, 91.06 };  //
            //chi=9.3093104 without scale and with momentum contribution and sigmap=0.0005 and 1000 events

            // best
            //{-62.17322, 570.7219, -82.45824, 632.9974, -13.68340, -14.01681, 1.0, 0.014, 0.02, 91.06 };  //
            //chi=9.197948 without scale and with momentum contribution and sigmap=0.0005 and 1000 events

            // after shifting the fiber detectors by half a fiber and sigma target != 0 (v1)
            //{-62.16766, 570.7336, -82.45762, 633.0091, -13.68223, -14.01564, 1.0, 0.0257332, 0.03173322, 91.07173 };
            //// chi=4.23709109286 without scale and with momentum contribution and sigmap=0.0005 and 1000 events
            //{-62.16210, 570.7453, -82.45700, 633.0208, -13.68106, -14.01447, 1.0, 0.03746642, 0.04346644, 91.08346 };
            //// chi=4.158161482 without scale and with momentum contribution and sigmap=0.0005 and 1000 events
            //{-62.16210, 570.7453, -82.45700, 633.0208, -13.68106, -14.01447, 1.0, 0.04746642, 0.04346644, 91.08346 };
            //// chi=4.10682275 without scale and with momentum contribution and sigmap=0.0005 and 1000 events
            //{-62.16210, 570.7453, -82.45700, 633.0208, -13.68106, -14.01447, 1.0, 0.05746642, 0.04346644, 91.08346 };
            //// chi=4.04711486 without scale and with momentum contribution and sigmap=0.0005 and 1000 events

            //{-62.15710, 570.7503, -82.47700, 633.0258, -13.68056, -14.01397, 1.0, 0.085, 0.0486, 91.085 };  //
            //chi=3.97238620 without scale and with momentum contribution and sigmap=0.0005 and 1000 events
            // 19th opti
            //{-62.15710, 570.7603, -82.47700, 633.0258, -13.68056, -14.01397, 1.0, 0.085, 0.0486, 91.085 };  //
            //chi=3.967967 without scale and with momentum contribution and sigmap=0.0005 and 1000 events

            // after shifting the fiber detectors by half a fiber and sigma target = 0
            //{-62.16766, 570.7336, -82.45762, 633.0091, -13.68223, -14.01564, 1.0, 0.0257332, 0.0317332, 91.07173 }; //
            //chi=4.30623122 without scale and with momentum contribution and sigmap=0.01
            //{-62.16210, 570.7453, -82.45700, 633.0208, -13.68106, -14.01447, 1.0, 0.03746642, 0.04346642, 91.08346 };
            //// chi=4.60040758 without scale and with momentum contribution and sigmap=0.01
            //{-62.15654, 570.7570, -82.45638, 633.0325, -13.67989, -14.01330, 1.0, 0.04919964, 0.05519964, 91.09519 };
            //// chi=4.1924227136 without scale and with momentum contribution and sigmap=0.01
            //{-62.15098, 570.7687, -82.45576, 633.0442, -13.67872, -14.01213, 1.0, 0.06093286, 0.06693286, 91.10692 };
            //// chi=4.03942520 without scale and with momentum contribution and sigmap=0.01
            //{-62.14542, 570.7804, -82.45514, 633.0559, -13.67755, -14.01096, 1.0, 0.07266608, 0.07866608, 91.11865 };
            //// chi=4.003279422 without scale and with momentum contribution and sigmap=0.01
            //{-62.13986, 570.7921, -82.45452, 633.0676, -13.67638, -14.00979, 1.0, 0.08439930, 0.0903993, 91.13038 };
            //// chi=3.965502623 without scale and with momentum contribution and sigmap=0.01
            //{-62.13986, 570.7921, -82.45452, 633.0676, -13.66638, -14.00979, 1.0, 0.08439930, 0.0903993, 91.13038 };
            //// chi=3.963153075 without scale and with momentum contribution and sigmap=0.01
            //{-62.13986, 570.7921, -82.45452, 633.0676, -13.66638, -14.00979, 1.0, 0.09439930, 0.0903993, 91.13038 };
            //// chi=3.954176985 without scale and with momentum contribution and sigmap=0.01
            //{-62.13986, 570.7921, -82.45452, 633.0676, -13.66638, -14.00979, 1.0, 0.09439930, 0.0903993, 91.14038 };
            //// chi=3.93942016 without scale and with momentum contribution and sigmap=0.01

            //{-62.14986, 570.7954, -82.45119, 633.0709, -13.66605, -14.00946, 1.0, 0.073, 0.0805, 91.321 };  //
            //chi=4.033744125 without scale and with momentum contribution and sigmap=0.01
            // 18th opti
            //{-62.14986, 570.7954, -82.45119, 633.0709, -13.66605, -14.00946, 1.0, 0.073, 0.0805, 91.321 };  //
            //chi=4.034172779 without scale and with momentum contribution and sigmap=0.01

            // new data set
            //{-62.14430, 570.8071, -82.45057, 633.0826, -13.66488, -14.00829, 1.0, 0.08473322, 0.09223322, 91.33273 };
            //// chi= 13.0799417 without scale and with momentum contribution and sigmap=0.01
            //{-62.13874, 570.8188, -82.44995, 633.0943, -13.66371, -14.00712, 1.0, 0.09646644, 0.1039664, 91.34446 };
            //// chi=11.10464082 without scale and with momentum contribution and sigmap=0.01

            // sigma target = 0.5
            //{-62.13318, 570.8305, -82.44933, 633.1060, -13.66254, -14.00595, 1.0, 0.1081997, 0.1156996, 91.35619 }; //
            //chi=12.88408862 without scale and with momentum contribution and sigmap=0.01
            //{-62.12762, 570.8422, -82.44871, 633.1177, -13.66137, -14.00478, 1.0, 0.1199329, 0.1274328, 91.36792 }; //
            //chi=12.8292641785 without scale and with momentum contribution and sigmap=0.01
            //{-62.12206, 570.8539, -82.44809, 633.1294, -13.66020, -14.00361, 1.0, 0.1316661, 0.1391660, 91.37965 }; //
            //chi=12.7657087843 without scale and with momentum contribution and sigmap=0.01

            // mit fib23 z fix
            //{-62.12137, 570.8555, -82.44793, 633.1310, -13.66004, -14.00345, 1.0, 0.1332196, 0.1407195, 91.2 };  //
            //chi=12.768289574 without scale and with momentum contribution and sigmap=0.01
            //{-62.11824, 570.8711, -82.46043, 633.1466, -13.65848, -14.00189, 1.0, 0.1488510, 0.1563509, 91.2 };  //
            //chi=12.7025623989 without scale and with momentum contribution and sigmap=0.01

            //{-62.86640, 570.7671, -82.80870, 632.4669, -14.000, -14.000, 1., 0., 0., 91.2  }; // Startwerte
            //{-62.76640, 570.7671, -82.80870, 632.4669, -14.000, -14.000, 1., 0., 0., 91.2  }; // chi=17.8061488
            //without scale and with momentum contribution and sigmap=0.01
            {
                -62.76640, 570.7671, -82.80870, 632.4669, -14.000, -14.000, 1., 0., 0., 91.2
            }; // chi= without scale and with momentum contribution and sigmap=0.01

        // Double_t step[7] = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1, 0.1 };
        Double_t step[10] = { 0.1, 0.1, 0.1, 0.1, 0.01, 0.01, 0.001, 0.1, 0.1, 0.1 };
        // Double_t step[10] = { 0.01, 0.01, 0.01, 0.01, 0.001, 0.001, 0.0001, 0.01, 0.01, 0.01 };
        // Double_t step[10] = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 };

        mini->SetFunction(fm1);

        // Set the free variables to be minimized!
        mini->SetLimitedVariable(0, "x30", variable[0], step[0], variable[0] - 2.5, variable[0] + 2.5);
        mini->SetLimitedVariable(1, "z30", variable[1], step[1], variable[1] - 2.5, variable[1] + 2.5);
        mini->SetLimitedVariable(2, "x32", variable[2], step[2], variable[2] - 2.5, variable[2] + 2.5);
        mini->SetLimitedVariable(3, "z32", variable[3], step[3], variable[3] - 2.5, variable[3] + 2.5);
        mini->SetLimitedVariable(4, "dangle30", variable[4], step[4], variable[4] - 1.5, variable[4] + 1.5);
        mini->SetLimitedVariable(5, "dangle32", variable[5], step[5], variable[5] - 1.5, variable[5] + 1.5);
        mini->SetLimitedVariable(6, "scale", variable[6], step[6], variable[6] - 0.2, variable[6] + 0.2);
        mini->SetLimitedVariable(7, "xfi23", variable[7], step[7], variable[7] - 2., variable[7] + 2.);
        mini->SetLimitedVariable(8, "yfi23", variable[8], step[8], variable[8] - 2., variable[8] + 2.);
        mini->SetLimitedVariable(9, "zfi23", variable[9], step[9], variable[9] - 2., variable[9] + 2.);
        // mini->FixVariable(0);
        // mini->FixVariable(1);
        // mini->FixVariable(2);
        // mini->FixVariable(3);
        // mini->FixVariable(4);
        // mini->FixVariable(5);
        mini->FixVariable(6);
        // mini->FixVariable(7);
        // mini->FixVariable(8);
        // mini->FixVariable(9);
        // mini->FixVariable(10);
        // mini->FixVariable(11);
    }
    else
    {
        Double_t variable[10] =
            //{ -111.1057, 537.81830, -131.065, 599.57030, -194.0000, -194.000, 0.0 }; // Startwerte
            //{ -112.7125, 537.8314, -133.3002, 601.2894, -193.6788, -193.7728, 0.0 }; // chi=33.68796
            //{ -112.7125, 537.8314, -133.3002, 601.2894, -193.6688, -193.7728, 0.0, 0., 0., 91.054 }; // chi=33.6687
            //{ -112.5986, 538.1367, -133.3184, 601.1964, -193.6053, -193.6904, 0.06381685, -0.2035961,
            //-0.06938534, 91.10460 }; // chi=23.49 { -112.6991, 538.3473, -133.0637, 601.9667, -193.5340, -193.6714,
            //0.10522, -0.6832888, 0.077313, 91.3638 }; // chi = 13.6557 { -112.6991, 538.3473, -133.0637, 601.9667,
            //-193.5340, -193.6714, 0.10522, -0.6832888, -0.02268696, 91.3638 }; // chi = 374.472509 mit 0.001 beim
            //Impuls { -112.6823, 538.3641, -133.0637, 601.9834, -193.5327, -193.6697, 0.25, -0.65, 0.1, 91.37 }; // chi
            //= 343.5 mit 0.001 beim Impuls gleiche Werte für Fib23

            //{ -111.1057, 537.81830, -131.065, 599.57030, -194.0000, -194.000, 0., 0., 0., 91.2 }; // // Startwerte
            //{ -111.1057, 537.5165, -131.065, 599.7954, -192.8191, -195.1456, 0., 0., 0., 91.2 }; // chi=3.4601 without
            //momentum contribution { -111.1076, 537.5163, -131.0599, 599.8083, -192.7725, -195.2901, 0., 0., 0., 91.2
            //}; // chi=12.1109  with momentum contribution and sigmap = 0.001 { -111.1076, 537.5163, -131.0599,
            //599.8083, -192.7725, -195.2901, 0., 0., 0., 91.2 }; // chi=  with momentum contribution and sigmap = 0.001
            //and 1000 events { -111.1159, 537.5371, -131.0557, 599.8291, -192.6704, -195.2880, 1., 0., 0., 91.2 }; //
            //chi=  with momentum contribution and sigmap = 0.001 and 1000 events { -111.0873, 537.5657, -131.0271,
            //599.8577, -192.6675, -195.2851, 0.9899833, 0., 0., 91.2 }; // chi=6.6469 with scale with momentum
            //contribution and sigmap = 0.001 and 1000 events { -111.0873, 537.5657, -130.9271, 599.8577, -192.6675,
            //-195.2851, 0.9899833, 0., 0., 91.2 }; // chi=5.05003 with scale with momentum contribution and sigmap =
            //0.001 and 1000 events { -111.0659, 537.5871, -130.9057, 599.8791, -192.6654, -195.2830, 1.0, 0., 0., 91.2
            //}; // chi=8.0800 with scale with momentum contribution and sigmap = 0.0005 and 1000 events

            // after better settings for step size of px, py, pz
            //{ -112.5232, 537.4851, -131.8563, 597.7853, -192.4332, -195.1948, 1.0, 0., 0., 91.2 }; // chi=68.1675
            //without scale with momentum contribution and sigmap = 0.0005 and 1000 events { -112.5232, 537.5851,
            //-131.8563, 597.7853, -192.4332, -195.1948, 1.0, 0., 0., 91.2 }; // chi=66.7380 with scale with momentum
            //contribution and sigmap = 0.0005 and 1000 events

            //{ -111.1057, 537.81830, -131.065, 599.57030, -194.0000, -194.000, 1., 0., 0., 91.2 }; // Startwerte
            //{ -112.6733, 537.9038, -131.8324, 597.6741, -193.8265, -193.8729, 1., 0., 0., 91.2 };  // chi=68.6396
            //without scale and with momentum contribution and sigmap=0.0005 and 1000 events { -112.7233, 537.8955,
            //-131.7407, 597.6658, -193.8273, -193.8737, 1., 0., 0., 91.2 };  // chi=69.0987 with scale and with
            //momentum contribution and sigmap=0.0005 and 1000 events

            // so far best
            //{ -112.6733, 537.9038, -131.8124, 597.6741, -193.8265, -193.8699, 1., 0., 0., 91.2 };  // chi=67.76508
            //without scale and with momentum contribution and sigmap=0.0005 and 1000 events
            // now with exp data
            //{ -112.6733, 537.9038, -131.8124, 597.7741, -193.8265, -193.8699, 1., 0.0, 0.0, 91.04 };  // chi=6.625
            //without scale and with momentum contribution and sigmap=0.0005 and 1000 events

            // now magnetic field runs with tracks which hit fi23a
            //{ -111.1057, 537.81830, -131.065, 599.57030, -194.0000, -194.000, 1., 0., 0., 91.2 }; // Startwerte
            //{ -111.1635, 537.8805, -130.9028, 599.6325, -193.9938, -193.9838, 1.0004, -0.179992, 0.0, 91.26224 }; //
            //chi2=47.1085

            // after taking out py of optimization
            //{ -111.1057, 537.81830, -131.065, 599.57030, -194.0000, -194.000, 1., 0., 0., 91.2 }; // Startwerte
            //{ -111.4004, 537.9037, -130.9963, 599.6557, -193.9915, -193.9815, 1., 0., 0., 91.2 }; // chi2=2.233
            //sigmap=0.001 { -111.5004, 538.0370, -130.9630, 599.6890, -193.9882, -193.9782, 1., 0., 0., 91.2 }; //
            //chi2=4.09  sigmap=0.001 { -111.5004, 538.0370, -130.9630, 599.6890, -193.9882, -193.9782, 1., 0., 0., 91.2
            //}; // chi2=4.09  sigmap=0.001 { -112.8888, 538.6268, -132.9156, 600.7398, -193.9050, -194.1189, 1., 0.,
            //0., 91.2 }; // chi2=80.60  sigmap=0.0005

            //{ -112.9549, 538.6962, -132.8462, 600.9092, -193.9033, -194.1164, 1.0, -0.1067407, 0.1694, 91.13455 }; //
            //chi2=32.253  sigmap=0.001 { -112.9549, 538.6962, -132.8462, 600.9092, -193.8933, -194.1164, 1.0,
            //-0.1067407, 0.1694, 91.13455 }; // chi2=32.253  sigmap=0.001

            //{ -112.6111, 537.9599, -131.7388, 597.8256, -193.8284, -193.8707, 1., -0.09688162, 0.03705294, 90.95676 };
            //// chi=32.98 without scale and with momentum contribution and sigmap=0.0005 and 1000 events { -112.6680,
            //537.9992, -131.6654, 597.9097, -193.8226, -193.8636, 1., -0.1853324, 0.0, 90.94827 };  // chi=4.809
            //without momentum { -112.6680, 537.9992, -131.6654, 597.9097, -193.8226, -193.8636, 1., -0.1853324,
            //0.0, 91.04827 };  // chi=14.2448 without momentum { -112.5680, 537.9992, -131.6654, 597.9097, -193.8226,
            //-193.8636, 1., 0.0, 0.0, 91.04827 };  // chi=14.4558 without momentum

            //{ -111.1057, 537.81830, -131.065, 599.57030, -194.0000, -194.000, 1., 0., 0., 91.2 }; // Startwerte
            //{ -111.4555, 537.9061, -131.0184, 599.5773, -193.9958, -193.9929, 1.000603, 0., 0., 91.29286 }; //
            //chi=794.5951 sigmap=0.001 { -111.4976, 537.7347, -131.0320, 599.7352, -193.9927, -193.9956, 1.001850, 0.,
            //0., 91.18721 }; // chi=66.8406 sigmap=0.005 { -111.3156, 537.7682, -130.9262, 599.6937, -194.0051,
            //-193.9957, 1.001041, 0., 0., 91.18226 }; // chi=41.5022 sigmap=0.01 { -111.3622, 537.7989, -130.9471,
            //599.6882, -193.9471, -193.9992, 0.9999294, 0., 0., 91.26170 }; // chi= 25.69364 ohne sigmap

            // Now with the same scaling factor for left and right
            //{ -111.4976, 537.8347, -131.0320, 599.7352, -193.9927, -193.9956, 1.000625, 0., 0., 91.18721 }; //
            //chi=41.05772 sigmap=0.01 { -111.4969, 537.8354, -131.0313, 599.7359, -193.9926, -193.9955, 1.000625, 0.,
            //0.005, 91.2 }; // chi=39.58178 sigmap=0.01 { -111.4969, 537.8454, -131.0313, 599.7359, -193.9926,
            //-193.9955, 1.000625, 0., 0.005, 91.2 }; // chi= 38.90724 sigmap=0.01 { -111.4969, 537.9454, -131.0313,
            //599.7359, -193.9926, -193.9955, 1.000625, 0., 0.005, 91.2 }; // chi=38.656349 sigmap=0.01 { -111.4969,
            //537.9454, -131.0313, 599.7359, -193.9926, -193.9955, 1.000625, 0., 0.005, 91.21 }; // chi=38.571849
            //sigmap=0.01

            // now px, py fix and target point same for C and 4He, deltaxy=0.1
            //{ -111.4969, 537.9454, -131.0313, 599.7359, -193.9926, -193.9955, 1.000625, 0., 0.005, 91.21 }; //
            //chi=38.571849 sigmap=0.01

            //{ -112.6733, 537.9038, -131.8124, 597.7741, -193.8265, -193.8699, 1.001, 0.0, 0.1, 91.04 };  //
            //chi=2.0641595  without sigmap and 1000 events { -112.6733, 537.9038, -131.8124, 597.7741, -193.8265,
            //-193.8699, 1.001, 0.0, 0.0, 91.04 };  // chi=11.5507665  sigmap=0.01 and 1000 events { -112.6733,
            //537.9038, -131.8124, 597.8741, -193.8265, -193.8699, 1.001, 0.0, 0.0, 91.04 };  // chi=9.635242
            //sigmap=0.01 and 1000 events { -112.6704, 537.9067, -131.8224, 597.8770, -193.8262, -193.8696, 1.001, 0.0,
            //0.0, 91.04286 };  // chi=9.6526795  sigmap=0.01 and 1000 events { -112.6704, 537.9067, -131.8124,
            //597.8770, -193.8262, -193.8696, 1.001, 0.0, 0.0, 91.04286 };  // chi=9.59178  sigmap=0.01 and 1000 events
            //{ -112.6686, 537.9085, -131.8138, 597.8774, -193.8260, -193.8694, 1.002, 0.0, 0.0, 91.04470 };  //
            //chi=9.5285792  sigmap=0.01 and 1000 events { -112.6686, 537.9085, -131.8138, 597.8774, -193.8260,
            //-193.8694, 1.003, 0.0, 0.0, 91.05470 };  // chi= 9.507102  sigmap=0.01 and 1000 events { -112.6686,
            //537.9185, -131.8138, 597.8774, -193.8260, -193.8694, 1.004, 0.0, 0.0, 91.04470 };  // chi=9.4949080
            //sigmap=0.01 and 1000 events

            // deltaxy=0.01
            //{ -112.6610, 537.9404, -131.8001, 597.8993, -193.8238, -193.8672, 1.004, 0.0, 0.0, 91.01408 };  //
            //chi=11.4077875  sigmap=0.01 and 1000 events { -112.6566, 537.9548, -131.8077, 597.9137, -193.8224,
            //-193.8658, 1.004144, 0.01440588, 0.01440588, 91.00208 };  // chi=11.3846919  sigmap=0.01 and 1000 events
            //{ -112.6466, 537.9548, -131.8077, 597.9137, -193.8224, -193.8658, 1.004144, 0.01440588,
            //0.01440588, 91.00208 };  // chi=11.361154  sigmap=0.01 and 1000 events

            //{ -112.6502, 537.9412, -131.7951, 597.9001, -193.8237, -193.8651, 1.003, 0.0, 0.0, 91.04613 };  //
            //chi=11.3687475  sigmap=0.01 and 1000 events

            //{ -112.6789, 537.9168, -131.7994, 597.7871, -193.8252, -193.8686, 1.001, 0.00185, 0.012968, 91.05297 }; //
            //chi=10.1793675 without scale and with momentum contribution and sigmap=0.0005 and 1000 events { -112.6789,
            //537.9168, -131.7994, 597.7871, -193.8252, -193.8686, 1.0011, 0.00185, 0.012968, 91.05297 };  //
            //chi=10.1642725 without scale and with momentum contribution and sigmap=0.0005 and 1000 events { -112.6787,
            //537.9180, -131.7982, 597.7883, -193.8251, -193.8251, 1.001112, 0.00085, 0.013088, 91.05417 };  //
            //chi=10.155750 without scale and with momentum contribution and sigmap=0.0005 and 1000 events

            //{ -112.6787, 537.9280, -131.7982, 597.7883, -193.8251, -193.8251, 1.001, 0.014, 0.02, 91.06 };  //
            //chi=10.2093026 without scale and with momentum contribution and sigmap=0.0005 and 1000 events

            // best
            //{ -112.6704, 537.9361, -131.7968, 597.8064, -193.8233, -193.8233, 1.0, 0.014, 0.02, 91.06 };  //
            //chi=10.283307 without scale and with momentum contribution and sigmap=0.0005 and 1000 events

            // after shifting the fiber detectors by half a fiber and sigma target != 0 (v1)
            //{ -112.6568, 537.9497, -131.8079, 597.8200, -193.8219, -193.8219, 1.0, 0.01647017, 0.03358581, 91.07359 };
            //// chi=4.573087871 without scale and with momentum contribution and sigmap=0.01 { -112.6512, 537.9614,
            //-131.8073, 597.8317, -193.8207, -193.8207, 1.0, 0.02820339, 0.04531903, 91.08532 };  // chi=4.5542152564
            //without scale and with momentum contribution and sigmap=0.01 { -112.6512, 537.9614, -131.8073, 597.8317,
            //-193.8207, -193.8207, 1.0, 0.0820339, 0.04531903, 91.08532 };  // chi=4.53375333 without scale and with
            //momentum contribution and sigmap=0.01 { -112.6479, 537.9647, -131.8273, 597.8350, -193.8204,
            //-193.8204, 1.0, 0.08536725, 0.04865238, 91.08865 };  // chi=4.4667076165 without scale and with momentum
            //contribution and sigmap=0.01

            //{ -112.6429, 537.9697, -131.8473, 597.8400, -193.8199, -193.8199, 1.0, 0.085, 0.0486, 91.085 };  //
            //chi=4.382976767 without scale and with momentum contribution and sigmap=0.01
            // 19th opti
            //{ -112.6429, 537.9697, -131.8473, 597.8400, -193.8199, -193.8199, 1.0, 0.085, 0.0486, 91.085 };  //
            //chi=4.382976787 without scale and with momentum contribution and sigmap=0.01

            // after shifting the fiber detectors by half a fiber and sigma target = 0
            //{ -112.6568, 537.9497, -131.8079, 597.8200, -193.8219, -193.8219, 1.0, 0.01647, 0.0335858, 91.17359 };  //
            //chi=4.35052984 without scale and with momentum contribution and sigmap=0.01 { -112.6512, 537.9614,
            //-131.8073, 597.8317, -193.8207, -193.8207, 1.0, 0.02820322, 0.045319, 91.18532 };  // chi=4.611918177
            //without scale and with momentum contribution and sigmap=0.01 { -112.6456, 537.9731, -131.8067, 597.8434,
            //-193.8195, -193.8195, 1.0, 0.03993644, 0.05705222, 91.19705 };  // chi=4.49821991 without scale and with
            //momentum contribution and sigmap=0.01 { -112.6400, 537.9848, -131.8061, 597.8551, -193.8183,
            //-193.8183, 1.0, 0.05166966, 0.06878544, 91.20878 };  // chi=4.4651826173 without scale and with momentum
            //contribution and sigmap=0.01 { -112.6344, 537.9965, -131.8055, 597.8668, -193.8171, -193.8171, 1.0,
            //0.06340288, 0.08051866, 91.22051 };  // chi=4.454226697 without scale and with momentum contribution and
            //sigmap=0.01 { -112.6344, 537.9965, -131.8055, 597.8668, -193.8171, -193.8171, 1.0, 0.06340288,
            //0.08051866, 91.32051 };  // chi=4.43927885 without scale and with momentum contribution and sigmap=0.01 {
            //-112.6244, 537.9965, -131.8055, 597.8668, -193.8171, -193.8171, 1.0, 0.06340288, 0.08051866, 91.32051 };
            //// chi=4.420378756 without scale and with momentum contribution and sigmap=0.01 { -112.6244, 537.9965,
            //-131.8055, 597.8668, -193.8171, -193.8171, 1.0, 0.07340288, 0.08051866, 91.32051 };  // chi=4.4041055
            //without scale and with momentum contribution and sigmap=0.01

            //{ -112.6211, 537.9998, -131.8155, 597.8701, -193.8168, -193.8168, 1.0, 0.073, 0.0805, 91.321 };  //
            //chi=4.400292866 without scale and with momentum contribution and sigmap=0.01
            // 18th opti
            //{ -112.6211, 537.9998, -131.8155, 597.8701, -193.8158, -193.8168, 1.0, 0.073, 0.0805, 91.321 };  //
            //chi=4.3970204 without scale and with momentum contribution and sigmap=0.01

            // new data set sigma target = 0.5
            //{ -112.6186, 538.0134, -131.8019, 597.8837, -193.8144, -193.8154, 1.0, 0.06188431, 0.09408581, 91.33459 };
            //// chi=14.908077996 without scale and with momentum contribution and sigmap=0.01 { -112.6130, 538.0251,
            //-131.7902, 597.8954, -193.8132, -193.8142, 1.0, 0.06250185, 0.1058190, 91.34632 };  // chi=12.692429667
            //without scale and with momentum contribution and sigmap=0.01 { -112.6105, 538.0387, -131.7766, 597.9090,
            //-193.8118, -193.8128, 1.0, 0.05138816, 0.1194048, 91.35991 };  // chi=14.72121481 without scale and with
            //momentum contribution and sigmap=0.01 { -112.6080, 538.0523, -131.7630, 597.9226, -193.8104,
            //-193.8114, 1.0, 0.04027247, 0.1329906, 91.37350 };  // chi=14.58486956 without scale and with momentum
            //contribution and sigmap=0.01

            // fib23 z fix
            //{ -112.6074, 538.0543, -131.7610, 597.9246, -193.8102, -193.8112, 1.0, 0.03888303, 0.1350362, 91.2 };  //
            //chi=14.56910959 without scale and with momentum contribution and sigmap=0.01

            //{ -111.1057, 537.8183, -131.065, 599.57030, -194.0000, -194.0000, 1., 0., 0., 91.2 }; // Startwerte
            {
                -111.3552, 537.7937, -130.9549, 599.5581, -193.9950, -193.9980, 1., 0.1100647, -0.00012172, 91.26071
            }; // chi=14.67405552 without scale and with momentum contribution and sigmap=0.01

        // Double_t step[7] = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1, 0.1 };
        Double_t step[10] = { 0.1, 0.1, 0.1, 0.1, 0.01, 0.01, 0.001, 0.1, 0.1, 0.1 };
        // Double_t step[10] = { 0.01, 0.01, 0.01, 0.01, 0.001, 0.001, 0.0001, 0.01, 0.01, 0.01 };
        // Double_t step[10] = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 };
        // Double_t step[10] = { 0.2, 0.5, 0.2, 0.5, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 };

        mini->SetFunction(fm1);

        // Set the free variables to be minimized!
        mini->SetLimitedVariable(0, "x31", variable[0], step[0], variable[0] - 2.5, variable[0] + 2.5);
        mini->SetLimitedVariable(1, "z31", variable[1], step[1], variable[1] - 2.5, variable[1] + 2.5);
        mini->SetLimitedVariable(2, "x33", variable[2], step[2], variable[2] - 2.5, variable[2] + 2.5);
        mini->SetLimitedVariable(3, "z33", variable[3], step[3], variable[3] - 2.5, variable[3] + 2.5);
        mini->SetLimitedVariable(4, "dangle31", variable[4], step[4], variable[4] - 1.5, variable[4] + 1.5);
        mini->SetLimitedVariable(5, "dangle33", variable[5], step[5], variable[5] - 1.5, variable[5] + 1.5);
        mini->SetLimitedVariable(6, "scale", variable[6], step[6], variable[6] - 0.2, variable[6] + 0.2);
        mini->SetLimitedVariable(7, "xfi23", variable[7], step[7], variable[7] - 2., variable[7] + 2.);
        mini->SetLimitedVariable(8, "yfi23", variable[8], step[8], variable[8] - 2., variable[8] + 2.);
        mini->SetLimitedVariable(9, "zfi23", variable[9], step[9], variable[9] - 2., variable[9] + 2.);
        // mini->FixVariable(0);
        // mini->FixVariable(1);
        // mini->FixVariable(2);
        // mini->FixVariable(3);
        // mini->FixVariable(4);
        // mini->FixVariable(5);
        mini->FixVariable(6);
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
    cout << "scale: " << mini->X()[6] << endl;
    cout << "dx23: " << mini->X()[7] << endl;
    cout << "dy23: " << mini->X()[8] << endl;
    cout << "dz23: " << mini->X()[9] << endl;
    // cout << "angleX: " << mini->X()[10] << endl;
    // cout << "angleZ: " << mini->X()[11] << endl;
    // cout << "field: " << mini->X()[9] << endl;
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
