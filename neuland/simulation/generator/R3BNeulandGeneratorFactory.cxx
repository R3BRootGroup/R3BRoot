#include "R3BNeulandGeneratorFactory.h"
#include "CosmicMuon.h"
#include "CosmicMuonDistributions.h"
#include "R3BException.h"
#include "R3BNeulandCommon.h"
#include "R3BPDGConverter.h"
#include <FairBoxGenerator.h>
#include <FairPrimaryGenerator.h>
#include <R3BROOTTypeJson.h> // NOLINT
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

namespace R3B::Neuland
{

    auto GeneratorFactory::Create() -> std::unique_ptr<FairPrimaryGenerator>
    {
        switch (options_.get().generator_type)
        {
            case GeneratorType::box:
                return create_box_generator();
                break;
            case GeneratorType::muon:
                return create_muon_generator();
                break;
            default:
                throw R3B::logic_error("Unrecgonized generator type");
        }
    }

    auto GeneratorFactory::create_muon_generator() -> std::unique_ptr<FairPrimaryGenerator>
    {
        auto& options = options_.get();
        const auto& position = options.position;

        auto detector_box_size = ::R3B::Neuland::DetectorBoxSize{};
        detector_box_size.xmin = position.x() - R3B::Neuland::BarLength / 2;
        detector_box_size.xmax = position.x() + R3B::Neuland::BarLength / 2;
        detector_box_size.ymin = position.y() - R3B::Neuland::BarLength / 2;
        detector_box_size.ymax = position.y() + R3B::Neuland::BarLength / 2;
        detector_box_size.zmin = position.z() - (R3B::Neuland::BarSize_Z * num_of_planes_ / 2);
        detector_box_size.zmax = position.z() + (R3B::Neuland::BarSize_Z * num_of_planes_ / 2);

        auto angle_dist = R3B::Neuland::AngleDist{};
        auto energy_dist = R3B::Neuland::EnergyDist{};
        auto position_dist = R3B::Neuland::PositionDist{};

        energy_dist.set_mean_sigma(options.energy.value, options.energy.error);

        position_dist.set_box_size(detector_box_size);

        auto CosmicMuonGenerator = R3B::Neuland::CreateTrackGenerator(angle_dist, energy_dist, position_dist);

        CosmicMuonGenerator->set_rd_engine(random_gen_);

        auto primGen = std::make_unique<FairPrimaryGenerator>();
        primGen->AddGenerator(CosmicMuonGenerator.release());
        return primGen;
    }

    auto GeneratorFactory::create_box_generator() -> std::unique_ptr<FairPrimaryGenerator>
    {
        // Primary particle generator
        auto& options = options_.get();
        auto converter = R3B::PDGConverter();
        auto boxGen =
            std::make_unique<FairBoxGenerator>(converter.get_pid(options.particle_type), options.multiplicity);
        const auto& position = options.position;
        boxGen->SetXYZ(position.x(), position.y(), position.z());
        boxGen->SetThetaRange(options.theta.min, options.theta.max);
        boxGen->SetPhiRange(options.phi.min, options.phi.max);
        boxGen->SetEkinRange(options.energy.value - options.energy.error, options.energy.value + options.energy.error);
        auto primGen = std::make_unique<FairPrimaryGenerator>();
        primGen->AddGenerator(boxGen.release());
        return primGen;
    }

    void to_json(nlohmann::ordered_json& json_obj, const GeneratorFactory::Options& options)
    {
        json_obj = nlohmann::ordered_json{
            { "type", magic_enum::enum_name(options.generator_type) },
            { "particle", options.particle_type },
            { "energy", options.energy },
            { "theta", options.theta },
            { "phi", options.phi },
            { "position", options.position },
        };
    }

    void from_json(const nlohmann::ordered_json& json_obj, GeneratorFactory::Options& options)
    {
        auto type_str = std::string{};
        json_obj.at("type").get_to(type_str);
        json_obj.at("particle").get_to(options.particle_type);
        json_obj.at("energy").get_to(options.energy);
        json_obj.at("theta").get_to(options.theta);
        json_obj.at("phi").get_to(options.phi);
        json_obj.at("position").get_to(options.position);
        auto type_str_val = magic_enum::enum_cast<GeneratorType>(type_str, magic_enum::case_insensitive);
        if (type_str_val.has_value())
        {
            options.generator_type = type_str_val.value();
        }
    }

} // namespace R3B::Neuland
