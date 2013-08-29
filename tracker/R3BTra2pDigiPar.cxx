#include "R3BTra2pDigiPar.h"
#include "FairLogger.h"
#include "TMath.h"

R3BTra2pDigiPar::R3BTra2pDigiPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void R3BTra2pDigiPar::putParams(FairParamList* list)
{
  LOG(INFO)<<"-I- R3BTra2pDigiPar::putParams() called"<<FairLogger::endl;
  if(!list) return;
//  list->add("max_paddle", (Int_t)nMaxPaddle);
//  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BTra2pDigiPar::getParams(FairParamList* list)
{
  LOG(INFO)<<"-I- R3BTra2pDigiPar::getParams() called"<<FairLogger::endl;
  if (!list) return kFALSE;
  LOG(INFO)<<"-I- R3BTra2pDigiPar::getParams() 1 ";
  
//  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
//  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
  return kTRUE;
}


void R3BTra2pDigiPar::Print(Option_t *option) const
{
    LOG(INFO)<<"-I- Tra Digi Parameters:"<<FairLogger::endl;
//    LOG(INFO)<<"   Max Paddle   = "<<nMaxPaddle<<FairLogger::endl;
//    LOG(INFO)<<"   Max Plane   = "<<nMaxPlane<<FairLogger::endl;
}

ClassImp(R3BTra2pDigiPar);
