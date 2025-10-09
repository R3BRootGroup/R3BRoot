/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
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
#include <fmt/core.h>

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
                                       double baselineFilt)
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
{
}

R3BActafMappedData::R3BActafMappedData(UInt_t pad, int det_mask, int timetag, int spill_nb)
    : fPad(pad)
    , fDetMask(det_mask)
    , fTimeTag(timetag)
    , fSpillNb(spill_nb)
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

    return fmt::format(
        "Pad: {}, Trace: {}, Energy : {}, Baseline: {}, Risetime: {}, "
        "Max-position: {}, Max-amplitude: {}, Leading-Time: {}, Rms: {}, Rms Filtered: {}, Baseline Filtered {}",
        GetPad(),
        trace_str,
        GetE(),
        GetBaseline(),
        GetRisetime(),
        GetMaxpos(),
        GetMaxampl(),
        GetLeadingEdgeTime(),
        GetRms(),
        GetRmsFilt(),
        GetBaselineFilt());
}

void R3BActafMappedData::Print(const Option_t*) const { std::cout << *this << std::endl; }

std::ostream& operator<<(std::ostream& os, const R3BActafMappedData& data)
{
    os << data.toString();
    return os;
}

ClassImp(R3BActafMappedData)
