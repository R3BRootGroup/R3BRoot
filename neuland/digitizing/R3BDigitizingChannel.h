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

#pragma once
#include "R3BShared.h"
#include <TRandom3.h>
#include <fmt/core.h>
#include <vector>

namespace R3B::Digitizing
{
    class AbstractPaddle;
    const double MAXTIME = 1e100;

    // output data structure of channel
    struct ChannelHit
    {
        double qdc{};
        double tdc{ MAXTIME };
        double qdcUnSat{};
        R3B::Side side{};
        ChannelHit() = default;
    };

    // output data structure for Cal Data
    struct ChannelCalSignal
    {
        double tot{};
        double tle{};
        R3B::Side side{};
        ChannelCalSignal() = default;
    };

    // input data structure of channel
    struct ChannelSignal
    {
        double time{};      //!< Time value of the channel signal.
        double intensity{}; //!< Intensity of the channel signal.

        /**
         * @brief Default constructor.
         */
        ChannelSignal() = default;

        /**
         * @brief Constructor with two input values
         *
         * @param time_val Time value of the channel.
         * @return intensity_val Intensity value of the channel.
         */
        ChannelSignal(double time_val, double intensity_val)
            : time{ time_val }
            , intensity{ intensity_val }
        {
        }

        /**
         * \brief Less operator between two channel signals.
         *
         * The channel signal is considered smaller if its time value is smaller.
         * @param rhs The other signal on the right side of the operator.
         * @return Ture if the time of the left side is smaller.
         */
        auto operator<(const ChannelSignal& rhs) const -> bool { return (time < rhs.time); }
    };

    class AbstractChannel
    {
      public:
        using Hit = ChannelHit;
        using CalSignal = ChannelCalSignal;
        using Signal = ChannelSignal;
        using Hits = std::vector<Hit>;
        using CalSignals = std::vector<CalSignal>;

        explicit AbstractChannel(R3B::Side side, bool has_cal_output = false);

        // rule of 5
        virtual ~AbstractChannel() = default;
        AbstractChannel(const AbstractChannel& other) = delete;
        auto operator=(const AbstractChannel& other) -> AbstractChannel& = delete;
        AbstractChannel(AbstractChannel&& other) = default;
        auto operator=(AbstractChannel&& other) -> AbstractChannel& = delete;

        void Construct();

        /**
         * \brief Reset the data in the Channel for the new event.
         *
         * Resetting the private members including signals_, cal_signals_ and trig_time_. This reset function also calls
         * the virtual function extra_reset, which is implemented in the child classes. The side_ and paddle_ member
         * variables are not reset.
         */
        void Reset();

        void AddSignal(const Signal& signal)
        {
            ++signal_size_;
            add_signal(signal);
        }
        virtual auto HasFired() -> bool { return not hits_.empty(); }

        void EnableCal(bool is_enabled = true) { has_cal_output_ = is_enabled; }

        // Getters:
        [[nodiscard]] auto GetSide() const -> R3B::Side { return side_; }
        [[nodiscard]] auto GetPaddle() const -> AbstractPaddle* { return paddle_; }
        [[nodiscard]] auto GetSignalSize() const -> int { return signal_size_; }

        [[nodiscard]] auto GetCalSignals() const -> const CalSignals& { return cal_signals_; }
        [[nodiscard]] auto GetHits() const -> const Hits& { return hits_; }
        [[nodiscard]] auto GetTrigTime() const -> double { return trig_time_; }

        void SetPaddle(AbstractPaddle* v_paddle) { paddle_ = v_paddle; }
        void AttachToPaddle(AbstractPaddle* paddle) { paddle_ = paddle; };
        static auto GetDefaultRandomGen() -> TRandom3&;

      private:
        bool has_cal_output_ = false;      //!< Flag of whether cal output is enabled
        R3B::Side side_;                   //!< side of the channel
        double trig_time_ = 0.;            //!< trigger time of the channel
        int signal_size_{};                //!< Size of the signal inputs
        AbstractPaddle* paddle_ = nullptr; //!< pointer to the paddle who owns this channel
        Hits hits_;                        //!< output hits from the channel
        CalSignals cal_signals_;           //!< output cal signals from the channel

        virtual void construct_hits(Hits& signals) = 0;
        virtual void construct_cal_signals(CalSignals& cal_signals) const {};
        virtual void extra_reset() {}
        virtual void pre_construct() {}
        virtual void add_signal(Signal signal) = 0;

        void calculate_trig_time();
    };
} // namespace R3B::Digitizing

template <>
class fmt::formatter<R3B::Digitizing::ChannelHit>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::Digitizing::ChannelHit& signal, FmtContent& ctn) const
    {
        return fmt::format_to(ctn.out(),
                              "{{qdc: {}, tdc: {}, adcUnSat: {}, side: {}}}",
                              signal.qdc,
                              signal.tdc,
                              signal.qdcUnSat,
                              signal.side);
    }
};

template <>
class fmt::formatter<R3B::Digitizing::ChannelCalSignal>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::Digitizing::ChannelCalSignal& signal, FmtContent& ctn) const
    {
        return fmt::format_to(
            ctn.out(), "{{ToT: {}, Leading edge: {}, side: {}}}", signal.tot, signal.tle, signal.side);
    }
};

template <>
class fmt::formatter<R3B::Digitizing::ChannelSignal>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::Digitizing::ChannelSignal& hit, FmtContent& ctn) const
    {
        return fmt::format_to(ctn.out(), "{{time: {} ns, light dep: {} MeV}}", hit.time, hit.intensity);
    }
};
