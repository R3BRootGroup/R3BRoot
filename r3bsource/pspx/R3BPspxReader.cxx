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

#include "R3BLogger.h"
#include "R3BPspxMappedData.h"
#include "R3BPspxMappedPar.h"
#include "R3BPspxReader.h"

#include "TClonesArray.h"
#include "ext_data_struct_info.hh"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_psp.h"
}

#define LENGTH(x) (sizeof x / sizeof *x)

R3BPspxReader::R3BPspxReader(EXT_STR_h101_PSP* data, size_t offset)
    : R3BReader("R3BPspxReader")
    , fData(data)
    , fOffset(offset)
    , fOnline(kFALSE)
    , fMappedItems(2 * LENGTH(((EXT_STR_h101_PSP_onion*)data)->PSPX)) // number of faces of detectors
{
    EXT_STR_h101_PSP_onion* data_o = (EXT_STR_h101_PSP_onion*)fData;
    for (Int_t d = 0; d < 2 * LENGTH(data_o->PSPX); d++)
    {
        fMappedItems[d] = new TClonesArray("R3BPspxMappedData");
    }
    printf("Length: %lu\n", LENGTH(data_o->PSPX));
    R3BLOG(INFO, "Created " << 2 * LENGTH(data_o->PSPX) << " detectors.");
}

R3BPspxReader::~R3BPspxReader()
{
    R3BLOG(DEBUG1, "");
    EXT_STR_h101_PSP_onion* data = (EXT_STR_h101_PSP_onion*)fData;
    for (Int_t d = 0; d < 2 * LENGTH(data->PSPX); d++)
    {
        delete fMappedItems[d];
    }
}

Bool_t R3BPspxReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    R3BLOG(INFO, "");
    EXT_STR_h101_PSP_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_PSP, 0);
    if (!ok)
    {
        R3BLOG(ERROR, "Failed to setup structure information.");
        return kFALSE;
    }
    const char xy[2] = { 'x', 'y' }; // orientation of detector face
    // Register output array in tree
    EXT_STR_h101_PSP_onion* data = (EXT_STR_h101_PSP_onion*)fData;
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

    return kTRUE;
}

/**
 * Does the unpacking to Mapped level. It is called for every event.
 * Converts plain raw data to multi-dimensional array.
 * Ignores energies with an error message.
 */
Bool_t R3BPspxReader::Read()
{
    EXT_STR_h101_PSP_onion* data = (EXT_STR_h101_PSP_onion*)fData;

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
                        energy = -1. * energy; // make sure energy values are positive. Necessary for compatibilty with
                                               // GSI Febex firmware

                    // if (!datas[strip-1]) datas[strip-1] = new ((*fMappedItems[d])[fMappedItems[d]->GetEntriesFast()])
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

void R3BPspxReader::Reset()
{
    EXT_STR_h101_PSP_onion* data = (EXT_STR_h101_PSP_onion*)fData;
    for (Int_t d = 0; d < 2 * LENGTH(data->PSPX); d++)
    {
        fMappedItems[d]->Clear();
    }
}

ClassImp(R3BPspxReader);
