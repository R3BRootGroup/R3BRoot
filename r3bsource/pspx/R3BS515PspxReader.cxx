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

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "R3BEventHeader.h"

#include "R3BLogger.h"
#include "R3BPspxMappedData.h"
#include "R3BS515PspxReader.h"

#include "TClonesArray.h"
#include "ext_data_struct_info.hh"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_psp.h"
}

template <typename T, size_t N>
static size_t LENGTH(T (&)[N])
{
    return N;
}

R3BS515PspxReader::R3BS515PspxReader(EXT_STR_h101_PSP_onion* data, size_t offset)
    : R3BReader("R3BPspxReader")
    , fDataOnion(data)
    , fOffset(offset)
    , fOnline(kFALSE)
    , fMappedItems(2 * LENGTH(data->PSPX)) // number of faces of detectors
    , fBuffer_i(0)
    , fShift(0)
{
    assert(fDataOnion);
    for (Int_t d = 0; d < 2 * LENGTH(fDataOnion->PSPX); d++)
    {
        fMappedItems[d] = new TClonesArray("R3BPspxMappedData");
    }
    R3BLOG(info, "Created " << 2 * LENGTH(fDataOnion->PSPX) << " detectors.");
}

R3BS515PspxReader::~R3BS515PspxReader()
{
    R3BLOG(debug1, "");
    for (Int_t d = 0; d < 2 * LENGTH(fDataOnion->PSPX); d++)
    {
        delete fMappedItems[d];
    }
}

Bool_t R3BS515PspxReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    R3BLOG(info, "");
    EXT_STR_h101_PSP_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_PSP, 0);
    if (!ok)
    {
        R3BLOG(error, "Failed to setup structure information.");
        return kFALSE;
    }

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, !mgr, "FairRootManager not found");

    header = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    if (!header)
    {
        R3BLOG(fatal, "EventHeader. not found");
    }
    const char xy[2] = { 'x', 'y' }; // orientation of detector face
    // Register output array in tree
    for (Int_t d = 0; d < LENGTH(fDataOnion->PSPX); d++)
    {
        for (Int_t f = 0; f < 2; f++)
        {
            // Register output array in tree
            FairRootManager::Instance()->Register(Form("Pspx%d_%cMapped", d + 1, xy[f]),
                                                  Form("Pspx%d_%c", d + 1, xy[f]),
                                                  fMappedItems[2 * d + f],
                                                  !fOnline);
            fMappedItems[2 * d + f]->Clear();
            R3BLOG(info, "Registered Pspx" << d + 1 << "_" << xy[f]);
        }
    }
    *fDataOnion = {};

    // allocate the memory for the events that will be stored in the buffer:
    for (Int_t i = 0; i < LENGTH(fEventBuffer); i++)
    {
        fEventBuffer[i] = new EXT_STR_h101_PSP_onion{};
    }

    return kTRUE;
}

Bool_t R3BS515PspxReader::WriteMappedData(EXT_STR_h101_PSP_onion* source)
{
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
    // loop over all detectors
    for (Int_t d = 0; d < LENGTH(source->PSPX); d++)
    {
        // loop over faces
        for (Int_t f = 0; f < 2; f++)
        {
            std::vector<R3BPspxMappedData*> datas(LENGTH(source->PSPX[0].F[0].S[0].EI));
            // loop over strip sides
            for (Int_t s = 0; s < 2; s++)
            {
                auto const& dfs = source->PSPX[d].F[f].S[s];
                uint32_t numChannels = dfs.E;
                // loop over channels
                for (Int_t i = 0; i < numChannels; i++)
                {
                    int32_t strip = dfs.EI[i]; // counting from 1 to max number of channels for an detector
                    int32_t energy = dfs.Ev[i];
                    if (energy < 0)
                        energy = -1. * energy; // make sure energy values are positive. Necessary for compatibilty with
                                               // GSI Febex firmware

                    // if (!datas[strip-1]) datas[strip-1] = new
                    // ((*fMappedItems[d])[fMappedItems[d]->GetEntriesFast()])
                    // R3BPspxMappedData(f+1,strip);
                    if (!datas[strip - 1])
                        datas[strip - 1] = new ((*fMappedItems[2 * d + f])[fMappedItems[2 * d + f]->GetEntriesFast()])
                            R3BPspxMappedData();
                    // assert(-1 == datas[strip-1]->GetEnergy(s));
                    datas[strip - 1]->SetValue(s, strip, energy);
                }
            }
        }
    }
    return kTRUE;
}

/**
 * Does the unpacking to Mapped level. It is called for every event.
 * Converts plain raw data to multi-dimensional array.
 * Ignores energies with an error message.
 */
Bool_t R3BS515PspxReader::R3BRead()
{
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
            *fEventBuffer[fBuffer_i] = *fDataOnion;
            fBuffer_i++;
            if (fBuffer_i == fShift)
            {
                return kTRUE;
            }
        }
        // now instead of reading current event from "data" directly  we read from fEventBuffer

        if (fBuffer_i == fShift)
        {
            // read from Event_buffer[0]
            WriteMappedData(fEventBuffer[0]);
            // store data of current event on the place in fEventBuffer of event that has just been read
            *fEventBuffer[0] = *fDataOnion;
            // place current event after all other events that are already in the fEventBuffer
            fEventBuffer[fShift] = fEventBuffer[0];
            // translate all events to one element before in the fEventBuffer array (prepare to read from
            // element [0] in the next event)
            for (Int_t i = 0; i < fShift; i++)
            {
                fEventBuffer[i] = fEventBuffer[i + 1];
            }
        }
    }
    // if fShift = 0 do the standard reading procedure:
    else if (fShift == 0)
    {
        WriteMappedData(fDataOnion);
    }
    return kTRUE;
}

void R3BS515PspxReader::Reset()
{
    for (Int_t d = 0; d < 2 * LENGTH(fDataOnion->PSPX); d++)
    {
        fMappedItems[d]->Clear();
    }
}

ClassImp(R3BS515PspxReader);
