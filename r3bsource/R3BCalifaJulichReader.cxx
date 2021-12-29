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

#include "FairLogger.h"
#include "FairRootManager.h"

#include "R3BAmsMappedData.h"
#include "R3BCalifaJulichReader.h"
#include "R3BCalifaMappedData.h"
#include "TClonesArray.h"

/**
 ** ext_h101_califa.h was created by running
 ** $unpacker --ntuple=STRUCT_HH,RAW:CALIFA,id=h101_CALIFA,NOTRIGEVENTNO,ext_h101_califa.h
 **/

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_califa_julich.h"
}

R3BCalifaJulichReader::R3BCalifaJulichReader(EXT_STR_h101_CALIFA* data, size_t offset)
    : R3BReader("R3BCalifaJulichReader")
    , fNEvent(0)
    , fData(data)
    , fOffset(offset)
    , fOnline(kFALSE)
    , fArrayCalifa(new TClonesArray("R3BCalifaMappedData"))
    , fArrayAms(new TClonesArray("R3BAmsMappedData"))
{
}

R3BCalifaJulichReader::~R3BCalifaJulichReader()
{
    if (fArrayCalifa)
        delete fArrayCalifa;
    if (fArrayAms)
        delete fArrayAms;
}

Bool_t R3BCalifaJulichReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    LOG(INFO) << "R3BCalifaJulichReader::Init()";
    EXT_STR_h101_CALIFA_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_CALIFA, 1);

    if (!ok)
    {
        LOG(ERROR) << "R3BCalifaJulichReader::Failed to setup structure information.";
        // LOG(ERROR) << "sizeof=" << sizeof(EXT_STR_h101_CALIFA);
        return kFALSE;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("CalifaMappedData", "Califa", fArrayCalifa, !fOnline);
    FairRootManager::Instance()->Register("AmsMappedData", "Ams", fArrayAms, !fOnline);

    return kTRUE;
}

Bool_t R3BCalifaJulichReader::Read()
{
    LOG(DEBUG) << "R3BCalifaJulichReader::Read() Event data.";

    // SELECT THE FOR LOOP BASED ON THE MAPPING...
    for (int crystal = 0; crystal < fData->CALIFA_ENE; ++crystal)
    {
        UShort_t channelNumber = fData->CALIFA_ENEI[crystal];
        int16_t energy = fData->CALIFA_ENEv[crystal];
        int16_t nf = fData->CALIFA_NFv[crystal];
        int16_t ns = fData->CALIFA_NSv[crystal];

        uint64_t febextime = ((uint64_t)fData->CALIFA_TSMSBv[crystal] << 32) | (uint64_t)fData->CALIFA_TSLSBv[crystal];

        uint64_t wrts = 0; //((uint64_t)fData->CALIFA_WRTS_T4v[crystal] << 48) |
                           //                  ((uint64_t)fData->CALIFA_WRTS_T3v[crystal] << 32) |
                           //                  ((uint64_t)fData->CALIFA_WRTS_T2v[crystal] << 16) |
                           //                  (uint64_t)fData->CALIFA_WRTS_T1v[crystal];

        int32_t ov = 0; // fData->CALIFA_OVv[crystal];
        int16_t pu = 0; // fData->CALIFA_PILEUPv[crystal];
        int16_t dc = 0; // fData->CALIFA_DISCARDv[crystal];

        int16_t tot = 0; // fData->CALIFA_TOTv[crystal];

        if (channelNumber < 384)
            new ((*fArrayCalifa)[fArrayCalifa->GetEntriesFast()])
                R3BCalifaMappedData(channelNumber, energy, nf, ns, febextime, wrts, ov, pu, dc, tot);

        else if (channelNumber < 448)
            new ((*fArrayAms)[fArrayAms->GetEntriesFast()]) R3BAmsMappedData(0, channelNumber - 384, energy);
    }
    fNEvent += 1;
    return kTRUE;
}

void R3BCalifaJulichReader::Reset()
{
    // Reset the output array
    fArrayCalifa->Clear();
    fArrayAms->Clear();
}

ClassImp(R3BCalifaJulichReader);
