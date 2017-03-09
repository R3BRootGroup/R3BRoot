#include <iostream>
#include "R3BPtofHitPar.h"


R3BPtofHitPar::R3BPtofHitPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  //TODO initialize the fields
}

void R3BPtofHitPar::putParams(FairParamList* list)
{
  std::cout << "-I- R3BPtofHitPar::putParams() called" << std::endl;
  if(!list) return;
  
  //TODO add the fields you added to the header to the list:
  //    list->add("someName", someParameter);
  
}

Bool_t R3BPtofHitPar::getParams(FairParamList* list)
{
  std::cout << "-I- R3BPtofHitPar::getParams() called" << std::endl;
  if (!list) return kFALSE;
  std::cout << "-I- R3BPtofHitPar::getParams() 1 " << std::endl;

  //TODO get the values of the fields you added from the list:
  //    list->fill("someName", &someParameter);

  
  return kTRUE;
}


void R3BPtofHitPar::Print(Option_t *option) const
{
    std::cout << "-I- Ptof Hit Parameters:" << std::endl;
    
    //TODO make some useful output
}

ClassImp(R3BPtofHitPar)
