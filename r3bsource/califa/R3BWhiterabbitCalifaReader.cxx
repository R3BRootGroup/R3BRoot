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

#include <FairRootManager.h>

#include <TClonesArray.h>

#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BWRData.h"
#include "R3BWhiterabbitCalifaReader.h"

/**
 ** ext_h101_wrcalifa.h was created by running
 ** $unpacker --ntuple=STRUCT_HH,RAW:TIMESTAMP_CALIFA,id=h101_WRCALIFA,NOTRIGEVENTNO,ext_h101_wrcalifa.h
 **/

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_wrcalifa.h"
}

R3BWhiterabbitCalifaReader::R3BWhiterabbitCalifaReader(EXT_STR_h101_WRCALIFA* data,
                                                       size_t offset,
                                                       uint32_t whiterabbit_id1,
                                                       uint32_t whiterabbit_id2,
                                                       uint32_t whiterabbit_id3,
                                                       uint32_t whiterabbit_id4)
    : R3BReader("R3BWhiterabbitCalifaReader")
    , fData(data)
    , fOffset(offset)
    , fWhiterabbitId1(whiterabbit_id1)
    , fWhiterabbitId2(whiterabbit_id2)
    , fWhiterabbitId3(whiterabbit_id3)
    , fWhiterabbitId4(whiterabbit_id4)
    , fEventHeader(nullptr)
    , fArray(new TClonesArray("R3BWRData"))
{
}

R3BWhiterabbitCalifaReader::~R3BWhiterabbitCalifaReader()
{
    if (fArray)
    {
        delete fArray;
    }
}

Bool_t R3BWhiterabbitCalifaReader::Init(ext_data_struct_info* a_struct_info)
{
    int okay;
    R3BLOG(info, "");
    EXT_STR_h101_WRCALIFA_ITEMS_INFO(okay, *a_struct_info, fOffset, EXT_STR_h101_WRCALIFA, 0);

    if (!okay)
    {
        R3BLOG(warn, "Failed to setup structure information");
        return kFALSE;
    }

    // Look for the R3BEventHeader
    FairRootManager* frm = FairRootManager::Instance();
    fEventHeader = dynamic_cast<R3BEventHeader*>(frm->GetObject("EventHeader."));
    R3BLOG(info, "R3BEventHeader found");

    // Register output array in tree
    FairRootManager::Instance()->Register("WRCalifaData", "WRCalifa", fArray, !fOnline);

    fData->TIMESTAMP_CALIFA1ID = 0;
    fData->TIMESTAMP_CALIFA2ID = 0;
    fData->TIMESTAMP_CALIFA3ID = 0;
    fData->TIMESTAMP_CALIFA4ID = 0;
    return kTRUE;
}

