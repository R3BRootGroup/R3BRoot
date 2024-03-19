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
#include "gtest/gtest.h"

namespace
{
    namespace Digitizing = R3B::Digitizing;
    using TmxChannel = Digitizing::Neuland::Tamex::Channel;
    using FQTPeak = Digitizing::Neuland::Tamex::FQTPeak;
    using PMTPeak = Digitizing::Neuland::Tamex::PMTPeak;
    using Channel = Digitizing::Channel;
    using TmxPar = Digitizing::Neuland::Tamex::Params;

    class testNeulandTamexChannel : public ::testing::Test
    {
      protected:
        testNeulandTamexChannel()
            : fChannel{ std::make_unique<TmxChannel>(Digitizing::ChannelSide::left) }
        {
            SetChannelPar(fChannel->GetPar());
        }

        void SetUp() override {}

        [[nodiscard]] auto GetChannel() const -> TmxChannel* { return fChannel.get(); }

        [[nodiscard]] auto GetPar() const -> TmxPar& { return fChannel->GetPar(); }

        static void SetChannelPar(TmxPar& par)
        {
            par.fPMTThresh = 1.;
            par.fSaturationCoefficient = 0.012;
            par.fExperimentalDataIsCorrectedForSaturation = true;
            par.fEnergyGain = 15.0;
            par.fPedestal = 14.0;
            par.fTimeMax = 1000.;
            par.fTimeMin = 1.;
            par.fQdcMin = 0.67;
        }

        void AddHit(double time, double light) { fChannel->AddHit({ time, light }); }

        [[nodiscard]] auto GetSignals() const -> const Channel::Signals& { return fChannel->GetSignals(); }
        [[nodiscard]] auto GetPMTPeaks() const -> const std::vector<PMTPeak>& { return fChannel->GetPMTPeaks(); }
        [[nodiscard]] auto GetPeaks() const -> const std::vector<FQTPeak>& { return fChannel->GetFQTPeaks(); }

      private:
        std::unique_ptr<TmxChannel> fChannel = nullptr;
    };

    TEST_F(testNeulandTamexChannel, basic_hit_processing) // NOLINT
    {
        AddHit(20., 20.);
        auto signals = GetSignals();
        auto peaks = GetPeaks();

        // check whether signal got lost
        ASSERT_EQ(peaks.size(), 1) << "no peak is generated!";
        ASSERT_EQ(signals.size(), 1) << "No channel signal is outputted!";

        // check PMT saturation
        ASSERT_LT(peaks[0].GetQDC(), 20.) << "PMT saturation is not implemented!";
        ASSERT_NE(signals[0].tdc, 20.) << "tdc value is not smeared!";
    }

    TEST_F(testNeulandTamexChannel, pmt_threshold_check) // NOLINT
    {
        AddHit(20., 0.5);
        auto signals = GetSignals();
        auto peaks = GetPeaks();
        ASSERT_EQ(signals.size(), 0) << "PMT threshold doesn't filter out low energy signals!";
    }

    TEST_F(testNeulandTamexChannel, pmt_threshold_overlap) // NOLINT
    {
        AddHit(20., 0.5);
        AddHit(20., 0.5);
        AddHit(20., 0.5);
        AddHit(20., 0.5);
        auto signals = GetSignals();
        auto peaks = GetPeaks();
        ASSERT_EQ(signals.size(), 1) << "overlapped signals cannot pass PMT threshold!";
    }

    TEST_F(testNeulandTamexChannel, ifno_timeRes_check) // NOLINT
    {
        decltype(auto) par = GetPar();
        par.fTimeRes = 0.0;

        AddHit(20., 20.);
        const auto& signals = GetSignals();
        ASSERT_DOUBLE_EQ(signals[0].tdc, 20.) << "tdc value is not correctly passed on!";
    }

    TEST_F(testNeulandTamexChannel, signal_pileup_check) // NOLINT
    {
        AddHit(20., 20.);
        AddHit(20., 1.5);
        ASSERT_EQ(GetSignals().size(), 1) << "overlapping failed!";

        auto width = GetPeaks().back().GetWidth();
        AddHit(width + 21., 1.5);
        ASSERT_EQ(GetSignals().size(), 2) << "should not be overlapped!";
    }

    // TODO: this test is highly depenedent on how signals are piled up. Therefore, it's better to have different tests
    // for different pileup strategy.
    TEST_F(testNeulandTamexChannel, signal_multiPileup_check) // NOLINT
    {
        AddHit(20., 5.);
        ASSERT_EQ(GetSignals().size(), 1);
        AddHit(GetPeaks().back().GetTETime() + 5., 5.);
        ASSERT_EQ(GetSignals().size(), 2);
        AddHit(GetPeaks().back().GetTETime() + 5., 5.);
        ASSERT_EQ(GetSignals().size(), 3);
        AddHit(GetPeaks().back().GetTETime() + 5., 5.);
        ASSERT_EQ(GetSignals().size(), 4);
        AddHit(GetPeaks().back().GetTETime() + 5., 5.);
        ASSERT_EQ(GetSignals().size(), 5);
        auto minTime = GetPeaks().front().GetLETime();
        auto maxTime = GetPeaks().back().GetTETime();
        auto qdc_test = FQTPeak::WidthToQdc(maxTime - minTime + 10., GetPar());
        AddHit(minTime - 5., qdc_test);
        ASSERT_EQ(GetSignals().size(), 1);
    }

    constexpr auto AssessReduction(double inE, double outE) { return (inE - outE) / inE; }

    TEST_F(testNeulandTamexChannel, PMT_saturation_check) // NOLINT
    {
        const auto lowE = 10.;
        const auto highE = 40.;

        AddHit(20., lowE);
        AddHit(200., highE);
        const auto& pmtSignals = GetPMTPeaks();
        ASSERT_EQ(pmtSignals.size(), 2);
        const auto lowE_out = pmtSignals[0].GetQDC();
        const auto highE_out = pmtSignals[1].GetQDC();
        ASSERT_LT(AssessReduction(lowE, lowE_out), AssessReduction(highE, highE_out)) << "PMT saturation not applied!";
    }
} // namespace
