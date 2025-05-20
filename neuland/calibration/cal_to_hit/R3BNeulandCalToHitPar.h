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

#pragma once

#include "R3BShared.h"
#include "R3BValueError.h"
#include <R3BNeulandParSet.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace R3B::Neuland
{
    struct HitModulePar
    {
        unsigned int module_num = 0;                 // 1-based
        ValueError<double> t_diff;                   // ns
        ValueError<double> t_sync;                   // ns
        ValueError<double> effective_speed;          // cm/ns
        ValueError<double> light_attenuation_length; // cm, 1/alpha
        ValueError<double> light_attenuation_factor; // exp(alpha*L/2)
        LRPair<ValueError<double>> pedestal;         //
        LRPair<ValueError<double>> energy_gain;      //
        LRPair<ValueError<double>> pmt_saturation;   //
        LRPair<ValueError<double>> pmt_threshold;    //
        ClassDefNV(HitModulePar, 2);
    };

    class Cal2HitPar : public ParSet
    {
      public:
        explicit Cal2HitPar(std::string_view name = "NeulandHitPar",
                            std::string_view title = "Neuland Cal2Hit calibration",
                            std::string_view context = "TestDefaultContext",
                            Bool_t own = kTRUE);

        Cal2HitPar(const Cal2HitPar&) = default;
        Cal2HitPar(Cal2HitPar&&) = default;
        auto operator=(const Cal2HitPar&) -> Cal2HitPar& = default;
        auto operator=(Cal2HitPar&&) -> Cal2HitPar& = default;
        ~Cal2HitPar() override = default;

        void CalculateEnergyCutoff();
        void Reset() { clear(); }

        // setters:
        void SetDistanceToTarget(double distance) { distance_to_target_ = distance; }
        void SetEnergyCutoff(double cutoff) { energy_cut_ = cutoff; }
        void SetGlobalTimeOffset(double offset) { global_time_offset_ = offset; }
        void SetNumOfModules(int num) { num_of_modules = num; }
        void AddModulePar(const HitModulePar& module_par)
        {
            const auto mNum = module_par.module_num;
            module_pars_.insert_or_assign(mNum, module_par);
        }

        // getter:
        auto GetDistanceToTarget() const { return distance_to_target_; }
        auto GetEnergyCutoff() const { return energy_cut_; }
        auto GetGlobalTimeOffset() const { return global_time_offset_; }
        [[deprecated("Use GetDistancesToFirstPlane instead")]] auto GetDistanceToFirstPlane(
            unsigned int plane_num) const
        {
            return distances_to_first_plane_.at(plane_num);
        }
        auto GetDistancesToFirstPlane() const -> const auto& { return distances_to_first_plane_; }
        auto GetNumModulePar() const { return module_pars_.size(); }
        auto GetModuleParAt(unsigned int module_num) const -> const ::R3B::Neuland::HitModulePar&
        {
            return module_pars_.at(module_num);
        }
        auto HasModuleParAt(int module_num) const -> bool
        {
            return module_pars_.find(module_num) != module_pars_.end();
        }
        auto GetModulePars() const -> const std::unordered_map<unsigned int, ::R3B::Neuland::HitModulePar>&
        {
            return module_pars_;
        }
        // no auto because pybind from pyROOT
        auto GetListOfModulePar() const -> const std::unordered_map<unsigned int, ::R3B::Neuland::HitModulePar>&
        {
            return module_pars_;
        }
        auto GetListOfModuleParRef() -> auto& { return module_pars_; }
        auto GetNumOfModules() const -> int { return num_of_modules; }

      private:
        int num_of_modules = 0;
        double global_time_offset_ = 0.; // in ns
        double distance_to_target_ = 0.; // in cm
        double energy_cut_ = 0.;         // in MeV
        std::vector<double> distances_to_first_plane_;
        std::unordered_map<unsigned int, ::R3B::Neuland::HitModulePar> module_pars_;
        void clear() override
        {
            global_time_offset_ = 0.;
            distance_to_target_ = 0.;
            energy_cut_ = 0.;
            distances_to_first_plane_.clear();
            module_pars_.clear();
        }

      public:
        ClassDefOverride(Cal2HitPar, 2);
    };

} // namespace R3B::Neuland

using R3BNeulandHitPar2 = R3B::Neuland::Cal2HitPar;
