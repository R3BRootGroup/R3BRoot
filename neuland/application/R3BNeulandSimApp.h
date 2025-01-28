#pragma once

#include "R3BNeulandApp.h"
#include <R3BNeulandCommon.h>

class FairRunSim;
class TRandom3;

namespace R3B::Neuland
{
    constexpr auto DEFAULT_NEULAND_Z_POS = 1650.; // cm
    constexpr auto DEFAULT_NEULAND_ENERGY = 0.6;  // GeV

    struct XYZCoordinate
    {
        double x{};
        double y{};
        double z{};
    };

    class SimulationApplication : public Application
    {
      public:
        struct Options
        {
            Application::Options general;
            struct Simulation
            {
                int event_print_num = 1;
                bool store_trajectory = true;
                std::string material_filename = "media_r3b.geo";
                std::string engine = "TGeant4";
                struct Generator
                {
                    int random_seed = 0;
                    std::string type = "box";
                    int multiplicity = 1;
                    double energy = DEFAULT_NEULAND_ENERGY;
                } generator;
            } simulation;
            struct Detector
            {
                struct Cave
                {
                    bool enable = true;
                    std::string name = "CAVE";
                    std::string geo_file = "r3b_cave.geo";
                } cave;
                struct Neuland
                {
                    bool enable = true;
                    std::string name = "Neuland";
                    int num_of_dp = MaxNumberOfPlanes / 2;
                    XYZCoordinate location{ 0, 0, DEFAULT_NEULAND_Z_POS };
                    bool enable_auto_geo_build = true;
                } neuland;
            } detectors;
        };
        explicit SimulationApplication();

      private:
        Options options_;
        std::unique_ptr<TRandom3> random_gen_;

        // virtual private member functions:
        void pre_init(FairRun* run) override;
        void post_init(FairRun* run) override;
        void print_json_options() override;
        void dump_json_options(const std::string& filename) override;
        void run_action(FairRun* run, int num_of_events) override;
        void ParseApplicationOption(const std::vector<std::string>& filename) override
        {
            ParseApplicationOptionImp(filename, options_);
        }

        // non-virtual private member functions:
        void setup_engine(FairRunSim* run);
        void setup_generator(FairRunSim* run);
        void setup_detectors(FairRunSim* run) const;
    };

} // namespace R3B::Neuland
