#ifndef R3BFI9READER_H
#define R3BFI9READER_H

#include "R3BBunchedFiberReader.h"

typedef struct EXT_STR_h101_FIBNINE_t EXT_STR_h101_FIBNINE;
typedef struct EXT_STR_h101_FIBNINE_onion_t EXT_STR_h101_FIBNINE_onion;

class R3BFi9Reader: public R3BBunchedFiberReader
{
  public:
    R3BFi9Reader(EXT_STR_h101_FIBNINE *, UInt_t);
    Bool_t Init(ext_data_struct_info *);

  private:
    EXT_STR_h101_FIBNINE_onion *fData;

  public:
    ClassDef(R3BFi9Reader, 1);
};

#endif
