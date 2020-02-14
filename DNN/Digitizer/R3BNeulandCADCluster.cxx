#include "R3BNeulandCADCluster.h"
#include <algorithm>
#include <numeric>
#include <stdexcept>

R3BNeulandCADHit R3BNeulandCADCluster::GetForemostHit() const
{
    auto min = std::min_element(fHits.cbegin(), fHits.cend(), [](const R3BNeulandCADHit& a, const R3BNeulandCADHit& b) {
        return a.GetPosition().Z() < b.GetPosition().Z();
    });
    if (min == fHits.end())
    {
        throw std::logic_error("R3BNeulandCADCluster::GetFirstHit(): Cluster has no Hits!");
    }
    return *min;
}

R3BNeulandCADHit R3BNeulandCADCluster::GetFirstHit() const
{
    auto min = std::min_element(fHits.cbegin(), fHits.cend(), [](const R3BNeulandCADHit& a, const R3BNeulandCADHit& b) {
        return a.GetT() < b.GetT();
    });
    if (min == fHits.end())
    {
        throw std::logic_error("R3BNeulandCADCluster::GetFirstHit(): Cluster has no Hits!");
    }
    return *min;
}

R3BNeulandCADHit R3BNeulandCADCluster::GetLastHit() const
{
    auto max = std::max_element(fHits.cbegin(), fHits.cend(), [](const R3BNeulandCADHit& a, const R3BNeulandCADHit& b) {
        return a.GetT() < b.GetT();
    });
    if (max == fHits.end())
    {
        throw std::logic_error("R3BNeulandCADCluster::GetLastHit(): Cluster has no Hits!");
    }
    return *max;
}

R3BNeulandCADHit R3BNeulandCADCluster::GetMaxEnergyHit() const
{
    auto max = std::max_element(fHits.cbegin(), fHits.cend(), [](const R3BNeulandCADHit& a, const R3BNeulandCADHit& b) {
        return a.GetE() < b.GetE();
    });
    if (max == fHits.end())
    {
        throw std::logic_error("R3BNeulandCADCluster::GetLastHit(): Cluster has no Hits!");
    }
    return *max;
}

Double_t R3BNeulandCADCluster::GetE() const
{
    return std::accumulate(
        fHits.begin(), fHits.end(), 0., [](const Double_t sum, const R3BNeulandCADHit& hit) { return sum + hit.GetE(); });
}

Double_t R3BNeulandCADCluster::GetT() const { return GetFirstHit().GetT(); }

TVector3 R3BNeulandCADCluster::GetPosition() const { return GetFirstHit().GetPosition(); };

TVector3 R3BNeulandCADCluster::GetEnergyCentroid() const
{
    // analog to Geometrical Centroid \vec{c} = \frac{\sum_i (\vec{r}_{i} \cdot V_i)}{\sum_i V_i}
    TVector3 centroid =
        std::accumulate(fHits.cbegin(), fHits.cend(), TVector3(), [](const TVector3& c, const R3BNeulandCADHit& hit) {
            return c + (hit.GetPosition() * hit.GetE());
        });
    return centroid * (1. / GetE());
}

Double_t R3BNeulandCADCluster::GetEnergyMoment() const
{
    const TVector3 centroid = GetEnergyCentroid();
    Double_t mom = std::accumulate(fHits.cbegin(), fHits.cend(), 0., [&](const Double_t c, const R3BNeulandCADHit& hit) {
        return c + (hit.GetPosition() - centroid).Mag() * hit.GetE();
    });
    return mom / GetE();
}

Double_t R3BNeulandCADCluster::GetRCluster(Double_t beta) const
{
    // Equation 4.2 in TDR (Page 55).
    return std::abs(beta - GetBeta()) / GetE();
}

std::ostream& operator<<(std::ostream& os, const R3BNeulandCADCluster& cluster)
{
    os << "R3BNeulandCADCluster: NeuLAND Cluster with size " << cluster.GetSize() << std::endl;
    if (cluster.GetSize() > 0)
    {
        os << "    Sum Energy: " << cluster.GetE() << std::endl
           << "    FirstHit: " << cluster.GetFirstHit() << "    LastHit: " << cluster.GetLastHit();
    }
    return os;
}

void R3BNeulandCADCluster::Print(const Option_t*) const { std::cout << *this; }

ClassImp(R3BNeulandCADCluster)
