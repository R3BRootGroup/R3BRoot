#include "R3BStartrackHitPar.h"
#include "FairLogger.h"
#include "TMath.h"

R3BStartrackHitPar::R3BStartrackHitPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void R3BStartrackHitPar::putParams(FairParamList* list)
{
  LOG(INFO)<<"-I- R3BStartrackHitPar::putParams() called";
  if(!list) return;
  list->add("fThreshold", (Double_t)fThreshold);
  list->add("fEStripResolution", (Double_t)fEStripResolution);

//  list->add("max_paddle", (Int_t)nMaxPaddle);
//  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BStartrackHitPar::getParams(FairParamList* list)
{
  LOG(INFO)<<"-I- R3BStartrackHitPar::getParams() called";
  if (!list) return kFALSE;
  LOG(INFO)<<"-I- R3BStartrackHitPar::getParams() 1 ";

  if (!list->fill("fThreshold",&fThreshold,1)) return kFALSE;
  if (!list->fill("fEStripResolution",&fEStripResolution,1)) return kFALSE;

  
//  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
//  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
  return kTRUE;
}


void R3BStartrackHitPar::Print(Option_t *option) const
{
    LOG(INFO)<<"-I- STaRTracker Hit Parameters:";
    LOG(INFO)<<" Energy Threshold   = "<< fThreshold;
    LOG(INFO)<<" Energy resolution  = "<< fEStripResolution;

//    LOG(INFO)<<"   Max Paddle   = "<<nMaxPaddle;
//    LOG(INFO)<<"   Max Plane   = "<<nMaxPlane;
}

ClassImp(R3BStartrackHitPar);
