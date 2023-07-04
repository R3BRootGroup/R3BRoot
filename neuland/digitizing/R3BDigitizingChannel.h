/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#pragma once
#include "Validated.h"
#include <TRandom3.h>
#include <vector>

namespace R3B::Digitizing
{
    class Paddle;
    const double MAXTIME = 1e100;

    enum class ChannelSide
    {
        right = 1,
        left = 2
    };
    // output data structure of channel
    struct ChannelSignal
    {
        double qdc{};
        double tdc{ MAXTIME };
        double qdcUnSat{};
        ChannelSide side{};
        ChannelSignal() = default;
    };

    // input data structure of channel
    struct ChannelHit
    {
        double time{};
        double light{};
        ChannelHit() = default;
        ChannelHit(double p_time, double p_light)
            : time{ p_time }
            , light{ p_light }
        {
        }
        auto operator<(const ChannelHit& rhs) const -> bool { return (time < rhs.time); }
    };

    class Channel
    {
      public:
        using Signal = ChannelSignal;
        using Hit = ChannelHit;
        using Signals = std::vector<Signal>;

        explicit Channel(ChannelSide);

        // rule of 5
        virtual ~Channel() = default; // FIXME: Root doesn't like pure virtual destructors (= 0;)
        Channel(const Channel& other) = delete;
        auto operator=(const Channel& other) -> Channel& = delete;
        Channel(Channel&& other) = default;
        auto operator=(Channel&& other) -> Channel& = delete;

        virtual void AddHit(Hit hit) = 0;
        virtual auto HasFired() -> bool;

        // Getters:
        virtual auto GetTrigTime() -> double;
        auto GetSignals() -> const Signals&;
        auto GetSide() const -> ChannelSide { return fSide; }
        auto GetPaddle() const -> Paddle* { return fPaddle; }

        void SetPaddle(Paddle* v_paddle) { fPaddle = v_paddle; }
        auto Is_ValidSignals() -> bool { return fSignals.valid(); }
        void InvalidateSignals() { fSignals.invalidate(); }
        void InvalidateTrigTime() { fTrigTime.invalidate(); }
        virtual void AttachToPaddle(Paddle* paddle) { fPaddle = paddle; };
        static auto GetDefaultRandomGen() -> TRandom3&;

      private:
        virtual auto ConstructSignals() -> Signals = 0;
        Paddle* fPaddle = nullptr;           // pointer to the paddle who owns this channel
        ChannelSide fSide;                   // side of the channel
        mutable Validated<Signals> fSignals; // output signals from the channel
        mutable Validated<double> fTrigTime;
    };
} // namespace R3B::Digitizing
