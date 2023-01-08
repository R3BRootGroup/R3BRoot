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

#include "DigitizingTacQuila.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>

namespace Neuland
{
    namespace TacQuila
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

        Channel::Channel(const Params& p, SideOfChannel side)
            : par(p)
            , Digitizing::Channel(side)
        {
        }

        void Channel::AddHit(const Double_t mcTime, const Double_t mcLight, const Double_t dist)
        {
            fPMTHits.emplace_back(mcTime, mcLight, dist);
            // NOTE: Sorting after every hit may not be efficient, but this way
            // FindThresholdExeeding hit can be made const
            std::sort(fPMTHits.begin(), fPMTHits.end());
            cachedFirstHitOverThresh.invalidate();
        }

        bool Channel::HasFired() const
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

        Double_t Channel::GetQDC() const
        {
            if (!cachedQDC.valid())
            {
                cachedQDC.set(BuildQDC());
            }
            return cachedQDC;
        }

        Double_t Channel::GetTDC() const
        {
            if (!cachedTDC.valid())
            {
                cachedTDC.set(BuildTDC());
            }
            return cachedTDC;
        }

        Double_t Channel::GetEnergy() const
        {
            if (!cachedEnergy.valid())
            {
                cachedEnergy.set(BuildEnergy());
            }
            return cachedEnergy;
        }

        Double_t Channel::BuildQDC() const
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
                        light += hit.light;
                    }
                }
                return light;
            }
            else
            {
                return 0.;
            }
        }

        Double_t Channel::BuildTDC() const
        {
            if (HasFired())
            {
                return (*cachedFirstHitOverThresh.get()).time + par.fRnd->Gaus(0., par.fTimeRes);
            }
            else
            {
                return -1.;
            }
        }

        Double_t Channel::BuildEnergy() const
        {
            Double_t e = GetQDC();
            // Apply reverse attenuation (TODO: Should be last?)
            e = e * exp((2. * (Digitizing::Paddle::gHalfLength)) * Digitizing::Paddle::gAttenuation / 2.);
            // Apply saturation
            e = e / (1. + par.fSaturationCoefficient * e);
            // Apply energy smearing
            e = par.fRnd->Gaus(e, par.fEResRel * e);
            // Apply reverse saturation
            if (par.fExperimentalDataIsCorrectedForSaturation)
            {
                e = e / (1. - par.fSaturationCoefficient * e);
            }
            return e;
        }

        std::vector<Digitizing::PMTHit>::const_iterator Channel::FindThresholdExceedingHit() const
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
                currentHeightOfLightPulse *= exp(-Digitizing::Paddle::gLambda * (hit.time - previousTime));
                previousTime = hit.time;

                // Add the current light pulse
                currentHeightOfLightPulse += hit.light;

                // If the light pulse is higher than the threshold, this hit causes the pmt to fire
                if (currentHeightOfLightPulse *
                        exp(Digitizing::Paddle::gAttenuation * Digitizing::Paddle::gHalfLength) >
                    par.fPMTThresh)
                {
                    return hit_it;
                }
            }

            // Threshold was not exceeded
            return fPMTHits.end();
        }

    } // namespace TacQuila

    DigitizingTacQuila::DigitizingTacQuila()
        : fTQP(TacQuila::Params())
    {
    }

    std::unique_ptr<Digitizing::Channel> DigitizingTacQuila::BuildChannel(Digitizing::Channel::SideOfChannel side)
    {
        return std::unique_ptr<Digitizing::Channel>(new TacQuila::Channel(fTQP, side));
    }
}; // namespace Neuland
