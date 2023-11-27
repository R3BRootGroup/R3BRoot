#pragma once

#include "R3BNeulandMapToCalPar.h"
#include <FairSink.h>
#include <R3BLogger.h>
#include <R3BNeulandCommon.h>
#include <R3BShared.h>
#include <TH2I.h>
#include <fmt/core.h>
#include <map>

namespace R3B::Neuland::calibration
{
    enum class FTCalErrorMethod
    {
        exact,
        approx,
        uniform_only,
        none
    };

    auto FTType2Str(FTType type) -> std::string_view;

    class FTCalStrategy
    {
      public:
        FTCalStrategy() = default;
        using ValueErrors = TCalVFTXModulePar::ValueErrors;

        void Set_max_bin_number(unsigned int num) { max_bin_number_ = num; }
        void Set_cycle_period(double time) { cycle_period_ = time; } // ns
        void Set_error_method(FTCalErrorMethod method) { error_method_ = method; }

        [[nodiscard]] auto Get_cycle_period() const -> auto { return cycle_period_; }
        auto GetChannel2Time(TH1* hist) const -> FTChannel2TimeRelation;

      private:
        unsigned int max_bin_number_ = 0;
        double cycle_period_ = 0.; // nano seconds
        FTCalErrorMethod error_method_ = FTCalErrorMethod::none;
    };

    template <typename Derived>
    class FTBaseCal
    {
      public:
        using ValueErrors = TCalVFTXModulePar::ValueErrors;
        FTBaseCal(std::string_view hist_name, unsigned int moduleNum, const std::vector<FTType>& types)
            : hist_name_(hist_name)
            , moduleNum_{ moduleNum }
        {
            EmptyInitAllDistributions(types);
        }

        void InitAllDistributions(TH1* hist)
        {
            for (auto& [type, distribution] : fTDistribution_)
            {
                distribution = std::unique_ptr<TH1>(dynamic_cast<TH1*>(hist->Clone()));
                distribution->SetDirectory(nullptr);
                auto name = fmt::format("{0}_{1:04d}_{2}", hist_name_, moduleNum_, FTType2Str(type));
                distribution->SetNameTitle(name.c_str(), name.c_str());
            }
        }

        void WriteHist2File(TDirectory* sink)
        {
            for (const auto& [_, distribution] : fTDistribution_)
            {
                R3BLOG(
                    debug4,
                    fmt::format("Writting histogram {} into sink file {}", distribution->GetName(), sink->GetName()));
                sink->WriteObject(distribution.get(), distribution->GetName());
            }
        }

        void Write2Par(unsigned int moduleID,
                       Map2CalPar& t_cal_par,
                       std::map<FTType, FTChannel2TimeRelation> valueErrors)
        {
            auto modulePar = TCalVFTXModulePar{ moduleID };

            for (const auto& [type, rel] : valueErrors)
            {
                R3BLOG(debug4, fmt::format("type: {} CalRelSize: {}", FTType2Str(type), rel.value_error.size()));
            }

            modulePar.Set_correlation(std::move(valueErrors));
            t_cal_par.SetModuleParam(moduleID, std::move(modulePar));
        }

        // Getter:
        [[nodiscard]] auto GetDistribution(FTType type) { return fTDistribution_.at(type).get(); }
        [[nodiscard]] auto GetAllDistributions() -> const auto& { return fTDistribution_; }
        [[nodiscard]] auto GetHistName() const -> const auto& { return hist_name_; }
        [[nodiscard]] auto GetModuleNum() const -> const auto& { return moduleNum_; }

        template <typename... Args>
        inline void AddFineTime(FTType type, Args&&... args)
        {
            auto* hist = static_cast<typename Derived::HistType*>(GetDistribution(type)); // NOLINT
            hist->Fill(std::forward<Args>(args)...);
        }

      private:
        std::string_view hist_name_;
        unsigned int moduleNum_ = 0;
        // fine time distributions:
        std::map<FTType, std::unique_ptr<TH1>> fTDistribution_;

        void EmptyInitAllDistributions(const std::vector<FTType>& types)
        {
            for (const auto& type : types)
            {
                fTDistribution_.insert_or_assign(type, nullptr);
            }
        }
    };

    // calirbation class for trigger mapped data
    class ModuleCal : public FTBaseCal<ModuleCal>
    {
      public:
        using HistType = TH1I;
        explicit ModuleCal(std::string_view hist_name, unsigned int mID);
        inline void Fill(FTType type, unsigned int ftValue) { AddFineTime(type, ftValue); }
        template <typename Strategy>
        void Write_to_par(const Strategy& strategy, Map2CalPar& t_cal_par)
        {
            const auto& distributions = GetAllDistributions();
            auto correlations = std::map<FTType, FTChannel2TimeRelation>{};
            for (const auto& [ftType, distribution2D] : distributions)
            {
                auto* hist = static_cast<HistType*>(distribution2D.get()); // NOLINT
                if (hist != nullptr)
                {
                    correlations.insert_or_assign(ftType, strategy.GetChannel2Time(hist));
                }
            }
            Write2Par(GetModuleNum(), t_cal_par, std::move(correlations));
        }
    };

    // calibration class for mapped data
    class PlaneCal : public FTBaseCal<PlaneCal>
    {
      public:
        using HistType = TH2I;
        explicit PlaneCal(std::string_view hist_name, unsigned int mID);

        inline void Fill(FTType type, unsigned int ftValue, unsigned int barID) { AddFineTime(type, barID, ftValue); }

        // template <typename Strategy>
        void Write_to_par(const FTCalStrategy& strategy, Map2CalPar& t_cal_par)
        {
            const auto& distributions = GetAllDistributions();
            for (int barNum{ 1 }; barNum <= BarsPerPlane; ++barNum)
            {
                auto correlations = std::map<FTType, FTChannel2TimeRelation>{};
                for (const auto& [ftType, distribution2D] : distributions)
                {
                    auto* hist2D = static_cast<HistType*>(distribution2D.get()); // NOLINT
                    auto* hist1D = hist2D->ProjectionY("projection", barNum, barNum);
                    if (hist1D != nullptr)
                    {
                        correlations.insert_or_assign(ftType, strategy.GetChannel2Time(hist1D));
                    }
                }
                Write2Par(Neuland_PlaneBar2ModuleNum(GetModuleNum(), barNum), t_cal_par, std::move(correlations));
            }
        }
    };

    template <typename CalType>
    class FTEngine
    {
      public:
        explicit FTEngine(std::string hist_name)
            : hist_name_{ std::move(hist_name) }
        {
        }

        template <typename... AdditionalPar>
        void Fill(FTType type, unsigned int ftValue, unsigned int moduleNum, AdditionalPar&&... pars)
        {
            auto iter = cals_.find(moduleNum);
            if (iter == cals_.end())
            {
                iter = cals_.emplace(moduleNum, CalType{ hist_name_, moduleNum }).first;
            }
            iter->second.Fill(type, ftValue, std::forward<AdditionalPar>(pars)...);
        }

        template <typename Strategy>
        void Writer_to_TCalPar(const Strategy& write_strategy, Map2CalPar& t_cal_par)
        {
            for (auto& [_, cal] : cals_)
            {
                cal.Write_to_par(write_strategy, t_cal_par);
            }
        }

        void WriteHist2File(TDirectory* sink)
        {
            for (auto& [_, cal] : cals_)
            {
                cal.WriteHist2File(sink);
            }
        }

      private:
        std::map<unsigned int, CalType> cals_;
        std::string hist_name_;
    };

} // namespace R3B::Neuland::calibration
