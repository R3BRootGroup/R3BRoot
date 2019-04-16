#include "R3BTraDigiPar.h"
#include "FairLogger.h"
#include "TMath.h"

R3BTraDigiPar::R3BTraDigiPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void R3BTraDigiPar::putParams(FairParamList* list)
{
  LOG(INFO) << "R3BTraDigiPar::putParams() called";
  if(!list) return;
//  list->add("max_paddle", (Int_t)nMaxPaddle);
//  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BTraDigiPar::getParams(FairParamList* list)
{
  LOG(INFO) << "R3BTraDigiPar::getParams() called";
  if (!list) return kFALSE;
  LOG(INFO) << "R3BTraDigiPar::getParams() 1 ";
  
//  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
//  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
  return kTRUE;
}


void R3BTraDigiPar::Print(Option_t *option) const
{
    LOG(INFO) << "Tra Digi Parameters:";
//    LOG(INFO)<<"   Max Paddle   = "<<nMaxPaddle;
//    LOG(INFO)<<"   Max Plane   = "<<nMaxPlane;
}

ClassImp(R3BTraDigiPar);
