/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -------------------------------------------------------------------
// -----       R3BFootHit2Track source file                      -----
// -----       Created 16/09/2025 by V. Panin                    -----
// -------------------------------------------------------------------

// ROOT headers
#include <Math/Factory.h>
#include <Math/Functor.h>
#include <Math/GSLMinimizer.h>
#include <Math/Minimizer.h>
#include <Minuit2/Minuit2Minimizer.h>
#include <TClonesArray.h>
#include <TF1.h>
#include <TMath.h>
#include <TRandom3.h>
#include <TRotation.h>
#include <TVector3.h>
#include <iomanip>
#include <memory>
#include <vector>

// FAIR headers
#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>

// Data headers
#include "R3BEventHeader.h"
#include "R3BFootHit2Track.h"
#include "R3BFootHitData.h"
#include "R3BFootMappingPar.h"
#include "R3BFrsData.h"
#include "R3BLogger.h"
#include "R3BTrackingParticle.h"

R3BFootHit2Track::R3BFootHit2Track(const TString& name, int iVerbose)
    : FairTask(name, iVerbose)
{
}

R3BFootHit2Track::~R3BFootHit2Track()
{
    if (fIncomingTrackFoot)
        delete fIncomingTrackFoot;
    if (fOutgoingTrackFoot)
        delete fOutgoingTrackFoot;
}

InitStatus R3BFootHit2Track::Init()
{
    FairRootManager* rootManager = FairRootManager::Instance();
    R3BLOG_IF(fatal, rootManager == nullptr, "FairRootManager not found");

    fHeader = dynamic_cast<R3BEventHeader*>(rootManager->GetObject("EventHeader."));
    R3BLOG_IF(fatal, NULL == fHeader, "EventHeader. not found");

    // Input FOOT hit data
    fFootHitData = dynamic_cast<TClonesArray*>(rootManager->GetObject("FootHitData"));
    R3BLOG_IF(fatal, !fFootHitData, "FootHitData not found");

    // Input FRS data
    fFrsData = dynamic_cast<TClonesArray*>(rootManager->GetObject("FrsData"));
    R3BLOG_IF(fatal, !fFrsData, "FrsData not found");

    // Output data
    fIncomingTrackFoot = new TClonesArray("R3BTrackingParticle");
    rootManager->Register("IncomingTrackFoot", "Incoming_tracks_in_FOOT", fIncomingTrackFoot, !fOnline);

    fOutgoingTrackFoot = new TClonesArray("R3BTrackingParticle");
    rootManager->Register("OutgoingTrackFoot", "Outgoing_tracks_in_FOOT", fOutgoingTrackFoot, !fOnline);

    NEventsTrig1 = 0;
    NgoodEventsInFrs = 0;
    NgoodEventsInAllFoots = 0;

    Reset();
    SetMappingPar();
    R3BLOG(info, "R3BFootHit2Track is initialized!");
    return kSUCCESS;
}

InitStatus R3BFootHit2Track::ReInit()
{
    SetMappingPar();
    return kSUCCESS;
}

void R3BFootHit2Track::SetMappingPar()
{
    detector_planes.clear();
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");

    fMap_Par = dynamic_cast<R3BFootMappingPar*>(rtdb->getContainer("footMappingPar"));
    R3BLOG_IF(fatal, !fMap_Par, "Couldn't get handle on footMappingPar container");

    R3BLOG(info, "footMappingPar found");

    fMaxNumDet = fMap_Par->GetNumDets(); // Number of foot detectors
    R3BLOG_IF(fatal, fMaxNumDet != 8, "Expecting 8 FOOTs in the MappingPar file");

    for (auto i = 0; i < fMaxNumDet; i++)
    {
        TVector3 pos(fMap_Par->GetOffsetX(i + 1),
                     fMap_Par->GetOffsetY(i + 1),
                     fMap_Par->GetDist2target(i + 1) * 10.); // Z from cm to mm
        TVector3 rot(fMap_Par->GetAngleTheta(i + 1), fMap_Par->GetAnglePsi(i + 1), fMap_Par->GetAnglePhi(i + 1));
        foot_position.push_back(pos);
        foot_rotation.push_back(rot);

        // Store detector planes
        DetectorPlane det_plane;
        det_plane.id = i + 1;
        det_plane.v1.SetXYZ(0, 0, 0);
        det_plane.v2.SetXYZ(1, 0, 0);
        det_plane.v3.SetXYZ(0, 1, 0);
        Transform_Point("Det2Lab", det_plane.v1, &pos, &rot);
        Transform_Point("Det2Lab", det_plane.v2, &pos, &rot);
        Transform_Point("Det2Lab", det_plane.v3, &pos, &rot);
        det_plane.norm = (det_plane.v2 - det_plane.v1).Cross(det_plane.v3 - det_plane.v1);
        det_plane.position = pos;
        det_plane.rotation = rot;
        det_plane.name = Form("foot%d", i + 1);
        detector_planes.push_back(det_plane);
    }
    // fMap_Par->printParams();
}

