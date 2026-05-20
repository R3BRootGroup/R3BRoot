/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BNeuland.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "R3BDetectorList.h"
#include "R3BException.h"
#include "R3BMCStack.h"
#include "R3BNeulandGeoPar.h"
#include "R3BNeulandPoint.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TVirtualMC.h"
#include <FairGeoLoader.h>
#include <FairRootManager.h>
#include <FairVolume.h>
#include <R3BLogger.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <TCollection.h>
#include <TGeoMatrix.h>
#include <TGeoNode.h>
#include <TMathBase.h>
#include <TString.h>
#include <fairlogger/Logger.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <memory>
#include <string>

// Initialize variables from Birk' s Law
constexpr auto seconds_to_nanoseconds = 1e9;
constexpr auto BirkdP = 1.032;
constexpr auto BirkC1 = 0.013 / BirkdP;
constexpr auto BirkC2 = 9.6e-6 / (BirkdP * BirkdP);

namespace
{
    inline auto GetLightYield(const int charge, const double length, const double edep) -> double
    {
        // Apply Birk's law ( Adapted from G3BIRK/Geant3)
        if (charge != 0 && length > 0)
        {
            auto birkC1Mod = BirkC1;

            // Apply correction for higher charge states
            if (TMath::Abs(charge) >= 2)
            {
                birkC1Mod *= 7.2 / 12.6; // NOLINT
            }

            const double dedxcm = 1000. * edep / length;
            const double lightYield = edep / (1. + birkC1Mod * dedxcm + BirkC2 * dedxcm * dedxcm);
            return lightYield;
        }
        return edep; // Rarely very small energy depositions have no length?
    }
} // namespace

R3BNeuland::R3BNeuland()
    : R3BNeuland("")
{
}

R3BNeuland::R3BNeuland(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot)
    : R3BNeuland(geoFile, { trans, rot })
{
}

R3BNeuland::R3BNeuland(const TString& geoFile, const TGeoCombiTrans& combi)
    : FairDetector{ "R3BNeuland", true, kNEULAND }
    , rot_trans_{ combi }
    , geo_file_{ geoFile.Data() }
{
}

R3BNeuland::R3BNeuland(int nDP, const TGeoTranslation& trans, const TGeoRotation& rot)
    : R3BNeuland(nDP, { trans, rot })
{
}

R3BNeuland::R3BNeuland(const int nDP, const TGeoCombiTrans& combi)
    : R3BNeuland(fmt::format("neuland_v3_{}dp.geo.root", nDP), combi)
{
    num_of_planes_ = 2 * nDP;
}

void R3BNeuland::Initialize()
{
    LOG(info) << "R3BNeuland initialization ...";

    FairDetector::Initialize();

    write_parameter_file();
    reset_values();
}

auto R3BNeuland::ProcessHits(FairVolume* /*v*/) -> bool
{
    // New hit in detector
    if (gMC->IsTrackEntering())
    {
        if (!is_last_hit_done_)
        {
            LOG(warn) << "R3BNeuland: Incomplete hit discarded";
            reset_values();
        }

        is_last_hit_done_ = kFALSE;
        energy_loss_ = 0.;
        light_yield_ = 0.;
        time_ = gMC->TrackTime() * seconds_to_nanoseconds;
        length_ = gMC->TrackLength();
        gMC->TrackPosition(pos_in_);
        gMC->TrackMomentum(mom_in_);
        gMC->CurrentVolOffID(1, paddle_id_);

        particle_id_ = gMC->TrackPid();
        track_pid_map_.emplace(gMC->GetStack()->GetCurrentTrackNumber(), gMC->TrackPid());
        if (auto search = track_pid_map_.find(gMC->GetStack()->GetCurrentParentTrackNumber());
            search != track_pid_map_.end())
        {
            parent_particle_id_ = search->first;
        }
    }

    // Sum energy loss for all steps in the active volume
    energy_loss_ += gMC->Edep();
    light_yield_ += GetLightYield(gMC->TrackCharge(), gMC->TrackStep(), gMC->Edep());

    // Set additional parameters at exit of active volume. Create R3BNeulandPoint.
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())
    {
        // Do not save a hit if no energy deposited
        constexpr auto minimum_energy_cutoff = 1e-20;
        if (energy_loss_ < minimum_energy_cutoff || light_yield_ < minimum_energy_cutoff)
        {
            reset_values();
            return kTRUE;
        }

        track_id_ = gMC->GetStack()->GetCurrentTrackNumber();
        gMC->TrackPosition(pos_out_);
        gMC->TrackMomentum(mom_out_);

        // Add Point
        LOGP(debug,
             "R3BNeuland: Adding Point at (\"{}\", \"{}\", \"{}\") cm, paddle {}, track {}, energy loss {} GeV",
             pos_in_.X(),
             pos_in_.Y(),
             pos_in_.Z(),
             paddle_id_,
             track_id_,
             energy_loss_,
             gMC->GetStack()->GetCurrentParentTrackNumber());
        auto* neuland_point =
            dynamic_cast<R3BNeulandPoint*>(tca_points_buffer_->ConstructedAt(tca_points_buffer_->GetEntriesFast()));
        neuland_point->SetTrackID(track_id_);
        neuland_point->SetDetectorID(paddle_id_);
        neuland_point->SetPosition(pos_in_.Vect());
        neuland_point->SetMomentum(mom_in_.Vect());
        neuland_point->SetTime(time_);
        neuland_point->SetLength(length_);
        neuland_point->SetEnergyLoss(energy_loss_);
        neuland_point->SetEventID(gMC->CurrentEvent());
        neuland_point->SetLightYield(light_yield_);
        neuland_point->SetParticleId(particle_id_);
        neuland_point->SetParentParticleId(parent_particle_id_);

        // Increment number of LandPoints for this track
        auto* stack = dynamic_cast<R3BStack*>(gMC->GetStack());
        stack->AddPoint(kNEULAND);
        reset_values();
    }

    return kTRUE;
}

