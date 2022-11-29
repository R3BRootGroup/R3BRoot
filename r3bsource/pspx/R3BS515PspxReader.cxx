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
#include "FairRuntimeDb.h"
#include "R3BEventHeader.h"

#include "R3BLogger.h"
#include "R3BPspxMappedData.h"
#include "R3BPspxMappedPar.h"
#include "R3BS515PspxReader.h"

#include "TClonesArray.h"
#include "ext_data_struct_info.hh"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_psp.h"
}

#define LENGTH(x) (sizeof x / sizeof *x)

R3BS515PspxReader::R3BS515PspxReader(EXT_STR_h101_PSP* data, size_t offset)
    : R3BReader("R3BPspxReader")
    , fData(data)
    , fOffset(offset)
    , fOnline(kFALSE)
    , fMappedItems(2 * LENGTH(((EXT_STR_h101_PSP_onion*)data)->PSPX)) // number of faces of detectors
    , fBuffer_i(0)
    , fShift(0)
{
    EXT_STR_h101_PSP_onion* data_o = reinterpret_cast<EXT_STR_h101_PSP_onion*>(fData);
    for (Int_t d = 0; d < 2 * LENGTH(data_o->PSPX); d++)
    {
        fMappedItems[d] = new TClonesArray("R3BPspxMappedData");
    }
    printf("Length: %lu\n", LENGTH(data_o->PSPX));
    R3BLOG(INFO, "Created " << 2 * LENGTH(data_o->PSPX) << " detectors.");
}

R3BS515PspxReader::~R3BS515PspxReader()
{
    R3BLOG(DEBUG1, "");
    EXT_STR_h101_PSP_onion* data = reinterpret_cast<EXT_STR_h101_PSP_onion*>(fData);
    for (Int_t d = 0; d < 2 * LENGTH(data->PSPX); d++)
    {
        delete fMappedItems[d];
    }

    for (Int_t i = 0; i < 20; i++)
    {
        delete fEventBuffer[i];
    }
}

Bool_t R3BS515PspxReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    R3BLOG(INFO, "");
    EXT_STR_h101_PSP_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_PSP, 0);
    if (!ok)
    {
        R3BLOG(ERROR, "Failed to setup structure information.");
        return kFALSE;
    }

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(FATAL, !mgr, "FairRootManager not found");

    header = (R3BEventHeader*)mgr->GetObject("EventHeader.");
    if (!header)
    {
        header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
        R3BLOG(WARNING, "EventHeader. not found");
    }
    const char xy[2] = { 'x', 'y' }; // orientation of detector face
    // Register output array in tree
    EXT_STR_h101_PSP_onion* data = reinterpret_cast<EXT_STR_h101_PSP_onion*>(fData);
    for (Int_t d = 0; d < LENGTH(data->PSPX); d++)
    {
        for (Int_t f = 0; f < 2; f++)
        {
            // Register output array in tree
            FairRootManager::Instance()->Register(Form("Pspx%d_%cMapped", d + 1, xy[f]),
                                                  Form("Pspx%d_%c", d + 1, xy[f]),
                                                  fMappedItems[2 * d + f],
                                                  !fOnline);
            fMappedItems[2 * d + f]->Clear();
            R3BLOG(INFO, "Registered Pspx" << d + 1 << "_" << xy[f]);
        }
    }
    memset(fData, 0, sizeof *fData);
    // allocate the memory for the events that will be stored in the buffer:
    for (Int_t i = 0; i < 20; i++)
    {
        fEventBuffer[i] = new EXT_STR_h101_PSP_onion{};
    }

    return kTRUE;
}

/**
 * Does the unpacking to Mapped level. It is called for every event.
 * Converts plain raw data to multi-dimensional array.
 * Ignores energies with an error message.
 */
