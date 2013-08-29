#include "R3BTarget2pDigiPar.h"
#include "FairLogger.h"
#include "TMath.h"

R3BTarget2pDigiPar::R3BTarget2pDigiPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void R3BTarget2pDigiPar::putParams(FairParamList* list)
{
  LOG(INFO)<<"R3BTarget2pDigiPar::putParams() called"<<FairLogger::endl;
  if(!list) return;
//  list->add("max_paddle", (Int_t)nMaxPaddle);
//  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BTarget2pDigiPar::getParams(FairParamList* list)
{
  LOG(INFO)<<"R3BTarget2pDigiPar::getParams() called"<<FairLogger::endl;
  if (!list) return kFALSE;
  LOG(INFO)<<"R3BTarget2pDigiPar::getParams() 1 ";
  
//  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
//  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
  return kTRUE;
}


void R3BTarget2pDigiPar::Print(Option_t *option) const
{
    LOG(INFO)<<"Target2p Digi Parameters:"<<FairLogger::endl;
//    LOG(INFO)<<"   Max Paddle   = "<<nMaxPaddle<<FairLogger::endl;
//    LOG(INFO)<<"   Max Plane   = "<<nMaxPlane<<FairLogger::endl;
}

ClassImp(R3BTarget2pDigiPar);
