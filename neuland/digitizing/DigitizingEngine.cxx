#include "DigitizingEngine.h"

namespace Neuland {

const Double_t DigitizingEngine::fSaturationCoefficient = 0.012;

const Double_t DigitizingEngine::fIntegrationTime = 400.;

//  c = 2.99792458E8;  // m/s
//  cMedia = c / 1.58 * 1e9; // speed of light in material in [cm/ns]
const Double_t DigitizingEngine::fcMedium = 14.; // speed of light in material in [cm/ns]

// light attenuation of plastic scintillator
//att = 0.0047619047619; // [1/cm]
const Double_t DigitizingEngine::fAttenuation = 0.008; // [1/cm]

// decay constant of BC408 scintillator
const Double_t DigitizingEngine::fLambda = 1. / 2.1; // [1/ns]

const Double_t DigitizingEngine::fThresh = 0.16;

// TODO: So, a detector with less than 200 paddles is LAND?
/*if (!fLandDigiPar->GetGeometryFileName().Contains("proto") &&
        !fLandDigiPar->GetGeometryFileName().Contains("jap") &&
        npaddles < 202) {
    // LAND detector
    timeRes = 0.25; // ns
}
// FIXME: So, "jap" has a different time resolution?
if (fLandDigiPar->GetGeometryFileName().Contains("jap")) {
    timeRes = 0.010;
}*/
const Double_t DigitizingEngine::fTimeRes = 0.15; // ns

Double_t DigitizingEngine::fPaddleHalfLength = 0.;

TRandom3 *DigitizingEngine::fRnd = new TRandom3();


DigitizingEngine::DigitizingEngine()
{
}

DigitizingEngine::~DigitizingEngine()
{
}


Double_t DigitizingEngine::GetTriggerTime() const
{
    Double_t triggerTime = 1e100;
    for (const auto &kv : paddles) {
        const Paddle &paddle = kv.second;

        // TODO: Should be easier with std::min?
        if (paddle.leftPMT.HasFired() && paddle.leftPMT.GetTDC() < triggerTime) {
            triggerTime = paddle.leftPMT.GetTDC();
        }
        if (paddle.rightPMT.HasFired() && paddle.rightPMT.GetTDC() < triggerTime) {
            triggerTime = paddle.rightPMT.GetTDC();
        }
    }
    return triggerTime;
}



bool DigitizingEngine::PMT::HasFired() const
{
    if (!cachedFirstHitOverThresh.valid()) {
        cachedFirstHitOverThresh.set(FindThresholdExceedingHit());
        cachedQDC.invalidate();
        cachedTDC.invalidate();
        cachedEnergy.invalidate();
    }
    // If there is a hit that exceeded the threshold, the PMT fill fire
    return cachedFirstHitOverThresh.get() != hits.end();
}


Double_t DigitizingEngine::PMT::GetQDC() const
{
    if (!cachedQDC.valid()) {
        cachedQDC.set(BuildQDC());
    }
    return cachedQDC;
}


Double_t DigitizingEngine::PMT::BuildQDC() const
{
    if (HasFired()) {
        // @mheil: Should that be calculated using an exponetial with the
        // prior accumulated light / eponentail decay or just all the light that arrives?
        // Maxmimul pulse height or just sum over all light = charge?
        Double_t light = 0.;
        for (auto hit_it = cachedFirstHitOverThresh.get(); hit_it != hits.end(); hit_it++) {
            const PMTHit &hit = *hit_it;
            if (hit.time < (*cachedFirstHitOverThresh.get()).time + fIntegrationTime) {
                light += hit.light;
            }
        }
        return light;
    } else {
        return 0.;
    }
}


Double_t DigitizingEngine::PMT::GetTDC() const
{
    if (!cachedTDC.valid()) {
        cachedTDC.set(BuildTDC());
    }
    return cachedTDC;
}


Double_t DigitizingEngine::PMT::BuildTDC() const
{
    if (HasFired()) {
        return (*cachedFirstHitOverThresh.get()).time + fRnd->Gaus(0., fTimeRes);
    } else {
        return -1.;
    }
}


Double_t DigitizingEngine::PMT::GetEnergy() const
{
    if (!cachedEnergy.valid()) {
        cachedEnergy.set(BuildEnergy());
    }
    return cachedEnergy;
}


Double_t DigitizingEngine::PMT::BuildEnergy() const
{
    Double_t e;
    e = GetQDC() * exp((2.*(fPaddleHalfLength)) * fAttenuation / 2.);
    e = e / (1. + fSaturationCoefficient * e);
    e = fRnd->Gaus(e, 0.05 * e);
    return e;
}


std::vector<DigitizingEngine::PMTHit>::const_iterator DigitizingEngine::PMT::FindThresholdExceedingHit() const
{
    // Note that this accumulated light is NOT used for the QDC Value
    // @mheil: Is that actually correct?
    Double_t currentHeightOfLightPulse = 0.;
    Double_t previousTime = 0.;

    // TODO: Could do this shorter with find_if?
    for (std::vector<PMTHit>::const_iterator hit_it = hits.begin(); hit_it != hits.end(); hit_it++) {
        const PMTHit &hit = *hit_it;

        // Until the light of this hit arrives at the pmt, the previous light pulses have decayed
        currentHeightOfLightPulse *= exp(-fLambda * (hit.time - previousTime));
        previousTime = hit.time;

        // Add the current light pulse
        currentHeightOfLightPulse += fLambda * hit.light;

        // If the light pulse is higher than the treshold, this hit causes the pmt to fire
        if (currentHeightOfLightPulse > fThresh) {
            return hit_it;
        }
    }

    // Threshold was not exceeded
    return hits.end();
}


}; //namespace
