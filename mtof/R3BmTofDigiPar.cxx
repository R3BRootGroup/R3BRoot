#include <iostream>
#include "R3BmTofDigiPar.h"
#include "TMath.h"

R3BmTofDigiPar::R3BmTofDigiPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void R3BmTofDigiPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BmTofDigiPar::putParams() called"<<std::endl;
  if(!list) return;
//  list->add("max_paddle", (Int_t)nMaxPaddle);
//  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BmTofDigiPar::getParams(FairParamList* list)
{
  std::cout<<"-I- R3BmTofDigiPar::getParams() called"<<std::endl;
  if (!list) return kFALSE;
  std::cout<<"-I- R3BmTofDigiPar::getParams() 1 ";
  
//  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
//  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
  return kTRUE;
}


void R3BmTofDigiPar::Print(Option_t *option) const
{
    std::cout<<"-I- mTof Digi Parameters:"<<std::endl;
//    std::cout<<"   Max Paddle   = "<<nMaxPaddle<<std::endl;
//    std::cout<<"   Max Plane   = "<<nMaxPlane<<std::endl;
}

ClassImp(R3BmTofDigiPar);
