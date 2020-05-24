#ifndef R3BROOT_R3BNEULANDNEUTRONSRVALUE_H
#define R3BROOT_R3BNEULANDNEUTRONSRVALUE_H

#include "FairTask.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandMultiplicity.h"
#include "R3BNeulandNeutron.h"
#include "TCAConnector.h"

class R3BNeulandNeutronsRValue : public FairTask
{
  public:
    R3BNeulandNeutronsRValue(double EkinRefMeV,
                             TString inputMult = "NeulandMultiplicity",
                             TString inputCluster = "NeulandClusters",
                             TString output = "NeulandNeutrons");
    ~R3BNeulandNeutronsRValue() override = default;
    void Exec(Option_t*) override;

  protected:
    InitStatus Init() override;

  private:
    const double fEkinRefMeV;
    const TString fInputMultName;
    const R3BNeulandMultiplicity* fMultiplicity;     //!
    TCAInputConnector<R3BNeulandCluster> fClusters;  //!
    TCAOutputConnector<R3BNeulandNeutron> fNeutrons; //!

    void SortClustersByRValue(std::vector<R3BNeulandCluster*>&) const;
    void PrioritizeTimeWiseFirstCluster(std::vector<R3BNeulandCluster*>&) const;
    void FilterClustersByEnergyDeposit(std::vector<R3BNeulandCluster*>&) const;
    void FilterClustersByKineticEnergy(std::vector<R3BNeulandCluster*>&) const;
    void FilterClustersByElasticScattering(std::vector<R3BNeulandCluster*>&) const;

    ClassDefOverride(R3BNeulandNeutronsRValue, 0)
};

#endif // R3BROOT_R3BNEULANDNEUTRONSRVALUE_H
