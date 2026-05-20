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

#include "R3BNeulandCluster.h"
#include "R3BNeulandHit.h"
#include <Math/Vector3Dfwd.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <ostream>
#include <stdexcept>

R3BNeulandHit R3BNeulandCluster::GetForemostHit() const
{
    auto min = std::min_element(fHits.cbegin(),
                                fHits.cend(),
                                [](const R3BNeulandHit& a, const R3BNeulandHit& b)
                                { return a.GetPosition().Z() < b.GetPosition().Z(); });
    if (min == fHits.end())
    {
        throw std::logic_error("R3BNeulandCluster::GetFirstHit(): Cluster has no Hits!");
    }
    return *min;
}

R3BNeulandHit R3BNeulandCluster::GetFirstHit() const
{
    auto min = std::min_element(fHits.cbegin(),
                                fHits.cend(),
                                [](const R3BNeulandHit& a, const R3BNeulandHit& b) { return a.GetT() < b.GetT(); });
    if (min == fHits.end())
    {
        throw std::logic_error("R3BNeulandCluster::GetFirstHit(): Cluster has no Hits!");
    }
    return *min;
}

R3BNeulandHit R3BNeulandCluster::GetLastHit() const
{
    auto max = std::max_element(fHits.cbegin(),
                                fHits.cend(),
                                [](const R3BNeulandHit& a, const R3BNeulandHit& b) { return a.GetT() < b.GetT(); });
    if (max == fHits.end())
    {
        throw std::logic_error("R3BNeulandCluster::GetLastHit(): Cluster has no Hits!");
    }
    return *max;
}

R3BNeulandHit R3BNeulandCluster::GetMaxEnergyHit() const
{
    auto max = std::max_element(fHits.cbegin(),
                                fHits.cend(),
                                [](const R3BNeulandHit& a, const R3BNeulandHit& b) { return a.GetE() < b.GetE(); });
    if (max == fHits.end())
    {
        throw std::logic_error("R3BNeulandCluster::GetLastHit(): Cluster has no Hits!");
    }
    return *max;
}

Double_t R3BNeulandCluster::GetE() const
{
    return std::accumulate(
        fHits.begin(), fHits.end(), 0., [](const Double_t sum, const R3BNeulandHit& hit) { return sum + hit.GetE(); });
}

Double_t R3BNeulandCluster::GetT() const { return GetFirstHit().GetT(); }

auto R3BNeulandCluster::GetPosition() const -> ROOT::Math::XYZVector { return GetFirstHit().GetPosition(); };

auto R3BNeulandCluster::GetEnergyCentroid() const -> ROOT::Math::XYZVector
{
    // analog to Geometrical Centroid \vec{c} = \frac{\sum_i (\vec{r}_{i} \cdot V_i)}{\sum_i V_i}
    const auto centroid = std::accumulate(fHits.cbegin(),
                                          fHits.cend(),
                                          ROOT::Math::XYZVector(),
                                          [](const ROOT::Math::XYZVector& cluster, const R3BNeulandHit& hit)
                                          { return cluster + (hit.GetPosition() * hit.GetE()); });
    return centroid * (1. / GetE());
}

auto R3BNeulandCluster::GetEnergyMoment() const -> Double_t
{
    const ROOT::Math::XYZVector centroid = GetEnergyCentroid();
    Double_t mom = std::accumulate(fHits.cbegin(),
                                   fHits.cend(),
                                   0.,
                                   [&](const Double_t cluster, const R3BNeulandHit& hit)
                                   { return cluster + ((hit.GetPosition() - centroid).r() * hit.GetE()); });
    return mom / GetE();
}

Double_t R3BNeulandCluster::GetRCluster(Double_t beta) const
{
    // Equation 4.2 in TDR (Page 55).
    return std::abs(beta - GetBeta()) / GetE();
}

Double_t R3BNeulandCluster::GetRECluster(Double_t ekin) const { return std::abs(ekin - GetEToF()) / GetE(); }

std::ostream& operator<<(std::ostream& os, const R3BNeulandCluster& cluster)
{
    os << "R3BNeulandCluster: NeuLAND Cluster with size " << cluster.GetSize() << std::endl;
    if (cluster.GetSize() > 0)
    {
        os << "    Sum Energy: " << cluster.GetE() << std::endl
           << "    FirstHit: " << cluster.GetFirstHit() << "    LastHit: " << cluster.GetLastHit();
    }
    return os;
}

void R3BNeulandCluster::Print(const Option_t*) const { std::cout << *this; }

ClassImp(R3BNeulandCluster)
