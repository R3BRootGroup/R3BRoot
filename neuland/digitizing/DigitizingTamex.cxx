/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "DigitizingTamex.h"

#include <algorithm>
#include <iostream>

#include "FairLogger.h"

namespace Neuland
{
    Tamex::Params::Params(TRandom3* rnd)
        : fPMTThresh(1.)                // [MeV]
        , fSaturationCoefficient(0.012) //
        , fExperimentalDataIsCorrectedForSaturation(true)
        , fTimeRes(0.15) // time + Gaus(0., fTimeRes) [ns]
        , fEResRel(0.05) // Gaus(e, fEResRel * e) []
        , fEnergyGain(15.0)
        , fPedestal(14.0)
        , fTimeMax(1000)
        , fTimeMin(1)
        , fQdcMin(0.67)
        , fRnd(rnd)
    {
    }

    Tamex::TmxPeak::TmxPeak()
        : TmxPeak(Digitizing::PMTHit{}, nullptr)
    {
    }

    Tamex::TmxPeak::TmxPeak(const Digitizing::PMTHit& hit, Tamex::Channel* channel)
        : fChannel{ channel }
    {
        if (!fChannel)
            LOG(fatal) << "no channel is linked to the signal peak!";
        auto side = fChannel->GetSide();
        auto par = fChannel->GetPar();

        auto light = hit.light;
        // apply saturation coefficent
        if (par.fExperimentalDataIsCorrectedForSaturation)
            fQdc = light / (1. + par.fSaturationCoefficient * light);
        else
            fQdc = light;

        // calculate the time and the width of the signal
        fTime = hit.time;
        fWidth = QdcToWidth(fQdc);
    }

    Tamex::TmxPeak::operator Digitizing::Channel::Signal() const
    {
        if (!cachedSignal.valid())
            cachedSignal.set(fChannel->TmxPeakToSignal(*this));

        return cachedSignal.get();
    }

    bool Tamex::TmxPeak::operator==(const TmxPeak& sig) const
    {
        if (sig.fTime == 0 && fTime == 0)
            LOG(warn) << "the times of both PMT signals are 0!";
        return !((fTime > sig.fTime + sig.fWidth) || (sig.fTime > fTime + fWidth));
    }

    void Tamex::TmxPeak::operator+=(TmxPeak& sig)
    {
        cachedSignal.invalidate();
        auto qdc_prev = fQdc;

        fTime = (fTime > sig.fTime) ? fTime : sig.fTime;
        fQdc += sig.fQdc;

        // change the width of peak to make sure its correlation to qdc stays the same
        if (!fChannel)
            LOG(fatal) << "no channel is linked to the signal peak!";
        auto par = fChannel->GetPar();
        if (par.fEnergyGain != 0)
            fWidth = QdcToWidth(fQdc);
    }

    Double_t Tamex::TmxPeak::QdcToWidth(Double_t qdc) const
    {
        Double_t width;
        auto par = fChannel->GetPar();
        if (qdc > par.fQdcMin)
            width = qdc * par.fEnergyGain + par.fPedestal;
        else
            width = qdc * par.fEnergyGain * (par.fPedestal + 1);
        return std::move(width);
    }

    Tamex::Channel::Channel(TRandom3* rnd, const SideOfChannel side)
        : Digitizing::Channel{ side }
        , par{ rnd }
    {
    }

    bool Tamex::Channel::HasFired() const { return (GetSignals().size() > 0); }

    Int_t Tamex::Channel::CheckOverlapping(TmxPeak& peak) const
    {
        auto it = std::find_if(fTmxPeaks.begin(), fTmxPeaks.end(), [&](const TmxPeak& s) { return (s == peak); });
        if (it == fTmxPeaks.end())
        {
            return -1;
        }
        else
        {
            return static_cast<Int_t>(it - fTmxPeaks.begin());
        }
    }

    Int_t Tamex::Channel::RecheckOverlapping(Int_t index)
    {
        auto it = fTmxPeaks.begin();
        if (index >= fTmxPeaks.size())
            LOG(fatal) << "DigitizingTamex::RecheckOverlapping: cannot check the peak with overflowing index!";
        while (it != fTmxPeaks.end())
        {
            Int_t i = 0;
            it = std::find_if(fTmxPeaks.begin(), fTmxPeaks.end(), [&](const TmxPeak& p) {
                bool res = false;
                if (index != i)
                {
                    res = (p == fTmxPeaks[index]);
                }
                i++;
                return res;
            });

            if (it == fTmxPeaks.end())
                continue;

            i = static_cast<int>(it - fTmxPeaks.begin());

            if (index == fTmxPeaks.size() - 1)
            {
                std::swap(index, i);
            }
            fTmxPeaks[index] += fTmxPeaks[i];
            RemovePeakAt(i);
        }
        return index;
    }

