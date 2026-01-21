#include "R3BLosMapped2Cal.h"
#include "R3BLosMapped2CalPar.h"
#include "R3BLosProvideTStart.h"
#include "R3BNeulandApp.h"
#include "R3BNeulandCalMonitorTask.h"
#include "R3BNeulandCalToHitParTask.h"
#include "R3BNeulandCalToHitTask.h"
#include "R3BNeulandCommonFunc.h"
#include "R3BNeulandHitCosmicMonitorTask.h"
#include "R3BNeulandHitOnlineMonitorTask.h"
#include "R3BNeulandMapDataConverterTask.h"
#include "R3BNeulandMapToCalParTask.h"
#include "R3BNeulandMapToCalTask.h"
#include "R3BNeulandMillepede.h"
#include "R3BNeulandSimCalToCal.h"
#include <CLI/CLI.hpp>
#include <FairRun.h>
#include <FairRunAna.h>
#include <R3BEventHeader.h>
#include <R3BNeulandAnalysisApp.h>
#include <R3BNeulandClusterFinder.h>
#include <R3BNeulandDigitizer.h>
#include <R3BNeulandHitMon.h>
#include <R3BNeulandMultiplicityBayes.h>
#include <R3BNeulandMultiplicityBayesTrain.h>
#include <R3BNeulandMultiplicityCalorimetricTrain.h>
#include <R3BNeulandNeutronsRValue.h>
#include <R3BNeulandPrimaryClusterFinder.h>
#include <R3BNeulandPrimaryInteractionFinder.h>
#include <fairlogger/Logger.h>
#include <fmt/base.h>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

using nlohmann::ordered_json;

namespace R3B::Neuland
{
    using Options = AnalysisApplication::Options;

    AnalysisApplication::AnalysisApplication()
        : CLIApplication{ "neuland_ana", std::make_unique<FairRunAna>(), std::ref(options_.general) }
    {
        options_.general.input.data.emplace_back("sim.output.root");
        options_.general.input.par.emplace_back("sim.par.root");

        options_.general.output.data = "digi.output.root";
        options_.general.output.par = "digi.par.root";
    }

    void AnalysisApplication::setup_application_options(CLI::App& program_options)
    {
        const auto analysis_option_group = std::string{ "Analysis options" };
        program_options
            .add_option("--paddle", options_.tasks.neuland_digitizer.paddle, R"(Set the paddle name. e.g. "neuland")")
            ->capture_default_str()
            ->group(analysis_option_group);
        program_options
            .add_option("--channel", options_.tasks.neuland_digitizer.channel, R"(Set the channel name. e.g. "tamex")")
            ->capture_default_str()
            ->group(analysis_option_group);
    }

