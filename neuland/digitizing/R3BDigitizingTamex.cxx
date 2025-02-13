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
#include <cmath>

#include "R3BException.h"
#include <FairRunAna.h>
#include <R3BLogger.h>
#include <algorithm>

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
                                         ChannelSide channel_side)
        {
            auto side = (channel_side == ChannelSide::right) ? Side::right : Side::left;

            par.fSaturationCoefficient = module_par.pmt_saturation.get(side).value;
            par.fEnergyGain = module_par.energy_gain.get(side).value;
            par.fPedestal = module_par.pedestal.get(side).value;
            par.fPMTThresh = module_par.pmt_threshold.get(side).value;
            par.fQdcMin = 1 / par.fEnergyGain;

            // TODO: Add other parameters:
        }
    } // namespace

    // global variables for default options:
    const size_t TmxPeaksInitialCapacity = 10;

    Params::Params(TRandom3& rnd)
        : fRnd{ rnd }
    {
    }

    PMTPeak::PMTPeak(Digitizing::Channel::Hit pmtHit, const Channel& channel)
        : time_(pmtHit.time)
    {
        const auto& par = channel.GetParConstRef();
        // apply saturation coefficent
        qdc_ = pmtHit.light / (1. + par.fSaturationCoefficient * pmtHit.light);
    };

    auto PMTPeak::operator+=(const PMTPeak& other) -> PMTPeak&
    {
        qdc_ += other.qdc_;
        time_ = (time_ < other.time_) ? time_ : other.time_;
        return *this;
    }

    FQTPeak::FQTPeak(const PMTPeak& pmtPeak, Channel* channel)
        : qdc_(pmtPeak.GetQDC())
        , leading_edge_time_(pmtPeak.GetLETime())
        , channel_ptr_(channel)
    {
        if (channel_ptr_ == nullptr)
        {
            LOG(fatal) << "channel is not bound to FQTPeak object!";
        }
        const auto& par = channel->GetParConstRef();

        // calculate the time and the width of the signal
        width_ = QdcToWidth(qdc_, par);
        trailing_edge_time_ = leading_edge_time_ + width_;
    }

    auto FQTPeak::operator==(const FQTPeak& other) const -> bool
    {
        if (other.leading_edge_time_ == 0 && leading_edge_time_ == 0)
        {
            LOG(warn) << "the times of both PMT signals are 0!";
        }
        return (leading_edge_time_ <= (other.leading_edge_time_ + other.width_)) &&
               (other.leading_edge_time_ <= (leading_edge_time_ + width_));
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
        width_ = trailing_edge_time_ - leading_edge_time_;
        qdc_ = WidthToQdc(width_, channel_ptr_->GetParConstRef());
    }

    Channel::Channel(ChannelSide side,
                     PeakPileUpStrategy strategy,
                     const Params& par,
                     R3B::Neuland::Cal2HitPar* cal_to_hit_par)
        : Digitizing::Channel{ side }
        , pileup_strategy_{ strategy }
        , neuland_hit_par_{ cal_to_hit_par }
        , par_{ par }
    {
        pmt_peaks_.reserve(TmxPeaksInitialCapacity);
    }

    Channel::Channel(ChannelSide side, PeakPileUpStrategy strategy, TRandom3& rnd)
        : Channel{ side, strategy, Params{ rnd } }
    {
    }

    void Channel::AttachToPaddle(Digitizing::Paddle* paddle)
    {

        if (paddle == nullptr)
        {
            return;
        }
        if (CheckPaddleIDInHitPar())
        {
            const auto& module_par = neuland_hit_par_->GetModuleParAt(paddle->GetPaddleID());
            set_par_with_hit_module_par(par_, module_par, GetSide());
        }
    }

    auto Channel::CheckPaddleIDInHitPar() const -> bool
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

    void Channel::AddHit(Hit newHit)
    {
        if (newHit.time < par_.fTimeMin || newHit.time > par_.fTimeMax)
        {
            return;
        }
        InvalidateSignals();
        InvalidateTrigTime();
        pmt_peaks_.emplace_back(newHit, *this);
    }

    auto Channel::CreateSignal(const FQTPeak& peak) const -> Signal
    {
        auto peakQdc = peak.GetQDC();
        auto peakTime = peak.GetLETime();
        auto qdc = ToQdc(peakQdc);

        LOG(debug) << "qdc Signal " << qdc << " and tdc " << peakTime << "\n";

        auto signal = Signal{};
        signal.qdcUnSat = ToUnSatQdc(qdc);
        signal.qdc = qdc;
        signal.tdc = ToTdc(peakTime);
        signal.side = this->GetSide();
        LOG(debug) << "R3BDigitizingTamex: Create a signal with qdc " << signal.qdc << " and tdc " << signal.tdc
                   << "\n";
        return signal;
    }

    auto Channel::CreateCalSignal(const FQTPeak& peak) const -> CalSignal
    {
        auto peakQdc = peak.GetQDC();
        auto peakTime = peak.GetLETime();
        auto qdc = ToQdc(peakQdc);

        LOG(debug) << "qdc Cal " << qdc << " and tdc " << peakTime << "\n";

        auto signal = CalSignal{};
        signal.tot = CalculateTOT(qdc);
        signal.tle = peakTime;
        signal.side = this->GetSide();
        LOG(debug) << "R3BDigitizingTamex: Create a CalSignal with tot " << signal.tot << " and let " << signal.tle
                   << "\n qdc: " << qdc << "\n";
        return signal;
    }

    auto Channel::CalculateTOT(const double& qdc) const -> double
    {
        // ToDo: Decide if ERROR stuff is needed
        //  if (GetErrorCalculation())
        //  {
        //      ValueError<double> qdc_err_val{ qdc, 0 };
        //      auto par_err_val = GetParErrVal();
        //      auto tot_err_val = qdc_err_val * par_err_val.energyGain + par_err_val.pedestal;
        //      auto randGen = GetDefaultRandomGen();
        //      return randGen.Gaus(tot_err_val.value, tot_err_val.error);
        //  }
        //  else {
        //
        //      auto par = GetParConstRef();
        //      return (qdc * par.fEnergyGain + par.fPedestal);
        //  }

        auto par = GetParConstRef();
        return ((qdc * par.fEnergyGain) + par.fPedestal);
    }

    template <typename Peak>
    void Channel::PeakPileUp(/* inout */ std::vector<Peak>& peaks)
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

    void Channel::PeakPileUpWithDistance(/* inout */ std::vector<FQTPeak>& peaks, double distance)
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
                                                   front_peak->AddQDC(peak.GetQDC());
                                                   last_leading_time = peak.GetLETime();
                                                   return true;
                                               }
                                               return false;
                                           });
            peaks.erase(end_peak, peaks.end());
        }
    }

    void Channel::PeakPileUpInTimeWindow(/* inout */ std::vector<FQTPeak>& peaks, double time_window)
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
                              front_peak.AddQDC(peak.GetQDC());
                          }
                      });
        peaks.erase(peaks.begin() + 1, peaks.end());
    }

    void Channel::FQTPeakPileUp(/* inout */ std::vector<FQTPeak>& peaks)
    {
        switch (pileup_strategy_)
        {
            case PeakPileUpStrategy::width:
                PeakPileUp(peaks);
                break;
            case PeakPileUpStrategy::distance:
                PeakPileUpWithDistance(peaks, par_.fPileUpDistance);
                break;
            case PeakPileUpStrategy::time_window:
                PeakPileUpInTimeWindow(peaks, par_.fPileUpTimeWindow);
                break;
            default:
                break;
        }
    }

    template <typename Peak>
    void Channel::ApplyThreshold(std::vector<Peak>& peaks)
    {
        // apply threshold on energy using c++ erase-remove idiom:
        auto it_end =
            std::remove_if(peaks.begin(),
                           peaks.end(),
                           [this](const auto& peak) { return peak.GetQDC() < this->GetParConstRef().fPMTThresh; });
        peaks.erase(it_end, peaks.end());
    }

    auto Channel::ConstructFQTPeaks(std::vector<PMTPeak>& pmtPeaks) -> std::vector<FQTPeak>
    {
        auto FQTPeaks = std::vector<FQTPeak>{};
        FQTPeaks.reserve(pmtPeaks.size());

        // sorting pmt peaks according to time:
        std::sort(pmtPeaks.begin(), pmtPeaks.end());

        PeakPileUp(pmtPeaks);
        ApplyThreshold(pmtPeaks);
        for (auto const& peak : pmtPeaks)
        {
            FQTPeaks.emplace_back(peak, this);
        }
        return FQTPeaks;
    }

    auto Channel::ConstructSignals() -> Signals
    {
        fqt_peaks_ = ConstructFQTPeaks(pmt_peaks_);
        // signal pileup:
        FQTPeakPileUp(fqt_peaks_);

        // construct Channel signals:
        auto signals = std::vector<Signal>{};
        signals.reserve(fqt_peaks_.size());

        for (const auto& peak : fqt_peaks_)
        {
            signals.emplace_back(CreateSignal(peak));
        }
        return signals;
    }

    auto Channel::ConstructCalSignals() -> CalSignals
    {
        fqt_peaks_ = ConstructFQTPeaks(pmt_peaks_);
        // signal pileup:
        FQTPeakPileUp(fqt_peaks_);

        // construct Channel signals:
        auto cal_signals = std::vector<CalSignal>{};
        cal_signals.reserve(fqt_peaks_.size());

        for (const auto& peak : fqt_peaks_)
        {
            cal_signals.emplace_back(CreateCalSignal(peak));
        }
        return cal_signals;
    }

    auto Channel::GetCalSignals() -> CalSignals { return ConstructCalSignals(); }

    auto Channel::GetFQTPeaks() -> const std::vector<FQTPeak>&
    {

        if (!Is_ValidSignals())
        {
            ConstructSignals();
        }
        return fqt_peaks_;
    }

    auto Channel::GetPMTPeaks() -> const std::vector<PMTPeak>&
    {
        if (!Is_ValidSignals())
        {
            ConstructSignals();
        }
        return pmt_peaks_;
    }

    auto Channel::ToQdc(double qdc) const -> double
    {
        // apply energy smearing
        qdc = par_.fRnd.get().Gaus(qdc, par_.fEResRel * qdc);
        return qdc;
    }

    auto Channel::ToTdc(double time) const -> double { return time + par_.fRnd.get().Gaus(0., par_.fTimeRes); }

    auto Channel::ToUnSatQdc(double qdc) const -> double
    {
        // Apply reverse saturation
        if (par_.fExperimentalDataIsCorrectedForSaturation)
        {
            qdc = qdc / (1 - par_.fSaturationCoefficient * qdc);
            LOG(debug) << "ToUnSatQdc: fSaturationCoefficient = " << par_.fSaturationCoefficient << '\n';
        }
        // Apply reverse attenuation
        return qdc;
    }
} // namespace R3B::Digitizing::Neuland::Tamex
