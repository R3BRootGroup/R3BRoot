#include "R3BNeulandTSyncAnalysis.h"
#include "R3BDataMonitor.h"
#include "R3BException.h"
#include "R3BNeulandCalToHitPar.h"
#include "R3BNeulandCommon.h"
#include "R3BNeulandCommonFunc.h"
#include "R3BValueError.h"
#include <array>
#include <cstdlib>
#include <fairlogger/Logger.h>
#include <fmt/format.h>
#include <functional>
#include <magic_enum/magic_enum.hpp>
#include <root/TH1.h>
#include <root/TH2.h>
#include <unordered_map>
#include <utility>
#include <vector>

// NOLINTBEGIN(misc-include-cleaner)
#include <R3BFormatters.h>
#include <fmt/ranges.h>
#include <range/v3/algorithm/adjacent_find.hpp>
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/any_of.hpp>
#include <range/v3/algorithm/count_if.hpp>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/max_element.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/map.hpp>
#include <range/v3/view/transform.hpp>
// NOLINTEND(misc-include-cleaner)

namespace R3B::Neuland::Calibration
{
    namespace
    {
        auto extract_mean_value(TH2D* histogram, int module_num) -> R3B::ValueErrorD
        {
            // use 0.9 quantiles to remove outliners
            static constexpr auto quantile_ratio = 0.9;
            auto* hist_y_proj = histogram->ProjectionY("_py", module_num, module_num);
            if (hist_y_proj->GetEntries() == 0)
            {
                LOGP(warn, "Histogram {} has not entry at the bar number {}", histogram->GetName(), module_num);
                return {};
            }
            const auto x_pos = calculate_hist_quantiles(hist_y_proj, quantile_ratio);
            hist_y_proj->GetXaxis()->SetRangeUser(x_pos[0], x_pos[1]);
            return { hist_y_proj->GetMean(), hist_y_proj->GetStdDev() };
        }

        void offset_to_tsync_ref_bar(Cal2HitPar& hit_par)
        {
            const auto ref_t_sync = hit_par.GetModuleParAt(DEFAULT_TSYNC_REFERENCE_BAR_NUM).t_sync;

            for (auto& [module_number, module_par] : hit_par.GetListOfModuleParRef())
            {
                module_par.t_sync -= ref_t_sync;
            }
        }

        template <typename ViewType>
        auto check_all_same_view(ViewType view) -> bool
        {
            auto is_ok = ranges::adjacent_find(view, std::not_equal_to{}) == view.end();
            return is_ok;
        }
    } // namespace

    void TSyncEngine::init()
    {
        if (num_of_modules_ == 0)
        {
            throw R3B::logic_error("Number of modules is not set!");
        }

        num_of_dp_ = num_of_modules_ / BarsPerPlane / 2;

        for (const auto double_plane_id : ranges::views::iota(0, num_of_dp_))
        {
            const auto plane_id = double_plane_id * 2;
            const auto local_ref_module_num = (plane_id + 1) * BarsPerPlane;
            ref_bars_in_planes_.try_emplace(plane_id, local_ref_module_num);
        }
    }

    void TSyncEngine::init_hist(DataMonitor& data_monitor)
    {
        hist_horizontal_plane_ = data_monitor.add_hist<TH2D>("hist_horizontal_plane",
                                                             "Time diff values of the bars in a horizontal plane",
                                                             num_of_modules_,
                                                             0.5,
                                                             0.5 + num_of_modules_,
                                                             TIME_SUM_BIN_NUM,
                                                             -max_time_difference_,
                                                             max_time_difference_);
        hist_horizontal_dp_ = data_monitor.add_hist<TH2D>("hist_horizontal_dp",
                                                          "Time diff values of the bars in a horizontal double plane",
                                                          num_of_modules_,
                                                          0.5,
                                                          0.5 + num_of_modules_,
                                                          TIME_SUM_BIN_NUM,
                                                          -max_time_difference_,
                                                          max_time_difference_);
        hist_vertical_dp_ = data_monitor.add_hist<TH2D>("hist_vertical_dp",
                                                        "Time diff values of the bars in a vertical double plane",
                                                        num_of_modules_,
                                                        0.5,
                                                        0.5 + num_of_modules_,
                                                        TIME_SUM_BIN_NUM,
                                                        -max_time_difference_,
                                                        max_time_difference_);

        hist_record_type_ = data_monitor.add_hist<TH1I>("hist_record_type", "TSync record types", 1, 0., 0.);
    }

    void TSyncEngine::add_point(double t_sum, int module_num)
    {
        if (module_num == 0)
        {
            return;
        }
        buffer_points_.emplace_back(Point{ .module_num = module_num, .t_mean = t_sum / 2 });
    }

    void TSyncEngine::end_of_event()
    {
        analyze_event();

        if (hist_record_type_ == nullptr)
        {
            return;
        }
        hist_record_type_->Fill(magic_enum::enum_name(record_type_).data(), 1);
        if (record_type_ != RecordType::invalid)
        {
            LOGP(debug,
                 "Filling data of record type {:?} with bar numbers: [{}]",
                 magic_enum::enum_name(record_type_),
                 fmt::join(
                     buffer_points_ | ranges::views::transform([](const Point& point) { return point.module_num; }),
                     ", "));
            fill_histogram(record_type_);
        }
    }

