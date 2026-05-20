/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BDigitizingPaddle.h"
#include "R3BDigitizingChannel.h"
#include "R3BShared.h"
#include <algorithm>
#include <fairlogger/Logger.h>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace R3B::Digitizing
{
    AbstractPaddle::AbstractPaddle(int paddleID, SignalCouplingStrategy strategy)
        : paddle_id_{ paddleID }
        , signal_coupling_strategy_{ std::move(strategy) }
    {
    }

    void AbstractPaddle::SetChannel(std::unique_ptr<AbstractChannel> channel)
    {
        channel->SetPaddle(this);
        channel->AttachToPaddle(this);
        if (channel->GetSide() == Side::left)
        {
            left_channel_ = std::move(channel);
        }
        else if (channel->GetSide() == Side::right)
        {
            right_channel_ = std::move(channel);
        }
        else
        {
            LOG(error) << "Channel side is invalid!";
        }
    }

    auto AbstractPaddle::GetChannel(R3B::Side side) const -> const Digitizing::AbstractChannel&
    {
        if (side == Side::left)
        {
            return *left_channel_;
        }
        return *right_channel_;
    }

    void AbstractPaddle::DepositLight(const Signal& signal)
    {
        auto channelHits = compute_channel_signals(signal);
        left_channel_->AddSignal(channelHits.left);
        right_channel_->AddSignal(channelHits.right);
    }

    auto AbstractPaddle::HasFired() const -> bool
    {
        if (left_channel_ == nullptr || right_channel_ == nullptr)
        {
            LOG(fatal) << "channels failed to be constructed when checking fire! ";
            return false;
        }
        return (left_channel_->HasFired() && right_channel_->HasFired());
    }

    auto AbstractPaddle::HasHalfFired() const -> bool
    {
        return (left_channel_->HasFired() && !right_channel_->HasFired()) ||
               (!left_channel_->HasFired() && right_channel_->HasFired());
    }

    void AbstractPaddle::construct_paddle_signals(Hits& paddle_signals,
                                                  const AbstractChannel::Hits& firstSignals,
                                                  const AbstractChannel::Hits& secondSignals) const
    {
        auto channelSignalPairs = signal_coupling_strategy_(*this, firstSignals, secondSignals);

        auto paddleSignals = std::vector<Hit>();
        paddleSignals.reserve(channelSignalPairs.size());

        for (auto& it : channelSignalPairs)
        {
            // warning if two channel signals are from the same side
            if (it.left.get().side == it.right.get().side)
            {
                LOG(warn) << "DigitizingEngine: ConstructPaddleSignals(): cannot construct the paddle signal from the "
                             "channels on the same side!";
            }

            // swap the channel signals if necessary
            if (it.left.get().side != Side::left)
            {
                std::swap(it.left, it.right);
            }
            auto paddleSignal = Hit{ LRPair{ &(it.left.get()), &(it.right.get()) } };

            paddleSignal.energy = compute_energy(it.left, it.right);
            paddleSignal.time = compute_time(it.left, it.right);
            paddleSignal.position = compute_position(it.left, it.right);
            paddle_signals.push_back(paddleSignal);
        }
    }

    void AbstractPaddle::Construct()
    {
        signal_hits_.clear();
        pre_construct();
        left_channel_->Construct();
        right_channel_->Construct();
        if (HasFired())
        {
            construct_paddle_signals(signal_hits_, left_channel_->GetHits(), right_channel_->GetHits());
        }
    }

    void AbstractPaddle::Reset()
    {
        signal_hits_.clear();
        left_channel_->Reset();
        right_channel_->Reset();
        paddle_id_ = -1;
    }

    auto AbstractPaddle::SignalCouplingByTime(const AbstractPaddle& /*self*/,
                                              const AbstractChannel::Hits& firstSignals,
                                              const AbstractChannel::Hits& secondSignals)
        -> std::vector<ChannelSignalPair>
    {
        auto firstSignalRefs =
            std::vector<std::reference_wrapper<const AbstractChannel::Hit>>(firstSignals.begin(), firstSignals.end());
        auto secondSignalRefs =
            std::vector<std::reference_wrapper<const AbstractChannel::Hit>>(secondSignals.begin(), secondSignals.end());
        auto channelSignalPairs = std::vector<ChannelSignalPair>{};
        channelSignalPairs.reserve(std::min(firstSignals.size(), secondSignals.size()));

        auto sortSignalByTime = [](auto signals)
        {
            std::sort(signals.begin(),
                      signals.end(),
                      [](auto& first, auto& second) { return first.get().tdc < second.get().tdc; });
        };
        sortSignalByTime(firstSignalRefs);
        sortSignalByTime(secondSignalRefs);

        for (auto first_it = firstSignalRefs.begin(), second_it = secondSignalRefs.begin();
             first_it < firstSignalRefs.end() && second_it < secondSignalRefs.end();
             ++first_it, ++second_it)
        {
            channelSignalPairs.emplace_back(*first_it, *second_it);
        }
        return channelSignalPairs;
    }

    auto AbstractPaddle::GetTrigTime() const -> double
    {
        return std::min(right_channel_->GetTrigTime(), left_channel_->GetTrigTime());
    }

} // namespace R3B::Digitizing
