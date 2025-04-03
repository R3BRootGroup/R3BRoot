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
#include "R3BShared.h"
#include <cstdint>
#include <gmock/gmock-function-mocker.h>
#include <gmock/gmock-spec-builders.h>
#include <gmock/gmock.h>
#include <vector>

/**
 * Simulation of Mock Bar/Paddle
 * @author Yanzhao Wang
 * @since April 12, 2023
 */
namespace R3B::testing::Neuland
{
    using Digitizing::AbstractChannel;
    using Digitizing::AbstractPaddle;
    using R3B::Side;

    struct GMockNeulandPaddle : public AbstractPaddle
    {
        explicit GMockNeulandPaddle(uint16_t paddleID)
            : AbstractPaddle{ paddleID }
        {
        }
        // NOLINTBEGIN(modernize-use-trailing-return-type)
        MOCK_METHOD(double,
                    compute_time,
                    (const AbstractChannel::Hit& firstSignal, const AbstractChannel::Hit& secondSignal),
                    (const, override));
        MOCK_METHOD(double,
                    compute_energy,
                    (const AbstractChannel::Hit& firstSignal, const AbstractChannel::Hit& secondSignal),
                    (const, override));
        MOCK_METHOD(double,
                    compute_position,
                    (const AbstractChannel::Hit& firstSignal, const AbstractChannel::Hit& secondSignal),
                    (const, override));
        MOCK_METHOD(Pair<AbstractChannel::Signal>, compute_channel_signals, (const Signal& hit), (const, override));
        // NOLINTEND(modernize-use-trailing-return-type)
    };

    struct GMockChannel : public AbstractChannel
    {
        explicit GMockChannel(Side side)
            : Digitizing::AbstractChannel{ side } {};
        void add_signal(Signal hit) override { hits_.push_back(hit); }

        MOCK_METHOD((void), construct_hits, (Hits&), (override));
        std::vector<Signal> hits_;

        void DelegateToFake()
        {
            ON_CALL(*this, construct_hits)
                .WillByDefault(
                    [this](Hits& signal)
                    {
                        for (const auto& hit : hits_)
                        {
                            signal.emplace_back(convert_hit_to_signal(hit));
                        }
                    });
        }
        void extra_reset() override { hits_.clear(); }

      private:
        auto convert_hit_to_signal(const Signal& hit) -> Hit
        {
            auto signal = Hit{};
            signal.qdc = hit.intensity;
            signal.qdcUnSat = hit.intensity;
            signal.tdc = hit.time;
            signal.side = GetSide();
            return signal;
        }
    };
} // namespace R3B::testing::Neuland
