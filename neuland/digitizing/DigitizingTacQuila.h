#ifndef NEULAND_DIGITIZING_TACQUILA_H
#define NEULAND_DIGITIZING_TACQUILA_H

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

#include "TRandom3.h"

#include "DigitizingEngine.h"
#include "Validated.h"

namespace Neuland
{
    class DigitizingTacQuila : public DigitizingEngine
    {
        struct Params
        {
            Double_t fPaddleHalfLength;      // [cm]
            Double_t fPMTThresh;             // [MeV]
            Double_t fSaturationCoefficient; //
            Double_t fTimeRes;               // time + Gaus(0., fTimeRes) [ns]
            Double_t fEResRel;               // Gaus(e, fEResRel * e) []
            Double_t fIntegrationTime;       // [ns]
            Double_t fcMedium;               // speed of light in material in [cm/ns]
            Double_t fAttenuation;           // light attenuation of plastic scintillator [1/cm]
            Double_t fLambda;                // decay constant of BC408 scintillator [1/ns]
            std::unique_ptr<TRandom3> fRnd;

            Params();
        };

        struct PMTHit
        {
            Double_t time;
            Double_t light;

            bool operator<(const PMTHit& rhs) const { return (time < rhs.time); }
            PMTHit(const Double_t mcTime, const Double_t mcLight, const Double_t dist, const Params&);
        };

      public:
        class PMT : public DigitizingEngine::PMT
        {
          private:
            std::vector<PMTHit> hits;
            // NOTE: Some expensive calculations and random distributions are cached
            // so they do not need to be recomputed every time a Getter is called
            mutable Validated<std::vector<PMTHit>::const_iterator> cachedFirstHitOverThresh;
            mutable Validated<Double_t> cachedQDC;
            mutable Validated<Double_t> cachedTDC;
            mutable Validated<Double_t> cachedEnergy;
            const Params& par;

          public:
            PMT(const Params&);
            void AddHit(const Double_t mcTime, const Double_t mcLight, const Double_t dist) override;
            bool HasFired() const override;
            Double_t GetQDC() const override;
            Double_t GetTDC() const override;
            Double_t GetEnergy() const override;

          private:
            Double_t BuildQDC() const;
            Double_t BuildTDC() const;
            Double_t BuildEnergy() const;
            std::vector<PMTHit>::const_iterator FindThresholdExceedingHit() const;
        };

      public:
        class Paddle : public DigitizingEngine::Paddle
        {
          private:
            const Params& par;
            PMT leftPMT;
            PMT rightPMT;

          public:
            Paddle(const Params&);
            Double_t GetEnergy() const override;
            Double_t GetTime() const override;
            Double_t GetPosition() const override;
            bool HasFired() const override;

            void DepositLight(const Double_t time, const Double_t light, const Double_t dist) override;

            const DigitizingEngine::PMT* GetLeftPMT() const override { return &leftPMT; }
            const DigitizingEngine::PMT* GetRightPMT() const override { return &rightPMT; }
        };

      public:
        void SetPaddleHalfLength(const Double_t v) override { par.fPaddleHalfLength = v; }

        void SetPMTThreshold(const Double_t v) { par.fPMTThresh = v; }
        void SetSaturationCoefficient(const Double_t v) { par.fSaturationCoefficient = v; }
        void SetTimeRes(const Double_t v) { par.fTimeRes = v; }
        void SetERes(const Double_t v) { par.fEResRel = v; }
        void SetIntegrationTime(const Double_t v) { par.fIntegrationTime = v; }

        void DepositLight(const Int_t paddle_id,
                          const Double_t time,
                          const Double_t light,
                          const Double_t dist) override;

        Double_t GetTriggerTime() const override;
        std::map<Int_t, std::unique_ptr<DigitizingEngine::Paddle>> ExtractPaddles() override;

      private:
        std::map<Int_t, std::unique_ptr<Paddle>> paddles;
        Params par;
    };

}; // namespace

#endif // NEULAND_DIGITIZING_TACQUILA_H
