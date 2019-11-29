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

#ifndef R3BROOT_RecoBayesWCP_H
#define R3BROOT_RecoBayesWCP_H

#include "Likelihood.h"
#include "ReconstructionEngine.h"
#include <TCAConnector.h> // Delete me
#include <functional>
#include <map>
#include <numeric>

class TH1D;

namespace Neuland
{
    class RecoBayesWCP : public ReconstructionEngine
    {
      public:
        struct ScoredCluster
        {
            const R3BNeulandCluster* cluster;
            double prim;
            double sec;

            explicit ScoredCluster(const R3BNeulandCluster* c)
                : cluster(c)
                , prim(0.)
                , sec(0.)
            {
            }
        };

        struct ClusterLikelihood
        {
            const Likelihood likelihood;
            const std::function<double(const ScoredCluster& sc)> calc;

            ClusterLikelihood(const std::string& lhfile, std::function<double(const ScoredCluster&)> calc_)
                : likelihood(lhfile)
                , calc(std::move(calc_))
            {
            }

            void Apply(ScoredCluster& sc) const
            {
                auto e = calc(sc);
                sc.prim *= likelihood.P(e, 1);
                sc.sec *= likelihood.P(e, 0);
            }
        };

        using ScoredClusters = std::vector<ScoredCluster>;

        RecoBayesWCP(std::vector<unsigned int> hypos, const std::string& prefix);
        ~RecoBayesWCP() override;

        unsigned int GetNMult(const ScoredClusters&) const;
        void ScoreCluster(ScoredCluster&) const;

        std::vector<R3BNeulandNeutron> GetNeutrons(const std::vector<R3BNeulandCluster*>&) const override;

        void Init() override { fPrimaryClusters.Init(); } // Delete Me

      private:
        const std::vector<unsigned int> fHypotheses;
        const Likelihood fPNclus;
        const Likelihood fPEtot;
        TH1D* fhPrim;

        const std::vector<ClusterLikelihood> fClusterLikelihoods;

        TCAInputConnector<R3BNeulandCluster> fPrimaryClusters; // Delete Me
    };
} // namespace Neuland

#endif // R3BROOT_RecoBayesWCP_H
