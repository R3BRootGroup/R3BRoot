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

#include <fstream>

#include <FairLogger.h>
#include <TClonesArray.h>

#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaMappingReader.h"

using namespace std;

R3BCalifaMappingReader::R3BCalifaMappingReader(const char* filename)
    : fname(filename)
{
}

InitStatus R3BCalifaMappingReader::Init()
{
    // Read channel mapping
    ifstream fmap(fname.c_str());
    if (!fmap.good())
    {
        LOG(ERROR) << "R3BCalifaMappingReader::Init(): Could not open mapping file " << fname << " for reading!";
        return kFATAL;
    }

    LOG(DEBUG) << "CALIFA Channel Mapping: ";
    int fbxChannel, crystalId;
    while (fmap.good())
    {
        fmap >> fbxChannel >> crystalId;
        channelMap[fbxChannel] = crystalId;

        LOG(DEBUG) << fbxChannel << " - " << crystalId;
    }

    fmap.close();

    // Get CrystalHit array
    fCrystalCalDataCA = dynamic_cast<TClonesArray*>(FairRootManager::Instance()->GetObject("CaloCrystalHit"));
    if (!fCrystalCalDataCA)
    {
        LOG(ERROR) << "R3BCalifaMappingReader::Init(): Could not get CaloCrystalHit";
        return kFATAL;
    }

    return kSUCCESS;
}

void R3BCalifaMappingReader::Exec(Option_t* option)
{
    R3BCalifaCrystalCalData* hit;
    for (int i = 0; i < fCrystalCalDataCA->GetEntries(); i++)
    {
        if (!(hit = dynamic_cast<R3BCalifaCrystalCalData*>(fCrystalCalDataCA->At(i))))
        {
            LOG(ERROR) << "R3BCalifaMappingReader::Exec(): CalifaCrystalCalData is not of type R3BCalifaCrystalCalData";
            return;
        }

        if (channelMap.count(hit->GetCrystalId()) == 0)
        {
            LOG(ERROR) << "R3BCalifaMappingReader::Exec(): Invalid channel number " << hit->GetCrystalId();
            return;
        }

        //    LOG(DEBUG) << "R3BCalifaMappingReader::Exec(): Mapping channel " << hit->GetCrystalId() << " to crystal ID
        //    " << channelMap[hit->GetCrystalId()];

        hit->SetCrystalId(channelMap[hit->GetCrystalId()]);
    }
}

ClassImp(R3BCalifaMappingReader)
