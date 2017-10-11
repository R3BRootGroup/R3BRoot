// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                    from R3BStartrackUnpackPar                           -----
// -----                Created 21/07/2014 by H. Alvarez-Pol               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include <iostream>
#include "R3BStartrackUnpackPar.h"
#include "TMath.h"

R3BStartrackUnpackPar::R3BStartrackUnpackPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  validityMask = new Int_t[numberOfStrips];
  clear();
}

void R3BStartrackUnpackPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BStartrackUnpackPar::putParams() called"<<std::endl;

  if(!list) return;
  list->add("validityMask", (Int_t*)validityMask, numberOfStrips);
  list->add("gosip_header_size", (Int_t)gosip_header_size);
  list->add("gosip_sub_header_size", (Int_t)gosip_sub_header_size);
  list->add("event_t_size", (Int_t)event_t_size);
  list->add("trace_head_t_size", (Int_t)trace_head_t_size);
  list->add("max_submemory_id", (Int_t)max_submemory_id);
  list->add("max_module_id", (Int_t)max_module_id);
  list->add("max_sfp_id", (Int_t)max_sfp_id);
  list->add("max_pc_id", (Int_t)max_pc_id);
}


Bool_t R3BStartrackUnpackPar::getParams(FairParamList* list)
{
  std::cout<<"-I- R3BStartrackUnpackPar::getParams() called"<<std::endl;
  if (!list) return kFALSE;
  std::cout<<"-I- R3BStartrackUnpackPar::getParams() 1 ";
  
  if (!list->fill("validityMask", validityMask, numberOfStrips)) return kFALSE;
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


void R3BStartrackUnpackPar::Print(Option_t* option) const
{
  std::cout<<"-I- STARTRACKER Unpack Parameters:"<<std::endl;
  for(Int_t i=0; i<numberOfStrips; i++) {
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

ClassImp(R3BStartrackUnpackPar);
