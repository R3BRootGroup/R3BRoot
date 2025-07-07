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

// #include <R3BNeulandJsonHeader.h> // NOLINT
#include <R3BNeulandParSet.h>
#include <Rtypes.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace R3B::Neuland
{
    class CalibrationBasePar : public Neuland::ParSet
    {
      public:
        using TrigIDMap = std::unordered_map<int, std::pair<int, int>>; //<! key: bar_number. Value: module number on
                                                                        // the left (first) and right (second) side.
        explicit CalibrationBasePar(std::string_view name = "NeulandCalibrationBasePar",
                                    std::string_view title = "Neuland calibration base par",
                                    std::string_view context = "TestDefaultContext",
                                    bool own = true)
            : Neuland::ParSet(name.data(), title.data(), context.data(), own)
        {
        }

        [[nodiscard]] auto get_trig_id_map() const -> const auto& { return trigIDMap_; }
        [[nodiscard]] auto get_exp_ids() const -> const auto& { return exp_ids_; }
        auto get_offspill_tpat_pos() const -> int { return offspill_tpat_pos_; }
        auto get_num_of_planes() const -> int { return num_of_planes_; }

        // setters:
        void set_num_of_planes(int num_of_plane) { num_of_planes_ = num_of_plane; }
        void set_exp_ids(const std::string& exp_ids) { exp_ids_ = exp_ids; }
        void set_offspill_tpat_pos(int pos) { offspill_tpat_pos_ = pos; }
        void set_trig_id_map(TrigIDMap trigIDMap) { trigIDMap_ = std::move(trigIDMap); }

      private:
        std::string exp_ids_;
        int offspill_tpat_pos_ = 0;
        int num_of_planes_ = 0;
        TrigIDMap trigIDMap_;

      public:
        ClassDefOverride(CalibrationBasePar, 1);
    };
} // namespace R3B::Neuland
