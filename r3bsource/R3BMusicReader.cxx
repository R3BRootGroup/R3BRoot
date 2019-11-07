#include "FairLogger.h"

#include "FairRootManager.h"
#include "R3BMusicMappedData.h"
#include "R3BMusicReader.h"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_music.h"
}

#include <iostream>

using namespace std;

R3BMusicReader::R3BMusicReader(EXT_STR_h101_MUSIC* data, UInt_t offset)
    : R3BReader("R3BMusicReader")
    , fData(data)
    , fOffset(offset)
    , fLogger(FairLogger::GetLogger())
    , fArray(new TClonesArray("R3BMusicMappedData"))
{
}

R3BMusicReader::~R3BMusicReader()
{
    if (fArray)
    {
        delete fArray;
    }
}

Bool_t R3BMusicReader::Init(ext_data_struct_info* a_struct_info)
{
    int ok;
    EXT_STR_h101_MUSIC_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_MUSIC, 0);
    if (!ok)
    {
        perror("ext_data_struct_info_item");
        LOG(ERROR) << "Failed to setup structure information.";
        return kFALSE;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("MusicMappedData", "Music", fArray, kTRUE);

    // clear struct_writer's output struct. Seems ucesb doesn't do that
    // for channels that are unknown to the current ucesb config.
    EXT_STR_h101_MUSIC_onion* data = (EXT_STR_h101_MUSIC_onion*)fData;
    data->MUSIC_EM = 0;
    data->MUSIC_TM = 0;
    data->MUSIC_TREFM = 0;
    for (int mult = 0; mult < NUM_MUSIC_ANODES; mult++)
        multPerAnode[mult] = 0;

    return kTRUE;
}

Bool_t R3BMusicReader::Read()
{
    // Convert plain raw data to multi-dimensional array
    EXT_STR_h101_MUSIC_onion* data = (EXT_STR_h101_MUSIC_onion*)fData;

    ReadData(data);
    return kTRUE;
}

void R3BMusicReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

Bool_t R3BMusicReader::ReadData(EXT_STR_h101_MUSIC_onion* data)
{

    // --- ---------------- --- //
    // --- TREF : TIME ONLY --- //
    // --- ---------------- --- //

    // return the number of Tref
    // good event : nTref=1 (one channel only, which can have multi hits)
    UShort_t nTref = data->MUSIC_TREFM;
    if (nTref > 1)
        LOG(ERROR) << "R3BMusicReader::ReadData ERROR ! ONLY ONE Tref SIGNAL BUT HERE MULTIPLE SIGNALS IS FOUND!";

    uint32_t curTref = 0;
    uint32_t nextTref;
    if (nTref == 1)
    {
        nextTref = data->MUSIC_TREFME[0];
        multPerAnode[8] = nextTref - curTref;
        if (multPerAnode[8] != data->MUSIC_TREF)
            LOG(ERROR) << "R3BMusicReader::ReadData ERROR ! multiplicity of Tref not consistent!";
        for (int hit = curTref; hit < nextTref; hit++)
            R3BMusicMappedData* mapped =
                new ((*fArray)[fArray->GetEntriesFast()]) R3BMusicMappedData(9, data->MUSIC_TREFv[hit], 0);
    }

    // --- ---------------------- --- //
    // --- ANODES : ENERGY + TIME --- //
    // --- ---------------------- --- //

    // 0<=nAnodesEnergy<=16
    // 0<=nAnodesTime<=16
    UShort_t nAnodesEnergy = data->MUSIC_EM;
    UShort_t nAnodesTime = data->MUSIC_TM;

    // mail from R. Schneider from June 16, 2016: "if you have several hits of one channel in the window of interest,
    // you get an energy and a timing for each hit."
    // --> multiplicity per anode (for anode 1 to anode 8) should also be the same in energy and time
    // mail from R. Schneider from May 21st 2019 : "the hits from one channel are kept in the chronological order."
    // --> for one anode with multi-hit, the first hit in energy correspond to the first hit in time
    if (nAnodesEnergy != nAnodesTime)
        LOG(ERROR) << "R3BMusicReader::ReadData ERROR ! NOT THE SAME NUMBER OF ANODES HITTED IN ENERGY AND TIME";

    // ENERGY AND TIME ARE SORTED
    uint32_t curAnodeTimeStart = 0;
    uint32_t curAnodeEnergyStart = 0;
    for (UShort_t a = 0; a < nAnodesTime; a++)
    {
        // EMI and TMI give the 1-based anode number
        UShort_t idAnodeTime = data->MUSIC_TMI[a];
        UShort_t idAnodeEnergy = data->MUSIC_EMI[a];
        if (idAnodeEnergy != idAnodeTime)
            LOG(ERROR) << "R3BMusicReader::ReadData ERROR ! MISMATCH FOR ANODE ID IN ENERGY #" << idAnodeEnergy
                       << " AND TIME #" << idAnodeTime;
        uint32_t nextAnodeTimeStart = data->MUSIC_TME[a];
        uint32_t nextAnodeEnergyStart = data->MUSIC_EME[a];
        multPerAnode[idAnodeTime - 1] = nextAnodeTimeStart - curAnodeTimeStart;
        if (multPerAnode[idAnodeTime - 1] != (nextAnodeEnergyStart - curAnodeEnergyStart))
            LOG(ERROR) << "R3BMusicReader::ReadData ERROR ! MISMATCH FOR MULTIPLICITY PER ANODE IN ENERGY AND TIME";
        for (int hit = curAnodeTimeStart; hit < nextAnodeTimeStart; hit++)
        {
            R3BMusicMappedData* mapped = new ((*fArray)[fArray->GetEntriesFast()])
                R3BMusicMappedData(idAnodeEnergy, data->MUSIC_Tv[hit], data->MUSIC_Ev[hit]);
        }
        curAnodeEnergyStart = nextAnodeEnergyStart;
        curAnodeTimeStart = nextAnodeTimeStart;
    } // end of loop over the anodes from 1 to 8

    return kTRUE;
}

ClassImp(R3BMusicReader)
