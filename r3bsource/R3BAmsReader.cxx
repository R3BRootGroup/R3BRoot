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

#include "R3BAmsReader.h"
#include "FairLogger.h"

#include "TMath.h"
#include "TRandom.h"

#include "FairRootManager.h"
#include "R3BAmsMappedData.h"
#include "TClonesArray.h"
#include "ext_data_struct_info.hh"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_ams.h"
}

R3BAmsReader::R3BAmsReader(EXT_STR_h101_AMS_onion* data, UInt_t offset)
    : R3BReader("R3BAmsReader")
    , fNEvent(0)
    , fData(data)
    , fOffset(offset)
    , fOnline(kFALSE)
    , fArray(new TClonesArray("R3BAmsMappedData"))
{
}

R3BAmsReader::~R3BAmsReader()
{
    if (fArray)
    {
        delete fArray;
    }
}

Bool_t R3BAmsReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    LOG(INFO) << "R3BAmsReader::Init";
    EXT_STR_h101_AMS_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_AMS, 0);

    if (!ok)
    {
        LOG(ERROR) << "R3BAmsReader::Failed to setup structure information.";
        return kFALSE;
    }

    // Register output array in tree
    if (!fOnline)
    {
        FairRootManager::Instance()->Register("AmsMappedData", "AMS", fArray, kTRUE);
    }
    else
    {
        FairRootManager::Instance()->Register("AmsMappedData", "AMS", fArray, kFALSE);
    }

    return kTRUE;
}

Bool_t R3BAmsReader::Read()
{
    LOG(DEBUG) << "R3BAmsReader::Read() Event data.";
    // Read AMS detectors
    for (int d = 0; d < 6; d++)
        if (fData->SST[d]._ == 1024)
            for (int strip = 0; strip < fData->SST[d]._; ++strip)
            {
                new ((*fArray)[fArray->GetEntriesFast()]) R3BAmsMappedData(d, strip, fData->SST[d].E[strip]);
            }
    fNEvent += 1;
    return kTRUE;
}

void R3BAmsReader::Reset()
{
    // Reset the output array
    fArray->Clear();
    //	fNEvent = 0;
}

ClassImp(R3BAmsReader)
