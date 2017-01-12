#include "DigitizingTacQuila.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>

namespace Neuland
{
    DigitizingTacQuila::Params::Params()
        : fRnd(new TRandom3())
    {
        // Default Values
        fPaddleHalfLength = 0.;         // [cm]
        fPMTThresh = 1.;                // [MeV]
        fSaturationCoefficient = 0.012; //
        fTimeRes = 0.15;                // time + Gaus(0., fTimeRes) [ns]
        fEResRel = 0.05;                // Gaus(e, fEResRel * e) []
        fIntegrationTime = 400.;        // [ns]
        fcMedium = 14.;                 // speed of light in material in [cm/ns]
        fAttenuation = 0.008;           // light attenuation of plastic scintillator [1/cm]
        fLambda = 1. / 2.1;             // decay constant of BC408 scintillator [1/ns]
    }

    // Calculate the time of arrival and the amount of light that arrives at
    // the PMT based on the deposition in the paddle
    DigitizingTacQuila::PMTHit::PMTHit(const Double_t mcTime,
                                       const Double_t mcLight,
                                       const Double_t dist,
                                       const Params& par)
    {
        time = mcTime + (par.fPaddleHalfLength + dist) / par.fcMedium;
        light = mcLight * exp(-par.fAttenuation * (par.fPaddleHalfLength + dist));
    }

    void DigitizingTacQuila::PMT::AddHit(const Double_t mcTime, const Double_t mcLight, const Double_t dist)
    {
        hits.push_back({ mcTime, mcLight, dist, par });
        // NOTE: Sorting after every hit may not be efficient, but this way
        // FindThresholdExeeding hit can be made const
        std::sort(hits.begin(), hits.end());
        cachedFirstHitOverThresh.invalidate();
    }

    DigitizingTacQuila::PMT::PMT(const Params& p)
        : par(p)
    {
    }

    bool DigitizingTacQuila::PMT::HasFired() const
    {
        if (!cachedFirstHitOverThresh.valid())
        {
            cachedFirstHitOverThresh.set(FindThresholdExceedingHit());
            cachedQDC.invalidate();
            cachedTDC.invalidate();
            cachedEnergy.invalidate();
        }
        // If there is a hit that exceeded the threshold, the PMT fill fire
        return cachedFirstHitOverThresh.get() != hits.end();
    }

    Double_t DigitizingTacQuila::PMT::GetQDC() const
    {
        if (!cachedQDC.valid())
        {
            cachedQDC.set(BuildQDC());
        }
        return cachedQDC;
    }

    Double_t DigitizingTacQuila::PMT::GetTDC() const
    {
        if (!cachedTDC.valid())
        {
            cachedTDC.set(BuildTDC());
        }
        return cachedTDC;
    }

    Double_t DigitizingTacQuila::PMT::GetEnergy() const
    {
        if (!cachedEnergy.valid())
        {
            cachedEnergy.set(BuildEnergy());
        }
        return cachedEnergy;
    }

