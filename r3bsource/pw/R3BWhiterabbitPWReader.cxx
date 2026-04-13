/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                          *
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

#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BWRData.h"
#include "R3BWhiterabbitPWReader.h"

#include <TClonesArray.h>

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_wrpw.h"
}

R3BWhiterabbitPWReader::R3BWhiterabbitPWReader(EXT_STR_h101_WRPW_onion* data, size_t offset, UInt_t whiterabbit_id)
    : R3BReader("R3BWhiterabbitPWReader")
    , fData(data)
    , fOffset(offset)
    , fWhiterabbitId(whiterabbit_id)
    , fArray(std::make_unique<TClonesArray>("R3BWRData"))
{
}

Bool_t R3BWhiterabbitPWReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t okey = 0;
    R3BLOG(info, "");
    EXT_STR_h101_WRPW_ITEMS_INFO(okey, *a_struct_info, fOffset, EXT_STR_h101_WRPW, 0);

    if (!okey)
    {
        R3BLOG(error, "Failed to setup structure information");
        return kFALSE;
    }

    // Look for the R3BEventHeader
    auto* frm = FairRootManager::Instance();
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
    FairRootManager::Instance()->Register("WRPWData", "WRPW", fArray.get(), !fOnline);
    Reset();

    return kTRUE;
}

Bool_t R3BWhiterabbitPWReader::R3BRead()
{
    if (fData->TIMESTAMP_PW_ID && fData->TIMESTAMP_PW_ID != fWhiterabbitId)
    {
        TString strMessage = Form("Event %lu: Whiterabbit ID mismatch for PW: expected 0x%x, got 0x%x.\n",
                                  fEventHeader->GetEventno(),
                                  fWhiterabbitId,
                                  fData->TIMESTAMP_PW_ID);
        LOG(error) << strMessage;
    }
    if (fEventHeader != nullptr)
    {
        UInt_t id = fData->TIMESTAMP_PW_ID;
        uint64_t timestamp = ((uint64_t)fData->TIMESTAMP_PW_WR_T[3] << 48) |
                             ((uint64_t)fData->TIMESTAMP_PW_WR_T[2] << 32) |
                             ((uint64_t)fData->TIMESTAMP_PW_WR_T[1] << 16) | (uint64_t)fData->TIMESTAMP_PW_WR_T[0];
        fNEvent = fEventHeader->GetEventno();
        new ((*fArray)[fArray->GetEntriesFast()]) R3BWRData(timestamp, id);
    }

    return kTRUE;
}

void R3BWhiterabbitPWReader::Reset()
{
    // Reset the output array
    fArray->Clear();
    fData->TIMESTAMP_PW_ID = 0;
}

ClassImp(R3BWhiterabbitPWReader)
