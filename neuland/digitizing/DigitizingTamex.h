#ifndef R3BROOT_DIGITIZINGTAMEX_H
#define R3BROOT_DIGITIZINGTAMEX_H

#include "DigitizingEngine.h"

namespace Neuland
{
    namespace Tamex
    {
        class Channel : public Digitizing::Channel
        {
          public:
            Channel() = default;
            ~Channel() override = default;
            void AddHit(Double_t mcTime, Double_t mcLight, Double_t dist) override;
            bool HasFired() const override;
            Double_t GetQDC() const override;
            Double_t GetTDC() const override;
            Double_t GetEnergy() const override;
        };

    } // namespace Tamex

    class DigitizingTamex : public DigitizingEngine
    {
      public:
        DigitizingTamex() = default;
        ~DigitizingTamex() override = default;
        std::unique_ptr<Digitizing::Channel> BuildChannel() override;
    };
} // namespace Neuland

#endif // R3BROOT_DIGITIZINGTAMEX_H
