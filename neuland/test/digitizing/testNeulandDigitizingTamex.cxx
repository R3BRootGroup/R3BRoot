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

#include "R3BDigitizingChannel.h"
#include "R3BDigitizingTamex.h"
#include "R3BShared.h"
#include "gtest/gtest.h"
#include <memory>
#include <vector>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
namespace
{
    namespace Digitizing = R3B::Digitizing;
    using TmxChannel = Digitizing::Neuland::Tamex::Channel;
    using FQTPeak = Digitizing::Neuland::Tamex::FQTPeak;
    using PMTPeak = Digitizing::Neuland::Tamex::PMTPeak;
    using Channel = Digitizing::AbstractChannel;
    using TmxPar = Digitizing::Neuland::Tamex::Params;

    class testNeulandTamexChannel : public ::testing::Test
    {
      protected:
        testNeulandTamexChannel()
            : channel_{ std::make_unique<TmxChannel>(R3B::Side::left) }
        {
            auto par = channel_->GetPar();
            SetChannelPar(par);
            channel_->SetPar(par);
        }

        void SetUp() override {}

        void SetPar(const TmxPar& par) { channel_->SetPar(par); }

        void Construct() { channel_->Construct(); }

        [[nodiscard]] auto GetChannel() const -> TmxChannel* { return channel_.get(); }

        [[nodiscard]] auto GetPar() const -> const TmxPar& { return channel_->GetPar(); }

        static void SetChannelPar(TmxPar& par)
        {
            par.pmt_thresh = 1.;
            par.saturation_coefficient = 0.012;
            par.experimental_data_is_corrected_for_saturation = true;
            par.energy_gain = 15.0;
            par.pedestal = 14.0;
            par.max_time = 1000.;
            par.min_time = 1.;
            par.min_energy = 0.67;
        }

        void AddSignal(double time, double light) { channel_->add_signal({ time, light }); }

        [[nodiscard]] auto GetSignals() const -> const Channel::Hits& { return channel_->GetHits(); }
        [[nodiscard]] auto GetPMTPeaks() const -> const std::vector<PMTPeak>& { return channel_->GetPMTPeaks(); }
        [[nodiscard]] auto GetPeaks() const -> const std::vector<FQTPeak>& { return channel_->GetFQTPeaks(); }

      private:
        std::unique_ptr<TmxChannel> channel_ = nullptr;
    };

    TEST_F(testNeulandTamexChannel, basic_hit_processing) // NOLINT
    {
        AddSignal(20., 20.);
        Construct();

        auto signals = GetSignals();
        auto peaks = GetPeaks();

        // check whether signal got lost
        ASSERT_EQ(peaks.size(), 1) << "no peak is generated!";
        ASSERT_EQ(signals.size(), 1) << "No channel signal is outputted!";

        // check PMT saturation
        ASSERT_LT(peaks[0].GetEnergy(), 20.) << "PMT saturation is not implemented!";
        ASSERT_NE(signals[0].tdc, 20.) << "tdc value is not smeared!";
    }

    TEST_F(testNeulandTamexChannel, pmt_threshold_check) // NOLINT
    {
        AddSignal(20., 0.5);
        Construct();
        auto signals = GetSignals();
        auto peaks = GetPeaks();
        ASSERT_EQ(signals.size(), 0) << "PMT threshold doesn't filter out low energy signals!";
    }

    TEST_F(testNeulandTamexChannel, pmt_threshold_overlap) // NOLINT
    {
        AddSignal(20., 0.5);
        AddSignal(20., 0.5);
        AddSignal(20., 0.5);
        AddSignal(20., 0.5);
        Construct();
        auto signals = GetSignals();
        auto peaks = GetPeaks();
        ASSERT_EQ(signals.size(), 1) << "overlapped signals cannot pass PMT threshold!";
    }

    TEST_F(testNeulandTamexChannel, ifno_timeRes_check) // NOLINT
    {
        auto par = GetPar();
        par.time_res = 0.0;
        SetPar(par);

        AddSignal(20., 20.);
        Construct();
        const auto& signals = GetSignals();
        ASSERT_DOUBLE_EQ(signals[0].tdc, 20.) << "tdc value is not correctly passed on!";
    }

    TEST_F(testNeulandTamexChannel, signal_pileup_check) // NOLINT
    {
        AddSignal(20., 20.);
        AddSignal(20., 1.5);
        Construct();
        ASSERT_EQ(GetSignals().size(), 1) << "overlapping failed!";

        auto width = GetPeaks().back().GetToT();
        AddSignal(width + 21., 1.5);
        Construct();
        ASSERT_EQ(GetSignals().size(), 2) << "should not be overlapped!";
    }

    // TODO: this test is highly dependent on how signals are piled up. Therefore, it's better to have different tests
    // for different pileup strategy.
    TEST_F(testNeulandTamexChannel, signal_multiPileup_check) // NOLINT
    {
        AddSignal(20., 5.);
        Construct();
        ASSERT_EQ(GetSignals().size(), 1);
        AddSignal(GetPeaks().back().GetTETime() + 5., 5.);
        Construct();
        ASSERT_EQ(GetSignals().size(), 2);
        AddSignal(GetPeaks().back().GetTETime() + 5., 5.);
        Construct();
        ASSERT_EQ(GetSignals().size(), 3);
        AddSignal(GetPeaks().back().GetTETime() + 5., 5.);
        Construct();
        ASSERT_EQ(GetSignals().size(), 4);
        AddSignal(GetPeaks().back().GetTETime() + 5., 5.);
        Construct();
        ASSERT_EQ(GetSignals().size(), 5);
        auto minTime = GetPeaks().front().GetLETime();
        auto maxTime = GetPeaks().back().GetTETime();
        auto qdc_test = FQTPeak::ToT2Energy(maxTime - minTime + 10., GetPar());
        AddSignal(minTime - 5., qdc_test);
        Construct();
        ASSERT_EQ(GetSignals().size(), 1);
    }

    constexpr auto AssessReduction(double inE, double outE) { return (inE - outE) / inE; }

    TEST_F(testNeulandTamexChannel, PMT_saturation_check) // NOLINT
    {
        const auto lowE = 10.;
        const auto highE = 40.;

        AddSignal(20., lowE);
        AddSignal(200., highE);
        Construct();
        const auto& pmtSignals = GetPMTPeaks();
        ASSERT_EQ(pmtSignals.size(), 2);
        const auto lowE_out = pmtSignals[0].GetHeight();
        const auto highE_out = pmtSignals[1].GetHeight();
        ASSERT_LT(AssessReduction(lowE, lowE_out), AssessReduction(highE, highE_out)) << "PMT saturation not applied!";
    }
} // namespace
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
