#include <iostream>
#include "R3BTof2pDigiPar.h"
#include "TMath.h"

R3BTof2pDigiPar::R3BTof2pDigiPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void R3BTof2pDigiPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BTof2pDigiPar::putParams() called"<<std::endl;
  if(!list) return;
//  list->add("max_paddle", (Int_t)nMaxPaddle);
//  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BTof2pDigiPar::getParams(FairParamList* list)
{
  std::cout<<"-I- R3BTof2pDigiPar::getParams() called"<<std::endl;
  if (!list) return kFALSE;
  std::cout<<"-I- R3BTof2pDigiPar::getParams() 1 ";
  
//  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
//  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
  return kTRUE;
}


void R3BTof2pDigiPar::Print(Option_t *option) const
{
    std::cout<<"-I- Tof Digi Parameters:"<<std::endl;
//    std::cout<<"   Max Paddle   = "<<nMaxPaddle<<std::endl;
//    std::cout<<"   Max Plane   = "<<nMaxPlane<<std::endl;
}

ClassImp(R3BTof2pDigiPar);
