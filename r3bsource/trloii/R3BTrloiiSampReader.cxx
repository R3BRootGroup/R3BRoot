/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BTrloiiSampReader.h"
#include "FairRootManager.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BSampLosMappedData.h"
#include "R3BSamplerMappedData.h"
#include "TClonesArray.h"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_samp.h"
#include "ext_h101_samplos.h"
#include "ext_h101_samplosms.h"
#include "ext_h101_sampms.h"
}

using namespace std;

R3BTrloiiSampReader::R3BTrloiiSampReader(EXT_STR_h101_SAMP* data, size_t offset)
    : R3BReader("R3BTrloiiSampReader")
    , fSampData(data)
    , fOffset(offset)
    , fSNum(1)
    , fSCNum(1)
    , fArray(new TClonesArray("R3BSamplerMappedData"))
    , fOnline(kFALSE)
{
}

R3BTrloiiSampReader::R3BTrloiiSampReader(EXT_STR_h101_SAMPLOS* data, size_t offset)
    : R3BReader("R3BTrloiiSampReader")
    , fSampLosData(data)
    , fOffset(offset)
    , fSNum(1)
    , fSCNum(2)
    , fArrayLH(new TClonesArray("R3BSampLosMappedData"))
    , fOnline(kFALSE)
{
}

R3BTrloiiSampReader::R3BTrloiiSampReader(EXT_STR_h101_SAMPMS* data, size_t offset)
    : R3BReader("R3BTrloiiSampMSReader")
    , fSampMSData(data)
    , fOffset(offset)
    , fSNum(2)
    , fSCNum(1)
    , fArray(new TClonesArray("R3BSamplerMappedData"))
    , fOnline(kFALSE)
{
}

R3BTrloiiSampReader::R3BTrloiiSampReader(EXT_STR_h101_SAMPLOSMS* data, size_t offset)
    : R3BReader("R3BTrloiiSampMSReader")
    , fSampLosMSData(data)
    , fOffset(offset)
    , fSNum(2)
    , fSCNum(2)
    , fArrayLH(new TClonesArray("R3BSampLosMappedData"))
    , fOnline(kFALSE)
{
}

R3BTrloiiSampReader::~R3BTrloiiSampReader()
{
    if (fArray)
    {
        delete fArray;
    }

    if (fArrayLH)
    {
        delete fArrayLH;
    }
}

Bool_t R3BTrloiiSampReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok = 0;

    if (1 == fSNum && 1 == fSCNum)
    {
        EXT_STR_h101_SAMP_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_SAMP, 0);
        R3BLOG(INFO, "R3BTrloiiSampReader::Init() SAMP");
    }
    else if (1 == fSNum && 2 == fSCNum)
    {
        EXT_STR_h101_SAMPLOS_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_SAMPLOS, 0);
        R3BLOG(INFO, "R3BTrloiiSampReader::Init() SAMPLOS");
    }
    else if (2 == fSNum && 1 == fSCNum)
    {
        EXT_STR_h101_SAMPMS_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_SAMPMS, 0);
        R3BLOG(INFO, "R3BTrloiiSampReader::Init() SAMP master start");
    }
    else if (2 == fSNum && 2 == fSCNum)
    {
        EXT_STR_h101_SAMPLOSMS_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_SAMPLOSMS, 0);
        R3BLOG(INFO, "R3BTrloiiSampReader::Init() SAMPLOS master start");
    }

    if (!ok)
    {
        perror("ext_data_struct_info_item");
        R3BLOG(ERROR, "Failed to setup structure information.");
        return kFALSE;
    }

    // Register output array in tree
    if (1 == fSNum && 1 == fSCNum)
    {
        FairRootManager::Instance()->Register("SamplerMapped", "Sampler mapped data", fArray, !fOnline);
        Reset();
        memset(fSampData, 0, sizeof *fSampData);
        EXT_STR_h101_SAMP_onion* data = (EXT_STR_h101_SAMP_onion*)fSampData;
        data->SAMP = 0;
    }
    else if (1 == fSNum && 2 == fSCNum)
    {
        FairRootManager::Instance()->Register("SamplerMapped", "Sampler mapped data", fArrayLH, !fOnline);
        Reset();
        memset(fSampLosData, 0, sizeof *fSampLosData);
        EXT_STR_h101_SAMPLOS_onion* data = (EXT_STR_h101_SAMPLOS_onion*)fSampLosData;
        data->SAMPLOSL = 0;
        data->SAMPLOSH = 0;
    }
    else if (2 == fSNum && 1 == fSCNum)
    {
        FairRootManager::Instance()->Register("SamplerMSMapped", "Sampler Master Start mapped data", fArray, !fOnline);
        Reset();
        memset(fSampMSData, 0, sizeof *fSampMSData);
        EXT_STR_h101_SAMPMS_onion* data = (EXT_STR_h101_SAMPMS_onion*)fSampMSData;
        data->SAMPMS = 0;
    }
    else if (2 == fSNum && 2 == fSCNum)
    {
        FairRootManager::Instance()->Register(
            "SamplerMSMapped", "Sampler Master Start mapped data", fArrayLH, !fOnline);
        Reset();
        memset(fSampLosMSData, 0, sizeof *fSampLosMSData);
        EXT_STR_h101_SAMPLOSMS_onion* data = (EXT_STR_h101_SAMPLOSMS_onion*)fSampLosMSData;
        data->SAMPMSL = 0;
        data->SAMPMSH = 0;
    }

    return kTRUE;
}

