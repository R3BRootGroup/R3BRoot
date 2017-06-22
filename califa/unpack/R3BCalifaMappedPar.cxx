// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                         R3BCalifaMappedPar                        -----
// -----                Created 21/07/2014 by H. Alvarez-Pol               -----
// -----                Modified 20/03/2017 by P. Cabanelas                -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include <iostream>
#include "R3BCalifaMappedPar.h"
#include "TMath.h"

R3BCalifaMappedPar::R3BCalifaMappedPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  validityMask = new Int_t[numberOfCrystals];
  clear();
}

void R3BCalifaMappedPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BCalifaMappedPar::putParams() called"<<std::endl;

  if(!list) return;
  list->add("validityMask", (Int_t*)validityMask, numberOfCrystals);
  list->add("gosip_header_size", (Int_t)gosip_header_size);
  list->add("gosip_sub_header_size", (Int_t)gosip_sub_header_size);
  list->add("event_t_size", (Int_t)event_t_size);
  list->add("trace_head_t_size", (Int_t)trace_head_t_size);
  list->add("max_submemory_id", (Int_t)max_submemory_id);
  list->add("max_module_id", (Int_t)max_module_id);
  list->add("max_sfp_id", (Int_t)max_sfp_id);
  list->add("max_pc_id", (Int_t)max_pc_id);
}


Bool_t R3BCalifaMappedPar::getParams(FairParamList* list)
{
  std::cout<<"-I- R3BCalifaMappedPar::getParams() called"<<std::endl;
  if (!list) return kFALSE;
  std::cout<<"-I- R3BCalifaMappedPar::getParams() 1 ";
  
  if (!list->fill("validityMask", validityMask, numberOfCrystals)) return kFALSE;
  if (!list->fill("gosip_header_size", &gosip_header_size,1)) return kFALSE;
  if (!list->fill("gosip_sub_header_size", &gosip_sub_header_size,1)) return kFALSE;
  if (!list->fill("event_t_size", &event_t_size,1)) return kFALSE;
  if (!list->fill("trace_head_t_size", &trace_head_t_size,1)) return kFALSE;
  if (!list->fill("max_submemory_id", &max_submemory_id,1)) return kFALSE;
  if (!list->fill("max_module_id", &max_module_id,1)) return kFALSE;
  if (!list->fill("max_sfp_id", &max_sfp_id,1)) return kFALSE;
  if (!list->fill("max_pc_id", &max_pc_id,1)) return kFALSE;
 
  return kTRUE;
}


void R3BCalifaMappedPar::Print(Option_t* option) const
{
  std::cout<<"-I- CALIFA Unpack Parameters:"<<std::endl;
  for(Int_t i=0; i<numberOfCrystals; i++) {
    std::cout<<" validityMask["<< i<<"] " << validityMask[i]<<std::endl;
    std::cout<<" gosip_header_size "<< gosip_header_size <<std::endl;
    std::cout<<" gosip_sub_header_size "<< gosip_sub_header_size <<std::endl;
    std::cout<<" event_t_size "<< event_t_size <<std::endl;
    std::cout<<" trace_head_t_size "<< trace_head_t_size <<std::endl;
    std::cout<<" max_submemory_id "<< max_submemory_id <<std::endl;
    std::cout<<" max_module_id "<< max_module_id<<std::endl;
    std::cout<<" max_sfp_id "<< max_sfp_id <<std::endl;
    std::cout<<" max_pc_id "<< max_pc_id <<std::endl; 
  }  
}

ClassImp(R3BCalifaMappedPar);
