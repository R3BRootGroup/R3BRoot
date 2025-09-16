/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// Created on 16/09/2025 by V.Panin

#include "R3BTrackingG249.h"
#include "R3BEventHeader.h"
#include "R3BFiberMAPMTHitData.h"
#include "R3BFrsData.h"
#include "R3BLosHitData.h"
#include "R3BTofdHitData.h"
#include "R3BTrackingParticle.h"

#include "R3BLogger.h"
#include "R3BMCTrack.h"
#include "R3BMDFWrapper.h"
#include "R3BTrack.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TCutG.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TVector3.h"

#include "Math/Factory.h"
#include "Math/Functor.h"
#include "Math/GSLMinimizer.h"
#include "Math/Minimizer.h"
#include "Minuit2/Minuit2Minimizer.h"

#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

R3BTrackingG249::R3BTrackingG249()
    : R3BTrackingG249("TrackingG249", 1)
{
}

R3BTrackingG249::R3BTrackingG249(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fNEvents(0)
    , maxevent(0)
    , DoAlignment(false)
    , reference_PoQ(0.)
    , delta_PoQ(0.)
    , GladCurrent(-1)
    , GladReferenceCurrent(-1)
    , FrsCutMeanZ(-1)
    , FrsCutRadiusZ(-1)
    , FrsCutMeanAoZ(-1)
    , FrsCutRadiusAoZ(-1)
    , FrsBrhoMin(-1)
    , FrsBrhoMax(-1)
    , FiberEnergyMin(-1)
    , FiberEnergyMax(-1)
    , FiberTimeMin(0)
    , FiberTimeMax(0)
    , TofdEnergyMin(-1)
    , TofdEnergyMax(-1)
{
}

R3BTrackingG249::~R3BTrackingG249()
{
    if (MDF_PoQ)
        delete MDF_PoQ;
}

// Check if all inputs parameters and cuts are properly set in the steering  macros
bool R3BTrackingG249::MacroParametersAreOk()
{
    if (GladCurrent < 0 || GladReferenceCurrent < 0 || FrsCutMeanZ < 0 || FrsCutMeanAoZ < 0 || FrsCutRadiusZ < 0 ||
        FrsCutRadiusAoZ < 0 || FiberEnergyMin < 0 || FiberEnergyMax < 0 || FiberTimeMin == FiberTimeMax)
        return false;
    else
        return true;
}

InitStatus R3BTrackingG249::Init()
{
    R3BLOG(info, "");

    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, !mgr, "FairRootManager not found");

    fHeader = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    R3BLOG_IF(fatal, !fHeader, "EventHeader. is not found");

    assert(DET_MAX + 1 == sizeof(fDetectorNames) / sizeof(fDetectorNames[0]));
    R3BLOG(info, Form("Reading %d fiber detectors", NOF_FIB_DET));

    // Reading all detector branches
    for (auto det = 0; det < DET_MAX; det++)
    {
        fDataItems.push_back(dynamic_cast<TClonesArray*>(mgr->GetObject(Form("%s", fDetectorNames[det]))));
        R3BLOG_IF(fatal, !fDataItems.at(det), Form("Cannot find tree branch %s", fDetectorNames[det]));
    }

    if (!MacroParametersAreOk()) // check essential input parameters form the macro
        R3BLOG(fatal, Form("Some cuts are not set in the macros or negative values are used"));

    MakeDetectorPlanes(); // fix orientation of det planes behind GLAD

    // Initializing all MDF functions
    R3BLOG(info, "Reading MDF function for PoQ");
    MDF_PoQ = new R3BMDFWrapper(MDF_PoQ_filename.Data());

    // Declare output TClonesArray
    fTrackItems = new TClonesArray("R3BTrackingParticle");
    mgr->Register("FragmentMDFTrack", "Fragment_tracks_from_MDF", fTrackItems, kTRUE);
    Reset(); // reset output TClones array

    if (DoAlignment)
        align_data.clear();

    h2_lab_XZ = new TH2F("h2_lab_XZ", "h2_lab_XZ", 1000, -200, 1000, 1000, -600, 600);

    R3BLOG(info, "R3BTrackingG249 task is initialized!");
    return kSUCCESS;
}

