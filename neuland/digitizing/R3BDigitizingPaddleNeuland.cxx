#include "R3BDigitizingPaddleNeuland.h"
#include <R3BNeulandCalToHitPar.h>
#include <R3BNeulandCommon.h>
#include <cmath>

namespace R3B::Digitizing::Neuland
{
    static const uint8_t DEFAULT_ITERATION = 8U;
    static auto CheckMatchValidity(const std::vector<Paddle::ChannelSignalPair>& matchedPairs,
                                   const Channel::Signal& signal) -> bool;

    const double NeulandPaddle::ReverseAttenFac = std::exp(NeulandPaddle::gHalfLength * NeulandPaddle::gAttenuation);

    template <uint8_t iterations = DEFAULT_ITERATION>
    auto FastExp(const Float_t val) -> Float_t
    {
        auto exp = 1.F + val / (iterations >> 1U);
        for (auto i = 0; i < iterations; ++i)
        {
            exp *= exp;
        }
        return exp;
    }

    NeulandPaddle::NeulandPaddle(uint16_t paddleID)
        : Digitizing::Paddle(paddleID, SignalCouplingNeuland)
    {
        LOG(debug) << "NeulandPaddle: Using constructor 1" << std::endl;
    }

    NeulandPaddle::NeulandPaddle(uint16_t paddleID, R3B::Neuland::Cal2HitPar* cal_to_hit_par)
        : Digitizing::Paddle(paddleID, SignalCouplingNeuland)
    {
        LOG(debug) << "NeulandPaddle: Using constructor 2" << std::endl;
        const auto& module_par = cal_to_hit_par->GetModulePars().at(paddleID);
        effective_speed_ = module_par.effectiveSpeed.value;
        gAttenuation_ = module_par.lightAttenuationFactor.value;
        time_offset_ = module_par.tDiff.value;
        time_sync_ = module_par.tSync.value;
    }

    auto NeulandPaddle::MatchSignals(const Channel::Signal& firstSignal,
                                     const Channel::Signal& secondSignal) const -> float
    {
        auto firstE = static_cast<Float_t>(firstSignal.qdcUnSat);
        auto secondE = static_cast<Float_t>(secondSignal.qdcUnSat);
        auto firstT = firstSignal.tdc;
        auto secondT = secondSignal.tdc;

        // keep the exponent always negative to prevent the exploding of exponential function
        auto res = 0.F;
        if (firstT > secondT)
        {
            res = std::abs((firstE / secondE) *
                               FastExp<4>(static_cast<Float_t>(gAttenuation_ * effective_speed_ * (firstT - secondT))) -
                           1);
        }
        else
        {
            res =
                std::abs((secondE / firstE) * FastExp<4>(static_cast<Float_t>(gAttenuation_ * effective_speed_ *
                                                                              static_cast<Float_t>(secondT - firstT))) -
                         1);
        }
        return res;
    }

    inline auto NeulandPaddle::ComputeEnergy(const Channel::Signal& firstSignal,
                                             const Channel::Signal& secondSignal) const -> double
    {
        return std::sqrt(firstSignal.qdcUnSat * secondSignal.qdcUnSat) * ReverseAttenFac_;
    }

    inline auto NeulandPaddle::ComputeTime(const Channel::Signal& firstSignal,
                                           const Channel::Signal& secondSignal) const -> double
    {
        //LOG(info) << "ComputeTime: using eff_speed:" << effective_speed_ << std::endl;
        return (firstSignal.tdc + secondSignal.tdc) / 2 - gHalfLength_ / effective_speed_ - time_sync_;
    }

    inline auto NeulandPaddle::ComputePosition(const Channel::Signal& leftSignal,
                                               const Channel::Signal& rightSignal) const -> double
    {
        if (leftSignal.side == rightSignal.side)
        {
            R3BLOG(fatal, "cannot compute position with signals from same side!");
            return 0.F;
        }

        return (leftSignal.side == ChannelSide::left)
                   ? (leftSignal.tdc - rightSignal.tdc + time_offset_) / 2 * effective_speed_
                   : (rightSignal.tdc - leftSignal.tdc + time_offset_) / 2 * effective_speed_;
    }

    auto NeulandPaddle::ComputeChannelHits(const Hit& hit) const -> Paddle::Pair<Channel::Hit>
    {
        auto channel_side_right = ChannelSide{ ChannelSide::right };
        auto channel_side_left = ChannelSide{ ChannelSide::left };
        auto rightChannelHit = GenerateChannelHit(hit.time, hit.LightDep, hit.DistToPaddleCenter, channel_side_right);
        auto leftChannelHit =
            GenerateChannelHit(hit.time, hit.LightDep, -1 * hit.DistToPaddleCenter, channel_side_left);
        return { leftChannelHit, rightChannelHit };
    }

    auto NeulandPaddle::GenerateChannelHit(const double mcTime,
                                           const double mcLight,
                                           const double dist,
                                           enum ChannelSide channel_side) const -> Channel::Hit
    {
        auto light = double{ mcLight * std::exp(-NeulandPaddle::gAttenuation_ * (NeulandPaddle::gHalfLength_ - dist)) };
        int site_sign = (channel_side == ChannelSide::right) ? 1 : -1;

        auto time = double{ mcTime + (NeulandPaddle::gHalfLength_ - dist) / effective_speed_ +
                            site_sign * time_offset_ * 0.5 + time_sync_ };

        return { time, light };
    }

    auto NeulandPaddle::SignalCouplingNeuland(const Paddle& self,
                                              const Channel::Signals& firstSignals,
                                              const Channel::Signals& secondSignals) -> std::vector<ChannelSignalPair>
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
                                           { return (self.MatchSignals(it, left) < self.MatchSignals(it, right)); });
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
