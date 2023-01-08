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

// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                        R3BStartrackUnpack                         -----
// -----                           Version 0.1                             -----
// -----                    Created 28.05.2014 by M. Labiche               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

// ROOT headers
#include "TClonesArray.h"
#include "TH1F.h"
#include "TMath.h"

// Fair headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunOnline.h"

// Startracker headers
#include "R3BStartrackRawHit.h"
#include "R3BStartrackUnpack.h"
//#include "R3BSTaRTraHit.h"

//#define NUMCHANNELS 1500
//#define NUMMODULES 1500

#include <iostream>
using namespace std;

// R3BStartrackUnpack: Constructor
R3BStartrackUnpack::R3BStartrackUnpack(char* strTraDir,
                                       Short_t type,
                                       Short_t subType,
                                       Short_t procId,
                                       Short_t subCrate,
                                       Short_t control)
    : FairUnpack(type, subType, procId, subCrate, control)
    , fRawData(new TClonesArray("R3BStartrackRawHit"))
    , fNHits(0)
{
}

// Virtual R3BStartrackUnpack: Public method
R3BStartrackUnpack::~R3BStartrackUnpack()
{
    LOG(info) << "R3BStartrackUnpack: Delete instance";
    delete fRawData;
}

// Init: Public method
Bool_t R3BStartrackUnpack::Init()
{
    Register();
    return kTRUE;
}

// Register: Protected method
void R3BStartrackUnpack::Register()
{
    LOG(info) << "Registration of Startrack Unpacker";
    FairRootManager* fMan = FairRootManager::Instance();
    if (!fMan)
    {
        return;
    }
    fMan->Register("StartrackRawHit", "Raw data from R3B Si Tracker", fRawData, kTRUE);
}

// DoUnpack: Public method
Bool_t R3BStartrackUnpack::DoUnpack(Int_t* data, Int_t size) // used for Mbs format datafile
{

    // TODO: adapt it for Tracker when  data format is known for tracker

    // LOG(info) << "R3BStartrackUnpack : Unpacking... size = " << size;

    UInt_t l_s = 0;
    UInt_t* pl_data = (UInt_t*)(data + l_s);

    UInt_t wordtype; //
    UInt_t info_field = 0;
    UInt_t info_code = 0;
    UInt_t hitbit = 0;
    UInt_t module_id = 0; // module id
    UInt_t side = 0;
    UInt_t asic_id = 0;  // Chip id
    UInt_t strip_id = 0; // strip id
    UInt_t adcData = 0;  // adc value for energy loss in Si
    UInt_t ts1 = 0;
    UInt_t ts2 = 0;

    while (l_s < size)
    {

        // UInt_t* p1 = (UInt_t*)(data + l_s);
        // UInt_t l_sam_id = (p1[0] & 0xf0000000) >> 28; // identifies the sam
        // UInt_t l_gtb_id = (p1[0] & 0x0f000000) >> 24; // 0 or 1, identifies which of the 2 cables of the sam
        // UInt_t l_lec = (p1[0] & 0x00f00000) >> 20;

        UInt_t l_da_siz = (pl_data[0] & 0x000001ff); // To be changed for tracker maybe ?
        // UInt_t l_da_siz = 1;  // = size ; To be changed for tracker maybe ?

        // LOG(info) << "R3BStartrackUnpack :   size:" << l_da_siz;

        l_s += 1;

        pl_data = (UInt_t*)(data + l_s);

        for (Int_t i1 = 0; i1 < l_da_siz; i1 += 2)
        {
            // data starting with the 2 first high bit at 10 (=2):  (l_s=0)
            // if( (pl_data[l_s] & 0xC0000000) == 0x80000000 && ( (pl_data[l_s] & 0xFFFFFFFF) != 0xFFFFFFFF ) )  //
            // 0x8=1000
            if (((pl_data[l_s] >> 30) & 0x3) == 0x2 && ((pl_data[l_s] & 0xFFFFFFFF) != 0xFFFFFFFF)) // 0x8=1000
            {
                // LOG(info) << "R3BStartrackUnpack :   wordA :" <<  (pl_data[l_s] & 0x80000000);
                // wordtype=10;
                wordtype = (pl_data[l_s] >> 30) & 0x3; // bit 31:30

                info_field = pl_data[l_s] & 0x000FFFFF;
                module_id = (pl_data[l_s] >> 24) & 0x0000003F;
                info_code = (pl_data[l_s] >> 20) & 0x0000000F;
                // wordtype=10;
                wordtype = (pl_data[l_s] >> 30) & 0x3; // bit 31:30

                // read timestamp High bit
                // ts1= (pl_data[l_s] & 0x000FFFFF);
                // LOG(info) << "R3BStartrackUnpack :   ts1 :" <<  ts1;
            }

            // data word starting with the 2 first high bit at 11 (=3):
            // if( (pl_data[l_s] & 0xC0000000) == 0xC0000000 && ( (pl_data[l_s] & 0xFFFFFFFF) != 0xFFFFFFFF ) )  //
            // 0x8=1000

            if ((((pl_data[l_s] >> 30) & 0x3) == 3) && (((pl_data[l_s] & 0xFFFFFFFF) != 0xFFFFFFFF)))
            {
                // LOG(info) << " pl_data: " <<  ((pl_data[l_s] >> 30) & 0x3) ;

                // LOG(info) << "R3BStartrackUnpack :   wordB :" <<  (pl_data[l_s] & 0xC0000000);

                // wordtype=11;
                wordtype = (pl_data[l_s] >> 30) & 0x3; // bit 31:30
                if (wordtype != 3)
                    cout << wordtype << endl;

                hitbit = (pl_data[l_s] >> 29) & 0x01;

                // LOG(info) << "R3BStartrackUnpack :   hitbit :" <<  hitbit;

                adcData = (pl_data[l_s] & 0x00000FFF);

                UInt_t ADCchanIdent = (pl_data[l_s] >> 12) & 0x0001FFFF; // 17 bits after a shift of 12 bit

                module_id = (ADCchanIdent >> 12) & 0x0000001F;

                side = (ADCchanIdent >> 11) & 0x00000001;

                asic_id = (ADCchanIdent >> 7) & 0x0000000F;

                strip_id = ADCchanIdent & 0x0000007F;
            }
            else if (((pl_data[l_s] >> 30) & 0x3) == 0x2 && ((pl_data[l_s] & 0xFFFFFFFF) != 0xFFFFFFFF))
            {
                wordtype = (pl_data[l_s] >> 30) & 0x3;    // bit 31:30
                info_field = (pl_data[l_s]) & 0x000FFFFF; // bits 0:19;

                module_id = (pl_data[l_s] >> 24) & 0x0000003F;
                info_code = (pl_data[l_s] >> 20) & 0x0000000F; // bits 20:23
            }

            // read timestamp low bit
            if (((pl_data[l_s] >> 28) & 0xF) == 0x0)
                ts2 = (pl_data[(l_s)] & 0x0FFFFFFF);

            l_s += 2;

            // l_s++;

            // LOG(debug) << "R3BStartrackerUnpack : Strip_ID IS " << strip_id << ",  Chip ID IS " << asic_id << " ,
            // Ladder ID IS " << module_id << " , ADC Data IS " << adcData; new ((*fRawData)[fNHits])
            // R3BStartrackRawHit(module_id, side, asic_id, strip_id, adcData, lclock);

            // new ((*fRawData)[fNHits]) R3BStartrackRawHit(wordtype, hitbit, module_id, side, asic_id, strip_id,
            // adcData, lclock); new ((*fRawData)[fNHits]) R3BStartrackRawHit(wordtype, hitbit, module_id, side,
            // asic_id, strip_id, adcData, ts2);
            new ((*fRawData)[fNHits]) R3BStartrackRawHit(
                wordtype, hitbit, module_id, side, asic_id, strip_id, adcData, ts2, info_field, info_code);
            fNHits++;
        }
    }

    // LOG(info) << "R3BStartrackUnpack : Number of hits in Startracker: " << fNHits;
    return kTRUE;
}

