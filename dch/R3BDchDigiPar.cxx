#include <iostream>
#include "R3BDchDigiPar.h"
#include "TMath.h"

R3BDchDigiPar::R3BDchDigiPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void R3BDchDigiPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BDchDigiPar::putParams() called"<<std::endl;
  if(!list) return;
//  list->add("max_paddle", (Int_t)nMaxPaddle);
//  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BDchDigiPar::getParams(FairParamList* list)
{
  std::cout<<"-I- R3BDchDigiPar::getParams() called"<<std::endl;
  if (!list) return kFALSE;
  std::cout<<"-I- R3BDchDigiPar::getParams() 1 ";
  
//  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
//  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
  return kTRUE;
}


void R3BDchDigiPar::Print()
{
    std::cout<<"-I- Dch Digi Parameters:"<<std::endl;
//    std::cout<<"   Max Paddle   = "<<nMaxPaddle<<std::endl;
//    std::cout<<"   Max Plane   = "<<nMaxPlane<<std::endl;
}

ClassImp(R3BDchDigiPar);
