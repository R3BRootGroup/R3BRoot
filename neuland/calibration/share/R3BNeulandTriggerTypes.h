#pragma once

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

#include <R3BNeulandCommon.h>
#include <bitset>

constexpr auto TPAT_BITSIZE = 16;

namespace R3B::Neuland
{
    enum class CalTrigger
    {
        unrecognised,
        onspill,
        offspill,
        allspill,
        all
    };

    inline constexpr auto Int2CalTrigger(int value) -> CalTrigger
    {
        switch (value)
        {
            case 1:
                return CalTrigger::onspill;
            case 2:
                return CalTrigger::offspill;
            case -1:
                return CalTrigger::all;
            default:
                return CalTrigger::unrecognised;
        }
    }

    // off_spill_bit is 1 based
    inline constexpr auto CalTrigger2Tpat(CalTrigger cal_trigger, int off_spill_bit) -> std::bitset<TPAT_BITSIZE>
    {
        switch (cal_trigger)
        {
            case CalTrigger::onspill:
                return std::bitset<TPAT_BITSIZE>{}.set(NeulandOnSpillTpatPos);
            case CalTrigger::offspill:
                return std::bitset<TPAT_BITSIZE>{}.set(off_spill_bit - 1);
            case CalTrigger::allspill:
            {
                auto tpat = std::bitset<TPAT_BITSIZE>{};
                tpat.set(NeulandOnSpillTpatPos);
                tpat.set(off_spill_bit - 1);
                return tpat;
            }
            case CalTrigger::all:
                return std::bitset<TPAT_BITSIZE>{}.set();
            default:
                return std::bitset<TPAT_BITSIZE>{};
        }
    }

    inline auto CheckTriggerWithTpat(CalTrigger trigger, int tpat, int off_spill_bit) -> bool
    {
        if (trigger == CalTrigger::all)
        {
            return true;
        }
        const auto tpatID = std::bitset<TPAT_BITSIZE>(tpat);
        return (tpatID & CalTrigger2Tpat(trigger, off_spill_bit)).any();
    }

    inline auto CalTrigger2Str(CalTrigger cal_trigger) -> std::string
    {
        switch (cal_trigger)
        {
            case CalTrigger::onspill:
                return std::string{ "onspill" };
            case CalTrigger::offspill:
                return std::string{ "offspill" };
            case CalTrigger::allspill:
                return std::string{ "allspill" };
            case CalTrigger::all:
                return std::string{ "all" };
            default:
                return std::string{ "error" };
        }
    }
} // namespace R3B::Neuland
