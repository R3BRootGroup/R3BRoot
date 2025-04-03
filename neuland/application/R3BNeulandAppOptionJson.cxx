#include "R3BNeulandAppOptionJson.h"
#include "R3BDigitizingTamex.h"
#include "R3BException.h"
#include "R3BNeulandAnalysisApp.h"
#include "R3BNeulandApp.h"
#include "R3BNeulandCalToHitParTask.h"
#include "R3BNeulandSimApp.h"
#include <R3BROOTTypeJson.h> //NOLINT
#include <fmt/core.h>
#include <fmt/format.h>
#include <magic_enum/magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::ordered_json;

namespace R3B::Digitizing::Neuland::Tamex
{
    void to_json(json& json_obj, const Params& param)
    {
        json_obj =
            json{ { "fPMTThresh", param.pmt_thresh },
                  { "fSaturationCoefficient", param.saturation_coefficient },
                  { "fExperimentalDataIsCorrectedForSaturation", param.experimental_data_is_corrected_for_saturation },
                  { "fTimeRes", param.time_res },
                  { "fEResRel", param.energy_res_rel },
                  { "fEnergyGain", param.energy_gain },
                  { "fPedestal", param.pedestal },
                  { "fTimeMax", param.max_time },
                  { "fTimeMin", param.min_time },
                  { "fPileUpTimeWindow", param.pileup_time_window },
                  { "fPileUpDistance", param.pileup_distance },
                  { "fQdcMin", param.min_energy } };
    }