    void TSyncEngine::fill_histogram(RecordType record_type)
    {
        if (buffer_ref_time_ == 0.)
        {
            LOGP(error, "reference time is still 0.");
        }
        LOGP(debug, "reference time is {}", buffer_ref_time_);
        LOGP(debug,
             "Filling data of record type {:?} with time values: [{}]",
             magic_enum::enum_name(record_type_),
             fmt::join(buffer_points_ | ranges::views::transform([](const Point& point) { return point.t_mean; }),
                       ", "));

        for (const auto& point : buffer_points_)
        {
            // calculate difference to the reference bar
            const auto reduced_t_mean = point.t_mean - buffer_ref_time_;

            if (max_time_difference_ > 0. and std::abs(reduced_t_mean) > max_time_difference_)
            {
                continue;
            }

            switch (record_type)
            {
                case RecordType::xy_plane:
                {
                    fill_one_plane_histogram(reduced_t_mean, point.module_num);
                    break;
                }
                case RecordType::same_horizontal_dp:
                case RecordType::same_vertical_dp:
                {
                    fill_two_planes_histogram(reduced_t_mean, point.module_num);
                    break;
                }
                default:
                {
                    return;
                }
            }
        }
    }

    void TSyncEngine::fill_one_plane_histogram(double time_val, int module_num)
    {
        hist_horizontal_plane_->Fill(module_num, time_val);
    }

    void TSyncEngine::fill_two_planes_histogram(double time_val, int module_num)
    {
        if (IsModuleNumHorizontal(module_num))
        {
            hist_horizontal_plane_->Fill(module_num, time_val);
        }
        else
        {
            switch (record_type_)
            {
                case RecordType::same_horizontal_dp:
                {
                    hist_horizontal_dp_->Fill(module_num, time_val);
                    break;
                }
                case RecordType::same_vertical_dp:
                {
                    hist_vertical_dp_->Fill(module_num, time_val);
                    break;
                }
                default:
                {
                    return;
                }
            }
        }
    }

    void TSyncEngine::reset()
    {
        buffer_points_.clear();
        buffer_ref_time_ = 0.;
        record_type_ = RecordType::invalid;
        event_ref_module_num_ = 0;
    }

    void TSyncEngine::set_plane_ref_bar_time(const std::vector<Point>& collection, int plane_num)
    {
        const auto module_num = ref_bars_in_planes_.at(plane_num - 1);
        auto ref_bar_iter = ranges::find(collection, module_num, &Point::module_num);
        if (ref_bar_iter != collection.end())
        {
            buffer_ref_time_ = ref_bar_iter->t_mean;
        }
        else
        {
            LOGP(error,
                 "module numbers [{}] doesn't contain any reference numbers: [{}]",
                 fmt::join(collection | ranges::views::transform([](const Point& point) { return point.module_num; }),
                           ", "),
                 fmt::join(ref_bars_in_planes_ | ranges::views::values, ", "));
        }
    }

    auto TSyncEngine::try_in_xy_plane(const std::vector<Point>& collection) -> bool
    {
        const auto plane_num = ModuleID2PlaneNum(collection.front().module_num - 1);

        if (not IsPlaneIDHorizontal(plane_num - 1))
        {
            return false;
        }

        // check all bars are in the same plane
        if (not ranges::all_of(collection,
                               [plane_num](const auto& point) -> bool
                               { return ModuleID2PlaneNum(point.module_num - 1) == plane_num; }))
        {
            return false;
        }

        // check if contains ref bar
        auto ref_bar_iter = ranges::find(collection, ref_bars_in_planes_.at(plane_num - 1), &Point::module_num);
        if (ref_bar_iter != collection.end())
        {
            event_ref_module_num_ = ref_bar_iter->module_num;
            set_plane_ref_bar_time(collection, plane_num);
            return true;
        }
        return false;
    }

    auto TSyncEngine::try_in_same_dp(const std::vector<Point>& collection, int offset) -> bool
    {
        const auto ref_dp_plane_id = ((ModuleID2PlaneID(collection.front().module_num - 1) + offset) / 2);

        if (ref_dp_plane_id >= num_of_dp_)
        {
            return false;
        }

        // check all bars are in the same double plane
        if (not ranges::all_of(collection,
                               [ref_dp_plane_id, offset](const auto& point) -> bool
                               { return (ModuleID2PlaneID(point.module_num - 1) + offset) / 2 == ref_dp_plane_id; }))
        {
            return false;
        }

        // check if contains any local reference bar
        auto ref_bar_iter = ranges::find(collection, ref_bars_in_planes_.at(ref_dp_plane_id * 2), &Point::module_num);
        if (ref_bar_iter != collection.end())
        {
            event_ref_module_num_ = ref_bar_iter->module_num;
            buffer_ref_time_ = ref_bar_iter->t_mean;
            return true;
        }
        return false;
    }

