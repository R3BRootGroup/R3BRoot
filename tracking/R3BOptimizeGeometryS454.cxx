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

#include "R3BOptimizeGeometryS454.h"
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
    R3BOptimizeGeometryS454* gThisTask;
} // namespace GlobalChi2

using namespace GlobalChi2;

R3BOptimizeGeometryS454::R3BOptimizeGeometryS454(const char* name, Bool_t vis, Int_t verbose)
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
    , fLeft(kTRUE)
{
    // this is the list of detectors (active areas) we use for tracking
    fDetectorsLeft->AddDetector("target", kTarget, "TargetGeoPar");
    fDetectorsLeft->AddDetector("fi3b", kTargetGlad, "fi3bGeoPar", "Fi3bHit");
    fDetectorsLeft->AddDetector("fi12", kAfterGlad, "fi12GeoPar", "Fi12Hit");
    fDetectorsLeft->AddDetector("fi10", kAfterGlad, "fi10GeoPar", "Fi10Hit");
    fDetectorsLeft->AddDetector("tofd", kTof, "tofdGeoPar", "TofdHit");

    fDetectorsRight->AddDetector("target", kTarget, "TargetGeoPar");
    fDetectorsRight->AddDetector("fi3a", kTargetGlad, "fi3aGeoPar", "Fi3aHit");
    fDetectorsRight->AddDetector("fi11", kAfterGlad, "fi11GeoPar", "Fi11Hit");
    fDetectorsRight->AddDetector("fi13", kAfterGlad, "fi13GeoPar", "Fi13Hit");
    fDetectorsRight->AddDetector("tofd", kTof, "tofdGeoPar", "TofdHit");
}

