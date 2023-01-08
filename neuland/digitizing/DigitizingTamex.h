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
    class DigitizingTamex;

    namespace Tamex
    {
        class Channel;
        struct Params
        {
            Double_t fPMTThresh;             // [MeV]
            Double_t fSaturationCoefficient; // Saturation coefficient of PMTs
            Bool_t fExperimentalDataIsCorrectedForSaturation;
            Double_t fTimeRes; // time + Gaus(0., fTimeRes) [ns]
            Double_t fEResRel; // Gaus(e, fEResRel * e) []
            Double_t fEnergyGain;
            Double_t fPedestal;
            Double_t fTimeMax;
            Double_t fTimeMin;
            Double_t fQdcMin;
            TRandom3* fRnd;

            Params(TRandom3*);
        };

        class TmxPeak
        {
          public:
            TmxPeak();
            TmxPeak(const Digitizing::PMTHit&, Channel*);

            // Getters:
            Double_t GetWidth() const { return fWidth; }
            Double_t GetQDC() const { return fQdc; }
            Double_t GetTime() const { return fTime; }

            bool operator==(const TmxPeak&) const;
            void operator+=(TmxPeak&);
            Double_t QdcToWidth(Double_t) const;
            explicit operator Digitizing::Channel::Signal() const;
            bool valid() const { return cachedSignal.valid(); }

          private:
            Double_t fWidth = 0.0; // the temperal width of the TmxPeak in [ns]
            Double_t fQdc = 0.0;   // the qdc value in [MeV] (without threshold)
            Double_t fTime = 0.0;  // leading edge of the TmxPeak in [ns]
            Channel* fChannel;     // pointer to the channel that generates this peak
            mutable Validated<Digitizing::Channel::Signal> cachedSignal;
        };

        class Channel : public Digitizing::Channel
        {
            friend class Neuland::DigitizingTamex;

          public:
            ~Channel() override = default;
            void AddHit(Double_t mcTime, Double_t mcLight, Double_t dist) override;
            bool HasFired() const override;

            // Getters:
            const Tamex::Params& GetPar() const { return par; }
            const std::vector<TmxPeak>& GetTmxPeaks() const { return fTmxPeaks; }
            const Double_t GetTrigTime() const override; 

            void SetPaddle(Digitizing::Paddle* paddle) override;
            Signal TmxPeakToSignal(const TmxPeak& peak) const;

          private:
            mutable std::vector<TmxPeak> fTmxPeaks;
            mutable Validated<Double_t> fTrigTime;
            Tamex::Params par;

            // private ctor. Only constructable via DigitizingEngine.
            Channel(TRandom3*, const SideOfChannel);
            Int_t CheckOverlapping(TmxPeak&) const;
            Int_t RecheckOverlapping(Int_t index);
            void RemovePeakAt(Int_t index) const;
            void RemoveZero(std::vector<Signal>&) const;
            Double_t ToQdc(Double_t) const;
            Double_t ToTdc(Double_t) const;
            Double_t ToEnergy(Double_t) const;
            void ConstructSignals() const override;
        };

    } // namespace Tamex

    class DigitizingTamex : public DigitizingEngine
    {
      public:
        DigitizingTamex();
        ~DigitizingTamex() override = default;
        std::unique_ptr<Digitizing::Channel> BuildChannel(Digitizing::Channel::SideOfChannel side) override;

      private:
        // single random generator is shared by all paddles.
        std::unique_ptr<TRandom3> fRnd;
    };
} // namespace Neuland

#endif // R3BROOT_DIGITIZINGTAMEX_H
