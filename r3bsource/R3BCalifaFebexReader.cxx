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

#include "R3BCalifaFebexReader.h"
#include "FairLogger.h"

#include "FairRootManager.h"
#include "R3BCalifaMappedData.h"
#include "TClonesArray.h"
#include "ext_data_struct_info.hh"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_califa.h"
    // for future reference: ext_h101_califa.h was created by running
    // $unpacker --ntuple=STRUCT_HH,RAW:CALIFA,id=h101_CALIFA,NOTRIGEVENTNO,ext_h101_califa.h
}

R3BCalifaFebexReader::R3BCalifaFebexReader(EXT_STR_h101_CALIFA* data, size_t offset)
    : R3BReader("R3BCalifaFebexReader")
    , fNEvent(0)
    , fData(data)
    , fOffset(offset)
    , fOnline(kFALSE)
    , fArray(new TClonesArray("R3BCalifaMappedData"))
{
}

R3BCalifaFebexReader::~R3BCalifaFebexReader()
{
    if (fArray)
        delete fArray;
}

Bool_t R3BCalifaFebexReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    LOG(INFO) << "R3BCalifaFebexReader::Init()";
    EXT_STR_h101_CALIFA_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_CALIFA, 0);

    if (!ok)
    {
        LOG(ERROR) << "R3BCalifaFebexReader::Failed to setup structure information.";
        return kFALSE;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("CalifaMappedData", "Califa", fArray, !fOnline);

    return kTRUE;
}

Bool_t R3BCalifaFebexReader::Read()
{
    LOG(DEBUG) << "R3BCalifaFebexReader::Read() Event data.";

    // SELECT THE FOR LOOP BASED ON THE MAPPING...
    for (int crystal = 0; crystal < fData->CALIFA_ENE; ++crystal)
    {
        UShort_t channelNumber = fData->CALIFA_ENEI[crystal];
        int16_t energy = fData->CALIFA_ENEv[crystal];
        int16_t nf = fData->CALIFA_NFv[crystal];
        int16_t ns = fData->CALIFA_NSv[crystal];

        uint64_t febextime = ((uint64_t)fData->CALIFA_TSMSBv[crystal] << 32) | (uint64_t)fData->CALIFA_TSLSBv[crystal];

        uint64_t wrts = ((uint64_t)fData->CALIFA_WRTS_T4v[crystal] << 48) |
                        ((uint64_t)fData->CALIFA_WRTS_T3v[crystal] << 32) |
                        ((uint64_t)fData->CALIFA_WRTS_T2v[crystal] << 16) | (uint64_t)fData->CALIFA_WRTS_T1v[crystal];

        int32_t ov = fData->CALIFA_OVv[crystal];
        int16_t pu = fData->CALIFA_PILEUPv[crystal];
        int16_t dc = fData->CALIFA_DISCARDv[crystal];

        int16_t tot = fData->CALIFA_TOTv[crystal];

        new ((*fArray)[fArray->GetEntriesFast()])
            R3BCalifaMappedData(channelNumber, energy, nf, ns, febextime, wrts, ov, pu, dc, tot);
    }
    fNEvent += 1;
    return kTRUE;
}

void R3BCalifaFebexReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

ClassImp(R3BCalifaFebexReader);
