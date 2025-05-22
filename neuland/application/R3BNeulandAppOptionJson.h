#pragma once

#include "R3BNeulandApp.h"
#include "R3BNeulandDigitizer.h"
#include "R3BNeulandJsonHeader.h"
#include <R3BDigitizingTamex.h>
#include <R3BNeulandAnalysisApp.h>
#include <R3BNeulandSimApp.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

namespace R3B::Digitizing::Neuland::Tamex
{
    using json = nlohmann::ordered_json;
    void to_json(json& json_obj, const Params& param);

    void from_json(const json& json_obj, Params& param);

} // namespace R3B::Digitizing::Neuland::Tamex
namespace R3B::Neuland
{
    // =============================================================================================
    // general options:
    template <>
    void to_json(nlohmann::ordered_json& json_obj, const CLIApplication::Options& option);

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, CLIApplication::Options& option);

    template <>
    void to_json(nlohmann::ordered_json& json_obj, const CLIApplication::Options::Input& option);

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, CLIApplication::Options::Input& option);

    template <>
    void to_json(nlohmann::ordered_json& json_obj, const CLIApplication::Options::Output& option);

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, CLIApplication::Options::Output& option);

    // =============================================================================================
    // Anaysis options:
    template <>
    void to_json(nlohmann::ordered_json& json_obj, const AnalysisApplication::Options& option);

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, AnalysisApplication::Options& option);

    template <>
    void to_json(nlohmann::ordered_json& json_obj, const AnalysisApplication::Options::Tasks& option);

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, AnalysisApplication::Options::Tasks& option);

    // =============================================================================================
    // tasks specialization:
    template <>
    void to_json(nlohmann::ordered_json& json_obj, const R3B::Neuland::DigiTaskOptions& option);

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, R3B::Neuland::DigiTaskOptions& option);

    template <>
    void to_json(nlohmann::ordered_json& json_obj, const AnalysisApplication::Options::Tasks::MultiTrain& option);

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, AnalysisApplication::Options::Tasks::MultiTrain& option);

    template <>
    void to_json(nlohmann::ordered_json& json_obj, const AnalysisApplication::Options::Tasks::NeutronRValue& option);

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, AnalysisApplication::Options::Tasks::NeutronRValue& option);

    template <>
    void to_json(nlohmann::ordered_json& json_obj, const AnalysisApplication::Options::Tasks::Cal2HitParTask& option);

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, AnalysisApplication::Options::Tasks::Cal2HitParTask& option);

    // =============================================================================================
    // Simuation options:
    template <>
    void to_json(nlohmann::ordered_json& json_obj, const SimulationApplication::Options& option);

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, SimulationApplication::Options& option);

    template <>
    void to_json(nlohmann::ordered_json& json_obj, const SimulationApplication::Options::Simulation& option);

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, SimulationApplication::Options::Simulation& option);

    template <>
    void to_json(nlohmann::ordered_json& json_obj, const SimulationApplication::Options::Detector& option);

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, SimulationApplication::Options::Detector& option);

    // =============================================================================================
    // Detector options:

    template <>
    void to_json(nlohmann::ordered_json& json_obj, const SimulationApplication::Options::Detector::Cave& option);

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, SimulationApplication::Options::Detector::Cave& option);

    template <>
    void to_json(nlohmann::ordered_json& json_obj, const SimulationApplication::Options::Detector::Neuland& option);

    template <>
    void from_json(const nlohmann::ordered_json& json_obj, SimulationApplication::Options::Detector::Neuland& option);
} // namespace R3B::Neuland
