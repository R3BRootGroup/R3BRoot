#include <iostream>
#include "R3BTra2pDigiPar.h"
#include "TMath.h"

R3BTra2pDigiPar::R3BTra2pDigiPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void R3BTra2pDigiPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BTra2pDigiPar::putParams() called"<<std::endl;
  if(!list) return;
//  list->add("max_paddle", (Int_t)nMaxPaddle);
//  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BTra2pDigiPar::getParams(FairParamList* list)
{
  std::cout<<"-I- R3BTra2pDigiPar::getParams() called"<<std::endl;
  if (!list) return kFALSE;
  std::cout<<"-I- R3BTra2pDigiPar::getParams() 1 ";
  
//  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
//  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
  return kTRUE;
}


void R3BTra2pDigiPar::Print(Option_t *option) const
{
    std::cout<<"-I- Tra Digi Parameters:"<<std::endl;
//    std::cout<<"   Max Paddle   = "<<nMaxPaddle<<std::endl;
//    std::cout<<"   Max Plane   = "<<nMaxPlane<<std::endl;
}

ClassImp(R3BTra2pDigiPar);
