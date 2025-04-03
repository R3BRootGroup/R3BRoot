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

#include "R3BDigitizingChannel.h"
#include "R3BShared.h"
#include <TRandom3.h>
#include <algorithm>

namespace R3B::Digitizing
{
    AbstractChannel::AbstractChannel(R3B::Side side, bool has_cal_output)
        : has_cal_output_{ has_cal_output }
        , side_(side)

    {
    }

    auto AbstractChannel::GetDefaultRandomGen() -> TRandom3&
    {
        static auto random3 = TRandom3{};
        return random3;
    }

    void AbstractChannel::Construct()
    {
        hits_.clear();
        cal_signals_.clear();

        pre_construct();
        construct_hits(hits_);
        if (has_cal_output_)
        {
            construct_cal_signals(cal_signals_);
        }
        calculate_trig_time();
    }

    void AbstractChannel::Reset()
    {
        hits_.clear();
        cal_signals_.clear();
        trig_time_ = 0.;
        signal_size_ = 0;
        extra_reset();
    }

    void AbstractChannel::calculate_trig_time()
    {
        const auto& signals = GetHits();
        auto it = std::min_element(signals.begin(),
                                   signals.end(),
                                   [](const auto& left_v, const auto& right_v) { return left_v.tdc < right_v.tdc; });
        trig_time_ = it != signals.end() ? it->tdc : MAXTIME;
    }
} // namespace R3B::Digitizing
