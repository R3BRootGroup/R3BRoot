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

#include "R3BNeulandDigitizer.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "NeulandParticleFilter.h"
#include "NeulandSimCalData.h"
#include "R3BDataMonitor.h"
#include "R3BDigitizingChannelMock.h"
#include "R3BDigitizingEngine.h"
#include "R3BDigitizingPaddle.h"
#include "R3BDigitizingPaddleMock.h"
#include "R3BDigitizingPaddleNeuland.h"
#include "R3BDigitizingTacQuila.h"
#include "R3BDigitizingTamex.h"
#include "R3BException.h"
#include "R3BNeulandCalToHitPar.h"
#include "R3BNeulandCommon.h"
#include "R3BNeulandGeoPar.h"
#include "R3BNeulandHit.h"
#include <FairTask.h>
#include <R3BNeulandCommonFunc.h>
#include <R3BShared.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <TFile.h>
#include <TH1.h>
#include <TVector3.h>
#include <algorithm>
#include <array>
#include <fairlogger/Logger.h>
#include <fmt/core.h>
#include <functional>
#include <map>
#include <memory>
#include <range/v3/view.hpp>
#include <range/v3/view/zip.hpp>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

constexpr auto MAX_SIZE_BIN = 20;

namespace R3B::Neuland
{
    namespace
    {
        inline auto CreateDigiEngine(const DigiTaskOptions& option, std::string_view hit_par_name, FairRun* run)
        {
            namespace Digitizing = R3B::Digitizing;
            using NeulandPaddle = Digitizing::Neuland::Paddle;
            namespace Tamex = Digitizing::Neuland::Tamex;
            using TamexChannel = Tamex::Channel;
            using Digitizing::UseChannel;
            using Digitizing::UsePaddle;
            using MockPaddle = Digitizing::Neuland::MockPaddle;
            using MockChannel = Digitizing::Neuland::MockChannel;
            using TacquilaChannel = Digitizing::Neuland::TacQuila::Channel;

            auto pileup_strategy = option.pileup_strategy;
            const auto& tamex_par = option.tamex_par;
            Cal2HitPar* cal_to_hit_par{ nullptr };
            if (option.enable_hit_par)
            {
                LOGP(info, "cal_to_hit_par is used in digitization task!");
                cal_to_hit_par = std::make_unique<Cal2HitPar>(hit_par_name).release();
                run->GetRuntimeDb()->addContainer(cal_to_hit_par);
            }
            else
            {
                LOGP(info, "cal_to_hit_par is not used in digitization task!");
            }
            return std::map<std::pair<const std::string, const std::string>,
                            std::function<std::unique_ptr<Digitizing::EngineInterface>()>>{
                { { "neuland", "tamex" },
                  [&tamex_par, pileup_strategy, cal_to_hit_par, enable_sim_cal = option.enable_sim_cal]()
                  {
                      return Digitizing::CreateEngine(
                          UsePaddle<NeulandPaddle>(cal_to_hit_par),
                          UseChannel<TamexChannel>(pileup_strategy, tamex_par, cal_to_hit_par, enable_sim_cal));
                  } },
                { { "neuland", "tacquila" },
                  [cal_to_hit_par]() {
                      return Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(cal_to_hit_par),
                                                      UseChannel<TacquilaChannel>());
                  } },
                { { "mock", "tamex" },
                  [&tamex_par, pileup_strategy, cal_to_hit_par, enable_sim_cal = option.enable_sim_cal]()
                  {
                      return Digitizing::CreateEngine(
                          UsePaddle<MockPaddle>(),
                          UseChannel<TamexChannel>(pileup_strategy, tamex_par, cal_to_hit_par, enable_sim_cal));
                  } },
                { { "neuland", "mock" },
                  [cal_to_hit_par]() {
                      return Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(cal_to_hit_par),
                                                      UseChannel<MockChannel>());
                  } },
                { { "mock", "mock" },
                  []() { return Digitizing::CreateEngine(UsePaddle<MockPaddle>(), UseChannel<MockChannel>()); } }
            };
        }

    } // namespace

    Digitizer::Digitizer()
        : Digitizer(Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(), UseChannel<TacquilaChannel>()))
    {
    }

    Digitizer::Digitizer(std::unique_ptr<Digitizing::EngineInterface> engine,
                         std::string_view points_name,
                         std::string_view hits_name,
                         std::string_view cal_hits_name)
        : FairTask("R3BNeulandDigitizer")
        , neuland_points_{ points_name }
        , neuland_hits_{ hits_name }
        , neuland_cal_hits_{ cal_hits_name }
        , digitizing_engine_(std::move(engine))
    {
    }

    void Digitizer::SetEngine(std::unique_ptr<Digitizing::EngineInterface> engine)
    {
        digitizing_engine_ = std::move(engine);
    }

    void Digitizer::SetParContainers()
    {
        FairRunAna* run = FairRunAna::Instance();
        if (run == nullptr)
        {
            throw R3B::runtime_error("R3BNeulandDigitizer::SetParContainers: No analysis run");
        }

        FairRuntimeDb* rtdb = run->GetRuntimeDb();
        if (rtdb == nullptr)
        {
            throw R3B::runtime_error("R3BNeulandDigitizer::SetParContainers: No runtime database");
        }

        neuland_geo_par_ = dynamic_cast<R3BNeulandGeoPar*>(rtdb->getContainer("R3BNeulandGeoPar"));
        if (neuland_geo_par_ == nullptr)
        {
            throw R3B::runtime_error("R3BNeulandDigitizer::SetParContainers: No R3BNeulandGeoPar");
        }
    }

    auto Digitizer::Init() -> InitStatus
    {
        if (neuland_geo_par_ == nullptr)
        {
            throw R3B::runtime_error("neuland_geo_par is still nullptr during the initialization!");
        }
        digitizing_engine_->Init(BarsPerPlane);
        neuland_points_.init();
        neuland_hits_.init();
        if (has_cal_output_)
        {
            neuland_cal_hits_.init();
        }
        init_histograms();

        return kSUCCESS;
    }

    void Digitizer::init_histograms()
    {
        // Initialize control histograms
        auto const paddle_size = neuland_geo_par_->GetNumberOfModules();

        hist_multi_one_ = data_monitor_.add_hist<TH1I>(
            "MultiplicityOne", "Paddle multiplicity: only one PMT per paddle", paddle_size, 0, paddle_size);

        hist_multi_two_ = data_monitor_.add_hist<TH1I>(
            "MultiplicityTwo", "Paddle multiplicity: both PMTs of a paddle", paddle_size, 0, paddle_size);
        auto const timeBinSize = 200;
        hist_rl_time_to_trig_ =
            data_monitor_.add_hist<TH1F>("hRLTimeToTrig", "R/Ltime-triggerTime", timeBinSize, -100., 100.);
        if (has_size_monitor_)
        {
            hist_paddle_hit_size_ = data_monitor_.add_hist<TH1D>(
                "paddle_hit_size", "Size of paddle hit per paddle", MAX_SIZE_BIN, 0.5, MAX_SIZE_BIN + 0.5);
            hist_channel_signal_size_ = data_monitor_.add_hist<TH1D>(
                "channel_signal_size", "Size of channel signals per channel", MAX_SIZE_BIN, 0.5, MAX_SIZE_BIN + 0.5);
            hist_channel_hit_size_ = data_monitor_.add_hist<TH1D>(
                "channel_hit_size", "Size of channel hits per channel", MAX_SIZE_BIN, 0.5, MAX_SIZE_BIN + 0.5);

            hist_point_size_ = data_monitor_.add_hist<TH1D>(
                "filtered_point_size", "Size of points per paddle", MAX_SIZE_BIN, 0.5, MAX_SIZE_BIN + 0.5);

            // init point size checker
            for (int paddle_id{}; paddle_id < paddle_size; ++paddle_id)
            {
                point_size_tracker_.emplace(paddle_id + 1, 0);
            }
        }
    }

    void Digitizer::Exec(Option_t* /*option*/)
    {
        neuland_hits_.clear();
        if (has_cal_output_)
        {
            neuland_cal_hits_.clear();
        }
        digitizing_engine_->Reset();

        fill_points_to_engine();

        digitizing_engine_->Construct();

        fill_histograms();

        auto paddle_action = [this](const Digitizing::AbstractPaddle& paddle)
        {
            if (!paddle.HasFired())
            {
                return;
            }

            if (has_size_monitor_)
            {
                fill_size_histograms(paddle);
            }

            if (has_cal_output_)
            {
                fill_cal_data(paddle);
            }
            fill_hit_data(paddle);
        };

        digitizing_engine_->DoEachPaddle(paddle_action);

        LOG(debug) << fmt::format("Produced {} hits", neuland_hits_.size());
    }

    void Digitizer::fill_size_histograms(const Digitizing::AbstractPaddle& paddle)
    {

        using R3B::Side;

        const auto& paddle_hits = paddle.GetHits();
        hist_paddle_hit_size_->Fill(static_cast<double>(paddle_hits.size()));

        auto sides = std::array<Side, 2>{ Side::left, Side::right };
        for (const auto side : sides)
        {
            const auto& channel = paddle.GetChannel(side);
            hist_channel_hit_size_->Fill(static_cast<double>(channel.GetHits().size()));
            hist_channel_signal_size_->Fill(static_cast<double>(channel.GetSignalSize()));
        }
    }

    void Digitizer::fill_points_to_engine()
    {
        static constexpr auto GeVToMeVFac = 1000.;

        // Look at each Land Point, if it deposited energy in the scintillator, store it with reference to the bar
        for (const auto& point : neuland_points_)
        {
            if ((neuland_point_filter_.GetMode() != ParticleFilter::Mode::pass) and
                (not neuland_point_filter_.IsPointAllowed(point)))
            {
                continue;
            }
            if (point.GetEnergyLoss() > 0.)
            {
                const auto paddle_ID = point.GetPaddle();

                // Convert position of point to paddle-coordinates, including any rotation or translation
                const auto& position = point.GetPosition();
                const auto converted_position = neuland_geo_par_->ConvertToLocalCoordinates(position, paddle_ID);
                LOGP(debug2, "Point in paddle: {}, with global position XYZ: {}", paddle_ID, position);
                LOGP(debug2, "Converted to local position XYZ: {}", converted_position);

                // Within the paddle frame, the relevant distance of the light from the pmt is always given by the
                // X-Coordinate
                const auto dist = converted_position.X();
                digitizing_engine_->DepositLight(paddle_ID, point.GetTime(), point.GetLightYield() * GeVToMeVFac, dist);
                if (has_size_monitor_)
                {
                    ++(point_size_tracker_.at(paddle_ID));
                }
            } // eloss
        } // points
    }

    void Digitizer::fill_histograms()
    {
        // Fill control histograms
        const auto triggerTime = digitizing_engine_->GetTriggerTime();

        hist_rl_time_to_trig_->Fill(triggerTime);
        hist_multi_one_->Fill(digitizing_engine_->DoAllPaddles(
            [](auto paddles_view)
            {
                return static_cast<double>(std::count_if(paddles_view.begin(),
                                                         paddles_view.end(),
                                                         [](const auto& paddle) { return paddle.HasHalfFired(); }));
            }));

        hist_multi_two_->Fill(digitizing_engine_->DoAllPaddles(
            [](auto paddles_view)
            {
                return static_cast<double>(std::count_if(
                    paddles_view.begin(), paddles_view.end(), [](const auto& paddle) { return paddle.HasFired(); }));
            }));
        if (has_size_monitor_)
        {
            for (const auto [key, value] : point_size_tracker_)
            {
                if (value == 0)
                {
                    continue;
                }
                hist_point_size_->Fill(value);
            }
        }
    }

    auto Digitizer::Create(const DigiTaskOptions& option, FairRun* run) -> std::unique_ptr<Digitizer>
    {
        auto read_branch_names = std::vector<std::string>{};
        auto write_branch_names = std::vector<std::string>{};

        parse_io_branch_names(option, read_branch_names, 2, write_branch_names, 2); // NOLINT
        auto engine_map = CreateDigiEngine(option, read_branch_names.at(1), run);
        auto engine_gen = engine_map.at({ option.paddle, option.channel });
        auto task = std::make_unique<Digitizer>(
            engine_gen(), read_branch_names.at(0), write_branch_names.at(0), write_branch_names.at(1));
        task->EnableCalDataOutput(option.enable_sim_cal);
        task->EnableSizeMonitor(option.enable_size_monitor);
        task->SetName(option.name.c_str());
        auto point_filter = ParticleFilter::Create(option.point_filter);
        LOG(info) << point_filter.Print();
        task->SetPointFilter(std::move(point_filter));
        return task;
    }

    void Digitizer::fill_hit_data(const R3B::Digitizing::AbstractPaddle& paddle)
    {
        const auto& signals = paddle.GetHits();

        for (const auto& signal : signals)
        {
            const TVector3 hitPositionLocal = TVector3(signal.position, 0., 0.);
            const TVector3 hitPositionGlobal =
                neuland_geo_par_->ConvertToGlobalCoordinates(hitPositionLocal, paddle.GetPaddleID());
            const TVector3 hitPixel = neuland_geo_par_->ConvertGlobalToPixel(hitPositionGlobal);

            auto hit = R3BNeulandHit{ paddle.GetPaddleID(),
                                      signal.left_channel_hit->tdc,
                                      signal.right_channel_hit->tdc,
                                      signal.time,
                                      signal.left_channel_hit->qdcUnSat,
                                      signal.right_channel_hit->qdcUnSat,
                                      signal.energy,
                                      hitPositionGlobal,
                                      hitPixel };

            if (hit_filters_.IsValid(hit))
            {
                neuland_hits_.get().emplace_back(std::move(hit));
                LOGP(debug, "Adding neuland hit: {}", hit);
            }
        } // loop over all hits for each paddle
    }

    void Digitizer::fill_cal_data(const R3B::Digitizing::AbstractPaddle& paddle)
    {
        auto& cal_hits = neuland_cal_hits_.get();

        const auto paddle_ID = paddle.GetPaddleID();
        const auto& left_channel = paddle.GetLeftChannelRef();
        const auto& right_channel = paddle.GetRightChannelRef();

        const auto& left_channel_signals = left_channel.GetCalSignals();
        const auto& right_channel_signals = right_channel.GetCalSignals();

        if (left_channel_signals.size() != right_channel_signals.size())
        {
            return;
        }

        for (const auto& [left, right] : ranges::zip_view(left_channel_signals, right_channel_signals))
        {

            auto cal_data = SimCalData{ paddle_ID, left.tot, right.tot, left.tle, right.tle };

            if (cal_hit_filter_.IsValid(cal_data))
            {
                cal_hits.push_back(std::move(cal_data));
                LOGP(debug2, "Adding sim cal with {}", cal_data);
            }
        } // loop over signals
    }

    void Digitizer::FinishTask() { data_monitor_.save_to_sink(); }

    void Digitizer::FinishEvent()
    {
        if (has_size_monitor_)
        {
            for (auto& [key, value] : point_size_tracker_)
            {
                value = 0;
            }
        }
    }

} // namespace R3B::Neuland
