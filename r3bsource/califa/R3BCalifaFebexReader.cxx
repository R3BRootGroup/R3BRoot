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


#include "FairSink.h"
#include "FairSource.h"

#include "FairRootManager.h"


#include "R3BCalifaFebexReader.h"
#include "R3BCalifaMappedData.h"
#include "R3BLogger.h"

#include "TClonesArray.h"

/**
 ** ext_h101_califa.h was created by running
 ** $unpacker --ntuple=STRUCT_HH,RAW:CALIFA,id=h101_CALIFA,NOTRIGEVENTNO,ext_h101_califa.h
 **/

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_califa.h"
}

R3BCalifaFebexReader::R3BCalifaFebexReader(EXT_STR_h101_CALIFA* data, size_t offset)
    : R3BReader("R3BCalifaFebexReader")
    , fNEvent(0)
    , fData(data)
    , fOffset(offset)
    , fOnline(kFALSE)
      //    , fArraytrig(new TClonesArray("R3BCalifaMappedData"))
{
}

R3BCalifaFebexReader::~R3BCalifaFebexReader()
{
}

Bool_t R3BCalifaFebexReader::Init(ext_data_struct_info* a_struct_info)
{
    Int_t ok;
    R3BLOG(info, "");
    EXT_STR_h101_CALIFA_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_CALIFA, 1);

    if (!ok)
    {
        LLOG(fatal) << "Failed to setup structure information";
        return kFALSE;
    }

    fStructInfo = a_struct_info;

    FairRootManager::Instance()->RegisterAny(R3BCalifaMappedData::default_container_name, fOutMap, !fOnline);

    LLOG(info) << " writing to " << R3BCalifaMappedData::default_container_name << "\n";
    
    memset(fData, 0, sizeof *fData);

    return kTRUE;
}

void R3BCalifaFebexReader::AssertOV()
{
    if (!fStructInfo)
        return;

    bool done{};
    auto si = (ext_data_structure_info*)*fStructInfo;
    int start = 1;
    while (!done)
    {
        const char* name;
        uint32_t offset, map_success;

        int res = ext_data_struct_info_map_success(si, start, &name, &offset, &map_success);
        done = res < 1;
        if (res == 1 && !strcmp(name, "CALIFA_OV") && map_success & EXT_DATA_ITEM_MAP_NOT_FOUND)
        {
            assert(0 && "CALIFA_OV was not set by the experiment unpacker. Please fix your unpacker.");
        }
        start = 0;
    }
    fStructInfo = nullptr;
}

Bool_t R3BCalifaFebexReader::R3BRead()
{
    static bool complained{};
    // on the first event, check the result of struct info
    AssertOV(); // disable on your own risk.

    R3BLOG(debug1, "Event data.");

    // SELECT THE FOR LOOP BASED ON THE MAPPING...
    for (int crystal = 0; crystal < fData->CALIFA_ENE; ++crystal)
    {
      uint16_t channelNumber = fData->CALIFA_ENEI[crystal];
      auto res=fOutMap->emplace(std::make_pair(channelNumber, 
					       R3BCalifaMappedData(channelNumber)));
      assert(res.second && "duplicate crystal number");
      auto& h=(*res.first).second;
      h.fEnergy = fData->CALIFA_ENEv[crystal];
      h.fNf = fData->CALIFA_NFv[crystal];
      h.fNs = fData->CALIFA_NSv[crystal];
      h.fFebexTime = ((uint64_t)fData->CALIFA_TSMSBv[crystal] << 32) | (uint64_t)fData->CALIFA_TSLSBv[crystal];

      h.fWrts = ((uint64_t)fData->CALIFA_WRTS_T4v[crystal] << 48)
	| ((uint64_t)fData->CALIFA_WRTS_T3v[crystal] << 32)
	| ((uint64_t)fData->CALIFA_WRTS_T2v[crystal] << 16)
	| (uint64_t)fData->CALIFA_WRTS_T1v[crystal];
      
      h.fOverFlow = fData->CALIFA_OVv[crystal];
      h.fPileup = fData->CALIFA_PILEUPv[crystal];
      h.fDiscard = fData->CALIFA_DISCARDv[crystal];
      h.fTot = fData->CALIFA_TOTv[crystal];

    }
    fNEvent += 1;
    return kTRUE;
}

void R3BCalifaFebexReader::Reset()
{
    // Reset the output array
    fOutMap->clear();
}

ClassImp(R3BCalifaFebexReader);
