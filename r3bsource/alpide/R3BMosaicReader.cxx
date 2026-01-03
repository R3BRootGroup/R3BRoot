/******************************************************************************
 *   Copyright (C) 2024 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2024-2026 Members of R3B Collaboration                     *
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

#include "R3BAlpideMappedData.h"
#include "R3BLogger.h"
#include "R3BMosaicReader.h"
#include "R3BWRData.h"

#include <TClonesArray.h>
#include <ext_data_struct_info.hh>

/**
 ** ext_h101_mosaic.h was created by running
 ** $unpacker --ntuple=STRUCT_HH,RAW:MOSAIC,id=h101_MOSAIC,NOTRIGEVENTNO,ext_h101_mosaic.h
 **/

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_mosaic202402.h"
#include "ext_h101_mosaic202506.h"
#include "ext_h101_mosaic202507.h"
}

R3BMosaicReader::R3BMosaicReader(EXT_STR_h101_MOSAIC202402_onion* data, size_t offset)
    : R3BReader("R3BMosaicReader")
    , fData2402(data)
    , fNbMosaic(sizeof(fData2402->MOSAIC) / sizeof(fData2402->MOSAIC[0]))
    , fOffset(offset)
    , fArray(new TClonesArray("R3BAlpideMappedData"))
{
}

R3BMosaicReader::R3BMosaicReader(EXT_STR_h101_MOSAIC202506_onion* data, size_t offset)
    : R3BReader("R3BMosaicReader")
    , fData2506(data)
    , fOffset(offset)
    , fArray(new TClonesArray("R3BAlpideMappedData"))
    , fArray_TS(new TClonesArray("R3BWRData"))
    , fVersion(UnpackerMosaicVersion::v202506)
{
}

R3BMosaicReader::R3BMosaicReader(EXT_STR_h101_MOSAIC202507_onion* data, size_t offset)
    : R3BReader("R3BMosaicReader")
    , fData2507(data)
    , fOffset(offset)
    , fArray(new TClonesArray("R3BAlpideMappedData"))
    , fArray_TS(new TClonesArray("R3BWRData"))
    , fVersion(UnpackerMosaicVersion::v202507)
{
}

R3BMosaicReader::~R3BMosaicReader()
{
    if (fArray)
    {
        delete fArray;
    }
    if (fArray_TS)
    {
        delete fArray_TS;
    }
}

Bool_t R3BMosaicReader::Init(ext_data_struct_info* a_struct_info)
{
    int okay = 0;
    R3BLOG(info, "");

    if (fVersion == UnpackerMosaicVersion::v202402)
    {
        EXT_STR_h101_MOSAIC202402_ITEMS_INFO(okay, *a_struct_info, fOffset, EXT_STR_h101_MOSAIC202402, 0);
        memset(fData2402, 0, sizeof(*fData2402));
    }
    else if (fVersion == UnpackerMosaicVersion::v202506)
    {
        EXT_STR_h101_MOSAIC202506_ITEMS_INFO(okay, *a_struct_info, fOffset, EXT_STR_h101_MOSAIC202506, 0);
        memset(fData2506, 0, sizeof(*fData2506));
    }
    else if (fVersion == UnpackerMosaicVersion::v202507)
    {
        EXT_STR_h101_MOSAIC202507_ITEMS_INFO(okay, *a_struct_info, fOffset, EXT_STR_h101_MOSAIC202507, 0);
        memset(fData2507, 0, sizeof(*fData2507));
    }

    R3BLOG_IF(fatal, !okay, "Failed to setup structure information.");

    // Register output array in tree
    FairRootManager::Instance()->Register("AlpideMappedData", "ALPIDE_Map", fArray, !fOnline);
    FairRootManager::Instance()->Register("WRAlpideData", "WRAlpide", fArray_TS, !fOnline);
    Reset();

    return kTRUE;
}

Bool_t R3BMosaicReader::R3BRead()
{
    R3BLOG(debug1, "Event data: " << fNEvent);
    fNEvent++;
    if (fVersion == UnpackerMosaicVersion::v202402)
    {
        return R3BRead202402();
    }
    else if (fVersion == UnpackerMosaicVersion::v202506)
    {
        return R3BRead202506();
    }
    else if (fVersion == UnpackerMosaicVersion::v202507)
    {
        return R3BRead202507();
    }
    else
    {
        return kFALSE;
    }
}

