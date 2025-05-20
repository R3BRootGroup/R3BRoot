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

#include "R3BNeulandPrimaryInteractionFinder.h"
#include "FairLogger.h"
#include "R3BMCTrack.h"
#include "R3BNeulandHit.h"
#include "R3BNeulandPoint.h"
#include "TFile.h"
#include <FairRootManager.h>
#include <FairTask.h>
#include <RtypesCore.h>
#include <TDirectory.h>
#include <TH1.h>
#include <TH2.h>
#include <fairlogger/Logger.h>
#include <map>
#include <string_view>
#include <vector>

namespace
{
    constexpr auto NEUTRON_PID = 2112;
    constexpr auto MIN_TOF_DEFAULT = 1e99;

    auto IsPrimaryTrack(const R3BMCTrack& track) -> bool
    {
        // TODO: The test can be modified to rely on some other information,
        // e.g. if the neutrons were created in a reaction in the target simulated with Geant
        return track.GetMotherId() == -1 && track.GetPdgCode() == NEUTRON_PID;
    }

    // Map a hit to every point if possible (else -1)
    auto MapPointsToHits(const std::vector<R3BNeulandPoint>& points, const std::vector<R3BNeulandHit>& hits)
        -> std::map<const R3BNeulandPoint*, const R3BNeulandHit*>
    {
        auto point_to_hit_map = std::map<const R3BNeulandPoint*, const R3BNeulandHit*>{};
        for (const auto& point : points)
        {
            point_to_hit_map[&point] = nullptr;
            for (const auto& hit : hits)
            {
                if (point.GetPaddle() == hit.GetPaddle())
                {
                    point_to_hit_map[&point] = &hit;
                }
            }
        }
        return point_to_hit_map;
    };

    // Map a primary track to every point if possible (else -1)
    auto MapPointsToPrimaryTracks(const std::vector<R3BNeulandPoint>& points, const std::vector<R3BMCTrack>& tracks)
        -> std::map<const R3BNeulandPoint*, const R3BMCTrack*>
    {
        std::map<const R3BNeulandPoint*, const R3BMCTrack*> point_to_track_map;
        for (const auto& point : points)
        {
            point_to_track_map[&point] = nullptr;
            auto iTrack = point.GetTrackID();
            while (iTrack > -1)
            {
                const auto& track = tracks.at(iTrack);
                if (IsPrimaryTrack(track))
                {
                    point_to_track_map[&point] = &track;
                    break;
                }
                // Else, start tracing back:
                iTrack = track.GetMotherId();
            }
        }
        return point_to_track_map;
    }

    auto FindFirstPoint(const R3BMCTrack& track,
                        const std::map<const R3BNeulandPoint*, const R3BMCTrack*>& point_to_track_map)
        -> const R3BNeulandPoint*
    {
        const R3BNeulandPoint* minPoint{ nullptr };
        //  Search for min ToF:
        auto minToF = MIN_TOF_DEFAULT;

        for (auto [map_point, map_track] : point_to_track_map)
        {
            // Only look at points traced back to this track:
            if (map_track == &track)
            {
                const auto ToF = map_point->GetTime();
                if (ToF < minToF)
                {
                    minToF = ToF;
                    minPoint = map_point;
                }
            }
        }
        return minPoint;
    }

    auto FindFirstHit(const R3BMCTrack& track,
                      const std::map<const R3BNeulandPoint*, const R3BMCTrack*>& point_to_track_map,
                      const std::map<const R3BNeulandPoint*, const R3BNeulandHit*>& point_to_hit_map)
        -> const R3BNeulandHit*
    {
        const R3BNeulandPoint* minHitPoint = nullptr;
        //  Search for min ToF:
        double minHitToF = MIN_TOF_DEFAULT;

        for (const auto [map_point, map_track] : point_to_track_map)
        {
            // Only look at points traced back to this track:
            // AND where a hit is registered
            if (map_track == &track && point_to_hit_map.find(map_point) != point_to_hit_map.end())
            {
                const auto ToF = map_point->GetTime();
                if (ToF < minHitToF)
                {
                    minHitToF = ToF;
                    minHitPoint = map_point;
                }
            }
        }
        return minHitPoint != nullptr ? point_to_hit_map.at(minHitPoint) : nullptr;
    }

} // namespace

