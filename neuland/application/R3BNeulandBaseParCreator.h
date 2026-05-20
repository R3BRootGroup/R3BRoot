#pragma once

#include "CLI/CLI.hpp"
#include "R3BNeulandBasePar.h"
#include "R3BNeulandCLIAbstract.h"
#include <string>

namespace R3B::Neuland
{
    class BaseParCreator : public CLIAbstract
    {
      public:
        BaseParCreator() = default;

      private:
        constexpr static auto DEFAULT_RUN_ID = 999;

        int run_id_ = DEFAULT_RUN_ID;
        int offspill_pos_ = 0;
        int num_of_planes_ = 0;
        std::string log_level_ = "info";
        std::string exp_id_;
        std::string output_par_file_name_;
        std::string input_header_file_name_;

        // struct LineParsingOutput
        // {
        //     int plane_id = -1;
        //     int side_id = -1;
        //     int bar_id = -1;
        //     int trig_module_id = -1;
        // };

        auto parsing_header_file() -> CalibrationBasePar::TrigIDMap;
        void parsing_first_line(const std::string& line_str);

        void setup_options(CLI::App& program_options) override;
        void init() override;
        void run() override;
    };
} // namespace R3B::Neuland
