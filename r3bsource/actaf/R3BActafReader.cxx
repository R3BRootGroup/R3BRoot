/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRuntimeDb.h>

#include "R3BActafMappedData.h"
#include "R3BActafMappingPar.h"
#include "R3BActafReader.h"
#include "R3BLogger.h"

#include <TClonesArray.h>
#include <algorithm>
#include <array>
#include <ext_data_struct_info.hh>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

/**
 ** ext_h101_actaf202X.h was created by running
 ** $unpacker --ntuple=STRUCT_HH,RAW:ACTAF,id=h101_ACTAF202X,NOTRIGEVENTNO,ext_h101_actaf202X.h
 **/

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_actaf2023.h"
#include "ext_h101_actaf2025.h"
}

#define MAX_MODULES2023 (sizeof data->ACTAF / sizeof data->ACTAF[0])
#define MAX_MODULES2025 (sizeof data->ACTAF_ADC_MOD / sizeof data->ACTAF_ADC_MOD[0])

template <class Cont>
int FindMaxPosition(const Cont& signal)
{
    return std::distance(signal.begin(), std::max_element(signal.begin(), signal.end()));
}

template <class Cont>
int FindMaxAmplitude(const Cont& signal)
{
    return *std::max_element(signal.begin(), signal.end());
}

