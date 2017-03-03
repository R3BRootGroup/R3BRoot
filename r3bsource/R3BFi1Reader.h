#ifndef R3BFI1READER_H
#define R3BFI1READER_H

#include "R3BBunchedFiberReader.h"

typedef struct EXT_STR_h101_FIBONE_t EXT_STR_h101_FIBONE;
typedef struct EXT_STR_h101_FIBONE_onion_t EXT_STR_h101_FIBONE_onion;

class R3BFi1Reader: public R3BBunchedFiberReader
{
  public:
    R3BFi1Reader(EXT_STR_h101_FIBONE *, UInt_t);
    Bool_t Init(ext_data_struct_info *);

  private:
    EXT_STR_h101_FIBONE_onion *fData;

  public:
    ClassDef(R3BFi1Reader, 1);
};

#endif
