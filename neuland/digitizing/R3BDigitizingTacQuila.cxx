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

#include "R3BDigitizingTacQuila.h"
#include "R3BDigitizingChannel.h"
#include "R3BDigitizingPaddleNeuland.h"
#include "R3BShared.h"
#include <RtypesCore.h>
#include <TRandom3.h>
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

namespace R3B::Digitizing::Neuland::TacQuila
{
    Params::Params()
        : fPMTThresh(1.)                // [MeV]
        , fSaturationCoefficient(0.012) //
        , fExperimentalDataIsCorrectedForSaturation(true)
        , fTimeRes(0.15)         // time + Gaus(0., fTimeRes) [ns]
        , fEResRel(0.05)         // Gaus(e, fEResRel * e) []
        , fIntegrationTime(400.) // [ns]
        , fRnd(new TRandom3())
    {
    }

    const Params TACQUILA_DEFAULT_PARAM = Params{};
    Channel::Channel(Side side, const Params& para)
        : Digitizing::AbstractChannel(side)
        , par(para)
    {
    }

    void Channel::add_signal(Signal newHit)
    {
        fPMTHits.emplace_back(newHit.time, newHit.intensity);
        // NOTE: Sorting after every hit may not be efficient, but this way
        // FindThresholdExeeding hit can be made const
        std::sort(fPMTHits.begin(), fPMTHits.end());
        cachedFirstHitOverThresh.invalidate();
    }

    auto Channel::HasFired() -> bool
    {
        if (!cachedFirstHitOverThresh.valid())
        {
            cachedFirstHitOverThresh.set(FindThresholdExceedingHit());
            cachedQDC.invalidate();
            cachedTDC.invalidate();
            cachedEnergy.invalidate();
        }
        // If there is a hit that exceeded the threshold, the PMT fill fire
        return cachedFirstHitOverThresh.get() != fPMTHits.end();
    }

    auto Channel::GetQDC() -> double
    {
        if (!cachedQDC.valid())
        {
            cachedQDC.set(BuildQDC());
        }
        return cachedQDC;
    }

    auto Channel::GetTDC() -> double
    {
        if (!cachedTDC.valid())
        {
            cachedTDC.set(BuildTDC());
        }
        return cachedTDC;
    }

    auto Channel::GetEnergy() -> double
    {
        if (!cachedEnergy.valid())
        {
            cachedEnergy.set(BuildEnergy());
        }
        return cachedEnergy;
    }

    auto Channel::BuildQDC() -> double
    {
        if (HasFired())
        {
            // @mheil: Should that be calculated using an exponential with the
            // prior accumulated light / exponential decay or just all the light that arrives?
            // Maximum pulse height or just sum over all light = charge?
            Double_t light = 0.;
            for (auto hit_it = cachedFirstHitOverThresh.get(); hit_it != fPMTHits.end(); hit_it++)
            {
                const auto hit = *hit_it;
                if (hit.time < (*cachedFirstHitOverThresh.get()).time + par.fIntegrationTime)
                {
                    light += hit.intensity;
                }
            }
            return light;
        }

        return 0.;
    }

    auto Channel::BuildTDC() -> double
    {
        if (HasFired())
        {
            return (*cachedFirstHitOverThresh.get()).time + par.fRnd->Gaus(0., par.fTimeRes);
        }

        return -1.;
    }

    auto Channel::BuildEnergy() -> double
    {
        Double_t energy = GetQDC();
        // Apply reverse attenuation (TODO: Should be last?)
        // e = e * exp((2. * (NeulandPaddle::gHalfLength)) * NeulandPaddle::gAttenuation / 2.);
        // Apply saturation
        energy = energy / (1. + par.fSaturationCoefficient * energy);
        // Apply energy smearing
        energy = par.fRnd->Gaus(energy, par.fEResRel * energy);
        // Apply reverse saturation
        if (par.fExperimentalDataIsCorrectedForSaturation)
        {
            energy = energy / (1. - par.fSaturationCoefficient * energy);
        }
        return energy;
    }

    auto Channel::FindThresholdExceedingHit() const -> std::vector<Channel::Signal>::const_iterator
    {
        // Note that this accumulated light is NOT used for the QDC Value
        // @mheil: Is that actually correct?
        Double_t currentHeightOfLightPulse = 0.;
        Double_t previousTime = 0.;

        // TODO: Could do this shorter with find_if?
        for (auto hit_it = fPMTHits.begin(); hit_it != fPMTHits.end(); hit_it++)
        {
            auto hit = *hit_it;

            // Until the light of this hit arrives at the pmt, the previous light pulses have decayed
            currentHeightOfLightPulse *= exp(-DEFAULT_LAMBDA * (hit.time - previousTime));
            previousTime = hit.time;

            // Add the current light pulse
            currentHeightOfLightPulse += hit.intensity;

            // If the light pulse is higher than the threshold, this hit causes the pmt to fire
            if (currentHeightOfLightPulse * exp(Paddle::DEFAULT_ATTENUATION * Paddle::HALF_BAR_LENGTH) > par.fPMTThresh)
            {
                return hit_it;
            }
        }

        // Threshold was not exceeded
        return fPMTHits.end();
    }

    void Channel::extra_reset()
    {
        fPMTHits.clear();
        cachedFirstHitOverThresh.invalidate();
        cachedQDC.invalidate();
        cachedTDC.invalidate();
        cachedEnergy.invalidate();
    }

    void Channel::construct_hits(Hits& signals)
    {
        auto signal = Hit{};
        signal.qdcUnSat = GetEnergy();
        signal.qdc = GetQDC();
        signal.tdc = GetTDC();
        signal.side = this->GetSide();
        signals.push_back(signal);
    }
} // namespace R3B::Digitizing::Neuland::TacQuila
