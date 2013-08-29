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
  LOG(INFO) << "R3BTraDigiPar::putParams() called" << FairLogger::endl;
  if(!list) return;
//  list->add("max_paddle", (Int_t)nMaxPaddle);
//  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BTraDigiPar::getParams(FairParamList* list)
{
  LOG(INFO) << "R3BTraDigiPar::getParams() called" << FairLogger::endl;
  if (!list) return kFALSE;
  LOG(INFO) << "R3BTraDigiPar::getParams() 1 " << FairLogger::endl;
  
//  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
//  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
  return kTRUE;
}


void R3BTraDigiPar::Print(Option_t *option) const
{
    LOG(INFO) << "Tra Digi Parameters:" << FairLogger::endl;
//    LOG(INFO)<<"   Max Paddle   = "<<nMaxPaddle<<FairLogger::endl;
//    LOG(INFO)<<"   Max Plane   = "<<nMaxPlane<<FairLogger::endl;
}

ClassImp(R3BTraDigiPar);