    void from_json(const json& json_obj, Params& param)
    {
        json_obj.at("fPMTThresh").get_to(param.pmt_thresh);
        json_obj.at("fSaturationCoefficient").get_to(param.saturation_coefficient);
        json_obj.at("fExperimentalDataIsCorrectedForSaturation")
            .get_to(param.experimental_data_is_corrected_for_saturation);
        json_obj.at("fTimeRes").get_to(param.time_res);
        json_obj.at("fEResRel").get_to(param.energy_res_rel);
        json_obj.at("fEnergyGain").get_to(param.energy_gain);
        json_obj.at("fPedestal").get_to(param.pedestal);
        json_obj.at("fTimeMax").get_to(param.max_time);
        json_obj.at("fTimeMin").get_to(param.min_time);
        json_obj.at("fPileUpTimeWindow").get_to(param.pileup_time_window);
        json_obj.at("fPileUpDistance").get_to(param.pileup_distance);
        json_obj.at("fQdcMin").get_to(param.min_energy);
    }

} // namespace R3B::Digitizing::Neuland::Tamex
namespace R3B::Neuland
{
    template <>
    void to_json(nlohmann::ordered_json& json_obj, const MinimizerLimVar& option)
    {
        json_obj = nlohmann::ordered_json{
            { "init", option.init }, { "step", option.step }, { "lower", option.lower }, { "upper", option.upper }
        };
    }

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, MinimizerLimVar& option)
    {
        json_obj.at("init").get_to(option.init);
        json_obj.at("step").get_to(option.step);
        json_obj.at("lower").get_to(option.lower);
        json_obj.at("upper").get_to(option.upper);
    }

    // =============================================================================================
    // general options:
    template <>
    void to_json(nlohmann::ordered_json& json_obj, const Application::Options& option)
    {
        json_obj =
            nlohmann::ordered_json{ { "run-id", option.run_id },         { "number-of-events", option.event_num },
                                    { "enable-mpi", option.enable_mpi }, { "log-level", option.log_level },
                                    { "input", option.input },           { "output", option.output } };
    }

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, Application::Options& option)
    {
        json_obj.at("run-id").get_to(option.run_id);
        json_obj.at("number-of-events").get_to(option.event_num);
        json_obj.at("enable-mpi").get_to(option.enable_mpi);
        json_obj.at("log-level").get_to(option.log_level);
        json_obj.at("input").get_to(option.input);
        json_obj.at("output").get_to(option.output);
    }

    template <>
    void to_json(nlohmann::ordered_json& json_obj, const Application::Options::Input& option)
    {
        json_obj = nlohmann::ordered_json{
            { "working-dir", option.working_dir }, { "data", option.data },        { "tree-data", option.tree_data },
            { "first-par", option.par },           { "second-par", option.par_2 },
        };
    }

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, Application::Options::Input& option)
    {
        json_obj.at("working-dir").get_to(option.working_dir);
        json_obj.at("data").get_to(option.data);
        json_obj.at("tree-data").get_to(option.tree_data);
        json_obj.at("first-par").get_to(option.par);
        json_obj.at("second-par").get_to(option.par_2);
    }

    template <>
    void to_json(nlohmann::ordered_json& json_obj, const Application::Options::Output& option)
    {
        json_obj = nlohmann::ordered_json{
            { "working-dir", option.working_dir },
            { "data", option.data },
            { "par", option.par },
        };
    }

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, Application::Options::Output& option)
    {
        json_obj.at("working-dir").get_to(option.working_dir);
        json_obj.at("data").get_to(option.data);
        json_obj.at("par").get_to(option.par);
    }

    // =============================================================================================
    // Anaysis options:
    template <>
    void to_json(json& json_obj, const AnalysisApplication::Options& option)
    {
        json_obj = json{ { "general", option.general }, { "tasks", option.tasks } };
    }

    template <>
    void from_json(const json& json_obj, AnalysisApplication::Options& option)
    {
        json_obj.at("general").get_to(option.general);
        json_obj.at("tasks").get_to(option.tasks);
    }

    template <>
    void to_json(json& json_obj, const AnalysisApplication::Options::Tasks& option)
    {
        json_obj = json{
            { option.digi.name, option.digi },
            { option.sim_cal_to_cal.name, option.sim_cal_to_cal },
            { option.hit_monitor.name, option.hit_monitor },
            { option.prim_inter_finder.name, option.prim_inter_finder },
            { option.cluster_finder.name, option.cluster_finder },
            { option.prim_cluster_finder.name, option.prim_cluster_finder },
            { option.multi_calorimeter_train.name, option.multi_calorimeter_train },
            { option.multi_bayes_train.name, option.multi_bayes_train },
            { option.multi_bayes.name, option.multi_bayes },
            { option.neutron_r_value.name, option.neutron_r_value },
            { option.cal_to_hit_par_task.name, option.cal_to_hit_par_task },
        };
    }

    template <>
    void from_json(const json& json_obj, AnalysisApplication::Options::Tasks& option)
    {
        json_obj.at(option.digi.name).get_to(option.digi);
        json_obj.at(option.hit_monitor.name).get_to(option.hit_monitor);
        json_obj.at(option.prim_inter_finder.name).get_to(option.prim_inter_finder);
        json_obj.at(option.cluster_finder.name).get_to(option.cluster_finder);
        json_obj.at(option.prim_cluster_finder.name).get_to(option.prim_cluster_finder);
        json_obj.at(option.multi_calorimeter_train.name).get_to(option.multi_calorimeter_train);
        json_obj.at(option.multi_bayes_train.name).get_to(option.multi_bayes_train);
        json_obj.at(option.multi_bayes.name).get_to(option.multi_bayes);
        json_obj.at(option.neutron_r_value.name).get_to(option.neutron_r_value);
        json_obj.at(option.sim_cal_to_cal.name).get_to(option.sim_cal_to_cal);
        json_obj.at(option.cal_to_hit_par_task.name).get_to(option.cal_to_hit_par_task);
    }

    // =============================================================================================
    // tasks specialization:
    template <>
    void to_json(json& json_obj, const AnalysisApplication::Options::Tasks::Digi& option)
    {
        json_obj = json{
            { "enable", option.enable },
            { "channel", option.channel },
            { "paddle", option.paddle },
            { "par", option.tamex_par },
            { "pileup-strategy", magic_enum::enum_name(option.pileup_strategy) },
            { "enable-sim-cal", option.enable_sim_cal },
            { "enable-hit-par", option.enable_hit_par },
            { "enable-size-monitor", option.enable_size_monitor },
            { "read", option.read },
            { "write", option.write },
        };
    }

    template <>
    void from_json(const json& json_obj, AnalysisApplication::Options::Tasks::Digi& option)
    {
        json_obj.at("enable").get_to(option.enable);
        json_obj.at("channel").get_to(option.channel);
        json_obj.at("paddle").get_to(option.paddle);
        json_obj.at("par").get_to(option.tamex_par);
        json_obj.at("enable-sim-cal").get_to(option.enable_sim_cal);
        json_obj.at("enable-hit-par").get_to(option.enable_hit_par);
        json_obj.at("enable-size-monitor").get_to(option.enable_size_monitor);
        json_obj.at("read").get_to(option.read);
        json_obj.at("write").get_to(option.write);

        // parse enum string
        auto enum_name = std::string{};
        json_obj.at("pileup-strategy").get_to(enum_name);
        auto enum_val = magic_enum::enum_cast<Digitizing::Neuland::Tamex::PeakPileUpStrategy>(
            enum_name, magic_enum::case_insensitive);
        if (enum_val.has_value())
        {
            option.pileup_strategy = enum_val.value();
        }
        else
        {
            throw R3B::logic_error(fmt::format("Cannot parse the enum string {:?} to PeakPileUpStrategy enum class. "
                                               "Please check if the enum string is correct!",
                                               enum_name));
        }
    }

    template <>
    void to_json(json& json_obj, const AnalysisApplication::Options::Tasks::MultiTrain& option)
    {
        json_obj = json{
            { "enable", option.enable },
            { "use-hit", option.use_hit },
            { "weight", option.weight },
            { "edep-opt", option.edep_opt },
            { "edep-off-opt", option.edep_off_opt },
            { "n-cluster-opt", option.n_cluster_opt },
            { "n-cluster-off-opt", option.n_cluster_off_opt },
            { "read", option.read },
            { "write", option.write },
        };
    }

    template <>
    void from_json(const json& json_obj, AnalysisApplication::Options::Tasks::MultiTrain& option)
    {
        json_obj.at("enable").get_to(option.enable);
        json_obj.at("use-hit").get_to(option.use_hit);
        json_obj.at("weight").get_to(option.weight);
        json_obj.at("edep-opt").get_to(option.edep_opt);
        json_obj.at("edep-off-opt").get_to(option.edep_off_opt);
        json_obj.at("n-cluster-opt").get_to(option.n_cluster_opt);
        json_obj.at("n-cluster-off-opt").get_to(option.n_cluster_off_opt);
        json_obj.at("read").get_to(option.read);
        json_obj.at("write").get_to(option.write);
    }

    template <>
    void to_json(json& json_obj, const AnalysisApplication::Options::Tasks::NeutronRValue& option)
    {
        json_obj = json{
            { "enable", option.enable },
            { "neutron-energy-MeV", option.neutron_energy_mev },
            { "read", option.read },
            { "write", option.write },
        };
    }

    template <>
    void from_json(const json& json_obj, AnalysisApplication::Options::Tasks::NeutronRValue& option)
    {
        json_obj.at("enable").get_to(option.enable);
        json_obj.at("neutron-energy-MeV").get_to(option.neutron_energy_mev);
        json_obj.at("read").get_to(option.read);
        json_obj.at("write").get_to(option.write);
    }

    template <>
    void to_json(json& json_obj, const AnalysisApplication::Options::Tasks::Cal2HitParTask& option)
    {
        json_obj = json{
            { "enable", option.enable },
            { "min-stat", option.min_stat },
            { "method", magic_enum::enum_name(option.method) },
            { "read", option.read },
            { "write", option.write },
        };
    }

    template <>
    void from_json(const json& json_obj, AnalysisApplication::Options::Tasks::Cal2HitParTask& option)
    {
        json_obj.at("enable").get_to(option.enable);
        json_obj.at("min-stat").get_to(option.min_stat);
        json_obj.at("read").get_to(option.read);
        json_obj.at("write").get_to(option.write);

        auto method_str = std::string{};
        json_obj.at("method").get_to(method_str);

        auto enum_val = magic_enum::enum_cast<Cal2HitParMethod>(method_str, magic_enum::case_insensitive);
        if (enum_val.has_value())
        {
            option.method = enum_val.value();
        }
        else
        {
            throw R3B::logic_error(fmt::format("{} cannot be parsed to the enum class Cal2HitParMethod!", method_str));
        }
    }

    // =============================================================================================
    // Simuation options:
    template <>
    void to_json(nlohmann::ordered_json& json_obj, const SimulationApplication::Options& option)
    {
        json_obj = json{ { "general", option.general },
                         { "simulation", option.simulation },
                         { "detectors", option.detectors } };
    }

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, SimulationApplication::Options& option)
    {
        json_obj.at("general").get_to(option.general);
        json_obj.at("simulation").get_to(option.simulation);
        json_obj.at("detectors").get_to(option.detectors);
    }

    template <>
    void to_json(nlohmann::ordered_json& json_obj, const SimulationApplication::Options::Simulation& option)
    {
        json_obj = json{
            { "event-print-num", option.event_print_num },
            { "store-trajectory", option.store_trajectory },
            { "material-filename", option.material_filename },
            { "engine", option.engine },
            { "random-seed", option.random_seed },
            { "generator", option.generator },
        };
    }

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, SimulationApplication::Options::Simulation& option)
    {
        json_obj.at("event-print-num").get_to(option.event_print_num);
        json_obj.at("store-trajectory").get_to(option.store_trajectory);
        json_obj.at("material-filename").get_to(option.material_filename);
        json_obj.at("engine").get_to(option.engine);
        json_obj.at("generator").get_to(option.generator);
    }

    template <>
    void to_json(nlohmann::ordered_json& json_obj, const SimulationApplication::Options::Detector& option)
    {
        json_obj = json{ { "cave", option.cave }, { "neuland", option.neuland } };
    }

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, SimulationApplication::Options::Detector& option)
    {
        json_obj.at("cave").get_to(option.cave);
        json_obj.at("neuland").get_to(option.neuland);
    }

    // =============================================================================================
    // Detector options:

    template <>
    void to_json(nlohmann::ordered_json& json_obj, const SimulationApplication::Options::Detector::Cave& option)
    {
        json_obj = json{ { "enable", option.enable }, { "name", option.name }, { "geo-file", option.geo_file } };
    }

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, SimulationApplication::Options::Detector::Cave& option)
    {
        json_obj.at("enable").get_to(option.enable);
        json_obj.at("name").get_to(option.name);
        json_obj.at("geo-file").get_to(option.geo_file);
    }

    template <>
    void to_json(nlohmann::ordered_json& json_obj, const SimulationApplication::Options::Detector::Neuland& option)
    {
        json_obj = json{ { "enable", option.enable },
                         { "name", option.name },
                         { "num-of-dp", option.num_of_dp },
                         { "location", option.location },
                         { "enable-auto-geo-build", option.enable_auto_geo_build } };
    }

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, SimulationApplication::Options::Detector::Neuland& option)
    {
        json_obj.at("enable").get_to(option.enable);
        json_obj.at("name").get_to(option.name);
        json_obj.at("num-of-dp").get_to(option.num_of_dp);
        json_obj.at("location").get_to(option.location);
        json_obj.at("enable-auto-geo-build").get_to(option.enable_auto_geo_build);
    }
} // namespace R3B::Neuland