// DoUnpack2: Public method  (use for R3B Si tracker and data file from DL)
// Each block_main should contain a pair of words:
/**/

Bool_t R3BStartrackUnpack::DoUnpack2(Int_t* data_word0, Int_t* data_word1, Int_t size)
{

    LOG(info) << "R3BStartrackUnpack2 : Unpacking... size = " << size;

    UInt_t l_s = 0;

    UInt_t* pl_data_word0 = (UInt_t*)data_word0;
    UInt_t* pl_data_word1 = (UInt_t*)data_word1;

    UInt_t wordtype; // 11 or 10
    // UInt_t evsize;
    UInt_t info_field = 0;
    UInt_t info_code = 0;
    UInt_t timestamp;
    UInt_t energy = 0;     // energy loss in Si
    UInt_t side = 0;       // module side
    UInt_t channel_id = 0; // Channel id
    UInt_t asic_id = 0;    // Chip id
    UInt_t module_id = 0;  // module id
    UInt_t hitbit = 0;     // 0 or 1

    // cout << "size=" << size << endl;
    // cout << "data_word0=" << *data_word0 << endl;
    // cout << "data_word1=" << *data_word1 << endl;
    // cout << "data_word0=" << *pl_data_word0 << endl;
    // cout << "data_word1=" << *pl_data_word1 << endl;

    // LOG(info) << "Unpacking Startracker data";
    LOG(debug) << "Unpacking Startracker data";

    wordtype = (*data_word0 >> 30) & 0x3; // bit 31:30

    if (wordtype == 2)
        LOG(info) << "Words type 2(10) or 3(11)=" << wordtype;

    // Check if word_0 begins with:
    // - 10 then is type A word.
    // - 11 then is type B word.

    // while(l_s < size) {
    //  l_s++;

    // A (10):
    if ((*data_word0 & 0xC0000000) == 0x80000000 &&
        (((*data_word0 & 0xFFFFFFFF) != 0xFFFFFFFF) && ((*data_word1 & 0xFFFFFFFF) != 0xFFFFFFFF)))
    {
        // cout << "Words type A (msb=10)." << std::endl;
        LOG(debug) << "Words type A (msb=10).";
        word_0A = *pl_data_word0;
        word_1A = *pl_data_word1;

        while (l_s < size)
        {
            l_s++;

            // Check the trailer: reject or keep the block.
            /*if( (*data_word0 & 0xFFFFFFFF) == 0xFFFFFFFF ||
              (*data_word1 & 0xFFFFFFFF) == 0xFFFFFFFF) {
              // this marks the point after which there is no more good data in the block_main
              // log_file << "End of block " << itr_1 << std::endl;
              //LOG(info) << "End of block ";
              //flag_terminator=true;
              break;
              }
            */

            info_field = word_0A & 0x000FFFFF;        // bits 0:19
            info_code = (word_0A >> 20) & 0x0000000F; // bits 20:23
            module_id = (word_0A >> 24) & 0x0000003F; // bits 24:29

            // Extract time stamp.
            timestamp = (unsigned long)(word_1A & 0x0FFFFFFF);

            // LOG(info) << "Info_field " << info_field;

        } // end of while(l_s<size)

    } // end of A word case

    // B (11):
    if ((*data_word0 & 0xC0000000) == 0xC0000000 &&
        (((*data_word0 & 0xFFFFFFFF) != 0xFFFFFFFF) && ((*data_word1 & 0xFFFFFFFF) != 0xFFFFFFFF)))
    {
        // cout << "Words type B (msb=11)."<< std::endl;
        LOG(debug) << "Words type B (msb=11)." << std::endl;
        word_0B = *pl_data_word0;
        word_1B = *pl_data_word1;

        while (l_s < size)
        {
            l_s++;

            // Check the trailer: reject or keep the block.
            /*
              if( (*data_word0 & 0xFFFFFFFF) == 0xFFFFFFFF ||
              (*data_word1 & 0xFFFFFFFF) == 0xFFFFFFFF) {
              // this marks the point after which there is no more good data in the block_main
              // log_file << "End of block " << itr_1 << std::endl;
              //LOG(info) << "End of block ";
              //flag_terminator=true;
              break;
              }
            */

            LOG(debug) << "At GOSIP memory";

            // Real R3B Si Tracker data channel

            // Extract time stamp.
            // Msb from word_0A bits 0-19.
            // Lsb from word_0B bits 0-27.
            // timestamp =  (unsigned long long int) (word_0A & 0x000FFFFF) << 28 | (word_1B & 0x0FFFFFFF);
            timestamp = (unsigned long)(word_1B & 0x0FFFFFFF);
            // timestamp =  (unsigned long long int) (word_1A & 0x000FFFFF) << 28 | (word_1B & 0x0FFFFFFF);

            // Extract Energy (ADC counts).
            // From word_2: 12 bits (0-11) = ADC_data
            energy = (word_0B & 0x00000FFF);

            // Extract channel id.
            // From word_2: 7 bits (12-18).
            channel_id = (word_0B & 0x0007F000) >> 12;

            // Extract ASIC id.
            // From word_2: 4bits (19-22).
            asic_id = (word_0B & 0x00780000) >> 19;

            // Extract Side.
            // From word_2: 1 bit (23).
            side = (word_0B & 0x00800000) >> 23;

            // Extract module id.
            // From word_2 bits 23-28.
            // module_id = (word_0B & 0x1F800000) >> 23;
            // From word_2: 5 bits (24-28).
            module_id = (word_0B & 0x1F000000) >> 24;

            // Extract hit bit .
            // From word_2: bit 29.
            hitbit = (word_0B & 0x20000000) >> 29;

        } // end of while(l_s<size) for B

    } // end of B word case

    // LOG(info) << " --------- event "
    LOG(debug) << " --------- event "
               << "        hitbit " << hitbit << "        Channel_id " << channel_id << "        ASIC_id " << asic_id
               << "        Side " << side << "        Module_id " << module_id << "        energy " << energy
               << "        timestamp " << timestamp << "=================================";

    // new ((*fRawData)[fNHits]) R3BStartrackRawHit(channel_id, asic_id, module_id, energy, timestamp);
    // new ((*fRawData)[fNHits]) R3BStartrackRawHit(wordtype, hitbit, module_id, side, asic_id, channel_id, energy,
    // timestamp);
    new ((*fRawData)[fNHits]) R3BStartrackRawHit(
        wordtype, hitbit, module_id, side, asic_id, channel_id, energy, timestamp, info_field, info_code);
    fNHits++;

    LOG(debug) << "End of memory";
    LOG(debug) << "R3BStartrackUnpack: Number of Si Tracker raw hits: " << fNHits;
    // LOG(info) << "R3BStartrackUnpack: Number of Si Tracker raw hits: " << fNHits;

    return kTRUE;
}
/**/

// Reset: Public method
void R3BStartrackUnpack::Reset()
{
    LOG(debug) << "Clearing Data Structure";
    fRawData->Clear();
    fNHits = 0;
}

ClassImp(R3BStartrackUnpack)
