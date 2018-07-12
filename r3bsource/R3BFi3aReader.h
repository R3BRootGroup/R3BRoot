#ifndef R3BFI3AREADER_H
#define R3BFI3AREADER_H

#include "R3BBunchedFiberReader.h"

typedef struct EXT_STR_h101_FIBTHREEA_t EXT_STR_h101_FIBTHREEA;
typedef struct EXT_STR_h101_FIBTHREEA_onion_t EXT_STR_h101_FIBTHREEA_onion;

class R3BFi3aReader: public R3BBunchedFiberReader
{
  public:
    R3BFi3aReader(EXT_STR_h101_FIBTHREEA *, UInt_t);
    Bool_t Init(ext_data_struct_info *);

  private:
    EXT_STR_h101_FIBTHREEA_onion *fData;

  public:
    ClassDef(R3BFi3aReader, 1);
};

#endif
