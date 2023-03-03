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

#include "R3BRoluReader.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "R3BEventHeader.h"
#include "R3BRoluMappedData.h"
#include "TClonesArray.h"
extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_rolu.h"
}
#include "TMath.h"
#define IS_NAN(x) TMath::IsNaN(x)
#define NUM_ROLU_DETECTORS (sizeof data->ROLU / sizeof data->ROLU[0])
#define NUM_ROLU_CHANNELS 4
#include <iostream>

using namespace std;

R3BRoluReader::R3BRoluReader(EXT_STR_h101_ROLU* data, UInt_t offset)
    : R3BReader("R3BRoluReader")
    , fData(data)
    , fOffset(offset)
    , fOnline(kFALSE)
    , fLogger(FairLogger::GetLogger())
    , fArray(new TClonesArray("R3BRoluMappedData"))
    , fArrayTrigger(new TClonesArray("R3BRoluMappedData"))
{
}

R3BRoluReader::~R3BRoluReader()
{
    delete fArray;
    delete fArrayTrigger;
}

Bool_t R3BRoluReader::Init(ext_data_struct_info* a_struct_info)
{

    int ok;

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(ERROR) << "FairRootManager not found";

    // header = (R3BEventHeader*)mgr->GetObject("EventHeader");

    EXT_STR_h101_ROLU_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_ROLU, 0);
    if (!ok)
    {
        perror("ext_data_struct_info_item");
        LOG(error) << "Failed to setup ROLU structure information.";
        return kFALSE;
    }

    // Register output array in tree
    if (!fOnline)
    {
        FairRootManager::Instance()->Register("RoluMapped", "Land", fArray, kTRUE);
        FairRootManager::Instance()->Register("RoluTriggerMapped", "Land", fArrayTrigger, kTRUE);
    }
    else
    {
        FairRootManager::Instance()->Register("RoluMapped", "Land", fArray, kFALSE);
        FairRootManager::Instance()->Register("RoluTriggerMapped", "Land", fArrayTrigger, kFALSE);
    }
    fArray->Clear();

    // clear struct_writer's output struct. Seems ucesb doesn't do that
    // for channels that are unknown to the current ucesb config.
    EXT_STR_h101_ROLU_onion* data = (EXT_STR_h101_ROLU_onion*)fData;

    cout << "ROLU num Dets: " << NUM_ROLU_DETECTORS << endl;
    for (uint32_t d = 0; d < NUM_ROLU_DETECTORS; d++)
    {
        data->ROLU[d].TTFLM = 0;
        data->ROLU[d].TTFTM = 0;
        data->ROLU[d].TTCLM = 0;
        data->ROLU[d].TTCTM = 0;
    }
    return kTRUE;
}

