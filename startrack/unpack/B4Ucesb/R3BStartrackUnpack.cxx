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
    //  LOG(info) << "Subcrate=" << subCrate << endl;
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
    LOG(info) << "Registration Completed";
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

    LOG(info) << "R3BStartrackUnpack : Unpacking... size = " << size;

    UInt_t l_s = 0;
    // Int_t nInfo4 = 0;
    // Int_t nInfo5 = 0;

    UInt_t* pl_data = (UInt_t*)(data + l_s); // let's find 0x00000200

    l_s++;
    UInt_t* wr1 = (UInt_t*)(data + l_s); // MBS/WR sent by us to MBS
    l_s++;
    UInt_t* wr2 = (UInt_t*)(data + l_s); // MBS/WR sent by us to MBS
    l_s++;
    UInt_t* wr3 = (UInt_t*)(data + l_s); // MBS/WR sent by us to MBS
    l_s++;
    UInt_t* wr4 = (UInt_t*)(data + l_s); // MBS/WR sent by us to MBS
    l_s++;

    //  WRhb1: WR high bits time stamp in header
    //  WRhb2: WR high bits time stamp in header
    //  WRlb:  Wr low bits time stamp in header
    //  WR:    WR time stamp in header
    // WRhb = UInt_t( ((wr4[0] & 0x00000fff) << 16) + ((wr3[0] & 0x0000ffff) << 4) + ((wr2[0] & 0x0000f000) >> 12) ); //
    // bits: 28-59
    UInt_t WRvhb = UInt_t((wr4[0] & 0x0000ffff));                                       // bits: 48-63
    UInt_t WRhb = UInt_t(((wr3[0] & 0x0000ffff) << 4) + ((wr2[0] & 0x0000f000) >> 12)); // 20 bits: 28-47
    UInt_t WRlb = UInt_t((((wr2[0] & 0x00000fff) << 16) + (wr1[0] & 0x0000ffff)));      // 28 bits: 0-27
    // + wr3[0] << 32 + wr2[0] << 16 + wr1[0];

    // long long WR = (WRhb1 << 48) + (WRhb2 << 28) + WRlb;

    // initialisation of ts_hb1 and 2 as this is not done in the Si Tracker DAQ unless word 10 appear with 4&5 or 7&8
    // info codes.

    // ts_hb1; // 63:48 in Si (16 bits)
    // ts_hb2; // 47:28 in Si (20 bits)
    // ts_hb=0 ; // in Si
    // ts_lb=0 ; // in Si
    // long long ts=0 ; // 27:0 in Si (28 bits)

    // UInt_t ts_hb1=WRhb1; // high bits 48:63
    // UInt_t ts_hb2=WRhb2; // high bits 28:47
    // UInt_t ts_hb=WRhb2;  // (WRhb1 << 20)+WRhb2  // full high bit

    // if( pl_data[0]==0x00000200)
    //{

    // LOG(info) << "R3BStartrackUnpack :   pl_data[0]:" << pl_data[0];
    // LOG(info) << "R3BStartrackUnpack :   wr1[0]:" << wr1[0];
    // LOG(info) << "R3BStartrackUnpack :   wr2[0]:" << wr2[0];
    // LOG(info) << "R3BStartrackUnpack :   wr3[0]:" <<  wr3[0];
    // LOG(info) << "R3BStartrackUnpack :   wr4[0]:" <<  wr4[0];
    // LOG(info) << "R3BStartrackUnpack :   WRlb:" <<  WRlb;
    // LOG(info) << "R3BStartrackUnpack :   WRhb:" <<  WRhb;
    // LOG(info) << "R3BStartrackUnpack :   size:" << l_da_siz;
    //}

    /*
    UInt_t wordtype;  //
    UInt_t info_field;
    UInt_t info_code;

    UInt_t ts_vhb=0;
    UInt_t ts_hb=0;
    UInt_t ts_lb=0;
    UInt_t ts_lb_part1=0;
    UInt_t ts_lb_part2=0;
    UInt_t ts_lb_part2_inv=0;
    UInt_t ts_lb_part3=0;
    //long long ts=0;

    // Time information from an external input (ie: other than Si tracker)
    UInt_t tsExt_vhb=0; // 63:48 in Si (16 bits)
    UInt_t tsExt_hb=0; // 47:28 in Si (20 bits)
    UInt_t tsExt_lb=0; // 27:0 in Si (28 bits)
    //long long tsExt=0;

    UInt_t hitbit=0; // real values are: 0 or 1
    UInt_t module_id=0;  // module id, real values are  1 to 30
    UInt_t side=0; // real values are  0 or 1
    UInt_t asic_id=0;    // Chip id, real values are 0 to 15
    UInt_t strip_id=0;   // strip id, real values are 0 to 127
    UInt_t adcData=0;  // adc value for energy loss in Si
    */

    if (pl_data[0] == 0x00000200) // identifiant du Silicon tracker
    {

        while (l_s < size)
        {

            // pl_data = (UInt_t*)(data + l_s);

            // for (Int_t i1 = 0; i1 < 2; i1++)
            //{

            //
            // Only do a reset in order to check that the same number of time info_code=4 (7,14) and info code =5 (8,15)
            //
            /*
              info_code=0;
              info_field=0;
              tsExt_lb=tsExt_hb=tsExt_vhb=0;
              ts_hb=0;
              ts_vhb=0;
              adcData=0;
              ts_lb=0;
           */

            if ((((pl_data[l_s] >> 30) & 0x3) == 0x2) && (((pl_data[l_s] & 0xFFFFFFFF) != 0xFFFFFFFF))) // word type 2
            {

                wordtype = (pl_data[l_s] >> 30) & 0x3; // bit 31:30
                // cout << "wordtype=" << wordtype << endl;

                // info_code = (pl_data[l_s] >> 20) & 0x0000000F; //bits 20:23

                info_code = (pl_data[l_s] & 0x00F00000) >> 20; // bits 20:23

                info_field = (pl_data[l_s]) & 0x000FFFFF; // 20 bits: 0-19;

                // tsExt_lb = (pl_data[(l_s+1)] & 0x0FFFFFFF); // low bit timestamp from master trigger  (To be checked)

                if (info_code == 4 || info_code == 7)
                {
                    ts_hb = info_field;
                    ts_code47_lb = (pl_data[l_s + 1] & 0x0FFFFFFF);
                }
                if (info_code == 5 || info_code == 8)
                    ts_vhb = info_field;
                // if(info_code == 7) ts_hb=info_field;
                // if(info_code == 8) ts_vhb=info_field;

                // ts= (ts_hb1 << 48) + (ts_hb2 << 28) + ts_lb;

                if (info_code == 14)
                    tsExt_hb = info_field; // high bit (47:28) time from the external Master
                if (info_code == 15)
                    tsExt_vhb = info_field; // high bit (63:48)time from the external Master
                if (info_code == 14 || info_code == 15)
                {
                    tsExt_lb =
                        (pl_data[(l_s + 1)] & 0x0FFFFFFF); // low bit timestamp from master trigger  (To be checked)
                }

                // tsExt= (tsExt_hb1 << 48) + (tsExt_hb2 << 28) + tsExt_lb;  // full timestamp from Master trigger

                // if(info_code==4)nInfo4++;
                // if(info_code==5)nInfo5++;

                l_s += 2;

                // filling the Raw hit TClonesArray for word type 2
                new ((*fRawData)[fNHits]) R3BStartrackRawHit(WRvhb,
                                                             WRhb,
                                                             WRlb,
                                                             wordtype,
                                                             hitbit,
                                                             module_id,
                                                             side,
                                                             asic_id,
                                                             strip_id,
                                                             adcData,
                                                             ts_vhb,
                                                             ts_hb,
                                                             ts_lb,
                                                             tsExt_vhb,
                                                             tsExt_hb,
                                                             tsExt_lb,
                                                             ts_code47_lb,
                                                             info_field,
                                                             info_code);
                fNHits++;

                //
                // reset after filled the TClonesArray
                //
                info_code = info_field = 0;
                tsExt_lb = tsExt_hb = tsExt_vhb = 0;
                ts_hb = 0;
                ts_vhb = 0;
            }
            else if (((pl_data[l_s] >> 30) & 0x3) == 0x3 &&
                     ((pl_data[l_s] & 0xFFFFFFFF) != 0xFFFFFFFF)) // word of type 3
            {

                // LOG(info) << " pl_data[l_s]: " <<  ((pl_data[l_s] >> 30) & 0x3) ;

                // LOG(info) << "R3BStartrackUnpack :   wordB :" <<  (pl_data[l_s] & 0xC0000000);

                // wordtype=11 <=> 3 ;
                wordtype = (pl_data[l_s] >> 30) & 0x3; // bit 31:30
                if (wordtype != 3)
                    cout << wordtype << endl;

                hitbit = (pl_data[l_s] >> 29) & 0x01;

                // LOG(info) << "R3BStartrackUnpack :   hitbit :" <<  hitbit;

                adcData = (pl_data[l_s] & 0x00000FFF);

                UInt_t ADCchanIdent = (pl_data[l_s] >> 12) & 0x0001FFFF; // 17 bits after a shift of 12 bit

                module_id = ((ADCchanIdent >> 12) & 0x0000001F) + 0x1; // not adding 0x1 t from 1 to 30

                side = ((ADCchanIdent >> 11) & 0x00000001) + 0x1; // adding 0x1 to have real values from 1 to 2

                // Do the following test only for the S438 test run (GSI Oct. 2014), please put in comments otherwise
                if (side == 0x1)
                {
                    adcData = ((~(adcData)) & 0x00000FFF); // we invert all adcData bit and take the last 12 bits only
                }

                asic_id = ((ADCchanIdent >> 7) & 0x0000000F) + 0x1; // adding 0x1 to have real values from 1 to 16

                strip_id = (ADCchanIdent & 0x0000007F) + 0x1; // adding 0x1 to have real values from 1 to 128

                // if S438 data (Oct 2014 data):
                /*
                //cout << "ts_init=" << (pl_data[l_s+1] & 0x0FFFFFFF)  << endl;
                ts_lb_part1 = pl_data[l_s+1] & 0x00000003; //take the 2 first less significant  bits;
                //cout << "ts_lb_part1=" << ts_lb_part1 << endl;
                ts_lb_part2= (pl_data[l_s+1] >> 2) & 0x00000FFF; // take the 12 bits after a shift of 2 bits
                //cout << "ts_lb_part2=" << ts_lb_part2 << endl;
                ts_lb_part2_inv=  (~(ts_lb_part2)) & 0x00000FFF; // invert and take only the first 12 inverted bits;
                //cout << "ts_lb_part2_inv=" << ts_lb_part2_inv << endl;
                ts_lb_part3 = pl_data[l_s+1] & 0x0FFFC000; //from bit 14 to 27;
                //cout << "ts_lb_part3=" << ts_lb_part3 << endl;
                ts_lb= (((ts_lb_part3 >> 2) + ts_lb_part2_inv) << 2)  + ts_lb_part1 ;
                */
                // cout << "ts_lb end=" << ts_lb << endl;

                // if not S438 data (ie after Oct 2014):
                ts_lb = pl_data[l_s + 1] & 0x0FFFFFFF; // low bit time stamp in Silicon

                // LOG(info) << "R3BStartrackUnpack :   pl_data[ls+1]:" << pl_data[l_s+1];

                l_s += 2;

                // LOG(debug) << "R3BStartrackerUnpack : Strip_ID IS " << strip_id << ",  Chip ID IS " << asic_id << " ,
                // Ladder ID IS " << module_id << " , ADC Data IS " << adcData; new ((*fRawData)[fNHits])
                // R3BStartrackRawHit(module_id, side, asic_id, strip_id, adcData, lclock);

                // new ((*fRawData)[fNHits]) R3BStartrackRawHit(wordtype, hitbit, module_id, side, asic_id, strip_id,
                // adcData, ts2); new ((*fRawData)[fNHits]) R3BStartrackRawHit(WRhb2,WRlb, wordtype, hitbit, module_id,
                // side, asic_id, strip_id, adcData, ts_hb, ts_lb,tsExt_hb , tsExt_lb,info_field, info_code); new
                // ((*fRawData)[fNHits]) R3BStartrackRawHit(WR,WRlb, wordtype, hitbit, module_id, side, asic_id,
                // strip_id, adcData, ts, ts_lb, tsExt, tsExt_lb,info_field, info_code); fNHits++;

                // Filling the TCloneArray for word type 3
                new ((*fRawData)[fNHits]) R3BStartrackRawHit(WRvhb,
                                                             WRhb,
                                                             WRlb,
                                                             wordtype,
                                                             hitbit,
                                                             module_id,
                                                             side,
                                                             asic_id,
                                                             strip_id,
                                                             adcData,
                                                             ts_vhb,
                                                             ts_hb,
                                                             ts_lb,
                                                             tsExt_vhb,
                                                             tsExt_hb,
                                                             tsExt_lb,
                                                             ts_code47_lb,
                                                             info_field,
                                                             info_code);
                fNHits++;

                // Resetting after filling the TClonesArray:
                adcData = 0;
                ts_lb = 0;
            }
            else
            {
                LOG(info) << " Warning:  Word not recognised !!!  :";

                LOG(info) << " pl_data[l_s-2]: " << ((pl_data[l_s - 2]) & 0xFFFFFFFF);
                LOG(info) << " pl_data[l_s-1]: " << ((pl_data[l_s - 1]) & 0xFFFFFFFF);
                LOG(info) << " pl_data[l_s]: " << ((pl_data[l_s]) & 0xFFFFFFFF);
                LOG(info) << " pl_data[l_s+1]: " << ((pl_data[l_s + 1]) & 0xFFFFFFFF);
                LOG(info) << " pl_data[l_s+2]: " << ((pl_data[l_s + 2]) & 0xFFFFFFFF);
                LOG(info) << " pl_data[l_s+3]: " << ((pl_data[l_s + 3]) & 0xFFFFFFFF);
                LOG(info) << " pl_data[l_s+14]: " << ((pl_data[l_s + 14]) & 0xFFFFFFFF);
                LOG(info) << " pl_data[l_s+15]: " << ((pl_data[l_s + 15]) & 0xFFFFFFFF);

                l_s++; // move to next word
            }

            //}

            // cout << "NHits= "<< fNHits  << endl;
            // cout << "nInfor4= "<< nInfo4  << " nInfo5= "<< nInfo5 << endl;
        }
    }

    LOG(info) << "R3BStartrackUnpack : Number of hits in Startracker: " << fNHits;
    return kTRUE;
}

