// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                           R3BLandUnpack                                     -----
// -----                           Version 0.1                                       -----
// -----         Adapted by M.I. Cherciu @ 01.2014 after Y.Gonzalez code             -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

#ifndef R3BLANDUNPACK_H
#define R3BLANDUNPACK_H

#include "FairUnpack.h"

class TClonesArray;
class TH1F;
//static const UInt_t gosip_header_size = 16;
//static const UInt_t gosip_sub_header_size = 8;
//static const Int_t  event_t_size = 44;
//static const Int_t  trace_head_t_size = 8;


class R3BLandUnpack : public FairUnpack {
 public:
  //Constructor
  R3BLandUnpack(char *strCalDir,
                Short_t type = 94, Short_t subType = 9400,
                Short_t procId = 10,
                Short_t subCrate = -1, Short_t control = 3);
  
  //Destructor
  virtual ~R3BLandUnpack();
    
  //Fair specific
  virtual Bool_t Init();
  virtual Bool_t DoUnpack(Int_t* data, Int_t size);
  virtual void Reset();

 protected:
  virtual void Register();
 
 private:
  TClonesArray *fRawData;
  Int_t         fNHits;
   
  
 public:
  //Class definition
  ClassDef(R3BLandUnpack, 0)
};

#endif
