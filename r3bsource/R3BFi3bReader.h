#ifndef R3BFI3BREADER_H
#define R3BFI3BREADER_H

#include "R3BBunchedFiberReader.h"

typedef struct EXT_STR_h101_FIBTHREEB_t EXT_STR_h101_FIBTHREEB;
typedef struct EXT_STR_h101_FIBTHREEB_onion_t EXT_STR_h101_FIBTHREEB_onion;

class R3BFi3bReader: public R3BBunchedFiberReader
{
  public:
    R3BFi3bReader(EXT_STR_h101_FIBTHREEB *, UInt_t);
    Bool_t Init(ext_data_struct_info *);

  private:
    EXT_STR_h101_FIBTHREEB_onion *fData;

  public:
    ClassDef(R3BFi3bReader, 1);
};

#endif