// DoUnpack2: Public method  (use for R3B Si tracker and data file from DL)
// Each block_main should contain a pair of words:

/*
Bool_t R3BStartrackUnpack::DoUnpack2(Int_t *data_word0, Int_t *data_word1, Int_t size)
{

  LOG(info) << "R3BStartrackUnpack2 : Unpacking... size = " << size;

  UInt_t l_s = 0;

  UInt_t *pl_data_word0 = (UInt_t*) data_word0;
  UInt_t *pl_data_word1 = (UInt_t*) data_word1;

  UInt_t wordtype;  // 11 or 10
  //UInt_t evsize;
  UInt_t info_field=0;
  UInt_t info_code=0;
  UInt_t timestamp;
  UInt_t energy=0;  // energy loss in Si
  UInt_t side=0;  // module side
  UInt_t channel_id=0;  // Channel id
  UInt_t asic_id=0;  // Chip id
  UInt_t module_id=0;  // module id
  UInt_t hitbit=0;  // 0 or 1



  //cout << "size=" << size << endl;
  //cout << "data_word0=" << *data_word0 << endl;
  //cout << "data_word1=" << *data_word1 << endl;
  //cout << "data_word0=" << *pl_data_word0 << endl;
  //cout << "data_word1=" << *pl_data_word1 << endl;


  //LOG(info) << "Unpacking Startracker data";
  LOG(debug) << "Unpacking Startracker data";


  wordtype = (*data_word0 >> 30) & 0x3; // bit 31:30

  if(wordtype==2) LOG(info) << "Words type 2(10) or 3(11)=" << wordtype;

  // Check if word_0 begins with:
  // - 10 then is type A word.
  // - 11 then is type B word.

  //while(l_s < size) {
  //  l_s++;

      // A (10):
      if ( (*data_word0 & 0xC0000000)==0x80000000 && ( ((*data_word0 & 0xFFFFFFFF) != 0xFFFFFFFF) && ((*data_word1 &
0xFFFFFFFF) != 0xFFFFFFFF)) ){
    //cout << "Words type A (msb=10)." << std::endl;
    LOG(debug) << "Words type A (msb=10).";
    word_0A=*pl_data_word0;
    word_1A=*pl_data_word1;

    while(l_s < size) {
    l_s++;


    // Check the trailer: reject or keep the block.
    //if( (*data_word0 & 0xFFFFFFFF) == 0xFFFFFFFF ||
    //  (*data_word1 & 0xFFFFFFFF) == 0xFFFFFFFF) {
    //  // this marks the point after which there is no more good data in the block_main
    //  // log_file << "End of block " << itr_1 << std::endl;
    //  //LOG(info) << "End of block ";
    //  //flag_terminator=true;
    //  break;
    //  }
    //

    info_field = word_0A & 0x000FFFFF; //bits 0:19
    info_code = (word_0A >> 20) & 0x0000000F; //bits 20:23
    module_id = (word_0A >> 24) & 0x0000003F; //bits 24:29

    // Extract time stamp.
    timestamp =  (unsigned long) (word_1A & 0x0FFFFFFF);

    // LOG(info) << "Info_field " << info_field;

    }  // end of while(l_s<size)

      } //end of A word case

      // B (11):
      if ( (*data_word0 & 0xC0000000)==0xC0000000 && ( ((*data_word0 & 0xFFFFFFFF) != 0xFFFFFFFF) && ((*data_word1 &
0xFFFFFFFF) != 0xFFFFFFFF)) ){
    //cout << "Words type B (msb=11)."<< std::endl;
    LOG(debug) << "Words type B (msb=11)."<< std::endl;
    word_0B=*pl_data_word0;
    word_1B=*pl_data_word1;


    while(l_s < size) {
    l_s++;

    // Check the trailer: reject or keep the block.

    //  if( (*data_word0 & 0xFFFFFFFF) == 0xFFFFFFFF ||
    //  (*data_word1 & 0xFFFFFFFF) == 0xFFFFFFFF) {
    //  // this marks the point after which there is no more good data in the block_main
    //  // log_file << "End of block " << itr_1 << std::endl;
    //  //LOG(info) << "End of block ";
    //  //flag_terminator=true;
    //  break;
    //  }


    LOG(debug) << "At GOSIP memory";

    // Real R3B Si Tracker data channel

    // Extract time stamp.
    // Msb from word_0A bits 0-19.
    // Lsb from word_0B bits 0-27.
    //timestamp =  (unsigned long long int) (word_0A & 0x000FFFFF) << 28 | (word_1B & 0x0FFFFFFF);
    timestamp =  (unsigned long) (word_1B & 0x0FFFFFFF);
    //timestamp =  (unsigned long long int) (word_1A & 0x000FFFFF) << 28 | (word_1B & 0x0FFFFFFF);

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
    //module_id = (word_0B & 0x1F800000) >> 23;
    // From word_2: 5 bits (24-28).
    module_id = (word_0B & 0x1F000000) >> 24;

    // Extract hit bit .
    // From word_2: bit 29.
    hitbit = (word_0B & 0x20000000) >> 29;

    } // end of while(l_s<size) for B


      } // end of B word case

        //LOG(info) << " --------- event "
    LOG(debug) << " --------- event "
    << "        hitbit " << hitbit
    << "        Channel_id " << channel_id
    << "        ASIC_id " << asic_id
    << "        Side " << side
    << "        Module_id " << module_id
    << "        energy " << energy
    << "        timestamp " << timestamp
    << "=================================";

    //new ((*fRawData)[fNHits]) R3BStartrackRawHit(channel_id, asic_id, module_id, energy, timestamp);
    //new ((*fRawData)[fNHits]) R3BStartrackRawHit(wordtype, hitbit, module_id, side, asic_id, channel_id, energy,
timestamp);
    //new ((*fRawData)[fNHits]) R3BStartrackRawHit(wordtype, hitbit, module_id, side, asic_id, channel_id, energy,
timestamp, info_field, info_code); new ((*fRawData)[fNHits]) R3BStartrackRawHit(timestamp, timestamp, wordtype, hitbit,
module_id, side, asic_id, channel_id, energy, timestamp, timestamp,timestamp, timestamp, info_field, info_code);
    fNHits++;



  LOG(debug) << "End of memory";
  LOG(debug) << "R3BStartrackUnpack: Number of Si Tracker raw hits: " << fNHits;
  //LOG(info) << "R3BStartrackUnpack: Number of Si Tracker raw hits: " << fNHits;


  return kTRUE;
}
*/

// Reset: Public method
void R3BStartrackUnpack::Reset()
{
    LOG(debug) << "Clearing Data Structure";
    fRawData->Clear();
    fNHits = 0;
}

ClassImp(R3BStartrackUnpack)