Bool_t R3BWhiterabbitCalifaReader::R3BRead()
{
    /*printf("califa %08x %08x %08x %08x %08x \n",
    fData->TIMESTAMP_CALIFA_ID,
    fData->TIMESTAMP_CALIFA_WR_T4, fData->TIMESTAMP_CALIFA_WR_T3,
    fData->TIMESTAMP_CALIFA_WR_T2, fData->TIMESTAMP_CALIFA_WR_T1);*/

    if (fWhiterabbitId1 != fData->TIMESTAMP_CALIFA1ID && fData->TIMESTAMP_CALIFA1ID > 0)
    {
        char strMessage[1000];
        snprintf(strMessage,
                 sizeof strMessage,
                 "Event %lu: Whiterabbit ID mismatch: expected 0x%x, got 0x%x.\n",
                 fEventHeader->GetEventno(),
                 fWhiterabbitId1,
                 fData->TIMESTAMP_CALIFA1ID);
        LOG(error) << strMessage;
    }

    if (fWhiterabbitId2 != fData->TIMESTAMP_CALIFA2ID && fData->TIMESTAMP_CALIFA2ID > 0)
    {
        char strMessage[1000];
        snprintf(strMessage,
                 sizeof strMessage,
                 "Event %lu: Whiterabbit ID mismatch: expected 0x%x, got 0x%x.\n",
                 fEventHeader->GetEventno(),
                 fWhiterabbitId2,
                 fData->TIMESTAMP_CALIFA2ID);
        LOG(error) << strMessage;
    }

    if (fWhiterabbitId3 != fData->TIMESTAMP_CALIFA3ID && fData->TIMESTAMP_CALIFA3ID > 0)
    {
        char strMessage[1000];
        snprintf(strMessage,
                 sizeof strMessage,
                 "Event %lu: Whiterabbit ID mismatch: expected 0x%x, got 0x%x.\n",
                 fEventHeader->GetEventno(),
                 fWhiterabbitId3,
                 fData->TIMESTAMP_CALIFA3ID);
        LOG(error) << strMessage;
    }

    if (fWhiterabbitId4 != fData->TIMESTAMP_CALIFA4ID && fData->TIMESTAMP_CALIFA4ID > 0)
    {
        char strMessage[1000];
        snprintf(strMessage,
                 sizeof strMessage,
                 "Event %lu: Whiterabbit ID mismatch: expected 0x%x, got 0x%x.\n",
                 fEventHeader->GetEventno(),
                 fWhiterabbitId4,
                 fData->TIMESTAMP_CALIFA4ID);
        LOG(error) << strMessage;
    }

    if (fEventHeader != nullptr)
    {
        fNEvent = fEventHeader->GetEventno();
        uint64_t timestamp1 = ((uint64_t)fData->TIMESTAMP_CALIFA1WR_T4 << 48) |
                              ((uint64_t)fData->TIMESTAMP_CALIFA1WR_T3 << 32) |
                              ((uint64_t)fData->TIMESTAMP_CALIFA1WR_T2 << 16) | (uint64_t)fData->TIMESTAMP_CALIFA1WR_T1;
        if (timestamp1 > 0)
            new ((*fArray)[fArray->GetEntriesFast()]) R3BWRData(timestamp1, fWhiterabbitId1);

        uint64_t timestamp2 = ((uint64_t)fData->TIMESTAMP_CALIFA2WR_T4 << 48) |
                              ((uint64_t)fData->TIMESTAMP_CALIFA2WR_T3 << 32) |
                              ((uint64_t)fData->TIMESTAMP_CALIFA2WR_T2 << 16) | (uint64_t)fData->TIMESTAMP_CALIFA2WR_T1;
        if (timestamp2 > 0)
            new ((*fArray)[fArray->GetEntriesFast()]) R3BWRData(timestamp2, fWhiterabbitId2);

        uint64_t timestamp3 = ((uint64_t)fData->TIMESTAMP_CALIFA3WR_T4 << 48) |
                              ((uint64_t)fData->TIMESTAMP_CALIFA3WR_T3 << 32) |
                              ((uint64_t)fData->TIMESTAMP_CALIFA3WR_T2 << 16) | (uint64_t)fData->TIMESTAMP_CALIFA3WR_T1;
        if (timestamp3 > 0)
            new ((*fArray)[fArray->GetEntriesFast()]) R3BWRData(timestamp3, fWhiterabbitId3);

        uint64_t timestamp4 = ((uint64_t)fData->TIMESTAMP_CALIFA4WR_T4 << 48) |
                              ((uint64_t)fData->TIMESTAMP_CALIFA4WR_T3 << 32) |
                              ((uint64_t)fData->TIMESTAMP_CALIFA4WR_T2 << 16) | (uint64_t)fData->TIMESTAMP_CALIFA4WR_T1;
        if (timestamp4 > 0)
            new ((*fArray)[fArray->GetEntriesFast()]) R3BWRData(timestamp4, fWhiterabbitId4);
    }
    else
    {
        fNEvent++;
    }

    fData->TIMESTAMP_CALIFA1ID = 0;
    fData->TIMESTAMP_CALIFA2ID = 0;
    fData->TIMESTAMP_CALIFA3ID = 0;
    fData->TIMESTAMP_CALIFA4ID = 0;
    return kTRUE;
}

void R3BWhiterabbitCalifaReader::Reset()
{
    // Reset the output array
    fArray->Clear();
    fNEvent = 0;
}

ClassImp(R3BWhiterabbitCalifaReader)
