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
#include "TRandom.h"

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
    , fForward(kTRUE)
    , fBfield(-1710.)
    , fFitter(nullptr)
    , fEnergyLoss(kTRUE)
    , fSimu(kTRUE)
{
    // this is the list of detectors (active areas) we use for tracking
    /*
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
    */
    fDetectors->AddDetector("target", kTarget, "TargetGeoPar");
    fDetectors->AddDetector("fi23a", kTargetGlad, "fi23aGeoPar", "Fi23aHit");
    fDetectors->AddDetector("fi23b", kTargetGlad, "fi23bGeoPar", "Fi23bHit");
    fDetectors->AddDetector("fi31", kAfterGlad, "fi31GeoPar", "Fi31Hit");
    fDetectors->AddDetector("fi30", kAfterGlad, "fi30GeoPar", "Fi30Hit");
    fDetectors->AddDetector("fi33", kAfterGlad, "fi33GeoPar", "Fi33Hit");
    fDetectors->AddDetector("fi32", kAfterGlad, "fi32GeoPar", "Fi32Hit");
    fDetectors->AddDetector("tofd", kTof, "tofdGeoPar", "TofdHit");
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
    fSide = 3; // 0 = front, 1 = left, 2 = right
    FairRootManager* man = FairRootManager::Instance();

    fArrayMCTracks = (TClonesArray*)man->GetObject("MCTrack");
    if (NULL == fArrayMCTracks)
    {
        LOG(error) << "No MC Track array found in input file.";
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

    // fDetectorsLeft->Init();
    // fDetectorsRight->Init();
    fDetectors->Init();

    // For the moment we have to change manually between the setup
    gSetup = fDetectors;
    // gSetup = fDetectorsLeft;
    // gSetup = fDetectorsRight;

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

    // fDetectorsLeft->SetParContainers();
    // fDetectorsRight->SetParContainers();
    fDetectors->SetParContainers();
}

void R3BOptimizeGeometryS494::Exec(const Option_t*)
{

    // For the moment one has to swap manually between the setups
    /*
        //R3BTrackingDetector* target = fDetectorsLeft->GetByName("target");
        R3BTrackingDetector* fi23a = fDetectorsLeft->GetByName("fi23a");
        R3BTrackingDetector* fi23b = fDetectorsLeft->GetByName("fi23b");
        R3BTrackingDetector* fi30 = fDetectorsLeft->GetByName("fi30");
        R3BTrackingDetector* fi31 = fDetectorsRight->GetByName("fi31");
        R3BTrackingDetector* fi32 = fDetectorsLeft->GetByName("fi32");
        R3BTrackingDetector* fi33 = fDetectorsRight->GetByName("fi33");
        R3BTrackingDetector* tof = fDetectorsLeft->GetByName("tofd");
    */

    R3BTrackingDetector* target = fDetectors->GetByName("target");
    R3BTrackingDetector* fi23a = fDetectors->GetByName("fi23a");
    R3BTrackingDetector* fi23b = fDetectors->GetByName("fi23b");
    R3BTrackingDetector* fi31 = fDetectors->GetByName("fi31");
    R3BTrackingDetector* fi30 = fDetectors->GetByName("fi30");
    R3BTrackingDetector* fi33 = fDetectors->GetByName("fi33");
    R3BTrackingDetector* fi32 = fDetectors->GetByName("fi32");
    R3BTrackingDetector* tof = fDetectors->GetByName("tofd");

    if (tof->fArrayHits->GetEntriesFast() < 1)
        return;
    if (fi23a->fArrayHits->GetEntriesFast() < 1)
        return;
    if (fi23b->fArrayHits->GetEntriesFast() < 1)
        return;
    if (fi30->fArrayHits->GetEntriesFast() + fi32->fArrayHits->GetEntriesFast() + fi31->fArrayHits->GetEntriesFast() +
            fi33->fArrayHits->GetEntriesFast() <
        4)
        return;

    // fDetectorsLeft->CopyToBuffer();
    fDetectors->CopyToBuffer();
    cout << "Saved events: " << gThisTask->GetNEvents() << endl;
    fNEvents += 1;
}

double Chi2AllEvents(const double* xx)
{

    // For the moment ...
    Int_t fSide = 3;
    Double_t chi2_all = 0;
    Double_t fi23x, fi23y, fi23z, fi30x, fi30z, fi30a, fi31x, fi31z, fi31a, fi32x, fi32z, fi32a, fi33x, fi33z, fi33a,
        tofdx, tofdy, tofdz;

    Int_t nof = 0;
    cout.precision(7);
    cout << "new correction: " << xx[0] << "  " << xx[1] << "  " << xx[2] << "  " << xx[3] << "  " << xx[4] << "  "
         << xx[5] << "  " << xx[6] << "  " << xx[7] << "  " << xx[8] << "  " << xx[9] << "  " << xx[10] << "  "
         << xx[11] << "  " << xx[12] << "  " << xx[13] << "  " << xx[14] << "  " << xx[15] << "  " << endl;

    fi23x = 0.00000000;
    fi23y = 0.0000000;
    // fi23z = 91.2;
    fi23z = 91.04;

    fi30x = -62.18069;
    fi30z = 570.59330;
    // fi30a = -13.68626;
    fi30a = -14.;

    fi31x = -112.67330;
    fi31z = 537.9038;
    // fi31a = -193.8265;
    fi31a = -194.;

    fi32x = -82.41713;
    fi32z = 632.9688;
    // fi32a = -14.02967;
    fi32a = -14.;

    fi33x = -131.8124;
    fi33z = 597.7741;
    // fi33a = -193.8699;
    fi33a = -194.;

    tofdx = -129.300;
    tofdy = 0.0;
    tofdz = 685.4;

    if (fSide == 3) // infront
    {
        fi23x = xx[0];
        fi23y = xx[1];
        fi23z = xx[2];
        fi30x = xx[3];
        fi30z = xx[4];
        fi32x = xx[5];
        fi31x = xx[6];
        fi31z = xx[7];
        fi33x = xx[8];
        tofdx = xx[9];
        tofdy = xx[10];
        tofdz = xx[11];
        fi30a = xx[12];
        fi31a = xx[13];
        fi32a = xx[14];
        fi33a = xx[15];
    }

    fi32z = fi30z + 62.77813349;
    fi33z = fi31z + 62.77813349;
    // simu:
    fi32z = fi30z + 62.3755;
    fi33z = fi31z + 59.8702;

    // for break-up run we need this line
    //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(xx[6]);

    //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(xx[15]);
    // Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
    // Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
    // cout << "Field:" << field << " scale: " << scale << endl;

    R3BTrackingDetector* fi23a = gSetup->GetByName("fi23a");

    fi23a->pos0 = TVector3(0., 0., 0.);
    fi23a->pos1 = TVector3(5., 5., 0.);
    fi23a->pos2 = TVector3(-5., 5., 0.);

    fi23a->pos0.RotateY(0. * TMath::DegToRad());
    fi23a->pos1.RotateY(0. * TMath::DegToRad());
    fi23a->pos2.RotateY(0. * TMath::DegToRad());

    TVector3 trans3a(fi23x, 0., fi23z - 0.015);

    fi23a->pos0 += trans3a;
    fi23a->pos1 += trans3a;
    fi23a->pos2 += trans3a;
    fi23a->norm = ((fi23a->pos1 - fi23a->pos0).Cross(fi23a->pos2 - fi23a->pos0)).Unit();

    R3BTrackingDetector* fi23b = gSetup->GetByName("fi23b");

    fi23b->pos0 = TVector3(0., 0., 0.);
    fi23b->pos1 = TVector3(5., 5., 0.);
    fi23b->pos2 = TVector3(-5., 5., 0.);

    fi23b->pos0.RotateY(0. * TMath::DegToRad());
    fi23b->pos1.RotateY(0. * TMath::DegToRad());
    fi23b->pos2.RotateY(0. * TMath::DegToRad());

    TVector3 trans3b(0., fi23y, fi23z + 0.015);

    fi23b->pos0 += trans3b;
    fi23b->pos1 += trans3b;
    fi23b->pos2 += trans3b;
    fi23b->norm = ((fi23b->pos1 - fi23b->pos0).Cross(fi23b->pos2 - fi23b->pos0)).Unit();

    R3BTrackingDetector* fi30 = gSetup->GetByName("fi30");
    fi30->pos0 = TVector3(0., 0., 0.);
    fi30->pos1 = TVector3(25., 25., 0.);
    fi30->pos2 = TVector3(-25., 25., 0.);

    fi30->pos0.RotateY(fi30a * TMath::DegToRad());
    fi30->pos1.RotateY(fi30a * TMath::DegToRad());
    fi30->pos2.RotateY(fi30a * TMath::DegToRad());

    TVector3 trans30(fi30x, 0., fi30z);

    fi30->pos0 += trans30;
    fi30->pos1 += trans30;
    fi30->pos2 += trans30;
    fi30->norm = ((fi30->pos1 - fi30->pos0).Cross(fi30->pos2 - fi30->pos0)).Unit();

    R3BTrackingDetector* fi31 = gSetup->GetByName("fi31");

    fi31->pos0 = TVector3(0., 0., 0.);
    fi31->pos1 = TVector3(25., 25., 0.);
    fi31->pos2 = TVector3(-25., 25., 0.);

    fi31->pos0.RotateY(fi31a * TMath::DegToRad());
    fi31->pos1.RotateY(fi31a * TMath::DegToRad());
    fi31->pos2.RotateY(fi31a * TMath::DegToRad());
    TVector3 trans31(fi31x, 0., fi31z);

    fi31->pos0 += trans31;
    fi31->pos1 += trans31;
    fi31->pos2 += trans31;
    fi31->norm = ((fi31->pos1 - fi31->pos0).Cross(fi31->pos2 - fi31->pos0)).Unit();

    R3BTrackingDetector* fi32 = gSetup->GetByName("fi32");

    fi32->pos0 = TVector3(0., 0., 0.);
    fi32->pos1 = TVector3(25., 25., 0.);
    fi32->pos2 = TVector3(-25., 25., 0.);

    fi32->pos0.RotateY(fi32a * TMath::DegToRad());
    fi32->pos1.RotateY(fi32a * TMath::DegToRad());
    fi32->pos2.RotateY(fi32a * TMath::DegToRad());

    TVector3 trans32(fi32x, 0., fi32z);

    fi32->pos0 += trans32;
    fi32->pos1 += trans32;
    fi32->pos2 += trans32;
    fi32->norm = ((fi32->pos1 - fi32->pos0).Cross(fi32->pos2 - fi32->pos0)).Unit();

    R3BTrackingDetector* fi33 = gSetup->GetByName("fi33");
    fi33->pos0 = TVector3(0., 0., 0.);
    fi33->pos1 = TVector3(25., 25., 0.);
    fi33->pos2 = TVector3(-25., 25., 0.);

    fi33->pos0.RotateY(fi33a * TMath::DegToRad());
    fi33->pos1.RotateY(fi33a * TMath::DegToRad());
    fi33->pos2.RotateY(fi33a * TMath::DegToRad());

    TVector3 trans33(fi33x, 0., fi33z);

    fi33->pos0 += trans33;
    fi33->pos1 += trans33;
    fi33->pos2 += trans33;
    fi33->norm = ((fi33->pos1 - fi33->pos0).Cross(fi33->pos2 - fi33->pos0)).Unit();

    R3BTrackingDetector* tofd = gSetup->GetByName("tofd");

    tofd->pos0 = TVector3(0., 0., 0.);
    tofd->pos1 = TVector3(60., 60., 0.);
    tofd->pos2 = TVector3(-50., 50., 0.);

    tofd->pos0.RotateY(18. * TMath::DegToRad());
    tofd->pos1.RotateY(18. * TMath::DegToRad());
    tofd->pos2.RotateY(18. * TMath::DegToRad());

    TVector3 transtofd(tofdx, tofdy, tofdz);

    tofd->pos0 += transtofd;
    tofd->pos1 += transtofd;
    tofd->pos2 += transtofd;
    tofd->norm = ((tofd->pos1 - tofd->pos0).Cross(tofd->pos2 - tofd->pos0)).Unit();

    for (Int_t iev = 0; iev < gThisTask->GetNEvents(); iev++)
    {
        gSetup->TakeHitsFromBuffer(iev);
        Double_t fieldScale;

        // für break-up events wieder rein

        //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(xx[15]);
        // Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
        // Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
        // cout << "Field:" << field << " scale: " << scale << endl;

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
    R3BTrackingDetector* fi31 = gSetup->GetByName("fi31");
    R3BTrackingDetector* fi30 = gSetup->GetByName("fi30");
    R3BTrackingDetector* fi33 = gSetup->GetByName("fi33");
    R3BTrackingDetector* fi32 = gSetup->GetByName("fi32");
    R3BTrackingDetector* tof = gSetup->GetByName("tofd");

    if (target->hits.size() < 1)
        target->hits.push_back(new R3BHit(0, 0.0, 0.0, 0., 0., 0));

    // if (tof->hits.size() > 0 && debug)
    {
        // cout << "*************** NEW EVENT ****" << fNEvents << ", " << fNEvents_nonull << endl;
        cout << "Hits ToFD: " << tof->hits.size() << endl;
        cout << "Hits right: " << fi23a->hits.size() << "  " << fi23b->hits.size() << "  " << fi31->hits.size() << "  "
             << fi33->hits.size() << endl;
        cout << "Hits left: " << fi23a->hits.size() << "  " << fi23b->hits.size() << "  " << fi32->hits.size() << "  "
             << fi30->hits.size() << endl;

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

    // Start values
    Double_t beta = 0.;
    Double_t beta0 = 0.;
    Double_t x0 = 0.;
    Double_t y0 = 0.;
    Double_t z0 = 0.;
    Double_t px0 = 0;
    Double_t py0 = 0.;
    Double_t pz0 = 0.;
    Double_t p = 0.;
    Double_t mass = 0.;

    Double_t betaHe = 0.;
    Double_t x0He = 0.;
    Double_t y0He = 0.;
    Double_t z0He = 0.;
    Double_t px0He = 0;
    Double_t py0He = 0.;
    Double_t pz0He = 0.;
    Double_t pHe = 0.;
    Double_t massHe = 0.;

    Double_t betaC = 0.;
    Double_t x0C = 0.;
    Double_t y0C = 0.;
    Double_t z0C = 0.;
    Double_t px0C = 0;
    Double_t py0C = 0.;
    Double_t pz0C = 0.;
    Double_t pC = 0.;
    Double_t massC = 0.;

    Double_t betaO = 0.;
    Double_t x0O = 0.;
    Double_t y0O = 0.;
    Double_t z0O = 0.;
    Double_t px0O = 0;
    Double_t py0O = 0.;
    Double_t pz0O = 0.;
    Double_t pO = 0.;
    Double_t massO = 0.;

    Double_t pBeamz = 17.3915;
    Double_t pzmem = 0.;
    Double_t pymem = 0.;
    Double_t xmem = 0.;
    Double_t ymem = 0.;
    Double_t zmem = 0.;
    Double_t yC = -1000;
    Double_t yCexp = -1000;
    Double_t yCfib = -1000;
    Double_t yCfibexp = -1000;
    Double_t masmem = -1000;
    Double_t chargemem = -1000.;
    Double_t AoverZmem = -1000.;
    Double_t pCx = -1000., pCy = -1000., pCz = -10000.;
    Double_t pHex = -1000., pHey = -1000., pHez = -10000.;
    Int_t ixtmax;
    Double_t Erel = 0;

    Double_t AoverZ = 2.;

    // Important: Set charge and initial position and momentum of the particle
    R3BTrackingParticle* particle = new R3BTrackingParticle(10., x0, y0, z0, px0, py0, pz0, beta, mass);

    // try to fit all possible combination of hits.

    fPropagator->SetVis(kFALSE);

    Int_t nCand = 0;

    Double_t mChi2 = -1;
    Double_t yChi2 = -1;
    Double_t zChi2 = -1;

    Int_t ifi23a = 0;
    Int_t ifi23b = 0;
    Int_t ifi30 = 0;
    Int_t ifi31 = 0;
    Int_t ifi32 = 0;
    Int_t ifi33 = 0;
    Int_t itof = 0;
    if (0 == fi23a->hits.size())
    {
        ifi23a = -1;
    }
    if (0 == fi23b->hits.size())
    {
        ifi23b = -1;
    }
    if (0 == fi30->hits.size())
    {
        ifi30 = -1;
    }
    if (0 == fi31->hits.size())
    {
        ifi31 = -1;
    }
    if (0 == fi32->hits.size())
    {
        ifi32 = -1;
    }
    if (0 == fi33->hits.size())
    {
        ifi33 = -1;
    }
    if (0 == tof->hits.size())
    {
        itof = -1;
    }
    Int_t charge_mem = 0;
    Bool_t alpha = kFALSE;
    Bool_t carbon = kFALSE;
    Bool_t oxygen = kFALSE;
    Bool_t writeOutC = kFALSE;

    R3BTrackingParticle* candidate;
    // The idea is to loop twice over the ToF wall hits.
    // First we want to look for 12C particle and then for 4He

    Int_t Icountleft = 0;
    Int_t Icountright = 0;

    Int_t lmin;
    Int_t lmax;
    if (fBfield != -1710.0)
    {
        lmin = 0;
        lmax = 1;
    }
    else
    {
        lmin = 1;
        lmax = 3;
    }
    // cout << "Test l: " << lmin << "  " << lmax << "  " << fBfield << endl;

    for (Int_t l = lmin; l < lmax; l++)
    {
        // l = 0: 16O
        // l = 1: 12C
        // l = 1: 4He

        Int_t charge = 0;
        Int_t charge_requested = 0;
        Double_t Charge = 0;
        Double_t m0 = 0.;
        Double_t p0 = 0.;

        if (l == 0)
        {
            charge_requested = 8;
        }
        else if (l == 1)
        {
            charge_requested = 6;
        }
        else if (l == 2)
        {
            charge_requested = 2;
        }

        // Loop over all combinations of hits
        for (Int_t i = 0; i < tof->hits.size(); i++) // loop over all ToFD hits
        {
            if (fSimu)
            {
                // For tracking of simulations:
                //   charge = sqrt(tof->hits.at(i)->GetEloss()) * 26.76 + 0.5;
                //   Charge = sqrt(tof->hits.at(i)->GetEloss()) * 26.76;

                // if digi used
                Charge = tof->hits.at(i)->GetEloss();
                charge = (int)(Charge + 0.5);

                // if digiHit used
                //  charge = (int)(tof->hits.at(i)->GetEloss() * 88.196293 + 1.5024036 + 0.5);
                //  Charge = tof->hits.at(i)->GetEloss() * 88.196293 + 1.5024036;
            }
            else
            {
                // For tracking of exp. data:
                charge = (int)(tof->hits.at(i)->GetEloss() + 0.5);
                Charge = tof->hits.at(i)->GetEloss();
            }
            if (debug)
                cout << "Charge: " << charge << " requested charge: " << charge_requested << endl;

            if (charge != charge_requested)
                continue;

            beta0 = 0.7593; // velocity could eventually be calculated from ToF
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
                m0 = 11.17486339;
                // m0 = 11.178;
                p0 = 13.043625;
                if (!fSimu)
                    pC = p0;
                massC = m0;
                if (fSimu)
                {
                    //	x0 = x0C;
                    // y0 = y0C;
                }
            }
            if (charge == 2)
            {
                m0 = 3.728401291;
                p0 = 4.347875;
                if (!fSimu)
                    pHe = p0;
                massHe = m0;
                if (fSimu)
                {
                    // x0 = x0He;
                    // y0 = y0He;
                }
            }

            if (debug)
            {
                cout << "Mass: " << m0 << endl;
                cout << "Position on TofD: " << tof->hits.at(i)->GetX() << endl;
            }
            /*
            if (!tof->free_hit[i]) // if the hit was used already, continue
            {
                if (debug)
                    cout << "ToFD hit already used" << endl;
                continue;
            }
            */

            if (tof->hits.at(i)->GetX() > 0 && fi30->hits.size() > 0 && fi32->hits.size() > 0 &&
                fi23a->hits.size() > 0 && fi23b->hits.size() > 0)
            {
                // left branch in beam direction, don't consider hits in the detectors of the other side

                if (!fSimu)
                {
                    //( (R3BGladFieldMap*) FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(1.002);
                }
                // simu from 01/02/22:
                if (fSimu)
                {
                    /*   ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleY(-14.);
                       ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionZ(174.95);
                       ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionY(1.08);
                       ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleZ(0.);
                       ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(1.00);*/
                }

                // R3BTrackingDetector* target = fDetectorsLeft->GetByName("target");
                if (fNEventsLeft == 0)
                {
                    // target->hits.push_back(new R3BHit(0, 0.0, 0.0, 0., 0., 0));
                }

                do // fi32
                {
                    do // fi30
                    {
                        do // fi23b
                        {

                            do // fi23a
                            {

                                x0 = 0.;
                                y0 = 0.;
                                z0 = 0.;

                                // Create object for particle which will be fitted
                                if (l < 2)
                                {
                                    candidate = new R3BTrackingParticle(charge, x0, y0, z0, 0., 0., p0, beta0, m0);
                                }
                                else
                                {
                                    candidate =
                                        new R3BTrackingParticle(charge, xmem, ymem, zmem, 0., 0., p0, beta0, m0);
                                }

                                if (debug)
                                {
                                    cout << "left side of setup" << endl;
                                    cout << "Charge requested: " << charge_requested << endl;
                                    cout << "Start values to fit, x0: " << x0 << " y0: " << y0 << " z0: " << z0
                                         << " p0: " << p0 << " beta0: " << beta0 << " m0: " << m0 << endl;
                                    cout << "Hit Tofd # " << i << " x: " << tof->hits.at(i)->GetX()
                                         << " y: " << tof->hits.at(i)->GetY() << endl;
                                }
                                if (ifi23a > -1 && debug)
                                    cout << " Fi23a left # " << ifi23a << " x: " << fi23a->hits.at(ifi23a)->GetX()
                                         << endl;
                                if (ifi23b > -1 && debug)
                                    cout << " left Fi23b # " << ifi23b << " y: " << fi23b->hits.at(ifi23b)->GetY()
                                         << endl;
                                if (ifi30 > -1 && debug)
                                    cout << " fi30 # " << ifi30 << " x: " << fi30->hits.at(ifi30)->GetX() << endl;
                                if (ifi32 > -1 && debug)
                                    cout << " fi32 # " << ifi32 << " x: " << fi32->hits.at(ifi32)->GetX() << endl;
                                // add points through which tracker has to go:
                                candidate->AddHit("target", 0);
                                candidate->AddHit("tofd", i);
                                candidate->AddHit("fi23a", ifi23a);
                                candidate->AddHit("fi23b", ifi23b);
                                candidate->AddHit("fi32", ifi32);
                                candidate->AddHit("fi30", ifi30);

                                // fDetectors = fDetectorsLeft;

                                Int_t status = 10;
                                if (fForward)
                                {
                                    status = fFitter->FitTrackMomentumForward(candidate, fDetectors);
                                }
                                else
                                {
                                    // status = fFitter->FitTrackBackward2D(candidate, fDetectors);
                                    status = fFitter->FitTrackMomentumBackward(candidate, fDetectors);
                                }
                                if (debug)
                                    cout << " Chi: " << candidate->GetChi2() << "  "
                                         << candidate->GetStartMomentum().Mag() << "  "
                                         << 1000. * (candidate->GetStartMomentum().Mag() - p0) *
                                                (candidate->GetStartMomentum().Mag() - p0)
                                         << endl;
                                if (debug)
                                    cout << "--------------------------------" << endl;
                                nCand += 1;
                                Icountleft += 1;

                                //     cout << fNEvents<< ", LEFT SIDE: Charge: "<< charge<<", Momentum: " <<
                                //     candidate->GetMomentum().Mag()<<
                                //  ", Momentum Z: "<<candidate->GetMomentum().Z() <<
                                //   ", Momentum X: "<<candidate->GetMomentum().X()<< endl;

                                if (TMath::IsNaN(candidate->GetMomentum().Z()))
                                {
                                    delete candidate;
                                    continue;
                                }

                                if (10 > status)
                                {
                                    if (fForward)
                                    {
                                        candidate->Reset();
                                    }
                                    else
                                    {
                                        candidate->SetStartPosition(candidate->GetPosition());
                                        candidate->SetStartMomentum(-1. * candidate->GetMomentum());
                                        candidate->SetStartBeta(beta0);
                                        candidate->UpdateMomentum();

                                        // candidate->SetStartPosition(candidate->GetPosition()); // @target
                                        // candidate->SetStartMomentum(-1. * candidate->GetMomentum());
                                        // candidate->SetStartBeta(0.8328);
                                        // candidate->SetStartBeta(beta0);
                                        // candidate->UpdateMomentum();
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

                        ifi30 += 1;
                    } while (ifi30 < fi30->hits.size());
                    ifi30 = 0;
                    if (0 == fi30->hits.size())
                        ifi30 = -1;

                    ifi32 += 1;
                } while (ifi32 < fi32->hits.size());
                ifi32 = 0;
                if (0 == fi32->hits.size())
                    ifi32 = -1;
            } // end if left branch

            if (tof->hits.at(i)->GetX() < 0 && fi31->hits.size() > 0 && fi33->hits.size() > 0 &&
                fi23a->hits.size() > 0 && fi23b->hits.size() > 0)
            {
                // right branch in beam direction, don't consider hits in the detectors of the other side

                if (!fSimu)
                {
                    //( (R3BGladFieldMap*) FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(1.004);
                }
                // simu from 01/02/22:
                if (fSimu)
                {
                    /*  ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleY(-14.);
                      ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionZ(174.95);
                      ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionY(1.08);
                      ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionAngleZ(0.);
                      ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(1.00);*/
                }

                // R3BTrackingDetector* target = fDetectorsRight->GetByName("target");
                if (fNEventsRight == 0)
                {
                    // R3BHit(detId, x,  y, eloss,  time, hitId = -1);
                    // target->hits.push_back(new R3BHit(0, 0.0, 0.0, 0., 0., 0));
                }

                do // fi33
                {
                    do // fi31
                    {
                        do // fi23b
                        {
                            do // fi23a
                            {

                                x0 = 0.;
                                y0 = 0.;
                                z0 = 0.;

                                // Create object for particle which will be fitted
                                if (l < 2)
                                {
                                    candidate = new R3BTrackingParticle(charge, x0, y0, z0, 0., 0., p0, beta0, m0);
                                }
                                else
                                {
                                    candidate =
                                        new R3BTrackingParticle(charge, xmem, ymem, zmem, 0., 0., p0, beta0, m0);
                                }

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
                                    cout << "Fi23a # " << ifi23a << " x: " << fi23a->hits.at(ifi23a)->GetX() << endl;
                                if (ifi23b > -1 && debug)
                                    cout << "Fi23b # " << ifi23b << " y: " << fi23b->hits.at(ifi23b)->GetY() << endl;
                                if (ifi33 > -1 && debug)
                                    cout << "Fi33 # " << ifi33 << " x: " << fi33->hits.at(ifi33)->GetX() << endl;
                                if (ifi31 > -1 && debug)
                                    cout << "Fi31  # " << ifi31 << " x: " << fi31->hits.at(ifi31)->GetX() << endl;

                                candidate->AddHit("target", 0);
                                candidate->AddHit("tofd", i);
                                candidate->AddHit("fi23a", ifi23a);
                                candidate->AddHit("fi23b", ifi23b);
                                candidate->AddHit("fi31", ifi31);
                                candidate->AddHit("fi33", ifi33);

                                // fDetectors = fDetectorsRight;

                                Int_t status = 10;
                                if (fForward)
                                {
                                    status = fFitter->FitTrackMomentumForward(candidate, fDetectors);
                                }
                                else
                                {
                                    // status = fFitter->FitTrackBackward2D(candidate, fDetectors);
                                    status = fFitter->FitTrackMomentumBackward(candidate, fDetectors);
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
                                Icountright += 1;

                                //    cout <<fNEvents<<", RIGHT SIDE: Charge: "<< charge<<", Momentum: " <<
                                //    candidate->GetMomentum().Mag()<<", Momentum Z:
                                //    "<<candidate->GetMomentum().Z()<<
                                //   ", Momentum X: "<<candidate->GetMomentum().X() << " status: "<<status<<endl;

                                if (TMath::IsNaN(candidate->GetMomentum().Z()))
                                {
                                    delete candidate;
                                    continue;
                                }

                                if (10 > status)
                                {
                                    if (fForward)
                                    {
                                        candidate->Reset();
                                    }
                                    else
                                    {
                                        candidate->SetStartPosition(candidate->GetPosition());
                                        candidate->SetStartMomentum(-1. * candidate->GetMomentum());
                                        candidate->SetStartBeta(beta0);
                                        candidate->UpdateMomentum();

                                        // candidate->SetStartPosition(candidate->GetPosition());
                                        // candidate->SetStartMomentum(-1. * candidate->GetMomentum());
                                        // candidate->SetStartBeta(0.8328);
                                        // candidate->SetStartBeta(beta0);
                                        // candidate->UpdateMomentum();

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

            charge_mem = charge;

        } // end for TofD

        charge = charge_mem;
        Charge = float(charge_mem);

        // if (candidate->GetSize() > 0 && !fSimu)
        {
            // candidate->Clear();
        }

        //   if(candidate) delete candidate;

        fh_ncand->Fill(nCand);

        R3BTrackingParticle* bestcandidate =
            new R3BTrackingParticle(-1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000);
        minChi2 = 1e10;
        Double_t pChi2 = 0.;
        Double_t parChi2 = 0.;
        Double_t xChi2 = 0.;

        if (fFragments.size() > 0)
        {
            for (auto const& x : fFragments)
            {
                pChi2 = (x->GetStartMomentum().Mag() - p0) * (x->GetStartMomentum().Mag() - p0);
                xChi2 = x->GetChi2();

                //  parChi2 = sqrt(pChi2 * pChi2 + xChi2 * xChi2 * 100.);
                parChi2 = xChi2;

                if (l < 2) // 12C or 16O
                {
                    if (parChi2 < minChi2)
                    {
                        bestcandidate = x;
                        minChi2 = parChi2;
                        pzmem = x->GetStartMomentum().Z();
                        xmem = x->GetStartPosition().X();
                        ymem = x->GetStartPosition().Y();
                        zmem = x->GetStartPosition().Z();
                        if (l == 1)
                        {
                            pCx = x->GetStartMomentum().X() * 1000.;
                            pCy = x->GetStartMomentum().Y() * 1000.;
                            pCz = x->GetStartMomentum().Z() * 1000.;
                            carbonP.SetPxPyPzE(
                                pCx, pCy, pCz, sqrt(pow(pCx, 2) + pow(pCy, 2) + pow(pCz, 2) + pow(mC, 2)));
                            p12C = carbonP.Vect();
                            // cout << "For event: "<<fNEvents_nonull<<" new min chi2 for C: " << minChi2 <<endl;
                            minChi2_12C = minChi2;
                        }
                        // cout << "New min chi2: " << minChi2 << endl;
                        // cout << "Corresponding Mass   : " << x->GetMass() << endl;
                        // cout << "Corresponding Mass   : " << bestcandidate->GetMass() << endl;
                    }
                }
                if (l == 2) // 4He
                {
                    pHex = x->GetStartMomentum().X() * 1000.;
                    pHey = x->GetStartMomentum().Y() * 1000.;
                    pHez = x->GetStartMomentum().Z() * 1000.;
                    alphaP.SetPxPyPzE(pHex, pHey, pHez, sqrt(pow(pHex, 2) + pow(pHey, 2) + pow(pHez, 2) + pow(mHe, 2)));
                    Double_t theta_26 = alphaP.Angle(carbonP.Vect()) * TMath::RadToDeg(); // angle alpha carbon (theta)
                    Double_t m_inva = (alphaP + carbonP).M();                             // invariant mass
                    Erel = m_inva - mHe - mC;                                             // relative Energy
                    Double_t Erel_check = 3.87359e-01 - 3.55075e-01 * theta_26 + 6.57182e-01 * theta_26 * theta_26;

                    // cout<<"4He selecting: "<<endl;
                    // cout << "Erel: " << Erel << "; chi2 C/He: " << minChi2_12C <<", "<<minChi2 << endl;
                    // cout<<"candidate: "<<x->GetStartMomentum().Z()<<", "<<x->GetStartPosition().X()<<", "<<
                    // x->GetStartPosition().Y()<<endl;

                    // cout << "For event: "<<fNEvents_nonull<<" possible He: " << parChi2 << ", Erel: "<<Erel<<",
                    // "<<Erel_check<<endl;

                    if (parChi2 < minChi2)
                    // if (abs(Erel-Erel_check) < 1.1  && parChi2 < minChi2)
                    {
                        // if (parChi2 < minChi2)
                        {
                            bestcandidate = x;
                            minChi2 = parChi2;

                            // cout << "For event: "<<fNEvents_nonull<<" new min chi2 for He: " << minChi2 << ", Erel:
                            // "<<Erel<<", "<<Erel_check<<endl;

                            // cout << "Corresponding Mass   : " << x->GetMass() << endl;
                            // cout << "Corresponding Mass   : " << bestcandidate->GetMass() << endl;
                        }
                    }
                    //	cout<<"selected: "<<bestcandidate->GetStartMomentum().Z()<<",
                    //"<<bestcandidate->GetStartPosition().X()<<", "<< 	bestcandidate->GetStartPosition().Y()<<",
                    //"<<minChi2<<", "<<Erel<<endl;
                }
            }

            if (minChi2 > 1.e5)
                continue;

            if (bestcandidate->GetStartMomentum().X() < 0)
            {
                fi23a->free_hit[bestcandidate->GetHitIndexByName("fi23a")] = false;
                fi23b->free_hit[bestcandidate->GetHitIndexByName("fi23b")] = false;
                fi31->free_hit[bestcandidate->GetHitIndexByName("fi31")] = false;
                fi33->free_hit[bestcandidate->GetHitIndexByName("fi33")] = false;
            }
            else
            {
                fi23a->free_hit[bestcandidate->GetHitIndexByName("fi23a")] = false;
                fi23b->free_hit[bestcandidate->GetHitIndexByName("fi23b")] = false;
                fi30->free_hit[bestcandidate->GetHitIndexByName("fi30")] = false;
                fi32->free_hit[bestcandidate->GetHitIndexByName("fi32")] = false;
            }
            tof->free_hit[bestcandidate->GetHitIndexByName("tofd")] = false;

            Double_t x0soll = 0.;
            Double_t y0soll = 0.;
            Double_t z0soll = 0.;
            Double_t psoll = 0.;
            Double_t px0soll = 0.;
            Double_t py0soll = 0.;
            Double_t pz0soll = 0.;
            Double_t beta0soll = 0.;
            Double_t m0soll = 0.;

            if (l == 0)
            {
                if (debug)
                    cout << "16O" << endl;
                ;
                oxygen = kTRUE;
                if (fSimu)
                {
                    x0soll = x0O;
                    y0soll = y0O;
                    z0soll = z0O;
                    px0soll = px0O;
                    py0soll = py0O;
                    pz0soll = pz0O;
                    psoll = pO;
                    m0soll = massO;
                    beta0soll = betaO;
                }
                else
                {
                    x0soll = x0;
                    y0soll = y0;
                    z0soll = z0;
                    px0soll = 0.;
                    py0soll = 0.;
                    pz0soll = 17.3915;
                    psoll = 17.3915;
                    m0soll = m0;
                    beta0soll = 0.7593209;
                }
            }

            if (l == 1)
            {
                if (debug)
                    cout << "12C" << endl;

                carbon = kTRUE;
                x0soll = x0C;
                y0soll = y0C;
                z0soll = z0C;
                px0soll = px0C;
                py0soll = py0C;
                pz0soll = pz0C;
                psoll = pC;
                m0soll = massC;
                beta0soll = betaC;
                counterC += 1;
                chargemem = Charge;
            }

            if (l == 2)
            {
                if (debug)
                    cout << "4He" << endl;
                ;
                alpha = kTRUE;
                x0soll = xmem; // x0He;
                y0soll = ymem; // y0He;
                z0soll = zmem; // z0He;
                px0soll = px0He;
                py0soll = py0He;
                pz0soll = pz0He;
                psoll = pHe;
                m0soll = massHe;
                beta0soll = betaHe;
                counterHe += 1;
            }
            if (debug)
            {
                cout << "Results after tracking :" << endl;
                cout << "Charge   : " << charge << endl;
                cout << "Position (soll) x: " << x0soll << " y: " << y0soll << " z: " << z0soll << endl;
                cout << "Position (ist)  x: " << bestcandidate->GetStartPosition().X()
                     << " y: " << bestcandidate->GetStartPosition().Y()
                     << " z: " << bestcandidate->GetStartPosition().Z() << endl;

                cout << "Momentum (soll): " << psoll << " px : " << px0soll << " py: " << py0soll << " pz: " << pz0soll
                     << endl;
                cout << "Momentum (ist) : " << bestcandidate->GetStartMomentum().Mag()
                     << " px : " << bestcandidate->GetStartMomentum().X()
                     << " py: " << bestcandidate->GetStartMomentum().Y()
                     << " pz: " << bestcandidate->GetStartMomentum().Z() << endl;
                cout << "Erel: " << Erel << "; chi2 C/He: " << minChi2_12C << ", " << minChi2 << endl;

                //   cout << "Beta   : " << bestcandidate->GetStartBeta() << endl;
                // LOG(debug) << "chi2: " << bestcandidate->GetChi2() << endl;
                cout << "chi: " << minChi2 << endl;
            }

            if (minChi2 < 1.e4)
            {
                totalChi2Mass += minChi2;
                totalEvents++;
            }
            totalChi2P += minChi2;

            fPropagator->SetVis(fVis);
            bestcandidate->Reset();
        }
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

    } // end for two particle (12C and 4He)

    Double_t psum = sqrt(pow(pHex, 2) + pow(pHey, 2) + pow(pHez, 2)) + sqrt(pow(pCx, 2) + pow(pCy, 2) + pow(pCz, 2));

    /*
        cout << "chi2 12C: " << minChi2_12C  << endl;
        cout << "Momentum 12C: " << pCx << "  " << pCy << "  " << pCz << endl;
        cout << "chi2 4He: " << minChi2 << endl;
        cout << "Momentum 4He: " << pHex << "  " << pHey << "  " << pHez << endl;
        cout << "Sum momentum: " << psum << endl;
    */

    Double_t ps = 17391.5;
    mChi2 = sqrt(minChi2_12C * minChi2_12C + minChi2 * minChi2);
    Double_t pchi = pow(psum - ps, 2) / (pow(ps, 2) * 0.01 * 0.01);

    // cout << "chi2: " << mChi2 << "  chi2 12C: " << minChi2_12C << "  chi2 4He: " << minChi2
    //	 << "  diff momentum: " << psum - ps << "  pchi2: " << pchi << endl;

    mChi2 = sqrt(mChi2 * mChi2 + pchi * pchi);
    // cout << "final chi2: " << mChi2 << endl;
    if (mChi2 < 1000000.)
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
    mini->SetMaxFunctionCalls(1000); // for Minuit/Minuit2
    mini->SetMaxIterations(100);     // for GSL
    mini->SetTolerance(10.);
    mini->SetPrintLevel(1);

    // create funciton wrapper for minmizer
    // a IMultiGenFunction type
    ROOT::Math::Functor* fm1 = new ROOT::Math::Functor(&Chi2AllEvents, 16);
    mini->SetFunction(*fm1);
    // ROOT::Math::Functor* fm = new ROOT::Math::Functor(&Chi2MomentumForward, 6);
    // minimum_m->SetFunction(*fm);

    // Double_t step[6] = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
    // Double_t step[6] = { 0.5, 0.5, 0.1, 0.5, 0.5, 0.1};
    // Double_t step[6] = { 0.05, 0.05, 0.5, 0.1, 0.1, 0.1};
    // Double_t step[12] = { 0.04, 0.04, 0.1, 0.2, 0.5, 0.2, 0.2, 0.5, 0.2, 1., 0.5, 1.0};
    // Double_t step[12] = { 0.02, 0.02, 0.05, 0.1, 0.2, 0.1, 0.1, 0.2, 0.2, 0.5, 0.1, 0.5};

    Double_t step[16] = { 0.04, 0.04, 0.1, 0.2, 0.5, 0.2, 0.2, 0.5, 0.2, 1., 0.5, 1.0, 0.1, 0.1, 0.1, 0.1 };

    Double_t variable[16] =
        // optimizing simulation:
        //{0.00000000, 0.0000000, 91.04, -62.18069, 570.59330, -82.41713, 632.9688, -112.67330, 537.9038, -131.8124,
        // 597.7741, -129.300, 0.000000, 685.4}; // Sollwerte
        // Fit with original values
        // sigmap=0.01
        //{0.000000, 0.000000, 91.04000, -62.18069, 570.5933, -82.41713, 632.9688, -112.6733, 537.9038, -131.8124,
        // 597.7741, -129.3000, 0.000000, 685.4000}; // chi2= 0.142730675204785967

        // sigmap=0.001
        //{0.000000, 0.1000000, 91.04000, -62.18069, 570.5933, -82.41713, 632.9688, -112.6733, 537.9038, -131.8124,
        // 597.7741, -129.3000, 0.000000, 685.4000}; // chi2= 4.12500310858948804 {0.000000, 0.000000, 91.04000,
        //-62.18069, 570.5933, -82.41713, 632.9688, -112.6733, 537.9038, -131.8124, 597.7741, -129.3000, 0.000000,
        // 685.4000}; // chi2=4.07227540674969646 mit Winkel 14% fest

        // sigmap=0.0001
        //{-0.001179510, 0.01208862, 91.04000, -62.17542, 570.7196, -82.40230, 632.9888, -112.6699, 537.9187, -131.8102,
        // 597.7924, -129.2392, 0.06082249, 685.4608}; // chi2=408.1240

        // Mit falschen Startwerten
        // sigmap=0.0001
        //{0.02408969, 0.04569157, 91.04000, -62.26778, 571.1855, -82.76687, 634.2639, -112.3088, 537.5762, -132.0604,
        // 599.3395, -128.3451, 2.475059, 684.8408}; // chi2=410.3349

        // sigmap=0.001
        //{0.02437795, 0.04567445, 91.04000, -62.26776, 571.1845, -82.76679, 634.2639, -112.3084, 537.5762, -132.0604,
        // 599.3390, -128.3452, 2.474973, 684.8407}; // chi2= 4.26020079814127062

        // sigmap=0.01
        //{0.01453212, 0.03342885, 91.04000, -62.28075, 571.1616, -82.70578, 634.2163, -112.3135, 537.5297, -132.0830,
        // 599.2899, -128.4068, 2.413361, 684.7791}; // chi2=0.319770475910918728

        // optimizing simulation:
        //{0.00000000, 0.0000000, 91.04, -62.18069, 570.59330, -82.41713, 632.9688, -112.67330, 537.9038, -131.8124,
        // 597.7741, -129.300, 0.000000, 685.4}; // Sollwerte {0.000000, 0.000000, 91.20000, -62.73862, 570.8949,
        //-82.88055, 632.5947, -111.2138, 537.3539, -130.9372, 599.6981, -129.8087, 0.1277786, 685.5209}; // chi2=
        // 0.881279731277969658 {0.000000, 0.000000, 91.20000, -62.73862, 570.8949, -82.88055, 632.5947, -111.2138,
        // 537.3539, -130.9372, 599.6981, -129.3087, 0.1277786, 685.5209}; // chi2=0.660139196776171611 {0.000000,
        // 0.000000, 91.20000, -62.73862, 570.8949, -82.88055, 632.5947, -111.2138, 537.3539, -130.9372, 599.6981,
        //-129.3087, 0.1277786, 686.0209}; // chi2=0.650199401917536779 {0.000000, 0.000000, 91.04000, -62.73862,
        // 570.8949, -82.88055, 632.5947, -111.2138, 537.3539, -130.9372, 599.6981, -129.3087, 0.1277786, 686.5209}; //
        // chi2=  0.655454000437383444 only fi23z fixed and closer to the real value {0.000000, 0.000000, 91.04000,
        //-62.73862, 570.8949, -82.88055, 632.5947, -111.2138, 537.3539, -130.9372, 599.6981, -129.3087, 0.6277786,
        // 686.5209}; // chi2= 0.649709592424370097 even with the correct value for fi23z we do not get the correct
        // valus behing GLAD

        //{0.000000, 0.000000, 91.04000, -62.83862, 571.3949, -82.88055, 632.5947, -111.2138, 537.3539, -130.9372,
        // 599.6981, -129.3087, 0.6277786, 686.5209}; // chi2=0.913052830465099774 {0.000000, 0.000000, 91.04000,
        //-62.63862, 570.8949, -82.88055, 633.0947, -111.2138, 537.3539, -130.9372, 599.6981, -129.3087, 0.6277786,
        // 686.5209}; // chi2=0.713576737021483609 {0.1000000, 0.000000, 91.04000, -62.33862, 570.8949, -82.48055,
        // 632.5947, -110.8138, 537.3539, -130.5372, 599.6981, -129.3087, 0.6277786, 686.5209}; // chi2=
        // 0.637351347044025629 {0.1032655, 0.003265522, 91.04817, -62.33658, 570.9360, -82.46421, 632.6358, -110.8281,
        // 537.3950, -130.5209, 599.7392, -129.2247, 0.6688702, 686.6049}; // chi2=0.546765993861350941 {0.1069975,
        // 0.006997547, 91.05750, -62.15292, 570.9470, -82.24553, 632.6828, -111.0094, 537.4420, -130.7329, 599.7862,
        //-129.1287, 0.7158314, 686.7009}; // chi2=0.442740413512922648 {0.1107295, 0.01072957, 91.06683, -61.96926,
        // 570.9940, -82.02685, 632.7298, -111.1907, 537.4890, -130.9449, 599.7972, -129.0327, 0.7627926, 686.7969}; //
        // chi2=0.357039981932632644 {0.1507295, 0.01072957, 91.06683, -61.76926, 570.9940, -81.82685, 632.7298,
        //-110.9907, 537.4890, -130.7449, 599.7972, -129.0327, 0.7627926, 686.7969}; // chi2=0.336486000337920865
        //{0.1407295, -0.04072957, 91.06683, -61.76926, 570.9940, -81.82685, 632.7298, -110.9907, 537.4890, -130.7449,
        // 599.7972, -129.0327, 0.7627926, 686.7969}; // chi2=0.314052372849565276 {0.1438930, -0.03756610, 91.02474,
        //-61.77641, 571.0338, -81.81102, 632.7696, -110.9892, 537.5288, -130.7291, 599.8370, -128.9514, 0.8026002,
        // 686.8782}; // chi2=0.306935437933759436 {0.1438930, -0.01756610, 91.02474, -61.77641, 571.0338, -81.81102,
        // 632.7696, -110.9892, 537.5288, -130.7291, 599.8370, -128.9514, 0.8026002, 686.8782}; //
        // chi2=0.306239936401970947 {0.1484857, -0.01472132, 91.00688, -61.49154, 571.0014, -81.53623, 632.9052,
        //-111.3138, 537.7175, -131.0351, 599.7697, -128.7941, 0.8964599, 687.0999}; // chi2=0.258352008051687343
        //{0.1599213, -0.005736641, 91.02631, -61.21216, 570.9318, -81.25690, 633.0555, -111.6348, 537.9153, -131.3451,
        // 599.7363, -128.7975, 0.9767267, 687.2231}; // chi2=0.198430834308950005

        //{0.000000, 0.001364421, 91.04407, -61.70979, 571.0212, -81.75453, 633.1449, -112.1146, 538.0047, -131.8249,
        // 599.8257, -128.6149, 1.066082, 687.4057}; // chi2= 0.511481664469694230 {0.000000, 0.04136442, 91.04407,
        //-61.70979, 571.0212, -81.75453, 633.1449, -112.1146, 538.0047, -131.8249, 599.8257, -128.6149, 1.066082,
        // 687.4057}; // chi2=0.511614096777162741 {0.04000000, 0.04136442, 91.04407, -61.50979, 571.0212, -81.55453,
        // 633.1449, -111.9146, 538.0047, -131.6249, 599.8257, -128.6149, 1.066082, 687.4057}; //
        // chi2=0.463494751108864511 {0.04000000, 0.04136442, 91.04407, -61.50979, 570.0212, -81.55453, 632.1449,
        //-111.9146, 537.0047, -131.6249, 598.8257, -128.6149, 1.066082, 687.4057}; // chi2=0.463494751108864511

        //{0.00000000, 0.0000000, 91.04, -62.18069, 570.59330, -82.41713, 632.9688, -112.67330, 537.9038, -131.8124,
        // 597.7741, -129.300, 0.000000, 685.4}; // Sollwerte {0.02111439, 0.01881943, 91.05172, -61.38909, 570.3240,
        //-81.61317, -111.7939, 537.3075, -131.8179, -128.6265, 1.368918, 687.5616}; // chi2=0.718554535874488032
        //{0.02111439, 0.01881943, 91.05172, -62.38909, 570.3240, -82.61317, -112.5939, 537.3075, -131.8179,
        //-128.6265, 1.368918, 687.5616}; // chi2=2.69415610349340273 {0.02111439, 0.01881943, 91.05172, -62.38909,
        // 570.8240, -82.61317, -112.5939, 537.3075, -131.8179, -128.6265, 1.368918, 687.5616}; //
        // chi2= 1.89754985946848298 {0.02111439, 0.01881943, 91.05172, -62.38909, 571.3240, -82.61317, -112.5939,
        // 537.3075, -131.8179, -128.6265, 1.368918, 687.5616}; // chi2=1.47928247009374281 {0.02111439,
        // 0.01881943, 91.05172, -62.38909, 571.8240, -82.61317, -112.5939, 537.3075, -131.8179, -128.6265, 1.368918,
        // 687.5616}; // chi2=1.33848420329257412 {0.02111439, 0.05881943, 91.05172, -62.38909, 571.8240, -82.61317,
        //-112.5939, 537.3075, -131.8179, -128.6265, 1.368918, 687.5616}; // chi2= 1.32885467892933851 {0.02111439,
        // 0.05881943, 91.05172, -62.38909, 571.3240, -82.61317, -112.5939, 537.3075, -131.8179, -128.6265, 1.368918,
        // 687.5616}; // chi2=1.47607599896616137 {0.02111439, 0.05881943, 91.15172, -62.38909, 571.0240, -82.61317,
        //-112.5939, 537.3075, -131.8179, -128.6265, 1.368918, 687.5616}; // chi2=1.65895644057430003 {0.02111439,
        // 0.05881943, 91.25172, -62.38909, 570.7240, -82.61317, -112.5939, 537.3075, -131.8179, -128.6265, 1.368918,
        // 687.5616}; // chi2=1.65321215057258231

        //{0.00000000, 0.0000000, 91.04, -62.18069, 570.59330, -82.41713, -112.67330, 537.9038, -131.8124, -129.300,
        // 0.000000, 685.4, -14.0000, -194.000, -14.00000, -194.0000}; // Sollwerte {0.03283771, 0.1088873, 91.19843,
        //-61.38693, 570.3240, -81.51328, -112.2161, 536.8294, -131.4037, -130.0280, -0.8484640, 686.4270}; //
        // chi2=0.245016814816687689
        {
            0.000000,  0.000000,  91.04000, -62.18069, 571.0933,  -82.41713, -112.6733, 537.9038,
            -131.8124, -129.3000, 0.000000, 685.4000,  -14.00000, -194.0000, -14.00000, -194.0000
        }; // chi2= 0.220908828713118205

    // optimizing data:
    //{0.0000, 0.0000, 91.2000, -62.8664, 570.7671, -82.8087, 632.4669, -111.1057, 537.8183, -131.0650, 599.5703,
    //-129.9365, 0.0000, 685.3931 }; // Startwerte
    //**{0.003105026, 0.003013546, 91.00973, -62.60724, 571.6605, -83.09290, -112.4505, 537.8837, -132.5744,
    //-129.7832, 1.450881, 686.0178}; // chi2=6.49095336433652381 {0.003105026, 0.003013546, 91.00973, -62.60724,
    // 571.6605, -83.09290, -111.0505, 537.8837, -132.0744, -129.7832, 1.450881, 686.0178}; // chi2=6.49095336433652381

    //{0.05595744, 0.02689265, 91.08238, -62.57482, 571.9951, -83.06653, -111.8807, 538.0327, -131.9033,
    //-129.5164, 2.018317, 686.3882, -13.80000, -193.8000, -14.00000, -193.9000}; // chi2=6.51506744992757358

    //{0.003105026, 0.003013546, 91.05973, -62.60724, 571.6605, -83.09290, -112.0505, 537.8837, -132.0744,
    //-129.7832, 1.450881, 686.5178, -13.50000, -194.0000, -13.90000, -194.0000}; // chi2=6.54484704948382490
    //{0.003105026, 0.003013546, 91.05973, -62.60724, 571.6605, -83.09290, -112.0505, 537.8837, -132.0744,
    //-129.7832, 1.450881, 686.5178, -13.50000, -194.0000, -13.90000, -194.0000}; // chi2=6.54484704948382490

    // 23th opti
    //{0.007419462, 0.007327982, 91.02052, -62.59230, 571.7103, -83.07132, -112.4498, 538.1335, -132.5532,
    //-129.6732, 1.492008, 686.1278, -13.89355, -193.8936, -13.99355, -193.9936}; // chi2=6.47629440483378715
    //**{0.007419462, 0.007327982, 91.02052, -62.59230, 571.7103, -83.07132, -112.4498, 538.1335, -132.5532,
    //-129.6732, 1.492008, 686.1278, -13.89355, -193.8936, -13.99355, -193.9936}; // chi2=6.47629440483378715

    //{0.003105026, 0.003013546, 91.20973, -62.60724, 571.6605, -83.09290, -112.4505, 538.0837, -132.5744,
    //-129.7832, 1.450881, 686.0178, -13.80000, -193.9000, -14.00000, -194.0000}; // chi2=6.48060878289542863
    //{0.003105026, 0.003013546, 91.20973, -62.60724, 571.6605, -83.09290, -112.4505, 538.0837, -132.5744,
    //-129.7832, 1.450881, 686.0178, -13.80000, -193.9000, -14.00000, -194.0000}; // chi2=6.48060878289542863

    // 24th opti
    //{0.02685527, 0.006763794, 91.21911, -62.60515, 571.9077, -83.07413, -111.8890, 537.8799, -131.8845,
    //-129.6868, 1.498072, 686.1142, -13.30000, -194.0000, -14.00000, -194.0000}; // chi2=6.54738938100870627
    //{0.02685527, 0.006763794, 91.21911, -62.60515, 571.9077, -83.07413, -111.8890, 537.8799, -131.8845,
    //-129.6868, 1.498072, 686.1142, -13.30000, -193.9000, -14.00000, -194.0000}; // chi2=6.54537379263417218

    //{0.04546912, 0.005469112, 91.21368, -61.50426, 569.4236, -81.91508, -111.3277, 536.2884, -131.3374, -129.7959,
    // 0.5688213, 686.5337, -13.78632, -193.9864, -13.98632, -193.9864}; // chi2=6.67285888283632023 {0.04546912,
    // 0.005469112, 91.21368, -61.50426, 569.4236, -81.91508, -111.3277, 536.2884, -131.3374, -129.7959, 1.068821,
    // 686.5337, -13.78632, -193.9864, -13.98632, -193.9864}; // chi2=6.66015462590739293

    //{0.04000000, 0.000000, 91.20000, -61.89551, 568.0617, -82.48430, -112.0670, 539.4183, -131.9824, -129.9365,
    // 0.000000, 685.3931, -13.40000, -194.0000, -14.00000, -194.0000}; // chi2=6.91643990899193106 {0.04000000,
    // 0.000000, 91.20000, -61.89551, 568.0617, -82.48430, -112.0670, 539.4183, -131.9824, -129.9365, 0.000000,
    // 685.3931, -13.30000, -194.0000, -14.00000, -194.0000}; // chi2=6.90326018132230690

    //{0.08273456, 0.002734556, 91.20684, -62.59202, 571.3701, -83.13345, -111.2996, 537.8069, -131.2216, -129.8662,
    // 0.5344107, 686.4634, -13.79316, -193.9932, -13.99316, -193.9932}; // chi2=6.83228495327764573 {0.08273456,
    // 0.002734556, 91.20684, -62.59202, 571.3701, -83.13345, -111.2996, 537.8069, -131.2216, -129.8662, 1.034411,
    // 686.4634, -13.79316, -193.9932, -13.99316, -193.9932}; // chi2=6.82022721839787582

    //{0.002578296, 0.002578296, 91.20645, -61.93748, 569.9297, -82.37411, -112.0235, 537.9715, -132.0360,
    //-129.8702, 1.532444, 686.4594, -13.49355, -193.9936, -13.99355, -193.9936}; // chi2=6.58617352952353485
    //{0.002578296, 0.002578296, 91.20645, -61.93748, 569.9297, -82.37411, -112.0235, 537.9715, -132.0360,
    //-129.8702, 1.532444, 686.4594, -13.39355, -193.9936, -13.99355, -193.9936}; // chi2=6.56639618190188923

    //{0.08558631, 0.005586308, 91.21397, -61.55766, 570.4371, -81.93620, -111.4078, 537.6437, -131.3479, -128.7929,
    // 0.07029598, 685.5367, -13.67958, -193.9797, -13.97958, -193.9797}; // chi2=6.65153844181366161 {0.08558631,
    // 0.005586308, 91.21397, -61.55766, 570.4371, -81.93620, -111.4078, 537.6437, -131.3479, -128.7929, 0.5702960,
    // 685.5367, -13.67958, -193.9797, -13.97958, -193.9797}; // chi2=6.64103471120423183

    //{0.002578296, 0.002578296, 91.20645, -61.93748, 569.9297, -82.37411, -112.0235, 537.9715, -132.0360, -129.8702,
    // 0.03244446, 686.4594, -13.89355, -193.9936, -13.99355, -193.9936}; // chi2=6.68150342585423029 {0.002578296,
    // 0.002578296, 91.20645, -61.93748, 569.9297, -82.37411, -112.0235, 537.9715, -132.0360, -129.8702, 0.03244446,
    // 686.4594, -13.79355, -193.9936, -13.99355, -193.9936}; // chi2=6.66350367672652411

    //{0.005312852, 0.005312852, 91.21329, -62.22269, 568.8977, -82.82109, -111.2812, 536.7523, -131.2213, -129.7999,
    // 0.5668551, 685.5297, -13.98671, -193.8868, -13.98671, -193.9868}; // chi2=7.00000387469867835 {0.005312852,
    // 0.005312852, 91.21329, -62.22269, 568.8977, -82.82109, -111.2812, 536.7523, -131.2213, -129.7999, 1.066855,
    // 685.5297, -13.98671, -193.8868, -13.98671, -193.9868}; // chi2=6.98743663590845454

    // 22th opti
    //{0.02998811, 0.002034778, 91.00775, -62.10974, 570.9135, -82.52534, -112.1957, 538.4522, -132.2102,
    //-130.3123, 1.357143, 685.6236, -13.69511, -14.06870, -193.8646, -193.7331}; // chi2=6.68156341422219846
    //{0.02998811, 0.002034778, 91.00775, -62.10974, 570.9135, -82.52534, -112.1957, 538.4522, -132.2102,
    //-129.3123, 1.357143, 685.6236, -13.69511, -14.06870, -193.8646, -193.7331}; // chi2=6.58021963114203867

    //{0.000000,  0.000000,  91.20000,  -63.23128,  571.4778,  -83.86853,   -111.5392,  535.7847,  -131.6388, -129.9365,
    // 0.000000,  685.3931, -14.0000, -194.0000, -14.00000, -194.0000}; {0.000000, 0.000000, 91.20000, -63.23128,
    // 571.4778, -83.86853, -111.5392, 535.7847, -131.6388, -129.9365, 0.5000000, 685.3931, -14.00000, -194.0000,
    //-14.00000, -194.0000}; // chi2=6.83519091773565179 {0.000000, 0.000000, 91.20000, -63.23128, 571.4778, -83.86853,
    //-111.5392, 535.7847, -131.6388, -129.9365, 1.000000, 685.3931, -14.00000, -194.0000, -14.00000, -194.0000}; //
    // chi2=6.82371932248600999 {0.000000, 0.000000, 91.20000, -63.23128, 571.4778, -83.86853, -111.5392, 535.7847,
    //-131.6388, -129.9365, 1.500000, 685.3931, -14.00000, -194.0000, -14.00000, -194.0000}; // chi2=6.81062792238942372
    //{0.000000, 0.000000, 91.20000, -63.23128, 571.4778, -83.86853, -111.5392, 535.7847, -131.6388,
    //-129.9365, 1.500000, 685.3931, -14.00000, -193.9000, -14.00000, -194.0000}; // chi2=6.80076678303024007 {0.000000,
    // 0.000000, 91.20000, -63.23128, 571.4778, -83.86853, -111.5392, 535.7847, -131.6388, -129.9365, 1.500000,
    // 685.3931, -14.00000, -193.8000, -14.00000, -194.0000}; // chi2=6.78917172804679314 {0.000000, 0.000000, 91.20000,
    //-63.23128, 571.4778, -83.86853, -111.5392, 535.7847, -131.6388, -129.9365, 1.500000, 685.3931, -14.00000,
    // -193.7000, -14.00000, -194.0000}; // chi2=6.77825079469429692 {0.000000, 0.000000, 91.20000, -63.23128, 571.4778,
    //-83.86853, -111.5392, 535.7847, -131.6388, -129.9365, 1.500000, 685.3931, -13.90000, -193.7000, -14.00000,
    //-194.0000}; // chi2=6.76919540135359021

    //{0.000000,  0.000000,  91.20000,  -63.23128,  571.4778,  -83.86853,   -112.4498, 538.1335, -132.5532,  -129.9365,
    // 0.000000,  685.3931, -14.0000, -194.0000, -14.00000, -194.0000}; {0.000000, 0.000000, 91.20000, -63.23128,
    // 571.4778, -83.86853, -112.4498, 538.1335, -132.5532, -129.9365, 0.5000000, 685.3931, -14.00000, -194.0000,
    //-14.00000, -194.0000}; // chi2=6.76820737833888852 {0.000000, 0.000000, 91.20000, -63.23128, 571.4778, -83.86853,
    //-112.4498, 538.1335, -132.5532, -129.9365, 1.000000, 685.3931, -14.00000, -194.0000, -14.00000, -194.0000}; //
    // chi2=6.75681442931180065 {0.000000, 0.000000, 91.20000, -63.23128, 571.4778, -83.86853, -112.4498, 538.1335,
    //-132.5532, -129.9365, 1.500000, 685.3931, -14.00000, -194.0000, -14.00000, -194.0000}; // chi2=6.74358098651590421
    //{0.000000, 0.000000, 91.20000, -63.23128, 571.4778, -83.86853, -112.4498, 538.1335, -132.5532,
    //-129.9365, 1.500000, 685.3931, -13.90000, -194.0000, -14.00000, -194.0000}; // chi2=6.73433354495062186 {0.000000,
    // 0.000000, 91.20000, -63.23128, 571.4778, -83.86853, -112.4498, 538.1335, -132.5532, -129.9365, 1.500000,
    // 685.3931, -13.80000, -194.0000, -14.00000, -194.0000}; // chi2=6.72803841972988081 {0.000000, 0.000000, 91.20000,
    //-63.23128, 571.4778, -83.86853, -112.4498, 538.1335, -132.5532, -129.9365, 1.500000, 685.3931, -13.80000,
    // -193.9000, -14.00000, -194.0000}; // chi2=6.72276343730673887 {0.000000, 0.000000, 91.20000, -63.23128, 571.4778,
    //-83.86853, -112.4498, 538.1335, -132.5532, -129.9365, 1.500000, 685.3931, -13.70000, -193.9000, -14.00000,
    //-194.0000}; // chi2=6.71954779402023039

    //{0.000000,  0.000000,  91.20000,  -64.23128,  572.4778,  -83.86853,   -111.5392,  535.7847,  -131.6388, -129.9365,
    // 0.000000,  685.3931, -14.0000, -194.0000, -14.00000, -194.0000};
    //{-0.05046619, 0.02448567, 91.37383, -63.74065, 573.3532, -84.29503, -111.6913, 535.8178, -131.7519, -130.3291,
    //-1.911900, 685.6379, -13.83124, -193.9125, -14.01615, -193.9270}; // chi2=6.96815677858580251
    //{-0.05046619, 0.02448567, 91.37383, -63.74065, 573.3532, -84.29503, -111.6913, 535.8178, -131.7519, -130.3291,
    //-1.411900, 685.6379, -13.83124, -193.9125, -14.01615, -193.9270}; // chi2=6.87791842949475729
    //{-0.05046619, 0.02448567, 91.37383, -63.74065, 573.3532, -84.29503, -111.6913, 535.8178, -131.7519, -130.3291,
    //-0.9119000, 685.6379, -13.83124, -193.9125, -14.01615, -193.9270}; // chi2=6.81268321951758171
    //{-0.05046619, 0.02448567, 91.37383, -63.74065, 573.3532, -84.29503, -111.6913, 535.8178, -131.7519, -130.3291,
    //-0.4119000, 685.6379, -13.83124, -193.9125, -14.01615, -193.9270}; // chi2= 6.76297676253985713
    //{-0.05046619, 0.02448567, 91.37383, -63.74065, 573.3532, -84.29503, -111.6913, 535.8178, -131.7519, -130.3291,
    // 0.08810000, 685.6379, -13.83124, -193.9125, -14.01615, -193.9270}; // chi2=6.73063076624607159
    //{-0.04788789, 0.02706397, 91.38028, -63.73439, 573.3856, -84.28213, -111.6784, 535.8502, -131.7515, -130.2628,
    // 0.1205445, 685.7042, -13.82479, -193.9061, -14.00970, -193.9206}; // chi2=6.71458348676870820
    //{-0.04788789, 0.02706397, 91.38028, -63.73439, 573.8856, -84.28213, -111.6784, 535.8502, -131.7515, -130.2628,
    // 0.1205445, 685.7042, -13.82479, -193.9061, -14.00970, -193.9206}; // chi2=6.67649385701459153

    mini->SetLimitedVariable(0, "xfi23", variable[0], step[0], variable[0] - 2.0, variable[0] + 2.0);
    mini->SetLimitedVariable(1, "yfi23", variable[1], step[1], variable[1] - 2.0, variable[1] + 2.0);
    mini->SetLimitedVariable(2, "zfi23", variable[2], step[2], variable[2] - 2.0, variable[2] + 2.0);
    mini->SetLimitedVariable(3, "x30", variable[3], step[3], variable[3] - 2.5, variable[3] + 2.5);
    mini->SetLimitedVariable(4, "z30", variable[4], step[4], variable[4] - 2.5, variable[4] + 2.5);
    mini->SetLimitedVariable(5, "x32", variable[5], step[5], variable[5] - 2.5, variable[5] + 2.5);
    mini->SetLimitedVariable(6, "x31", variable[6], step[6], variable[6] - 2.5, variable[6] + 2.5);
    mini->SetLimitedVariable(7, "z31", variable[7], step[7], variable[7] - 2.5, variable[7] + 2.5);
    mini->SetLimitedVariable(8, "x33", variable[8], step[8], variable[8] - 2.5, variable[8] + 2.5);
    mini->SetLimitedVariable(9, "xtofd", variable[9], step[9], variable[9] - 2.5, variable[9] + 2.5);
    mini->SetLimitedVariable(10, "ytofd", variable[10], step[10], variable[10] - 2.5, variable[10] + 2.5);
    mini->SetLimitedVariable(11, "ztofd", variable[11], step[11], variable[11] - 2.5, variable[11] + 2.5);
    mini->SetLimitedVariable(12, "a30", variable[12], step[12], variable[12] - 2.5, variable[12] + 2.5);
    mini->SetLimitedVariable(13, "a31", variable[13], step[13], variable[13] - 2.5, variable[13] + 2.5);
    mini->SetLimitedVariable(14, "a32", variable[14], step[14], variable[14] - 2.5, variable[14] + 2.5);
    mini->SetLimitedVariable(15, "a33", variable[15], step[15], variable[15] - 2.5, variable[15] + 2.5);
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
    // mini->FixVariable(12);
    // mini->FixVariable(13);

    Int_t status = 0;
    // do the minimization
    mini->Minimize();

    status = mini->Status();
    cout << "Status: " << status << endl;
    cout << "optimized values: " << endl;
    cout.precision(7);

    cout << "{" << mini->X()[0] << ", " << mini->X()[1] << ", " << mini->X()[2] << ", " << mini->X()[3] << ", "
         << mini->X()[4] << ", " << mini->X()[5] << ", " << mini->X()[6] << ", " << mini->X()[7] << ", " << mini->X()[8]
         << ", " << mini->X()[9] << ", " << mini->X()[10] << ", " << mini->X()[11] << ", " << mini->X()[12] << ", "
         << mini->X()[13] << ", " << mini->X()[14] << ", " << mini->X()[15] << "}; // chi2= " << endl;

    mini->Clear();

    /*
        Double_t v[14];
        for(Int_t i=0; i<100000;i++)
        {
            //{0.0000, 0.0000, 91.2000, -62.8664, 570.7671, -82.8087, 632.4669, -111.1057, 537.8183, -131.0650,
       599.5703, -129.9365, 0.0000, 685.3931 }; // Startwerte

            v[0] = 0.;
            v[1] = 0.;
            v[2] = 91.2;
            v[3] = gRandom->Gaus(-62.8664, 1.);
            v[4] = gRandom->Gaus(570.7671, 1.);
            v[5] = gRandom->Gaus(-82.8087, 1.);
            v[6] = v[4] + 62.77813349;
            v[7] = gRandom->Gaus(-111.1057, 1.);
            v[8] = gRandom->Gaus(537.8183, 1.);
            v[9] = gRandom->Gaus(-131.0650, 1.);
            v[10] = v[8] + 62.77813349;
            v[11] = -129.9365;
            v[12] = 0;
            v[13] = 685.3931;

        Double_t chi2_all = 0;
        Double_t fi23x, fi23y, fi23z, fi30x, fi30z, fi30a, fi31x,
        fi31z, fi31a, fi32x, fi32z, fi32a, fi33x, fi33z, fi33a,
        tofdx, tofdy, tofdz;

        Int_t nof = 0;
        cout.precision(7);
        cout << "new correction: "
             << v[0] << "  " << v[1] << "  " << v[2] << "  "
             << v[3] << "  " << v[4] << "  " << v[5] << "  "
             << v[6] << "  " << v[7] << "  " << v[8] << "  "
             << v[9] << "  " << v[10] << "  " << v[11] << "  "
             << v[12] << "  " << v[13] << endl;

        fi23x = 0.00000000;
        fi23y = 0.0000000;
        //fi23z = 91.2;
        fi23z = 91.04;

        fi30x = -62.18069;
        fi30z = 570.59330;
        //fi30a = -13.68626;
        fi30a = -14.;

        fi31x = -112.67330;
        fi31z = 537.9038;
        //fi31a = -193.8265;
        fi31a = -194.;



        fi32x = -82.41713;
        fi32z = 632.9688;
        //fi32a = -14.02967;
        fi32a = -14.;

        fi33x = -131.8124;
        fi33z = 597.7741;
        //fi33a = -193.8699;
        fi33a = -194.;

        tofdx = -129.300;
        tofdy = 0.0;
        tofdz = 685.4;

        fi23x = v[0];
        fi23y = v[1];
        fi23z = v[2];
        fi30x = v[3];
        fi30z = v[4];
        fi32x = v[5];
        fi32z = v[6];
        fi31x = v[7];
        fi31z = v[8];
        fi33x = v[9];
        fi33z = v[10];
        tofdx = v[11];
        tofdy = v[12];
        tofdz = v[13];

        //for break-up run we need this line
        //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(v[6]);

        //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(v[15]);
        //Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
        //Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
        //cout << "Field:" << field << " scale: " << scale << endl;


        R3BTrackingDetector* fi23a = gSetup->GetByName("fi23a");

        fi23a->pos0 = TVector3(0., 0., 0.);
        fi23a->pos1 = TVector3(5., 5., 0.);
        fi23a->pos2 = TVector3(-5., 5., 0.);

        fi23a->pos0.RotateY(0. * TMath::DegToRad());
        fi23a->pos1.RotateY(0. * TMath::DegToRad());
        fi23a->pos2.RotateY(0. * TMath::DegToRad());

        TVector3 trans3a(fi23x, 0., fi23z - 0.015);

        fi23a->pos0 += trans3a;
        fi23a->pos1 += trans3a;
        fi23a->pos2 += trans3a;
        fi23a->norm = ((fi23a->pos1 - fi23a->pos0).Cross(fi23a->pos2 - fi23a->pos0)).Unit();

        R3BTrackingDetector* fi23b = gSetup->GetByName("fi23b");

        fi23b->pos0 = TVector3(0., 0., 0.);
        fi23b->pos1 = TVector3(5., 5., 0.);
        fi23b->pos2 = TVector3(-5., 5., 0.);

        fi23b->pos0.RotateY(0. * TMath::DegToRad());
        fi23b->pos1.RotateY(0. * TMath::DegToRad());
        fi23b->pos2.RotateY(0. * TMath::DegToRad());

        TVector3 trans3b(0., fi23y, fi23z + 0.015);

        fi23b->pos0 += trans3b;
        fi23b->pos1 += trans3b;
        fi23b->pos2 += trans3b;
        fi23b->norm = ((fi23b->pos1 - fi23b->pos0).Cross(fi23b->pos2 - fi23b->pos0)).Unit();


        R3BTrackingDetector* fi30 = gSetup->GetByName("fi30");
        fi30->pos0 = TVector3(0., 0., 0.);
        fi30->pos1 = TVector3(25., 25., 0.);
        fi30->pos2 = TVector3(-25., 25., 0.);

        fi30->pos0.RotateY(fi30a * TMath::DegToRad());
        fi30->pos1.RotateY(fi30a * TMath::DegToRad());
        fi30->pos2.RotateY(fi30a * TMath::DegToRad());

        TVector3 trans30(fi30x, 0., fi30z);

        fi30->pos0 += trans30;
        fi30->pos1 += trans30;
        fi30->pos2 += trans30;
        fi30->norm = ((fi30->pos1 - fi30->pos0).Cross(fi30->pos2 - fi30->pos0)).Unit();


        R3BTrackingDetector* fi31 = gSetup->GetByName("fi31");

        fi31->pos0 = TVector3(0., 0., 0.);
        fi31->pos1 = TVector3(25., 25., 0.);
        fi31->pos2 = TVector3(-25., 25., 0.);

        fi31->pos0.RotateY(fi31a * TMath::DegToRad());
        fi31->pos1.RotateY(fi31a * TMath::DegToRad());
        fi31->pos2.RotateY(fi31a * TMath::DegToRad());
        TVector3 trans31(fi31x, 0., fi31z);

        fi31->pos0 += trans31;
        fi31->pos1 += trans31;
        fi31->pos2 += trans31;
        fi31->norm = ((fi31->pos1 - fi31->pos0).Cross(fi31->pos2 - fi31->pos0)).Unit();

        R3BTrackingDetector* fi32 = gSetup->GetByName("fi32");

        fi32->pos0 = TVector3(0., 0., 0.);
        fi32->pos1 = TVector3(25., 25., 0.);
        fi32->pos2 = TVector3(-25., 25., 0.);

        fi32->pos0.RotateY(fi32a * TMath::DegToRad());
        fi32->pos1.RotateY(fi32a * TMath::DegToRad());
        fi32->pos2.RotateY(fi32a * TMath::DegToRad());

        TVector3 trans32(fi32x, 0., fi32z);

        fi32->pos0 += trans32;
        fi32->pos1 += trans32;
        fi32->pos2 += trans32;
        fi32->norm = ((fi32->pos1 - fi32->pos0).Cross(fi32->pos2 - fi32->pos0)).Unit();


        R3BTrackingDetector* fi33 = gSetup->GetByName("fi33");
        fi33->pos0 = TVector3(0., 0., 0.);
        fi33->pos1 = TVector3(25., 25., 0.);
        fi33->pos2 = TVector3(-25., 25., 0.);

        fi33->pos0.RotateY(fi33a * TMath::DegToRad());
        fi33->pos1.RotateY(fi33a * TMath::DegToRad());
        fi33->pos2.RotateY(fi33a * TMath::DegToRad());

        TVector3 trans33(fi33x, 0., fi33z);

        fi33->pos0 += trans33;
        fi33->pos1 += trans33;
        fi33->pos2 += trans33;
        fi33->norm = ((fi33->pos1 - fi33->pos0).Cross(fi33->pos2 - fi33->pos0)).Unit();


        R3BTrackingDetector* tofd = gSetup->GetByName("tofd");

        tofd->pos0 = TVector3(0., 0., 0.);
        tofd->pos1 = TVector3(60., 60., 0.);
        tofd->pos2 = TVector3(-50., 50., 0.);

        tofd->pos0.RotateY(18. * TMath::DegToRad());
        tofd->pos1.RotateY(18. * TMath::DegToRad());
        tofd->pos2.RotateY(18. * TMath::DegToRad());

        TVector3 transtofd(tofdx, tofdy, tofdz);

        tofd->pos0 += transtofd;
        tofd->pos1 += transtofd;
        tofd->pos2 += transtofd;
        tofd->norm = ((tofd->pos1 - tofd->pos0).Cross(tofd->pos2 - tofd->pos0)).Unit();

        for (Int_t iev = 0; iev < gThisTask->GetNEvents(); iev++)
        {
            gSetup->TakeHitsFromBuffer(iev);
            Double_t fieldScale;

            // für break-up events wieder rein

            //((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrectionScale(v[15]);
            // Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
            // Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
            // cout << "Field:" << field << " scale: " << scale << endl;


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




        }
    */
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
        LOG(error) << "Unsupported type of field.";
        return kFALSE;
    }
    return kTRUE;
}

ClassImp(R3BOptimizeGeometryS494)
