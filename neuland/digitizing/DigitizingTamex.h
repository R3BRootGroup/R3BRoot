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

#ifndef R3BROOT_DIGITIZINGTAMEX_H
#define R3BROOT_DIGITIZINGTAMEX_H
/**
 * NeuLAND Tamex digitizing module with multiple hits per event
 * @author Yanzhao Wang
 * @since July 15, 2022
 *
 *
 */

#include "DigitizingEngine.h"
#include "TRandom3.h"
#include "Validated.h"
#include <iostream>

namespace Neuland
{
    namespace Tamex
    {
        struct Params
        {
            Double_t fPMTThresh;             // [MeV]
            Double_t fSaturationCoefficient; //
            Bool_t fExperimentalDataIsCorrectedForSaturation;
            Double_t fTimeRes; // time + Gaus(0., fTimeRes) [ns]
            Double_t fEResRel; // Gaus(e, fEResRel * e) []
            std::shared_ptr<TRandom3> fRnd;

            Params();
        };

        class Channel : public Digitizing::Channel
        {
          public:
            explicit Channel(const Tamex::Params&);
            ~Channel() override = default;
            void AddHit(Double_t mcTime, Double_t mcLight, Double_t dist) override;
            bool HasFired() const override;
            Double_t GetQDC(UShort_t index) const override;
            Double_t GetTDC(UShort_t index) const override;
            Double_t GetEnergy(UShort_t index) const override;
            Int_t GetNHits() const override;
            // std::vector<Digitizing::PMTHit> GetPMTHits() const;

          private:
            mutable std::vector<Validated<Double_t>> cachedQDC;
            mutable std::vector<Validated<Double_t>> cachedTDC;
            mutable std::vector<Validated<Double_t>> cachedEnergy;
            const Tamex::Params& par;
        };

    } // namespace Tamex

    class DigitizingTamex : public DigitizingEngine
    {
      public:
        DigitizingTamex();
        ~DigitizingTamex() override = default;
        std::unique_ptr<Digitizing::Channel> BuildChannel() override;

      private:
        Tamex::Params fTmP;
    };
} // namespace Neuland

#endif // R3BROOT_DIGITIZINGTAMEX_H
