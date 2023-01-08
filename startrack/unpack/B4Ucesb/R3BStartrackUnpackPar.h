/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                   from R3BCaloUnpackPar                           -----
// -----                Created 21/07/2014 by H. Alvarez-Pol               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BSTARTRACKUNPACKPAR_H
#define R3BSTARTRACKUNPACKPAR_H

#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

// static const UInt_t numberOfCrystals = 1952;
static const UInt_t numberOfStrips = 128; // per ASIC

class R3BStartrackUnpackPar : public FairParGenericSet
{
  public:
    R3BStartrackUnpackPar(const char* name = "R3BStartrackUnpackPar",
                          const char* title = "Strip Unpacking Parameters",
                          const char* context = "TestDefaultContext");
    ~R3BStartrackUnpackPar(void){};
    void clear(void){};
    void putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print(Option_t* option = "") const;
    /** Accessor functions **/
    const Int_t GetValidityMast(Int_t crystal) { return validityMask[crystal]; }
    const UInt_t GetGosip_header_size() { return gosip_header_size; }
    const UInt_t GetGosip_sub_header_size() { return gosip_sub_header_size; }
    const Int_t GetEvent_t_size() { return event_t_size; }
    const Int_t GetTrace_head_t_size() { return trace_head_t_size; }
    const UInt_t GetMax_submemory_id() { return max_submemory_id; }
    const UInt_t GetMax_module_id() { return max_module_id; }
    const UInt_t GetMax_sfp_id() { return max_sfp_id; }
    const UInt_t GetMax_pc_id() { return max_pc_id; }

    void SetValidityMask(Int_t crystal, Int_t value) { validityMask[crystal] = value; }
    void SetGosip_header_size(UInt_t value) { gosip_header_size = value; }
    void SetGosip_sub_header_size(UInt_t value) { gosip_sub_header_size = value; }
    void SetEvent_t_size(Int_t value) { event_t_size = value; }
    void SetTrace_head_t_size(Int_t value) { trace_head_t_size = value; }
    void SetMax_submemory_id(UInt_t value) { max_submemory_id = value; }
    void SetMax_module_id(UInt_t value) { max_module_id = value; }
    void SetMax_sfp_id(UInt_t value) { max_sfp_id = value; }
    void SetMax_pc_id(UInt_t value) { max_pc_id = value; }

  private:
    // Mask for valid/invalid/present/defect/unused crystals. [int×number_of_crystals].
    Int_t* validityMask;

    Int_t gosip_header_size;
    Int_t gosip_sub_header_size;
    Int_t event_t_size;
    Int_t trace_head_t_size;

    Int_t max_submemory_id;
    Int_t max_module_id;
    Int_t max_sfp_id;
    Int_t max_pc_id;

    ClassDef(R3BStartrackUnpackPar, 1); //
};

#endif /* !R3BSTARTRACKUNPACKPAR_H*/
