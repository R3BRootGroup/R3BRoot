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
#include "R3BRpcReader.h"
#include "TClonesArray.h"
#include "R3BRpcStripMappedData.h"
#include "R3BRpcRefMappedData.h"
#include "R3BRpcPmtMappedData.h"


extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_rpc.h"
}


R3BRpcReader::R3BRpcReader(EXT_STR_h101_RPC* data, size_t offset)
    : R3BReader("R3BRpcReader")
    , fData(data)
    , fLogger(FairLogger::GetLogger())
    , fArrayPmt(new TClonesArray("R3BRpcPmtMappedData"))
    , fArrayRef(new TClonesArray("R3BRpcRefMappedData"))
    , fArrayStrip(new TClonesArray("R3BRpcStripMappedData"))
    , fOnline(kFALSE)
    , fOffset(offset)
    , fCoarseReference(0)
    , fNEvent(0)
{
}

R3BRpcReader::~R3BRpcReader() {

  if(fArrayPmt)
   delete fArrayPmt;

  if(fArrayRef)
   delete fArrayRef;

  if(fArrayStrip)
   delete fArrayStrip;
}

Bool_t R3BRpcReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;

    LOG(INFO) << "R3BRpcReader::Init()";
    EXT_STR_h101_RPC_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_RPC,1);

    if (!ok)
    {
        LOG(ERROR) << "R3BRpcReader::Failed to setup structure information.";
        //LOG(error) << "Failed to setup structure information.";
        return kFALSE;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("RpcRefMappedData", "Rpc", fArrayRef, !fOnline);
    FairRootManager::Instance()->Register("RpcPmtMappedData", "Rpc", fArrayPmt, !fOnline);
    FairRootManager::Instance()->Register("RpcStripMappedData", "Rpc", fArrayStrip,!fOnline);

     return kTRUE;
}


Bool_t R3BRpcReader::Read()
{

    uint64_t Fine_time;
    uint64_t Coarse_time;
    UShort_t channelNumber;
    UShort_t Edge;
    UShort_t Side;


    LOG(DEBUG) << "R3BRpcReader::Read() Event data.";


    for (int i = 0; i < fData->RPC_FT_RM; i++)
    {

        int begin = 0;

        if(i!=0){begin=fData->RPC_FT_RME[i-1];}

        for (int t = begin; t < fData->RPC_FT_RME[i]; t++){

                channelNumber = fData->RPC_FT_RMI[i];
                Fine_time = fData->RPC_FT_Rv[t];
                Coarse_time = fData->RPC_CT_Rv[t];
                Edge = fData->RPC_E_Rv[t];
                Side = 1;

                new ((*fArrayStrip)[fArrayStrip->GetEntriesFast()])
            R3BRpcStripMappedData(channelNumber, Fine_time, Coarse_time, Edge, Side);

            //R3BRpcStripMappedData(UShort_t StripId, uint64_t FineTime, uint64_t CoarseTime, UShort_t Edge, UShort_t Side)

        }
    }


    for (int i = 0; i < fData->RPC_REF_FTM; i++)
    {

        int begin = 0;

        if(i!=0){begin=fData->RPC_REF_FTME[i-1];}

        for (int t = begin; t < fData->RPC_REF_FTME[i]; t++){

                channelNumber = fData->RPC_REF_FTMI[i];
                Fine_time = fData->RPC_REF_FTv[t];
                Coarse_time = fData->RPC_REF_CTv[t];

                new ((*fArrayRef)[fArrayRef->GetEntriesFast()])
            R3BRpcRefMappedData(channelNumber,Fine_time,Coarse_time);

            //R3BRpcRefMappedData::R3BRpcRefMappedData(UShort_t RefId, uint64_t FineTime, uint64_t CoarseTime)

        }
    }


    // -------------------- Filling PMT data ----------------------
    for (int i = 0; i < fData->RPC_FT_PMTM; i++)
    {

        int begin = 0;

        if(i!=0){begin=fData->RPC_FT_PMTME[i-1];}

        for (int t = begin; t < fData->RPC_FT_PMTME[i]; t++){

                channelNumber = fData->RPC_FT_PMTMI[i];
                Fine_time = fData->RPC_FT_PMTv[t];
                Coarse_time = fData->RPC_CT_PMTv[t];

                Edge         = fData->RPC_E_PMTv[t];
                new ((*fArrayPmt)[fArrayPmt->GetEntriesFast()])
            R3BRpcPmtMappedData(channelNumber,Fine_time,Coarse_time,Edge);

            //R3BRpcRefMappedData::R3BRpcRefMappedData(UShort_t RefId, uint64_t FineTime, uint64_t CoarseTime)

        }
    }


    fNEvent += 1;

    return kTRUE;
}

void R3BRpcReader::Reset()
{
    // Reset the output array
    fArrayRef->Clear();
    fArrayPmt->Clear();
    fArrayStrip->Clear();


}

ClassImp(R3BRpcReader)
