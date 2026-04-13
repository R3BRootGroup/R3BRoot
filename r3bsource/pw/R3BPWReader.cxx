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

#include "R3BPWReader.h"
#include "R3BLogger.h"
#include "R3BPWMappedData.h"
#include <FairRootManager.h>

#include <TClonesArray.h>
#include <byteswap.h>
extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_pw.h"
}

R3BPWReader::R3BPWReader(EXT_STR_h101_PW* data, size_t offset)
    : R3BReader("R3BPWReader")
    , fData(data)
    , fArrayPW(std::make_unique<TClonesArray>("R3BPWMappedData"))
    , fOffset(offset)
{
}

Bool_t R3BPWReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t okey = 0;
    R3BLOG(info, "");
    EXT_STR_h101_PW_ITEMS_INFO(okey, *a_struct_info, fOffset, EXT_STR_h101_PW, 0);
    if (!okey)
    {
        R3BLOG(error, "Failed to setup structure information.");
        return kFALSE;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("R3BPWMappedData", "PW Mapped Data", fArrayPW.get(), !fOnline);
    Reset();
    memset(fData, 0, sizeof *fData);

    return kTRUE;
}

Bool_t R3BPWReader::R3BRead()
{
    R3BLOG(debug1, "Event data.");

    uint64_t Fine_time;
    uint64_t Coarse_time;
    uint64_t Epoch;
    UShort_t DetId;
    UShort_t channelNumber;
    UShort_t Edge;

    for (int i = 0; i < fData->PW_FTM; i++)
    {
        int begin = 0;

        if (i != 0)
        {
            begin = fData->PW_FTME[i - 1];
        }

        for (int t = begin; t < fData->PW_FTME[i]; t++)
        {
            DetId = 0;
            channelNumber = fData->PW_FTMI[i];
            Fine_time = fData->PW_FTv[t];
            Coarse_time = fData->PW_CTv[t];
            Edge = fData->PW_Ev[t];
            Epoch = bswap_32(fData->PW_EPOCHv[t]) & 0x0FFFFFFF;

            new ((*fArrayPW)[fArrayPW->GetEntriesFast()])
                R3BPWMappedData(DetId, channelNumber, Fine_time, Coarse_time, Edge, Epoch);
        }
    }

    // -------------------- Filling REF data ----------------------
    for (int i = 0; i < fData->PW_REF_FTM; i++)
    {
        int begin = 0;

        if (i != 0)
        {
            begin = fData->PW_REF_FTME[i - 1];
        }

        for (int t = begin; t < fData->PW_REF_FTME[i]; t++)
        {

            DetId = 1;
            channelNumber = fData->PW_REF_FTMI[i];
            Fine_time = fData->PW_REF_FTv[t];
            Coarse_time = fData->PW_REF_CTv[t];
            Epoch = bswap_32(fData->PW_REF_EPOCHv[t]) & 0x0FFFFFFF;

            new ((*fArrayPW)[fArrayPW->GetEntriesFast()])
                R3BPWMappedData(DetId, channelNumber, Fine_time, Coarse_time, 0, Epoch);
        }
    }

    fNEvent += 1;
    return kTRUE;
}

void R3BPWReader::Reset()
{
    // Reset the output array
    fArrayPW->Clear();
}

ClassImp(R3BPWReader)
