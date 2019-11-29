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

#include "RecoBayesWCP.h"
#include "TH1D.h"
#include <fstream>
#include <iostream>
#include <utility>

Neuland::RecoBayesWCP::RecoBayesWCP(std::vector<unsigned int> hypos, const std::string& prefix)
    : fHypotheses(std::move(hypos))
    , fPNclus(prefix + ".nclus.dat")
    , fPEtot(prefix + ".etot.dat")
    , fhPrim(new TH1D("RecoBayesWCPPrim", "RecoBayesWCPPrim", 1000, 0., 1.))
    , fClusterLikelihoods({
          { prefix + ".energy.dat", [](const ScoredCluster& sc) { return sc.cluster->GetE(); } },
          { prefix + ".etof.dat", [](const ScoredCluster& sc) { return sc.cluster->GetEToF(); } },
          { prefix + ".rval.dat", [](const ScoredCluster& sc) { return std::log10(sc.cluster->GetRCluster(0.793)); } },
          { prefix + ".size.dat", [](const ScoredCluster& sc) { return sc.cluster->GetSize(); } },
          { prefix + ".time.dat", [](const ScoredCluster& sc) { return sc.cluster->GetT(); } },
      })
    , fPrimaryClusters("NeulandPrimaryClustersMultiple") // Delete Me
{
}

Neuland::RecoBayesWCP::~RecoBayesWCP() { fhPrim->Write(); }

std::vector<R3BNeulandNeutron> Neuland::RecoBayesWCP::GetNeutrons(const std::vector<R3BNeulandCluster*>& clusters) const
{
    std::vector<ScoredCluster> scoredClusters(clusters.begin(), clusters.end());

    for (auto& sc : scoredClusters)
    {
        ScoreCluster(sc);
    }

    std::sort(scoredClusters.begin(), scoredClusters.end(), [](const ScoredCluster& a, const ScoredCluster& b) {
        return a.prim > b.prim;
    });

    const unsigned int mult = GetNMult(scoredClusters);
    // const auto mult = fPrimaryClusters.Retrieve().size(); // Delete Me

    std::vector<R3BNeulandNeutron> neutrons;
    neutrons.reserve(mult);
    for (unsigned int i = 0; i < mult && i < scoredClusters.size(); i++)
    {
        neutrons.emplace_back(*scoredClusters.at(i).cluster);
    }

    return neutrons;
}

void Neuland::RecoBayesWCP::ScoreCluster(Neuland::RecoBayesWCP::ScoredCluster& sc) const
{
    sc.prim = 0.5;
    sc.sec = 0.5;

    for (const auto& cl : fClusterLikelihoods)
    {
        cl.Apply(sc);
    }

    auto norm = sc.prim + sc.sec;
    if (norm < 0.001)
    {
        sc.prim = 0;
        sc.sec = 1;
    }
    else
    {
        sc.prim /= norm;
        sc.sec /= norm;
    }

    fhPrim->Fill(sc.prim);
}

unsigned int Neuland::RecoBayesWCP::GetNMult(const ScoredClusters& clusters) const
{
    if (clusters.empty())
    {
        return 0;
    }

    const auto Etot =
        std::accumulate(clusters.cbegin(), clusters.cend(), 0., [](const Double_t a, const ScoredCluster& b) {
            return a + b.cluster->GetE();
        });
    const auto nClusters = clusters.size();

    std::map<unsigned int, double> calc;
    double sum = 0.;
    for (const auto& h : fHypotheses)
    {
        // Prior
        calc[h] = 1. / double(fHypotheses.size());
        // P(Energy | h)
        calc[h] *= fPEtot.P(Etot, h);
        // P(Nclus | h)
        calc[h] *= fPNclus.P(nClusters, h);

        if (h <= nClusters)
        {
            calc[h] *= clusters.at(h - 1).prim;
            // const auto med = std::accumulate(clusters.begin(),
            //                           clusters.begin() + h,
            //                           0.,
            //                           [](const double s, const ScoredCluster& sc) { return s + sc.prim; }) /
            //           double(h);
            // calc[h] *= med;
        }
        else
        {
            calc[h] *= 0;
        }

        if (std::isnan(calc.at(h)))
        {
            calc[h] = 0;
        }

        // Sum
        sum += calc.at(h);
    }

    if (sum > 0)
    {
        for (const auto& h : fHypotheses)
        {
            calc[h] /= sum;
        }
    }

    auto pr = std::max_element(calc.begin(),
                               calc.end(),
                               [](const std::map<int, double>::value_type& p1,
                                  const std::map<int, double>::value_type& p2) { return p1.second < p2.second; });

    return pr->first;
}
