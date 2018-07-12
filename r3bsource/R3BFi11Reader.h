#ifndef R3BFI11READER_H
#define R3BFI11READER_H

#include "R3BBunchedFiberReader.h"

typedef struct EXT_STR_h101_FIBELEVEN_t EXT_STR_h101_FIBELEVEN;
typedef struct EXT_STR_h101_FIBELEVEN_onion_t EXT_STR_h101_FIBELEVEN_onion;

class R3BFi11Reader: public R3BBunchedFiberReader
{
  public:
    R3BFi11Reader(EXT_STR_h101_FIBELEVEN *, UInt_t);
    Bool_t Init(ext_data_struct_info *);

  private:
    EXT_STR_h101_FIBELEVEN_onion *fData;

  public:
    ClassDef(R3BFi11Reader, 1);
};

#endif
