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

#include "R3BTofiReader.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BTofiMappedData.h"
#include "TClonesArray.h"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_tofi.h"
}

// #define MAX_TOFI_CARDS (sizeof data->TOFI_TRIGCLI / sizeof data->TOFI_TRIGCLI[0])
#define MAX_TOFI_PLANES 1
#define MAX_TOFI_PADDLES 24

R3BTofiReader::R3BTofiReader(EXT_STR_h101_TOFI* data, UInt_t offset)
    : R3BReader("R3BTofiReader")
    , fData(data)
    , fOffset(offset)
    , fLogger(FairLogger::GetLogger())
    , fArray(new TClonesArray("R3BTofiMappedData"))
    , fArrayTrigger(new TClonesArray("R3BTofiMappedData"))
{
}

R3BTofiReader::~R3BTofiReader()
{
    if (fArray)
    {
        delete fArray;
    }
    if (fArrayTrigger)
    {
        delete fArrayTrigger;
    }
}

Bool_t R3BTofiReader::Init(ext_data_struct_info* a_struct_info)
{
    int ok;
    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(error) << "FairRootManager not found";

    header = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    if (header)
        R3BLOG(info, "EventHeader. was found");
    else
        R3BLOG(info, "EventHeader. was not found");

    EXT_STR_h101_TOFI_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_TOFI, 0);

    if (!ok)
    {
        perror("ext_data_struct_info_item");
        LOG(error) << "Failed to setup structure information.";
        return kFALSE;
    }
    // Register output array in tree
    FairRootManager::Instance()->Register("TofiMapped", "Land", fArray, kTRUE);
    FairRootManager::Instance()->Register("TofiTriggerMapped", "Land", fArrayTrigger, kTRUE);

    // initial clear (set number of hits to 0)
    EXT_STR_h101_TOFI_onion* data = (EXT_STR_h101_TOFI_onion*)fData;
    memset(data, 0, sizeof *data);

    return kTRUE;
}

Bool_t R3BTofiReader::Read()
{
    // Convert plain raw data to multi-dimensional array
    EXT_STR_h101_TOFI_onion* data = (EXT_STR_h101_TOFI_onion*)fData;

    // puts("NEW Event");
    // Leading.
    for (uint32_t t = 0; t < 2; t++)
    {
        auto const& side = data->TOFI_T[t];

        uint32_t j = 0;
        for (uint32_t i = 0; i < side.TCLM; i++)
        {
            uint32_t ch_i = side.TCLMI[i];
            uint32_t end = side.TCLME[i];
            for (; j < end; j++)
            {
                new ((*fArray)[fArray->GetEntriesFast()])
                    R3BTofiMappedData(1, t + 1, ch_i, 1, side.TCLv[j], side.TFLv[j]);

                //         std::cout<<"TOFI READER leading: "<<t + 1<<", "<< ch_i<<", "<<  side.TCLv[j]<<", "<<
                //          side.TFLv[j]<<std::endl;
            }
        }
    }

    // Trailing.
    for (uint32_t t = 0; t < 2; t++)
    {
        auto const& side = data->TOFI_T[t];

        uint32_t j = 0;
        for (uint32_t i = 0; i < side.TCLM; i++)
        {
            uint32_t ch_i = side.TCLMI[i];
            uint32_t end = side.TCLME[i];
            for (; j < end; j++)
            {
                new ((*fArray)[fArray->GetEntriesFast()])
                    R3BTofiMappedData(1, t + 1, ch_i, 2, side.TCTv[j], side.TFTv[j]);

                //      std::cout<<"TOFI READER trailing: "<<t + 1<<", "<< ch_i<<", "<<  side.TCTv[j]<<", "<<
                //      side.TFTv[j]<<std::endl;
            }
        }
    }

    // Triggers.
    {
        for (uint32_t i = 0; i < data->TOFI_TRIGFL; i++)
        {
            uint32_t ch_i = data->TOFI_TRIGFLI[i];
            new ((*fArrayTrigger)[fArrayTrigger->GetEntriesFast()])
                R3BTofiMappedData(MAX_TOFI_PLANES + 1, 1, ch_i, 1, data->TOFI_TRIGCLv[i], data->TOFI_TRIGFLv[i]);
        }
    }

    return kTRUE;
}

void R3BTofiReader::Reset()
{
    // Reset the output array
    fArray->Clear();
    fArrayTrigger->Clear();
}

ClassImp(R3BTofiReader)
