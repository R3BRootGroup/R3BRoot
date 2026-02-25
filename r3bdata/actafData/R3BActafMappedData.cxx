/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BActafMappedData.h"

#include <Rtypes.h>
#include <RtypesCore.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fmt/core.h>
#include <iostream>
#include <ostream>
#include <string>

R3BActafMappedData::R3BActafMappedData(UInt_t pad,
                                       const std::array<double, ACTAF_BINS>& trace,
                                       double energy,
                                       double baseline,
                                       double risetime,
                                       int maxpos,
                                       double maxamplitude,
                                       double leadingedge10,
                                       double rms,
                                       double rmsFilt,
                                       double baselineFilt,
                                       double maw)
    : fPad(pad)
    , fE(energy)
    , fBaseline(baseline)
    , fRisetime(risetime)
    , fMaxpos(maxpos)
    , fMaxamplitude(maxamplitude)
    , fTrace(trace)
    , fLeadingEdge10(leadingedge10)
    , fRms(rms)
    , fRmsFilt(rmsFilt)
    , fBaselineFilt(baselineFilt)
    , fMaw(maw)
{
}

// NOLINTNEXTLINE
R3BActafMappedData::R3BActafMappedData(UInt_t pad, int det_mask, uint64_t timetag, int spill_nb, uint64_t rawtimetag)
    : fPad(pad)
    , fDetMask(det_mask)
    , fTimeTag(timetag)
    , fSpillNb(spill_nb)
    , fRawTimeTag(rawtimetag)
{
}

std::string R3BActafMappedData::toString() const
{
    std::string trace_str = "[";
    for (size_t i = 0; i < fTrace.size(); ++i)
    {
        trace_str += std::to_string(fTrace[i]);
        if (i + 1 < fTrace.size())
            trace_str += ", ";
    }
    trace_str += "]";

    return fmt::format("Pad: {}, Trace: {}, Energy : {}, Baseline: {}, Risetime: {}, "
                       "Max-position: {}, Max-amplitude: {}, Leading-Time: {}, Rms: {}",
                       GetPad(),
                       trace_str,
                       GetE(),
                       GetBaseline(),
                       GetRisetime(),
                       GetMaxpos(),
                       GetMaxampl(),
                       GetLeadingEdgeTime(),
                       GetRms());
}

void R3BActafMappedData::Print(const Option_t* /*option*/) const { std::cout << *this << '\n'; }

std::ostream& operator<<(std::ostream& output, const R3BActafMappedData& data)
{
    output << data.toString();
    return output;
}

ClassImp(R3BActafMappedData)
