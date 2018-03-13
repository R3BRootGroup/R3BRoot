#include "RecoTDR.h"
#include "FairLogger.h"
#include "FairRtdbRun.h"
#include "FairRuntimeDb.h"
#include "IsElastic.h"
#include "R3BNeulandNeutron2DPar.h"
#include <numeric>

Neuland::RecoTDR::RecoTDR()
    : fPar(nullptr)
    , fEBeamMeV(0.)
    , fBetaBeam(0.)
    , fClusterMinE(2.5) // Magic numbers from R3BNeutronTracker2D Advanced Method
{
}

void Neuland::RecoTDR::Init()
{
    auto rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(FATAL) << "Neuland::RecoTDR::Init: No FairRuntimeDb!" << FairLogger::endl;
        return;
    }

    fPar = (R3BNeulandNeutron2DPar*)rtdb->getContainer("R3BNeulandNeutron2DPar");
    if (!fPar)
    {
        LOG(FATAL) << "Neuland::RecoTDR::Init: No R3BNeulandNeutron2DPar!" << FairLogger::endl;
        return;
    }

    // FIXME: FairRuntimeDB needs to be forced to load the Data from the second file with Run Id 1
    rtdb->initContainers(rtdb->getCurrentRun()->getRunId(), 1);
    // fPar->printParams();
}

void Neuland::RecoTDR::SetBeam(Double_t EBeamMeV, Double_t BetaBeam)
{
    fEBeamMeV = EBeamMeV;
    fBetaBeam = BetaBeam;
}

std::vector<R3BNeulandNeutron> Neuland::RecoTDR::GetNeutrons(const std::vector<R3BNeulandCluster*>& c) const
{
    // Recreate R3BNeutronTracker2D Advanced Method
    std::vector<R3BNeulandCluster*> clusters(c);           // make a local copy, non-owning pointers, guaranteed alive.
    const auto nNeutrons = FindNumberOfNeutrons(clusters); // uses calorimetry, needs to run before filters
    FilterClustersByElasticScattering(clusters); // Check all pairs of clusters. Remove clusters from elastic scattering
    FilterClustersByEnergyDeposit(clusters);
    FilterClustersByBeta(clusters);
    SortClustersByRValue(clusters);
    PrioritizeTimeWiseFirstCluster(clusters);

    std::vector<R3BNeulandNeutron> neutrons;
    for (UInt_t n = 0; n < clusters.size() && n < nNeutrons; n++)
    {
        neutrons.emplace_back(clusters.at(n)->GetT(), clusters.at(n)->GetPosition());
    }
    return neutrons;
}

void Neuland::RecoTDR::SortClustersByRValue(std::vector<R3BNeulandCluster*>& clusters) const
{
    std::sort(clusters.begin(), clusters.end(), [&](const R3BNeulandCluster* a, const R3BNeulandCluster* b) {
        return a->GetRCluster(fBetaBeam) < b->GetRCluster(fBetaBeam);
    });
}

void Neuland::RecoTDR::FilterClustersByEnergyDeposit(std::vector<R3BNeulandCluster*>& clusters) const
{
    clusters.erase(
        std::remove_if(
            clusters.begin(), clusters.end(), [&](const R3BNeulandCluster* a) { return a->GetE() < fClusterMinE; }),
        clusters.end());
}

void Neuland::RecoTDR::FilterClustersByBeta(std::vector<R3BNeulandCluster*>& clusters) const
{
    clusters.erase(std::remove_if(clusters.begin(),
                                  clusters.end(),
                                  [&](const R3BNeulandCluster* a) {
                                      // Magic numbers from R3BNeutronTracker2D Advanced Method
                                      return std::abs(a->GetBeta() - fBetaBeam) > (0.05 * 600. / fEBeamMeV);
                                  }),
                   clusters.end());
}

void Neuland::RecoTDR::FilterClustersByElasticScattering(std::vector<R3BNeulandCluster*>& clusters) const
{
    std::map<const R3BNeulandCluster*, bool> marked;

    for (const auto& c : clusters)
    {
        marked[c] = false;
    }

    for (const auto& a : clusters)
    {
        for (const auto& b : clusters)
        {
            if (a != b && a->GetT() < b->GetT() && Neuland::IsElastic(a, b))
            {
                marked[b] = true;
            }
        }
    }

    clusters.erase(
        std::remove_if(clusters.begin(), clusters.end(), [&](const R3BNeulandCluster* a) { return marked.at(a); }),
        clusters.end());
}

UInt_t Neuland::RecoTDR::FindNumberOfNeutrons(std::vector<R3BNeulandCluster*>& clusters) const
{
    const auto Etot =
        std::accumulate(clusters.cbegin(), clusters.cend(), 0., [](const Double_t a, const R3BNeulandCluster* b) {
            return a + b->GetE();
        });
    const auto nClusters = clusters.size();
    if (fPar == nullptr)
    {
        return 0; // Can test better this way. Don't forget to Init();
    }
    return fPar->GetNeutronMultiplicity(Etot, nClusters);
}

void Neuland::RecoTDR::PrioritizeTimeWiseFirstCluster(std::vector<R3BNeulandCluster*>& clusters) const
{
    auto timewiseFirstCluster =
        std::min_element(clusters.begin(), clusters.end(), [](const R3BNeulandCluster* a, const R3BNeulandCluster* b) {
            return a->GetT() < b->GetT();
        });
    std::rotate(clusters.begin(), timewiseFirstCluster, timewiseFirstCluster + 1);
}
