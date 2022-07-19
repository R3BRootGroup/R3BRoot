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

#ifndef NEULAND_DIGITIZING_ENGINE_H
#define NEULAND_DIGITIZING_ENGINE_H

#include "Rtypes.h"
#include <map>
#include <memory>
#include <vector>
#include "FairLogger.h"

namespace Neuland
{
    namespace Digitizing
    {
        struct PMTHit
        {
            Double_t time;
            Double_t light;

            bool operator<(const PMTHit& rhs) const { return (time < rhs.time); }

            PMTHit() = default;
            PMTHit(Double_t mcTime, Double_t mcLight, Double_t dist);
        };

        class Channel
        {
          public:
            virtual ~Channel() = default; // FIXME: Root doesn't like pure virtual destructors (= 0;)
            virtual void AddHit(Double_t mcTime, Double_t mcLight, Double_t dist) = 0;
            virtual bool HasFired() const = 0;
            virtual Double_t GetQDC(UShort_t index) const {return GetQDC();}
            virtual Double_t GetTDC(UShort_t index) const {return GetTDC();}
            virtual Double_t GetEnergy(UShort_t index) const {return GetEnergy();}
            virtual Int_t GetNHits() const {return 1;};

            // for backward compatibility
            virtual Double_t GetQDC() const {return GetQDC(0);}
            virtual Double_t GetTDC() const {return GetTDC(0);}
            virtual Double_t GetEnergy() const {return GetEnergy(0);}

          protected:
            std::vector<PMTHit> fPMTHits;
        };

        class Paddle
        {
          public:
            Paddle(std::unique_ptr<Channel> l, std::unique_ptr<Channel> r);
            void DepositLight(Double_t time, Double_t light, Double_t dist);

            bool HasFired() const;
            bool HasHalfFired() const;
            Double_t GetEnergy(UShort_t index = 0) const;
            Double_t GetTime(UShort_t index = 0) const;
            Double_t GetPosition(UShort_t index = 0) const;
            const UShort_t GetNHits() const;

            const Channel* GetLeftChannel() const { return fLeftChannel.get(); }
            const Channel* GetRightChannel() const { return fRightChannel.get(); }

          protected:
            std::unique_ptr<Channel> fLeftChannel;
            std::unique_ptr<Channel> fRightChannel;

          public:
            static constexpr Double_t gHalfLength = 135.;   // [cm]
            static constexpr Double_t gCMedium = 14.;       // speed of light in material in [cm/ns]
            static constexpr Double_t gAttenuation = 0.008; // light attenuation of plastic scintillator [1/cm]
            static constexpr Double_t gLambda = 1. / 2.1;
        };
    } // namespace Digitizing

    // abstract class, cannot be instantiated, to be used as a base class.
    class DigitizingEngine
    {
      public:
        virtual ~DigitizingEngine() = default; // FIXME: Root doesn't like pure virtual destructors (= 0;)
        virtual std::unique_ptr<Digitizing::Channel> BuildChannel() = 0;

        void DepositLight(Int_t paddle_id, Double_t time, Double_t light, Double_t dist);
        Double_t GetTriggerTime() const;
        std::map<Int_t, std::unique_ptr<Digitizing::Paddle>> ExtractPaddles();

      protected:
        std::map<Int_t, std::unique_ptr<Digitizing::Paddle>> paddles;
    };
} // namespace Neuland

#endif // NEULAND_DIGITIZING_ENGINE_H
