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
// This task reconstruct the Si tracker data time stamp according to the raw hit information
//
// This task should become obsolete when the ASIC hdml programming is comppleted
//
//
//

#include "R3BStartrackRecTS.h"
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

#include <iostream>
#include <vector>

// Default constructor
R3BStartrackRecTS::R3BStartrackRecTS()
    : FairTask("R3BStartrackRecTS", 1)
    , fRawData(NULL)
    , fNevents(0)
    ,             // This is actually the number of blocks
    fTotalHits(0) //, This is the accumulater number of word of type 3 in the run
// fCal_Par(NULL)
{
}

// Standard constructor
R3BStartrackRecTS::R3BStartrackRecTS(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fRawData(NULL)
    , fNevents(0)
    , fTotalHits(0) //,
// fCal_Par(NULL)
{
}

// Destructor
R3BStartrackRecTS::~R3BStartrackRecTS() {}

InitStatus R3BStartrackRecTS::Init()
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
    // TH1F *TS = new TH1F("TS","Time stamp ns (Reconstructed from hit strips) ",1000,707400,707750.);

    /*
        TS = new TH1F("TS","Time Stamp reconstructed from hit strips ",75,141480,141540.);  // without vhb
    TSext = new TH1F("TSext","Time Stamp reconstructed from external signal ",75,141480,141540.); // without vhb
    TS_p = new TH1F("TS_p","Time Stamp reconstructed from hit strips ",75,141480,141540.); // without vhb
    TSext_p = new TH1F("TSext_p","Time Stamp reconstructed from external signal ",75,141480,141540.);// without vhb
    TS_n = new TH1F("TS_n","Time Stamp reconstructed from hit strips ",75,141480,141540.); // without vhb
    TSext_n = new TH1F("TSext_n","Time Stamp reconstructed from external signal ",75,141480,141540.); // without vhb
    */

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

    // TH1F *TS_TSext_diff = new TH1F("TS_TSext_diff","Time stamp difference (Si - Ext trigger) ",5000,-0.4,.8);
    TS_TSext_diff = new TH1F("TS_TSext_diff", "Time stamp difference (Si - Ext trigger) in sec ", 5000, -20., 20.);
    // TS_TSext = new TH2F("TS_TSext"," TS vs TSext",300,141480.,141540., 300,141480.,141540.); // without vhb
    TS_TSext = new TH2F("TS_TSext", " TS vs TSext", 750, 1267370., 1267450., 750, 1267370, 1267450.); // with vhb

    // TH2F *TS_event = new TH2F("TS_event"," TS vs hit#",41000,1.,410000.,3000,141480,141540.);
    // TS_event = new TH2F("TS_event"," TS vs hit#",4100,1.,410000.,300,141480,141540.); // without vhb
    // TS_event_reduce = new TH2F("TS_event_reduce"," TS vs hit#",5000,1.,50000.,100,141480,141490.); // without vhb
    TS_event = new TH2F("TS_event", " TS vs hit#", 4100, 1., 410000., 750, 1267370, 1267450.);              // with vhb
    TS_event_reduce = new TH2F("TS_event_reduce", " TS vs hit#", 5000, 1., 50000., 100, 1267370, 1267380.); // with vhb
    //  TH2F *TS_event = new TH2F("TS_event"," TS vs hit#",4100,1.,410000.,300,141480,141540.);
    // TSext_event = new TH2F("TSext_event"," TSext vs hit#",4100,1.,410000.,300,141480,141540); // without vhb
    // ADC_TS = new TH2F("ADC_TS"," ADC value vs TS",75,141480,141540,4000,1.,4000.); // without vhb
    // ADC_TS_p = new TH2F("ADC_TS_p"," ADC value vs TS ",75,141480,141540,4000,1.,4000.); // without vhb
    // ADC_TS_n = new TH2F("ADC_TS_n"," ADC value vs TS ",75,141480,141540,4000,1.,4000.); // without vhb
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
    output_Tree->Branch("StartrackRecTS",
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

    Info4 = false;

    return kSUCCESS;
}

// Add NULL Checks

// kSUCCESS if no error occure (during the initialisation for instance)
// kERROR  if error is not crucial
// kFATAL  if error is crucial

void R3BStartrackRecTS::Exec(Option_t* option) // called for each data block
{
    // loop over input data
    R3BStartrackRawHit* item;
    R3BStartrackRawHit* item2;

    Int_t nItems = fRawData->GetEntriesFast(); // returns the number of hits in the block

    cout << "nItems=" << nItems << endl;

    unsigned long long my_tm_stp_5 = 0.;
    unsigned long my_tm_stp_lb_new = 0.;
    unsigned long my_tm_stp_lb_cor = 0.;
    unsigned long my_tm_stp_hb_cor = 0.;
    unsigned long my_tm_stp_vhb_cor = 0.;
    unsigned long ts_lb_part1 = 0.;
    unsigned long ts_lb_part2 = 0.;
    unsigned long ts_lb_part2_inv = 0.;
    unsigned long ts_lb_part3 = 0.;
    /*
    unsigned long long my_tm_stp_ext_5=0.;
    unsigned long my_tm_stp_lb_ext_new=0.;
    unsigned long my_tm_stp_lb_ext_cor=0.;
    unsigned long my_tm_stp_hb_ext_cor=0.;
    unsigned long my_tm_stp_vhb_ext_cor=0.;
    unsigned long ts_lb_ext_part1=0.;
    unsigned long ts_lb_ext_part2=0.;
    unsigned long ts_lb_ext_part2_inv=0.;
    unsigned long ts_lb_ext_part3=0.;
    */

    //
    // Below is the equivalent of the root macros: macros/r3b/Startrack/StartrackTimeOrder.C :
    //

    // Info4= false;

    //	for(Int_t i=0; i<7791; i++)
    for (Int_t i = 0; i < nItems; i++) // nItem is the number of hits in the block
    {

        item = (R3BStartrackRawHit*)fRawData->At(i);

        if (NULL == item)
        {
            continue;
        } // means just ignore the currnt i and move to the other

        my_wordtype = item->GetWordType();

        // cout << "my_wordtype=" << my_wordtype << endl;

        if (my_wordtype == 3)
        { // 11
            my_det_id = item->GetModuleId();
            my_hitbit = item->GetHitBit();
            my_side_id = item->GetSide();
            my_asic_id = item->GetAsicId();
            my_ch_id = item->GetStripId();
            my_adc_data = item->GetADCdata();

            my_tm_stp_lb = (item->GetTimelb());
            // my_tm_stp_lb=(item->GetTimelb())*1e-6; // why *1e6 ?? to be checked !!

            // cout << "my_tm_stp_lb=" << my_tm_stp_lb << endl;

            // rec time stamp 47:0  To be used if time stamp already reconstructed in the unpacker
            // my_tm_stp=branchTimevhb->GetValue(ii,0,true);
            // if(fTotalHits==0)cout << "my_tm_stp_hb= " <<  tm_stp_msb_modules[my_det_id]  << endl; // To check it is
            // zero to begin with

            // my_tm_stp= (  (tm_stp_msb_modules[my_det_id]<< 28 ) + (my_tm_stp_lb) );  // vhbit ignored
            my_tm_stp =
                ((my_tm_stp_vhb << 48) | ((tm_stp_msb_modules[my_det_id] << 28) | (my_tm_stp_lb))); // with vhbit

            if (Info4)
                fTotalHits++; // We start counting after Info4 is true (to ignore the time stamp without high bits)
                              // cout << "Total hit of type 3= " << fTotalHits << endl;

            /* if S438 data only (Oct 2014 data):                    */
            /* One needs to correct some inverted bits               */
            /* This part should be removed for future experiments!!  */
            my_tm_stp_5 = my_tm_stp / 5.; // divide by 5 ns  since Vic already multiply by 5ns before sending data to
                                          // mbs (5n since  mbs uses a 200MHz clock and WhiteRabbit 1GHz)
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

            // reconstruct after /5 and corrected inverted bits
            my_tm_stp =
                ((my_tm_stp_vhb_cor << 48) | ((my_tm_stp_hb_cor << 28) | (my_tm_stp_lb_cor))) * 5; // re multiply by 5

            // cout << "my_tm_stp final=" << my_tm_stp << endl;

            /* End of S438 data correction */
        }
        else if (my_wordtype == 2)
        { // 10

            my_info_code = item->GetInfoCode();
            my_info_field = item->GetInfoField();

            // cout << "my_info_code=" << my_info_code << endl;

            /*
            // external ts (bits: 47-28) and (bits: 63-48)
            if(my_info_code==14 || my_info_code==15){

              if(my_info_code==14){
                my_tm_stp_Ext_hb=item->GetTimeExthb(); //branchTimeExthb->GetValue(ii,0,true);
                //cout << "my_tm_stp_Ext_vhb   = " << my_tm_stp_Ext_vhb << endl;
                //cout << "my_tm_stp_Ext_hb   = " << my_tm_stp_Ext_hb << " which should be equals to  Infofield =" <<
            my_info_field <<  endl;
                //cout << "my_tm_stp_Ext_lb   = " << my_tm_stp_Ext_lb << endl;

                my_tm_stp_ext= ( (my_tm_stp_Ext_hb<< 28 ) | (my_tm_stp_Ext_lb) );
                //cout << "my_tm_stp_ext   = " << my_tm_stp_ext << endl;

              }else   // my_info_code=15 -> external ts (63:48)
                {
                my_tm_stp_Ext_vhb=item->GetTimeExtvhb();
                //cout << "my_tm_stp_Ext_vhb   = " << my_tm_stp_Ext_vhb << endl;
                //cout << "my_tm_stp_Ext_hb   = " << my_tm_stp_Ext_hb << endl;
                //cout << "my_tm_stp_Ext_lb   = " << my_tm_stp_Ext_lb << endl;
                }

            } */

            // external ts (47:28)
            if (my_info_code == 14)
            {
                my_tm_stp_Ext_lb = item->GetTimeExtlb(); // branchTimeExtlb->GetValue(ii,0,true);
                my_tm_stp_Ext_hb = item->GetTimeExthb(); // branchTimeExthb->GetValue(ii,0,true);
                // cout << "my_tm_stp_Ext_vhb   = " << my_tm_stp_Ext_vhb << endl;
                // cout << "my_tm_stp_Ext_hb   = " << my_tm_stp_Ext_hb << " which should be equals to  Infofield =" <<
                // my_info_field <<  endl; cout << "my_tm_stp_Ext_lb   = " << my_tm_stp_Ext_lb << endl;

                // my_tm_stp_ext= ( (my_tm_stp_Ext_hb<< 28 ) | (my_tm_stp_Ext_lb) );
                // cout << "my_tm_stp_ext   = " << my_tm_stp_ext << endl;
            }
            // external ts (63:48)
            if (my_info_code == 15)
            {
                my_tm_stp_Ext_vhb = item->GetTimeExtvhb();
                // my_tm_stp_ext=  (( (my_tm_stp_Ext_hb<< 28 ) | (my_tm_stp_Ext_lb) );  //  vhb ignored
                my_tm_stp_ext =
                    ((my_tm_stp_Ext_vhb << 48) | (((my_tm_stp_Ext_hb << 28) | (my_tm_stp_Ext_lb)))); // with vhb
                // cout << "my_tm_stp_Ext_vhb   = " << my_tm_stp_Ext_vhb << endl;
                // cout << "my_tm_stp_Ext_hb   = " << my_tm_stp_Ext_hb << endl;
                // cout << "my_tm_stp_Ext_lb   = " << my_tm_stp_Ext_lb << endl;

                /* if S438 data only (Oct 2014 data):                    */
                /* One needs to correct some inverted bits               */
                /* This part should be removed for future experiments!!
                   my_tm_stp_ext_5 = my_tm_stp_ext/5. ; // divide by 5 ns  since Vic already multiply by 5ns before
                   sending data to mbs (5n since  mbs uses a 200MHz clock and WhiteRabbit 1GHz)
                   //cout << "my_tm_stp_ext from lmd / by 5=" << my_tm_stp_ext_5 << endl;

                   my_tm_stp_lb_ext_new= my_tm_stp_ext_5 & 0x0FFFFFFF;  // first 28 bits
                   my_tm_stp_hb_ext_cor= my_tm_stp_ext_5 >> 28 & 0x000FFFFF; // next 20 bits
                   my_tm_stp_vhb_ext_cor= my_tm_stp_ext_5 >> 48 & 0x000FFFF; // next 16 bits
                   //cout <<  "my_tm_stp_lb_ext after /5=" << my_tm_stp_lb_new_ext << endl;
                   //cout <<  "my_tm_stp_hb_ext after /5=" << my_tm_stp_hb_cor_ext << endl;
                   //cout <<  "my_tm_stp_vhb_ext after /5=" << my_tm_stp_vhb_cor_ext << endl;

                   //std::bitset<32> x(my_tm_stp_lb_new);
                   //cout << "ts_lb_init=" <<  x  << endl;
                   //cout << "ts_init=" <<  my_tm_stp_ext_lb & 0x0FFFFFFF  << endl;
                   ts_lb_ext_part1 =  my_tm_stp_lb_ext_new & 0x00000003; //take the 2 first less significant  bits;
                   //cout << "ts_lb_part1=" << ts_lb_ext_part1 << endl;
                   ts_lb_ext_part2= (my_tm_stp_lb_ext_new >> 2) & 0x00000FFF; // take the 12 bits after a shift of 2
                   bits
                   //cout << "ts_lb_part2=" << ts_lb_ext_part2 << endl;
                   ts_lb_ext_part2_inv=  (~(ts_lb_ext_part2)) & 0x00000FFF; // invert and take only the first 12
                   inverted bits;
                   //cout << "ts_lb_part2_inv=" << ts_lb_ext_part2_inv << endl;
                   ts_lb_ext_part3 =  my_tm_stp_lb_ext_new & 0x0FFFC000; //from bit 14 to 27;
                   //cout << "ts_lb_part3=" << ts_lb_ext_part3 << endl;
                   my_tm_stp_lb_ext_cor= (((ts_lb_ext_part3 >> 2) + ts_lb_ext_part2_inv) << 2)  + ts_lb_ext_part1 ;

                   //std::bitset<32> y(my_tm_stp_lb_ext_cor);
                   //cout << "ts_lb end=" << y << endl;
                   //cout << "ts_lb end= " << my_tm_stp_lb_ext_cor << endl;

                   // reconstruct after /5 and corrected inverted bits
                   my_tm_stp_ext= (  (my_tm_stp_vhb_ext_cor<< 48 ) | ((my_tm_stp_hb_ext_cor<< 28 ) |
                   (my_tm_stp_lb_ext_cor)) ) *5; // re multiply by 5

                   //cout << "my_tm_stp_ext final=" << my_tm_stp_ext << endl;

                   End of S438 data correction */
            }

            // Data ts (47:28)
            if (my_info_code == 4 || my_info_code == 5 || my_info_code == 7 || my_info_code == 8)
            {
                // if(my_info_code==4 || my_info_code==7){

                // if(my_info_code==4 || my_info_code==7)my_tm_stp_hb= my_info_field ;  //  47:28 timestamp
                if (my_info_code == 4 || my_info_code == 5)
                {
                    Info4 = true; // set Info4 true when the first info_code=4 (or seven) appears; = New high bit time
                                  // stamps

                    if (my_info_code == 4)
                        my_tm_stp_hb = my_info_field; //  47:28 timestamp
                    // if(my_info_code==5 || my_info_code==8) my_tm_stp_vhb= my_info_field;  //  63:48
                    // cout << "my_tm_stp_hb   = " << my_tm_stp_hb << endl;
                    // cout << "which should be equals to   = " << branchTimehb->GetValue(ii,0,true) << endl;
                    // my_tm_stp_msb= (my_tm_stp_vhb << 20) |  my_tm_stp_hb;

                    if (my_info_code == 4)
                        my_tm_stp_msb = my_tm_stp_hb;
                    // if(my_info_code==4)cout << "my_tm_stp_hb = " << my_tm_stp_hb << endl;

                    if (my_info_code == 4)
                        tm_stp_msb_modules[my_det_id] = my_tm_stp_msb; // update for use with other data types
                    // cout << "tm_stp_msb_modules[my_det_id] = " << tm_stp_msb_modules[my_det_id] << endl;

                    // reconstruct time stamp= MSB+LSB
                    if (my_info_code == 5)
                        my_tm_stp_vhb = my_info_field; //  63:48
                    // if(my_info_code==5) cout << "my_tm_stp_vhb   = " << my_tm_stp_vhb << endl;

                    // if we ignore the highest bits ( my_tm_stp_vhb):
                    // my_tm_stp= (  (tm_stp_msb_modules[my_det_id]<< 28 ) | (my_tm_stp_lb) ); // vhbit ignored
                    // my_tm_stp= (  (my_tm_stp_vhb<< 48) + ((tm_stp_msb_modules[my_det_id]<< 28 ) + (my_tm_stp_lb) ));
                    // // with vhbit cout << "my_tm_stp_vhb   = " << my_tm_stp_vhb << endl; cout << "my_tm_stp   = " <<
                    // my_tm_stp << endl;

                    Info4 = true; // set Info4 true
                }
                else if ((my_info_code == 7 || my_info_code == 8) && !Info4)
                {
                    Info4 = true; // set Info4 true when the first info_code=4 (or seven) appears; = New high bit time
                                  // stamps

                    if (my_info_code == 7)
                        my_tm_stp_hb = my_info_field; //  47:28 timestamp
                    // if(my_info_code==5 || my_info_code==8) my_tm_stp_vhb= my_info_field;  //  63:48
                    // cout << "my_tm_stp_hb   = " << my_tm_stp_hb << endl;
                    // cout << "which should be equals to   = " << branchTimehb->GetValue(ii,0,true) << endl;
                    // my_tm_stp_msb= (my_tm_stp_vhb << 20) |  my_tm_stp_hb;

                    if (my_info_code == 7)
                        my_tm_stp_msb = my_tm_stp_hb;
                    // cout << "my_det_id = " << my_det_id << endl;

                    if (my_info_code == 7)
                        tm_stp_msb_modules[my_det_id] = my_tm_stp_msb; // update for use with other data types
                    // cout << "tm_stp_msb_modules[my_det_id] = " << tm_stp_msb_modules[my_det_id] << endl;

                    // reconstruct time stamp= MSB+LSB

                    if (my_info_code == 8)
                        my_tm_stp_vhb = my_info_field; //  63:48#

                    // if we ignore the highest bits ( my_tm_stp_vhb):
                    // my_tm_stp= (  (tm_stp_msb_modules[my_det_id]<< 28 ) | (my_tm_stp_lb) );  // vhbit ignore
                    // my_tm_stp= ( (my_tm_stp_vhb<< 48) + ((tm_stp_msb_modules[my_det_id]<< 28 ) + (my_tm_stp_lb) ));
                    // // with vhbit cout << "my_tm_stp_vhb   = " << my_tm_stp_vhb << endl; cout << "my_tm_stp   = " <<
                    // my_tm_stp << endl;
                }
            }
        }
        else
        {
            cout << "Attention : neither a word of type 2 or 3 !!!  this wordtype is " << my_wordtype << endl;
        }

        // if((my_wordtype==3) && (my_tm_stp/1e9<1000)) cout << " Attention : my_tm_stp=" << my_tm_stp << endl;
        // if((my_wordtype==3) && (my_tm_stp/1e9<1000)) cout << " Attention : my_tm_stp=" << my_tm_stp/1e9  << " and
        // my_tm_stp_lb =" <<  my_tm_stp_lb << " and my_info_code =" <<  my_info_code << endl;

        // if((my_wordtype==3))
        // if((my_wordtype==3) && (my_tm_stp/1e9 >1000))  // my_tm_stp/1e9 >1000 is to remove type=3 data that have been
        // recorded before a type=2 word was found in the data stream
        if ((my_wordtype == 3) && Info4) // my_tm_stp/1e9 >1000 is to remove type=3 data that have been recorded before
                                         // a type=2 word was found in the data stream
        {

            // s_entry.tm_stp = ts_temp.at(j); //reconstructed timestamp (MSB+LSB)
            // cout << " filling my_tm_stp=" << my_tm_stp << endl; //
            // s_entry.tm_stp = my_tm_stp/1e9; //reconstructed timestamp (MSB+LSB)
            s_entry.tm_stp = my_tm_stp; // reconstructed timestamp (MSB+LSB) in nsec
            // cout << " filling my_tm_stp_ext=" << my_tm_stp_ext << endl; //
            // s_entry.tm_stp_ext = my_tm_stp_ext/1e9; // in sec
            s_entry.tm_stp_ext = my_tm_stp_ext; // in nsec
            s_entry.nhit = fTotalHits;          //
            s_entry.type = my_wordtype;         //
            s_entry.hit = my_hitbit;
            s_entry.ModuleId = my_det_id;
            s_entry.Side = my_side_id;
            s_entry.AsicId = my_asic_id; // new *R3B*
            s_entry.StripId = my_ch_id;
            // not used *R3B* ->    int type; // QQQ: 0= 20 MeV or 1 GeV (decays), 1= 20 GeV (checked pulser data only
            // in type 0) type>=10: type = info_code+10 (i.e., PAUSE, RESUME, SYNC100, etc...)
            s_entry.ADCdata = my_adc_data;
            s_entry.sync_flag = my_sync_flag; // check SYNC100 pulses received for this module
            s_entry.pause_flag =
                my_pause_flag; // check Pause signals followed by proper Resume signal: true= SYNC100 paused...

            /*
            std::cout << "*** ENTRY FILLED:\n"
                  <<     "    type= "<< my_wordtype << std::endl
                  <<     "    mod_id= "<< my_det_id << std::endl;
            std::cout << " *!->NF<-!* " << endl;
            std::cout << " " << endl;
            */

            // Fill histograms

            TS->Fill(my_tm_stp / 1e9);                  // in sec !!!! if the 5ns already implemented in Vic's readout
            TSext->Fill(my_tm_stp_ext / 1e9);           // in sec !!!! if the 5ns already implemented in Vic's readout
            ADC_TS->Fill(my_tm_stp / 1e9, my_adc_data); // in sec !!!! if the 5ns already implemented in Vic's readout
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
                (my_tm_stp / 1e9 - my_tm_stp_ext / 1e9)); // in sec !!!! if the 5ns already implemented in Vic's readout
            TS_TSext->Fill(my_tm_stp / 1e9,
                           my_tm_stp_ext / 1e9);         // in sec !!!! if the 5ns already implemented in Vic's readout
            TS_event->Fill(fTotalHits, my_tm_stp / 1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
            TS_event_reduce->Fill(fTotalHits,
                                  my_tm_stp / 1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
            TSext_event->Fill(fTotalHits,
                              my_tm_stp_ext / 1e9); // in sec !!!! if the 5ns already implemented in Vic's readout

            output_Tree->Fill();
        }
        else
        {
            /*
             std::cout << "*** ENTRY NOT FILLED:\n"
             <<     "    type= "<< my_wordtype << std::endl
             <<     "    mod_id= "<< my_det_id << std::endl;
             std::cout << " *!->NF<-!* " << endl;
             std::cout << " " << endl;
            */
        } // end of if(fill_flag && my_tm_stp/1e9>1000)

    } // end of loop over the hit
}

void R3BStartrackRecTS::FinishEvent()
{
    fNevents += 1;

    cout << "data blocks=" << fNevents << endl;
}

void R3BStartrackRecTS::FinishTask()
{

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
    TS_TSext->Write();        //
    TS_event->Write();        //
    TS_event_reduce->Write(); //
    TSext_event->Write();     //

    // Write Sorted tree
    output_Tree->Write();

    // This ti to create and save a graph rather than a 2D histogrma
    output_Tree->Draw("StartrackRecTS.nhit:StartrackRecTS.tm_stp", "", "");
    cout << "SelectedRows=" << output_Tree->GetSelectedRows() << endl;
    ;
    TGraph* gr = new TGraph(output_Tree->GetSelectedRows(), output_Tree->GetV1(), output_Tree->GetV2());
    gr->Draw("ap");
    gr->Write();

    /*
  output_Tree->Draw("StartrackRecTS.tm_stp:StartrackRecTS.ADCdata","","");
     TGraph *gr2 = new TGraph(output_Tree->GetSelectedRows(),
                                 output_Tree->GetV1(), output_Tree->GetV2());
    gr2->Draw("ap");
    gr2->Write();
    */
}

ClassImp(R3BStartrackRecTS)
