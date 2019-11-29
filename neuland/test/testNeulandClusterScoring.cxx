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

#include "ClusterScoring.h"
#include "gtest/gtest.h"

namespace
{
    using Neuland::ClusterScoring;

    class DummyClusters : public testing::Test
    {
      protected:
        void SetUp() override
        {
            auto cluster = new R3BNeulandCluster{ R3BNeulandHit{} };
            clusters_.push_back(cluster);
        }

        void TearDown() override
        {
            for (auto cluster : clusters_)
            {
                delete cluster;
            }
            clusters_.clear();
        }

        std::vector<R3BNeulandCluster*> clusters_;
    };

    TEST_F(DummyClusters, takesAVectorOfClustersAndReturnsAVectorOfNeutrons)
    {
        ClusterScoring rs{};
        std::vector<R3BNeulandNeutron> neutrons = rs.GetNeutrons(clusters_);
        EXPECT_EQ(neutrons.size(), 1u);
    }

    TEST_F(DummyClusters, canSetAMinimumScoreThatClustersHaveToReach)
    {
        ClusterScoring rs{};
        rs.SetMinScore(1);
        auto neutrons = rs.GetNeutrons(clusters_);
        EXPECT_EQ(neutrons.size(), 0u);
    }

    TEST_F(DummyClusters, canAddScorersThatScoreClusters)
    {
        ClusterScoring rs{};
        rs.SetMinScore(1);
        ClusterScoring::Scorer scorer = [](ClusterScoring::ScoredClusters& scoredClusters) {
            for (auto& scoredCluster : scoredClusters)
            {
                scoredCluster.score += 1.;
            }
        };
        rs.AddScorer(scorer);
        auto neutrons = rs.GetNeutrons(clusters_);
        EXPECT_EQ(neutrons.size(), 1u);
    }
} // namespace
