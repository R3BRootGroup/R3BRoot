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
#include <cstddef>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace R3B::Neuland
{
    struct HitModulePar
    {
        int module_num = 0;                          //!< 1-based
        ValueError<double> t_diff;                   //!< ns
        ValueError<double> t_sync;                   //!< ns
        ValueError<double> effective_speed;          //!< cm/ns
        ValueError<double> light_attenuation_length; //!< cm, 1/alpha
        ValueError<double> light_attenuation_factor; //!< exp(alpha*L/2)
        LRPair<ValueError<double>> pedestal;         //!<
        LRPair<ValueError<double>> energy_gain;      //!<
        LRPair<ValueError<double>> pmt_saturation;   //!<
        LRPair<ValueError<double>> pmt_threshold;    //!<

        HitModulePar() = default;
        explicit HitModulePar(int module_number)
            : module_num{ module_number }
        {
        }

        ClassDefNV(HitModulePar, 2);
    };

    class Cal2HitPar : public ParSet
    {
      public:
        explicit Cal2HitPar(std::string_view name = "NeulandHitPar",
                            std::string_view title = "Neuland Cal2Hit calibration",
                            std::string_view context = "TestDefaultContext",
                            bool own = true);

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
        [[deprecated]] void SetNumOfModules(int num) {}
        void AddModulePar(const HitModulePar& module_par)
        {
            const auto mNum = module_par.module_num;
            module_pars_.insert_or_assign(mNum, module_par);
        }
        auto AddNewModulePar(int module_num) -> HitModulePar&
        {
            auto& module_par = module_pars_.try_emplace(module_num).first->second;
            module_par.module_num = module_num;
            return module_par;
        }
        void SetModulePars(std::unordered_map<int, ::R3B::Neuland::HitModulePar> module_pars)
        {
            module_pars_ = std::move(module_pars);
        }

        // getter:
        auto GetDistanceToTarget() const { return distance_to_target_; }
        auto GetEnergyCutoff() const { return energy_cut_; }
        auto GetGlobalTimeOffset() const { return global_time_offset_; }
        [[deprecated("Use GetDistancesToFirstPlane instead")]] auto GetDistanceToFirstPlane(int plane_num) const
        {
            return distances_to_first_plane_.at(plane_num);
        }
        auto GetDistancesToFirstPlane() const -> const auto& { return distances_to_first_plane_; }
        auto GetModuleParAt(int module_num) const -> const ::R3B::Neuland::HitModulePar&;
        auto HasModuleParAt(int module_num) const -> bool
        {
#ifdef HAS_CPP_STANDARD_17
            return module_pars_.find(module_num) != module_pars_.end();
#else
            return module_pars_.contains(module_num);
#endif
        }
        auto GetModulePars() const -> const std::unordered_map<int, ::R3B::Neuland::HitModulePar>&
        {
            return module_pars_;
        }
        // no auto because pybind from pyROOT
        auto GetListOfModulePar() const -> const std::unordered_map<int, ::R3B::Neuland::HitModulePar>&
        {
            return module_pars_;
        }
        auto GetListOfModuleParRef() -> auto& { return module_pars_; }
        auto GetNumOfModules() const -> std::size_t { return module_pars_.size(); }

      private:
        int num_of_modules = 0;
        double global_time_offset_ = 0.; // in ns
        double distance_to_target_ = 0.; // in cm
        double energy_cut_ = 0.;         // in MeV
        std::vector<double> distances_to_first_plane_;
        std::unordered_map<int, ::R3B::Neuland::HitModulePar> module_pars_;
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