Bool_t R3BS515PspxReader::Read()
{
    EXT_STR_h101_PSP_onion* data = reinterpret_cast<EXT_STR_h101_PSP_onion*>(fData);

#if 0
    // this is the data structure we have to read:
    struct {
      struct {
        struct {
          uint32_t E;
          uint32_t EI[32 /* E */];
          uint32_t Ev[32 /* E */];
        } S[2];
      } F[2];
    } PSPX[N];

    F = Face (front/back of detector)
    S = Side (end of strip)
#endif

    if (fShift > 0)
    {
        // skip TPAT = 0 events :
        if (header->GetTpat() == 0)
        {
            return kTRUE;
        }
        // store number = fShift events into buffer
        if (fBuffer_i < fShift)
        {
            *fEventBuffer[fBuffer_i] = *data;
            fBuffer_i++;
            if (fBuffer_i == fShift)
            {
                fBuffer_i = fShift + 1;
            }
        }
        // now instead of reading current event from "data" directly  we read from fEventBuffer
        // reading the event:
        // loop over all detectors
        if (fBuffer_i == fShift)
        {
            // read from Event_buffer[0]
            for (Int_t d = 0; d < LENGTH(fEventBuffer[0]->PSPX); d++)
            {
                // loop over faces
                for (Int_t f = 0; f < 2; f++)
                {
                    std::vector<R3BPspxMappedData*> datas(LENGTH(fEventBuffer[0]->PSPX[0].F[0].S[0].EI));
                    // loop over strip sides
                    for (Int_t s = 0; s < 2; s++)
                    {
                        auto const& dfs = fEventBuffer[0]->PSPX[d].F[f].S[s];
                        uint32_t numChannels = dfs.E;
                        // loop over channels
                        for (Int_t i = 0; i < numChannels; i++)
                        {
                            int32_t strip = dfs.EI[i]; // counting from 1 to max number of channels for an detector
                            int32_t energy = dfs.Ev[i];
                            if (energy < 0)
                                energy =
                                    -1. *
                                    energy; // make sure energy values are positive. Necessary for compatibilty with
                                            // GSI Febex firmware

                            // if (!datas[strip-1]) datas[strip-1] = new
                            // ((*fMappedItems[d])[fMappedItems[d]->GetEntriesFast()])
                            // R3BPspxMappedData(f+1,strip);
                            if (!datas[strip - 1])
                                datas[strip - 1] =
                                    new ((*fMappedItems[2 * d + f])[fMappedItems[2 * d + f]->GetEntriesFast()])
                                        R3BPspxMappedData();
                            // assert(-1 == datas[strip-1]->GetEnergy(s));
                            datas[strip - 1]->SetValue(s, strip, energy);
                        }
                    }
                }
            }
            // store data of current event on the place in fEventBuffer of event that has just been read
            *fEventBuffer[0] = *data;
            // place current event after all other events that are already in the fEventBuffer
            fEventBuffer[fShift] = fEventBuffer[0];
            // translate all events to one element before in the fEventBuffer array (prepare to read from element [0] in
            // the next event)
            for (Int_t i = 0; i < fShift; i++)
            {
                fEventBuffer[i] = fEventBuffer[i + 1];
            }
        }
        if (fBuffer_i == fShift + 1)
        {
            fBuffer_i = fBuffer_i - 1;
        }
    }

    // if fShift = 0 do the standard reading procedure:
    else if (fShift == 0)
    {
        for (Int_t d = 0; d < LENGTH(data->PSPX); d++)
        {
            // loop over faces
            for (Int_t f = 0; f < 2; f++)
            {
                std::vector<R3BPspxMappedData*> datas(LENGTH(data->PSPX[0].F[0].S[0].EI));
                // loop over strip sides
                for (Int_t s = 0; s < 2; s++)
                {
                    auto const& dfs = data->PSPX[d].F[f].S[s];
                    uint32_t numChannels = dfs.E;
                    // loop over channels
                    for (Int_t i = 0; i < numChannels; i++)
                    {
                        int32_t strip = dfs.EI[i]; // counting from 1 to max number of channels for an detector
                        int32_t energy = dfs.Ev[i];
                        if (energy < 0)
                            energy =
                                -1. * energy; // make sure energy values are positive. Necessary for compatibilty with
                                              // GSI Febex firmware

                        // if (!datas[strip-1]) datas[strip-1] = new
                        // ((*fMappedItems[d])[fMappedItems[d]->GetEntriesFast()])
                        // R3BPspxMappedData(f+1,strip);
                        if (!datas[strip - 1])
                            datas[strip - 1] =
                                new ((*fMappedItems[2 * d + f])[fMappedItems[2 * d + f]->GetEntriesFast()])
                                    R3BPspxMappedData();
                        // assert(-1 == datas[strip-1]->GetEnergy(s));
                        datas[strip - 1]->SetValue(s, strip, energy);
                    }
                }
            }
        }
    }
    return kTRUE;
}

void R3BS515PspxReader::Reset()
{
    EXT_STR_h101_PSP_onion* data = reinterpret_cast<EXT_STR_h101_PSP_onion*>(fData);
    for (Int_t d = 0; d < 2 * LENGTH(data->PSPX); d++)
    {
        fMappedItems[d]->Clear();
    }
}

ClassImp(R3BS515PspxReader);
