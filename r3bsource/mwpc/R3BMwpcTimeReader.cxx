/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include <FairLogger.h>
#include <FairRootManager.h>
#include <TClonesArray.h>

#include "R3BLogger.h"
#include "R3BMwpcTimeMappedData.h"
#include "R3BMwpcTimeReader.h"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_mwpc_time.h"
}

#define NUM_SOFMWPC_DETECTORS 4
#define NUM_SOFMWPC_PLANES_MAX 3

using namespace std;

R3BMwpcTimeReader::R3BMwpcTimeReader(EXT_STR_h101_SOFMWPC_TIME* data, size_t offset)
    : R3BReader("R3BMwpcTimeReader")
    , fMaxDet(4)
    , fData(data)
    , fOffset(offset)
    , fOnline(kFALSE)
    , fArrayMwpcT0(new TClonesArray("R3BMwpcTimeMappedData")) // class name
    , fArrayMwpcT1(new TClonesArray("R3BMwpcTimeMappedData")) // class name
    , fArrayMwpcT2(new TClonesArray("R3BMwpcTimeMappedData")) // class name
    , fArrayMwpcT3(new TClonesArray("R3BMwpcTimeMappedData")) // class name
{
}

R3BMwpcTimeReader::~R3BMwpcTimeReader()
{
    R3BLOG(debug1, "Destructor");
    if (fArrayMwpcT0)
    {
        delete fArrayMwpcT0;
    }
    if (fArrayMwpcT1)
    {
        delete fArrayMwpcT1;
    }
    if (fArrayMwpcT2)
    {
        delete fArrayMwpcT2;
    }
    if (fArrayMwpcT3)
    {
        delete fArrayMwpcT3;
    }
}

Bool_t R3BMwpcTimeReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    R3BLOG(info, "");
    EXT_STR_h101_SOFMWPC_TIME_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_SOFMWPC_TIME, 0);
    if (!ok)
    {
        R3BLOG(error, "Failed to setup structure information.");
        return kFALSE;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("Mwpc0TimeMappedData", "MWPC0", fArrayMwpcT0, !fOnline);
    if (fMaxDet > 1)
        FairRootManager::Instance()->Register("Mwpc1TimeMappedData", "MWPC1", fArrayMwpcT1, !fOnline);
    if (fMaxDet > 2)
        FairRootManager::Instance()->Register("Mwpc2TimeMappedData", "MWPC2", fArrayMwpcT2, !fOnline);
    if (fMaxDet > 3)
        FairRootManager::Instance()->Register("Mwpc3TimeMappedData", "MWPC3", fArrayMwpcT3, !fOnline);
    R3BLOG(info, "Max. number of MWPCs: " << fMaxDet);
    Reset();
    memset(fData, 0, sizeof *fData);

    // clear struct_writer's output struct. Seems ucesb doesn't do that
    // for channels that are unknown to the current ucesb config.
    // no multi-hit capabiity
    auto* data = reinterpret_cast<EXT_STR_h101_SOFMWPC_TIME_onion*>(fData);
    for (int d = 0; d < NUM_SOFMWPC_DETECTORS; d++)
        for (int p = 0; p < NUM_SOFMWPC_PLANES_MAX; p++)
            data->SOFMWPC[d].TIME = 0;
    return kTRUE;
}

Bool_t R3BMwpcTimeReader::R3BRead()
{
    // Convert plain raw data to multi-dimensional array
    auto* data = reinterpret_cast<EXT_STR_h101_SOFMWPC_TIME_onion*>(fData);

    // loop over all detectors
    for (int d = 0; d < NUM_SOFMWPC_DETECTORS; d++)
    {
        uint32_t numberOfTimes = data->SOFMWPC[d].TIME;
        for (int mult = 0; mult < numberOfTimes; mult++)
        {
            uint16_t plane = data->SOFMWPC[d].TIMEI[mult];
            uint16_t time = data->SOFMWPC[d].TIMEv[mult];
            switch (d)
            {
                case 0:
                    new ((*fArrayMwpcT0)[fArrayMwpcT0->GetEntriesFast()]) R3BMwpcTimeMappedData(plane, time);
                    break;
                case 1:
                    new ((*fArrayMwpcT1)[fArrayMwpcT1->GetEntriesFast()]) R3BMwpcTimeMappedData(plane, time);
                    break;
                case 2:
                    new ((*fArrayMwpcT2)[fArrayMwpcT2->GetEntriesFast()]) R3BMwpcTimeMappedData(plane, time);
                    break;
                case 3:
                    new ((*fArrayMwpcT3)[fArrayMwpcT3->GetEntriesFast()]) R3BMwpcTimeMappedData(plane, time);
                    break;
                default:
                    break;
            }
        }
    }

    return kTRUE;
}

void R3BMwpcTimeReader::Reset()
{
    // Reset the output array
    fArrayMwpcT0->Clear();
    fArrayMwpcT1->Clear();
    fArrayMwpcT2->Clear();
    fArrayMwpcT3->Clear();
}

ClassImp(R3BMwpcTimeReader)
