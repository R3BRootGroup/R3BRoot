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
#pragma once
#include "R3BDigitizingChannel.h"
#include "R3BDigitizingPaddle.h"
#include "R3BNeulandCommon.h"
#include "R3BShared.h"
#include <cmath>
#include <vector>

/**
 * Simulation of NeuLAND Bar/Paddle
 * @author Yanzhao Wang
 * @since July 15, 2022
 */

namespace R3B::Neuland
{
    class Cal2HitPar;
}

namespace R3B::Digitizing::Neuland
{
    class Paddle : public Digitizing::AbstractPaddle
    {
      public:
        explicit Paddle(int paddle_id);
        explicit Paddle(int paddle_id, R3B::Neuland::Cal2HitPar* cal_to_hit_par);

        [[nodiscard]] auto match_hits(const AbstractChannel::Hit& firstSignal,
                                      const AbstractChannel::Hit& secondSignal) const -> float override;
        [[nodiscard]] static auto HitCouplingNeuland(const AbstractPaddle& self,
                                                     const AbstractChannel::Hits& firstSignals,
                                                     const AbstractChannel::Hits& secondSignals)
            -> std::vector<ChannelSignalPair>;
        [[nodiscard]] auto GenerateChannelSignal(double mcTime,
                                                 double mcLight,
                                                 double dist,
                                                 enum Side channel_side) const -> AbstractChannel::Signal;

        static constexpr double HALF_BAR_LENGTH = 135.;      // [cm]
        static constexpr double DEFAULT_ATTENUATION = 0.008; // light attenuation of plastic scintillator [1/cm]

      private:
        double attenuation_ = DEFAULT_ATTENUATION;
        double reverse_atten_fac_ = std::exp(HALF_BAR_LENGTH * DEFAULT_ATTENUATION);
        double effective_speed_ = R3B::Neuland::Calibration::DEFAULT_EFFECTIVE_C;
        double time_offset_ = 0.0;
        double time_sync_ = 0.0;

        R3B::Neuland::Cal2HitPar* cal_to_hit_par_ = nullptr;

        void pre_construct() override;

        [[nodiscard]] auto compute_time(const AbstractChannel::Hit& firstSignal,
                                        const AbstractChannel::Hit& secondSignal) const -> double override;
        [[nodiscard]] auto compute_energy(const AbstractChannel::Hit& firstSignal,
                                          const AbstractChannel::Hit& secondSignal) const -> double override;
        [[nodiscard]] auto compute_position(const AbstractChannel::Hit& leftSignal,
                                            const AbstractChannel::Hit& rightSignal) const -> double override;
        [[nodiscard]] auto compute_channel_signals(const Signal& signal) const
            -> Pair<AbstractChannel::Signal> override;
    };
} // namespace R3B::Digitizing::Neuland
