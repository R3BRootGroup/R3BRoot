// ---------------------------------------------------------------------------------------
// -----                           R3BLosUnpack                                      -----
// -----                  Created 09-04-2014 by D.Kresan                             -----
// ---------------------------------------------------------------------------------------

#include <iomanip>

// ROOT headers
#include "TClonesArray.h"
#include "TMath.h"

// Fair headers
#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

// LOS headers
#include "R3BLosRawHit.h"
#include "R3BLosUnpack.h"

// R3BLosUnpack: Constructor
R3BLosUnpack::R3BLosUnpack(Short_t type, Short_t subType, Short_t procId, Short_t subCrate, Short_t control)
    : FairUnpack(type, subType, procId, subCrate, control)
    , fRawData(new TClonesArray("R3BLosRawHit"))
    , fNHits(0)
{
}

// Virtual R3BLosUnpack: Public method
R3BLosUnpack::~R3BLosUnpack()
{
    LOG(INFO) << "R3BLosUnpack: Delete instance" << FairLogger::endl;
    delete fRawData;
}

// Init: Public method
Bool_t R3BLosUnpack::Init()
{
    Register();
    return kTRUE;
}

// Register: Protected method
void R3BLosUnpack::Register()
{
    //  LOG(DEBUG) << "Registering" << FairLogger::endl;
    LOG(INFO) << "R3BLosUnpack : Registering..." << FairLogger::endl;
    FairRootManager* fMan = FairRootManager::Instance();
    if (!fMan)
    {
        return;
    }
    fMan->Register("LosRawHit", "Los", fRawData, kTRUE);
}

// DoUnpack: Public method
Bool_t R3BLosUnpack::DoUnpack(Int_t* data, Int_t size)
{
    if (size < 2)
    {
        return kFALSE;
    }

    LOG(DEBUG) << "R3BLosUnpack : Unpacking... size = " << size << FairLogger::endl;

    //    LOG(INFO) << "!!!!!!! " << std::hex;
    //    for (Int_t i = 0; i < size; i++)
    //    {
    //        LOG(INFO) << data[i] << " ";
    //    }
    //    LOG(INFO) << std::dec << FairLogger::endl;

    UInt_t* p1 = (UInt_t*)(data + 3); // !!!!! for s438b

    if ((p1[0] & 0xff000000) != 0xab000000)
    {
        FairLogger::GetLogger()->Error(MESSAGE_ORIGIN, "Wrong LOS sub-event header");
        return kFALSE;
    }

    UInt_t l_i = 0;

    UInt_t nr_cha = 0x1ff & (p1[l_i] >> 9);
    LOG(DEBUG) << "R3BLosUnpack : Number of channels to read: " << nr_cha << FairLogger::endl;

    if (nr_cha)
    {
        l_i += 1;
        nr_cha -= 1;
    }

    if (nr_cha)
    {
        l_i += 1;
        nr_cha -= 1;
        while (l_i < size)
        {
            UInt_t raw_ch = (p1[l_i] & 0x7e000000) >> 25;  // raw channel 0...63
            UInt_t t_leading = (p1[l_i] & 0x7ff);          // tdc time
            UInt_t c_leading = (p1[l_i] & 0xfff800) >> 11; // clock
            LOG(DEBUG) << "######### ch:" << raw_ch << ", tdc:" << t_leading << ", clock:" << c_leading
                       << FairLogger::endl;
            new ((*fRawData)[fNHits]) R3BLosRawHit(raw_ch, t_leading, c_leading);
            fNHits += 1;
            if (0 == nr_cha)
            {
                break;
            }
            l_i += 1;
            nr_cha -= 1;
        }
    }

    LOG(DEBUG) << "R3BLosUnpack : Number of hits in LOS: " << fNHits << FairLogger::endl;
    return kTRUE;
}

// Reset: Public method
void R3BLosUnpack::Reset()
{
    LOG(DEBUG) << "Clearing Data Structure" << FairLogger::endl;
    fRawData->Clear();
    fNHits = 0;
}

ClassImp(R3BLosUnpack)
