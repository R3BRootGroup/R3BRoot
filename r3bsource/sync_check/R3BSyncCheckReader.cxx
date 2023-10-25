/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
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
#include "R3BSyncCheckData.h"
#include "R3BSyncCheckReader.h"
#include "TClonesArray.h"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_sync_check.h"
}

R3BSyncCheckReader::R3BSyncCheckReader(EXT_STR_h101_SYNC_CHECK* data, size_t offset)
    : R3BReader("R3BSyncCheckReader")
    , fNEvent(1)
    , fData(data)
    , fOffset(offset)
    , fArray(new TClonesArray("R3BSyncCheckData"))
{
}

R3BSyncCheckReader::~R3BSyncCheckReader()
{
    if (fArray)
    {
        delete fArray;
    }
}

Bool_t R3BSyncCheckReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    R3BLOG(info, "");
    EXT_STR_h101_SYNC_CHECK_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_SYNC_CHECK, 0);
    if (!ok)
    {
        R3BLOG(fatal, "Failed to setup structure information");
        return kFALSE;
    }
    // Register output array in tree
    FairRootManager::Instance()->Register("SyncCheckData", "SyncCheck", fArray, kTRUE);
    Reset();
    memset(fData, 0, sizeof *fData);
    return kTRUE;
}

Bool_t R3BSyncCheckReader::R3BRead()
{
    new ((*fArray)[fArray->GetEntriesFast()]) R3BSyncCheckData(fData->SYNC_CHECK_MASTER,
                                                               fData->SYNC_CHECK_MASTERRR,
                                                               fData->SYNC_CHECK_MUSIC,
                                                               fData->SYNC_CHECK_RPC,
                                                               fData->SYNC_CHECK_STWO,
                                                               fData->SYNC_CHECK_FT1V,
                                                               fData->SYNC_CHECK_FT2V);
    fNEvent++;
    return kTRUE;
}

void R3BSyncCheckReader::Reset() { fArray->Clear(); }

ClassImp(R3BSyncCheckReader);
