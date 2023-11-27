
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

#pragma once

#include <FairParSet.h>
#include <FairRuntimeDb.h>

namespace R3B::Neuland
{
    class ParSet : public FairParSet
    {
      public:
        explicit ParSet(std::string_view name = "parSet",
                        std::string_view title = "Neuland parameter",
                        std::string_view context = "TestDefaultContext",
                        bool own = true);

        // rule of 5
        ParSet(const ParSet&) = default;
        ParSet(ParSet&&) = default;
        auto operator=(const ParSet&) -> ParSet& = default;
        auto operator=(ParSet&&) -> ParSet& = default;
        ~ParSet() override = default;

        void Write2File() { FairParSet::write(); }

      private:
        auto init(FairParIo* inputFile) -> bool override;
        auto write(FairParIo* outputFile) -> int override;
        void print() override{};

      public:
        ClassDefOverride(ParSet, 1);
    };
} // namespace R3B::Neuland
