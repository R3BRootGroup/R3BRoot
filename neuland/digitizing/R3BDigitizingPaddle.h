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
#include "R3BShared.h"
#include <R3BLogger.h>
#include <RtypesCore.h>
#include <functional>
#include <memory>
#include <vector>

namespace R3B::Digitizing
{

    template <typename Type>
    struct LRPair
    {
        Type left{};
        Type right{};
        LRPair(Type p_left, Type p_right)
            : left(p_left)
            , right(p_right)
        {
        }
        LRPair() = default;
    };

    struct PaddleHit
    {
        double energy{};   //!< The energy of the paddle hit [MeV].
        double time{};     //!< The time of the paddle hit [ns].
        double position{}; //!< The distance to the center point of the paddle [cm].
        const AbstractChannel::Hit* left_channel_hit = nullptr;        //!< Reference to the left channel hit.
        const AbstractChannel::Hit* right_channel_hit = nullptr;       //!< Reference to the right channel hit.
        const AbstractChannel::CalSignal* left_channel_cal = nullptr;  //!< Reference to the left channel cal.
        const AbstractChannel::CalSignal* right_channel_cal = nullptr; //!< Reference to the right channel cal.

        PaddleHit() = default;

        explicit PaddleHit(LRPair<const AbstractChannel::CalSignal*> p_signals)
            : left_channel_cal{ p_signals.left }
            , right_channel_cal{ p_signals.right }
        {
        }

        explicit PaddleHit(LRPair<const AbstractChannel::Hit*> p_signals)
            : left_channel_hit{ p_signals.left }
            , right_channel_hit{ p_signals.right }
        {
        }
    };

    struct PaddleSignal
    {
        double time;               //<! Time of the energy depostion [ns]
        double energy_dep;         //<! Energy depostion [MeV]
        double distance_to_center; //<! Distance to the center point of the paddle [cm]
    };

    class AbstractPaddle
    {
      public:
        template <typename Type>
        using Pair = LRPair<Type>;
        using Hit = PaddleHit;
        using Signal = PaddleSignal;
        using Hits = std::vector<Hit>;
        using ChannelSignalPair = Pair<std::reference_wrapper<const AbstractChannel::Hit>>;
        using SignalCouplingStrategy = std::function<std::vector<ChannelSignalPair>(const AbstractPaddle&,
                                                                                    const AbstractChannel::Hits&,
                                                                                    const AbstractChannel::Hits&)>;

        explicit AbstractPaddle(int paddleID, SignalCouplingStrategy strategy = SignalCouplingByTime);
        [[nodiscard]] auto HasFired() const -> bool;
        [[nodiscard]] auto HasHalfFired() const -> bool;
        void Construct();
        void Reset();

        // rule of 5
        virtual ~AbstractPaddle() = default;
        AbstractPaddle(const AbstractPaddle& other) = delete;
        auto operator=(const AbstractPaddle& other) -> AbstractPaddle& = delete;
        AbstractPaddle(AbstractPaddle&& other) = default;
        auto operator=(AbstractPaddle&& other) -> AbstractPaddle& = delete;

        void DepositLight(const Signal& signal);

        void SetChannel(std::unique_ptr<AbstractChannel> channel);

        void SetSignalCouplingStrategy(const SignalCouplingStrategy& strategy) { signal_coupling_strategy_ = strategy; }

        // Getters:
        [[nodiscard]] auto GetPaddleID() const -> int { return paddle_id_; }
        [[nodiscard]] auto GetHits() const -> const std::vector<Hit>& { return signal_hits_; }
        [[nodiscard]] auto GetSignalCouplingStragtegy() const -> const SignalCouplingStrategy&
        {
            return signal_coupling_strategy_;
        }
        [[nodiscard]] auto GetLeftChannel() const -> const AbstractChannel* { return left_channel_.get(); }
        [[nodiscard]] auto GetLeftChannelRef() const -> auto& { return *left_channel_; }
        [[nodiscard]] auto GetChannel(R3B::Side side) const -> const Digitizing::AbstractChannel&;
        [[nodiscard]] auto GetRightChannelRef() const -> auto& { return *right_channel_; }
        [[nodiscard]] auto GetRightChannel() const -> const AbstractChannel* { return right_channel_.get(); }
        [[nodiscard]] auto GetTrigTime() const -> double;

        // Setters:
        void SetPaddleID(int id) { paddle_id_ = id; }

        [[nodiscard]] virtual auto match_hits(const AbstractChannel::Hit& /*firstSignal*/,
                                              const AbstractChannel::Hit& /*secondSignal*/) const -> float
        {
            return 0.;
        }

      private:
        int paddle_id_{};
        Hits signal_hits_;
        std::unique_ptr<AbstractChannel> left_channel_{};
        std::unique_ptr<AbstractChannel> right_channel_{};
        SignalCouplingStrategy signal_coupling_strategy_;

        // virtual private functions
        virtual void extra_reset() {}
        virtual void pre_construct() {}
        [[nodiscard]] virtual auto compute_time(const AbstractChannel::Hit& firstSignal,
                                                const AbstractChannel::Hit& secondSignal) const -> double = 0;
        [[nodiscard]] virtual auto compute_energy(const AbstractChannel::Hit& firstSignal,
                                                  const AbstractChannel::Hit& secondSignal) const -> double = 0;
        [[nodiscard]] virtual auto compute_position(const AbstractChannel::Hit& rightSignal,
                                                    const AbstractChannel::Hit& leftSignal) const -> double = 0;
        [[nodiscard]] virtual auto compute_channel_signals(const Signal& hit) const
            -> Pair<AbstractChannel::Signal> = 0;

        // non-virtual private functions
        void construct_paddle_signals(Hits& paddle_signals,
                                      const AbstractChannel::Hits& firstSignals,
                                      const AbstractChannel::Hits& secondSignals) const;

      public:
        static auto SignalCouplingByTime(const AbstractPaddle& self,
                                         const AbstractChannel::Hits& firstSignals,
                                         const AbstractChannel::Hits& secondSignals) -> std::vector<ChannelSignalPair>;
    };
} // namespace R3B::Digitizing
#endif
