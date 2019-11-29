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

#ifndef NEULANDRECONSTRUCTIONENGINEH
#define NEULANDRECONSTRUCTIONENGINEH

#include "R3BNeulandCluster.h"
#include "R3BNeulandNeutron.h"
#include <vector>

namespace Neuland
{
    class ReconstructionEngine
    {
      public:
        virtual ~ReconstructionEngine() = default; // FIXME: Root doesn't like = 0?
        virtual void Init() {}
        virtual std::vector<R3BNeulandNeutron> GetNeutrons(const std::vector<R3BNeulandCluster*>&) const = 0;
    };
} // namespace Neuland

#endif // NEULANDRECONSTRUCTIONENGINEH
