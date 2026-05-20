#include "R3BNeulandSimApp.h"
#include "R3BCave.h"
#include "R3BNeuland.h"
#include "R3BNeulandApp.h"
#include <FairRun.h>
#include <FairRunSim.h>
#include <G4RunManager.hh>
#include <R3BFieldConst.h>
#include <R3BNeulandGeneratorFactory.h>
#include <TG4EventAction.h>
#include <TGeoMatrix.h>
#include <TRandom3.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace R3B::Neuland
{
    SimulationApplication::SimulationApplication()
        : CLIApplication{ "neuland_sim", std::make_unique<FairRunSim>(), std::ref(options_.general) }
    {
        options_.general.input.par.clear();
        options_.general.input.data.clear();
        options_.general.output.data = "sim.output.root";
        options_.general.output.par = "sim.par.root";
    }

    void SimulationApplication::pre_init(FairRun* run)
    {
        auto* sim_run = dynamic_cast<FairRunSim*>(run);
        setup_engine(sim_run);
        setup_generator(sim_run);
        setup_detectors(sim_run);
    }

    void SimulationApplication::post_init(FairRun* /*run*/)
    {
        const auto& options = options_.simulation;
        auto* geant_run = G4RunManager::GetRunManager();
        geant_run->SetPrintProgress(options.event_print_num);
        auto* event =
            dynamic_cast<TG4EventAction*>(const_cast<G4UserEventAction*>(geant_run->GetUserEventAction())); // NOLINT
        event->VerboseLevel(0);
    }

    void SimulationApplication::setup_engine(FairRunSim* run)
    {
        const auto& options = options_;
        run->SetName(options.simulation.engine.c_str());
        run->SetRunId(options.general.run_id);
        run->SetStoreTraj(options.simulation.store_trajectory);
        run->SetMaterials(options.simulation.material_filename.c_str());
        auto fairField = std::make_unique<R3BFieldConst>();
        run->SetField(fairField.release());
    }

    void SimulationApplication::setup_generator(FairRunSim* run)
    {
        const auto& options_pos = options_.detectors.neuland;
        random_gen_ = std::make_unique<TRandom3>(options_.simulation.random_seed);
        generator_factory_.SetNumOfPlanes(options_pos.num_of_dp * 2);
        generator_factory_.SetRandomGen(random_gen_.get());
        run->SetGenerator(generator_factory_.Create().release());
    }

    void SimulationApplication::setup_detectors(FairRunSim* run) const
    {
        const auto& options = options_.detectors;

        const auto& cave_options = options.cave;
        if (cave_options.enable)
        {
            auto cave = std::make_unique<R3BCave>(cave_options.name.c_str());
            cave->SetGeometryFileName(cave_options.geo_file.c_str());
            run->AddModule(cave.release());
        }

        const auto& neuland_options = options.neuland;
        if (neuland_options.enable)
        {

            const auto& location = neuland_options.location;
            auto const neulandGeoTrans = TGeoTranslation{ location.x(), location.y(), location.z() };
            auto neuland = std::make_unique<R3BNeuland>(neuland_options.num_of_dp, neulandGeoTrans);
            if (neuland_options.enable_auto_geo_build)
            {
                neuland->EnableAutoGeoBuild();
            }
            run->AddModule(neuland.release());
        }
    }

    void SimulationApplication::ParseApplicationOption(const std::vector<std::string>& filename)
    {
        ParseApplicationOptionImp(filename, options_);
    }

    void SimulationApplication::print_json_options() { CLIApplication::print_json_options(options_); }

    void SimulationApplication::dump_json_options(const std::string& filename)
    {
        CLIApplication::dump_json_options(options_, filename);
    }

    void SimulationApplication::run_action(FairRun* run, int num_of_events) { run->Run(num_of_events); }
} // namespace R3B::Neuland