bool R3BTrackingG249::Get_all_TCA_with_MinimumMul()
{
    // Get TClonesArrays from every detector
    los_DataItems = fDataItems.at(LOS_HITDATA);
    frs_DataItems = fDataItems.at(FRS_DATA);
    foot_DataItems = fDataItems.at(DET_FOOT);
    foot_incoming_tracks = fDataItems.at(INCOMING_FOOT_TRACK);
    foot_outgoing_tracks = fDataItems.at(OUTGOING_FOOT_TRACK);
    tofd_DataItems = fDataItems.at(DET_TOFD);

    // Minimum multiplicity condition
    if (frs_DataItems->GetEntriesFast() < 1 || los_DataItems->GetEntriesFast() < 1 ||
        tofd_DataItems->GetEntriesFast() < 1 || foot_incoming_tracks->GetEntriesFast() < 1 ||
        foot_outgoing_tracks->GetEntriesFast() < 1 || fDataItems.at(DET_FI30)->GetEntriesFast() < 1 ||
        fDataItems.at(DET_FI32)->GetEntriesFast() < 1 ||
        (fDataItems.at(DET_FI31)->GetEntriesFast() < 1 && fDataItems.at(DET_FI33)->GetEntriesFast() < 1))
        return false;
    else
        return true;
}

bool R3BTrackingG249::Construct_FiberTrack()
{
    // Check the map and pick three fibers for tracking
    auto it_f32 = maxHitPerFiber.find(DET_FI32);
    auto it_f30 = maxHitPerFiber.find(DET_FI30);
    auto it_f33 = maxHitPerFiber.find(DET_FI33);
    auto it_f31 = maxHitPerFiber.find(DET_FI31);

    if (it_f32 == maxHitPerFiber.end() || it_f30 == maxHitPerFiber.end())
        return false; // first two fibers must see some hits

    if (it_f33 == maxHitPerFiber.end() && it_f31 == maxHitPerFiber.end())
        return false; // at least one hit in the last 2 fibers

    fiber_track.f1.SetXYZ(it_f32->second->GetX(), 0, 0);
    fiber_track.f2.SetXYZ(0, it_f30->second->GetY(), 0);

    if (it_f33 == maxHitPerFiber.end())
    {
        fiber_track.f3.SetXYZ(it_f31->second->GetX(), 0, 0);
        fiber_track.last_fiber = Form("Fib%d", 31);
    }

    if (it_f31 == maxHitPerFiber.end())
    {
        fiber_track.f3.SetXYZ(it_f33->second->GetX(), 0, 0);
        fiber_track.last_fiber = Form("Fib%d", 33);
    }

    if (it_f31 != maxHitPerFiber.end() && it_f33 != maxHitPerFiber.end()) // found hits in both last fibers
    {
        // take the one with highest energy
        if (it_f33->second->GetEloss() > it_f31->second->GetEloss())
        {
            fiber_track.f3.SetXYZ(it_f33->second->GetX(), 0, 0);
            fiber_track.last_fiber = Form("Fib%d", 33);
        }
        else
        {
            fiber_track.f3.SetXYZ(it_f31->second->GetX(), 0, 0);
            fiber_track.last_fiber = Form("Fib%d", 31);
        }
    }
    // if(fiber_track.last_fiber == "Fib31")
    //     return false;

    if (DoAlignment) // store fiber hits for the alignment
    {
        track_for_alignment.fib_track = fiber_track;
        align_data.push_back(track_for_alignment);
    }

    //------- Transform Fiber hits to the lab coordinates
    // This will modify vectors inside fiber_track struct
    TransformPoint("Det2Lab", fiber_track.f1, &f32_angles, &f32_position);
    TransformPoint("Det2Lab", fiber_track.f2, &f30_angles, &f30_position);

    (fiber_track.last_fiber == "Fib31") ? TransformPoint("Det2Lab", fiber_track.f3, &f31_angles, &f31_position)
                                        : TransformPoint("Det2Lab", fiber_track.f3, &f33_angles, &f33_position);

    //------ Fixup fiber track by extrapolating X and Z coordinates of f30 (Y)
    FixupFiberTrack();
    return true;
}

