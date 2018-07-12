#ifndef R3BFI0READER_H
#define R3BFI0READER_H

#include "R3BBunchedFiberReader.h"

typedef struct EXT_STR_h101_FIBZERO_t EXT_STR_h101_FIBZERO;
typedef struct EXT_STR_h101_FIBZERO_onion_t EXT_STR_h101_FIBZERO_onion;

class R3BFi0Reader: public R3BBunchedFiberReader
{
  public:
    R3BFi0Reader(EXT_STR_h101_FIBZERO *, UInt_t);
    Bool_t Init(ext_data_struct_info *);

  private:
    EXT_STR_h101_FIBZERO_onion *fData;

  public:
    ClassDef(R3BFi0Reader, 1);
};

#endif