    void AnalysisApplication::pre_init(FairRun* run)
    {

        auto event_header = std::make_unique<R3BEventHeader>();
        auto read_branch_names = std::vector<std::string>{};
        auto write_branch_names = std::vector<std::string>{};
        LOGP(info, "Setting the event header to be R3BEventHeader!");
        run->SetEventHeader(std::move(event_header));

        auto task_option = options_.tasks;

        if (const auto& option = task_option.neuland_digitizer; option.enable)
        {
            auto task = Digitizer::Create(option, run);
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neuland_sim_cal_to_cal; option.enable)
        {
            parse_io_branch_names(option, read_branch_names, 1, write_branch_names, 1);
            auto task = std::make_unique<R3B::Neuland::SimCal2Cal>(read_branch_names.at(0), write_branch_names.at(0));
            task->SetName(option.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neuland_hit_mon; option.enable)
        {
            parse_io_branch_names(option, read_branch_names, 1, write_branch_names, 0);
            auto task = std::make_unique<R3BNeulandHitMon>(read_branch_names.at(0));
            task->SetName(option.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neuland_primary_interaction_finder; option.enable)
        {
            parse_io_branch_names(option, read_branch_names, 2, write_branch_names, 3);
            auto task = std::make_unique<R3BNeulandPrimaryInteractionFinder>(read_branch_names.at(0),
                                                                             read_branch_names.at(1),
                                                                             write_branch_names.at(0),
                                                                             write_branch_names.at(1),
                                                                             write_branch_names.at(2));
            task->SetName(option.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neuland_cluster_finder; option.enable)
        {
            parse_io_branch_names(option, read_branch_names, 1, write_branch_names, 1);
            auto task = std::make_unique<R3BNeulandClusterFinder>(read_branch_names.at(0), write_branch_names.at(0));
            task->SetName(option.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neuland_primary_cluster_finder; option.enable)
        {
            parse_io_branch_names(option, read_branch_names, 2, write_branch_names, 2);
            auto task = std::make_unique<R3BNeulandPrimaryClusterFinder>(
                read_branch_names.at(0), read_branch_names.at(1), write_branch_names.at(0), write_branch_names.at(1));
            task->SetName(option.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neuland_multi_calorimeter_train; option.enable)
        {
            parse_io_branch_names(option, read_branch_names, 3, write_branch_names, 0);
            auto task = std::make_unique<R3BNeulandMultiplicityCalorimetricTrain>(
                read_branch_names.at(0), read_branch_names.at(1), read_branch_names.at(2));
            task->SetName(option.name.c_str());
            task->SetUseHits(option.use_hit);
            task->SetWeight(option.weight);
            task->SetEdepOpt(option.edep_opt.init, option.edep_opt.step, option.edep_opt.lower, option.edep_opt.upper);
            task->SetEdepOffOpt(option.edep_off_opt.init,
                                option.edep_off_opt.step,
                                option.edep_off_opt.lower,
                                option.edep_off_opt.upper);
            task->SetNclusterOffOpt(option.n_cluster_opt.init,
                                    option.n_cluster_opt.step,
                                    option.n_cluster_opt.lower,
                                    option.n_cluster_opt.upper);
            task->SetNclusterOffOpt(option.n_cluster_off_opt.init,
                                    option.n_cluster_off_opt.step,
                                    option.n_cluster_off_opt.lower,
                                    option.n_cluster_off_opt.upper);
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neuland_multi_bayes_train; option.enable)
        {
            parse_io_branch_names(option, read_branch_names, 2, write_branch_names, 0);
            auto task =
                std::make_unique<R3BNeulandMultiplicityBayesTrain>(read_branch_names.at(0), read_branch_names.at(1));
            task->SetName(option.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neuland_multi_bayes; option.enable)
        {
            parse_io_branch_names(option, read_branch_names, 1, write_branch_names, 1);
            auto task =
                std::make_unique<R3BNeulandMultiplicityBayes>(read_branch_names.at(0), write_branch_names.at(0));
            task->SetName(option.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neuland_neutron_r_value; option.enable)
        {
            parse_io_branch_names(option, read_branch_names, 2, write_branch_names, 1);
            auto task = std::make_unique<R3BNeulandNeutronsRValue>(
                option.neutron_energy_mev, read_branch_names.at(0), read_branch_names.at(1), write_branch_names.at(0));
            task->SetName(option.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neuland_map_data_converter_task; option.enable)
        {
            parse_io_branch_names(option, read_branch_names, 2, write_branch_names, 2);
            auto task = std::make_unique<Calibration::MapDataConverterTask>(
                read_branch_names.at(0), read_branch_names.at(1), write_branch_names.at(0), write_branch_names.at(1));
            task->SetName(option.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neuland_map_to_cal_par_task; option.enable)
        {
            parse_io_branch_names(option, read_branch_names, 2, write_branch_names, 2);
            auto task = std::make_unique<R3B::Neuland::Map2CalParTask>(
                read_branch_names.at(0), read_branch_names.at(1), write_branch_names.at(0), write_branch_names.at(1));
            task->SetTrigEnabled(option.has_trig_enabled);
            task->SetErrorMethod(option.error_method);
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neuland_map_to_cal_task; option.enable)
        {
            auto task = std::make_unique<std::remove_cvref_t<decltype(option)>::Task>(option);
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.los_map_to_cal_par_task; option.enable)
        {
            parse_io_branch_names(option, read_branch_names, 2, write_branch_names, 1);
            auto task = std::make_unique<R3BLosMapped2CalPar>();
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.los_map_to_cal_task; option.enable)
        {
            parse_io_branch_names(option, read_branch_names, 3, write_branch_names, 1);
            auto task = std::make_unique<R3BLosMapped2Cal>();
            static constexpr auto LOS_MODULE_NUM = 8;
            task->SetNofModules(1, LOS_MODULE_NUM);
            task->SetTrigger(1);
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neuland_cal_to_hit_par_task; option.enable)
        {
            auto task = std::make_unique<std::remove_cvref_t<decltype(option)>::Task>(option);
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.los_provide_t_start; option.enable)
        {
            parse_io_branch_names(option, read_branch_names, 2, write_branch_names, 0);
            auto task = std::make_unique<R3BLosProvideTStart>();
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neuland_cal_to_hit_task; option.enable)
        {
            parse_io_branch_names(option, read_branch_names, 2, write_branch_names, 1);
            auto task = std::make_unique<R3B::Neuland::Cal2HitTask>(
                read_branch_names.at(0), read_branch_names.at(1), write_branch_names.at(0));
            task->SetTrigger(option.mode);
            task->SetGlobalTimeOffset(option.global_time_offset);
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neuland_cosmic_monitor_task; option.enable)
        {
            auto task = std::make_unique<std::remove_cvref_t<decltype(option)>::Task>(option);
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neuland_hit_online_monitor_task; option.enable)
        {
            auto task = std::make_unique<std::remove_cvref_t<decltype(option)>::Task>(option);
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neuland_cal_monitor_task; option.enable)
        {
            parse_io_branch_names(option, read_branch_names, 1, write_branch_names, 0);
            auto task = std::make_unique<R3B::Neuland::CalMonitorTask>(read_branch_names.at(0));
            task->SetTrigger(option.mode);
            run->AddTask(task.release());
        }
    }

    void AnalysisApplication::set_parameters() {}

    void AnalysisApplication::print_json_options() { CLIApplication::print_json_options(options_); }

    void AnalysisApplication::dump_json_options(const std::string& filename)
    {
        CLIApplication::dump_json_options(options_, filename);
    }

    void AnalysisApplication::ParseApplicationOption(const std::vector<std::string>& filename)
    {
        ParseApplicationOptionImp(filename, options_);
    }
} // namespace R3B::Neuland
