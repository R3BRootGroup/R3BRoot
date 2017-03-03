#ifndef R3BFI6READER_H
#define R3BFI6READER_H

#include "R3BBunchedFiberReader.h"

typedef struct EXT_STR_h101_FIBSIX_t EXT_STR_h101_FIBSIX;
typedef struct EXT_STR_h101_FIBSIX_onion_t EXT_STR_h101_FIBSIX_onion;

class R3BFi6Reader: public R3BBunchedFiberReader
{
  public:
    R3BFi6Reader(EXT_STR_h101_FIBSIX *, UInt_t);
    Bool_t Init(ext_data_struct_info *);

  private:
    EXT_STR_h101_FIBSIX_onion *fData;

  public:
    ClassDef(R3BFi6Reader, 1);
};

#endif
