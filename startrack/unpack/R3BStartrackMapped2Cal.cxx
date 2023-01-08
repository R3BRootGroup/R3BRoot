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

// ------------------------------------------------------------
// -----                  R3BStartrackMapped2Cal          -----
// -----            Created July 2017 by M.Labiche        -----
// ------------------------------------------------------------

/* July 2017
 * = Task to order the Startrack data in time,
 * before reconstructing tracks.
 * This allow to check the graph "ts vs HitId" before and after time ordering
 * Here a hit is any strip hit.
 */

#include <iostream>
#include <limits>
using namespace std;

// Fair headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"

// ROOT headers
#include "TClonesArray.h"
#include "TMath.h"

//#include "R3BEventHeader.h"

// Startrack headers
#include "R3BStartrackCalData.h"
#include "R3BStartrackMapped2Cal.h"
#include "R3BStartrackMappedData.h"

R3BStartrackMapped2Cal::R3BStartrackMapped2Cal() {}

R3BStartrackMapped2Cal::R3BStartrackMapped2Cal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItemsCA(NULL)
    , fCalItemsCA(new TClonesArray("R3BStartrackCalData"))
    , fNofCalItems(0)
    , fW3TSlast(0) // last TS of od word3 in the block (for continuity check)
    , fNbReject(0) // Word3 rejected due to suspicious Reconstructed TS
    , fN_first(0)
{
}

R3BStartrackMapped2Cal::~R3BStartrackMapped2Cal()
{
    if (fCalItemsCA)
    {
        delete fCalItemsCA;
        fCalItemsCA = NULL;
        fNofCalItems = 0;
        fW3TSlast = 0;
        fNbReject = 0;
        fN_first = 0;
    }
}

InitStatus R3BStartrackMapped2Cal::Init()
{
    /**
     * Initialize output data. Read input data and parameters.
     * The parameters get saved in dedicated arrays.
     * Print parameters, if verbosity is set to info.
     */

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";
    // header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

    // get access to Mapped data
    fMappedItemsCA = (TClonesArray*)mgr->GetObject("StartrackMapped"); // = branch name in TTree
    if (!fMappedItemsCA)
    {
        LOG(fatal) << "Branch StartrackMapped not found";
        return kFATAL;
    }

    // request storage of Cal data in output tree
    mgr->Register("StartrackCalData", "StartrackCalibrated", fCalItemsCA, kTRUE);

    // histos:

    // fTsHit = new TH2F("TsHit", "Ts vs hit_id", 4010, 0, 401000, 4000, 140e3, 142e3);

    return kSUCCESS;
}

// Note that the container may still be empty at this point.
void R3BStartrackMapped2Cal::SetParContainers()
{
    /**
     * Initialize/Reads parameter file for conversion.
     */

    LOG(info) << "R3BStartrackMapped2Cal :: SetParContainers() ";

    fStartrackCalPar = (R3BStartrackMapped2CalPar*)FairRuntimeDb::instance()->getContainer("StartrackCalPar");
    if (!fStartrackCalPar)
    {
        LOG(error) << "Could not get access to StartrackCalPar-Container.";
        fNofCalPars = 0;
        return;
    }

    // fStartrackCalPar->printparams();
}

InitStatus R3BStartrackMapped2Cal::ReInit()
{
    LOG(info) << " R3BStartrackMapped2Cal :: ReInit() ";

    SetParContainers();

    return kSUCCESS;
}

/*
 *  In this loop,  we order the data in time according to the time-stamp
 *
 */

