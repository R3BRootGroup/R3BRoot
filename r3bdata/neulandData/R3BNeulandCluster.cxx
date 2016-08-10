#include "R3BNeulandCluster.h"

#include <algorithm>
#include <stdexcept>
#include <numeric>

using namespace std;

R3BNeulandDigi R3BNeulandCluster::GetFirstDigi() const
{
    auto min = std::min_element(fDigis.begin(),
                                fDigis.end(),
                                [](const R3BNeulandDigi& a, const R3BNeulandDigi& b)
                                {
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
    auto max = std::max_element(fDigis.begin(),
                                fDigis.end(),
                                [](const R3BNeulandDigi& a, const R3BNeulandDigi& b)
                                {
                                    return a.GetT() < b.GetT();
                                });
    if (max == fDigis.end())
    {
        throw std::logic_error("R3BNeulandCluster::GetLastDigi(): Cluster has no Digis!");
    }
    return *max;
}

Double_t R3BNeulandCluster::GetE() const
{
    return std::accumulate(fDigis.begin(),
                           fDigis.end(),
                           0.,
                           [](const Double_t sum, const R3BNeulandDigi& digi)
                           {
                               return sum + digi.GetE();
                           });
}

Double_t R3BNeulandCluster::GetT() const
{
    return GetFirstDigi().GetT();
}

ostream& operator<<(ostream& os, const R3BNeulandCluster& cluster)
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

ClassImp(R3BNeulandCluster)
