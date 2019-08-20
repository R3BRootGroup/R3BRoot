#include "R3BTargetDigiPar.h"
#include "FairLogger.h"
#include "TMath.h"

R3BTargetDigiPar::R3BTargetDigiPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void R3BTargetDigiPar::putParams(FairParamList* list)
{
  LOG(INFO)<<"R3BTargetDigiPar::putParams() called";
  if(!list) return;
//  list->add("max_paddle", (Int_t)nMaxPaddle);
//  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BTargetDigiPar::getParams(FairParamList* list)
{
  LOG(INFO)<<"R3BTargetDigiPar::getParams() called";
  if (!list) return kFALSE;
  LOG(INFO)<<"R3BTargetDigiPar::getParams() 1 ";
  
//  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
//  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
  return kTRUE;
}


void R3BTargetDigiPar::Print(Option_t *option) const
{
    LOG(INFO)<<"Target Digi Parameters:";
//    LOG(INFO)<<"   Max Paddle   = "<<nMaxPaddle;
//    LOG(INFO)<<"   Max Plane   = "<<nMaxPlane;
}

ClassImp(R3BTargetDigiPar);