void R3BTrackingG249::Exec(Option_t* /*option*/)
{
    fNEvents += 1;
    if (fTrigger > 0 && (fHeader->GetTrigger() != fTrigger))
        return;
    if (fTpat > 0 && (fHeader->GetTpat() != fTpat))
        return;
    if (!Get_all_TCA_with_MinimumMul())
        return;
    Reset(); // Clear output TClonesArray

    //------------- Get LOS Data ---------------
    // for now using only one/first hit
    auto los_item = dynamic_cast<R3BLosHitData*>(los_DataItems->At(0));
    R3BLOG_IF(fatal, !los_item, "ERROR in the LOS data");

    //------------- Get FRS Data ---------------
    double frsZ = -1;
    double frsAoQ = -1;
    double frsBrho = -1;
    double frsBeta = -1;
    for (auto j = 0; j < frs_DataItems->GetEntriesFast(); ++j)
    {
        auto frs_item = dynamic_cast<R3BFrsData*>(frs_DataItems->At(j));
        R3BLOG_IF(fatal, !frs_item, "ERROR in the FRS data");
        if (frs_item->GetStaId() != 2)
            continue; // 2 - new VFTX with 10ps resolution
        frsZ = frs_item->GetZ();
        frsAoQ = frs_item->GetAq();
        frsBrho = frs_item->GetBrho();
        frsBeta = frs_item->GetBeta();
    }
    if (frsZ < 0 || frsAoQ < 0 || frsBrho < 0 || frsBeta < 0)
        return;
    double frsGamma = 1. / sqrt(1 - pow(frsBeta, 2));

    // Check if the event is within FRS cut
    if (FrsCutMeanZ > 0 && FrsCutMeanAoZ > 0 && !IsInsideFrsEllipticPIDCut(frsZ, frsAoQ))
        return;

    //-------------- Reading TOFD data ------------
    R3BTofdHitData* tofd_hit = nullptr;
    bool is_good_tofd = false;
    for (auto i = 0; i < tofd_DataItems->GetEntriesFast(); ++i)
    {
        tofd_hit = dynamic_cast<R3BTofdHitData*>(tofd_DataItems->At(i));
        R3BLOG_IF(fatal, !tofd_hit, "ERROR in the TOFD data");
        if (tofd_hit->GetDetId() == 1) // only hits from first plane
        {
            is_good_tofd = true;
            break;
        }
    }
    if (!is_good_tofd)
        return;

    // Check if tofd charge is within cuts
    if (TofdEnergyMin > 0 && TofdEnergyMax > 0 &&
        (tofd_hit->GetEloss() < TofdEnergyMin || tofd_hit->GetEloss() > TofdEnergyMax))
        return;

    //------------- Get incoming and outgoing tracks in FOOTs ----------
    auto in_foot_track = dynamic_cast<R3BTrackingParticle*>(foot_incoming_tracks->At(0));
    auto out_foot_track = dynamic_cast<R3BTrackingParticle*>(foot_outgoing_tracks->At(0));
    if (!in_foot_track || !out_foot_track)
        return;

    // This one will be stored into the output tree:
    R3BTrackingParticle global_track = *out_foot_track;

    if (DoAlignment)
    {
        track_for_alignment.foot_track = *out_foot_track;
        track_for_alignment.reference_PoQ = frsBrho / 3.3356 - delta_PoQ; // Brho to PoQ [GeV/c/e] conversion factor
    }
    Collect_FiberHits();
    if (!Construct_FiberTrack() || DoAlignment)
        return;

    h2_lab_XZ->Fill(out_foot_track->GetStartPosition().Z() * 0.1, out_foot_track->GetStartPosition().X() * 0.1);
    h2_lab_XZ->Fill(fiber_track.f1.Z(), fiber_track.f1.X());
    h2_lab_XZ->Fill(fiber_track.f2.Z(), fiber_track.f2.X());
    h2_lab_XZ->Fill(fiber_track.f3.Z(), fiber_track.f3.X());

    double start_X = out_foot_track->GetStartPosition().X() * 0.1; // must be cm
    double start_Y = out_foot_track->GetStartPosition().Y() * 0.1; // must be cm
    double start_Z = out_foot_track->GetStartPosition().Z() * 0.1; // must be cm
    double start_TX = out_foot_track->GetStartMomentum().X() / out_foot_track->GetStartMomentum().Z();
    double start_TY = out_foot_track->GetStartMomentum().Y() / out_foot_track->GetStartMomentum().Z();

    int k = 0;
    mdf_data[k] = start_X;
    mdf_data[++k] = start_Y;
    mdf_data[++k] = start_Z;
    mdf_data[++k] = start_TX;
    mdf_data[++k] = start_TY;
    mdf_data[++k] = fiber_track.f1.X(); // must be in cm
    mdf_data[++k] = fiber_track.f1.Z(); // must be in cm
    mdf_data[++k] = (fiber_track.f3.X() - fiber_track.f1.X()) / (fiber_track.f3.Z() - fiber_track.f1.Z());
    mdf_data[++k] = (fiber_track.f2.Y() - start_Y) / (fiber_track.f2.Z() - start_Z);

    Double_t PoQ = MDF_PoQ->MDF(mdf_data) * GladCurrent / GladReferenceCurrent;
    Double_t AoZ = PoQ / frsBeta / frsGamma / AMU;

    // Configure the global track and store it
    TVector3 fragment_momentum = global_track.GetStartMomentum();
    fragment_momentum.SetMag(PoQ);
    global_track.SetStartMomentum(fragment_momentum);
    global_track.SetCharge(tofd_hit->GetEloss());
    global_track.SetMass(AoZ);
    global_track.SetBeta(frsBeta);
    AddTrackData(global_track);
    return;
}

