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

#include "RecoBayes.h"
#include "TH1D.h"
#include <fstream>
#include <iostream>
#include <utility>

Neuland::RecoBayes::RecoBayes(std::vector<unsigned int> hypos, const std::string& prefix)
    : fHypotheses(std::move(hypos))
    , fPNclus(prefix + ".nclus.dat")
    , fPEtot(prefix + ".etot.dat")
    , fhPrim(new TH1D("RecoBayesPrim", "RecoBayesPrim", 1000, 0., 1.))
    , fMinPrim(0.2)
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

Neuland::RecoBayes::~RecoBayes() { fhPrim->Write(); }

std::vector<R3BNeulandNeutron> Neuland::RecoBayes::GetNeutrons(const std::vector<R3BNeulandCluster*>& clusters) const
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
    /*unsigned int j = 0;
    for (unsigned int i = 0; i < mult; i++)
    {
        if (j < scoredClusters.size() && scoredClusters.at(j).prim > fMinPrim)
        {
            neutrons.emplace_back(scoredClusters.at(j));
        }
        else
        {
            // Total abomination and a crime
            j = 0;
            neutrons.emplace_back(scoredClusters.at(j));
            // neutrons.emplace_back(R3BNeulandNeutron());
            // std::cout << "denied!";
        }
        j++;
    }*/

    unsigned int j = 0;
    const double MinPrimMult = 0.25;
    while (neutrons.size() < mult)
    {
        const auto i = neutrons.size();
        if (i < scoredClusters.size() && scoredClusters.at(i).prim > fMinPrim)
        {
            neutrons.emplace_back(*scoredClusters.at(i).cluster);
        }
        else
        {
            if (j < scoredClusters.size() && scoredClusters.at(j).prim > MinPrimMult)
            {
                neutrons.emplace_back(*scoredClusters.at(j).cluster);
                j++;
            }
            else
            {
                break;
            }
        }
    }

    return neutrons;
}

void Neuland::RecoBayes::ScoreCluster(Neuland::RecoBayes::ScoredCluster& sc) const
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

    std::cout << " -> " << sc.prim << ", " << sc.sec << std::endl;

    fhPrim->Fill(sc.prim);
}

unsigned int Neuland::RecoBayes::GetNMult(const ScoredClusters& clusters) const
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
    double norm = 0.;
    for (const auto& h : fHypotheses)
    {
        // Prior
        calc[h] = 1. / double(fHypotheses.size());
        // P(Energy | h)
        calc[h] *= fPEtot.P(Etot, h);
        // P(Nclus | h)
        calc[h] *= fPNclus.P(nClusters, h);

        if (std::isnan(calc.at(h)))
        {
            calc[h] = 0;
        }

        norm += calc.at(h);
    }

    if (norm > 0)
    {
        for (const auto& h : fHypotheses)
        {
            calc[h] /= norm;
        }
    }

    auto pr = std::max_element(calc.begin(),
                               calc.end(),
                               [](const std::map<int, double>::value_type& p1,
                                  const std::map<int, double>::value_type& p2) { return p1.second < p2.second; });

    return pr->first;
}
