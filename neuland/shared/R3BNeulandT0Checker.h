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

#include <optional>

namespace R3B::Neuland
{
    class T0Checker
    {
      public:
        T0Checker() = default;
        void TestAndSet(int coarse_time);
        [[nodiscard]] auto DoCorrection(int value) const -> int;

      private:
        std::optional<int> correction_ = 0;
        std::optional<int> cached_t0_;
    };
} // namespace R3B::Neuland
