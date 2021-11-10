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

#include "R3BTrloiiData.h"
#include "R3BTrloiiScalerReader.h"

#include "TClonesArray.h"
#include "ext_data_struct_info.hh"

/**
 ** ext_h101_trlo.h was created by running
 ** $unpacker --ntuple=STRUCT_HH,RAW:TRLO,id=h101_TRLO,NOTRIGEVENTNO,ext_h101_trlo.h
 **/

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_trlo.h"
}

R3BTrloiiScalerReader::R3BTrloiiScalerReader(EXT_STR_h101_TRLO_onion* data, size_t offset)
    : R3BReader("R3BTrloiiScalerReader")
    , fNEvent(0)
    , fData(data)
    , fOffset(offset)
    , fOnline(kFALSE)
    , fArray(new TClonesArray("R3BTrloiiData"))
{
}

R3BTrloiiScalerReader::~R3BTrloiiScalerReader()
{
    if (fArray)
        delete fArray;
}

Bool_t R3BTrloiiScalerReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    LOG(INFO) << "R3BTrloiiScalerReader::Init()";
    EXT_STR_h101_TRLO_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_TRLO, 0);

    if (!ok)
    {
        LOG(ERROR) << "R3BTrloiiScalerReader::Failed to setup structure information.";
        return kFALSE;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("TrloiiData", "TRLOII info", fArray, !fOnline);
    fArray->Clear();

    return kTRUE;
}

Bool_t R3BTrloiiScalerReader::Read()
{
    LOG(DEBUG) << "R3BTrloiiScalerReader::Read() Event data.";

    for (int ch = 0; ch < 16; ++ch)
    {
        new ((*fArray)[fArray->GetEntriesFast()]) R3BTrloiiData(1, ch + 1, fData->TRLO[0].TRLORAW_MAIN[ch]);
        new ((*fArray)[fArray->GetEntriesFast()]) R3BTrloiiData(2, ch + 1, fData->TRLO[0].TRLOBDT_MAIN[ch]);
        new ((*fArray)[fArray->GetEntriesFast()]) R3BTrloiiData(3, ch + 1, fData->TRLO[0].TRLOADT_MAIN[ch]);
        new ((*fArray)[fArray->GetEntriesFast()]) R3BTrloiiData(4, ch + 1, fData->TRLO[0].TRLOARD_MAIN[ch]);
    }

    fNEvent += 1;
    return kTRUE;
}

void R3BTrloiiScalerReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

ClassImp(R3BTrloiiScalerReader);
