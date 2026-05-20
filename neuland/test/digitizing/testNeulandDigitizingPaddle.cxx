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

#include "MockModels.h"
#include "R3BDigitizingChannel.h"
#include "R3BDigitizingPaddle.h"
#include "R3BDigitizingPaddleNeuland.h"
#include "R3BShared.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-spec-builders.h>
#include <memory>
#include <utility>
#include <vector>

/**
 * gtest for neuland digitizing modules
 * @author Yanzhao Wang
 * @since April 12, 2023
 */
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
namespace
{
    namespace Digitizing = R3B::Digitizing;
    using Channel = Digitizing::AbstractChannel;
    using Digitizing::PaddleSignal;
    using NeulandPaddle = Digitizing::Neuland::Paddle;
    using R3B::Side;
    using R3B::testing::Neuland::GMockChannel;
    // using ::testing::Return;
    template <typename Type>
    using Pair = Digitizing::LRPair<Type>;

    class testNeulandPaddle : public ::testing::Test
    {
      protected:
        testNeulandPaddle() = default;

        void SetUp() override
        {
            auto leftChannel = std::make_unique<GMockChannel>(Side::left);
            auto rightChannel = std::make_unique<GMockChannel>(Side::right);
            leftChannel->DelegateToFake();
            rightChannel->DelegateToFake();
            channels_.left = leftChannel.get();
            channels_.right = rightChannel.get();
            paddle_->SetChannel(std::move(leftChannel));
            paddle_->SetChannel(std::move(rightChannel));
        }

        // void SetExpectCall()
        // {
        //     EXPECT_CALL(*channels_.left,
        //     ConstructSignals()).Times(1).WillOnce(Return(std::move(channelSignals_.left)));
        //     EXPECT_CALL(*channels_.right, ConstructSignals())
        //         .Times(1)
        //         .WillOnce(Return(std::move(channelSignals_.right)));
        // }

        template <Side side>
        void AddChannelSignal(const double& time, const double& energy)
        {
            auto signal = Channel::Hit{};
            signal.tdc = time;
            signal.qdc = energy;
            signal.qdcUnSat = energy;
            signal.side = side;
            if constexpr (side == Side::right)
            {
                channelSignals_.right.push_back(signal);
            }
            else
            {
                channelSignals_.left.push_back(signal);
            }
        }

        template <Side side>
        auto ExtractChannelSignals()
        {
            if constexpr (side == Side::right)
            {
                return std::move(channelSignals_.right);
            }
            else
            {
                return std::move(channelSignals_.left);
            }
        }
        [[nodiscard]] auto GetChannels() const -> const auto& { return channels_; }
        [[nodiscard]] auto GetPaddle() const -> auto* { return paddle_.get(); }

      private:
        std::unique_ptr<NeulandPaddle> paddle_ = std::make_unique<NeulandPaddle>(0);
        Pair<std::vector<Channel::Hit>> channelSignals_;
        Pair<GMockChannel*> channels_ = { nullptr, nullptr };
    };

    TEST_F(testNeulandPaddle, check_inputN_to_channel)
    {
        auto* paddle = GetPaddle();
        paddle->DepositLight(PaddleSignal{ 10., 20., 0. });
        paddle->DepositLight(PaddleSignal{ 10., 20., 0. });
        paddle->DepositLight(PaddleSignal{ 20., 0., 0. });
        EXPECT_CALL(*GetChannels().left, construct_hits(testing::_)).Times(1);
        EXPECT_CALL(*GetChannels().right, construct_hits(testing::_)).Times(1);
        paddle->Construct();
        const auto& leftHits = GetChannels().left->hits_;
        const auto& rightHits = GetChannels().right->hits_;
        ASSERT_EQ(leftHits.size(), rightHits.size()) << "Numbers of hits of both PMT channels don't match!";
        ASSERT_EQ(leftHits.size(), 3) << "Number of hits for a PMT channel doesn't match with paddle inputs";
    }

    TEST_F(testNeulandPaddle, check_light_attenuation) // NOLINT
    {
        auto* paddle = GetPaddle();
        auto const inputE = 20.;
        auto paddle_signal = PaddleSignal{ 10., inputE, 0 };
        paddle->DepositLight(paddle_signal);
        EXPECT_CALL(*GetChannels().left, construct_hits(testing::_)).Times(1);
        EXPECT_CALL(*GetChannels().right, construct_hits(testing::_)).Times(1);
        paddle->Construct();
        const auto& leftHits = GetChannels().left->hits_;
        const auto& rightHits = GetChannels().right->hits_;
        ASSERT_LT(leftHits[0].intensity, inputE) << "No light attenuation for the left channel!";
        ASSERT_LT(rightHits[0].intensity, inputE) << "No light attenuation for the right channel!";
    }

    TEST_F(testNeulandPaddle, check_positive_direction) // NOLINT
    {
        auto* paddle = GetPaddle();
        auto const inputE = 20.;
        auto const initPos = 0.8 * NeulandPaddle::HALF_BAR_LENGTH;
        auto paddle_signal = PaddleSignal{ 10., inputE, initPos };
        paddle->DepositLight(paddle_signal);
        EXPECT_CALL(*GetChannels().left, construct_hits(testing::_)).Times(1);
        EXPECT_CALL(*GetChannels().right, construct_hits(testing::_)).Times(1);
        paddle->Construct();
        const auto& leftHits = GetChannels().left->hits_;
        const auto& rightHits = GetChannels().right->hits_;
        ASSERT_LT(leftHits[0].intensity, rightHits[0].intensity)
            << "left channel has larger light input with longer distance!";
        ASSERT_GT(leftHits[0].time, rightHits[0].time) << "light hit left channel ealier with longer distance!";
    }