    void TSyncEngine::analyze_event()
    {
        if (buffer_points_.empty())
        {
            return;
        }

        if (try_in_xy_plane(buffer_points_))
        {
            record_type_ = RecordType::xy_plane;
        }
        else if (try_in_same_dp(buffer_points_))
        {
            record_type_ = RecordType::same_horizontal_dp;
        }
        else if (try_in_same_dp(buffer_points_, 1))
        {
            record_type_ = RecordType::same_vertical_dp;
        }
        else
        {
            record_type_ = RecordType::invalid;
        }
    }

    auto TSyncEngine::calculate_best_vertical_bar_num() const -> int
    {
        auto vertical_entry = [this](int vertical_number)
        {
            auto sum = 0.;
            for (int dp_id{}; dp_id < num_of_dp_; ++dp_id)
            {
                auto plane_num = ((dp_id * 2 + 1) * BarsPerPlane) + vertical_number;
                sum += hist_horizontal_dp_->Integral(plane_num, plane_num, -1, -1);
                sum += hist_vertical_dp_->Integral(plane_num, plane_num, -1, -1);
            }
            return sum;
        };

        auto vertical_bar_num_entries_view =
            ranges::views::iota(1, BarsPerPlane + 1) |
            ranges::views::transform(
                [vertical_entry](auto vertical_offset_number)
                { return std::make_pair(vertical_offset_number, vertical_entry(vertical_offset_number)); });
        auto element = ranges::max_element(vertical_bar_num_entries_view, std::less{}, &std::pair<int, double>::second);
        return (*element).first;
    }

    auto TSyncEngine::calculate_time_diff_to_prev_ref(int plane_id, int best_vert_bar_num) const -> ValueErrorD
    {
        const auto module_num = ((plane_id - 1) * BarsPerPlane) + best_vert_bar_num;
        const auto t_diff_prev = extract_mean_value(hist_horizontal_dp_, module_num);
        const auto t_diff_after = extract_mean_value(hist_vertical_dp_, module_num);

        return t_diff_prev - t_diff_after;
    }

    auto TSyncEngine::calculate_time_diffs_to_first() const -> std::unordered_map<int, ValueErrorD>
    {
        const auto best_vert_bar_num = calculate_best_vertical_bar_num();
        LOGP(info, "Best vertical bar number: {}", best_vert_bar_num);

        using EntryPair = std::pair<int, ValueErrorD>;
        auto ref_plane_consecutive_t_diff =
            ref_bars_in_planes_ | ranges::views::keys |
            ranges::views::transform(
                [best_vert_bar_num, this](auto plane_id)
                {
                    return (plane_id == 0)
                               ? std::make_pair(plane_id, ValueErrorD{})
                               : std::make_pair(plane_id, calculate_time_diff_to_prev_ref(plane_id, best_vert_bar_num));
                }) |
            ranges::to<std::vector<EntryPair>>();
        ranges::sort(ref_plane_consecutive_t_diff, std::less{}, &EntryPair::first);
        auto time_diff = ValueErrorD{};
        return ref_plane_consecutive_t_diff |
               ranges::views::transform(
                   [&time_diff](const auto& entry_pair)
                   {
                       time_diff += entry_pair.second;
                       return std::make_pair(entry_pair.first, time_diff);
                   }) |
               ranges::to<std::unordered_map<int, ValueErrorD>>();
    }

    void TSyncEngine::calibrate(Cal2HitPar& hit_par)
    {
        LOGP(info, "Begin time synchronization calibration ...");
        auto ref_time_diff_to_first = calculate_time_diffs_to_first();

        LOGP(info, "TSync diffs from the top horizontal bars: {}", fmt::join(ref_time_diff_to_first, ", "));

        for (auto& [key, module_par] : hit_par.GetListOfModuleParRef())
        {
            const auto module_num = module_par.module_num;
            const auto dp_id = ModuleID2PlaneID(module_num - 1) / 2;
            const auto t_sync_corr = ref_time_diff_to_first.at(dp_id * 2);

            const auto t_sync_val = [this](int p_module_num)
            {
                if (IsModuleNumHorizontal(p_module_num))
                {
                    static constexpr auto top_bar_number = 50;
                    const auto vert_dist_diff =
                        GetBarVerticalDisplacement(top_bar_number) - GetBarVerticalDisplacement(p_module_num);
                    return extract_mean_value(hist_horizontal_plane_, p_module_num) - vert_dist_diff / CLight;
                }
                return extract_mean_value(hist_horizontal_dp_, p_module_num);
            }(module_num);

            module_par.t_sync = t_sync_val + t_sync_corr;
        }
        offset_to_tsync_ref_bar(hit_par);

        // // TEST: try to set error to zero
        // for (auto& [module_num, module_par] : hit_par.GetListOfModuleParRef())
        // {
        //     module_par.t_sync.error = 0.;
        // }

        LOGP(info, "Time synchronization calibration ends.");
    }
} // namespace R3B::Neuland::Calibration
