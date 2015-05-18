#include <iostream>
#include "R3BdTofDigiPar.h"
#include "TMath.h"

R3BdTofDigiPar::R3BdTofDigiPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void R3BdTofDigiPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BdTofDigiPar::putParams() called"<<std::endl;
  if(!list) return;
//  list->add("max_paddle", (Int_t)nMaxPaddle);
//  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BdTofDigiPar::getParams(FairParamList* list)
{
  std::cout<<"-I- R3BdTofDigiPar::getParams() called"<<std::endl;
  if (!list) return kFALSE;
  std::cout<<"-I- R3BdTofDigiPar::getParams() 1 ";
  
//  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
//  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
  return kTRUE;
}


void R3BdTofDigiPar::Print(Option_t *option) const
{
    std::cout<<"-I- mTof Digi Parameters:"<<std::endl;
//    std::cout<<"   Max Paddle   = "<<nMaxPaddle<<std::endl;
//    std::cout<<"   Max Plane   = "<<nMaxPlane<<std::endl;
}

ClassImp(R3BdTofDigiPar);
