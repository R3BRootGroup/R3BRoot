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

#include "DigitizingEngine.h"
#include "FairLogger.h"
#include <algorithm>
#include <cmath>

namespace Neuland
{
    namespace Digitizing
    {
        // Calculate the time of arrival and the amount of light that arrives at
        // the PMT based on the deposition in the paddle
        PMTHit::PMTHit(const Double_t mcTime, const Double_t mcLight, const Double_t dist)
        {
            time = mcTime + (Paddle::gHalfLength + dist) / Paddle::gCMedium;
            light = mcLight * exp(-Paddle::gAttenuation * (Paddle::gHalfLength + dist));
        }

        Paddle::Paddle(std::unique_ptr<Channel> l, std::unique_ptr<Channel> r)
            : fLeftChannel(std::move(l))
            , fRightChannel(std::move(r))
        {
        }

        void Paddle::DepositLight(const Double_t time, const Double_t light, const Double_t dist)
        {
            fLeftChannel->AddHit(time, light, -1. * dist);
            fRightChannel->AddHit(time, light, dist);
        }

        bool Paddle::HasFired() const { return (fLeftChannel->HasFired() && fRightChannel->HasFired()); }

        bool Paddle::HasHalfFired() const
        {
            return (fLeftChannel->HasFired() && !fRightChannel->HasFired()) ||
                   (!fLeftChannel->HasFired() && fRightChannel->HasFired());
        }

        Double_t Paddle::GetEnergy(UShort_t index) const
        {
            return std::sqrt(fLeftChannel->GetEnergy(index) * fRightChannel->GetEnergy(index));
        }

        Double_t Paddle::GetTime(UShort_t index) const
        {
            return (fLeftChannel->GetTDC(index) + fRightChannel->GetTDC(index)) / 2. - gHalfLength / gCMedium;
        }

        Double_t Paddle::GetPosition(UShort_t index) const
        {
            return (fRightChannel->GetTDC(index) - fLeftChannel->GetTDC(index)) / 2. * gCMedium;
        }

        const UShort_t Paddle::GetNHits() const
        {
            if (fRightChannel->GetNHits() != fLeftChannel->GetNHits())
            {
                LOG(ERROR) << "DigitizingEngine: nhits from both side of PMTs don't match!";
                return 0;
            }
            else
            {
                return fRightChannel->GetNHits();
            }
        }
    } // namespace Digitizing

    void DigitizingEngine::DepositLight(const Int_t paddle_id,
                                        const Double_t time,
                                        const Double_t light,
                                        const Double_t dist)
    {
        if (paddles.find(paddle_id) == paddles.end())
        {
            paddles[paddle_id] =
                std::unique_ptr<Digitizing::Paddle>(new Digitizing::Paddle(this->BuildChannel(), this->BuildChannel()));
        }
        paddles.at(paddle_id)->DepositLight(time, light, dist);
    }

    Double_t DigitizingEngine::GetTriggerTime() const
    {
        Double_t triggerTime = 1e100;
        for (const auto& kv : paddles)
        {
            const auto& paddle = kv.second;

            // TODO: Should be easier with std::min?
            if (paddle->GetLeftChannel()->HasFired() && paddle->GetLeftChannel()->GetTDC(0) < triggerTime)
            {
                triggerTime = paddle->GetLeftChannel()->GetTDC(0);
            }
            if (paddle->GetRightChannel()->HasFired() && paddle->GetRightChannel()->GetTDC(0) < triggerTime)
            {
                triggerTime = paddle->GetRightChannel()->GetTDC(0);
            }
        }
        return triggerTime;
    }

    std::map<Int_t, std::unique_ptr<Digitizing::Paddle>> DigitizingEngine::ExtractPaddles()
    {
        return std::move(paddles);
    }

} // namespace Neuland
