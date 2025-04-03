#include "R3BDigitizingEngine.h"
#include "R3BDigitizingTamex.h"
#include "R3BNeulandApp.h"
#include "R3BNeulandCalToHitParTask.h"
#include "R3BNeulandSimCalToCal.h"
#include <CLI/CLI.hpp>
#include <FairRun.h>
#include <FairRunAna.h>
#include <R3BDigitizingChannelMock.h>
#include <R3BDigitizingPaddleMock.h>
#include <R3BDigitizingPaddleNeuland.h>
#include <R3BDigitizingTacQuila.h>
#include <R3BEventHeader.h>
#include <R3BException.h>
#include <R3BFileSource2.h>
#include <R3BLogger.h>
#include <R3BNeulandAnalysisApp.h>
#include <R3BNeulandAppOptionJson.h>
#include <R3BNeulandClusterFinder.h>
#include <R3BNeulandDigitizer.h>
#include <R3BNeulandHitMon.h>
#include <R3BNeulandMultiplicityBayes.h>
#include <R3BNeulandMultiplicityBayesTrain.h>
#include <R3BNeulandMultiplicityCalorimetricTrain.h>
#include <R3BNeulandNeutronsRValue.h>
#include <R3BNeulandPrimaryClusterFinder.h>
#include <R3BNeulandPrimaryInteractionFinder.h>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <fmt/core.h>
#include <fstream>
#include <functional>
#include <ios>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Digitizing = R3B::Digitizing;
namespace Tamex = Digitizing::Neuland::Tamex;
using NeulandPaddle = Digitizing::Neuland::Paddle;
using MockPaddle = Digitizing::Neuland::MockPaddle;
using TamexChannel = Tamex::Channel;
using TacquilaChannel = Digitizing::Neuland::TacQuila::Channel;
using MockChannel = Digitizing::Neuland::MockChannel;
using Digitizing::UseChannel;
using Digitizing::UsePaddle;
using json = nlohmann::ordered_json;

namespace
{
    void resolve_branch_names(const std::string& input, std::vector<std::string>& output)
    {
        output.clear();
        boost::split(output, input, boost::is_any_of(";"));
        // trim the empty spaces
        std::for_each(output.begin(), output.end(), [](auto& name) { boost::trim(name); });
        // remove empty names
        output.erase(std::remove(output.begin(), output.end(), ""), output.end());
    }

    template <typename Option>
    void parse_branch_names(const Option& option,
                            std::vector<std::string>& read,
                            int read_num,
                            std::vector<std::string>& write,
                            int write_num)
    {
        resolve_branch_names(option.read, read);
        if (read.size() != read_num)
        {
            throw R3B::logic_error(fmt::format(
                "Task {:?} requires {} read branch(es) but only received {} branch(es)! Parsed string: {:?}",
                option.name,
                read_num,
                read.size(),
                option.read));
        }
        resolve_branch_names(option.write, write);
        if (write.size() != write_num)
        {
            throw R3B::logic_error(fmt::format(
                "Task {:?} requires {} write branch(es) but only received {} branch(es)! Parsed string: {:?}",
                option.name,
                read_num,
                write.size(),
                option.write));
        }
    }
} // namespace

namespace R3B::Neuland
{
    using Options = AnalysisApplication::Options;

    namespace
    {
        template <typename Depender, typename Dependee>
        void requires_dependecy(const Depender& depender, const Dependee& dependee)
        {
            if (not dependee.enable)
            {
                throw R3B::logic_error(fmt::format(
                    "Cannot run the task {} because its dependee tasks {} is disabled!", depender.name, dependee.name));
            }
        }
    } // namespace

