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
#include "R3BActafUtils.h"
#include "R3BLogger.h"

#include <TClonesArray.h>
#include <algorithm>
#include <array>
#include <ext_data_struct_info.hh>
#include <iostream>
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

    // Looking for the R3BEventHeader
    auto* frm = FairRootManager::Instance();
    fEventHeader = dynamic_cast<R3BEventHeader*>(frm->GetObject("EventHeader."));

    frm->Register("ActafMappedData", "Actaf mapped data", fArray.get(), !fOnline);
    Reset();

    mapping = std::vector<std::vector<int>>(fMapping_Par->GetNbFADCModules(), std::vector<int>(ACTAF_ECHN, 0));
    for (auto index = 0; index < fMapping_Par->GetNbPads(); ++index)
    {
        auto mod = fMapping_Par->GetFADCModule(index) - 1;
        auto chn = fMapping_Par->GetFADCChannel(index) - 1;
        mapping[mod][chn] = index + 1;
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
            maxPos[chn] = R3BActafUtils::FindMaxPosition(trace[chn]);
            maxAmplitude[chn] = R3BActafUtils::FindMaxAmplitude(trace[chn]);
            baselineMean[chn] = R3BActafUtils::ComputeBaselineMean(trace[chn], maxPos[chn]);
            integral[chn] = R3BActafUtils::IntegratePulse(trace[chn], maxPos[chn], baselineMean[chn]);

            R3BActafUtils::SubtractBaseline(trace[chn], baselineMean[chn], correctedtrace);
            riseTime[chn] = R3BActafUtils::ComputeRiseTime(correctedtrace);
            leadingEdge10[chn] = R3BActafUtils::ComputeLeadingEdge10(correctedtrace);
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
        std::array<double, ACTAF_BINS> maw{};

        for (int chn = 0; chn < ACTAF_ECHN; ++chn)
        {
            maxPos[chn] = R3BActafUtils::FindMaxPosition(trace[chn]);
            maxAmplitude[chn] = R3BActafUtils::FindMaxAmplitude(trace[chn]);
            if (maxPos[chn] > 0)
            {
                baselineMean[chn] = R3BActafUtils::ComputeBaselineMean(trace[chn], maxPos[chn]);
                baselineStdOld[chn] = R3BActafUtils::ComputeBaselineMean(trace[chn], maxPos[chn], 0);
            }
            else
            {
                baselineMean[chn] = R3BActafUtils::ComputeBaselineMean(trace[chn], 400);
                baselineStdOld[chn] = R3BActafUtils::ComputeBaselineMean(trace[chn], 400, 0);
            }
            integral[chn] = R3BActafUtils::IntegratePulse(trace[chn], maxPos[chn], baselineMean[chn]);

            R3BActafUtils::SubtractBaseline(trace[chn], baselineMean[chn], correctedtrace);
            baselineStdNew[chn] = R3BActafUtils::ComputeBaselineMean(correctedtrace, maxPos[chn], 0);
            riseTime[chn] = R3BActafUtils::ComputeRiseTime(correctedtrace);
            leadingEdge10[chn] = R3BActafUtils::ComputeLeadingEdge10(correctedtrace);
            baselineMeanNew[chn] = R3BActafUtils::ComputeBaselineMean(correctedtrace, maxPos[chn]) + baselineMean[chn];
            maw[chn] = R3BActafUtils::CalculateMAW(correctedtrace);
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
                                                                         baselineMeanNew[chn],
                                                                         maw[chn]);
        }
    }

    if (fPrevSpillNb == 0)
        fPrevSpillNb = data->AMBERSPILLNB;

    if (data->AMBERSPILLNB != fPrevSpillNb)
    {
        fPrevTimeStamp = fNextTimeStamp;
        fPrevSpillNb = data->AMBERSPILLNB;
    }

    fEventHeader->SetTimeStamp(fPrevTimeStamp + data->AMBERTIMETAG);
    fNextTimeStamp = fPrevTimeStamp + data->AMBERTIMETAG;

    // if (data->DETECTORMASK > 0)
    { // Extra pad 130 for AMBER specific IDs
        new ((*fArray)[fArray->GetEntriesFast()]) R3BActafMappedData(
            130, data->DETECTORMASK, fPrevTimeStamp + data->AMBERTIMETAG, data->AMBERSPILLNB, data->AMBERTIMETAG);
    }
    return kTRUE;
}

void R3BActafReader::Reset() { fArray->Clear(); }

ClassImp(R3BActafReader)
