#ifndef NEULANDRECONSTRUCTIONENGINEH
#define NEULANDRECONSTRUCTIONENGINEH

#include "R3BNeulandCluster.h"
#include "R3BNeulandNeutron.h"
#include <vector>

namespace Neuland
{
    class ReconstructionEngine
    {
      public:
        virtual ~ReconstructionEngine() = default; // FIXME: Root doesn't like = 0?
        virtual void Init() {}
        virtual std::vector<R3BNeulandNeutron> GetNeutrons(const std::vector<R3BNeulandCluster*>&) const = 0;
    };
}

#endif // NEULANDRECONSTRUCTIONENGINEH
