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

#include "R3BHFIBReader.h"
#include "R3BHFIBMappedData.h"
#include "R3BLogger.h"
#include <FairRootManager.h>

#include <TClonesArray.h>
#include <byteswap.h>
extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_hfib.h"
}

R3BHFIBReader::R3BHFIBReader(EXT_STR_h101_HFIB* data, size_t offset)
    : R3BReader("R3BHFIBReader")
    , fData(data)
    , fArrayHFIB(std::make_unique<TClonesArray>("R3BHFIBMappedData"))
    , fOffset(offset)
{
}

Bool_t R3BHFIBReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t okey = 0;
    R3BLOG(info, "");
    EXT_STR_h101_HFIB_ITEMS_INFO(okey, *a_struct_info, fOffset, EXT_STR_h101_HFIB, 0);
    if (!okey)
    {
        R3BLOG(error, "Failed to setup structure information.");
        return kFALSE;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("R3BHFIBMappedData", "HFIB Mapped Data", fArrayHFIB.get(), !fOnline);
    Reset();
    memset(fData, 0, sizeof *fData);

    return kTRUE;
}

Bool_t R3BHFIBReader::R3BRead()
{
    R3BLOG(debug1, "Event data.");

    uint64_t Fine_time;
    uint64_t Coarse_time;
    uint64_t Epoch;
    UShort_t DetId;
    UShort_t channelNumber;
    UShort_t Edge;
    Int_t TDC;

    auto* data = reinterpret_cast<EXT_STR_h101_HFIB_onion*>(fData);
    // loop over 8 TDCs
    for (int T = 0; T < 8; T++)
    {
        auto const& tdc = data->HFIB_TDC[T];
        for (int i = 0; i < tdc.FTM; i++)
        {
            int begin = 0;

            if (i != 0)
            {
                begin = tdc.FTME[i - 1];
            }

            for (int t = begin; t < tdc.FTME[i]; t++)
            {
                DetId = 1;
                channelNumber = tdc.FTMI[i];
                Fine_time = tdc.FTv[t];
                Coarse_time = tdc.CTv[t];
                Edge = tdc.Ev[t];
                TDC = T + 1;
                Epoch = bswap_32(tdc.EPOCHv[t]) & 0x0FFFFFFF;

                new ((*fArrayHFIB)[fArrayHFIB->GetEntriesFast()])
                    R3BHFIBMappedData(DetId, TDC, channelNumber, Fine_time, Coarse_time, Edge, Epoch);
            }
        }
    }

    // -------------------- Filling REF data ----------------------
    for (int i = 0; i < fData->HFIB_REF_FTM; i++)
    {
        int begin = 0;

        if (i != 0)
        {
            begin = fData->HFIB_REF_FTME[i - 1];
        }

        for (int t = begin; t < fData->HFIB_REF_FTME[i]; t++)
        {

            DetId = 0;
            channelNumber = fData->HFIB_REF_FTMI[i];
            TDC = channelNumber;
            Fine_time = fData->HFIB_REF_FTv[t];
            Coarse_time = fData->HFIB_REF_CTv[t];
            Epoch = bswap_32(fData->HFIB_REF_EPOCHv[t]) & 0x0FFFFFFF;

            if (TDC < 9)
            {
                new ((*fArrayHFIB)[fArrayHFIB->GetEntriesFast()])
                    R3BHFIBMappedData(DetId, TDC, channelNumber, Fine_time, Coarse_time, 0, Epoch);
            }
        }
    }

    fNEvent += 1;
    return kTRUE;
}

void R3BHFIBReader::Reset()
{
    // Reset the output array
    fArrayHFIB->Clear();
}

ClassImp(R3BHFIBReader)
