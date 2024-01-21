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
#include <unordered_map>

namespace R3B::Neuland
{
    class T0Entry
    {
      public:
        T0Entry() = default;
        explicit T0Entry(int t0_value);
        void SetInitial(int t0_value);
        [[nodiscard]] auto GetCorrection(int coarse_time) const -> int;

      private:
        std::optional<int> t0_ref_;
    };

    class T0Checker
    {
      public:
        T0Checker() = default;
        void Add(int module_num, int t0_value);
        auto GetCorrection(int module_num, int t0_value) const -> int;

      private:
        std::unordered_map<int, T0Entry> entries_;
    };

} // namespace R3B::Neuland
