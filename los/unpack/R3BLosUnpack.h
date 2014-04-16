// ---------------------------------------------------------------------------------------
// -----                           R3BLosUnpack                                      -----
// -----                  Created 09-04-2014 by D.Kresan                             -----
// ---------------------------------------------------------------------------------------

#ifndef R3BLOSUNPACK_H
#define R3BLOSUNPACK_H

#include "FairUnpack.h"

#define VME_CAEN        1
#define VME_CAEN_MASK              0xff000000
#define VME_CAEN_SCALER_SIG        0x04000000
#define VME_CAEN_MTDC_MASK         0xf0000000
#define VME_CAEN_MTDC_SIG          0x40000000

#define VME_MESYTEC     3
#define VME_MESYTEC_MASK           0xff000000
#define VME_MESYTEC_SIGNATURE      0x40000000

#define VME_VUPROM      4
#define VME_VUPROM_MASK              0xff000000
#define VME_VUPROM_SIGNATURE         0xfe000000

#define VME_VFTX        5
#define VME_VFTX_MASK              0xff000000
#define VME_VFTX_SIGNATURE         0xab000000
#define VME_VFTX_GEOM_MASK         0x0000001f
#define VME_VFTX_GEOM_SHIFT                 0

class TClonesArray;

class R3BLosUnpack : public FairUnpack
{
  public:
    // Constructor
    R3BLosUnpack(char* strCalDir, Short_t type = 88, Short_t subType = 8800, Short_t procId = 10, Short_t subCrate = 7, Short_t control = 5);

    // Destructor
    virtual ~R3BLosUnpack();

    // Fair specific
    virtual Bool_t Init();
    virtual Bool_t DoUnpack(Int_t* data, Int_t size);
    virtual void Reset();

  protected:
    virtual void Register();

  private:
    TClonesArray* fRawData;
    Int_t fNHits;

  public:
    // Class definition
    ClassDef(R3BLosUnpack, 0)
};

#endif
