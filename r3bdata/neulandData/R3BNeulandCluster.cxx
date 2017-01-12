#include "R3BNeulandCluster.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>

R3BNeulandDigi R3BNeulandCluster::GetForemostDigi() const
{
    auto min = std::min_element(fDigis.cbegin(), fDigis.cend(), [](const R3BNeulandDigi& a, const R3BNeulandDigi& b) {
        return a.GetPosition().Z() < b.GetPosition().Z();
    });
    if (min == fDigis.end())
    {
        throw std::logic_error("R3BNeulandCluster::GetFirstDigi(): Cluster has no Digis!");
    }
    return *min;
}

R3BNeulandDigi R3BNeulandCluster::GetFirstDigi() const
{
    auto min = std::min_element(fDigis.cbegin(), fDigis.cend(), [](const R3BNeulandDigi& a, const R3BNeulandDigi& b) {
        return a.GetT() < b.GetT();
    });
    if (min == fDigis.end())
    {
        throw std::logic_error("R3BNeulandCluster::GetFirstDigi(): Cluster has no Digis!");
    }
    return *min;
}

R3BNeulandDigi R3BNeulandCluster::GetLastDigi() const
{
    auto max = std::max_element(fDigis.cbegin(), fDigis.cend(), [](const R3BNeulandDigi& a, const R3BNeulandDigi& b) {
        return a.GetT() < b.GetT();
    });
    if (max == fDigis.end())
    {
        throw std::logic_error("R3BNeulandCluster::GetLastDigi(): Cluster has no Digis!");
    }
    return *max;
}

R3BNeulandDigi R3BNeulandCluster::GetMaxEnergyDigi() const
{
    auto max = std::max_element(fDigis.cbegin(), fDigis.cend(), [](const R3BNeulandDigi& a, const R3BNeulandDigi& b) {
        return a.GetE() < b.GetE();
    });
    if (max == fDigis.end())
    {
        throw std::logic_error("R3BNeulandCluster::GetLastDigi(): Cluster has no Digis!");
    }
    return *max;
}

Double_t R3BNeulandCluster::GetE() const
{
    return std::accumulate(fDigis.begin(), fDigis.end(), 0., [](const Double_t sum, const R3BNeulandDigi& digi) {
        return sum + digi.GetE();
    });
}

Double_t R3BNeulandCluster::GetT() const { return GetFirstDigi().GetT(); }

TVector3 R3BNeulandCluster::GetPosition() const { return GetFirstDigi().GetPosition(); };

TVector3 R3BNeulandCluster::GetEnergyCentroid() const
{
    // analog to Geometrical Centroid \vec{c} = \frac{\sum_i (\vec{r}_{i} \cdot V_i)}{\sum_i V_i}
    TVector3 centroid =
        std::accumulate(fDigis.cbegin(), fDigis.cend(), TVector3(), [](const TVector3& c, const R3BNeulandDigi& digi) {
            return c + (digi.GetPosition() * digi.GetE());
        });
    return centroid * (1. / GetE());
}

Double_t R3BNeulandCluster::GetEnergyMoment() const
{
    const TVector3 centroid = GetEnergyCentroid();
    Double_t mom =
        std::accumulate(fDigis.cbegin(), fDigis.cend(), 0., [&](const Double_t c, const R3BNeulandDigi& digi) {
            return c + (digi.GetPosition() - centroid).Mag() * digi.GetE();
        });
    return mom / GetE();
}

std::ostream& operator<<(std::ostream& os, const R3BNeulandCluster& cluster)
{
    os << "R3BNeulandCluster: NeuLAND Cluster with size " << cluster.GetSize() << std::endl;
    if (cluster.GetSize() > 0)
    {
        os << "    Sum Energy: " << cluster.GetE() << std::endl
           << "    FirstDigi: " << cluster.GetFirstDigi() << "    LastDigi: " << cluster.GetLastDigi();
    }
    return os;
}

void R3BNeulandCluster::Print(const Option_t*) const { std::cout << *this; }

Double_t R3BNeulandCluster::GetRCluster(Double_t beta) const
{
    // Equation 4.2 in TDR (Page 55).
    return std::abs(beta - GetBeta()) / GetE();
}

ClassImp(R3BNeulandCluster)
