#include "R3BDigitizingPaddleNeuland.h"
#include "R3BDigitizingChannel.h"
#include "R3BDigitizingPaddle.h"
#include "R3BLogger.h"
#include "R3BShared.h"
#include <R3BNeulandCalToHitPar.h>
#include <R3BNeulandCommon.h>
#include <RtypesCore.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fairlogger/Logger.h>
#include <utility>
#include <vector>

namespace R3B::Digitizing::Neuland
{
    namespace
    {
        const uint8_t DEFAULT_ITERATION = 8U;
        // check if a signal is matched to two or more signals. If so, discard the lastest match.
        auto CheckMatchValidity(const std::vector<AbstractPaddle::ChannelSignalPair>& matchedPairs,
                                const AbstractChannel::Hit& signal) -> bool
        {
            auto is_valid = true;
            auto it_existed = find_if(matchedPairs.begin(),
                                      matchedPairs.end(),
                                      [&signal](const auto& pair) -> bool { return &(pair.right.get()) == &(signal); });
            if (it_existed != matchedPairs.end())
            {
                R3BLOG(debug, "one signal is matched again to another signal! The signal is discarded.");
                is_valid = false;
            }
            return is_valid;
        }

        template <uint8_t iterations = DEFAULT_ITERATION>
        auto FastExp(const Float_t val) -> Float_t
        {
            auto exp = 1.F + (val / (iterations >> 1U));
            for (auto i = 0; i < iterations; ++i)
            {
                exp *= exp;
            }
            return exp;
        }
    } // namespace

    Paddle::Paddle(int paddle_id)
        : Digitizing::AbstractPaddle(paddle_id, HitCouplingNeuland)
    {
    }

    Paddle::Paddle(int paddle_id, R3B::Neuland::Cal2HitPar* cal_to_hit_par)
        : Digitizing::AbstractPaddle(paddle_id, HitCouplingNeuland)
        , cal_to_hit_par_{ cal_to_hit_par }
    {
    }

    auto Paddle::match_hits(const AbstractChannel::Hit& firstSignal,
                            const AbstractChannel::Hit& secondSignal) const -> float
    {
        auto firstE = static_cast<float>(firstSignal.qdcUnSat);
        auto secondE = static_cast<float>(secondSignal.qdcUnSat);
        auto firstT = firstSignal.tdc;
        auto secondT = secondSignal.tdc;

        // keep the exponent always negative to prevent the exploding of exponential function
        auto res = 0.F;
        if (firstT > secondT)
        {
            res = std::abs(((firstE / secondE) *
                            FastExp<4>(static_cast<float>(attenuation_ * effective_speed_ * (firstT - secondT)))) -
                           1);
        }
        else
        {
            res = std::abs(((secondE / firstE) * FastExp<4>(static_cast<float>(attenuation_ * effective_speed_ *
                                                                               static_cast<float>(secondT - firstT)))) -
                           1);
        }
        return res;
    }

    void Paddle::pre_construct()
    {
        if (cal_to_hit_par_ == nullptr)
        {
            return;
        }
        const auto& module_par = cal_to_hit_par_->GetModulePars().at(GetPaddleID());
        effective_speed_ = module_par.effective_speed.value;
        attenuation_ = 1. / module_par.light_attenuation_length.value;
        time_offset_ = module_par.t_diff.value;
        time_sync_ = module_par.t_sync.value;
        reverse_atten_fac_ = std::exp(Paddle::HALF_BAR_LENGTH * attenuation_);
    }

    inline auto Paddle::compute_energy(const AbstractChannel::Hit& firstSignal,
                                       const AbstractChannel::Hit& secondSignal) const -> double
    {
        return std::sqrt(firstSignal.qdcUnSat * secondSignal.qdcUnSat) * reverse_atten_fac_;
    }

    inline auto Paddle::compute_time(const AbstractChannel::Hit& firstSignal,
                                     const AbstractChannel::Hit& secondSignal) const -> double
    {
        // LOG(info) << "ComputeTime: using eff_speed:" << effective_speed_ << std::endl;
        return ((firstSignal.tdc + secondSignal.tdc) / 2) - (HALF_BAR_LENGTH / effective_speed_) - time_sync_;
    }

