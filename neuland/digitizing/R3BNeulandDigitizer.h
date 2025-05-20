/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#pragma once
#include "FairTask.h"
#include "Filterable.h"
#include "NeulandParticleFilter.h"
#include "NeulandSimCalData.h"
#include "R3BDataMonitor.h"
#include "R3BDigitizingEngine.h"
#include "R3BDigitizingPaddle.h"
#include "R3BDigitizingPaddleNeuland.h"
#include "R3BDigitizingTacQuila.h"
#include "R3BDigitizingTamex.h"
#include "R3BIOConnector.h"
#include "R3BNeulandGeoPar.h"
#include "R3BNeulandHit.h"
#include "R3BNeulandPoint.h"
#include <Rtypes.h>
#include <RtypesCore.h>
#include <TClonesArray.h>
#include <TH1.h>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

class TGeoNode;
class TH1F;
class TH2F;

/**
 * NeuLAND digitizing finder task
 * @author Yanzhao Wang
 *
 */

namespace R3B::Neuland
{
    /**
     * @class DigiTaskOptions
     * @brief Configuration struct for R3B::Neuland::Digitizer used in R3B::Neuland::AnalysisApplication
     *
     * Example:
     *
     * @code{.cpp}
     *
     * auto option = R3B::Neuland::DigitTaskOptions();
     * option.enable_sim_cal = true;
     * auto task = R3B::Neuland::Digitizer::Create(option, run);
     *
     * @endcode
     */
    struct DigiTaskOptions
    {
        bool enable = false;                   //!< Flag to enable task.
        std::string name = "NeulandDigitizer"; //!< Name of the task.
        /**
         * Channel name used in the task.
         * Available values: ["tamex", "tacquila", "mock"]
         */
        std::string channel = "tamex";
        /**
         * Paddle name used in the task.
         * Available values: ["neuland", "mock"]
         */
        std::string paddle = "neuland";
        /// Flag to enable the simulated cal level data output. @sa R3B::Neuland::SimCalData
        bool enable_sim_cal = false;
        /// Flag to enable the usage of cal_to_hit parameter. @sa R3B::Digitizing::Neuland::Tamex::Channel
        bool enable_hit_par = false;
        /// Flag to enable the checking of hit/signal sizes in histograms.
        bool enable_size_monitor = false;
        ParticleFilter::Options point_filter; //!< Point level filter options used in the digitizer.
        R3B::Digitizing::Neuland::Tamex::Params tamex_par{
            R3B::Digitizing::Neuland::Tamex::Channel::GetDefaultRandomGen()
        }; //!< Tamex parameters used in the Digitizer. This has no effect if enable_hit_par is true. @sa
           //!< R3B::Digitizing::Neuland::Tamex::Params.
        Digitizing::Neuland::Tamex::PeakPileUpStrategy pileup_strategy = Digitizing::Neuland::Tamex::
            PeakPileUpStrategy::width;                    //!< Pileup strategy for the tamex channel. @sa
                                                          //!< R3B::Digitizing::Neuland::Tamex::PeakPileUpStrategy
        std::string read = "NeulandPoints;NeulandHitPar"; //!< Input data names required by the digitizer
        std::string write = "NeulandHits;NeulandSimCal";  //!< Output data names from the digitizer

        ClassDefNV(DigiTaskOptions, 1); // NOLINT
    };

    /**
     * @class Digitizer
     * @brief Main digitizer task class for NeuLAND.
     *
     * The class utilizes the _Bridge Design Pattern_ such that users can freely combine different paddle
     * implementations with different channel implementations. The actual digitization process is implemented in its
     * engine class @ref R3B::Digitizing::Engine, which takes a paddle class generator together with a channel class
     * generator. Details about the digitization process of NeuLAND can be found in this @ref neuland_digitizing
     * "documentation".
     */
    class Digitizer : public FairTask
    {
      public:
        using NeulandPaddle = Digitizing::Neuland::Paddle;
        using TacquilaChannel = Digitizing::Neuland::TacQuila::Channel;
        using TamexChannel = Digitizing::Neuland::Tamex::Channel;
        template <typename Type>
        using UseChannel = Digitizing::UseChannel<Type>;
        template <typename Type>
        using UsePaddle = Digitizing::UsePaddle<Type>;

        /**
         * @brief Constructor with no input parameters.
         */
        Digitizer();

        /**
         * @brief Constructor with the given engine and input/output data branch names.
         *
         * @param engine Engine object
         * @param points_name The branch name of the input point level data.
         * @param hits_name The branch name of the output hit level data.
         * @param cal_hits_name The branch name of the output cal level data if has_cal_output_ is true.
         * @see EnableCalDataOutput
         */
        explicit Digitizer(std::unique_ptr<Digitizing::EngineInterface> engine,
                           std::string_view points_name = "NeulandPoints",
                           std::string_view hits_name = "NeulandHits",
                           std::string_view cal_hits_name = "NeulandSimCal");

