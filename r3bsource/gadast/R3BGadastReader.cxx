/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
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

#include "R3BGadastMappedData.h"
#include "R3BGadastReader.h"
#include "R3BLogger.h"

#include "TClonesArray.h"
#include <iostream>

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_gadast.h"
}

R3BGadastReader::R3BGadastReader(EXT_STR_h101_GADAST_onion* data, size_t offset)
    : R3BReader("R3BGadastReader")
    , fData(data)
    , fOffset(offset)
    , fOnline(kFALSE)
    , fArray(new TClonesArray("R3BGadastMappedData"))
{
}

R3BGadastReader::~R3BGadastReader()
{
    if (fArray)
    {
        delete fArray;
    }
}

Bool_t R3BGadastReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    LOG(info) << "R3BGadastReader::Init()";
    EXT_STR_h101_GADAST_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_GADAST, 0);
    if (!ok)
    {
        perror("ext_data_struct_info_item");
        LOG(error) << "Failed to setup structure information.";
        return kFALSE;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("GadastMappedData", "GADAST", fArray, !fOnline);
    Reset();

    return kTRUE;
}

Bool_t R3BGadastReader::R3BRead()
{
    // Read data for different dates, if needed!
    ReadData();
    return kTRUE;
}

void R3BGadastReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

Bool_t R3BGadastReader::ReadData()
{

/*
  uint32_t GADAST_E[4];
  uint32_t GADAST_S[4];
  uint32_t GADAST_T[4];
  uint32_t GADAST_TREF[1];
  uint32_t GADAST_TTRIG[1];
*/
   R3BLOG(debug1, "Event data.");

   new ((*fArray)[fArray->GetEntriesFast()]) R3BGadastMappedData(10, fData->GADAST_TREF[0], 0);
   new ((*fArray)[fArray->GetEntriesFast()]) R3BGadastMappedData(11, fData->GADAST_TTRIG[0], 0);
   
    // SELECT THE FOR LOOP BASED ON THE MAPPING...
    for (int crystal = 0; crystal < NUM_GADAST_ANODES; ++crystal)
    {
       new ((*fArray)[fArray->GetEntriesFast()]) R3BGadastMappedData(crystal+1, fData->GADAST_T[crystal], fData->GADAST_E[crystal]);
    }

    return kTRUE;
}

ClassImp(R3BGadastReader)
