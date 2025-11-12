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

#include "R3BActafCalData.h"
#include <fmt/core.h>

R3BActafCalData::R3BActafCalData(UInt_t pad,
                                 double energy,
                                 double maxampl,
                                 double leadingedge,
                                 double zpos,
                                 double syntime,
                                 const std::array<double, ACTAF_BINS>& trace,
                                 double rmsRaw,
                                 double rms,
                                 double meanRaw,
                                 double mean)
    : fPad(pad)
    , fEnergy(energy)
    , fEMaxAmpl(maxampl)
    , fLeadingEdge(leadingedge)
    , fZPos(zpos)
    , fSynTime(syntime)
    , fTrace(trace)
    , fRmsRaw(rmsRaw)
    , fRms(rms)
    , fMeanRaw(meanRaw)
    , fMean(mean)
{
}

std::string R3BActafCalData::toString() const
{
    std::string trace_str = "[";
    for (size_t i = 0; i < fTrace.size(); ++i)
    {
        trace_str += std::to_string(fTrace[i]);
        if (i + 1 < fTrace.size())
            trace_str += ", ";
    }
    trace_str += "]";

    return fmt::format("Pad: {}, Energy : {}, Max-ampl : {}, Leading-Time: {}, Z-Pos: {}, Syn-Time: {}",
                       GetPad(),
                       GetEnergy(),
                       GetEMaxAmpl(),
                       GetLeadingEdgeTime(),
                       GetZpos(),
                       GetSynTime(),
                       trace_str);
}

void R3BActafCalData::Print(const Option_t*) const { std::cout << *this << std::endl; }

std::ostream& operator<<(std::ostream& os, const R3BActafCalData& data)
{
    os << data.toString();
    return os;
}

ClassImp(R3BActafCalData)