void R3BFootHit2Track::Exec(Option_t* /*option*/)
{
    R3BLOG_IF(fatal, detector_planes.size() != fMaxNumDet, "FOOT planes are not initialiazed!");

    if (fHeader->GetTrigger() != 1)
        return;
    NEventsTrig1++;

    Reset();                          // reset output TCAs
    R3BTrackingParticle in_particle;  // charge, x, y, z, px, py, pz, beta, mass
    R3BTrackingParticle out_particle; // charge, x, y, z, px, py, pz, beta, mass

    auto nFootHits = fFootHitData->GetEntriesFast();
    if (nFootHits == 0)
        return;

    auto nFrsHits = fFrsData->GetEntriesFast();
    if (nFrsHits == 0)
        return;

    double frsZ = 0;
    double frsAoQ = 0;
    for (auto j = 0; j < nFrsHits; ++j)
    {
        auto frs_item = dynamic_cast<R3BFrsData*>(fFrsData->At(j));

        if (frs_item->GetStaId() != vftxNumber)
            continue; // 2 - new VFTX with 10ps resolution

        frsZ = frs_item->GetZ();
        frsAoQ = frs_item->GetAq();
    }
    in_particle.SetCharge(frsZ);
    in_particle.SetMass(frsAoQ);
    if (frsAoQ < 1 || frsZ < 1)
        return;
    NgoodEventsInFrs++;

    maxHitPerDet.clear();   // clear map for highest energy hits
    maxHitTCAIndex.clear(); // clear map for TCA indexes of highest energy hits
    // IMPORTANT! Hits in FOOTs must be defined in the local coordinates of every detector
    for (auto j = 0; j < nFootHits; ++j)
    {
        auto foot_item = dynamic_cast<R3BFootHitData*>(fFootHitData->At(j));
        R3BLOG_IF(fatal, !foot_item, "ERROR in the foot hit");
        auto detId = foot_item->GetDetId();
        auto energy = foot_item->GetZCharge();

        auto it = maxHitPerDet.find(detId); // seacrh this FOOT in the map
        if (it == maxHitPerDet.end())       // first hit for this FOOT
        {
            maxHitPerDet[detId] = foot_item;
            maxHitTCAIndex[detId] = j;
        }
        else // compare with current max energy
        {
            if (energy > it->second->GetEnergy())
            {
                maxHitPerDet[detId] = foot_item;
                maxHitTCAIndex[detId] = j;
            }
        }
    }

    // Store TCA indexes inside each particle
    int count_foots = 0;
    for (int detId = 1; detId < 9; ++detId)
    {
        auto it = maxHitTCAIndex.find(detId);
        if (it == maxHitTCAIndex.end())
        {
            R3BLOG(debug, Form("No data in FOOT %d", detId));
            break; // Make sure all 8 detectors are present
        }
        count_foots++;
        (detId > 4 ? out_particle : in_particle).AddHit("foot" + std::to_string(detId), maxHitTCAIndex[detId]);
    }
    if (count_foots != 8)
    {
        R3BLOG(debug, "Skip tracking in this event due to insufficient FOOT data");
        return;
    }
    NgoodEventsInAllFoots++;

    // Fit incoming particle track
    if (FitParticle(in_particle, TrackDirection::In))
    {
        AddTrack(in_particle);
    }
    else
        R3BLOG(error, "Failed to track incoming particle in FOOTs");

    // Fit outfoing particle track
    if (FitParticle(out_particle, TrackDirection::Out))
    {
        AddTrack(out_particle);
    }
    else
        R3BLOG(error, "Failed to track outgoing particle in FOOTs");
}

