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

#ifndef NEULANDRECONSTRUCTIONOSCORINGH
#define NEULANDRECONSTRUCTIONOSCORINGH

#include "ElasticScattering.h"
#include "IsElastic.h"
#include "ReconstructionEngine.h"
#include <TCAConnector.h> // Delete me
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
        static const Scorer scoreFirstCluster;

        void Init() override { fPrimaryClusters.Init(); } // Delete Me

      private:
        std::vector<Scorer> fOps;
        Double_t fMinScore;
        UInt_t fMaxN;

        TCAInputConnector<R3BNeulandCluster> fPrimaryClusters; // Delete Me
    };
} // namespace Neuland

#endif // NEULANDRECONSTRUCTIONOSCORINGH