template <class Cont>
double ComputeBaselineMean(const Cont& signal, int numBins, bool returnMean = 1)
{
    numBins = std::min(static_cast<int>(numBins * 0.8), static_cast<int>(signal.size()));
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
double IntegratePulse(const Cont& signal, int maxIdx, double baseline)
{
    int left = maxIdx, right = maxIdx;
    const int size = static_cast<int>(signal.size());

    while (left > 0 && signal[left] > baseline)
        --left;

    while (right < size - 1 && signal[right] > baseline)
        ++right;

    return std::accumulate(signal.begin() + left, signal.begin() + right, 0.0) - (right - left) * baseline;
}

inline void SubtractBaseline(const std::array<double, ACTAF_BINS>& signal,
                             double baseline,
                             std::array<double, ACTAF_BINS>& out)
{
    for (size_t i = 0; i < signal.size(); ++i)
        out[i] = signal[i] - baseline;
}

inline double ComputeLeadingEdge10(const std::array<double, ACTAF_BINS>& x, int maxIdx, double frac = 0.1)
{
    if (x.empty() || maxIdx <= 0 || maxIdx >= static_cast<int>(x.size()))
        return -1.0;

    const double xmax = *std::max_element(x.begin(), x.begin() + maxIdx + 1);
    const double thr = frac * xmax;

    if (frac > 0)
    {
        for (int i = 1; i <= maxIdx; ++i)
        {
            if (x[i - 1] < thr && x[i] >= thr)
            {
                const double y0 = x[i - 1], y1 = x[i];
                const double dy = y1 - y0;
                if (dy <= 0)
                    return static_cast<double>(i);
                const double alpha = (thr - y0) / dy;
                return static_cast<double>(i - 1) + alpha;
            }
        }
    }
    else
    {
        double t90 = ComputeLeadingEdge10(x, maxIdx, 0.9);
        double t10 = ComputeLeadingEdge10(x, maxIdx, 0.1);

        if (t10 < 0.0 || t90 < 0.0 || t90 <= t10)
            return -1.0;

        const double T10 = 0.1 * xmax;
        const double T90 = 0.9 * xmax;

        const double m = (T90 - T10) / (t90 - t10);
        const double b = T10 - m * t10;

        return -b / m;
    }

    return -1.0;
}

inline double ComputeRiseTime(const std::array<double, ACTAF_BINS>& signal, double maxIdx, bool useT0 = true)
{

    if (maxIdx <= 0 || maxIdx >= static_cast<int>(signal.size()))
        return -1.0;

    double t90 = ComputeLeadingEdge10(signal, maxIdx, 0.9);

    double t0 = -1;

    if (useT0)
        t0 = ComputeLeadingEdge10(signal, maxIdx, 0);
    else
        t0 = ComputeLeadingEdge10(signal, maxIdx, 0.1);

    return t90 - t0;
}

inline void ApplySGFilter(std::array<double, ACTAF_BINS>& signal, std::vector<double> coeffs)
{
    auto n = signal.size(), m = coeffs.size();
    int half = m / 2;

    std::vector<double> output(n), ext(n + 2 * half);

    for (int i = 0; i < half; i++)
        ext[i] = signal[0];
    for (int i = 0; i < n; i++)
        ext[i + half] = signal[i];
    for (int i = 0; i < half; i++)
        ext[n + half + i] = signal[n - 1];

    for (int i = 0; i < n; i++)
    {
        double sum = 0.0;
        for (int j = 0; j < m; j++)
            sum += coeffs[j] * ext[i + j];
        output[i] = sum;
    }

    for (int i = 0; i < n; i++)
        signal[i] = output[i];
}

// ------------------------------ Reader impl ----------------------------------

R3BActafReader::R3BActafReader(EXT_STR_h101_ACTAF2023_onion* data, size_t offset)
    : R3BReader("R3BActafReader")
    , fData23(data)
    , fOffset(offset)
    , fArray(std::make_unique<TClonesArray>("R3BActafMappedData"))
    , fVersion(UnpackerVersion::v2023)
{
}

R3BActafReader::R3BActafReader(EXT_STR_h101_ACTAF2025_onion* data, size_t offset)
    : R3BReader("R3BActafReader")
    , fData25(data)
    , fOffset(offset)
    , fArray(std::make_unique<TClonesArray>("R3BActafMappedData"))
    , fVersion(UnpackerVersion::v2025)
{
}

void R3BActafReader::SetParContainers()
{
    auto* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");

    fMapping_Par = dynamic_cast<R3BActafMappingPar*>(rtdb->getContainer("actafMappingPar"));
    R3BLOG_IF(fatal, !fMapping_Par, "Could not get handle on actafMappingPar container");
}

Bool_t R3BActafReader::Init(ext_data_struct_info* a_struct_info)
{
    R3BLOG(info, "");
    Int_t okey = 0;
    constexpr int esf = 1;

    if (fVersion == UnpackerVersion::v2023)
    {
        EXT_STR_h101_ACTAF2023_ITEMS_INFO(okey, *a_struct_info, fOffset, EXT_STR_h101_ACTAF2023, esf);
        memset(fData23, 0, sizeof *fData23);
    }
    else if (fVersion == UnpackerVersion::v2025)
    {
        EXT_STR_h101_ACTAF2025_ITEMS_INFO(okey, *a_struct_info, fOffset, EXT_STR_h101_ACTAF2025, esf);
        memset(fData25, 0, sizeof *fData25);
    }
    else if (fVersion == UnpackerVersion::v2027)
    {
        R3BLOG(error, "UnpackerVersion 2027 not defined yet.");
        return kFALSE;
    }
    else
    {
        R3BLOG(error, "UnpackerVersion not found.");
        return kFALSE;
    }

    if (!okey)
    {
        R3BLOG(error, "Failed to setup structure information");
        return kFALSE;
    }

    FairRootManager::Instance()->Register("ActafMappedData", "Actaf mapped data", fArray.get(), !fOnline);
    Reset();

    mapping = std::vector<std::vector<int>>(fMapping_Par->GetNbFADCModules(), std::vector<int>(ACTAF_ECHN, 0));
    for (auto index = 0; index < fMapping_Par->GetNbPads(); ++index)
    {
        auto mod = fMapping_Par->GetFADCModule(index) - 1;
        auto chn = fMapping_Par->GetFADCChannel(index) - 1;
        mapping[mod][chn] = index + 1;
    }

    fNbSgCoeffs = fMapping_Par->GetNbSGCoeffs();
    fSgCoeffs.resize(fNbSgCoeffs);
    for (auto index = 0; index < fNbSgCoeffs; ++index)
    {
        fSgCoeffs[index] = fMapping_Par->GetSGCoeff(index);
    }
    return kTRUE;
}

Bool_t R3BActafReader::R3BRead()
{
    R3BLOG(debug1, "Event data.");

    fNEvent += 1;
    if (fVersion == UnpackerVersion::v2023)
    {
        return R3BRead2023();
    }
    else if (fVersion == UnpackerVersion::v2025)
    {
        return R3BRead2025();
    }
    else
    {
        return kFALSE;
    }
}

bool R3BActafReader::R3BRead2023()
{
    auto* data = reinterpret_cast<EXT_STR_h101_ACTAF2023_onion*>(fData23);

    for (int mod = 0; mod < MAX_MODULES2023; ++mod)
    {
        std::array<std::array<double, ACTAF_BINS>, ACTAF_ECHN> trace{};

        for (int flat = 0; flat < data->ACTAF[mod].TRACERAW; ++flat)
        {
            const int row = flat / ACTAF_BINS;
            const int col = flat % ACTAF_BINS;
            if (row >= ACTAF_ECHN || col >= ACTAF_BINS)
                continue;
            trace[row][col] = data->ACTAF[mod].TRACERAWv[flat];
        }

        std::array<int, ACTAF_ECHN> maxPos{};
        std::array<double, ACTAF_ECHN> baselineMean{};
        std::array<double, ACTAF_ECHN> integral{};
        std::array<int, ACTAF_ECHN> maxAmplitude{};
        std::array<double, ACTAF_ECHN> riseTime{};
        std::array<double, ACTAF_ECHN> leadingEdge10{};

        std::array<double, ACTAF_BINS> correctedtrace{};

        for (int chn = 0; chn < ACTAF_ECHN; ++chn)
        {
            maxPos[chn] = FindMaxPosition(trace[chn]);
            maxAmplitude[chn] = FindMaxAmplitude(trace[chn]);
            baselineMean[chn] = ComputeBaselineMean(trace[chn], maxPos[chn]);
            integral[chn] = IntegratePulse(trace[chn], maxPos[chn], baselineMean[chn]);

            SubtractBaseline(trace[chn], baselineMean[chn], correctedtrace);
            riseTime[chn] = ComputeRiseTime(correctedtrace, maxPos[chn], true);
            leadingEdge10[chn] = ComputeLeadingEdge10(correctedtrace, maxPos[chn], 0.);

            // Store corrected trace
            trace[chn] = correctedtrace;
        }

        const int nChToWrite = data->ACTAF[mod].CH;
        for (int chn = 0; chn < nChToWrite && chn < ACTAF_ECHN; ++chn)
        {
            new ((*fArray)[fArray->GetEntriesFast()]) R3BActafMappedData(mapping[mod][chn],
                                                                         trace[chn],
                                                                         integral[chn],
                                                                         baselineMean[chn],
                                                                         riseTime[chn],
                                                                         maxPos[chn],
                                                                         maxAmplitude[chn] - baselineMean[chn],
                                                                         leadingEdge10[chn]);
        }
    }
    return kTRUE;
}

bool R3BActafReader::R3BRead2025()
{
    auto* data = reinterpret_cast<EXT_STR_h101_ACTAF2025_onion*>(fData25);

    for (int mod = 0; mod < MAX_MODULES2025; ++mod)
    {
        std::array<std::array<double, ACTAF_BINS>, ACTAF_ECHN> trace{};

        for (int chn = 0; chn < ACTAF_ECHN; ++chn)
        {
            for (int i = 0; i < ACTAF_BINS; ++i)
            {
                trace[chn][i] = data->ACTAF_ADC_MOD[mod].CH[chn].v[i];
            }
        }

        std::array<int, ACTAF_ECHN> maxPos{};
        std::array<double, ACTAF_ECHN> baselineMean{};
        std::array<double, ACTAF_ECHN> baselineMeanNew{};
        std::array<double, ACTAF_ECHN> baselineStdOld{};
        std::array<double, ACTAF_ECHN> baselineStdNew{};
        std::array<double, ACTAF_ECHN> integral{};
        std::array<int, ACTAF_ECHN> maxAmplitude{};
        std::array<double, ACTAF_ECHN> riseTime{};
        std::array<double, ACTAF_ECHN> leadingEdge10{};
        std::array<double, ACTAF_BINS> correctedtrace{};

        for (int chn = 0; chn < ACTAF_ECHN; ++chn)
        {
            maxPos[chn] = FindMaxPosition(trace[chn]);
            maxAmplitude[chn] = FindMaxAmplitude(trace[chn]);
            baselineMean[chn] = ComputeBaselineMean(trace[chn], maxPos[chn]);
            baselineStdOld[chn] = ComputeBaselineMean(trace[chn], maxPos[chn], 0);
            integral[chn] = IntegratePulse(trace[chn], maxPos[chn], baselineMean[chn]);

            SubtractBaseline(trace[chn], baselineMean[chn], correctedtrace);
            if (fApplyFilter)
                ApplySGFilter(correctedtrace, fSgCoeffs);
            baselineStdNew[chn] = ComputeBaselineMean(correctedtrace, maxPos[chn], 0);
            riseTime[chn] = ComputeRiseTime(correctedtrace, maxPos[chn], true);
            leadingEdge10[chn] = ComputeLeadingEdge10(correctedtrace, maxPos[chn], 0);
            baselineMeanNew[chn] = ComputeBaselineMean(correctedtrace, maxPos[chn]) + baselineMean[chn];

            // Store corrected trace
            trace[chn] = correctedtrace;
        }

        const int nChToWrite = (mod < 8 ? ACTAF_ECHN : 1);
        for (int chn = 0; chn < nChToWrite; ++chn)
        {
            const UInt_t pad = (mod < 8 ? mapping[mod][chn] : 129);
            new ((*fArray)[fArray->GetEntriesFast()]) R3BActafMappedData(pad,
                                                                         trace[chn],
                                                                         integral[chn],
                                                                         baselineMean[chn],
                                                                         riseTime[chn],
                                                                         maxPos[chn],
                                                                         maxAmplitude[chn] - baselineMean[chn],
                                                                         leadingEdge10[chn],
                                                                         baselineStdOld[chn],
                                                                         baselineStdNew[chn],
                                                                         baselineMeanNew[chn]);
        }
    }

    if (data->DETECTORMASK > 0)
    { // Extra pad 130 for AMBER specific IDs
        new ((*fArray)[fArray->GetEntriesFast()])
            R3BActafMappedData(130, data->DETECTORMASK, data->AMBERTIMETAG, data->AMBERSPILLNB);
    }
    return kTRUE;
}

void R3BActafReader::Reset() { fArray->Clear(); }

ClassImp(R3BActafReader)
