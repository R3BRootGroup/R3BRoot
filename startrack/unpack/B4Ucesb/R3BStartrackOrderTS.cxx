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

//
//
// This task order the Si tracker data in time according to the data TimeStamp, during the analysis of lmd files
// If this task is not performed, the data is not always ordered in time
//
// Note, since this task is applied data block by data block, the data is only order within the current block.
//
// This task should become obsolete when the ASIC hdml programming is comppleted
//
//
//

#include "R3BStartrackOrderTS.h"
#include <FairRootManager.h>
using namespace std;

#include "TCanvas.h"
#include "TClonesArray.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"

#include "TString.h"

#include "R3BStartrackRawHit.h"

#include "FairLogger.h"

//#include "R3BTofCalPar.h"
//#include "R3BTofTCalPar.h"
#include "FairRuntimeDb.h"

#include "FairRunOnline.h"

#include <bitset>
#include <iostream>
#include <vector>

// Default constructor
R3BStartrackOrderTS::R3BStartrackOrderTS()
    : FairTask("R3BStartrackOrderTS", 1)
    , fRawData(NULL)
    , fNevents(0)
    ,             // This is actually the number of blocks
    fTotalHits(0) //, This is the accumulater number of word of type 3 in the run
// fCal_Par(NULL)
{
}

