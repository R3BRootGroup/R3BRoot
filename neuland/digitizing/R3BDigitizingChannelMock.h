#pragma once
#include "R3BDigitizingChannel.h"
#include "R3BShared.h"

namespace R3B::Digitizing::Neuland
{
    class MockChannel : public Digitizing::AbstractChannel
    {
      public:
        explicit MockChannel(Side side)
            : Digitizing::AbstractChannel{ side } {};
        void add_signal(Signal newHit) override
        {
            auto signal = Hit{};
            signal.qdc = newHit.intensity;
            signal.qdcUnSat = newHit.intensity;
            signal.tdc = newHit.time;
            signal.side = GetSide();
            signals_.emplace_back(signal);
        }

      private:
        Hits signals_;
        void construct_hits(Hits& signals) override { signals = signals_; }
        void extra_reset() override { signals_.clear(); }
    };
} // namespace R3B::Digitizing::Neuland