    inline auto Paddle::compute_position(const AbstractChannel::Hit& leftSignal,
                                         const AbstractChannel::Hit& rightSignal) const -> double
    {
        if (leftSignal.side == rightSignal.side)
        {
            R3BLOG(fatal, "cannot compute position with signals from same side!");
            return 0.F;
        }

        return (leftSignal.side == Side::left)
                   ? (leftSignal.tdc - rightSignal.tdc + time_offset_) / 2 * effective_speed_
                   : (rightSignal.tdc - leftSignal.tdc + time_offset_) / 2 * effective_speed_;
    }

    auto Paddle::compute_channel_signals(const Signal& signal) const -> AbstractPaddle::Pair<AbstractChannel::Signal>
    {
        auto channel_side_right = Side{ Side::right };
        auto channel_side_left = Side{ Side::left };
        auto rightChannelHit =
            GenerateChannelSignal(signal.time, signal.energy_dep, signal.distance_to_center, channel_side_right);
        auto leftChannelHit =
            GenerateChannelSignal(signal.time, signal.energy_dep, -1 * signal.distance_to_center, channel_side_left);
        return { leftChannelHit, rightChannelHit };
    }

    auto Paddle::GenerateChannelSignal(const double mcTime,
                                       const double mcLight,
                                       const double dist,
                                       enum Side channel_side) const -> AbstractChannel::Signal
    {
        const auto light = mcLight * std::exp(-Paddle::attenuation_ * (Paddle::HALF_BAR_LENGTH - dist));
        const auto site_sign = (channel_side == Side::right) ? 1 : -1;

        const auto time = mcTime + ((Paddle::HALF_BAR_LENGTH - dist) / effective_speed_) +
                          (site_sign * time_offset_ * 0.5) + time_sync_;

        return { time, light };
    }

    auto Paddle::HitCouplingNeuland(const AbstractPaddle& self,
                                    const AbstractChannel::Hits& firstSignals,
                                    const AbstractChannel::Hits& secondSignals) -> std::vector<ChannelSignalPair>
    {
        // step1: determine the signals with smaller size:
        decltype(auto) smallerSizeSignals = (firstSignals.size() < secondSignals.size()) ? firstSignals : secondSignals;
        decltype(auto) largerSizeSignals = (firstSignals.size() >= secondSignals.size()) ? firstSignals : secondSignals;

        // create empty pairs to be filled:
        auto channelPairs = std::vector<ChannelSignalPair>{};
        channelPairs.reserve(smallerSizeSignals.size());

        // step2: signal matching:
        for (const auto& it : smallerSizeSignals)
        {
            // find the element from largerSizeSignals with minimum matching value
            auto it_min = std::min_element(largerSizeSignals.begin(),
                                           largerSizeSignals.end(),
                                           [&it = std::as_const(it), &self](const auto& left, const auto& right) -> bool
                                           { return (self.match_hits(it, left) < self.match_hits(it, right)); });
            if (it_min == largerSizeSignals.end())
            {
                LOG(warn) << "DigitizingPaddleNeuland.cxx::SignalCouplingNeuland(): failed to find minimum value!";
            }
            else
            {
                if (CheckMatchValidity(channelPairs, *it_min))
                {
                    channelPairs.emplace_back(it, *it_min);
                }
            }
        }
        // step3: output pairs
        return channelPairs;
    }

    // check if a signal is matched to two or more signals. If so, discard the latest match.
    static auto CheckMatchValidity(const std::vector<Paddle::ChannelSignalPair>& matchedPairs,
                                   const Channel::Signal& signal) -> bool
    {
        auto is_valid = true;
        auto it_existed = find_if(matchedPairs.begin(),
                                  matchedPairs.end(),
                                  [&signal](const auto& pair) -> bool { return &(pair.right.get()) == &(signal); });
        if (it_existed != matchedPairs.end())
        {
            LOG(debug) << "DigitizingPaddleNeuland.cxx::CheckMatchValidity(): one signal is matched again to another "
                          "signal! The signal is discarded.";
            is_valid = false;
        }
        return is_valid;
    }

} // namespace R3B::Digitizing::Neuland
