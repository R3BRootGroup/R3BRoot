#include <iostream>
#include "R3BTargetDigiPar.h"
#include "TMath.h"

R3BTargetDigiPar::R3BTargetDigiPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void R3BTargetDigiPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BTargetDigiPar::putParams() called"<<std::endl;
  if(!list) return;
//  list->add("max_paddle", (Int_t)nMaxPaddle);
//  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BTargetDigiPar::getParams(FairParamList* list)
{
  std::cout<<"-I- R3BTargetDigiPar::getParams() called"<<std::endl;
  if (!list) return kFALSE;
  std::cout<<"-I- R3BTargetDigiPar::getParams() 1 ";
  
//  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
//  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
  return kTRUE;
}


void R3BTargetDigiPar::Print()
{
    std::cout<<"-I- Target Digi Parameters:"<<std::endl;
//    std::cout<<"   Max Paddle   = "<<nMaxPaddle<<std::endl;
//    std::cout<<"   Max Plane   = "<<nMaxPlane<<std::endl;
}

ClassImp(R3BTargetDigiPar);
