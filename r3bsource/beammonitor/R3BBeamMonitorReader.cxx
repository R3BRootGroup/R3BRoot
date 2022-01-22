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

#include "R3BBeamMonitorReader.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "R3BBeamMonitorMappedData.h"
#include "R3BEventHeader.h"
#include "TClonesArray.h"
extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_bmon.h"
}
#include "TMath.h"
#define IS_NAN(x) TMath::IsNaN(x)
#define NUM_BMON 3
#include <iostream>

using namespace std;

R3BBeamMonitorReader::R3BBeamMonitorReader(EXT_STR_h101_BMON* data, UInt_t offset)
    : R3BReader("R3BBeamMonitorReader")
    , fData(data)
    , fOffset(offset)
    , fLogger(FairLogger::GetLogger())
    , fArray(new TClonesArray("R3BBeamMonitorMappedData"))
{
}

R3BBeamMonitorReader::~R3BBeamMonitorReader() {}

Bool_t R3BBeamMonitorReader::Init(ext_data_struct_info* a_struct_info)
{

    int ok;

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(ERROR) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

    EXT_STR_h101_BMON_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_BMON, 0);
    if (!ok)
    {
        perror("ext_data_struct_info_item");
        LOG(ERROR) << "Failed to setup structure information.";

        return kFALSE;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("BeamMonitorMapped", "Land", fArray, kTRUE);
    fArray->Clear();

    // clear struct_writer's output struct. Seems ucesb doesn't do that
    // for channels that are unknown to the current ucesb config.
    EXT_STR_h101_BMON_onion* data = (EXT_STR_h101_BMON_onion*)fData;
    data->IC = 0;
    data->SEETRAM = 0;
    data->TOFDOR = 0;

    return kTRUE;
}

Bool_t R3BBeamMonitorReader::Read()
{

    // Convert plain raw data to multi-dimensional array
    EXT_STR_h101_BMON_onion* data = (EXT_STR_h101_BMON_onion*)fData;

    new ((*fArray)[fArray->GetEntriesFast()]) R3BBeamMonitorMappedData(data->IC, data->SEETRAM, data->TOFDOR);

    fNEvents += 1;

    return kTRUE;
}

void R3BBeamMonitorReader::FinishTask() {}

void R3BBeamMonitorReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

ClassImp(R3BBeamMonitorReader)
