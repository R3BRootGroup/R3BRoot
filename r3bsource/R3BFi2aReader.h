#ifndef R3BFI2AREADER_H
#define R3BFI2AREADER_H

#include "R3BBunchedFiberReader.h"

typedef struct EXT_STR_h101_FIBTWOA_t EXT_STR_h101_FIBTWOA;
typedef struct EXT_STR_h101_FIBTWOA_onion_t EXT_STR_h101_FIBTWOA_onion;

class R3BFi2aReader: public R3BBunchedFiberReader
{
  public:
    R3BFi2aReader(EXT_STR_h101_FIBTWOA *, UInt_t);
    Bool_t Init(ext_data_struct_info *);

  private:
    EXT_STR_h101_FIBTWOA_onion *fData;

  public:
    ClassDef(R3BFi2aReader, 1);
};

#endif
