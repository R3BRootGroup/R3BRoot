/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "FairLogger.h"
#include "FairRootManager.h"

#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BWRData.h"
#include "R3BWhiterabbitActafReader.h"

#include "TClonesArray.h"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_wractaf.h"
}

#define NB_ACTAF_DETS (sizeof fData->TIMESTAMP_ACTAF / sizeof fData->TIMESTAMP_ACTAF[0])

R3BWhiterabbitActafReader::R3BWhiterabbitActafReader(EXT_STR_h101_WRACTAF_onion* data,
                                                     size_t offset,
                                                     std::vector<UInt_t> whiterabbit_id)
    : R3BReader("R3BWhiterabbitActafReader")
    , fNEvent(0)
    , fData(data)
    , fOffset(offset)
    , fOnline(kFALSE)
    , fWhiterabbitId(whiterabbit_id)
    , fEventHeader(nullptr)
    , fArray(new TClonesArray("R3BWRData"))
{
}

R3BWhiterabbitActafReader::~R3BWhiterabbitActafReader()
{
    if (fArray)
    {
        delete fArray;
    }
}

Bool_t R3BWhiterabbitActafReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    R3BLOG(info, "");
    EXT_STR_h101_WRACTAF_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_WRACTAF, 0);

    if (!ok)
    {
        R3BLOG(error, "Failed to setup structure information");
        return kFALSE;
    }

    // Look for the R3BEventHeader
    FairRootManager* frm = FairRootManager::Instance();
    fEventHeader = dynamic_cast<R3BEventHeader*>(frm->GetObject("EventHeader."));
    if (!fEventHeader)
    {
        R3BLOG(warn, "EventHeader. not found");
        fEventHeader = dynamic_cast<R3BEventHeader*>(frm->GetObject("R3BEventHeader"));
    }
    else
    {
        R3BLOG(info, "R3BEventHeader found");
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("WRActafData", "WRActaf", fArray, !fOnline);
    Reset();

    return kTRUE;
}

Bool_t R3BWhiterabbitActafReader::R3BRead()
{
    for (uint32_t d = 0; d < NB_ACTAF_DETS; d++)
    {
        if (fData->TIMESTAMP_ACTAF[d].ID && fData->TIMESTAMP_ACTAF[d].ID != fWhiterabbitId[d])
        {
            char strMessage[1000];
            snprintf(strMessage,
                     sizeof strMessage,
                     "Event %lu: Whiterabbit ID mismatch for det=%u: expected 0x%x, got 0x%x.\n",
                     fEventHeader->GetEventno(),
                     d,
                     fWhiterabbitId[d],
                     fData->TIMESTAMP_ACTAF[d].ID);
            LOG(error) << strMessage;
        }
        if (fEventHeader != nullptr)
        {
            uint64_t timestamp = ((uint64_t)fData->TIMESTAMP_ACTAF[d].WR_T[3] << 48) |
                                 ((uint64_t)fData->TIMESTAMP_ACTAF[d].WR_T[2] << 32) |
                                 ((uint64_t)fData->TIMESTAMP_ACTAF[d].WR_T[1] << 16) |
                                 (uint64_t)fData->TIMESTAMP_ACTAF[d].WR_T[0];
            fNEvent = fEventHeader->GetEventno();
            new ((*fArray)[fArray->GetEntriesFast()]) R3BWRData(timestamp, d + 1);
        }
    }

    return kTRUE;
}

void R3BWhiterabbitActafReader::Reset()
{
    // Reset the output array
    fArray->Clear();
    fNEvent = 0;
    for (uint32_t d = 0; d < NB_ACTAF_DETS; d++)
    {
        fData->TIMESTAMP_ACTAF[d].ID = 0;
    }
}

ClassImp(R3BWhiterabbitActafReader);