auto R3BNeuland::CheckIfSensitive(std::string name) -> bool { return name == "volBC408"; }

void R3BNeuland::EndOfEvent()
{
    if (fVerboseLevel != 0)
    {
        Print();
    }
    Reset();
}

void R3BNeuland::FinishEvent()
{
    auto& points = neuland_points_.get();
    points.reserve(tca_points_buffer_->GetEntriesFast());
    for (auto* point : TRangeDynCast<R3BNeulandPoint>(tca_points_buffer_.get()))
    {
        points.push_back(*point);
    }
}

void R3BNeuland::Print(Option_t* /*unused*/) const
{
    LOG(info) << "R3BNeuland: " << neuland_points_.get_constref().size() << " Neuland Points registered in this event";
}

void R3BNeuland::Reset()
{
    neuland_points_.clear();
    tca_points_buffer_->Clear();
    reset_values();
    track_pid_map_.clear();
}

void R3BNeuland::reset_values()
{
    is_last_hit_done_ = kTRUE;
    track_id_ = 0;
    paddle_id_ = -1;
    time_ = 0;
    length_ = 0.;
    energy_loss_ = 0.;
    light_yield_ = 0.;
    pos_in_.Clear();
    pos_out_.Clear();
    mom_in_.Clear();
    mom_out_.Clear();
}

void R3BNeuland::write_parameter_file()
{
    FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
    neuland_geo_par_ = dynamic_cast<R3BNeulandGeoPar*>(rtdb->getContainer("R3BNeulandGeoPar"));

    // Really bad way to find the Neuland *node* (not the volume!)
    TGeoNode* geoNodeNeuland = nullptr;
    for (int i{}; i < gGeoManager->GetTopNode()->GetNdaughters(); i++)
    {
        if (TString(gGeoManager->GetTopNode()->GetDaughter(i)->GetVolume()->GetName()) == "volNeuland")
        {
            geoNodeNeuland = gGeoManager->GetTopNode()->GetDaughter(i);
            break;
        }
    }

    if (geoNodeNeuland == nullptr)
    {
        LOG(fatal) << "volNeuland not found";
    }

    neuland_geo_par_->SetNeulandGeoNode(geoNodeNeuland);
    neuland_geo_par_->setChanged();
}

auto R3BNeuland::GetCollection(int iColl) const -> TClonesArray*
{
    if (iColl == 0)
    {
        return tca_points_buffer_.get();
    }
    return nullptr;
}

void R3BNeuland::Register() { neuland_points_.init(); }

void R3BNeuland::ConstructGeometry()
{
    if (is_geo_auto_built)
    {
        create_geo();
    }
    else
    {
        create_geo_from_root_file();
    }
}

void R3BNeuland::create_geo()
{

    auto* geo_loader = FairGeoLoader::Instance();
    if (geo_loader == nullptr)
    {
        geo_loader = std::make_unique<FairGeoLoader>("TGeo", "FairGeoLoader").release();
    }
    auto* neuland_geo = geo_creator_.construct_volume(num_of_planes_, geo_loader);
    if (auto* top_volume = gGeoManager->GetTopVolume(); top_volume != nullptr)
    {
        // Use copy_id 0 since only one neuland is needed
        auto* neuland_node = top_volume->AddNode(neuland_geo, 0, rot_trans_.MakeClone());

        // This will set each bar as a sensitive volume
        ExpandNode(neuland_node);
    }
    else
    {
        throw R3B::runtime_error("Top volume from gGeoManager is nullptr!");
    }
}

void R3BNeuland::create_geo_from_root_file()
{
    SetGeometryFileName(geo_file_.c_str());
    if (!GetGeometryFileName().EndsWith(".root"))
    {
        R3BLOG(fatal, GetName() << " (which is a " << ClassName() << ") geometry file is not specified");
    }
    R3BLOG(info,
           fmt::format("Constructing {} (which is a {}) geometry from ROOT file {} ...",
                       GetName(),
                       ClassName(),
                       GetGeometryFileName().Data()));
    ConstructRootGeometry();
    if (not rot_trans_.IsIdentity())
    {
        if (auto* top_node = gGeoManager->GetTopNode(); top_node != nullptr)
        {
            auto* neuland_node = top_node->GetDaughter(gGeoManager->GetTopNode()->GetNdaughters() - 1);
            dynamic_cast<TGeoNodeMatrix*>(neuland_node)->SetMatrix(rot_trans_.MakeClone());
        }
        else
        {
            throw R3B::runtime_error("Top node from gGeoManager is nullptr!");
        }
    }
}

ClassImp(R3BNeuland);
