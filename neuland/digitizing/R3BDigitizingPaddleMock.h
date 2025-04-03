/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/
#pragma once
/**
 * Simulation of Mock Bar/Paddle
 * @author Yanzhao Wang
 * @since March 13, 2023
 *
 *
 */

#include "R3BDigitizingChannel.h"
#include "R3BDigitizingPaddle.h"
#include "R3BLogger.h"
#include "R3BShared.h"
#include <RtypesCore.h>
#include <cstdint>

namespace R3B::Digitizing::Neuland
{
    class MockPaddle : public Digitizing::AbstractPaddle
    {
      public:
        explicit MockPaddle(uint16_t paddleID)
            : Digitizing::AbstractPaddle{ paddleID }
        {
        }

      private:
        [[nodiscard]] auto compute_time(const AbstractChannel::Hit& firstSignal,
                                        const AbstractChannel::Hit& secondSignal) const -> double override
        {
            return (firstSignal.tdc + secondSignal.tdc) / 2;
        }
        [[nodiscard]] auto compute_energy(const AbstractChannel::Hit& firstSignal,
                                          const AbstractChannel::Hit& secondSignal) const -> double override
        {
            return (firstSignal.qdcUnSat + secondSignal.qdcUnSat) / 2;
        }
        [[nodiscard]] auto compute_position(const AbstractChannel::Hit& leftSignal,
                                            const AbstractChannel::Hit& rightSignal) const -> double override
        {
            if (leftSignal.side == rightSignal.side)
            {
                R3BLOG(fatal, "cannot compute position with signals from same side!");
                return 0.F;
            }
            return (leftSignal.side == Side::left) ? (leftSignal.tdc - rightSignal.tdc) / 2 * gCMedium
                                                   : (rightSignal.tdc - leftSignal.tdc) / 2 * gCMedium;
        }
        [[nodiscard]] auto compute_channel_signals(const Signal& hit) const -> Pair<AbstractChannel::Signal> override
        {
            auto rightChannelHit = GenerateMockChannelHit(hit.time, hit.energy_dep, hit.distance_to_center);
            auto leftChannelHit = GenerateMockChannelHit(hit.time, hit.energy_dep, -1 * hit.distance_to_center);
            return { leftChannelHit, rightChannelHit };
        }

      public:
        static constexpr double gHalfLength = 135.; // [cm]
        static constexpr double gCMedium = 30.;     // speed of light in material in [cm/ns]
        static auto GenerateMockChannelHit(Double_t mcTime, Double_t mcLight, Double_t dist) -> AbstractChannel::Signal
        {
            auto time = mcTime - (MockPaddle::gHalfLength + dist) / MockPaddle::gCMedium;
            auto light = mcLight;
            return { time, light };
        }
    };
} // namespace R3B::Digitizing::Neuland
