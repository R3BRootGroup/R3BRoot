#ifndef R3BFI10READER_H
#define R3BFI10READER_H

#include "R3BBunchedFiberReader.h"

typedef struct EXT_STR_h101_FIBTEN_t EXT_STR_h101_FIBTEN;
typedef struct EXT_STR_h101_FIBTEN_onion_t EXT_STR_h101_FIBTEN_onion;

class R3BFi10Reader: public R3BBunchedFiberReader
{
  public:
    R3BFi10Reader(EXT_STR_h101_FIBTEN *, UInt_t);
    Bool_t Init(ext_data_struct_info *);

  private:
    EXT_STR_h101_FIBTEN_onion *fData;

  public:
    ClassDef(R3BFi10Reader, 1);
};

#endif
