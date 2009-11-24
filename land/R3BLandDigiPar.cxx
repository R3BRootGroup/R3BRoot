#include <iostream>
#include "R3BLandDigiPar.h"
#include "TMath.h"

R3BLandDigiPar::R3BLandDigiPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void R3BLandDigiPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BLandDigiPar::putParams() called"<<std::endl;
  if(!list) return;
  list->add("max_paddle", (Int_t)nMaxPaddle);
  list->add("max_plane", (Int_t)nMaxPlane);
}

Bool_t R3BLandDigiPar::getParams(FairParamList* list)
{
  std::cout<<"-I- R3BLandDigiPar::getParams() called"<<std::endl;
  if (!list) return kFALSE;
  std::cout<<"-I- R3BLandDigiPar::getParams() 1 ";
  
  if (!list->fill("max_paddle", &nMaxPaddle)) return kFALSE;
  if (!list->fill("max_plane", &nMaxPlane)) return kFALSE;
  return kTRUE;
}


void R3BLandDigiPar::Print()
{
    std::cout<<"-I- Land Digi Parameters:"<<std::endl;
    std::cout<<"   Max Paddle   = "<<nMaxPaddle<<std::endl;
    std::cout<<"   Max Plane   = "<<nMaxPlane<<std::endl;
}

ClassImp(R3BLandDigiPar);