bool R3BMosaicReader::R3BRead202402()
{
    for (int mosid = 0; mosid < fNbMosaic; mosid++)
    {
        for (int hits = 0; hits < fData2402->MOSAIC[mosid].CHIP; hits++)
        {
            int fChipId = fData2402->MOSAIC[mosid].CHIPv[hits];

            int fAlpideId = map_mosaics[mosid] * fNb_sensors_flex + fChipId + 1; // 1-base
            R3BLOG_IF(error, fAlpideId < 1, "Wrong fAlpideId: " << fAlpideId);

            fAlpideId = map_sensors[fAlpideId - 1]; // 1-base

            new ((*fArray)[fArray->GetEntriesFast()]) R3BAlpideMappedData(fAlpideId,
                                                                          0,
                                                                          mosid + 1,
                                                                          fChipId,
                                                                          fData2402->MOSAIC[mosid].ROWv[hits],
                                                                          fData2402->MOSAIC[mosid].COLv[hits]);
        }
    }
    return kTRUE;
}

bool R3BMosaicReader::R3BRead202506()
{
    // MOSAIC-3 corresponds to the second flex
    for (int hits = 0; hits < fData2506->MOSAIC3CHIP; hits++)
    {
        int fChipId = fData2506->MOSAIC3CHIPv[hits];

        int fAlpideId = fNb_sensors_flex + (fChipId + 1); // 1-base
        R3BLOG_IF(error, fAlpideId < 1, "Wrong fAlpideId: " << fAlpideId);

        new ((*fArray)[fArray->GetEntriesFast()])
            R3BAlpideMappedData(fAlpideId, 0, 2, fChipId, fData2506->MOSAIC3ROWv[hits], fData2506->MOSAIC3COLv[hits]);
    }

    // MOSAIC-4 corresponds to the first flex
    for (int hits = 0; hits < fData2506->MOSAIC4CHIP; hits++)
    {
        int fChipId = fData2506->MOSAIC4CHIPv[hits];

        int fAlpideId = fChipId + 1; // 1-base
        R3BLOG_IF(error, fAlpideId < 1, "Wrong fAlpideId: " << fAlpideId);

        new ((*fArray)[fArray->GetEntriesFast()])
            R3BAlpideMappedData(fAlpideId, 0, 1, fChipId, fData2506->MOSAIC4ROWv[hits], fData2506->MOSAIC4COLv[hits]);
    }

    // reading timestamps
    uint64_t timestamp_m3 = ((uint64_t)fData2506->MOSAIC3T_HI << 32) | (fData2506->MOSAIC3T_LO);
    uint64_t timestamp_m4 = ((uint64_t)fData2506->MOSAIC4T_HI << 32) | (fData2506->MOSAIC4T_LO);
    new ((*fArray_TS)[fArray_TS->GetEntriesFast()]) R3BWRData(timestamp_m3, 3);
    new ((*fArray_TS)[fArray_TS->GetEntriesFast()]) R3BWRData(timestamp_m4, 4);

    return kTRUE;
}

