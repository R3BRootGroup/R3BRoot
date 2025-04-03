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

#include "R3BDigitizingTamex.h"

#include <FairRuntimeDb.h>
#include <TRandom3.h>
#include <cmath>

#include "R3BDigitizingChannel.h"
#include "R3BDigitizingPaddle.h"
#include "R3BException.h"
#include "R3BNeulandCalToHitPar.h"
#include "R3BShared.h"
#include <FairRunAna.h>
#include <R3BLogger.h>
#include <algorithm>
#include <cstddef>
#include <fairlogger/Logger.h>
#include <functional>
#include <vector>

namespace R3B::Digitizing::Neuland::Tamex
{
    // some declarations for static functions:
    namespace
    {
        template <class T>
        auto CheckOverlapping(const T& peak, std::vector<T>& peaks) -> decltype(peaks.begin());
        template <class T>
        void ReOverlapping(typename std::vector<T>::iterator v_iter, std::vector<T>& peaks);
        template <class T>
        void RemovePeakAt(typename std::vector<T>::iterator v_iter, std::vector<T>& peaks);

        void set_par_with_hit_module_par(Tamex::Params& par,
                                         const R3B::Neuland::HitModulePar& module_par,
                                         Side channel_side)
        {
            auto side = (channel_side == Side::right) ? Side::right : Side::left;

            par.saturation_coefficient = module_par.pmt_saturation.get(side).value;
            par.energy_gain = module_par.energy_gain.get(side).value;
            par.pedestal = module_par.pedestal.get(side).value;
            par.pmt_thresh = module_par.pmt_threshold.get(side).value;
            par.min_energy = 1 / par.energy_gain;

            // TODO: Add other parameters:
        }
    } // namespace

    // global variables for default options:
    const size_t TmxPeaksInitialCapacity = 10;

    Params::Params(TRandom3& rnd)
        : rnd_gen{ rnd }
    {
    }

    PMTPeak::PMTPeak(Digitizing::AbstractChannel::Signal channel_signal, const Channel& channel)
        : time_(channel_signal.time)
    {
        const auto& par = channel.GetPar();
        // apply saturation coefficent
        height_ = channel_signal.intensity / (1. + par.saturation_coefficient * channel_signal.intensity);
    };

    auto PMTPeak::operator+=(const PMTPeak& other) -> PMTPeak&
    {
        height_ += other.height_;
        time_ = (time_ < other.time_) ? time_ : other.time_;
        return *this;
    }

    FQTPeak::FQTPeak(const PMTPeak& pmtPeak, Channel* channel)
        : energy_(pmtPeak.GetHeight())
        , leading_edge_time_(pmtPeak.GetLETime())
        , channel_ptr_(channel)
    {
        if (channel_ptr_ == nullptr)
        {
            LOG(fatal) << "channel is not bound to FQTPeak object!";
        }
        const auto& par = channel->GetPar();

        // calculate the time and the width of the signal
        time_over_thresh_ = Energy2ToT(energy_, par);
        trailing_edge_time_ = leading_edge_time_ + time_over_thresh_;
    }

    auto FQTPeak::operator==(const FQTPeak& other) const -> bool
    {
        if (other.leading_edge_time_ == 0 && leading_edge_time_ == 0)
        {
            LOG(warn) << "the times of both PMT signals are 0!";
        }
        return (leading_edge_time_ <= (other.leading_edge_time_ + other.time_over_thresh_)) &&
               (other.leading_edge_time_ <= (leading_edge_time_ + time_over_thresh_));
    }

    void FQTPeak::operator+=(const FQTPeak& other)
    {
        if (channel_ptr_ == nullptr)
        {
            throw R3B::logic_error("channel is not bound to FQTPeak object!");
        }
        leading_edge_time_ =
            (leading_edge_time_ < other.leading_edge_time_) ? leading_edge_time_ : other.leading_edge_time_;
        trailing_edge_time_ =
            (trailing_edge_time_ > other.trailing_edge_time_) ? trailing_edge_time_ : other.trailing_edge_time_;
        time_over_thresh_ = trailing_edge_time_ - leading_edge_time_;
        energy_ = ToT2Energy(time_over_thresh_, channel_ptr_->GetPar());
    }

