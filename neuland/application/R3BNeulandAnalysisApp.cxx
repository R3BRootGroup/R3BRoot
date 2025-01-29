#include "R3BNeulandSimCalToCal.h"
#include <CLI/CLI.hpp>
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
#include <fstream>

namespace Digitizing = R3B::Digitizing;
namespace Tamex = Digitizing::Neuland::Tamex;
using NeulandPaddle = Digitizing::Neuland::NeulandPaddle;
using MockPaddle = Digitizing::Neuland::MockPaddle;
using TamexChannel = Tamex::Channel;
using TacquilaChannel = Digitizing::Neuland::TacQuila::Channel;
using MockChannel = Digitizing::Neuland::MockChannel;
using Digitizing::UseChannel;
using Digitizing::UsePaddle;
using json = nlohmann::ordered_json;

namespace
{
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

    auto AnalysisApplication::create_neuland_digi_engine_map(Tamex::PeakPileUpStrategy pileup_strategy,
                                                             const Tamex::Params& tamex_par,
                                                             bool has_cal_to_hit_par)
    {
        R3B::Neuland::Cal2HitPar* cal_to_hit_par{ nullptr };
        if (has_cal_to_hit_par)
        {
            R3BLOG(info, "cal_to_hit_par is used in digitization task!");
            cal_to_hit_par = std::make_unique<R3B::Neuland::Cal2HitPar>().release();
            get_run()->GetRuntimeDb()->addContainer(cal_to_hit_par);
        }
        else
        {
            R3BLOG(info, "cal_to_hit_par is not used in digitization task!");
        }
        return std::map<std::pair<const std::string, const std::string>,
                        std::function<std::unique_ptr<Digitizing::DigitizingEngineInterface>()>>{
            { { "neuland", "tamex" },
              [&tamex_par, pileup_strategy, cal_to_hit_par]()
              {
                  return Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(cal_to_hit_par),
                                                  UseChannel<TamexChannel>(pileup_strategy, tamex_par, cal_to_hit_par));
              } },
            { { "neuland", "tacquila" },
              [cal_to_hit_par]() {
                  return Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(cal_to_hit_par),
                                                  UseChannel<TacquilaChannel>());
              } },
            { { "mock", "tamex" },
              [&tamex_par, pileup_strategy, cal_to_hit_par]()
              {
                  return Digitizing::CreateEngine(UsePaddle<MockPaddle>(),
                                                  UseChannel<TamexChannel>(pileup_strategy, tamex_par, cal_to_hit_par));
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
        auto task_option = options_.tasks;
        run->SetEventHeader(std::make_unique<R3BEventHeader>().release());
        const auto& digi_options = task_option.digi;
        if (digi_options.enable)
        {
            auto engine_map = create_neuland_digi_engine_map(
                digi_options.pileup_strategy, digi_options.tamex_par, digi_options.enable_hit_par);
            auto engine_gen = engine_map.at({ digi_options.paddle, digi_options.channel });
            auto task = std::make_unique<R3BNeulandDigitizer>(engine_gen());
            task->EnableCalDataOutput(digi_options.enable_sim_cal);
            task->SetName(task_option.digi.name.c_str());
            run->AddTask(task.release());
        }
        else
        {
            task_option.cluster_finder.enable = false;
            task_option.sim_cal_to_cal.enable = false;
        }

        if (task_option.sim_cal_to_cal.enable)
        {
            auto task = std::make_unique<R3B::Neuland::SimCal2Cal>();
            task->SetName(task_option.sim_cal_to_cal.name.c_str());
            run->AddTask(task.release());
        }

        if (task_option.hit_monitor.enable)
        {
            // requires_dependecy(task_option.hit_monitor, task_option.digi);
            auto task = std::make_unique<R3BNeulandHitMon>();
            task->SetName(task_option.hit_monitor.name.c_str());
            run->AddTask(task.release());
        }

        if (task_option.prim_inter_finder.enable)
        {
            // requires_dependecy(task_option.prim_inter_finder, task_option.digi);
            auto task = std::make_unique<R3BNeulandPrimaryInteractionFinder>();
            task->SetName(task_option.prim_inter_finder.name.c_str());
            run->AddTask(task.release());
        }

        if (task_option.cluster_finder.enable)
        {
            // requires_dependecy(task_option.cluster_finder, task_option.digi);
            auto task = std::make_unique<R3BNeulandClusterFinder>();
            task->SetName(task_option.cluster_finder.name.c_str());
            run->AddTask(task.release());
        }

        if (task_option.prim_cluster_finder.enable)
        {
            // requires_dependecy(task_option.prim_cluster_finder, task_option.prim_inter_finder);
            // requires_dependecy(task_option.prim_cluster_finder, task_option.cluster_finder);
            auto task = std::make_unique<R3BNeulandPrimaryClusterFinder>();
            task->SetName(task_option.prim_cluster_finder.name.c_str());
            run->AddTask(task.release());
        }

        if (task_option.multi_calorimeter_train.enable)
        {
            const auto calo_options = task_option.multi_calorimeter_train;
            // requires_dependecy(calo_options, task_option.prim_inter_finder);
            // requires_dependecy(calo_options, task_option.cluster_finder);
            auto task = std::make_unique<R3BNeulandMultiplicityCalorimetricTrain>();
            task->SetName(calo_options.name.c_str());
            task->SetUseHits(calo_options.use_hit);
            task->SetWeight(calo_options.weight);
            task->SetEdepOpt(calo_options.edep_opt.init,
                             calo_options.edep_opt.step,
                             calo_options.edep_opt.lower,
                             calo_options.edep_opt.upper);
            task->SetEdepOffOpt(calo_options.edep_off_opt.init,
                                calo_options.edep_off_opt.step,
                                calo_options.edep_off_opt.lower,
                                calo_options.edep_off_opt.upper);
            task->SetNclusterOffOpt(calo_options.n_cluster_opt.init,
                                    calo_options.n_cluster_opt.step,
                                    calo_options.n_cluster_opt.lower,
                                    calo_options.n_cluster_opt.upper);
            task->SetNclusterOffOpt(calo_options.n_cluster_off_opt.init,
                                    calo_options.n_cluster_off_opt.step,
                                    calo_options.n_cluster_off_opt.lower,
                                    calo_options.n_cluster_off_opt.upper);
            run->AddTask(task.release());
        }

        if (task_option.multi_bayes_train.enable)
        {
            // requires_dependecy(task_option.multi_bayes_train, task_option.cluster_finder);
            auto task = std::make_unique<R3BNeulandMultiplicityBayesTrain>();
            task->SetName(task_option.multi_bayes_train.name.c_str());
            run->AddTask(task.release());
        }

        if (task_option.multi_bayes.enable)
        {
            // requires_dependecy(task_option.multi_bayes, task_option.cluster_finder);
            auto task = std::make_unique<R3BNeulandMultiplicityBayes>();
            task->SetName(task_option.multi_bayes.name.c_str());
            run->AddTask(task.release());
        }

        if (task_option.neutron_r_value.enable)
        {
            requires_dependecy(task_option.neutron_r_value, task_option.multi_bayes);
            requires_dependecy(task_option.neutron_r_value, task_option.cluster_finder);
            auto task = std::make_unique<R3BNeulandNeutronsRValue>(task_option.neutron_r_value.neutron_energy_mev);
            task->SetName(task_option.neutron_r_value.name.c_str());
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

} // namespace R3B::Neuland
