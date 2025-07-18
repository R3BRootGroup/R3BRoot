#pragma once

#include "R3BNeulandMillepede.h"
#include <nlohmann/json_fwd.hpp>

namespace R3B::Neuland::Calibration
{
    inline void to_json(nlohmann::ordered_json& json_obj, const MillepedeOptions& options)
    {
        json_obj = nlohmann::ordered_json{
            { "outdir-has-timestamp", options.outdir_has_timestamp },
            { "num-of-threads", options.num_of_threads },
            { "scale-factor", options.scale_factor },
            { "t-diff-residual-cut", options.t_diff_residual_cut },
            { "p-value-cut", options.p_value_cut },
            { "mille-par-filename", options.mille_par_filename },
            { "pede-par-filename", options.pede_par_filename },
            { "mille-log-filename", options.mille_log_filename },
        };
    }

    inline void from_json(const nlohmann::ordered_json& json_obj, MillepedeOptions& options)
    {
        json_obj.at("outdir-has-timestamp").get_to(options.outdir_has_timestamp);
        json_obj.at("num-of-threads").get_to(options.num_of_threads);
        json_obj.at("scale-factor").get_to(options.scale_factor);
        json_obj.at("t-diff-residual-cut").get_to(options.t_diff_residual_cut);
        json_obj.at("p-value-cut").get_to(options.p_value_cut);
        json_obj.at("mille-par-filename").get_to(options.mille_par_filename);
        json_obj.at("pede-par-filename").get_to(options.pede_par_filename);
        json_obj.at("mille-log-filename").get_to(options.mille_log_filename);
    }
} // namespace R3B::Neuland::Calibration
