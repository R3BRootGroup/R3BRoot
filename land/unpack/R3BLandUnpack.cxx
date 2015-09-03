// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                           R3BLandUnpack                                     -----
// -----                           Version 0.1                                       -----
// -----         Adapted by M.I. Cherciu @ 01.2014 after Y.Gonzalez code             -----
// -----                                                                             -----
// -----        * modification @ 03.2014: added TClonesArray for R3BLandHit          -----
// ---------------------------------------------------------------------------------------

// ROOT headers
#include "TClonesArray.h"
#include "TH1F.h"
#include "TMath.h"

// Fair headers
#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

// Land headers
#include "R3BLandRawHit.h"
#include "R3BLandUnpack.h"
#include "R3BLandHit.h"

// R3BLandUnpack: Constructor
R3BLandUnpack::R3BLandUnpack(Short_t type, Short_t subType, Short_t procId, Short_t subCrate, Short_t control)
    : FairUnpack(type, subType, procId, subCrate, control)
    , fRawData(new TClonesArray("R3BLandRawHit"))
    , fNHits(0)
{
}

// Virtual R3BLandUnpack: Public method
R3BLandUnpack::~R3BLandUnpack()
{
    LOG(INFO) << "R3BLandUnpack: Delete instance" << FairLogger::endl;
    delete fRawData;
}

// Init: Public method
Bool_t R3BLandUnpack::Init()
{
    Register();
    return kTRUE;
}

// Register: Protected method
void R3BLandUnpack::Register()
{
    //  LOG(DEBUG) << "Registering" << FairLogger::endl;
    LOG(INFO) << "R3BLandUnpack : Registering..." << FairLogger::endl;
    FairRootManager* fMan = FairRootManager::Instance();
    if (!fMan)
    {
        return;
    }
    fMan->Register("LandRawHit", "Land", fRawData, kFALSE);
}

// DoUnpack: Public method
Bool_t R3BLandUnpack::DoUnpack(Int_t* data, Int_t size)
{
    LOG(DEBUG) << "R3BLandUnpack : Unpacking... size = " << size << FairLogger::endl;

    UInt_t l_i = 0;

    Int_t n17 = 0;

    while (l_i < size)
    {
        n17 = 0;

        UInt_t* p1 = (UInt_t*)(data + l_i);
        UInt_t l_sam_id = (p1[0] & 0xf0000000) >> 28; // identifies the sam
        UInt_t l_gtb_id = (p1[0] & 0x0f000000) >> 24; // 0 or 1, identifies which of the 2 cables of the sam
        UInt_t l_lec = (p1[0] & 0x00f00000) >> 20;
        UInt_t l_da_siz = (p1[0] & 0x000001ff);

        LOG(DEBUG) << "R3BLandUnpack : SAM:" << l_sam_id << ",  GTB:" << l_gtb_id << ",  lec:" << l_lec << ",  size:" << l_da_siz << FairLogger::endl;

        l_i += 1;

        p1 = (UInt_t*)(data + l_i);

        for (Int_t i1 = 0; i1 < l_da_siz; i1 += 2)
        {
            UInt_t tac_addr;
            UInt_t tac_ch;
            UInt_t cal;
            UInt_t clock;
            UInt_t tac_data;
            UInt_t qdc_data;
            tac_addr = (p1[i1] & 0xf8000000) >> 27;
            tac_ch = (p1[i1] & 0x07c00000) >> 22;
            cal = (p1[i1] & 0x003C0000) >> 18;
            clock = 63 - ((p1[i1] & 0x0003f000) >> 12);
            tac_data = 4095 - ((p1[i1] & 0x00000fff));
            qdc_data = (p1[i1 + 1] & 0x00000fff);
            l_i += 2;

            if (16 == tac_ch)
            {
                n17 += 1;
            }
            LOG(DEBUG) << "R3BLandUnpack : TAC ADDR IS " << tac_addr << ",  TAC CH IS " << tac_ch << ",  TAC Data IS " << tac_data << ",  QDC Data IS " << qdc_data
            << FairLogger::endl;
            new ((*fRawData)[fNHits]) R3BLandRawHit(l_sam_id, l_gtb_id, tac_addr, tac_ch, cal, clock, tac_data, qdc_data);
            fNHits++;
        }

        LOG(DEBUG) << "R3BLandUnpack : n17=" << n17 << FairLogger::endl;
    }

    LOG(DEBUG) << "R3BLandUnpack : Number of hits in LAND: " << fNHits << FairLogger::endl;
    return kTRUE;
}

// Reset: Public method
void R3BLandUnpack::Reset()
{
    LOG(DEBUG) << "R3BLandUnpack : Clearing Data Structure" << FairLogger::endl;
    fRawData->Clear();
    fNHits = 0;
}

ClassImp(R3BLandUnpack)
