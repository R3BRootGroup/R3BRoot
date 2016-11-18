#ifndef NEULANDRECONSTRUCTIONNEURALNETWORKMULTIPLICITY_H
#define NEULANDRECONSTRUCTIONNEURALNETWORKMULTIPLICITY_H

#include "ReconstructionEngine.h"

#include "FairMQSocketZMQ.h"
#include <string>

namespace Neuland
{
    class ReconstructionNeuralNetworkMultiplicity : public ReconstructionEngine
    {
      public:
        ReconstructionNeuralNetworkMultiplicity(const Double_t EToFMin,
                                                const Double_t EToFMax,
                                                const Double_t EDepMin,
                                                const TString server = "tcp://localhost:55555");
        ~ReconstructionNeuralNetworkMultiplicity();

        void Init() override;
        std::vector<R3BNeulandNeutron> GetNeutrons(const std::vector<R3BNeulandCluster*>&) const override;

      private:
        Double_t fEToFMin;
        Double_t fEToFMax;
        Double_t fEDepMin;
        std::string fServer;
        FairMQSocketZMQ* fSocket;
    };
}

#endif // NEULANDRECONSTRUCTIONNEURALNETWORKMULTIPLICITY_H