// NOLINTNEXTLINE: bugprone-easily-swappable-parameters
R3BNeulandPrimaryInteractionFinder::R3BNeulandPrimaryInteractionFinder(std::string_view pointsIn,
                                                                       std::string_view hitsIn,
                                                                       std::string_view pointsOut,
                                                                       std::string_view hitsOut,
                                                                       std::string_view tracksOut)
    : FairTask("R3BNeulandPrimaryInteractionFinder")
    , fTracksIn("MCTrack")
    , fPointsIn(pointsIn)
    , fHitsIn(hitsIn)
    , fTracksOut(tracksOut)
    , fPointsOut(pointsOut)
    , fHitsOut(hitsOut)
    , fhDistance(new TH1D("fhDistance", "Distance firstPoint to firstHit", 10000, 0, 1000))
    , fhPointsVsHits(new TH2D("fhPointsVsHits", "Number of Primary Points vs Number of Primary Hits", 6, 0, 6, 6, 0, 6))
    , fhPointVsHitPaddle(
          new TH2D("fhPointVsHitPaddle", "First Point Paddle vs First Hit Paddle", 3001, -1, 3000, 3001, -1, 3000))
{
    fhDistance->GetXaxis()->SetTitle("Distance [cm]");
    fhPointsVsHits->GetXaxis()->SetTitle("# First Points");
    fhPointsVsHits->GetYaxis()->SetTitle("# First Hits");
    fhPointVsHitPaddle->GetXaxis()->SetTitle("First Point Paddle");
    fhPointVsHitPaddle->GetYaxis()->SetTitle("First Hit Paddle");
}

auto R3BNeulandPrimaryInteractionFinder::Init() -> InitStatus
{
    fTracksIn.init();
    fPointsIn.init();
    fHitsIn.init();
    fTracksOut.init();
    fPointsOut.init();
    fHitsOut.init();
    return kSUCCESS;
}

void R3BNeulandPrimaryInteractionFinder::Exec(Option_t* /*option*/)
{
    const auto& tracks = fTracksIn.read();
    const auto& points = fPointsIn.get();
    const auto& hits = fHitsIn.get();
    fTracksOut.clear();
    fPointsOut.clear();
    fHitsOut.clear();

    const auto point_to_hit_map = MapPointsToHits(points, hits);
    const auto point_to_track_map = MapPointsToPrimaryTracks(points, tracks);

    if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug))
    {
        LOG(debug) << "R3BNeulandPrimaryInteractionFinder: Points without Hit in: ";
        for (const auto [map_point, map_hit] : point_to_hit_map)
        {
            if (map_hit == nullptr)
            {
                const auto& point = map_point;
                LOG(debug) << point->GetDetectorID() << ":" << tracks.at(point->GetTrackID()).GetPdgCode() << ":"
                           << point->GetLightYield() << ":" << point->GetEnergyLoss() << "\t";
            }
        }
    }

    for (const auto& track : tracks)
    {
        if (IsPrimaryTrack(track))
        {
            fTracksOut.get().push_back(track);

            const auto* firstPoint = FindFirstPoint(track, point_to_track_map);
            const auto* firstHit = FindFirstHit(track, point_to_track_map, point_to_hit_map);

            if (firstPoint != nullptr)
            {
                fPointsOut.get().push_back(*firstPoint);
            }

            if (firstHit != nullptr)
            {
                // NOTE: This may add the same hit multiple times.
                fHitsOut.get().push_back(*firstHit);
            }

            if ((firstHit != nullptr) && (firstPoint != nullptr))
            {
                fhDistance->Fill((firstPoint->GetPosition() - firstHit->GetPosition()).Mag());
            }

            fhPointVsHitPaddle->Fill((firstPoint != nullptr) ? firstPoint->GetPaddle() : -1,
                                     (firstHit != nullptr) ? firstHit->GetPaddle() : -1);
        }
    }

    fhPointsVsHits->Fill(static_cast<double>(fPointsOut.get().size()), static_cast<double>(fHitsOut.get().size()));
}

void R3BNeulandPrimaryInteractionFinder::Finish()
{
    TDirectory* tmp = gDirectory;
    FairRootManager::Instance()->GetOutFile()->cd();
    gDirectory->mkdir("R3BNeulandPrimaryInteractionFinder");
    gDirectory->cd("R3BNeulandPrimaryInteractionFinder");

    fhDistance->Write();
    fhPointsVsHits->Write();
    fhPointVsHitPaddle->Write();

    gDirectory = tmp;
}