    Channel::Channel(Side side,
                     PeakPileUpStrategy strategy,
                     const Params& par,
                     R3B::Neuland::Cal2HitPar* cal_to_hit_par,
                     bool has_cal_output)
        : Digitizing::AbstractChannel{ side, has_cal_output }
        , pileup_strategy_{ strategy }
        , neuland_hit_par_{ cal_to_hit_par }
        , par_{ par }
    {
        pmt_peaks_.reserve(TmxPeaksInitialCapacity);
    }

    Channel::Channel(Side side, PeakPileUpStrategy strategy, TRandom3& rnd)
        : Channel{ side, strategy, Params{ rnd } }
    {
    }

    auto Channel::check_paddle_id_in_hit_par() const -> bool
    {
        auto is_valid = false;
        if (neuland_hit_par_ == nullptr)
        {
            return false;
        }
        if (not neuland_hit_par_->hasChanged())
        {
            R3BLOG(warn, "Can't setup parameter in the root file correctly!.");
            return false;
        }

        auto PaddleId_max = neuland_hit_par_->GetNumModulePar();
        if (GetPaddle()->GetPaddleID() > PaddleId_max)
        {
            LOG(warn) << "Paddle id " << GetPaddle()->GetPaddleID() << " exceeds the id " << PaddleId_max
                      << " in the parameter file!";
            is_valid = false;
        }
        else
        {
            is_valid = true;
        }

        return is_valid;
    }

    void Channel::add_signal(Signal new_signal)
    {
        if (new_signal.time < par_.min_time || new_signal.time > par_.max_time)
        {
            return;
        }
        pmt_peaks_.emplace_back(new_signal, *this);
    }

    auto Channel::CreateHit(const FQTPeak& peak) const -> Hit
    {
        auto peakQdc = peak.GetEnergy();
        auto peakTime = peak.GetLETime();
        auto qdc = smear_energy(peakQdc);

        auto signal = Hit{};
        signal.qdcUnSat = to_unsat_energy(qdc);
        signal.qdc = qdc;
        signal.tdc = smear_time(peakTime);
        signal.side = this->GetSide();
        // R3BLOG(debug3, format("Create a signal {}", signal));
        return signal;
    }

    void Channel::extra_reset()
    {
        pmt_peaks_.clear();
        fqt_peaks_.clear();
        par_ = Tamex::Params{ par_.rnd_gen };
    }

    auto Channel::CreateCalSignal(const FQTPeak& peak) const -> CalSignal
    {
        auto peak_energy = peak.GetEnergy();
        auto peakTime = peak.GetLETime();
        auto smeared_energy = smear_energy(peak_energy);

        auto signal = CalSignal{};
        signal.tot = calculate_ToT(smeared_energy);
        signal.tle = peakTime;
        signal.side = this->GetSide();
        // R3BLOG(debug2, fmt::format("Creating a cal signal {}", signal));
        return signal;
    }

    auto Channel::calculate_ToT(double energy) const -> double
    {
        const auto& par = GetPar();
        return ((energy * par.energy_gain) + par.pedestal);
    }

    template <typename Peak>
    void Channel::do_peak_pileup(/* inout */ std::vector<Peak>& peaks)
    {
        if (peaks.size() <= 1)
        {
            return;
        }

        std::sort(peaks.begin(), peaks.end(), std::less{});
        for (auto front_peak = peaks.begin(); front_peak != peaks.end(); ++front_peak)
        {
            auto end_peak = std::remove_if(front_peak + 1,
                                           peaks.end(),
                                           [&front_peak](auto& peak)
                                           {
                                               if (*front_peak == peak)
                                               {
                                                   (*front_peak) += peak;
                                                   return true;
                                               }
                                               return false;
                                           });
            peaks.erase(end_peak, peaks.end());
        }
    }

    void Channel::peak_pileup_with_distance(/* inout */ std::vector<FQTPeak>& peaks, double distance)
    {
        if (peaks.empty())
        {
            return;
        }
        std::sort(peaks.begin(), peaks.end(), std::less{});

        for (auto front_peak = peaks.begin(); front_peak != peaks.end(); ++front_peak)
        {
            auto last_leading_time = front_peak->GetLETime();
            auto end_peak = std::remove_if(front_peak + 1,
                                           peaks.end(),
                                           [&distance, &front_peak, &last_leading_time](FQTPeak& peak)
                                           {
                                               if ((peak - last_leading_time) < distance)
                                               {
                                                   front_peak->AddEnergy(peak.GetEnergy());
                                                   last_leading_time = peak.GetLETime();
                                                   return true;
                                               }
                                               return false;
                                           });
            peaks.erase(end_peak, peaks.end());
        }
    }

