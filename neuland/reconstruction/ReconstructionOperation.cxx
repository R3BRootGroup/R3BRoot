#include "ReconstructionOperation.h"

#include "ClusteringEngine.h"
#include "ElasticScattering.h"
#include "FairLogger.h"
#include "FairRtdbRun.h"
#include "FairRuntimeDb.h"
#include "IsElastic.h"
#include "R3BNeulandNeutron2DPar.h"
#include <numeric>

namespace Neuland
{
    void Multiplicity2DCalibr::Init()
    {
        FairRuntimeDb* rtdb = FairRuntimeDb::instance();
        if (!rtdb)
        {
            LOG(FATAL) << "Multiplicity2DCalibr: No FairRuntimeDb!" << FairLogger::endl;
        }

        fPar = (R3BNeulandNeutron2DPar*)rtdb->getContainer("R3BNeulandNeutron2DPar");
        if (!fPar)
        {
            LOG(FATAL) << "Multiplicity2DCalibr: No R3BNeulandNeutron2DPar!" << FairLogger::endl;
        }

        // FIXME: FairRuntimeDB needs to be forced to load the Data from the second file with Run Id 1
        rtdb->initContainers(rtdb->getCurrentRun()->getRunId(), 1);
        fPar->printParams();
    }

    void Multiplicity2DCalibr::Exec(NeutronCandidates& clusters, Multiplicity& mult, Neutrons&) const
    {
        const Double_t Etot =
            std::accumulate(clusters.cbegin(), clusters.cend(), 0., [](const Double_t a, const R3BNeulandCluster* b) {
                return a + b->GetE();
            });
        const Int_t nClusters = clusters.size();
        // Get Number of neutrons using the previously calibrated cuts stored in the parameter file
        mult = fPar->GetNeutronMultiplicity(Etot, nClusters);
    }

    void GetNeutrons::Exec(NeutronCandidates& clusters, Multiplicity& multiplicity, Neutrons& neutrons) const
    {
        std::sort(clusters.begin(), clusters.end(), [](const R3BNeulandCluster* a, const R3BNeulandCluster* b) {
            return a->GetFirstDigi().GetEToF() > b->GetFirstDigi().GetEToF();
        });

        for (UInt_t n = 0; n < multiplicity && neutrons.size() <= multiplicity && n < clusters.size(); n++)
        {
            neutrons.push_back(
                R3BNeulandNeutron(clusters.at(n)->GetFirstDigi().GetT(), clusters.at(n)->GetFirstDigi().GetPosition()));
        }
    }

    ElasticProtonScatteringRemoveSecondaries::ElasticProtonScatteringRemoveSecondaries()
        : fTracker([](const R3BNeulandCluster* a, const R3BNeulandCluster* b) { return Neuland::IsElastic(a, b); })
    {
    }

    void ElasticProtonScatteringRemoveSecondaries::Exec(NeutronCandidates& clusters, Multiplicity&, Neutrons&) const
    {
        auto connectedclusters = fTracker.Clusterize(clusters);

        clusters.clear();
        for (auto& connectedcluster : connectedclusters)
        {
            std::sort(connectedcluster.begin(),
                      connectedcluster.end(),
                      [](const R3BNeulandCluster* a, const R3BNeulandCluster* b) {
                          return a->GetFirstDigi().GetT() < b->GetFirstDigi().GetT();
                      });
            clusters.push_back(connectedcluster.at(0));
        }
    }

    void UseClustersWithProtonTrackMatchingEToF::Exec(NeutronCandidates& clusters,
                                                      Multiplicity& multiplicity,
                                                      Neutrons& neutrons) const
    {
        auto i = std::begin(clusters);

        while (i != std::end(clusters))
        {
            const R3BNeulandCluster* cluster = *i;
            // const auto ep = Neuland::RecoilProtonEnergy(cluster);
            // const bool isMatchingDepositedEnergy = std::abs(ep - 1.25 * cluster->GetE()) < 50.;
            const auto en = Neuland::NeutronEnergyFromElasticProtonScattering(cluster);
            const bool isMatchingToF = std::abs(en - cluster->GetFirstDigi().GetEToF()) < fMaxNeutronEnergyDifference;
            if (neutrons.size() < multiplicity /*&& isMatchingDepositedEnergy*/ && isMatchingToF)
            {
                neutrons.push_back(
                    R3BNeulandNeutron(cluster->GetFirstDigi().GetT(), cluster->GetFirstDigi().GetPosition()));
                i = clusters.erase(i);
            }
            else
            {
                ++i;
            }
        }
    }
}

namespace Neuland
{
    void ReconstructionOperations::Init()
    {
        for (auto& op : fOps)
        {
            op->Init();
        }
    }

    std::vector<R3BNeulandNeutron> ReconstructionOperations::GetNeutrons(
        const std::vector<R3BNeulandCluster*>& cin) const
    {
        auto clusters(cin);
        UInt_t multiplicity(0);
        std::vector<R3BNeulandNeutron> neutrons;

        for (const auto& op : fOps)
        {
            op->Exec(clusters, multiplicity, neutrons);
        }

        return neutrons;
    }
}
