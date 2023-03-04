
/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum f�r Schwerionenforschung GmbH    *
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

#include "R3BWhiterabbitLosReader.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BWRLosData.h"
#include "TClonesArray.h"
using namespace std;
extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_wrlos.h"
}

R3BWhiterabbitLosReader::R3BWhiterabbitLosReader(EXT_STR_h101_WRLOS* data, UInt_t offset, UInt_t whiterabbit_id)
    : R3BReader("R3BWhiterabbitLosReader")
    , fNEvent(0)
    , fData(data)
    , fOffset(offset)
    , fOnline(kFALSE)
    , fLogger(FairLogger::GetLogger())
    , fWhiterabbitId(whiterabbit_id)
    , fEventHeader(nullptr)
    , fArray(new TClonesArray("R3BWRLosData"))
{
}

R3BWhiterabbitLosReader::~R3BWhiterabbitLosReader()
{
    if (fArray)
    {
        delete fArray;
    }
}

Bool_t R3BWhiterabbitLosReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    LOG(info) << "R3BWhiterabbitLosReader::Init";
    EXT_STR_h101_WRLOS_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_WRLOS, 0);

    if (!ok)
    {
        LOG(error) << "R3BWhiterabbitLosReader::Failed to setup structure information.";
        return kFALSE;
    }
    FairRootManager* mgr = FairRootManager::Instance();
    fEventHeader = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    if (fEventHeader)
        R3BLOG(info, "EventHeader. was found");
    else
        R3BLOG(info, "EventHeader. was not found");
    // Register output array in tree
    if (!fOnline)
    {
        FairRootManager::Instance()->Register("WRLosData", "WRLos", fArray, kTRUE);
    }
    else
    {
        FairRootManager::Instance()->Register("WRLosData", "WRLos", fArray, kFALSE);
    }

    fData->TIMESTAMP_LOS_ID = 0;

    return kTRUE;
}

Bool_t R3BWhiterabbitLosReader::Read()
{

    if (!fData->TIMESTAMP_LOS_ID)
    {
        return kTRUE;
    }
    /*
        printf("Los %08x %08x %08x %08x %08x \n",
        fData->TIMESTAMP_LOS_ID,
        fData->TIMESTAMP_LOS_WR_T4, fData->TIMESTAMP_LOS_WR_T3,
        fData->TIMESTAMP_LOS_WR_T2, fData->TIMESTAMP_LOS_WR_T1);
    */
    if (fWhiterabbitId != fData->TIMESTAMP_LOS_ID)
    {
        char strMessage[1000];
        snprintf(strMessage,
                 sizeof strMessage,
                 "Event %u: Whiterabbit ID mismatch: expected 0x%x, got 0x%x.\n",
                 fEventHeader->GetEventno(),
                 fWhiterabbitId,
                 fData->TIMESTAMP_LOS_ID);
        LOG(error) << strMessage;
    }

    if (fEventHeader != nullptr)
    {
        uint64_t timestamp = ((uint64_t)fData->TIMESTAMP_LOS_WR_T4 << 48) |
                             ((uint64_t)fData->TIMESTAMP_LOS_WR_T3 << 32) |
                             ((uint64_t)fData->TIMESTAMP_LOS_WR_T2 << 16) | (uint64_t)fData->TIMESTAMP_LOS_WR_T1;
        // fEventHeader->SetTimeStamp(timestamp);
        fNEvent = fEventHeader->GetEventno();
        new ((*fArray)[fArray->GetEntriesFast()]) R3BWRLosData(timestamp);

        // cout<<"WRLOS READER TIME: "<<timestamp<<", "<<fArray->GetEntriesFast()<<endl;
    }
    else
    {
        fNEvent++;
    }

    fData->TIMESTAMP_LOS_ID = 0;

    return kTRUE;
}

void R3BWhiterabbitLosReader::Reset()
{
    // Reset the output array
    fArray->Clear();
    fNEvent = 0;
}

ClassImp(R3BWhiterabbitLosReader)
