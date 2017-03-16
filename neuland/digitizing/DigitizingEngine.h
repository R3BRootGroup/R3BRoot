#ifndef NEULAND_DIGITIZING_ENGINE_H
#define NEULAND_DIGITIZING_ENGINE_H

#include <map>
#include <memory>
#include <vector>

#include "Rtypes.h"

namespace Neuland
{
    // abstract class, cannot be instantiated, to be used as a base class.
    class DigitizingEngine
    {
      public:
        class PMT
        {
          public:
            virtual void AddHit(const Double_t mcTime, const Double_t mcLight, const Double_t dist) = 0;
            virtual bool HasFired() const = 0;
            virtual Double_t GetQDC() const = 0;
            virtual Double_t GetTDC() const = 0;
            virtual Double_t GetEnergy() const = 0;

            virtual ~PMT() {}
        };

      public:
        class Paddle
        {
          public:
            virtual void DepositLight(const Double_t time, const Double_t light, const Double_t dist) = 0;

            virtual Double_t GetEnergy() const = 0;
            virtual Double_t GetTime() const = 0;
            virtual Double_t GetPosition() const = 0;
            virtual bool HasFired() const = 0;

            virtual const PMT* GetLeftPMT() const = 0;
            virtual const PMT* GetRightPMT() const = 0;

            virtual ~Paddle() {}
        };

      public:
        virtual void SetPaddleHalfLength(const Double_t) = 0;

        virtual void DepositLight(const Int_t paddle_id,
                                  const Double_t time,
                                  const Double_t light,
                                  const Double_t dist) = 0;

        virtual Double_t GetTriggerTime() const = 0;
        virtual std::map<Int_t, std::unique_ptr<Paddle>> ExtractPaddles() = 0;

        virtual ~DigitizingEngine() {}
    };
}

#endif // NEULAND_DIGITIZING_ENGINE_H
