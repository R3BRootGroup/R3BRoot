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

#include "R3BNeulandSignalMatcher.h"

namespace R3B::Neuland
{

    auto SignalMatcher::GetGoodnessOfMatch(const Input& firstSignal, const Input& secondSignal, const Par& par) -> float
    {
        auto firstE = static_cast<float>(firstSignal.energy);
        auto secondE = static_cast<float>(secondSignal.energy);
        auto firstT = firstSignal.time;
        auto secondT = secondSignal.time;

        // keep the exponent always negative to prevent the exploding of exponential function
        auto res = 0.F;
        if (firstT > secondT)
        {
            // res = std::abs(
            //     (firstE / secondE) * std::exp(static_cast<float>(par.attenuation * par.c_medium * (firstT -
            //     secondT))) - 1);
            res = std::abs((firstE / secondE) *
                           std::exp(static_cast<float>(par.attenuation * par.c_medium * (firstT - secondT))));
        }
        else
        {
            // res = std::abs((secondE / firstE) * std::exp(static_cast<float>(par.attenuation * par.c_medium *
            //                                                                static_cast<float>(secondT - firstT))) -
            //                1);
            res = std::abs((secondE / firstE) * std::exp(static_cast<float>(par.attenuation * par.c_medium *
                                                                            static_cast<float>(secondT - firstT))));
        }
        return res;
    }

} // namespace R3B::Neuland
