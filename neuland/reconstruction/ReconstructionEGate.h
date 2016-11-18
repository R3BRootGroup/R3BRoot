#ifndef NEULANDRECONSTRUCTIONEGATEH
#define NEULANDRECONSTRUCTIONEGATEH

#include "ReconstructionEngine.h"

namespace Neuland
{
    class ReconstructionEGate : public ReconstructionEngine
    {
      public:
        ReconstructionEGate(const Double_t EToFMin, const Double_t EToFMax, const Double_t EDepMin);

        void Init() override;
        std::vector<R3BNeulandNeutron> GetNeutrons(const std::vector<R3BNeulandCluster*>&) const override;

      private:
        Double_t fEToFMin;
        Double_t fEToFMax;
        Double_t fEDepMin;
    };
}

#endif // NEULANDRECONSTRUCTIONEGATEH
