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
/**
 * NeuLAND Tamex digitizing module with multiple hits per event
 * @author Yanzhao Wang
 * @since July 15, 2022
 *
 *
 */

#include "R3BDigitizingChannel.h"
#include "R3BDigitizingPaddle.h"
#include "R3BShared.h"
#include "TRandom3.h"
#include <R3BNeulandCalToHitPar.h>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <vector>

class R3BNeulandHitPar;
class R3BNeulandHitModulePar;

namespace R3B::Digitizing::Neuland::Tamex
{
    class Channel;
    struct Params
    {
        // NOLINTBEGIN
        bool experimental_data_is_corrected_for_saturation = true; //!< Flag if saturation effect enabled
        double pmt_thresh = 1.;                                    //!< [MeV]
        double saturation_coefficient = 0.012;                     //!< Saturation coefficient of PMTs
        double time_res = 0.15;                                    //!< time + Gaus(0., fTimeRes) [ns]
        double energy_res_rel = 0.05;                              //!< Gaus(e, fEResRel * e) []
        double energy_gain = 15.0;                                 //!< Energy gain
        double pedestal = 14.0;                                    //!< Energy offset parameter [ns]
        double max_time = 1000.;                                   //!< [ns]
        double min_time = 1.;                                      //!< [ns]
        double pileup_time_window = 1000.;                         //!< [ns]
        double pileup_distance = 100.;                             //!< [ns]
        double min_energy = 0.067;                                 //!< minimal energy of a FQT peak [MeV]
        std::reference_wrapper<TRandom3> rnd_gen;                  //!< Reference to shared random generator
        // NOLINTEND

        explicit Params(TRandom3&);
    };

    enum class PeakPileUpStrategy : uint8_t
    {
        width,
        distance,
        time_window
    };

    class PMTPeak
    {
      public:
        PMTPeak() = default;
        PMTPeak(Digitizing::AbstractChannel::Signal channel_signal, const Channel&);
        auto operator<(const PMTPeak& rhs) const -> bool { return (time_ < rhs.time_); }
        auto operator==(const PMTPeak& rhs) const -> bool { return std::abs(time_ - rhs.time_) < peakWidth; }
        auto operator+=(const PMTPeak& other) -> PMTPeak&;
        [[nodiscard]] auto GetHeight() const -> double { return height_; }
        [[nodiscard]] auto GetLETime() const -> double { return time_; }
        static constexpr double peakWidth = 15.0; // ns

      private:
        double height_ = 0.0;
        double time_ = 0.0;
    };

    class FQTPeak
    {
      public:
        FQTPeak(const PMTPeak& pmtPeak, Channel* channel);
        FQTPeak() = default;

        // Getters:
        [[nodiscard]] auto GetToT() const -> double { return time_over_thresh_; }
        [[nodiscard]] auto GetEnergy() const -> double { return energy_; }
        [[nodiscard]] auto GetLETime() const -> double { return leading_edge_time_; }
        [[nodiscard]] auto GetTETime() const -> double { return trailing_edge_time_; }

        auto operator==(const FQTPeak& other) const -> bool;
        void operator+=(const FQTPeak& other);
        auto operator-(const FQTPeak& other) const -> double { return leading_edge_time_ - other.leading_edge_time_; }
        auto operator-(double time) const -> double { return leading_edge_time_ - time; };
        auto operator>(const FQTPeak& other) const -> bool { return leading_edge_time_ - other.leading_edge_time_ > 0; }
        auto operator<(const FQTPeak& other) const -> bool { return leading_edge_time_ - other.leading_edge_time_ < 0; }

        void AddEnergy(double energy) { energy_ += energy; }

        static auto ToT2Energy(double width, const Params& par) -> double
        {
            return std::max(1., width - par.pedestal) / par.energy_gain;
        }

