#include "R3BNeulandCalMonitorTask.h"
#include "R3BDataMonitor.h"
#include "R3BNeulandCalibrationTask.h"
#include "R3BNeulandCommon.h"
#include "R3BNeulandCommonFunc.h"
#include <FairRootManager.h>
#include <algorithm>
#include <fmt/format.h>
#include <functional>
#include <ranges>
#include <string_view>

namespace R3B::Neuland
{

    constexpr auto MINIMAL_N_PLANE = 1;
    CalMonitorTask::CalMonitorTask(std::string_view cal_data_name)
        : CalibrationTask("R3BNeulandCalMonitorTask", 1)
        , cal_data_{ cal_data_name }
    {
    }

    void CalMonitorTask::ExtraInit([[maybe_unused]] FairRootManager* rootMan) { cal_data_.init(); }

    void CalMonitorTask::HistogramInit(DataMonitor& histograms)
    {
        const auto n_plane = GetBasePar()->get_num_of_planes();

        hist_module_hit_num_ = histograms.add_hist<TH2D>("module_hit_num",
                                                         "Number of hits for each module",
                                                         n_plane,
                                                         -0.5,
                                                         -0.5 + n_plane,
                                                         BarsPerPlane,
                                                         -0.5,
                                                         -0.5 + BarsPerPlane);
        hist_module_hit_num_->GetXaxis()->SetTitle("Plane ID");
        hist_module_hit_num_->GetYaxis()->SetTitle("Bar ID");

        for (auto planes_num : std::views::iota(MINIMAL_N_PLANE, n_plane + 1))
        {
            auto* hist =
                hist_module_hit_n_planes_
                    .try_emplace(planes_num,
                                 histograms.add_hist<TH2D>(
                                     fmt::format("module_hit_{}_planes", planes_num),
                                     fmt::format("Number of hits for each module if {} planes hit", planes_num),
                                     n_plane,
                                     -0.5,
                                     -0.5 + n_plane,
                                     BarsPerPlane,
                                     -0.5,
                                     -0.5 + BarsPerPlane)

                                     )
                    .first->second;
            hist->GetXaxis()->SetTitle("Plane ID");
            hist->GetYaxis()->SetTitle("Bar ID");
        }
        static constexpr auto MAX_MODULE_NUM = 200;
        hist_n_module_n_planes_ = histograms.add_hist<TH2D>("n_module_n_planes",
                                                            "Number of hit modules vs n hit planes",
                                                            n_plane + 1,
                                                            -0.5,
                                                            0.5 + n_plane,
                                                            MAX_MODULE_NUM,
                                                            0.5,
                                                            0.5 + MAX_MODULE_NUM);
        hist_n_module_n_planes_->GetXaxis()->SetTitle("Plane ID");
        hist_n_module_n_planes_->GetYaxis()->SetTitle("Number of modules");

        hist_module_hit_n_half_planes_ =
            histograms.add_hist<TH2D>("module_hit_n_half_planes",
                                      "Number of hits for each module if first 13 planes hit",
                                      n_plane,
                                      -0.5,
                                      -0.5 + n_plane,
                                      BarsPerPlane,
                                      -0.5,
                                      -0.5 + BarsPerPlane);
        hist_plane_hit_num_ =
            histograms.add_hist<TH1D>("plane_hit_num", "Number of hit planes", n_plane + 1, -0.5, 0.5 + n_plane);
        hist_plane_hit_num_->GetXaxis()->SetTitle("Number of planes hit");
        hist_plane_hit_num_->GetYaxis()->SetTitle("Counts");

        constexpr static auto MAX_HIT_SIZE = 10;
        hist_bar_hit_sizes_ = histograms.add_hist<TH1D>(
            "hit_sizes_of_bar", "Hit sizes of each bar", MAX_HIT_SIZE + 1, -0.5, 0.5 + MAX_HIT_SIZE);
        hist_bar_hit_sizes_->GetXaxis()->SetTitle("Number of hits in the bar");
        hist_bar_hit_sizes_->GetYaxis()->SetTitle("Counts");
    }

    void CalMonitorTask::TriggeredExec()
    {
        const auto n_plane = GetBasePar()->get_num_of_planes();
        plane_counter_.clear();
        plane_counter_.resize(n_plane, 0);

        for (const auto& bar_data : cal_data_)
        {
            if (bar_data.left.size() != bar_data.right.size())
            {
                continue;
            }
            hist_bar_hit_sizes_->Fill(static_cast<double>(bar_data.left.size()));
            if (bar_data.left.size() != 1)
            {
                continue;
            }
            const auto module_num = bar_data.module_num;
            const auto plane_id = Common::ModuleID2PlaneID(module_num - 1);
            const auto bar_id = Common::ModuleNum2BarID(module_num);
            hist_module_hit_num_->Fill(plane_id, bar_id);
            ++plane_counter_.at(plane_id);
        }

        const auto n_hit_plane = std::ranges::count_if(plane_counter_, [](auto val) -> bool { return val != 0; });
        const auto n_hit_plane_half = std::ranges::count_if(plane_counter_ | std::views::take(n_plane / 2),
                                                            [](auto val) -> bool { return val != 0; });
        const auto n_hit_modules = std::ranges::fold_left(plane_counter_, 0, std::plus{});
        hist_plane_hit_num_->Fill(static_cast<double>(n_hit_plane));
        hist_n_module_n_planes_->Fill(static_cast<double>(n_hit_plane), n_hit_modules);

        // auto has_bar = false;
        // auto has_bar_1 = false;
        for (const auto& bar_data : cal_data_)
        {
            if (bar_data.left.size() != bar_data.right.size())
            {
                continue;
            }
            hist_bar_hit_sizes_->Fill(static_cast<double>(bar_data.left.size()));
            if (bar_data.left.size() != 1)
            {
                continue;
            }
            const auto module_num = bar_data.module_num;
            const auto plane_id = Common::ModuleID2PlaneID(module_num - 1);
            const auto bar_id = Common::ModuleNum2BarID(module_num);
            if (n_hit_plane >= MINIMAL_N_PLANE)
            {
                hist_module_hit_n_planes_.at(static_cast<int>(n_hit_plane))->Fill(plane_id, bar_id);
            }
            if (n_hit_plane_half == n_plane / 2)
            {
                hist_module_hit_n_half_planes_->Fill(plane_id, bar_id);
            }
            // if(module_num == 501 or module_num == 550)
            // {
            //     has_bar = true;
            // }
            // if(module_num == 701 or module_num == 750)
            // {
            //     has_bar_1 = true;
            // }
        }
        // if (n_hit_plane == 12 and has_bar)
        // {
        //     fmt::println("------------left----------------");
        //     fmt::println("plane_id: {:>2}", fmt::join(std::views::iota(0, n_plane), ", "));
        //     fmt::println("vec     : {:>2}", fmt::join(plane_counter_, ", "));
        //     fmt::println("module_n: {:>2}", fmt::join(cal_data_ | std::views::transform([](const auto&
        //     cal_data){return cal_data.module_num;}), ", "));
        // }
        // if (n_hit_plane == 12 and has_bar_1)
        // {
        //     fmt::println("------------right----------------");
        //     fmt::println("plane_id: {:>2}", fmt::join(std::views::iota(0, n_plane), ", "));
        //     fmt::println("vec     : {:>2}", fmt::join(plane_counter_, ", "));
        //     fmt::println("module_n: {:>2}", fmt::join(cal_data_ | std::views::transform([](const auto&
        //     cal_data){return cal_data.module_num;}), ", "));
        // }
    }
} // namespace R3B::Neuland
