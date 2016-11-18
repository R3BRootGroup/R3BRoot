#ifndef NEULANDRECONSTRUCTION2DCALIBRH
#define NEULANDRECONSTRUCTION2DCALIBRH

#include "ReconstructionEngine.h"
class R3BNeulandNeutron2DPar;

namespace Neuland
{
    class Reconstruction2DCalibr : public ReconstructionEngine
    {
      public:
        Reconstruction2DCalibr(const Double_t EToFMin = 0.,
                               const Double_t EToFMax = 100000.,
                               const Double_t EDepMin = 0.);

        void Init() override;
        std::vector<R3BNeulandNeutron> GetNeutrons(const std::vector<R3BNeulandCluster*>&) const override;

      private:
        Double_t fEToFMin;
        Double_t fEToFMax;
        Double_t fEDepMin;
        R3BNeulandNeutron2DPar* fPar;
    };
}

#endif // NEULANDRECONSTRUCTION2DCALIBRH
