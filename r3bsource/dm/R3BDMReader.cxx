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

#include "R3BDMReader.h"
#include "R3BDMMappedData.h"
#include "R3BLogger.h"
#include <FairRootManager.h>

#include <TClonesArray.h>
#include <byteswap.h>
extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_dm.h"
}

R3BDMReader::R3BDMReader(EXT_STR_h101_DM* data, size_t offset)
    : R3BReader("R3BDMReader")
    , fData(data)
    , fArrayDM(std::make_unique<TClonesArray>("R3BDMMappedData"))
    , fOffset(offset)
{
}

Bool_t R3BDMReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t okey = 0;
    R3BLOG(info, "");
    EXT_STR_h101_DM_ITEMS_INFO(okey, *a_struct_info, fOffset, EXT_STR_h101_DM, 0);
    if (!okey)
    {
        R3BLOG(error, "Failed to setup structure information.");
        return kFALSE;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("R3BDMMappedData", "DM Mapped Data", fArrayDM.get(), !fOnline);
    Reset();
    memset(fData, 0, sizeof *fData);

    return kTRUE;
}

Bool_t R3BDMReader::R3BRead()
{
    R3BLOG(debug1, "Event data.");
    uint64_t Fine_time;
    uint64_t Coarse_time;
    uint64_t Epoch;
    UShort_t DetId;
    UShort_t channelNumber;
    UShort_t Edge;
    Int_t TDC;

    for (int i = 0; i < fData->DM_FTM; i++)
    {
        int begin = 0;

        if (i != 0)
        {
            begin = fData->DM_FTME[i - 1];
        }

        for (int t = begin; t < fData->DM_FTME[i]; t++)
        {
            channelNumber = fData->DM_FTMI[i];
            Fine_time = fData->DM_FTv[t];
            Coarse_time = fData->DM_CTv[t];
            Edge = fData->DM_Ev[t];
            Epoch = bswap_32(fData->DM_EPOCHv[t]) & 0x0FFFFFFF;
            if (channelNumber < 19)
            {
                TDC = 0;
                DetId = 1;
            }
            if (channelNumber >= 19 && channelNumber < 37)
            {
                TDC = 1;
                DetId = 1;
            }
            if (channelNumber >= 37 && channelNumber < 55)
            {
                TDC = 2;
                DetId = 2;
            }
            if (channelNumber >= 55)
            {
                TDC = 3;
                DetId = 2;
            }
            new ((*fArrayDM)[fArrayDM->GetEntriesFast()])
                R3BDMMappedData(DetId, channelNumber, Fine_time, Coarse_time, Edge, Epoch, TDC);
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

            DetId = 0;
            channelNumber = fData->PW_REF_FTMI[i];
            Fine_time = fData->PW_REF_FTv[t];
            Coarse_time = fData->PW_REF_CTv[t];
            Epoch = bswap_32(fData->PW_REF_EPOCHv[t]) & 0x0FFFFFFF;
            TDC = channelNumber - 2;
            if (TDC >= 0 && TDC < 4)
            {
                new ((*fArrayDM)[fArrayDM->GetEntriesFast()])
                    R3BDMMappedData(DetId, channelNumber, Fine_time, Coarse_time, 0, Epoch, TDC);
            }
        }
    }

    fNEvent += 1;
    return kTRUE;
}

void R3BDMReader::Reset()
{
    // Reset the output array
    fArrayDM->Clear();
}

ClassImp(R3BDMReader)