Bool_t R3BFootHit2Track::PropagateParticleToPlane(int detId, TVector3& intersect)
{
    TVector3 pos = thisTrackingParticle->GetStartPosition();
    TVector3 dir = thisTrackingParticle->GetStartMomentum().Unit();

    Double_t denom = dir.Dot(detector_planes[detId - 1].norm);
    if (denom == 0)
        return kFALSE;

    const Double_t t = (detector_planes[detId - 1].v1 - pos).Dot(detector_planes[detId - 1].norm) / denom;
    if (t < 0)
        return kFALSE;

    intersect = pos + t * dir; // intersection point of the paricle with the detector plane in the lab frome
    Transform_Point("Lab2Det",
                    intersect,
                    &detector_planes[detId - 1].position,
                    &detector_planes[detId - 1].rotation); // intersection in detector frame
    return kTRUE;
}

void R3BFootHit2Track::MakeParticle(double dtx, double dty, double particle_offset)
{
    R3BFootHitData* hit = nullptr;
    double tx = 0;
    double ty = 0;
    TVector3 start_position(0, 0, 0);

    const int i_offset = (fInOutFlag == TrackDirection::Out) ? 4 : 0;
    for (int i = 0; i < 4; i++)
    {
        auto it = maxHitPerDet.find(i + 1 + i_offset);
        R3BLOG_IF(fatal, it == maxHitPerDet.end(), "ERROR in the FOOT map of highest energy hits");

        hit = it->second;
        R3BLOG_IF(fatal, !hit, "ERROR accessing R3BFootHit pointer inside the map");

        det_points[i].SetXYZ(hit->GetPos(), 0, 0);
        Transform_Point("Det2Lab", det_points[i], &foot_position[i + i_offset], &foot_rotation[i + i_offset]);
    }
    if (fInOutFlag == TrackDirection::In)
    {
        tx = (det_points[2].X() - det_points[0].X()) / (det_points[2].Z() - det_points[0].Z()) + dtx;
        ty = (det_points[3].Y() - det_points[1].Y()) / (det_points[3].Z() - det_points[1].Z()) + dty;
        start_position.SetXYZ(det_points[0].X(), det_points[1].Y() + particle_offset, det_points[0].Z());
    }
    else if (fInOutFlag == TrackDirection::Out)
    {
        tx = (det_points[3].X() - det_points[1].X()) / (det_points[3].Z() - det_points[1].Z()) + dtx;
        ty = (det_points[2].Y() - det_points[0].Y()) / (det_points[2].Z() - det_points[0].Z()) + dty;
        start_position.SetXYZ(det_points[1].X() + particle_offset, det_points[0].Y(), det_points[0].Z());
    }
    TVector3 start_direction(tx, ty, 1);
    start_direction.SetMag(10.);
    thisTrackingParticle->SetStartPosition(start_position);
    thisTrackingParticle->SetStartMomentum(start_direction);
}

double R3BFootHit2Track::FitParticleError(const double* par)
{
    R3BLOG_IF(fatal, !thisTrackingParticle, "thisTrackingParticle is null!");

    const double dtx = par[0];
    const double dty = par[1];
    const double particle_offset = par[2];
    double chi2 = 0.;
    double residual = 0;
    MakeParticle(dtx, dty, particle_offset);
    TVector3 intersect_point;

    int i_offset = (fInOutFlag == TrackDirection::Out) ? 4 : 0;
    for (int i = 1; i < 4; i++) // skip first detector which is a start point of the particle
    {
        if (!PropagateParticleToPlane(i + 1 + i_offset, intersect_point))
        {
            R3BLOG(fatal, "Failed to prpagate particle to the plane");
        }

        auto it = maxHitPerDet.find(i + 1 + i_offset);
        R3BLOG_IF(fatal, it == maxHitPerDet.end(), "ERROR to access entry in the map of FOOT hits");

        double hit_pos = it->second->GetPos();
        residual = hit_pos - intersect_point.X();
        chi2 += residual * residual;
    }
    return sqrt(chi2 / 3.);
}

bool R3BFootHit2Track::RunMinimizer()
{
    int attempts = 0;
    while (attempts < 10) // Retry 10 times if status is bad
    {
        PrepareMinimizer();
        // Use a lambda to bind this->FitParticleError
        auto fitLambda = [this](const double* par) -> double { return this->FitParticleError(par); };
        ROOT::Math::Functor f(fitLambda, NVarsFunctor);
        minimizer->SetFunction(f);

        minimizer->Minimize();
        if (minimizer->Status() != 0) // valid minimum
        {
            R3BLOG(debug, "Retrying minimization after getting minimizer status: " << minimizer->Status());
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
            R3BLOG(debug, Form("Parameter %d too close to limit, retrying...", i));
            attempts++;
            continue;
        }
        R3BLOG(debug, "SUCCESS minimizer on tracking this particle");
        return true;
    }
    return false;
}

