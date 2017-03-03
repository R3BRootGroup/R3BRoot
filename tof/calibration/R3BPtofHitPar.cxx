#include <iostream>
#include "R3BPtofHitPar.h"


R3BPtofHitPar::R3BPtofHitPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
  , fTOffset1(TArrayD(12))
  , fTOffset2(TArrayD(12))
  , fZScale(TArrayD(12))
  , fVEff(TArrayD(12))
{
}

void R3BPtofHitPar::putParams(FairParamList* list)
{
  std::cout << "-I- R3BPtofHitPar::putParams() called" << std::endl;
  if(!list) return;
  
  list->add("TOffset1Params", fTOffset1);
  list->add("TOffset2Params", fTOffset2);
  list->add("ZScaleParams", fZScale);
  list->add("VEffParams", fVEff);
}

Bool_t R3BPtofHitPar::getParams(FairParamList* list)
{
  std::cout << "-I- R3BPtofHitPar::getParams() called" << std::endl;
  if (!list) return kFALSE;
  std::cout << "-I- R3BPtofHitPar::getParams() 1 " << std::endl;

  if (!list->fill("TOffset1Params", &fTOffset1))
      return kFALSE;
  if (!list->fill("TOffset2Params", &fTOffset2))
      return kFALSE;
  if (!list->fill("ZScaleParams", &fZScale))
      return kFALSE;
  if (!list->fill("VEffParams", &fVEff))
      return kFALSE;
  
  return kTRUE;
}


void R3BPtofHitPar::Print(Option_t *option) const
{
    std::cout << "-I- Ptof Hit Parameters:" << std::endl;
    
    for (Int_t i = 0; i < 12; i++){
      std::cout << "Paddle Nr. " << i << " : TOffset1 = " << fTOffset1.At(i) << "   TOffset2 = " << fTOffset2.At(i) 
		<< "   ZScale = " << fZScale.At(i) << "   VEff = " << fVEff.At(i) << std::endl;
    }
}

ClassImp(R3BPtofHitPar)
