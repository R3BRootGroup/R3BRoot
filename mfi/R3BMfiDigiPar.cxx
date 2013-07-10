#include <iostream>
#include "R3BMfiDigiPar.h"
#include "TMath.h"

R3BMfiDigiPar::R3BMfiDigiPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void R3BMfiDigiPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BMfiDigiPar::putParams() called"<<std::endl;
  if(!list) return;
//  list->add("max_paddle", (Int_t)nMaxPaddle);
//  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BMfiDigiPar::getParams(FairParamList* list)
{
  std::cout<<"-I- R3BMfiDigiPar::getParams() called"<<std::endl;
  if (!list) return kFALSE;
  std::cout<<"-I- R3BMfiDigiPar::getParams() 1 ";
  
//  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
//  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
  return kTRUE;
}


void R3BMfiDigiPar::Print()
{
    std::cout<<"-I- Mfi Digi Parameters:"<<std::endl;
//    std::cout<<"   Max Paddle   = "<<nMaxPaddle<<std::endl;
//    std::cout<<"   Max Plane   = "<<nMaxPlane<<std::endl;
}

ClassImp(R3BMfiDigiPar);