    Double_t DigitizingTacQuila::PMT::BuildQDC() const
    {
        if (HasFired())
        {
            // @mheil: Should that be calculated using an exponential with the
            // prior accumulated light / exponential decay or just all the light that arrives?
            // Maximum pulse height or just sum over all light = charge?
            Double_t light = 0.;
            for (auto hit_it = cachedFirstHitOverThresh.get(); hit_it != hits.end(); hit_it++)
            {
                const PMTHit& hit = *hit_it;
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

    Double_t DigitizingTacQuila::PMT::BuildTDC() const
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

    Double_t DigitizingTacQuila::PMT::BuildEnergy() const
    {
        Double_t e = GetQDC();
        // Apply reverse attenuation (TODO: Should be last?)
        e = e * exp((2. * (par.fPaddleHalfLength)) * par.fAttenuation / 2.);
        // Apply saturation
        e = e / (1. + par.fSaturationCoefficient * e);
        // Apply energy smearing
        e = par.fRnd->Gaus(e, par.fEResRel * e);
        // Apply reverse saturation
        e = e / (1. - par.fSaturationCoefficient * e);
        return e;
    }

    std::vector<DigitizingTacQuila::PMTHit>::const_iterator DigitizingTacQuila::PMT::FindThresholdExceedingHit() const
    {
        // Note that this accumulated light is NOT used for the QDC Value
        // @mheil: Is that actually correct?
        Double_t currentHeightOfLightPulse = 0.;
        Double_t previousTime = 0.;

        // TODO: Could do this shorter with find_if?
        for (std::vector<PMTHit>::const_iterator hit_it = hits.begin(); hit_it != hits.end(); hit_it++)
        {
            const PMTHit& hit = *hit_it;

            // Until the light of this hit arrives at the pmt, the previous light pulses have decayed
            currentHeightOfLightPulse *= exp(-par.fLambda * (hit.time - previousTime));
            previousTime = hit.time;

            // Add the current light pulse
            currentHeightOfLightPulse += hit.light;

            // If the light pulse is higher than the threshold, this hit causes the pmt to fire
            if (currentHeightOfLightPulse * exp(par.fAttenuation * par.fPaddleHalfLength) > par.fPMTThresh)
            {
                return hit_it;
            }
        }

        // Threshold was not exceeded
        return hits.end();
    }

    DigitizingTacQuila::Paddle::Paddle(const Params& p)
        : par(p)
        , leftPMT(p)
        , rightPMT(p)
    {
    }

    inline Double_t DigitizingTacQuila::Paddle::GetEnergy() const
    {
        return std::sqrt(leftPMT.GetEnergy() * rightPMT.GetEnergy());
    }

    inline Double_t DigitizingTacQuila::Paddle::GetTime() const
    {
        return (leftPMT.GetTDC() + rightPMT.GetTDC()) / 2. - par.fPaddleHalfLength / par.fcMedium;
    }

    inline Double_t DigitizingTacQuila::Paddle::GetPosition() const
    {
        return (rightPMT.GetTDC() - leftPMT.GetTDC()) / 2. * par.fcMedium;
    }

    inline bool DigitizingTacQuila::Paddle::HasFired() const { return (leftPMT.HasFired() && rightPMT.HasFired()); }

    void DigitizingTacQuila::Paddle::DepositLight(const Double_t time, const Double_t light, const Double_t dist)
    {
        if (par.fPaddleHalfLength == 0.)
        {
            throw std::invalid_argument("Paddle Length has not been set");
        }
        leftPMT.AddHit(time, light, -1. * dist);
        rightPMT.AddHit(time, light, dist);
    }

    void DigitizingTacQuila::DepositLight(const Int_t paddle_id,
                                          const Double_t time,
                                          const Double_t light,
                                          const Double_t dist)
    {
        if (paddles.find(paddle_id) == paddles.end())
        {
            paddles[paddle_id] = std::unique_ptr<Paddle>(new Paddle(par));
        }
        paddles.at(paddle_id)->DepositLight(time, light, dist);
    }

    Double_t DigitizingTacQuila::GetTriggerTime() const
    {
        Double_t triggerTime = 1e100;
        for (const auto& kv : paddles)
        {
            const auto& paddle = kv.second;

            // TODO: Should be easier with std::min?
            if (paddle->GetLeftPMT()->HasFired() && paddle->GetLeftPMT()->GetTDC() < triggerTime)
            {
                triggerTime = paddle->GetLeftPMT()->GetTDC();
            }
            if (paddle->GetRightPMT()->HasFired() && paddle->GetRightPMT()->GetTDC() < triggerTime)
            {
                triggerTime = paddle->GetRightPMT()->GetTDC();
            }
        }
        return triggerTime;
    }

    std::map<Int_t, std::unique_ptr<DigitizingEngine::Paddle>> DigitizingTacQuila::ExtractPaddles()
    {
        std::map<Int_t, std::unique_ptr<DigitizingEngine::Paddle>> out;
        for (auto& kv : paddles)
        {
            // std::unique_ptr<DigitizingTacQuila::Paddle> -> std::unique_ptr<DigitizingEngine::Paddle>
            out[kv.first] = std::move(kv.second);
        }
        paddles.clear();
        return out;
    }

}; // namespace
