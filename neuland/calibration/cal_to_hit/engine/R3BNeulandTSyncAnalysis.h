#pragma once

#include "R3BValueError.h"
#include <cstdint>
#include <unordered_map>
#include <vector>

class TH2D;
class TH1I;

namespace R3B
{
    class DataMonitor;
}

namespace R3B::Neuland
{
    class Cal2HitPar;
} // namespace R3B::Neuland

namespace R3B::Neuland::Calibration
{

    /**
     * @class TSyncEngine
     * @brief Main interface for TSync calculation
     *
     * To be added here ...
     */

    class TSyncEngine
    {
      public:
        struct Point
        {
            int module_num = 0;
            double t_mean = 0;
        };

        enum class RecordType : uint8_t
        {
            xy_plane,           //!< All points within one single horizontal plane
            same_horizontal_dp, //!< All the points within horizontal plane followed by vertical one
            same_vertical_dp,   //!< All the points within vertical plane followed by horizontal one
            invalid,            //!< Invalid points to be thrown away.
        };

        TSyncEngine() = default;

        /**
         * @brief Add the t_sum value from a bar
         *
         * This adds the t_sum value and bar number to a collection for the current event.
         * @param t_sum Summation of the time values of the two PMTs on the same bar
         * @return module_num Global module number of the bar (1-based)
         */
        void add_point(double t_sum, int module_num);

        // /**
        //  * @brief set histograms.
        //  *
        //  * @param hist Pointer to the histogram
        //  */
        // void set_tsync_hist(TH2D* hist) { hist_tsync_ = hist; }

        void init_hist(DataMonitor& data_monitor);

        /**
         * @brief Set the bar num of the reference bar.
         *
         * @param ref_bar_num Bar number (1-based) of the reference bar
         */

        void set_max_time_difference(double max_time_val) { max_time_difference_ = max_time_val; }

        void init();

        /**
         * @brief Actions in the end of the event.
         *
         * This method must be called in the end of the event to clear the internal buffers and values.
         */
        void end_of_event();

        void reset();

        void set_number_of_modules(int num_of_modules) { num_of_modules_ = num_of_modules; }

        void calibrate(Cal2HitPar& hit_par);

        [[nodiscard]] auto get_hist_tsync() const -> TH2D* { return hist_horizontal_plane_; }
        auto get_record_type() const -> RecordType { return record_type_; }
        auto get_num_of_dp() const -> int { return num_of_dp_; }

      private:
        RecordType record_type_ = RecordType::invalid;
        int num_of_modules_ = 0;
        int num_of_dp_ = 0;
        double max_time_difference_ = 0.;
        TH1I* hist_record_type_ = nullptr;
        TH2D* hist_horizontal_plane_ = nullptr;
        TH2D* hist_horizontal_dp_ = nullptr;
        TH2D* hist_vertical_dp_ = nullptr;
        std::unordered_map<int, int> ref_bars_in_planes_; //!<  key: plane_id, value: global module number

        // temp variables
        int event_ref_module_num_ = 0;
        double buffer_ref_time_ = 0.;
        std::vector<Point> buffer_points_;

        static constexpr auto TIME_SUM_BIN_NUM = 500;

        void analyze_event();

        auto try_in_xy_plane(const std::vector<Point>& collection) -> bool;
        /**
         * @brief Check if all points are in the same horizontal or vertical double plane.
         *
         * @param collection A collection of points
         * @param offset 0 if checking horizontal dp. 1 if vertical dp.
         */
        auto try_in_same_dp(const std::vector<Point>& collection, int offset = 0) -> bool;
        auto try_in_same_vertical_dp(const std::vector<Point>& collection) -> bool;
        void set_plane_ref_bar_time(const std::vector<Point>& collection, int plane_num);
        void fill_histogram(RecordType record_type);
        void fill_one_plane_histogram(double time_val, int module_num);
        void fill_two_planes_histogram(double time_val, int module_num);

        auto calculate_best_vertical_bar_num() const -> int;
        auto calculate_time_diff_to_prev_ref(int plane_id, int best_vert_bar_num) const -> ValueErrorD;
        auto calculate_time_diffs_to_first() const -> std::unordered_map<int, ValueErrorD>;

        // auto check_global_ref_bar(const std::vector<Point>& collection) -> bool;
        // template <typename RefCriterion>
        // auto try_with_ref_bar(const std::vector<Point>& collection, RefCriterion criterion) -> bool;
        // auto try_in_yz_plane(const std::vector<Point>& collection) -> bool;
        // auto try_with_ref_z_direction(const std::vector<Point>& collection) -> bool;
    };
} // namespace R3B::Neuland::Calibration
