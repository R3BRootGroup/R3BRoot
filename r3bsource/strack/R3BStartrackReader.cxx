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

// Reader for the Silicon Tracker (STarTrack) Detector
// Created 26/05/2017 - Marc Labiche (marc.labiche@stfc.ac.uk)
//
#include "R3BStartrackReader.h"
#include "FairLogger.h"

#include "FairRootManager.h"
#include "R3BStartrackMappedData.h"
#include "TClonesArray.h"
#include "ext_data_struct_info.hh"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_star.h"
}

using namespace std;

R3BStartrackReader::R3BStartrackReader(EXT_STR_h101_STAR* data, UInt_t offset)
    : R3BReader("R3BStartrackReader")
    , fData(data)
    , fOffset(offset)
    , fts_vhb(0) // keeping this information for in between data blocks
    , fts_hb(0)  // keeping this information for in between data blocks
    , fLogger(FairLogger::GetLogger())
    , fArray(new TClonesArray("R3BStartrackMappedData"))
{
}

R3BStartrackReader::~R3BStartrackReader()
{
    if (fArray)
    {
        delete fArray;
    }
}

Bool_t R3BStartrackReader::Init(ext_data_struct_info* a_struct_info)
{
    int ok;

    EXT_STR_h101_STAR_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_STAR_t, 0);

    if (!ok)
    {
        perror("ext_data_struct_info_item");
        LOG(error) << "Failed to setup structure information.";
        return kFALSE;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("StartrackMapped", "Startrack", fArray, kTRUE);

    return kTRUE;
}

