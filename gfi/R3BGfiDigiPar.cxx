#include <iostream>
#include "R3BGfiDigiPar.h"
#include "TMath.h"

R3BGfiDigiPar::R3BGfiDigiPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void R3BGfiDigiPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BGfiDigiPar::putParams() called"<<std::endl;
  if(!list) return;
//  list->add("max_paddle", (Int_t)nMaxPaddle);
//  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BGfiDigiPar::getParams(FairParamList* list)
{
  std::cout<<"-I- R3BGfiDigiPar::getParams() called"<<std::endl;
  if (!list) return kFALSE;
  std::cout<<"-I- R3BGfiDigiPar::getParams() 1 ";
  
//  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
//  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
  return kTRUE;
}


void R3BGfiDigiPar::Print()
{
    std::cout<<"-I- Gfi Digi Parameters:"<<std::endl;
//    std::cout<<"   Max Paddle   = "<<nMaxPaddle<<std::endl;
//    std::cout<<"   Max Plane   = "<<nMaxPlane<<std::endl;
}

ClassImp(R3BGfiDigiPar);
