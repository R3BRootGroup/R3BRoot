#ifndef NEULANDRECONSTRUCTIONOSCORINGH
#define NEULANDRECONSTRUCTIONOSCORINGH

#include "ElasticScattering.h"
#include "IsElastic.h"
#include "ReconstructionEngine.h"
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace Neuland
{
    class ClusterScoring : public ReconstructionEngine
    {
      public:
        struct ScoredCluster
        {
            const R3BNeulandCluster* cluster;
            double score;

            explicit ScoredCluster(const R3BNeulandCluster* c)
                : cluster(c)
                , score(0.)
            {
            }
            ScoredCluster(const R3BNeulandCluster* c, double s)
                : cluster(c)
                , score(s)
            {
            }
        };

        using ScoredClusters = std::vector<ScoredCluster>; // Workaround for ROOT Dictionary Generator
        using Scorer = std::function<void(Neuland::ClusterScoring::ScoredClusters&)>;

        ClusterScoring();
        explicit ClusterScoring(const std::vector<Scorer>&);

        void AddScorer(const Scorer& s) { fOps.push_back(s); }
        void SetMinScore(const Double_t i) { fMinScore = i; }
        void SetMaxN(const UInt_t i) { fMaxN = i; }
        std::vector<R3BNeulandNeutron> GetNeutrons(const std::vector<R3BNeulandCluster*>&) const override;

        static const Scorer scoreEnergy;
        static const Scorer scoreHitSequenceWithDescecdingZPosition;
        static const Scorer scoreProtonTracksMatchingToF;
        static const Scorer scoreElasticSecondaryInterations;
        static const Scorer scoreClusterSize;
        static const Scorer handleMultiHitClusters;

      private:
        std::vector<Scorer> fOps;
        Double_t fMinScore;
        UInt_t fMaxN;
    };
} // namespace Neuland

#endif // NEULANDRECONSTRUCTIONOSCORINGH
