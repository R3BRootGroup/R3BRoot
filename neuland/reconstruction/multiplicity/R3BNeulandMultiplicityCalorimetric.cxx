#include "R3BNeulandMultiplicityCalorimetric.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRtdbRun.h"
#include "FairRuntimeDb.h"
#include <numeric>
#include <utility>

R3BNeulandMultiplicityCalorimetric::R3BNeulandMultiplicityCalorimetric(std::string_view input, std::string_view output)
    : FairTask("R3BNeulandMultiplicityCalorimetric")
    , fClusters(input)
    , fMultiplicity(std::make_unique<R3BNeulandMultiplicity>())
    , multiplicity_par_ptr{ fMultiplicity.get() }
    , fOutputName(output)
    , fPar(nullptr)
{
}

auto R3BNeulandMultiplicityCalorimetric::Init() -> InitStatus
{
    // Input
    fClusters.init();

    // Output
    auto* ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        throw R3B::runtime_error("TCAInputConnector: No FairRootManager");
    }
    ioman->RegisterAny(fOutputName, multiplicity_par_ptr, true);

    // Parameter
    auto* rtdb = FairRuntimeDb::instance();
    if (rtdb == nullptr)
    {
        throw R3B::runtime_error("R3BNeulandMultiplicityCalorimetric::Init: No FairRuntimeDb!");
    }
    fPar = dynamic_cast<R3BNeulandMultiplicityCalorimetricPar*>(
        rtdb->getContainer("R3BNeulandMultiplicityCalorimetricPar"));
    if (fPar == nullptr)
    {
        throw R3B::runtime_error("R3BNeulandMultiplicityCalorimetric::Init: No R3BNeulandMultiplicityCalorimetricPar!");
    }
    // FIXME: FairRuntimeDB needs to be forced to load the Data from the second file with Run Id 1
    rtdb->initContainers(rtdb->getCurrentRun()->getRunId(), 1);

    return kSUCCESS;
}

void R3BNeulandMultiplicityCalorimetric::Exec(Option_t* /*option*/)
{
    fMultiplicity->m.fill(0.);

    const auto& clusters = fClusters.get();
    const auto Etot =
        std::accumulate(clusters.cbegin(),
                        clusters.cend(),
                        0.,
                        [](const double sum, const R3BNeulandCluster& cluster) { return sum + cluster.GetE(); });
    const auto nClusters = clusters.size();

    // Calorimetric Multiplicity is One-Hot
    fMultiplicity->m.at(fPar->GetNeutronMultiplicity(Etot, static_cast<double>(nClusters))) = 1.;
}

ClassImp(R3BNeulandMultiplicityCalorimetric)