    void Tamex::Channel::AddHit(Double_t mcTime, Double_t mcLight, Double_t dist)
    {
        auto newHit = Digitizing::PMTHit{ mcTime, mcLight, dist };
        if (newHit.time < par.fTimeMin || newHit.time > par.fTimeMax)
            return;

        fSignals.invalidate();
        fTrigTime.invalidate();

        auto peak = TmxPeak{ std::move(newHit), this };

        auto index = CheckOverlapping(peak);

        if (index < 0)
        {
            fTmxPeaks.push_back(std::move(peak));
        }
        else
        {
            fTmxPeaks[index] += peak;
            index = RecheckOverlapping(index);
        }
    }

    void Tamex::Channel::RemovePeakAt(Int_t i) const
    {
        if (i >= fTmxPeaks.size())
        {
            LOG(fatal) << "DigitizingTamex::RemovePeakAt: Cannot remove the peak with the overflowing index! ";
            return;
        }
        if (i != fTmxPeaks.size() - 1)
        {
            fTmxPeaks[i] = std::move(fTmxPeaks.back());
        }
        fTmxPeaks.pop_back();
    }

    Digitizing::Channel::Signal Tamex::Channel::TmxPeakToSignal(const TmxPeak& peak) const
    {
        auto peakQdc = peak.GetQDC();
        auto peakTime = peak.GetTime();
        auto qdc = ToQdc(peakQdc);

        return { qdc, ToTdc(peakTime), ToEnergy(qdc), this->GetSide() };
    }

    void Tamex::Channel::RemoveZero(std::vector<Signal>& signals) const
    {
        // remove signals with 0 energy using c++ erase-remove idiom:
        auto it = std::remove_if(signals.begin(), signals.end(), [](const Signal& s) { return s.energy == 0.0; });
        signals.erase(it, signals.end());
    }

    void Tamex::Channel::ConstructSignals() const
    {
        auto signals = std::vector<Signal>{};
        signals.reserve(fTmxPeaks.size());

        std::transform(fTmxPeaks.begin(), fTmxPeaks.end(), std::back_inserter(signals), [](TmxPeak& peak) {
            return static_cast<Signal>(peak);
        });
        RemoveZero(signals);
        fSignals.set(std::move(signals));
    }

    void Tamex::Channel::SetPaddle(Digitizing::Paddle* paddle)
    {
        fPaddle = paddle;
        auto hitModulePar = paddle->GetHitModulePar();
        if (hitModulePar)
        {
            par.fSaturationCoefficient = hitModulePar->GetPMTSaturation(fSide);
            par.fEnergyGain = hitModulePar->GetEnergyGain(fSide);
            par.fPedestal = hitModulePar->GetPedestal(fSide);
            par.fPMTThresh = hitModulePar->GetPMTThreshold(fSide);
            par.fQdcMin = 1 / par.fEnergyGain;
        }
    }

    Double_t Tamex::Channel::ToQdc(Double_t qdc) const
    {
        // apply energy smearing
        qdc = par.fRnd->Gaus(qdc, par.fEResRel * qdc);

        // set qdc to zero if below PMT threshold
        qdc = (qdc > par.fPMTThresh) ? qdc : 0.0;
        return qdc;
    }

    Double_t Tamex::Channel::ToTdc(Double_t time) const { return time + par.fRnd->Gaus(0., par.fTimeRes); }

    Double_t Tamex::Channel::ToEnergy(Double_t e) const
    {
        // Apply reverse saturation
        if (par.fExperimentalDataIsCorrectedForSaturation)
        {
            e = e / (1. - par.fSaturationCoefficient * e);
        }
        // Apply reverse attenuation
        e = e * exp((2. * (Digitizing::Paddle::gHalfLength)) * Digitizing::Paddle::gAttenuation / 2.);
        return e;
    }

    const Double_t Tamex::Channel::GetTrigTime() const
    {
        if (!fTrigTime.valid())
        {
            auto signals = GetSignals();
            auto it = std::min_element(
                signals.begin(), signals.end(), [](const Signal& l, const Signal& r) { return l.tdc < r.tdc; });
            fTrigTime.set(it->tdc);
        }
        return fTrigTime.get();
    }

    DigitizingTamex::DigitizingTamex()
        : fRnd(new TRandom3{})
    {
    }

    std::unique_ptr<Digitizing::Channel> DigitizingTamex::BuildChannel(Digitizing::Channel::SideOfChannel side)
    {
        return std::unique_ptr<Digitizing::Channel>(new Tamex::Channel(fRnd.get(), side));
    }

} // namespace Neuland
