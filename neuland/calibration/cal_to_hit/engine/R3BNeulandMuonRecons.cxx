/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BNeulandMuonRecons.h"

#include <R3BException.h>
#include <R3BLogger.h>
#include <R3BNeulandCalData2.h>
#include <R3BNeulandCalToHitPar.h>
#include <R3BNeulandCommon.h>
#include <R3BNeulandHitModulePar.h>
#include <R3BNeulandHitPar.h>
#include <R3BNeulandParDirCreator.h>
#include <R3BShared.h>
#include <algorithm>
#include <cmath>
#include <fmt/format.h>
#include <vector>

namespace R3B::Neuland::Calibration
{
    namespace
    {
        void fill_module_parameters(const std::vector<R3BNeulandHitModulePar>& old_module_pars,
                                    R3B::Neuland::Cal2HitPar& new_module_pars)
        {
#ifdef HAS_CPP_STANDARD_23
            auto t_sync_ref_par_iter = std::ranges::find(
                old_module_pars, DEFAULT_TSYNC_REFERENCE_BAR_NUM, &R3BNeulandHitModulePar::GetModuleId);
#else
            auto t_sync_ref_par_iter = std::find_if(old_module_pars.begin(),
                                                    old_module_pars.end(),
                                                    [](const R3BNeulandHitModulePar& par)
                                                    { return par.GetModuleId() == DEFAULT_TSYNC_REFERENCE_BAR_NUM; });
#endif
            if (t_sync_ref_par_iter == old_module_pars.end())
            {
                throw R3B::logic_error(
                    fmt::format("Reference module  for tsync parameter with the module number {} doesn't exist!",
                                DEFAULT_TSYNC_REFERENCE_BAR_NUM));
            }
            const auto t_sync_ref_value = t_sync_ref_par_iter->GetTSync();

            for (const auto& par : old_module_pars)
            {
                auto new_par = R3B::Neuland::HitModulePar{};
                // GetModuleId return 1 based module Num
                new_par.module_num = par.GetModuleId();
                new_par.t_diff.value = par.GetTDiff();
                new_par.t_sync.value = par.GetTSync() - t_sync_ref_value;
                new_par.effective_speed.value = -2 * par.GetEffectiveSpeed();
                new_par.light_attenuation_length.value = par.GetLightAttenuationLength();
                new_par.light_attenuation_factor.value =
                    std::exp(R3B::Neuland::TotalBarLength / new_par.light_attenuation_length.value / 2);

                // Getters accept 1 as the left side and 2 as the right side

                const auto left_index = 1 + toIndex(R3B::Side::left);
                const auto right_index = 1 + toIndex(R3B::Side::right);

                new_par.pedestal.left().value = par.GetPedestal(left_index);
                new_par.pedestal.right().value = par.GetPedestal(right_index);

                new_par.energy_gain.left().value = par.GetEnergyGain(left_index);
                new_par.energy_gain.right().value = par.GetEnergyGain(right_index);

                new_par.pmt_saturation.left().value = par.GetPMTSaturation(left_index);
                new_par.pmt_saturation.right().value = par.GetPMTSaturation(right_index);

                new_par.pmt_threshold.left().value = par.GetPMTThreshold(left_index);
                new_par.pmt_threshold.right().value = par.GetPMTThreshold(right_index);
                new_module_pars.AddModulePar(new_par);
            }
        }
    } // namespace

    void MuonReconstruction::Init()
    {
        const auto module_size = GetModuleSize();
        if (module_size == 0)
        {
            throw R3B::runtime_error("module size is 0 during the initialization!");
        }

        // To set the number of planes in legacy hitCalibrationEngine
        auto hit_par_temp = R3BNeulandHitPar{};
        hit_par_temp.SetNumberOfPlanes(static_cast<int>(module_size) / BarsPerPlane);
        hit_cal_engine_.Init(&hit_par_temp);
    }

    void MuonReconstruction::AddSignals(const std::vector<BarCalData>& signals)
    {
        for (const auto& signal : signals)
        {
            add_bar_signal(signal, Side::left);
            add_bar_signal(signal, Side::right);
        }
    }

    void MuonReconstruction::add_bar_signal(const BarCalData& barSignal, Side side)
    {
        // change to 0 based indexing
        const auto module_id = static_cast<int>(barSignal.module_num - 1);

        const auto& calData = (side == Side::left) ? barSignal.left : barSignal.right;

        for (const auto& calSignal : calData)
        {
            // 1 is the right side and 0 is the left side
            hit_cal_engine_.Set(module_id,
                                static_cast<int>(toIndex(side)),
                                calSignal.leading_time.value - calSignal.trigger_time.value,
                                static_cast<int>(calSignal.time_over_threshold.value));
            if (hit_cal_engine_.IsValid(module_id))
            {
                cosmic_tracker_.AddPoint(module_id, hit_cal_engine_.GetPosition(module_id));
            }
        }
    }

    void MuonReconstruction::EndOfEvent(unsigned int event_num)
    {
        const auto added_points_num = cosmic_tracker_.GetBarIDs().size();
        const auto& track = cosmic_tracker_.GetTrack();
        if (track.Interactions.empty() or added_points_num < 6)
        {
            R3BLOG(debug, "Failed to get the cosmic track!");
        }
        else
        {
            hit_cal_engine_.Add(track, event_num);
        }
    }

    void MuonReconstruction::Calibrate(Cal2HitPar& hit_par)
    {
        hit_par.Reset();
        // auto* new_dir = ParDirCreator{}.mkdir(DEFAULT_HIST_MONITOR_DIR);
        auto* new_dir = ParDirCreator{}.mkdir("Histograms");
        fill_module_parameters(hit_cal_engine_.Calibrate(new_dir), hit_par);
    }

} // namespace R3B::Neuland::Calibration
