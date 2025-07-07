#include "R3BNeulandCommon.h"
#include "R3BNeulandTSyncAnalysis.h"
#include <gtest/gtest.h>

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)
namespace
{
    using TSync = R3B::Neuland::Calibration::TSyncEngine;
    using RecordType = R3B::Neuland::Calibration::TSyncEngine::RecordType;
    using Point = R3B::Neuland::Calibration::TSyncEngine::Point;

    // void add_z_direction_ref_points(TSync& engine)
    // {
    //     const auto num_of_dp = engine.get_num_of_dp();
    //     const auto ref_bar_num = engine.get_ref_bar_num();

    //     const auto vertical_offset = 10;

    //     for (int plane_id{}; plane_id < 2 * num_of_dp; ++plane_id)
    //     {
    //         auto module_num = R3B::Neuland::IsPlaneIDHorizontal(plane_id) ? ref_bar_num + 100 * (plane_id / 2)
    //                                                                       : vertical_offset + 50 + 100 * (plane_id /
    //                                                                       2);
    //         engine.add_point(0., module_num);
    //     }
    // }

    void add_horizontal_plane_points(TSync& engine)
    {
        const auto num_of_dp = engine.get_num_of_dp();
        const auto target_plane_id = 8;

        for (int local_bar_id{}; local_bar_id < R3B::Neuland::BarsPerPlane; ++local_bar_id)
        {
            auto module_num = target_plane_id * R3B::Neuland::BarsPerPlane + local_bar_id + 1;
            engine.add_point(0., module_num);
        }
    }

    TEST(TestNeulandTSyncEngine, test_horizontal_plane)
    {
        auto engine = TSync{};
        auto record_type = RecordType::invalid;

        engine.set_number_of_modules(1300);
        engine.init();

        EXPECT_EQ(engine.get_record_type(), RecordType::invalid);
        add_horizontal_plane_points(engine);
        engine.end_of_event();
        EXPECT_EQ(engine.get_record_type(), RecordType::xy_plane);
        engine.reset();
        EXPECT_EQ(engine.get_record_type(), RecordType::invalid);
    }
} // namespace
// NOLINTEND (cppcoreguidelines-avoid-magic-numbers)