    auto AnalysisApplication::create_neuland_digi_engine_map(const Options::Tasks::Digi& option,
                                                             std::string_view hit_par_name)
    {
        auto pileup_strategy = option.pileup_strategy;
        const auto& tamex_par = option.tamex_par;
        R3B::Neuland::Cal2HitPar* cal_to_hit_par{ nullptr };
        if (option.enable_hit_par)
        {
            R3BLOG(info, "cal_to_hit_par is used in digitization task!");
            cal_to_hit_par = std::make_unique<R3B::Neuland::Cal2HitPar>(hit_par_name).release();
            get_run()->GetRuntimeDb()->addContainer(cal_to_hit_par);
        }
        else
        {
            R3BLOG(info, "cal_to_hit_par is not used in digitization task!");
        }
        return std::map<std::pair<const std::string, const std::string>,
                        std::function<std::unique_ptr<Digitizing::EngineInterface>()>>{
            { { "neuland", "tamex" },
              [&tamex_par, pileup_strategy, cal_to_hit_par, enable_sim_cal = option.enable_sim_cal]()
              {
                  return Digitizing::CreateEngine(
                      UsePaddle<NeulandPaddle>(cal_to_hit_par),
                      UseChannel<TamexChannel>(pileup_strategy, tamex_par, cal_to_hit_par, enable_sim_cal));
              } },
            { { "neuland", "tacquila" },
              [cal_to_hit_par]() {
                  return Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(cal_to_hit_par),
                                                  UseChannel<TacquilaChannel>());
              } },
            { { "mock", "tamex" },
              [&tamex_par, pileup_strategy, cal_to_hit_par, enable_sim_cal = option.enable_sim_cal]()
              {
                  return Digitizing::CreateEngine(
                      UsePaddle<MockPaddle>(),
                      UseChannel<TamexChannel>(pileup_strategy, tamex_par, cal_to_hit_par, enable_sim_cal));
              } },
            { { "neuland", "mock" },
              [cal_to_hit_par]() {
                  return Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(cal_to_hit_par), UseChannel<MockChannel>());
              } },
            { { "mock", "mock" },
              []() { return Digitizing::CreateEngine(UsePaddle<MockPaddle>(), UseChannel<MockChannel>()); } }
        };
    }

    AnalysisApplication::AnalysisApplication()
        : Application{ "neuland_ana", std::make_unique<FairRunAna>(), std::ref(options_.general) }
    {
        options_.general.input.data.emplace_back("sim.output.root");
        options_.general.input.par = "sim.par.root";

        options_.general.output.data = "digi.output.root";
        options_.general.output.par = "digi.par.root";
    }

    void AnalysisApplication::setup_application_options(CLI::App& program_options)
    {
        const auto analysis_option_group = std::string{ "Analysis options" };
        program_options.add_option("--paddle", options_.tasks.digi.paddle, R"(Set the paddle name. e.g. "neuland")")
            ->capture_default_str()
            ->group(analysis_option_group);
        program_options.add_option("--channel", options_.tasks.digi.channel, R"(Set the channel name. e.g. "tamex")")
            ->capture_default_str()
            ->group(analysis_option_group);
    }

    void AnalysisApplication::pre_init(FairRun* run)
    {

        auto EvntHeader = std::make_unique<R3BEventHeader>();
        auto read_branch_names = std::vector<std::string>{};
        auto write_branch_names = std::vector<std::string>{};
        run->SetEventHeader(EvntHeader.release());

        auto task_option = options_.tasks;
        run->SetEventHeader(std::make_unique<R3BEventHeader>().release());

        if (const auto& option = task_option.digi; option.enable)
        {
            parse_branch_names(option, read_branch_names, 2, write_branch_names, 2);
            auto engine_map = create_neuland_digi_engine_map(option, read_branch_names.at(1));
            auto engine_gen = engine_map.at({ option.paddle, option.channel });
            auto task = std::make_unique<R3BNeulandDigitizer>(
                engine_gen(), read_branch_names.at(0), write_branch_names.at(0), write_branch_names.at(1));
            task->EnableCalDataOutput(option.enable_sim_cal);
            task->EnableSizeMonitor(option.enable_size_monitor);
            task->SetName(task_option.digi.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.sim_cal_to_cal; option.enable)
        {
            parse_branch_names(option, read_branch_names, 1, write_branch_names, 1);
            auto task = std::make_unique<R3B::Neuland::SimCal2Cal>(read_branch_names.at(0), write_branch_names.at(0));
            task->SetName(option.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.hit_monitor; option.enable)
        {
            parse_branch_names(option, read_branch_names, 1, write_branch_names, 0);
            auto task = std::make_unique<R3BNeulandHitMon>(read_branch_names.at(0));
            task->SetName(option.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.prim_inter_finder; option.enable)
        {
            parse_branch_names(option, read_branch_names, 2, write_branch_names, 3);
            auto task = std::make_unique<R3BNeulandPrimaryInteractionFinder>(read_branch_names.at(0),
                                                                             read_branch_names.at(1),
                                                                             write_branch_names.at(0),
                                                                             write_branch_names.at(1),
                                                                             write_branch_names.at(2));
            task->SetName(option.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.cluster_finder; option.enable)
        {
            parse_branch_names(option, read_branch_names, 1, write_branch_names, 1);
            auto task = std::make_unique<R3BNeulandClusterFinder>(read_branch_names.at(0), write_branch_names.at(0));
            task->SetName(option.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.cluster_finder; option.enable)
        {
            parse_branch_names(option, read_branch_names, 2, write_branch_names, 2);
            auto task = std::make_unique<R3BNeulandPrimaryClusterFinder>(
                read_branch_names.at(0), read_branch_names.at(1), write_branch_names.at(0), write_branch_names.at(1));
            task->SetName(option.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.multi_calorimeter_train; option.enable)
        {
            parse_branch_names(option, read_branch_names, 3, write_branch_names, 0);
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

        if (const auto& option = task_option.multi_bayes_train; option.enable)
        {
            parse_branch_names(option, read_branch_names, 2, write_branch_names, 0);
            auto task =
                std::make_unique<R3BNeulandMultiplicityBayesTrain>(read_branch_names.at(0), read_branch_names.at(1));
            task->SetName(option.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.multi_bayes; option.enable)
        {
            parse_branch_names(option, read_branch_names, 1, write_branch_names, 1);
            auto task =
                std::make_unique<R3BNeulandMultiplicityBayes>(read_branch_names.at(0), write_branch_names.at(0));
            task->SetName(option.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.neutron_r_value; option.enable)
        {
            parse_branch_names(option, read_branch_names, 2, write_branch_names, 1);
            auto task = std::make_unique<R3BNeulandNeutronsRValue>(
                option.neutron_energy_mev, read_branch_names.at(0), read_branch_names.at(1), write_branch_names.at(0));
            task->SetName(option.name.c_str());
            run->AddTask(task.release());
        }

        if (const auto& option = task_option.cal_to_hit_par_task; option.enable)
        {
            parse_branch_names(option, read_branch_names, 2, write_branch_names, 1);
            auto task = std::make_unique<R3B::Neuland::Cal2HitParTask>(
                option.method, read_branch_names.at(0), read_branch_names.at(1), write_branch_names.at(0));
            task->SetMinStat(option.min_stat);
            run->AddTask(task.release());
        }
    }

    void AnalysisApplication::set_parameters() {}

    void AnalysisApplication::print_json_options()
    {
        auto json_obj = json{ options_ };
        if (json_obj.is_array())
        {
            fmt::print("{}\n", json_obj.front().dump(4));
        }
        else
        {
            fmt::print("{}\n", json_obj.dump(4));
        }
    }

    void AnalysisApplication::dump_json_options(const std::string& filename)
    {
        auto file = std::ofstream{ filename, std::ios::trunc };
        auto json_obj = json{ options_ };
        if (json_obj.is_array())
        {
            file << json_obj.front().dump(4);
        }
        else
        {
            file << json_obj.dump(4);
        }
        R3BLOG(info, fmt::format("Configuration of neuland_ana is saved into the file {:?}", filename));
    }

    void AnalysisApplication::ParseApplicationOption(const std::vector<std::string>& filename)
    {
        ParseApplicationOptionImp(filename, options_);
    }
} // namespace R3B::Neuland
