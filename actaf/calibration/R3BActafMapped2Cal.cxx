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

// -----------------------------------------------------------------
// -----            R3BActafMapped2Cal source file             -----
// -----      Created 14/09/25 by J.L. Rodriguez-Sanchez       -----
// -----------------------------------------------------------------

// ROOT headers
#include <TClonesArray.h>
#include <TMath.h>
#include <array>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

// FAIR headers
#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRuntimeDb.h>

// ACTAF headers
#include "R3BActafCalData.h"
#include "R3BActafCalPar.h"
#include "R3BActafMapped2Cal.h"
#include "R3BLogger.h"

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

// R3BActafMapped2Cal::Default Constructor --------------------------
R3BActafMapped2Cal::R3BActafMapped2Cal()
    : R3BActafMapped2Cal("R3BActafMapped2Cal", 1)
{
}

// R3BActafMapped2Cal::Standard Constructor --------------------------
R3BActafMapped2Cal::R3BActafMapped2Cal(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
    fEGain.resize(fPad);
    fEThr.resize(fPad);
}

// Virtual R3BActafMapped2Cal::Destructor
R3BActafMapped2Cal::~R3BActafMapped2Cal()
{
    R3BLOG(debug1, "");
    if (fActafCalData)
        delete fActafCalData;

    if (fCal_Par)
        delete fCal_Par;
}

void R3BActafMapped2Cal::SetParContainers()
{
    // Parameter Container
    // Reading actafCalPar from FairRuntimeDb

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(error, !rtdb, "FairRuntimeDb not found");

    fCal_Par = dynamic_cast<R3BActafCalPar*>(rtdb->getContainer("actafCalPar"));
    if (!fCal_Par)
    {
        R3BLOG(error, "actafCalPar container not found");
    }
    else
    {
        R3BLOG(info, "actafCalPar found");
    }
}

void R3BActafMapped2Cal::SetParameter()
{
    if (fCal_Par == nullptr)
    {
        R3BLOG(error, "actaf CalPar not found");
        return;
    }

    for (int i = 0; i < fPad; i++)
    {
        fEGain[i] = fCal_Par->GetGainVal(i);
        fEThr[i] = fCal_Par->GetThresholdVal(i);
    }

    fNbSgCoeffs = fCal_Par->GetNbSGCoeffs();
    fSgCoeffs.resize(fNbSgCoeffs);
    for (auto index = 0; index < fNbSgCoeffs; ++index)
    {
        fSgCoeffs[index] = fCal_Par->GetSGCoeff(index);
    }
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BActafMapped2Cal::Init()
{
    R3BLOG(info, "");
    auto* mgr = FairRootManager::Instance();
    if (!mgr)
    {
        R3BLOG(fatal, "FairRootManager not found");
        return kFATAL;
    }

    // INPUT DATA
    fActafMappedData = dynamic_cast<TClonesArray*>(mgr->GetObject("ActafMappedData"));
    if (!fActafMappedData)
    {
        R3BLOG(fatal, "ActafMappedData not found");
        return kFATAL;
    }

    // OUTPUT DATA
    fActafCalData = new TClonesArray("R3BActafCalData");
    mgr->Register("ActafCalData", "ACTAF_Cal", fActafCalData, !fOnline);
    Reset();

    SetParameter();

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BActafMapped2Cal::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BActafMapped2Cal::ApplySGFilter(std::array<double, ACTAF_BINS>& signal, std::vector<double> coeffs)
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

// -----   Public method Execution   --------------------------------------------
void R3BActafMapped2Cal::Exec(Option_t*)
{
    // Reset entries in the output arrays
    Reset();

    // Reading the Input -- Mapped Data --
    auto nHits = fActafMappedData->GetEntriesFast();
    if (nHits == 0)
    {
        return;
    }

    double synTagTime = 0.;
    for (size_t index = 0; index < nHits; ++index)
    {
        auto mappedData = dynamic_cast<R3BActafMappedData const*>(fActafMappedData->At(index));
        auto pad = mappedData->GetPad();
        if (pad == 129)
            synTagTime = mappedData->GetLeadingEdgeTime() * fConversionCh2ns; // in ns
    }

    for (size_t index = 0; index < nHits; ++index)
    {
        auto mappedData = dynamic_cast<R3BActafMappedData const*>(fActafMappedData->At(index));
        auto pad = mappedData->GetPad();
        if (pad >= 129 || pad == fPulserCh) // syn-time or pulser channel
            continue;

        std::array<double, ACTAF_BINS> waveform = mappedData->GetTrace();

        // Calculate rms before filtering
        int maxPos = FindMaxPosition(waveform);
        double rmsRaw = ComputeBaselineMean(waveform, maxPos, 0);
        double meanRaw = ComputeBaselineMean(waveform, maxPos, 1) + mappedData->GetBaseline();
        // Apply the SG filter to the waveform
        if (fApplySGFilter)
            ApplySGFilter(waveform, fSgCoeffs);

        auto integral = IntegratePulse(waveform, mappedData->GetMaxpos());
        auto energy = integral * fEGain[pad - 1];
        auto energyMaxAmpl = mappedData->GetMaxampl() * fEGain[pad - 1];

        auto drift = mappedData->GetLeadingEdgeTime() * fConversionCh2ns; // in ns
        auto zpos = drift * fVelocity;                                    // in cm
        auto syntime = drift - synTagTime;                                // in ns

        // Calculation of RMS from the waveform after filtering
        maxPos = FindMaxPosition(waveform);
        double rms = ComputeBaselineMean(waveform, maxPos, 0);
        double mean = ComputeBaselineMean(waveform, maxPos, 1) + mappedData->GetBaseline();

        // MAW parameter value
        double maw = mappedData->GetMaw();

        if (energyMaxAmpl >= fEThr[pad - 1] && energy < fMaxE)
            AddCalData(pad, energy, energyMaxAmpl, drift, zpos, syntime, waveform, rmsRaw, rms, meanRaw, mean, maw);
    }
    return;
}

// -----   Public method Reset   ------------------------------------------------
void R3BActafMapped2Cal::Reset()
{
    R3BLOG(debug1, "Clearing CalData Structure");
    if (fActafCalData)
    {
        fActafCalData->Clear();
    }
}

// -----   Private method AddCalData  --------------------------------------------
R3BActafCalData* R3BActafMapped2Cal::AddCalData(UInt_t padId,
                                                double energy,
                                                double maxampl,
                                                double drift,
                                                double zpos,
                                                double syntime,
                                                const std::array<double, ACTAF_BINS>& trace,
                                                double rmsRaw,
                                                double rms,
                                                double meanRaw,
                                                double mean,
                                                double maw)
{
    // It fills the R3BActafCalData
    TClonesArray& clref = *fActafCalData;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size])
        R3BActafCalData(padId, energy, maxampl, drift, zpos, syntime, trace, rmsRaw, rms, meanRaw, mean, maw);
}

ClassImp(R3BActafMapped2Cal)