void R3BStartrackMapped2Cal::Exec(Option_t* option)
{

    LOG(debug) << "Calibrating StarTrack Mapped Data";

    if (!fMappedItemsCA)
    {
        printf("Cannot access Startrack mapped items\n");
        return;
    }

    if (fCalItemsCA)
        fCalItemsCA->Delete();

    R3BStartrackMappedData* rawHit;
    Int_t rawHit_id = 0;
    UInt_t nHits = 0;
    ULong64_t my_rec_tm_stp = 0;
    ULong64_t my_rec_tm_stp_ext = 0;
    Int_t my_wordtype = 0;
    Int_t my_ladder_id = 0;
    Int_t my_side = 0;
    Int_t my_asic_id = 0;
    Int_t my_strip_id = 0;
    Double32_t my_energy = 0.;

    ULong64_t my_new_ts = 0.;  //
    ULong64_t my_prev_ts = 0.; //
    ULong64_t my_new_ts_ext = 0.;
    Int_t my_new_ladderId = -1;
    Int_t my_new_side = -1;
    Int_t my_new_asicId = -1;
    Int_t my_new_stripId = -1;
    Int_t my_new_energy = -1;

    Int_t LastBlockLastTS = fW3TSlast;
    Int_t NbReject = 0;

    Int_t nRawHits; // Nb of RawHits in current event
    Int_t calHits = 0;

    nRawHits = fMappedItemsCA->GetEntries(); // same as: Int_t nHits = fMappedItemsCA->GetEntriesFast(); = total number
                                             // if words (type 2 & 3)

    if (nRawHits) // ie.: data from Startrack found
    {
        cout << " " << endl;
        cout << "Number of Startrack rawHits in this block=" << nRawHits << endl; // Check nRawHits in tracker only

        bool no_sync_flag;
        no_sync_flag = true;
        bool ts_sort_flag;
        ts_sort_flag = false;

        // vector<long long> ts_temp;
        vector<ULong64_t> ts_temp;
        //	vector<long long> index_block_temp, index_hit_temp;
        vector<ULong64_t> index_hit_temp;
        ts_temp.reserve(4096);
        index_hit_temp.reserve(4096);

        //    for (Int_t ihit = 0; ihit < 2; ihit++)
        for (Int_t ihit = 0; ihit < nRawHits; ihit++)
        {
            R3BStartrackMappedData* hit = (R3BStartrackMappedData*)fMappedItemsCA->At(ihit);
            if (!hit)
                continue;

            my_wordtype = hit->GetWordtype();
            my_rec_tm_stp = hit->GetTS(); // time stamp already fully reconstructed in R3BStartrackReader

            // Consider only Word of type 3 and remove hits at the beaginning of the run that have no high bits time
            // stamp information tag.
            if (my_wordtype == 3 && ((my_rec_tm_stp & 0xFFFFFFFFF0000000)) != 0x0)
            {
                my_ladder_id = hit->GetLadderId();
                my_side = hit->GetSide();
                my_asic_id = hit->GetAsicId();
                my_strip_id = hit->GetStripId();
                my_energy = hit->GetADCdata();
                my_rec_tm_stp = hit->GetTS();        // time stamp already fully reconstructed in R3BStartrackReader
                my_rec_tm_stp_ext = hit->GetTSExt(); // reconstructed full time stamp of External signal (trigger)

                ts_sort_flag = false;

                fN_first++; //

                // vectors to order data by time-stamp
                ts_temp.push_back(my_rec_tm_stp);
                index_hit_temp.push_back(ihit);

                // cout << "ts_temp.size= "<<ts_temp.size() << endl;
                // cout << "index_hit_temp.size= "<<index_hit_temp.size() << endl;

                // fN_first++;  //
            }

            if ((ihit + 1) == nRawHits)
            { // last hit in the block <=> ts_sort_flag= true; since TS is already fully reconstructed

                Int_t n_loop = index_hit_temp.size();

                // cout << "Number of hits of wordtype==3 in this block (=n_loop)=" << n_loop << endl;	          //
                // n_loop = number of hits of wordtype==3 in the block cout << "Total number of hits of wordtype==3 in
                // the run so far (=fN_first)=" << fN_first << endl;	  // fN_first = total number of hits of
                // wordtype==3 in the run

                InsertionSort(ts_temp, index_hit_temp); // when calling InsertionSort
                // InsertionSort2(ts_temp,index_block_temp, index_hit_temp);     // when calling InsertionSort2
                ts_sort_flag = false;

                for (Int_t j = 0; j < n_loop; j++)
                { // transport sorted data to output_tree

                    R3BStartrackMappedData* newhit = (R3BStartrackMappedData*)fMappedItemsCA->At(index_hit_temp.at(j));

                    my_new_ts = (ts_temp.at(j));        // in nanosec
                    my_new_ts_ext = newhit->GetTSExt(); // in nanosec
                    my_new_ladderId = newhit->GetLadderId();
                    my_new_side = newhit->GetSide();
                    my_new_asicId = newhit->GetAsicId();
                    my_new_stripId = newhit->GetStripId();
                    my_new_energy = newhit->GetADCdata(); // TODO also : energy calibration !!

                    nHits = j + (fN_first - n_loop); //
                                                     // nHits= j + (fN_first) ;  //

                    if (j == 0)
                        cout << " previous block last TS was =" << fW3TSlast << endl;

                    if (j > 0 && ((my_new_ts - my_prev_ts) >
                                  1e8)) // To get rid of hit with probably wrongly reconstructed time stamp
                    {
                        // ignore this hit as time reconstruction probably wrong !!
                        // cout<< " Ignoring this hit:" << nHits <<  " because difference with previous ts is
                        // suspiciously huge:" << my_new_ts - my_prev_ts << endl;
                        nHits--;
                        NbReject++;
                        fNbReject++;

                        if (fabs(my_new_ts - my_prev_ts) > 1e8)
                            my_prev_ts = ts_temp.at(j - 1);
                        // if(j==n_loop-1)fW3TSlast=my_prev_ts;
                    }
                    else
                    {

                        if (((Int_t)(my_new_ts - LastBlockLastTS) < 0))
                        {
                            // cout<< " Ignoring this hit:" << nHits <<  " because difference with previous bloc ts is
                            // negatif:" << (Int_t)(my_new_ts - LastBlockLastTS)  << endl;
                            if (j > 0)
                                my_prev_ts = ts_temp.at(j - 1);
                            nHits--;
                            NbReject++;
                            fNbReject++;
                        }
                        else
                        {
                            my_prev_ts = ts_temp.at(j);

                            new ((*fCalItemsCA)[fNofCalItems]) R3BStartrackCalData(my_new_ts,
                                                                                   my_new_ts_ext,
                                                                                   my_new_ladderId,
                                                                                   my_new_side,
                                                                                   my_new_asicId,
                                                                                   my_new_stripId,
                                                                                   my_new_energy,
                                                                                   nHits);

                            fNofCalItems++;
                            fW3TSlast = ts_temp.at(j); // record last ts in the block for continuity check with TS at
                                                       // the beginning of the next block.
                        }

                        // cout << "nHits=" << nHits << " my_new_ts=" << my_new_ts<< endl;
                        // fTsHit->Fill(nHits,my_new_ts);
                    }
                }
            }

        } // end of loop on the number of hit in the block

        index_hit_temp.clear();
        ts_temp.clear();

        // cout << "Nb Rejections in the block =" << NbReject << endl;
        cout << "Total Number of accepted hit in StarTrack:" << fN_first << endl;
        cout << "Rate of StarTrack hit rejections in the run : "
             << (Double_t)(fNbReject * 100. / (fN_first + fNbReject)) << "%" << endl;
        cout << " Rejection are mainly due to TS at the begining of a block < last TS in previous data block" << endl;

    } // end of if data from Startrack (= end of block)
}