    TEST_F(testNeulandPaddle, check_coupling_counts) // NOLINT
    {
        auto* paddle = GetPaddle();
        paddle->DepositLight(PaddleSignal{ 10., 20., 0.5 * NeulandPaddle::HALF_BAR_LENGTH });
        paddle->DepositLight(PaddleSignal{ 60., 10., -0.3 * NeulandPaddle::HALF_BAR_LENGTH });
        EXPECT_CALL(*GetChannels().left, construct_hits(testing::_)).Times(1);
        EXPECT_CALL(*GetChannels().right, construct_hits(testing::_)).Times(1);
        paddle->Construct();
        // const auto& leftHits = GetChannels().left->hits_;
        // const auto& rightHits = GetChannels().right->hits_;
        // AddChannelSignal<Side::left>(leftHits[0].time, leftHits[0].light);
        // AddChannelSignal<Side::left>(leftHits[1].time, leftHits[1].light);
        // AddChannelSignal<Side::right>(rightHits[1].time, rightHits[1].light);
        // AddChannelSignal<Side::right>(rightHits[0].time, rightHits[0].light);
        const auto& signals = paddle->GetHits();
        ASSERT_EQ(signals.size(), 2) << "Fail to reconstruct the same number of signals!";
    }

    TEST_F(testNeulandPaddle, check_output_singal) // NOLINT
    {
        auto* paddle = GetPaddle();
        paddle->DepositLight(PaddleSignal{ 10., 20., 0.5 * NeulandPaddle::HALF_BAR_LENGTH });
        EXPECT_CALL(*GetChannels().left, construct_hits(testing::_)).Times(1);
        EXPECT_CALL(*GetChannels().right, construct_hits(testing::_)).Times(1);
        paddle->Construct();
        // const auto& leftHits = GetChannels().left->hits_;
        // const auto& rightHits = GetChannels().right->hits_;
        // AddChannelSignal<Side::left>(leftHits[0].time, leftHits[0].light);
        // AddChannelSignal<Side::right>(rightHits[0].time, rightHits[0].light);
        auto const& signals = paddle->GetHits();
        ASSERT_EQ(signals.size(), 1) << "Fail to reconstruct the same number of signals!";
        const double err = 0.1 * 0.5 * NeulandPaddle::HALF_BAR_LENGTH;
        ASSERT_NEAR(0.5 * NeulandPaddle::HALF_BAR_LENGTH, signals.front().position, err)
            << "reconstructed position is way different to predefined position!";
    }

    TEST_F(testNeulandPaddle, check_reset)
    {
        auto* paddle = GetPaddle();
        paddle->DepositLight(PaddleSignal{ 10., 20., 0.5 * NeulandPaddle::HALF_BAR_LENGTH });
        EXPECT_CALL(*GetChannels().left, construct_hits(testing::_)).Times(2);
        EXPECT_CALL(*GetChannels().right, construct_hits(testing::_)).Times(2);
        paddle->Construct();
        EXPECT_EQ(paddle->GetHits().size(), 1);
        paddle->Reset();
        paddle->Construct();
        EXPECT_EQ(paddle->GetHits().size(), 0);
    }

    // TEST_F(testNeulandPaddle, check_coupling_NoEQ_counts) // NOLINT
    // {
    //     auto* paddle = GetPaddle();
    //     paddle->DepositLight(PaddleSignal{ 10., 20., 0.5 * NeulandPaddle::gHalfLength });
    //     paddle->DepositLight(PaddleSignal{ 60., 10., -0.3 * NeulandPaddle::gHalfLength });
    //     const auto& leftHits = GetChannels().left->hits_;
    //     const auto& rightHits = GetChannels().right->hits_;
    //     AddChannelSignal<Side::left>(leftHits[0].time, leftHits[0].light);
    //     AddChannelSignal<Side::left>(leftHits[1].time, leftHits[1].light);
    //     AddChannelSignal<Side::right>(rightHits[1].time, rightHits[1].light);
    //     SetExpectCall();
    //     const auto& signals = paddle->GetSignals();
    //     ASSERT_EQ(signals.size(), 1) << "inequal number of channel signals are not paired correctly!";
    // }

    // TEST_F(testNeulandPaddle, check_coupling_badParing) // NOLINT
    // {
    //     FairLogger::GetLogger()->SetLogScreenLevel("error");
    //     auto* paddle = GetPaddle();
    //     paddle->DepositLight(PaddleSignal{ 10., 20., 0.5 * NeulandPaddle::gHalfLength });
    //     paddle->DepositLight(PaddleSignal{ 60., 10., -0.3 * NeulandPaddle::gHalfLength });
    //     const auto& leftHits = GetChannels().left->hits_;
    //     const auto& rightHits = GetChannels().right->hits_;
    //     AddChannelSignal<Side::left>(leftHits[0].time, leftHits[0].light);
    //     AddChannelSignal<Side::left>(leftHits[0].time, leftHits[0].light);
    //     AddChannelSignal<Side::right>(rightHits[0].time, rightHits[0].light);
    //     AddChannelSignal<Side::right>(rightHits[1].time, rightHits[1].light);
    //     SetExpectCall();
    //     const auto& signals = paddle->GetSignals();
    //     ASSERT_EQ(signals.size(), 1) << "recursive paring is not prohibited!";
    // }
} // namespace
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
