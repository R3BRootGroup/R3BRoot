#ifndef R3BROOT_LIKELIHOOD_H
#define R3BROOT_LIKELIHOOD_H

#include <map>
#include <string>

namespace Neuland
{
    class Likelihood
    {
      public:
        using E = double;
        using H = int;

        explicit Likelihood(const std::string& filename);

        double P(E e, H h) const;

      private:
        std::string fFileName;
        std::map<E, std::map<H, double>> fData;
    };
} // namespace Neuland

#endif // R3BROOT_LIKELIHOOD_H
