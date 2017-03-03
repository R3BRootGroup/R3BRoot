#ifndef R3BFI5READER_H
#define R3BFI5READER_H

#include "R3BBunchedFiberReader.h"

typedef struct EXT_STR_h101_FIBFIVE_t EXT_STR_h101_FIBFIVE;
typedef struct EXT_STR_h101_FIBFIVE_onion_t EXT_STR_h101_FIBFIVE_onion;

class R3BFi5Reader: public R3BBunchedFiberReader
{
  public:
    R3BFi5Reader(EXT_STR_h101_FIBFIVE *, UInt_t);
    Bool_t Init(ext_data_struct_info *);

  private:
    EXT_STR_h101_FIBFIVE_onion *fData;

  public:
    ClassDef(R3BFi5Reader, 1);
};

#endif