Bool_t R3BTrloiiSampReader::Read()
{
    if (1 == fSNum && 1 == fSCNum)
    {
        for (uint32_t i = 0; i < fSampData->SAMP; ++i)
        {
            // auto ch = fSampData->SAMPI[i]; //unused atm, normally SAMPI[i]==i
            auto v = fSampData->SAMPv[i];
            new ((*fArray)[fArray->GetEntriesFast()]) R3BSamplerMappedData(v);
        }
    }

    else if (1 == fSNum && 2 == fSCNum)
    {
        if (fSampLosData->SAMPLOSL != fSampLosData->SAMPLOSH)
        {
            R3BLOG(WARNING, "SAMPLOSL and SAMPLOSH array size mismatch!");
            return kFALSE;
        }
        else
        {
            for (uint32_t i = 0; i < fSampLosData->SAMPLOSL; ++i)
            {
                auto vl = fSampLosData->SAMPLOSLv[i];
                auto vh = fSampLosData->SAMPLOSHv[i];
                new ((*fArrayLH)[fArrayLH->GetEntriesFast()]) R3BSampLosMappedData(vl, vh);
            }
        }
    }

    else if (2 == fSNum && 1 == fSCNum)
    {
        for (uint32_t i = 0; i < fSampMSData->SAMPMS; ++i)
        {
            auto v = fSampMSData->SAMPMSv[i];
            new ((*fArray)[fArray->GetEntriesFast()]) R3BSamplerMappedData(v);
        }
    }

    else if (2 == fSNum && 2 == fSCNum)
    {
        if (fSampLosMSData->SAMPMSL != fSampLosMSData->SAMPMSH)
        {
            R3BLOG(WARNING, "SAMPMSL and SAMPMSH array size mismatch!");
            return kFALSE;
        }
        else
        {
            for (uint32_t i = 0; i < fSampLosMSData->SAMPMSL; ++i)
            {
                auto vl = fSampLosMSData->SAMPMSLv[i];
                auto vh = fSampLosMSData->SAMPMSHv[i];
                new ((*fArrayLH)[fArrayLH->GetEntriesFast()]) R3BSampLosMappedData(vl, vh);
            }
        }
    }

    return kTRUE;
}

void R3BTrloiiSampReader::Reset()
{
    // Reset the output arrays
    if (1 == fSCNum)
        fArray->Clear();
    if (2 == fSCNum)
        fArrayLH->Clear();
}

ClassImp(R3BTrloiiSampReader)