void R3BTrackingG249::Collect_FiberHits()
{
    //------------- Get Fiber Hits - only highest energy hit in every detector --------
    maxHitPerFiber.clear();
    maxHitTCAIndex.clear();
    for (auto f = 0; f < NOF_FIB_DET; ++f)
    {
        for (auto i = 0; i < fDataItems.at(f)->GetEntriesFast(); ++i)
        {
            auto fiber_hit = dynamic_cast<R3BFiberMAPMTHitData*>(fDataItems.at(f)->At(i));
            R3BLOG_IF(fatal, !fiber_hit, "ERROR in the Fiber data");
            auto fiber_energy = fiber_hit->GetEloss();
            auto fiber_time = fiber_hit->GetTime();
            // Energy window
            if (FiberEnergyMin > 0 && FiberEnergyMax > 0 &&
                (fiber_energy < FiberEnergyMin || fiber_energy > FiberEnergyMax))
                continue;
            // Time window
            if (FiberTimeMin != FiberTimeMax && (fiber_time < FiberTimeMin || fiber_time > FiberTimeMax))
                continue;

            auto it = maxHitPerFiber.find(f); // seacrh this fiber in the map
            if (it == maxHitPerFiber.end())   // first hit for this fiber
            {
                maxHitPerFiber[f] = fiber_hit;
                maxHitTCAIndex[f] = i;
            }
            else // compare with current max energy
            {
                if (fiber_energy > it->second->GetEloss())
                {
                    maxHitPerFiber[f] = fiber_hit;
                    maxHitTCAIndex[f] = i;
                }
            }
        }
    }
}

void R3BTrackingG249::Reset()
{
    if (fTrackItems)
        fTrackItems->Clear();
}

void R3BTrackingG249::FinishEvent()
{
    for (auto& DataItem : fDataItems)
    {
        DataItem->Clear();
    }
    fTrackItems->Clear();
}

void R3BTrackingG249::FinishTask()
{
    LOG(info) << "Processed " << fNEvents << " events\n\n";
    h2_lab_XZ->Write();
    if (DoAlignment)
        Alignment();
}

