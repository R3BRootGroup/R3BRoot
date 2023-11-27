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

#include "R3BPaddleTamexMappedData2.h"

namespace R3B
{
    MapBarSignal::MapBarSignal(R3B::Side side, const DoubleEdgeSignal& signal)
        : MapBarSignal{}
    {
        push_back(side, signal);
    }

    PaddleTamexMappedData::PaddleTamexMappedData(unsigned int pID)
        : plane_num{ pID }
    {
    }

    void PaddleTamexMappedData::push_back(R3B::Side side, unsigned int barID, const R3B::DoubleEdgeSignal& signal)
    {
        if (auto barIter = bars.find(barID); barIter != bars.end())
        {
            auto& [_, bar] = *barIter;
            bar.push_back(side, signal);
        }
        else
        {
            bars.insert(std::make_pair(barID, R3B::MapBarSignal{ side, signal }));
        }
    }
} // namespace R3B

ClassImp(R3B::MapBarSignal);
ClassImp(R3B::PaddleTamexMappedData)
