#include "R3BNeulandBaseParCreator.h"
#include "CLI/CLI.hpp"
#include "R3BException.h"
#include "R3BNeulandBasePar.h"
#include "R3BNeulandCommon.h"
#include <FairBaseParSet.h>
#include <FairGeoParSet.h>
#include <FairLogger.h>
#include <FairParRootFileIo.h>
#include <FairParSet.h>
#include <FairRuntimeDb.h>
#include <fairlogger/Logger.h>
#include <fmt/core.h>
#include <fstream>
#include <ios>
#include <memory>
#include <re2/re2.h>
#include <string>
#include <utility>

namespace R3B::Neuland
{
    namespace
    {
        void add_parameter(FairParSet* par, FairRuntimeDb* rtdb)
        {
            par->setChanged();
            if (rtdb->addContainer(par); par == nullptr)
            {
                throw R3B::runtime_error("Calibration parameter becomes nullptr!");
            }
        }

        auto parsing_a_line(const std::string& line_str, CalibrationBasePar::TrigIDMap& trig_id_map) -> bool
        {
            auto plane_id = -1;
            auto side_id = -1;
            auto bar_id = -1;
            auto trig_module_id = -1;
            auto res = RE2::FullMatch(line_str,
                                      R"(.*\[(\d+)\]\[(\d+)\]\[(\d+)\]\s+\=\s+(\d+);)",
                                      &plane_id,
                                      &side_id,
                                      &bar_id,
                                      &trig_module_id);
            if (not res)
            {
                return false;
            }

            auto& module_nums =
                trig_id_map.try_emplace((plane_id * BarsPerPlane) + bar_id + 1, std::make_pair(-1, -1)).first->second;
            if (side_id == 0)
            {
                module_nums.first = trig_module_id + 1;
            }
            else
            {
                module_nums.second = trig_module_id + 1;
            }

            return true;
        }

        auto check_trig_id_map(CalibrationBasePar::TrigIDMap& trig_id_map) -> bool
        {
            auto res = true;
            for (const auto& [bar_number, trig_ids] : trig_id_map)
            {
                auto has_trig_id = (trig_ids.first != -1) and (trig_ids.second != -1);
                if (not has_trig_id)
                {
                    LOGP(error, "Trig module numbers are incomplete for the bar number {}!", bar_number);
                }
                res &= has_trig_id;
            }
            return res;
        }
    } // namespace

    void BaseParCreator::setup_options(CLI::App& program_options)
    {

        program_options.add_option("-s, --severity", log_level_, "Set the severity level");
        program_options
            .add_option("-i, --input", input_header_file_name_, "Set the filename of the input header file.")
            ->required(true);
        program_options
            .add_option("-o, --output", output_par_file_name_, "Set the filename of the ouput parameter root file.")
            ->required(true);
        program_options.add_option("-e, --exp-id", exp_id_, "Set the experiment id, such as \"s118\"")->required(true);
        program_options
            .add_option("--offspill-pos", offspill_pos_, "Set the TPAT bit position of neuland offspill data")
            ->required(true);
    }

    void BaseParCreator::init() { FairLogger::GetLogger()->SetLogScreenLevel(log_level_.c_str()); }

    auto BaseParCreator::parsing_header_file() -> CalibrationBasePar::TrigIDMap
    {
        auto ifile = std::ifstream{ input_header_file_name_, std::ios::in };
        if (not ifile.is_open())
        {
            throw R3B::logic_error(fmt::format("Cannot open the header file {}", input_header_file_name_));
        }
        auto line_string = std::string{};
        std::getline(ifile, line_string, '\n');
        parsing_first_line(line_string);

        auto trig_id_map = CalibrationBasePar::TrigIDMap{};
        while (std::getline(ifile, line_string, '\n'))
        {
            parsing_a_line(line_string, trig_id_map);
        }
        return trig_id_map;
    }

    void BaseParCreator::parsing_first_line(const std::string& line_str)
    {
        if (not RE2::FullMatch(line_str, R"(.*\[(\d+)\]\[\d+\]\[\d+\];)", &num_of_planes_))
        {
            throw R3B::runtime_error(fmt::format("Failed to extract the plane number from the string {:?}.", line_str));
        }
    }

    void BaseParCreator::run()
    {
        auto trig_id_map = parsing_header_file();

        if (not check_trig_id_map(trig_id_map))
        {
            throw R3B::runtime_error("Trig ID map is incomplete for some bars!");
        }

        auto base_par = std::make_unique<R3B::Neuland::CalibrationBasePar>();
        base_par->set_num_of_planes(num_of_planes_);
        base_par->set_exp_ids(exp_id_);
        base_par->set_offspill_tpat_pos(offspill_pos_);
        base_par->set_trig_id_map(std::move(trig_id_map));

        auto rtdb = std::unique_ptr<FairRuntimeDb>(FairRuntimeDb::instance());
        auto parOut = std::make_unique<FairParRootFileIo>(false);
        parOut->open(output_par_file_name_.c_str(), "RECREATE");
        add_parameter(base_par.release(), rtdb.get());
        add_parameter(std::make_unique<FairBaseParSet>().release(), rtdb.get());
        add_parameter(std::make_unique<FairGeoParSet>().release(), rtdb.get());
        rtdb->setOutput(parOut.release());
        rtdb->saveOutput();
        rtdb->addRun(run_id_);
        rtdb->writeContainers();
    }
} // namespace R3B::Neuland
