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
#include "R3BValueError.h"
#include <R3BMinMaxValue.h>
#include <TObject.h>
#include <cassert>
#include <cmath>
#include <limits>

namespace R3B::Neuland
{
    // Constants

    constexpr auto SQRT_12 = 3.464101615;
    constexpr auto NaN = std::numeric_limits<double>::quiet_NaN();
    constexpr auto Inf = std::numeric_limits<double>::infinity();

    // Useful functions I do not want to redefine in every file

    template <typename T>
    constexpr auto Sqr(const T val) -> T
    {
        return val * val;
    }

    // Millepede calibration defaults:
    constexpr auto DEFAULT_EFFECTIVE_C = 8.;                // cm/ns
    constexpr auto DEFAULT_CALIBRATION_P_VALUE_CUT = 1e-10; // any smaller values will be discarded
    constexpr auto DEFAULT_T_DIFF_RESIDUAL_CUT = 400;       // any larger values will be discarded

    // Initialize variables from Birk' s Law

    constexpr double BirkdP = 1.032;
    constexpr double BirkC1 = 0.013 / BirkdP;
    constexpr double BirkC2 = 9.6e-6 / (BirkdP * BirkdP);

    constexpr auto GetLightYieldAfterBirk(const int charge, const double length_mm, const double edep_MeV) -> double
    {
        return (charge * length_mm == 0.
                    ? edep_MeV
                    : edep_MeV / (1. + BirkC1 * (Sqr(charge) > 1 ? 7.2 / 12.6 : 1.) * (edep_MeV / length_mm) +
                                  BirkC2 * Sqr(edep_MeV / length_mm)));
    }

    // Physical Constants

    constexpr auto CLight = 29.9792458;     // Speed of light [cm/ns]
    constexpr auto InvCLight = 1. / CLight; // Speed of light [cm/ns]>
    constexpr auto MUON_MASS = 0.105;       // Muonmass [GeV]

    // Electronics Constants

    constexpr auto COARSE_TIME_CLOCK_FREQUENCY_MHZ = 200.F;
    constexpr auto MaxCalTime = 5. * 2048;
    constexpr auto MaxFTValue = 4097;
    constexpr auto MAXCTValue = 2048U;
    // Geometry & Material Constants

    constexpr auto BarSize_XY = 5.0;                                  // cm NeuLAND parameter
    constexpr auto BarUncertainty_XY = BarSize_XY / SQRT_12;          // cm NeuLAND parameter
    constexpr auto BarSize_Z = 5.0;                                   // cm NeuLAND parameter
    constexpr auto BarUncertainty_Z = BarSize_Z / SQRT_12;            // cm NeuLAND parameter
    constexpr auto BarLength = 250.0;                                 // cm NeuLAND parameter
    constexpr auto LightGuideLength = 10.0;                           // cm NeuLAND parameter
    constexpr auto TotalBarLength = BarLength + 2 * LightGuideLength; // cm NeuLAND parameter, Bar including Light Guide

    constexpr auto ScintillatorDensity = 1.032;        // g / cm^3
    constexpr auto MIPStoppingPowerPerDensity = 1.956; // MeV cm^2 / g
    constexpr auto MIPStoppingPower = 1.73;            // MeV / cm

    constexpr auto FirstHorizontalPlane = 0;
    constexpr auto BarsPerPlane = 50;
    constexpr auto MaxNumberOfPlanes = 26;
    constexpr auto MaxNumberOfBars = MaxNumberOfPlanes * BarsPerPlane;

    // naming convention:
    // _num starts at 1 and _id starts at 0
    // module number has the range of 1 ~ BarsPerPlane * NumOfPlanes
    // bar number has the range of 1 ~ BarsPerPlane
    constexpr auto GetBarVerticalDisplacement(int module_num) -> double
    {
        const auto bar_num = module_num % BarsPerPlane;
        return (2 * bar_num - 1 - BarsPerPlane) / 2. * BarSize_XY;
    }
    constexpr auto IsPlaneIDHorizontal(int plane_id) -> bool { return (plane_id % 2 == FirstHorizontalPlane); }
    constexpr auto IsPlaneIDVertical(int plane_id) -> bool { return !IsPlaneIDHorizontal(plane_id); }
    constexpr auto ModuleID2PlaneID(int moduleID) -> int { return moduleID / BarsPerPlane; }
    constexpr auto ModuleID2PlaneNum(int moduleID) -> int { return ModuleID2PlaneID(moduleID) + 1; }
    // planeNum, barNum and ModuleNum is 1-based
    constexpr auto Neuland_PlaneBar2ModuleNum(unsigned int planeNum, unsigned int barNum) -> unsigned int
    {
        assert(planeNum > 0);
        return (planeNum - 1) * BarsPerPlane + barNum;
    }
    template <typename T = double>
    constexpr auto PlaneID2ZPos(int plane_id) -> T
    {
        return static_cast<T>((plane_id + 0.5) * BarSize_Z);
    }
    template <typename T = double>
    constexpr auto ModuleNum2ZPos(int module_num) -> T
    {
        return PlaneID2ZPos<T>(ModuleID2PlaneID(module_num - 1));
    }

    // Average Parameters

    constexpr auto AvgTimeResolution = 0.150;   // ns
    constexpr auto AvgEffectiveCLight = -7.95;  // cm / ns
    constexpr auto AvgGain = 15;                // MeV / ns
    constexpr auto AvgThreshold = 1.75;         // MeV
    constexpr auto AvgAttenuationLength = 400.; // cm

    constexpr auto SaturationCoefficient = 1.75e-3; // 1 / ns

    // NeuLAND TPAT:
    constexpr auto NeulandOnSpillTpatPos = 0U; // 0 based

    // Default values:
    constexpr auto DEFAULT_BOX_GENERATOR_THETA = MinMaxValueD{ 0., 3. }; // degree
    constexpr auto DEFAULT_BOX_GENERATOR_PHI = MinMaxValueD{ 0., 360. }; // degree
    constexpr auto DEFAULT_GENERATOR_ENERGY = ValueErrorD{ 1., 0.2 };    // [GeV]
} // namespace R3B::Neuland
