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

#ifndef NEULAND_DIGITIZING_ENGINE_H
#define NEULAND_DIGITIZING_ENGINE_H

#include "FairLogger.h"
#include "R3BNeulandHitPar.h"
#include "Rtypes.h"
#include "Validated.h"
#include <map>
#include <memory>
#include <vector>

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

        class Paddle;

        // channels of PMT plus digitisation module. Input interaction points and output digital signals.
        class Channel
        {
          public:
            enum SideOfChannel
            {
                right = 1,
                left = 2
            };

            struct Signal
            {
                Double_t qdc{};
                Double_t tdc{};
                Double_t energy{};
                SideOfChannel side{};
                Signal() = default;
                Signal(Double_t q, Double_t t, Double_t e, SideOfChannel s)
                    : qdc{ q }
                    , tdc{ t }
                    , energy{ e }
                    , side{ s }
                {
                }
            };

            Channel(SideOfChannel);
            virtual ~Channel() = default; // FIXME: Root doesn't like pure virtual destructors (= 0;)
            virtual void AddHit(Double_t mcTime, Double_t mcLight, Double_t dist) = 0;
            virtual bool HasFired() const = 0;
            virtual void SetPaddle(Paddle* paddle) { fPaddle = paddle; };
            virtual const Double_t GetTrigTime() const = 0;

            const std::vector<Signal>& GetSignals() const;
            const SideOfChannel GetSide() const { return fSide; }

          protected:
            std::vector<PMTHit> fPMTHits;                    // input PMT hits to the channel
            const SideOfChannel fSide;                       // side of the channel
            mutable Validated<std::vector<Signal>> fSignals; // output signals from the channel
            Paddle* fPaddle;                                 // pointer to the paddle who owns this channel

          private:
            virtual void ConstructSignals() const = 0;
        };

        class Paddle
        {
          public:
            template <class T>
            struct Pair
            {
                T left;
                T right;
                Pair(T l, T r)
                    : left(l)
                    , right(r){};
            };

            struct Signal
            {
                Double_t energy{};
                Double_t time{};
                Double_t position{};
                const Channel::Signal& leftChannel;
                const Channel::Signal& rightChannel;
                Signal(const Channel::Signal& leftSignal, const Channel::Signal& rightSignal)
                    : leftChannel{ leftSignal }
                    , rightChannel{ rightSignal }
                {
                }
            };

            Paddle(const Int_t paddleID,
                   std::unique_ptr<Channel> l,
                   std::unique_ptr<Channel> r,
                   R3BNeulandHitPar* par = nullptr);
            virtual ~Paddle() {}
            void DepositLight(Double_t time, Double_t light, Double_t dist);

            bool HasFired() const;
            bool HasHalfFired() const;

            void SetHitModulePar(R3BNeulandHitPar* par);
            R3BNeulandHitModulePar* GetHitModulePar() const { return fHitModulePar; }

            Int_t GetPaddleId() const { return fPaddleId; }
            const std::vector<Signal>& GetSignals() const;

            const Channel* GetLeftChannel() const { return fLeftChannel.get(); }
            const Channel* GetRightChannel() const { return fRightChannel.get(); }

          protected:
            using ChannelSignals = std::vector<Channel::Signal>;
            using PaddleSignals = std::vector<Signal>;
            std::unique_ptr<Channel> fLeftChannel;
            std::unique_ptr<Channel> fRightChannel;
            R3BNeulandHitModulePar* fHitModulePar = nullptr;
            mutable Validated<PaddleSignals> fSignals;
            const Int_t fPaddleId;

          public:
            static constexpr Double_t gHalfLength = 135.;   // [cm]
            static constexpr Double_t gCMedium = 14.;       // speed of light in material in [cm/ns]
            static constexpr Double_t gAttenuation = 0.008; // light attenuation of plastic scintillator [1/cm]
            static constexpr Double_t gLambda = 1. / 2.1;

          private:
            virtual std::vector<Pair<int>> ConstructIndexMap(const ChannelSignals& leftSignals,
                                                             const ChannelSignals& rightSignals) const;
            virtual PaddleSignals ConstructPaddelSignals(const ChannelSignals& leftSignals,
                                                         const ChannelSignals& rightSignals,
                                                         const std::vector<Pair<int>>& indexMapping) const;
            virtual Float_t CompareSignals(const Channel::Signal& firstSignal,
                                           const Channel::Signal& secondSignal) const;
            virtual Double_t ComputeTime(const Channel::Signal& firstSignal, const Channel::Signal& secondSignal) const;
            virtual Double_t ComputeEnergy(const Channel::Signal& firstSignal,
                                           const Channel::Signal& secondSignal) const;
            virtual Double_t ComputePosition(const Channel::Signal& rightSignal,
                                             const Channel::Signal& leftSignal) const;
        };
    } // namespace Digitizing

    // abstract class, cannot be instantiated, to be used as a base class.
    class DigitizingEngine
    {
      public:
        virtual ~DigitizingEngine() = default; // FIXME: Root doesn't like pure virtual destructors (= 0;)
        virtual std::unique_ptr<Digitizing::Channel> BuildChannel(Digitizing::Channel::SideOfChannel) = 0;

        void SetHitPar(R3BNeulandHitPar* par) { fNeulandHitPar = par; }
        void DepositLight(Int_t paddle_id, Double_t time, Double_t light, Double_t dist);
        Double_t GetTriggerTime() const;
        std::map<Int_t, std::unique_ptr<Digitizing::Paddle>> ExtractPaddles();

      protected:
        std::map<Int_t, std::unique_ptr<Digitizing::Paddle>> paddles;
        R3BNeulandHitPar* fNeulandHitPar = nullptr;
    };
} // namespace Neuland

#endif // NEULAND_DIGITIZING_ENGINE_H