void R3BStartrackMapped2Cal::FinishEvent()
{
    if (fCalItemsCA)
    {
        fCalItemsCA->Clear();
        fNofCalItems = 0;
    }
}

void R3BStartrackMapped2Cal::FinishTask()
{

    // fTsHit->Write();
}

// The following ordering function only works if the number of entries of the tree (input_Tree->GetEntries()) is the
// total number of hits in the data stream and not the number of data blocks
// void R3BStartrackMapped2Cal::InsertionSort(vector<long long> & v_ts, vector<long long> & v_index){
void R3BStartrackMapped2Cal::InsertionSort(std::vector<ULong64_t>& v_ts, std::vector<ULong64_t>& v_index)
{

    int v_n = v_ts.size();

    // cout << "v_n size=" << v_n << endl;
    for (int i = 1; i < v_n; i++)
    {

        long long next_ts, next_index;
        next_ts = v_ts.at(i);
        next_index = v_index.at(i);

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
            v_index.at(j) = v_index.at(j - 1);
        }

        // v_ts[j]= next_ts;
        v_ts.at(j) = next_ts;
        // v_index[j]= next_index;
        v_index.at(j) = next_index;
    }

    return;
}

// The following ordering function only works if the number of entries of the tree (input_Tree->GetEntries()) is the the
// number of data blocks and not total number of hits in the data stream
void R3BStartrackMapped2Cal::InsertionSort2(std::vector<long long>& v_ts,
                                            std::vector<long long>& v_block_index,
                                            std::vector<long long>& v_hit_index)
{

    int v_n = v_ts.size();

    cout << "v_n size=" << v_n << endl;
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

    return;
}

ClassImp(R3BStartrackMapped2Cal)
