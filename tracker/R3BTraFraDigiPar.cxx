#include "R3BTraFraDigiPar.h"
#include "FairLogger.h"
#include "TMath.h"

R3BTraFraDigiPar::R3BTraFraDigiPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void R3BTraFraDigiPar::putParams(FairParamList* list)
{
  LOG(INFO)<<"-I- R3BTraFraDigiPar::putParams() called"<<FairLogger::endl;
  if(!list) return;
//  list->add("max_paddle", (Int_t)nMaxPaddle);
//  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BTraFraDigiPar::getParams(FairParamList* list)
{
  LOG(INFO)<<"-I- R3BTraFraDigiPar::getParams() called"<<FairLogger::endl;
  if (!list) return kFALSE;
  LOG(INFO)<<"-I- R3BTraFraDigiPar::getParams() 1 ";
  
//  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
//  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
  return kTRUE;
}


void R3BTraFraDigiPar::Print(Option_t *option) const
{
    LOG(INFO)<<"-I- TraFra Digi Parameters:"<<FairLogger::endl;
//    LOG(INFO)<<"   Max Paddle   = "<<nMaxPaddle<<FairLogger::endl;
//    LOG(INFO)<<"   Max Plane   = "<<nMaxPlane<<FairLogger::endl;
}

ClassImp(R3BTraFraDigiPar);