Bool_t R3BStartrackReader::Read()
{
    // Convert plain raw data to multi-dimensional array
    EXT_STR_h101_STAR_onion_t* data = (EXT_STR_h101_STAR_onion_t*)fData;

    // Display data
    // LOG(info) << "R3BStartrackReader::Read() Event data:";

    Int_t wordtype = -1;
    Int_t hitbit = -1;
    Int_t ladder_id = -1;
    Int_t side = -1;
    Int_t asic_id = -1;
    Int_t strip_id = -1;
    Int_t info_code = 0;
    Int_t info_field = 0;
    UInt_t adcData = 0;

    //    UInt_t WRvhb=0;
    //    UInt_t WRhb=0;
    //    UInt_t WRlb=0;
    uint64_t TS = -1;
    uint64_t TSExt = -1;
    //    UInt_t ts_vhb=0.; // replaced by fts_vhb
    //    UInt_t ts_hb=0.;  // replaced by fts_hb
    UInt_t ts_lb = 0;
    UInt_t tsExt_vhb = 0;
    UInt_t tsExt_hb = 0;
    UInt_t tsExt_lb = 0;

    uint32_t starN = data->sit_n; // not necessarly number of hits! (b/c multi hit)  = block size
    // uint32_t  stardata= data->sit_data[starN]; // not necessarly number of hits! (b/c multi hit)

    if (starN)
    {
        // cout << "starN= " << starN << endl;

        // loop over channels
        // uint32_t curChannelStart=0;     // index in v for first item of current channel
        // for (int i=0;i<starN;i++) {
        for (int i = 0; i < starN; i = i + 2)
        {
            uint32_t starWord1 = data->sit_data[i];     // wordA
            uint32_t starWord2 = data->sit_data[i + 1]; // wordB
            // uint32_t nextChannelStart=data->sit_data[i];  // index in v for first item of next channel

            // cout << "stardata1= " << starWord1 << endl;
            // cout << "stardata2= " << starWord2 << endl;

            // Case of wordtype=2:
            if (((starWord1 >> 30) == 0x2) && (((starWord1 & 0xFFFFFFFF) != 0xFFFFFFFF)))
            {
                // Decoding the first 32bit word stardata2:
                wordtype = (starWord1 >> 30) & 0x3; // bit 31:30

                info_code = (starWord1 & 0x00F00000) >> 20; // bits 20:23

                info_field = starWord1 & 0x000FFFFF; // 20 bits: 0-19;
                if (info_code == 4 || info_code == 7)
                    fts_hb = info_field; //  high bit (47:28) time from startrack hit strip
                if (info_code == 5 || info_code == 8)
                    fts_vhb = info_field; //  very high bit (63:48)time from startrack hit strip
                if (info_code == 14)
                    tsExt_hb = info_field; // high bit (47:28) time from the external Master
                if (info_code == 15)
                    tsExt_vhb = info_field; // very high bit (63:48)time from the external Master

                // Decoding the second 32bit word stardata2:
                if (info_code == 14 || info_code == 15)
                {
                    tsExt_lb = (starWord2 & 0x0FFFFFFF); // low bit timestamp from master trigger
                }

                // if(tsExt_vhb && tsExt_hb && tsExt_lb ) TSExt= ((uint64_t) tsExt_vhb << 47) | ((uint64_t) tsExt_hb <<
                // 28 ) | tsExt_lb; // Reconstructed full timestamp for Ext signal
                if (tsExt_vhb && tsExt_hb && tsExt_lb)
                    TSExt = ((uint64_t)tsExt_hb << 28) |
                            tsExt_lb; // Reconstructed timestamp from MSB (most significant bits) and sLSB (slightly
                                      // lower significant bits) for a startrack signal
                // so, if either tsExt_vhb, tsExt_hb or tsExt_lb happen to be zero, TSExt will not be intialized.
                // Otherwise, tsExt_vhb is ignored for the TS.
                // if this is working as intended, I do not want to know anything more about it.

                // Case of wordtype=3:
            }
            else if (((starWord1 >> 30) & 0x3) == 0x3 &&
                     ((starWord1 & 0xFFFFFFFF) != 0xFFFFFFFF)) // = case of wordtype=3
            {
                // Decoding the first 32bit word:
                wordtype = (starWord1 >> 30) & 0x3; // bit 31:30
                if (wordtype != 3)
                    LOG(info) << wordtype;

                hitbit = (starWord1 >> 29) & 0x01;

                // LOG(info) << "R3BStartrackUnpack :   hitbit :" <<  hitbit;

                adcData = (starWord1 & 0x00000FFF);

                UInt_t ADCchanIdent = (starWord1 >> 12) & 0x0001FFFF; // 17 bits after a shift of 12 bit

                ladder_id = ((ADCchanIdent >> 12) & 0x0000001F) + 0x1; // not adding 0x1 t from 1 to 30

                side = ((ADCchanIdent >> 11) & 0x00000001) + 0x1; // adding 0x1 to have real values from 1 to 2

                // Do the following test only for the S438 test run (GSI Oct. 2014), please put in comments otherwise
                if (side == 0x1)
                {
                    adcData = ((~(adcData)) & 0x00000FFF); // we invert all adcData bit and take the last 12 bits only
                }

                asic_id = ((ADCchanIdent >> 7) & 0x0000000F) + 0x1; // adding 0x1 to have real values from 1 to 16

                strip_id = (ADCchanIdent & 0x0000007F) + 0x1; // adding 0x1 to have real values from 1 to 128

                // Decoding the second 32bit word:
                // if S438 data (Oct 2014 data):
                // cout << "ts_init=" << pl_data[l_s+1] << endl;
                UInt_t ts_lb_part1 = starWord2 & 0x00000003; // take the 2 first less significant  bits;
                // cout << "ts_lb_part1=" << ts_lb_part1 << endl;
                UInt_t ts_lb_part2 = (starWord2 >> 2) & 0x00000FFF; // take the 12 bits after a shift of 2 bits
                // cout << "ts_lb_part2=" << ts_lb_part2 << endl;
                UInt_t ts_lb_part2_inv =
                    (~(ts_lb_part2)) & 0x00000FFF; // invert and take only the first 12 inverted bits;
                // cout << "ts_lb_part2_inv=" << ts_lb_part2_inv << endl;
                UInt_t ts_lb_part3 = starWord2 & 0x0FFFC000; // from bit 14 to 27;
                // cout << "ts_lb_part3=" << ts_lb_part3 << endl;
                ts_lb = (((ts_lb_part3 >> 2) + ts_lb_part2_inv) << 2) + ts_lb_part1;
                // cout << "ts_lb end=" << ts_lb << endl;

                // if(ts_vhb && ts_hb && ts_lb) TS= ((uint64_t) ts_vhb << 47) | ((uint64_t) ts_hb << 28) |  ts_lb ; //
                // Reconstructed full timestamp for a startrack signal
                if (fts_hb && ts_lb)
                    TS = ((uint64_t)fts_hb << 28) |
                         ts_lb; // Reconstructed timestamp from MSB (most significant bits) and sLSB (slightly lower
                                // significant bits) for a startrack signal
            }

            // if we had multi hit data, we would need to read
            // j=curChannelStart; j < nextChannelStart; j++.
            // For the PSPs, however, we take the first hit only:

            // Marc: Todo: from data, identify, ladder,Side,Asic.Strip.Edep.TimeStp and fill R3BSTaRTraMappedData
            // new ((*fArray)[fArray->GetEntriesFast()]) R3BStartrackMappedData(stardata1); //
            // new ((*fArray)[fArray->GetEntriesFast()]) R3BStartrackMappedData(stardata1, stardata2); //

            // A good hit should habe hitbit==1
            if (hitbit == 1)
            {
                // new ((*fArray)[fArray->GetEntriesFast()]) R3BStartrackMappedData(WRvhb, WRhb, WRlb, wordtype, hitbit,
                // ladder_id, side, asic_id, strip_id, adcData, ts_vhb, ts_hb, ts_lb, tsExt_vhb, tsExt_hb,
                // tsExt_lb,info_field, info_code); //
                new ((*fArray)[fArray->GetEntriesFast()]) R3BStartrackMappedData(TS,
                                                                                 TSExt,
                                                                                 wordtype,
                                                                                 hitbit,
                                                                                 ladder_id,
                                                                                 side,
                                                                                 asic_id,
                                                                                 strip_id,
                                                                                 adcData,
                                                                                 fts_vhb,
                                                                                 fts_hb,
                                                                                 ts_lb,
                                                                                 tsExt_vhb,
                                                                                 tsExt_hb,
                                                                                 tsExt_lb,
                                                                                 info_field,
                                                                                 info_code); //
            }
        }
    }

    return kTRUE;
}

void R3BStartrackReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

ClassImp(R3BStartrackReader)
