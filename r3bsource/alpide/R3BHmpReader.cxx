/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include <FairRootManager.h>

#include "R3BHmpMappedData.h"
#include "R3BHmpReader.h"
#include "R3BLogger.h"

#include "ext_data_struct_info.hh"
#include <TClonesArray.h>

/**
 ** ext_h101_hmp.h was created by running
 ** $unpacker --ntuple=STRUCT_HH,RAW:HMP,id=h101_HMP,NOTRIGEVENTNO,ext_h101_hmp.h
 **/

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_hmp.h"
}

R3BHmpReader::R3BHmpReader(EXT_STR_h101_HMP_onion* data, size_t offset)
    : R3BReader("R3BHmpReader")
    , fData(data)
    , fArray(new TClonesArray("R3BHmpMappedData"))
{
}

R3BHmpReader::~R3BHmpReader()
{
    R3BLOG(debug1, "");
    if (fArray)
    {
        delete fArray;
    }
}

Bool_t R3BHmpReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t okay = 0;
    R3BLOG(info, "");
    EXT_STR_h101_HMP_ITEMS_INFO(okay, *a_struct_info, fOffset, EXT_STR_h101_HMP, 0);

    R3BLOG_IF(fatal, !okay, "Failed to setup structure information.");

    // Register output array in tree
    FairRootManager::Instance()->Register("HmpMappedData", "HMP_Map", fArray, !fOnline);
    Reset();
    memset(fData, 0, sizeof(*fData));

    return kTRUE;
}

Bool_t R3BHmpReader::R3BRead()
{
    R3BLOG(debug1, "Event data: " << fNEvent);
    new ((*fArray)[fArray->GetEntriesFast()]) R3BHmpMappedData(fData->HMP_TSYS_LO,
                                                               fData->HMP_TSYS_HI,
                                                               fData->HMP_VOLT[0],
                                                               fData->HMP_CURR[0],
                                                               fData->HMP_VOLT[1],
                                                               fData->HMP_CURR[1],
                                                               fData->HMP_VOLT[2],
                                                               fData->HMP_CURR[2],
                                                               fData->HMP_VOLT[3],
                                                               fData->HMP_CURR[3]);
    fNEvent++;
    return kTRUE;
}

void R3BHmpReader::Reset()
{
    // Reset the output array
    if (fArray)
    {
        fArray->Clear();
    }
}

ClassImp(R3BHmpReader)
