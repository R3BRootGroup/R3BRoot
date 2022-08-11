/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#ifndef NEULAND_DIGITIZING_TACQUILA_H
#define NEULAND_DIGITIZING_TACQUILA_H

#include "DigitizingEngine.h"
#include "TRandom3.h"
#include "Validated.h"

namespace Neuland
{
    namespace TacQuila
    {
        struct Params
        {
            Double_t fPMTThresh;             // [MeV]
            Double_t fSaturationCoefficient; //
            Bool_t fExperimentalDataIsCorrectedForSaturation;
            Double_t fTimeRes;         // time + Gaus(0., fTimeRes) [ns]
            Double_t fEResRel;         // Gaus(e, fEResRel * e) []
            Double_t fIntegrationTime; // [ns]
            std::shared_ptr<TRandom3> fRnd;

            Params();
        };

        class Channel : public Digitizing::Channel
        {
          public:
            explicit Channel(const TacQuila::Params&);
            ~Channel() override = default;
            void AddHit(Double_t mcTime, Double_t mcLight, Double_t dist) override;
            bool HasFired() const override;
            Double_t GetQDC(UShort_t index) const override;
            Double_t GetTDC(UShort_t index) const override;
            Double_t GetEnergy(UShort_t index) const override;

          private:
            // NOTE: Some expensive calculations and random distributions are cached
            // so they do not need to be recomputed every time a Getter is called
            mutable Validated<std::vector<Digitizing::PMTHit>::const_iterator> cachedFirstHitOverThresh;
            std::vector<Digitizing::PMTHit>::const_iterator FindThresholdExceedingHit() const;

            Double_t BuildQDC() const;
            mutable Validated<Double_t> cachedQDC;

            Double_t BuildTDC() const;
            mutable Validated<Double_t> cachedTDC;

            Double_t BuildEnergy() const;
            mutable Validated<Double_t> cachedEnergy;

            const TacQuila::Params& par;
        };

    } // namespace TacQuila

    class DigitizingTacQuila : public DigitizingEngine
    {
      public:
        DigitizingTacQuila();
        ~DigitizingTacQuila() override = default;
        std::unique_ptr<Digitizing::Channel> BuildChannel() override;

        void SetPMTThreshold(const Double_t v) { fTQP.fPMTThresh = v; }
        void SetSaturationCoefficient(const Double_t v) { fTQP.fSaturationCoefficient = v; }
        void SetExperimentalDataIsCorrectedForSaturation(const Bool_t v)
        {
            fTQP.fExperimentalDataIsCorrectedForSaturation = v;
        }
        void SetTimeRes(const Double_t v) { fTQP.fTimeRes = v; }
        void SetERes(const Double_t v) { fTQP.fEResRel = v; }
        void SetIntegrationTime(const Double_t v) { fTQP.fIntegrationTime = v; }

      private:
        TacQuila::Params fTQP;
    };
} // namespace Neuland

#endif // NEULAND_DIGITIZING_TACQUILA_H