        static auto Energy2ToT(double height, const Params& par) -> double
        {
            auto time_over_thresh = 0.0;
            if (height > par.min_energy)
            {
                time_over_thresh = height * par.energy_gain + par.pedestal;
            }
            else
            {
                time_over_thresh = height * par.energy_gain * (par.pedestal + 1);
            }
            return time_over_thresh;
        }
        explicit operator Digitizing::AbstractChannel::Hit() const;

      private:
        double time_over_thresh_ = 0.0;   //<! The temperal time-over-thresh of the TmxPeak in [ns]
        double energy_ = 0.0;             //<! The energy value of the FQT signal [MeV] (without threshold)
        double leading_edge_time_ = 0.0;  //<! Leading edge of the TmxPeak in [ns]
        double trailing_edge_time_ = 0.0; //<! Tailing edge of the TmxPeak
        Channel* channel_ptr_ = nullptr;  //<! Pointer to the channel which the peak belongs to
    };

    class Channel : public Digitizing::AbstractChannel
    {
      public:
        Channel(Side, PeakPileUpStrategy strategy, TRandom3&);
        Channel(Side,
                PeakPileUpStrategy strategy,
                const Params&,
                R3B::Neuland::Cal2HitPar* cal_to_hit_par = nullptr,
                bool has_cal_output = false);
        explicit Channel(Side side, PeakPileUpStrategy strategy = PeakPileUpStrategy::width)
            : Channel(side, strategy, GetDefaultRandomGen())
        {
        }
        // Setters:
        void SetPileUpStrategy(PeakPileUpStrategy strategy) { pileup_strategy_ = strategy; }
        void SetPar(const Tamex::Params& par) { par_ = par; }

        // Getters:
        [[nodiscard]] auto GetPar() const -> const Tamex::Params& { return par_; }
        [[nodiscard]] auto GetFQTPeaks() const -> const std::vector<FQTPeak>& { return fqt_peaks_; }
        [[nodiscard]] auto GetPMTPeaks() const -> const std::vector<PMTPeak>& { return pmt_peaks_; }
        auto GetCal2HitPar() -> auto* { return neuland_hit_par_; }

        void add_signal(Signal /*signal*/) override;
        [[nodiscard]] auto CreateHit(const FQTPeak& peak) const -> Hit;
        [[nodiscard]] auto CreateCalSignal(const FQTPeak& peak) const -> CalSignal;

      private:
        PeakPileUpStrategy pileup_strategy_ = PeakPileUpStrategy::width;
        std::vector<PMTPeak> pmt_peaks_;
        std::vector<FQTPeak> fqt_peaks_;
        R3B::Neuland::Cal2HitPar* neuland_hit_par_ = nullptr;
        Tamex::Params par_;

        // private virtual functions
        void construct_hits(Hits& hits) override;
        void construct_cal_signals(CalSignals& cal_signals) const override;
        void extra_reset() override;
        void pre_construct() override;

        // private non-virtual functions
        void set_hit_module_par(int PaddleId);
        [[nodiscard]] auto check_paddle_id_in_hit_par() const -> bool;
        [[nodiscard]] auto smear_energy(double) const -> double;
        [[nodiscard]] auto smear_time(double) const -> double;
        [[nodiscard]] auto to_unsat_energy(double) const -> double;
        [[nodiscard]] auto calculate_ToT(double energy) const -> double;
        template <typename Peak>
        void apply_threshold(/* inout */ std::vector<Peak>&);
        void construct_FQT_peaks(std::vector<FQTPeak>& FQTPeaks, std::vector<PMTPeak>& pmtPeaks);
        template <typename Peak>
        static void do_peak_pileup(/* inout */ std::vector<Peak>& peaks);

        static void peak_pileup_with_distance(/* inout */ std::vector<FQTPeak>& peaks, double distance);
        static void peak_pileup_in_time_window(/* inout */ std::vector<FQTPeak>& peaks, double time_window);
        void fqt_peak_pileup(/* inout */ std::vector<FQTPeak>& peaks);
    };

} // namespace R3B::Digitizing::Neuland::Tamex
