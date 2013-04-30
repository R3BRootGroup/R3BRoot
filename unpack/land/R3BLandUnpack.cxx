// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BLandUnpack                           -----
// -----                    Created 12.04.2013 by D.Kresan                 -----
// -----------------------------------------------------------------------------

#include <iostream>
using namespace std;

#include "TClonesArray.h"

#include "FairRootManager.h"

#include "TRootEvent.h"
#include "TLANDEvent.h"
#include "R3BLandRawHit.h"
#include "R3BLandUnpack.h"


R3BLandUnpack::R3BLandUnpack(char *strCalDir,
			     Int_t type, Int_t subType)
  : FairUnpack(type, subType)
{
  fLand = new TLANDEvent(strCalDir);
  fLand->DoLANDMapping();
  //fLand->TLANDTSync();
  fRootLand = new TRootLANDEvent();
  fRawData = new TClonesArray("R3BLandRawHit");
}


R3BLandUnpack::~R3BLandUnpack()
{
  delete fLand;
  delete fRootLand;
  delete fRawData;
}


Bool_t R3BLandUnpack::Init()
{
  Register();

  return kTRUE;
}


Bool_t R3BLandUnpack::DoUnpack(Int_t *data, Int_t size)
{
  fLand->ProcessCurrentEventMBS(data, size);
  fLand->GetLANDRawData(fRootLand, kFALSE);
  for(Int_t i = 0; i < fRootLand->rawmulti; i++) {
    new ((*fRawData)[i]) R3BLandRawHit(fRootLand->padID[i],
 				       fRootLand->qdc1[i],
 				       fRootLand->qdc2[i]);
  }
  
  return kTRUE;
}


void R3BLandUnpack::Register()
{
  FairRootManager *fMan = FairRootManager::Instance();
  if(! fMan) {
    return;
  }
  fMan->Register("LandRawHit", "Raw data from LAND", fRawData, kTRUE);
}


void R3BLandUnpack::Reset()
{
  fRawData->Clear();
}


ClassImp(R3BLandUnpack)


