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

#ifndef DIGITIZING_PADDLE_H
#define DIGITIZING_PADDLE_H

#include "R3BDigitizingChannel.h"
#include <R3BLogger.h>
#include <RtypesCore.h>
#include <functional>
#include <memory>

namespace R3B::Digitizing
{

    template <typename Type>
    struct LRPair
    {
        Type left{};
        Type right{};
        LRPair(Type p_left, Type p_right)
            : left(p_left)
            , right(p_right){};
        LRPair() = default;
    };

    struct PaddleSignal
    {
        double energy{};
        double time{};
        double position{};
        const Channel::Signal& leftChannel;
        const Channel::Signal& rightChannel;
        explicit PaddleSignal(LRPair<const Channel::Signal&> p_signals)
            : leftChannel{ p_signals.left }
            , rightChannel{ p_signals.right }
        {
        }
    };

    struct PaddleHit
    {
        double time;               // ns
        double LightDep;           // MeV
        double DistToPaddleCenter; // cm
    };

    class Paddle
    {
      public:
        template <typename Type>
        using Pair = LRPair<Type>;
        using Signal = PaddleSignal;
        using Hit = PaddleHit;
        using Signals = std::vector<Signal>;
        using ChannelSignalPair = Pair<std::reference_wrapper<const Channel::Signal>>;
        using SignalCouplingStrategy =
            std::function<std::vector<ChannelSignalPair>(const Channel::Signals&, const Channel::Signals&)>;

        explicit Paddle(int paddleID, SignalCouplingStrategy strategy = SignalCouplingByTime);
        auto HasFired() const -> bool;
        auto HasHalfFired() const -> bool;

        // rule of 5
        virtual ~Paddle() = default;
        Paddle(const Paddle& other) = delete;
        auto operator=(const Paddle& other) -> Paddle& = delete;
        Paddle(Paddle&& other) = default;
        auto operator=(Paddle&& other) -> Paddle& = delete;

        void DepositLight(const Hit& hit);

        void SetChannel(std::unique_ptr<Channel> channel);
        void SetSignalCouplingStrategy(const SignalCouplingStrategy& strategy) { fSignalCouplingStrategy = strategy; }

        // Getters:
        auto GetPaddleID() const -> int { return fPaddleID; }
        auto GetSignals() const -> const std::vector<Signal>&;
        auto GetSignalCouplingStragtegy() const -> const SignalCouplingStrategy& { return fSignalCouplingStrategy; }
        auto GetLeftChannel() const -> const Channel* { return fLeftChannel.get(); }
        auto& GetLeftChannelRef() { return *fLeftChannel; }
        auto& GetRightChannelRef() { return *fRightChannel; }
        auto GetRightChannel() const -> const Channel* { return fRightChannel.get(); }
        auto GetTrigTime() const -> double;

      private:
        mutable Validated<Signals> fSignals;
        const int fPaddleID;
        std::unique_ptr<Channel> fLeftChannel{};
        std::unique_ptr<Channel> fRightChannel{};
        SignalCouplingStrategy fSignalCouplingStrategy;
        // virtual std::function<indexMapFunc> IndexMapFunc() const { return ConstructIndexMapByTime; }
        virtual auto ConstructPaddelSignals(const Channel::Signals& firstSignals,
                                            const Channel::Signals& secondSignals) const -> Signals;
        virtual auto ComputeTime(const Channel::Signal& firstSignal, const Channel::Signal& secondSignal) const
            -> double = 0;
        virtual auto ComputeEnergy(const Channel::Signal& firstSignal, const Channel::Signal& secondSignal) const
            -> double = 0;
        virtual auto ComputePosition(const Channel::Signal& rightSignal, const Channel::Signal& leftSignal) const
            -> double = 0;
        virtual auto ComputeChannelHits(const Hit& hit) const -> Pair<Channel::Hit> = 0;

      public:
        static auto SignalCouplingByTime(const Channel::Signals& firstSignals, const Channel::Signals& secondSignals)
            -> std::vector<ChannelSignalPair>;
    };
} // namespace R3B::Digitizing
#endif