R3BOptimizeGeometryS454::~R3BOptimizeGeometryS454() {}

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
InitStatus R3BOptimizeGeometryS454::Init()
{
    FairRootManager* man = FairRootManager::Instance();

    fArrayMCTracks = (TClonesArray*)man->GetObject("MCTrack");
    if (NULL == fArrayMCTracks)
    {
        LOG(error) << "No MC Track array found in input file.";
        //        return kERROR;
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
    gSetup = fDetectorsLeft;
    // gSetup = fDetectorsRight;

    gThisTask = this;

    fh_mult_fi3a = new TH1F("h_mult_fi3a", "Multiplicity fi3a", 20, -0.5, 19.5);
    fh_mult_fi3b = new TH1F("h_mult_fi3b", "Multiplicity fi3b", 20, -0.5, 19.5);
    fh_mult_fi12 = new TH1F("h_mult_fi12", "Multiplicity fi12", 20, -0.5, 19.5);
    fh_mult_fi11 = new TH1F("h_mult_fi11", "Multiplicity fi11", 20, -0.5, 19.5);
    fh_mult_fi10 = new TH1F("h_mult_fi10", "Multiplicity fi10", 20, -0.5, 19.5);
    fh_mult_fi13 = new TH1F("h_mult_fi13", "Multiplicity fi13", 20, -0.5, 19.5);
    fh_mult_tofd = new TH1F("h_mult_tofd", "Multiplicity TOFd", 20, -0.5, 19.5);
    fh_eloss_fi3a_mc = new TH1F("h_eloss_fi3a_mc", "Energy loss fi3a (MC truth)", 2000, 0., 200.);
    fh_eloss_fi3a = new TH1F("h_eloss_fi3a", "Energy loss fi3a", 2000, 0., 200.);
    fh_eloss_fi3b_mc = new TH1F("h_eloss_fi3b_mc", "Energy loss fi3b (MC truth)", 2000, 0., 500.);
    fh_eloss_fi3b = new TH1F("h_eloss_fi3b", "Energy loss fi3b", 2000, 0., 500.);
    fh_eloss_fi12_mc = new TH1F("h_eloss_fi12_mc", "Energy loss fi12 (MC truth)", 2000, 0., 500.);
    fh_eloss_fi12 = new TH1F("h_eloss_fi12", "Energy loss fi12", 2000, 0., 500.);
    fh_eloss_fi11_mc = new TH1F("h_eloss_fi11_mc", "Energy loss fi11 (MC truth)", 2000, 0., 500.);
    fh_eloss_fi11 = new TH1F("h_eloss_fi11", "Energy loss fi11", 2000, 0., 500.);
    fh_eloss_fi10_mc = new TH1F("h_eloss_fi10_mc", "Energy loss fi10 (MC truth)", 2000, 0., 1000.);
    fh_eloss_fi10 = new TH1F("h_eloss_fi10", "Energy loss fi10", 2000, 0., 1000.);
    fh_eloss_fi13_mc = new TH1F("h_eloss_fi13_mc", "Energy loss fi13 (MC truth)", 2000, 0., 500.);
    fh_eloss_fi13 = new TH1F("h_eloss_fi13", "Energy loss fi13", 2000, 0., 500.);
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

InitStatus R3BOptimizeGeometryS454::ReInit()
{
    SetParContainers();

    if (!InitPropagator())
    {
        return kERROR;
    }

    return kSUCCESS;
}

void R3BOptimizeGeometryS454::SetParContainers()
{
    fFieldPar = (R3BFieldPar*)FairRuntimeDb::instance()->getContainer("R3BFieldPar");

    fDetectorsLeft->SetParContainers();
    fDetectorsRight->SetParContainers();
    fDetectors->SetParContainers();
}

void R3BOptimizeGeometryS454::Exec(const Option_t*)
{
    // For the moment one has to swap manually between the setups

    R3BTrackingDetector* target = fDetectorsLeft->GetByName("target");
    R3BTrackingDetector* tof = fDetectorsLeft->GetByName("tofd");
    R3BTrackingDetector* fi3b = fDetectorsLeft->GetByName("fi3b");
    R3BTrackingDetector* fi12 = fDetectorsLeft->GetByName("fi12");
    R3BTrackingDetector* fi10 = fDetectorsLeft->GetByName("fi10");
    R3BTrackingDetector* fi3a = fDetectorsRight->GetByName("fi3a");
    R3BTrackingDetector* fi11 = fDetectorsRight->GetByName("fi11");
    R3BTrackingDetector* fi13 = fDetectorsRight->GetByName("fi13");
    if (tof->fArrayHits->GetEntriesFast() > 0)
    {
        fDetectorsLeft->CopyToBuffer();
        cout << "Copy Hits: " << fi3b->events[fNEvents].size() << "  " << fi12->events[fNEvents].size() << "  "
             << fi10->events[fNEvents].size() << "  " << tof->events[fNEvents].size() << endl;

        fNEvents += 1;
        cout << "Saved events: " << gThisTask->GetNEvents() << endl;
    }

    /*
        R3BTrackingDetector* target = fDetectorsRight->GetByName("target");
        R3BTrackingDetector* tof = fDetectorsRight->GetByName("tofd");
        R3BTrackingDetector* fi3a = fDetectorsRight->GetByName("fi3a");
        R3BTrackingDetector* fi11 = fDetectorsRight->GetByName("fi11");
        R3BTrackingDetector* fi13 = fDetectorsRight->GetByName("fi13");
        R3BTrackingDetector* fi3b = fDetectorsLeft->GetByName("fi3b");
        R3BTrackingDetector* fi12 = fDetectorsLeft->GetByName("fi12");
        R3BTrackingDetector* fi10 = fDetectorsLeft->GetByName("fi10");


        if (tof->fArrayHits->GetEntriesFast() > 0)
        {
            fDetectorsRight->CopyToBuffer();
            cout << "Copy Hits: "
            << fi3a->events[fNEvents].size() << "  " << fi11->events[fNEvents].size() << "  "
            << fi13->events[fNEvents].size() << "  " << tof->events[fNEvents].size() << endl;

            fNEvents += 1;
            cout << "Saved events: " << gThisTask->GetNEvents() << endl;
        }
    */
}

double Chi2AllEvents(const double* xx)
{
    Bool_t fLeft = kTRUE;
    Double_t chi2_all = 0;
    Int_t nof = 0;
    cout << "new correction: " << xx[0] << "  " << xx[1] << "  " << xx[2] << "  " << xx[3] << "  " << xx[4] << "  "
         << xx[5] << "  " << xx[6] << "  " << xx[7] << "  " << xx[8] << endl;

    if (fLeft)
    {
        R3BTrackingDetector* fi12 = gSetup->GetByName("fi12");

        fi12->pos0 = TVector3(0., 0., 0.);
        fi12->pos1 = TVector3(25., 25., 0.);
        fi12->pos2 = TVector3(-25., 25., 0.);

        fi12->pos0.RotateY(xx[5] * TMath::DegToRad());
        fi12->pos1.RotateY(xx[5] * TMath::DegToRad());
        fi12->pos2.RotateY(xx[5] * TMath::DegToRad());

        TVector3 trans12(xx[2], 0., xx[3]);

        fi12->pos0 += trans12;
        fi12->pos1 += trans12;
        fi12->pos2 += trans12;
        fi12->norm = ((fi12->pos1 - fi12->pos0).Cross(fi12->pos2 - fi12->pos0)).Unit();

        R3BTrackingDetector* fi10 = gSetup->GetByName("fi10");
        fi10->pos0 = TVector3(0., 0., 0.);
        fi10->pos1 = TVector3(25., 25., 0.);
        fi10->pos2 = TVector3(-25., 25., 0.);

        fi10->pos0.RotateY(xx[4] * TMath::DegToRad());
        fi10->pos1.RotateY(xx[4] * TMath::DegToRad());
        fi10->pos2.RotateY(xx[4] * TMath::DegToRad());

        TVector3 trans10(xx[0], 0., xx[1]);

        fi10->pos0 += trans10;
        fi10->pos1 += trans10;
        fi10->pos2 += trans10;
        fi10->norm = ((fi10->pos1 - fi10->pos0).Cross(fi10->pos2 - fi10->pos0)).Unit();

        R3BTrackingDetector* fi3b = gSetup->GetByName("fi3b");

        fi3b->pos0 = TVector3(0., 0., 0.);
        fi3b->pos1 = TVector3(5., 5., 0.);
        fi3b->pos2 = TVector3(-5., 5., 0.);

        fi3b->pos0.RotateY(0. * TMath::DegToRad());
        fi3b->pos1.RotateY(0. * TMath::DegToRad());
        fi3b->pos2.RotateY(0. * TMath::DegToRad());

        TVector3 trans3b(xx[7], 0., xx[8]);

        fi3b->pos0 += trans3b;
        fi3b->pos1 += trans3b;
        fi3b->pos2 += trans3b;
        fi3b->norm = ((fi3b->pos1 - fi3b->pos0).Cross(fi3b->pos2 - fi3b->pos0)).Unit();
    }
    else
    {

        R3BTrackingDetector* fi11 = gSetup->GetByName("fi11");

        fi11->pos0 = TVector3(0., 0., 0.);
        fi11->pos1 = TVector3(25., 25., 0.);
        fi11->pos2 = TVector3(-25., 25., 0.);

        fi11->pos0.RotateY(xx[4] * TMath::DegToRad());
        fi11->pos1.RotateY(xx[4] * TMath::DegToRad());
        fi11->pos2.RotateY(xx[4] * TMath::DegToRad());
        cout << "Rotation: " << xx[4] << endl;
        TVector3 trans11(xx[0], 0., xx[1]);

        fi11->pos0 += trans11;
        fi11->pos1 += trans11;
        fi11->pos2 += trans11;
        fi11->norm = ((fi11->pos1 - fi11->pos0).Cross(fi11->pos2 - fi11->pos0)).Unit();

        R3BTrackingDetector* fi13 = gSetup->GetByName("fi13");
        fi13->pos0 = TVector3(0., 0., 0.);
        fi13->pos1 = TVector3(25., 25., 0.);
        fi13->pos2 = TVector3(-25., 25., 0.);

        fi13->pos0.RotateY(xx[5] * TMath::DegToRad());
        fi13->pos1.RotateY(xx[5] * TMath::DegToRad());
        fi13->pos2.RotateY(xx[5] * TMath::DegToRad());
        cout << "Rotation: " << xx[5] << endl;

        TVector3 trans13(xx[2], 0., xx[3]);

        fi13->pos0 += trans13;
        fi13->pos1 += trans13;
        fi13->pos2 += trans13;
        fi13->norm = ((fi13->pos1 - fi13->pos0).Cross(fi13->pos2 - fi13->pos0)).Unit();

        R3BTrackingDetector* fi3a = gSetup->GetByName("fi3a");

        fi3a->pos0 = TVector3(0., 0., 0.);
        fi3a->pos1 = TVector3(5., 5., 0.);
        fi3a->pos2 = TVector3(-5., 5., 0.);

        fi3a->pos0.RotateY(180. * TMath::DegToRad());
        fi3a->pos1.RotateY(180. * TMath::DegToRad());
        fi3a->pos2.RotateY(180. * TMath::DegToRad());

        TVector3 trans3a(xx[7], 0., xx[8]);

        fi3a->pos0 += trans3a;
        fi3a->pos1 += trans3a;
        fi3a->pos2 += trans3a;
        fi3a->norm = ((fi3a->pos1 - fi3a->pos0).Cross(fi3a->pos2 - fi3a->pos0)).Unit();
    }

    for (Int_t iev = 0; iev < gThisTask->GetNEvents(); iev++)
    // for(Int_t iev = 0; iev < 1000; iev++)
    {
        gSetup->TakeHitsFromBuffer(iev);

        Double_t fieldScale = 1672.0 / 3584. * 1.0; // standard
        Double_t scale = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetScale();
        Double_t field = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(0., 0., 240.);
        cout << "Field:" << field << " scale: " << scale << endl;

        fieldScale = -1672.0 / 3584. / scale * xx[6]; // run395
        cout << "Setting field to " << fieldScale << endl;
        ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->SetTrackerCorrection(fieldScale);
        field = ((R3BGladFieldMap*)FairRunAna::Instance()->GetField())->GetBy(0., 0., 240.);
        cout << "Field after:" << field << endl;

        /*
                if(fLeft)
                {
                    if(iev == 0)
                    {
                        Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                        Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field:" << field << " scale: " << scale << endl;
                        fieldScale	= -1102.0 / 3584. / scale * xx[6];	//run395
                        cout << "Setting field to " << 	fieldScale << endl;
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrection(fieldScale);
                        field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field after:" << 	field << endl;

                    }
                    if(iev == 200)
                    {
                        Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                        Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field:" << field << " scale: " << scale<< endl;
                        fieldScale	= -1292.0 / 3584. / scale * xx[6];	//run391
                        cout << "Setting field to " << 	fieldScale << endl;
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrection(fieldScale);
                        field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field after:" << 	field << endl;
                    }
            //		if(iev == 400)
                    if(iev == 394)
                    {
                        Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                        Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field:" << field << " scale: " << scale << endl;
                        fieldScale	= -1482.0 / 3584. / scale * xx[6];	//run389
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
                        fieldScale	= 1862.0/ 3584. * xx[6];	//run399
                        cout << "Setting field to " << 	fieldScale << endl;
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrection(fieldScale);
                        field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field after:" << 	field << endl;

                    }
                    if(iev == 201)
                    {
                        Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                        Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field:" << field << " scale: " << scale<< endl;
                        fieldScale	= 2052.0/ 3584. * xx[6];	//run401
                        cout << "Setting field to " << 	fieldScale << endl;
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrection(fieldScale);
                        field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field after:" << 	field << endl;
                    }
                    if(iev == 401)
                    {
                        Double_t scale = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetScale();
                        Double_t field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field:" << field << " scale: " << scale << endl;
                        fieldScale	= 2242.0/ 3584. * xx[6];	//run405
                        cout << "Setting field to " << 	fieldScale << endl;
                        ( (R3BGladFieldMap*) FairRunAna::Instance()->GetField() )->SetTrackerCorrection(fieldScale);
                        field = ((R3BGladFieldMap *) FairRunAna::Instance()->GetField())->GetBy(0.,0.,240.);
                        cout << "Field after:" << 	field << endl;
                    }
                }
        */
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

double R3BOptimizeGeometryS454::Chi2()
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
    R3BTrackingDetector* tof = gSetup->GetByName("tofd");

    // for the moment one has to define manually the setup

    R3BTrackingDetector* fi3b = gSetup->GetByName("fi3b");
    R3BTrackingDetector* fi12 = gSetup->GetByName("fi12");
    R3BTrackingDetector* fi10 = gSetup->GetByName("fi10");
    R3BTrackingDetector* fi3a = fDetectorsRight->GetByName("fi3a");
    R3BTrackingDetector* fi11 = fDetectorsRight->GetByName("fi11");
    R3BTrackingDetector* fi13 = fDetectorsRight->GetByName("fi13");

    /*
        R3BTrackingDetector* fi3b = fDetectorsLeft->GetByName("fi3b");
        R3BTrackingDetector* fi12 = fDetectorsLeft->GetByName("fi12");
        R3BTrackingDetector* fi10 = fDetectorsLeft->GetByName("fi10");
        R3BTrackingDetector* fi3a = gSetup->GetByName("fi3a");
        R3BTrackingDetector* fi11 = gSetup->GetByName("fi11");
        R3BTrackingDetector* fi13 = gSetup->GetByName("fi13");
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
    Int_t ifi3a = 0;
    Int_t ifi3b = 0;
    Int_t ifi12 = 0;
    Int_t ifi11 = 0;
    Int_t ifi10 = 0;
    Int_t ifi13 = 0;
    Int_t itof = 0;
    if (0 == fi3a->hits.size())
        ifi3a = -1;
    if (0 == fi3b->hits.size())
        ifi3b = -1;
    if (0 == fi12->hits.size())
        ifi12 = -1;
    if (0 == fi11->hits.size())
        ifi11 = -1;
    if (0 == fi10->hits.size())
        ifi10 = -1;
    if (0 == fi13->hits.size())
        ifi13 = -1;
    if (0 == tof->hits.size())
        itof = -1;

    Int_t charge = 0;
    Double_t Charge = 0;
    Int_t charge_requested = 6;
    Double_t m0 = 0.;
    Double_t p0 = 0.;

    // Loop over all combinations of hits
    for (Int_t i = 0; i < tof->hits.size(); i++) // loop over all ToFD hits
    // do // tof wall
    {
        if (fSimu)
        {
            // For tracking of simulations:
            charge = sqrt(tof->hits.at(i)->GetEloss()) / 0.037675 + 0.5;
            Charge = sqrt(tof->hits.at(i)->GetEloss()) / 0.037675;
        }
        else
        {
            // For tracking of exp. data:
            charge = tof->hits.at(i)->GetEloss();
            Charge = tof->hits.at(i)->GetEloss();
        }

        cout << "Charge: " << charge << " requested charge: " << charge_requested << endl;
        if (charge != charge_requested)
            // if(!(charge == 2 || charge == 6))
            continue;

        cout << "Charge accepted: " << charge << endl;
        Double_t beta0 = 0.76; // velocity could eventually be calculated from ToF
        tof->res_t = 0.03;

        if (charge == 8)
        {
            m0 = 15.0124;
            p0 = 17.389;
        }
        if (charge == 6)
        {
            m0 = 11.1749;
            p0 = 13.042;
        }
        if (charge == 2)
        {
            m0 = 3.7284;
            p0 = 4.348;
        }

        if (tof->hits.at(i)->GetX() > 0 && fi10->hits.size() > 0 && fi12->hits.size() > 0)
        {
            // left branch in beam direction, don't consider hits in the detectors of the other side
            do // fi10
            {
                do // fi12
                {
                    do // fi3b
                    {
                        // Create object for particle which will be fitted
                        R3BTrackingParticle* candidate =
                            new R3BTrackingParticle(charge, 0., 0., 0., 0., 0., p0, beta0, m0); // 17.39

                        // cout << "left side of setup" << endl;
                        // cout << "Hit Tofd # " << i << " x: " << tof->hits.at(i)->GetX()
                        //	<< " y: " << tof->hits.at(i)->GetY() << endl;
                        // if(ifi10 > -1) cout << " Fi10 # " <<  ifi10 << " x: "<< fi10->hits.at(ifi10)->GetX() << endl;
                        // if(ifi12 > -1) cout << " Fi12 # " <<  ifi12 << " x: "<< fi12->hits.at(ifi12)->GetX() << endl;
                        // cout << "Hit target # " << " x: " << target->hits.at(0)->GetX() << endl;
                        //                               cout << "Fi3b  # " << " x: " << fi3b->hits.at(0)->GetX() <<
                        //                               endl;

                        candidate->AddHit("target", 0);
                        candidate->AddHit("tofd", i);
                        candidate->AddHit("fi3b", ifi3b);
                        candidate->AddHit("fi12", ifi12);
                        candidate->AddHit("fi10", ifi10);

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

                        ifi3b += 1;
                    } while (ifi3b < fi3b->hits.size());
                    ifi3b = 0;
                    if (0 == fi3b->hits.size())
                        ifi3b = -1;

                    ifi12 += 1;
                } while (ifi12 < fi12->hits.size());
                ifi12 = 0;
                if (0 == fi12->hits.size())
                    ifi12 = -1;

                ifi10 += 1;
            } while (ifi10 < fi10->hits.size());
            ifi10 = 0;
            if (0 == fi10->hits.size())
                ifi10 = -1;

        } // end if left branch
        if (tof->hits.at(i)->GetX() < 0 && fi11->hits.size() > 0 && fi13->hits.size() > 0)
        {
            // right branch in beam direction, don't consider hits in the detectors of the other side
            do // fi13
            {
                if (ifi13 >= 0)
                    fh_eloss_fi13_mc->Fill(1000. * fi13->hits.at(ifi13)->GetEloss()); // MeV
                do                                                                    // fi11
                {
                    if (ifi11 >= 0)
                        fh_eloss_fi11_mc->Fill(1000. * fi11->hits.at(ifi11)->GetEloss()); // MeV
                    do                                                                    // fi3a
                    {
                        if (ifi3a >= 0)
                            fh_eloss_fi3a_mc->Fill(1000. * fi3a->hits.at(ifi3a)->GetEloss()); // MeV

                        // Create object for particle which will be fitted
                        R3BTrackingParticle* candidate =
                            new R3BTrackingParticle(charge, 0., 0., 0., 0., 0., p0, beta0, m0); // 17.39

                        cout << "right side of setup" << endl;
                        cout << "Hit Tofd # " << i << " x: " << tof->hits.at(i)->GetX()
                             << " y: " << tof->hits.at(i)->GetY() << endl;
                        if (ifi13 > -1)
                            cout << "Fi13 # " << ifi13 << " x: " << fi13->hits.at(ifi13)->GetX() << endl;
                        if (ifi11 > -1)
                            cout << "Fi11 # " << ifi11 << " x: " << fi11->hits.at(ifi11)->GetX() << endl;
                        cout << "Hit target # "
                             << " x: " << target->hits.at(0)->GetX() << endl;

                        candidate->AddHit("target", 0);
                        candidate->AddHit("tofd", i);
                        candidate->AddHit("fi3a", ifi3a);
                        candidate->AddHit("fi11", ifi11);
                        candidate->AddHit("fi13", ifi13);

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
                        ifi3a += 1;
                    } while (ifi3a < fi3a->hits.size());
                    ifi3a = 0;
                    if (0 == fi3a->hits.size())
                        ifi3a = -1;

                    ifi11 += 1;
                } while (ifi11 < fi11->hits.size());
                ifi11 = 0;
                if (0 == fi11->hits.size())
                    ifi11 = -1;

                ifi13 += 1;
            } while (ifi13 < fi13->hits.size());
            ifi13 = 0;
            if (0 == fi13->hits.size())
                ifi13 = -1;
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
        pChi2 = 10000. * (candidate->GetStartMomentum().Mag() - p0) * (candidate->GetStartMomentum().Mag() - p0);
        // mChi2 = candidate->GetChi2();
        mChi2 = sqrt(candidate->GetChi2() * candidate->GetChi2() + pChi2 * pChi2);
        // mChi2 = pChi2;
        cout << "pchi: " << pChi2 << "  " << candidate->GetChi2() << "  " << mChi2 << endl;
        LOG(info) << "Results after tracking mass:";
        LOG(info) << "Position x: " << candidate->GetStartPosition().X() << " y: " << candidate->GetStartPosition().Y()
                  << " z: " << candidate->GetStartPosition().Z();
        cout << "Momentum : " << candidate->GetStartMomentum().Mag() << " px : " << candidate->GetStartMomentum().X()
             << " py: " << candidate->GetStartMomentum().Y() << " pz: " << candidate->GetStartMomentum().Z() << endl;
        LOG(info) << "Mass   : " << candidate->GetMass();
        LOG(info) << "Beta   : " << candidate->GetStartBeta();
        LOG(info) << "chi2: " << candidate->GetChi2() << endl;
        LOG(info) << "mchi2: " << mChi2 << endl;

        fh_residuals->Fill(candidate->GetStartMomentum().Mag() - p0);

    } // end if(fFragments.size() > 0

    delete particle;

    if (mChi2 < 10000.)
    {
        return mChi2;
    }
    return -1.;
}

void R3BOptimizeGeometryS454::Finish()
{
    cout << "Total chi2 for mass: " << totalChi2Mass << endl;
    cout << "Total chi2 for momentum: " << totalChi2P << endl;
    // Optimize global Chi2

    // Place here Minuit part for
    // minimization of Chi2AllEvents()

    ROOT::Math::Minimizer* mini = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Scan");
    // fMinimum = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
    // Migrad, Simplex, Scan,
    // set tolerance , etc...
    mini->SetMaxFunctionCalls(10000); // for Minuit/Minuit2
    mini->SetMaxIterations(1000);     // for GSL
    mini->SetTolerance(100.);
    mini->SetPrintLevel(1);

    // create funciton wrapper for minmizer
    // a IMultiGenFunction type
    ROOT::Math::Functor fm1(&Chi2AllEvents, 9);
    if (fLeft)
    {
        // Double_t variable[7] = {-95.2,  667.7,  -67.9,  583.5,  -17.7, -17.6,  0.9833};
        // Double_t variable[9] = {-95.15,  667.7,  -67.87,  583.5,  -17.69, -17.63,  1., 5.8, 80.5};
        // Double_t variable[9] = {-94.98,  667.6,  -67.87,  583.4,  -17.64, -17.35,  1., 5.55, 80.53}; // best with
        // mag=1 Double_t variable[9] = {-94.85,  667.6,  -67.87,  583.4,  -17.67, -17.23,  1.019, 5.55, 80.53}; //best
        // with mag!=0 Double_t variable[9] = {-94.66,  667.7,  -67.73,  583.4,  -17.95, -17.08,  1.0, 5.35, 80.59};
        // //chi=739 Double_t variable[9] = {-94.46,  667.8,  -67.73,  583.4,  -18.15, -17.10,  1.0, 5.31, 80.49};
        // //chi=613 Double_t variable[9] = {-94.46,  667.8,  -67.73,  583.4,  -18.19, -17.08,  1.0, 5.31, 80.49}; //chi=
        // Double_t variable[9] = {-94.46,  667.9 , -67.73,  583.4,  -18.39, -17.28,  1.000,  5.310 , 80.49}; //chi=2672
        // Double_t variable[9] = {-94.52,  667.8 , -67.73,  583.2,  -18.58, -17.48,  1.000,  5.310 , 80.47}; //chi=2283
        // Double_t variable[9] = {-94.49,  667.8 , -67.73,  583.2,  -18.74, -17.61,  0.9897,  5.390 , 80.53}; //chi=910
        // Double_t variable[9] = {-94.45,  667.8 , -67.73,  583.0,  -18.72, -17.31,  0.9897,  5.390 , 80.53}; //chi=910
        // Double_t variable[9] = {-94.58,  667.8 , -67.73,  583.0,  -18.78, -17.27,  0.9997,  5.52 , 80.59}; //chi=919
        // best with 4He and 12C Double_t variable[9] = {-94.72,  667.8 , -67.62,  583.0,  -19.00, -16.95,  0.9997,  5.6
        // , 80.65}; //chi=532 only 4He Double_t variable[9] = {-94.62,  667.8 , -67.62,  583.0,  -18.97, -16.95, 0.9997,
        // 5.6 , 80.65}; //chi=532 only 4He Double_t variable[9] = {-94.62,  668.2 , -67.38,  582.9,  -19.37, -16.88,
        // 1.019,  5.59 , 80.47}; //chi=3313 only 4He Double_t variable[9] = {-94.46,  667.8 , -67.73,  583.4,  -18.15,
        // -17.10,  1.0,  5.31 , 80.49}; //chi=3313 only 12C Double_t variable[9] = {-94.56,  667.8 , -67.73,  583.4,
        // -18.25, -17.06,  1.0,  5.31 , 80.49}; //chi=3693 only 12C Double_t variable[9] = {-94.63,  667.8 , -67.73,
        // 583.4,  -18.68, -17.28,  1.009,  5.45 , 80.50}; //chi=3754 only 12C

        // Double_t variable[9] = {-94.67,  668.1 , -67.77,  583.4,  -17.7, -17.7,  1.000,  5.83 , 80.5}; //chi=13.51
        // only 12C Double_t variable[9] = {-94.64,  667.8 , -67.73,  583.2,  -18.68, -17.48,  1.009,  5.45 , 80.48};
        // //chi=13.51 only 12C Double_t variable[9] = {-94.82,  668.1 , -67.77,  583.4,  -17.83, -17.69,  1.019,  5.7
        // , 80.51}; //chi=15.94 only 12C Double_t variable[9] = {-94.93,  668.1 , -67.77,  583.4,  -17.64, -17.51,
        // 1.019,  5.7 , 80.51}; //chi=14.55 only 12C Double_t variable[9] = {-94.93,  668.1 , -67.77,  583.4,  -17.64,
        // -17.51,  0.990,  5.7 , 80.51}; //chi=14.52 only 12C Double_t variable[9] = {-94.77,  668.1 , -67.77,  583.4,
        // -17.83, -17.71,  0.980,  5.5 , 80.50}; //chi=16.01 only 12C Double_t variable[9] = {-94.69,  668.1 , -67.74,
        // 583.6,  -17.83, -17.80,  0.970,  5.3 , 80.64}; //chi=15.8 only 12C Double_t variable[9] = {-94.49,  668.3 ,
        // -67.69,  583.5,  -17.88, -17.87,  0.960,  5.24 , 80.7}; //chi=15.9 only 12C Double_t variable[9] = {-94.3,
        // 668.3 , -67.69,  583.5,  -17.88, -17.68,  0.950,  5.07 , 80.69}; //chi=15.0 only 12C Double_t variable[9] =
        // {-94.14,  668.3 , -67.69,  583.5,  -17.72, -17.73,  0.9570,  4.91 , 80.77}; //chi=15.56 only 12C Double_t
        // variable[9] = {-94.00,  668.3 , -67.57,  583.5,  -17.52, -17.59,  0.9570,  4.91 , 80.77}; //chi=15.15 only 12C

        // Double_t variable[9] = {-94.67,  668.1 , -67.77,  583.4,  -17.83, -17.71,  1.000,  5.7 , 80.50}; //chi=16.01
        // only 12C Double_t variable[9] = {-94.82,  668.1 , -67.77,  583.4,  -17.83, -17.69,  1.010,  5.56 , 80.47};
        // //chi=14.64 only 12C Double_t variable[9] = {-94.88,  668.1 , -67.77,  583.4,  -17.93, -17.63,  1.010,  5.56
        // , 80.47}; //chi=14.88 only 12C Double_t variable[9] = {-94.88,  668.1 , -67.77,  583.4,  -18.13, -17.69,
        // 1.010,  5.56 , 80.47}; //chi=14.85 only 12C Double_t variable[9] = {-94.98,  668.1 , -67.93,  583.4,  -18.09,
        // -17.83,  1.010,  5.56 , 80.47}; //chi=14.69 only 12C Double_t variable[9] = {-94.98,  668.1 , -67.93,  583.4,
        // -18.08, -17.83,  1.010,  5.56 , 80.47}; //chi=14.69 only 12C

        // Double_t variable[9] = {-94.64,  667.8 , -67.73,  583.2,  -18.68, -17.48,  1.009,  5.45 , 80.48}; //chi=13.51
        // only 12C Double_t variable[9] = {-94.76,  667.8 , -67.73,  583.2,  -18.31, -17.68,  1.009,  5.45 , 80.65};
        // //chi=14.47 only 12C Double_t variable[9] = {-94.76,  667.8 , -67.73,  583.2,  -18.12, -17.58,  1.009,  5.45
        // , 80.65}; //chi=14.35 only 12C

        //----------------------------------------------------
        // neuer Datensatz
        // Optimierung mit 12C ohne Bedingung auf Magnetfeld
        // Double_t variable[9] = {-94.75,  667.8 , -67.73,  583.2,  -18.23, -17.46,  1.009,  5.45 , 80.65}; //chi=10.24
        // only 12C Double_t variable[9] = {-94.75,  667.8 , -67.73,  583.2,  -18.23, -17.46,  1.009,  5.45 , 80.65};
        // //chi=10.24 only 12C

        // 12C und mit Zwang auf mag = 0.961
        // Double_t variable[9] = {-94.55,  668.0 , -67.55,  583.0,  -18.24, -17.27,  0.961,  5.25 , 80.68}; //chi=25.26
        // only 12C Double_t variable[9] = {-94.38,  668.0 , -67.42,  583.0,  -18.40, -17.24,  0.961,  5.05 , 80.73};
        // //chi=19.45 only 12C Double_t variable[9] = {-94.18,  667.8 , -67.42,  582.8,  -18.58, -17.43,  0.961,  4.850
        // , 80.63}; //chi=13.70 only 12C Double_t variable[9] = {-93.98,  668.0 , -67.40,  582.9,  -18.45, -17.28,
        // 0.961,  4.850 , 80.50}; //chi=10.57 only 12C Double_t variable[9] = {-93.96,  668.1 , -67.40,  582.9,  -18.47,
        // -17.20,  0.961,  4.850 , 80.50}; //chi=10.52 only 12C

        // 4He oder 12C und ohne Bedingung auf Magnetfeld
        // Double_t variable[9] = {-94.59,  667.8 , -67.73,  583.2,  -18.12, -17.39,  1.009,  5.44 , 80.65}; //chi=6.58
        // Double_t variable[9] = {-94.59,  667.9 , -67.73,  583.2,  -18.01, -17.23,  1.009,  5.44 , 80.65}; //chi=6.24
        // Double_t variable[9] = {-94.61,  668.0 , -67.73,  583.2,  -18.20, -17.23,  1.009,  5.44 , 80.65}; //chi=6.12
        // Double_t variable[9] = {-94.59,  668.2 , -67.73,  583.3,  -17.86, -17.29,  0.989,  5.52 , 80.58}; //chi=5.62
        // Double_t variable[9] = {-94.69,  668.2 , -67.73,  583.3,  -17.83, -17.44,  0.989,  5.52 , 80.58}; //chi=5.57
        // Double_t variable[9] = {-94.69,  668.2 , -67.73,  583.3,  -17.83, -17.44,  0.989,  5.52 , 80.58}; //chi=5.57

        // 4He oder 12C und Mit Zwang auf mag=1.006
        // Double_t variable[9] = {-94.60,  668.1 , -67.73,  583.2,  -18.04, -17.34,  1.006,  5.44 , 80.65}; //chi=6.87
        // Double_t variable[9] = {-94.63,  668.2 , -67.73,  583.2,  -17.88, -17.34,  1.006,  5.44 , 80.65}; //chi=6.90

        // 4He und ohne Bedingung auf Magnetfeld
        // Double_t variable[9] = {-94.69,  668.2 , -67.73,  583.3,  -17.83, -17.44,  0.989,  5.62 , 80.58}; //chi=5.38
        // Double_t variable[9] = {-94.69,  668.1 , -67.73,  583.3,  -17.76, -17.64,  0.989,  5.62 , 80.59}; //chi=5.30

        // 4He und ohne Bedingung auf Magnetfeld und ohne Impuls im chi2 mitzuzählen
        // Double_t variable[9] = {-94.85,  668.1 , -67.73,  583.2,  -17.74, -17.53,  0.969,  5.62 , 80.40}; //chi=3.45
        // Double_t variable[9] = {-94.86,  668.1 , -67.73,  583.4,  -17.73, -17.67,  0.969,  5.620 , 80.40}; //chi=3.22
        // Double_t variable[9] = {-94.86,  668.1 , -67.73,  583.4,  -17.74, -17.69,  0.9690,  5.620 , 80.40};
        // //chi=3.28 Double_t variable[9] = {-94.80,  668.1 , -67.73,  583.4,  -17.74, -17.79,  0.92760,  5.620
        // , 80.40}; //chi=3.10 Double_t variable[9] = {-94.78,  668.1 , -67.73,  583.4,  -17.67, -17.69,  0.9076,  5.624
        // , 80.42}; //chi=2.92 Double_t variable[9] = {-94.80,  668.1 , -67.73,  583.4,  -17.62, -17.75,  0.9076,  5.624
        // , 80.42}; //chi=2.92 Double_t variable[9] = {-94.80,  668.1 , -67.73,  583.4,  -17.57, -17.75,  0.9076,  5.624
        // , 80.42}; //chi=2.92

        // 4He und ohne Bedingung auf Magnetfeld aber mit Impuls im chi2 mitzuzählen (100*)
        // Double_t variable[9] = {-94.90,  667.9 , -67.65,  583.5,  -17.61, -17.75,  0.9476,  5.615 , 80.43};
        // //chi=31.54 Double_t variable[9] = {-94.81,  667.9 , -67.65,  583.5,  -17.43, -17.93,  0.9656,  5.620
        // , 80.43}; //chi=28.06 Double_t variable[9] = {-94.80,  667.9 , -67.65,  583.5,  -17.31, -17.81,  0.9656, 5.620
        // , 80.43}; //chi=28.00 Double_t variable[9] = {-94.80,  667.9 , -67.65,  583.5,  -17.31, -17.81,  0.9656, 5.620
        // , 80.43}; //chi=28.00

        // 4He und ohne Bedingung auf Magnetfeld aber mit Impuls im chi2 mitzuzählen (1000*)
        // Double_t variable[9] = {-94.61,  668.1 , -67.67,  583.7,  -17.18, -17.75,  0.9846,  5.581 , 80.42};
        // //chi=40.73 Double_t variable[9] = {-94.44,  668.1 , -67.67,  583.7,  -17.31, -17.94,  0.9846,  5.578
        // , 80.43}; //chi=39.81 Double_t variable[9] = {-94.46,  668.2 , -67.67,  583.7,  -17.25, -17.99,  0.9846, 5.578
        // , 80.43}; //chi=39.83 Double_t variable[9] = {-94.47,  668.2 , -67.67,  583.7,  -17.24, -17.93,  0.9846, 5.578
        // , 80.43}; //chi=39.84

        // 4He und ohne Bedingung auf Magnetfeld aber mit Impuls im chi2 mitzuzählen (10000*)
        // Double_t variable[9] = {-94.47,  668.2 , -67.67,  583.9,  -17.08, -17.94,  1.001,  5.578 , 80.43}; //chi=486
        // Double_t variable[9] = {-94.59,  668.2 , -67.65,  583.9,  -16.91, -18.03,  1.001,  5.578 , 80.43}; //chi=485
        // Double_t variable[9] = {-94.58,  668.2 , -67.65,  583.9,  -16.97, -18.23,  1.001,  5.578 , 80.43};
        // //chi=484.6

        // 12C und ohne Bedingung auf Magnetfeld aber mit Impuls im chi2 mitzuzählen (10000*)
        // Double_t variable[9] = {-94.41,  668.2 , -67.65,  583.9,  -16.90, -18.21,  1.018,  5.570 , 80.43}; //chi=558
        // Double_t variable[9] = {-94.41,  668.2 , -67.65,  583.9,  -16.81, -18.01,  1.018,  5.570 , 80.43}; //chi=557
        Double_t variable[9] = { -94.40, 668.2, -67.65, 583.9, -17.01, -18.11, 1.018, 5.570, 80.43 }; // chi=557

        Double_t step[9] = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.01, 0.01, 0.01 };

        mini->SetFunction(fm1);

        // Set the free variables to be minimized!
        mini->SetLimitedVariable(0, "x10", variable[0], step[0], variable[0] - 0.5, variable[0] + 0.5);
        mini->SetLimitedVariable(1, "z10", variable[1], step[1], variable[1] - 0.5, variable[1] + 0.5);
        mini->SetLimitedVariable(2, "x12", variable[2], step[2], variable[2] - 0.5, variable[2] + 0.5);
        mini->SetLimitedVariable(3, "z12", variable[3], step[3], variable[3] - 0.5, variable[3] + 0.5);
        mini->SetLimitedVariable(4, "dangle10", variable[4], step[4], variable[4] - 0.5, variable[4] + 0.5);
        mini->SetLimitedVariable(5, "dangle12", variable[5], step[5], variable[5] - 0.5, variable[5] + 0.5);
        mini->SetLimitedVariable(6, "dmag", variable[6], step[6], variable[6] - 0.05, variable[6] + 0.05);
        mini->SetLimitedVariable(7, "x3b", variable[7], step[7], variable[7] - 0.5, variable[7] + 0.5);
        mini->SetLimitedVariable(8, "z3b", variable[8], step[8], variable[8] - 0.5, variable[8] + 0.5);
    }
    else
    {
        // Double_t variable[7] = { -129.9,  589.7,  -157.7,  676.0,  -198.2,  -196.9,  0.9433};
        // Double_t variable[9] = { -130.0,  589.6,  -157.8,  676.0,  -198.3,  -196.8,  1.0, -5.8, 80.5};
        // Double_t variable[9] = { -130.0,  589.6,  -157.8,  676.0,  -198.2,  -196.8,  1.0, -5.68, 80.64};
        // Double_t variable[9] = { -129.8,  589.6,  -157.8,  676.0,  -198.4,  -196.9,  1.0, -5.58, 80.68}; // best with
        // mag=1 Double_t variable[9] = { -129.8,  589.7,  -157.9,  676.0,  -198.7,  -196.6,  0.9890, -5.46, 80.65}; //
        // best with mag!=1 Double_t variable[9] = { -129.7,  589.7,  -157.9,  676.0,  -198.7,  -196.5,  1.0,
        // -5.31, 80.67}; // chi=248.7 Double_t variable[9] = { -129.7,  589.7,  -157.9,  676.0,  -198.8,  -196.4,  1.0,
        // -5.31, 80.67}; // chi=247 Double_t variable[9] = { -129.8,  589.7,  -158.0,  676.0,  -198.9,  -196.3,  1.0,
        // -5.23, 80.68}; // chi=1040 Double_t variable[9] = { -129.8,  589.7,  -158.0,  676.0,  -198.9,  -196.2,  1.0,
        // -5.35, 80.71}; // chi= Double_t variable[9] = { -129.7,  589.7,  -158.1,  676.0,  -198.9,  -196.2,  1.000,
        // -5.350,  80.71}; // chi=7456 Double_t variable[9] = { -129.7,  589.7,  -158.1,  676.0,  -198.9,  -196.2,
        // 1.000,  -5.350,  80.71}; // chi=7456 Double_t variable[9] = { -129.7,  589.7,  -158.1,  676.0,  -198.9,
        // -196.2,  1.000,  -5.350,  80.71}; // chi=7643 Double_t variable[9] = { -129.4,  589.5,  -158.1,  676.4,
        // -198.6,  -196.3,  0.9997,  -5.270,  80.68}; // best chi=5514 Double_t variable[9] = { -129.6,  589.5,  -158.1,
        // 676.4,  -198.6,  -196.4,  0.98,  -5.270,  80.68}; // chi=5514 Double_t variable[9] = { -129.6,  589.5, -158.1,
        // 676.4,  -198.8,  -196.4,  0.9997,  -5.270,  80.67}; // chi=5700 Double_t variable[9] = { -129.6,  589.5,
        // -158.1,  676.4,  -198.7,  -196.4,  0.9997,  -5.270,  80.67}; // chi=5439  best with 4He and 12C Double_t
        // variable[9] = { -129.5,  589.5,  -158.0,  676.4,  -198.2,  -196.4,  0.9997,  -5.210,  80.64}; // chi=2291 only
        // 4He Double_t variable[9] = { -129.6,  589.5,  -158.0,  676.5,  -198.4,  -196.5,  1.013,  -5.11,  80.61}; //
        // chi=37677  only 12C Double_t variable[9] = { -129.7,  589.6,  -158.1,  676.5,  -198.4,  -196.6,  1.013, -5.28,
        // 80.58}; // chi=7418  only 4He Double_t variable[9] = { -129.7,  589.6,  -158.1,  676.0,  -198.8,  -196.4, 1.0,
        // -5.31,  80.67}; // chi=7418  only 4He Double_t variable[9] = { -129.6,  589.5,  -158.1,  676.0,  -198.9,
        // -196.5,  1.011,  -5.2,  80.54}; // chi=7955  only 4He

        // Double_t variable[9] = { -127.9,  587.7,  -156.5,  674.7,  -197.7,  -197.7,  1.000,  -5.83,  80.50}; //
        // chi=13.16  only 12C Double_t variable[9] = { -129.6,  589.5,  -158.1,  676.2,  -199.2,  -196.5,  0.981,
        // -5.197,  80.53}; // chi=13.16  only 12C Double_t variable[9] = { -128.1,  587.5,  -156.3,  674.7,  -197.9,
        // -197.7,  0.980,  -5.51,  80.59}; // chi=20.15  only 12C Double_t variable[9] = { -128.1,  587.5,  -156.3,
        // 674.7,  -197.9,  -197.8,  0.9770,  -5.51,  80.58}; // chi=18.65  only 12C Double_t variable[9] = { -128.1,
        // 587.5,  -156.3,  674.7,  -197.9,  -198.0,  0.9570,  -5.45,  80.74}; // chi=12.66  only 12C Double_t
        // variable[9] = { -128.1,  587.5,  -156.3,  674.7,  -198.1,  -198.1,  0.9570,  -5.45,  80.89}; // chi=12.08 only
        // 12C

        //----------------------------------------------------
        // neuer Datensatz
        // Optimierung mit 12C ohne Bedingung auf Magnetfeld
        // Double_t variable[9] = { -128.1,  587.6,  -156.3,  674.7,  -198.1,  -197.9,  0.961,  -5.46,  80.91}; //
        // chi=10.77  only 12C Double_t variable[9] = { -128.1,  587.6,  -156.3,  674.7,  -198.1,  -197.9,  0.961, -5.46,
        // 80.91}; // chi=10.77  only 12C

        // 12C und mit Zwang auf mag = 1.009
        // Double_t variable[9] = { -128.3,  587.8,  -156.5,  674.6,  -198.0,  -197.9,  1.009,  -5.27,  81.06}; //
        // chi=20.47  only 12C Double_t variable[9] = { -128.2,  587.8,  -156.6,  674.7,  -198.2,  -197.7,  1.009, -5.09,
        // 81.06}; // chi=15.72  only 12C Double_t variable[9] = { -128.1,  587.8,  -156.7,  674.7,  -198.4,  -197.7,
        // 1.009,  -4.89,  81.02}; // chi=12.31  only 12C Double_t variable[9] = { -128.1,  587.8,  -156.7,  674.7,
        // -198.4,  -197.7,  1.009,  -4.89,  80.95}; // chi=11.93  only 12C Double_t variable[9] = { -128.1,  587.8,
        // -156.7,  674.7,  -198.4,  -197.5,  1.009,  -4.89,  80.96}; // chi=11.85  only 12C

        // 4He oder 12C und ohne Bedingung auf Magnetfeld
        // Double_t variable[9] = { -128.1,  587.6,  -156.3,  674.7,  -198.1,  -197.7,  1.003,  -5.46,  80.97}; //
        // chi=7.91 Double_t variable[9] = { -128.1,  587.6,  -156.3,  674.7,  -198.2,  -197.7,  1.003,  -5.46,  80.95};
        // // chi=7.47 Double_t variable[9] = { -128.1,  587.6,  -156.3,  674.7,  -198.3,  -197.3,  1.006,  -5.44,
        // 80.88}; // chi=7.56 Double_t variable[9] = { -128.1,  587.7,  -156.3,  674.6,  -198.5,  -197.4,  1.006, -5.44,
        // 80.88}; // chi=7.50 Double_t variable[9] = { -128.1,  587.7,  -156.3,  674.6,  -198.7,  -197.5,  1.006, -5.44,
        // 80.88}; // chi=7.50

        // 4He oder 12C und Mit Zwang auf 1.006
        // Double_t variable[9] = { -128.1,  587.6,  -156.3,  674.7,  -198.3,  -197.5,  1.006,  -5.44,  80.95}; //
        // chi=7.73 Double_t variable[9] = { -128.1,  587.6,  -156.3,  674.7,  -198.5,  -197.4,  1.006,  -5.44,  80.95};
        // // chi=7.55

        // 4He und ohne Bedingung auf Magnetfeld
        // Double_t variable[9] = { -128.1,  587.7,  -156.3,  674.6,  -198.7,  -197.5,  1.006,  -5.44,  80.88}; //
        // chi=7.35 Double_t variable[9] = { -128.1,  587.7,  -156.3,  674.6,  -198.7,  -197.5,  1.006,  -5.44,  80.88};
        // // chi=7.35

        // 4He und ohne Bedingung auf Magnetfeld und ohne Impuls im chi2 mitzubrücksichtigen
        // Double_t variable[9] = { -128.1,  587.7,  -156.3,  674.6,  -198.7,  -197.5,  1.006,  -5.44,  80.88}; //
        // chi=5.23 Double_t variable[9] = { -128.1,  587.8,  -156.3,  674.6,  -198.6,  -197.7,  0.9456,  -5.29,  80.90};
        // // chi=5.64 Double_t variable[9] = { -128.1,  587.7,  -156.3,  674.6,  -198.7,  -197.6,  0.9276,  -5.33,
        // 80.78}; // chi=3.98 Double_t variable[9] = { -128.1,  587.7,  -156.3,  674.6,  -198.6,  -197.4,  0.9276,
        // -5.33,  80.78}; // chi=3.87 Double_t variable[9] = { -128.1,  587.7,  -156.3,  674.6,  -198.6,  -197.4,
        // 0.9276,  -5.33,  80.78}; // chi=3.83 Double_t variable[9] = { -128.1,  587.7,  -156.3,  674.6,  -198.6,
        // -197.3,  0.9276,  -5.330,  80.78}; // chi=3.83 Double_t variable[9] = { -128.1,  587.7,  -156.3,  674.6,
        // -198.6,  -197.3,  0.8956,  -5.330,  80.78}; // chi=3.77

        // 4He und ohne Bedingung auf Magnetfeld aber mit Impuls im chi2 mitzuzählen (100*)
        // Double_t variable[9] = { -128.1,  587.7,  -156.4,  674.6,  -198.7,  -197.2,  0.9356,  -5.350,  80.79}; //
        // chi=28.28 Double_t variable[9] = { -128.0,  587.7,  -156.4,  674.6,  -198.8,  -197.0,  0.9516,  -5.356,
        // 80.80}; // chi=27.42 Double_t variable[9] = { -128.1,  587.7,  -156.4,  674.6,  -198.7,  -196.8,  0.9516,
        // -5.356,  80.80}; // chi=27.22 Double_t variable[9] = { -128.1,  587.7,  -156.4,  674.6,  -198.7,  -196.7,
        // 0.9516,  -5.356,  80.80}; // chi=27.21

        // 4He und ohne Bedingung auf Magnetfeld aber mit Impuls im chi2 mitzuzählen (1000*)
        // Double_t variable[9] = { -128.1,  587.7,  -156.4,  674.6,  -199.0,  -196.9,  0.9846,  -5.365,  80.80}; //
        // chi=50.07 Double_t variable[9] = { -128.1,  587.7,  -156.4,  674.6,  -199.2,  -196.9,  0.9846,  -5.365,
        // 80.80}; // chi=50.35 Double_t variable[9] = { -128.1,  587.7,  -156.4,  674.6,  -199.4,  -197.0,  0.9846,
        // -5.365,  80.80}; // chi=50.18 Double_t variable[9] = { -128.1,  587.7,  -156.4,  674.6,  -199.4,  -197.0,
        // 0.9846,  -5.365,  80.80}; // chi=50.34

        // 4He und ohne Bedingung auf Magnetfeld aber mit Impuls im chi2 mitzuzählen (10000*)
        // Double_t variable[9] = { -128.2,  587.7,  -156.4,  674.6,  -199.4,  -197.0,  0.9846,  -5.354,  80.80}; //
        // chi=470 Double_t variable[9] = { -128.1,  587.7,  -156.4,  674.6,  -199.5,  -197.2,  0.9846,  -5.354,  80.80};
        // // chi=469 Double_t variable[9] = { -128.2,  587.7,  -156.4,  674.6,  -199.6,  -197.3,  0.9846,  -5.354,
        // 80.80}; // chi=469.2

        // 12C und ohne Bedingung auf Magnetfeld aber mit Impuls im chi2 mitzuzählen (10000*)
        // Double_t variable[9] = { -128.4,  587.7,  -156.4,  674.6,  -199.5,  -197.1,  0.9776,  -5.365,  80.80}; //
        // chi=411 Double_t variable[9] = { -128.4,  587.7,  -156.4,  674.6,  -199.5,  -197.1,  0.9776,  -5.365,  80.80};
        // // chi=404
        Double_t variable[9] = { -128.4, 587.7, -156.4, 674.6, -199.5, -197.2, 0.9776, -5.365, 80.80 }; // chi=404

        Double_t step[9] = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.01, 0.01, 0.01 };

        mini->SetFunction(fm1);

        // Set the free variables to be minimized!
        mini->SetLimitedVariable(0, "x11", variable[0], step[0], variable[0] - 0.5, variable[0] + 0.5);
        mini->SetLimitedVariable(1, "z11", variable[1], step[1], variable[1] - 0.5, variable[1] + 0.5);
        mini->SetLimitedVariable(2, "x13", variable[2], step[2], variable[2] - 0.5, variable[2] + 0.5);
        mini->SetLimitedVariable(3, "z13", variable[3], step[3], variable[3] - 0.5, variable[3] + 0.5);
        mini->SetLimitedVariable(4, "dangle11", variable[4], step[4], variable[4] - 0.5, variable[4] + 0.5);
        mini->SetLimitedVariable(5, "dangle13", variable[5], step[5], variable[5] - 0.5, variable[5] + 0.5);
        mini->SetLimitedVariable(6, "dmag", variable[6], step[6], variable[6] - 0.05, variable[6] + 0.05);
        mini->SetLimitedVariable(7, "x3a", variable[7], step[7], variable[7] - 0.5, variable[7] + 0.5);
        mini->SetLimitedVariable(8, "z3a", variable[8], step[8], variable[8] - 0.5, variable[8] + 0.5);
    }
    Int_t status = 0;

    // do the minimization
    mini->Minimize();

    status = mini->Status();
    cout << "Status: " << status << endl;
    cout << "optimized values: " << endl;
    if (fLeft)
    {
        cout << "dx10: " << mini->X()[0] << " dz10 " << mini->X()[1] << endl;
        cout << "dx12: " << mini->X()[2] << " dz12 " << mini->X()[3] << endl;
        cout << "dangle10: " << mini->X()[4] << "dangle12: " << mini->X()[5] << endl;
        cout << "mag field: " << mini->X()[6] << endl;
        cout << "dx3b: " << mini->X()[7] << " dz3b " << mini->X()[8] << endl;
    }
    else
    {
        cout << "dx11: " << mini->X()[0] << " dz11 " << mini->X()[1] << endl;
        cout << "dx13: " << mini->X()[2] << " dz13 " << mini->X()[3] << endl;
        cout << "dangle11: " << mini->X()[4] << " dangle13: " << mini->X()[5] << endl;
        cout << "mag field: " << mini->X()[6] << endl;
        cout << "dx3a: " << mini->X()[7] << " dz3a " << mini->X()[8] << endl;
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
        fh_mult_fi3b->Draw();
        c11->cd(2);
        fh_mult_fi12->Draw();
        c11->cd(3);
        fh_mult_fi3a->Draw();
        c11->cd(4);
        fh_mult_tofd->Draw();
        c11->cd(0);

        new TCanvas("c2", "", 50, 50, 400, 400);
        fh_eloss_fi3a_mc->SetLineStyle(2);
        fh_eloss_fi3a_mc->SetLineColor(2);
        fh_eloss_fi3a_mc->Draw();
        fh_eloss_fi3a->Draw("same");

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

        LOG(info) << fwhm / fh_A_reco2->GetMean();

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

Bool_t R3BOptimizeGeometryS454::InitPropagator()
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

ClassImp(R3BOptimizeGeometryS454)