Bool_t R3BRoluReader::Read()
{

    // Convert plain raw data to multi-dimensional array
    EXT_STR_h101_ROLU_onion* data = (EXT_STR_h101_ROLU_onion*)fData;

    /*
     * For variable definition, see structure EXT_STR_h101_ROLU_onion_t
     * in ext_str_h101_rolu.h
     * TTFL = Size of the array TFLv contaning fine leading times
     * TTFLv = Array containing the actual data on the fine leading times
     * TTFLM = No of channels having fine leading time
     * TTFLMI = Array of TFLM size containing the channel numbers of each channel with data
     * TTFLME = Array of TFLM size containing the index of the first element of the next channel in data array TFLv
     *
     * TTCL = Size of the array TCLv contaning coarse leading times
     * TTCLv = Array containing the actual data on the coarse leading times
     * TTCLM = No of channels having coarse leading time
     * TTCLMI = Array of TCLM size containing the channel numbers of each channel with data
     * TTCLME = Array of TCLM size containing the index of the first element of the next channel in data array TCLv
     *
     * The same logic is for trailing times: TTFT, TTFTv, ..., TTCTMI, TTCTME
     */

    // loop over all detectors

    for (uint32_t d = 0; d < 2; d++)
    {

        // Coarse counter reset recovery:
        // Divide the course counter range 0..8191 into four pieces.
        // If we have _any_ hit in the uppermost quarter, then all hits in the
        // lowermost quarter will later be shifted up by the course counter range.

        bool do_increment = false;
        uint32_t const c_tamex3_range = 2048;

        // if (data->ROLU[d].TTCL < 1)
        //   return kFALSE;

        //      if(d==1)   cout<<" **** NEW EVENT **** "<<endl;

        //     if(d==1)  cout<<"NUM DATA Coar: "<<data->ROLU[d].TTCL<<"; NUM CHANNELS: "<<data->ROLU[d].TTCLM<<endl;
        //    if(d==1)  cout<<"NUM DATA Fine: "<<data->ROLU[d].TTFL<<"; NUM CHANNELS: "<<data->ROLU[d].TTFLM<<endl;

        uint32_t numData = data->ROLU[d].TTCL;
        uint32_t numChannels = data->ROLU[d].TTCLM;
        for (uint32_t i = 0; i < numData; i++)
        {
            uint32_t coarse_leading = data->ROLU[d].TTCLv[i];
            if (coarse_leading > 3 * c_tamex3_range / 4)
            {
                do_increment = true;
                break;
            }
        }
        if (!do_increment)
        {
            numData = data->ROLU[d].TTCT;
            numChannels = data->ROLU[d].TTCTM;
            for (uint32_t i = 0; i < numData; i++)
            {
                uint32_t coarse_trailing = data->ROLU[d].TTCTv[i];
                if (coarse_trailing > 3 * c_tamex3_range / 4)
                {
                    do_increment = true;
                    break;
                }
            }
        }

        // Leading mapping.
        numChannels = data->ROLU[d].TTCLM;
        uint32_t curChannelStart = 0;
        //   if(d==1) cout<<"numChannels: "<<numChannels<<", "<<curChannelStart<<endl;
        for (uint32_t i = 0; i < numChannels; i++)
        {
            uint32_t channel = data->ROLU[d].TTCLMI[i];
            uint32_t nextChannelStart = data->ROLU[d].TTCLME[i];
            for (uint32_t j = curChannelStart; j < nextChannelStart; j++)
            {
                uint32_t coarse_leading = data->ROLU[d].TTCLv[j];
                if (do_increment && coarse_leading < c_tamex3_range / 4)
                {
                    coarse_leading += c_tamex3_range;
                }
                new ((*fArray)[fArray->GetEntriesFast()])
                    R3BRoluMappedData(d + 1, channel, 0, data->ROLU[d].TTFLv[j], coarse_leading);

                //       if(d==1)     cout<<"TAMEX leading: "<< d+1<<", "<<channel<<", "<<data->ROLU[d].TTFLv[j]<<", "<<
                //       coarse_leading<<endl;
            }
            curChannelStart = nextChannelStart;
        }

        //
        // Trailing.
        // Matched against leading edges.
        //

        numChannels = data->ROLU[d].TTCTM;
        curChannelStart = 0;
        for (uint32_t i = 0; i < numChannels; i++)
        {
            uint32_t channel = data->ROLU[d].TTCTMI[i];
            uint32_t nextChannelStart = data->ROLU[d].TTCTME[i];
            for (uint32_t j = curChannelStart; j < nextChannelStart; j++)
            {
                uint32_t coarse_trailing = data->ROLU[d].TTCTv[j];
                if (do_increment && coarse_trailing < c_tamex3_range / 4)
                {
                    coarse_trailing += c_tamex3_range;
                }

                // Find a leading edge to pair up with.
                Int_t n = fArray->GetEntriesFast();
                for (Int_t k = 0; k < n; k++)
                {
                    R3BRoluMappedData const* hit = (R3BRoluMappedData*)fArray->At(k);

                    UInt_t const iTypeL = hit->GetType();
                    UInt_t const iCha = hit->GetChannel();

                    // Only consider leading data.
                    if (0 == iTypeL && iCha == channel)
                    {
                        uint32_t coarse_leading = hit->GetTimeCoarse();
                        int32_t tot = coarse_trailing - coarse_leading;
                        // 30 units -> 30 * 5 = 150 ns.
                        if ((tot <= 30) && (tot >= 0))
                        {
                            new ((*fArray)[fArray->GetEntriesFast()])
                                R3BRoluMappedData(d + 1, channel, 1, data->ROLU[d].TTFTv[j], coarse_trailing);

                            //      cout<<"TAMEX trailing: "<< d+1<<", "<<channel<<", "<<data->ROLU[d].TTFTv[j]<<", "<<
                            //         coarse_trailing<<endl;

                            break;
                        }
                    }
                }
            }
            curChannelStart = nextChannelStart;
        }
    }

    // Triggers.
    {
        for (uint32_t i = 0; i < data->ROLU[0].TRIGFL; i++)
        {
            uint32_t ch_i = data->ROLU[0].TRIGFLI[i];
            new ((*fArrayTrigger)[fArrayTrigger->GetEntriesFast()])
                R3BRoluMappedData(3, 1, 1, data->ROLU[0].TRIGFLv[i], data->ROLU[0].TRIGCLv[i]);
        }
    }

    fNEvents += 1;

    return kTRUE;
}

void R3BRoluReader::FinishTask() {}

void R3BRoluReader::Reset()
{
    // Reset the output array
    fArray->Clear();
    fArrayTrigger->Clear();
}

ClassImp(R3BRoluReader)