// Standard constructor
R3BStartrackOrderTS::R3BStartrackOrderTS(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fRawData(NULL)
    , fNevents(0)
    , fTotalHits(0)
    , fCount_45_78(0)
    , //
    fCount_rejectA(0)
    , //
    fCount_rejectB(0)
    ,                  //
    fCount_disorder(0) //
// fCal_Par(NULL)
{
}

// Destructor
R3BStartrackOrderTS::~R3BStartrackOrderTS() {}

InitStatus R3BStartrackOrderTS::Init()
{

    FairRootManager* mgr = FairRootManager::Instance(); // singleton class
    FairRunOnline* run = FairRunOnline::Instance();     // Added for web-base event display

    fRawData = (TClonesArray*)mgr->GetObject(
        "StartrackRawHit"); // StartrackRawHit is the name of the branch object in the tree to get the information from

    // fCal_Par = (R3BTofCalPar*)FairRuntimeDb::instance()->getContainer("StartrackCalPar");
    // fCal_Par->setChanged();

    if (!fRawData)
        return kFATAL;

    // Creating Histograms

    // For Run 280-3364 (C target)

    TS = new TH1F("TS", "Time Stamp reconstructed from hit strips (sec)", 750, 1267370, 1267450.); // with vhb
    TSext =
        new TH1F("TSext", "Time Stamp reconstructed from external signal (sec) ", 750, 1267370, 1267450.); // with vhb
    TS_p = new TH1F("TS_p", "Time Stamp reconstructed from hit strips (sec) ", 750, 1267370, 1267450.);    // with vhb
    TSext_p =
        new TH1F("TSext_p", "Time Stamp reconstructed from external signal (sec) ", 750, 1267370, 1267450.); // with vhb
    TS_n = new TH1F("TS_n", "Time Stamp reconstructed from hit strips (sec) ", 750, 1267370, 1267450.);      // with vhb
    TSext_n =
        new TH1F("TSext_n", "Time Stamp reconstructed from external signal (sec)", 750, 1267370, 1267450.); // with vhb

    ADC = new TH1F("ADC", "ADC values ", 400, 0, 4000.);

    TS_TSext_diff = new TH1F("TS_TSext_diff", "Time stamp difference (Si - Ext trigger) ", 5000, -20., 20.);
    TS_TSext = new TH2F("TS_TSext", " TS vs TSext", 750, 1267370., 1267450., 750, 1267370, 1267450.); // with vhb

    TS_event = new TH2F("TS_event", " TS vs hit#", 4100, 1., 410000., 750, 1267370, 1267450.);              // with vhb
    TS_event_reduce = new TH2F("TS_event_reduce", " TS vs hit#", 5000, 1., 50000., 100, 1267370, 1267380.); // with vhb

    TSext_event = new TH2F("TSext_event", " TSext vs hit#", 4100, 1., 410000., 750, 1267370, 1267450.); // without vhb
    ADC_TS = new TH2F("ADC_TS", " ADC value vs TS", 750, 1267370, 1267450., 4000, 1., 4000.);           // without vhb
    ADC_TS_p = new TH2F("ADC_TS_p", " ADC value vs TS ", 750, 1267370, 1267450., 4000, 1., 4000.);      // without vhb
    ADC_TS_n = new TH2F("ADC_TS_n", " ADC value vs TS ", 750, 1267370, 1267450., 4000, 1., 4000.);      // without vhb

    Asic_Side = new TH2F("Asic_Side", " Asic ID vs Det. Side ", 5, 0, 5, 15, 0., 15.);

    run->AddObject(TS);
    run->AddObject(TSext);
    run->AddObject(TS_p);
    run->AddObject(TSext_p);
    run->AddObject(TS_n);
    run->AddObject(TSext_n);
    run->AddObject(ADC);
    run->AddObject(TS_TSext_diff);
    run->AddObject(TS_TSext);
    run->AddObject(TS_event);
    run->AddObject(TS_event_reduce);
    run->AddObject(TSext_event);
    run->AddObject(ADC_TS);
    run->AddObject(ADC_TS_p);
    run->AddObject(ADC_TS_n);

    // Define output tree

    // creating the time sorted File and Tree for the Si tracker

    // output File and Tree for the analysis result
    // outFile = new TFile("Out.root","RECREATE");

    output_Tree = new TTree("cbmsimTSsorted", "cbmsimTSsorted"); // cbmsim to keep same tree structure than raw tree.

    // Defining outT branches
    output_Tree->Branch("StartrackOrderTS",
                        &s_entry.tm_stp,
                        "tm_stp/L:tm_stp_ext/L:nhit/I:type/I:hit/I:ModuleId/I:Side/I:AsicId/I:StripId/I:ADCdata/"
                        "I:sync_flag/O:pause_flag/O");

    run->AddObject(output_Tree);

    // outFile->cd();

    // Add some initialisation:

    my_tm_stp = 0;
    my_wordtype = -1; //
    my_hitbit = -1;
    my_det_id = -1;
    my_side_id = -1;
    my_asic_id = -1;
    my_ch_id = -1;
    my_adc_data = -1;
    my_sync_flag = false;
    my_pause_flag = false;
    my_tm_stp_ext = 0;

    my_new_wordtype = -1; //
    my_new_hitbit = -1;
    my_new_det_id = -1;
    my_new_side_id = -1;
    my_new_asic_id = -1;
    my_new_ch_id = -1;
    my_new_adc_data = -1;

    my_info_code = 0;
    my_info_field = 0;
    my_tm_stp_hb = 0;
    my_tm_stp_vhb = 0;
    my_tm_stp_lb = 0;

    word2_rec_time = 0;

    // for extarnal timestamp trigger
    my_tm_stp_Ext_hb = 0;
    my_tm_stp_Ext_vhb = 0;
    my_tm_stp_Ext_lb = 0;

    // arrays to check synchronization is going fine (or not!)
    for (int j = 0; j < 30; j++)
    {
        tm_stp_msb_modules[j] = 0; // 30 = total number of ladders
    }
    my_tm_stp_msb = 0; // most significant bits of time-stamp (in info code)

    // Info3= false;
    ts_sort_flag = false;
    FirstSort_flag = false;
    // FirstSort_flag4= false;
    // FirstSort_flag7= false;
    Word3 = false;
    Info4 = false;

    // n_first=0;
    // n_loop=0;

    return kSUCCESS;
}

// Add NULL Checks

// kSUCCESS if no error occure (during the initialisation for instance)
// kERROR  if error is not crucial
// kFATAL  if error is crucial

void R3BStartrackOrderTS::Exec(Option_t* option)
{

    // loop over input data
    R3BStartrackRawHit* item;
    R3BStartrackRawHit* item2;

    int difflb = 0, diff45, diff78;

    Int_t nItems = fRawData->GetEntriesFast(); // returns the number of hits in the block

    cout << "nItems=" << nItems << endl;

    //
    // Below is the equivalent of the root macros: macros/r3b/Startrack/StartrackTimeOrder.C :
    //

    //
    // all things for ordering data by time stamp
    //

    // Reseting at the begining of a new data block
    ts_sort_flag = false;
    FirstSort_flag = false;
    // FirstSort_flag4= false;
    // FirstSort_flag7= false;
    Word3 = false;

    Info4 = false; // Set Info 4 to false at the beginning of each block reading so the first word type 3 (adc) are
                   // ignored until code=4 is found

    cout << "Info4 or 7=" << Info4 << endl;

    vector<long long> ts_temp;
    vector<long long> index_block_temp, index_hit_temp;
    ts_temp.reserve(2048);
    // index_block_temp.reserve(2048);
    index_hit_temp.reserve(2048);
    const int MAXsort = 50000; // do we need maximum number when we do not want to sort any more!?

    unsigned long long my_tm_stp_5;
    unsigned long my_tm_stp_lb_new;
    unsigned long my_tm_stp_lb_cor;
    unsigned long my_tm_stp_hb_cor;
    unsigned long my_tm_stp_vhb_cor;
    unsigned long ts_lb_part1;
    unsigned long ts_lb_part2;
    unsigned long ts_lb_part2_inv;
    unsigned long ts_lb_part3;

    for (Int_t i = 0; i < nItems; i++)
    {
        item = (R3BStartrackRawHit*)fRawData->At(i);

        if (NULL == item)
        {
            continue;
        } // means just ignore the currnt i and move to the other

        // no_sync_flag=true;

        my_wordtype = item->GetWordType();

        cout << "#################" << endl;
        cout << "my_wordtype=" << my_wordtype << endl;

        //
        // Word type 3 (11):
        //

        if (my_wordtype == 3)
            my_det_id = item->GetModuleId();

        if (my_wordtype == 3 && !Info4)
            fCount_rejectA++;

        if (my_wordtype == 3 && Info4)
        { // ie:  my_wordtype=11   and  with the condition Info4=true, we ignore data before the first info_code=4 (or
          // 7) appeared

            if (Info4)
                Word3 = true; //  Info4 is already true (ie: a word of type 2 has appeared already in the datastream)
                              //  and now we found a word of type 3 so we put Word3=true

            difflb = (item->GetTimelb()) - tm_stp_code47_lb; // calculate the difference of adc low-bit ts and the last
                                                             // low-bit ts from info_code 4/5 or 7/8.

            if (abs(difflb) >= 1.e8)
            {
                // abs(difflb) >= 1.e8: Sometime (not sure why?), in between 2 sets of word of type 2 (ie:code4/5 or
                // 7/8), the first ADC word (type 3) has a low-bit time stamp which is close to the max range of the
                // low-bit time stamp (28 bit) suggesting that this adc word should be recombined with the previous
                // high-bit ts (ie: bit 20 to 48).
                // So here we check that the difference is not the same order of magnitude than the low-bit range (28
                // bits <=> 2^28=268435456) if it is we ignore that adc word.
                // cout << "!!!!!!!!!!!! hit: " <<  fTotalHits++ << " " << item->GetTimelb()  << " " << tm_stp_code47_lb
                // << " " << diff << endl;
                ts_sort_flag = false;
                fCount_rejectB++;
            }
            else
            {

                // Get Si tracker data:
                my_det_id = item->GetModuleId();
                my_hitbit = item->GetHitBit();
                my_side_id = item->GetSide();
                my_asic_id = item->GetAsicId();
                my_ch_id = item->GetStripId();
                my_adc_data = item->GetADCdata();

                // my_tm_stp_lb=(item->GetTimelb());
                my_tm_stp_lb =
                    (item->GetTimelb()); // this includes the factor 5 as Vic already * by 5 before sending to MBS
                cout << "my_tm_stp_lb=" << my_tm_stp_lb << endl;

                my_tm_stp = ((my_tm_stp_vhb << 48) | ((tm_stp_msb_modules[my_det_id] << 28) | (my_tm_stp_lb)));
                cout << "my_tm_stp from lmd before divided by 5=" << my_tm_stp << endl;

                /* if S438 data only (Oct 2014 data):                    */
                /* One needs to correct some inverted bits in the low-bit time stamp  of ADC words (type 3) and only. */
                /* There is no need to do the same for non adc words (ie: word type 2) as there's no inversion there */
                /* This part should be removed for future experiments!!  */
                my_tm_stp_5 = my_tm_stp / 5.; // divide by 5 ns  since Vic already multiply by 5ns before sending data
                                              // to mbs (5n since  mbs uses a 200MHz clock and WhiteRabbit 1GHz)
                // cout << "my_tm_stp from lmd / by 5=" << my_tm_stp_5 << endl;

                my_tm_stp_lb_new = my_tm_stp_5 & 0x0FFFFFFF;       // first 28 bits
                my_tm_stp_hb_cor = my_tm_stp_5 >> 28 & 0x000FFFFF; // next 20 bits
                my_tm_stp_vhb_cor = my_tm_stp_5 >> 48 & 0x000FFFF; // next 16 bits
                // cout <<  "my_tm_stp_lb after /5=" << my_tm_stp_lb_new<< endl;
                // cout <<  "my_tm_stp_hb after /5=" << my_tm_stp_hb_cor<< endl;
                // cout <<  "my_tm_stp_vhb after /5=" << my_tm_stp_vhb_cor<< endl;

                // std::bitset<32> x(my_tm_stp_lb_new);
                // cout << "ts_lb_init=" <<  x  << endl;
                // cout << "ts_init=" <<  my_tm_stp_lb & 0x0FFFFFFF  << endl;
                ts_lb_part1 = my_tm_stp_lb_new & 0x00000003; // take the 2 first less significant  bits;
                // cout << "ts_lb_part1=" << ts_lb_part1 << endl;
                ts_lb_part2 = (my_tm_stp_lb_new >> 2) & 0x00000FFF; // take the 12 bits after a shift of 2 bits
                // cout << "ts_lb_part2=" << ts_lb_part2 << endl;
                ts_lb_part2_inv = (~(ts_lb_part2)) & 0x00000FFF; // invert and take only the first 12 inverted bits;
                // cout << "ts_lb_part2_inv=" << ts_lb_part2_inv << endl;
                ts_lb_part3 = my_tm_stp_lb_new & 0x0FFFC000; // from bit 14 to 27;
                // cout << "ts_lb_part3=" << ts_lb_part3 << endl;
                my_tm_stp_lb_cor = (((ts_lb_part3 >> 2) + ts_lb_part2_inv) << 2) + ts_lb_part1;

                // std::bitset<32> y(my_tm_stp_lb_cor);
                // cout << "ts_lb end=" << y << endl;
                // cout << "ts_lb end= " << my_tm_stp_lb_cor << endl;

                // Counting how often the new time stamp is smaller (anterior) than the previous time stamp
                if (((my_tm_stp_vhb_cor << 48) | ((my_tm_stp_hb_cor << 28) | (my_tm_stp_lb_cor))) * 5. - my_tm_stp < 0)
                    fCount_disorder++;
                // cout << "diff my_tm_stp final=" << (  (my_tm_stp_vhb_cor<< 48 ) | ((my_tm_stp_hb_cor<< 28 ) |
                // (my_tm_stp_lb_cor)) ) *5. - my_tm_stp << endl;

                // reconstruct time stamp
                my_tm_stp = ((my_tm_stp_vhb_cor << 48) | ((my_tm_stp_hb_cor << 28) | (my_tm_stp_lb_cor))) *
                            5.; // re multiply by 5

                cout << "my_tm_stp final=" << my_tm_stp << endl;

                /* End of S438 data correction */

                // Count the number of word of type 3:
                if (Info4)
                    fTotalHits++; // We start counting after Info4 is true
                cout << "Total hit of type 3= " << fTotalHits << endl;

                // let's keep filling the vectors ts_temp, index_block and  index_hit_temp for word of type 3
                // with the condition Info4=true, we ignore data before the first info_code=4 (or 7) appeared
                // cout << "filling vector with word of type 3 " << endl;
                // vectors to order data by time-stamp
                ts_temp.push_back(my_tm_stp);
                index_hit_temp.push_back(i);

                // cout << "ts_temp.size= "<<ts_temp.size() << endl;
                // cout << "index_block_temp.size= "<<index_block_temp.size() << endl;
                // cout << "index_hit_temp.size= "<<index_hit_temp.size() << endl;

                // if temp arrays too large, we sort data
                if (ts_temp.size() > MAXsort)
                {
                    ts_sort_flag = true;
                }
                // if it is the last word of the block, we sort data
                else if ((i + 1) == nItems)
                {
                    ts_sort_flag = true;
                }

                cout << "ts_sort_flag= " << ts_sort_flag << endl;
            }
        }

        // cout << "Info4=" << Info4 <<  "  ts_sort_flag=" << ts_sort_flag << "  high bit ts=" << my_tm_stp_hb << endl;

        //
        // Word type 2 (10)
        //

        if (my_wordtype == 2 && !ts_sort_flag)
        { // binary: 10

            // get dat info:
            my_info_code = item->GetInfoCode();
            my_info_field = item->GetInfoField();
            tm_stp_code47_lb = item->GetTime47lb();

            cout << "my_info_code=" << my_info_code << endl;

            // external input signal ts (47:28)
            if (my_info_code == 14)
            {
                my_tm_stp_Ext_lb = item->GetTimeExtlb(); // branchTimeExthb->GetValue(ii,0,true);
                my_tm_stp_Ext_hb = item->GetTimeExthb(); // branchTimeExthb->GetValue(ii,0,true);
                cout << "my_tm_stp_Ext_vhb   = " << my_tm_stp_Ext_vhb << endl;
                cout << "my_tm_stp_Ext_hb   = " << my_tm_stp_Ext_hb
                     << " which should be equals to  Infofield =" << my_info_field << endl;
                cout << "my_tm_stp_Ext_lb   = " << my_tm_stp_Ext_lb << endl;

                // my_tm_stp_ext= ( (my_tm_stp_Ext_hb<< 28 ) | (my_tm_stp_Ext_lb) );
                // cout << "my_tm_stp_ext   = " << my_tm_stp_ext << endl;

                if ((i + 2) == nItems) // if it is the word2 (14) before the last word2 of the block, we'll sort data in
                                       // the next loop
                {
                    ts_sort_flag = true;
                }

                cout << "ts_sort_flag=" << ts_sort_flag << endl;
            }
            // external input signal ts (63:48)
            if (my_info_code == 15)
            {
                my_tm_stp_Ext_vhb = item->GetTimeExtvhb();
                // my_tm_stp_ext=  (( (my_tm_stp_Ext_hb<< 28 ) | (my_tm_stp_Ext_lb) );  //  vhb ignored
                my_tm_stp_ext =
                    ((my_tm_stp_Ext_vhb << 48) | (((my_tm_stp_Ext_hb << 28) | (my_tm_stp_Ext_lb)))); // with vhb
                // cout << "my_tm_stp_Ext_vhb   = " << my_tm_stp_Ext_vhb << endl;
                // cout << "my_tm_stp_Ext_hb   = " << my_tm_stp_Ext_hb << endl;
                // cout << "my_tm_stp_Ext_lb   = " << my_tm_stp_Ext_lb << endl;
            }

            // Data ts (47:28)
            if (my_info_code == 4 || my_info_code == 5 || my_info_code == 7 || my_info_code == 8)
            {
                // if(my_info_code==4 || my_info_code==7){

                // no_sync_flag= false;  // set no_sync_flag to false

                if (my_info_code == 4 || my_info_code == 7)
                    my_tm_stp_hb = my_info_field; //  47:28 timestamp (if both 4/5 and 7/8)
                // if(/*my_info_code==4 ||*/ my_info_code==7)my_tm_stp_hb= my_info_field ;  //  47:28 timestamp (if only
                // 7&8)

                if (my_info_code == 4 || my_info_code == 5) // Info 4/5 occurs periodically (every 2^17*5ns)

                {
                    /**/

                    Info4 = true; // set Info4 true when the first info_code=4 (or seven) appears; = New high bit time
                                  // stamps

                    if (my_info_code == 4) // Note: there should always info_code=5 after an info_code=4
                    {

                        cout << "my_info_field = " << my_info_field << endl;
                        cout << "tm_stp_code47_lb = " << tm_stp_code47_lb << endl;
                        cout << " my_tm_stp_vhb = " << my_tm_stp_vhb << endl;
                        cout << " new reconstructed time = "
                             << ((my_tm_stp_vhb << 48) | ((my_info_field << 28) | (tm_stp_code47_lb))) / 5 << endl;
                        cout << " word2 previuosly reconstructed time = " << word2_rec_time << endl;

                        if (fTotalHits &&
                            ((my_tm_stp_vhb << 48) | (((my_info_field) << 28) | (tm_stp_code47_lb))) / 5 != 0)
                        {
                            diff45 = ((my_tm_stp_vhb << 48) | (((my_info_field) << 28) | (tm_stp_code47_lb))) / 5 -
                                     word2_rec_time;
                            if (word2_rec_time)
                                ts_code47_diff.push_back(diff45); // this should be = 5ns * 2tothe17

                            word2_rec_time =
                                ((my_tm_stp_vhb << 48) | (((my_info_field) << 28) | (tm_stp_code47_lb))) / 5;

                            fCount_45_78++;
                            // cout << " Count_45_78 = " << fCount_45_78 << endl;
                            ts_code47.push_back(word2_rec_time);
                            ts_code47_i.push_back(fCount_45_78);
                        }

                        if (!FirstSort_flag || (my_info_field != my_tm_stp_hb))
                        {

                            FirstSort_flag = true;

                            cout << " 4 my_tm_stp_hb before = " << my_tm_stp_hb << endl;
                            my_tm_stp_hb = my_info_field; //  47:28 timestamp
                            my_tm_stp_msb = my_tm_stp_hb;
                            cout << " 4 my_tm_stp_hb  after= " << my_tm_stp_hb << endl;

                            tm_stp_msb_modules[my_det_id] = my_tm_stp_msb; // update for use with other data types
                            // cout << "tm_stp_msb_modules[my_det_id] = " << tm_stp_msb_modules[my_det_id] << endl;

                            if (Word3 && abs(difflb) < 1.e8)
                            {
                                ts_sort_flag = true; // ie: if it is not the first time info_code=4 (or 7) appears
                                Word3 = false;       // reset Word3
                            }
                        }
                        else if ((i + 2) ==
                                 nItems) // if it is the word before last in the block, we'll sort data in the next word
                        {
                            ts_sort_flag = true;
                        }
                    }

                    // reconstruct time stamp= MSB+LSB
                    if (my_info_code == 5) //  Note: there should always info_code=5 after an info_code=4
                    {
                        if (my_info_field != my_tm_stp_vhb) // if The very high bit time stamp part has increased
                        {
                            // cout << "my_tm_stp_vhb  before  = " << my_tm_stp_vhb << endl;
                            // TS_4_5_diff->Fill( ((my_tm_stp_vhb<< 48 ) | ((my_tm_stp_hb<< 28 ) | (my_tm_stp_lb)) )/1e9
                            // ); // in sec !!!! if the 5ns already implemented in Vic's readout
                            my_tm_stp_vhb = my_info_field; //  63:48
                            // cout << "my_tm_stp_vhb  after  = " << my_tm_stp_vhb << endl;

                            // TS_4_5->Fill( fTotalHits, ((my_tm_stp_vhb<< 48 ) | ((my_tm_stp_hb<< 28 ) |
                            // (my_tm_stp_lb)) )); // in sec !!!! if the 5ns already implemented in Vic's readout

                            if (Word3 && difflb < 1.e8)
                            {
                                ts_sort_flag = true; // ie: if it is not the first time info_code=4 (or 7) appears
                                Word3 = false;       // reset Word3
                            }
                        }
                    }

                    /**/

                    //}else if( (my_info_code==7 || my_info_code==8) && !Info4)
                }
                else if ((my_info_code == 7 ||
                          my_info_code == 8)) // Info 7/8 occurs when high bit timestamp is found not ordered !

                {
                    Info4 = true; // set Info4 true when the first info_code=4 (or seven) appears

                    if (my_info_code == 7)
                    {

                        cout << "my_info_field = " << my_info_field << endl;
                        cout << "tm_stp_code47_lb = " << tm_stp_code47_lb << endl;
                        cout << " my_tm_stp_vhb = " << my_tm_stp_vhb << endl;
                        cout << " new reconstructed time = "
                             << ((my_tm_stp_vhb << 48) | ((my_info_field << 28) | (tm_stp_code47_lb))) / 5 << endl;
                        cout << " word2 previuosly reconstructed time = " << word2_rec_time << endl;

                        if (fTotalHits &&
                            ((my_tm_stp_vhb << 48) | ((my_info_field << 28) | (tm_stp_code47_lb))) / 5 != 0)
                        {
                            diff78 = ((my_tm_stp_vhb << 48) | (((my_info_field) << 28) | (tm_stp_code47_lb))) / 5 -
                                     word2_rec_time;
                            if (word2_rec_time)
                                ts_code47_diff.push_back(diff78); // this should be = 5ns * 2tothe17

                            word2_rec_time =
                                ((my_tm_stp_vhb << 48) | (((my_info_field) << 28) | (tm_stp_code47_lb))) / 5;

                            fCount_45_78++;
                            // cout << " Count_45_78 = " << fCount_45_78 << endl;
                            ts_code47.push_back(word2_rec_time);
                            ts_code47_i.push_back(fCount_45_78);
                        }

                        if (!FirstSort_flag ||
                            (my_info_field != my_tm_stp_hb)) // if The very high bit time stamp part has increased
                        {

                            FirstSort_flag = true; //

                            cout << " 7 tm_stp_msb_modules[my_det_id] (hb) before = " << tm_stp_msb_modules[my_det_id]
                                 << endl;
                            my_tm_stp_hb = my_info_field; //  47:28 timestamp
                            my_tm_stp_msb = my_tm_stp_hb;
                            tm_stp_msb_modules[my_det_id] = my_tm_stp_msb;
                            cout << "7 tm_stp_msb_modules[my_det_id] (hb) after= " << tm_stp_msb_modules[my_det_id]
                                 << endl;

                            if (Word3 && abs(difflb) < 1.e8)
                            {
                                ts_sort_flag = true; // ie: if it is not the first time info_code=4 (or 7) appears
                                Word3 = false;       // reset Word3
                            }
                            else if ((i + 2) == nItems) // if it is the word before last in the block, we'll sort data
                                                        // in the next loop
                            {
                                ts_sort_flag = true;
                            }
                        }
                    }
                    // reconstruct time stamp= MSB+LSB
                    // cout << "my_tm_stp_lb   = " << my_tm_stp_lb << endl;

                    if (my_info_code == 8)
                    {

                        if (my_info_field != my_tm_stp_vhb) // if The high bit time stamp part has increased
                        {
                            cout << "my_tm_stp_vhb  before  = " << my_tm_stp_vhb << endl;
                            my_tm_stp_vhb = my_info_field; //  63:48
                            cout << "my_tm_stp_vhb  after  = " << my_tm_stp_vhb << endl;

                            if (Word3 && abs(difflb) < 1.e8)
                            {
                                ts_sort_flag = true; // ie: if it is not the first time info_code=4 (or 7) appears
                                Word3 = false;       // reset Word3
                            }
                        }
                    }
                }
                else
                {
                    ts_sort_flag = false; // neither an info_code=4 (then 5) or 7 (then 8)
                }
            }

        } // end of type 2 word

        cout << "Info4=" << Info4 << "  ts_sort_flag=" << ts_sort_flag << "  high bit ts=" << my_tm_stp_hb << endl;

        if ((my_wordtype == 2 && ts_sort_flag) || (my_wordtype == 3 && ts_sort_flag))
        {
            // if( (my_wordtype==2 && ts_sort_flag) /*|| (my_wordtype==3 && ts_sort_flag)*/ ) {
            // We add the condition: "(my_wordtype==3 && ts_sort_flag)" in case the block finishes during a sequence of
            // word3 instead of word2

            long long n_first = index_hit_temp.at(0);
            long long n_loop = index_hit_temp.size();
            // n_first=index_hit_temp.at(0);
            // n_loop= index_hit_temp.size();

            cout << "n_first=" << n_first << endl;
            cout << "n_loop=" << n_loop << endl;

            InsertionSort(ts_temp, index_hit_temp); // when calling InsertionSort
            // InsertionSort2(ts_temp,index_block_temp, index_hit_temp); // when calling InsertionSort2

            ts_sort_flag = false;
            bool fill_flag = false;

            // transport sorted data to output_tree
            for (long long j = 0; j < n_loop; j++)
            { // transport sorted data to output_tree

                // cout << "j= " << j << endl;

                item = (R3BStartrackRawHit*)fRawData->At(index_hit_temp.at(j));

                cout << "index_hit_temp.at(j=" << j << ")= " << index_hit_temp.at(j) << endl;

                my_tm_stp = ts_temp.at(j);
                cout << "my_tm_stp= " << ts_temp.at(j) << endl;
                cout << "my_new Type=" << item->GetWordType() /*branchWordType->GetValue(index_hit_temp.at(j),0,true)*/
                     << endl;                                 // when calling InsertionSort2

                my_new_wordtype = item->GetWordType(); // branchWordType->GetValue(index_hit_temp.at(j),0,true);
                my_new_hitbit = item->GetHitBit(); // branchHitValue->GetValue(index_hit_temp.at(j),0,true);     // when
                                                   // calling InsertionSort2
                my_new_det_id = item->GetModuleId(); // branchModuleId->GetValue(index_hit_temp.at(j),0,true);     //
                                                     // when calling InsertionSort2
                my_new_side_id = item->GetSide();    // branchEnergy->GetValue(index_hit_temp.at(j),0,true);  // when
                                                     // calling InsertionSort2
                my_new_asic_id = item->GetAsicId();  // branchAsicId->GetValue(index_hit_temp.at(j),0,true);   // when
                                                     // calling InsertionSort2
                my_new_ch_id = item->GetStripId();   // branchStripId->GetValue(index_hit_temp.at(j),0,true);    // when
                                                     // calling InsertionSort2
                my_new_adc_data = item->GetADCdata(); // branchEnergy->GetValue(index_hit_temp.at(j),0,true);    // when
                                                      // calling InsertionSort2

                // s_entry.tm_stp = ts_temp.at(j); //reconstructed timestamp (MSB+LSB)
                // cout << " filling my_tm_stp=" << my_tm_stp << endl; //
                // s_entry.tm_stp = my_tm_stp/1e9; //reconstructed timestamp (MSB+LSB) in sec
                s_entry.tm_stp = my_tm_stp; // reconstructed timestamp (MSB+LSB) in nsec
                // cout << " filling my_tm_stp=" << my_tm_stp << endl; //
                // cout << "n_loop=" << n_loop << endl;
                // cout << "n_first=" << n_first << endl;
                s_entry.tm_stp_ext = my_tm_stp_ext; // in nsec
                // s_entry.tm_stp_ext = my_tm_stp_ext/1.e9; // in sec
                s_entry.nhit = fTotalHits - n_loop + (j + 1); // j+1 instaed of j, to start at event=1
                s_entry.type = my_new_wordtype;               //
                s_entry.hit = my_new_hitbit;
                s_entry.ModuleId = my_new_det_id;
                s_entry.Side = my_new_side_id;
                s_entry.AsicId = my_new_asic_id; // new *R3B*
                s_entry.StripId = my_new_ch_id;
                // not used *R3B* ->    int type; // QQQ: 0= 20 MeV or 1 GeV (decays), 1= 20 GeV (checked pulser data
                // only in type 0) type>=10: type = info_code+10 (i.e., PAUSE, RESUME, SYNC100, etc...)
                s_entry.ADCdata = my_new_adc_data;
                s_entry.sync_flag = my_sync_flag; // check SYNC100 pulses received for this module
                s_entry.pause_flag =
                    my_pause_flag; // check Pause signals followed by proper Resume signal: true= SYNC100 paused...

                // Fill histograms

                TS->Fill(my_tm_stp / 1e9);        // in sec !!!! if the 5ns already implemented in Vic's readout
                TSext->Fill(my_tm_stp_ext / 1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
                ADC_TS->Fill(my_tm_stp / 1e9,
                             my_adc_data); // in sec !!!! if the 5ns already implemented in Vic's readout
                if (my_side_id == 1)
                {
                    TS_n->Fill(my_tm_stp / 1e9);        // in sec !!!! if the 5ns already implemented in Vic's readout
                    TSext_n->Fill(my_tm_stp_ext / 1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
                    ADC_TS_n->Fill(my_tm_stp / 1e9,
                                   my_adc_data); // in sec !!!! if the 5ns already implemented in Vic's readout
                }
                if (my_side_id == 2)
                {
                    TSext_p->Fill(my_tm_stp_ext / 1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
                    ADC_TS_p->Fill(my_tm_stp / 1e9,
                                   my_adc_data); // in sec !!!! if the 5ns already implemented in Vic's readout
                }

                Asic_Side->Fill((my_side_id - 1), (my_asic_id - 1));
                ADC->Fill(my_adc_data);
                TS_TSext_diff->Fill(
                    (my_tm_stp / 1e9 -
                     my_tm_stp_ext / 1e9)); // in sec !!!! if the 5ns already implemented in Vic's readout
                TS_TSext->Fill(my_tm_stp / 1e9,
                               my_tm_stp_ext / 1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
                TS_event->Fill(fTotalHits,
                               my_tm_stp / 1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
                TSext_event->Fill(fTotalHits,
                                  my_tm_stp_ext / 1e9); // in sec !!!! if the 5ns already implemented in Vic's readout

                output_Tree->Fill();
            }

            index_hit_temp.clear();
            ts_temp.clear();

            ts_sort_flag = false; // reset ts_sort_flag to false
        }

    } // end of loop over the hit
}

void R3BStartrackOrderTS::FinishEvent()
{
    fNevents += 1;

    cout << "fNevents=" << fNevents << endl;
}

void R3BStartrackOrderTS::FinishTask()
{

    cout << " " << endl;
    cout << "Number of time the timestamp was found smaller than the previous one: " << fCount_disorder
         << ". So: " << fCount_disorder * 100 / fTotalHits << "% of accepted hits" << endl;
    cout << "Total hits found in Si Tracker:" << fTotalHits + fCount_rejectA + fCount_rejectB << endl;
    cout << "Hits rejected because no high-bit ts information available at beginning of the block: " << fCount_rejectA
         << " -> " << fCount_rejectA * 100. / (fTotalHits + fCount_rejectA + fCount_rejectB) << "%" << endl;
    cout << "Hits rejected because of suspected missing blocks (suspicious jump of high-bit ts values by more than 1 "
            "unit): "
         << fCount_rejectB << " -> " << fCount_rejectB * 100. / (fTotalHits + fCount_rejectA + fCount_rejectB) << "%"
         << endl;
    cout << "Accepted hits: " << fTotalHits << " -> "
         << fTotalHits * 100 / (fTotalHits + fCount_rejectA + fCount_rejectB) << "%" << endl;
    cout << " " << endl;

    // Write histos

    TS->Write();
    TSext->Write();
    ADC_TS->Write();
    TS_n->Write();
    TSext_n->Write();
    ADC_TS_n->Write();
    TSext_p->Write();
    ADC_TS_p->Write();
    Asic_Side->Write();
    ADC->Write();
    TS_TSext_diff->Write();
    TS_TSext->Write();    //
    TS_event->Write();    //
    TSext_event->Write(); //

    // Write Sorted tree
    output_Tree->Write();

    // This ti to create and save a graph rather than a 2D histogrma
    output_Tree->Draw("StartrackOrderTS.nhit:StartrackOrderTS.tm_stp", "", "");
    cout << "SelectedRows=" << output_Tree->GetSelectedRows() << endl;
    ;
    TGraph* gr = new TGraph(output_Tree->GetSelectedRows(), output_Tree->GetV1(), output_Tree->GetV2());
    gr->Draw("ap");
    gr->Write();

    TGraph* gr2 = new TGraph(fCount_45_78, &ts_code47_i[0], &ts_code47[0]);
    // gr2->Draw("AL");
    gr2->Write();

    TGraph* gr3 = new TGraph(fCount_45_78 - 1, &ts_code47_i[0], &ts_code47_diff[0]);
    // gr3->Draw("AL");
    gr3->Write();

    // LOG(info) << "Number of Entries: " << fh_tdc[15]->GetEntries();

    // adding calibration:
    // for(Int_t iModule =0; iModule <16; iModule++){
    // create container and set the module ID
    // R3BTofTCalPar* pTCal = new R3BTofTCalPar();
    // pTCal->SetBarId(i);

    // Set Calibration values (inside of loop over time channels)
    // index - current number of linear segments (bins)
    // pTCal->SetBinLowAt(1024,0);
    // pTCal->SetBinUpAt(1024+50-1, 0);
    // pTCal->SetTimeAt(15,0);
    // pTCal->IncrementNofChannels();

    // Add to set of parameters
    // fCal_Par->AddTCalPar(pTCal);

    //}

    // outFile->Write();
    // outFile->Close();

    // fCal_Par->setChanged();
}
// void InsertionSort(std::vector<long long> & v_ts, std::vector<long long> & v_index){
// The following ordering function only works if the number of entries of the tree (input_Tree->GetEntries()) is the
// total number of hits in the data stream and not the number of data blocks

void R3BStartrackOrderTS::InsertionSort(vector<long long>& v_ts, vector<long long>& v_index)
{

    int v_n = v_ts.size();

    // cout << "v_n size=" << v_n << endl;

    for (int i = 1; i < v_n; i++)
    {

        // cout << "i=" <<  i << endl;

        long long next_ts, next_index;
        next_ts = v_ts.at(i);
        next_index = v_index.at(i);

        // cout << "next_ts=" <<  v_ts.at(i) << endl;
        // cout << "next_ts=" <<  v_ts.at(i-1) << endl;
        // cout << "next_index=" << v_index.at(i)  << endl;
        // cout << "next_index=" << v_index.at(i-1)  << endl;

        int j;
        for (j = i; j > 0 && v_ts.at(j - 1) > next_ts; j--)
        {

            v_ts[j] = v_ts.at(j - 1);
            // v_ts.at(j)= v_ts.at(j-1);
            v_index[j] = v_index.at(j - 1);
            // v_index.at(j)= v_index.at(j-1);
        }

        v_ts[j] = next_ts;
        // v_ts.at(j)= next_ts;
        v_index[j] = next_index;
        // v_index.at(j)= next_index;

        // cout << "end insertion"<< endl;
    }
    // cout << "end loopinsertion"<< endl;

    return;
}

// The following ordering function only works if the number of entries of the tree (input_Tree->GetEntries()) is the the
// number of data blocks and not total number of hits in the data stream
void R3BStartrackOrderTS::InsertionSort2(vector<long long>& v_ts,
                                         vector<long long>& v_block_index,
                                         vector<long long>& v_hit_index)
{

    int v_n = v_ts.size();

    // cout << "v_n size=" << v_n << endl;
    for (int i = 1; i < v_n; i++)
    {

        long long next_ts, next_block_index, next_hit_index;
        next_ts = v_ts.at(i);
        next_block_index = v_block_index.at(i);
        next_hit_index = v_hit_index.at(i);

        // cout << "i=" <<  i << endl;
        // cout << "next_ts=" <<  v_ts.at(i) << endl;
        // cout << "next_ts=" <<  v_ts.at(i-1) << endl;
        // cout << "next_index=" << v_index.at(i)  << endl;
        // cout << "next_index=" << v_index.at(i-1)  << endl;

        int j;
        for (j = i; j > 0 && v_ts.at(j - 1) > next_ts; j--)
        {

            // v_ts[j]= v_ts.at(j-1);
            v_ts.at(j) = v_ts.at(j - 1);
            // v_index[j]= v_index.at(j-1);
            // v_index.at(j)= v_index.at(j-1);
            v_block_index.at(j) = v_block_index.at(j - 1);
            v_hit_index.at(j) = v_hit_index.at(j - 1);
        }

        // v_ts[j]= next_ts;
        v_ts.at(j) = next_ts;
        // v_index[j]= next_index;
        v_block_index.at(j) = next_block_index;
        v_hit_index.at(j) = next_hit_index;
    }

    // return;
}

ClassImp(R3BStartrackOrderTS)
