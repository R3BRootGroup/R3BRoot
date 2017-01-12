#include "ClusterScoring.h"

namespace Neuland
{
    ClusterScoring::ClusterScoring()
        : fMinScore(0.)
        , fMaxN(100)
    {
    }

    ClusterScoring::ClusterScoring(const std::vector<Scorer>& ops)
        : fOps(ops)
        , fMinScore(0.)
        , fMaxN(100)
    {
    }

    std::vector<R3BNeulandNeutron> ClusterScoring::GetNeutrons(const std::vector<R3BNeulandCluster*>& clusters) const
    {
        std::vector<R3BNeulandNeutron> neutrons;
        std::vector<ScoredCluster> scoredClusters(clusters.begin(), clusters.end());

        for (const auto& op : fOps)
        {
            op(scoredClusters);
        }

        std::sort(scoredClusters.begin(), scoredClusters.end(), [](const ScoredCluster& a, const ScoredCluster& b) {
            // Primary: Score descending
            if (a.score > b.score)
                return true;
            else if (a.score < b.score)
                return false;

            // Secondary: Time ascending
            if (a.cluster->GetT() < b.cluster->GetT())
                return true;
            else if (a.cluster->GetT() > b.cluster->GetT())
                return false;

            return false;
        });

        for (const auto& sc : scoredClusters)
        {
            if (sc.score >= fMinScore && neutrons.size() < fMaxN)
            {
                neutrons.emplace_back(sc.cluster->GetT(), sc.cluster->GetPosition());
            }
        }
        return neutrons;
    }

    // Prefer clusters with high energy deposition
    const ClusterScoring::Scorer ClusterScoring::scoreEnergy = [](ScoredClusters& scs) {
        std::for_each(scs.begin(), scs.end(), [](ScoredCluster& sc) {
            if (sc.cluster->GetE() < 5.)
            {
                sc.score -= 1.;
            }
            if (sc.cluster->GetE() > 50. && sc.cluster->GetE() < 150.)
            {
                sc.score += 1.;
            }
            if (sc.cluster->GetE() > 150.)
            {
                sc.score += 4.;
            }
        });
    };

    const ClusterScoring::Scorer ClusterScoring::scoreHitSequenceWithDescecdingZPosition = [](ScoredClusters& scsin) {
        if (scsin.size() < 3)
        {
            return;
        }

        // Only consider clusters with positive score so far
        std::vector<ScoredCluster*> scs;
        for (auto& sc : scsin)
        {
            if (sc.score > 0)
            {
                scs.push_back(&sc);
            }
        }

        // Sort by time asc
        std::sort(scs.begin(), scs.end(), [](const ScoredCluster* a, const ScoredCluster* b) {
            return a->cluster->GetT() < b->cluster->GetT();
        });

        // Check if also sorted by Z desc -> First Hit in Time in the back of the detector
        const auto sortedEnd =
            std::is_sorted_until(scs.begin(), scs.end(), [](const ScoredCluster* a, const ScoredCluster* b) {
                return a->cluster->GetPosition().Z() > b->cluster->GetPosition().Z();
            });
        const auto sortedDistance = std::distance(scs.begin(), sortedEnd);
        if (sortedDistance > 2)
        {
            std::for_each(scs.begin(), sortedEnd, [](ScoredCluster* a) { a->score += 10.; });
        }
    };

    const ClusterScoring::Scorer ClusterScoring::scoreProtonTracksMatchingToF = [](ScoredClusters& scs) {
        for (auto& sc : scs)
        {
            if (sc.cluster->GetSize() < 3 || sc.cluster->GetE() < 30)
            {
                continue;
            }

            const auto en = Neuland::NeutronEnergyFromElasticProtonScattering(sc.cluster);
            const bool isMatchingToF = std::abs(en - sc.cluster->GetFirstDigi().GetEToF()) < 50.;
            if (isMatchingToF)
            {
                sc.score += (50. - std::abs(en - sc.cluster->GetFirstDigi().GetEToF())) / 5.;
            }
        }
    };

    const ClusterScoring::Scorer ClusterScoring::scoreElasticSecondaryInterations = [](ScoredClusters& scs) {
        for (auto ita = scs.begin(); ita != scs.end(); ita++)
        {
            for (auto itb = ita + 1; itb != scs.end(); itb++)
            {
                if (IsElastic(ita->cluster, itb->cluster))
                {
                    ita->score += 3.;
                    itb->score -= 5.;
                }
                if (IsElastic(itb->cluster, ita->cluster))
                {
                    ita->score -= 5.;
                    itb->score += 3.;
                }
            }
        }
    };

    const ClusterScoring::Scorer ClusterScoring::scoreClusterSize = [](ScoredClusters& scs) {
        std::for_each(scs.begin(), scs.end(), [](ScoredCluster& sc) {
            if (sc.cluster->GetSize() < 5)
            {
                sc.score -= 1.;
            }
            if (sc.cluster->GetSize() > 5 && sc.cluster->GetSize() < 10)
            {
                sc.score += 1.;
            }
            if (sc.cluster->GetSize() > 10 && sc.cluster->GetSize() < 15)
            {
                sc.score += 3.;
            }
            if (sc.cluster->GetSize() > 15)
            {
                sc.score += 7.;
            }
        });
    };

    const ClusterScoring::Scorer ClusterScoring::handleMultiHitClusters = [](ScoredClusters& scs) {
        ScoredClusters queue;
        for (auto sc : scs)
        {
            if (sc.cluster->GetSize() >= 2 &&
                (sc.cluster->GetForemostDigi().GetT() - sc.cluster->GetFirstDigi().GetT()) > 0.1)
            {
                queue.emplace_back(sc.cluster, sc.score - 1.);
            }
        }
        scs.insert(scs.end(), queue.begin(), queue.end());
    };

} // namespace Neuland
