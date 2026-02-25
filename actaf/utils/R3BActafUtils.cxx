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

#include "R3BActafUtils.h"

namespace R3BActafUtils
{
    void ApplySGFilter(std::array<double, ACTAF_BINS>& signal, std::vector<double> coeffs)
    {
        if (coeffs.size() % 2 == 0)
            throw std::runtime_error("SG filter requires odd number of coefficients");

        auto n = signal.size(), m = coeffs.size();
        int half = m / 2;

        std::vector<double> output(n), ext(n + 2 * half);

        for (auto i = 0; i < half; i++)
            ext[i] = signal[0];
        for (auto i = 0; i < n; i++)
            ext[i + half] = signal[i];
        for (auto i = 0; i < half; i++)
            ext[n + half + i] = signal[n - 1];

        for (auto i = 0; i < n; i++)
        {
            double sum = 0.0;
            for (auto j = 0; j < m; j++)
                sum += coeffs[j] * ext[i + j];
            output[i] = sum;
        }

        for (auto i = 0; i < n; i++)
            signal[i] = output[i];
    }

    void SubtractBaseline(const std::array<double, ACTAF_BINS>& signal,
                          double baseline,
                          std::array<double, ACTAF_BINS>& out)
    {
        for (size_t i = 0; i < signal.size(); ++i)
            out[i] = signal[i] - baseline;
    }

    double ComputeLeadingEdge90(const std::array<double, ACTAF_BINS>& x)
    {
        const int size = static_cast<int>(x.size());
        if (size < 2)
            return -1.0;

        const double xmax = *std::max_element(x.begin(), x.end());
        const double thr = 0.9 * xmax;

        for (int i = 1; i < size; ++i)
        {
            if (x[i - 1] < thr && x[i] >= thr)
            {
                const double alpha = (thr - x[i - 1]) / (x[i] - x[i - 1]);
                return (i - 1) + alpha;
            }
        }

        return -1.0;
    }

    double ComputeLeadingEdge10(const std::array<double, ACTAF_BINS>& x)
    {
        const int size = static_cast<int>(x.size());
        if (size < 2)
            return -1.0;

        const double xmax = *std::max_element(x.begin(), x.end());
        const double thr = 0.1 * xmax;

        for (int i = 1; i < size; ++i)
        {
            if (x[i - 1] < thr && x[i] >= thr)
            {
                const double alpha = (thr - x[i - 1]) / (x[i] - x[i - 1]);
                return (i - 1) + alpha;
            }
        }

        return -1.0;
    }

    double ComputeRiseTime(const std::array<double, ACTAF_BINS>& signal)
    {
        auto t90 = ComputeLeadingEdge90(signal);
        auto t10 = ComputeLeadingEdge10(signal);

        return ((t90 > 0 && t10 > 0) ? t90 - t10 : -1.0);
    }

} // namespace R3BActafUtils
