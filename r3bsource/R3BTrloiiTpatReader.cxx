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

#include "R3BTrloiiTpatReader.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "R3BEventHeader.h"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_tpat.h"
}

using namespace std;

R3BTrloiiTpatReader::R3BTrloiiTpatReader(EXT_STR_h101_TPAT* data, UInt_t offset)
    : R3BReader("R3BTrloiiTpatReader")
    , fNEvent(0)
    , fData(data)
    , fOffset(offset)
    , fEventHeader(nullptr)
{
}

R3BTrloiiTpatReader::~R3BTrloiiTpatReader()
{
    if (fEventHeader)
        delete fEventHeader;
}

Bool_t R3BTrloiiTpatReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    LOG(INFO) << "R3BTrloiiTpatReader::Init";
    EXT_STR_h101_TPAT_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_TPAT, 0);

    if (!ok)
    {
        perror("ext_data_struct_info_item");
        LOG(error) << "Failed to setup structure information.";
        return kFALSE;
    }

    FairRootManager* frm = FairRootManager::Instance();
    fEventHeader = (R3BEventHeader*)frm->GetObject("EventHeader.");
    if (!fEventHeader)
    {
        LOG(WARNING) << "R3BTrloiiTpatReader::Init() R3BEventHeader not found";
    }
    else
        LOG(INFO) << "R3BTrloiiTpatReader::Init() R3BEventHeader found";

    return kTRUE;
}

Bool_t R3BTrloiiTpatReader::Read()
{

    // LOG(info) << "TrloiiTpatReader::Read BEGIN";

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
        LOG(DEBUG1) << "R3BTrloiiTpatReader : event : " << fNEvent;
    }

    /* Display data */
    // char str[256];
    // sprintf(str, "  Trlo II Tpat = 0x%04x.", fData->TPATv[0]);
    // LOG(info) << str;

    // LOG(info) << "TrloiiTpatReader::Read END";

    return kTRUE;
}

void R3BTrloiiTpatReader::Reset() { fNEvent = 0; }

ClassImp(R3BTrloiiTpatReader)
