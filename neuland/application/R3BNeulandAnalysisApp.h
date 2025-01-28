#pragma once

#include "R3BDigitizingTamex.h"
#include "R3BNeulandApp.h"

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

    class AnalysisApplication : public Application
    {
      public:
        using TamexChannel = Tamex::Channel;

        struct Options
        {
            Application::Options general;
            struct Tasks
            {
                struct Digi
                {
                    bool enable = true;
                    std::string name = "NeulandDigitizer";
                    std::string channel = "tamex";
                    std::string paddle = "neuland";
                    bool enable_sim_cal = false;
                    bool enable_hit_par = false;
                    R3B::Digitizing::Neuland::Tamex::Params tamex_par{ TamexChannel::GetDefaultRandomGen() };
                    Digitizing::Neuland::Tamex::PeakPileUpStrategy pileup_strategy =
                        Digitizing::Neuland::Tamex::PeakPileUpStrategy::width;
                } digi;
                struct SimCal2Cal
                {
                    bool enable = false;
                    std::string name = "NeulandSimCal2Cal";
                } sim_cal_to_cal;
                struct HitMon
                {
                    bool enable = false;
                    std::string name = "NeulandHitMon";
                } hit_monitor;
                struct PrimInteractionFinder
                {
                    bool enable = false;
                    std::string name = "NeulandPrimaryInteractionFinder";
                } prim_inter_finder;
                struct ClusterFinder
                {
                    bool enable = false;
                    std::string name = "NeulandClusterFinder";
                } cluster_finder;
                struct PrimClusterFinder
                {
                    bool enable = false;
                    std::string name = "NeulandPrimaryClusterFinder";
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
                } multi_calorimeter_train;
                struct MultiBayesTrain
                {
                    bool enable = false;
                    std::string name = "NeulandMultiplicityBayesTrain";
                } multi_bayes_train;
                struct MultiBayes
                {
                    bool enable = false;
                    std::string name = "NeulandMultiplicityBayes";
                } multi_bayes;
                struct NeutronRValue
                {
                    bool enable = false;
                    std::string name = "NeulandNeutronsRValue";
                    double neutron_energy_mev = RVAUE_DEFAULT_NEUTRON_ENERGY;
                } neutron_r_value;
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
        void ParseApplicationOption(const std::vector<std::string>& filename) override
        {
            ParseApplicationOptionImp(filename, options_);
        }

        // non-virtual private member functions:
        void set_parameters();
        auto create_neuland_digi_engine_map(Tamex::PeakPileUpStrategy pileup_strategy,
                                            const Tamex::Params& tamex_par,
                                            bool has_cal_to_hit_par = false);
    };
} // namespace R3B::Neuland
