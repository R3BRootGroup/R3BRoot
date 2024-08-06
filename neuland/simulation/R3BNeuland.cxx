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

#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "R3BMCStack.h"
#include "R3BNeuland.h"
#include "R3BNeulandGeoPar.h"
#include "R3BNeulandPoint.h"
#include <FairRootManager.h>
#include <fmt/format.h>

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
    : R3BDetector("R3BNeuland", kNEULAND, geoFile, combi)
{
}

R3BNeuland::R3BNeuland(int nDP, const TGeoTranslation& trans, const TGeoRotation& rot)
    : R3BNeuland(nDP, { trans, rot })
{
}

R3BNeuland::R3BNeuland(const int nDP, const TGeoCombiTrans& combi)
    : R3BNeuland(fmt::format("neuland_v3_{}dp.geo.root", nDP), combi)
{
}

void R3BNeuland::Initialize()
{
    LOG(info) << "R3BNeuland initialization ...";

    FairDetector::Initialize();

    WriteParameterFile();
    ResetValues();
}

Bool_t R3BNeuland::ProcessHits(FairVolume*)
{
    // New hit in detector
    if (gMC->IsTrackEntering())
    {
        if (!fIsLastHitDone)
        {
            LOG(warn) << "R3BNeuland: Incomplete hit discarded";
            reset_values();
        }

        fIsLastHitDone = kFALSE;
        fEnergyLoss = 0.;
        fLightYield = 0.;
        fTime = gMC->TrackTime() * seconds_to_nanoseconds;
        fLength = gMC->TrackLength();
        gMC->TrackPosition(fPosIn);
        gMC->TrackMomentum(fMomIn);
        gMC->CurrentVolOffID(1, fPaddleId);

        fParticleId = gMC->TrackPid();
        fTrackPidMap.emplace(gMC->GetStack()->GetCurrentTrackNumber(), gMC->TrackPid());
        if (auto search = fTrackPidMap.find(gMC->GetStack()->GetCurrentParentTrackNumber());
            search != fTrackPidMap.end())
        {
            fParentParticleId = search->first;
        }
    }

    // Sum energy loss for all steps in the active volume
    fEnergyLoss += gMC->Edep();
    fLightYield += GetLightYield(gMC->TrackCharge(), gMC->TrackStep(), gMC->Edep());

    // Set additional parameters at exit of active volume. Create R3BNeulandPoint.
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())
    {
        // Do not save a hit if no energy deposited
        constexpr auto minimum_energy_cutoff = 1e-20;
        if (fEnergyLoss < minimum_energy_cutoff || fLightYield < minimum_energy_cutoff)
        {
            reset_values();
            return kTRUE;
        }

        fTrackId = gMC->GetStack()->GetCurrentTrackNumber();
        gMC->TrackPosition(fPosOut);
        gMC->TrackMomentum(fMomOut);

        // Add Point
        LOG(debug) << "R3BNeuland: Adding Point at (" << fPosIn.X() << ", " << fPosIn.Y() << ", " << fPosIn.Z()
                   << ") cm,  paddle " << fPaddleId << ", track " << fTrackId << ", energy loss " << fEnergyLoss
                   << " GeV " << gMC->GetStack()->GetCurrentParentTrackNumber();
        auto* neuland_point =
            dynamic_cast<R3BNeulandPoint*>(TCAPointsBuffer->ConstructedAt(TCAPointsBuffer->GetEntriesFast()));
        neuland_point->SetTrackID(fTrackId);
        neuland_point->SetDetectorID(fPaddleId);
        neuland_point->SetPosition(fPosIn.Vect());
        neuland_point->SetMomentum(fMomIn.Vect());
        neuland_point->SetTime(fTime);
        neuland_point->SetLength(fLength);
        neuland_point->SetEnergyLoss(fEnergyLoss);
        neuland_point->SetEventID(gMC->CurrentEvent());
        neuland_point->SetLightYield(fLightYield);
        neuland_point->SetParticleId(fParticleId);
        neuland_point->SetParentParticleId(fParentParticleId);
        // fNeulandPoints.get().emplace_back(fTrackId,
        //                                   fPaddleId,
        //                                   fPosIn.Vect(),
        //                                   fMomIn.Vect(),
        //                                   fTime,
        //                                   fLength,
        //                                   fEnergyLoss,
        //                                   gMC->CurrentEvent(),
        //                                   fLightYield,
        //                                   fParticleId,
        //                                   fParentParticleId);

        // Increment number of LandPoints for this track
        auto* stack = dynamic_cast<R3BStack*>(gMC->GetStack());
        stack->AddPoint(kNEULAND);
        reset_values();
    }

    return kTRUE;
}

auto R3BNeuland::CheckIfSensitive(std::string name) -> bool { return name == "volBC408"; }

auto R3BNeuland::GetCollection(int iColl) const -> TClonesArray*
{
    if (iColl != 0)
    {
        return nullptr;
    }
    return TCAPointsBuffer.get();
}

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
    auto& points = fNeulandPoints.get();
    points.reserve(TCAPointsBuffer->GetEntriesFast());
    for (auto* point : TRangeDynCast<R3BNeulandPoint>(TCAPointsBuffer.get()))
    {
        points.push_back(*point);
    }
}

void R3BNeuland::Print(Option_t* /*unused*/) const
{
    LOG(info) << "R3BNeuland: " << fNeulandPoints.get_constref().size() << " Neuland Points registered in this event";
}

void R3BNeuland::Reset()
{
    fNeulandPoints.clear();
    TCAPointsBuffer->Clear();
    reset_values();
    fTrackPidMap.clear();
}

void R3BNeuland::reset_values()
{
    fIsLastHitDone = kTRUE;
    fTrackId = 0;
    fPaddleId = -1;
    fPosIn.Clear();
    fPosOut.Clear();
    fMomIn.Clear();
    fMomOut.Clear();
    fTime = fLength = fEnergyLoss = fLightYield = 0;
}

void R3BNeuland::write_parameter_file()
{
    FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
    fNeulandGeoPar = dynamic_cast<R3BNeulandGeoPar*>(rtdb->getContainer("R3BNeulandGeoPar"));

    // Really bad way to find the Neuland *node* (not the volume!)
    TGeoNode* geoNodeNeuland = nullptr;
    for (int i = 0; i < gGeoManager->GetTopNode()->GetNdaughters(); i++)
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

    fNeulandGeoPar->SetNeulandGeoNode(geoNodeNeuland);
    fNeulandGeoPar->setChanged();
}

void R3BNeuland::Register()
{
    LOG(info) << "R3BNeuland initialization ...";

    fNeulandPoints.init();

    write_parameter_file();
    reset_values();
}

ClassImp(R3BNeuland);
