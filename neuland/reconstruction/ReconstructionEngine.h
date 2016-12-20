#ifndef NEULANDRECONSTRUCTIONENGINEH
#define NEULANDRECONSTRUCTIONENGINEH

#include <vector>
#include "R3BNeulandNeutron.h"
#include "R3BNeulandCluster.h"

namespace Neuland
{
    // abstract class, cannot be instantiated, but can be used as a base class.
    class ReconstructionEngine
    {
      public:
        virtual void Init() = 0;
        virtual std::vector<R3BNeulandNeutron> GetNeutrons(const std::vector<R3BNeulandCluster*>&) const = 0;

        virtual ~ReconstructionEngine() {}
    };
}

#endif // NEULANDRECONSTRUCTIONENGINEH
