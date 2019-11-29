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

#include "R3BNeulandTamexReader.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "R3BPaddleTamexMappedData.h"
#include "TClonesArray.h"
#include "ext_data_struct_info.hh"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_raw_nnp_tamex.h"
}

R3BNeulandTamexReader::R3BNeulandTamexReader(EXT_STR_h101_raw_nnp_tamex* data, UInt_t offset)
    : R3BReader("R3BNeulandTamexReader")
    , fData(data)
    , fOffset(offset)
    , fArray(new TClonesArray("R3BPaddleTamexMappedData"))
    , fNofPlanes(sizeof(((EXT_STR_h101_raw_nnp_tamex_onion_t*)(data))->NN_P) /
                 sizeof(*(((EXT_STR_h101_raw_nnp_tamex_onion_t*)(data))->NN_P)))
{
}

R3BNeulandTamexReader::~R3BNeulandTamexReader() {}

Bool_t R3BNeulandTamexReader::Init(ext_data_struct_info* a_struct_info)
{
    int ok;

    EXT_STR_h101_raw_nnp_tamex_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_raw_nnp_tamex, 0);

    if (!ok)
    {
        perror("ext_data_struct_info_item");
        LOG(error) << "Failed to setup structure information.";
        return kFALSE;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("NeulandMappedData", "Neuland", fArray, kTRUE);

    return kTRUE;
}

Bool_t R3BNeulandTamexReader::Read()
{
    const auto data = (EXT_STR_h101_raw_nnp_tamex_onion_t*)fData;

    for (int plane = 0; plane < fNofPlanes; ++plane)
    {
        for (int pm = 0; pm < 2; ++pm)
        {
            int idx = 0;
            int start = 0;
            int stop = 0;
            int bar;
            int cLE, fLE, cTE, fTE;

            // the counter for coarse time and fine time should be always the same:
            if (data->NN_P[plane].tcl_T[pm].BM != data->NN_P[plane].tfl_T[pm].BM ||
                data->NN_P[plane].tcl_T[pm].B != data->NN_P[plane].tfl_T[pm].B ||
                data->NN_P[plane].tct_T[pm].BM != data->NN_P[plane].tft_T[pm].BM ||
                data->NN_P[plane].tct_T[pm].B != data->NN_P[plane].tft_T[pm].B)
            {
                // fLogger->Info(MESSAGE_ORIGIN, "  Bad event, counter of coarse times and fine times do not match \n");
                return kFALSE;
            }

            // the counter for leading and trailing edge should be always the same:
            if (data->NN_P[plane].tcl_T[pm].B != data->NN_P[plane].tct_T[pm].B ||
                data->NN_P[plane].tfl_T[pm].B != data->NN_P[plane].tft_T[pm].B)
            {
                // fLogger->Info(MESSAGE_ORIGIN, "  Bad event, mismatch of trailing and leading edges \n");
                return kFALSE;
            }

            for (int hit = 0; hit < data->NN_P[plane].tcl_T[pm].BM; hit++)
            {
                bar = data->NN_P[plane].tcl_T[pm].BMI[hit];
                stop = data->NN_P[plane].tcl_T[pm].BME[hit];
                // fLogger->Info(MESSAGE_ORIGIN, "  bar %d, multihit %d \n", bar, stop-start);

                for (int multi = start; multi < stop; multi++)
                {
                    cLE = data->NN_P[plane].tcl_T[pm].Bv[multi];
                    fLE = data->NN_P[plane].tfl_T[pm].Bv[multi];
                    cTE = data->NN_P[plane].tct_T[pm].Bv[multi];
                    fTE = data->NN_P[plane].tft_T[pm].Bv[multi];

                    // fLogger->Info(MESSAGE_ORIGIN, " leading coarse time %d, fine time %d \n", cLE, fLE);
                    // fLogger->Info(MESSAGE_ORIGIN, " trailing coarse time %d, fine time %d \n",cTE, fTE);

                    auto mapped = new ((*fArray)[fArray->GetEntriesFast()]) R3BPaddleTamexMappedData(plane + 1, bar);
                    if (0 == pm)
                    {
                        mapped->fCoarseTime1LE = cLE;
                        mapped->fFineTime1LE = fLE;
                        mapped->fCoarseTime1TE = cTE;
                        mapped->fFineTime1TE = fTE;
                    }
                    else
                    {
                        mapped->fCoarseTime2LE = cLE;
                        mapped->fFineTime2LE = fLE;
                        mapped->fCoarseTime2TE = cTE;
                        mapped->fFineTime2TE = fTE;
                    }
                }
                start = stop;
            }
        }
    }

    return kTRUE;
}

void R3BNeulandTamexReader::Reset() { fArray->Clear(); }

ClassImp(R3BNeulandTamexReader)
