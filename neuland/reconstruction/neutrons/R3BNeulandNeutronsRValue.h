#pragma once

#include "FairTask.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandMultiplicity.h"
#include "R3BNeulandNeutron.h"
#include <R3BIOConnector.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <string>
#include <string_view>
#include <vector>

class R3BNeulandNeutronsRValue : public FairTask
{
  public:
    explicit R3BNeulandNeutronsRValue(double EkinRefMeV,
                                      std::string_view inputMult = "NeulandMultiplicity",
                                      std::string_view inputCluster = "NeulandClusters",
                                      std::string_view output = "NeulandNeutrons");

  private:
    double fEkinRefMeV;
    std::string fInputMultName;
    const R3BNeulandMultiplicity* fMultiplicity;             //!
    R3B::InputVectorConnector<R3BNeulandCluster> fClusters;  //!
    R3B::OutputVectorConnector<R3BNeulandNeutron> fNeutrons; //!
    std::vector<R3BNeulandCluster> cluster_buffer_;

    auto Init() -> InitStatus override;
    void Exec(Option_t* /*option*/) override;

    void SortClustersByRValue(std::vector<R3BNeulandCluster>&) const;
    void FilterClustersByKineticEnergy(std::vector<R3BNeulandCluster>&) const;

    static void PrioritizeTimeWiseFirstCluster(std::vector<R3BNeulandCluster>&);
    static void FilterClustersByEnergyDeposit(std::vector<R3BNeulandCluster>&);
    static void FilterClustersByElasticScattering(std::vector<R3BNeulandCluster>&);

    ClassDefOverride(R3BNeulandNeutronsRValue, 1);
};
