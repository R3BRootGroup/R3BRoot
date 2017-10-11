// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                        R3BCalifaMappedPar                         -----
// -----                Created 21/07/2014 by H. Alvarez-Pol               -----
// -----                Modified 20/03/2017 by P. Cabanelas                -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BCALIFAMAPPEDPAR_H
#define R3BCALIFAMAPPEDPAR_H

#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

static const UInt_t numberOfCrystals = 1952;

class R3BCalifaMappedPar : public FairParGenericSet
{
 public :
  R3BCalifaMappedPar (const char* name="R3BCalifaMappedPar",
		    const char* title="Crystal Unpacking Parameters",
		    const char* context="TestDefaultContext");
  ~R3BCalifaMappedPar(void){};
  void clear(void){};
  void putParams(FairParamList* list);
  Bool_t getParams(FairParamList* list);
  
  void Print(Option_t* option="") const;
  /** Accessor functions **/
  const Int_t GetValidityMast(Int_t crystal){return validityMask[crystal];}
  const UInt_t GetGosip_header_size(){return gosip_header_size;}
  const UInt_t GetGosip_sub_header_size(){return gosip_sub_header_size;}
  const Int_t GetEvent_t_size(){return event_t_size;}
  const Int_t GetTrace_head_t_size(){return trace_head_t_size;}
  const UInt_t GetMax_submemory_id(){return max_submemory_id;}
  const UInt_t GetMax_module_id(){return max_module_id;}
  const UInt_t GetMax_sfp_id(){return max_sfp_id;}
  const UInt_t GetMax_pc_id(){return max_pc_id;}
  
  void SetValidityMask(Int_t crystal, Int_t value){validityMask[crystal]=value;}
  void SetGosip_header_size(UInt_t value){gosip_header_size=value;}
  void SetGosip_sub_header_size(UInt_t value){gosip_sub_header_size=value;}
  void SetEvent_t_size(Int_t value){event_t_size=value;}
  void SetTrace_head_t_size(Int_t value){trace_head_t_size=value;}
  void SetMax_submemory_id(UInt_t value){max_submemory_id=value;}
  void SetMax_module_id(UInt_t value){max_module_id=value;}
  void SetMax_sfp_id(UInt_t value){max_sfp_id=value;}
  void SetMax_pc_id(UInt_t value){max_pc_id=value;}
  
 private:
  
  //Mask for valid/invalid/present/defect/unused crystals. [int×number_of_crystals].
  Int_t* validityMask; 

  Int_t gosip_header_size;
  Int_t gosip_sub_header_size;
  Int_t  event_t_size;
  Int_t  trace_head_t_size;
  
  Int_t max_submemory_id;
  Int_t max_module_id;
  Int_t max_sfp_id;
  Int_t max_pc_id;
    
  ClassDef(R3BCalifaMappedPar,1); //
};

#endif /* !R3BCALIFAMAPPEDPAR_H*/

