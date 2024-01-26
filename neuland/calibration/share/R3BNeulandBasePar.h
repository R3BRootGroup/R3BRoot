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

#define JSON_USE_IMPLICIT_CONVERSIONS 0 // NOLINT
#include <R3BNeulandParSet.h>
#include <nlohmann/json.hpp>
#include <unordered_map>

namespace R3B::Neuland
{
    class CalibrationBasePar : public Neuland::ParSet
    {
      public:
        using TrigIDMap = std::unordered_map<unsigned int, std::pair<unsigned int, unsigned int>>;
        explicit CalibrationBasePar(std::string_view name = "NeulandCalibrationBasePar",
                                    std::string_view title = "Neuland calibration base par",
                                    std::string_view context = "TestDefaultContext",
                                    bool own = true);
        void SetTrigIDMap(TrigIDMap trigIDMap) { trigIDMap_ = std::move(trigIDMap); }

        [[nodiscard]] auto GetTrigIDMap() const -> const auto& { return trigIDMap_; }
        [[nodiscard]] auto GetExpIds() const -> const auto& { return exp_ids_; }
        auto GetOffSpillTpatPos() const -> int { return offspill_tpat_pos_; }
        auto GetNumOfPlanes() const -> int { return num_of_planes_; }

        friend void to_json(nlohmann::json& jsn, const CalibrationBasePar& par);
        friend void from_json(const nlohmann::json& jsn, CalibrationBasePar& par);

      private:
        std::string exp_ids_;
        int offspill_tpat_pos_ = 0;
        int num_of_planes_ = 0;
        TrigIDMap trigIDMap_;

      public:
        ClassDefOverride(CalibrationBasePar, 1);
    };
} // namespace R3B::Neuland
