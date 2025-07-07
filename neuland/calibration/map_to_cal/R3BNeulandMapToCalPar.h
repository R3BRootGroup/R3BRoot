#pragma once

#include "R3BTrigIDMappingFinder.h"
#include "R3BValueError.h"
#include <R3BNeulandParSet.h>
#include <Rtypes.h>
#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <unordered_map>
#include <utility>
#include <vector>

namespace R3B
{
    struct FTChannel2TimeRelation
    {
        double hist_overflow{};
        std::vector<ValueError<double>> value_error;
        ClassDefNV(FTChannel2TimeRelation, 1);
    };

    enum class FTType : uint8_t
    {
        leftleading,
        rightleading,
        lefttrailing,
        righttrailing,
        trigger,
    };

    class TCalVFTXModulePar
    {
      public:
        using ValueErrors = std::vector<ValueError<double>>;
        TCalVFTXModulePar() = default;
        explicit TCalVFTXModulePar(int module_num)
            : module_num_{ module_num }
        {
        }

        // Setters:
        void Set_correlation(std::map<FTType, FTChannel2TimeRelation> correlation)
        {
            finetime_correlation_ = std::move(correlation);
        }

        // Getters:
        [[nodiscard]] auto GetFineTime(FTType ftType, uint channel_Num) const -> ValueError<double>;
        [[nodiscard]] auto GetModuleNum() const { return module_num_; }

      private:
        int module_num_ = 0;
        std::map<FTType, FTChannel2TimeRelation> finetime_correlation_;

      public:
        ClassDefNV(TCalVFTXModulePar, 1);
    };

    class Map2CalPar : public Neuland::ParSet
    {
      public:
        using TrigIDMap = Neuland::TrigIDMappingFinder::TrigIDMap;
        explicit Map2CalPar(std::string_view name = "NeulandMap2CalPar",
                            std::string_view title = "Neuland Map2Cal calibration",
                            std::string_view context = "TestDefaultContext",
                            bool own = true);
        // ~R3BTCalPar2() { LOG(debug) << " R3BTCalPar2 deleted!"; }
        void AddModuleParam(int module_num, TCalVFTXModulePar modulePar);

        // Setters:
        void SetTrigEnabled(bool is_enabled) { is_trig_enabled_ = is_enabled; }
        void SetSlowClockFrequency(float frq) { slow_clock_frequency_ = frq; }
        void SetModuleParam(int module_num, TCalVFTXModulePar par)
        {
            module_params_.insert_or_assign(module_num, std::move(par));
        }

        void SetParam(std::unordered_map<int, TCalVFTXModulePar> moduleParams)
        {
            module_params_ = std::move(moduleParams);
        }

        // Getters:
        [[nodiscard]] auto GetSlowClockFrequency() const -> const auto& { return slow_clock_frequency_; }
        [[nodiscard]] auto GetParams() const -> const auto& { return module_params_; }
        [[nodiscard]] auto GetParamAt(int module_num) const -> const auto& { return module_params_.at(module_num); }
        auto HasTrigEnabled() const -> bool { return is_trig_enabled_; }

      private:
        float slow_clock_frequency_ = 0; // GHz
        bool is_trig_enabled_ = true;
        std::unordered_map<int, TCalVFTXModulePar> module_params_;

        void clear() override { module_params_.clear(); }

      public:
        ClassDefOverride(Map2CalPar, 1);
    };
} // namespace R3B

using R3BTCalPar2 = R3B::Map2CalPar;