void R3BTrackingG249::TransformPoint(TString flag, TVector3& hit_point, TVector3* rotation, TVector3* det_offset)
{
    TRotation r;
    // First Euler rotation around Y axis
    r.RotateY(rotation->Y());

    // get local X axis after first rotation
    TVector3 v3_localX(1, 0, 0);
    v3_localX.SetTheta(r.ThetaX());
    v3_localX.SetPhi(r.PhiX());

    // Second Euler rotation around local X axis
    r.Rotate(rotation->X(), v3_localX);

    // get local Z axis after second rotation
    TVector3 v3_localZ(0, 0, 1);
    v3_localZ.SetTheta(r.ThetaZ());
    v3_localZ.SetPhi(r.PhiZ());

    // final rotation around local Z axis
    r.Rotate(rotation->Z(), v3_localZ);

    if (flag == "Det2Lab")
    {
        hit_point.Transform(r);
        hit_point += *det_offset;
    }
    else if (flag == "Lab2Det")
    {
        hit_point -= *det_offset;
        auto rr = r.Inverse();
        hit_point.Transform(rr);
    }
    else
    {
        R3BLOG(fatal, "Wrong flag in the TransformPoint() function");
    }
}

void R3BTrackingG249::SetFrsEllipticPIDCut(double meanZ, double rZ, double meanAoZ, double rAoZ)
{
    FrsCutMeanZ = meanZ;
    FrsCutRadiusZ = rZ;
    FrsCutMeanAoZ = meanAoZ;
    FrsCutRadiusAoZ = rAoZ;
}

bool R3BTrackingG249::IsInsideFrsEllipticPIDCut(double z, double aoz)
{
    Double_t ell = pow(((z - FrsCutMeanZ) / (FrsCutRadiusZ)), 2) + pow(((aoz - FrsCutMeanAoZ) / (FrsCutRadiusAoZ)), 2);
    return (ell <= 1.);
}

void R3BTrackingG249::FixupFiberTrack()
{
    // Define two (X,Z) points of the fib30 plane:
    TVector3 f30_edge[2];
    f30_edge[0].SetXYZ(-1, 0, 0);
    f30_edge[1].SetXYZ(1, 0, 0);
    TransformPoint("Det2Lab", f30_edge[0], &f30_angles, &f30_position);
    TransformPoint("Det2Lab", f30_edge[1], &f30_angles, &f30_position);
    // Parameterize f30 plane
    double f30_slope = (f30_edge[1].X() - f30_edge[0].X()) / (f30_edge[1].Z() - f30_edge[0].Z());
    double f30_offset = f30_edge[0].X() - f30_slope * f30_edge[0].Z();
    // Get track info from X fibers
    double track_slope = (fiber_track.f3.X() - fiber_track.f1.X()) / (fiber_track.f3.Z() - fiber_track.f1.Z());
    double track_offset = (fiber_track.f3.X() - track_slope * fiber_track.f3.Z());
    // Extrapolate final X and Z position in fiber 30
    fiber_track.f2.SetZ((track_offset - f30_offset) / (f30_slope - track_slope)); // extrapolated
    fiber_track.f2.SetX(track_slope * fiber_track.f2.Z() + track_offset);         // extrapolated
}

void R3BTrackingG249::MakeDetectorPlanes()
{
    detector_planes.clear();
    // Define and store detector planes
    for (auto f = 0; f < NOF_FIB_DET; ++f)
    {
        TVector3 pos;
        TVector3 rot;
        TString det_name;
        switch (f)
        {
            case DET_FI32:
                rot = f32_angles;
                pos = f32_position;
                det_name = "Fib32";
                break;

            case DET_FI30:
                rot = f30_angles;
                pos = f30_position;
                det_name = "Fib30";
                break;

            case DET_FI33:
                rot = f33_angles;
                pos = f33_position;
                det_name = "Fib33";
                break;

            case DET_FI31:
                rot = f31_angles;
                pos = f31_position;
                det_name = "Fib31";
                break;

            default:
                continue;
                break;
        }
        DetectorPlane det_plane;
        det_plane.id = f;
        det_plane.v1.SetXYZ(0, 0, 0);
        det_plane.v2.SetXYZ(1, 0, 0);
        det_plane.v3.SetXYZ(0, 1, 0);
        TransformPoint("Det2Lab", det_plane.v1, &rot, &pos);
        TransformPoint("Det2Lab", det_plane.v2, &rot, &pos);
        TransformPoint("Det2Lab", det_plane.v3, &rot, &pos);
        det_plane.norm = (det_plane.v2 - det_plane.v1).Cross(det_plane.v3 - det_plane.v1);
        det_plane.position = pos;
        det_plane.rotation = rot;
        det_plane.name = det_name;
        detector_planes.push_back(det_plane);
    }
}

