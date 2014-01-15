// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                           R3BLandUnpack                                     -----
// -----                           Version 0.1                                       -----
// -----         Adapted by M.I. Cherciu @ 01.2014 after Y.Gonzalez code             -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

//ROOT headers
#include "TClonesArray.h"
#include "TH1F.h"
#include "TMath.h"
//Fair headers
#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

//Land headers
#include "R3BLandRawHit.h"
#include "R3BLandUnpack.h"


//R3BLandUnpack: Constructor
R3BLandUnpack::R3BLandUnpack(char *strCalDir,
                             Short_t type, Short_t subType,
                             Short_t procId,
                             Short_t subCrate, Short_t control)
: FairUnpack(type, subType, procId, subCrate, control),
fRawData(new TClonesArray("R3BLandRawHit")),
fNHits(0)
{
}



//Virtual R3BLandUnpack: Public method
R3BLandUnpack::~R3BLandUnpack()
{
  LOG(INFO) << "R3BLandUnpack: Delete instance" << FairLogger::endl;
  delete fRawData;
}



//Init: Public method
Bool_t R3BLandUnpack::Init()
{
  Register();
  return kTRUE;
}



//Register: Protected method
void R3BLandUnpack::Register()
{
//  LOG(DEBUG) << "Registering" << FairLogger::endl;
  LOG(INFO) << "Registering ..." << FairLogger::endl;
  FairRootManager *fMan = FairRootManager::Instance();
  if(! fMan) {
    return;
  }
  fMan->Register("LandRawHit", "Raw data from Land", fRawData, kTRUE);
}



//DoUnpack: Public method
Bool_t R3BLandUnpack::DoUnpack(Int_t *data, Int_t size)
{

  UInt_t l_s = 0;
  UInt_t *pl_data = (UInt_t*) data;
  
  
  LOG(DEBUG) << "Unpacking" << FairLogger::endl;
  //LOG(INFO) << "Unpacking" << FairLogger::endl;
    

  while(l_s < size) {
   // Remove 0xadd... words
//    while((data[l_s] & 0xfff00000) == 0xadd00000) {
    while((data[l_s] & 0xfff00000) == 0) {
      l_s++;
    }
    
    ULong_t tac_addr;
    UInt_t tac_ch;
    UShort_t cal;
    ULong_t cntrl_data;
    ULong_t tac_data;
    UInt_t qdc_data;
    tac_addr = (pl_data[l_s] & 0xf8000000) >> 27;
    tac_ch = (pl_data[l_s] & 0x07c00000) >> 22;
    cal = (pl_data[l_s] & 0x003C0000) >> 18;
    cntrl_data = (pl_data[l_s] & 0x0003f000) >> 12;
    tac_data = (pl_data[l_s++] & 0x00000fff);
    qdc_data = (pl_data[l_s++] & 0x00000fff);

    LOG(DEBUG) << "TAC ADDR IS " << tac_addr << FairLogger::endl;
    LOG(DEBUG) << "TAC CH IS " << tac_ch << FairLogger::endl;
    LOG(DEBUG) << "TAC Data IS " << tac_data << FairLogger::endl;
    LOG(DEBUG) << "QDC Data IS " << qdc_data << FairLogger::endl;
       
 
    new ((*fRawData)[fNHits]) R3BLandRawHit(tac_addr, tac_ch, cal, cntrl_data, tac_data, qdc_data);     
    fNHits++;
    }  
 
  LOG(INFO) << "R3BLandUnpack: Number of hits per event in LAND: " << fNHits << FairLogger::endl;
  return kTRUE;
}
 
//Reset: Public method
void R3BLandUnpack::Reset()
{
  LOG(DEBUG) << "Clearing Data Structure" << FairLogger::endl;
//  LOG(INFO) << "Clearing Data Structure" << FairLogger::endl;
  fRawData->Clear();
  fNHits = 0;
}


ClassImp(R3BLandUnpack)