bool R3BFootHit2Track::FitParticle(R3BTrackingParticle& part,
                                   TrackDirection direction) // flag = "incoming" or "outgoing"
{
    fInOutFlag = direction; // used inside the error function to fit incoming or outgoing track

    thisTrackingParticle = std::make_unique<R3BTrackingParticle>();
    if (!RunMinimizer())
    {
        R3BLOG(error, "Fitting particle has failed!");
        return false;
    }
    const double* xs = minimizer->X();
    double dtx = xs[0];
    double dty = xs[1];
    double dpos = xs[2];
    MakeParticle(dtx, dty, dpos);
    TVector3 start_position = thisTrackingParticle->GetStartPosition();
    TVector3 start_momentum = thisTrackingParticle->GetStartMomentum();
    part.SetStartPosition(start_position);
    part.SetStartMomentum(start_momentum);
    // std::cout << "\n\n\nParticle Z=" << part.GetCharge() << ", A=" <<  part.GetMass();
    thisTrackingParticle.reset();
    return true;
}

R3BTrackingParticle* R3BFootHit2Track::AddTrack(const R3BTrackingParticle& particle)
{
    TClonesArray& clref = (fInOutFlag == TrackDirection::In) ? (*fIncomingTrackFoot) : (*fOutgoingTrackFoot);
    int size = clref.GetEntriesFast();
    return new (clref.ConstructedAt(size)) R3BTrackingParticle(particle);
}

void R3BFootHit2Track::SetupMinimizer() // configure minimizer for FitParticle()
{
    minimizer.reset(ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad"));
    R3BLOG_IF(fatal, !minimizer, "Failed to create ROOT minimizer");
    for (int i = 0; i < 2; ++i)
    {
        min_offset[i] = -0.08; // tx and ty, roughly in radians
        max_offset[i] = 0.08;
        step[i] = 0.0001;
    }
    min_offset[2] = -10.; // mm
    max_offset[2] = 10.;  // mm
    step[2] = 0.01;
    Double_t precision = 1e-10; // 0 - default precision will be automaticalle determined
    Double_t tolerance = 2.0;
    minimizer->SetMaxFunctionCalls(100000); // for Minuit/Minuit2
    minimizer->SetMaxIterations(100);       // for GSL
    minimizer->SetTolerance(tolerance);
    minimizer->SetPrecision(precision);
    minimizer->SetPrintLevel(0);
    minimizer->SetStrategy(0); // to run faster
}

void R3BFootHit2Track::Reset()
{
    R3BLOG(debug, "Clearing incoming and outgoing tracks");
    if (fIncomingTrackFoot)
        fIncomingTrackFoot->Clear();
    if (fOutgoingTrackFoot)
        fOutgoingTrackFoot->Clear();
}

void R3BFootHit2Track::PrepareMinimizer()
{
    SetupMinimizer();
    for (int i = 0; i < NVarsFunctor; ++i)
    {
        offset[i] = gRandom->Uniform(min_offset[i] * 0.5, max_offset[i] * 0.5);
        minimizer->SetVariable(i, Form("par%d", i), offset[i], step[i]);
        minimizer->SetVariableLimits(i, min_offset[i], max_offset[i]);
    }
}

void R3BFootHit2Track::Transform_Point(TString flag, TVector3& hit_point, TVector3* det_offset, TVector3* rotation)
{
    TRotation r;
    // First Euler rotation around Y axis
    r.RotateY(rotation->Y() * TMath::DegToRad());

    // get local X axis after first rotation
    TVector3 v3_localX(1, 0, 0);
    v3_localX.SetTheta(r.ThetaX());
    v3_localX.SetPhi(r.PhiX());

    // Second Euler rotation around local X axis
    r.Rotate(rotation->X() * TMath::DegToRad(), v3_localX);

    // get local Z axis after second rotation
    TVector3 v3_localZ(0, 0, 1);
    v3_localZ.SetTheta(r.ThetaZ());
    v3_localZ.SetPhi(r.PhiZ());

    // final rotation around local Z axis
    r.Rotate(rotation->Z() * TMath::DegToRad(), v3_localZ);

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
        R3BLOG(fatal, "Wrong flag in the Transform_Point() function");
    }
}

void R3BFootHit2Track::FinishTask()
{
    std::cout << "\nN events with Trig=1 " << NEventsTrig1;
    std::cout << "\nN good events in FRS = " << NgoodEventsInFrs;
    std::cout << "\nN good events in all FOOTs = " << NgoodEventsInAllFoots << "\n";
}

ClassImp(R3BFootHit2Track)