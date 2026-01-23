#pragma once

#include "R3BDigitizingTamex.h"
#include "R3BFTCalEngine.h"
#include "R3BNeulandApp.h"
#include "R3BNeulandDigitizer.h"
#include "R3BNeulandHitCosmicMonitorTask.h"
#include "R3BNeulandHitOnlineMonitorTask.h"
#include "R3BNeulandMapToCalTask.h"
#include "R3BNeulandTriggerTypes.h"
#include <R3BNeulandCalToHitParTask.h>
#include <string>
#include <string_view>
#include <vector>

namespace R3B::Neuland
{
    namespace Digitizing = R3B::Digitizing;
    namespace Tamex = Digitizing::Neuland::Tamex;
    constexpr auto RVALUE_DEFAULT_NEUTRON_ENERGY = 600.;

    struct MinimizerLimVar
    {
        double init{};
        double step{};
        double lower{};
        double upper{};
    };

    constexpr auto DEFAULT_EDEP_OPT = MinimizerLimVar{ .init = 200., .step = 25., .lower = 50., .upper = 1500. };
    constexpr auto DEFAULT_EDEP_OFF_OPT = MinimizerLimVar{ .init = 5., .step = 1., .lower = 0., .upper = 250. };
    constexpr auto DEFAULT_N_CLUSTER_OPT = MinimizerLimVar{ .init = 10., .step = 5., .lower = 5., .upper = 50. };
    constexpr auto DEFAULT_N_CLUSTER_OFF_OPT = MinimizerLimVar{ .init = 2., .step = 1., .lower = 0., .upper = 10. };

    class AnalysisApplication : public CLIApplication
    {
      public:
        using TamexChannel = Tamex::Channel;

        struct Options
        {
            CLIApplication::Options general;
            struct Tasks
            {
                R3B::Neuland::DigiTaskOptions neuland_digitizer;
                struct SimCal2Cal
                {
                    bool enable = false;
                    std::string name = "NeulandSimCal2Cal";
                    std::string read = "NeulandSimCal";
                    std::string write = "NeulandCalData";
                } neuland_sim_cal_to_cal;
                struct HitMon
                {
                    bool enable = false;
                    std::string name = "NeulandHitMon";
                    std::string read = "NeulandHits";
                    std::string write;
                } neuland_hit_mon;
                struct PrimInteractionFinder
                {
                    bool enable = false;
                    std::string name = "NeulandPrimaryInteractionFinder";
                    std::string read = "NeulandPoints;NeulandHits";
                    std::string write = "NeulandPrimaryPoints;NeulandPrimaryHits;NeulandPrimaryTracks";
                } neuland_primary_interaction_finder;
                struct ClusterFinder
                {
                    std::string read = "NeulandHits";
                    std::string write = "NeulandClusters";
                    bool enable = false;
                    std::string name = "NeulandClusterFinder";
                } neuland_cluster_finder;
                struct PrimClusterFinder
                {
                    bool enable = false;
                    std::string name = "NeulandPrimaryClusterFinder";
                    std::string read = "NeulandPrimaryHits;NeulandClusters";
                    std::string write = "NeulandPrimaryClusters;NeulandSecondaryClusters";
                } neuland_primary_cluster_finder;
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
                } neuland_multi_calorimeter_train;
                struct MultiBayesTrain
                {
                    bool enable = false;
                    std::string name = "NeulandMultiplicityBayesTrain";
                    std::string read = "NeulandClusters;NeulandPrimaryTracks";
                    std::string write;
                } neuland_multi_bayes_train;
                struct MultiBayes
                {
                    bool enable = false;
                    std::string name = "NeulandMultiplicityBayes";
                    std::string read = "NeulandClusters";
                    std::string write = "NeulandMultiplicity";
                } neuland_multi_bayes;
                struct NeutronRValue
                {
                    bool enable = false;
                    double neutron_energy_mev = RVALUE_DEFAULT_NEUTRON_ENERGY;
                    std::string name = "NeulandNeutronsRValue";
                    std::string read = "NeulandMultiplicity;NeulandClusters";
                    std::string write = "NeulandNeutrons";
                } neuland_neutron_r_value;
                struct MapDataConverterTask
                {
                    bool enable = false;
                    std::string name = "NeulandMapDataConverterTask";
                    std::string read = "NeulandMappedData;NeulandTrigMappedData";
                    std::string write = "NeulandMapData;NeulandTrigMapData";
                } neuland_map_data_converter_task;
                struct Map2CalParTask
                {
                    bool enable = false;
                    bool has_trig_enabled = true;
                    calibration::FTCalErrorMethod error_method = calibration::FTCalErrorMethod::approx;
                    std::string name = "NeulandMap2CalParTask";
                    std::string read = "NeulandMapData;NeulandTrigMapData";
                    std::string write = "LandTCalPar;LandTrigTCalPar";
                } neuland_map_to_cal_par_task;
                struct Map2CalTaskConfig neuland_map_to_cal_task;
                struct Cal2HitParTaskConfig neuland_cal_to_hit_par_task;
                struct Cal2HitTask
                {
                    bool enable = false;
                    CalTrigger mode = CalTrigger::offspill;
                    double global_time_offset = 0.;
                    std::string name = "NeulandCal2HitTask";
                    std::string read = "NeulandCalData;NeulandHitPar";
                    std::string write = "NeulandHits";
                } neuland_cal_to_hit_task;
                struct CalMonitorTask
                {
                    bool enable = false;
                    CalTrigger mode = CalTrigger::offspill;
                    std::string name = "NeulandCalMonitorTask";
                    std::string read = "NeulandCalData";
                    std::string write;
                } neuland_cal_monitor_task;
                Calibration::CosmicMonitorTaskConfig neuland_cosmic_monitor_task;
                Calibration::HitOnlineMonitorTaskConfig neuland_hit_online_monitor_task;
                struct LosMap2CalParTask
                {
                    bool enable = false;
                    std::string name = "LosMap2CalParTask";
                    std::string read = "LosMapped;LosTriggerMapped";
                    std::string write = "LosTCalPar";
                } los_map_to_cal_par_task;
                struct LosMap2CalTask
                {
                    bool enable = false;
                    std::string name = "LosMap2CalTask";
                    std::string read = "LosMapped;LosTriggerMapped;LosTCalPar";
                    std::string write = "LosCal;LosTriggerCal";
                } los_map_to_cal_task;
                struct LosProvideTStart
                {
                    bool enable = false;
                    std::string name = "LosProvideTStart";
                    std::string read = "LosCal;LosTriggerCal";
                    std::string write;
                } los_provide_t_start;
            } tasks;
        };

        explicit AnalysisApplication();

        void set_channel(std::string_view channel) { options_.tasks.neuland_digitizer.channel = channel; }
        void set_paddle(std::string_view paddle) { options_.tasks.neuland_digitizer.paddle = paddle; }

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
