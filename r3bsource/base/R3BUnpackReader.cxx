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

#include "FairEventHeader.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunOnline.h"

#include "R3BEventHeader.h"
#include "R3BUnpackReader.h"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_unpack.h"
}
#include <iostream>

R3BUnpackReader::R3BUnpackReader(EXT_STR_h101_unpack* data, size_t offset)
    : R3BReader("R3BUnpackReader")
    , fNEvent(0)
    , fData(data)
    , fOffset(offset)
    , fHeader(NULL)
{
}

R3BUnpackReader::~R3BUnpackReader()
{
    if (fHeader)
    {
        delete fHeader;
    }
}

Bool_t R3BUnpackReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    LOG(INFO) << "R3BUnpackReader::Init()";
    EXT_STR_h101_unpack_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_unpack, 0);

    if (!ok)
    {
        perror("ext_data_struct_info_item");
        LOG(error) << "Failed to setup structure information.";
        return kFALSE;
    }

    // Look for the R3BEventHeader
    auto frm = FairRootManager::Instance();
    fHeader = (R3BEventHeader*)frm->GetObject("EventHeader.");
    if (!fHeader)
    {
        fHeader = new R3BEventHeader();
        frm->Register("EventHeader.", "R3BEvtHeader", fHeader, kTRUE);
        LOG(WARNING) << "R3BUnpackReader::Init() R3BEventHeader not found";
    }
    else
        LOG(info) << "R3BUnpackReader::Init() R3BEventHeader found";

    return kTRUE;
}

Bool_t R3BUnpackReader::Read()
{
    /* Display data */
    //	LOG(info) << "  Event data:";
    //  char strMessage[1000];
    //	sprintf(strMessage, "  %10d (d%10d): %2d\n",
    //		fData->EVENTNO,
    //		fData->EVENTNO - fNEvent,
    //		fData->TRIGGER);
    //  LOG(info) << strMessage;

    fNEvent = fData->EVENTNO;

    if (0 == (fNEvent % 1000))
    {
        LOG(DEBUG1) << "R3BUnpackReader : event : " << fNEvent << ", trigger : " << fData->TRIGGER;
    }

    fHeader->SetTrigger(fData->TRIGGER);
    fHeader->SetEventno(fNEvent);

    //    if(fData->TRIGGER != 1) cout<<"R3BUnpack: "<<fData->TRIGGER<<endl;

    return kTRUE;
}

void R3BUnpackReader::Reset() { fNEvent = 0; }

ClassImp(R3BUnpackReader);
