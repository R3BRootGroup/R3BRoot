// ----------------------------------------------------------------------
// -----                         R3BTimeStampUnpack                 -----
// -----                  Created 26-02-2015 by D. Kresan           -----
// ----------------------------------------------------------------------

#include "R3BTimeStampUnpack.h"
#include "R3BEventHeader.h"

#include "FairRootManager.h"
#include "FairLogger.h"

#define WR__ID_L16   0x03e1
#define WR__ID_M16   0x04e1
#define WR__ID_H16   0x05e1
#define WR__ID_T16   0x06e1

R3BTimeStampUnpack::R3BTimeStampUnpack(Short_t type, Short_t subType, Short_t procId, Short_t subCrate, Short_t control)
    : FairUnpack(type, subType, procId, subCrate, control)
    , fHeader(NULL)
{
}

R3BTimeStampUnpack::~R3BTimeStampUnpack()
{
}

Bool_t R3BTimeStampUnpack::Init()
{
    FairRootManager *rootMgr = FairRootManager::Instance();
    
    fHeader = (R3BEventHeader*)rootMgr->GetObject("R3BEventHeader");
    
    return kTRUE;
}

Bool_t R3BTimeStampUnpack::DoUnpack(Int_t* data, Int_t size)
{
    UInt_t l_s = 0;
    ULong64_t rabbit1, rabbit2, rabbit3, rabbit4;
    
    l_s++; // skip first module id (should be 0x0300)
    
    if((data[l_s] >> 16) != WR__ID_L16)
    {
        LOG(ERROR) << "Broken time stamp..." << FairLogger::endl;
    }
    rabbit1 =  data[l_s++] & 0xffff;
    
    if((data[l_s] >> 16) != WR__ID_M16)
    {
        LOG(ERROR) << "Broken time stamp..." << FairLogger::endl;
    }
    rabbit2 =  data[l_s++] & 0xffff;
    
    if((data[l_s] >> 16) != WR__ID_H16)
    {
        LOG(ERROR) << "Broken time stamp..." << FairLogger::endl;
    }
    rabbit3 =  data[l_s++] & 0xffff;
    
    if((data[l_s] >> 16) != WR__ID_T16)
    {
        LOG(ERROR) << "Broken time stamp..." << FairLogger::endl;
    }
    rabbit4 =  data[l_s++] & 0xffff;
    
    ULong64_t rabbitStamp = (rabbit4 << 48) | (rabbit3 << 32) | ( rabbit2 << 16 ) | rabbit1;
    
    fHeader->SetTimeStamp(rabbitStamp);
    
//    LOG(INFO) << FairLogger::endl << FairLogger::endl << "TimeStamp = " << rabbit1 << " : " << rabbit2 << " : " << rabbit3 << " : " << rabbit4 << "  =  " << rabbitStamp << FairLogger::endl;
    
    return kTRUE;
}

void R3BTimeStampUnpack::Reset()
{
    fHeader->SetTimeStamp(0);
}

ClassImp(R3BTimeStampUnpack)