R3BTrackingParticle* R3BTrackingG249::AddTrackData(const R3BTrackingParticle& particle)
{
    TClonesArray& clref = *fTrackItems;
    int size = clref.GetEntriesFast();
    return new (clref.ConstructedAt(size)) R3BTrackingParticle(particle);
}

void R3BTrackingG249::SetupMinimizer() // configure minimizer for FitParticle()
{
    minimizer.reset(ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad"));
    R3BLOG_IF(fatal, !minimizer, "Failed to create ROOT minimizer");
    for (auto i = 0; i < 2; ++i) // X, Z (F32), X, Y, Z, (F30), X, Z (lastFib)
    {
        min_offset[i] = -10; // in cm
        max_offset[i] = 10;
        step[i] = 0.001;
    }

    Double_t precision = 1e-10; // 0 - default precision will be automatically determined
    Double_t tolerance = 0.2;
    minimizer->SetMaxFunctionCalls(100000); // for Minuit/Minuit2
    minimizer->SetMaxIterations(100);       // for GSL
    minimizer->SetTolerance(tolerance);
    minimizer->SetPrecision(precision);
    minimizer->SetPrintLevel(0);
    minimizer->SetStrategy(0); // to run faster

    for (int i = 0; i < NVarsFunctor; ++i)
    {
        offset[i] = gRandom->Uniform(min_offset[i] * 0.5, max_offset[i] * 0.5);
        minimizer->SetVariable(i, Form("par%d", i), offset[i], step[i]);
        minimizer->SetVariableLimits(i, min_offset[i], max_offset[i]);
    }
}

void R3BTrackingG249::Alignment()
{
    R3BLOG(info, "-------- Run alignment for the global MDF tracking ------------- ");
    R3BLOG(info, Form("Number of sample tracks for the alignment: %ld", align_data.size()));

    int attempts = 0;
    while (attempts < 10) // Retry 10 times if status is bad
    {
        SetupMinimizer();
        // Use a lambda to bind this->AlignmentError
        auto fitLambda = [this](const double* par) -> double { return this->AlignmentError(par); };
        ROOT::Math::Functor f(fitLambda, NVarsFunctor);
        minimizer->SetFunction(f);
        minimizer->Minimize();
        if (minimizer->Status() != 0) // valid minimum
        {
            R3BLOG(info, Form("Minimization attempt: %d ", attempts + 1));
            R3BLOG(info, "Retrying minimization after getting minimizer status: " << minimizer->Status());
            attempts++;
            continue;
        }
        const double* xs = minimizer->X();
        int i;
        for (i = 0; i < NVarsFunctor; ++i) // check proximity to the limits
        {
            if (fabs((xs[i] - min_offset[i]) / min_offset[i]) < 0.1 ||
                fabs((max_offset[i] - xs[i]) / max_offset[i]) < 0.1)
                break;
        }
        if (i != NVarsFunctor)
        {
            R3BLOG(info, Form("Parameter %d too close to limit, retrying...", i));
            continue;
        }
        else
            R3BLOG(info, "Good parameters are found!");

        std::cout << "\n-- Minimum: f(";
        for (i = 0; i < NVarsFunctor; i++)
        {
            std::cout << xs[i];
            if (i != NVarsFunctor - 1)
                std::cout << ",";
        }
        std::cout << ") = " << minimizer->MinValue();
        std::cout << "\n-- Minimizer status: " << minimizer->Status() << std::endl;

        R3BLOG(info, "SUCCESS minimizer for the alignment");
        return;
    }
    return;
}

double R3BTrackingG249::AlignmentError(const double* par)
{
    TVector3 f1_pos(f32_position.X(), f32_position.Y(), f32_position.Z());

    TVector3 f2_pos(f30_position.X(), f30_position.Y(), f30_position.Z());

    TVector3 f3_pos(f33_position.X() + par[0], f33_position.Y(), f33_position.Z() + par[1]);

    TVector3 f1_rot(f32_angles.X(), f32_angles.Y(), f32_angles.Z());

    TVector3 f2_rot(f30_angles.X(), f30_angles.Y(), f30_angles.Z());

    TVector3 f3_rot(f33_angles.X(), f33_angles.Y(), f33_angles.Z());

    TVector3 out_track_momentum;

    double v2 = 0; // error
    int counter = 0;
    for (const auto& t : align_data)
    {
        fiber_track.f1 = t.fib_track.f1;
        fiber_track.f2 = t.fib_track.f2;
        fiber_track.f3 = t.fib_track.f3;
        fiber_track.last_fiber = t.fib_track.last_fiber;
        TransformPoint("Det2Lab", fiber_track.f1, &f1_rot, &f1_pos);
        TransformPoint("Det2Lab", fiber_track.f2, &f2_rot, &f2_pos);
        TransformPoint("Det2Lab", fiber_track.f3, &f3_rot, &f3_pos);

        // Fixup fib30 hit
        //  Define two (X,Z) points of the fib30 plane:
        TVector3 f30_edge[2];
        f30_edge[0].SetXYZ(-1, 0, 0);
        f30_edge[1].SetXYZ(1, 0, 0);
        TransformPoint("Det2Lab", f30_edge[0], &f2_rot, &f2_pos);
        TransformPoint("Det2Lab", f30_edge[1], &f2_rot, &f2_pos);
        // Parameterize f30 plane
        double f30_slope = (f30_edge[1].X() - f30_edge[0].X()) / (f30_edge[1].Z() - f30_edge[0].Z());
        double f30_offset = f30_edge[0].X() - f30_slope * f30_edge[0].Z();
        // Get track info from X fibers
        double track_slope = (fiber_track.f3.X() - fiber_track.f1.X()) / (fiber_track.f3.Z() - fiber_track.f1.Z());
        double track_offset = (fiber_track.f3.X() - track_slope * fiber_track.f3.Z());
        // Extrapolate final X and Z position in fiber 30
        fiber_track.f2.SetZ((track_offset - f30_offset) / (f30_slope - track_slope)); // extrapolated
        fiber_track.f2.SetX(track_slope * fiber_track.f2.Z() + track_offset);         // extrapolated

        double start_X = t.foot_track.GetStartPosition().X() * 0.1; // must be cm
        double start_Y = t.foot_track.GetStartPosition().Y() * 0.1; // must be cm
        double start_Z = t.foot_track.GetStartPosition().Z() * 0.1; // must be cm
        double start_TX = t.foot_track.GetStartMomentum().X() / t.foot_track.GetStartMomentum().Z();
        double start_TY = t.foot_track.GetStartMomentum().Y() / t.foot_track.GetStartMomentum().Z();

        int k = 0;
        mdf_data[k] = start_X;
        mdf_data[++k] = start_Y;
        mdf_data[++k] = start_Z;
        mdf_data[++k] = start_TX;
        mdf_data[++k] = start_TY;

        mdf_data[++k] = fiber_track.f1.X(); // must be in cm
        mdf_data[++k] = fiber_track.f1.Z(); // must be in cm
        mdf_data[++k] = (fiber_track.f3.X() - fiber_track.f1.X()) / (fiber_track.f3.Z() - fiber_track.f1.Z());
        mdf_data[++k] = (fiber_track.f2.Y() - start_Y) / (fiber_track.f2.Z() - start_Z);

        Double_t PoQ = MDF_PoQ->MDF(mdf_data) * GladCurrent / GladReferenceCurrent;
        v2 += pow(PoQ - t.reference_PoQ + 0.00414749, 2);
        counter++;
    }
    v2 /= counter;
    double v = sqrt(v2);
    return v;
}

ClassImp(R3BTrackingG249);