    void Channel::peak_pileup_in_time_window(/* inout */ std::vector<FQTPeak>& peaks, double time_window)
    {
        if (peaks.empty())
        {
            return;
        }
        std::sort(peaks.begin(), peaks.end(), std::less{});

        auto& front_peak = peaks.front();
        std::for_each(peaks.begin() + 1,
                      peaks.end(),
                      [&front_peak, time_window](FQTPeak& peak)
                      {
                          if ((peak - front_peak) < time_window)
                          {
                              front_peak.AddEnergy(peak.GetEnergy());
                          }
                      });
        peaks.erase(peaks.begin() + 1, peaks.end());
    }

    void Channel::fqt_peak_pileup(/* inout */ std::vector<FQTPeak>& peaks)
    {
        switch (pileup_strategy_)
        {
            case PeakPileUpStrategy::width:
                do_peak_pileup(peaks);
                break;
            case PeakPileUpStrategy::distance:
                peak_pileup_with_distance(peaks, par_.pileup_distance);
                break;
            case PeakPileUpStrategy::time_window:
                peak_pileup_in_time_window(peaks, par_.pileup_time_window);
                break;
            default:
                break;
        }
    }

    template <typename Peak>
    void Channel::apply_threshold(std::vector<Peak>& peaks)
    {
        // apply threshold on energy using c++ erase-remove idiom:
        auto it_end = std::remove_if(peaks.begin(),
                                     peaks.end(),
                                     [this](const auto& peak) { return peak.GetHeight() < this->GetPar().pmt_thresh; });
        peaks.erase(it_end, peaks.end());
    }

    void Channel::construct_FQT_peaks(std::vector<FQTPeak>& FQTPeaks, std::vector<PMTPeak>& pmtPeaks)
    {
        FQTPeaks.reserve(pmtPeaks.size());

        // sorting pmt peaks according to time:
        std::sort(pmtPeaks.begin(), pmtPeaks.end());

        do_peak_pileup(pmtPeaks);
        apply_threshold(pmtPeaks);
        for (auto const& peak : pmtPeaks)
        {
            FQTPeaks.emplace_back(peak, this);
        }
    }
    void Channel::pre_construct()
    {
        if (check_paddle_id_in_hit_par())
        {
            const auto& module_par = neuland_hit_par_->GetModuleParAt(GetPaddle()->GetPaddleID());
            set_par_with_hit_module_par(par_, module_par, GetSide());
        }
    }

    void Channel::construct_hits(Hits& hits)
    {
        construct_FQT_peaks(fqt_peaks_, pmt_peaks_);
        // signal pileup:
        fqt_peak_pileup(fqt_peaks_);

        // construct Channel signals:
        hits.reserve(fqt_peaks_.size());

        for (const auto& peak : fqt_peaks_)
        {
            hits.emplace_back(CreateHit(peak));
        }
    }

    void Channel::construct_cal_signals(CalSignals& cal_signals) const
    {
        // construct Channel signals:
        cal_signals.reserve(fqt_peaks_.size());

        for (const auto& peak : fqt_peaks_)
        {
            cal_signals.emplace_back(CreateCalSignal(peak));
        }
    }

    auto Channel::smear_energy(double qdc) const -> double
    {
        // apply energy smearing
        qdc = par_.rnd_gen.get().Gaus(qdc, par_.energy_res_rel * qdc);
        return qdc;
    }

    auto Channel::smear_time(double time) const -> double { return time + par_.rnd_gen.get().Gaus(0., par_.time_res); }

    auto Channel::to_unsat_energy(double qdc) const -> double
    {
        // Apply reverse saturation
        if (par_.experimental_data_is_corrected_for_saturation)
        {
            qdc = qdc / (1 - par_.saturation_coefficient * qdc);
        }
        // Apply reverse attenuation
        return qdc;
    }
} // namespace R3B::Digitizing::Neuland::Tamex
