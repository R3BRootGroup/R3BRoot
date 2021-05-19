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

#include "R3BWhiterabbitAmsReader.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "R3BEventHeader.h"
#include "R3BWRAmsData.h"
#include "TClonesArray.h"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_wrams.h"
}

R3BWhiterabbitAmsReader::R3BWhiterabbitAmsReader(EXT_STR_h101_WRAMS* data, UInt_t offset, UInt_t whiterabbit_id)
    : R3BReader("R3BWhiterabbitAmsReader")
    , fNEvent(0)
    , fData(data)
    , fOffset(offset)
    , fOnline(kFALSE)
    , fLogger(FairLogger::GetLogger())
    , fWhiterabbitId(whiterabbit_id)
    , fEventHeader(nullptr)
    , fArray(new TClonesArray("R3BWRAmsData"))
{
}

R3BWhiterabbitAmsReader::~R3BWhiterabbitAmsReader()
{
    if (fArray)
    {
        delete fArray;
    }
    if (fEventHeader)
        delete fEventHeader;
}

Bool_t R3BWhiterabbitAmsReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    LOG(INFO) << "R3BWhiterabbitAmsReader::Init";
    EXT_STR_h101_WRAMS_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_WRAMS, 0);

    if (!ok)
    {
        LOG(ERROR) << "R3BWhiterabbitAmsReader::Failed to setup structure information.";
        return kFALSE;
    }

    // Look for the R3BEventHeader
    FairRootManager* frm = FairRootManager::Instance();
    fEventHeader = (R3BEventHeader*)frm->GetObject("EventHeader.");
    if (!fEventHeader)
    {
        LOG(WARNING) << "R3BWhiterabbitAmsReader::Init() R3BEventHeader not found";
    }
    else
        LOG(INFO) << "R3BWhiterabbitAmsReader::Init() R3BEventHeader found";

    // Register output array in tree
    if (!fOnline)
    {
        FairRootManager::Instance()->Register("WRAmsData", "WRAms", fArray, kTRUE);
    }
    else
    {
        FairRootManager::Instance()->Register("WRAmsData", "WRAms", fArray, kFALSE);
    }

    fData->TIMESTAMP_AMS_ID = 0;

    return kTRUE;
}

Bool_t R3BWhiterabbitAmsReader::Read()
{
    if (!fData->TIMESTAMP_AMS_ID)
    {
        return kTRUE;
    }
    /*printf("ams %08x %08x %08x %08x %08x \n",
    fData->TIMESTAMP_AMS_ID,
    fData->TIMESTAMP_AMS_WR_T4, fData->TIMESTAMP_AMS_WR_T3,
    fData->TIMESTAMP_AMS_WR_T2, fData->TIMESTAMP_AMS_WR_T1);*/

    if (fWhiterabbitId != fData->TIMESTAMP_AMS_ID)
    {
        char strMessage[1000];
        snprintf(strMessage,
                 sizeof strMessage,
                 "Event %u: Whiterabbit ID mismatch: expected 0x%x, got 0x%x.\n",
                 fEventHeader->GetEventno(),
                 fWhiterabbitId,
                 fData->TIMESTAMP_AMS_ID);
        LOG(error) << strMessage;
    }

    if (fEventHeader != nullptr)
    {
        uint64_t timestamp = ((uint64_t)fData->TIMESTAMP_AMS_WR_T4 << 48) |
                             ((uint64_t)fData->TIMESTAMP_AMS_WR_T3 << 32) |
                             ((uint64_t)fData->TIMESTAMP_AMS_WR_T2 << 16) | (uint64_t)fData->TIMESTAMP_AMS_WR_T1;
        // fEventHeader->SetTimeStamp(timestamp);
        fNEvent = fEventHeader->GetEventno();
        new ((*fArray)[fArray->GetEntriesFast()]) R3BWRAmsData(timestamp);
    }
    else
    {
        fNEvent++;
    }

    fData->TIMESTAMP_AMS_ID = 0;
    return kTRUE;
}

void R3BWhiterabbitAmsReader::Reset()
{
    // Reset the output array
    fArray->Clear();
    fNEvent = 0;
}

ClassImp(R3BWhiterabbitAmsReader)
