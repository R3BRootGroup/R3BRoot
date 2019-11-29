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

#include "R3BTofdReader.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "R3BTofdMappedData.h"
#include "TClonesArray.h"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_tofd.h"
}

//#define MAX_TOFD_PLANES   6
#define MAX_TOFD_PLANES (sizeof data->TOFD_P / sizeof data->TOFD_P[0])

R3BTofdReader::R3BTofdReader(EXT_STR_h101_TOFD* data, UInt_t offset)
    : R3BReader("R3BTofdReader")
    , fData(data)
    , fOffset(offset)
    , fLogger(FairLogger::GetLogger())
    , fArray(new TClonesArray("R3BTofdMappedData"))
{
}

R3BTofdReader::~R3BTofdReader() {}

Bool_t R3BTofdReader::Init(ext_data_struct_info* a_struct_info)
{
    int ok;

    EXT_STR_h101_TOFD_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_TOFD, 0);

    if (!ok)
    {
        perror("ext_data_struct_info_item");
        LOG(error) << "Failed to setup structure information.";
        return kFALSE;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("TofdMapped", "Land", fArray, kTRUE);

    // initial clear (set number of hits to 0)
    EXT_STR_h101_TOFD_onion* data = (EXT_STR_h101_TOFD_onion*)fData;
    for (int d = 0; d < MAX_TOFD_PLANES; d++)
    {
        for (int t = 0; t < 2; t++)
        {
            data->TOFD_P[d].T[t].TFLM = 0;
            data->TOFD_P[d].T[t].TFTM = 0;
        }
    }

    return kTRUE;
}

Bool_t R3BTofdReader::Read()
{
    // Convert plain raw data to multi-dimensional array
    EXT_STR_h101_TOFD_onion* data = (EXT_STR_h101_TOFD_onion*)fData;

    for (uint32_t d = 0; d < MAX_TOFD_PLANES; d++)
    {
        for (uint32_t t = 0; t < 2; t++)
        {
            auto const& side = data->TOFD_P[d].T[t];

            //
            // TAMEX3.
            //

            // Leading.
            auto numChannels = side.TCLM;
            uint32_t curChannelStart = 0;
            for (uint32_t i = 0; i < numChannels; i++)
            {
                uint32_t channel = side.TCLMI[i];
                uint32_t nextChannelStart = side.TCLME[i];
                for (uint32_t j = curChannelStart; j < nextChannelStart; j++)
                {
                    new ((*fArray)[fArray->GetEntriesFast()])
                        R3BTofdMappedData(d + 1, t + 1, channel, 1, side.TCLv[j], side.TFLv[j]);
                }
                curChannelStart = nextChannelStart;
            }

            // Trailing.
            numChannels = side.TCTM;
            curChannelStart = 0;
            for (uint32_t i = 0; i < numChannels; i++)
            {
                uint32_t channel = side.TCTMI[i];
                uint32_t nextChannelStart = side.TCTME[i];
                for (uint32_t j = curChannelStart; j < nextChannelStart; j++)
                {
                    new ((*fArray)[fArray->GetEntriesFast()])
                        R3BTofdMappedData(d + 1, t + 1, channel, 2, side.TCTv[j], side.TFTv[j]);
                }
                curChannelStart = nextChannelStart;
            }

        } // for side
    }     // for planes

    return kTRUE;
}

void R3BTofdReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

ClassImp(R3BTofdReader)
