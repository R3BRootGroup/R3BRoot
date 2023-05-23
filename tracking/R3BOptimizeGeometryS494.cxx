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

    /*
        cout << "tof: " << tof->fArrayHits->GetEntriesFast() << endl;
        cout << "fi23a: " << fi23a->fArrayHits->GetEntriesFast() << endl;
        cout << "fi23b: " << fi23b->fArrayHits->GetEntriesFast() << endl;
        cout << "fi30: " << fi30->fArrayHits->GetEntriesFast() << endl;
        cout << "fi31: " << fi31->fArrayHits->GetEntriesFast() << endl;
        cout << "fi32: " << fi32->fArrayHits->GetEntriesFast() << endl;
        cout << "fi33: " << fi33->fArrayHits->GetEntriesFast() << endl;
    */
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
    // fi32z = fi30z + 62.3755;
    // fi33z = fi31z + 59.8702;

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

    if (tof->hits.size() > 0 && debug)
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

    Double_t ps = 17341.5; // 17391.5;
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

    for (Int_t i = 0; i < 1; i++)
    {
        /*
                Double_t variable[16];
                gRandom->SetSeed(0);
                variable[0] = 0.;
                variable[1] = 0.;
                variable[2] = 91.2;
                variable[3] = gRandom->Gaus(-62.8664, 1.);
                variable[4] = gRandom->Gaus(570.7671, 1.);
                variable[5] = gRandom->Gaus(-82.8087, 1.);
                variable[6] = gRandom->Gaus(-111.1057, 1.);
                variable[7] = gRandom->Gaus(537.8183, 1.);
                variable[8] = gRandom->Gaus(-131.0650, 1.);
                variable[9] = -129.9365;
                variable[10] = 0;
                variable[11] = 685.3931;
                variable[12] = -14.0;
                variable[13] = -194.0;
                variable[14] = -14.0;
                variable[15] = -194.0;
        */
        Double_t variable[16] =
            // 25th with p0=17390
            //{-0.02037841, 0.01464830, 90.67313, -62.79618, 571.0850, -83.30178, -112.0710, 536.9482, -132.2141,
            //	-131.4738, 0.2756137, 684.7434, -13.88817, -193.7690, -14.01239, -194.0022}; // chi2=1.48850119848933971
            //{-0.01730203, 0.01772468, 90.68082, -62.79330, 571.1237, -83.31297, -112.0780, 536.9869, -132.1987,
            //	-131.3947, 0.3143254, 684.8225, -13.88048, -193.7613, -14.00470, -193.9945}; // chi2=1.24072566940617390
            //{-0.01730203, 0.01772468, 90.68082, -62.79330, 571.1237, -83.31297, -112.0780, 536.9869, -132.1987,
            //	-131.3947, 0.8143254, 684.8225, -13.88048, -193.7613, -14.00470, -193.9945}; // chi2=1.20745419499713780
            //{-0.01456747, 0.02045924, 90.68766, -62.79213, 571.1581, -83.29928, -112.0843, 537.0213, -132.1850,
            //	-131.3244, 0.8487361, 684.8928, -13.87364, -193.7545, -13.99786, -193.9877}; // chi2=1.17632354018118512
            //{-0.01198917, 0.02303754, 90.69411, -62.77923, 571.1905, -83.29889, -112.0780, 537.0537, -132.1721,
            //	-131.2581, 0.8811806, 684.9591, -13.86719, -193.7481, -13.99141, -193.9813}; // chi2=1.12492071216752310
            //{-0.01198917, 0.02303754, 90.69411, -62.77923, 571.1905, -83.29889, -112.0780, 537.0537, -132.1721,
            //	-131.2581, 1.381181, 684.9591, -13.86719, -193.7481, -13.99141, -193.9813}; // chi2=1.10637955517630138
            //{-0.01198917, 0.02303754, 90.69411, -62.77923, 571.1905, -83.29889, -112.0780, 537.0537, -132.1721,
            //	-131.2581, 1.881181, 684.9591, -13.86719, -193.7481, -13.99141, -193.9813}; // chi2=1.10299038321039711
            //{-0.01198917, 0.02303754, 90.69411, -62.77923, 571.1905, -83.29889, -112.0780, 537.0537, -132.1721,
            //	-131.2581, 1.881181, 684.9591, -13.76719, -193.7481, -13.99141, -193.9813}; // chi2=1.09988996060669009

            // 25th with p0=17440
            //{-0.009410874, 0.02561584, 90.70056, -62.77884, 571.2229, -83.28599, -112.0717, 537.0861, -132.1592,
            //	-131.1918, 1.913625, 685.0254, -13.76074, -193.7417, -13.98496, -193.9749}; // chi2=1.08166285237111093
            //{-0.009410874, 0.02561584, 90.70056, -62.77884, 571.2229, -83.28599, -112.0717, 537.0861, -132.1592,
            //	-131.1918, 1.913625, 686.0254, -13.76074, -193.7417, -13.98496, -193.9749}; // chi2= 1.05350864036361047

            // 25th with p0=17340
            //{-0.009410874, 0.02561584, 90.70056, -62.77884, 571.2229, -83.28599, -112.0717, 537.0861, -132.1592,
            //	-131.1918, 2.413625, 686.0254, -13.76074, -193.7417, -13.98496, -193.9749}; // chi2= 1.16891813069972872
            //{-0.009410874, 0.02561584, 90.70056, -62.77884, 571.2229, -83.28599, -112.0717, 537.0861, -132.1592,
            //	-131.1918, 2.413625, 686.0254, -13.76074, -193.7417, -13.98496, -193.8749}; // chi2= 1.16080875585959720
            //{-0.009410874, 0.02561584, 90.70056, -62.77884, 571.2229, -83.28599, -112.0717, 537.0861, -132.1592,
            //	-131.1918, 2.413625, 686.0254, -13.66074, -193.7417, -13.98496, -193.8749}; // chi2= 1.15428931533978152

            // 23th with magnetic field * 0.997
            //{0.007419462, 0.007327982, 91.02052, -62.59230, 571.7103, -83.07132, -112.4498, 538.1335, -132.5532,
            //	-129.6732, 1.492008, 686.1278, -13.89355, -193.8936, -13.99355, -193.9936}; // chi2=
            //{0.009842597, 0.02506024, 91.03416, -62.61968, 571.5177, -83.10083, -112.4515, 538.5057, -132.5629,
            //	-130.9031, 1.731501, 685.6793, -13.86188, -193.8732, -13.95295, -193.9672}; // chi2=1.22680262470444368
            //{0.01257715, 0.02779480, 91.04100, -62.61851, 571.5521, -83.08714, -112.4578, 538.5401, -132.5492,
            //	-130.8328, 1.765912, 685.7496, -13.85504, -193.8664, -13.94611, -193.9604}; // chi2= 1.02758556647395038
            //{0.01257715, 0.02779480, 91.04100, -62.61851, 571.5521, -83.08714, -112.4578, 538.5401, -132.5492,
            //	-130.8328, 2.265912, 685.7496, -13.85504, -193.8664, -13.94611, -193.9604}; // chi2= 1.00424379012609521
            //{0.01257715, 0.02779480, 91.04100, -62.61851, 571.5521, -83.08714, -112.4578, 538.5401, -132.5492,
            //	-130.8328, 2.265912, 685.7496, -13.85504, -193.7664, -13.94611, -193.9604}; // chi2=0.990895651951266943

            // 23th with magnetic field * 1.003
            //{0.007419462, 0.007327982, 91.02052, -62.59230, 571.7103, -83.07132, -112.4498, 538.1335, -132.5532,
            //	-129.6732, 1.492008, 686.1278, -13.89355, -193.8936, -13.99355, -193.9936}; // chi2=
            //{0.009842597, 0.01547302, 91.03416, -62.61968, 571.5177, -83.10083, -112.4515, 538.5057, -132.5629,
            //	-130.9031, 1.731501, 685.6793, -13.86188, -193.8530, -13.95715, -193.9493}; // chi2=1.60108105002465750
            //{0.04984260, 0.01547302, 91.03416, -62.61968, 571.5177, -83.10083, -112.4515, 538.5057, -132.5629,
            //	-130.9031, 1.731501, 685.6793, -13.86188, -193.8530, -13.95715, -193.9493}; // chi2= 1.24251680154495525
            //{0.05242090, 0.01805132, 91.04061, -62.60678, 571.5501, -83.10044, -112.4452, 538.5381, -132.5500,
            //	-130.8368, 1.763945, 685.7456, -13.85543, -193.8466, -13.95070, -193.9429}; // chi2=1.17887346043446772
            {
                0.05242090, 0.01805132, 91.04061, -62.60678, 571.5501,  -83.10044, -112.4452, 538.5381,
                -132.5500,  -130.8368,  2.263945, 685.7456,  -13.85543, -193.8466, -13.95070, -193.9429
            }; // chi2= 1.14414971356381012

        // 26th
        //{-0.03294922, 0.06573896, 91.32341, -63.25731, 571.2752, -83.87121, -112.3497, 537.9647, -132.4498,
        //	-131.3346, 1.118805, 685.6031, -13.77710, -193.9673, -13.91639, -193.8224}; // chi2=1.31495456224623952
        //{-0.03013653, 0.06855165, 91.33044, -63.25575, 571.3106, -83.85713, -112.3622, 538.0001, -132.4357,
        //	-131.2623, 1.154199, 685.6754, -13.77007, -193.9603, -13.90936, -193.8154}; // chi2= 1.22208756597302770
        //{-0.03013653, 0.06855165, 91.33044, -63.25575, 571.3106, -83.85713, -112.3622, 538.0001, -132.4357,
        //	-131.2623, 1.654199, 685.6754, -13.77007, -193.9603, -13.90936, -193.8154}; // chi2=1.20279929188022416
        //{-0.03013653, 0.06855165, 91.33044, -63.25575, 571.3106, -83.85713, -112.3622, 538.0001, -132.4357,
        //	-131.2623, 2.154199, 685.6754, -13.77007, -193.9603, -13.90936, -193.8154}; // chi2=1.19645397255706754
        //{-0.03013653, 0.06855165, 91.33044, -63.25575, 571.3106, -83.85713, -112.3622, 538.0001, -132.4357,
        //	-131.2623, 2.654199, 685.6754, -13.77007, -193.9603, -13.90936, -193.8154}; // chi2=1.18315449131641448

        // 27th
        //{-0.009050367, 0.02042431, 91.23062, -63.06119, 571.9916, -83.56908, -111.5637, 535.8030, -131.6557,
        //	-131.2707, 0.3071948, 685.0063, -13.70400, -193.9704, -13.99977, -193.9275}; // chi2=1.27331139271876292
        //{-0.009050367, 0.02042431, 91.23062, -63.06119, 571.9916, -83.56908, -111.5637, 535.8030, -131.6557,
        //	-131.2707, 0.8071948, 685.0063, -13.70400, -193.9704, -13.99977, -193.9275}; // chi2=1.23867786381840106
        //{-0.009050367, 0.02042431, 91.23062, -63.06119, 571.9916, -83.56908, -111.5637, 535.8030, -131.6557,
        //	-131.2707, 1.307195, 685.0063, -13.70400, -193.9704, -13.99977, -193.9275}; // chi2=1.21873478455433482
        //{-0.009050367, 0.02042431, 91.23062, -63.06119, 571.9916, -83.56908, -111.5637, 535.8030, -131.6557,
        //	-131.2707, 1.307195, 686.0063, -13.70400, -193.9704, -13.99977, -193.9275}; // chi2=1.20399498051247855
        //{-0.009050367, 0.02042431, 91.23062, -63.06119, 571.9916, -83.56908, -111.5637, 535.8030, -131.6557,
        //	-131.2707, 1.307195, 686.0063, -13.60400, -193.9704, -13.99977, -193.9275}; // chi2=1.19075003151397185

        // 28th
        //{0.04625585, 0.01286908, 91.19404, -62.17203, 571.4941, -82.59911, -111.6179, 536.6789, -131.6886,
        //	-130.3676, 0.04557677, 685.0166, -13.91210, -193.9869, -13.91737, -193.9667}; // chi2=1.25544534853062184
        //{0.04899041, 0.01560364, 91.20088, -62.17829, 571.5285, -82.58542, -111.6167, 536.7133, -131.6749,
        //	-130.2973, 0.07998744, 685.0869, -13.90526, -193.9801, -13.91053, -193.9599}; // chi2=1.19766990816090035
        //{0.04899041, 0.01560364, 91.20088, -62.17829, 571.5285, -82.58542, -111.6167, 536.7133, -131.6749,
        //	-130.2973, 0.5799874, 685.0869, -13.90526, -193.9801, -13.91053, -193.9599}; // chi2=1.16068529018810351
        //{0.04899041, 0.01560364, 91.20088, -62.17829, 571.5285, -82.58542, -111.6167, 536.7133, -131.6749,
        //	-130.2973, 1.079987, 685.0869, -13.90526, -193.9801, -13.91053, -193.9599}; // chi2=1.13702210398045600
        //{0.04899041, 0.01560364, 91.20088, -62.17829, 571.5285, -82.58542, -111.6167, 536.7133, -131.6749,
        //	-130.2973, 1.579987, 685.0869, -13.90526, -193.9801, -13.91053, -193.9599}; // chi2=1.12405421894011948

        // 29th
        //{-0.008380227, 0.03415434, 91.28513, -62.87009, 570.8051, -83.46177, -112.3696, 537.7761, -132.5018,
        //	-131.0832, 0.3950629, 684.9746, -13.89912, -193.9056, -13.97584, -193.9980}; // chi2=1.33843937276682867
        //{-0.005567540, 0.03696703, 91.29216, -62.88260, 570.8405, -83.44769, -112.3680, 537.8115, -132.4877,
        //	-131.0109, 0.4304567, 685.0469, -13.89209, -193.8986, -13.96881, -193.9910}; // chi2=1.21146136853659536
        //{-0.005567540, 0.03696703, 91.29216, -62.88260, 570.8405, -83.44769, -112.3680, 537.8115, -132.4877,
        //	-131.0109, 0.9304567, 685.0469, -13.89209, -193.8986, -13.96881, -193.9910}; // chi2=1.17606713398174900
        //{-0.002989244, 0.03954533, 91.29861, -62.88221, 570.8729, -83.43479, -112.3617, 537.8439, -132.4748,
        //	-130.9446, 0.9629012, 685.1132, -13.88564, -193.8922, -13.96236, -193.9846}; // chi2=1.15501110991006462
        //{-0.002989244, 0.03954533, 91.29861, -62.88221, 570.8729, -83.43479, -112.3617, 537.8439, -132.4748,
        //	-130.9446, 1.462901, 685.1132, -13.88564, -193.8922, -13.96236, -193.9846}; // chi2=1.13592729882334686

        // 30th
        //{-0.002413338, 0.02330757, 91.31333, -63.15804, 571.8914, -83.70800, -112.0187, 538.0546, -132.0508,
        //	-130.8027, 0.1122220, 685.4103, -13.91202, -194.0032, -13.98511, -194.2469}; // chi2=1.35095925101544934
        //{-0.002413338, 0.02330757, 91.31333, -63.15804, 571.8914, -83.70800, -112.0187, 538.0546, -132.0508,
        //	-130.8027, 0.6122220, 685.4103, -13.91202, -194.0032, -13.98511, -194.2469}; // chi2=1.31352388862773806
        //{-0.002413338, 0.02330757, 91.31333, -63.15804, 571.8914, -83.70800, -112.0187, 538.0546, -132.0508,
        //	-130.8027, 1.112222, 685.4103, -13.91202, -194.0032, -13.98511, -194.2469}; // chi2=1.28975599007743802
        //{-0.002413338, 0.02330757, 91.31333, -63.15804, 571.8914, -83.70800, -112.0187, 538.0546, -132.0508,
        //	-130.8027, 1.112222, 685.4103, -13.81202, -194.0032, -13.98511, -194.2469}; // chi2=1.27380996520324818

        // 31st
        //{-0.01211027, -0.06018144, 91.41302, -62.89371, 571.7118, -83.36804, -112.2176, 537.5520, -132.3778,
        //	-131.9892, -0.2337305, 687.2859, -13.65638, -193.3601, -13.96543, -193.8758}; // chi2=1.38939836629903302
        //{-0.009033894, -0.05710506, 91.42071, -62.89083, 571.7505, -83.37923, -112.2246, 537.5907, -132.3624,
        //	-131.9101, -0.1950188, 687.3650, -13.64869, -193.3524, -13.95774, -193.8681}; // chi2=1.18732828938195878
        //{-0.009033894, -0.05710506, 91.42071, -62.89083, 571.7505, -83.37923, -112.2246, 537.5907, -132.3624,
        //	-131.9101, 0.3049812, 687.3650, -13.64869, -193.3524, -13.95774, -193.8681}; // chi2=1.15349080332796960
        //{-0.009033894, -0.05710506, 91.42071, -62.89083, 571.7505, -83.37923, -112.2246, 537.5907, -132.3624,
        //	-131.9101, 0.8049812, 687.3650, -13.64869, -193.3524, -13.95774, -193.8681}; // chi2=1.13383692592783336

        // 32nd
        //{0.03122322, 0.006507541, 91.25347, -62.00168, 570.6652, -82.39069, -112.8062, 538.9467, -132.9528,
        //	-131.2456, 0.2073727, 685.4856, -13.95687, -193.9374, -13.99095, -193.9881}; // chi2=1.43355219744225670
        //{0.03380152, 0.009085837, 91.25992, -61.98878, 570.6976, -82.39030, -112.7999, 538.9791, -132.9399,
        //	-131.1793, 0.2398172, 685.5519, -13.95042, -193.9310, -13.98450, -193.9817}; // chi2=1.26596448312136944
        //{0.03637982, 0.01166413, 91.26637, -61.98252, 570.7300, -82.37740, -112.7995, 539.0115, -132.9270,
        //	-131.1130, 0.2722617, 685.6182, -13.94397, -193.9246, -13.97805, -193.9753}; // chi2=1.18832258539144764
        //{0.03895812, 0.01424243, 91.27282, -61.97626, 570.7624, -82.36450, -112.7866, 539.0439, -132.9266,
        //	-131.0467, 0.3047062, 685.6845, -13.93752, -193.9182, -13.97160, -193.9689}; // chi2=1.09409775281960520

        // 33rd
        //{0.07020274, -0.1150818, 91.21642, -62.55341, 570.9947, -83.08373, -111.8818, 537.2993, -131.9842,
        //	-130.8163, 0.1886150, 685.1187, -13.99566, -193.8371, -13.83383, -193.9584}; // chi2=1.18270114578385299

        // 34th

        // 35th

        // 36th

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
        cout << "Attempt: " << i << endl;
        cout << "Status: " << status << endl;
        cout << "optimized values: " << endl;
        cout.precision(7);

        cout << "{" << mini->X()[0] << ", " << mini->X()[1] << ", " << mini->X()[2] << ", " << mini->X()[3] << ", "
             << mini->X()[4] << ", " << mini->X()[5] << ", " << mini->X()[6] << ", " << mini->X()[7] << ", "
             << mini->X()[8] << ", " << mini->X()[9] << ", " << mini->X()[10] << ", " << mini->X()[11] << ", "
             << mini->X()[12] << ", " << mini->X()[13] << ", " << mini->X()[14] << ", " << mini->X()[15]
             << "}; // chi2= " << endl;

        mini->Clear();
    }
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
