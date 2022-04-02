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
#include "FairRunOnline.h"

#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BTrloiiTpatReader.h"
#include "R3BUcesbSource.h"

/**
 ** ext_h101_tpat.h was created by running
 ** $unpacker --ntuple=STRUCT_HH,RAW:TPAT,RAW:TPATFAIL,id=h101_TPAT,NOTRIGEVENTNO,ext_h101_tpat.h
 **/

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_tpat.h"
}

R3BTrloiiTpatReader::R3BTrloiiTpatReader(EXT_STR_h101_TPAT* data, size_t offset)
    : R3BReader("R3BTrloiiTpatReader")
    , fNEvent(0)
    , fData(data)
    , fOffset(offset)
    , fTrigger(-1)
    , fTpat(-1)
    , fEventHeader(nullptr)
{
}

R3BTrloiiTpatReader::~R3BTrloiiTpatReader()
{
    R3BLOG(INFO, "");
    if (fEventHeader)
    {
        delete fEventHeader;
    }
}

Bool_t R3BTrloiiTpatReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    R3BLOG(INFO, "");
    EXT_STR_h101_TPAT_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_TPAT, 0);
    if (!ok)
    {
        R3BLOG(FATAL, "Failed to setup structure information");
        return kFALSE;
    }

    FairRootManager* frm = FairRootManager::Instance();
    fEventHeader = (R3BEventHeader*)frm->GetObject("EventHeader.");
    if (!fEventHeader)
    {
        R3BLOG(FATAL, "EventHeader. not found");
        return kFALSE;
    }
    else
    {
        R3BLOG(INFO, "EventHeader. found");
    }
    
    R3BUcesbSource* source = (R3BUcesbSource*)frm->GetSource();
    if (fTpat >= 0 && source->GetSkipEvents())
    {
        R3BLOG(WARNING,
               "Both SetSkipEvents() in R3BUcesbSource and SetTpat() in R3BTrloiiTpatReader are defined to select "
               "events based on tpat values.");
        R3BLOG(INFO, "Thus SetSkipEvents() in R3BUcesbSource will be disabled.");
        source->SetSkipEvents(false);
    }

    Reset();
    memset(fData, 0, sizeof *fData);

    return kTRUE;
}

Bool_t R3BTrloiiTpatReader::Read()
{
    R3BLOG(DEBUG1, "Event data.");
    if (fEventHeader)
    {
        fEventHeader->SetTpat(0);
        if (fData->TPAT > 0)
        {
            fEventHeader->SetTpat(fData->TPATv[0]);
            fNEvent = fEventHeader->GetEventno();
        }
    }
    if (!fEventHeader || fData->TPAT <= 0)
        fNEvent++;

    if (0 == (fNEvent % 1000000))
    {
        R3BLOG(DEBUG1, "Event : " << fNEvent);
    }

    if (fTrigger > 0 && fEventHeader && fTpat < 0)
    {
        if (fEventHeader->GetTrigger() == fTrigger)
        {
            FairRunOnline::Instance()->MarkFill(kTRUE);
        }
        else
        {
            FairRunOnline::Instance()->MarkFill(kFALSE);
        }
    }

    if (fTpat >= 0 && fEventHeader && fTrigger <= 0)
    {
        if ((fEventHeader->GetTpat() & fTpat) == fTpat)
        {
            FairRunOnline::Instance()->MarkFill(kTRUE);
        }
        else
        {
            FairRunOnline::Instance()->MarkFill(kFALSE);
        }
    }

    if (fTpat >= 0 && fEventHeader && fTrigger > 0)
    {
        if ((fEventHeader->GetTpat() & fTpat) == fTpat && fEventHeader->GetTrigger() == fTrigger)
        {
            FairRunOnline::Instance()->MarkFill(kTRUE);
        }
        else
        {
            FairRunOnline::Instance()->MarkFill(kFALSE);
        }
    }

    return kTRUE;
}

void R3BTrloiiTpatReader::Reset() { fNEvent = 0; }

ClassImp(R3BTrloiiTpatReader);
