#include "ReconstructionEGate.h"

namespace Neuland
{
    ReconstructionEGate::ReconstructionEGate(const Double_t EToFMin, const Double_t EToFMax, const Double_t EDepMin)
        : fEToFMin(EToFMin)
        , fEToFMax(EToFMax)
        , fEDepMin(EDepMin)
    {
    }

    void ReconstructionEGate::Init() {}

    std::vector<R3BNeulandNeutron> ReconstructionEGate::GetNeutrons(
        const std::vector<R3BNeulandCluster*>& clusters) const
    {
        std::vector<R3BNeulandCluster*> filteredclusters;
        std::copy_if(clusters.begin(),
                     clusters.end(),
                     std::back_inserter(filteredclusters),
                     [&](const R3BNeulandCluster* a)
                     {
                         return a->GetFirstDigi().GetEToF() > fEToFMin && a->GetFirstDigi().GetEToF() < fEToFMax &&
                                a->GetE() > fEDepMin;
                     });

        std::vector<R3BNeulandNeutron> neutrons;
        neutrons.reserve(filteredclusters.size());
        for (const auto cluster : filteredclusters)
        {
            const R3BNeulandDigi digi = cluster->GetFirstDigi();
            neutrons.push_back(R3BNeulandNeutron(digi.GetT(), digi.GetPosition()));
        }
        return neutrons;
    }
}
