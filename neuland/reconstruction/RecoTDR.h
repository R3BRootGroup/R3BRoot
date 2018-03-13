#ifndef NEULAND_RECOTDR_H
#define NEULAND_RECOTDR_H

#include "ReconstructionEngine.h"
#include <functional>

class R3BNeulandNeutron2DPar;

namespace Neuland
{
    class RecoTDR : public ReconstructionEngine
    {
      public:
        RecoTDR();
        void Init() override;
        R3BNeulandNeutron2DPar* Get2DPar() { return fPar; }
        std::vector<R3BNeulandNeutron> GetNeutrons(const std::vector<R3BNeulandCluster*>&) const override;
        void SetBeam(Double_t EBeamMeV, Double_t BetaBeam);
        void SortClustersByRValue(std::vector<R3BNeulandCluster*>&) const;
        void FilterClustersByEnergyDeposit(std::vector<R3BNeulandCluster*>&) const;
        void FilterClustersByBeta(std::vector<R3BNeulandCluster*>&) const;
        void FilterClustersByElasticScattering(std::vector<R3BNeulandCluster*>&) const;
        void PrioritizeTimeWiseFirstCluster(std::vector<R3BNeulandCluster*>&) const;
        UInt_t FindNumberOfNeutrons(std::vector<R3BNeulandCluster*>&) const;

      private:
        R3BNeulandNeutron2DPar* fPar; // non-owning
        Double_t fEBeamMeV;
        Double_t fBetaBeam;
        Double_t fClusterMinE;
    };
}

#endif // NEULAND_RECOTDR_H
