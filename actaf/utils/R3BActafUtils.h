/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -------------------------------------------------------------
// -----               R3BActafUtils                       -----
// -----     Created 25/02/26 by J.L. Rodriguez-Sanchez    -----
// -------------------------------------------------------------

#pragma once

#include "R3BActafMappedData.h"

#include <Rtypes.h>
#include <TMath.h>
#include <array>
#include <cmath>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

namespace R3BActafUtils
{
    template <class Cont>
    int FindMaxPosition(const Cont& signal)
    {
        return std::distance(signal.begin(), std::max_element(signal.begin(), signal.end()));
    }

    template <class Cont>
    double IntegratePulse(const Cont& signal, int maxIdx, double baseline = 0.)
    {
        int left = maxIdx, right = maxIdx;
        const int size = static_cast<int>(signal.size());

        while (left > 0 && signal[left] > baseline)
            --left;

        while (right < size - 1 && signal[right] > baseline)
            ++right;

        return std::accumulate(signal.begin() + left, signal.begin() + right, 0.0) - (right - left) * baseline;
    }

    template <class Cont>
    double ComputeBaselineMean(const Cont& signal, int numBins, bool returnMean = 1)
    {
        numBins = std::min(static_cast<int>(numBins * 0.5), static_cast<int>(signal.size()));
        if (numBins <= 0)
            return 0.0;

        double mean = 0;

        mean = std::accumulate(signal.begin(), signal.begin() + numBins, 0.0) / numBins;

        if (returnMean)
            return mean;

        else
        {
            double variance = 0.0;
            for (int i = 0; i < numBins; ++i)
            {
                double diff = signal[i] - mean;
                variance += diff * diff;
            }
            variance /= numBins;

            double stddev = std::sqrt(variance);
            return stddev;
        }
    }

    template <class Cont>
    int FindMaxAmplitude(const Cont& signal)
    {
        return *std::max_element(signal.begin(), signal.end());
    }

    template <class Cont>
    double CalculateMAW(const Cont& signal, int averaging = 4, int peakTimeNs = 200, int gapTimeNs = 200)
    {
        const int binsPeakingTime = peakTimeNs / averaging;
        const int binsGapTime = gapTimeNs / averaging;

        const int nBins = static_cast<int>(signal.size());

        if (nBins < binsPeakingTime + binsGapTime)
            return 0;

        double maxDiff = -1.0;
        // int binMaxDiff = -1;

        for (int i = 2 * binsPeakingTime + binsGapTime; i < nBins; ++i)
        {
            double sumPeaking = 0.0;
            for (int j = i - binsPeakingTime; j < i; ++j)
                sumPeaking += signal[j];

            double sumGap = 0.0;
            for (int j = i - 2 * binsPeakingTime - binsGapTime; j < i - binsPeakingTime - binsGapTime; ++j)
                sumGap += signal[j];

            const double diff = sumPeaking - sumGap;

            if (diff > maxDiff)
            {
                maxDiff = diff;
                // binMaxDiff = i;
            }
        }

        const double mawmax = (maxDiff == -1.0) ? 0.0 : maxDiff * averaging;
        return mawmax;
    }

    void ApplySGFilter(std::array<double, ACTAF_BINS>& signal, std::vector<double> coeffs);

    double ComputeRiseTime(const std::array<double, ACTAF_BINS>& signal);

    double ComputeLeadingEdge90(const std::array<double, ACTAF_BINS>& x);

    double ComputeLeadingEdge10(const std::array<double, ACTAF_BINS>& x);

    void SubtractBaseline(const std::array<double, ACTAF_BINS>& signal,
                          double baseline,
                          std::array<double, ACTAF_BINS>& out);

}; // namespace R3BActafUtils
