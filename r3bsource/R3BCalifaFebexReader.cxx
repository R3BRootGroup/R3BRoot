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

#include "R3BCalifaFebexReader.h"
#include "FairLogger.h"

#include "FairRootManager.h"
#include "R3BCalifaMappedData.h"
#include "TClonesArray.h"
#include "ext_data_struct_info.hh"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_califa.h"
    // for future reference: ext_h101_califa.h was created by running
    // $unpacker --ntuple=STRUCT_HH,RAW:CALIFA,id=h101_califa,NOTRIGEVENTNO,ext_h101_califa.h
}

R3BCalifaFebexReader::R3BCalifaFebexReader(EXT_STR_h101_califa* data, size_t offset)
    : R3BReader("R3BCalifaFebexReader")
    , fData(data)
    , fOffset(offset)
{
}

R3BCalifaFebexReader::~R3BCalifaFebexReader()
{
    // note: we do not delete fArray, as FairRootManager still may
    // hold a copy of the pointer.
}

Bool_t R3BCalifaFebexReader::Init(ext_data_struct_info* a_struct_info)
{
    if (!fArray)
        fArray = (new TClonesArray("R3BCalifaMappedData"));

    int ok;
    LOG(info) << __PRETTY_FUNCTION__;
    EXT_STR_h101_califa_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_califa, 0);

    if (!ok)
    {
        LOG(error) << __PRETTY_FUNCTION__ << ": Failed to setup structure information.";
        return false;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("CalifaMappedData", "Califa", fArray, !fOnline);

    return true;
}

Bool_t R3BCalifaFebexReader::Read()
{
    // LOG(DEBUG) << __PRETTY_FUNCTION__ << " Event data.";
    assert(fArray->GetEntriesFast() == 0);
    for (int crystal = 0; crystal < fData->CALIFA_ENE; ++crystal)
    {
        auto& m = *(new ((*fArray)[crystal]) R3BCalifaMappedData());
        m.fCrystalId = fData->CALIFA_ENEI[crystal];

        m.fEnergy = fData->CALIFA_ENEv[crystal];
        m.fNf = fData->CALIFA_NFv[crystal];
        m.fNs = fData->CALIFA_NSv[crystal];
        m.fTot = fData->CALIFA_TOTv[crystal];
        // clang-format off
        m.fFebexTS = cast_shift(fData->CALIFA_TSMSBv[crystal], 32)
          | cast_shift(fData->CALIFA_TSLSBv[crystal], 0);

// Below are not in s494 unpacker:       
        m.fWRTS = 0;
       
        m.fOverflow = 0;
        m.fPileup = 0;
        m.fDiscard = 0;
        
/*
        m.fWRTS = cast_shift(fData->CALIFA_WRTS_T4v[crystal], 48)
          | cast_shift(fData->CALIFA_WRTS_T3v[crystal], 32)
          | cast_shift(fData->CALIFA_WRTS_T2v[crystal], 16)
          | cast_shift(fData->CALIFA_WRTS_T1v[crystal], 0);
        // clang-format on

        m.fOverflow = fData->CALIFA_OVv[crystal];
        m.fPileup = fData->CALIFA_PILEUPv[crystal];
        m.fDiscard = fData->CALIFA_DISCARDv[crystal];
*/
    }
    fNEvent += 1;
    return true;
}

// clang-format off

void R3BCalifaFebexReader::Reset()
{
  fArray->Clear();
}
// clang-format on

ClassImp(R3BCalifaFebexReader);
