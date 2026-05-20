#include <R3BPDGConverter.h>
#include <gtest/gtest.h>

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)
namespace
{
    constexpr auto proton_pid = 2212;
    constexpr auto electron_pid = 11;
    constexpr auto carbon_12_pid = 1000060120;
    constexpr auto carbon_13_pid = 1000060130;

    TEST(R3BPDGConverterTest, test_pid_query)
    {
        auto converter = R3B::PDGConverter{};

        auto proton_name = converter.get_name(proton_pid);
        EXPECT_STREQ("proton", proton_name.c_str());

        auto electron_name = converter.get_name(electron_pid);
        EXPECT_STREQ("e-", electron_name.c_str());

        auto carbon_12_name = converter.get_name(carbon_12_pid);
        EXPECT_STREQ("C-12", carbon_12_name.c_str());

        auto carbon_13_name = converter.get_name(carbon_13_pid);
        EXPECT_STREQ("C-13", carbon_13_name.c_str());
    }

    TEST(R3BPDGConverterTest, test_name_query)
    {
        auto converter = R3B::PDGConverter{};

        EXPECT_EQ(proton_pid, converter.get_pid("proton"));
        EXPECT_EQ(electron_pid, converter.get_pid("e-"));
        EXPECT_EQ(carbon_12_pid, converter.get_pid("C-12"));
        EXPECT_EQ(carbon_13_pid, converter.get_pid("C-13"));
    }

    TEST(R3BPDGConverterTest, test_name_type)
    {
        auto converter = R3B::PDGConverter{};

        EXPECT_EQ(R3B::PDGConverter::Type::basic, converter.get_type("proton"));
        EXPECT_EQ(R3B::PDGConverter::Type::basic, converter.get_type("e-"));
        EXPECT_EQ(R3B::PDGConverter::Type::nuclear, converter.get_type("C-12"));
        EXPECT_EQ(R3B::PDGConverter::Type::nuclear, converter.get_type("C-13"));
    }

    TEST(R3BPDGConverterTest, test_pid_type)
    {
        auto converter = R3B::PDGConverter{};

        EXPECT_EQ(R3B::PDGConverter::Type::basic, converter.get_type(proton_pid));
        EXPECT_EQ(R3B::PDGConverter::Type::basic, converter.get_type(electron_pid));
        EXPECT_EQ(R3B::PDGConverter::Type::nuclear, converter.get_type(carbon_12_pid));
        EXPECT_EQ(R3B::PDGConverter::Type::nuclear, converter.get_type(carbon_13_pid));
    }
} // namespace
// NOLINTEND (cppcoreguidelines-avoid-magic-numbers)
