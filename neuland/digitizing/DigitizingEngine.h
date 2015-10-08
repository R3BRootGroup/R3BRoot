#ifndef _NEULAND_DIGITIZING_ENGINE_H_
#define _NEULAND_DIGITIZING_ENGINE_H_

/* Neuland Digitizing Engine
 * @author: Jan Mayer
 *
 * The Neuland Digitizing Engine (DE) handles the detector physics and data acquisition
 * part of the conversion from raw deposited energy (that has already been converted to
 * light), to experiment-like detector hits.
 *
 * Note: This class does not depend on any R3BRoot code. The in- and output, that is the
 * the actual conversion from `LandPoints` to `LandDigis`, is handled by tasks, e.g. by
 * the R3BNeulandDigitizer.
 *
 *
 * All scintillator bars in the NeuLAND detector are equipped a photomultiplier (PMT) on
 * the left and right side (leftPMT, rightPMT), forming a Paddle. The DE handles these
 * paddles independently by id, without knowledge about physical position or orientation.
 *
 * DigitizingEngine
 *  └ map<id,paddle> paddles
 *
 * Paddle
 *  ├ PMT leftPMT
 *  └ PMT rightPMT
 *
 *
 * Each light emission created by deposition of energy is transported to the PMT and arrives
 * at a specific time and with a specific amplitude (PMTHit). Based on these PMTHits, the
 * PMT response (HasFired, TDC, QDC, Energy) is calculated based on a plethora of "variable"
 * values (e.g. threshold) and material "constants" (e.g. speed of light), see cxx file.
 *
 * PMT
 *  └ vector<PMTHit> hits;
 *
 * PMTHit
 *  ├ time
 *  └ light
 *
 */

#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <cmath>

#include "TRandom3.h"

#include "Validated.h"

namespace Neuland {

class DigitizingEngine {

protected:
    static const Double_t fSaturationCoefficient;
    static const Double_t fcMedium;
    static const Double_t fAttenuation;
    static const Double_t fIntegrationTime;
    static const Double_t fLambda;
    static const Double_t fThresh;
    static const Double_t fTimeRes;
    static Double_t fPaddleHalfLength;
    static TRandom3 *fRnd;

public:
    struct PMTHit {
        Double_t time;
        Double_t light;

        bool operator < (const PMTHit &rhs) const
        {
            return (time < rhs.time);
        }

        /* Calculate the time of arrival and the amount of light that arrives at
         * the PMT based on the deposition in the paddle */
        PMTHit(const Double_t &mcTime, const Double_t &mcLight, const Double_t &dist)
        {
            time = mcTime + (fPaddleHalfLength + dist) / fcMedium;
            light = mcLight * exp(-fAttenuation * (fPaddleHalfLength + dist));
        }
    };

    class PMT {
    private:
        std::vector<PMTHit> hits;
        // NOTE: Some expensive calculations and random distributions are cached
        // so they do not need to be recomputed every time a Getter is called
        mutable Validated<std::vector<PMTHit>::const_iterator> cachedFirstHitOverThresh;
        mutable Validated<Double_t> cachedQDC;
        mutable Validated<Double_t> cachedTDC;
        mutable Validated<Double_t> cachedEnergy;

    public:
        void AddHit(const Double_t &mcTime, const Double_t &mcLight, const Double_t &dist)
        {
            hits.push_back(PMTHit(mcTime, mcLight, dist));
            // NOTE: Sorting after every hit may not be efficient, but this way
            // FindThresholdExeeding hit can be made const
            std::sort(hits.begin(), hits.end());
            cachedFirstHitOverThresh.invalidate();
        }

        bool HasFired() const;
        Double_t GetQDC() const;
        Double_t GetTDC() const;
        Double_t GetEnergy() const;
        const std::vector<PMTHit> &GetHits() const
        {
            return hits;
        }

    private:
        std::vector<PMTHit>::const_iterator FindThresholdExceedingHit() const;
        Double_t BuildQDC() const;
        Double_t BuildTDC() const;
        Double_t BuildEnergy() const;
    };

    struct Paddle {
        PMT leftPMT;
        PMT rightPMT;

        Double_t GetPaddleEnergy() const
        {
            return sqrt(leftPMT.GetEnergy() * rightPMT.GetEnergy());
        }

        Double_t GetPaddleTime() const
        {
            return (leftPMT.GetTDC() + rightPMT.GetTDC()) / 2. - fPaddleHalfLength / fcMedium;
        }

        Double_t GetPosition() const
        {
            return (rightPMT.GetTDC() - leftPMT.GetTDC()) / 2.*fcMedium;
        }

        bool HasFired() const
        {
            return (leftPMT.HasFired() && rightPMT.HasFired());
        }


    };

public:
    std::map<Int_t, Paddle> paddles;

    DigitizingEngine();
    ~DigitizingEngine();

    void SetPaddleHalfLength(const Double_t &v)
    {
        fPaddleHalfLength = v;
    }


    void Clear()
    {
        paddles.clear();
    }


    Double_t GetTriggerTime() const;

    void DepositLight(const Int_t &paddle_id, const Double_t &time, const Double_t &light, const Double_t &dist)
    {
        if (fPaddleHalfLength == 0.) {
            throw std::invalid_argument("Paddle Legth has not been set");
        }
        paddles[paddle_id].leftPMT.AddHit(time, light, -1. * dist);
        paddles[paddle_id].rightPMT.AddHit(time, light, dist);
    }

};

}; //namespace

#endif //_NEULAND_DIGITIZING_ENGINE_H_
