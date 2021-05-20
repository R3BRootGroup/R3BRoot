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
        LOG(ERROR) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

    EXT_STR_h101_TOFI_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_TOFI, 0);

    if (!ok)
    {
        perror("ext_data_struct_info_item");
        LOG(error) << "Failed to setup structure information.";
        return kFALSE;
    }
    puts("1");
    // Register output array in tree
    FairRootManager::Instance()->Register("TofiMapped", "Land", fArray, kTRUE);
    FairRootManager::Instance()->Register("TofiTriggerMapped", "Land", fArrayTrigger, kTRUE);
    puts("2");

    // initial clear (set number of hits to 0)
    EXT_STR_h101_TOFI_onion* data = (EXT_STR_h101_TOFI_onion*)fData;

    for (int p = 0; p < 24; p++)
    {
        for (int t = 0; t < 2; t++) // tube
        {
            data->TOFI_T[t].LC[p] = 0;
            data->TOFI_T[t].LF[p] = 0;
            data->TOFI_T[t].TC[p] = 0;
            data->TOFI_T[t].TF[p] = 0;
        }
    }

    return kTRUE;
}

Bool_t R3BTofiReader::Read()
{
    // Convert plain raw data to multi-dimensional array
    EXT_STR_h101_TOFI_onion* data = (EXT_STR_h101_TOFI_onion*)fData;

    // puts("NEW Event");
    for (uint32_t d = 0; d < MAX_TOFI_PADDLES; d++)
    {
        for (uint32_t t = 0; t < 2; t++)
        {
            auto const& side = data->TOFI_T[t];
            // Leading.

            //    if(side.LC[d] > 0) std::cout<<"LEADING: "<<d+1<<", "<<t+1<<", "<<side.LC[d]<<",
            //    "<<side.LF[d]<<std::endl;
            if (side.LC[d] > 0)
                new ((*fArray)[fArray->GetEntriesFast()]) R3BTofiMappedData(1, t + 1, d + 1, 1, side.LC[d], side.LF[d]);
        }
    }

    for (uint32_t d = 0; d < MAX_TOFI_PADDLES; d++)
    {
        for (uint32_t t = 0; t < 2; t++)
        {
            auto const& side = data->TOFI_T[t];
            // Trailing.
            //      if(side.TC[d] > 0)   std::cout<<"Trailing: "<<d+1<<", "<<t+1<<", "<<side.TC[d]<<",
            //      "<<side.TF[d]<<std::endl;
            if (side.TC[d] > 0)
                new ((*fArray)[fArray->GetEntriesFast()]) R3BTofiMappedData(1, t + 1, d + 1, 2, side.TC[d], side.TF[d]);
        }
    }

    // Leading TAMEX trigger times.
    {
        auto numChannels = data->TOFI_TRIGFL;
        for (uint32_t i = 0; i < numChannels; i++)
        {
            uint32_t channel = data->TOFI_TRIGFLI[i];
            //    std::cout<<"Tigger: "<<channel<<", "<<i<<", "<< data->TOFI_TRIGCLv[i]<<",
            //    "<<data->TOFI_TRIGFLv[i]<<std::endl;
            new ((*fArrayTrigger)[fArrayTrigger->GetEntriesFast()])
                R3BTofiMappedData(MAX_TOFI_PLANES + 1, 1, channel, 1, data->TOFI_TRIGCLv[i], data->TOFI_TRIGFLv[i]);
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