        /**
         * @brief Generator of the digitizing class
         *
         * Create a task from the specifed options and run instance.
         * @param option The options used to configure the task
         * @param run The pointer to the current FairRun.
         * @return A unique pointer to the new task.
         */
        static auto Create(const R3B::Neuland::DigiTaskOptions& option, FairRun* run) -> std::unique_ptr<Digitizer>;

        /**
         * @brief Setter of the internal engine.
         *
         * Set the internal engine, which is derived from class Digitizing::EngineInterface.
         * @param engine Engine object
         */
        void SetEngine(std::unique_ptr<Digitizing::EngineInterface> engine);

        /**
         * @brief Add a new hit filter.
         *
         * @param filter New hit filter
         * @see Filterable
         */
        void AddFilter(const Filterable<R3BNeulandHit&>::Filter& filter) { hit_filters_.Add(filter); }

        /**
         * @brief Add a new cal level filter
         *
         * @param filter New cal level filter
         * @see Filterable
         */
        void AddFilterCal(const Filterable<R3B::Neuland::SimCalData&>::Filter& filter) { cal_hit_filter_.Add(filter); }

        /**
         * @brief Add a new point level filter
         *
         * Adding a new point level filter according to particle types and energies.
         * @param filter A new point level filter
         * @see R3B::Neuland::ParticleFilter
         */
        void SetPointFilter(R3B::Neuland::ParticleFilter filter) { neuland_point_filter_ = std::move(filter); }

        /**
         * @brief Enable sim cal data output
         *
         * Enable sim cal data output, which can be converted to experimental cal level data by the task
         * R3B::Neuland::SimCal2Cal.
         * @param is_enabled Flag to enable sim cal data output.
         * @see R3B::Neuland::SimCal2Cal
         */
        void EnableCalDataOutput(bool is_enabled) { has_cal_output_ = is_enabled; }

        /**
         * @brief Enable the checking on sizes of different data levels
         *
         * Enable the checking the sizes of signal/hit data from the paddle and channel classes. The sizes will be
         * filled into the histograms #hist_point_size_, #hist_channel_signal_size_, #hist_channel_hit_size_ and
         * #hist_paddle_hit_size_.
         * @param is_enbaled Flag to enable the checking
         */
        void EnableSizeMonitor(bool is_enabled = true) { has_size_monitor_ = is_enabled; }

        /**
         * @brief Getter of the sim cal output flag
         *
         * @return Flag whether the sim cal output is enabled
         * @see #EnableCalDataOutput()
         */
        [[nodiscard]] auto HasCalDataOutput() const -> bool { return has_cal_output_; }

      private:
        bool has_cal_output_ = false;
        bool has_size_monitor_ = false;

        R3B::InputVectorConnector<R3BNeulandPoint> neuland_points_{ "NeulandPoints" };
        R3B::OutputVectorConnector<R3BNeulandHit> neuland_hits_{ "NeulandHits" };
        R3B::OutputVectorConnector<R3B::Neuland::SimCalData> neuland_cal_hits_{ "NeulandSimCal" };

        std::unique_ptr<Digitizing::EngineInterface> digitizing_engine_; // owning

        R3B::Neuland::ParticleFilter neuland_point_filter_;
        Filterable<R3BNeulandHit&> hit_filters_;
        Filterable<R3B::Neuland::SimCalData&> cal_hit_filter_;

        R3BNeulandGeoPar* neuland_geo_par_ = nullptr; // non-owning
        std::unordered_map<int, int>
            point_size_tracker_; //<! Tracker to calculate point number per paddle with the paddle number as the key

        R3B::DataMonitor data_monitor_;
        TH1I* hist_multi_one_ = nullptr;
        TH1I* hist_multi_two_ = nullptr;
        TH1F* hist_rl_time_to_trig_ = nullptr;

        TH1D* hist_point_size_ = nullptr;
        TH1D* hist_channel_signal_size_ = nullptr;
        TH1D* hist_channel_hit_size_ = nullptr;
        TH1D* hist_paddle_hit_size_ = nullptr;

        void fill_points_to_engine();
        void fill_cal_data(const R3B::Digitizing::AbstractPaddle& paddle);
        void fill_hit_data(const R3B::Digitizing::AbstractPaddle& paddle);
        void fill_histograms();
        void fill_size_histograms(const Digitizing::AbstractPaddle& paddle);
        void init_histograms();

        auto Init() -> InitStatus override;
        void FinishTask() override;
        void FinishEvent() override;
        void SetParContainers() override;
        void Exec(Option_t* /*option*/) override;

      public:
        ClassDefOverride(Digitizer, 1) // NOLINT
    };

} // namespace R3B::Neuland

using R3BNeulandDigitizer = R3B::Neuland::Digitizer;
