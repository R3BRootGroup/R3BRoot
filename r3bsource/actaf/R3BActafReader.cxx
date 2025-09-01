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

#include "R3BActafMappedData.h"
#include "R3BActafReader.h"
#include "R3BLogger.h"

#include <TClonesArray.h>
#include <algorithm>
#include <ext_data_struct_info.hh>
#include <iostream>
#include <numeric>
#include <vector>

constexpr int eChn = 16;
constexpr int Bins = 2692;

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

int FindMaxPosition(const std::vector<UInt_t>& signal)
{
    return std::distance(signal.begin(), std::max_element(signal.begin(), signal.end()));
}

double ComputeBaselineMean(const std::vector<UInt_t>& signal, int numBins)
{
    numBins = std::min(static_cast<int>(numBins * 0.8), static_cast<int>(signal.size()));
    if (numBins <= 0)
        return 0.0; // Avoid division by zero

    return std::accumulate(signal.begin(), signal.begin() + numBins, 0.0) / numBins;
}

double IntegratePulse(const std::vector<UInt_t>& signal, int maxIdx, double baseline)
{
    int left = maxIdx, right = maxIdx;
    int size = signal.size();

    // Expand left until the signal falls back to the baseline
    while (left > 0 && signal[left] > baseline)
        --left;

    // Expand right until the signal falls back to the baseline
    while (right < size - 1 && signal[right] > baseline)
        ++right;

    // Compute the integral by summing the values within the range
    return std::accumulate(signal.begin() + left, signal.begin() + right, 0.0) - (right - left) * baseline;
}

// Function to compute the maximum amplitude of a signal
int FindMaxAmplitude(const std::vector<UInt_t>& signal) { return *std::max_element(signal.begin(), signal.end()); }

// Function to subtract the baseline from the signal
std::vector<double> SubtractBaseline(const std::vector<UInt_t>& signal, double baseline)
{
    std::vector<double> correctedSignal(signal.size());
    for (size_t i = 0; i < signal.size(); ++i)
        correctedSignal[i] = signal[i] - baseline;

    return correctedSignal;
}

// Function to compute the risetime (time from 10% to 90% of max amplitude)
int ComputeRiseTime(const std::vector<double>& signal, double maxValue)
{
    double threshold10 = 0.1 * maxValue; // 10% of peak amplitude
    double threshold90 = 0.9 * maxValue; // 90% of peak amplitude

    int t10 = -1, t90 = -1;

    // Scan the signal to find the first occurrence of 10% and 90% of the max amplitude
    for (size_t i = 0; i < signal.size(); ++i)
    {
        if (signal[i] >= threshold10 && t10 == -1)
            t10 = i;
        if (signal[i] >= threshold90)
        {
            t90 = i;
            break; // Stop as soon as we find t90
        }
    }

    // Ensure both t10 and t90 were found
    if (t10 == -1 || t90 == -1)
        return -1; // Return -1 if risetime cannot be computed

    return t90 - t10; // Rise time in number of bins
}

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

    // Register output array in tree
    FairRootManager::Instance()->Register("ActafMappedData", "Actaf mapped data", fArray.get(), !fOnline);
    Reset();

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
        std::vector<std::vector<UInt_t>> trace(eChn, std::vector<UInt_t>(Bins));
        for (int chn = 0; chn < data->ACTAF[mod].TRACERAW; ++chn)
        {
            int row = chn / Bins;
            int col = chn % Bins;
            trace[row][col] = data->ACTAF[mod].TRACERAWv[chn];
        }

        std::vector<int> maxPos(eChn);
        std::vector<double> baselineMean(eChn);
        std::vector<double> integral(eChn);
        std::vector<int> maxAmplitude(eChn);
        std::vector<int> riseTime(eChn);

        for (int chn = 0; chn < eChn; ++chn)
        {
            maxPos[chn] = FindMaxPosition(trace[chn]);
            maxAmplitude[chn] = FindMaxAmplitude(trace[chn]);
            baselineMean[chn] = ComputeBaselineMean(trace[chn], maxPos[chn]);
            integral[chn] = IntegratePulse(trace[chn], maxPos[chn], baselineMean[chn]);

            // Subtract baseline from signal
            std::vector<double> correctedtrace = SubtractBaseline(trace[chn], baselineMean[chn]);
            riseTime[chn] = ComputeRiseTime(correctedtrace, maxAmplitude[chn] - baselineMean[chn]);
        }

        for (int chn = 0; chn < data->ACTAF[mod].CH; ++chn)
        {
            new ((*fArray)[fArray->GetEntriesFast()]) R3BActafMappedData(mod * eChn + (chn + 1),
                                                                         trace[chn],
                                                                         integral[chn],
                                                                         baselineMean[chn],
                                                                         riseTime[chn],
                                                                         maxPos[chn],
                                                                         maxAmplitude[chn] - baselineMean[chn]);
        }
    }
    return kTRUE;
}

bool R3BActafReader::R3BRead2025()
{
    auto* data = reinterpret_cast<EXT_STR_h101_ACTAF2025_onion*>(fData25);
    for (int mod = 0; mod < MAX_MODULES2025 - 1; ++mod)
    {
        std::vector<std::vector<UInt_t>> trace(eChn, std::vector<UInt_t>(Bins));
        for (int chn = 0; chn < eChn; ++chn)
        {
            for (int size = 0; size < Bins; size++)
            {
                // std::cout<<data->ACTAF_ADC_MOD[mod].CH[chn].v[size]<<std::endl;
                trace[chn][size] = data->ACTAF_ADC_MOD[mod].CH[chn].v[size];
            }
        }

        std::vector<int> maxPos(eChn);
        std::vector<double> baselineMean(eChn);
        std::vector<double> integral(eChn);
        std::vector<int> maxAmplitude(eChn);
        std::vector<int> riseTime(eChn);

        for (int chn = 0; chn < eChn; ++chn)
        {
            maxPos[chn] = FindMaxPosition(trace[chn]);
            maxAmplitude[chn] = FindMaxAmplitude(trace[chn]);
            baselineMean[chn] = ComputeBaselineMean(trace[chn], maxPos[chn]);
            integral[chn] = IntegratePulse(trace[chn], maxPos[chn], baselineMean[chn]);

            // Subtract baseline from signal
            std::vector<double> correctedtrace = SubtractBaseline(trace[chn], baselineMean[chn]);
            riseTime[chn] = ComputeRiseTime(correctedtrace, maxAmplitude[chn] - baselineMean[chn]);
        }

        for (int chn = 0; chn < eChn; ++chn)
        {
            new ((*fArray)[fArray->GetEntriesFast()]) R3BActafMappedData(mod * eChn + (chn + 1),
                                                                         trace[chn],
                                                                         integral[chn],
                                                                         baselineMean[chn],
                                                                         riseTime[chn],
                                                                         maxPos[chn],
                                                                         maxAmplitude[chn] - baselineMean[chn]);
        }
    }

    if (data->AMBERTIMETAG > 0)
    {
        new ((*fArray)[fArray->GetEntriesFast()])
            R3BActafMappedData((MAX_MODULES2025 - 1) * eChn + 1, std::vector<UInt_t>(), 0, 0, data->AMBERTIMETAG, 0, 0);
    }

    return kTRUE;
}

void R3BActafReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

ClassImp(R3BActafReader)
