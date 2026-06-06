/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
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

#include "R3BCalifaFebexReader.h"
#include "R3BCalifaMappedData.h"
#include "R3BLogger.h"

#include <TClonesArray.h>

/**
 ** ext_h101_califa.h was created by running
 ** $unpacker --ntuple=STRUCT_HH,RAW:CALIFA,id=h101_CALIFA,NOTRIGEVENTNO,ext_h101_califa.h
 **/

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_califa202402.h"
#include "ext_h101_califa202506.h"
}

R3BCalifaFebexReader::R3BCalifaFebexReader(EXT_STR_h101_CALIFA202402_onion* data, size_t offset)
    : R3BReader("R3BCalifaFebexReader")
    , fNEvent(0)
    , fData24(data)
    , fOffset(offset)
    , fOnline(kFALSE)
    , fArray(new TClonesArray("R3BCalifaMappedData"))
    , fArraytrig(new TClonesArray("R3BCalifaMappedData"))
{
}

R3BCalifaFebexReader::R3BCalifaFebexReader(EXT_STR_h101_CALIFA202506_onion* data, size_t offset)
    : R3BReader("R3BCalifaFebexReader")
    , fNEvent(0)
    , fData25(data)
    , fOffset(offset)
    , fOnline(kFALSE)
    , fArray(new TClonesArray("R3BCalifaMappedData"))
    , fArraytrig(new TClonesArray("R3BCalifaMappedData"))
{
}

R3BCalifaFebexReader::~R3BCalifaFebexReader()
{
    if (fArray)
        delete fArray;
    if (fArraytrig)
        delete fArraytrig;
}

Bool_t R3BCalifaFebexReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t okay = 0;
    R3BLOG(info, "");
    if (fData24 != nullptr)
    {
        EXT_STR_h101_CALIFA202402_ITEMS_INFO(okay, *a_struct_info, fOffset, EXT_STR_h101_CALIFA202402, 1);
        memset(fData24, 0, sizeof *fData24);
    }
    else if (fData25 != nullptr)
    {
        EXT_STR_h101_CALIFA202506_ITEMS_INFO(okay, *a_struct_info, fOffset, EXT_STR_h101_CALIFA202506, 1);
        memset(fData25, 0, sizeof *fData25);
    }

    if (okay == 0)
    {
        R3BLOG(error, "Failed to setup structure information");
        return kFALSE;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("CalifaMappedData", "Califa mapped data", fArray, !fOnline);
    FairRootManager::Instance()->Register("CalifaMappedtrigData", "Califa mapped trigger data", fArraytrig, !fOnline);
    Reset();

    return kTRUE;
}

Bool_t R3BCalifaFebexReader::R3BRead()
{
    R3BLOG(debug1, "Event data.");

    if (fData24 != nullptr)
    {
        fData = reinterpret_cast<EXT_STR_h101_CALIFA202402_onion*>(fData24);
    }
    else
    {
        fData = reinterpret_cast<EXT_STR_h101_CALIFA202506_onion*>(fData25);
    }
    // SELECT THE FOR LOOP BASED ON THE MAPPING...
    std::visit(
        [this](auto data)
        {
            for (int crystal = 0; crystal < data->CALIFA_ENE; ++crystal)
            {
                UShort_t channelNumber = data->CALIFA_ENEI[crystal];
                int16_t energy = data->CALIFA_ENEv[crystal];
                int16_t nf = data->CALIFA_NFv[crystal];
                int16_t ns = data->CALIFA_NSv[crystal];

                uint64_t febextime =
                    ((uint64_t)data->CALIFA_TSMSBv[crystal] << 32) | (uint64_t)data->CALIFA_TSLSBv[crystal];

                uint64_t wrts = ((uint64_t)data->CALIFA_WRTS_T[3].v[crystal] << 48) |
                                ((uint64_t)data->CALIFA_WRTS_T[2].v[crystal] << 32) |
                                ((uint64_t)data->CALIFA_WRTS_T[1].v[crystal] << 16) |
                                (uint64_t)data->CALIFA_WRTS_T[0].v[crystal];

                int32_t ov = data->CALIFA_OVv[crystal];
                int16_t pu = data->CALIFA_PILEUPv[crystal];
                int16_t dc = data->CALIFA_DISCARDv[crystal];

                int16_t tot = data->CALIFA_TOTv[crystal];

                new ((*fArray)[fArray->GetEntriesFast()])
                    R3BCalifaMappedData(channelNumber, energy, nf, ns, febextime, wrts, ov, pu, dc, tot);
            }

            // Trigger signals for correlations
            for (int i = 0; i < data->CALIFA_TRGENE; ++i)
            {
                UShort_t channelNumber = data->CALIFA_TRGENEI[i];
                int16_t energy = data->CALIFA_TRGENEv[i];
                new ((*fArraytrig)[fArraytrig->GetEntriesFast()])
                    R3BCalifaMappedData(channelNumber, energy, 0, 0, 0, 0, 0, 0, 0, 0);
            }
        },
        fData);

    fNEvent += 1;
    return kTRUE;
}

void R3BCalifaFebexReader::Reset()
{
    // Reset the output array
    fArray->Clear();
    fArraytrig->Clear();
}

ClassImp(R3BCalifaFebexReader)