bool R3BMosaicReader::R3BRead202507()
{
    // MOSAIC-3 corresponds to the first flex
    for (int hits = 0; hits < fData2507->MOSAIC3CHIP; hits++)
    {
        int fChipId = fData2507->MOSAIC3CHIPv[hits];

        int fAlpideId = fChipId + 1; // 1-base
        R3BLOG_IF(error, fAlpideId < 1, "Wrong fAlpideId: " << fAlpideId);

        new ((*fArray)[fArray->GetEntriesFast()])
            R3BAlpideMappedData(fAlpideId, 0, 1, fChipId, fData2507->MOSAIC3ROWv[hits], fData2507->MOSAIC3COLv[hits]);
    }

    // MOSAIC-4 corresponds to the second flex
    for (int hits = 0; hits < fData2507->MOSAIC4CHIP; hits++)
    {
        int fChipId = fData2507->MOSAIC4CHIPv[hits];

        int fAlpideId = fNb_sensors_flex + (fChipId + 1); // 1-base
        R3BLOG_IF(error, fAlpideId < 1, "Wrong fAlpideId: " << fAlpideId);

        new ((*fArray)[fArray->GetEntriesFast()])
            R3BAlpideMappedData(fAlpideId, 0, 2, fChipId, fData2507->MOSAIC4ROWv[hits], fData2507->MOSAIC4COLv[hits]);
    }

    // MOSAIC-5 corresponds to the 3rd flex
    for (int hits = 0; hits < fData2507->MOSAIC5CHIP; hits++)
    {
        int fChipId = fData2507->MOSAIC5CHIPv[hits];

        int fAlpideId = 2 * fNb_sensors_flex + (fChipId + 1); // 1-base
        R3BLOG_IF(error, fAlpideId < 1, "Wrong fAlpideId: " << fAlpideId);

        new ((*fArray)[fArray->GetEntriesFast()])
            R3BAlpideMappedData(fAlpideId, 0, 3, fChipId, fData2507->MOSAIC5ROWv[hits], fData2507->MOSAIC5COLv[hits]);
    }

    // MOSAIC-9 corresponds to the 4th flex
    for (int hits = 0; hits < fData2507->MOSAIC9CHIP; hits++)
    {
        int fChipId = fData2507->MOSAIC9CHIPv[hits];

        int fAlpideId = 3 * fNb_sensors_flex + (fChipId + 1); // 1-base
        R3BLOG_IF(error, fAlpideId < 1, "Wrong fAlpideId: " << fAlpideId);

        new ((*fArray)[fArray->GetEntriesFast()])
            R3BAlpideMappedData(fAlpideId, 0, 4, fChipId, fData2507->MOSAIC9ROWv[hits], fData2507->MOSAIC9COLv[hits]);
    }

    // MOSAIC-10 corresponds to the 5th flex
    for (int hits = 0; hits < fData2507->MOSAIC10CHIP; hits++)
    {
        int fChipId = fData2507->MOSAIC10CHIPv[hits];

        int fAlpideId = 4 * fNb_sensors_flex + (fChipId + 1); // 1-base
        R3BLOG_IF(error, fAlpideId < 1, "Wrong fAlpideId: " << fAlpideId);

        new ((*fArray)[fArray->GetEntriesFast()])
            R3BAlpideMappedData(fAlpideId, 0, 5, fChipId, fData2507->MOSAIC10ROWv[hits], fData2507->MOSAIC10COLv[hits]);
    }

    // reading timestamps
    uint64_t timestamp_m3 = ((uint64_t)fData2507->MOSAIC3T_HI << 32) | (fData2507->MOSAIC3T_LO);
    uint64_t timestamp_m4 = ((uint64_t)fData2507->MOSAIC4T_HI << 32) | (fData2507->MOSAIC4T_LO);
    uint64_t timestamp_m5 = ((uint64_t)fData2507->MOSAIC5T_HI << 32) | (fData2507->MOSAIC5T_LO);
    uint64_t timestamp_m9 = ((uint64_t)fData2507->MOSAIC9T_HI << 32) | (fData2507->MOSAIC9T_LO);
    uint64_t timestamp_m10 = ((uint64_t)fData2507->MOSAIC10T_HI << 32) | (fData2507->MOSAIC10T_LO);
    new ((*fArray_TS)[fArray_TS->GetEntriesFast()]) R3BWRData(timestamp_m3, 3);
    new ((*fArray_TS)[fArray_TS->GetEntriesFast()]) R3BWRData(timestamp_m4, 4);
    new ((*fArray_TS)[fArray_TS->GetEntriesFast()]) R3BWRData(timestamp_m5, 5);
    new ((*fArray_TS)[fArray_TS->GetEntriesFast()]) R3BWRData(timestamp_m9, 9);
    new ((*fArray_TS)[fArray_TS->GetEntriesFast()]) R3BWRData(timestamp_m10, 10);

    return kTRUE;
}

void R3BMosaicReader::Reset()
{
    // Reset the output array
    if (fArray)
    {
        fArray->Clear();
    }
    // Reset the output array
    if (fArray_TS)
    {
        fArray_TS->Clear();
    }
}

ClassImp(R3BMosaicReader)
