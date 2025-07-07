#pragma once

#include "R3BDigitizingTamex.h"
#include "R3BFTCalEngine.h"
#include "R3BNeulandApp.h"
#include "R3BNeulandDigitizer.h"
#include "R3BNeulandTriggerTypes.h"
#include <R3BNeulandCalToHitParTask.h>
#include <string>
#include <string_view>
#include <vector>

namespace R3B::Neuland
{
    namespace Digitizing = R3B::Digitizing;
    namespace Tamex = Digitizing::Neuland::Tamex;
    constexpr auto RVAUE_DEFAULT_NEUTRON_ENERGY = 600.;

    struct MinimizerLimVar
    {
        double init{};
        double step{};
        double lower{};
        double upper{};
    };

    constexpr auto DEFAULT_EDEP_OPT = MinimizerLimVar{ 200., 25., 50., 1500. };
    constexpr auto DEFAULT_EDEP_OFF_OPT = MinimizerLimVar{ 5., 1., 0., 250. };
    constexpr auto DEFAULT_N_CLUSTER_OPT = MinimizerLimVar{ 10., 5., 5., 50. };
    constexpr auto DEFAULT_N_CLUSTER_OFF_OPT = MinimizerLimVar{ 2., 1., 0., 10. };
    constexpr auto DEFAULT_MIN_STAT = 10;

    class AnalysisApplication : public CLIApplication
    {
      public:
        using TamexChannel = Tamex::Channel;

        struct Options
        {
            CLIApplication::Options general;
            struct Tasks
            {
                R3B::Neuland::DigiTaskOptions digi;
                struct SimCal2Cal
                {
                    bool enable = false;
                    std::string name = "NeulandSimCal2Cal";
                    std::string read = "NeulandSimCal";
                    std::string write = "NeulandCalData";
                } sim_cal_to_cal;
                struct HitMon
                {
                    bool enable = false;
                    std::string name = "NeulandHitMon";
                    std::string read = "NeulandHits";
                    std::string write;
                } hit_monitor;
                struct PrimInteractionFinder
                {
                    bool enable = false;
                    std::string name = "NeulandPrimaryInteractionFinder";
                    std::string read = "NeulandPoints;NeulandHits";
                    std::string write = "NeulandPrimaryPoints;NeulandPrimaryHits;NeulandPrimaryTracks";
                } prim_inter_finder;
                struct ClusterFinder
                {
                    std::string read = "NeulandHits";
                    std::string write = "NeulandClusters";
                    bool enable = false;
                    std::string name = "NeulandClusterFinder";
                } cluster_finder;
                struct PrimClusterFinder
                {
                    bool enable = false;
                    std::string name = "NeulandPrimaryClusterFinder";
                    std::string read = "NeulandPrimaryHits;NeulandClusters";
                    std::string write = "NeulandPrimaryClusters;NeulandSecondaryClusters";
                } prim_cluster_finder;
                struct MultiTrain
                {
                    bool enable = false;
                    bool use_hit = false;
                    double weight = 0.;
                    MinimizerLimVar edep_opt = DEFAULT_EDEP_OPT;
                    MinimizerLimVar edep_off_opt = DEFAULT_EDEP_OFF_OPT;
                    MinimizerLimVar n_cluster_opt = DEFAULT_N_CLUSTER_OPT;
                    MinimizerLimVar n_cluster_off_opt = DEFAULT_N_CLUSTER_OFF_OPT;
                    std::string name = "NeulandMultiplicityCalorimetricTrain";
                    std::string read = "NeulandClusters;NeulandPrimaryTracks;NeulandPrimaryHits";
                    std::string write;
                } multi_calorimeter_train;
                struct MultiBayesTrain
                {
                    bool enable = false;
                    std::string name = "NeulandMultiplicityBayesTrain";
                    std::string read = "NeulandClusters;NeulandPrimaryTracks";
                    std::string write;
                } multi_bayes_train;
                struct MultiBayes
                {
                    bool enable = false;
                    std::string name = "NeulandMultiplicityBayes";
                    std::string read = "NeulandClusters";
                    std::string write = "NeulandMultiplicity";
                } multi_bayes;
                struct NeutronRValue
                {
                    bool enable = false;
                    double neutron_energy_mev = RVAUE_DEFAULT_NEUTRON_ENERGY;
                    std::string name = "NeulandNeutronsRValue";
                    std::string read = "NeulandMultiplicity;NeulandClusters";
                    std::string write = "NeulandNeutrons";
                } neutron_r_value;
                struct MapDataConverterTask
                {
                    bool enable = false;
                    std::string name = "MapDataConverterTask";
                    std::string read = "NeulandMappedData;NeulandTrigMappedData";
                    std::string write = "NeulandMapData;NeulandTrigMapData";
                } map_data_converter_task;
                struct Map2CalParTask
                {
                    bool enable = false;
                    bool has_trig_enabled = true;
                    calibration::FTCalErrorMethod error_method = calibration::FTCalErrorMethod::approx;
                    std::string name = "NeulandMap2CalParTask";
                    std::string read = "NeulandMapData;NeulandTrigMapData";
                    std::string write = "LandTCalPar;LandTrigTCalPar";
                } map_to_cal_par_task;
                struct Map2CalTask
                {
                    bool enable = false;
                    bool enable_pulse_mode = false;
                    bool enable_walk_effect = true;
                    int min_stat = 1;
                    std::string name = "NeulandMap2CalTask";
                    std::string read = "NeulandMapData;NeulandTrigMapData;LandTCalPar;LandTrigTCalPar";
                    std::string write = "NeulandCalData";
                } map_to_cal_task;
                struct Cal2HitParTask
                {
                    bool enable = false;
                    int min_stat = DEFAULT_MIN_STAT;
                    CalTrigger mode = CalTrigger::offspill;
                    Cal2HitParMethod method = Cal2HitParMethod::recons;
                    std::string name = "NeulandCal2HitParTask";
                    std::string read = "NeulandCalData;NeulandCalibrationBasePar";
                    std::string write = "NeulandHitPar";
                } cal_to_hit_par_task;
            } tasks;
        };

        explicit AnalysisApplication();

        void set_channel(std::string_view channel) { options_.tasks.digi.channel = channel; }
        void set_paddle(std::string_view paddle) { options_.tasks.digi.paddle = paddle; }

      private:
        Options options_;

        // virtual private member functions:
        void pre_init(FairRun* run) override;
        void print_json_options() override;
        void dump_json_options(const std::string& filename) override;
        void setup_application_options(CLI::App& program_options) override;
        void ParseApplicationOption(const std::vector<std::string>& filename) override;

        // non-virtual private member functions:
        void set_parameters();
        auto create_neuland_digi_engine_map(const R3B::Neuland::DigiTaskOptions& option, std::string_view hit_par_name);
    };
} // namespace R3B::Neuland
