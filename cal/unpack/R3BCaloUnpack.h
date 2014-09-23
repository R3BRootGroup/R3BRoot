// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BCaloUnpack                            -----
// -----                           Version 0.1                             -----
// -----                    Created 11.10.2013 by Y.Gonzalez               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BCALOUNPACK_H
#define R3BCALOUNPACK_H

#include "FairUnpack.h"

#include "R3BCaloUnpackPar.h"

class TClonesArray;


static const UInt_t gosip_header_size = 16;
static const UInt_t gosip_sub_header_size = 8;
static const Int_t  event_t_size = 44;
static const Int_t  trace_head_t_size = 8;

static const UInt_t max_submemory_id = 20;
static const UInt_t max_module_id = 256;
static const UInt_t max_sfp_id = 4;
static const UInt_t max_pc_id = 2;


class R3BCaloUnpack : public FairUnpack {
 public:
  //Constructor
  R3BCaloUnpack(char *strCalDir,
                Short_t type = 10, Short_t subType = 1,
                Short_t procId = 1,
                Short_t subCrate = 0, Short_t control = 9);
  
  //Destructor
  virtual ~R3BCaloUnpack();
  
  //Fair specific
  virtual Bool_t Init();
  /** Virtual method SetParContainers **/
  virtual void SetParContainers();

  virtual Bool_t DoUnpack(Int_t* data, Int_t size, Int_t l_s = 5);
  virtual void Reset();
    
 protected:
  virtual void Register();
 
 private:
  TClonesArray *fRawData;
  Int_t         fNHits;

  R3BCaloUnpackPar* fCaloUnpackPar;

 public:
  //Class definition
  ClassDef(R3BCaloUnpack, 0)
};

#endif
